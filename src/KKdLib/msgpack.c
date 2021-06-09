/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.h"
#include "utf8.h"

#define ALLOCATE_MSGPACK(a, b) \
if (CHECK_MSGPACK(a)) \
    (b).ptr = force_malloc(sizeof(a));
#define ALLOCATE_MSGPACK_PTR(a, b) \
if (CHECK_MSGPACK(a)) \
    (b)->ptr = force_malloc(sizeof(a));

vector_func(msgpack);

void msgpack_init_map(msgpack* msg, wchar_t* name) {
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK_PTR(msgpack_map, msg);
    wstring_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_init_array(msgpack* msg, wchar_t* name, size_t length) {
    msg->type = MSGPACK_ARRAY;
    ALLOCATE_MSGPACK_PTR(msgpack_array, msg);
    wstring_init(&msg->name, name);

    msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
    vector_msgpack_append(ptr, length);
    ptr->end = ptr->begin + length;
}

void msgpack_init_null(msgpack* msg, wchar_t* name) {
    msg->type = MSGPACK_NULL;
    wstring_init(&msg->name, name);
}

void msgpack_init_bool(msgpack* msg, wchar_t* name, bool val) {
    msg->type = MSGPACK_BOOL;
    ALLOCATE_MSGPACK_PTR(bool, msg);
    wstring_init(&msg->name, name);

    bool* ptr = SELECT_MSGPACK_PTR(bool, msg);
    *ptr = val;
}

void msgpack_init_int8_t(msgpack* msg, wchar_t* name, int8_t val) {
    msg->type = MSGPACK_INT8;
    ALLOCATE_MSGPACK_PTR(int8_t, msg);
    wstring_init(&msg->name, name);

    int8_t* ptr = SELECT_MSGPACK_PTR(int8_t, msg);
    *ptr = val;
}

void msgpack_init_uint8_t(msgpack* msg, wchar_t* name, uint8_t val) {
    msg->type = MSGPACK_UINT8;
    ALLOCATE_MSGPACK_PTR(uint8_t, msg);
    wstring_init(&msg->name, name);

    uint8_t* ptr = SELECT_MSGPACK_PTR(uint8_t, msg);
    *ptr = val;
}

void msgpack_init_int16_t(msgpack* msg, wchar_t* name, int16_t val) {
    msg->type = MSGPACK_INT16;
    ALLOCATE_MSGPACK_PTR(int16_t, msg);
    wstring_init(&msg->name, name);

    int16_t* ptr = SELECT_MSGPACK_PTR(int16_t, msg);
    *ptr = val;
}

void msgpack_init_uint16_t(msgpack* msg, wchar_t* name, uint16_t val) {
    msg->type = MSGPACK_UINT16;
    ALLOCATE_MSGPACK_PTR(uint16_t, msg);
    wstring_init(&msg->name, name);

    uint16_t* ptr = SELECT_MSGPACK_PTR(uint16_t, msg);
    *ptr = val;
}

void msgpack_init_int32_t(msgpack* msg, wchar_t* name, int32_t val) {
    msg->type = MSGPACK_INT32;
    ALLOCATE_MSGPACK_PTR(int32_t, msg);
    wstring_init(&msg->name, name);

    int32_t* ptr = SELECT_MSGPACK_PTR(int32_t, msg);
    *ptr = val;
}

void msgpack_init_uint32_t(msgpack* msg, wchar_t* name, uint32_t val) {
    msg->type = MSGPACK_UINT32;
    ALLOCATE_MSGPACK_PTR(uint32_t, msg);
    wstring_init(&msg->name, name);

    uint32_t* ptr = SELECT_MSGPACK_PTR(uint32_t, msg);
    *ptr = val;
}

void msgpack_init_int64_t(msgpack* msg, wchar_t* name, int64_t val) {
    msg->type = MSGPACK_INT64;
    ALLOCATE_MSGPACK_PTR(int64_t, msg);
    wstring_init(&msg->name, name);

    int64_t* ptr = SELECT_MSGPACK_PTR(int64_t, msg);
    *ptr = val;
}

void msgpack_init_uint64_t(msgpack* msg, wchar_t* name, uint64_t val) {
    msg->type = MSGPACK_UINT64;
    ALLOCATE_MSGPACK_PTR(uint64_t, msg);
    wstring_init(&msg->name, name);

    uint64_t* ptr = SELECT_MSGPACK_PTR(uint64_t, msg);
    *ptr = val;
}

void msgpack_init_float_t(msgpack* msg, wchar_t* name, float_t val) {
    msg->type = MSGPACK_FLOAT;
    ALLOCATE_MSGPACK_PTR(float_t, msg);
    wstring_init(&msg->name, name);

    float_t* ptr = SELECT_MSGPACK_PTR(float_t, msg);
    *ptr = val;
}

void msgpack_init_double_t(msgpack* msg, wchar_t* name, double_t val) {
    msg->type = MSGPACK_DOUBLE;
    ALLOCATE_MSGPACK_PTR(double_t, msg);
    wstring_init(&msg->name, name);

    double_t* ptr = SELECT_MSGPACK_PTR(double_t, msg);
    *ptr = val;
}

void msgpack_init_char_string(msgpack* msg, wchar_t* name, char* val) {
    wchar_t* temp = utf8_decode(val);
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK_PTR(wstring, msg);
    wstring_init(&msg->name, name);
    wstring_init(SELECT_MSGPACK_PTR(wstring, msg), temp);
    free(temp);
}

void msgpack_init_string(msgpack* msg, wchar_t* name, wchar_t* val) {
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK_PTR(wstring, msg);
    wstring_init(&msg->name, name);
    wstring_init(SELECT_MSGPACK_PTR(wstring, msg), val);
}

bool msgpack_check_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return SELECT_MSGPACK_PTR(msgpack_array, msg)->begin == 0;
    else if (msg->type == MSGPACK_MAP)
        return SELECT_MSGPACK_PTR(msgpack_map, msg)->begin == 0;
    return msg->type == MSGPACK_NONE;
}

bool msgpack_check_not_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return SELECT_MSGPACK_PTR(msgpack_array, msg)->begin != 0;
    else if (msg->type == MSGPACK_MAP)
        return SELECT_MSGPACK_PTR(msgpack_map, msg)->begin != 0;
    return msg->type != MSGPACK_NONE;
}

msgpack* msgpack_get_by_index(msgpack* msg, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return 0;

    msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
    if ((ssize_t)index < ptr->end - ptr->begin)
        return &ptr->begin[index];
    return 0;
}

void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return;

    msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
    if ((ssize_t)index < ptr->end - ptr->begin) {
        msgpack_free(&ptr->begin[index]);
        ptr->begin[index] = *m;
    }
}

msgpack* msgpack_get_by_name(msgpack* msg, wchar_t* name) {
    if (msg->type != MSGPACK_MAP)
        return 0;

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    for (msgpack* i = ptr->begin; i != ptr->end; i++)
        if (!wcscmp(wstring_access(&i->name), name))
            return i;

    return 0;
}

void msgpack_set_by_name(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    for (msgpack* i = ptr->begin; i != ptr->end; i++)
        if (!wcscmp(wstring_access(&i->name), wstring_access(&m->name))) {
            msgpack_free(i);
            *i = *m;
            return;
        }

    msgpack_append(msg, m);
}

void msgpack_append(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    vector_msgpack_push_back(ptr, m);
    memset(m->data, 0, sizeof(m->data));
}

void msgpack_append_bool(msgpack* msg, wchar_t* name, bool val) {
    msgpack m;
    m.type = MSGPACK_BOOL;
    ALLOCATE_MSGPACK(bool, m);
    wstring_init(&m.name, name);

    bool* ptr = SELECT_MSGPACK(bool, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int8_t(msgpack* msg, wchar_t* name, int8_t val) {
    msgpack m;
    m.type = MSGPACK_INT8;
    ALLOCATE_MSGPACK(int8_t, m);
    wstring_init(&m.name, name);

    int8_t* ptr = SELECT_MSGPACK(int8_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint8_t(msgpack* msg, wchar_t* name, uint8_t val) {
    msgpack m;
    m.type = MSGPACK_UINT8;
    ALLOCATE_MSGPACK(uint8_t, m);
    wstring_init(&m.name, name);

    uint8_t* ptr = SELECT_MSGPACK(uint8_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int16_t(msgpack* msg, wchar_t* name, int16_t val) {
    msgpack m;
    m.type = MSGPACK_INT16;
    ALLOCATE_MSGPACK(int16_t, m);
    wstring_init(&m.name, name);

    int16_t* ptr = SELECT_MSGPACK(int16_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint16_t(msgpack* msg, wchar_t* name, uint16_t val) {
    msgpack m;
    m.type = MSGPACK_UINT16;
    ALLOCATE_MSGPACK(uint16_t, m);
    wstring_init(&m.name, name);

    uint16_t* ptr = SELECT_MSGPACK(uint16_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int32_t(msgpack* msg, wchar_t* name, int32_t val) {
    msgpack m;
    m.type = MSGPACK_INT32;
    ALLOCATE_MSGPACK(int32_t, m);
    wstring_init(&m.name, name);

    int32_t* ptr = SELECT_MSGPACK(int32_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint32_t(msgpack* msg, wchar_t* name, uint32_t val) {
    msgpack m;
    m.type = MSGPACK_UINT32;
    ALLOCATE_MSGPACK(uint32_t, m);
    wstring_init(&m.name, name);

    uint32_t* ptr = SELECT_MSGPACK(uint32_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int64_t(msgpack* msg, wchar_t* name, int64_t val) {
    msgpack m;
    m.type = MSGPACK_INT64;
    ALLOCATE_MSGPACK(int64_t, m);
    wstring_init(&m.name, name);

    int64_t* ptr = SELECT_MSGPACK(int64_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint64_t(msgpack* msg, wchar_t* name, uint64_t val) {
    msgpack m;
    m.type = MSGPACK_UINT64;
    ALLOCATE_MSGPACK(uint64_t, m);
    wstring_init(&m.name, name);

    uint64_t* ptr = SELECT_MSGPACK(uint64_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_float_t(msgpack* msg, wchar_t* name, float_t val) {
    msgpack m;
    m.type = MSGPACK_FLOAT;
    ALLOCATE_MSGPACK(float_t, m);
    wstring_init(&m.name, name);

    float_t* ptr = SELECT_MSGPACK(float_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_double_t(msgpack* msg, wchar_t* name, double_t val) {
    msgpack m;
    m.type = MSGPACK_DOUBLE;
    ALLOCATE_MSGPACK(double_t, m);
    wstring_init(&m.name, name);

    double_t* ptr = SELECT_MSGPACK(double_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_char_string(msgpack* msg, wchar_t* name, char* val) {
    msgpack m;
    wchar_t* temp = utf8_decode(val);
    m.type = MSGPACK_STRING;
    ALLOCATE_MSGPACK(wstring, m);
    wstring_init(&m.name, name);
    wstring_init(SELECT_MSGPACK(wstring, m), temp);
    free(temp);
    msgpack_append(msg, &m);
}

void msgpack_append_string(msgpack* msg, wchar_t* name, wchar_t* val) {
    msgpack m;
    m.type = MSGPACK_STRING;
    ALLOCATE_MSGPACK(wstring, m);
    wstring_init(&m.name, name);
    wstring_init(SELECT_MSGPACK(wstring, m), val);
    msgpack_append(msg, &m);
}

void msgpack_set_null(msgpack* msg, wchar_t* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_NULL;
    wstring_init(&msg->name, name);
}

void msgpack_set_array(msgpack* msg, wchar_t* name, msgpack_array* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_ARRAY;
    ALLOCATE_MSGPACK_PTR(msgpack_array, msg);
    wstring_init(&msg->name, name);

    msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
    *ptr = *val;
    val->capacity_end = val->end = val->begin = 0;
}

void msgpack_set_array_empty(msgpack* msg, wchar_t* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_ARRAY;
    ALLOCATE_MSGPACK_PTR(msgpack_array, msg);
    wstring_init(&msg->name, name);

    msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_set_map(msgpack* msg, wchar_t* name, msgpack_map* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK_PTR(msgpack_map, msg);
    wstring_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    *ptr = *val;
    val->capacity_end = val->end = val->begin = 0;
}

void msgpack_set_map_empty(msgpack* msg, wchar_t* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_MAP;
    ALLOCATE_MSGPACK_PTR(msgpack_map, msg);
    wstring_init(&msg->name, name);

    msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_set_bool(msgpack* msg, wchar_t* name, bool val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_BOOL;
    ALLOCATE_MSGPACK_PTR(bool, msg);
    wstring_init(&msg->name, name);

    bool* ptr = SELECT_MSGPACK_PTR(bool, msg);
    *ptr = val;
}

void msgpack_set_int8_t(msgpack* msg, wchar_t* name, int8_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT8;
    ALLOCATE_MSGPACK_PTR(int8_t, msg);
    wstring_init(&msg->name, name);

    int8_t* ptr = SELECT_MSGPACK_PTR(int8_t, msg);
    *ptr = val;
}

void msgpack_set_uint8_t(msgpack* msg, wchar_t* name, uint8_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT8;
    ALLOCATE_MSGPACK_PTR(uint8_t, msg);
    wstring_init(&msg->name, name);

    uint8_t* ptr = SELECT_MSGPACK_PTR(uint8_t, msg);
    *ptr = val;
}

void msgpack_set_int16_t(msgpack* msg, wchar_t* name, int16_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT16;
    ALLOCATE_MSGPACK_PTR(int16_t, msg);
    wstring_init(&msg->name, name);

    int16_t* ptr = SELECT_MSGPACK_PTR(int16_t, msg);
    *ptr = val;
}

void msgpack_set_uint16_t(msgpack* msg, wchar_t* name, uint16_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT16;
    ALLOCATE_MSGPACK_PTR(uint16_t, msg);
    wstring_init(&msg->name, name);

    uint16_t* ptr = SELECT_MSGPACK_PTR(uint16_t, msg);
    *ptr = val;
}

void msgpack_set_int32_t(msgpack* msg, wchar_t* name, int32_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT32;
    ALLOCATE_MSGPACK_PTR(int32_t, msg);
    wstring_init(&msg->name, name);

    int32_t* ptr = SELECT_MSGPACK_PTR(int32_t, msg);
    *ptr = val;
}

void msgpack_set_uint32_t(msgpack* msg, wchar_t* name, uint32_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT32;
    ALLOCATE_MSGPACK_PTR(uint32_t, msg);
    wstring_init(&msg->name, name);

    uint32_t* ptr = SELECT_MSGPACK_PTR(uint32_t, msg);
    *ptr = val;
}

void msgpack_set_int64_t(msgpack* msg, wchar_t* name, int64_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT64;
    ALLOCATE_MSGPACK_PTR(int64_t, msg);
    wstring_init(&msg->name, name);

    int64_t* ptr = SELECT_MSGPACK_PTR(int64_t, msg);
    *ptr = val;
}

void msgpack_set_uint64_t(msgpack* msg, wchar_t* name, uint64_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT64;
    ALLOCATE_MSGPACK_PTR(uint64_t, msg);
    wstring_init(&msg->name, name);

    uint64_t* ptr = SELECT_MSGPACK_PTR(uint64_t, msg);
    *ptr = val;
}

void msgpack_set_float_t(msgpack* msg, wchar_t* name, float_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_FLOAT;
    ALLOCATE_MSGPACK_PTR(float_t, msg);
    wstring_init(&msg->name, name);

    float_t* ptr = SELECT_MSGPACK_PTR(float_t, msg);
    *ptr = val;
}

void msgpack_set_double_t(msgpack* msg, wchar_t* name, double_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_DOUBLE;
    ALLOCATE_MSGPACK_PTR(double_t, msg);
    wstring_init(&msg->name, name);

    double_t* ptr = SELECT_MSGPACK_PTR(double_t, msg);
    *ptr = val;
}

void msgpack_set_char_string(msgpack* msg, wchar_t* name, char* val) {
    if (!msg)
        return;

    wchar_t* temp = utf8_decode(val);
    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK_PTR(wstring, msg);
    wstring_init(&msg->name, name);
    wstring_init(SELECT_MSGPACK_PTR(wstring, msg), temp);
    free(temp);
}

void msgpack_set_string(msgpack* msg, wchar_t* name, wchar_t* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    ALLOCATE_MSGPACK_PTR(wstring, msg);
    wstring_init(&msg->name, name);
    wstring_init(SELECT_MSGPACK_PTR(wstring, msg), val);
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
        return *SELECT_MSGPACK_PTR(bool, m);
    return 0;
}

int8_t msgpack_read_int8_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return (int8_t)*SELECT_MSGPACK_PTR(uint8_t, m);
    return 0;
}

uint8_t msgpack_read_uint8_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint8_t)*SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    return 0;
}

int16_t msgpack_read_int16_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return (int16_t)*SELECT_MSGPACK_PTR(uint16_t, m);
    return 0;
}

uint16_t msgpack_read_uint16_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint16_t)(int16_t)*SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint16_t)(int16_t)*SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    return 0;
}

int32_t msgpack_read_int32_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (int32_t)*SELECT_MSGPACK_PTR(uint32_t, m);
    return 0;
}

uint32_t msgpack_read_uint32_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint32_t)(int32_t)*SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint32_t)(int32_t)*SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint32_t)(int32_t)*SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK_PTR(uint32_t, m);
    return 0;
}

int64_t msgpack_read_int64_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return *SELECT_MSGPACK_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (int64_t)*SELECT_MSGPACK_PTR(uint64_t, m);
    return 0;
}

uint64_t msgpack_read_uint64_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint64_t)(int64_t)*SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint64_t)(int64_t)*SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)(int64_t)*SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)*SELECT_MSGPACK_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK_PTR(uint64_t, m);
    return 0;
}

float_t msgpack_read_float_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (float_t)*SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (float_t)*SELECT_MSGPACK_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (float_t)*SELECT_MSGPACK_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (float_t)*SELECT_MSGPACK_PTR(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *SELECT_MSGPACK_PTR(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return (float_t)*SELECT_MSGPACK_PTR(double_t, m);
    return 0;
}

double_t msgpack_read_double_t(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *SELECT_MSGPACK_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *SELECT_MSGPACK_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *SELECT_MSGPACK_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *SELECT_MSGPACK_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *SELECT_MSGPACK_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *SELECT_MSGPACK_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (double_t)*SELECT_MSGPACK_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (double_t)*SELECT_MSGPACK_PTR(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *SELECT_MSGPACK_PTR(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return *SELECT_MSGPACK_PTR(double_t, m);
    return 0;
}

void msgpack_read_char_buffer_string(msgpack* msg, wchar_t* name, string* c) {
    if (!msg) {
        string_init(c, 0);
        return;
    }

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        wstring* ptr = SELECT_MSGPACK_PTR(wstring, m);
        char* temp = utf8_encode(wstring_access(ptr));
        string_init(c, temp);
        free(temp);
    }
    else
        string_init(c, 0);
}

void msgpack_read_buffer_string(msgpack* msg, wchar_t* name, wstring* c) {
    if (!msg) {
        wstring_init(c, 0);
        return;
    }

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        wstring* ptr = SELECT_MSGPACK_PTR(wstring, m);
        wstring_init(c, wstring_access(ptr));
    }
    else
        wstring_init(c, 0);
}

char* msgpack_read_char_string(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        wstring* ptr = SELECT_MSGPACK_PTR(wstring, m);
        return utf8_encode(wstring_access(ptr));
    }
    return 0;
}

wchar_t* msgpack_read_string(msgpack* msg, wchar_t* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        wstring* ptr = SELECT_MSGPACK_PTR(wstring, m);
        wchar_t* val = force_malloc_s(wchar_t, ptr->length + 1);
        memcpy(val, wstring_access(ptr), sizeof(wchar_t) * (ptr->length + 1));
        return val;
    }
    return 0;
}

void msgpack_free(msgpack* msg) {
    if (!msg)
        return;

    wstring_dispose(&msg->name);

    switch (msg->type) {
    case MSGPACK_STRING:
        wstring_dispose(SELECT_MSGPACK_PTR(wstring, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, msg);
        for (msgpack* i = ptr->begin; i != ptr->end; i++)
            msgpack_free(i);
        vector_msgpack_free(ptr);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = SELECT_MSGPACK_PTR(msgpack_map, msg);
        for (msgpack* i = ptr->begin; i != ptr->end; i++)
            msgpack_free(i);
        vector_msgpack_free(ptr);
    } break;
    }
    memset(msg, 0, sizeof(msgpack));
}
