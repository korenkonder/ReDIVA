/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "blur.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../shader_ft.hpp"

static void post_process_blur_radius_calculate(post_process_blur* blur);
static void post_process_blur_radius_calculate_gaussian_kernel(float_t* gaussian_kernel,
    float_t radius, int32_t stride, int32_t offset);

post_process_blur::post_process_blur() : data(),
width(), height(), width_down(), height_down(), tex_down(), count_down() {
    tex[0].init(256, 144, 0, GL_RGBA16F, 0);
    tex[1].init(128, 72, 0, GL_RGBA16F, 0);
    tex[2].init(64, 36, 0, GL_RGBA16F, 0);
    tex[3].init(32, 18, 0, GL_RGBA16F, 0);
    tex[4].init(8, 8, 0, GL_R32F, 0);
    tex[5].init(256, 144, 0, GL_RGBA16F, 0);
}

post_process_blur:: ~post_process_blur() {
    if (!this)
        return;

    for (int32_t i = 0; i < count_down; i++)
        tex_down[i].free_data();
    free_def(tex_down);
    free_def(height_down);
    free_def(width_down);
}

void post_process_blur::get_blur(render_texture* rt) {
    uniform_value[U_ALPHA_MASK] = 0;

    int32_t i = 0;
    if (count_down > 0) {
        uniform_value[U_REDUCE] = 1;
        render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);
        for (; i < count_down; i++) {
            glViewport(0, 0, width_down[i], height_down[i]);
            tex_down[i].bind();
            gl_state_active_bind_texture_2d(0, i
                ? tex_down[i - 1].color_texture->tex
                : rt->color_texture->tex);
            render_texture::draw_params(&shaders_ft,
                width_down[i], height_down[i], 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        i--;
    }

    uniform_value[U_REDUCE] = 3;
    render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);
    shaders_ft.local_frag_set(2, 1.1f, 1.1f, 1.1f, 0.0f);

    glViewport(0, 0, 256, 144);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, count_down > 0
        ? tex_down[i].color_texture->tex
        : rt->color_texture->tex);
    render_texture::draw_params(&shaders_ft, width, height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_GAUSS] = 1;
    render_texture::shader_set(&shaders_ft, SHADER_FT_GAUSS);

    glViewport(0, 0, 256, 144);
    tex[5].bind();
    gl_state_active_bind_texture_2d(0, tex[0].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 256, 144,
        1.0f, data.intensity.x * 0.5f, data.intensity.y * 0.5f, data.intensity.z * 0.5f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 128, 72);
    tex[1].bind();
    gl_state_active_bind_texture_2d(0, tex[0].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 256, 144, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 64, 36);
    tex[2].bind();
    gl_state_active_bind_texture_2d(0, tex[1].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 128, 72, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_REDUCE] = 1;
    render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 32, 18);
    tex[3].bind();
    gl_state_active_bind_texture_2d(0, tex[2].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 64, 36, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f);

    uniform_value[U_EXPOSURE] = 0;
    render_texture::shader_set(&shaders_ft, SHADER_FT_EXPOSURE);

    glViewport(0, 0, 8, 8);
    tex[4].bind();
    gl_state_active_bind_texture_2d(0, tex[3].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    vec3 intensity = data.intensity;
    vec3* gauss = data.gauss;

    vec3 v[POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE];
    for (int32_t i = 0; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        v[i] = gauss[i] * intensity;

    uniform_value[U_GAUSS] = 0;
    render_texture::shader_set(&shaders_ft, SHADER_FT_GAUSS);
    shaders_ft.local_frag_set(1, 1.0f, 0.0f, 1.0f, 0.0f);
    for (int32_t i = 0; i < POST_PROCESS_BLUR_GAUSSIAN_KERNEL_SIZE; i++)
        shaders_ft.local_frag_set(4ULL + i, v[i].x, v[i].y, v[i].z, 0.0f);

    glViewport(0, 0, 128, 72);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[1].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 128, 72, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[1].fbos[0],
        0, 0, 128, 72,
        0, 0, 128, 72, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 64, 36);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[2].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 64, 36, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[2].fbos[0],
        0, 0, 64, 36,
        0, 0, 64, 36, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 32, 18);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[3].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[3].fbos[0],
        0, 0, 32, 18,
        0, 0, 32, 18, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    shaders_ft.local_frag_set(1, 0.0f, 1.0f, 0.0f, 1.0f);

    glViewport(0, 0, 128, 72);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[1].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 128, 72, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[1].fbos[0],
        0, 0, 128, 72,
        0, 0, 128, 72, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 64, 36);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[2].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 64, 36, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[2].fbos[0],
        0, 0, 64, 36,
        0, 0, 64, 36, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, 32, 18);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[3].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 32, 18, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    fbo::blit(tex[0].fbos[0], tex[3].fbos[0],
        0, 0, 32, 18,
        0, 0, 32, 18, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    uniform_value[U_REDUCE] = 7;
    render_texture::shader_set(&shaders_ft, SHADER_FT_REDUCE);

    glViewport(0, 0, 256, 144);
    tex[0].bind();
    gl_state_active_bind_texture_2d(0, tex[5].color_texture->tex);
    gl_state_active_bind_texture_2d(1, tex[1].color_texture->tex);
    gl_state_active_bind_texture_2d(2, tex[2].color_texture->tex);
    gl_state_active_bind_texture_2d(3, tex[3].color_texture->tex);
    render_texture::draw_params(&shaders_ft, 32, 18, 0.25f, 0.15f, 0.25f, 0.25f, 0.25f);
}

void post_process_blur::init_fbo(int32_t width, int32_t height) {
    if (!this || (this->width == width && this->height == height))
        return;

    this->width = width > 1 ? width : 1;
    this->height = height > 1 ? height : 1;

    int32_t i = 0;
    width = this->width;
    height = this->height;
    while (width > 512 && height > 288) {
        width /= 2;
        height /= 2;
        i++;
    }

    render_texture* tex_down = this->tex_down;
    int32_t* width_down = this->width_down;
    int32_t* height_down = this->height_down;

    if (!tex_down)
        tex_down = force_malloc_s(render_texture, i);
    else if (count_down < i) {
        render_texture* temp = force_malloc_s(render_texture, i);
        memcpy(temp, tex_down, sizeof(render_texture) * count_down);
        free_def(tex_down);
        tex_down = temp;
    }

    if (!width_down)
        width_down = force_malloc_s(int32_t, i);
    else if (count_down < i) {
        int32_t* temp = force_malloc_s(int32_t, i);
        memcpy(temp, width_down, sizeof(int32_t) * count_down);
        free_def(width_down);
        width_down = temp;
    }

    if (!height_down)
        height_down = force_malloc_s(int32_t, i);
    else if (count_down < i) {
        int32_t* temp = force_malloc_s(int32_t, i);
        memcpy(temp, height_down, sizeof(int32_t) * count_down);
        free_def(height_down);
        height_down = temp;
    }

    count_down = i;

    i = 0;
    width = this->width;
    height = this->height;
    while (width > 512 && height > 288) {
        width_down[i] = width /= 2;
        height_down[i] = height /= 2;
        i++;
    }

    for (i = 0; i < count_down; i++)
        tex_down[i].init(width_down[i], height_down[i], 0, GL_RGBA16F, 0);

    gl_state_bind_framebuffer(0);

    this->tex_down = tex_down;
    this->width_down = width_down;
    this->height_down = height_down;
}

void post_process_blur::initialize_data(const vec3& radius, const vec3& intensity) {
    data.radius.x = clamp_def(radius.x, 1.0f, 3.0f);
    data.radius.y = clamp_def(radius.y, 1.0f, 3.0f);
    data.radius.z = clamp_def(radius.z, 1.0f, 3.0f);
    data.intensity.x = clamp_def(intensity.x, 0.0f, 2.0f);
    data.intensity.y = clamp_def(intensity.y, 0.0f, 2.0f);
    data.intensity.z = clamp_def(intensity.z, 0.0f, 2.0f);
    post_process_blur_radius_calculate(this);
}

vec3 post_process_blur::get_intensity() {
    return data.intensity;
}

void post_process_blur::set_intensity(const vec3& value) {
    data.intensity.x = clamp_def(value.x, 0.0f, 2.0f);
    data.intensity.y = clamp_def(value.y, 0.0f, 2.0f);
    data.intensity.z = clamp_def(value.z, 0.0f, 2.0f);
}

vec3 post_process_blur::get_radius() {
    return data.radius;
}

void post_process_blur::set_radius(const vec3& value) {
    vec3 temp;
    temp.x = clamp_def(value.x, 1.0f, 3.0f);
    temp.y = clamp_def(value.y, 1.0f, 3.0f);
    temp.z = clamp_def(value.z, 1.0f, 3.0f);
    if (memcmp(&temp, &data.radius, sizeof(vec3))) {
        data.radius = temp;
        post_process_blur_radius_calculate(this);
    }
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
