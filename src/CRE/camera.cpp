/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.hpp"
#include "render_context.hpp"
#include "resolution_mode.hpp"

static void camera_calculate_projection(camera* c);
static void camera_calculate_view(camera* c);
static void camera_calculate_forward(camera* c);

camera::camera() : forward(), rotation(), view_point(), interest(), depth(), aet_depth(),
use_up(), field_1E4(), field_1F0(), field_1FC(), field_208(), yaw(), pitch(), roll(),
aspect(), fov(), aet_fov(), max_distance(), min_distance(),
proj_left_offset(), proj_right_offset(), proj_bottom_offset(), proj_top_offset(),
fast_change(), fast_change_hist0(), fast_change_hist1(), ignore_fov(), ignore_min_dist() {

}

camera::~camera() {

}

void camera::initialize(double_t aspect) {
    this->aspect = aspect;
    view = mat4_identity;
    inv_view = mat4_identity;
    projection = mat4_identity;
    inv_projection = mat4_identity;
    view_projection = mat4_identity;
    inv_view_projection = mat4_identity;
    view_projection_prev = mat4_identity;
    inv_view_projection_prev = mat4_identity;
    ignore_fov = false;
    ignore_min_dist = false;

    set_view_point({ 0.0f, 0.0f, 0.0f });
    set_interest({ 0.0f, 0.0f, -1.0f });

    reset();
}

float_t camera::get_min_distance() {
    return min_distance;
}

void camera::set_min_distance(float_t value) {
    if (!ignore_min_dist)
        min_distance = value;
}

float_t camera::get_max_distance() {
    return max_distance;
}

void camera::set_max_distance(float_t value) {
    max_distance = value;
}

double_t camera::get_aspect() {
    return aspect;
}

void camera::set_aspect(double_t value) {
    aspect = value;
}

float_t camera::get_fov() {
    return fov;
}

void camera::set_fov(float_t value) {
    fov = clamp_def(value, 1.0f, 180.0f);
}

float_t camera::get_pitch() {
    return pitch;
}

void camera::set_pitch(float_t value) {
    value = fmodf(value, 360.0f);
    pitch = clamp_def(value, -89.5f, 89.5f);
}

float_t camera::get_yaw() {
    return yaw;
}

void camera::set_yaw(float_t value) {
    yaw = fmodf(value, 360.0f);
}

float_t camera::get_roll() {
    return roll;
}

void camera::set_roll(float_t value) {
    roll = fmodf(value, 360.0f);
}

void camera::get_view_point(vec3& value) {
    value = view_point;
}

void camera::get_view_point(vec4& value) {
    *(vec3*)&value = view_point;
    value.w = 0.0f;
}

void camera::set_view_point(const vec3& value) {
    view_point = value;
}

void camera::set_view_point(const vec3&& value) {
    view_point = value;
}

void camera::get_interest(vec3& value) {
    value = interest;
}

void camera::set_interest(const vec3& value) {
    interest = value;
}

void camera::set_interest(const vec3&& value) {
    interest = value;
}

void camera::get_up(bool& use_up, vec3& value) {
    use_up = this->use_up;
    value = up;
}

void camera::set_up(bool use_up, const vec3& value) {
    this->use_up = use_up;
    up = value;
}

void camera::set_up(bool use_up, const vec3&& value) {
    this->use_up = use_up;
    up = value;
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
    set_min_distance(0.05f);
    set_max_distance(6000.0f);
    fast_change = false;
    fast_change_hist0 = false;
    fast_change_hist1 = false;
    use_up = false;
    up = { 0.0f, 1.0f, 0.0f };
    camera_calculate_forward(this);
    set_position(0.0f);
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
    camera_calculate_projection(this);
    camera_calculate_view(this);

    mat4_mul(&view, &projection, &view_projection);
    mat4_invert(&view_projection, &inv_view_projection);
}

static void camera_calculate_forward(camera* c) {
    c->forward.x = (float_t)(cos(c->pitch * DEG_TO_RAD) * sin(c->yaw * DEG_TO_RAD));
    c->forward.y = (float_t)sin(c->pitch * DEG_TO_RAD);
    c->forward.z = (float_t)(cos(c->pitch * DEG_TO_RAD) * -cos(c->yaw * DEG_TO_RAD));
}

static void camera_calculate_projection(camera* c) {
    resolution_struct* res_wind = res_window_get();
    resolution_struct* res_wind_int = res_window_internal_get();

    const float_t sprite_half_width = (float_t)res_wind->width * 0.5f;
    const float_t sprite_half_height = (float_t)res_wind->height * 0.5f;
    const float_t render_half_width = (float_t)res_wind_int->width * 0.5f;
    const float_t render_half_height = (float_t)res_wind_int->height * 0.5f;

    const float_t aet_fov = atanf(tanf((float_t)(39.6 * 0.5 * DEG_TO_RAD)) * 0.75f) * 2.0f * RAD_TO_DEG_FLOAT;
    c->aet_fov = aet_fov;

    const float_t aet_depth = sprite_half_height / tanf(c->aet_fov * 0.5f * DEG_TO_RAD_FLOAT);
    c->aet_depth = aet_depth;

    const float_t depth = sprite_half_height / tanf(c->fov * 0.5f * DEG_TO_RAD_FLOAT);
    c->depth = depth;

    float_t spr_2d_range = c->min_distance / aet_depth;
    float_t spr_2d_range_x = spr_2d_range * sprite_half_width;
    float_t spr_2d_range_y = spr_2d_range * sprite_half_height;

    mat4 spr_2d_proj;
    mat4_frustrum(-spr_2d_range_x, spr_2d_range_x, spr_2d_range_y, -spr_2d_range_y,
        c->min_distance, 3000.0f, &spr_2d_proj);

    vec3 spr_2d_viewpoint = { sprite_half_width, sprite_half_height, aet_depth };
    vec3 spr_2d_interest = { sprite_half_width, sprite_half_height, 0.0f };
    vec3 spr_2d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_2d_view;
    mat4_look_at(&spr_2d_viewpoint, &spr_2d_interest, &spr_2d_up, &spr_2d_view);

    mat4_mul(&spr_2d_view, &spr_2d_proj, &c->view_projection_aet_2d);

    float_t aet_3d_depth = render_half_height / tanf(c->aet_fov * 0.5f * DEG_TO_RAD_FLOAT);

    float_t spr_3d_range = c->min_distance / aet_3d_depth;
    float_t spr_3d_range_x = spr_3d_range * render_half_width;
    float_t spr_3d_range_y = spr_3d_range * render_half_height;

    mat4 spr_3d_proj;
    mat4_frustrum(-spr_3d_range_x, spr_3d_range_x, spr_3d_range_y, -spr_3d_range_y,
        c->min_distance, 3000.0f, &spr_3d_proj);

    vec3 spr_3d_viewpoint = { render_half_width, render_half_height, aet_3d_depth };
    vec3 spr_3d_interest = { render_half_width, render_half_height, 0.0f };
    vec3 spr_3d_up = { 0.0f, 1.0f, 0.0f };
    mat4 spr_3d_view;
    mat4_look_at(&spr_3d_viewpoint, &spr_3d_interest, &spr_3d_up, &spr_3d_view);

    mat4_mul(&spr_3d_view, &spr_3d_proj, &c->view_projection_aet_3d);

    extern render_context* rctx_ptr;
    vec2 persp_scale = 1.0f;
    vec2 persp_offset = rctx_ptr->render.get_taa_offset();
    mat4_persp_offset(c->fov * DEG_TO_RAD_FLOAT, (float_t)c->aspect,
        c->min_distance, c->max_distance, &persp_scale, &persp_offset, &c->projection);
    mat4_invert(&c->projection, &c->inv_projection);

    float_t height = (float_t)res_wind_int->height * 0.5f;
    float_t width = (float_t)res_wind_int->height * (float_t)c->aspect * 0.5f;

    c->field_1E4 = vec3::normalize({  c->depth, 0.0f, -width });
    c->field_1F0 = vec3::normalize({ -c->depth, 0.0f, -width });
    c->field_1FC = vec3::normalize({ 0.0f,  c->depth, -height });
    c->field_208 = vec3::normalize({ 0.0f, -c->depth, -height });
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

cam_data::cam_data() : view_point(0.0f, 0.0f, 1.0f), interest(), up(0.0f, 1.0f, 0.0f),
    fov(30.0f * DEG_TO_RAD_FLOAT), aspect(4.0f / 3.0f), min_distance(1.0f), max_distance(200.0f),
    view_mat(), proj_mat(), view_proj_mat(), persp_scale(), persp_offset() {

}

void cam_data::calc_ortho_proj_mat(float_t left, float_t right,
    float_t bottom, float_t top, const vec2& scale, const vec2& offset) {
    mat4_ortho_offset(left, right, bottom, top, min_distance, max_distance, &scale, &offset, &proj_mat);
}

void cam_data::calc_persp_proj_mat() {
    mat4_persp(fov, aspect, min_distance, max_distance, &proj_mat);
    persp_scale = 1.0f;
    persp_offset = 0.0f;
}

void cam_data::calc_persp_proj_mat_offset(const vec2& persp_scale, const vec2& persp_offset) {
    mat4_persp_offset(fov, aspect, min_distance, max_distance, &persp_scale, &persp_offset, &proj_mat);
    this->persp_scale = persp_scale;
    this->persp_offset = persp_offset;
}

void cam_data::calc_view_mat() {
    mat4_look_at(&view_point, &interest, &up, &view_mat);
}

void cam_data::calc_view_proj_mat() {
    mat4_mul(&view_mat, &proj_mat, &view_proj_mat);
}

void cam_data::get(const camera* cam) {
    view_point = cam->view_point;
    interest = cam->interest;
    up = cam->up;
    fov = cam->fov * DEG_TO_RAD_FLOAT;
    aspect = (float_t)cam->aspect;
    min_distance = cam->min_distance;
    max_distance = cam->max_distance;
    view_mat = cam->view;
    proj_mat = cam->projection;
    view_proj_mat = cam->view_projection;
    persp_scale = 1.0f;
    persp_offset = 0.0f;
}

float_t cam_data::get_aspect() const {
    return aspect;
}

float_t cam_data::get_fov() const {
    return fov;
}

const vec3& cam_data::get_interest() const {
    return interest;
}

float_t cam_data::get_min_distance() const {
    return min_distance;
}

float_t cam_data::get_max_distance() const {
    return max_distance;
}

const mat4& cam_data::get_proj_mat() const {
    return proj_mat;
}

const mat4& cam_data::get_view_mat() const {
    return view_mat;
}

const vec3& cam_data::get_view_point() const {
    return view_point;
}

const mat4& cam_data::get_view_proj_mat() const {
    return view_proj_mat;
}

void cam_data::set_aspect(float_t value) {
    aspect = value;
}

void cam_data::set_fov(float_t value) {
    fov = value;
}

void cam_data::set_interest(const vec3& value) {
    interest = value;
}

void cam_data::set_min_distance(float_t value) {
    min_distance = value;
}

void cam_data::set_max_distance(float_t value) {
    max_distance = value;
}

void cam_data::set_up(const vec3& value) {
    up = value;
}

void cam_data::set_view_point(const vec3& value) {
    view_point = value;
}
