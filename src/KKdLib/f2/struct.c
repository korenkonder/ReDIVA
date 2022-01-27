/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../divafile.h"

vector_func(f2_struct)

static void f2_struct_read_data(stream* s, f2_struct* st, f2_header* h);
static void f2_struct_write_inner(stream* s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x);
static void f2_struct_get_length(f2_struct* s, bool shift_x);
static void f2_struct_write_pof(stream* s, vector_size_t* pof, uint32_t depth, bool shift_x);
static void f2_struct_write_enrs(stream* s, vector_enrs_entry* enrs, uint32_t depth);

void f2_struct_read(f2_struct* st, char* path) {
    memset(st, 0, sizeof(f2_struct));

    stream s;
    io_open(&s, path, "rb");
    if (&s.io.stream) {
        divafile_sdecrypt(&s);

        f2_header h;
        f2_header_read(&s, &h);
        f2_struct_read_data(&s, st, &h);
    }
    io_free(&s);
}

void f2_struct_wread(f2_struct* st, wchar_t* path) {
    memset(st, 0, sizeof(f2_struct));

    stream s;
    io_wopen(&s, path, L"rb");
    if (&s.io.stream) {
        divafile_sdecrypt(&s);

        f2_header h;
        f2_header_read(&s, &h);
        f2_struct_read_data(&s, st, &h);
    }
    io_free(&s);
}

void f2_struct_mread(f2_struct* st, void* data, size_t length) {
    memset(st, 0, sizeof(f2_struct));

    stream s;
    io_mopen(&s, data, length);
    if (&s.io.data.data) {
        divafile_sdecrypt(&s);

        f2_header h;
        f2_header_read(&s, &h);
        f2_struct_read_data(&s, st, &h);
    }
    io_free(&s);
}

void f2_struct_sread(f2_struct* st, stream* s) {
    memset(st, 0, sizeof(f2_struct));

    if (s->io.stream || s->io.data.data) {
        divafile_sdecrypt(s);

        f2_header h;
        f2_header_read(s, &h);
        f2_struct_read_data(s, st, &h);
    }
}

void f2_struct_write(f2_struct* st, char* path, bool use_depth, bool shift_x) {
    stream s;
    io_open(&s, path, "wb");
    if (s.io.stream) {
        f2_struct_get_length(st, shift_x);
        f2_struct_write_inner(&s, st, 0, use_depth, shift_x);
    }
    io_free(&s);
}

void f2_struct_wwrite(f2_struct* st, wchar_t* path, bool use_depth, bool shift_x) {
    stream s;
    io_wopen(&s, path, L"wb");
    if (s.io.stream) {
        f2_struct_get_length(st, shift_x);
        f2_struct_write_inner(&s, st, 0, use_depth, shift_x);
    }
    io_free(&s);
}

void f2_struct_mwrite(f2_struct* st, void** data, size_t* length, bool use_depth, bool shift_x) {
    if (!st || !data)
        return;

    f2_struct_get_length(st, shift_x);
    stream s;
    io_mopen(&s, 0, st->header.data_size + 0x40ULL);
    f2_struct_write_inner(&s, st, 0, use_depth, shift_x);

    io_mcopy(&s, data, length);
    io_free(&s);
}

void f2_struct_swrite(f2_struct* st, stream* s, bool use_depth, bool shift_x) {
    if (s->io.stream || s->type == STREAM_MEMORY) {
        f2_struct_get_length(st, shift_x);
        f2_struct_write_inner(s, st, 0, use_depth, shift_x);
    }
}

void f2_struct_free(f2_struct* st) {
    if (st->data)
        free(st->data);
    st->length = 0;
    if (st->pof.begin)
        vector_size_t_free(&st->pof, 0);
    if (st->enrs.begin)
        enrs_free(&st->enrs);
    if (st->sub_structs.begin)
        vector_f2_struct_free(&st->sub_structs, f2_struct_free);
    memset(st, 0, sizeof(f2_struct));
}

static void f2_struct_get_length(f2_struct* s, bool shift_x) {
    bool has_enrs, has_pof, has_sub_structs;
    f2_struct* i;
    uint32_t l;
    vector_f2_struct ls;

    has_pof = vector_length(s->pof) > 0 ? true : false;
    has_enrs = vector_length(s->enrs) > 0 ? true : false;
    has_sub_structs = vector_length(s->sub_structs) > 0 ? true : false;

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

    if (has_enrs || has_pof || has_sub_structs)
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

    uint32_t sig;
    size_t length = (size_t)h->data_size - l;
    size_t position = 0;
    while (length > position) {
        f2_header_read(s, h);
        sig = reverse_endianness_uint32_t(h->signature);
        l = h->use_section_size ? h->section_size : h->data_size;
        position += (size_t)h->length + l;
        if (sig == 'EOFC')
            break;
        else if (sig == 'ENRS') {
            size_t pos = io_get_position(s);
            enrs_read(s, &st->enrs);
            io_set_position(s, pos + l, SEEK_SET);
        }
        else if ((sig & 0xF0FFFFFF) == 'POF0') {
            size_t pos = io_get_position(s);
            pof_read(s, &st->pof, sig == 'POF1');
            io_set_position(s, pos + l, SEEK_SET);
        }
        else {
            f2_struct* s_st = vector_f2_struct_reserve_back(&st->sub_structs);
            f2_struct_read_data(s, s_st, h);
        }
    }

    if (vector_length(st->sub_structs) < 1)
        vector_f2_struct_free(&st->sub_structs, f2_struct_free);
}

static void f2_struct_write_inner(stream* s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x) {
    st->header.depth = use_depth ? depth : 0;
    f2_header_write(s, &st->header, st->header.length == 0x40);
    if (st->data)
        io_write(s, st->data, st->length);
    if (st->enrs.begin)
        f2_struct_write_enrs(s, &st->enrs, use_depth ? depth + 1 : 0);
    if (st->pof.begin)
        f2_struct_write_pof(s, &st->pof, use_depth ? depth + 1 : 0, shift_x);
    if (st->sub_structs.begin)
        for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++)
            f2_struct_write_inner(s, i, depth + 1, use_depth, shift_x);
    if (st->enrs.begin || st->pof.begin || st->sub_structs.begin)
        f2_header_write_end_of_container(s, use_depth ? depth + 1 : 0);
    if (!depth)
        f2_header_write_end_of_container(s, 0);
}

static void f2_struct_write_pof(stream* s, vector_size_t* pof, uint32_t depth, bool shift_x) {
    size_t len = pof_length(pof, shift_x);
    f2_header h;
    memset(&h, 0, sizeof(f2_header));
    h.signature = shift_x ? reverse_endianness_uint32_t('POF1') : reverse_endianness_uint32_t('POF0');
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
    h.signature = reverse_endianness_uint32_t('ENRS');
    h.length = 0x20;
    h.depth = depth;
    h.use_section_size = true;
    h.data_size = h.section_size = (uint32_t)align_val(len, 0x10);
    f2_header_write(s, &h, false);
    enrs_write(s, enrs);
}
