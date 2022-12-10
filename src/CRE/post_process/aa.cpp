/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "aa.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../shader_ft.hpp"

static void post_process_aa_calculate_area_texture_data(uint8_t* a1, int32_t a2, int32_t a3);
static void post_process_aa_calculate_area_texture_data_sub(float_t* a1,
    float_t* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
static float_t post_process_aa_calculate_area_texture_data_sub_sub(int32_t a1, int32_t a2);


post_process_aa::post_process_aa() : width(), height(), mlaa_area_texture() {
    uint8_t pixels[35 * 35 * 2];
    for (int32_t i = 0; i < 5; i++)
        for (int32_t j = 0; j < 5; j++)
            post_process_aa_calculate_area_texture_data(pixels, j, i);

    glGenTextures(1, &mlaa_area_texture);
    gl_state_bind_texture_2d(mlaa_area_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 35, 35, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
    gl_state_bind_texture_2d(0);
}

post_process_aa::~post_process_aa() {
    if (!this)
        return;

    glDeleteTextures(1, &mlaa_area_texture);
}

void post_process_aa::apply_mlaa(render_texture* rt,
    render_texture* buf_rt, GLuint* samplers, int32_t ss_alpha_mask) {
    mlaa_buffer->bind();
    gl_state_active_bind_texture_2d(0, rt->color_texture->tex);
    gl_state_bind_sampler(0, samplers[1]);
    //uniform_value[U_MLAA] = 0;
    //shaders_ft.set(SHADER_FT_MLAA);
    uniform_value[U_ALPHA_MASK] = 0;
    shaders_ft.set(SHADER_FT_MLAA_EDGE);
    render_texture::draw_params(&shaders_ft, width, height);

    mlaa_buffer[1].bind();
    gl_state_active_bind_texture_2d(0, mlaa_buffer[0].color_texture->tex);
    gl_state_active_bind_texture_2d(1, mlaa_area_texture);
    gl_state_bind_sampler(0, samplers[0]);
    gl_state_bind_sampler(1, samplers[1]);
    //uniform_value[U_MLAA_SEARCH] = 2;
    //uniform_value[U_MLAA] = 1;
    //shaders_ft.set(SHADER_FT_MLAA);
    uniform_value[U_MLAA_SEARCH] = 2;
    shaders_ft.set(SHADER_FT_MLAA_AREA);
    render_texture::draw_params(&shaders_ft, width, height);

    buf_rt->bind();
    gl_state_active_bind_texture_2d(0, rt->color_texture->tex);
    gl_state_active_bind_texture_2d(1, mlaa_buffer[1].color_texture->tex);
    gl_state_bind_sampler(0, samplers[1]);
    //uniform_value[U_MLAA] = 2;
    //uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
    //shaders_ft.set(SHADER_FT_MLAA);
    uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
    shaders_ft.set(SHADER_FT_MLAA_BLEND);
    render_texture::draw_params(&shaders_ft, width, height);
    uniform_value[U_ALPHA_MASK] = 0;
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_active_bind_texture_2d(1, 0);

    fbo::blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->color_texture->width, buf_rt->color_texture->height,
        0, 0, rt->color_texture->width, rt->color_texture->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void post_process_aa::init_fbo(int32_t width, int32_t height) {
    if (!this || (this->width == width && this->height == height))
        return;

    this->width = max_def(width, 1);
    this->height = max_def(height, 1);

    mlaa_buffer[0].init(this->width, this->height, 0, GL_RGBA8, 0);
    mlaa_buffer[1].init(this->width, this->height, 0, GL_RGBA8, 0);
}

static void post_process_aa_calculate_area_texture_data(uint8_t* a1, int32_t a2, int32_t a3) {
    uint8_t* v7 = &a1[7 * 7 * 10 * a3 + 14 * a2];
    for (int32_t i = 0; i < 7; i++) {
        for (int32_t j = 0; j < 7; j++) {
            float_t v11 = 0.0f;
            float_t v12 = 0.0f;
            post_process_aa_calculate_area_texture_data_sub(&v11, &v12, a2, a3, j, i);
            v7[0] = (uint8_t)(v11 * 255.89999f);
            v7[1] = (uint8_t)(v12 * 255.89999f);
            v7 += 2;
        }
        v7 += 28 * 2;
    }
}
static void post_process_aa_calculate_area_texture_data_sub(float_t* a2,
    float_t* a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7) {
    float_t v8 = 0.0f;
    float_t v10 = 0.0f;
    int32_t v11 = a6 + a7 + 1;
    v11 = min_def(v11, 7);

    int32_t v12;
    if (((a6 + a7 + 1) & 1) != 0)
        v12 = (a6 + a7) / 2 + 1;
    else
        v12 = (a6 + a7 + 1) / 2;

    float_t v14;
    int32_t v15;
    switch (a5) {
    case 0:
        if (a4 == 1)
            v10 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v11);
        else if (a4 == 3)
            v8 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v11);
        break;
    case 1:
        if (a4 == 0)
            v10 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v11);
        else if (a4 == 1) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 >= v12)
                    v10 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v10 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12) * 2.0f;
            }
        }
        else if (a4 == 3 || a4 == 4) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 < v12)
                    v8 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v10 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v8 = v10 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12);
            }
        }
        break;
    case 3:
        if (a4 == 0)
            v8 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v11);
        else if (a4 == 1 || a4 == 4) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 < v12)
                    v10 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v8 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v8 = v10 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12);
            }
        }
        else if (a4 == 3) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 < v12)
                    v8 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v8 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12) * 2.0f;
            }
        }
        break;
    case 4:
        if (a4 == 1) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 < v12)
                    v10 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v8 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v8 = v10 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12);
            }
        }
        else if (a4 == 3) {
            if (((a6 + a7 + 1) & 1) == 0) {
                v14 = -0.5f / (float_t)v12;
                if (a6 < v12)
                    v8 = ((float_t)(a6 * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    v10 = ((float_t)(a7 * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            else {
                v15 = v12 - 1;
                if (a6 < v15)
                    v8 = post_process_aa_calculate_area_texture_data_sub_sub(a6, v12);
                else if (a7 < v15)
                    v10 = post_process_aa_calculate_area_texture_data_sub_sub(a7, v12);
                else
                    v8 = v10 = post_process_aa_calculate_area_texture_data_sub_sub(v15, v12);
            }
        }
        break;
    }

    *a2 = v8;
    *a3 = v10;
    return;
}

static float_t post_process_aa_calculate_area_texture_data_sub_sub(int32_t a1, int32_t a2) {
    float_t v2 = -0.5f / ((float_t)a2 - 0.5f);
    float_t v3 = (float_t)a1 * v2 + 0.5f;
    if (a1 >= a2 - 1)
        return (v3 * 0.5f) * 0.5f;
    else
        return ((float_t)(a1 + 1) * v2 + v3 + 0.5f) * 0.5f;
}
