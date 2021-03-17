/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "f2_header.h"

f2_header* f2_header_init() {
    f2_header* h = force_malloc(sizeof(f2_header));
    return h;
}

void f2_header_dispose(f2_header* h) {
    if (!h)
        return;

    free(h);
}

void f2_header_read(stream* s, f2_header* h) {
    memset(h, 0, sizeof(f2_header));

    if (!s->io)
        return;

    io_read(s, h, 0x20);
    if (h->length == 0x40)
        io_read(s, (uint8_t*)h + 0x20, 0x20);
}

void f2_header_write(stream* s, f2_header* h, bool extended) {
    if (!s->io)
        return;

    h->length = extended ? 0x40 : 0x20;
    io_write(s, h, h->length);
}

void f2_header_write_end_of_container(stream* s, uint32_t depth) {
    if (!s->io)
        return;

    f2_header h;
    memset(&h, 0, sizeof(f2_header));
    h.signature = 0x43464F45;
    h.length = 0x20;
    h.depth = depth;
    h.use_section_size = true;
    f2_header_write(s, &h, false);
}
