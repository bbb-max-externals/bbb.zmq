#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace bbb::parse_bounds {

inline bool contains(std::size_t size, std::size_t offset, std::uint64_t needed) {
	return offset <= size && needed <= size - offset;
}

inline bool multiply(std::size_t bytes, std::int64_t count, std::size_t &result) {
	if (count < 0 || (bytes && static_cast<std::uint64_t>(count) >
		std::numeric_limits<std::size_t>::max() / bytes)) return false;
	result = bytes * static_cast<std::size_t>(count);
	return true;
}

inline bool matrix_bytes(std::size_t cell, std::int64_t planes, std::int64_t width,
	std::int64_t height, std::uint64_t budget, std::size_t &bytes) {
	std::size_t total = cell;
	if (!cell || planes <= 0 || width <= 0 || height <= 0 ||
		!multiply(total, planes, total) || !multiply(total, width, total) ||
		!multiply(total, height, total) || total > budget) return false;
	bytes = total;
	return true;
}

} // namespace bbb::parse_bounds
