#include <c74_min.h>
#include "bbb/runtime.hpp"
#include "bbb/parser_vm.hpp"
#include <sstream>
#include <unordered_map>
#include <cstring>

using namespace c74::min;

static c74::max::t_symbol *ps_jit_matrix = nullptr;

static c74::max::t_jit_object *create_jit_matrix(c74::max::t_symbol *name, bbb::PrimitiveType celltype,
	int64_t planes, int64_t dim1, int64_t dim2)
{
	c74::max::t_symbol *type_sym = c74::max::_jit_sym_char;
	switch (celltype) {
	case bbb::PrimitiveType::I16: case bbb::PrimitiveType::U16:
	case bbb::PrimitiveType::I32: case bbb::PrimitiveType::U32:
		type_sym = c74::max::_jit_sym_long; break;
	case bbb::PrimitiveType::F32:
		type_sym = c74::max::_jit_sym_float32; break;
	case bbb::PrimitiveType::F64:
		type_sym = c74::max::_jit_sym_float64; break;
	default: break;
	}

	c74::max::t_jit_object *mtx = (c74::max::t_jit_object *)c74::max::jit_object_new(
		c74::max::_jit_sym_jit_matrix, type_sym, (long)planes, (long)dim1, (long)dim2);
	if (!mtx) return nullptr;

	if (name && name != c74::max::gensym("")) {
		c74::max::jit_object_register(mtx, name);
	}
	return mtx;
}

static void copy_matrix_data(c74::max::t_jit_object *mtx, const bbb::MatrixData &md) {
	c74::max::t_jit_object *info = (c74::max::t_jit_object *)c74::max::jit_object_method(mtx, c74::max::_jit_sym_getinfo);
	if (!info) return;

	long dimstride[2] = {0, 0};
	c74::max::jit_object_method(info, c74::max::_jit_sym_getinfo, c74::max::gensym("dimstride"), dimstride, 0L, 0L);

	char *bp = nullptr;
	c74::max::jit_object_method(mtx, c74::max::_jit_sym_getdata, &bp);
	if (!bp) return;

	size_t cell_sz = bbb::primitive_size(md.celltype);
	size_t row_src = (size_t)(md.planes * md.dim1) * cell_sz;

	for (long y = 0; y < (long)md.dim2; ++y) {
		const uint8_t *src = md.data.data() + y * row_src;
		char *dst = bp + y * dimstride[1];
		std::memcpy(dst, src, row_src);
	}
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
		if (!ps_jit_matrix) ps_jit_matrix = c74::max::gensym("jit_matrix");
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

		auto view_id = (bbb::ViewId)(int)args[1];
		auto view = bbb::Runtime::instance().packet_store.get_view(view_id);
		if (!view) {
			error_out.send("error", "packet_not_found", (int)view_id);
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

				std::string reg_key = "bbb.zmq.parse." + std::to_string((uintptr_t)this) + "." + out.selector;
				auto it = cached_matrices_.find(reg_key);
				c74::max::t_jit_object *mtx = nullptr;

				if (it != cached_matrices_.end() && it->second) {
					mtx = it->second;
				} else {
					c74::max::t_symbol *sym = c74::max::gensym(reg_key.c_str());
					mtx = create_jit_matrix(sym, md.celltype, md.planes, md.dim1, md.dim2);
					if (mtx) {
						cached_matrices_[reg_key] = mtx;
					}
				}

				if (!mtx) continue;
				copy_matrix_data(mtx, md);

				c74::max::t_symbol *reg_name = c74::max::gensym(reg_key.c_str());
				c74::max::t_atom a_mat;
				c74::max::atom_setsym(&a_mat, reg_name);
				c74::max::outlet_anything(maxobj(), ps_jit_matrix, 1, &a_mat);
			}
		}

		for (auto &err : result.errors) {
			error_out.send("error", (int)view_id, err.message);
		}

		return {};
	}};
};

MIN_EXTERNAL(bbb_zmq_parse);
