#include "bbb/send_message.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

namespace {

int failures{0};

void expect(bool condition, const char *message) {
	if (!condition) {
		std::cerr << "FAIL: " << message << '\n';
		++failures;
	}
}

// Exercise the production multipart helper with real libzmq sockets and the
// external's shutdown -> join -> close policy, without requiring a Max host.
void test_blocked_sender(bool saturated, bool multipart) {
	zmq::context_t context{1};
	zmq::socket_t sender{context, zmq::socket_type::push};
	zmq::socket_t receiver{context, zmq::socket_type::pull};
	sender.set(zmq::sockopt::linger, 0);
	receiver.set(zmq::sockopt::linger, 0);
	sender.set(zmq::sockopt::sndhwm, 1);
	receiver.set(zmq::sockopt::rcvhwm, 1);
	sender.bind("inproc://shutdown-test");
	if (saturated) receiver.connect("inproc://shutdown-test");

	std::atomic<bool> running{true};
	std::atomic<size_t> sent{0};
	std::promise<void> entered;
	std::promise<int> completed;
	auto result = completed.get_future();
	std::thread worker{[&] {
		entered.set_value();
		try {
			const std::vector<bbb::Frame> frames = multipart
				? std::vector<bbb::Frame>{{0x01}, {}, {0x02, 0x03}}
				: std::vector<bbb::Frame>{{0x01}};
			while (running && bbb::send_message(sender, frames, running)) ++sent;
			completed.set_value(0);
		} catch (const zmq::error_t &error) {
			completed.set_value(error.num());
		}
	}};
	entered.get_future().wait();
	expect(result.wait_for(std::chrono::milliseconds{100}) == std::future_status::timeout,
		"sender blocks without consumers or at HWM before shutdown");
	if (saturated) expect(0 < sent, "HWM test fills the receiver pipe before blocking");
	else expect(sent == 0, "unconnected PUSH cannot deliver a message");

	// Merely clearing the flag (the old stop policy) cannot interrupt send.
	running = false;
	expect(result.wait_for(std::chrono::milliseconds{50}) == std::future_status::timeout,
		"running=false alone leaves the blocking send stuck");
	const auto started = std::chrono::steady_clock::now();
	context.shutdown();
	expect(result.wait_for(std::chrono::seconds{2}) == std::future_status::ready,
		"context shutdown interrupts blocking send");
	worker.join();
	expect(result.get() == ETERM, "interrupted send reports ETERM");
	sender.close();
	receiver.close();
	context.close();
	expect(std::chrono::steady_clock::now() - started < std::chrono::seconds{2},
		"zero linger permits complete teardown without draining queued messages");
}

void test_multipart_and_restart() {
	// Fresh contexts model repeated start/stop cycles, including a zero-byte part.
	for (int cycle = 0; cycle < 5; ++cycle) {
		zmq::context_t context{1};
		zmq::socket_t sender{context, zmq::socket_type::push};
		zmq::socket_t receiver{context, zmq::socket_type::pull};
		sender.set(zmq::sockopt::linger, 0);
		receiver.set(zmq::sockopt::linger, 0);
		receiver.set(zmq::sockopt::rcvtimeo, 1000);
		sender.bind("inproc://multipart-test");
		receiver.connect("inproc://multipart-test");
		std::atomic<bool> running{true};
		const std::vector<bbb::Frame> frames{{0x01}, {}, {0x02, 0x03}};
		expect(bbb::send_message(sender, frames, running), "normal multipart send succeeds");
		for (size_t index = 0; index < frames.size(); ++index) {
			zmq::message_t part;
			expect(receiver.recv(part).has_value(), "receiver obtains each multipart frame");
			const auto *bytes = static_cast<const uint8_t *>(part.data());
			expect(bbb::Frame(bytes, bytes + part.size()) == frames[index], "multipart bytes are unchanged");
			expect(receiver.get(zmq::sockopt::rcvmore) == (index + 1 < frames.size()),
				"multipart boundaries are unchanged");
		}
		running = false;
		expect(!bbb::send_message(sender, frames, running), "stopped sender does not start another message");
		context.shutdown();
	}
}

void test_undelivered_close() {
	zmq::context_t context{1};
	zmq::socket_t sender{context, zmq::socket_type::push};
	sender.set(zmq::sockopt::linger, 0);
	sender.set(zmq::sockopt::sndtimeo, 1000);
	// inproc permits enqueueing before a peer binds. No peer ever binds here.
	sender.connect("inproc://never-bound");
	std::atomic<bool> running{true};
	expect(bbb::send_message(sender, {{0x01}}, running), "queue a message for a future peer");
	const auto started = std::chrono::steady_clock::now();
	context.shutdown();
	sender.close();
	context.close();
	expect(std::chrono::steady_clock::now() - started < std::chrono::seconds{2},
		"zero linger discards undelivered data instead of waiting for a future peer");
}

} // namespace

int main() {
	test_blocked_sender(false, false);
	test_blocked_sender(false, true);
	test_blocked_sender(true, false);
	test_blocked_sender(true, true);
	test_multipart_and_restart();
	test_undelivered_close();
	if (failures != 0) return 1;
	std::cout << "bbb.zmq send shutdown tests passed\n";
	return 0;
}
