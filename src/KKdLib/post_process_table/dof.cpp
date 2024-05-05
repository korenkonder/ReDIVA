/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dof.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../str_utils.hpp"

static void dof_read_inner(dof* d, stream& s, uint32_t header_length);
static void dof_write_inner(dof* d, stream& s);

dof::dof() : ready(), big_endian(), is_x() {
    murmurhash = hash_murmurhash_empty;
}

dof::~dof() {

}

void dof::read(const char* path) {
    if (!path)
        return;

    char* path_dft = str_utils_add(path, ".dft");
    if (path_check_file_exists(path_dft)) {
        f2_struct st;
        st.read(path_dft);
        if (st.header.signature == reverse_endianness_uint32_t('DOFT')) {
            memory_stream s_doft;
            s_doft.open(st.data);
            s_doft.big_endian = st.header.use_big_endian;
            dof_read_inner(this, s_doft, st.header.length);
        }
    }
    free_def(path_dft);
}

void dof::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_dex = str_utils_add(path, L".dft");
    if (path_check_file_exists(path_dex)) {
        f2_struct st;
        st.read(path_dex);
        if (st.header.signature == reverse_endianness_uint32_t('DOFT')) {
            memory_stream s_doft;
            s_doft.open(st.data);
            s_doft.big_endian = st.header.use_big_endian;
            dof_read_inner(this, s_doft, st.header.length);
        }
    }
    free_def(path_dex);
}

void dof::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    f2_struct st;
    st.read(data, size);
    if (st.header.signature == reverse_endianness_uint32_t('DOFT')) {
        memory_stream s_doft;
        s_doft.open(st.data);
        s_doft.big_endian = st.header.use_big_endian;
        dof_read_inner(this, s_doft, st.header.length);
    }
}

void dof::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_dex = str_utils_add(path, ".dft");
    file_stream s;
    s.open(path_dex, "wb");
    if (s.check_not_null())
        dof_write_inner(this, s);
    free_def(path_dex);
}

void dof::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_dex = str_utils_add(path, L".dft");
    file_stream s;
    s.open(path_dex, L"wb");
    if (s.check_not_null())
        dof_write_inner(this, s);
    free_def(path_dex);
}

void dof::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    dof_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool dof::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    dof* d = (dof*)data;
    d->read(path.c_str());

    return d->ready;
}

static void dof_read_inner(dof* d, stream& s, uint32_t header_length) {
    d->data.resize(0);

    uint32_t count = s.read_uint32_t_reverse_endianness();
    int64_t offset = s.read_offset_f2(header_length);
    if (count < 1)
        return;

    d->data.resize(count);

    bool big_endian = s.big_endian;
    bool is_x = false;
    if (offset) {
        s.position_push(offset, SEEK_SET);
        for (dof_data& i : d->data) {
            i.flags = (dof_flags)s.read_uint32_t_reverse_endianness();
            i.focus = s.read_float_t_reverse_endianness(i.focus);
            i.focus_range = s.read_float_t_reverse_endianness(i.focus_range);
            i.fuzzing_range = s.read_float_t_reverse_endianness(i.fuzzing_range);
            i.ratio = s.read_float_t_reverse_endianness(i.ratio);
            i.quality = s.read_float_t_reverse_endianness(i.quality);
            i.chara_id = -1;
        }
        s.position_pop();
    }
    else {
        is_x = true;

        offset = s.read_offset_x();

        s.position_push(offset, SEEK_SET);
        for (dof_data& i : d->data) {
            i.flags = (dof_flags)s.read_uint32_t_reverse_endianness();
            i.focus = s.read_float_t_reverse_endianness(i.focus);
            i.focus_range = s.read_float_t_reverse_endianness(i.focus_range);
            i.fuzzing_range = s.read_float_t_reverse_endianness(i.fuzzing_range);
            i.ratio = s.read_float_t_reverse_endianness(i.ratio);
            i.quality = s.read_float_t_reverse_endianness(i.quality);
            i.chara_id = s.read_int32_t_reverse_endianness(i.chara_id);
        }
        s.position_pop();
    }

    d->ready = true;
    d->big_endian = big_endian;
    d->is_x = is_x;
}

static void dof_write_inner(dof* d, stream& s) {
    bool big_endian = d->big_endian;
    bool is_x = d->is_x;

    memory_stream s_doft;
    s_doft.open();
    s_doft.big_endian = big_endian;

    uint32_t o;
    enrs e;
    enrs_entry ee;
    pof pof;
    uint32_t murmurhash = 0;
    if (d->data.size() > 0) {
        if (!is_x) {
            ee = { 0, 1, 16, 1 };
            ee.append(0, 2, ENRS_DWORD);
            e.vec.push_back(ee);
            o = 16;

            ee = { o, 1, 24, (uint32_t)d->data.size() };
            ee.append(0, 6, ENRS_DWORD);
            e.vec.push_back(ee);
            o = (uint32_t)(24 * d->data.size());
        }
        else {
            ee = { 0, 2, 16, 1 };
            ee.append(0, 1, ENRS_DWORD);
            ee.append(4, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            o = 16;

            ee = { o, 1, 28, (uint32_t)d->data.size() };
            ee.append(0, 7, ENRS_DWORD);
            e.vec.push_back(ee);
            o = (uint32_t)(28 * d->data.size());
        }

        if (!is_x) {
            s_doft.write_uint32_t((uint32_t)d->data.size());
            pof.add(s, 0x40);
            s.write_offset_x(0x50);
            s.align_write(0x10);

            for (dof_data& i : d->data) {
                s.write_uint32_t_reverse_endianness(i.flags);
                s.write_float_t_reverse_endianness(i.focus);
                s.write_float_t_reverse_endianness(i.focus_range);
                s.write_float_t_reverse_endianness(i.fuzzing_range);
                s.write_float_t_reverse_endianness(i.ratio);
                s.write_float_t_reverse_endianness(i.quality);
            }
        }
        else {
            s_doft.write_uint32_t((uint32_t)d->data.size());
            s.align_write(0x08);
            pof.add(s, 0x00);
            s.write_offset_x(0x10);
            s.align_write(0x10);

            for (dof_data& i : d->data) {
                s.write_uint32_t_reverse_endianness(i.flags);
                s.write_float_t_reverse_endianness(i.focus);
                s.write_float_t_reverse_endianness(i.focus_range);
                s.write_float_t_reverse_endianness(i.fuzzing_range);
                s.write_float_t_reverse_endianness(i.ratio);
                s.write_float_t_reverse_endianness(i.quality);
                s.write_int32_t_reverse_endianness(i.chara_id);
            }
        }

        murmurhash = d->murmurhash;
    }

    f2_struct st;
    s_doft.align_write(0x10);
    s_doft.copy(st.data);
    s_doft.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('DOFT');
    st.header.length = 0x40;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;
    st.header.murmurhash = murmurhash;
    st.header.inner_signature = 0x03;

    st.write(s, true, is_x);
}
