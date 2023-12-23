/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.hpp"
#include "resolution_mode.hpp"

static void camera_calculate_projection(camera* c);
static void camera_calculate_projection_aet(camera* c);
static void camera_calculate_view(camera* c);
static void camera_calculate_forward(camera* c);

camera::camera() : forward(), rotation(), view_point(), interest(), fov_correct_height(), aet_depth(),
use_up(), field_1E4(), field_1F0(), field_1FC(), field_208(), yaw(), pitch(), roll(), aspect(),
fov(), fov_rad(), max_distance(), min_distance(), changed_view(), changed_proj(), changed_proj_aet(),
fast_change(), fast_change_hist0(), fast_change_hist1(), ignore_fov(), ignore_min_dist() {

}

camera::~camera() {

}

void camera::initialize(double_t aspect) {
    this->aspect = aspect;
    fov = 0.0f;
    fov_correct_height = 0.0f;
    aet_depth = 0.0f;
    forward = { 0.0f, 0.0f, -1.0f };
    rotation = { 0.0f, 0.0f, 0.0f };
    use_up = false;
    up = { 0.0f, 1.0f, 0.0f };
    view = mat4_identity;
    inv_view = mat4_identity;
    projection = mat4_identity;
    inv_projection = mat4_identity;
    view_projection = mat4_identity;
    inv_view_projection = mat4_identity;
    view_projection_prev = mat4_identity;
    inv_view_projection_prev = mat4_identity;
    changed_proj = true;
    changed_proj_aet = true;
    changed_view = true;
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    ignore_fov = false;
    ignore_min_dist = false;

    set_view_point({ 0.0f, 0.0f, 0.0f });
    set_interest({ 0.0f, 0.0f, -1.0f });
    set_min_distance(0.05f);
    set_max_distance(6000.0f);
    set_pitch(0.0f);
    set_yaw(0.0f);
    set_roll(0.0f);
    set_fov(32.2673416137695f);
    camera_calculate_forward(this);
    set_position(0.0f);
    update();
}

float_t camera::get_min_distance() {
    return min_distance;
}

void camera::set_min_distance(float_t value) {
    if (!ignore_min_dist && min_distance != value) {
        min_distance = value;
        changed_proj = true;
        changed_proj_aet = true;
    }
}

float_t camera::get_max_distance() {
    return max_distance;
}

void camera::set_max_distance(float_t value) {
    if (max_distance != value) {
        max_distance = value;
        changed_proj = true;
    }
}

double_t camera::get_aspect() {
    return aspect;
}

void camera::set_aspect(double_t value) {
    if (aspect != value) {
        aspect = value;
        changed_proj = true;
    }
}

float_t camera::get_fov() {
    return fov;
}

void camera::set_fov(float_t value) {
    value = clamp_def(value, 1.0f, 180.0f);
    if (!ignore_fov && fov != value) {
        fov = value;
        fov_rad = value * DEG_TO_RAD_FLOAT;
        changed_proj = true;
    }
}

float_t camera::get_pitch() {
    return pitch;
}

void camera::set_pitch(float_t value) {
    value = fmodf(value, 360.0f);
    value = clamp_def(value, -89.5f, 89.5f);
    if (pitch != value) {
        pitch = value;
        changed_view = true;
    }
}

float_t camera::get_yaw() {
    return yaw;
}

void camera::set_yaw(float_t value) {
    value = fmodf(value, 360.0f);
    if (yaw != value) {
        yaw = value;
        changed_view = true;
    }
}

float_t camera::get_roll() {
    return roll;
}

void camera::set_roll(float_t value) {
    value = fmodf(value, 360.0f);
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
    if (view_point != value) {
        view_point = value;
        changed_view = true;
    }
}

void camera::set_view_point(const vec3&& value) {
    if (view_point != value) {
        view_point = value;
        changed_view = true;
    }
}

void camera::get_interest(vec3& value) {
    value = interest;
}

void camera::set_interest(const vec3& value) {
    if (interest != value) {
        interest = value;
        changed_view = true;
    }
}

void camera::set_interest(const vec3&& value) {
    if (interest != value) {
        interest = value;
        changed_view = true;
    }
}

void camera::get_up(bool& use_up, vec3& value) {
    use_up = this->use_up;
    value = up;
}

void camera::set_up(bool use_up, const vec3& value) {
    if (this->use_up != use_up) {
        this->use_up = use_up;
        changed_view = true;
    }

    if (up != value) {
        up = value;
        changed_view = true;
    }
}

void camera::set_up(bool use_up, const vec3&& value) {
    if (this->use_up != use_up) {
        this->use_up = use_up;
        changed_view = true;
    }

    if (up != value) {
        up = value;
        changed_view = true;
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

void camera::set_ignore_fov(bool value) {
    ignore_fov = value;
}

void camera::set_ignore_min_dist(bool value) {
    ignore_min_dist = value;
}

void camera::reset() {
    set_pitch(0.0f);
    set_yaw(0.0f);
    set_roll(0.0f);
    set_fov(32.2673416137695f);
    min_distance = 0.05f;
    max_distance = 6000.0f;
    changed_proj = true;
    changed_proj_aet = true;
    changed_view = true;
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    use_up = false;
    up = { 0.0f, 1.0f, 0.0f };
    camera_calculate_forward(this);
    set_position(0.0f);
    update_data();
}

void camera::move(float_t move_x, float_t move_y) {
    if (move_x != 0.0f || move_y != 0.0f) {
        vec3 up = { 0.0f, 1.0f, 0.0f };
        vec3 view_point = this->view_point;
        view_point += vec3::normalize(vec3::cross(forward, up)) * (float_t)move_y;
        view_point += forward * (float_t)move_x;
        set_view_point(view_point);
        set_interest(view_point + forward);
    }
}

void camera::rotate(float_t rotate_x, float_t rotate_y) {
    if (rotate_x != 0.0f)
        set_yaw(get_yaw() + rotate_x);

    if (rotate_y != 0.0f)
        set_pitch(get_pitch() + rotate_y);

    if (rotate_x != 0.0f || rotate_y != 0.0f) {
        camera_calculate_forward(this);

        set_interest(view_point + forward);
    }
}

void camera::set(const vec3& view_point, const vec3& interest,
    const vec3& trans, const vec3& rot, const vec3& scale, float_t roll, float_t fov) {
    vec3 _vp;
    vec3 _int;
    float_t _roll;
    float_t _fov;

    _vp = view_point;
    _int = interest;
    _roll = roll;
    _fov = fov;

    mat4 cam;
    mat4_translate(&trans, &cam);
    mat4_mul_rotate_zyx(&cam, &rot, &cam);
    mat4_scale_rot(&cam, &scale, &cam);
    mat4_transform_point(&cam, &_vp, &_vp);
    mat4_transform_point(&cam, &_int, &_int);

    set_view_point(_vp);
    set_interest(_int);
    set_roll(_roll);
    set_fov(_fov);
}

void camera::set(const vec3&& view_point, const vec3&& interest,
    const vec3&& trans, const vec3&& rot, const vec3&& scale, float_t roll, float_t fov) {
    set(view_point, interest, trans, rot, scale, roll, fov);
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
        mat4_mul(&view, &projection, &view_projection);
        mat4_invert(&view_projection, &inv_view_projection);
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
    mat4_persp(c->fov_rad, (float_t)c->aspect, c->min_distance, c->max_distance, &c->projection);
    mat4_invert(&c->projection, &c->inv_projection);

    resolution_struct* res_wind_int = res_window_internal_get();

    float_t fov_correct_height = ((float_t)res_wind_int->height * 0.5f) / tanf((float_t)(c->fov_rad * 0.5));

    float_t height = (float_t)res_wind_int->height * 0.5f;
    float_t width = (float_t)res_wind_int->height * (float_t)c->aspect * 0.5f;

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
    resolution_struct* res_wind_int = res_window_internal_get();
    resolution_struct* res_wind = res_window_get();

    float_t sprite_half_width = (float_t)res_wind->width * 0.5f;
    float_t sprite_half_height = (float_t)res_wind->height * 0.5f;
    float_t render_half_width = (float_t)res_wind_int->width * 0.5f;
    float_t render_half_height = (float_t)res_wind_int->height * 0.5f;

    static float_t field_20 = tanf(atanf(tanf(0.34557518363f) * 0.75f));

    float_t aet_depth = sprite_half_height / field_20;
    c->aet_depth = aet_depth;

    float_t fov_correct_height = sprite_half_height / tanf((float_t)c->fov_rad * 0.5f);
    c->fov_correct_height = fov_correct_height;

    float_t v8 = (float_t)c->min_distance / aet_depth;
    float_t v8a = v8 * sprite_half_width;
    float_t v8b = v8 * sprite_half_height;

    mat4 spr_2d_proj;
    mat4_frustrum(-v8a, v8a, v8b, -v8b, c->min_distance, 3000.0, &spr_2d_proj);

    vec3 spr_2d_viewpoint = { sprite_half_width, sprite_half_height, aet_depth };
    vec3 spr_2d_interest = { sprite_half_width, sprite_half_height, 0.0f };
    vec3 spr_2d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_2d_view;
    mat4_look_at(&spr_2d_viewpoint, &spr_2d_interest, &spr_2d_up, &spr_2d_view);

    mat4_mul(&spr_2d_view, &spr_2d_proj, &c->view_projection_aet_2d);

    float_t v13c = render_half_height / tanf((float_t)c->fov_rad * 0.5f);

    float_t v13 = (float_t)c->min_distance / v13c;
    float_t v13a = v13 * render_half_width;
    float_t v13b = v13 * render_half_height;

    mat4 spr_3d_proj;
    mat4_frustrum(-v13a, v13a, v13b, -v13b, c->min_distance, 3000.0, &spr_3d_proj);

    vec3 spr_3d_viewpoint = { render_half_width, render_half_height, v13c };
    vec3 spr_3d_interest = { render_half_width, render_half_height, 0.0f };
    vec3 spr_3d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_3d_view;
    mat4_look_at(&spr_3d_viewpoint, &spr_3d_interest, &spr_3d_up, &spr_3d_view);

    mat4_mul(&spr_3d_view, &spr_3d_proj, &c->view_projection_aet_3d);
}

static void camera_calculate_view(camera* c) {
    if (c->use_up) {
        mat4_translate(&c->view_point, &c->inv_view);
        vec3 z_axis = vec3::normalize(c->view_point - c->interest);
        vec3 x_axis = vec3::normalize(vec3::cross(c->up, z_axis));
        vec3 y_axis = vec3::normalize(vec3::cross(z_axis, x_axis));

        *(vec3*)&c->inv_view.row0 = x_axis;
        *(vec3*)&c->inv_view.row1 = y_axis;
        *(vec3*)&c->inv_view.row2 = z_axis;
        mat4_invert(&c->inv_view, &c->view);
        mat4_get_rotation(&c->inv_view, &c->rotation);
    }
    else {
        vec3 direction = c->view_point - c->interest;

        vec3 rotation;
        rotation.x = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
        rotation.y = atan2f(direction.x, direction.z);
        rotation.z = (float_t)(c->roll * DEG_TO_RAD);

        c->rotation = rotation;
        rotation = -rotation;

        vec3 view_point = -c->view_point;

        mat4_rotate_zxy(&rotation, &c->view);
        mat4_mul_translate(&c->view, &view_point, &c->view);
        mat4_invert(&c->view, &c->inv_view);
    }

    mat4_to_mat3(&c->view, &c->view_mat3);
    mat3_invert(&c->view_mat3, &c->inv_view_mat3);
    mat4_from_mat3(&c->view_mat3, &c->view_rot);
    mat4_from_mat3(&c->inv_view_mat3, &c->inv_view_rot);

    if (fabsf(c->rotation.x * RAD_TO_DEG_FLOAT - c->pitch) > 0.01f
        || fabsf(-c->rotation.y * RAD_TO_DEG_FLOAT - c->yaw) > 0.01f) {
        c->pitch = c->rotation.x * RAD_TO_DEG_FLOAT;
        c->yaw = -c->rotation.y * RAD_TO_DEG_FLOAT;
        camera_calculate_forward(c);
    }
}

cam_struct::cam_struct() {
    use_up = false;
    view_point = { 0.0f, 1.0f, 6.0f };
    interest = { 0.0f, 1.0f, 0.0f };
    fov = (float_t)(32.2673416137695 * DEG_TO_RAD);
    roll = 0.0f;
    up = { 0.0f, 1.0f, 0.0f };
    min_distance = 0.05f;
}

void cam_struct::get(camera* cam) {
    cam->get_view_point(view_point);
    cam->get_interest(interest);
    fov = (float_t)cam->get_fov() * DEG_TO_RAD_FLOAT;
    roll = (float_t)cam->get_roll() * DEG_TO_RAD_FLOAT;
}

void cam_struct::set(camera* cam) {
    cam->set_view_point(view_point);
    cam->set_interest(interest);
    cam->set_fov(fov * RAD_TO_DEG_FLOAT);
    cam->set_roll(roll * RAD_TO_DEG_FLOAT);
    cam->set_up(use_up, up);
    cam->set_min_distance(min_distance);
}
