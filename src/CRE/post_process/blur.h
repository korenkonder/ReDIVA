/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../shared.h"
#include "../render_texture.h"

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
};

extern post_process_blur* post_process_blur_init();
extern void post_process_get_blur(post_process_blur* blur, render_texture* rt);
extern void post_process_blur_init_fbo(post_process_blur* blur,
    int32_t width, int32_t height);
extern void post_process_blur_dispose(post_process_blur* blur);

extern void post_process_blur_initialize_data(post_process_blur* blur, const vec3* radius, const vec3* intensity);
extern void post_process_blur_get_radius(post_process_blur* blur, vec3* value);
extern void post_process_blur_set_radius(post_process_blur* blur, const vec3* value);
extern void post_process_blur_get_intensity(post_process_blur* blur, vec3* value);
extern void post_process_blur_set_intensity(post_process_blur* blur, const vec3* value);
