/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "f2_enrs.h"
#include "f2_header.h"
#include "f2_pof.h"
#include "len_array.h"

typedef struct f2_struct f2_struct;

len_array(f2_struct)

struct f2_struct {
    f2_header header;
    void* data;
    size_t length;
    len_array_f2_struct sub_structs;

    len_array_enrs_entry enrs;
    len_array_size_t pof;
};

extern f2_struct* f2_struct_init();
extern void f2_struct_dispose(f2_struct* st);
extern void f2_struct_read(f2_struct* st, char* path);
extern void f2_struct_wread(f2_struct* st, wchar_t* path);
extern void f2_struct_write(f2_struct* st, char* path, bool use_depth, bool shift_x);
extern void f2_struct_wwrite(f2_struct* st, wchar_t* path, bool use_depth, bool shift_x);
