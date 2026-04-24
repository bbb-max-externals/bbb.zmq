#include <c74_min.h>
#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include "bbb/runtime.hpp"

using namespace c74::min;

class bbb_zmq_recv : public object<bbb_zmq_recv> {
public:
	MIN_DESCRIPTION {"Receive ZMQ multipart messages and store frames internally"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.route, bbb.zmq.parse"};

	inlet<> input {this, "(messages)"};
	outlet<> output {this, "packet view handle"};

	attribute<std::string> endpoint {this, "endpoint", "tcp://*:5555",
		description {"ZMQ endpoint to bind/connect to"}
	};

	attribute<std::string> socket_type {this, "type", "sub",
		description {"Socket type: sub, pull, rep, router"}
	};

	attribute<int> bind_mode {this, "bind", 1,
		description {"1 = bind, 0 = connect"}
	};

	attribute<std::string> subscribe {this, "subscribe", "",
		description {"Subscription topic (sub sockets only)"}
	};

	attribute<int> hwm {this, "hwm", 1000,
		description {"High water mark"}
	};

	bbb_zmq_recv(const atoms &args = {});
	~bbb_zmq_recv();

	message<> start_msg {this, "start", "Start receiving", [this](const atoms &args) -> atoms {
		start();
		return {};
	}};

	message<> stop_msg {this, "stop", "Stop receiving", [this](const atoms &args) -> atoms {
		stop();
		return {};
	}};

	message<> bang_msg {this, "bang", "Start receiving", [this](const atoms &args) -> atoms {
		start();
		return {};
	}};

	message<> anything {this, "anything", "Handle messages", [this](const atoms &args) -> atoms {
		return {};
	}};

private:
	void start();
	void stop();
	void recv_loop();

	std::unique_ptr<zmq::context_t> context_;
	std::unique_ptr<zmq::socket_t> socket_;
	std::unique_ptr<std::thread> thread_;
	std::atomic<bool> running_{false};
	std::mutex socket_mutex_;

	struct PendingPacket {
		bbb::PacketId packet_id;
	};
	std::queue<PendingPacket> pending_;
	std::mutex pending_mutex_;

	timer<timer_options::deliver_on_scheduler> deliver_ {this, [this]() {
		std::queue<PendingPacket> to_deliver;
		{
			std::lock_guard<std::mutex> lock(pending_mutex_);
			to_deliver = std::move(pending_);
		}
		while (!to_deliver.empty()) {
			auto &pp = to_deliver.front();
			atoms a;
			a.push_back("packet");
			a.push_back((int)pp.packet_id);
			output.send(a);
			to_deliver.pop();
		}
	}};
};

bbb_zmq_recv::bbb_zmq_recv(const atoms &args) {
	argument<std::string> endpoint_arg {this, "endpoint", "ZMQ endpoint"};
}

bbb_zmq_recv::~bbb_zmq_recv() {
	stop();
}

void bbb_zmq_recv::start() {
	if (running_) return;

	try {
		context_ = std::make_unique<zmq::context_t>(1);
		zmq::socket_type ztype = zmq::socket_type::sub;

		auto stype = socket_type.get();
		if (stype == "pull") ztype = zmq::socket_type::pull;
		else if (stype == "rep") ztype = zmq::socket_type::rep;
		else if (stype == "router") ztype = zmq::socket_type::router;
		else if (stype == "pair") ztype = zmq::socket_type::pair;

		socket_ = std::make_unique<zmq::socket_t>(*context_, ztype);
		socket_->set(zmq::sockopt::rcvhwm, hwm.get());

		auto ep = endpoint.get();
		if (bind_mode.get()) {
			socket_->bind(ep);
		} else {
			socket_->connect(ep);
		}

		if (ztype == zmq::socket_type::sub) {
			auto sub = subscribe.get();
			if (sub.empty()) {
				socket_->set(zmq::sockopt::subscribe, "");
			} else {
				socket_->set(zmq::sockopt::subscribe, sub);
			}
		}

		running_ = true;
		thread_ = std::make_unique<std::thread>(&bbb_zmq_recv::recv_loop, this);
	} catch (const zmq::error_t &e) {
		cerr << "bbb.zmq.recv: " << e.what() << std::endl;
	}
}

void bbb_zmq_recv::stop() {
	running_ = false;
	if (context_) {
		context_.reset();
	}
	if (thread_ && thread_->joinable()) {
		thread_->join();
	}
	thread_.reset();
	socket_.reset();
}

void bbb_zmq_recv::recv_loop() {
	while (running_) {
		try {
			zmq::message_t msg;
			if (!socket_->recv(msg, zmq::recv_flags::dontwait)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			bbb::Packet packet;
			packet.endpoint = endpoint.get();
			packet.received_time = bbb::now_epoch_seconds();

			{
				bbb::Frame frame;
				frame.assign(static_cast<const uint8_t *>(msg.data()), static_cast<const uint8_t *>(msg.data()) + msg.size());
				packet.frames.push_back(std::move(frame));
			}

			int64_t more = socket_->get(zmq::sockopt::rcvmore);
			while (more) {
				zmq::message_t part;
				if (!socket_->recv(part, zmq::recv_flags::none)) break;
				bbb::Frame frame;
				frame.assign(static_cast<const uint8_t *>(part.data()), static_cast<const uint8_t *>(part.data()) + part.size());
				packet.frames.push_back(std::move(frame));
				more = socket_->get(zmq::sockopt::rcvmore);
			}

			auto id = bbb::Runtime::instance().packet_store.store(std::move(packet));

			{
				std::lock_guard<std::mutex> lock(pending_mutex_);
				pending_.push({id});
			}
			deliver_.delay(0);
		} catch (const zmq::error_t &e) {
			if (running_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}
}

MIN_EXTERNAL(bbb_zmq_recv);
