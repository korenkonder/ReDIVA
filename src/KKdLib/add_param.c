/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "add_param.h"
#include "f2_struct.h"
#include "io_path.h"
#include "io_stream.h"
#include "msgpack.h"
#include "io_json.h"
#include "io_msgpack.h"

add_param* add_param_init() {
    add_param* a = force_malloc(sizeof(add_param));
    return a;
}

void add_param_dispose(add_param* a) {
    if (!a)
        return;

    free(a->data);
    free(a);
}

void add_param_read_adp(add_param* a, char* path) {
    if (!a || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    add_param_wread_adp(a, path_buf);
    free(path_buf);
}

void add_param_wread_adp(add_param* a, wchar_t* path) {
    if (!a || !path)
        return;

    memset(a, 0, sizeof(add_param));
    wchar_t* path_adp = path_wadd_extension(path, L".adp");
    stream* s = io_wopen(path_adp, L"rb");
    if (s->io.stream) {
        size_t count = io_read_uint64_t(s);
        size_t data_length = io_read_uint64_t(s);
        size_t data_offset = io_read_uint64_t(s);
        a->length = count;
        a->data = force_malloc_s(sizeof(add_param_struct), count);
        io_read(s, a->data, sizeof(add_param_struct) * count);
        a->ready = true;
    }
    io_dispose(s);
    free(path_adp);
}

void add_param_write_adp(add_param* a, char* path) {
    if (!a || !path || !a->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    add_param_wwrite_adp(a, path_buf);
    free(path_buf);
}

void add_param_wwrite_adp(add_param* a, wchar_t* path) {
    if (!a || !path || !a->ready)
        return;

    wchar_t* path_adp = path_wadd_extension(path, L".adp");
    stream* s = io_wopen(path_adp, L"wb");
    if (s->io.stream) {
        size_t count = a->length;
        io_write_uint64_t(s, count);
        io_write_uint64_t(s, count * 0x20);
        io_write_uint64_t(s, 0x18);
        io_write(s, a->data, sizeof(add_param_struct) * count);
    }
    io_dispose(s);
    free(path_adp);
}

void add_param_read_mp(add_param* a, char* path, bool json) {
    if (!a || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    add_param_wread_mp(a, path_buf, json);
    free(path_buf);
}

void add_param_wread_mp(add_param* a, wchar_t* path, bool json) {
    if (!a || !path)
        return;

    memset(a, 0, sizeof(add_param));
    wchar_t* path_mp = path_wadd_extension(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"rb");
    msgpack* msg;
    if (json)
        msg = io_json_read(s);
    else
        msg = io_msgpack_read(s);

    msgpack* add_param = msgpack_read(msg, L"AddParam");
    if (add_param && add_param->type == MSGPACK_ARRAY) {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, add_param);
        a->length = ptr->length;
        a->data = force_malloc_s(sizeof(add_param_struct), a->length);
        for (size_t i = 0; i < a->length; i++) {
            if (ptr->data[i].type != MSGPACK_MAP)
                continue;

            msgpack* m = &ptr->data[i];
            add_param_struct* adps = &a->data[i];
            adps->time = msgpack_read_float_t(m, L"Time");
            adps->flags = msgpack_read_int32_t(m, L"Flags");
            adps->frame = msgpack_read_int32_t(m, L"Frame");
            adps->pv_branch = msgpack_read_int32_t(m, L"PVBranch");
            adps->id = msgpack_read_int32_t(m, L"ID");
            adps->value = msgpack_read_int32_t(m, L"Value");
        }
        a->ready = true;
    }
    msgpack_dispose(msg);
    io_dispose(s);
    free(path_mp);
}

void add_param_write_mp(add_param* a, char* path, bool json) {
    if (!a || !path || !a->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    add_param_wwrite_mp(a, path_buf, json);
    free(path_buf);
}

void add_param_wwrite_mp(add_param* a, wchar_t* path, bool json) {
    if (!a || !path || !a->ready)
        return;

    wchar_t* path_mp = path_wadd_extension(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"wb");
    if (s->io.stream) {
        msgpack* msg = msgpack_init_map(0);
        msgpack* add_param = msgpack_init_array(L"AddParam", a->length);
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, add_param);
        for (size_t i = 0; i < a->length; i++) {
            msgpack* m = &ptr->data[i];
            add_param_struct* adps = &a->data[i];
            msgpack_set_map_empty(m, 0);
            msgpack_append_float_t(m, L"Time", adps->time);
            msgpack_append_int32_t(m, L"Flags", adps->flags);
            msgpack_append_int32_t(m, L"Frame", adps->frame);
            if (adps->pv_branch)
                msgpack_append_int32_t(m, L"PVBranch", adps->pv_branch);
            msgpack_append_int32_t(m, L"ID", adps->id);
            msgpack_append_int32_t(m, L"Value", adps->value);
        }
        msgpack_append(msg, add_param);
        msgpack_dispose(add_param);
        if (json)
            io_json_write(s, msg);
        else
            io_msgpack_write(s, msg);
        msgpack_dispose(msg);
    }
    io_dispose(s);
    free(path_mp);
}
