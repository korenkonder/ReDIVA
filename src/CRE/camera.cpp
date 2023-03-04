/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.hpp"

static void camera_calculate_projection(camera* c);
static void camera_calculate_projection_aet(camera* c);
static void camera_calculate_view(camera* c);
static void camera_calculate_forward(camera* c);

camera::camera() : forward(), rotation(), view_point(), interest(), fov_correct_height(),
aet_depth(), render_width(), render_height(), sprite_width(), sprite_height(),
field_1E4(), field_1F0(), field_1FC(), field_208(), yaw(), pitch(), roll(), aspect(),
fov(), fov_rad(), max_distance(), min_distance(), changed_view(), changed_proj(),
changed_proj_aet(), fast_change(), fast_change_hist0(), fast_change_hist1() {

}

camera::~camera() {

}

void camera::initialize(double_t aspect, int32_t render_width,
    int32_t render_height, int32_t sprite_width, int32_t sprite_height) {
    this->aspect = aspect;
    fov = 0.0;
    fov_correct_height = 0.0f;
    aet_depth = 0.0f;
    forward = { 0.0f, 0.0f, -1.0f };
    rotation = { 0.0f, 0.0f, 0.0f };
    view_point = { 0.0, 0.0f, 0.0f };
    interest = { 0.0f, 0.0f, -1.0f };
    this->render_width = render_width;
    this->render_height = render_height;
    this->sprite_width = sprite_width;
    this->sprite_height = sprite_height;
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
    changed_proj_aet = true;
    changed_view = true;
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    set_pitch(0.0);
    set_yaw(0.0);
    set_roll(0.0);
    set_fov(32.2673416137695);
    camera_calculate_forward(this);
    set_position(0.0f);
    update();
}

double_t camera::get_min_distance() {
    return min_distance;
}

void camera::set_min_distance(double_t value) {
    if (value != min_distance) {
        min_distance = value;
        changed_proj = true;
        changed_proj_aet = true;
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
    value = clamp_def(value, 1.0, 180.0);
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
    value = clamp_def(value, -89.5, 89.5);
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

void camera::get_view_point(vec4& value) {
    *(vec3*)&value = view_point;
    value.w = 0.0f;
}

void camera::set_view_point(const vec3& value) {
    if (value != view_point) {
        view_point = value;
        changed_view = true;
    }
}

void camera::set_view_point(const vec3&& value) {
    if (value != view_point) {
        view_point = value;
        changed_view = true;
    }
}

void camera::get_interest(vec3& value) {
    value = interest;
}

void camera::set_interest(const vec3& value) {
    if (value != interest) {
        interest = value;
        changed_view = true;
    }
}

void camera::set_interest(const vec3&& value) {
    if (value != interest) {
        interest = value;
        changed_view = true;
    }
}

void camera::get_res(int32_t& render_width, int32_t& render_height,
    int32_t& sprite_width, int32_t& sprite_height) {
    render_width = this->render_width;
    render_height = this->render_height;
    sprite_width = this->sprite_width;
    sprite_height = this->sprite_height;
}

void camera::set_res(int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height) {
    if (this->render_width != render_width || this->render_height != render_height
        || this->sprite_width != sprite_width || this->sprite_height != sprite_height) {
        this->render_width = render_width;
        this->render_height = render_height;
        this->sprite_width = sprite_width;
        this->sprite_height = sprite_height;
        changed_proj = true;
        changed_proj_aet = true;
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
    set_position(0.0f);
    update_data();
}

void camera::move(double_t move_x, double_t move_y) {
    if (move_x != 0.0 || move_y != 0.0) {
        vec3 up = { 0.0f, 1.0f, 0.0f };
        vec3 view_point = this->view_point;
        view_point += vec3::normalize(vec3::cross(forward, up)) * (float_t)move_y;
        view_point += forward * (float_t)move_x;
        set_view_point(view_point);
        set_interest(view_point + forward);
    }
}

void camera::rotate(double_t rotate_x, double_t rotate_y) {
    if (rotate_x != 0.0)
        set_yaw(get_yaw() + rotate_x);

    if (rotate_y != 0.0)
        set_pitch(get_pitch() + rotate_y);

    if (rotate_x != 0.0 || rotate_y != 0.0) {
        camera_calculate_forward(this);

        set_interest(view_point + forward);
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
    mat4_translate(&trans, &cam);
    mat4_rotate_mult(&cam, &rot, &cam);
    mat4_scale_rot(&cam, &scale, &cam);
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
    set_view_point(pos);
    set_interest(pos + forward);
}

void camera::set_position(const vec3&& pos) {
    set_view_point(pos);
    set_interest(pos + forward);
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

    if (changed_proj_aet)
        camera_calculate_projection_aet(this);

    if (changed_view)
        camera_calculate_view(this);

    if (changed_proj || changed_view) {
        mat4_mult(&view, &projection, &view_projection);
        mat4_inverse(&view_projection, &inv_view_projection);
    }

    changed_proj = false;
    changed_proj_aet = false;
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

    float_t fov_correct_height = (float_t)(((double_t)c->render_height * 0.5) / tan(c->fov_rad * 0.5));

    float_t height = (float_t)c->render_height * 0.5f;
    float_t width = (float_t)c->render_height * (float_t)c->aspect * 0.5f;

    c->field_1E4.x = fov_correct_height;
    c->field_1E4.y = 0.0f;
    c->field_1E4.z = -width;
    c->field_1E4 = vec3::normalize(c->field_1E4);
    c->field_1F0.x = -fov_correct_height;
    c->field_1F0.y = 0.0f;
    c->field_1F0.z = -width;
    c->field_1F0 = vec3::normalize(c->field_1F0);
    c->field_1FC.x = 0.0f;
    c->field_1FC.y = fov_correct_height;
    c->field_1FC.z = -height;
    c->field_1FC = vec3::normalize(c->field_1FC);
    c->field_208.x = 0.0f;
    c->field_208.y = -fov_correct_height;
    c->field_208.z = -height;
    c->field_208 = vec3::normalize(c->field_208);
}

static void camera_calculate_projection_aet(camera* c) {
    float_t sprite_half_width = (float_t)c->sprite_width * 0.5f;
    float_t sprite_half_height = (float_t)c->sprite_height * 0.5f;
    float_t render_half_width = (float_t)c->render_width * 0.5f;
    float_t render_half_height = (float_t)c->render_height * 0.5f;

    float_t aet_depth = sprite_half_height / (tanf(0.34557518363f) * 0.75f);
    c->aet_depth = aet_depth;

    float_t fov_correct_height = (float_t)(sprite_half_height / tan(c->fov_rad * 0.5));
    c->fov_correct_height = fov_correct_height;

    float_t v8a = (float_t)c->min_distance / aet_depth * sprite_half_width;
    float_t v8b = (float_t)c->min_distance / aet_depth * sprite_half_height;

    mat4 spr_2d_proj;
    mat4_frustrum(-v8a, v8a, v8b, -v8b, c->min_distance, 3000.0, &spr_2d_proj);

    vec3 spr_2d_viewpoint = { sprite_half_width, sprite_half_height, aet_depth };
    vec3 spr_2d_interest = { sprite_half_width, sprite_half_height, 0.0f };
    vec3 spr_2d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_2d_view;
    mat4_look_at(&spr_2d_viewpoint, &spr_2d_interest, &spr_2d_up, &spr_2d_view);

    mat4_mult(&spr_2d_view, &spr_2d_proj, &c->projection_aet_2d);

    float_t v13a = (float_t)c->min_distance / aet_depth * render_half_width;
    float_t v13b = (float_t)c->min_distance / aet_depth * render_half_height;

    mat4 spr_3d_proj;
    mat4_frustrum(-v13a, v13a, v13b, -v13b, c->min_distance, 3000.0, &spr_3d_proj);

    vec3 spr_3d_viewpoint = { render_half_width, render_half_height, render_half_height };
    vec3 spr_3d_interest = { render_half_width, render_half_height, 0.0f };
    vec3 spr_3d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_3d_view;
    mat4_look_at(&spr_3d_viewpoint, &spr_3d_interest, &spr_3d_up, &spr_3d_view);

    mat4_mult(&spr_3d_view, &spr_3d_proj, &c->projection_aet_3d);
}

static void camera_calculate_view(camera* c) {
    vec3 direction = c->view_point - c->interest;

    vec3 rotation;
    rotation.x = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    rotation.y = atan2f(direction.x, direction.z);
    rotation.z = (float_t)(c->roll * DEG_TO_RAD);

    c->rotation = rotation;
    rotation = -rotation;

    vec3 view_point = -c->view_point;

    mat4_rotate_z(rotation.z, &c->view);
    mat4_rotate_x_mult(&c->view, rotation.x, &c->view);
    mat4_rotate_y_mult(&c->view, rotation.y, &c->view);
    mat4_translate_mult(&c->view, &view_point, &c->view);

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
