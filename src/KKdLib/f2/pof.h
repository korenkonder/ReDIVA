/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vector.h"
#include "../io/stream.h"

extern void io_write_offset_pof_add(stream* s, ssize_t val,
    int32_t offset, bool is_x, vector_size_t* pof);
extern void io_write_offset_f2_pof_add(stream* s, ssize_t val,
    int32_t offset, vector_size_t* pof);
extern void io_write_offset_x_pof_add(stream* s, ssize_t val, vector_size_t* pof);
extern void pof_add(stream* s, vector_size_t* pof, size_t offset);
extern void pof_read(stream* s, vector_size_t* pof, bool shift_x);
extern void pof_write(stream* s, vector_size_t* pof, bool shift_x);
extern uint32_t pof_length(vector_size_t* pof, bool shift_x);
