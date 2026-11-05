#pragma once

#include "types.hpp"
#include <atomic>
#include <zmq.hpp>

namespace bbb {

// A false result or exception abandons this socket's multipart stream. The
// caller must stop using it rather than append the next message to a prefix.
inline bool send_message(zmq::socket_t &socket, const std::vector<Frame> &frames,
	const std::atomic<bool> &running) {
	for (size_t index = 0; index < frames.size(); ++index) {
		if (!running) return false;
		const auto &frame = frames[index];
		const auto flags = index + 1 < frames.size()
			? zmq::send_flags::sndmore : zmq::send_flags::none;
		if (!socket.send(zmq::buffer(frame.data(), frame.size()), flags)) return false;
	}
	return true;
}

} // namespace bbb
