#include <c74_min.h>
#include "bbb/runtime.hpp"
#include "bbb/parser_vm.hpp"
#include <sstream>

using namespace c74::min;

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

	message<> anything {this, "anything", MIN_FUNCTION {
		if (args.size() < 2) return {};
		if (!(args[0] == "packet")) return {};

		auto view_id = (bbb::ViewId)(int)args[1];
		auto view = bbb::Runtime::instance().packet_store.get_view(view_id);
		if (!view) {
			error_out.send("error", "packet_not_found", (int)view_id);
			return {};
		}

		auto name = schema_name_.empty() ? std::string(schema_attr.get()) : schema_name_;
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
			for (auto &atom : out.atoms) {
				if (std::holds_alternative<int64_t>(atom)) {
					a.push_back((int)std::get<int64_t>(atom));
				} else if (std::holds_alternative<double>(atom)) {
					a.push_back(std::get<double>(atom));
				} else if (std::holds_alternative<std::string>(atom)) {
					a.push_back(std::get<std::string>(atom));
				}
			}
			output.send(a);
		}

		for (auto &err : result.errors) {
			error_out.send("error", (int)view_id, err.message);
		}

		return {};
	}};
};

MIN_EXTERNAL(bbb_zmq_parse);
