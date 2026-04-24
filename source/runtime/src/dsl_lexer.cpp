#include "bbb/dsl_lexer.hpp"
#include <cctype>
#include <cstdlib>

namespace bbb::dsl {

const char *token_type_name(TokenType t) {
	switch (t) {
	case TokenType::Schema: return "schema";
	case TokenType::Endian: return "endian";
	case TokenType::Encoding: return "encoding";
	case TokenType::OnError: return "onerror";
	case TokenType::MaxBytes: return "maxbytes";
	case TokenType::MaxItems: return "maxitems";
	case TokenType::MaxAtoms: return "maxatoms";
	case TokenType::MaxString: return "maxstring";
	case TokenType::Skip: return "skip";
	case TokenType::Emit: return "emit";
	case TokenType::Little: return "little";
	case TokenType::Big: return "big";
	case TokenType::Error_: return "error";
	case TokenType::Drop: return "drop";
	case TokenType::Pass: return "pass";
	case TokenType::U8: return "u8";
	case TokenType::I8: return "i8";
	case TokenType::U16: return "u16";
	case TokenType::I16: return "i16";
	case TokenType::U32: return "u32";
	case TokenType::I32: return "i32";
	case TokenType::U64: return "u64";
	case TokenType::I64: return "i64";
	case TokenType::F32: return "f32";
	case TokenType::F64: return "f64";
	case TokenType::Bytes: return "bytes";
	case TokenType::String: return "string";
	case TokenType::Ident: return "identifier";
	case TokenType::Integer: return "integer";
	case TokenType::Float_: return "float";
	case TokenType::StringLiteral: return "string";
	case TokenType::LBrace: return "{";
	case TokenType::RBrace: return "}";
	case TokenType::LBracket: return "[";
	case TokenType::RBracket: return "]";
	case TokenType::Equals: return "=";
	case TokenType::Semicolon: return ";";
	case TokenType::At: return "@";
	case TokenType::Asterisk: return "*";
	case TokenType::Eof: return "eof";
	case TokenType::Unknown: return "unknown";
	}
	return "?";
}

static bool is_ident_start(char c) {
	return std::isalpha((unsigned char)c) || c == '_' || c == '/';
}

static bool is_ident_char(char c) {
	return std::isalnum((unsigned char)c) || c == '_' || c == '/' || c == '.';
}

struct KeywordEntry {
	const char *name;
	TokenType type;
};

static const KeywordEntry keywords[] = {
	{"schema", TokenType::Schema},
	{"endian", TokenType::Endian},
	{"encoding", TokenType::Encoding},
	{"onerror", TokenType::OnError},
	{"maxbytes", TokenType::MaxBytes},
	{"maxitems", TokenType::MaxItems},
	{"maxatoms", TokenType::MaxAtoms},
	{"maxstring", TokenType::MaxString},
	{"skip", TokenType::Skip},
	{"emit", TokenType::Emit},
	{"little", TokenType::Little},
	{"big", TokenType::Big},
	{"error", TokenType::Error_},
	{"drop", TokenType::Drop},
	{"pass", TokenType::Pass},
	{"u8", TokenType::U8},
	{"i8", TokenType::I8},
	{"u16", TokenType::U16},
	{"i16", TokenType::I16},
	{"u32", TokenType::U32},
	{"i32", TokenType::I32},
	{"u64", TokenType::U64},
	{"i64", TokenType::I64},
	{"f32", TokenType::F32},
	{"f64", TokenType::F64},
	{"bytes", TokenType::Bytes},
	{"string", TokenType::String},
};

LexerResult lex(const std::string &source) {
	LexerResult result;
	int line = 1;
	int column = 1;
	size_t pos = 0;

	while (pos < source.size()) {
		char c = source[pos];

		if (c == '\n') {
			++line;
			column = 1;
			++pos;
			continue;
		}
		if (std::isspace((unsigned char)c)) {
			++column;
			++pos;
			continue;
		}

		if (c == '/' && pos + 1 < source.size() && source[pos + 1] == '/') {
			while (pos < source.size() && source[pos] != '\n') {
				++pos;
			}
			continue;
		}

		if (c == '/' && pos + 1 < source.size() && source[pos + 1] == '*') {
			pos += 2;
			column += 2;
			while (pos + 1 < source.size() && !(source[pos] == '*' && source[pos + 1] == '/')) {
				if (source[pos] == '\n') {
					++line;
					column = 1;
				} else {
					++column;
				}
				++pos;
			}
			if (pos + 1 < source.size()) {
				pos += 2;
				column += 2;
			}
			continue;
		}

		if (c == '{') {
			result.tokens.push_back({TokenType::LBrace, "{", line, column});
			++pos; ++column; continue;
		}
		if (c == '}') {
			result.tokens.push_back({TokenType::RBrace, "}", line, column});
			++pos; ++column; continue;
		}
		if (c == '[') {
			result.tokens.push_back({TokenType::LBracket, "[", line, column});
			++pos; ++column; continue;
		}
		if (c == ']') {
			result.tokens.push_back({TokenType::RBracket, "]", line, column});
			++pos; ++column; continue;
		}
		if (c == '=') {
			result.tokens.push_back({TokenType::Equals, "=", line, column});
			++pos; ++column; continue;
		}
		if (c == ';') {
			result.tokens.push_back({TokenType::Semicolon, ";", line, column});
			++pos; ++column; continue;
		}
		if (c == '@') {
			result.tokens.push_back({TokenType::At, "@", line, column});
			++pos; ++column; continue;
		}
		if (c == '*') {
			result.tokens.push_back({TokenType::Asterisk, "*", line, column});
			++pos; ++column; continue;
		}

		if (c == '"') {
			int start_col = column;
			++pos; ++column;
			std::string value;
			while (pos < source.size() && source[pos] != '"') {
				if (source[pos] == '\\' && pos + 1 < source.size()) {
					++pos; ++column;
					switch (source[pos]) {
					case 'n': value += '\n'; break;
					case 't': value += '\t'; break;
					case '\\': value += '\\'; break;
					case '"': value += '"'; break;
					default: value += source[pos]; break;
					}
				} else {
					value += source[pos];
				}
				++pos; ++column;
			}
			if (pos < source.size()) {
				++pos; ++column;
			}
			result.tokens.push_back({TokenType::StringLiteral, value, line, start_col});
			continue;
		}

		if (c == '0' && pos + 1 < source.size() && (source[pos + 1] == 'x' || source[pos + 1] == 'X')) {
			int start_col = column;
			size_t start = pos;
			pos += 2; column += 2;
			while (pos < source.size() && std::isxdigit((unsigned char)source[pos])) {
				++pos; ++column;
			}
			result.tokens.push_back({TokenType::Integer, source.substr(start, pos - start), line, start_col});
			continue;
		}

		if (std::isdigit((unsigned char)c) || (c == '-' && pos + 1 < source.size() && std::isdigit((unsigned char)source[pos + 1]))) {
			int start_col = column;
			size_t start = pos;
			bool is_float = false;
			if (c == '-') {
				++pos; ++column;
			}
			while (pos < source.size() && std::isdigit((unsigned char)source[pos])) {
				++pos; ++column;
			}
			if (pos < source.size() && source[pos] == '.') {
				is_float = true;
				++pos; ++column;
				while (pos < source.size() && std::isdigit((unsigned char)source[pos])) {
					++pos; ++column;
				}
			}
			result.tokens.push_back({is_float ? TokenType::Float_ : TokenType::Integer, source.substr(start, pos - start), line, start_col});
			continue;
		}

		if (is_ident_start(c)) {
			int start_col = column;
			size_t start = pos;
			while (pos < source.size() && is_ident_char(source[pos])) {
				++pos; ++column;
			}
			std::string word = source.substr(start, pos - start);
			TokenType type = TokenType::Ident;
			for (auto &kw : keywords) {
				if (word == kw.name) {
					type = kw.type;
					break;
				}
			}
			result.tokens.push_back({type, word, line, start_col});
			continue;
		}

		result.error = "unexpected character '" + std::string(1, c) + "' at line " + std::to_string(line) + " column " + std::to_string(column);
		return result;
	}

	result.tokens.push_back({TokenType::Eof, "", line, column});
	return result;
}

} // namespace bbb::dsl
