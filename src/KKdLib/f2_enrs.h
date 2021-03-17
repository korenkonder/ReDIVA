/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "vector.h"

typedef enum enrs_type {
    ENRS_TYPE_WORD    = 0b00,
    ENRS_TYPE_DWORD   = 0b01,
    ENRS_TYPE_QWORD   = 0b10,
    ENRS_TYPE_INVALID = 0b11,
} enrs_type;

typedef enum enrs_value {
    ENRS_VALUE_INT8    = 0b00,
    ENRS_VALUE_INT16   = 0b01,
    ENRS_VALUE_INT32   = 0b10,
    ENRS_VALUE_INVALID = 0b11,
} enrs_value;

typedef struct enrs_sub_entry {
    uint32_t skip_bytes;
    uint32_t repeat_count;
    enrs_type type;
} enrs_sub_entry;

vector(enrs_sub_entry)

typedef struct enrs_entry {
    uint32_t offset;
    uint32_t count;
    uint32_t size;
    uint32_t repeat_count;
    vector_enrs_sub_entry sub;
} enrs_entry;

vector(enrs_entry)

void enrs_dispose(vector_enrs_entry* e);
extern void enrs_read(stream* s, vector_enrs_entry* enrs);
extern void enrs_write(stream* s, vector_enrs_entry* enrs);
extern uint32_t enrs_length(vector_enrs_entry* enrs);
