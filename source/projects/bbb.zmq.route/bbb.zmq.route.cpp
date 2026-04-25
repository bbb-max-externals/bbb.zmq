#include <c74_min.h>
#include "bbb/runtime.hpp"
#include <string>

using namespace c74::min;

class bbb_zmq_route : public object<bbb_zmq_route> {
public:
	MIN_DESCRIPTION {"Route by current first frame and consume it on match"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.routepass, bbb.zmq.recv"};

	inlet<> input {this, "packet view handle"};

	bbb_zmq_route(const atoms &args = {});
	~bbb_zmq_route() = default;

	message<> anything {this, "anything", MIN_FUNCTION {
		if (args.size() < 2) return {};
		if (!(args[0] == "packet")) return {};

		auto view_id = (bbb::ViewId)(int)args[1];
		auto view = bbb::Runtime::instance().packet_store.get_view(view_id);
		if (!view) return {};

		if (view->frame_count() == 0) {
			send_unmatched(view_id);
			return {};
		}

		auto &first_frame = view->frame(0);
		std::string key(first_frame.begin(), first_frame.end());

		bool is_text = !first_frame.empty();
		for (auto b : first_frame) {
			if (b == 0x00) { is_text = false; break; }
			if (b > 0x7E) { is_text = false; break; }
			if (b < 0x20 && b != 0x09 && b != 0x0A && b != 0x0D) { is_text = false; break; }
		}

		if (!is_text) {
			send_unmatched(view_id);
			return {};
		}

		for (size_t i = 0; i < route_keys_.size(); ++i) {
			if (key == route_keys_[i]) {
				auto new_view_id = bbb::Runtime::instance().packet_store.create_view(view->packet->id, view->start_frame + 1);
				atoms a;
				a.push_back("packet");
				a.push_back((int)new_view_id);
				send_to_outlet(outlets_[i], a);
				return {};
			}
		}

		send_unmatched(view_id);
		return {};
	}};

private:
	std::vector<std::string> route_keys_;
	std::vector<void*> outlets_;
	void* unmatched_outlet_ = nullptr;

	void send_to_outlet(void* out, const atoms& a) {
		if (!out || a.empty()) return;
		outlet_do_send(out, a);
	}

	void send_unmatched(bbb::ViewId view_id) {
		if (!unmatched_outlet_) return;
		atoms a;
		a.push_back("packet");
		a.push_back((int)view_id);
		send_to_outlet(unmatched_outlet_, a);
	}
};

bbb_zmq_route::bbb_zmq_route(const atoms &args) {
	for (auto &arg : args) {
		route_keys_.push_back((std::string)arg);
	}

	for (size_t i = 0; i < route_keys_.size(); ++i) {
		outlets_.push_back(c74::max::outlet_new(c74::min::object_base::maxobj(), nullptr));
	}
	unmatched_outlet_ = c74::max::outlet_new(c74::min::object_base::maxobj(), nullptr);
}

MIN_EXTERNAL(bbb_zmq_route);
