/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "video_packet.hpp"

video_packet_handler::video_packet_handler() : num_packet(), packets() {
    alloc.size = 0x1000000;
}

video_packet_handler::~video_packet_handler() {

}

void video_packet_handler::allocate(ssize_t num_packet) {
    this->num_packet = max_def(num_packet, 0);
    packets = alloc.allocate<video_packet>(this->num_packet);
}

void video_packet_handler::reset() {
    num_packet = 0;
    packets = 0;
    alloc.reset();
}

void video_packet_handler::set_data(size_t packet_index, const void* data, size_t size) {
    video_packet& packet = packets[packet_index];
    if (!packet.data || packet.data && packet.size < size)
        packet.data = alloc.allocate(size);
    packet.size = size;
    memcpy(packet.data, data, size);
}

void video_packet_handler::write(stream* s) {
    for (ssize_t i = 0; i < num_packet; i++) {
        video_packet& packet = packets[i];
        if (!packet.data || !packet.size)
            continue;

        s->write(packet.data, packet.size);
    }

    reset();
}
