/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.hpp"
#include "str_utils.hpp"

#define MSGPACK_ALLOCATE(a, b) \
if (MSGPACK_CHECK(a)) \
    (b).data.ptr = new (a);
#define MSGPACK_ALLOCATE_PTR(a, b) \
if (MSGPACK_CHECK(a)) \
    (b)->data.ptr = new (a);

msgpack::msgpack() : data() {
    type = MSGPACK_NULL;
}

msgpack::msgpack(const msgpack& m) : type(), data() {
    if (this == &m)
        return;

    switch (m.type) {
    case MSGPACK_BOOL: {
        MSGPACK_ALLOCATE_PTR(double_t, this);
        bool* s_ptr = MSGPACK_SELECT(bool, m);
        bool* d_ptr = MSGPACK_SELECT_PTR(bool, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT8: {
        MSGPACK_ALLOCATE_PTR(int8_t, this);
        int8_t* s_ptr = MSGPACK_SELECT(int8_t, m);
        int8_t* d_ptr = MSGPACK_SELECT_PTR(int8_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT8: {
        MSGPACK_ALLOCATE_PTR(uint8_t, this);
        uint8_t* s_ptr = MSGPACK_SELECT(uint8_t, m);
        uint8_t* d_ptr = MSGPACK_SELECT_PTR(uint8_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT16: {
        MSGPACK_ALLOCATE_PTR(int16_t, this);
        int16_t* s_ptr = MSGPACK_SELECT(int16_t, m);
        int16_t* d_ptr = MSGPACK_SELECT_PTR(int16_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT16: {
        MSGPACK_ALLOCATE_PTR(uint16_t, this);
        uint16_t* s_ptr = MSGPACK_SELECT(uint16_t, m);
        uint16_t* d_ptr = MSGPACK_SELECT_PTR(uint16_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT32: {
        MSGPACK_ALLOCATE_PTR(int32_t, this);
        int32_t* s_ptr = MSGPACK_SELECT(int32_t, m);
        int32_t* d_ptr = MSGPACK_SELECT_PTR(int32_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT32: {
        MSGPACK_ALLOCATE_PTR(uint32_t, this);
        uint32_t* s_ptr = MSGPACK_SELECT(uint32_t, m);
        uint32_t* d_ptr = MSGPACK_SELECT_PTR(uint32_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT64: {
        MSGPACK_ALLOCATE_PTR(int64_t, this);
        int64_t* s_ptr = MSGPACK_SELECT(int64_t, m);
        int64_t* d_ptr = MSGPACK_SELECT_PTR(int64_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT64: {
        MSGPACK_ALLOCATE_PTR(uint64_t, this);
        uint64_t* s_ptr = MSGPACK_SELECT(uint64_t, m);
        uint64_t* d_ptr = MSGPACK_SELECT_PTR(uint64_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_FLOAT: {
        MSGPACK_ALLOCATE_PTR(float_t, this);
        float_t* s_ptr = MSGPACK_SELECT(float_t, m);
        float_t* d_ptr = MSGPACK_SELECT_PTR(float_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_DOUBLE: {
        MSGPACK_ALLOCATE_PTR(double_t, this);
        double_t* s_ptr = MSGPACK_SELECT(double_t, m);
        double_t* d_ptr = MSGPACK_SELECT_PTR(double_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_STRING: {
        MSGPACK_ALLOCATE_PTR(std::string, this);
        std::string* s_ptr = MSGPACK_SELECT(std::string, m);
        std::string* d_ptr = MSGPACK_SELECT_PTR(std::string, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_ARRAY:
    case MSGPACK_MAP: {
        MSGPACK_ALLOCATE_PTR(msgpack_array, this);
        msgpack_array* s_ptr = MSGPACK_SELECT(msgpack_array, m);
        msgpack_array* d_ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
        *d_ptr = *s_ptr;
    } break;
    }
    type = m.type;
    name = m.name;
}

msgpack::msgpack(const char* name, bool array, size_t length) : data() {
    type = array ? MSGPACK_ARRAY : MSGPACK_MAP;
    MSGPACK_ALLOCATE_PTR(msgpack_array, this);
    if (name)
        this->name = std::string(name);

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    *ptr = {};
    ptr->resize(length);
}

msgpack::msgpack(const char* name, bool array, msgpack_array& val) : data() {
    type = array ? MSGPACK_ARRAY : MSGPACK_MAP;
    MSGPACK_ALLOCATE_PTR(msgpack_array, this);
    if (name)
        this->name = std::string(name);

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    *ptr = val;
}

msgpack::msgpack(const char* name) : data() {
    type = MSGPACK_NULL;
    if (name)
        this->name = std::string(name);
}

msgpack::msgpack(const char* name, bool val) : data() {
    type = MSGPACK_BOOL;
    MSGPACK_ALLOCATE_PTR(bool, this);
    if (name)
        this->name = std::string(name);

    bool* ptr = MSGPACK_SELECT_PTR(bool, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, int8_t val) : data() {
    type = MSGPACK_INT8;
    MSGPACK_ALLOCATE_PTR(int8_t, this);
    if (name)
        this->name = std::string(name);

    int8_t* ptr = MSGPACK_SELECT_PTR(int8_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, uint8_t val) : data() {
    type = MSGPACK_UINT8;
    MSGPACK_ALLOCATE_PTR(uint8_t, this);
    if (name)
        this->name = std::string(name);

    uint8_t* ptr = MSGPACK_SELECT_PTR(uint8_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, int16_t val) : data() {
    type = MSGPACK_INT16;
    MSGPACK_ALLOCATE_PTR(int16_t, this);
    if (name)
        this->name = std::string(name);

    int16_t* ptr = MSGPACK_SELECT_PTR(int16_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, uint16_t val) : data() {
    type = MSGPACK_UINT16;
    MSGPACK_ALLOCATE_PTR(uint16_t, this);
    if (name)
        this->name = std::string(name);

    uint16_t* ptr = MSGPACK_SELECT_PTR(uint16_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, int32_t val) : data() {
    type = MSGPACK_INT32;
    MSGPACK_ALLOCATE_PTR(int32_t, this);
    if (name)
        this->name = std::string(name);

    int32_t* ptr = MSGPACK_SELECT_PTR(int32_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, uint32_t val) : data() {
    type = MSGPACK_UINT32;
    MSGPACK_ALLOCATE_PTR(uint32_t, this);
    if (name)
        this->name = std::string(name);

    uint32_t* ptr = MSGPACK_SELECT_PTR(uint32_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, int64_t val) : data() {
    type = MSGPACK_INT64;
    MSGPACK_ALLOCATE_PTR(int64_t, this);
    if (name)
        this->name = std::string(name);

    int64_t* ptr = MSGPACK_SELECT_PTR(int64_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, uint64_t val) : data() {
    type = MSGPACK_UINT64;
    MSGPACK_ALLOCATE_PTR(uint64_t, this);
    if (name)
        this->name = std::string(name);

    uint64_t* ptr = MSGPACK_SELECT_PTR(uint64_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, float_t val) : data() {
    type = MSGPACK_FLOAT;
    MSGPACK_ALLOCATE_PTR(float_t, this);
    if (name)
        this->name = std::string(name);

    float_t* ptr = MSGPACK_SELECT_PTR(float_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, double_t val) : data() {
    type = MSGPACK_DOUBLE;
    MSGPACK_ALLOCATE_PTR(double_t, this);
    if (name)
        this->name = std::string(name);

    double_t* ptr = MSGPACK_SELECT_PTR(double_t, this);
    *ptr = val;
}

msgpack::msgpack(const char* name, const char* val) : data() {
    type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(std::string, this);
    if (name)
        this->name = std::string(name);
    std::string* ptr = MSGPACK_SELECT_PTR(std::string, this);
    *ptr = {};
    if (val)
        *ptr = std::string(val);
}

msgpack::msgpack(const char* name, const wchar_t* val) : data() {
    type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(std::string, this);
    if (name)
        this->name = std::string(name);
    std::string* ptr = MSGPACK_SELECT_PTR(std::string, this);
    *ptr = {};
    if (val)
        *ptr = utf16_to_utf8(val);
}

msgpack::msgpack(const char* name, std::string& val) : data() {
    type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(std::string, this);
    if (name)
        this->name = std::string(name);
    std::string* ptr = MSGPACK_SELECT_PTR(std::string, this);
    *ptr = {};
    if (val.size())
        *ptr = val;
}

msgpack::msgpack(const char* name, std::wstring& val) : data() {
    type = MSGPACK_STRING;
    MSGPACK_ALLOCATE_PTR(std::string, this);
    if (name)
        this->name = std::string(name);
    std::string* ptr = MSGPACK_SELECT_PTR(std::string, this);
    *ptr = {};
    if (val.size())
        *ptr = utf16_to_utf8(val);
}

msgpack::~msgpack() {
    switch (type) {
    case MSGPACK_BOOL: {
        bool* ptr = MSGPACK_SELECT_PTR(bool, this);
        if (MSGPACK_CHECK(bool)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_INT8: {
        int8_t* ptr = MSGPACK_SELECT_PTR(int8_t, this);
        if (MSGPACK_CHECK(int8_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_UINT8: {
        uint8_t* ptr = MSGPACK_SELECT_PTR(uint8_t, this);
        if (MSGPACK_CHECK(uint8_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_INT16: {
        int16_t* ptr = MSGPACK_SELECT_PTR(int16_t, this);
        if (MSGPACK_CHECK(int16_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_UINT16: {
        uint16_t* ptr = MSGPACK_SELECT_PTR(uint16_t, this);
        if (MSGPACK_CHECK(uint16_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_INT32: {
        int32_t* ptr = MSGPACK_SELECT_PTR(int32_t, this);
        if (MSGPACK_CHECK(int32_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_UINT32: {
        uint32_t* ptr = MSGPACK_SELECT_PTR(uint32_t, this);
        if (MSGPACK_CHECK(uint32_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_INT64: {
        int64_t* ptr = MSGPACK_SELECT_PTR(int64_t, this);
        if (MSGPACK_CHECK(int64_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_UINT64: {
        uint64_t* ptr = MSGPACK_SELECT_PTR(uint64_t, this);
        if (MSGPACK_CHECK(uint64_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_FLOAT: {
        float_t* ptr = MSGPACK_SELECT_PTR(float_t, this);
        if (MSGPACK_CHECK(float_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_DOUBLE: {
        double_t* ptr = MSGPACK_SELECT_PTR(double_t, this);
        if (MSGPACK_CHECK(double_t)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_STRING: {
        std::string* ptr = MSGPACK_SELECT_PTR(std::string, this);
        ptr->clear();
        ptr->shrink_to_fit();
        if (MSGPACK_CHECK(std::string)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    case MSGPACK_ARRAY:
    case MSGPACK_MAP: {
        msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
        ptr->clear();
        ptr->shrink_to_fit();
        if (MSGPACK_CHECK(msgpack_array)) {
            delete ptr;
            data.ptr = 0;
        }
    } break;
    }
}

bool msgpack::check_null() {
    if (type == MSGPACK_ARRAY || type == MSGPACK_MAP)
        return MSGPACK_SELECT_PTR(msgpack_array, this)->size() == 0;
    return type == MSGPACK_NONE;
}

bool msgpack::check_not_null() {
    if (type == MSGPACK_ARRAY || type == MSGPACK_MAP)
        return MSGPACK_SELECT_PTR(msgpack_array, this)->size() != 0;
    return type != MSGPACK_NONE;
}

msgpack* msgpack::get_by_index(size_t index) {
    if (type != MSGPACK_ARRAY)
        return 0;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    if (index < ptr->size())
        return &ptr->data()[index];
    return 0;
}

void msgpack::set_by_index(msgpack* m, size_t index) {
    if (type != MSGPACK_ARRAY)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    if (index < ptr->size()) {
        msgpack& msg = ptr->data()[index];
        msg.~msgpack();
        msg = *m;
    }
}

msgpack* msgpack::get_by_name(const char* name) {
    if (type != MSGPACK_MAP)
        return 0;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    for (msgpack& i : *ptr)
        if (!i.name.compare(name))
            return &i;

    return 0;
}

void msgpack::set_by_name(msgpack* m) {
    if (type != MSGPACK_MAP)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    for (msgpack& i : *ptr)
        if (!i.name.compare(m->name)) {
            i.~msgpack();
            i = *m;
            return;
        }

    append(m);
}

void msgpack::append(msgpack* m) {
    if (type != MSGPACK_MAP)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    ptr->push_back(*m);
    *m = {};
}

void msgpack::append(msgpack& m) {
    if (type != MSGPACK_MAP)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    ptr->push_back(m);
    m = {};
}

void msgpack::append(msgpack&& m) {
    if (type != MSGPACK_MAP)
        return;

    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
    ptr->push_back(m);
    m = {};
}

msgpack* msgpack::read(const char* name) {
    if (!this)
        return 0;

    return name ? get_by_name(name) : this;
}

bool msgpack::read_bool(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
    if (m && m->type == MSGPACK_BOOL)
        return *MSGPACK_SELECT_PTR(bool, m);
    return 0;
}

int8_t msgpack::read_int8_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return *MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return (int8_t)*MSGPACK_SELECT_PTR(uint8_t, m);
    return 0;
}

uint8_t msgpack::read_uint8_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
    if (!m)
        return 0;

    if (m->type == MSGPACK_INT8)
        return (uint8_t)*MSGPACK_SELECT_PTR(int8_t, m);
    else if (m->type == MSGPACK_UINT8)
        return *MSGPACK_SELECT_PTR(uint8_t, m);
    return 0;
}

int16_t msgpack::read_int16_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

uint16_t msgpack::read_uint16_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

int32_t msgpack::read_int32_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

uint32_t msgpack::read_uint32_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

int64_t msgpack::read_int64_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

uint64_t msgpack::read_uint64_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

float_t msgpack::read_float_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

double_t msgpack::read_double_t(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;
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

char* msgpack::read_utf8_string(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;

    if (m && m->type == MSGPACK_STRING) {
        std::string* ptr = MSGPACK_SELECT_PTR(std::string, m);
        size_t length = ptr->size();
        char* val = force_malloc_s(char, length + 1);
        memcpy(val, ptr->c_str(), length);
        val[length] = 0;
        return val;
    }
    return 0;
}

wchar_t* msgpack::read_utf16_string(const char* name) {
    if (!this)
        return 0;

    msgpack* m = name ? get_by_name(name) : this;

    if (m && m->type == MSGPACK_STRING) {
        std::string* ptr = MSGPACK_SELECT_PTR(std::string, m);
        return utf8_to_utf16(ptr->c_str());
    }
    return 0;
}

std::string msgpack::read_string(const char* name) {
    if (!this)
        return {};

    msgpack* m = name ? get_by_name(name) : this;

    if (m && m->type == MSGPACK_STRING) {
        std::string* ptr = MSGPACK_SELECT_PTR(std::string, m);
        return *ptr;
    }
    return {};
}

std::wstring msgpack::read_wstring(const char* name) {
    if (!this)
        return {};

    msgpack* m = name ? get_by_name(name) : this;

    if (m && m->type == MSGPACK_STRING) {
        std::string* ptr = MSGPACK_SELECT_PTR(std::string, m);
        return utf8_to_utf16(*ptr);
    }
    return {};
}

msgpack& msgpack::operator=(const msgpack& m) {
    if (this == &m)
        return *this;

    switch (m.type) {
    case MSGPACK_BOOL: {
        MSGPACK_ALLOCATE_PTR(double_t, this);
        bool* s_ptr = MSGPACK_SELECT(bool, m);
        bool* d_ptr = MSGPACK_SELECT_PTR(bool, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT8: {
        MSGPACK_ALLOCATE_PTR(int8_t, this);
        int8_t* s_ptr = MSGPACK_SELECT(int8_t, m);
        int8_t* d_ptr = MSGPACK_SELECT_PTR(int8_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT8: {
        MSGPACK_ALLOCATE_PTR(uint8_t, this);
        uint8_t* s_ptr = MSGPACK_SELECT(uint8_t, m);
        uint8_t* d_ptr = MSGPACK_SELECT_PTR(uint8_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT16: {
        MSGPACK_ALLOCATE_PTR(int16_t, this);
        int16_t* s_ptr = MSGPACK_SELECT(int16_t, m);
        int16_t* d_ptr = MSGPACK_SELECT_PTR(int16_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT16: {
        MSGPACK_ALLOCATE_PTR(uint16_t, this);
        uint16_t* s_ptr = MSGPACK_SELECT(uint16_t, m);
        uint16_t* d_ptr = MSGPACK_SELECT_PTR(uint16_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT32: {
        MSGPACK_ALLOCATE_PTR(int32_t, this);
        int32_t* s_ptr = MSGPACK_SELECT(int32_t, m);
        int32_t* d_ptr = MSGPACK_SELECT_PTR(int32_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT32: {
        MSGPACK_ALLOCATE_PTR(uint32_t, this);
        uint32_t* s_ptr = MSGPACK_SELECT(uint32_t, m);
        uint32_t* d_ptr = MSGPACK_SELECT_PTR(uint32_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_INT64: {
        MSGPACK_ALLOCATE_PTR(int64_t, this);
        int64_t* s_ptr = MSGPACK_SELECT(int64_t, m);
        int64_t* d_ptr = MSGPACK_SELECT_PTR(int64_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_UINT64: {
        MSGPACK_ALLOCATE_PTR(uint64_t, this);
        uint64_t* s_ptr = MSGPACK_SELECT(uint64_t, m);
        uint64_t* d_ptr = MSGPACK_SELECT_PTR(uint64_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_FLOAT: {
        MSGPACK_ALLOCATE_PTR(float_t, this);
        float_t* s_ptr = MSGPACK_SELECT(float_t, m);
        float_t* d_ptr = MSGPACK_SELECT_PTR(float_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_DOUBLE: {
        MSGPACK_ALLOCATE_PTR(double_t, this);
        double_t* s_ptr = MSGPACK_SELECT(double_t, m);
        double_t* d_ptr = MSGPACK_SELECT_PTR(double_t, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_STRING: {
        MSGPACK_ALLOCATE_PTR(std::string, this);
        std::string* s_ptr = MSGPACK_SELECT(std::string, m);
        std::string* d_ptr = MSGPACK_SELECT_PTR(std::string, this);
        *d_ptr = *s_ptr;
    } break;
    case MSGPACK_ARRAY:
    case MSGPACK_MAP: {
        MSGPACK_ALLOCATE_PTR(msgpack_array, this);
        msgpack_array* s_ptr = MSGPACK_SELECT(msgpack_array, m);
        msgpack_array* d_ptr = MSGPACK_SELECT_PTR(msgpack_array, this);
        *d_ptr = *s_ptr;
    } break;}
    type = m.type;
    name = m.name;
    return *this;
}
