/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"

struct camera {
    mat4 view;
    mat4 inv_view;
    mat4 projection;
    mat4 inv_projection;
    mat4 view_projection;
    mat4 inv_view_projection;
    mat4 view_projection_prev;
    mat4 inv_view_projection_prev;
    mat3 view_mat3;
    mat3 inv_view_mat3;
    mat4 view_rot;
    mat4 inv_view_rot;
    mat4 view_projection_aet_2d;
    mat4 view_projection_aet_3d;
    float_t fov_correct_height;
    float_t aet_depth;
    vec3 forward;
    vec3 rotation;
    vec3 view_point;
    vec3 interest;
    bool use_up;
    vec3 up;
    vec3 field_1E4;
    vec3 field_1F0;
    vec3 field_1FC;
    vec3 field_208;
    float_t yaw;
    float_t pitch;
    float_t roll;
    double_t aspect;
    float_t fov;
    float_t fov_rad;
    float_t max_distance;
    float_t min_distance;
    bool changed_view;
    bool changed_proj;
    bool changed_proj_aet;
    bool fast_change;
    bool fast_change_hist0;
    bool fast_change_hist1;
    bool ignore_fov;
    bool ignore_min_dist;

    camera();
    ~camera();

    void initialize(double_t aspect);
    float_t get_min_distance();
    void set_min_distance(float_t value);
    float_t get_max_distance();
    void set_max_distance(float_t value);
    double_t get_aspect();
    void set_aspect(double_t value);
    float_t get_fov();
    void set_fov(float_t value);
    float_t get_pitch();
    void set_pitch(float_t value);
    float_t get_yaw();
    void set_yaw(float_t value);
    float_t get_roll();
    void set_roll(float_t value);
    void get_view_point(vec3& value);
    void get_view_point(vec4& value);
    void set_view_point(const vec3& value);
    void set_view_point(const vec3&& value);
    void get_interest(vec3& value);
    void set_interest(const vec3& value);
    void set_interest(const vec3&& value);
    void get_up(bool& use_up, vec3& value);
    void set_up(bool use_up, const vec3& value);
    void set_up(bool use_up, const vec3&& value);
    void set_fast_change(bool value);
    void set_fast_change_hist0(bool value);
    void set_fast_change_hist1(bool value);
    void set_ignore_fov(bool value);
    void set_ignore_min_dist(bool value);
    void reset();
    void move(float_t move_x, float_t move_y);
    void rotate(float_t rotate_x, float_t rotate_y);
    void set(const vec3& view_point, const vec3& interest,
        const vec3& trans, const vec3& rot, const vec3& scale, float_t roll, float_t fov);
    void set(const vec3&& view_point, const vec3&& interest,
        const vec3&& trans, const vec3&& rot, const vec3&& scale, float_t roll, float_t fov);
    void set_position(const vec3& pos);
    void set_position(const vec3&& pos);
    void update();
    void update_data();
};

struct cam_struct {
    bool use_up;
    vec3 view_point;
    vec3 interest;
    float_t fov;
    float_t roll;
    vec3 up;
    float_t min_distance;

    cam_struct();
    void get(camera* cam);
    void set(camera* cam);
};
