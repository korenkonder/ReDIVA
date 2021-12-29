/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vector.h"
#include "enrs.h"
#include "header.h"
#include "pof.h"

typedef struct f2_struct f2_struct;

vector(f2_struct)

struct f2_struct {
    f2_header header;
    void* data;
    size_t length;
    vector_f2_struct sub_structs;

    vector_enrs_entry enrs;
    vector_size_t pof;
};

extern void f2_struct_read(f2_struct* st, char* path);
extern void f2_struct_wread(f2_struct* st, wchar_t* path);
extern void f2_struct_mread(f2_struct* st, void* data, size_t length);
extern void f2_struct_sread(f2_struct* st, stream* s);
extern void f2_struct_write(f2_struct* st, char* path, bool use_depth, bool shift_x);
extern void f2_struct_wwrite(f2_struct* st, wchar_t* path, bool use_depth, bool shift_x);
extern void f2_struct_mwrite(f2_struct* st, void** data, size_t* length, bool use_depth, bool shift_x);
extern void f2_struct_swrite(f2_struct* st, stream* s, bool use_depth, bool shift_x);
extern void f2_struct_free(f2_struct* st);
