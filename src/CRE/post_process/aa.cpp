/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "aa.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../shader_ft.hpp"

static void post_process_aa_generate_area_texture(post_process_aa* aa);
static void post_process_aa_calculate_area_texture_data(uint8_t* data, int32_t cross1, int32_t cross2);
static void post_process_aa_calculate_area_texture_data_area(float_t* val_left,
    float_t* val_right, int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright);
static float_t calc_area_tex_val(int32_t a1, int32_t a2);

#define MAX_EDGE_DETECTION_LEN (3)
#define GRID_SIDE_LEN (2 * MAX_EDGE_DETECTION_LEN + 1)
#define SIDE_LEN (5 * GRID_SIDE_LEN)

post_process_aa::post_process_aa() : width(), height(), mlaa_area_texture() {
    post_process_aa_generate_area_texture(this);
}

post_process_aa::~post_process_aa() {
    if (!this)
        return;

    if (mlaa_area_texture) {
        glDeleteTextures(1, &mlaa_area_texture);
        mlaa_area_texture = 0;
    }
}

void post_process_aa::apply_mlaa(render_texture* rt,
    render_texture* buf_rt, GLuint* samplers, int32_t ss_alpha_mask) {
    mlaa_buffer.bind();
    gl_state_active_bind_texture_2d(0, rt->color_texture->tex);
    gl_state_bind_sampler(0, samplers[1]);
    uniform_value[U_MLAA] = 0;
    shaders_ft.set(SHADER_FT_MLAA);
    render_texture::draw_quad(&shaders_ft, width, height);

    temp_buffer.bind();
    gl_state_active_bind_texture_2d(0, mlaa_buffer.color_texture->tex);
    gl_state_active_bind_texture_2d(1, mlaa_area_texture);
    gl_state_bind_sampler(0, samplers[0]);
    gl_state_bind_sampler(1, samplers[1]);
    uniform_value[U_MLAA] = 1;
    uniform_value[U_MLAA_SEARCH] = 2;
    shaders_ft.set(SHADER_FT_MLAA);
    render_texture::draw_quad(&shaders_ft, width, height);

    buf_rt->bind();
    gl_state_active_bind_texture_2d(0, rt->color_texture->tex);
    gl_state_active_bind_texture_2d(1, temp_buffer.color_texture->tex);
    gl_state_bind_sampler(0, samplers[1]);
    uniform_value[U_MLAA] = 2;
    uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
    shaders_ft.set(SHADER_FT_MLAA);
    render_texture::draw_quad(&shaders_ft, width, height);
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

    mlaa_buffer.init(this->width, this->height, 0, GL_RGBA8, 0);
    temp_buffer.init(this->width, this->height, 0, GL_RGBA8, 0);
}

static void post_process_aa_generate_area_texture(post_process_aa* aa) {
    if (aa->mlaa_area_texture) {
        glDeleteTextures(1, &aa->mlaa_area_texture);
        aa->mlaa_area_texture = 0;
    }

    uint8_t* data = (uint8_t*)malloc(SIDE_LEN * SIDE_LEN * 2);
    if (!data)
        return;

    for (int32_t cross2 = 0; cross2 < 5; cross2++)
        for (int32_t cross1 = 0; cross1 < 5; cross1++)
            post_process_aa_calculate_area_texture_data(data, cross1, cross2);

    glGenTextures(1, &aa->mlaa_area_texture);
    gl_state_bind_texture_2d(aa->mlaa_area_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, SIDE_LEN, SIDE_LEN, 0, GL_RG, GL_UNSIGNED_BYTE, data);
    gl_state_bind_texture_2d(0);
    free(data);
}

static void post_process_aa_calculate_area_texture_data(uint8_t* data, int32_t cross1, int32_t cross2) {
    uint8_t* _data = &data[(SIDE_LEN * GRID_SIDE_LEN * cross2 + GRID_SIDE_LEN * cross1) * 2];
    for (int32_t dright = 0; dright < GRID_SIDE_LEN; dright++) {
        for (int32_t dleft = 0; dleft < GRID_SIDE_LEN; dleft++) {
            float_t val_left = 0.0f;
            float_t val_right = 0.0f;
            post_process_aa_calculate_area_texture_data_area(&val_left, &val_right, cross1, cross2, dleft, dright);
            _data[0] = (uint8_t)(val_left  * 255.9f);
            _data[1] = (uint8_t)(val_right * 255.9f);
            _data += 2;
        }
        _data += GRID_SIDE_LEN * 4 * 2;
    }
}

static void post_process_aa_calculate_area_texture_data_area(float_t* val_left,
    float_t* val_right, int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright) {
    int32_t dist = dleft + dright + 1;
    float_t _val_left = 0.0f;
    float_t _val_right = 0.0f;
    switch (cross2) {
    case 0:
        switch (cross1) {
        case 1:
            _val_right = calc_area_tex_val(dleft, min_def(dist, GRID_SIDE_LEN));
            break;
        case 3:
            _val_left = calc_area_tex_val(dleft, min_def(dist, GRID_SIDE_LEN));
            break;
        }
        break;
    case 1:
        switch (cross1) {
        case 0:
            _val_right = calc_area_tex_val(dright, min_def(dist, GRID_SIDE_LEN));
            break;
        case 1:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_right = calc_area_tex_val(v15, v12) * 2.0f;
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft >= v12)
                    _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
        case 4:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        }
        break;
    case 3:
        switch (cross1) {
        case 0:
            _val_left = calc_area_tex_val(dright, min_def(dist, GRID_SIDE_LEN));
            break;
        case 1:
        case 4:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
            if (dist % 2){
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = calc_area_tex_val(v15, v12) * 2.0f;
            }
            else  {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        }
        break;
    case 4:
        switch (cross1) {
        case 1:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        }
        break;
    }

    *val_left = _val_left;
    *val_right = _val_right;
    return;
}

static float_t calc_area_tex_val(int32_t a1, int32_t a2) {
    float_t v2 = (float_t)(-0.5 / ((float_t)a2 - 0.5));
    float_t v3 = (float_t)a1 * v2 + 0.5f;
    if (a1 >= a2 - 1)
        return (v3 * 0.5f) * 0.5f;
    else
        return ((float_t)(a1 + 1) * v2 + 0.5f + v3) * 0.5f;
}
