/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"

typedef enum fbo_flags {
    FBO_DEPTH_ATTACHMENT = 0x01,
} fbo_flags;

typedef struct fbo_struct {
    fbo_flags flags;
    int32_t width;
    int32_t height;
    GLuint fbo;
    int32_t count;
    GLuint* textures;
} fbo_struct;

extern void fbo_init(fbo_struct* fbo, int32_t width, int32_t height,
    GLuint* color_textures, int32_t count, GLuint depth_texture);
extern void fbo_free(fbo_struct* fbo);
