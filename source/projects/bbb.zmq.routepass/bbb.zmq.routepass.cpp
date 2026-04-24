#include <c74_min.h>
#include "bbb/runtime.hpp"
#include <string>

using namespace c74::min;

class bbb_zmq_routepass : public object<bbb_zmq_routepass> {
public:
	MIN_DESCRIPTION {"Route by current first frame without consuming it"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.route, bbb.zmq.recv"};

	inlet<> input {this, "packet view handle"};

	std::vector<std::unique_ptr<outlet<>>> outlets;

	bbb_zmq_routepass(const atoms &args = {});
	~bbb_zmq_routepass() = default;

	message<> anything {this, "anything", "Handle packet input", [this](const atoms &args) -> atoms {
		if (args.size() < 2) return {};
		if (args[0] != "packet") return {};

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
				atoms a;
				a.push_back("packet");
				a.push_back((int)view_id);
				outlets[i]->send(a);
				return {};
			}
		}

		send_unmatched(view_id);
		return {};
	}};

private:
	std::vector<std::string> route_keys_;

	void send_unmatched(bbb::ViewId view_id) {
		if (outlets.empty()) return;
		auto &unmatched = outlets.back();
		atoms a;
		a.push_back("packet");
		a.push_back((int)view_id);
		unmatched->send(a);
	}
};

bbb_zmq_routepass::bbb_zmq_routepass(const atoms &args) {
	for (auto &arg : args) {
		route_keys_.push_back((std::string)arg);
	}

	for (size_t i = 0; i < route_keys_.size(); ++i) {
		outlets.push_back(std::make_unique<outlet<>>(this, "matched " + route_keys_[i]));
	}
	outlets.push_back(std::make_unique<outlet<>>(this, "unmatched"));
}

MIN_EXTERNAL(bbb_zmq_routepass);
