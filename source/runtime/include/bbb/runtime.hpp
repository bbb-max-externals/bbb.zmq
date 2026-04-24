#pragma once

#include "packet_store.hpp"
#include "schema_registry.hpp"

namespace bbb {

struct Runtime {
	static Runtime &instance();

	PacketStore &packet_store;
	SchemaRegistry &schema_registry;

private:
	Runtime();
};

} // namespace bbb
