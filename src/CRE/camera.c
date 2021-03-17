/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.h"

camera* camera_init() {
    camera* c = force_malloc(sizeof(camera));
    return c;
}

void camera_update_focal_length(camera* c);
void camera_rebuild_projection(camera* c);
void camera_calculate_rotation(camera* c);
void camera_calculate(camera* c);

void camera_initialize(camera* c, double_t aspect, double_t fov) {
    c->aspect = aspect;
    c->fov = fov;
    c->up = (vec3){ 0.0f, 1.0f, 0.0f };
    c->position = (vec3){ 0.0f, 0.0f, 0.0f };
    c->target = (vec3){ 0.0f, 0.0f, -1.0f };
    mat4_identity(&c->view);
    mat4_identity(&c->projection);
    mat4_identity(&c->view_projection);
    c->min_distance = 0.05;
    c->max_distance = 6000.0;
    camera_set_pitch(c, 0.0);
    camera_set_yaw(c, 0.0);
    camera_set_roll(c, 0.0);
    camera_calculate_rotation(c);
    camera_rebuild_projection(c);
    camera_calculate(c);
}

double_t camera_get_min_distance(camera* c) {
    return c->min_distance;
}

void camera_set_min_distance(camera* c, double_t value) {
    if (value != c->min_distance) {
        c->min_distance = value;
        camera_rebuild_projection(c);
    }
}

double_t camera_get_max_distance(camera* c) {
    return c->max_distance;
}

void camera_set_max_distance(camera* c, double_t value) {
    if (value != c->max_distance) {
        c->max_distance = value;
        camera_rebuild_projection(c);
    }
}

double_t camera_get_camera_aperture_w(camera* c) {
    return c->camera_aperture_w;
}

void camera_set_camera_aperture_w(camera* c, double_t value) {
    if (value != c->camera_aperture_w) {
        c->camera_aperture_w = value;
        camera_rebuild_projection(c);
    }
}

double_t camera_get_camera_aperture_h(camera* c) {
    return c->camera_aperture_h;
}

void camera_set_camera_aperture_h(camera* c, double_t value) {
    if (value != c->camera_aperture_h) {
        c->camera_aperture_h = value;
        camera_rebuild_projection(c);
    }
}

double_t camera_get_focal_length(camera* c) {
    return c->focal_length;
}

void camera_set_focal_length(camera* c, double_t value) {
    if (value != c->focal_length) {
        c->focal_length = value;
        camera_rebuild_projection(c);
    }
}

bool camera_get_fov_horizontal(camera* c) {
    return c->fov_horizontal;
}

void camera_set_fov_horizontal(camera* c, bool value) {
    if (value != c->fov_horizontal) {
        c->fov_horizontal = value;
        camera_rebuild_projection(c);
    }
}

void camera_update_focal_length(camera* c) {
    camera_set_aspect(c, c->camera_aperture_w / c->camera_aperture_h);
    if (c->camera_aperture_w == 0 || c->camera_aperture_h == 0 || c->focal_length == 0)
        return;

    c->fov = atan(c->camera_aperture_h * 12.7 / c->focal_length);
    camera_set_fov(c, c->fov * 180.0 / M_PI * 2.0);
}

double_t camera_get_aspect(camera* c) {
    return c->aspect;
}

void camera_set_aspect(camera* c, double_t value) {
    if (value != c->aspect) {
        c->aspect = value;
        camera_rebuild_projection(c);
    }
}

double_t camera_get_fov(camera* c) {
    return c->fov;
}

void camera_set_fov(camera* c, double_t value) {
    value = clamp(value, 1.0, 180.0);
    if (value != c->fov) {
        c->fov = value;
        camera_rebuild_projection(c);
    }
}

void camera_rebuild_projection(camera* c) {
    double_t fov = c->fov;
    if (c->fov_horizontal)
        fov = 2.0 * atan(1.0 / c->aspect * tan(fov * 0.5));
    fov = fov * M_PI / 180.0;

    double_t max_y = c->min_distance * tan(fov * 0.5);
    double_t min_y = -max_y;
    double_t max_x = max_y * c->aspect;
    double_t min_x = min_y * c->aspect;

    double_t x = max_x - min_x;
    double_t y = max_y - min_y;
    double_t a = (max_x + min_x) / x;
    double_t b = (max_y + min_y) / y;
    double_t e = c->max_distance - c->min_distance;
    double_t d = -(2.0f * c->max_distance * c->min_distance) / e;

    x = 2.0f * c->min_distance / x;
    y = 2.0f * c->min_distance / y;
    e = -(c->max_distance + c->min_distance) / e;

    c->projection.row0 = (vec4){ (float_t)x, 0.0f, 0.0f, 0.0f };
    c->projection.row1 = (vec4){ 0.0f, (float_t)y, 0.0f, 0.0f };
    c->projection.row2 = (vec4){ (float_t)a, (float_t)b, (float_t)e, -1.0f };
    c->projection.row3 = (vec4){ 0.0f, 0.0f, (float_t)d, 0.0f };

    mat4_mult(&c->view, &c->projection, &c->view_projection);
}

double_t camera_get_pitch(camera* c) {
    return c->pitch;
}

void camera_set_pitch(camera* c, double_t value) {
    c->pitch = clamp(value, -89.5, 89.5);
}

double_t camera_get_yaw(camera* c) {
    return c->yaw + 90.0;
}

void camera_set_yaw(camera* c, double_t value) {
    while (value > 180.0)
        value -= 360.0;
    while (value < -180.0)
        value += 360.0;
    c->yaw = value - 90.0;
}

double_t camera_get_roll(camera* c) {
    return c->roll;
}

void camera_set_roll(camera* c, double_t value) {
    while (value > 180.0)
        value -= 360.0;
    while (value < -180.0)
        value += 360.0;
    c->roll = value;
}

void camera_reset(camera* c) {
    camera_set_pitch(c, 0.0);
    camera_set_yaw(c, 0.0);
    camera_set_roll(c, 0.0);
    c->position = (vec3){ 0.0f, 0.0f, 0.0f };
    camera_calculate_rotation(c);
    camera_calculate(c);
}

void camera_set_point(camera* c, vec3* view_point, vec3* interest_point,
    vec3* trans, vec3* rot, vec3* scale, double_t roll, double_t fov) {
    if (fov > 0.0)
        camera_set_fov(c, fov);

    while (roll > 180.0)
        roll -= 360.0;
    while (roll < -180.0)
        roll += 360.0;
    c->position = *view_point;
    c->target = *interest_point;
    c->up.x = (float_t)sin(-c->roll * 180.0 / M_PI);
    c->up.y = (float_t)cos(c->roll * 180.0 / M_PI);
    c->up.z = 0;

    mat4 trans_rot;
    mat4_identity(&trans_rot);
    mat4_rotate_x_mult(&trans_rot, (float_t)rot->x, &trans_rot);
    mat4_rotate_y_mult(&trans_rot, (float_t)rot->y, &trans_rot);
    mat4_rotate_z_mult(&trans_rot, (float_t)rot->z, &trans_rot);
    trans_rot.row3 = (vec4){ trans->x, trans->y, trans->z, 1.0f };

    /*
    mat4* cam = mat4::CreateScale(scale) * transRot;
    c->position = new vec3(new Vector4(c->position, 1) * cam);
    c->target = new vec3(new Vector4(c->target, 1) * cam);
    vec3 tempVar(new Vector4(c->up, 1) * cam);
    c->up = (&tempVar)->NormalizeAlt();*/
    camera_set(c, &c->position, &c->target, &c->up);
}

void camera_set(camera* c, vec3* eye, vec3* target, vec3* up) {
    camera_look_at(c, eye, target, up, &c->view);
    mat4_mult(&c->view, &c->projection, &c->view_projection);
}

void camera_rotate_vec2(camera* c, vec2* rotate) {
    if (rotate->x != 0 || rotate->y != 0) {
        camera_set_yaw(c, camera_get_yaw(c) + rotate->x);
        camera_set_pitch(c, camera_get_pitch(c) + rotate->y);
        camera_calculate_rotation(c);
        camera_calculate(c);
    }
}

void camera_move_vec2(camera* c, vec2* move) {
    if (move->x != 0 || move->y != 0) {
        vec3 temp;
        vec3_cross(c->forward, c->up, temp);
        vec3_normalize(temp, temp);
        vec3_mult_scalar(temp, move->y, temp);
        vec3_add(c->position, temp, c->position);
        vec3_mult_scalar(c->forward, move->x, temp);
        vec3_add(c->position, temp, c->position);
        camera_calculate(c);
    }
}

void camera_move_vec3(camera* c, vec3* move) {
    if (move->x != 0 || move->y != 0 || move->z != 0) {
        c->position = *move;
        camera_calculate(c);
    }
}

void camera_calculate_rotation(camera* c) {
    c->right.x = 0;
    c->right.y = (float_t)cos(c->pitch * M_PI / 180.0);
    c->right.z = (float_t)sin(c->pitch * M_PI / 180.0);
    c->forward.x = (float_t)(cos(c->pitch * M_PI / 180.0) * cos(c->yaw * M_PI / 180.0));
    c->forward.y = (float_t)sin(c->pitch * M_PI / 180.0);
    c->forward.z = (float_t)(cos(c->pitch * M_PI / 180.0) * sin(c->yaw * M_PI / 180.0));
    //c->up.x = (float_t)sin(c->roll * M_PI / 180.0);
    //c->up.y = (float_t)cos(c->roll * M_PI / 180.0);
    //c->up.z = 0;
}

void camera_calculate(camera* c) {
    vec3_add(c->position, c->forward, c->target);
    camera_look_at(c, &c->position, &c->target, &c->up, &c->view);
    mat4_mult(&c->view, &c->projection, &c->view_projection);
}

void camera_look_at(camera* c, vec3* eye, vec3* target, vec3* up, mat4* view) {
    vec3 x, y, z;
    float_t t;
    vec3 xyz;

    vec3_sub(*eye, *target, z);
    vec3_normalize(z, z);

    vec3_cross(*up, z, x);
    vec3_normalize(x, x);
    vec3_length(x, t);
    if (t == 0.0f)
        x = (vec3){ 1.0f, 0.0f, 0.0f };
    vec3_cross(z, x, y);
    vec3_normalize(y, y);

    vec3_dot(x, *eye, xyz.x);
    vec3_dot(y, *eye, xyz.y);
    vec3_dot(z, *eye, xyz.z);
    view->row0 = (vec4){ x.x, y.x, z.x, 0.0f };
    view->row1 = (vec4){ x.y, y.y, z.y, 0.0f };
    view->row2 = (vec4){ x.z, y.z, z.z, 0.0f };
    *(vec3*)&view->row3 = xyz;
    view->row3.w = -1.0f;
    vec4_negate(view->row3, view->row3);

    c->rotation.x = asinf(view->row1.z);
    c->rotation.y = atan2f(-view->row0.z, view->row2.z);
    c->rotation.z = atan2f(-view->row1.x, view->row1.y);
    vec3_negate(c->rotation, c->rotation);
}

void camera_dispose(camera* c) {
    free(c);
}
