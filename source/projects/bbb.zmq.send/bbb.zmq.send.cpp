#include <c74_min.h>
#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cstring>
#include "bbb/runtime.hpp"

using namespace c74::min;

class bbb_zmq_send : public object<bbb_zmq_send> {
public:
	MIN_DESCRIPTION {"Send ZMQ multipart messages"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.recv"};

	inlet<> input {this, "(messages)"};

	attribute<symbol> endpoint {this, "endpoint", "tcp://*:5556",
		description {"ZMQ endpoint to bind/connect to"}
	};

	attribute<symbol> socket_type {this, "type", "pub",
		description {"Socket type: pub, push, pair"}
	};

	attribute<int> bind_mode {this, "bind", 1,
		description {"1 = bind, 0 = connect"}
	};

	attribute<int> hwm {this, "hwm", 1000,
		description {"High water mark (SNDHWM)"}
	};

	attribute<symbol> byte_order {this, "endian", "big",
		description {"Byte order for numeric values: big or little"}
	};

	bbb_zmq_send(const atoms &args = {});
	~bbb_zmq_send();

	message<> start_msg {this, "start", MIN_FUNCTION {
		start();
		return {};
	}};

	message<> stop_msg {this, "stop", MIN_FUNCTION {
		stop();
		return {};
	}};

	message<> bang_msg {this, "bang", MIN_FUNCTION {
		start();
		return {};
	}};

	message<> send_msg {this, "send", MIN_FUNCTION {
		OutgoingMessage msg;
		msg.frames.push_back(encode_atoms(args));
		enqueue(std::move(msg));
		return {};
	}};

	message<> frame_msg {this, "frame", MIN_FUNCTION {
		std::lock_guard<std::mutex> lock(frame_buffer_mutex_);
		frame_buffer_.push_back(encode_atoms(args));
		return {};
	}};

	message<> frame_bytes_msg {this, "frame_bytes", MIN_FUNCTION {
		bbb::Frame f;
		f.reserve(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			int v = (int)args[i];
			if (v < 0) v = 0;
			if (v > 255) v = 255;
			f.push_back(static_cast<uint8_t>(v));
		}
		std::lock_guard<std::mutex> lock(frame_buffer_mutex_);
		frame_buffer_.push_back(std::move(f));
		return {};
	}};

	message<> flush_msg {this, "flush", MIN_FUNCTION {
		OutgoingMessage msg;
		{
			std::lock_guard<std::mutex> lock(frame_buffer_mutex_);
			msg.frames = std::move(frame_buffer_);
			frame_buffer_.clear();
		}
		if (!msg.frames.empty()) {
			enqueue(std::move(msg));
		}
		return {};
	}};

	message<> anything {this, "anything", MIN_FUNCTION {
		if (args.size() >= 1 && args[0] == "jit_matrix" && args.size() >= 2) {
			auto s = (std::string)args[1];
			c74::max::t_symbol *mtx_name = c74::max::gensym(s.c_str());
			c74::max::t_jit_object *mtx = (c74::max::t_jit_object *)c74::max::jit_object_findregistered(mtx_name);
			if (mtx) {
				auto frame = extract_matrix_frame(mtx);
				OutgoingMessage msg;
				msg.frames.push_back(std::move(frame));
				enqueue(std::move(msg));
			}
		}
		return {};
	}};

private:
	struct OutgoingMessage {
		std::vector<bbb::Frame> frames;
	};

	void start();
	void stop();
	void send_loop();
	void enqueue(OutgoingMessage &&msg);
	bbb::Frame encode_atoms(const atoms &args);
	bbb::Frame extract_matrix_frame(c74::max::t_jit_object *mtx);

	bool is_big_endian() {
		auto e = std::string(byte_order.get().c_str());
		return e != "little";
	}

	std::unique_ptr<zmq::context_t> context_;
	std::unique_ptr<zmq::socket_t> socket_;
	std::unique_ptr<std::thread> thread_;
	std::atomic<bool> running_{false};

	std::queue<OutgoingMessage> outgoing_;
	std::mutex outgoing_mutex_;
	std::condition_variable outgoing_cv_;

	std::vector<bbb::Frame> frame_buffer_;
	std::mutex frame_buffer_mutex_;
};

bbb_zmq_send::bbb_zmq_send(const atoms &args) {
}

bbb_zmq_send::~bbb_zmq_send() {
	stop();
}

void bbb_zmq_send::enqueue(OutgoingMessage &&msg) {
	{
		std::lock_guard<std::mutex> lock(outgoing_mutex_);
		outgoing_.push(std::move(msg));
	}
	outgoing_cv_.notify_one();
}

void bbb_zmq_send::start() {
	if (running_) return;

	try {
		context_ = std::make_unique<zmq::context_t>(1);
		zmq::socket_type ztype = zmq::socket_type::pub;

		auto stype = std::string(socket_type.get().c_str());
		if (stype == "push") ztype = zmq::socket_type::push;
		else if (stype == "pair") ztype = zmq::socket_type::pair;

		socket_ = std::make_unique<zmq::socket_t>(*context_, ztype);
		socket_->set(zmq::sockopt::sndhwm, hwm.get());

		auto ep = std::string(endpoint.get().c_str());
		if (bind_mode.get()) {
			socket_->bind(ep);
		} else {
			socket_->connect(ep);
		}

		running_ = true;
		thread_ = std::make_unique<std::thread>(&bbb_zmq_send::send_loop, this);
	} catch (const zmq::error_t &e) {
		cerr << ("bbb.zmq.send: " + std::string(e.what())) << endl;
	}
}

void bbb_zmq_send::stop() {
	running_ = false;
	outgoing_cv_.notify_all();
	if (thread_ && thread_->joinable()) {
		thread_->join();
	}
	thread_.reset();
	socket_.reset();
	context_.reset();
}

void bbb_zmq_send::send_loop() {
	while (running_) {
		std::queue<OutgoingMessage> local;
		{
			std::unique_lock<std::mutex> lock(outgoing_mutex_);
			outgoing_cv_.wait(lock, [this] {
				return !outgoing_.empty() || !running_;
			});
			std::swap(local, outgoing_);
		}
		while (!local.empty()) {
			auto &msg = local.front();
			try {
				for (size_t i = 0; i < msg.frames.size(); ++i) {
					auto &frame = msg.frames[i];
					zmq::message_t zmsg(frame.size());
					std::memcpy(zmsg.data(), frame.data(), frame.size());
					auto flags = (i < msg.frames.size() - 1)
						? zmq::send_flags::sndmore
						: zmq::send_flags::none;
					socket_->send(zmsg, flags);
				}
			} catch (const zmq::error_t &) {
			}
			local.pop();
		}
	}
}

bbb::Frame bbb_zmq_send::encode_atoms(const atoms &args) {
	bbb::Frame out;
	bool big = is_big_endian();

	for (size_t i = 0; i < args.size(); ++i) {
		auto &a = args[i];
		if (a.a_type == c74::max::A_LONG) {
			int64_t v = a.a_w.w_long;
			for (int b = 0; b < 8; ++b) {
				int shift = big ? (56 - b * 8) : (b * 8);
				out.push_back(static_cast<uint8_t>((v >> shift) & 0xFF));
			}
		} else if (a.a_type == c74::max::A_FLOAT) {
			double v = a.a_w.w_float;
			uint64_t raw;
			std::memcpy(&raw, &v, 8);
			for (int b = 0; b < 8; ++b) {
				int shift = big ? (56 - b * 8) : (b * 8);
				out.push_back(static_cast<uint8_t>((raw >> shift) & 0xFF));
			}
		} else {
			auto s = std::string(a);
			out.insert(out.end(), s.begin(), s.end());
		}
	}
	return out;
}

bbb::Frame bbb_zmq_send::extract_matrix_frame(c74::max::t_jit_object *mtx) {
	bbb::Frame out;

	c74::max::t_jit_object *info = (c74::max::t_jit_object *)c74::max::jit_object_method(mtx, c74::max::_jit_sym_getinfo);
	if (!info) return out;

	long dim[2] = {0, 0};
	c74::max::jit_object_method(info, c74::max::_jit_sym_getinfo, c74::max::_jit_sym_dim, dim, 0L, 0L);

	long dimstride[2] = {0, 0};
	c74::max::jit_object_method(info, c74::max::_jit_sym_getinfo, c74::max::gensym("dimstride"), dimstride, 0L, 0L);

	long planecount = 1;
	c74::max::jit_object_method(info, c74::max::_jit_sym_getinfo, c74::max::_jit_sym_planecount, &planecount, 0L, 0L);

	c74::max::t_symbol *type_sym = c74::max::_jit_sym_char;
	c74::max::jit_object_method(info, c74::max::_jit_sym_getinfo, c74::max::_jit_sym_type, &type_sym, 0L, 0L);

	size_t cell_sz = 1;
	if (type_sym == c74::max::_jit_sym_long) cell_sz = 4;
	else if (type_sym == c74::max::_jit_sym_float32) cell_sz = 4;
	else if (type_sym == c74::max::_jit_sym_float64) cell_sz = 8;

	char *bp = nullptr;
	c74::max::jit_object_method(mtx, c74::max::_jit_sym_getdata, &bp);
	if (!bp) return out;

	size_t row_bytes = (size_t)(dim[0] * planecount) * cell_sz;
	out.reserve(row_bytes * dim[1]);

	for (long y = 0; y < dim[1]; ++y) {
		char *row = bp + y * dimstride[1];
		out.insert(out.end(), row, row + row_bytes);
	}

	return out;
}

MIN_EXTERNAL(bbb_zmq_send);
