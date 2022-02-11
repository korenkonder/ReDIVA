/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.h"
#include "str_utils.h"

#define MSGPACK_ALLOCATE(a, b) \
if (MSGPACK_CHECK(a)) \
    (b).ptr = force_malloc(sizeof(a));
#define MSGPACK_ALLOCATE_PTR(a, b) \
if (MSGPACK_CHECK(a)) \
    (b)->ptr = force_malloc(sizeof(a));

vector_func(msgpack);

void msgpack_init_map(msgpack* msg, char* name) {
    msg->type = MSGPACK_MAP;
    MSGPACK_ALLOCATE_PTR(msgpack_map, msg);
    string_init(&msg->name, name);

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_init_array(msgpack* msg, char* name, size_t length) {
    msg->type = MSGPACK_ARRAY;
    MSGPACK_ALLOCATE_PTR(msgpack_array, msg);
    string_init(&msg->name, name);

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
    vector_msgpack_reserve(ptr, length);
    ptr->end = ptr->begin + length;
}

void msgpack_init_null(msgpack* msg, char* name) {
    msg->type = MSGPACK_NULL;
    string_init(&msg->name, name);
}

void msgpack_init_bool(msgpack* msg, char* name, bool val) {
    msg->type = MSGPACK_bool;
    MSGPACK_ALLOCATE_PTR(bool, msg);
    string_init(&msg->name, name);

    bool* ptr = MSGPACK_SELECT_PTR(bool, msg);
    *ptr = val;
}

void msgpack_init_int8_t(msgpack* msg, char* name, int8_t val) {
    msg->type = MSGPACK_INT8;
    MSGPACK_ALLOCATE_PTR(int8_t, msg);
    string_init(&msg->name, name);

    int8_t* ptr = MSGPACK_SELECT_PTR(int8_t, msg);
    *ptr = val;
}

void msgpack_init_uint8_t(msgpack* msg, char* name, uint8_t val) {
    msg->type = MSGPACK_UINT8;
    MSGPACK_ALLOCATE_PTR(uint8_t, msg);
    string_init(&msg->name, name);

    uint8_t* ptr = MSGPACK_SELECT_PTR(uint8_t, msg);
    *ptr = val;
}

void msgpack_init_int16_t(msgpack* msg, char* name, int16_t val) {
    msg->type = MSGPACK_INT16;
    MSGPACK_ALLOCATE_PTR(int16_t, msg);
    string_init(&msg->name, name);

    int16_t* ptr = MSGPACK_SELECT_PTR(int16_t, msg);
    *ptr = val;
}

void msgpack_init_uint16_t(msgpack* msg, char* name, uint16_t val) {
    msg->type = MSGPACK_UINT16;
    MSGPACK_ALLOCATE_PTR(uint16_t, msg);
    string_init(&msg->name, name);

    uint16_t* ptr = MSGPACK_SELECT_PTR(uint16_t, msg);
    *ptr = val;
}

void msgpack_init_int32_t(msgpack* msg, char* name, int32_t val) {
    msg->type = MSGPACK_INT32;
    MSGPACK_ALLOCATE_PTR(int32_t, msg);
    string_init(&msg->name, name);

    int32_t* ptr = MSGPACK_SELECT_PTR(int32_t, msg);
    *ptr = val;
}

void msgpack_init_uint32_t(msgpack* msg, char* name, uint32_t val) {
    msg->type = MSGPACK_UINT32;
    MSGPACK_ALLOCATE_PTR(uint32_t, msg);
    string_init(&msg->name, name);

    uint32_t* ptr = MSGPACK_SELECT_PTR(uint32_t, msg);
    *ptr = val;
}

void msgpack_init_int64_t(msgpack* msg, char* name, int64_t val) {
    msg->type = MSGPACK_INT64;
    MSGPACK_ALLOCATE_PTR(int64_t, msg);
    string_init(&msg->name, name);

    int64_t* ptr = MSGPACK_SELECT_PTR(int64_t, msg);
    *ptr = val;
}

void msgpack_init_uint64_t(msgpack* msg, char* name, uint64_t val) {
    msg->type = MSGPACK_UINT64;
    MSGPACK_ALLOCATE_PTR(uint64_t, msg);
    string_init(&msg->name, name);

    uint64_t* ptr = MSGPACK_SELECT_PTR(uint64_t, msg);
    *ptr = val;
}

void msgpack_init_float_t(msgpack* msg, char* name, float_t val) {
    msg->type = MSGPACK_FLOAT;
    MSGPACK_ALLOCATE_PTR(float_t, msg);
    string_init(&msg->name, name);

    float_t* ptr = MSGPACK_SELECT_PTR(float_t, msg);
    *ptr = val;
}

void msgpack_init_double_t(msgpack* msg, char* name, double_t val) {
    msg->type = MSGPACK_DOUBLE;
    MSGPACK_ALLOCATE_PTR(double_t, msg);
    string_init(&msg->name, name);

    double_t* ptr = MSGPACK_SELECT_PTR(double_t, msg);
    *ptr = val;
}

void msgpack_init_utf8_string(msgpack* msg, char* name, char* val) {
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_init(MSGPACK_SELECT_PTR(string, msg), val);
}

void msgpack_init_utf16_string(msgpack* msg, char* name, wchar_t* val) {
    char* temp = utf16_to_utf8(val);
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_init(MSGPACK_SELECT_PTR(string, msg), temp);
    free(temp);
}

void msgpack_init_string(msgpack* msg, char* name, string* val) {
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_copy(val, MSGPACK_SELECT_PTR(string, msg));
}

void msgpack_init_wstring(msgpack* msg, char* name, wstring* val) {
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_copy_wstring(val, MSGPACK_SELECT_PTR(string, msg));
}

bool msgpack_check_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return MSGPACK_SELECT_PTR(msgpack_array, msg)->begin == 0;
    else if (msg->type == MSGPACK_MAP)
        return MSGPACK_SELECT_PTR(msgpack_map, msg)->begin == 0;
    return msg->type == MSGPACK_NONE;
}

bool msgpack_check_not_null(msgpack* msg) {
    if (msg->type == MSGPACK_ARRAY)
        return MSGPACK_SELECT_PTR(msgpack_array, msg)->begin != 0;
    else if (msg->type == MSGPACK_MAP)
        return MSGPACK_SELECT_PTR(msgpack_map, msg)->begin != 0;
    return msg->type != MSGPACK_NONE;
}

msgpack* msgpack_get_by_index(msgpack* msg, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return 0;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    if ((ssize_t)index < vector_length(*ptr))
        return &ptr->begin[index];
    return 0;
}

void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index) {
    if (msg->type != MSGPACK_ARRAY)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    if ((ssize_t)index < vector_length(*ptr)) {
        msgpack_free(&ptr->begin[index]);
        ptr->begin[index] = *m;
    }
}

msgpack* msgpack_get_by_name(msgpack* msg, char* name) {
    if (msg->type != MSGPACK_MAP)
        return 0;

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    for (msgpack* i = ptr->begin; i != ptr->end; i++)
        if (!str_utils_compare(string_data(&i->name), name))
            return i;

    return 0;
}

void msgpack_set_by_name(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    for (msgpack* i = ptr->begin; i != ptr->end; i++)
        if (!str_utils_compare(string_data(&i->name), string_data(&m->name))) {
            msgpack_free(i);
            *i = *m;
            return;
        }

    msgpack_append(msg, m);
}

void msgpack_append(msgpack* msg, msgpack* m) {
    if (msg->type != MSGPACK_MAP)
        return;

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    vector_msgpack_push_back(ptr, m);
    memset(m->data, 0, sizeof(m->data));
}

void msgpack_append_bool(msgpack* msg, char* name, bool val) {
    msgpack m;
    m.type = MSGPACK_bool;
    MSGPACK_ALLOCATE(bool, m);
    string_init(&m.name, name);

    bool* ptr = MSGPACK_SELECT(bool, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int8_t(msgpack* msg, char* name, int8_t val) {
    msgpack m;
    m.type = MSGPACK_INT8;
    MSGPACK_ALLOCATE(int8_t, m);
    string_init(&m.name, name);

    int8_t* ptr = MSGPACK_SELECT(int8_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint8_t(msgpack* msg, char* name, uint8_t val) {
    msgpack m;
    m.type = MSGPACK_UINT8;
    MSGPACK_ALLOCATE(uint8_t, m);
    string_init(&m.name, name);

    uint8_t* ptr = MSGPACK_SELECT(uint8_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int16_t(msgpack* msg, char* name, int16_t val) {
    msgpack m;
    m.type = MSGPACK_INT16;
    MSGPACK_ALLOCATE(int16_t, m);
    string_init(&m.name, name);

    int16_t* ptr = MSGPACK_SELECT(int16_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint16_t(msgpack* msg, char* name, uint16_t val) {
    msgpack m;
    m.type = MSGPACK_UINT16;
    MSGPACK_ALLOCATE(uint16_t, m);
    string_init(&m.name, name);

    uint16_t* ptr = MSGPACK_SELECT(uint16_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int32_t(msgpack* msg, char* name, int32_t val) {
    msgpack m;
    m.type = MSGPACK_INT32;
    MSGPACK_ALLOCATE(int32_t, m);
    string_init(&m.name, name);

    int32_t* ptr = MSGPACK_SELECT(int32_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint32_t(msgpack* msg, char* name, uint32_t val) {
    msgpack m;
    m.type = MSGPACK_UINT32;
    MSGPACK_ALLOCATE(uint32_t, m);
    string_init(&m.name, name);

    uint32_t* ptr = MSGPACK_SELECT(uint32_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_int64_t(msgpack* msg, char* name, int64_t val) {
    msgpack m;
    m.type = MSGPACK_INT64;
    MSGPACK_ALLOCATE(int64_t, m);
    string_init(&m.name, name);

    int64_t* ptr = MSGPACK_SELECT(int64_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_uint64_t(msgpack* msg, char* name, uint64_t val) {
    msgpack m;
    m.type = MSGPACK_UINT64;
    MSGPACK_ALLOCATE(uint64_t, m);
    string_init(&m.name, name);

    uint64_t* ptr = MSGPACK_SELECT(uint64_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_float_t(msgpack* msg, char* name, float_t val) {
    msgpack m;
    m.type = MSGPACK_FLOAT;
    MSGPACK_ALLOCATE(float_t, m);
    string_init(&m.name, name);

    float_t* ptr = MSGPACK_SELECT(float_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_double_t(msgpack* msg, char* name, double_t val) {
    msgpack m;
    m.type = MSGPACK_DOUBLE;
    MSGPACK_ALLOCATE(double_t, m);
    string_init(&m.name, name);

    double_t* ptr = MSGPACK_SELECT(double_t, m);
    *ptr = val;
    msgpack_append(msg, &m);
}

void msgpack_append_utf8_string(msgpack* msg, char* name, char* val) {
    msgpack m;
    m.type = MSGPACK_STRING;
    MSGPACK_ALLOCATE(string, m);
    string_init(&m.name, name);
    string_init(MSGPACK_SELECT(string, m), val);
    msgpack_append(msg, &m);
}

void msgpack_append_utf16_string(msgpack* msg, char* name, wchar_t* val) {
    msgpack m;
    char* temp = utf16_to_utf8(val);
    m.type = MSGPACK_STRING;
    MSGPACK_ALLOCATE(string, m);
    string_init(&m.name, name);
    string_init(MSGPACK_SELECT(string, m), temp);
    free(temp);
    msgpack_append(msg, &m);
}

void msgpack_append_string(msgpack* msg, char* name, string* val) {
    msgpack m;
    m.type = MSGPACK_STRING;
    MSGPACK_ALLOCATE(string, m);
    string_init(&m.name, name);
    string_copy(val, MSGPACK_SELECT(string, m));
    msgpack_append(msg, &m);
}

void msgpack_append_wstring(msgpack* msg, char* name, wstring* val) {
    msgpack m;
    m.type = MSGPACK_STRING;
    MSGPACK_ALLOCATE(string, m);
    string_init(&m.name, name);
    string_copy_wstring(val, MSGPACK_SELECT(string, m));
    msgpack_append(msg, &m);
}

void msgpack_set_null(msgpack* msg, char* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_NULL;
    string_init(&msg->name, name);
}

void msgpack_set_array(msgpack* msg, char* name, msgpack_array* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_ARRAY;
    MSGPACK_ALLOCATE_PTR(msgpack_array, msg);
    string_init(&msg->name, name);

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    *ptr = *val;
    val->capacity_end = val->end = val->begin = 0;
}

void msgpack_set_array_empty(msgpack* msg, char* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_ARRAY;
    MSGPACK_ALLOCATE_PTR(msgpack_array, msg);
    string_init(&msg->name, name);

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_set_map(msgpack* msg, char* name, msgpack_map* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_MAP;
    MSGPACK_ALLOCATE_PTR(msgpack_map, msg);
    string_init(&msg->name, name);

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    *ptr = *val;
    val->capacity_end = val->end = val->begin = 0;
}

void msgpack_set_map_empty(msgpack* msg, char* name) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_MAP;
    MSGPACK_ALLOCATE_PTR(msgpack_map, msg);
    string_init(&msg->name, name);

    msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
    ptr->capacity_end = ptr->end = ptr->begin = 0;
}

void msgpack_set_bool(msgpack* msg, char* name, bool val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_bool;
    MSGPACK_ALLOCATE_PTR(bool, msg);
    string_init(&msg->name, name);

    bool* ptr = MSGPACK_SELECT_PTR(bool, msg);
    *ptr = val;
}

void msgpack_set_int8_t(msgpack* msg, char* name, int8_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT8;
    MSGPACK_ALLOCATE_PTR(int8_t, msg);
    string_init(&msg->name, name);

    int8_t* ptr = MSGPACK_SELECT_PTR(int8_t, msg);
    *ptr = val;
}

void msgpack_set_uint8_t(msgpack* msg, char* name, uint8_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT8;
    MSGPACK_ALLOCATE_PTR(uint8_t, msg);
    string_init(&msg->name, name);

    uint8_t* ptr = MSGPACK_SELECT_PTR(uint8_t, msg);
    *ptr = val;
}

void msgpack_set_int16_t(msgpack* msg, char* name, int16_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT16;
    MSGPACK_ALLOCATE_PTR(int16_t, msg);
    string_init(&msg->name, name);

    int16_t* ptr = MSGPACK_SELECT_PTR(int16_t, msg);
    *ptr = val;
}

void msgpack_set_uint16_t(msgpack* msg, char* name, uint16_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT16;
    MSGPACK_ALLOCATE_PTR(uint16_t, msg);
    string_init(&msg->name, name);

    uint16_t* ptr = MSGPACK_SELECT_PTR(uint16_t, msg);
    *ptr = val;
}

void msgpack_set_int32_t(msgpack* msg, char* name, int32_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT32;
    MSGPACK_ALLOCATE_PTR(int32_t, msg);
    string_init(&msg->name, name);

    int32_t* ptr = MSGPACK_SELECT_PTR(int32_t, msg);
    *ptr = val;
}

void msgpack_set_uint32_t(msgpack* msg, char* name, uint32_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT32;
    MSGPACK_ALLOCATE_PTR(uint32_t, msg);
    string_init(&msg->name, name);

    uint32_t* ptr = MSGPACK_SELECT_PTR(uint32_t, msg);
    *ptr = val;
}

void msgpack_set_int64_t(msgpack* msg, char* name, int64_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_INT64;
    MSGPACK_ALLOCATE_PTR(int64_t, msg);
    string_init(&msg->name, name);

    int64_t* ptr = MSGPACK_SELECT_PTR(int64_t, msg);
    *ptr = val;
}

void msgpack_set_uint64_t(msgpack* msg, char* name, uint64_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_UINT64;
    MSGPACK_ALLOCATE_PTR(uint64_t, msg);
    string_init(&msg->name, name);

    uint64_t* ptr = MSGPACK_SELECT_PTR(uint64_t, msg);
    *ptr = val;
}

void msgpack_set_float_t(msgpack* msg, char* name, float_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_FLOAT;
    MSGPACK_ALLOCATE_PTR(float_t, msg);
    string_init(&msg->name, name);

    float_t* ptr = MSGPACK_SELECT_PTR(float_t, msg);
    *ptr = val;
}

void msgpack_set_double_t(msgpack* msg, char* name, double_t val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_DOUBLE;
    MSGPACK_ALLOCATE_PTR(double_t, msg);
    string_init(&msg->name, name);

    double_t* ptr = MSGPACK_SELECT_PTR(double_t, msg);
    *ptr = val;
}

void msgpack_set_utf8_string(msgpack* msg, char* name, char* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_init(MSGPACK_SELECT_PTR(string, msg), val);
}

void msgpack_set_utf16_string(msgpack* msg, char* name, wchar_t* val) {
    if (!msg)
        return;

    char* temp = utf16_to_utf8(val);
    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_init(MSGPACK_SELECT_PTR(string, msg), temp);
    free(temp);
}

void msgpack_set_string(msgpack* msg, char* name, string* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_copy(val, MSGPACK_SELECT_PTR(string, msg));
}

void msgpack_set_wstring(msgpack* msg, char* name, wstring* val) {
    if (!msg)
        return;

    msgpack_free(msg);
    msg->type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(string, msg);
    string_init(&msg->name, name);
    string_copy_wstring(val, MSGPACK_SELECT_PTR(string, msg));
}

msgpack* msgpack_read(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    return name ? msgpack_get_by_name(msg, name) : msg;
}

bool msgpack_read_bool(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (m && m->type == MSGPACK_bool)
        return *MSGPACK_SELECT_PTR(bool, m);
    return 0;
}

int8_t msgpack_read_int8_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return (int8_t)*MSGPACK_SELECT_PTR(uint8_t, m);
    return 0;
}

uint8_t msgpack_read_uint8_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint8_t)*MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    return 0;
}

int16_t msgpack_read_int16_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return (int16_t)*MSGPACK_SELECT_PTR(uint16_t, m);
    return 0;
}

uint16_t msgpack_read_uint16_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint16_t)(int16_t)*MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint16_t)(int16_t)*MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    return 0;
}

int32_t msgpack_read_int32_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (int32_t)*MSGPACK_SELECT_PTR(uint32_t, m);
    return 0;
}

uint32_t msgpack_read_uint32_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint32_t)(int32_t)*MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint32_t)(int32_t)*MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint32_t)(int32_t)*MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *MSGPACK_SELECT_PTR(uint32_t, m);
    return 0;
}

int64_t msgpack_read_int64_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *MSGPACK_SELECT_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return *MSGPACK_SELECT_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (int64_t)*MSGPACK_SELECT_PTR(uint64_t, m);
    return 0;
}

uint64_t msgpack_read_uint64_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint64_t)(int64_t)*MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return (uint64_t)(int64_t)*MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)(int64_t)*MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *MSGPACK_SELECT_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT32)
        return (uint64_t)*MSGPACK_SELECT_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *MSGPACK_SELECT_PTR(uint64_t, m);
    return 0;
}

float_t msgpack_read_float_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return (float_t)*MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return (float_t)*MSGPACK_SELECT_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (float_t)*MSGPACK_SELECT_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (float_t)*MSGPACK_SELECT_PTR(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *MSGPACK_SELECT_PTR(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return (float_t)*MSGPACK_SELECT_PTR(double_t, m);
    return 0;
}

double_t msgpack_read_double_t(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    else if (m->type == MSGPACK_INT16)
        return *MSGPACK_SELECT_PTR(int16_t, m);
    else if (m->type == MSGPACK_UINT16)
        return *MSGPACK_SELECT_PTR(uint16_t, m);
    else if (m->type == MSGPACK_INT32)
        return *MSGPACK_SELECT_PTR(int32_t, m);
    else if (m->type == MSGPACK_UINT32)
        return *MSGPACK_SELECT_PTR(uint32_t, m);
    else if (m->type == MSGPACK_INT64)
        return (double_t)*MSGPACK_SELECT_PTR(int64_t, m);
    else if (m->type == MSGPACK_UINT64)
        return (double_t)*MSGPACK_SELECT_PTR(uint64_t, m);
    else if (m->type == MSGPACK_FLOAT)
        return *MSGPACK_SELECT_PTR(float_t, m);
    else if (m->type == MSGPACK_DOUBLE)
        return *MSGPACK_SELECT_PTR(double_t, m);
    return 0;
}

char* msgpack_read_utf8_string(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        string* ptr = MSGPACK_SELECT_PTR(string, m);
        size_t length = ptr->length;
        char* val = force_malloc_s(char, length + 1);
        memcpy(val, string_data(ptr), length);
        val[length] = 0;
        return val;
    }
    return 0;
}

wchar_t* msgpack_read_utf16_string(msgpack* msg, char* name) {
    if (!msg)
        return 0;

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        string* ptr = MSGPACK_SELECT_PTR(string, m);
        wchar_t* temp = utf8_to_utf16(string_data(ptr));
        size_t length = utf16_length(temp);
        wchar_t* val = force_malloc_s(wchar_t, length + 1);
        memcpy(val, temp, sizeof(wchar_t) * length);
        val[length] = 0;
        free(temp);
        return val;
    }
    return 0;
}

void msgpack_read_string(msgpack* msg, char* name, string* str) {
    if (!msg) {
        *str = string_empty;
        return;
    }

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        string* ptr = MSGPACK_SELECT_PTR(string, m);
        string_init_length(str, string_data(ptr), ptr->length);
    }
    else
        *str = string_empty;
}

void msgpack_read_wstring(msgpack* msg, char* name, wstring* str) {
    if (!msg) {
        *str = wstring_empty;
        return;
    }

    msgpack* m = name ? msgpack_get_by_name(msg, name) : msg;

    if (m && m->type == MSGPACK_STRING) {
        string* ptr = MSGPACK_SELECT_PTR(string, m);
        wchar_t* temp = utf8_to_utf16(string_data(ptr));
        wstring_init(str, temp);
        free(temp);
    }
    else
        *str = wstring_empty;
}

void msgpack_free(msgpack* msg) {
    if (!msg)
        return;

    string_free(&msg->name);

    switch (msg->type) {
    case MSGPACK_STRING:
        string_free(MSGPACK_SELECT_PTR(string, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
        vector_msgpack_free(ptr, msgpack_free);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
        vector_msgpack_free(ptr, msgpack_free);
    } break;
    }
    memset(msg, 0, sizeof(msgpack));
}
