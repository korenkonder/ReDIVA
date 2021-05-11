/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.h"

#define ALLOCATE_MSGPACK(a, b) \
if (CHECK_MSGPACK(a)) \
    ((a*)##b->ptr) = force_malloc(sizeof(a));

static void msgpack_dispose_inner(msgpack* msg);

msgpack* msgpack_init_map(wchar_t* name) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK(msgpack_map, msg);
    wchar_t_buffer_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    ptr->length = 0;
    ptr->fulllength = 1;
    ptr->data = force_malloc_s(msgpack, 1);
    return msg;
}

msgpack* msgpack_init_array(wchar_t* name, size_t length) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_ARRAY;
    ALLOCATE_MSGPACK(msgpack_array, msg);
    wchar_t_buffer_init(&msg->name, name);

    msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
    ptr->length = ptr->fulllength = length;
    ptr->data = force_malloc_s(msgpack, length);
    return msg;
}

msgpack* msgpack_init_null(wchar_t* name) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_NULL;
    wchar_t_buffer_init(&msg->name, name);
    return msg;
}

msgpack* msgpack_init_bool(wchar_t* name, bool val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_BOOL;
    ALLOCATE_MSGPACK(bool, msg);
    wchar_t_buffer_init(&msg->name, name);

    bool* ptr = SELECT_MSGPACK(bool, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_int8_t(wchar_t* name, int8_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_INT8;
    ALLOCATE_MSGPACK(int8_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int8_t* ptr = SELECT_MSGPACK(int8_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_uint8_t(wchar_t* name, uint8_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_UINT8;
    ALLOCATE_MSGPACK(uint8_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint8_t* ptr = SELECT_MSGPACK(uint8_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_int16_t(wchar_t* name, int16_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_INT16;
    ALLOCATE_MSGPACK(int16_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int16_t* ptr = SELECT_MSGPACK(int16_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_uint16_t(wchar_t* name, uint16_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_UINT16;
    ALLOCATE_MSGPACK(uint16_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint16_t* ptr = SELECT_MSGPACK(uint16_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_int32_t(wchar_t* name, int32_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_INT32;
    ALLOCATE_MSGPACK(int32_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int32_t* ptr = SELECT_MSGPACK(int32_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_uint32_t(wchar_t* name, uint32_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_UINT32;
    ALLOCATE_MSGPACK(uint32_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint32_t* ptr = SELECT_MSGPACK(uint32_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_int64_t(wchar_t* name, int64_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_INT64;
    ALLOCATE_MSGPACK(int64_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int64_t* ptr = SELECT_MSGPACK(int64_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_uint64_t(wchar_t* name, uint64_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_UINT64;
    ALLOCATE_MSGPACK(uint64_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint64_t* ptr = SELECT_MSGPACK(uint64_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_float_t(wchar_t* name, float_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_FLOAT;
    ALLOCATE_MSGPACK(float_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    float_t* ptr = SELECT_MSGPACK(float_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_double_t(wchar_t* name, double_t val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_DOUBLE;
    ALLOCATE_MSGPACK(double_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    double_t* ptr = SELECT_MSGPACK(double_t, msg);
    *ptr = val;
    return msg;
}

msgpack* msgpack_init_string(wchar_t* name, wchar_t* val) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK(wchar_t_buffer, msg);
    wchar_t_buffer_init(&msg->name, name);
    wchar_t_buffer_init(SELECT_MSGPACK(wchar_t_buffer, msg), val);
    return msg;
}

bool msgpack_check_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return SELECT_MSGPACK(msgpack_array, msg)->data == 0;
    else if (msg->type == MSGPACK_MAP)
        return SELECT_MSGPACK(msgpack_map, msg)->data == 0;
    return msg->type == MSGPACK_NONE;
}

bool msgpack_check_not_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return SELECT_MSGPACK(msgpack_array, msg)->data != 0;
    else if (msg->type == MSGPACK_MAP)
        return SELECT_MSGPACK(msgpack_map, msg)->data != 0;
    return msg->type != MSGPACK_NONE;
}

msgpack* msgpack_get_by_index(msgpack* msg, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return 0;

    msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
    if (index < ptr->length)
        return &ptr->data[index];
    return 0;
}

void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return;

    msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
    if (index < ptr->length) {
        msgpack_dispose_inner(&ptr->data[index]);
        ptr->data[index] = *m;
    }
}

msgpack* msgpack_get_by_name(msgpack* msg, wchar_t* name) {
    if (msg->type != MSGPACK_MAP)
        return 0;

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    for (size_t i = 0; i < ptr->length; i++)
        if (!wcscmp(wchar_t_buffer_select(&ptr->data[i].name), name))
            return &ptr->data[i];

    return 0;
}

void msgpack_set_by_name(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    for (size_t i = 0; i < ptr->length; i++)
        if (!wcscmp(wchar_t_buffer_select(&ptr->data[i].name), wchar_t_buffer_select(&m->name))) {
            msgpack_dispose_inner(&ptr->data[i]);
            ptr->data[i] = *m;
            return;
        }

    msgpack_append(msg, m);
}

void msgpack_append(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    char_append_data_s(sizeof(msgpack), (char**)&ptr->data, &ptr->length, &ptr->fulllength, (char*)m, 1);
    memset(m, 0, sizeof(msgpack));
}

void msgpack_append_bool(msgpack* msg, wchar_t* name, bool val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_bool(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_int8_t(msgpack* msg, wchar_t* name, int8_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_int8_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_uint8_t(msgpack* msg, wchar_t* name, uint8_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_uint8_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_int16_t(msgpack* msg, wchar_t* name, int16_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_int16_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_uint16_t(msgpack* msg, wchar_t* name, uint16_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_uint16_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_int32_t(msgpack* msg, wchar_t* name, int32_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_int32_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_uint32_t(msgpack* msg, wchar_t* name, uint32_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_uint32_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_int64_t(msgpack* msg, wchar_t* name, int64_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_int64_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_uint64_t(msgpack* msg, wchar_t* name, uint64_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_uint64_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_float_t(msgpack* msg, wchar_t* name, float_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_float_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_double_t(msgpack* msg, wchar_t* name, double_t val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_double_t(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_append_string(msgpack* msg, wchar_t* name, wchar_t* val) {
    msgpack m;
    memset(&m, 0, sizeof(m));
    msgpack_set_string(&m, name, val);
    msgpack_append(msg, &m);
}

void msgpack_set_null(msgpack* msg, wchar_t* name) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_NULL;
    wchar_t_buffer_init(&msg->name, name);
}

void msgpack_set_array(msgpack* msg, wchar_t* name, msgpack_array* val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_ARRAY;
    ALLOCATE_MSGPACK(msgpack_array, msg);
    wchar_t_buffer_init(&msg->name, name);

    msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
    *ptr = *val;
}

void msgpack_set_map(msgpack* msg, wchar_t* name, msgpack_map* val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK(msgpack_map, msg);
    wchar_t_buffer_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    *ptr = *val;
}

void msgpack_set_map_empty(msgpack* msg, wchar_t* name) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK(msgpack_map, msg);
    wchar_t_buffer_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
    ptr->length = 0;
    ptr->fulllength = 1;
    ptr->data = force_malloc_s(msgpack, 1);
}

void msgpack_set_bool(msgpack* msg, wchar_t* name, bool val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_BOOL;
    ALLOCATE_MSGPACK(bool, msg);
    wchar_t_buffer_init(&msg->name, name);

    bool* ptr = SELECT_MSGPACK(bool, msg);
    *ptr = val;
}

void msgpack_set_int8_t(msgpack* msg, wchar_t* name, int8_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_INT8;
    ALLOCATE_MSGPACK(int8_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int8_t* ptr = SELECT_MSGPACK(int8_t, msg);
    *ptr = val;
}

void msgpack_set_uint8_t(msgpack* msg, wchar_t* name, uint8_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_UINT8;
    ALLOCATE_MSGPACK(uint8_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint8_t* ptr = SELECT_MSGPACK(uint8_t, msg);
    *ptr = val;
}

void msgpack_set_int16_t(msgpack* msg, wchar_t* name, int16_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_INT16;
    ALLOCATE_MSGPACK(int16_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int16_t* ptr = SELECT_MSGPACK(int16_t, msg);
    *ptr = val;
}

void msgpack_set_uint16_t(msgpack* msg, wchar_t* name, uint16_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_UINT16;
    ALLOCATE_MSGPACK(uint16_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint16_t* ptr = SELECT_MSGPACK(uint16_t, msg);
    *ptr = val;
}

void msgpack_set_int32_t(msgpack* msg, wchar_t* name, int32_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_INT32;
    ALLOCATE_MSGPACK(int32_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int32_t* ptr = SELECT_MSGPACK(int32_t, msg);
    *ptr = val;
}

void msgpack_set_uint32_t(msgpack* msg, wchar_t* name, uint32_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_UINT32;
    ALLOCATE_MSGPACK(uint32_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint32_t* ptr = SELECT_MSGPACK(uint32_t, msg);
    *ptr = val;
}

void msgpack_set_int64_t(msgpack* msg, wchar_t* name, int64_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_INT64;
    ALLOCATE_MSGPACK(int64_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    int64_t* ptr = SELECT_MSGPACK(int64_t, msg);
    *ptr = val;
}

void msgpack_set_uint64_t(msgpack* msg, wchar_t* name, uint64_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_UINT64;
    ALLOCATE_MSGPACK(uint64_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    uint64_t* ptr = SELECT_MSGPACK(uint64_t, msg);
    *ptr = val;
}

void msgpack_set_float_t(msgpack* msg, wchar_t* name, float_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_FLOAT;
    ALLOCATE_MSGPACK(float_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    float_t* ptr = SELECT_MSGPACK(float_t, msg);
    *ptr = val;
}

void msgpack_set_double_t(msgpack* msg, wchar_t* name, double_t val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_DOUBLE;
    ALLOCATE_MSGPACK(double_t, msg);
    wchar_t_buffer_init(&msg->name, name);

    double_t* ptr = SELECT_MSGPACK(double_t, msg);
    *ptr = val;
}

void msgpack_set_string(msgpack* msg, wchar_t* name, wchar_t* val) {
    if (!msg)
        return;

    msgpack_dispose_inner(msg);
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK(wchar_t_buffer, msg);
    wchar_t_buffer_init(&msg->name, name);
    wchar_t_buffer_init(SELECT_MSGPACK(wchar_t_buffer, msg), val);
}

msgpack* msgpack_read(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    return name ? msgpack_get_by_name(msg, name) : msg;
}

bool msgpack_read_bool(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (m && m->type == MSGPACK_BOOL)
        return *SELECT_MSGPACK(bool, m);
    return 0;
}

int8_t msgpack_read_int8_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return (int8_t)*SELECT_MSGPACK(uint8_t, m);
    return 0;
}

uint8_t msgpack_read_uint8_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint8_t)*SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    return 0;
}

int16_t msgpack_read_int16_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return (int16_t)*SELECT_MSGPACK(uint16_t, m);
    return 0;
}

uint16_t msgpack_read_uint16_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint16_t)(int16_t)*SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint16_t)(int16_t)*SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    return 0;
}

int32_t msgpack_read_int32_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (int32_t)*SELECT_MSGPACK(uint32_t, m);
    return 0;
}

uint32_t msgpack_read_uint32_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint32_t)(int32_t)*SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint32_t)(int32_t)*SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint32_t)(int32_t)*SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK(uint32_t, m);
    return 0;
}

int64_t msgpack_read_int64_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return *SELECT_MSGPACK(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (int64_t)*SELECT_MSGPACK(uint64_t, m);
    return 0;
}

uint64_t msgpack_read_uint64_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint64_t)(int64_t)*SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint64_t)(int64_t)*SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)(int64_t)*SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK(uint32_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)*SELECT_MSGPACK(int64_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK(uint64_t, m);
    return 0;
}

float_t msgpack_read_float_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (float_t)*SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (float_t)*SELECT_MSGPACK(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (float_t)*SELECT_MSGPACK(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (float_t)*SELECT_MSGPACK(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *SELECT_MSGPACK(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return (float_t)*SELECT_MSGPACK(double_t, m);
    return 0;
}

double_t msgpack_read_double_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (double_t)*SELECT_MSGPACK(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (double_t)*SELECT_MSGPACK(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *SELECT_MSGPACK(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return *SELECT_MSGPACK(double_t, m);
    return 0;
}

wchar_t* msgpack_read_string(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        wchar_t_buffer* ptr = SELECT_MSGPACK(wchar_t_buffer, m);
        wchar_t* val = force_malloc_s(wchar_t, ptr->length + 1);
        memcpy(val, wchar_t_buffer_select(ptr), sizeof(wchar_t) * (ptr->length + 1));
        return val;
    }
    return 0;
}

void msgpack_dispose(msgpack* msg) {
    if (!msg)
        return;

    switch (msg->type) {
    case MSGPACK_STRING:
        wchar_t_buffer_dispose(SELECT_MSGPACK(wchar_t_buffer, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
        for (size_t i = 0; i < ptr->length; i++)
            msgpack_dispose_inner(&ptr->data[i]);
        free(ptr->data);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
        for (size_t i = 0; i < ptr->length; i++)
            msgpack_dispose_inner(&ptr->data[i]);
        free(ptr->data);
    } break;
    }

    if (msg->type != MSGPACK_ARRAY && msg->type != MSGPACK_MAP && msg->type == MSGPACK_NONE)
        memset(msg, 0, sizeof(msgpack));
    else
        free(msg);
}

static void msgpack_dispose_inner(msgpack* msg) {
    if (!msg)
        return;

    wchar_t_buffer_dispose(&msg->name);

    switch (msg->type) {
    case MSGPACK_STRING:
        wchar_t_buffer_dispose(SELECT_MSGPACK(wchar_t_buffer, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
        for (size_t i = 0; i < ptr->length; i++)
            msgpack_dispose_inner(&ptr->data[i]);
        free(ptr->data);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
        for (size_t i = 0; i < ptr->length; i++)
            msgpack_dispose_inner(&ptr->data[i]);
        free(ptr->data);
    } break;
    }
    memset(msg, 0, sizeof(msgpack));
}
