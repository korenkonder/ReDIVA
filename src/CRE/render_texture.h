/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"
#include "shader.h"
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
};

extern int32_t render_texture_init(render_texture* rt, int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format);
extern int32_t render_texture_bind(render_texture* rt, int32_t index);
extern void render_texture_draw(render_texture* rt, bool depth);
extern void render_texture_draw_custom(shader_set_data* set);
extern void render_texture_draw_custom_glsl();
extern void render_texture_draw_params(shader_set_data* set, int32_t width, int32_t height,
    float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w);
extern int32_t render_texture_set_color_depth_textures(render_texture* rt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil);
extern void render_texture_shader_set(shader_set_data* set, uint32_t index);
extern void render_texture_shader_set_glsl(shader_glsl* shader);
extern void render_texture_free(render_texture* rt);
extern void render_texture_data_init();
extern void render_texture_data_free();
