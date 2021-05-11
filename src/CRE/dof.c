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

static  void dof_calculate(dof_struct* d, size_t height, double_t min_dist, double_t max_dist, double_t fov,
    float_t dist, float_t focal_length, float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio);

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
void dof_calculate_physical(dof_struct* d, size_t height,
    double_t min_dist, double_t max_dist, double_t fov, float_t dist,
    float_t focal_length, float_t f_number) {
    dof_calculate(d, height, min_dist, max_dist, fov, dist, focal_length, f_number, 0.0f, 1.0f, 0.0f);
}

void dof_calculate_f2(dof_struct* d, size_t height,
    double_t min_dist, double_t max_dist, double_t fov, float_t dist,
    float_t focus_range, float_t fuzzing_range, float_t ratio) {
    dof_calculate(d, height, min_dist, max_dist, fov, dist, 0.0f, 1.0f, focus_range, fuzzing_range, ratio);
}

void dof_dispose(dof_struct* d) {
    free(d);
}

static void dof_calculate(dof_struct* d, size_t height, double_t min_dist, double_t max_dist, double_t fov,
    float_t dist, float_t focal_length, float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio) {
    double_t fl = (double_t)focal_length;
    if (dist <= focal_length)
        fl = (double_t)dist + 0.1;
    fl = fl * fl * (1.0 / ((dist - fl) * f_number));
    d->data[0] = (float_t)((min_dist - max_dist) / (min_dist * max_dist));
    d->data[1] = (float_t)(1.0 / min_dist);
    d->data[2] = -(float_t)(((fl * dist) * (min_dist - max_dist)) * (1.0 / (min_dist * max_dist)));
    d->data[3] = (float_t)((1.0 - dist * (1.0 / min_dist)) * fl);
    d->data[4] = (float_t)((double_t)height / (tan(fov * 0.5) * 2.0 * min_dist));
    d->data[5] = d->data[4] * (float_t)(1.0 / 3.0);
    d->data[6] = (float_t)(1.0 / 3.0);
    d->data[7] = 3.0;
    d->data[8] = dist;
    d->data[9] = focus_range;
    d->data[10] = -4.5f / (fuzzing_range * fuzzing_range);
    d->data[11] = ratio * 8.0f;
}
