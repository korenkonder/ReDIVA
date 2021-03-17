/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "f2_enrs.h"
#include "f2_header.h"
#include "f2_pof.h"
#include "vector.h"

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

extern f2_struct* f2_struct_init();
extern void f2_struct_dispose(f2_struct* st);
extern void f2_struct_read(f2_struct* st, char* path);
extern void f2_struct_wread(f2_struct* st, wchar_t* path);
extern void f2_struct_read_memory(f2_struct* st, void* data, size_t length);
extern void f2_struct_write(f2_struct* st, char* path, bool use_depth, bool shift_x);
extern void f2_struct_wwrite(f2_struct* st, wchar_t* path, bool use_depth, bool shift_x);
extern void f2_struct_write_memory(f2_struct* st, void** data, size_t* length, bool use_depth, bool shift_x);
