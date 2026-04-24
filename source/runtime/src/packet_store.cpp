#include "bbb/packet_store.hpp"

namespace bbb {

const Frame &PacketView::frame(size_t relative_index) const {
	size_t abs = start_frame + relative_index;
	return packet->frames.at(abs);
}

size_t PacketView::frame_count() const {
	return packet->frames.size() - start_frame;
}

PacketStore &PacketStore::instance() {
	static PacketStore store;
	return store;
}

PacketId PacketStore::store(Packet packet) {
	std::lock_guard<std::mutex> lock(mutex_);
	auto id = next_packet_id_.fetch_add(1);
	packet.id = id;
	auto ptr = std::make_shared<Packet>(std::move(packet));
	packets_[id] = ptr;

	auto vid = next_view_id_.fetch_add(1);
	auto view = std::make_shared<PacketView>();
	view->id = vid;
	view->packet = ptr;
	view->start_frame = 0;
	views_[vid] = view;
	return id;
}

std::shared_ptr<const PacketView> PacketStore::get_view(ViewId id) const {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = views_.find(id);
	if (it != views_.end()) {
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<const Packet> PacketStore::get_packet(PacketId id) const {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = packets_.find(id);
	if (it != packets_.end()) {
		return it->second;
	}
	return nullptr;
}

ViewId PacketStore::create_view(PacketId packet_id, size_t start_frame) {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = packets_.find(packet_id);
	if (it == packets_.end()) {
		return 0;
	}
	auto vid = next_view_id_.fetch_add(1);
	auto view = std::make_shared<PacketView>();
	view->id = vid;
	view->packet = it->second;
	view->start_frame = start_frame;
	views_[vid] = view;
	return vid;
}

void PacketStore::release_packet(PacketId id) {
	std::lock_guard<std::mutex> lock(mutex_);
	packets_.erase(id);
}

void PacketStore::cleanup(double max_age_seconds) {
	std::lock_guard<std::mutex> lock(mutex_);
	double cutoff = now_epoch_seconds() - max_age_seconds;
	for (auto it = packets_.begin(); it != packets_.end();) {
		if (it->second->received_time < cutoff) {
			it = packets_.erase(it);
		} else {
			++it;
		}
	}
}

} // namespace bbb
