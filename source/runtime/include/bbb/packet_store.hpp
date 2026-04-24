#pragma once

#include "types.hpp"
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace bbb {

struct Packet {
	PacketId id;
	std::vector<Frame> frames;
	std::string endpoint;
	double received_time;
};

struct PacketView {
	ViewId id;
	std::shared_ptr<const Packet> packet;
	size_t start_frame;

	const Frame &frame(size_t relative_index) const;
	size_t frame_count() const;
};

class PacketStore {
public:
	static PacketStore &instance();

	PacketId store(Packet packet);
	std::shared_ptr<const PacketView> get_view(ViewId id) const;
	std::shared_ptr<const Packet> get_packet(PacketId id) const;
	ViewId create_view(PacketId packet_id, size_t start_frame);
	void release_packet(PacketId id);
	void cleanup(double max_age_seconds);

private:
	PacketStore() = default;
	std::unordered_map<PacketId, std::shared_ptr<Packet>> packets_;
	std::unordered_map<ViewId, std::shared_ptr<PacketView>> views_;
	std::atomic<PacketId> next_packet_id_{1};
	std::atomic<ViewId> next_view_id_{1};
	mutable std::mutex mutex_;
};

} // namespace bbb
