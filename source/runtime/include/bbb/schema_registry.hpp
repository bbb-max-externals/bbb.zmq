#pragma once

#include "schema.hpp"
#include <unordered_map>
#include <mutex>
#include <cstdint>

namespace bbb {

struct SchemaEntry {
	uint64_t version;
	std::shared_ptr<CompiledSchema> schema;
};

class SchemaRegistry {
public:
	static SchemaRegistry &instance();

	void register_schema(const std::string &name, std::shared_ptr<CompiledSchema> schema);
	std::shared_ptr<CompiledSchema> find(const std::string &name) const;
	void unregister(const std::string &name);
	void clear();
	std::vector<std::string> names() const;

private:
	SchemaRegistry() = default;
	std::unordered_map<std::string, SchemaEntry> entries_;
	uint64_t global_version_{1};
	mutable std::mutex mutex_;
};

} // namespace bbb
