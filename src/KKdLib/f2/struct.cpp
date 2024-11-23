/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../divafile.hpp"

static void f2_struct_read_data(stream& s, f2_struct* st, f2_header* h);
static void f2_struct_write_inner(stream& s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x);
static void f2_struct_get_length(f2_struct* s, bool shift_x);
static void f2_struct_write_pof(stream& s, pof* pof, uint32_t depth, bool shift_x);
static void f2_struct_write_enrs(stream& s, enrs* enrs, uint32_t depth);

f2_struct::f2_struct() {

}

f2_struct::~f2_struct() {

}

void f2_struct::read(const char* path) {
    if (!path)
        return;

    file_stream s;
    s.open(path, "rb");
    if (s.check_not_null()) {
        memory_stream ms;
        divafile::decrypt(s, ms);

        f2_header h;
        h.read(ms);
        f2_struct_read_data(ms, this, &h);
    }
}

void f2_struct::read(const wchar_t* path) {
    if (!path)
        return;

    file_stream s;
    s.open(path, L"rb");
    if (s.check_not_null()) {
        memory_stream ms;
        divafile::decrypt(s, ms);

        f2_header h;
        h.read(ms);
        f2_struct_read_data(ms, this, &h);
    }
}

void f2_struct::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (s.check_not_null()) {
        memory_stream ms;
        divafile::decrypt(s, ms);

        f2_header h;
        h.read(ms);
        f2_struct_read_data(ms, this, &h);
    }
}

void f2_struct::read(stream& s) {
    if (s.check_null())
        return;

    memory_stream ms;
    divafile::decrypt(s, ms);

    f2_header h;
    h.read(ms);
    f2_struct_read_data(ms, this, &h);
}

void f2_struct::write(const char* path, bool use_depth, bool shift_x) {
    if (!path)
        return;

    file_stream s;
    s.open(path, "wb");
    if (s.check_not_null()) {
        f2_struct_get_length(this, shift_x);
        f2_struct_write_inner(s, this, 0, use_depth, shift_x);
    }
}

void f2_struct::write(const wchar_t* path, bool use_depth, bool shift_x) {
    if (!path)
        return;

    file_stream s;
    s.open(path, L"wb");
    if (s.check_not_null()) {
        f2_struct_get_length(this, shift_x);
        f2_struct_write_inner(s, this, 0, use_depth, shift_x);
    }
}

void f2_struct::write(void** data, size_t* size, bool use_depth, bool shift_x) {
    if (!data || !size)
        return;

    f2_struct_get_length(this, shift_x);
    memory_stream s;
    s.open(0, header.get_data_size() + (size_t)F2_HEADER_EXTENDED_LENGTH);
    f2_struct_write_inner(s, this, 0, use_depth, shift_x);

    s.copy(data, size);
}

void f2_struct::write(stream& s, bool use_depth, bool shift_x) {
    if (s.check_null())
        return;

    f2_struct_get_length(this, shift_x);
    f2_struct_write_inner(s, this, 0, use_depth, shift_x);
}

static void f2_struct_get_length(f2_struct* s, bool shift_x) {
    bool has_pof = !!s->pof.vec.size();
    bool has_enrs = !!s->enrs.vec.size();
    bool has_sub_structs = !!s->sub_structs.size();

    if (!s->header.attrib.get_type() && (has_pof || has_enrs || has_sub_structs))
        s->header.attrib.set_type(0x01);

    s->header.set_section_size((uint32_t)s->data.size());

    uint32_t l = s->header.get_section_size();
    if (has_enrs)
        l += F2_HEADER_DEFAULT_LENGTH + align_val(s->enrs.length(), 0x10);

    if (has_pof) {
        s->pof.shift_x = shift_x;
        l += F2_HEADER_DEFAULT_LENGTH + align_val(s->pof.length(), 0x10);
    }

    if (has_sub_structs) {
        for (f2_struct& i : s->sub_structs) {
            f2_struct_get_length(&i, shift_x);
            l += i.header.get_data_size();
            l += i.header.get_length();
        }
        l += F2_HEADER_DEFAULT_LENGTH;
    }

    s->header.set_data_size(l);
}

static void f2_struct_read_data(stream& s, f2_struct* st, f2_header* h) {
    uint32_t l = h->get_section_size();
    st->header = *h;
    if (l) {
        st->data.resize(l);
        s.read(st->data.data(), l);
    }

    uint32_t sig;
    size_t length = (size_t)h->get_data_size() - l;
    size_t position = 0;
    while (length > position) {
        h->read(s);
        sig = reverse_endianness_uint32_t(h->signature);
        l = h->get_section_size();
        position += (size_t)h->get_length() + l;
        if (sig == 'EOFC')
            break;
        else if (sig == 'ENRS') {
            size_t pos = s.get_position();
            st->enrs.read(s);
            s.set_position(pos + l, SEEK_SET);
        }
        else if ((sig & 0xFFFFFFF0) == 'POF0') {
            size_t pos = s.get_position();
            st->pof.shift_x = sig == 'POF1';
            st->pof.read(s);
            s.set_position(pos + l, SEEK_SET);
        }
        else {
            st->sub_structs.push_back({});
            f2_struct_read_data(s, &st->sub_structs.back(), h);
        }
    }
}

static void f2_struct_write_inner(stream& s, f2_struct* st, uint32_t depth, bool use_depth, bool shift_x) {
    bool has_pof = !!st->pof.vec.size();
    bool has_enrs = !!st->enrs.vec.size();
    bool has_sub_structs = !!st->sub_structs.size();

    st->header.set_depth(use_depth ? (uint8_t)depth : 0);
    st->header.write(s);
    if (st->data.size())
        s.write(st->data.data(), st->data.size());
    if (has_enrs)
        f2_struct_write_enrs(s, &st->enrs, use_depth ? depth + 1 : 0);
    if (has_pof)
        f2_struct_write_pof(s, &st->pof, use_depth ? depth + 1 : 0, shift_x);
    if (has_sub_structs) {
        for (f2_struct& i : st->sub_structs)
            f2_struct_write_inner(s, &i, depth + 1, use_depth, shift_x);

        f2_header('EOFC', 0, use_depth ? depth + 1 : 0).write(s);
    }

    if (!depth) {
        f2_header('EOFC', 0).write(s);
    }
}

static void f2_struct_write_pof(stream& s, pof* pof, uint32_t depth, bool shift_x) {
    pof->shift_x = shift_x;
    f2_header(shift_x ? 'POF1' : 'POF0', (uint32_t)align_val(pof->length(), 0x10), depth).write(s);
    pof->write(s);
}

static void f2_struct_write_enrs(stream& s, enrs* enrs, uint32_t depth) {
    f2_header('ENRS', (uint32_t)align_val(enrs->length(), 0x10), depth).write(s);
    enrs->write(s);
}
