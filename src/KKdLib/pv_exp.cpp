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

const pv_exp_data pv_exp_data_null = { 999999.0f, -1, 0, 0.0f, 0.0f };

static void pv_exp_classic_read_inner(pv_exp* exp, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void pv_exp_classic_write_inner(pv_exp* exp, stream& s);

static void pv_exp_modern_read_inner(pv_exp* exp, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, uint32_t header_length);
static void pv_exp_modern_write_inner(pv_exp* exp, stream& s);

static const char* pv_exp_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

pv_exp_mot::pv_exp_mot() : face_data(), face_cl_data(), name() {

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

pv_exp::pv_exp() : ready(), modern(), big_endian(), is_x(), motion_data(), motion_num() {

}

void pv_exp::move_data(pv_exp* exp_src, prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!exp_src->ready) {
        ready = false;
        modern = false;
        big_endian = false;
        is_x = false;
        return;
    }

    ready = true;
    modern = exp_src->modern;
    big_endian = exp_src->big_endian;
    is_x = exp_src->is_x;

    uint32_t motion_num = exp_src->motion_num;
    pv_exp_mot* motion_data_src = exp_src->motion_data;
    pv_exp_mot* motion_data_dst = alloc->allocate<pv_exp_mot>(motion_num);
    this->motion_data = motion_data_dst;
    this->motion_num = motion_num;

    for (uint32_t i = 0; i < motion_num; i++) {
        pv_exp_mot& mot_src = motion_data_src[i];
        pv_exp_mot& mot_dst = motion_data_dst[i];

        size_t face_count = 0;
        pv_exp_data* face_data_src = mot_src.face_data;
        while (face_data_src->type != -1)
            face_count++;
        face_count++;

        size_t face_cl_count = 0;
        pv_exp_data* face_cl_data_src = mot_src.face_cl_data;
        while (face_cl_data_src->type != -1)
            face_cl_count++;
        face_cl_count++;

        mot_dst.face_data = alloc->allocate<pv_exp_data>(mot_src.face_data, face_count);
        mot_dst.face_cl_data = alloc->allocate<pv_exp_data>(mot_src.face_cl_data, face_cl_count);
        if (mot_src.name)
            mot_dst.name = alloc->allocate<char>(mot_src.name, utf8_length(mot_src.name) + 1);
        else
            mot_src.name = 0;
    }
}

void pv_exp::pack_file(void** data, size_t* size) {
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

void pv_exp::unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        pv_exp_classic_read_inner(this, alloc, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
            memory_stream s_expc;
            s_expc.open(st.data);
            s_expc.big_endian = st.header.attrib.get_big_endian();
            pv_exp_modern_read_inner(this, alloc, s_expc, st.header.get_length());
        }
    }
}

static void pv_exp_classic_read_inner(pv_exp* exp, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    if (s.read_uint32_t() != 0x64) {
        exp->ready = false;
        exp->modern = false;
        exp->big_endian = false;
        exp->is_x = false;
        return;
    }

    uint32_t motion_num = s.read_int32_t();
    pv_exp_mot* motion_data = alloc->allocate<pv_exp_mot>(motion_num);
    exp->motion_data = motion_data;
    exp->motion_num = motion_num;

    int64_t motion_offsets_offset = s.read_int32_t();
    int64_t name_offsets_offset = s.read_uint32_t();

    int64_t* offsets = force_malloc<int64_t>(motion_num * 3ULL);
    int64_t* face_offset = offsets;
    int64_t* face_cl_offset = offsets + motion_num;
    int64_t* name_offset = offsets + motion_num * 2ULL;

    s.position_push(motion_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < motion_num; i++) {
        face_offset[i] = s.read_uint32_t();
        face_cl_offset[i] = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(name_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < motion_num; i++)
        name_offset[i] = s.read_uint32_t();
    s.position_pop();

    for (uint32_t i = 0; i < motion_num; i++) {
        pv_exp_mot& mot = motion_data[i];

        if (face_offset[i]) {
            size_t face_count = 0;
            s.position_push(face_offset[i], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_count++;
            }
            face_count++;
            s.position_pop();

            pv_exp_data* face_data = alloc->allocate<pv_exp_data>(face_count);
            mot.face_data = face_data;
            s.position_push(face_offset[i], SEEK_SET);
            for (size_t j = face_count; j; j--, face_data++) {
                face_data->frame = s.read_float_t();
                face_data->type = s.read_int16_t();
                face_data->id = s.read_int16_t();
                face_data->value = s.read_float_t();
                face_data->trans = s.read_float_t();
            }
            s.position_pop();
        }

        if (face_cl_offset[i]) {
            size_t face_cl_count = 0;
            s.position_push(face_cl_offset[i], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_cl_count++;
            }
            face_cl_count++;
            s.position_pop();

            pv_exp_data* face_cl_data = alloc->allocate<pv_exp_data>(face_cl_count);
            mot.face_cl_data = face_cl_data;
            s.position_push(face_cl_offset[i], SEEK_SET);
            for (size_t j = face_cl_count; j; j--, face_cl_data++) {
                face_cl_data->frame = s.read_float_t();
                face_cl_data->type = s.read_int16_t();
                face_cl_data->id = s.read_int16_t();
                face_cl_data->value = s.read_float_t();
                face_cl_data->trans = s.read_float_t();
            }
            s.position_pop();
        }

        if (name_offset[i])
            mot.name = pv_exp_read_utf8_string_null_terminated_offset(alloc, s, name_offset[i]);
    }

    free_def(offsets);

    exp->ready = true;
    exp->modern = false;
    exp->big_endian = false;
    exp->is_x = false;
}

static void pv_exp_classic_write_inner(pv_exp* exp, stream& s) {
    uint32_t motion_num = exp->motion_num;

    s.write_uint32_t(0x64);
    s.write_uint32_t(motion_num);
    s.write_uint32_t(0x20);

    s.position_push(s.get_position(), SEEK_SET);
    s.write_uint32_t(0x00);

    s.write_uint32_t(0x00);
    s.write_uint32_t(0x00);

    for (uint32_t i = 0; i < motion_num; i++) {
        s.write_uint32_t(0x00);
        s.write_uint32_t(0x00);
        s.write_uint32_t(0x00);
    }
    s.align_write(0x20);

    int64_t* offsets = force_malloc<int64_t>(motion_num * 3ULL);
    int64_t* face_offset = offsets;
    int64_t* face_cl_offset = offsets + motion_num;
    int64_t* name_offset = offsets + motion_num * 2ULL;

    pv_exp_mot* motion_data = exp->motion_data;
    for (uint32_t i = 0; i < motion_num; i++) {
        pv_exp_mot& mot = motion_data[i];
        face_offset[i] = s.get_position();

        pv_exp_data* face_data = mot.face_data;
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
        s.align_write(0x10);

        face_cl_offset[i] = s.get_position();

        pv_exp_data* face_cl_data = mot.face_cl_data;
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
        s.align_write(0x10);
    }

    for (uint32_t i = 0; i < motion_num; i++) {
        name_offset[i] = s.get_position();
        s.write_utf8_string_null_terminated(motion_data[i].name);
    }
    s.align_write(0x10);

    s.set_position(0x20, SEEK_SET);
    for (uint32_t i = 0; i < motion_num; i++) {
        s.write_uint32_t((uint32_t)face_offset[i]);
        s.write_uint32_t((uint32_t)face_cl_offset[i]);
    }

    int64_t name_offsets_offset = s.get_position();
    for (uint32_t i = 0; i < motion_num; i++)
        s.write_uint32_t((uint32_t)name_offset[i]);

    free_def(offsets);
    s.position_pop();

    s.write_uint32_t((uint32_t)name_offsets_offset);
}

static void pv_exp_modern_read_inner(pv_exp* exp, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, uint32_t header_length) {
    if (s.read_uint32_t() != 0x64) {
        exp->ready = false;
        exp->modern = false;
        exp->big_endian = false;
        exp->is_x = false;
        return;
    }

    bool big_endian = s.big_endian;
    bool is_x = false;

    uint32_t motion_num = s.read_int32_t();
    pv_exp_mot* motion_data = alloc->allocate<pv_exp_mot>(motion_num);
    exp->motion_data = motion_data;
    exp->motion_num = motion_num;

    int64_t motion_offsets_offset = s.read_int32_t();
    int64_t name_offsets_offset = s.read_offset_f2(header_length);
    if (name_offsets_offset == 0x00) {
        is_x = true;
        name_offsets_offset = (int)s.read_offset_x();
    }

    int64_t* offsets = force_malloc<int64_t>(motion_num * 3ULL);
    int64_t* face_offset = offsets;
    int64_t* face_cl_offset = offsets + motion_num;
    int64_t* name_offset = offsets + motion_num * 2ULL;

    s.position_push(motion_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < motion_num; i++) {
            face_offset[i] = s.read_offset_f2(header_length);
            face_cl_offset[i] = s.read_offset_f2(header_length);
        }
    else
        for (uint32_t i = 0; i < motion_num; i++) {
            face_offset[i] = s.read_offset_x();
            face_cl_offset[i] = s.read_offset_x();
        }
    s.position_pop();

    s.position_push(name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < motion_num; i++)
            name_offset[i] = s.read_offset_f2(header_length);
    else
        for (uint32_t i = 0; i < motion_num; i++)
            name_offset[i] = s.read_offset_x();
    s.position_pop();

    for (uint32_t i = 0; i < motion_num; i++) {
        pv_exp_mot& mot = motion_data[i];

        if (face_offset[i]) {
            size_t face_count = 0;
            s.position_push(face_offset[i], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_count++;
            }
            face_count++;
            s.position_pop();

            pv_exp_data* face_data = alloc->allocate<pv_exp_data>(face_count);
            mot.face_data = face_data;
            s.position_push(face_offset[i], SEEK_SET);
            for (size_t j = face_count; j; j--, face_data++) {
                face_data->frame = s.read_float_t_reverse_endianness();
                face_data->type = s.read_int16_t_reverse_endianness();
                face_data->id = s.read_int16_t_reverse_endianness();
                face_data->value = s.read_float_t_reverse_endianness();
                face_data->trans = s.read_float_t_reverse_endianness();
            }
            s.position_pop();
        }

        if (face_cl_offset[i]) {
            size_t face_cl_count = 0;
            s.position_push(face_cl_offset[i], SEEK_SET);
            s.read(0x04);
            while (s.read_int16_t() != -1) {
                s.read(0x0E);
                face_cl_count++;
            }
            face_cl_count++;
            s.position_pop();

            pv_exp_data* face_cl_data = alloc->allocate<pv_exp_data>(face_cl_count);
            mot.face_cl_data = face_cl_data;
            s.position_push(face_cl_offset[i], SEEK_SET);
            for (size_t j = face_cl_count; j; j--, face_cl_data++) {
                face_cl_data->frame = s.read_float_t_reverse_endianness();
                face_cl_data->type = s.read_int16_t_reverse_endianness();
                face_cl_data->id = s.read_int16_t_reverse_endianness();
                face_cl_data->value = s.read_float_t_reverse_endianness();
                face_cl_data->trans = s.read_float_t_reverse_endianness();
            }
            s.position_pop();
        }

        if (name_offset[i])
            mot.name = pv_exp_read_utf8_string_null_terminated_offset(alloc, s, name_offset[i]);
    }

    free_def(offsets);

    exp->ready = true;
    exp->modern = true;
    exp->big_endian = big_endian;
    exp->is_x = is_x;
}

static void pv_exp_modern_write_inner(pv_exp* exp, stream& s) {
    bool big_endian = exp->big_endian;
    bool is_x = exp->is_x;
    uint32_t motion_num = exp->motion_num;

    memory_stream s_expc;
    s_expc.open();
    s_expc.big_endian = big_endian;

    s_expc.write_uint32_t_reverse_endianness(0x64);
    s_expc.write_uint32_t_reverse_endianness(motion_num);
    s_expc.write_offset(is_x ? 0x28 : 0x20, 0x20, is_x);

    s_expc.position_push(s_expc.get_position(), SEEK_SET);
    s_expc.write_offset(0x00, 0x20, is_x);

    s_expc.write_offset(0x00, 0x20, is_x);
    s_expc.write_offset(0x00, 0x20, is_x);

    if (!is_x)
        for (uint32_t i = 0; i < motion_num; i++) {
            s_expc.write_offset_f2(0x00, 0x20);
            s_expc.write_offset_f2(0x00, 0x20);
            s_expc.write_offset_f2(0x00, 0x20);
        }
    else
        for (uint32_t i = 0; i < motion_num; i++) {
            s_expc.write_offset_x(0x00);
            s_expc.write_offset_x(0x00);
            s_expc.write_offset_x(0x00);
        }
    s_expc.align_write(0x20);

    int64_t* offsets = force_malloc<int64_t>(motion_num * 3ULL);
    int64_t* face_offset = offsets;
    int64_t* face_cl_offset = offsets + motion_num;
    int64_t* name_offset = offsets + motion_num * 2ULL;

    pv_exp_mot* motion_data = exp->motion_data;
    for (uint32_t i = 0; i < motion_num; i++) {
        pv_exp_mot& mot = motion_data[i];
        face_offset[i] = s_expc.get_position();

        pv_exp_data* face_data = mot.face_data;
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

        face_cl_offset[i] = s_expc.get_position();

        pv_exp_data* face_cl_data = mot.face_cl_data;
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

    for (uint32_t i = 0; i < motion_num; i++) {
        name_offset[i] = s_expc.get_position();
        s_expc.write_utf8_string_null_terminated(motion_data[i].name);
    }
    s_expc.align_write(0x10);

    s_expc.set_position(is_x ? 0x28 : 0x20, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < motion_num; i++) {
            s_expc.write_offset_f2(face_offset[i], 0x20);
            s_expc.write_offset_f2(face_cl_offset[i], 0x20);
        }
    else
        for (uint32_t i = 0; i < motion_num; i++) {
            s_expc.write_offset_x(face_offset[i]);
            s_expc.write_offset_x(face_cl_offset[i]);
        }

    int64_t name_offsets_offset = s_expc.get_position();
    if (!is_x)
        for (uint32_t i = 0; i < motion_num; i++)
            s_expc.write_offset_f2(name_offset[i], 0x20);
    else
        for (uint32_t i = 0; i < motion_num; i++)
            s_expc.write_offset_x(name_offset[i]);

    free_def(offsets);

    s_expc.position_pop();

    s_expc.write_offset(name_offsets_offset, 0x20, is_x);

    f2_struct st;
    s_expc.align_write(0x10);
    s_expc.copy(st.data);
    s_expc.close();

    new (&st.header) f2_header('EXPC');
    st.header.attrib.set_big_endian(big_endian);

    st.write(s, true, exp->is_x);
}

inline static const char* pv_exp_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset) {
    size_t len = s.read_utf8_string_null_terminated_offset_length(offset);
    char* str = alloc->allocate<char>(len + 1);
    s.position_push(offset, SEEK_SET);
    s.read(str, len);
    s.position_pop();
    str[len] = 0;
    return str;
}
