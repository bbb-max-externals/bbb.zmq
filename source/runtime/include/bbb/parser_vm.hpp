#pragma once

#include "types.hpp"
#include "schema.hpp"
#include "packet_store.hpp"
#include <vector>
#include <string>
#include <variant>

namespace bbb {

struct MatrixData {
	PrimitiveType celltype = PrimitiveType::U8;
	int64_t planes = 1;
	int64_t dim1 = 0;
	int64_t dim2 = 0;
	std::vector<uint8_t> data;
};

struct ParseOutput {
	std::string selector;
	std::vector<std::variant<double, int64_t, std::string, MatrixData>> atoms;
};

struct ParseDiag {
	enum Code {
		ConstMismatch,
		OutOfBounds,
		UnterminatedString,
		FrameMissing,
		InvalidUTF8,
		TooManyAtoms,
		SchemaNotFound,
		VarLengthOverflow,
		RemainingTooLarge,
		StringTooLong,
		VarLengthNegative,
		Unknown
	};
	Code code = Unknown;
	std::string message;
};

struct ParseResult {
	std::vector<ParseOutput> outputs;
	std::vector<ParseDiag> errors;
	bool ok() const { return errors.empty(); }
};

ParseResult parse_frame(const CompiledSchema &schema, const Frame &frame);
ParseResult parse_frame(const CompiledSchema &schema, const Frame &frame, uint64_t maxbytes_override, uint64_t maxatoms_override);

} // namespace bbb
