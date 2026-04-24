#include "bbb/schema_registry.hpp"

namespace bbb {

SchemaRegistry &SchemaRegistry::instance() {
	static SchemaRegistry reg;
	return reg;
}

void SchemaRegistry::register_schema(const std::string &name, std::shared_ptr<CompiledSchema> schema) {
	std::lock_guard<std::mutex> lock(mutex_);
	SchemaEntry entry;
	entry.version = global_version_++;
	entry.schema = std::move(schema);
	entries_[name] = std::move(entry);
}

std::shared_ptr<CompiledSchema> SchemaRegistry::find(const std::string &name) const {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = entries_.find(name);
	if (it != entries_.end()) {
		return it->second.schema;
	}
	return nullptr;
}

void SchemaRegistry::unregister(const std::string &name) {
	std::lock_guard<std::mutex> lock(mutex_);
	entries_.erase(name);
}

void SchemaRegistry::clear() {
	std::lock_guard<std::mutex> lock(mutex_);
	entries_.clear();
}

std::vector<std::string> SchemaRegistry::names() const {
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<std::string> result;
	for (auto &pair : entries_) {
		result.push_back(pair.first);
	}
	return result;
}

} // namespace bbb
