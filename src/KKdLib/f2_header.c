/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "f2_header.h"

void f2_header_read(stream* s, f2_header* h) {
    memset(h, 0, sizeof(f2_header));

    switch (s->type) {
    case STREAM_FILE:
        if (!s->io.stream)
            return;
        break;
    case STREAM_MEMORY:
        if (!s->io.data.vec.begin)
            return;
        break;
    default:
        return;
    }

    io_read(s, h, 0x20);
    if (h->length == 0x40)
        io_read(s, (uint8_t*)h + 0x20, 0x20);
}

void f2_header_write(stream* s, f2_header* h, bool extended) {
    switch (s->type) {
    case STREAM_FILE:
        if (!s->io.stream)
            return;
        break;
    case STREAM_MEMORY:
        break;
    default:
        return;
    }

    h->length = extended ? 0x40 : 0x20;
    io_write(s, h, h->length);
}

void f2_header_write_end_of_container(stream* s, uint32_t depth) {
    switch (s->type) {
    case STREAM_FILE:
        if (!s->io.stream)
            return;
        break;
    case STREAM_MEMORY:
        break;
    default:
        return;
    }

    f2_header h;
    memset(&h, 0, sizeof(f2_header));
    h.signature = 0x43464F45;
    h.length = 0x20;
    h.depth = depth;
    h.use_section_size = true;
    f2_header_write(s, &h, false);
}
