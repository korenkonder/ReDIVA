/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "camera.hpp"
#include "../KKdLib/obj.hpp"
#include "render.hpp"
#include "render_context.hpp"
#include "screen_param.hpp"

static void calc_camera_matrix(CameraData* data);
static void calc_frustum_plane_normal(CameraData* data);
static int32_t frustum_plane_check(const vec4* plane, const vec4* vtx, int32_t count);
static void init_projection_matrix(CameraData* data);

static void calc_camera_forward(CameraData* data); // Added

// 0x1401F75B0
CameraData::CameraData() : pos(), intr(), rot_z(), pers(), pers_2d(), aspect(), clip_near(), clip_far(),
frustum_left_offset(), frustum_right_offset(), frustum_bottom_offset(), frustum_top_offset(), use_up(), up(),
ignore_pers(), ignore_near_clip(), cmat(), imat(), pmat(), vpmat(), imat3(), vpmat_2d(), vpmat_pre2d(),
fv(), fv_2d(), fpn_left(), fpn_right(), fpn_bottom(), fpn_top(), distance(), rot(), pers_tan(), portrait(),
discontinuity(), discontinuity2(), discontinuity3(), forward(), yaw(), pitch() {

}

// 0x1401F8A20
double_t CameraData::get_aspect() const {
    return aspect;
}

// 0x1401F8A30
float_t CameraData::get_far_clip() const {
    return clip_far;
}

// 0x1401F8A40
float_t CameraData::get_fv_2d() const {
    return fv_2d;
}

// 0x1401F8C10
bool CameraData::get_ignore_near_clip() const {
    return ignore_near_clip;
}

// 0x1401F8C20
bool CameraData::get_ignore_pers() const {
    return ignore_pers;
}

// 0x1401F8A50
void CameraData::get_imatrix(mat4* imat) const {
    if (imat)
        *imat = this->imat;
}

// 0x1401F8A70
vec3 CameraData::get_intr() const {
    return intr;
}

// 1401F8AA0
void CameraData::get_matrix(mat4* cmat, mat4* pmat, mat4* vpmat) const {
    if (cmat)
        *cmat = this->cmat;
    if (pmat)
        *pmat = this->pmat;
    if (vpmat)
        *vpmat = this->vpmat;
}

// 0x1401F8B00
float_t CameraData::get_near_clip() const {
    return clip_near;
}

// 0x1401F8B10
float_t CameraData::get_pers() const {
    return pers;
}

// 0x1401F8B20
vec3 CameraData::get_pos() const {
    return pos;
}

// 0x1401F8B50
float_t CameraData::get_rot_y() const {
    return rot.y * RAD_TO_DEG_FLOAT;
}

// 0x1401F8B70
float_t CameraData::get_rot_z() const {
    return rot_z;
}

// 0x1401F8B80
vec3 CameraData::get_up() const {
    return up;
}

// 0x1401F9390
void CameraData::set_aspect(double_t in_aspect) {
    aspect = in_aspect;
}

// 0x1401F93B0
void CameraData::set_discontinuity() {
    discontinuity = true;
}

// 0x1401F93A0
void CameraData::set_discontinuity2() {
    discontinuity2 = true;
}

// 0x1401F93C0
void CameraData::set_frustum_offset(
    float_t left_offset, float_t right_offset, float_t bottom_offset, float_t top_offset) {
    frustum_left_offset = left_offset;
    frustum_right_offset = right_offset;
    frustum_bottom_offset = bottom_offset;
    frustum_top_offset = top_offset;
}

// Missing
void CameraData::set_far_clip(float_t in_far_clip) {
    clip_far = in_far_clip;
}

// 0x1401F9570
void CameraData::set_ignore_near_clip(bool in_ignore_near_clip) {
    ignore_near_clip = in_ignore_near_clip;
}

// 0x1401F9580
void CameraData::set_ignore_pers(bool in_ignore_pers) {
    ignore_pers = in_ignore_pers;
}

// 0x1401F93F0
void CameraData::set_intr(const vec3& in_intr) {
    intr = in_intr;
}

// 0x1401F9410
void CameraData::set_near_clip(float_t in_near_clip) {
    if (!ignore_near_clip)
        clip_near = in_near_clip;
}

// 0x1401F9450
void CameraData::set_portrait(bool in_portrait) {
    portrait = in_portrait;
}

// 0x1401F9430
void CameraData::set_pers(float_t in_pers) {
    if (!ignore_pers)
        pers = in_pers;
}

// 0x1401F9460
void CameraData::set_pos(const vec3& in_pos) {
    pos = in_pos;
}

// 0x1401F9480
void CameraData::set_rot_z(float_t in_rot_z) {
    rot_z = in_rot_z;
}

// 0x1401F9490
void CameraData::set_up(bool in_use_up, const vec3& in_up) {
    use_up = in_use_up;
    up = in_up;
}

// 0x1401F8C70
void CameraData::init() {
    pos = { 0.0f, 0.0f, 10.0f };
    intr = { 0.0f, 0.0f, 0.0f };
    rot_z = 0.0f;
    clip_near = 0.05f;
    clip_far = 6000.0f;
    frustum_left_offset = 0.0f;
    frustum_right_offset = 0.0f;
    frustum_bottom_offset = 0.0f;
    frustum_top_offset = 0.0f;
    use_up = false;
    up = { 0.0f, 1.0f, 0.0f };
    ignore_pers = false;
    ignore_near_clip = false;

    pers = 32.2673416137695f;
    pers_2d = atanf(tanf((float_t)(39.6 * 0.5 * DEG_TO_RAD)) * 0.75f) * 2.0f * RAD_TO_DEG_FLOAT;
    pers_tan = tanf(pers * 0.5f * DEG_TO_RAD_FLOAT);

    const ScreenParam& screen_param = get_screen_param();
    const ScreenParam& render_screen_param = get_render_screen_param();

    const float_t half_height = (float_t)screen_param.height * 0.5f;
    const float_t render_half_height = (float_t)render_screen_param.height * 0.5f;

    aspect = render_screen_param.aspect;
    fv = render_half_height / tanf(pers * 0.5f * DEG_TO_RAD_FLOAT);
    fv_2d = half_height / tanf(pers_2d * 0.5f * DEG_TO_RAD_FLOAT);

    const vec3 dir = pos - intr;
    distance = vec3::length(dir);

    rot.x = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
    rot.y = atan2f(dir.x, dir.z);
    rot.z = rot_z * DEG_TO_RAD_FLOAT;

    portrait = false;
    discontinuity = false;
    discontinuity2 = false;
    discontinuity3 = false;

    // Added
    cmat = mat4_identity;
    imat = mat4_identity;
    pmat = mat4_identity;
    vpmat = mat4_identity;
    imat3 = mat3_identity;
    vpmat_2d = mat4_identity;
    vpmat_pre2d = mat4_identity;

    calc_camera_forward(this);
}

// 0x1401F8970
void CameraData::ctrl() {
    init_projection_matrix(this);
    calc_camera_matrix(this);

    discontinuity3 = discontinuity2;
    discontinuity2 = discontinuity;
    discontinuity = false;
}

// 0x1401F9590
const mat4& CameraData::set_projection_matrix_2d(bool is_pre2d) {
    if (is_pre2d)
        return vpmat_pre2d;
    return vpmat_2d;
}

// Added
void CameraData::move(float_t move_x, float_t move_y) {
    if (move_x != 0.0f || move_y != 0.0f) {
        vec3 up = { 0.0f, 1.0f, 0.0f };
        vec3 pos = this->pos;
        pos += vec3::normalize(vec3::cross(forward, up)) * (float_t)move_y;
        pos += forward * (float_t)move_x;
        set_pos(pos);
        set_intr(pos + forward);
    }
}

// Added
void CameraData::rotate(float_t rotate_x, float_t rotate_y) {
    if (rotate_x != 0.0f)
        set_yaw(get_yaw() + rotate_x);

    if (rotate_y != 0.0f)
        set_pitch(get_pitch() + rotate_y);

    if (rotate_x != 0.0f || rotate_y != 0.0f) {
        calc_camera_forward(this);

        set_intr(pos + forward);
    }
}

// Added
void CameraData::set_pos_forward(const vec3& pos) {
    set_pos(pos);
    set_intr(pos + forward);
}

// Added
float_t CameraData::get_pitch() const {
    return pitch;
}

// Added
void CameraData::set_pitch(float_t value) {
    value = fmodf(value, 360.0f);
    pitch = clamp_def(value, -89.5f, 89.5f);
}

// Added
float_t CameraData::get_yaw() const {
    return yaw;
}

// Added
void CameraData::set_yaw(float_t value) {
    yaw = fmodf(value, 360.0f);
}

// Added
float_t CameraData::get_roll() const {
    return rot_z;
}

// Added
void CameraData::set_roll(float_t value) {
    rot_z = fmodf(value, 360.0f);
}

// 0x1401F8C30
void CameraParam::init() {
    use_up = false;
    view_point = { 0.0f, 1.0f, 6.0f };
    interest = { 0.0f, 1.0f, 0.0f };
    v_fov = (float_t)(32.2673416137695 * DEG_TO_RAD);
    roll = 0.0f;
    up = { 0.0f, 1.0f, 0.0f };
    clip_near = 0.05f;
}

// 0x1401F8BA0
void CameraParam::get_from_camera(CameraData* data) {
    view_point = data->get_pos();
    interest = data->get_intr();
    v_fov = data->get_pers() * DEG_TO_RAD_FLOAT;
    roll = data->get_rot_z() * DEG_TO_RAD_FLOAT;
}

// 0x1401F9730
void CameraParam::set_to_camera(CameraData* data) {
    data->set_pos(view_point);
    data->set_intr(interest);
    data->set_pers(v_fov * RAD_TO_DEG_FLOAT);
    data->set_rot_z(roll * RAD_TO_DEG_FLOAT);
    data->set_up(use_up, up);
    data->set_near_clip(clip_near);
}

// 0x1401F9360
void CameraParam::reverse_side() {
    roll = -roll;
    view_point.x = -view_point.x;
    interest.x = -interest.x;
}

// 0x1401F7D60
vec2 calc_screen_pos(const mat4* cmat, float_t fv,
    const vec3* pos, bool with_render_offset) {
    vec2 sc_pos;
    vec3 cpos;
    mat4_transform_point(cmat, pos, &cpos);
    if (fabsf(cpos.z) >= 1.0e-10f) {
        float_t pers_div = 1.0f / cpos.z;
        sc_pos.x = cpos.x * fv * pers_div;
        sc_pos.y = cpos.y * fv * pers_div;
        const ScreenParam& render_screen_param = get_render_screen_param();
        sc_pos.x = (float_t)render_screen_param.width * 0.5f - sc_pos.x;
        sc_pos.y = (float_t)render_screen_param.height * 0.5f + sc_pos.y;
        if (with_render_offset) {
            const ScreenParam& screen_param = get_screen_param();
            sc_pos.x += (float_t)render_screen_param.xoffset;
            sc_pos.y += (float_t)(screen_param.height - render_screen_param.yoffset - render_screen_param.height);
        }
    }
    else
        sc_pos = 0.0f;
    return sc_pos;
}

// 0x1401F7EB0
vec3 calc_screen_pos(const mat4* vpmat, float_t fv,
    const vec3* pos, float_t r, bool with_render_offset) {
    vec3 sc_pos;
    calc_screen_pos_r(vpmat, fv, &sc_pos, pos, 0.0f, with_render_offset);
    return sc_pos;
}

// 0x1401F7EE0
float_t calc_screen_pos_r(const mat4* vpmat, float_t fv,
    vec3* sc_pos, const vec3* pos, float_t r, bool with_render_offset) {
    vec4 vppos = vec4(pos->x, pos->y, pos->z, 1.0f);
    mat4_transform_vector(vpmat, &vppos, &vppos);
    if (fabsf(vppos.w) >= 1.0e-10f) {
        float_t pers_div = 1.0f / vppos.w;
        float_t v11 = vppos.y * pers_div;
        float_t v12 = vppos.x * pers_div;
        const ScreenParam& render_screen_param = get_render_screen_param();
        float_t v14 = ((1.0f + v12) * 0.5f) * (float_t)render_screen_param.width;
        float_t v15 = ((1.0f - v11) * 0.5f) * (float_t)render_screen_param.height;
        if (with_render_offset) {
            const ScreenParam& screen_param = get_screen_param();
            v14 += (float_t)render_screen_param.xoffset;
            v15 += (float_t)(screen_param.height - render_screen_param.yoffset - render_screen_param.height);
        }
        sc_pos->x = v14;
        sc_pos->y = v15;
        sc_pos->z = -vppos.w;
        return fabsf(pers_div) * (fv * r);
    }

    *sc_pos = 0.0f;
    return 0.0f;
}

// 0x1401F8130
int32_t check_screen_aabb(const obj_axis_aligned_bounding_box* aabb, CameraData* cam, const mat4& mat) {
    vec3 points[8];
    points[0] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f,  0.0f));
    points[1] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, -0.0f));
    points[2] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f,  0.0f));
    points[3] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f, -0.0f));
    points[4] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f,  0.0f));
    points[5] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f, -0.0f));
    points[6] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f, -0.0f));
    points[7] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f,  0.0f));

    mat4 view_mat;
    mat4_mul(&mat, &cam->cmat, &view_mat);

    vec4 vtx[8];
    for (int32_t i = 0; i < 8; i++) {
        mat4_transform_point(&view_mat, &points[i], (vec3*)&vtx[i]);
        vtx[i].w = 1.0f;
    }

    vec4 plane[6];
    *(vec3*)&plane[0] = { 0.0f, 0.0f, -1.0f };
    plane[0].w = -cam->clip_near;
    *(vec3*)&plane[1] = cam->fpn_left;
    plane[1].w = 0.0f;
    *(vec3*)&plane[2] = cam->fpn_right;
    plane[2].w = 0.0f;
    *(vec3*)&plane[3] = cam->fpn_bottom;
    plane[3].w = 0.0f;
    *(vec3*)&plane[4] = cam->fpn_top;
    plane[4].w = 0.0f;
    *(vec3*)&plane[5] = { 0.0f, 0.0f, 1.0f };
    plane[5].w = cam->clip_far;

    for (int32_t i = 0; i < 6; i++)
        if (!frustum_plane_check(&plane[i], vtx, 8))
            return 0;
    return 1;
}

// 0x1401F9340
void project_screen(const mat4* vpmat, float_t fv,
    vec2* pos2d, const vec3* pos3d, bool with_render_offset) {
    project_screen(vpmat, fv, pos2d, pos3d, 0.0f, with_render_offset);
}

// 0x1401F92E0
void project_screen(const mat4* vpmat, float_t fv,
    vec2* pos2d, const vec3* pos3d, float_t r, bool with_render_offset) {
    vec3 pos = *pos3d;
    vec3 sc_pos;
    calc_screen_pos_r(vpmat, fv, &pos, &sc_pos, r, with_render_offset);
    *pos2d = vec2(sc_pos.x, sc_pos.y);
}

// 0x1401F7690
static void calc_camera_matrix(CameraData* data) {
    if (data->use_up) {
        mat4_translate(&data->pos, &data->imat);
        vec3 z_axis = vec3::normalize(data->pos - data->intr);
        vec3 x_axis = vec3::normalize(vec3::cross(data->up, z_axis));
        vec3 y_axis = vec3::normalize(vec3::cross(z_axis, x_axis));

        *(vec3*)&data->imat.row0 = x_axis;
        *(vec3*)&data->imat.row1 = y_axis;
        *(vec3*)&data->imat.row2 = z_axis;
        mat4_invert(&data->imat, &data->cmat);
        mat4_get_rotation_zyx(&data->imat, &data->rot);
    }
    else {
        const vec3 dir = data->pos - data->intr;
        data->distance = vec3::length(dir);

        data->rot.x = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
        data->rot.y = atan2f(dir.x, dir.z);
        data->rot.z = data->rot_z * DEG_TO_RAD_FLOAT;

        data->cmat = mat4_identity;
        mat4_mul_rotate_z(&data->cmat, data->portrait
            ? (float_t)(M_PI_2 - data->rot.z) : -data->rot.z, &data->cmat);
        mat4_mul_rotate_x(&data->cmat, -data->rot.x, &data->cmat);
        mat4_mul_rotate_y(&data->cmat, -data->rot.y, &data->cmat);
        mat4_mul_translate(&data->cmat, -data->pos.x, -data->pos.y, -data->pos.z, &data->cmat);
        mat4_invert(&data->cmat, &data->imat);
    }

    mat4_mul(&data->cmat, &data->pmat, &data->vpmat);

    const ScreenParam& render_screen_param = get_render_screen_param();

    const float_t render_half_height = (float_t)render_screen_param.height * 0.5f;

    data->fv = render_half_height / tanf(data->pers * 0.5f * DEG_TO_RAD_FLOAT);

    calc_frustum_plane_normal(data);

    // Added
    mat4_to_mat3(&data->cmat, &data->imat3);
    mat3_invert(&data->imat3, &data->imat3);

    if (fabsf(data->rot.x * RAD_TO_DEG_FLOAT - data->pitch) > 0.01f
        || fabsf(-data->rot.y * RAD_TO_DEG_FLOAT - data->yaw) > 0.01f) {
        data->pitch = data->rot.x * RAD_TO_DEG_FLOAT;
        data->yaw = -data->rot.y * RAD_TO_DEG_FLOAT;
        calc_camera_forward(data);
    }
}

// 0x1401F7B20
static void calc_frustum_plane_normal(CameraData* data) {
    const ScreenParam& render_screen_param = get_render_screen_param();

    float_t height = (float_t)render_screen_param.height * 0.5f;
    float_t width = (float_t)render_screen_param.height * (float_t)data->aspect * 0.5f;

    data->fpn_left = { data->fv, 0.0f, -width };
    data->fpn_left = vec3::normalize(data->fpn_left);
    data->fpn_right = { -data->fv, 0.0f, -width };
    data->fpn_right = vec3::normalize(data->fpn_right);
    data->fpn_bottom = { 0.0f, data->fv, -height };
    data->fpn_bottom = vec3::normalize(data->fpn_bottom);
    data->fpn_top = { 0.0f, -data->fv, -height };
    data->fpn_top = vec3::normalize(data->fpn_top);
}

// 0x1401F9220
static int32_t frustum_plane_check(const vec4* plane, const vec4* vtx, int32_t count) {
    for (int32_t i = 0; i < count; i++)
        if (vec4::dot(*plane, vtx[i]) > 0.0f)
            return 1;
    return 0;
}

// 0x1401F8E90
static void init_projection_matrix(CameraData* data) {
    const ScreenParam& screen_param = get_screen_param();
    const ScreenParam& render_screen_param = get_render_screen_param();

    const float_t half_width = (float_t)screen_param.width * 0.5f;
    const float_t half_height = (float_t)screen_param.height * 0.5f;
    const float_t render_half_width = (float_t)render_screen_param.width * 0.5f;
    const float_t render_half_height = (float_t)render_screen_param.height * 0.5f;

    const float_t fv_2d = data->fv_2d;

    const float_t clip_near_2d = data->clip_near;
    const float_t clip_far_2d = 3000.0f;

    const float_t range_2d = clip_near_2d / fv_2d;
    const float_t range_x_2d = range_2d * half_width;
    const float_t range_y_2d = range_2d * half_height;

    mat4 pmat_2d;
    mat4_frustum(-range_x_2d, range_x_2d, range_y_2d, -range_y_2d,
        clip_near_2d, clip_far_2d, &pmat_2d);

    const vec3 pos_2d(half_width, half_height, fv_2d);
    const vec3 intr_2d(half_width, half_height, 0.0f);
    const vec3 up_2d(0.0f, 1.0f, 0.0f);
    mat4 vmat_2d;
    mat4_look_at(&pos_2d, &intr_2d, &up_2d, &vmat_2d);

    mat4_mul(&vmat_2d, &pmat_2d, &data->vpmat_2d);

    const float_t fv_pre2d = render_half_height / tanf(data->pers_2d * 0.5f * DEG_TO_RAD_FLOAT);

    const float_t range_pre2d = clip_near_2d / fv_pre2d;
    const float_t range_x_pre2d = range_pre2d * render_half_width;
    const float_t range_y_pre2d = range_pre2d * render_half_height;

    mat4 pmat_pre2d;
    mat4_frustum(-range_x_pre2d, range_x_pre2d, range_y_pre2d, -range_y_pre2d,
        clip_near_2d, clip_far_2d, &pmat_pre2d);

    const vec3 pos_pre2d(render_half_width, render_half_height, fv_pre2d);
    const vec3 intr_pre2d(render_half_width, render_half_height, 0.0f);
    const vec3 up_pre2d(0.0f, 1.0f, 0.0f);
    mat4 vmat_pre2d;
    mat4_look_at(&pos_pre2d, &intr_pre2d, &up_pre2d, &vmat_pre2d);

    mat4_mul(&vmat_pre2d, &pmat_pre2d, &data->vpmat_pre2d);

    extern render_context* rctx_ptr;
    vec2 persp_scale = 1.0f;
    vec2 persp_offset;
    rctx_ptr->render->perspective(persp_offset);
    mat4_persp_offset(data->pers * DEG_TO_RAD_FLOAT, (float_t)data->aspect,
        data->clip_near, data->clip_far, &persp_scale, &persp_offset, &data->pmat);

    data->pers_tan = tanf(data->pers * 0.5f * DEG_TO_RAD_FLOAT);
}

// Added
static void calc_camera_forward(CameraData* data) {
    data->forward.x = (float_t)(cos(data->pitch * DEG_TO_RAD) * sin(data->yaw * DEG_TO_RAD));
    data->forward.y = (float_t)sin(data->pitch * DEG_TO_RAD);
    data->forward.z = (float_t)(cos(data->pitch * DEG_TO_RAD) * -cos(data->yaw * DEG_TO_RAD));
}
