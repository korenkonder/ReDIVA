/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../default.h"
#include "../io/stream.h"

typedef enum enrs_type {
    ENRS_WORD    = 0x0,
    ENRS_DWORD   = 0x1,
    ENRS_QWORD   = 0x2,
    ENRS_INVALID = 0x3,
} enrs_type;

typedef struct enrs_sub_entry {
    uint32_t skip_bytes;
    uint32_t repeat_count;
    enrs_type type;
} enrs_sub_entry;

class enrs_entry {
public:
    uint32_t offset;
    uint32_t count;
    uint32_t size;
    uint32_t repeat_count;
    std::vector<enrs_sub_entry> sub;

    enrs_entry();
    enrs_entry(uint32_t offset, uint32_t count, uint32_t size, uint32_t repeat_count);
    ~enrs_entry();
};

class enrs {
public:
    std::vector<enrs_entry> vec;

    enrs();
    ~enrs();

    void apply(void* data);
    uint32_t length();
    void read(stream* s);
    void write(stream* s);
};
