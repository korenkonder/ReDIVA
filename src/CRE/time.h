/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef struct time_struct {
    LARGE_INTEGER timestamp;
} time_struct;

extern double_t time_struct_calc_time(time_struct* t);
extern void time_struct_get_timestamp(time_struct* t);
