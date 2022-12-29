/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../shared.hpp"
#include "../render_texture.hpp"

#define POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE 7

struct post_process_blur_data {
    vec3 gauss[POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE];
    vec3 radius;
    vec3 intensity;
};

struct post_process_blur {
    post_process_blur_data data;
    int32_t width;
    int32_t height;
    render_texture tex[6];
    int32_t* width_down;
    int32_t* height_down;
    render_texture* tex_down;
    int32_t count_down;

    post_process_blur();
    ~post_process_blur();

    void get_blur(render_texture* rt);
    void init_fbo(int32_t width, int32_t height);
    void initialize_data(const vec3& radius, const vec3& intensity);

    vec3 get_intensity();
    void set_intensity(const vec3& value);
    vec3 get_radius();
    void set_radius(const vec3& value);
};
