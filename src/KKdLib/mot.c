/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.h"
#include "f2_struct.h"
#include "half_t.h"
#include "hash.h"
#include "io_path.h"
#include "io_stream.h"
#include "str_utils.h"

typedef struct mot_header_classic {
    uint32_t key_set_count_offset;
    uint32_t key_set_types_offset;
    uint32_t key_set_offset;
    uint32_t bone_info_offset;
} mot_header_classic;

typedef struct mot_header_modern {
    uint32_t hash;
    int64_t name_offset;
    int64_t key_set_count_offset;
    int64_t key_set_types_offset;
    int64_t key_set_offset;
    int64_t bone_info_offset;
    int64_t bone_hash_offset;
    int32_t bone_info_count;
} mot_header_modern;

vector_func(mot)

static void mot_read_inner(mot_set* ms, stream* s, bool modern);
static void mot_write_inner(mot_set* ms, stream* s);

mot_set* mot_init() {
    mot_set* ms = force_malloc(sizeof(mot_set));
    memset(ms, 0, sizeof(mot_set));
    return ms;
}

void mot_read(mot_set* ms, char* path, bool modern) {
    if (!ms || !path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream)
                mot_read_inner(ms, &s, false);
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_mot = str_utils_add(path, ".mot");
        if (path_check_file_exists(path_mot)) {
            stream s;
            io_open(&s, path_mot, "rb");
            if (s.io.stream)
                mot_read_inner(ms, &s, true);
            io_free(&s);
        }
        free(path_mot);
    }
}

void mot_wread(mot_set* ms, wchar_t* path, bool modern) {
    if (!ms || !path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_wcheck_file_exists(path_bin)) {
            stream s;
            io_wopen(&s, path_bin, L"rb");
            if (s.io.stream)
                mot_read_inner(ms, &s, false);
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_mot = str_utils_wadd(path, L".mot");
        if (path_wcheck_file_exists(path_mot)) {
            stream s;
            io_wopen(&s, path_mot, L"rb");
            if (s.io.stream)
                mot_read_inner(ms, &s, true);
            io_free(&s);
        }
        free(path_mot);
    }
}

void mot_mread(mot_set* ms, void* data, size_t length, bool modern) {
    if (!ms || !data)
        return;

    stream s;
    io_mopen(&s, data, length);
    mot_read_inner(ms, &s, modern);
    io_free(&s);
}

void mot_write(mot_set* ms, char* path) {
    if (!ms || !path || !ms->ready)
        return;

    if (!ms->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            mot_write_inner(ms, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_mot = str_utils_add(path, ".mot");
        stream s;
        io_open(&s, path_mot, "wb");
        if (s.io.stream)
            mot_write_inner(ms, &s);
        io_free(&s);
        free(path_mot);
    }
}

void mot_wwrite(mot_set* ms, wchar_t* path) {
    if (!ms || !path || !ms->ready)
        return;

    if (!ms->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            mot_write_inner(ms, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_mot = str_utils_wadd(path, L".mot");
        stream s;
        io_wopen(&s, path_mot, L"wb");
        if (s.io.stream)
            mot_write_inner(ms, &s);
        io_free(&s);
        free(path_mot);
    }
}

void mot_mwrite(mot_set* ms, void** data, size_t* length) {
    if (!ms || !data)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    mot_write_inner(ms, &s);

    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

void mot_dispose(mot_set* ms) {
    if (!ms)
        return;

    for (mot* i = ms->vec.begin; i != ms->vec.end; i++) {
        if (ms->modern)
            if (i->bone_info)
                for (size_t j = 0; j < i->bone_info_count; j++)
                    string_free(&i->bone_info[j].name);
        free(i->bone_info);

        if (i->key_set)
            for (size_t j = 0; j < i->key_set_count; j++)
                free(i->key_set[j].keys);
        free(i->key_set);
        string_free(&i->name);
    }
    vector_mot_free(&ms->vec);
    free(ms);
}

static void mot_read_inner(mot_set* ms, stream* s, bool modern) {
    if (!modern) {
        size_t count = 0;
        while (io_read_uint64_t(s) != 0) {
            io_read_uint64_t(s);
            count++;
        }

        if (count == 0) {
            ms->is_x = false;
            ms->modern = false;
            ms->ready = false;
            return;
        }

        io_set_position(s, 0x00, SEEK_SET);
        vector_mot_reserve(&ms->vec, count);
        mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
        for (size_t i = 0; i < count; i++) {
            mh[i].key_set_count_offset = io_read_uint32_t(s);
            mh[i].key_set_types_offset = io_read_uint32_t(s);
            mh[i].key_set_offset = io_read_uint32_t(s);
            mh[i].bone_info_offset = io_read_uint32_t(s);
        }

        for (size_t i = 0; i < count; i++) {
            mot m;
            memset(&m, 0, sizeof(mot));

            io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
            m.bone_info_count = 0;
            io_read_uint16_t(s);
            do
                m.bone_info_count++;
            while (io_read_uint16_t(s) != 0 && io_get_position(s) < s->length);

            io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
            m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
            for (size_t j = 0; j < m.bone_info_count; j++)
                m.bone_info[j].index = io_read_uint16_t(s);

            io_set_position(s, mh[i].key_set_count_offset, SEEK_SET);
            m.info = io_read_uint16_t(s);
            m.frame_count = io_read_uint16_t(s);

            m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
            io_set_position(s, mh[i].key_set_types_offset, SEEK_SET);
            for (int32_t j = 0, b = 0; j < m.key_set_count; j++) {
                if (j % 8 == 0)
                    b = io_read_uint16_t(s);

                m.key_set[j].type = (b >> (j % 8 * 2)) & 0x03;
            }

            io_set_position(s, mh[i].key_set_offset, SEEK_SET);
            for (int32_t j = 0; j < m.key_set_count; j++) {
                mot_key_set* mks = &m.key_set[j];
                if (mks->type == MOT_KEY_SET_NONE) {
                    mks->keys_count = 0;
                    mks->keys = 0;
                }
                else if (mks->type == MOT_KEY_SET_STATIC) {
                    mks->keys_count = 1;
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    mks->keys[0].frame = 0.0f;
                    mks->keys[0].value = io_read_float_t(s);
                }
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    mks->keys_count = io_read_uint16_t(s);
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    for (uint32_t k = 0; k < mks->keys_count; k++)
                        mks->keys[k].frame = io_read_uint16_t(s);
                    io_align_read(s, 0x04);

                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (uint32_t k = 0; k < mks->keys_count; k++) {
                            mks->keys[k].value = io_read_float_t(s);
                            mks->keys[k].tangent = io_read_float_t(s);
                        }
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++) {
                            mks->keys[k].value = io_read_float_t(s);
                            mks->keys[k].tangent = 0.0f;
                        }
                }
            }
            vector_mot_push_back(&ms->vec, &m);
        }
        free(mh);

        ms->is_x = false;
        ms->modern = false;
        ms->ready = true;
    }
    else {
        f2_struct st;
        f2_struct_sread(&st, s);
        if (st.header.signature == reverse_endianness_uint32_t('MOTC') && st.data) {
            stream s_motc;
            io_mopen(&s_motc, st.data, st.length);
            s_motc.is_big_endian = st.header.use_big_endian;

            io_set_position(&s_motc, 0x0C, SEEK_SET);
            bool is_x = io_read_uint32_t_stream_reverse_endianness(&s_motc) == 0;

            io_set_position(&s_motc, 0x00, SEEK_SET);
            vector_mot_reserve(&ms->vec, 1);
            mot_header_modern mh;
            memset(&mh, 0, sizeof(mot_header_modern));
            if (!is_x) {
                mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(&s_motc);
                mh.name_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.key_set_count_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.key_set_types_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.key_set_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.bone_info_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.bone_hash_offset = io_read_offset_f2(&s_motc, st.header.length);
                mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(&s_motc);
            }
            else {
                mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(&s_motc);
                mh.name_offset = io_read_offset_x(&s_motc);
                mh.key_set_count_offset = io_read_offset_x(&s_motc);
                mh.key_set_types_offset = io_read_offset_x(&s_motc);
                mh.key_set_offset = io_read_offset_x(&s_motc);
                mh.bone_info_offset = io_read_offset_x(&s_motc);
                mh.bone_hash_offset = io_read_offset_x(&s_motc);
                mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(&s_motc);
            }

            mot m;
            memset(&m, 0, sizeof(mot));
            m.div_frames = io_read_uint16_t_stream_reverse_endianness(&s_motc);;
            m.div_count = io_read_uint8_t(&s_motc);;

            io_read_string_null_terminated_offset(&s_motc, mh.name_offset, &m.name);

            io_set_position(&s_motc, mh.bone_info_offset, SEEK_SET);
            m.bone_info_count = mh.bone_info_count;
            m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
            if (!is_x)
                for (size_t j = 0; j < mh.bone_info_count; j++)
                    io_read_string_null_terminated_offset(&s_motc,
                        io_read_offset_f2(&s_motc, st.header.length), &m.bone_info[j].name);
            else
                for (size_t j = 0; j < mh.bone_info_count; j++)
                    io_read_string_null_terminated_offset(&s_motc,
                        io_read_offset_x(&s_motc), &m.bone_info[j].name);

            io_set_position(&s_motc, mh.bone_hash_offset, SEEK_SET);
            for (size_t j = 0; j < mh.bone_info_count; j++)
                io_read_uint64_t_stream_reverse_endianness(&s_motc);

            io_set_position(&s_motc, mh.key_set_count_offset, SEEK_SET);
            m.info = io_read_uint16_t_stream_reverse_endianness(&s_motc);
            m.frame_count = io_read_uint16_t_stream_reverse_endianness(&s_motc);

            m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
            io_set_position(&s_motc, mh.key_set_types_offset, SEEK_SET);
            for (int32_t j = 0, b = 0; j < m.key_set_count; j++) {
                if (j % 8 == 0)
                    b = io_read_uint16_t_stream_reverse_endianness(&s_motc);

                m.key_set[j].type = (b >> (j % 8 * 2)) & 0x03;
            }

            io_set_position(&s_motc, mh.key_set_offset, SEEK_SET);
            for (int32_t j = 0; j < m.key_set_count; j++) {
                mot_key_set* mks = &m.key_set[j];
                if (mks->type == MOT_KEY_SET_NONE) {
                    mks->keys_count = 0;
                    mks->keys = 0;
                }
                else if (mks->type == MOT_KEY_SET_STATIC) {
                    mks->keys_count = 1;
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    mks->keys[0].frame = 0.0f;
                    mks->keys[0].value = io_read_float_t_stream_reverse_endianness(&s_motc);
                }
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    mks->keys_count = io_read_uint16_t_stream_reverse_endianness(&s_motc);
                    mks->data_type = io_read_uint16_t_stream_reverse_endianness(&s_motc);
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].tangent = io_read_float_t_stream_reverse_endianness(&s_motc);
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].tangent = 0.0f;

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].value = half_to_float(io_read_half_t_stream_reverse_endianness(&s_motc));
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].value = io_read_float_t_stream_reverse_endianness(&s_motc);
                    io_align_read(&s_motc, 0x04);

                    for (uint32_t k = 0; k < mks->keys_count; k++)
                        mks->keys[k].frame = (float_t)io_read_uint16_t_stream_reverse_endianness(&s_motc);
                    io_align_read(&s_motc, 0x04);
                }
            }

            m.murmurhash = st.header.murmurhash;
            vector_mot_push_back(&ms->vec, &m);

            io_free(&s_motc);

            ms->is_x = is_x;
            ms->modern = true;
            ms->ready = true;
        }
        f2_struct_free(&st);
    }
}

static void mot_write_inner(mot_set* ms, stream* s) {
    if (!ms->modern) {
        size_t count = ms->vec.end - ms->vec.begin;
        io_set_position(s, count * 0x10 + 0x10, SEEK_SET);
        mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
        for (size_t i = 0; i < count; i++) {
            mot* m = &ms->vec.begin[i];

            mh[i].key_set_count_offset = (uint32_t)io_get_position(s);
            io_write_uint16_t(s, m->info);
            io_write_uint16_t(s, m->frame_count);

            mh[i].key_set_types_offset = (uint32_t)io_get_position(s);
            uint16_t key_set_type_buf = 0;
            for (int32_t j = 0; j < m->key_set_count; j++) {
                key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

                if (j % 8 == 7) {
                    io_write_uint16_t(s, key_set_type_buf);
                    key_set_type_buf = 0;
                }
            }

            if (m->key_set_count % 8 != 0)
                io_write_uint16_t(s, key_set_type_buf);
            io_align_write(s, 0x04);

            mh[i].key_set_offset = (uint32_t)io_get_position(s);
            for (int32_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_STATIC)
                    io_write_float_t(s, mks->keys[0].value);
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    io_write_uint16_t(s, (uint16_t)mks->keys_count);
                    for (size_t k = 0; k < mks->keys_count; k++)
                        io_write_uint16_t(s, (uint16_t)mks->keys[k].frame);

                    if (io_get_position(s) % 4 != 0)
                        io_write_uint16_t(s, 0x00);

                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (size_t k = 0; k < mks->keys_count; k++) {
                            io_write_float_t(s, mks->keys[k].value);
                            io_write_float_t(s, mks->keys[k].tangent);
                        }
                    else
                        for (size_t k = 0; k < mks->keys_count; k++)
                            io_write_float_t(s, mks->keys[k].value);
                }
            }
            io_align_write(s, 0x04);

            mh[i].bone_info_offset = (uint32_t)io_get_position(s);
            if (m->bone_info_count)
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_uint16_t(s, m->bone_info[j].index);
            else
                io_write_uint16_t(s, 0x00);
            io_write_uint16_t(s, 0x00);
        }
        io_align_write(s, 0x04);

        io_set_position(s, 0x00, SEEK_SET);
        for (size_t i = 0; i < count; i++) {
            io_write_uint32_t(s, mh[i].key_set_count_offset);
            io_write_uint32_t(s, mh[i].key_set_types_offset);
            io_write_uint32_t(s, mh[i].key_set_offset);
            io_write_uint32_t(s, mh[i].bone_info_offset);
        }
        free(mh);
    }
    else {
        stream s_motc;
        io_open(&s_motc, 0, 0);
        uint32_t o;
        vector_enrs_entry e = vector_empty(enrs_entry);
        enrs_entry ee;
        vector_size_t pof = vector_empty(size_t);
        uint32_t murmurhash = 0;
        if (ms->vec.end - ms->vec.begin > 0) {
            mot_header_modern mh;
            memset(&mh, 0, sizeof(mot_header_modern));
            mot* m = ms->vec.begin;

            if (!ms->is_x) {
                ee = (enrs_entry){ 0, 3, 48, 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 7, ENRS_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = 48;
            }
            else {
                ee = (enrs_entry){ 0, 3, 64, 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 7, ENRS_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = 64;
            };

            ee = (enrs_entry){ o, 1, 4, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_WORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = 4;

            ee = (enrs_entry){ o, 1,  (m->key_set_count + 3) / 4, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub,
                &(enrs_sub_entry){ 0, (m->key_set_count + 7) / 8, ENRS_WORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = (m->key_set_count + 3) / 4;
            o = align_val(o, 4);

            for (int32_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_STATIC) {
                    ee = (enrs_entry){ o, 1, 4, 1, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    o = 4;
                }
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        ee = (enrs_entry){ o, 1, 4, 1, vector_empty(enrs_sub_entry) };
                    else
                        ee = (enrs_entry){ o, 1, 3, 1, vector_empty(enrs_sub_entry) };

                    o = 4;
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        o += mks->keys_count * 4;

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        o += align_val(mks->keys_count * 2, 4);
                    else
                        o += mks->keys_count * 4;
                    o += align_val(mks->keys_count * 2, 4);
                    o = align_val(o, 4);
                    ee.size = o;

                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_WORD });
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ 0, mks->keys_count, ENRS_DWORD });
                    if (mks->data_type == MOT_KEY_SET_DATA_F16) {
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ 0, mks->keys_count, ENRS_WORD });
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ mks->keys_count % 2 == 1 ? 2 : 0, mks->keys_count, ENRS_WORD });
                    }
                    else {
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ 0, mks->keys_count, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ 0, mks->keys_count, ENRS_WORD });
                    }
                    vector_enrs_entry_push_back(&e, &ee);
                }
            }
            o = align_val(o, 16);

            if (!ms->is_x) {
                ee = (enrs_entry){ o, 1, m->bone_info_count * 4, 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = m->bone_info_count * 4;
            }
            else {
                ee = (enrs_entry){ o, 1, m->bone_info_count * 8, 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = m->bone_info_count * 8;
            }
            o = align_val(o, 16);

            ee = (enrs_entry){ o, 1, m->bone_info_count * 8, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = m->bone_info_count * 8;

            if (!ms->is_x) {
                io_write_uint64_t(&s_motc, 0);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
                io_write_int32_t(&s_motc, 0);
                io_write_int32_t(&s_motc, 0);
            }
            else {
                io_write_uint64_t(&s_motc, 0);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
                io_write_int32_t(&s_motc, 0);
            }
            io_write_uint16_t(&s_motc, 0);
            io_write_uint8_t(&s_motc, 0);
            io_align_write(&s_motc, 0x10);

            mh.hash = hash_utf8_murmurhash(string_data(&m->name), 0, false);

            mh.key_set_count_offset = io_get_position(&s_motc);
            io_write_uint16_t(&s_motc, m->info);
            io_write_uint16_t(&s_motc, m->frame_count);

            mh.key_set_types_offset = io_get_position(&s_motc);
            uint16_t key_set_type_buf = 0;
            for (int32_t j = 0; j < m->key_set_count; j++) {
                key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

                if (j % 8 == 7) {
                    io_write_uint16_t(&s_motc, key_set_type_buf);
                    key_set_type_buf = 0;
                }
            }

            if (m->key_set_count % 8 != 0)
                io_write_uint16_t(&s_motc, key_set_type_buf);
            io_align_write(&s_motc, 0x04);

            mh.key_set_offset = io_get_position(&s_motc);
            for (int32_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_STATIC)
                    io_write_float_t(&s_motc, mks->keys[0].value);
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    io_write_uint16_t(&s_motc, (uint16_t)mks->keys_count);
                    io_write_uint16_t(&s_motc, (uint16_t)mks->data_type);
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_float_t(&s_motc, mks->keys[k].tangent);

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_half_t(&s_motc, float_to_half(mks->keys[k].value));
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_float_t(&s_motc, mks->keys[k].value);
                    io_align_read(&s_motc, 0x04);

                    for (uint32_t k = 0; k < mks->keys_count; k++)
                        io_write_uint16_t(&s_motc, (uint16_t)mks->keys[k].frame);
                    io_align_read(&s_motc, 0x04);
                }
            }
            io_align_write(&s_motc, 0x10);

            size_t* bone_info_offsets = force_malloc_s(size_t, m->bone_info_count);
            mh.bone_info_offset = io_get_position(&s_motc);
            if (!ms->is_x)
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            else
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_align_write(&s_motc, 0x10);

            mh.bone_hash_offset = io_get_position(&s_motc);
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_uint64_t(&s_motc, hash_utf8_murmurhash(
                    string_data(&m->bone_info[j].name), 0, false));
            io_align_write(&s_motc, 0x10);

            mh.name_offset = io_get_position(&s_motc);
            io_write_string_null_terminated(&s_motc, &m->name);

            for (int32_t j = 0; j < m->bone_info_count; j++) {
                bone_info_offsets[j] = io_get_position(&s_motc);
                io_write_string_null_terminated(&s_motc, &m->bone_info[j].name);
            }
            io_align_write(&s_motc, 0x10);

            io_set_position(&s_motc, mh.bone_info_offset, SEEK_SET);
            mh.bone_info_count = m->bone_info_count;
            if (!ms->is_x)
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_offset_f2(&s_motc, bone_info_offsets[j], 0x40);
            else
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_offset_x(&s_motc, bone_info_offsets[j]);
            free(bone_info_offsets);

            io_set_position(&s_motc, 0x00, SEEK_SET);
            if (!ms->is_x) {
                io_write_uint64_t(&s_motc, (uint64_t)mh.hash);
                io_write_offset_f2(&s_motc, mh.name_offset, 0x40);
                io_write_offset_f2(&s_motc, mh.key_set_count_offset, 0x40);
                io_write_offset_f2(&s_motc, mh.key_set_types_offset, 0x40);
                io_write_offset_f2(&s_motc, mh.key_set_offset, 0x40);
                io_write_offset_f2(&s_motc, mh.bone_info_offset, 0x40);
                io_write_offset_f2(&s_motc, mh.bone_hash_offset, 0x40);
                io_write_int32_t(&s_motc, mh.bone_info_count);
            }
            else {
                io_write_uint64_t(&s_motc, (uint64_t)mh.hash);
                io_write_offset_x(&s_motc, mh.name_offset);
                io_write_offset_x(&s_motc, mh.key_set_count_offset);
                io_write_offset_x(&s_motc, mh.key_set_types_offset);
                io_write_offset_x(&s_motc, mh.key_set_offset);
                io_write_offset_x(&s_motc, mh.bone_info_offset);
                io_write_offset_x(&s_motc, mh.bone_hash_offset);
                io_write_int32_t(&s_motc, mh.bone_info_count);
            }

            if (m->div_frames > 0) {
                io_write_uint16_t(&s_motc, m->div_frames);
                io_write_uint8_t(&s_motc, m->div_count);
            }
            else {
                io_write_uint16_t(&s_motc, 0);
                io_write_uint8_t(&s_motc, 0);
            }

            murmurhash = m->murmurhash;
        }

        f2_struct st;
        memset(&st, 0, sizeof(f2_struct));

        io_align_write(&s_motc, 0x10);
        io_mcopy(&s_motc, &st.data, &st.length);
        io_free(&s_motc);

        st.enrs = e;
        st.pof = pof;

        st.header.signature = reverse_endianness_uint32_t('MOTC');
        st.header.length = 0x40;
        st.header.use_big_endian = false;
        st.header.use_section_size = true;
        st.header.murmurhash = murmurhash;
        st.header.inner_signature = 0xFF010008;

        f2_struct_swrite(&st, s, true, ms->is_x);
        f2_struct_free(&st);
    }
}
