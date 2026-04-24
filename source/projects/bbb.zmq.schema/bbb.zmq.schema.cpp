#include <c74_min.h>
#include "bbb/runtime.hpp"
#include "bbb/dsl_lexer.hpp"
#include "bbb/dsl_ast.hpp"
#include "bbb/dsl_parser.hpp"
#include <fstream>
#include <sstream>

using namespace c74::min;

class bbb_zmq_schema : public object<bbb_zmq_schema> {
public:
	MIN_DESCRIPTION {"Load, compile, and register DSL schemas"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.parse"};

	inlet<> input {this, "DSL text or commands"};
	outlet<> status_out {this, "status"};
	outlet<> error_out {this, "errors"};

	attribute<std::string> schema_name {this, "name", "",
		description {"Schema name for registration"}
	};

	attribute<std::string> file_path {this, "file", "",
		description {"Path to .zmqdsl file"}
	};

	attribute<int> autocompile {this, "autocompile", 0,
		description {"Auto-compile on set (0 or 1)"}
	};

	message<> read_msg {this, "read", "Read schema from file", [this](const atoms &args) -> atoms {
		if (args.size() < 1) return {};
		auto path = (std::string)args[0];
		read_file(path);
		return {};
	}};

	message<> reload_msg {this, "reload", "Reload from @file path", [this](const atoms &args) -> atoms {
		auto fp = file_path.get();
		if (!fp.empty()) {
			read_file(fp);
		}
		return {};
	}};

	message<> set_msg {this, "set", "Set DSL text", [this](const atoms &args) -> atoms {
		text_buffer_.clear();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) text_buffer_ += " ";
			text_buffer_ += (std::string)args[i];
		}
		if (autocompile.get()) {
			compile_text();
		}
		return {};
	}};

	message<> append_msg {this, "append", "Append DSL line", [this](const atoms &args) -> atoms {
		text_buffer_ += "\n";
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) text_buffer_ += " ";
			text_buffer_ += (std::string)args[i];
		}
		return {};
	}};

	message<> clear_msg {this, "clear", "Clear text buffer", [this](const atoms &args) -> atoms {
		text_buffer_.clear();
		return {};
	}};

	message<> compile_msg {this, "compile", "Compile current text buffer", [this](const atoms &args) -> atoms {
		compile_text();
		return {};
	}};

	message<> dump_msg {this, "dump", "Dump registered schemas", [this](const atoms &args) -> atoms {
		auto names = bbb::Runtime::instance().schema_registry.names();
		for (auto &name : names) {
			status_out.send("registered", name);
		}
		return {};
	}};

	message<> write_msg {this, "write", "Write text buffer to file", [this](const atoms &args) -> atoms {
		if (args.size() < 1) return {};
		auto path = (std::string)args[0];
		std::ofstream ofs(path);
		if (ofs.is_open()) {
			ofs << text_buffer_;
			status_out.send("written", path);
		} else {
			error_out.send("error", "could not write to " + path);
		}
		return {};
	}};

	message<> anything_msg {this, "anything", "Handle unknown messages", [this](const atoms &args) -> atoms {
		return {};
	}};

private:
	std::string text_buffer_;

	void read_file(const std::string &path) {
		std::ifstream ifs(path);
		if (!ifs.is_open()) {
			error_out.send("error", "could not read " + path);
			return;
		}
		std::ostringstream ss;
		ss << ifs.rdbuf();
		text_buffer_ = ss.str();
		file_path = path;
		compile_text();
	}

	void compile_text() {
		auto lex_result = bbb::dsl::lex(text_buffer_);
		if (!lex_result.ok()) {
			error_out.send("error", "lexer: " + lex_result.error);
			return;
		}

		auto parse_result = bbb::dsl::parse(lex_result.tokens);
		if (!parse_result.ok()) {
			for (auto &err : parse_result.errors) {
				std::string msg = "line " + std::to_string(err.line) + " column " + std::to_string(err.column) + " " + err.message;
				error_out.send("error", msg);
			}
			return;
		}

		auto compile_result = bbb::compile(parse_result.program);
		if (!compile_result.ok()) {
			for (auto &err : compile_result.errors) {
				std::string msg = "line " + std::to_string(err.line) + " " + err.message;
				error_out.send("error", msg);
			}
			return;
		}

		auto name_override = schema_name.get();

		for (auto &cs : compile_result.schemas) {
			std::string reg_name = name_override.empty() ? cs.name : name_override;

			if (!name_override.empty() && compile_result.schemas.size() > 1) {
				error_out.send("error", "code name_override_with_multiple_schemas");
				return;
			}

			auto schema_ptr = std::make_shared<bbb::CompiledSchema>(std::move(cs));
			bbb::Runtime::instance().schema_registry.register_schema(reg_name, schema_ptr);

			status_out.send("compiled", reg_name);
			status_out.send("schema", reg_name, "fields", (int)schema_ptr->field_decls.size(), "emits", (int)schema_ptr->instructions.size());
		}
	}
};

MIN_EXTERNAL(bbb_zmq_schema);
