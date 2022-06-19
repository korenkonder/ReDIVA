/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "shader.hpp"
#include "shader_glsl.hpp"
#include "texture.hpp"

struct render_texture {
    texture* color_texture;
    texture* depth_texture;
    int32_t binding;
    int32_t max_level;
    GLuint fbos[16];
    GLuint rbo;
    GLuint field_2C;

    render_texture();
    ~render_texture();

    int32_t bind(int32_t index = 0);
    void draw(bool depth = false);
    void free_data();
    int32_t init(int32_t width, int32_t height,
        int32_t max_level, GLenum color_format, GLenum depth_format);
    int32_t set_color_depth_textures(GLuint color_texture,
        int32_t max_level, GLuint depth_texture, bool stencil);

    static void draw_custom(shader_set_data* set);
    static void draw_custom_glsl();
    static void draw_params(shader_set_data* set, int32_t width, int32_t height,
        float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w);
    static void shader_set(shader_set_data* set, uint32_t index);
    static void shader_set_glsl(shader_glsl* shader);
};

extern void render_texture_data_init();
extern void render_texture_data_free();
