/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "vector.h"

extern void pof_add(stream* s, vector_size_t* pof, size_t offset);
extern void pof_read(stream* s, vector_size_t* pof, bool shift_x);
extern void pof_write(stream* s, vector_size_t* pof, bool shift_x);
extern uint32_t pof_length(vector_size_t* pof, bool shift_x);
