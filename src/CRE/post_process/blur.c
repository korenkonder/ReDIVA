/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "blur.h"
#include "../fbo.hpp"
#include "../gl_state.h"
#include "../post_process.hpp"
#include "../shader_ft.h"

static void post_process_blur_free_fbo(post_process_blur* blur);
static void post_process_blur_radius_calculate(post_process_blur* blur);
static void post_process_blur_radius_calculate_gaussian_kernel(float_t* gaussian_kernel,
    float_t radius, int32_t stride, int32_t offset);

post_process_blur* post_process_blur_init() {
    post_process_blur* blur = force_malloc_s(post_process_blur, 1);
    return blur;
}

void post_process_get_blur(post_process_blur* blur, render_texture* rt) {
    if (!blur)
        return;

    uniform_value[U01] = 0;

    int32_t i = 0;
    if (blur->count_down > 0) {
        uniform_value[U_REDUCE] = 1;
        render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);
        for (; i < blur->count_down; i++) {
            glViewport(0, 0, blur->width_down[i], blur->height_down[i]);
            render_texture_bind(&blur->tex_down[i], 0);
            gl_state_active_bind_texture_2d(0, i
                ? blur->tex_down[i - 1].color_texture->tex
                : rt->color_texture->tex);
            render_texture_draw_params(&shaders_ft,
                blur->width_down[i], blur->height_down[i], 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        i--;
    }

    uniform_value[U_REDUCE] = 3;
    render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);
    shader_local_frag_set(&shaders_ft, 2, 1.1f, 1.1f, 1.1f, 0.0f);

    glViewport(0, 0, 256, 144);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->count_down > 0
        ? blur->tex_down[i].color_texture->tex
        : rt->color_texture->tex);
    render_texture_draw_params(&shaders_ft, blur->width, blur->height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_GAUSS] = 1;
    render_texture_shader_set(&shaders_ft, SHADER_FT_GAUSS);

    glViewport(0, 0, 256, 144);
    render_texture_bind(&blur->tex[5], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[0].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 256, 144,
        1.0f, blur->data.intensity.x * 0.5f, blur->data.intensity.y * 0.5f, blur->data.intensity.z * 0.5f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 128, 72);
    render_texture_bind(&blur->tex[1], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[0].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 256, 144, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 64, 36);
    render_texture_bind(&blur->tex[2], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[1].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 128, 72, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 32, 18);
    render_texture_bind(&blur->tex[3], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[2].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 64, 36, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_EXPOSURE] = 0;
    render_texture_shader_set(&shaders_ft, SHADER_FT_EXPOSURE);

    glViewport(0, 0, 8, 8);
    render_texture_bind(&blur->tex[4], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[3].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    vec3 intensity = blur->data.intensity;
    vec3* gauss = blur->data.gauss;

    vec3 v[POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE];
    for (int32_t i = 0; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        vec3_mult(gauss[i], intensity, v[i]);

    uniform_value[U_GAUSS] = 0;
    render_texture_shader_set(&shaders_ft, SHADER_FT_GAUSS);
    shader_local_frag_set(&shaders_ft, 1, 1.0f, 0.0f, 1.0f, 0.0f);
    for (int32_t i = 0; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        shader_local_frag_set(&shaders_ft, 4ULL + i, v[i].x, v[i].y, v[i].z, 0.0f);

    glViewport(0, 0, 128, 72);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[1].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 128, 72, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[1].fbos[0],
        0, 0, 128, 72,
        0, 0, 128, 72, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 64, 36);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[2].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 64, 36, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[2].fbos[0],
        0, 0, 64, 36,
        0, 0, 64, 36, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 32, 18);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[3].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[3].fbos[0],
        0, 0, 32, 18,
        0, 0, 32, 18, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    shader_local_frag_set(&shaders_ft, 1, 0.0f, 1.0f, 0.0f, 1.0f);

    glViewport(0, 0, 128, 72);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[1].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 128, 72, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[1].fbos[0],
        0, 0, 128, 72,
        0, 0, 128, 72, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 64, 36);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[2].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 64, 36, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[2].fbos[0],
        0, 0, 64, 36,
        0, 0, 64, 36, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 32, 18);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[3].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(blur->tex[0].fbos[0], blur->tex[3].fbos[0],
        0, 0, 32, 18,
        0, 0, 32, 18, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    uniform_value[U_REDUCE] = 7;
    render_texture_shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 256, 144);
    render_texture_bind(&blur->tex[0], 0);
    gl_state_active_bind_texture_2d(0, blur->tex[5].color_texture->tex);
    gl_state_active_bind_texture_2d(1, blur->tex[1].color_texture->tex);
    gl_state_active_bind_texture_2d(2, blur->tex[2].color_texture->tex);
    gl_state_active_bind_texture_2d(3, blur->tex[3].color_texture->tex);
    render_texture_draw_params(&shaders_ft, 32, 18, 0.25f, 0.15f, 0.25f, 0.25f, 0.25f);

}

void post_process_blur_init_fbo(post_process_blur* blur,
    int32_t width, int32_t height) {
    if (!blur || (blur->width == width && blur->height == height))
        return;

    if (!blur->tex[0].color_texture)
        render_texture_init(&blur->tex[0], 256, 144, 0, GL_RGBA16F, 0);
    if (!blur->tex[1].color_texture)
        render_texture_init(&blur->tex[1], 128, 72, 0, GL_RGBA16F, 0);
    if (!blur->tex[2].color_texture)
        render_texture_init(&blur->tex[2], 64, 36, 0, GL_RGBA16F, 0);
    if (!blur->tex[3].color_texture)
        render_texture_init(&blur->tex[3], 32, 18, 0, GL_RGBA16F, 0);
    if (!blur->tex[4].color_texture)
        render_texture_init(&blur->tex[4], 8, 8, 0, GL_R32F, 0);
    if (!blur->tex[5].color_texture)
        render_texture_init(&blur->tex[5], 256, 144, 0, GL_RGBA16F, 0);

    blur->width = width > 1 ? width : 1;
    blur->height = height > 1 ? height : 1;

    int32_t i = 0;
    width = blur->width;
    height = blur->height;
    while (width > 512 && height > 288) {
        width /= 2;
        height /= 2;
        i++;
    }

    render_texture* tex_down = blur->tex_down;
    int32_t* width_down = blur->width_down;
    int32_t* height_down = blur->height_down;

    if (!tex_down)
        tex_down = force_malloc_s(render_texture, i);
    else if (blur->count_down < i) {
        render_texture* temp = force_malloc_s(render_texture, i);
        memcpy(temp, tex_down, sizeof(render_texture) * blur->count_down);
        free(tex_down);
        tex_down = temp;
    }

    if (!width_down)
        width_down = force_malloc_s(int32_t, i);
    else if (blur->count_down < i) {
        int32_t* temp = force_malloc_s(int32_t, i);
        memcpy(temp, width_down, sizeof(int32_t) * blur->count_down);
        free(width_down);
        width_down = temp;
    }

    if (!height_down)
        height_down = force_malloc_s(int32_t, i);
    else if (blur->count_down < i) {
        int32_t* temp = force_malloc_s(int32_t, i);
        memcpy(temp, height_down, sizeof(int32_t) * blur->count_down);
        free(height_down);
        height_down = temp;
    }

    blur->count_down = i;

    i = 0;
    width = blur->width;
    height = blur->height;
    while (width > 512 && height > 288) {
        width_down[i] = width /= 2;
        height_down[i] = height /= 2;
        i++;
    }

    for (i = 0; i < blur->count_down; i++)
        render_texture_init(&tex_down[i], width_down[i], height_down[i], 0, GL_RGBA16F, 0);

    gl_state_bind_framebuffer(0);

    blur->tex_down = tex_down;
    blur->width_down = width_down;
    blur->height_down = height_down;
}

void post_process_blur_dispose(post_process_blur* blur) {
    if (!blur)
        return;

    post_process_blur_free_fbo(blur);
    free(blur);
}

static void post_process_blur_free_fbo(post_process_blur* blur) {
    for (int32_t i = 0; i < blur->count_down; i++)
        render_texture_free(&blur->tex_down[i]);
    for (int32_t i = 0; i < 6; i++)
        render_texture_free(&blur->tex[i]);
    free(blur->tex_down);
    free(blur->height_down);
    free(blur->width_down);
}

void post_process_blur_initialize_data(post_process_blur* blur, vec3* radius, vec3* intensity) {
    blur->data.radius.x = clamp(radius->x, 0.0f, 3.0f);
    blur->data.radius.y = clamp(radius->y, 0.0f, 3.0f);
    blur->data.radius.z = clamp(radius->z, 0.0f, 3.0f);
    blur->data.intensity.x = clamp(intensity->x, 0.0f, 2.0f);
    blur->data.intensity.y = clamp(intensity->y, 0.0f, 2.0f);
    blur->data.intensity.z = clamp(intensity->z, 0.0f, 2.0f);
    post_process_blur_radius_calculate(blur);
}

inline void post_process_blur_get_radius(post_process_blur* blur, vec3* value) {
    *value = blur->data.radius;
}

inline void post_process_blur_set_radius(post_process_blur* blur, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 3.0f);
    temp.y = clamp(value->y, 0.0f, 3.0f);
    temp.z = clamp(value->z, 0.0f, 3.0f);
    if (temp.x != blur->data.radius.x || temp.y
        != blur->data.radius.y || temp.z != blur->data.radius.z) {
        blur->data.radius = temp;
        post_process_blur_radius_calculate(blur);
    }
}

inline void post_process_blur_get_intensity(post_process_blur* blur, vec3* value) {
    *value = blur->data.intensity;
}

inline void post_process_blur_set_intensity(post_process_blur* blur, vec3* value) {
    blur->data.intensity.x = clamp(value->x, 0.0f, 2.0f);
    blur->data.intensity.y = clamp(value->y, 0.0f, 2.0f);
    blur->data.intensity.z = clamp(value->z, 0.0f, 2.0f);
}

static void post_process_blur_radius_calculate(post_process_blur* blur) {
    float_t radius_scale = 0.8f;
    vec3 radius = blur->data.radius;
    post_process_blur_radius_calculate_gaussian_kernel((float_t*)blur->data.gauss, radius.x * radius_scale, 3, 0);
    post_process_blur_radius_calculate_gaussian_kernel((float_t*)blur->data.gauss, radius.y * radius_scale, 3, 1);
    post_process_blur_radius_calculate_gaussian_kernel((float_t*)blur->data.gauss, radius.z * radius_scale, 3, 2);
}

static void post_process_blur_radius_calculate_gaussian_kernel(float_t* gaussian_kernel,
    float_t radius, int32_t stride, int32_t offset) {
    if (stride < 1)
        stride = 1;
    if (offset < 0)
        offset = 0;

    gaussian_kernel[0 * stride + offset] = 1.0f;
    for (int32_t i = 1; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * stride + offset] = 0.0f;
    double_t temp_gaussian_kernel[POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE];
    double_t s = radius;
    s = -1.0 / (2.0 * s * s);
    double_t sum = 0.5;
    temp_gaussian_kernel[0] = 1.0;
    for (size_t i = 1; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        sum += temp_gaussian_kernel[i] = exp(i * i * s);

    sum = 0.5 / sum;
    for (size_t i = 0; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * stride + offset] = (float_t)(temp_gaussian_kernel[i] * sum);
}
