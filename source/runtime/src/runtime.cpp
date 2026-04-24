#include "bbb/runtime.hpp"

namespace bbb {

Runtime &Runtime::instance() {
	static Runtime rt;
	return rt;
}

Runtime::Runtime()
	: packet_store(PacketStore::instance())
	, schema_registry(SchemaRegistry::instance()) {}

} // namespace bbb
