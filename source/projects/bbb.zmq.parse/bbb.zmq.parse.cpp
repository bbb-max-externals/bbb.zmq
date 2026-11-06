#include <c74_min.h>
#include "bbb/runtime.hpp"
#include "bbb/parser_vm.hpp"
#include <sstream>
#include <unordered_map>
#include <cstring>
#include <limits>

using namespace c74::min;

// MatrixData contains packed bytes. Only native-width Jitter types can be
// copied verbatim; 16/64-bit integers need a conversion, not a truncated copy.
static bool matrix_info_for_data(const bbb::MatrixData &md,
	c74::max::t_jit_matrix_info &info, size_t &row_bytes)
{
	c74::max::jit_matrix_info_default(&info);
	switch (md.celltype) {
	case bbb::PrimitiveType::U8: case bbb::PrimitiveType::I8:
		info.type = c74::max::_jit_sym_char; break;
	case bbb::PrimitiveType::I32: case bbb::PrimitiveType::U32:
		info.type = c74::max::_jit_sym_long; break;
	case bbb::PrimitiveType::F32:
		info.type = c74::max::_jit_sym_float32; break;
	case bbb::PrimitiveType::F64:
		info.type = c74::max::_jit_sym_float64; break;
	default: return false;
	}

	if (md.planes < 1 || md.planes > c74::max::JIT_MATRIX_MAX_PLANECOUNT ||
		md.dim1 < 1 || md.dim2 < 1 ||
		md.dim1 > std::numeric_limits<long>::max() || md.dim2 > std::numeric_limits<long>::max()) return false;

	const size_t cell_bytes = static_cast<size_t>(md.planes) * bbb::primitive_size(md.celltype);
	// Division checks avoid overflowing the dimensions before checking the source.
	if (static_cast<uint64_t>(md.dim1) > md.data.size() / cell_bytes) return false;
	row_bytes = static_cast<size_t>(md.dim1) * cell_bytes;
	if (static_cast<uint64_t>(md.dim2) > md.data.size() / row_bytes) return false;

	info.planecount = static_cast<long>(md.planes);
	// The DSL has two dimensions, including N x 1 for a single row.
	info.dimcount = 2;
	info.dim[0] = static_cast<long>(md.dim1);
	info.dim[1] = static_cast<long>(md.dim2);
	return true;
}

static c74::max::t_jit_object *create_jit_matrix(c74::max::t_symbol *name,
	c74::max::t_jit_matrix_info &info)
{
	auto *mtx = static_cast<c74::max::t_jit_object *>(
		c74::max::jit_object_new(c74::max::_jit_sym_jit_matrix, &info));
	if (mtx) c74::max::jit_object_register(mtx, name);
	return mtx;
}

static bool copy_matrix_data(c74::max::t_jit_object *mtx, const bbb::MatrixData &md,
	c74::max::t_jit_matrix_info requested, size_t row_bytes)
{
	const auto saved_lock = reinterpret_cast<c74::max::t_atom_long>(
		c74::max::jit_object_method(mtx, c74::max::_jit_sym_lock, 1L));
	struct matrix_unlock {
		c74::max::t_jit_object *matrix;
		c74::max::t_atom_long saved_lock;
		~matrix_unlock() {
			c74::max::jit_object_method(matrix, c74::max::_jit_sym_lock, saved_lock);
		}
	} unlock{mtx, saved_lock};

	c74::max::t_jit_matrix_info info{};
	if (c74::max::jit_object_method(mtx, c74::max::_jit_sym_getinfo, &info)) return false;
	const auto matches = [&]() {
		return info.type == requested.type && info.planecount == requested.planecount &&
			info.dimcount == requested.dimcount && info.dim[0] == requested.dim[0] &&
			info.dim[1] == requested.dim[1];
	};
	// A cached selector can receive new dimensions or a recompiled schema.
	if (!matches()) {
		if (c74::max::jit_object_method(mtx, c74::max::_jit_sym_setinfo, &requested) ||
			c74::max::jit_object_method(mtx, c74::max::_jit_sym_getinfo, &info) || !matches()) return false;
	}

	const size_t cell_bytes = static_cast<size_t>(md.planes) * bbb::primitive_size(md.celltype);
	if (info.dimstride[0] != static_cast<long>(cell_bytes) || info.dimstride[1] <= 0 || info.size <= 0) return false;
	const size_t stride = static_cast<size_t>(info.dimstride[1]);
	const size_t size = static_cast<size_t>(info.size);
	const size_t rows = static_cast<size_t>(info.dim[1]);
	// Check the last row against the real allocation before writing any row.
	if (row_bytes > stride || row_bytes > size || rows - 1 > (size - row_bytes) / stride) return false;

	char *bp = nullptr;
	if (c74::max::jit_object_method(mtx, c74::max::_jit_sym_getdata, &bp) || !bp) return false;
	for (size_t y = 0; y < rows; ++y) {
		std::memcpy(bp + y * stride, md.data.data() + y * row_bytes, row_bytes);
	}
	return true;
}

class bbb_zmq_parse : public object<bbb_zmq_parse> {
public:
	MIN_DESCRIPTION {"Parse current packet frame using a compiled schema"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.schema, bbb.zmq.recv"};

	inlet<> input {this, "packet view handle"};
	outlet<> output {this, "parsed messages"};
	outlet<> error_out {this, "diagnostics"};

	attribute<symbol> schema_attr {this, "schema", "",
		description {"Schema name to use for parsing"}
	};

	attribute<int> frame_attr {this, "frame", 0,
		description {"Frame index relative to current view (0-based)"}
	};

	attribute<int> maxbytes_attr {this, "maxbytes", 0,
		description {"Override maxbytes (0 = use schema default)"}
	};

	attribute<int> maxitems_attr {this, "maxitems", 0,
		description {"Override maxitems (0 = use schema default)"}
	};

	attribute<int> maxatoms_attr {this, "maxatoms", 0,
		description {"Override maxatoms (0 = use schema default)"}
	};

	attribute<int> maxstring_attr {this, "maxstring", 0,
		description {"Override maxstring (0 = use schema default)"}
	};

	std::string schema_name_;
	std::unordered_map<std::string, c74::max::t_jit_object *> cached_matrices_;

	bbb_zmq_parse(const atoms &args = {}) {
		for (auto &arg : args) {
			if (arg.a_type == c74::max::A_SYM) {
				auto s = (std::string)arg;
				if (s != "frame" && s != "schema" && s != "maxbytes" && s != "maxatoms" && s != "maxstring" && s != "maxitems") {
					schema_name_ = s;
				}
			}
		}
	}

	~bbb_zmq_parse() {
		for (auto &kv : cached_matrices_) {
			if (kv.second) {
				c74::max::t_symbol *reg_name = c74::max::gensym(kv.first.c_str());
				c74::max::t_jit_object *existing = (c74::max::t_jit_object *)c74::max::jit_object_findregistered(reg_name);
				if (existing == kv.second) {
					c74::max::jit_object_unregister(kv.second);
				}
				c74::max::jit_object_free(kv.second);
			}
		}
	}

	message<> anything {this, "anything", MIN_FUNCTION {
		if (args.size() < 2) return {};
		if (!(args[0] == "packet")) return {};

		auto view_id = static_cast<bbb::ViewId>(static_cast<c74::max::t_atom_long>(args[1]));
		auto view = bbb::Runtime::instance().packet_store.get_view(view_id);
		if (!view) {
			error_out.send("error", "packet_not_found", static_cast<c74::max::t_atom_long>(view_id));
			return {};
		}

		auto name = schema_name_.empty() ? std::string(schema_attr.get().c_str()) : schema_name_;
		if (name.empty()) {
			error_out.send("error", "schema_not_specified");
			return {};
		}

		auto schema = bbb::Runtime::instance().schema_registry.find(name);
		if (!schema) {
			error_out.send("error", "schema_not_found", name);
			return {};
		}

		auto frame_index = frame_attr.get();
		if (frame_index < 0 || (size_t)frame_index >= view->frame_count()) {
			error_out.send("error", "frame_missing", frame_index);
			return {};
		}

		auto &frame = view->frame((size_t)frame_index);

		uint64_t mb = maxbytes_attr.get() > 0 ? (uint64_t)maxbytes_attr.get() : schema->maxbytes;
		uint64_t ma = maxatoms_attr.get() > 0 ? (uint64_t)maxatoms_attr.get() : schema->maxatoms;

		auto result = bbb::parse_frame(*schema, frame, mb, ma);

		for (auto &out : result.outputs) {
			atoms a;
			a.push_back(out.selector);
			bool has_matrix = false;

			for (auto &atom : out.atoms) {
				if (std::holds_alternative<int64_t>(atom)) {
					a.push_back((int)std::get<int64_t>(atom));
				} else if (std::holds_alternative<double>(atom)) {
					a.push_back(std::get<double>(atom));
				} else if (std::holds_alternative<std::string>(atom)) {
					a.push_back(std::get<std::string>(atom));
				} else if (std::holds_alternative<bbb::MatrixData>(atom)) {
					has_matrix = true;
				}
			}

			if (!has_matrix) {
				output.send(a);
				continue;
			}

			output.send(a);

			for (auto &atom : out.atoms) {
				if (!std::holds_alternative<bbb::MatrixData>(atom)) continue;
				auto &md = std::get<bbb::MatrixData>(atom);
				c74::max::t_jit_matrix_info requested{};
				size_t row_bytes = 0;
				if (!matrix_info_for_data(md, requested, row_bytes)) {
					error_out.send("error", "matrix_invalid_layout", out.selector);
					continue;
				}

				std::string reg_key = "bbb.zmq.parse." + std::to_string((uintptr_t)this) + "." + out.selector;
				auto it = cached_matrices_.find(reg_key);
				c74::max::t_jit_object *mtx = nullptr;

				if (it != cached_matrices_.end() && it->second) {
					mtx = it->second;
				} else {
					c74::max::t_symbol *sym = c74::max::gensym(reg_key.c_str());
					mtx = create_jit_matrix(sym, requested);
					if (mtx) {
						cached_matrices_[reg_key] = mtx;
					}
				}

				if (!mtx) {
					error_out.send("error", "matrix_create_failed", out.selector);
					continue;
				}
				if (!copy_matrix_data(mtx, md, requested, row_bytes)) {
					error_out.send("error", "matrix_copy_failed", out.selector);
					continue;
				}
				output.send(symbol("jit_matrix"), symbol(reg_key));
			}
		}

		for (auto &err : result.errors) {
			error_out.send("error", static_cast<c74::max::t_atom_long>(view_id), err.message);
		}

		return {};
	}};
};

MIN_EXTERNAL(bbb_zmq_parse);
