/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/txp.h"
#include <glad/glad.h>

typedef enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x1,
} texture_flags;

typedef struct texture {
    int32_t field_0;
    int32_t field_4;
    texture_flags flags;
    int16_t width;
    int16_t height;
    GLuint texture;
    GLenum target;
    GLenum internal_format;
    int32_t max_mipmap_level;
    int32_t size;
} texture;

vector_ptr(texture)

extern texture* texture_load_tex_2d(GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy);
extern texture* texture_load_tex_cube_map(GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr);
extern texture* texture_txp_load(txp* t);
extern void texture_dispose(texture* tex);

extern bool texture_txp_set_load(vector_txp* t, vector_ptr_texture* tex);
