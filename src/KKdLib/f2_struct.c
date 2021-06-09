/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "f2_struct.h"
#include "io_path.h"
#include "io_stream.h"

vector_func(f2_struct)

static void f2_struct_read_data(stream* s, f2_struct* st, f2_header* h);
static void f2_struct_write_inner(stream* s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x);
static void f2_struct_get_length(f2_struct* s, bool shift_x);
static void f2_struct_write_pof(stream* s, vector_size_t* pof, uint32_t depth, bool shift_x);
static void f2_struct_write_enrs(stream* s, vector_enrs_entry* enrs, uint32_t depth);

void f2_struct_free(f2_struct* st) {
    if (st->data)
        free(st->data);
    if (st->pof.begin)
        vector_size_t_free(&st->pof);
    if (st->enrs.begin)
        enrs_dispose(&st->enrs);
    if (st->sub_structs.begin) {
        for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++)
            f2_struct_free(i);
        vector_f2_struct_free(&st->sub_structs);
    }
}

void f2_struct_read(f2_struct* st, char* path) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    f2_struct_wread(st, path_buf);
    free(path_buf);
}

void f2_struct_wread(f2_struct* st, wchar_t* path) {
    memset(st, 0, sizeof(f2_struct));

    stream* s = io_wopen(path, L"rb");
    if (s->io.stream) {
        f2_header h;
        f2_header_read(s, &h);
        f2_struct_read_data(s, st, &h);
    }
    io_dispose(s);
}

void f2_struct_read_memory(f2_struct* st, void* data, size_t length) {
    memset(st, 0, sizeof(f2_struct));

    stream* s = io_open_memory(data, length);
    if (s->io.stream) {
        f2_header h;
        f2_header_read(s, &h);
        f2_struct_read_data(s, st, &h);
    }
    io_dispose(s);
}

void f2_struct_write(f2_struct* st, char* path, bool use_depth, bool shift_x) {
    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    f2_struct_wwrite(st, path_buf, use_depth, shift_x);
    free(path_buf);
}

void f2_struct_wwrite(f2_struct* st, wchar_t* path, bool use_depth, bool shift_x) {
    stream* s = io_wopen(path, L"wb");
    if (s->io.stream) {
        f2_struct_get_length(st, shift_x);
        f2_struct_write_inner(s, st, 0, use_depth, shift_x);
    }
    io_dispose(s);
}

void f2_struct_write_memory(f2_struct* st, void** data, size_t* length, bool use_depth, bool shift_x) {
    if (!st || !data || !length)
        return;

    f2_struct_get_length(st, shift_x);
    stream* s = io_open_memory(0, st->header.data_size + 0x40ULL);
    f2_struct_write_inner(s, st, 0, use_depth, shift_x);
    *length = s->io.data.data - s->io.data.vec.begin;
    *data = force_malloc(*length);
    memcpy(*data, s->io.data.vec.begin, *length);
    io_dispose(s);
}

static void f2_struct_get_length(f2_struct* s, bool shift_x) {
    bool has_enrs, has_pof, has_sub_structs;
    f2_struct* i;
    uint32_t l;
    vector_f2_struct ls;

    has_pof = s->pof.end - s->pof.begin > 0 ? true : false;
    has_enrs = s->enrs.end - s->enrs.begin > 0 ? true : false;
    has_sub_structs = s->sub_structs.end - s->sub_structs.begin > 0 ? true : false;

    s->header.section_size = s->data ? (uint32_t)s->length : 0;

    l = s->header.section_size;
    if (has_enrs) {
        uint32_t len = enrs_length(&s->enrs);
        l += 0x20 + align_val(len, 0x10);
    }

    if (has_pof) {
        uint32_t len = pof_length(&s->pof, shift_x);
        l += 0x20 + align_val(len, 0x10);
    }

    if (has_sub_structs) {
        ls = s->sub_structs;
        for (i = ls.begin; i != ls.end; i++) {
            f2_struct_get_length(i, shift_x);
            l += i->header.data_size;
            l += i->header.length;
        }
        s->sub_structs = ls;
    }

    if (has_pof || has_sub_structs)
        l += 0x20;

    s->header.data_size = l;
}

static void f2_struct_read_data(stream* s, f2_struct* st, f2_header* h) {
    uint32_t l = h->use_section_size ? h->section_size : h->data_size;
    uint32_t depth = h->depth;
    memset(st, 0, sizeof(f2_struct));
    st->header = *h;
    if (l) {
        st->data = force_malloc(l);
        io_read(s, st->data, l);
        st->length = l;
    }

    uint32_t lastSig = 0, sig;
    size_t length = (size_t)h->data_size - l;
    size_t position = 0;
    while (length > position) {
        f2_header_read(s, h);
        sig = h->signature;
        l = h->use_section_size ? h->section_size : h->data_size;
        position += (size_t)h->length + l;
        if (sig == 0x43464F45)
            break;
        else if (sig == 0x53524E45 || (sig & 0xF0FFFFFF) == 0x30464F50) {
            size_t pos = io_get_position(s);
            if (sig == 0x53524E45)
                enrs_read(s, &st->enrs);
            else
                pof_read(s, &st->pof, sig == 0x31464F50);
            io_set_position(s, pos + l, IO_SEEK_SET);
        }
        else {
            f2_struct str;
            f2_struct_read_data(s, &str, h);
            vector_f2_struct_push_back(&st->sub_structs, &str);
        }
        lastSig = sig;
    }

    if (st->sub_structs.end - st->sub_structs.begin < 1)
        vector_f2_struct_free(&st->sub_structs);
}

static void f2_struct_write_inner(stream* s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x) {
    st->header.depth = use_depth ? depth : 0;
    f2_header_write(s, &st->header, st->header.length == 0x40);
    if (st->data)
        io_write(s, st->data, st->length);
    if (st->pof.begin)
        f2_struct_write_pof(s, &st->pof, use_depth ? depth + 1 : 0, shift_x);
    if (st->enrs.begin)
        f2_struct_write_enrs(s, &st->enrs, 0);
    if (st->sub_structs.begin)
        for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++)
            f2_struct_write_inner(s, i, depth + 1, use_depth, shift_x);
    if (st->pof.begin || st->sub_structs.begin)
        f2_header_write_end_of_container(s, use_depth ? depth + 1 : 0);
    if (!depth)
        f2_header_write_end_of_container(s, 0);
}

static void f2_struct_write_pof(stream* s, vector_size_t* pof, uint32_t depth, bool shift_x) {
    size_t len = pof_length(pof, shift_x);
    f2_header h;
    memset(&h, 0, sizeof(f2_header));
    h.signature = shift_x ? 0x31464F50 : 0x30464F50;
    h.length = 0x20;
    h.depth = depth;
    h.use_section_size = true;
    h.data_size = h.section_size = (uint32_t)align_val(len, 0x10);
    f2_header_write(s, &h, false);
    pof_write(s, pof, shift_x);
}

static void f2_struct_write_enrs(stream* s, vector_enrs_entry* enrs, uint32_t depth) {
    size_t len = enrs_length(enrs);
    f2_header h;
    memset(&h, 0, sizeof(f2_header));
    h.signature = 0x53524E45;
    h.length = 0x20;
    h.depth = depth;
    h.use_section_size = true;
    h.data_size = h.section_size = (uint32_t)align_val(len, 0x10);
    f2_header_write(s, &h, false);
    enrs_write(s, enrs);
}
