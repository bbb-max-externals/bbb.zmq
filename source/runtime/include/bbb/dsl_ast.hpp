#pragma once

#include "dsl_lexer.hpp"
#include <variant>

namespace bbb::dsl {

struct Directive {
	enum Kind {
		Endian_,
		Encoding_,
		OnError_,
		MaxBytes_,
		MaxItems_,
		MaxAtoms_,
		MaxString_
	};
	Kind kind;
	std::variant<int, uint64_t, std::string> value;
};

struct FieldDecl {
	enum Kind {
		Scalar,
		FixedArray,
		VarArray,
		FixedBytes,
		VarBytes,
		RemainingBytes,
		NullString,
		FixedString,
		Skip_
	};
	Kind kind;
	std::string primitive_type;
	std::string name;
	int64_t fixed_size = 0;
	std::string length_field;
	uint64_t const_value = 0;
	bool has_const = false;
	int64_t skip_bytes = 0;
};

struct EmitExpr {
	std::string field_name;
	int index = -1;
	std::string modifier;
};

struct EmitDecl {
	std::string selector;
	std::vector<EmitExpr> expressions;
	bool short_form = false;
};

struct SchemaDecl {
	std::string name;
	std::vector<Directive> directives;
	std::vector<FieldDecl> fields;
	std::vector<EmitDecl> emits;
};

struct Program {
	std::vector<SchemaDecl> schemas;
};

struct ParseError {
	std::string message;
	int line = 0;
	int column = 0;
};

struct ParseResult {
	Program program;
	std::vector<ParseError> errors;
	bool ok() const { return errors.empty(); }
};

ParseResult parse(const std::vector<Token> &tokens);

} // namespace bbb::dsl
