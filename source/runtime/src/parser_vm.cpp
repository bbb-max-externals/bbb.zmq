#include "bbb/parser_vm.hpp"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace bbb {

template <typename T>
static T read_value(const uint8_t *data, Endian endian) {
	T value;
	std::memcpy(&value, data, sizeof(T));
	if (endian == Endian::Big) {
		uint8_t buf[sizeof(T)];
		std::memcpy(buf, &value, sizeof(T));
		for (size_t i = 0; i < sizeof(T) / 2; ++i) {
			std::swap(buf[i], buf[sizeof(T) - 1 - i]);
		}
		std::memcpy(&value, buf, sizeof(T));
	}
	return value;
}

static bool is_valid_utf8(const uint8_t *data, size_t len) {
	size_t i = 0;
	while (i < len) {
		uint8_t c = data[i];
		size_t seq_len = 0;
		if (c < 0x80) {
			seq_len = 1;
		} else if ((c & 0xE0) == 0xC0) {
			seq_len = 2;
		} else if ((c & 0xF0) == 0xE0) {
			seq_len = 3;
		} else if ((c & 0xF8) == 0xF0) {
			seq_len = 4;
		} else {
			return false;
		}
		if (i + seq_len > len) return false;
		for (size_t j = 1; j < seq_len; ++j) {
			if ((data[i + j] & 0xC0) != 0x80) return false;
		}
		i += seq_len;
	}
	return true;
}

struct FieldValue {
	enum Kind { Int, Float, Bytes_, String_, Matrix_ };
	Kind kind;
	int64_t int_value = 0;
	double float_value = 0.0;
	std::vector<uint8_t> bytes_value;
	std::string string_value;
	std::vector<FieldValue> array_values;
	int64_t matrix_planes = 0;
	int64_t matrix_dim1 = 0;
	int64_t matrix_dim2 = 0;
	PrimitiveType matrix_celltype = PrimitiveType::U8;
};

struct ParseContext {
	const CompiledSchema &schema;
	const Frame &frame;
	size_t offset = 0;
	uint64_t maxbytes;
	uint64_t maxatoms;
	std::unordered_map<std::string, FieldValue> fields;
	std::vector<ParseOutput> outputs;
	std::vector<ParseDiag> errors;
	bool failed = false;

	ParseContext(const CompiledSchema &s, const Frame &f, uint64_t mb, uint64_t ma)
		: schema(s), frame(f), maxbytes(mb), maxatoms(ma) {}

	bool check_bounds(size_t needed) {
		if (offset + needed > frame.size()) {
			ParseDiag diag;
			diag.code = ParseDiag::OutOfBounds;
			diag.message = "out of bounds at offset " + std::to_string(offset) + " need " + std::to_string(needed) + " have " + std::to_string(frame.size() - offset);
			errors.push_back(diag);
			failed = true;
			return false;
		}
		return true;
	}

	void read_scalar(PrimitiveType pt, const std::string &name) {
		size_t sz = primitive_size(pt);
		if (!check_bounds(sz)) return;
		const uint8_t *ptr = frame.data() + offset;
		offset += sz;

		FieldValue fv;
		switch (pt) {
		case PrimitiveType::U8:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<uint8_t>(ptr, schema.endian); break;
		case PrimitiveType::I8:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<int8_t>(ptr, schema.endian); break;
		case PrimitiveType::U16:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<uint16_t>(ptr, schema.endian); break;
		case PrimitiveType::I16:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<int16_t>(ptr, schema.endian); break;
		case PrimitiveType::U32:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<uint32_t>(ptr, schema.endian); break;
		case PrimitiveType::I32:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<int32_t>(ptr, schema.endian); break;
		case PrimitiveType::U64:
			fv.kind = FieldValue::Int; fv.int_value = (int64_t)read_value<uint64_t>(ptr, schema.endian); break;
		case PrimitiveType::I64:
			fv.kind = FieldValue::Int; fv.int_value = read_value<int64_t>(ptr, schema.endian); break;
		case PrimitiveType::F32:
			fv.kind = FieldValue::Float; fv.float_value = read_value<float>(ptr, schema.endian); break;
		case PrimitiveType::F64:
			fv.kind = FieldValue::Float; fv.float_value = read_value<double>(ptr, schema.endian); break;
		}
		fields[name] = std::move(fv);
	}

	void read_fixed_array(PrimitiveType pt, const std::string &name, int64_t count) {
		size_t elem_sz = primitive_size(pt);
		size_t total = (size_t)count * elem_sz;
		if (!check_bounds(total)) return;

		FieldValue fv;
		fv.kind = FieldValue::Bytes_;
		for (int64_t i = 0; i < count; ++i) {
			const uint8_t *ptr = frame.data() + offset;
			FieldValue elem;
			switch (pt) {
			case PrimitiveType::U8:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<uint8_t>(ptr, schema.endian); break;
			case PrimitiveType::I8:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<int8_t>(ptr, schema.endian); break;
			case PrimitiveType::U16:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<uint16_t>(ptr, schema.endian); break;
			case PrimitiveType::I16:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<int16_t>(ptr, schema.endian); break;
			case PrimitiveType::U32:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<uint32_t>(ptr, schema.endian); break;
			case PrimitiveType::I32:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<int32_t>(ptr, schema.endian); break;
			case PrimitiveType::U64:
				elem.kind = FieldValue::Int; elem.int_value = (int64_t)read_value<uint64_t>(ptr, schema.endian); break;
			case PrimitiveType::I64:
				elem.kind = FieldValue::Int; elem.int_value = read_value<int64_t>(ptr, schema.endian); break;
			case PrimitiveType::F32:
				elem.kind = FieldValue::Float; elem.float_value = read_value<float>(ptr, schema.endian); break;
			case PrimitiveType::F64:
				elem.kind = FieldValue::Float; elem.float_value = read_value<double>(ptr, schema.endian); break;
			}
			fv.array_values.push_back(std::move(elem));
			offset += elem_sz;
		}
		fields[name] = std::move(fv);
	}

	void read_var_array(PrimitiveType pt, const std::string &name, const std::string &length_field) {
		auto it = fields.find(length_field);
		if (it == fields.end() || it->second.kind != FieldValue::Int) {
			ParseDiag diag;
			diag.code = ParseDiag::VarLengthNegative;
			diag.message = "length field '" + length_field + "' not found or not integer";
			errors.push_back(diag);
			failed = true;
			return;
		}
		int64_t count = it->second.int_value;
		if (count < 0) {
			ParseDiag diag;
			diag.code = ParseDiag::VarLengthNegative;
			diag.message = "negative array length " + std::to_string(count);
			errors.push_back(diag);
			failed = true;
			return;
		}
		if ((uint64_t)count > schema.maxitems) {
			ParseDiag diag;
			diag.code = ParseDiag::VarLengthOverflow;
			diag.message = "array length " + std::to_string(count) + " exceeds maxitems " + std::to_string(schema.maxitems);
			errors.push_back(diag);
			failed = true;
			return;
		}
		read_fixed_array(pt, name, count);
	}

	void read_fixed_bytes(const std::string &name, int64_t count) {
		if ((uint64_t)count > maxbytes) {
			ParseDiag diag;
			diag.code = ParseDiag::OutOfBounds;
			diag.message = "bytes length " + std::to_string(count) + " exceeds maxbytes " + std::to_string(maxbytes);
			errors.push_back(diag);
			failed = true;
			return;
		}
		if (!check_bounds((size_t)count)) return;
		FieldValue fv;
		fv.kind = FieldValue::Bytes_;
		fv.bytes_value.assign(frame.data() + offset, frame.data() + offset + count);
		offset += count;
		fields[name] = std::move(fv);
	}

	void read_var_bytes(const std::string &name, const std::string &length_field) {
		auto it = fields.find(length_field);
		if (it == fields.end() || it->second.kind != FieldValue::Int) {
			ParseDiag diag;
			diag.code = ParseDiag::VarLengthNegative;
			diag.message = "length field '" + length_field + "' not found or not integer";
			errors.push_back(diag);
			failed = true;
			return;
		}
		int64_t count = it->second.int_value;
		if (count < 0) {
			ParseDiag diag;
			diag.code = ParseDiag::VarLengthNegative;
			diag.message = "negative bytes length " + std::to_string(count);
			errors.push_back(diag);
			failed = true;
			return;
		}
		read_fixed_bytes(name, count);
	}

	void read_remaining_bytes(const std::string &name) {
		size_t remaining = frame.size() - offset;
		FieldValue fv;
		fv.kind = FieldValue::Bytes_;
		fv.bytes_value.assign(frame.data() + offset, frame.data() + offset + remaining);
		offset = frame.size();
		fields[name] = std::move(fv);
	}

	void read_null_string(const std::string &name) {
		size_t start = offset;
		while (offset < frame.size()) {
			if (offset - start > schema.maxstring) {
				ParseDiag diag;
				diag.code = ParseDiag::StringTooLong;
				diag.message = "string field '" + name + "' exceeds maxstring " + std::to_string(schema.maxstring);
				errors.push_back(diag);
				failed = true;
				return;
			}
			if (frame[offset] == 0x00) {
				break;
			}
			++offset;
		}
		if (offset >= frame.size()) {
			ParseDiag diag;
			diag.code = ParseDiag::UnterminatedString;
			diag.message = "unterminated string field '" + name + "'";
			errors.push_back(diag);
			failed = true;
			return;
		}

		size_t len = offset - start;
		if (schema.encoding == Encoding::UTF8 && !is_valid_utf8(frame.data() + start, len)) {
			ParseDiag diag;
			diag.code = ParseDiag::InvalidUTF8;
			diag.message = "invalid utf8 in string field '" + name + "'";
			errors.push_back(diag);
			failed = true;
			return;
		}

		FieldValue fv;
		fv.kind = FieldValue::String_;
		fv.string_value.assign(reinterpret_cast<const char *>(frame.data() + start), len);
		++offset;
		fields[name] = std::move(fv);
	}

	void read_fixed_string(const std::string &name, int64_t size) {
		if (!check_bounds((size_t)size)) return;
		size_t content_len = 0;
		for (int64_t i = 0; i < size; ++i) {
			if (frame[offset + i] == 0x00) {
				content_len = (size_t)i;
				break;
			}
		}
		if (content_len == 0) {
			bool found_nul = false;
			for (int64_t i = 0; i < size; ++i) {
				if (frame[offset + i] == 0x00) {
					found_nul = true;
					break;
				}
			}
			if (!found_nul) content_len = (size_t)size;
		}

		if (schema.encoding == Encoding::UTF8 && !is_valid_utf8(frame.data() + offset, content_len)) {
			ParseDiag diag;
			diag.code = ParseDiag::InvalidUTF8;
			diag.message = "invalid utf8 in fixed string field '" + name + "'";
			errors.push_back(diag);
			failed = true;
			offset += size;
			return;
		}

		FieldValue fv;
		fv.kind = FieldValue::String_;
		fv.string_value.assign(reinterpret_cast<const char *>(frame.data() + offset), content_len);
		offset += size;
		fields[name] = std::move(fv);
	}

	int64_t resolve_dim(int64_t literal, const std::string &field_name) {
		if (!field_name.empty()) {
			auto it = fields.find(field_name);
			if (it == fields.end() || it->second.kind != FieldValue::Int) {
				ParseDiag diag;
				diag.code = ParseDiag::VarLengthNegative;
				diag.message = "matrix dimension field '" + field_name + "' not found or not integer";
				errors.push_back(diag);
				failed = true;
				return 0;
			}
			return it->second.int_value;
		}
		return literal;
	}

	void read_matrix(PrimitiveType pt, const std::string &name,
		int64_t planes_lit, const std::string &planes_field,
		int64_t dim1_lit, const std::string &dim1_field,
		int64_t dim2_lit, const std::string &dim2_field)
	{
		int64_t planes = resolve_dim(planes_lit, planes_field);
		if (failed) return;
		if (planes <= 0) planes = 1;

		int64_t d1 = resolve_dim(dim1_lit, dim1_field);
		if (failed) return;
		int64_t d2 = resolve_dim(dim2_lit, dim2_field);
		if (failed) return;

		if (d1 <= 0 || d2 <= 0) {
			ParseDiag diag;
			diag.code = ParseDiag::OutOfBounds;
			diag.message = "matrix dims must be > 0 for field '" + name + "'";
			errors.push_back(diag);
			failed = true;
			return;
		}

		size_t cell_sz = primitive_size(pt);
		size_t total = (size_t)(planes * d1 * d2) * cell_sz;
		if (!check_bounds(total)) return;

		FieldValue fv;
		fv.kind = FieldValue::Matrix_;
		fv.matrix_planes = planes;
		fv.matrix_dim1 = d1;
		fv.matrix_dim2 = d2;
		fv.matrix_celltype = pt;
		fv.bytes_value.assign(frame.data() + offset, frame.data() + offset + total);
		offset += total;
		fields[name] = std::move(fv);
	}

	void validate_const(const std::string &name, PrimitiveType pt, uint64_t expected) {
		auto it = fields.find(name);
		if (it == fields.end()) return;
		uint64_t actual = 0;
		if (it->second.kind == FieldValue::Int) {
			actual = (uint64_t)it->second.int_value;
		}
		if (actual != expected) {
			ParseDiag diag;
			diag.code = ParseDiag::ConstMismatch;
			char buf[128];
			snprintf(buf, sizeof(buf), "const mismatch field %s expected 0x%llX got 0x%llX",
				name.c_str(), (unsigned long long)expected, (unsigned long long)actual);
			diag.message = buf;
			errors.push_back(diag);
			if (schema.onerror == OnError::Error) {
				failed = true;
			}
		}
	}

	void do_emit(const std::string &selector, const std::vector<dsl::EmitExpr> &exprs) {
		ParseOutput out;
		out.selector = selector;
		uint64_t atom_count = 0;

		for (auto &expr : exprs) {
			auto it = fields.find(expr.field_name);
			if (it == fields.end()) continue;
			auto &fv = it->second;

			if (expr.modifier == "handle") {
				out.atoms.push_back("zmqbytes_" + expr.field_name);
				++atom_count;
			} else if (expr.modifier == "matrix") {
				if (fv.kind == FieldValue::Matrix_) {
					MatrixData md;
					md.celltype = fv.matrix_celltype;
					md.planes = fv.matrix_planes;
					md.dim1 = fv.matrix_dim1;
					md.dim2 = fv.matrix_dim2;
					md.data = fv.bytes_value;
					out.atoms.push_back(std::move(md));
					++atom_count;
				}
			} else if (expr.modifier == "list") {
				if (fv.kind == FieldValue::Bytes_) {
					for (auto b : fv.bytes_value) {
						out.atoms.push_back((int64_t)b);
						++atom_count;
					}
				}
			} else if (expr.modifier == "string") {
				if (fv.kind == FieldValue::Bytes_) {
					std::string s(reinterpret_cast<const char *>(fv.bytes_value.data()), fv.bytes_value.size());
					if (schema.encoding == Encoding::UTF8 && !is_valid_utf8(fv.bytes_value.data(), fv.bytes_value.size())) {
						ParseDiag diag;
						diag.code = ParseDiag::InvalidUTF8;
						diag.message = "invalid utf8 in @string for field '" + expr.field_name + "'";
						errors.push_back(diag);
						continue;
					}
					out.atoms.push_back(s);
					++atom_count;
				}
			} else if (expr.modifier == "cstring") {
				if (fv.kind == FieldValue::Bytes_) {
					size_t len = 0;
					for (auto b : fv.bytes_value) {
						if (b == 0x00) break;
						++len;
					}
					if (schema.encoding == Encoding::UTF8 && !is_valid_utf8(fv.bytes_value.data(), len)) {
						ParseDiag diag;
						diag.code = ParseDiag::InvalidUTF8;
						diag.message = "invalid utf8 in @cstring for field '" + expr.field_name + "'";
						errors.push_back(diag);
						continue;
					}
					std::string s(reinterpret_cast<const char *>(fv.bytes_value.data()), len);
					out.atoms.push_back(s);
					++atom_count;
				}
			} else if (expr.index == -2) {
				if (fv.kind == FieldValue::Bytes_ || !fv.array_values.empty()) {
					for (auto &elem : fv.array_values) {
						if (elem.kind == FieldValue::Int) {
							out.atoms.push_back(elem.int_value);
						} else if (elem.kind == FieldValue::Float) {
							out.atoms.push_back(elem.float_value);
						}
						++atom_count;
					}
				}
			} else if (expr.index >= 0) {
				if (fv.kind == FieldValue::Bytes_ && expr.index < (int)fv.array_values.size()) {
					auto &elem = fv.array_values[expr.index];
					if (elem.kind == FieldValue::Int) {
						out.atoms.push_back(elem.int_value);
					} else if (elem.kind == FieldValue::Float) {
						out.atoms.push_back(elem.float_value);
					}
					++atom_count;
				}
			} else {
				if (fv.kind == FieldValue::Int) {
					out.atoms.push_back(fv.int_value);
					++atom_count;
				} else if (fv.kind == FieldValue::Float) {
					out.atoms.push_back(fv.float_value);
					++atom_count;
				} else if (fv.kind == FieldValue::String_) {
					out.atoms.push_back(fv.string_value);
					++atom_count;
				}
			}

			if (atom_count > maxatoms) {
				ParseDiag diag;
				diag.code = ParseDiag::TooManyAtoms;
				diag.message = "too many atoms " + std::to_string(atom_count) + " max " + std::to_string(maxatoms);
				errors.push_back(diag);
				failed = true;
				return;
			}
		}

		outputs.push_back(std::move(out));
	}
};

ParseResult parse_frame(const CompiledSchema &schema, const Frame &frame) {
	return parse_frame(schema, frame, schema.maxbytes, schema.maxatoms);
}

ParseResult parse_frame(const CompiledSchema &schema, const Frame &frame, uint64_t maxbytes_override, uint64_t maxatoms_override) {
	ParseContext ctx(schema, frame, maxbytes_override, maxatoms_override);

	for (auto &instr : schema.instructions) {
		if (ctx.failed && schema.onerror == OnError::Error) break;

		switch (instr.kind) {
		case Instr::ReadScalar:
			ctx.read_scalar(instr.primitive, instr.field_name);
			break;
		case Instr::ReadFixedArray:
			ctx.read_fixed_array(instr.primitive, instr.field_name, instr.fixed_count);
			break;
		case Instr::ReadVarArray:
			ctx.read_var_array(instr.primitive, instr.field_name, instr.length_field);
			break;
		case Instr::ReadFixedBytes:
			ctx.read_fixed_bytes(instr.field_name, instr.fixed_count);
			break;
		case Instr::ReadVarBytes:
			ctx.read_var_bytes(instr.field_name, instr.length_field);
			break;
		case Instr::ReadRemainingBytes:
			ctx.read_remaining_bytes(instr.field_name);
			break;
		case Instr::ReadNullString:
			ctx.read_null_string(instr.field_name);
			break;
		case Instr::ReadFixedString:
			ctx.read_fixed_string(instr.field_name, instr.fixed_count);
			break;
		case Instr::ReadMatrix:
			ctx.read_matrix(instr.primitive, instr.field_name,
				instr.matrix_planes, instr.matrix_planes_field,
				instr.matrix_dim1, instr.matrix_dim1_field,
				instr.matrix_dim2, instr.matrix_dim2_field);
			break;
		case Instr::Skip_:
			if (ctx.check_bounds((size_t)instr.skip_bytes)) {
				ctx.offset += instr.skip_bytes;
			}
			break;
		case Instr::ValidateConst:
			ctx.validate_const(instr.field_name, instr.primitive, instr.const_value);
			break;
		case Instr::Emit_:
			if (!ctx.failed) {
				ctx.do_emit(instr.emit_selector, instr.emit_exprs);
			}
			break;
		}
	}

	ParseResult result;
	result.outputs = std::move(ctx.outputs);
	result.errors = std::move(ctx.errors);
	return result;
}

} // namespace bbb
