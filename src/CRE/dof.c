/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dof.h"

static const dof_debug dof_debug_default = {
    .flags = 0,
    .distance_to_focus = 10.0f,
    .focal_length = 40.0f,
    .f_number = 1.4f,
    .f2 = {
        .distance_to_focus = 10.0f,
        .focus_range = 1.0f,
        .fuzzing_range = 0.5f,
        .ratio = 1.0f,
    },
};

static const dof_pv dof_pv_default = {
    .enable = false,
    .f2 = {
        .distance_to_focus = 10.0f,
        .focus_range = 1.0f,
        .fuzzing_range = 0.5f,
        .ratio = 1.0f,
    },
};

dof_struct* dof_init() {
    dof_struct* d = force_malloc(sizeof(dof_struct));
    return d;
}

void dof_initialize(dof_struct* d, dof_debug* debug, dof_pv* pv) {
    if (debug)
        d->debug = *debug;
    else
        d->debug = dof_debug_default;

    if (pv)
        d->pv = *pv;
    else
        d->pv = dof_pv_default;
}

void dof_get_dof_debug(dof_struct* d, dof_debug* debug) {
    if (debug)
        *debug = d->debug;
}

void dof_set_dof_debug(dof_struct* d, dof_debug* debug) {
    if (debug)
        d->debug = *debug;
    else
        d->debug = dof_debug_default;
}

void dof_get_dof_pv(dof_struct* d, dof_pv* pv) {
    if (pv)
        *pv = d->pv;
}

void dof_set_dof_pv(dof_struct* d, dof_pv* pv) {
    if (pv)
        d->pv = *pv;
    else
        d->pv = dof_pv_default;
}

void dof_dispose(dof_struct* d) {
    free(d);
}
