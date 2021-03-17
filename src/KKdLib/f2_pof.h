/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "vector.h"

typedef enum pof_value {
    POF_VALUE_INVALID = 0b00,
    POF_VALUE_INT8    = 0b01,
    POF_VALUE_INT16   = 0b10,
    POF_VALUE_INT32   = 0b11,
} pof_value;

extern void pof_read(stream* s, vector_size_t* pof, bool shift_x);
extern void pof_write(stream* s, vector_size_t* pof, bool shift_x);
extern uint32_t pof_length(vector_size_t* pof, bool shift_x);
