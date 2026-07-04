#include "bbb/dsl_ast.hpp"
#include <cstdlib>
#include <cstring>
#include <optional>

namespace bbb::dsl {

class Parser {
public:
	Parser(const std::vector<Token> &tokens)
		: tokens_(tokens)
		, pos_(0) {}

	ParseResult parse_all() {
		ParseResult result;
		while (!at_end() && !match(TokenType::Eof)) {
			if (check(TokenType::Schema)) {
				auto schema = parse_schema();
				if (schema) {
					result.program.schemas.push_back(std::move(*schema));
				}
			} else {
				add_error(result, "expected schema declaration");
				advance();
			}
		}
		return result;
	}

private:
	const std::vector<Token> &tokens_;
	size_t pos_;

	bool at_end() const { return pos_ >= tokens_.size() || tokens_[pos_].type == TokenType::Eof; }
	const Token &current() const { return tokens_[pos_]; }
	const Token &peek(size_t offset = 0) const { return pos_ + offset < tokens_.size() ? tokens_[pos_ + offset] : tokens_.back(); }

	bool check(TokenType t) const { return !at_end() && current().type == t; }
	bool match(TokenType t) {
		if (check(t)) {
			advance();
			return true;
		}
		return false;
	}
	const Token &advance() {
		if (!at_end()) ++pos_;
		return tokens_[pos_ - 1];
	}
	const Token &consume(TokenType t, const std::string &msg) {
		if (check(t)) return advance();
		return advance();
	}

	void add_error(ParseResult &result, const std::string &msg) {
		ParseError err;
		err.message = msg;
		if (!at_end()) {
			err.line = current().line;
			err.column = current().column;
		}
		result.errors.push_back(err);
	}

	std::optional<SchemaDecl> parse_schema() {
		SchemaDecl schema;
		consume(TokenType::Schema, "expected 'schema'");
		if (at_end() || current().type != TokenType::Ident) {
			return std::nullopt;
		}
		schema.name = advance().text;
		consume(TokenType::LBrace, "expected '{'");

		while (!at_end() && !check(TokenType::RBrace)) {
			if (check(TokenType::Endian) || check(TokenType::Encoding) || check(TokenType::OnError) ||
				check(TokenType::MaxBytes) || check(TokenType::MaxItems) || check(TokenType::MaxAtoms) || check(TokenType::MaxString)) {
				parse_directive(schema);
			} else if (check(TokenType::Emit)) {
				auto emit = parse_emit();
				if (emit) schema.emits.push_back(std::move(*emit));
			} else if (check(TokenType::Skip)) {
				auto field = parse_skip();
				if (field) schema.fields.push_back(std::move(*field));
			} else if (is_primitive()) {
				auto field = parse_field();
				if (field) schema.fields.push_back(std::move(*field));
			} else if (check(TokenType::Bytes)) {
				auto field = parse_bytes_field();
				if (field) schema.fields.push_back(std::move(*field));
			} else if (check(TokenType::String)) {
				auto field = parse_string_field();
				if (field) schema.fields.push_back(std::move(*field));
			} else if (check(TokenType::Matrix)) {
				auto field = parse_matrix_field();
				if (field) schema.fields.push_back(std::move(*field));
			} else {
				advance();
			}
		}

		consume(TokenType::RBrace, "expected '}'");
		return schema;
	}

	bool is_primitive() const {
		if (at_end()) return false;
		auto t = current().type;
		return t == TokenType::U8 || t == TokenType::I8 ||
			t == TokenType::U16 || t == TokenType::I16 ||
			t == TokenType::U32 || t == TokenType::I32 ||
			t == TokenType::U64 || t == TokenType::I64 ||
			t == TokenType::F32 || t == TokenType::F64;
	}

	void parse_directive(SchemaDecl &schema) {
		auto tok = advance();
		Directive d;
		d.value = 0;

		switch (tok.type) {
		case TokenType::Endian: {
			d.kind = Directive::Endian_;
			if (match(TokenType::Little)) {
				d.value = std::string("little");
			} else if (match(TokenType::Big)) {
				d.value = std::string("big");
			}
			break;
		}
		case TokenType::Encoding: {
			d.kind = Directive::Encoding_;
			if (check(TokenType::Ident)) {
				d.value = advance().text;
			}
			break;
		}
		case TokenType::OnError: {
			d.kind = Directive::OnError_;
			if (match(TokenType::Error_)) d.value = std::string("error");
			else if (match(TokenType::Drop)) d.value = std::string("drop");
			else if (match(TokenType::Pass)) d.value = std::string("pass");
			break;
		}
		case TokenType::MaxBytes: {
			d.kind = Directive::MaxBytes_;
			if (check(TokenType::Integer)) d.value = parse_uint();
			break;
		}
		case TokenType::MaxItems: {
			d.kind = Directive::MaxItems_;
			if (check(TokenType::Integer)) d.value = parse_uint();
			break;
		}
		case TokenType::MaxAtoms: {
			d.kind = Directive::MaxAtoms_;
			if (check(TokenType::Integer)) d.value = parse_uint();
			break;
		}
		case TokenType::MaxString: {
			d.kind = Directive::MaxString_;
			if (check(TokenType::Integer)) d.value = parse_uint();
			break;
		}
		default: break;
		}
		consume(TokenType::Semicolon, "expected ';'");
		schema.directives.push_back(d);
	}

	uint64_t parse_uint() {
		auto tok = advance();
		if (tok.text.substr(0, 2) == "0x" || tok.text.substr(0, 2) == "0X") {
			return std::strtoull(tok.text.c_str(), nullptr, 16);
		}
		return std::strtoull(tok.text.c_str(), nullptr, 10);
	}

	std::optional<FieldDecl> parse_field() {
		FieldDecl field;
		field.primitive_type = advance().text;

		if (!check(TokenType::Ident)) return std::nullopt;
		field.name = advance().text;

		if (match(TokenType::LBracket)) {
			if (check(TokenType::Integer)) {
				field.fixed_size = (int64_t)parse_uint();
				field.kind = FieldDecl::FixedArray;
			} else if (check(TokenType::Ident)) {
				field.length_field = advance().text;
				field.kind = FieldDecl::VarArray;
			}
			consume(TokenType::RBracket, "expected ']'");
		} else {
			field.kind = FieldDecl::Scalar;
		}

		if (match(TokenType::Equals)) {
			field.has_const = true;
			field.const_value = parse_uint();
		}

		consume(TokenType::Semicolon, "expected ';'");
		return field;
	}

	std::optional<FieldDecl> parse_bytes_field() {
		FieldDecl field;
		advance();

		consume(TokenType::LBracket, "expected '['");
		if (check(TokenType::Asterisk)) {
			advance();
			field.kind = FieldDecl::RemainingBytes;
		} else if (check(TokenType::Integer)) {
			field.fixed_size = (int64_t)parse_uint();
			field.kind = FieldDecl::FixedBytes;
		} else if (check(TokenType::Ident)) {
			field.length_field = advance().text;
			field.kind = FieldDecl::VarBytes;
		}
		consume(TokenType::RBracket, "expected ']'");

		if (!check(TokenType::Ident)) return std::nullopt;
		field.name = advance().text;

		consume(TokenType::Semicolon, "expected ';'");
		return field;
	}

	std::optional<FieldDecl> parse_string_field() {
		FieldDecl field;
		advance();

		if (match(TokenType::LBracket)) {
			if (check(TokenType::Integer)) {
				field.fixed_size = (int64_t)parse_uint();
				field.kind = FieldDecl::FixedString;
			}
			consume(TokenType::RBracket, "expected ']'");
		} else {
			field.kind = FieldDecl::NullString;
		}

		if (!check(TokenType::Ident)) return std::nullopt;
		field.name = advance().text;

		consume(TokenType::Semicolon, "expected ';'");
		return field;
	}

	std::optional<FieldDecl> parse_matrix_field() {
		FieldDecl field;
		field.kind = FieldDecl::Matrix_;
		advance();

		if (!is_primitive()) return std::nullopt;
		field.primitive_type = advance().text;

		if (match(TokenType::Colon)) {
			if (check(TokenType::Integer)) {
				field.matrix_planes = (int64_t)parse_uint();
			} else if (check(TokenType::Ident)) {
				field.matrix_planes_field = advance().text;
			}
		}

		consume(TokenType::LBracket, "expected '[' for matrix dim1");
		if (check(TokenType::Integer)) {
			field.matrix_dim1 = (int64_t)parse_uint();
		} else if (check(TokenType::Ident)) {
			field.matrix_dim1_field = advance().text;
		}
		consume(TokenType::RBracket, "expected ']'");

		consume(TokenType::LBracket, "expected '[' for matrix dim2");
		if (check(TokenType::Integer)) {
			field.matrix_dim2 = (int64_t)parse_uint();
		} else if (check(TokenType::Ident)) {
			field.matrix_dim2_field = advance().text;
		}
		consume(TokenType::RBracket, "expected ']'");

		if (!check(TokenType::Ident)) return std::nullopt;
		field.name = advance().text;

		consume(TokenType::Semicolon, "expected ';'");
		return field;
	}

	std::optional<FieldDecl> parse_skip() {
		FieldDecl field;
		advance();
		field.kind = FieldDecl::Skip_;
		if (check(TokenType::Integer)) {
			field.skip_bytes = (int64_t)parse_uint();
		}
		consume(TokenType::Semicolon, "expected ';'");
		return field;
	}

	std::optional<EmitDecl> parse_emit() {
		EmitDecl emit;
		advance();

		if (check(TokenType::Semicolon)) {
			advance();
			return std::nullopt;
		}

		if (check(TokenType::Ident)) {
			auto first = advance().text;
			if (check(TokenType::Semicolon)) {
				emit.selector = first;
				emit.short_form = true;
				EmitExpr expr;
				expr.field_name = first;
				emit.expressions.push_back(expr);
				advance();
				return emit;
			}
			emit.selector = first;
		} else if (check(TokenType::StringLiteral)) {
			emit.selector = advance().text;
		}

		while (!at_end() && !check(TokenType::Semicolon)) {
			EmitExpr expr;
			if (check(TokenType::Ident)) {
				expr.field_name = advance().text;
				if (match(TokenType::LBracket)) {
					if (check(TokenType::Integer)) {
						expr.index = (int)parse_uint();
					} else if (check(TokenType::Asterisk)) {
						advance();
						expr.index = -2;
					}
					consume(TokenType::RBracket, "expected ']'");
				}
				if (match(TokenType::At)) {
					if (check(TokenType::Ident) || check(TokenType::String) || check(TokenType::Matrix)) {
						expr.modifier = advance().text;
					}
				}
			} else if (check(TokenType::Integer)) {
				expr.field_name = advance().text;
			} else if (check(TokenType::Float_)) {
				expr.field_name = advance().text;
			} else if (check(TokenType::StringLiteral)) {
				expr.field_name = advance().text;
			} else {
				break;
			}
			emit.expressions.push_back(expr);
		}

		consume(TokenType::Semicolon, "expected ';'");
		return emit;
	}
};

ParseResult parse(const std::vector<Token> &tokens) {
	Parser p(tokens);
	return p.parse_all();
}

} // namespace bbb::dsl
