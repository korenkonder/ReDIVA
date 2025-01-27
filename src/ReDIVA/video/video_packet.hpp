/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/io/stream.hpp"
#include "../../KKdLib/prj/stack_allocator.hpp"

struct video_packet {
    void* data;
    size_t size;
};

struct video_packet_handler {
    prj::stack_allocator alloc;
    ssize_t num_packet;
    video_packet* packets;

    video_packet_handler();
    ~video_packet_handler();

    void allocate(ssize_t num_packet);
    void reset();
    void set_data(size_t packet_index, const void* data, size_t size);
    void write(stream* s);
};
