/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.hpp"

static void camera_calculate_projection(camera* c);
static void camera_calculate_view(camera* c);
static void camera_calculate_forward(camera* c);

camera::camera() : view(), inv_view(), projection(), inv_projection(), view_projection(),
inv_view_projection(), view_projection_prev(), inv_view_projection_prev(), view_mat3(), inv_view_mat3(),
view_rot(), inv_view_rot(), forward(), rotation(), view_point(), interest(), width(), height(), field_1E4(),
field_1F0(), field_1FC(), field_208(), yaw(), pitch(), roll(), aspect(), fov(), fov_rad(),max_distance(),
min_distance(), changed_view(), changed_proj(), fast_change(), fast_change_hist0(), fast_change_hist1() {

}

camera::~camera() {

}

void camera::initialize(double_t aspect, int32_t width, int32_t height) {
    this->aspect = aspect;
    fov = 0.0;
    forward = { 0.0f, 0.0f, -1.0f };
    rotation = { 0.0f, 0.0f, 0.0f };
    view_point = { 0.0, 0.0f, 0.0f };
    interest = { 0.0f, 0.0f, -1.0f };
    this->width = width;
    this->height = height;
    view = mat4_identity;
    inv_view = mat4_identity;
    projection = mat4_identity;
    inv_projection = mat4_identity;
    view_projection = mat4_identity;
    inv_view_projection = mat4_identity;
    view_projection_prev = mat4_identity;
    inv_view_projection_prev = mat4_identity;
    min_distance = 0.05;
    max_distance = 6000.0;
    changed_proj = true;
    changed_view = true;
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    set_pitch(0.0);
    set_yaw(0.0);
    set_roll(0.0);
    set_fov(32.2673416137695);
    camera_calculate_forward(this);
    set_position(vec3_null);
    update();
}

double_t camera::get_min_distance() {
    return min_distance;
}

void camera::set_min_distance(double_t value) {
    if (value != min_distance) {
        min_distance = value;
        changed_proj = true;
    }
}

double_t camera::get_max_distance() {
    return max_distance;
}

void camera::set_max_distance(double_t value) {
    if (value != max_distance) {
        max_distance = value;
        changed_proj = true;
    }
}

double_t camera::get_aspect() {
    return aspect;
}

void camera::set_aspect(double_t value) {
    if (value != aspect) {
        aspect = value;
        changed_proj = true;
    }
}

double_t camera::get_fov() {
    return fov;
}

void camera::set_fov(double_t value) {
    value = clamp(value, 1.0, 180.0);
    if (value != fov) {
        fov = value;
        fov_rad = value * DEG_TO_RAD;
        changed_proj = true;
    }
}

double_t camera::get_pitch() {
    return pitch;
}

void camera::set_pitch(double_t value) {
    value = fmod(value, 360.0);
    value = clamp(value, -89.5, 89.5);
    if (pitch != value) {
        pitch = value;
        changed_view = true;
    }
}

double_t camera::get_yaw() {
    return yaw;
}

void camera::set_yaw(double_t value) {
    value = fmod(value, 360.0);
    if (yaw != value) {
        yaw = value;
        changed_view = true;
    }
}

double_t camera::get_roll() {
    return roll;
}

void camera::set_roll(double_t value) {
    value = fmod(value, 360.0);
    if (roll != value) {
        roll = value;
        changed_view = true;
    }
}

void camera::get_view_point(vec3& value) {
    value = view_point;
}

void camera::set_view_point(const vec3& value) {
    if (memcmp(&value, &view_point, sizeof(vec3))) {
        view_point = value;
        changed_view = true;
    }
}

void camera::set_view_point(const vec3&& value) {
    if (memcmp(&value, &view_point, sizeof(vec3))) {
        view_point = value;
        changed_view = true;
    }
}

void camera::get_interest(vec3& value) {
    value = interest;
}

void camera::set_interest(const vec3& value) {
    if (memcmp(&value, &interest, sizeof(vec3))) {
        interest = value;
        changed_view = true;
    }
}

void camera::set_interest(const vec3&& value) {
    if (memcmp(&value, &interest, sizeof(vec3))) {
        interest = value;
        changed_view = true;
    }
}

void camera::get_res(int32_t& width, int32_t& height) {
    width = this->width;
    height = this->height;
}

void camera::set_res(int32_t width, int32_t height) {
    if (this->width != width || this->height != height) {
        this->width = width;
        this->height = height;
        changed_proj = true;
    }
}

void camera::set_fast_change(bool value) {
    fast_change = value;
}

void camera::set_fast_change_hist0(bool value) {
    fast_change_hist0 = value;
}

void camera::set_fast_change_hist1(bool value) {
    fast_change_hist1 = value;
}

void camera::reset() {
    set_pitch(0.0);
    set_yaw(0.0);
    set_roll(0.0);
    set_fov(32.2673416137695);
    min_distance = 0.05;
    max_distance = 6000.0;
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    camera_calculate_forward(this);
    set_position(vec3_null);
    update_data();
}

void camera::move(double_t move_x, double_t move_y) {
    if (move_x != 0.0 || move_y != 0.0) {
        vec3 temp;
        vec3 view_point;
        vec3 interest;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        vec3_cross(forward, up, temp);
        vec3_normalize(temp, temp);
        vec3_mult_scalar(temp, (float_t)move_y, temp);
        vec3_add(this->view_point, temp, view_point);
        vec3_mult_scalar(forward, (float_t)move_x, temp);
        vec3_add(view_point, temp, view_point);

        vec3_add(view_point, forward, interest);
        set_view_point(view_point);
        set_interest(interest);
    }
}

void camera::rotate(double_t rotate_x, double_t rotate_y) {
    if (rotate_x != 0.0)
        set_yaw(get_yaw() + rotate_x);

    if (rotate_y != 0.0)
        set_pitch(get_pitch() + rotate_y);

    if (rotate_x != 0.0 || rotate_y != 0.0) {
        camera_calculate_forward(this);

        vec3 interest;
        vec3_add(view_point, forward, interest);
        set_interest(interest);
    }
}

void camera::set(const vec3& view_point, const vec3& interest,
    const vec3& trans, const vec3& rot, const vec3& scale, double_t roll, double_t fov) {
    vec3 _vp;
    vec3 _int;
    double_t _roll;
    double_t _fov;

    _vp = view_point;
    _int = interest;
    _roll = roll;
    _fov = fov;

    mat4 cam;
    mat4_translate(trans.x, trans.y, trans.z, &cam);
    mat4_rotate_mult(&cam, rot.x, rot.y, rot.z, &cam);
    mat4_scale_rot(&cam, scale.x, scale.y, scale.z, &cam);
    mat4_mult_vec3_trans(&cam, &_vp, &_vp);
    mat4_mult_vec3_trans(&cam, &_int, &_int);

    set_view_point(_vp);
    set_interest(_int);
    set_roll(_roll);
    set_fov(_fov);
}

void camera::set(const vec3&& view_point, const vec3&& interest,
    const vec3&& trans, const vec3&& rot, const vec3&& scale, double_t roll, double_t fov) {
    set(*(vec3*)&view_point, *(vec3*)&interest, *(vec3*)&trans, *(vec3*)&rot, *(vec3*)&scale, roll, fov);
}

void camera::set_position(const vec3& pos) {
    vec3 interest;
    vec3_add(pos, forward, interest);
    set_view_point(pos);
    set_interest(interest);
}

void camera::set_position(const vec3&& pos) {
    vec3 interest;
    vec3_add(pos, forward, interest);
    set_view_point(pos);
    set_interest(interest);
}

void camera::update() {
    update_data();

    fast_change_hist1 = fast_change_hist0;
    fast_change_hist0 = fast_change;
    fast_change = false;
}

void camera::update_data() {
    if (changed_proj)
        camera_calculate_projection(this);

    if (changed_view)
        camera_calculate_view(this);

    if (changed_proj || changed_view) {
        mat4_mult(&view, &projection, &view_projection);
        mat4_inverse(&view_projection, &inv_view_projection);
    }

    changed_proj = false;
    changed_view = false;
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
    vec3 direction;
    vec3_sub(c->view_point, c->interest, direction);

    vec3 rotation;
    rotation.x = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    rotation.y = atan2f(direction.x, direction.z);
    rotation.z = (float_t)(c->roll * DEG_TO_RAD);

    c->rotation = rotation;
    vec3_negate(rotation, rotation);

    vec3 view_point;
    vec3_negate(c->view_point, view_point);

    mat4_rotate_z(rotation.z, &c->view);
    mat4_rotate_x_mult(&c->view, rotation.x, &c->view);
    mat4_rotate_y_mult(&c->view, rotation.y, &c->view);
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
