#include <c74_min.h>
#include "bbb/runtime.hpp"
#include <sstream>

using namespace c74::min;

class bbb_zmq_peek : public object<bbb_zmq_peek> {
public:
	MIN_DESCRIPTION {"Debug: inspect packet frames and fields"};
	MIN_TAGS {"zmq"};
	MIN_AUTHOR {"bbb"};
	MIN_RELATED {"bbb.zmq.recv, bbb.zmq.parse"};

	inlet<> input {this, "packet view handle"};
	outlet<> output {this, "debug info"};

	attribute<int> verbose {this, "verbose", 1,
		description {"Verbosity level (0-2)"}
	};

	message<> anything {this, "anything", "Handle packet input", [this](const atoms &args) -> atoms {
		if (args.size() < 2) return {};
		if (args[0] != "packet") return {};

		auto view_id = (bbb::ViewId)(int)args[1];
		auto view = bbb::Runtime::instance().packet_store.get_view(view_id);
		if (!view) {
			output.send("peek", "view_not_found", (int)view_id);
			return {};
		}

		output.send("peek", "view_id", (int)view_id, "packet_id", (int)view->packet->id, "start_frame", (int)view->start_frame, "frames", (int)view->frame_count());

		if (verbose.get() >= 1) {
			for (size_t i = 0; i < view->frame_count(); ++i) {
				auto &frame = view->frame(i);
				std::string preview;
				bool is_text = !frame.empty();
				for (auto b : frame) {
					if (b > 0x7E || (b < 0x20 && b != 0x09 && b != 0x0A && b != 0x0D)) {
						is_text = false;
						break;
					}
				}
				if (is_text) {
					preview = std::string(frame.begin(), frame.end());
				} else {
					std::ostringstream ss;
					ss << "<binary " << frame.size() << " bytes>";
					preview = ss.str();
				}
				output.send("frame", (int)i, "size", (int)frame.size(), preview);
			}
		}

		if (verbose.get() >= 2) {
			for (size_t i = 0; i < view->frame_count(); ++i) {
				auto &frame = view->frame(i);
				atoms hex_atoms;
				hex_atoms.push_back("hex");
				hex_atoms.push_back((int)i);
				size_t max_bytes = std::min(frame.size(), (size_t)64);
				for (size_t j = 0; j < max_bytes; ++j) {
					hex_atoms.push_back((int)frame[j]);
				}
				if (frame.size() > 64) {
					hex_atoms.push_back("...");
				}
				output.send(hex_atoms);
			}
		}

		return {};
	}};
};

MIN_EXTERNAL(bbb_zmq_peek);
