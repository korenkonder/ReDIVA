/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"

struct CameraData {
    vec3 pos;
    vec3 intr;
    float_t rot_z;
    float_t pers;
    float_t pers_2d;
    double_t aspect;
    float_t clip_near;
    float_t clip_far;
    float_t frustum_left_offset;
    float_t frustum_right_offset;
    float_t frustum_bottom_offset;
    float_t frustum_top_offset;
    bool use_up;
    vec3 up;
    bool ignore_pers;
    bool ignore_near_clip;
    mat4 cmat;
    mat4 imat;
    mat4 pmat;
    mat4 vpmat;
    mat3 imat3; // Added
    mat4 vpmat_2d;
    mat4 vpmat_pre2d;
    float_t fv;
    float_t fv_2d;
    vec3 fpn_left;
    vec3 fpn_right;
    vec3 fpn_bottom;
    vec3 fpn_top;
    float_t distance;
    vec3 rot;
    float_t pers_tan;
    bool portrait;
    bool discontinuity;
    bool discontinuity2;
    bool discontinuity3;

    vec3 forward;
    float_t yaw;
    float_t pitch;

    CameraData();

    double_t get_aspect() const;
    float_t get_far_clip() const;
    float_t get_fv_2d() const;
    bool get_ignore_near_clip() const;
    bool get_ignore_pers() const;
    vec3 get_intr() const;
    void get_imatrix(mat4* imat) const;
    void get_matrix(mat4* cmat, mat4* pmat, mat4* vpmat) const;
    float_t get_near_clip() const;
    float_t get_pers() const;
    vec3 get_pos() const;
    float_t get_rot_y() const;
    float_t get_rot_z() const;
    vec3 get_up() const;

    void set_aspect(double_t in_aspect);
    void set_discontinuity();
    void set_discontinuity2();
    void set_frustum_offset(
        float_t left_offset, float_t right_offset, float_t bottom_offset, float_t top_offset);
    void set_far_clip(float_t in_far_clip);
    void set_ignore_near_clip(bool in_ignore_near_clip);
    void set_ignore_pers(bool in_ignore_pers);
    void set_intr(const vec3& in_intr);
    void set_near_clip(float_t in_near_clip);
    void set_pers(float_t in_pers);
    void set_portrait(bool in_portrait);
    void set_pos(const vec3& in_pos);
    void set_rot_z(float_t in_rot_z);
    void set_up(bool in_use_up, const vec3& in_up);

    void init();
    void ctrl();

    const mat4& set_projection_matrix_2d(bool is_pre2d);

    // Added
    void move(float_t move_x, float_t move_y);
    void rotate(float_t rotate_x, float_t rotate_y);

    float_t get_pitch() const;
    float_t get_yaw() const;
    float_t get_roll() const;

    void set_pos_forward(const vec3& pos);
    void set_pitch(float_t value);
    void set_yaw(float_t value);
    void set_roll(float_t value);
};

struct CameraParam {
    bool use_up;
    vec3 view_point;
    vec3 interest;
    float_t v_fov;
    float_t roll;
    vec3 up;
    float_t clip_near;

    inline CameraParam() : use_up(), view_point(),
        interest(), v_fov(), roll(), up(), clip_near() {
        init();
    }

    void init();
    void get_from_camera(CameraData* data);
    void set_to_camera(CameraData* data);
    void reverse_side();
};

extern vec2 calc_screen_pos(const mat4* cmat, float_t fv,
    const vec3* pos, bool with_render_offset);
extern vec3 calc_screen_pos(const mat4* vpmat, float_t fv,
    const vec3* pos, float_t r, bool with_render_offset);
extern float_t calc_screen_pos_r(const mat4* vpmat, float_t fv,
    vec3* sc_pos, const vec3* pos, float_t r, bool with_render_offset);
extern int32_t check_screen_aabb(const struct obj_axis_aligned_bounding_box* aabb,
    CameraData* cam, const mat4& mat);
extern void project_screen(const mat4* vpmat, float_t fv,
    vec2* pos2d, const vec3* pos3d, bool with_render_offset);
extern void project_screen(const mat4* vpmat, float_t fv,
    vec2* pos2d, const vec3* pos3d, float_t r, bool with_render_offset);
