#pragma once

#include <string>
#include <vector>

namespace bbb::dsl {

enum class TokenType {
	Schema,
	Endian,
	Encoding,
	OnError,
	MaxBytes,
	MaxItems,
	MaxAtoms,
	MaxString,
	Skip,
	Emit,
	Little,
	Big,
	Error_,
	Drop,
	Pass,
	U8, I8, U16, I16, U32, I32, U64, I64, F32, F64,
	Bytes,
	String,
	Matrix,
	Colon,
	Ident,
	Integer,
	Float_,
	StringLiteral,
	LBrace,
	RBrace,
	LBracket,
	RBracket,
	Equals,
	Semicolon,
	At,
	Asterisk,
	Eof,
	Unknown
};

struct Token {
	TokenType type;
	std::string text;
	int line;
	int column;
};

struct LexerResult {
	std::vector<Token> tokens;
	std::string error;
	bool ok() const { return error.empty(); }
};

LexerResult lex(const std::string &source);

const char *token_type_name(TokenType t);

} // namespace bbb::dsl
