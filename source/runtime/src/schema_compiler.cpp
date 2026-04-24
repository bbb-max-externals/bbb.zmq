#include "bbb/schema.hpp"
#include "bbb/dsl_ast.hpp"
#include <cstring>
#include <cstdlib>

namespace bbb {

size_t primitive_size(PrimitiveType t) {
	switch (t) {
	case PrimitiveType::U8: case PrimitiveType::I8: return 1;
	case PrimitiveType::U16: case PrimitiveType::I16: return 2;
	case PrimitiveType::U32: case PrimitiveType::I32: return 4;
	case PrimitiveType::U64: case PrimitiveType::I64: return 8;
	case PrimitiveType::F32: return 4;
	case PrimitiveType::F64: return 8;
	}
	return 0;
}

static PrimitiveType token_to_primitive(const std::string &name) {
	if (name == "u8") return PrimitiveType::U8;
	if (name == "i8") return PrimitiveType::I8;
	if (name == "u16") return PrimitiveType::U16;
	if (name == "i16") return PrimitiveType::I16;
	if (name == "u32") return PrimitiveType::U32;
	if (name == "i32") return PrimitiveType::I32;
	if (name == "u64") return PrimitiveType::U64;
	if (name == "i64") return PrimitiveType::I64;
	if (name == "f32") return PrimitiveType::F32;
	if (name == "f64") return PrimitiveType::F64;
	return PrimitiveType::U8;
}

static bool is_unsigned_integer(PrimitiveType t) {
	return t == PrimitiveType::U8 || t == PrimitiveType::U16 ||
		t == PrimitiveType::U32 || t == PrimitiveType::U64;
}

CompileResult compile(const dsl::Program &program) {
	CompileResult result;

	for (auto &schema_decl : program.schemas) {
		CompiledSchema cs;
		cs.name = schema_decl.name;

		for (auto &dir : schema_decl.directives) {
			switch (dir.kind) {
			case dsl::Directive::Endian_:
				cs.endian = (std::get<std::string>(dir.value) == "big") ? Endian::Big : Endian::Little;
				break;
			case dsl::Directive::Encoding_:
				cs.encoding = Encoding::UTF8;
				break;
			case dsl::Directive::OnError_:
				if (std::get<std::string>(dir.value) == "drop") cs.onerror = OnError::Drop;
				else if (std::get<std::string>(dir.value) == "pass") cs.onerror = OnError::Pass;
				else cs.onerror = OnError::Error;
				break;
			case dsl::Directive::MaxBytes_:
				cs.maxbytes = std::get<uint64_t>(dir.value);
				break;
			case dsl::Directive::MaxItems_:
				cs.maxitems = std::get<uint64_t>(dir.value);
				break;
			case dsl::Directive::MaxAtoms_:
				cs.maxatoms = std::get<uint64_t>(dir.value);
				break;
			case dsl::Directive::MaxString_:
				cs.maxstring = std::get<uint64_t>(dir.value);
				break;
			}
		}

		std::unordered_map<std::string, bool> unsigned_fields;
		for (auto &field : schema_decl.fields) {
			cs.field_decls.push_back(field);

			switch (field.kind) {
			case dsl::FieldDecl::Scalar: {
				auto pt = token_to_primitive(field.primitive_type);
				Instr instr;
				instr.kind = Instr::ReadScalar;
				instr.primitive = pt;
				instr.field_name = field.name;
				cs.instructions.push_back(instr);

				if (is_unsigned_integer(pt)) {
					unsigned_fields[field.name] = true;
				}

				if (field.has_const) {
					Instr val;
					val.kind = Instr::ValidateConst;
					val.field_name = field.name;
					val.primitive = pt;
					val.const_value = field.const_value;
					cs.instructions.push_back(val);
				}
				break;
			}
			case dsl::FieldDecl::FixedArray: {
				Instr instr;
				instr.kind = Instr::ReadFixedArray;
				instr.primitive = token_to_primitive(field.primitive_type);
				instr.field_name = field.name;
				instr.fixed_count = field.fixed_size;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::VarArray: {
				Instr instr;
				instr.kind = Instr::ReadVarArray;
				instr.primitive = token_to_primitive(field.primitive_type);
				instr.field_name = field.name;
				instr.length_field = field.length_field;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::FixedBytes: {
				Instr instr;
				instr.kind = Instr::ReadFixedBytes;
				instr.field_name = field.name;
				instr.fixed_count = field.fixed_size;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::VarBytes: {
				Instr instr;
				instr.kind = Instr::ReadVarBytes;
				instr.field_name = field.name;
				instr.length_field = field.length_field;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::RemainingBytes: {
				Instr instr;
				instr.kind = Instr::ReadRemainingBytes;
				instr.field_name = field.name;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::NullString: {
				Instr instr;
				instr.kind = Instr::ReadNullString;
				instr.field_name = field.name;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::FixedString: {
				Instr instr;
				instr.kind = Instr::ReadFixedString;
				instr.field_name = field.name;
				instr.fixed_count = field.fixed_size;
				cs.instructions.push_back(instr);
				break;
			}
			case dsl::FieldDecl::Skip_: {
				Instr instr;
				instr.kind = Instr::Skip_;
				instr.skip_bytes = field.skip_bytes;
				cs.instructions.push_back(instr);
				break;
			}
			}
		}

		for (auto &emit_decl : schema_decl.emits) {
			Instr instr;
			instr.kind = Instr::Emit_;
			instr.emit_selector = emit_decl.selector;
			instr.emit_exprs = emit_decl.expressions;
			cs.instructions.push_back(instr);
		}

		result.schemas.push_back(std::move(cs));
	}

	return result;
}

} // namespace bbb
