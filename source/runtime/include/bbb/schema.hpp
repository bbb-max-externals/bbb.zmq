#pragma once

#include "types.hpp"
#include "dsl_ast.hpp"
#include <vector>
#include <string>
#include <variant>

namespace bbb {

enum class PrimitiveType {
	U8, I8, U16, I16, U32, I32, U64, I64, F32, F64
};

size_t primitive_size(PrimitiveType t);

struct Instr {
	enum Kind {
		ReadScalar,
		ReadFixedArray,
		ReadVarArray,
		ReadFixedBytes,
		ReadVarBytes,
		ReadRemainingBytes,
		ReadNullString,
		ReadFixedString,
		Skip_,
		ValidateConst,
		Emit_
	};
	Kind kind;
	PrimitiveType primitive = PrimitiveType::U8;
	std::string field_name;
	int64_t fixed_count = 0;
	std::string length_field;
	uint64_t const_value = 0;
	std::string emit_selector;
	std::vector<dsl::EmitExpr> emit_exprs;
	int64_t skip_bytes = 0;
};

struct CompiledSchema {
	std::string name;
	Endian endian = Endian::Little;
	Encoding encoding = Encoding::UTF8;
	OnError onerror = OnError::Error;
	uint64_t maxbytes = 1048576;
	uint64_t maxitems = 4096;
	uint64_t maxatoms = 1024;
	uint64_t maxstring = 4096;
	std::vector<Instr> instructions;
	std::vector<dsl::FieldDecl> field_decls;
};

struct CompileError {
	std::string message;
	int line = 0;
	int column = 0;
};

struct CompileResult {
	std::vector<CompiledSchema> schemas;
	std::vector<CompileError> errors;
	bool ok() const { return errors.empty(); }
};

CompileResult compile(const dsl::Program &program);

} // namespace bbb
