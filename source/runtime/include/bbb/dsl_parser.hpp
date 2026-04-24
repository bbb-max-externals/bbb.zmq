#pragma once

#include "dsl_ast.hpp"
#include "dsl_lexer.hpp"
#include <vector>

namespace bbb::dsl {

ParseResult parse(const std::vector<Token> &tokens);

inline ParseResult parse_source(const std::string &source) {
	auto lex_result = lex(source);
	if (!lex_result.ok()) {
		ParseResult result;
		ParseError err;
		err.message = lex_result.error;
		result.errors.push_back(err);
		return result;
	}
	return parse(lex_result.tokens);
}

} // namespace bbb::dsl
