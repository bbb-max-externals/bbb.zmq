#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace bbb {

using PacketId = uint64_t;
using ViewId = uint64_t;
using Frame = std::vector<uint8_t>;

enum class Endian { Little, Big };
enum class Encoding { UTF8 };
enum class OnError { Error, Drop, Pass };

inline double now_epoch_seconds() {
	auto tp = std::chrono::system_clock::now();
	auto dur = tp.time_since_epoch();
	return std::chrono::duration<double>(dur).count();
}

} // namespace bbb
