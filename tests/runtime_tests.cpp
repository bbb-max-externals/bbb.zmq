#include "bbb/dsl_lexer.hpp"
#include "bbb/dsl_parser.hpp"
#include "bbb/packet_store.hpp"
#include "bbb/parser_vm.hpp"
#include "bbb/runtime.hpp"
#include "bbb/schema.hpp"
#include "bbb/schema_registry.hpp"

#include <cstdint>
#include <cmath>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace {

int failure_count = 0;

void expect(bool condition, const std::string &message) {
	if(!condition) {
		std::cerr << "FAIL: " << message << '\n';
		++failure_count;
	}
}

template <typename value_type>
void expect_equal(const value_type &actual, const value_type &expected, const std::string &message) {
	if(!(actual == expected)) {
		std::cerr << "FAIL: " << message << '\n';
		++failure_count;
	}
}

void expect_near(double actual, double expected, double tolerance, const std::string &message) {
	if(std::fabs(actual - expected) > tolerance) {
		std::cerr << "FAIL: " << message << " expected=" << expected << " actual=" << actual << '\n';
		++failure_count;
	}
}

bbb::CompiledSchema compile_single_schema(const std::string &source) {
	auto lex_result = bbb::dsl::lex(source);
	expect(lex_result.ok(), "lexer accepts schema source: " + lex_result.error);

	auto parse_result = bbb::dsl::parse(lex_result.tokens);
	expect(parse_result.ok(), "parser accepts schema source");

	auto compile_result = bbb::compile(parse_result.program);
	expect(compile_result.ok(), "compiler accepts parsed schema");
	expect_equal(compile_result.schemas.size(), std::size_t{1}, "one schema is compiled");

	if(compile_result.schemas.empty()) {
		return {};
	}
	return compile_result.schemas.front();
}

template <typename value_type>
const value_type &atom_as(const bbb::ParseOutput &output, std::size_t index) {
	expect(index < output.atoms.size(), "atom index in range");
	if(!(index < output.atoms.size())) {
		static const value_type fallback{};
		return fallback;
	}
	expect(std::holds_alternative<value_type>(output.atoms.at(index)), "atom type matches expectation");
	if(!std::holds_alternative<value_type>(output.atoms.at(index))) {
		static const value_type fallback{};
		return fallback;
	}
	return std::get<value_type>(output.atoms.at(index));
}

void append_float_be(bbb::Frame &frame, float value) {
	std::uint32_t bits{0};
	std::memcpy(&bits, &value, sizeof(bits));
	frame.push_back((std::uint8_t)((bits >> 24) & 0xFF));
	frame.push_back((std::uint8_t)((bits >> 16) & 0xFF));
	frame.push_back((std::uint8_t)((bits >> 8) & 0xFF));
	frame.push_back((std::uint8_t)(bits & 0xFF));
}

void append_float_le(bbb::Frame &frame, float value) {
	std::uint32_t bits{0};
	std::memcpy(&bits, &value, sizeof(bits));
	frame.push_back((std::uint8_t)(bits & 0xFF));
	frame.push_back((std::uint8_t)((bits >> 8) & 0xFF));
	frame.push_back((std::uint8_t)((bits >> 16) & 0xFF));
	frame.push_back((std::uint8_t)((bits >> 24) & 0xFF));
}

void test_big_endian_scalar_emit() {
	const auto schema = compile_single_schema(R"(
schema simple {
	endian big;
	u8 id;
	i32 value;
	f32 temperature;
	emit sample id value temperature;
}
)");

	bbb::Frame frame{
		0x7F,
		0x00, 0x00, 0x00, 0x2A,
	};
	append_float_be(frame, 1.5f);

	const auto result = bbb::parse_frame(schema, frame);
	expect(result.ok(), "big-endian scalar frame parses without diagnostics");
	expect_equal(result.outputs.size(), std::size_t{1}, "one scalar output");
	if(result.outputs.empty()) {
		return;
	}
	const auto &output = result.outputs.front();
	expect_equal(output.selector, std::string{"sample"}, "selector is preserved");
	expect_equal(output.atoms.size(), std::size_t{3}, "scalar emit atom count");
	expect_equal(atom_as<std::int64_t>(output, 0), std::int64_t{127}, "u8 atom");
	expect_equal(atom_as<std::int64_t>(output, 1), std::int64_t{42}, "i32 atom");
	expect_near(atom_as<double>(output, 2), 1.5, 0.000001, "f32 atom");
}

void test_little_endian_arrays_strings_and_directives() {
	const auto schema = compile_single_schema(R"(
schema imu {
	endian little;
	maxbytes 64;
	maxitems 8;
	maxatoms 16;
	maxstring 8;
	u16 magic = 0xCAFE;
	u8 version;
	u8 type;
	u64 timestamp;
	f32 accel[3];
	string[4] label;
	emit imu version type timestamp accel[0] accel[1] accel[2] label;
}
)");

	expect_equal(schema.endian, bbb::Endian::Little, "little-endian directive");
	expect_equal(schema.maxbytes, std::uint64_t{64}, "maxbytes directive");
	expect_equal(schema.maxitems, std::uint64_t{8}, "maxitems directive");
	expect_equal(schema.maxatoms, std::uint64_t{16}, "maxatoms directive");
	expect_equal(schema.maxstring, std::uint64_t{8}, "maxstring directive");

	bbb::Frame frame{
		0xFE, 0xCA,
		0x02,
		0x07,
		0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
	};
	append_float_le(frame, 0.25f);
	append_float_le(frame, -0.5f);
	append_float_le(frame, 1.0f);
	frame.push_back((std::uint8_t)'o');
	frame.push_back((std::uint8_t)'k');
	frame.push_back(0x00);
	frame.push_back((std::uint8_t)'x');

	const auto result = bbb::parse_frame(schema, frame);
	expect(result.ok(), "little-endian structured frame parses without diagnostics");
	expect_equal(result.outputs.size(), std::size_t{1}, "one structured output");
	if(result.outputs.empty()) {
		return;
	}

	const auto &output = result.outputs.front();
	expect_equal(output.selector, std::string{"imu"}, "structured selector");
	expect_equal(output.atoms.size(), std::size_t{7}, "structured atom count");
	expect_equal(atom_as<std::int64_t>(output, 0), std::int64_t{2}, "version atom");
	expect_equal(atom_as<std::int64_t>(output, 1), std::int64_t{7}, "type atom");
	expect_equal(atom_as<std::int64_t>(output, 2), std::int64_t{0x0102030405060708LL}, "timestamp atom");
	expect_near(atom_as<double>(output, 3), 0.25, 0.000001, "accel[0]");
	expect_near(atom_as<double>(output, 4), -0.5, 0.000001, "accel[1]");
	expect_near(atom_as<double>(output, 5), 1.0, 0.000001, "accel[2]");
	expect_equal(atom_as<std::string>(output, 6), std::string{"ok"}, "fixed string stops at NUL");
}

void test_variable_bytes_and_emit_modifiers() {
	const auto schema = compile_single_schema(R"(
schema blob {
	endian little;
	maxatoms 16;
	u8 length;
	bytes[length] payload;
	emit payload_values payload@list;
	emit payload_text payload@string;
}
)");

	const bbb::Frame frame{0x03, (std::uint8_t)'a', (std::uint8_t)'b', (std::uint8_t)'c'};
	const auto result = bbb::parse_frame(schema, frame);
	expect(result.ok(), "variable bytes frame parses");
	expect_equal(result.outputs.size(), std::size_t{2}, "two modifier outputs");
	if(result.outputs.size() < 2) {
		return;
	}

	expect_equal(result.outputs[0].selector, std::string{"payload_values"}, "bytes selector");
	expect_equal(result.outputs[0].atoms.size(), std::size_t{3}, "bytes @list atom count");
	expect_equal(atom_as<std::int64_t>(result.outputs[0], 0), std::int64_t{'a'}, "bytes[0]");
	expect_equal(atom_as<std::int64_t>(result.outputs[0], 1), std::int64_t{'b'}, "bytes[1]");
	expect_equal(atom_as<std::int64_t>(result.outputs[0], 2), std::int64_t{'c'}, "bytes[2]");

	expect_equal(result.outputs[1].selector, std::string{"payload_text"}, "text selector");
	expect_equal(result.outputs[1].atoms.size(), std::size_t{1}, "text @string atom count");
	expect_equal(atom_as<std::string>(result.outputs[1], 0), std::string{"abc"}, "payload @string");
}

void test_parse_errors_are_reported() {
	const auto schema = compile_single_schema(R"(
schema guarded {
	u16 magic = 0xCAFE;
	u8 value;
	emit guarded value;
}
)");

	const bbb::Frame wrong_magic{0x00, 0x00, 0x2A};
	const auto const_result = bbb::parse_frame(schema, wrong_magic);
	expect(!const_result.ok(), "const mismatch is reported");
	expect_equal(const_result.errors.empty(), false, "const mismatch has diagnostic");
	expect_equal(const_result.outputs.empty(), true, "const mismatch suppresses emit in error mode");
	if(!const_result.errors.empty()) {
		expect_equal(const_result.errors.front().code, bbb::ParseDiag::ConstMismatch, "const mismatch code");
	}

	const bbb::Frame short_frame{0xFE};
	const auto bounds_result = bbb::parse_frame(schema, short_frame);
	expect(!bounds_result.ok(), "short frame is reported");
	expect_equal(bounds_result.errors.empty(), false, "short frame has diagnostic");
	if(!bounds_result.errors.empty()) {
		expect_equal(bounds_result.errors.front().code, bbb::ParseDiag::OutOfBounds, "short frame code");
	}
}

void test_packet_store_and_registry() {
	auto &registry = bbb::SchemaRegistry::instance();
	registry.clear();

	auto schema = std::make_shared<bbb::CompiledSchema>();
	schema->name = "registered";
	registry.register_schema(schema->name, schema);
	expect(registry.find("registered") == schema, "registry returns registered schema");
	expect_equal(registry.names().size(), std::size_t{1}, "registry names count");
	registry.unregister("registered");
	expect(!registry.find("registered"), "registry unregister removes schema");

	auto &store = bbb::PacketStore::instance();
	bbb::Packet packet;
	packet.frames = {
		bbb::Frame{0x01, 0x02},
		bbb::Frame{0x03},
		bbb::Frame{0x04, 0x05, 0x06},
	};
	packet.endpoint = "inproc://unit";
	packet.received_time = bbb::now_epoch_seconds();
	const bbb::PacketId packet_id = store.store(std::move(packet));
	expect(packet_id != 0, "packet id is assigned");
	expect(store.get_packet(packet_id) != nullptr, "stored packet can be found");

	const bbb::ViewId view_id = store.create_view(packet_id, 1);
	expect(view_id != 0, "view id is assigned");
	const auto view = store.get_view(view_id);
	expect(view != nullptr, "created view can be found");
	if(view) {
		expect_equal(view->frame_count(), std::size_t{2}, "view frame count starts at offset");
		expect_equal(view->frame(0).size(), std::size_t{1}, "view first frame size");
		expect_equal(view->frame(1).at(2), std::uint8_t{0x06}, "view second frame payload");
	}

	expect_equal(store.create_view(0, 0), bbb::ViewId{0}, "view creation fails for missing packet");
	store.release_packet(packet_id);
	expect(store.get_packet(packet_id) == nullptr, "release removes packet lookup");
}

} // namespace

int main() {
	test_big_endian_scalar_emit();
	test_little_endian_arrays_strings_and_directives();
	test_variable_bytes_and_emit_modifiers();
	test_parse_errors_are_reported();
	test_packet_store_and_registry();

	if(failure_count != 0) {
		std::cerr << failure_count << " test assertion(s) failed\n";
		return 1;
	}

	std::cout << "bbb.zmq runtime tests passed\n";
	return 0;
}
