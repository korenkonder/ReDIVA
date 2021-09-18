/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"
#include "texture.h"

typedef struct render_texture {
    texture* color_texture;
    texture* depth_texture;
    int32_t binding;
    int32_t max_level;
    GLuint* fbos;
    GLuint rbo;
    GLuint field_2C;
} render_texture;

int32_t render_texture_init(render_texture* rt, int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format);
int32_t render_texture_bind(render_texture* rt, int32_t index);
void render_texture_free(render_texture* rt);
