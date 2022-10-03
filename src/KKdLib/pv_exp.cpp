/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_exp.hpp"
#include "f2/struct.hpp"
#include "io/file_stream.hpp"
#include "io/memory_stream.hpp"
#include "io/path.hpp"
#include "str_utils.hpp"

static void pv_exp_classic_read_inner(pv_exp* exp, stream& s);
static void pv_exp_classic_write_inner(pv_exp* exp, stream& s);
static void pv_exp_modern_read_inner(pv_exp* exp, stream& s, uint32_t header_length);
static void pv_exp_modern_write_inner(pv_exp* exp, stream& s);

pv_exp_mot::pv_exp_mot() : face_data(), face_cl_data() {

}

pv_exp_mot::~pv_exp_mot() {
    if (face_data) {
        delete[] face_data;
        face_data = 0;
    }

    if (face_cl_data) {
        delete[] face_cl_data;
        face_cl_data = 0;
    }
}

pv_exp::pv_exp() : ready(), modern(), big_endian(), is_x() {

}

pv_exp::~pv_exp() {

}

void pv_exp::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                pv_exp_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_dex = str_utils_add(path, ".dex");
        if (path_check_file_exists(path_dex)) {
            f2_struct st;
            st.read(path_dex);
            if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
                memory_stream s_expc;
                s_expc.open(st.data);
                s_expc.big_endian = st.header.use_big_endian;
                pv_exp_modern_read_inner(this, s_expc, st.header.length);
            }
        }
        free_def(path_dex);
    }
}

void pv_exp::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                pv_exp_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_dex = str_utils_add(path, L".dex");
        if (path_check_file_exists(path_dex)) {
            f2_struct st;
            st.read(path_dex);
            if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
                memory_stream s_expc;
                s_expc.open(st.data);
                s_expc.big_endian = st.header.use_big_endian;
                pv_exp_modern_read_inner(this, s_expc, st.header.length);
            }
        }
        free_def(path_dex);
    }
}

void pv_exp::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        pv_exp_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
            memory_stream s_expc;
            s_expc.open(st.data);
            s_expc.big_endian = st.header.use_big_endian;
            pv_exp_modern_read_inner(this, s_expc, st.header.length);
        }
    }
}

void pv_exp::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            pv_exp_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_dex = str_utils_add(path, ".dex");
        file_stream s;
        s.open(path_dex, "wb");
        if (s.check_not_null())
            pv_exp_modern_write_inner(this, s);
        free_def(path_dex);
    }
}

void pv_exp::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            pv_exp_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_dex = str_utils_add(path, L".dex");
        file_stream s;
        s.open(path_dex, L"wb");
        if (s.check_not_null())
            pv_exp_modern_write_inner(this, s);
        free_def(path_dex);
    }
}

void pv_exp::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        pv_exp_classic_write_inner(this, s);
    else
        pv_exp_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool pv_exp::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    pv_exp* exp = (pv_exp*)data;
    exp->read(s.c_str(), exp->modern);

    return exp->ready;
}

static void pv_exp_classic_read_inner(pv_exp* exp, stream& s) {
    if (s.read_uint32_t() != 0x64) {
        exp->ready = false;
        exp->modern = false;
        exp->big_endian = false;
        exp->is_x = false;
        return;
    }

    exp->motion_data.resize(0);
    exp->motion_data.resize(s.read_int32_t());

    int64_t motion_offsets_offset = s.read_int32_t();
    int64_t name_offsets_offset = s.read_uint32_t();

    int64_t* face_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* face_cl_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* name_offset = force_malloc_s(int64_t, exp->motion_data.size());

    s.position_push(motion_offsets_offset, SEEK_SET);
    for (pv_exp_mot& i : exp->motion_data) {
        face_offset[&i - exp->motion_data.data()] = s.read_uint32_t();
        face_cl_offset[&i - exp->motion_data.data()] = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(name_offsets_offset, SEEK_SET);
    for (pv_exp_mot& i : exp->motion_data)
        name_offset[&i - exp->motion_data.data()] = s.read_uint32_t();
    s.position_pop();

    for (pv_exp_mot& i : exp->motion_data) {
        if (face_offset[&i - exp->motion_data.data()]) {
            size_t face_count = 0;
            s.position_push(face_offset[&i - exp->motion_data.data()], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0C);
                face_count++;
            }
            face_count++;
            s.position_pop();

            i.face_data = new pv_exp_data[face_count];
            pv_exp_data* face_data = i.face_data;
            s.position_push(face_offset[&i - exp->motion_data.data()], SEEK_SET);
            for (size_t j = face_count; j; j--, face_data++) {
                face_data->frame = s.read_float_t();
                face_data->type = s.read_int16_t();
                face_data->id = s.read_int16_t();
                face_data->value = s.read_float_t();
                face_data->trans = s.read_float_t();
            }
            s.position_pop();
        }

        if (face_cl_offset[&i - exp->motion_data.data()]) {
            size_t face_cl_count = 0;
            s.position_push(face_cl_offset[&i - exp->motion_data.data()], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0C);
                face_cl_count++;
            }
            face_cl_count++;
            s.position_pop();

            i.face_cl_data = new pv_exp_data[face_cl_count];
            pv_exp_data* face_cl_data = i.face_cl_data;
            s.position_push(face_cl_offset[&i - exp->motion_data.data()], SEEK_SET);
            for (size_t j = face_cl_count; j; j--, face_cl_data++) {
                face_cl_data->frame = s.read_float_t();
                face_cl_data->type = s.read_int16_t();
                face_cl_data->id = s.read_int16_t();
                face_cl_data->value = s.read_float_t();
                face_cl_data->trans = s.read_float_t();
            }
            s.position_pop();
        }

        if (name_offset[&i - exp->motion_data.data()])
            i.name = s.read_string_null_terminated_offset(name_offset[&i - exp->motion_data.data()]);
    }

    free_def(face_offset);
    free_def(face_cl_offset);
    free_def(name_offset);

    exp->ready = true;
    exp->modern = false;
    exp->big_endian = false;
    exp->is_x = false;
}

static void pv_exp_classic_write_inner(pv_exp* exp, stream& s) {
    s.write_uint32_t(0x64);
    s.write_uint32_t((uint32_t)exp->motion_data.size());
    s.write_uint32_t(0x20);

    s.position_push(s.get_position(), SEEK_SET);
    s.write_uint32_t(0x00);

    s.write_uint32_t(0x00);
    s.write_uint32_t(0x00);

    for (pv_exp_mot& i : exp->motion_data) {
        s.write_uint32_t(0x00);
        s.write_uint32_t(0x00);
        s.write_uint32_t(0x00);
    }
    s.align_write(0x20);

    int64_t* face_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* face_cl_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* name_offset = force_malloc_s(int64_t, exp->motion_data.size());

    for (pv_exp_mot& i : exp->motion_data) {
        face_offset[&i - exp->motion_data.data()] = s.get_position();

        pv_exp_data* face_data = i.face_data;
        while (true) {
            s.write_float_t(face_data->frame);
            s.write_int16_t(face_data->type);
            s.write_int16_t(face_data->id);
            s.write_float_t(face_data->value);
            s.write_float_t(face_data->trans);

            if (face_data->type == -1)
                break;

            face_data++;
        }
        s.align_write(0x20);

        face_cl_offset[&i - exp->motion_data.data()] = s.get_position();

        pv_exp_data* face_cl_data = i.face_cl_data;
        while (true) {
            s.write_float_t(face_cl_data->frame);
            s.write_int16_t(face_cl_data->type);
            s.write_int16_t(face_cl_data->id);
            s.write_float_t(face_cl_data->value);
            s.write_float_t(face_cl_data->trans);

            if (face_cl_data->type == -1)
                break;

            face_cl_data++;
        }
        s.align_write(0x20);
    }

    for (pv_exp_mot& i : exp->motion_data) {
        name_offset[&i - exp->motion_data.data()] = s.get_position();
        s.write_string_null_terminated(i.name);
    }
    s.align_write(0x10);

    s.set_position(exp->is_x ? 0x28 : 0x20, SEEK_SET);
    for (pv_exp_mot& i : exp->motion_data) {
        s.write_uint32_t((uint32_t)face_offset[&i - exp->motion_data.data()]);
        s.write_uint32_t((uint32_t)face_cl_offset[&i - exp->motion_data.data()]);
    }

    int64_t name_offsets_offset = s.get_position();
    for (pv_exp_mot& i : exp->motion_data)
        s.write_uint32_t((uint32_t)name_offset[&i - exp->motion_data.data()]);

    free_def(face_offset);
    free_def(face_cl_offset);
    free_def(name_offset);
    s.position_pop();

    s.write_uint32_t((uint32_t)name_offsets_offset);
}

static void pv_exp_modern_read_inner(pv_exp* exp, stream& s, uint32_t header_length) {
    if (s.read_uint32_t() != 0x64) {
        exp->ready = false;
        exp->modern = false;
        exp->big_endian = false;
        exp->is_x = false;
        return;
    }

    bool big_endian = s.big_endian;
    bool is_x = false;

    exp->motion_data.resize(0);
    exp->motion_data.resize(s.read_int32_t());

    int64_t motion_offsets_offset = s.read_int32_t();
    int64_t name_offsets_offset = s.read_offset_f2(header_length);
    if (name_offsets_offset == 0x00) {
        is_x = true;
        name_offsets_offset = (int)s.read_offset_x();
    }

    int64_t* face_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* face_cl_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* name_offset = force_malloc_s(int64_t, exp->motion_data.size());

    s.position_push(motion_offsets_offset, SEEK_SET);
    if (!is_x)
        for (pv_exp_mot& i : exp->motion_data) {
            face_offset[&i - exp->motion_data.data()] = s.read_offset_f2(header_length);
            face_cl_offset[&i - exp->motion_data.data()] = s.read_offset_f2(header_length);
        }
    else
        for (pv_exp_mot& i : exp->motion_data) {
            face_offset[&i - exp->motion_data.data()] = s.read_offset_x();
            face_cl_offset[&i - exp->motion_data.data()] = s.read_offset_x();
        }
    s.position_pop();

    s.position_push(name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (pv_exp_mot& i : exp->motion_data)
            name_offset[&i - exp->motion_data.data()] = s.read_offset_f2(header_length);
    else
        for (pv_exp_mot& i : exp->motion_data)
            name_offset[&i - exp->motion_data.data()] = s.read_offset_x();
    s.position_pop();

    for (pv_exp_mot& i : exp->motion_data) {
        if (face_offset[&i - exp->motion_data.data()]) {
            size_t face_count = 0;
            s.position_push(face_offset[&i - exp->motion_data.data()], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_count++;
            }
            face_count++;
            s.position_pop();

            i.face_data = new pv_exp_data[face_count];
            pv_exp_data* face_data = i.face_data;
            s.position_push(face_offset[&i - exp->motion_data.data()], SEEK_SET);
            for (size_t j = face_count; j; j--, face_data++) {
                face_data->frame = s.read_float_t_reverse_endianness();
                face_data->type = s.read_int16_t_reverse_endianness();
                face_data->id = s.read_int16_t_reverse_endianness();
                face_data->value = s.read_float_t_reverse_endianness();
                face_data->trans = s.read_float_t_reverse_endianness();
            }
            s.position_pop();
        }

        if (face_cl_offset[&i - exp->motion_data.data()]) {
            size_t face_cl_count = 0;
            s.position_push(face_cl_offset[&i - exp->motion_data.data()], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_cl_count++;
            }
            face_cl_count++;
            s.position_pop();

            i.face_cl_data = new pv_exp_data[face_cl_count];
            pv_exp_data* face_cl_data = i.face_cl_data;
            s.position_push(face_cl_offset[&i - exp->motion_data.data()], SEEK_SET);
            for (size_t j = face_cl_count; j; j--, face_cl_data++) {
                face_cl_data->frame = s.read_float_t_reverse_endianness();
                face_cl_data->type = s.read_int16_t_reverse_endianness();
                face_cl_data->id = s.read_int16_t_reverse_endianness();
                face_cl_data->value = s.read_float_t_reverse_endianness();
                face_cl_data->trans = s.read_float_t_reverse_endianness();
            }
            s.position_pop();
        }

        if (name_offset[&i - exp->motion_data.data()])
            i.name = s.read_string_null_terminated_offset(name_offset[&i - exp->motion_data.data()]);
    }

    free_def(face_offset);
    free_def(face_cl_offset);
    free_def(name_offset);

    exp->ready = true;
    exp->modern = true;
    exp->big_endian = big_endian;
    exp->is_x = is_x;
}

static void pv_exp_modern_write_inner(pv_exp* exp, stream& s) {
    bool big_endian = exp->big_endian;
    bool is_x = exp->is_x;

    memory_stream s_expc;
    s_expc.open();
    s_expc.big_endian = big_endian;

    s_expc.write_uint32_t_reverse_endianness(0x64);
    s_expc.write_int32_t_reverse_endianness((int32_t)exp->motion_data.size());
    s_expc.write_offset(is_x ? 0x28 : 0x20, 0x20, is_x);

    s_expc.position_push(s_expc.get_position(), SEEK_SET);
    s_expc.write_offset(0x00, 0x20, is_x);

    s_expc.write_offset(0x00, 0x20, is_x);
    s_expc.write_offset(0x00, 0x20, is_x);

    if (!is_x)
        for (pv_exp_mot& i : exp->motion_data) {
            s_expc.write_offset_f2(0x00, 0x20);
            s_expc.write_offset_f2(0x00, 0x20);
            s_expc.write_offset_f2(0x00, 0x20);
        }
    else
        for (pv_exp_mot& i : exp->motion_data) {
            s_expc.write_offset_x(0x00);
            s_expc.write_offset_x(0x00);
            s_expc.write_offset_x(0x00);
        }
    s_expc.align_write(0x20);

    int64_t* face_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* face_cl_offset = force_malloc_s(int64_t, exp->motion_data.size());
    int64_t* name_offset = force_malloc_s(int64_t, exp->motion_data.size());

    for (pv_exp_mot& i : exp->motion_data) {
        face_offset[&i - exp->motion_data.data()] = s_expc.get_position();

        pv_exp_data* face_data = i.face_data;
        while (true) {
            s_expc.write_float_t_reverse_endianness(face_data->frame);
            s_expc.write_int16_t_reverse_endianness(face_data->type);
            s_expc.write_int16_t_reverse_endianness(face_data->id);
            s_expc.write_float_t_reverse_endianness(face_data->value);
            s_expc.write_float_t_reverse_endianness(face_data->trans);

            if (face_data->type == -1)
                break;

            face_data++;
        }
        s_expc.align_write(0x20);

        face_cl_offset[&i - exp->motion_data.data()] = s_expc.get_position();

        pv_exp_data* face_cl_data = i.face_cl_data;
        while (true) {
            s_expc.write_float_t_reverse_endianness(face_cl_data->frame);
            s_expc.write_int16_t_reverse_endianness(face_cl_data->type);
            s_expc.write_int16_t_reverse_endianness(face_cl_data->id);
            s_expc.write_float_t_reverse_endianness(face_cl_data->value);
            s_expc.write_float_t_reverse_endianness(face_cl_data->trans);

            if (face_cl_data->type == -1)
                break;

            face_cl_data++;
        }
        s_expc.align_write(0x20);
    }

    for (pv_exp_mot& i : exp->motion_data) {
        name_offset[&i - exp->motion_data.data()] = s_expc.get_position();
        s_expc.write_string_null_terminated(i.name);
    }
    s_expc.align_write(0x10);

    s_expc.set_position(is_x ? 0x28 : 0x20, SEEK_SET);
    if (!is_x)
        for (pv_exp_mot& i : exp->motion_data) {
            s_expc.write_offset_f2(face_offset[&i - exp->motion_data.data()], 0x20);
            s_expc.write_offset_f2(face_cl_offset[&i - exp->motion_data.data()], 0x20);
        }
    else
        for (pv_exp_mot& i : exp->motion_data) {
            s_expc.write_offset_x(face_offset[&i - exp->motion_data.data()]);
            s_expc.write_offset_x(face_cl_offset[&i - exp->motion_data.data()]);
        }

    int64_t name_offsets_offset = s_expc.get_position();
    if (!is_x)
        for (pv_exp_mot& i : exp->motion_data)
            s_expc.write_offset_f2(name_offset[&i - exp->motion_data.data()], 0x20);
    else
        for (pv_exp_mot& i : exp->motion_data)
            s_expc.write_offset_x(name_offset[&i - exp->motion_data.data()]);

    free_def(face_offset);
    free_def(face_cl_offset);
    free_def(name_offset);

    s_expc.position_pop();

    s_expc.write_offset(name_offsets_offset, 0x20, is_x);

    f2_struct st;
    s_expc.align_write(0x10);
    s_expc.copy(st.data);
    s_expc.close();

    st.header.signature = reverse_endianness_uint32_t('EXPC');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, exp->is_x);
}
