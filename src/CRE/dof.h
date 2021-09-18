/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"

typedef enum dof_debug_flags {
    DOF_DEBUG_USE_UI_PARAMS   = 0x1,
    DOF_DEBUG_ENABLE_DOF      = 0x2,
    DOF_DEBUG_ENABLE_PHYS_DOF = 0x4,
    DOF_DEBUG_AUTO_FOCUS      = 0x8,
} dof_debug_flags;

typedef struct dof_f2 {
    float_t distance_to_focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
} dof_f2;

typedef struct dof_debug {
    dof_debug_flags flags;
    float_t distance_to_focus;
    float_t focal_length;
    float_t f_number;
    dof_f2 f2;
} dof_debug;

typedef struct dof_pv {
    bool enable;
    dof_f2 f2;
} dof_pv;

typedef struct dof_struct {
    dof_debug debug;
    dof_pv pv;
} dof_struct;

extern dof_struct* dof_init();
extern void dof_initialize(dof_struct* d, dof_debug* debug, dof_pv* pv);
extern void dof_dispose(dof_struct* d);
