/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "vector.h"

typedef enum enrs_type {
    ENRS_TYPE_WORD    = 0x0,
    ENRS_TYPE_DWORD   = 0x1,
    ENRS_TYPE_QWORD   = 0x2,
    ENRS_TYPE_INVALID = 0x3,
} enrs_type;

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

extern void enrs_apply(vector_enrs_entry* enrs, void* data);
extern uint32_t enrs_length(vector_enrs_entry* enrs);
extern void enrs_read(stream* s, vector_enrs_entry* enrs);
extern void enrs_write(stream* s, vector_enrs_entry* enrs);
void enrs_dispose(vector_enrs_entry* e);
