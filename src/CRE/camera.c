/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.h"

camera* camera_init() {
    camera* c = force_malloc_s(camera, 1);
    return c;
}

static void camera_calculate_projection(camera* c);
static void camera_calculate_view(camera* c);
static void camera_calculate_forward(camera* c);

void camera_initialize(camera* c, double_t aspect, int32_t width, int32_t height) {
    c->aspect = aspect;
    c->fov = 0.0;
    c->forward = { 0.0f, 0.0f, -1.0f };
    c->rotation = { 0.0f, 0.0f, 0.0f };
    c->view_point = { 0.0, 0.0f, 0.0f };
    c->interest = { 0.0f, 0.0f, -1.0f };
    c->width = width;
    c->height = height;
    c->view = mat4_identity;
    c->inv_view = mat4_identity;
    c->projection = mat4_identity;
    c->inv_projection = mat4_identity;
    c->view_projection = mat4_identity;
    c->inv_view_projection = mat4_identity;
    c->view_projection_prev = mat4_identity;
    c->inv_view_projection_prev = mat4_identity;
    c->min_distance = 0.05;
    c->max_distance = 6000.0;
    c->changed_proj = true;
    c->changed_view = true;
    camera_set_pitch(c, 0.0);
    camera_set_yaw(c, 0.0);
    camera_set_roll(c, 0.0);
    camera_set_fov(c, 32.2673416137695);
    camera_calculate_forward(c);
    camera_set_position(c, (vec3*)&vec3_null);
    camera_update(c);
}

double_t camera_get_min_distance(camera* c) {
    return c->min_distance;
}

void camera_set_min_distance(camera* c, double_t value) {
    if (value != c->min_distance) {
        c->min_distance = value;
        c->changed_proj = true;
    }
}

double_t camera_get_max_distance(camera* c) {
    return c->max_distance;
}

void camera_set_max_distance(camera* c, double_t value) {
    if (value != c->max_distance) {
        c->max_distance = value;
        c->changed_proj = true;
    }
}

double_t camera_get_aspect(camera* c) {
    return c->aspect;
}

void camera_set_aspect(camera* c, double_t value) {
    if (value != c->aspect) {
        c->aspect = value;
        c->changed_proj = true;
    }
}

double_t camera_get_fov(camera* c) {
    return c->fov;
}

void camera_set_fov(camera* c, double_t value) {
    value = clamp(value, 1.0, 180.0);
    if (value != c->fov) {
        c->fov = value;
        c->fov_rad = value * DEG_TO_RAD;
        c->changed_proj = true;
    }
}

double_t camera_get_pitch(camera* c) {
    return c->pitch;
}

void camera_set_pitch(camera* c, double_t value) {
    value = fmod(value, 360.0);
    value = clamp(value, -89.5, 89.5);
    if (c->pitch != value) {
        c->pitch = value;
        c->changed_view = true;
    }
}

double_t camera_get_yaw(camera* c) {
    return c->yaw;
}

void camera_set_yaw(camera* c, double_t value) {
    value = fmod(value, 360.0);
    if (c->yaw != value) {
        c->yaw = value;
        c->changed_view = true;
    }
}

double_t camera_get_roll(camera* c) {
    return -c->roll;
}

void camera_set_roll(camera* c, double_t value) {
    value = fmod(value, 360.0);
    if (c->roll != value) {
        c->roll = value;
        c->changed_view = true;
    }
}

void camera_get_view_point(camera* c, vec3* value) {
    *value = c->view_point;
}

void camera_set_view_point(camera* c, vec3* value) {
    if (c->view_point.x != value->x
        || c->view_point.y != value->y
        || c->view_point.z != value->z) {
        c->view_point = *value;
        c->changed_view = true;
    }
}

void camera_get_interest(camera* c, vec3* value) {
    *value = c->interest;
}

void camera_set_interest(camera* c, vec3* value) {
    if (c->interest.x != value->x
        || c->interest.y != value->y
        || c->interest.z != value->z) {
        c->interest = *value;
        c->changed_view = true;
    }
}

void camera_get_res(camera* c, int32_t* width, int32_t* height) {
    *width = c->width;
    *height = c->height;
}

void camera_set_res(camera* c, int32_t width, int32_t height) {
    if (c->width != width || c->height != height) {
        c->width = width;
        c->height = height;
        c->changed_proj = true;
    }
}

void camera_set_fast_change(camera* c, bool value) {
    c->fast_change = value;
}

void camera_set_fast_change_hist0(camera* c, bool value) {
    c->fast_change_hist0 = value;
}

void camera_reset(camera* c) {
    camera_set_pitch(c, 0.0);
    camera_set_yaw(c, 0.0);
    camera_set_roll(c, 0.0);
    camera_set_fov(c, 32.2673416137695);
    camera_calculate_forward(c);
    camera_set_position(c, (vec3*)&vec3_null);
    camera_update(c);
}

void camera_move(camera* c, vec2d* move) {
    if (move->x != 0.0 || move->y != 0.0) {
        vec3 temp;
        vec3 view_point;
        vec3 interest;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        vec3_cross(c->forward, up, temp);
        vec3_normalize(temp, temp);
        vec3_mult_scalar(temp, (float_t)move->y, temp);
        vec3_add(c->view_point, temp, view_point);
        vec3_mult_scalar(c->forward, (float_t)move->x, temp);
        vec3_add(view_point, temp, view_point);

        vec3_add(view_point, c->forward, interest);
        camera_set_view_point(c, &view_point);
        camera_set_interest(c, &interest);
    }
}

void camera_rotate(camera* c, vec2d* rotate) {
    if (rotate->x != 0.0)
        camera_set_yaw(c, camera_get_yaw(c) + rotate->x);

    if (rotate->y != 0.0)
        camera_set_pitch(c, camera_get_pitch(c) + rotate->y);

    if (rotate->x != 0.0 || rotate->y != 0.0) {
        camera_calculate_forward(c);

        vec3 interest;
        vec3_add(c->view_point, c->forward, interest);
        camera_set_interest(c, &interest);
    }
}

void camera_roll(camera* c, double_t roll) {
    if (roll != 0.0f)
        camera_set_roll(c, -camera_get_roll(c) - roll);
}

void camera_set(camera* c, vec3* view_point, vec3* interest,
    vec3* trans, vec3* rot, vec3* scale, double_t roll, double_t fov) {
    vec3 _vp;
    vec3 _int;
    double_t _roll;
    double_t _fov;

    _vp = *view_point;
    _int = *interest;
    _roll = roll;
    _fov = fov;

    mat4 cam;
    mat4_translate(trans->x, trans->y, trans->z, &cam);
    mat4_rotate_mult(&cam, rot->x, rot->y, rot->z, &cam);
    mat4_scale_rot(&cam, scale->x, scale->y, scale->z, &cam);
    mat4_mult_vec3_trans(&cam, &_vp, &_vp);
    mat4_mult_vec3_trans(&cam, &_int, &_int);

    camera_set_view_point(c, &_vp);
    camera_set_interest(c, &_int);
    camera_set_roll(c, _roll);
    camera_set_fov(c, _fov);
}

void camera_set_position(camera* c, vec3* pos) {
    vec3 interest;
    vec3_add(*pos, c->forward, interest);
    camera_set_view_point(c, pos);
    camera_set_interest(c, &interest);
}

void camera_update(camera* c) {
    if (c->changed_proj)
        camera_calculate_projection(c);

    if (c->changed_view)
        camera_calculate_view(c);

    if (c->changed_proj || c->changed_view) {
        mat4_mult(&c->view, &c->projection, &c->view_projection);
        mat4_inverse(&c->view_projection, &c->inv_view_projection);
    }

    c->changed_proj = false;
    c->changed_view = false;

    c->fast_change_hist1 = c->fast_change_hist0;
    c->fast_change_hist0 = c->fast_change;
    c->fast_change = false;
}

void camera_dispose(camera* c) {
    free(c);
}

static void camera_calculate_forward(camera* c) {
    c->forward.x = (float_t)(cos(c->pitch * DEG_TO_RAD) * sin(c->yaw * DEG_TO_RAD));
    c->forward.y = (float_t)sin(c->pitch * DEG_TO_RAD);
    c->forward.z = (float_t)(cos(c->pitch * DEG_TO_RAD) * -cos(c->yaw * DEG_TO_RAD));
}

static void camera_calculate_projection(camera* c) {
    mat4_persp(c->fov_rad, c->aspect, c->min_distance, c->max_distance, &c->projection);
    mat4_inverse(&c->projection, &c->inv_projection);

    float_t fov_correct_height = (float_t)(((double_t)c->height * 0.5) / tan(c->fov_rad * 0.5));
    float_t height = (float_t)c->height * 0.5f;
    float_t width = (float_t)c->height * (float_t)c->aspect * 0.5f;

    c->field_1E4.x = fov_correct_height;
    c->field_1E4.y = 0.0f;
    c->field_1E4.z = -width;
    vec3_normalize(c->field_1E4, c->field_1E4);
    c->field_1F0.x = -fov_correct_height;
    c->field_1F0.y = 0.0f;
    c->field_1F0.z = -width;
    vec3_normalize(c->field_1F0, c->field_1F0);
    c->field_1FC.x = 0.0f;
    c->field_1FC.y = fov_correct_height;
    c->field_1FC.z = -height;
    vec3_normalize(c->field_1FC, c->field_1FC);
    c->field_208.x = 0.0f;
    c->field_208.y = -fov_correct_height;
    c->field_208.z = -height;
    vec3_normalize(c->field_208, c->field_208);
}

static void camera_calculate_view(camera* c) {
    vec3 dist;
    vec3_sub(c->view_point, c->interest, dist);

    vec3 rotation;
    rotation.x = atan2f(-dist.y, sqrtf(dist.x * dist.x + dist.z * dist.z));
    rotation.y = atan2f(dist.x, dist.z);
    rotation.z = (float_t)(c->roll * DEG_TO_RAD);

    c->rotation = rotation;
    vec3_negate(rotation, rotation);

    vec3 view_point;
    vec3_negate(c->view_point, view_point);

    c->view = mat4_identity;
    mat4_rot_z(&c->view, rotation.z, &c->view);
    mat4_rot_x(&c->view, rotation.x, &c->view);
    mat4_rot_y(&c->view, rotation.y, &c->view);
    mat4_translate_mult(&c->view, view_point.x, view_point.y, view_point.z, &c->view);

    mat4_inverse(&c->view, &c->inv_view);
    mat3_from_mat4(&c->view, &c->view_mat3);
    mat3_inverse(&c->view_mat3, &c->inv_view_mat3);
    mat4_from_mat3(&c->view_mat3, &c->view_rot);
    mat4_from_mat3(&c->inv_view_mat3, &c->inv_view_rot);

    if (fabs(c->rotation.x * RAD_TO_DEG - c->pitch) > 0.01
        || fabs(-c->rotation.y * RAD_TO_DEG - c->yaw) > 0.01) {
        c->pitch = c->rotation.x * RAD_TO_DEG;
        c->yaw = -c->rotation.y * RAD_TO_DEG;
        camera_calculate_forward(c);
    }
}
