#pragma once

#include "types.hpp"
#include "schema.hpp"
#include "packet_store.hpp"
#include <vector>
#include <string>
#include <variant>

namespace bbb {

struct ParseOutput {
	std::string selector;
	std::vector<std::variant<double, int64_t, std::string>> atoms;
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
