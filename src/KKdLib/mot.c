/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.h"
#include "f2_struct.h"
#include "io_stream.h"
#include "msgpack.h"
#include "io_json.h"
#include "io_msgpack.h"
#include "str_utils.h"

mot* mot_init() {
    mot* m = force_malloc(sizeof(mot));
    return m;
}

void mot_read_mot(mot* m, char* path) {
    if (!m || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mot(m, path_buf);
    free(path_buf);
}

void mot_wread_mot(mot* m, wchar_t* path) {
    if (!m || !path)
        return;

    memset(m, 0, sizeof(mot));
    wchar_t* path_mot = str_utils_wadd(path, L".mot");
    stream* s = io_wopen(path_mot, L"rb");
    if (s->io.stream) {
        size_t count = io_read_uint64_t(s);
        /*size_t data_length =*/ io_read_uint64_t(s);
        /*size_t data_offset =*/ io_read_uint64_t(s);
        m->length = count;
        m->data = force_malloc_s(mot_struct, count);
        io_read(s, m->data, sizeof(mot_struct) * count);
        m->ready = true;
    }
    io_dispose(s);
    free(path_mot);
}

void mot_write_mot(mot* m, char* path) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mot(m, path_buf);
    free(path_buf);
}

void mot_wwrite_mot(mot* m, wchar_t* path) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_mot = str_utils_wadd(path, L".mot");
    stream* s = io_wopen(path_mot, L"wb");
    if (s->io.stream) {
        size_t count = m->length;
        io_write_uint64_t(s, count);
        io_write_uint64_t(s, count * 0x20);
        io_write_uint64_t(s, 0x18);
        io_write(s, m->data, sizeof(mot_struct) * count);
    }
    io_dispose(s);
    free(path_mot);
}

void mot_read_mp(mot* m, char* path, bool json) {
    if (!m || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mp(m, path_buf, json);
    free(path_buf);
}

void mot_wread_mp(mot* m, wchar_t* path, bool json) {
    if (!m || !path)
        return;

    memset(m, 0, sizeof(mot));
    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"rb");
    msgpack* msg;
    if (json)
        msg = io_json_read(s);
    else
        msg = io_msgpack_read(s);

    msgpack* mot = msgpack_read(msg, L"AddParam");
    if (mot && mot->type == MSGPACK_ARRAY) {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, mot);
        m->length = ptr->length;
        m->data = force_malloc_s(mot_struct, m->length);
        for (size_t i = 0; i < m->length; i++) {
            if (ptr->data[i].type != MSGPACK_MAP)
                continue;

            msgpack* _m = &ptr->data[i];
            mot_struct* mots = &m->data[i];
            mots->time = msgpack_read_float_t(_m, L"Time");
            mots->flags = msgpack_read_int32_t(_m, L"Flags");
            mots->frame = msgpack_read_int32_t(_m, L"Frame");
            mots->pv_branch = msgpack_read_int32_t(_m, L"PVBranch");
            mots->id = msgpack_read_int32_t(_m, L"ID");
            mots->value = msgpack_read_int32_t(_m, L"Value");
        }
        m->ready = true;
    }
    msgpack_dispose(msg);
    io_dispose(s);
    free(path_mp);
}

void mot_write_mp(mot* m, char* path, bool json) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mp(m, path_buf, json);
    free(path_buf);
}

void mot_wwrite_mp(mot* m, wchar_t* path, bool json) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"wb");
    if (s->io.stream) {
        msgpack* msg = msgpack_init_map(0);
        msgpack* mot = msgpack_init_array(L"AddParam", m->length);
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, mot);
        for (size_t i = 0; i < m->length; i++) {
            msgpack* _m = &ptr->data[i];
            mot_struct* mots = &m->data[i];
            msgpack_set_map_empty(_m, 0);
            msgpack_append_float_t(_m, L"Time", mots->time);
            msgpack_append_int32_t(_m, L"Flags", mots->flags);
            msgpack_append_int32_t(_m, L"Frame", mots->frame);
            if (mots->pv_branch)
                msgpack_append_int32_t(_m, L"PVBranch", mots->pv_branch);
            msgpack_append_int32_t(_m, L"ID", mots->id);
            msgpack_append_int32_t(_m, L"Value", mots->value);
        }
        msgpack_append(msg, mot);
        msgpack_dispose(mot);
        if (json)
            io_json_write(s, msg);
        else
            io_msgpack_write(s, msg);
        msgpack_dispose(msg);
    }
    io_dispose(s);
    free(path_mp);
}

void mot_dispose(mot* m) {
    if (!m)
        return;

    free(m->data);
    free(m);
}
