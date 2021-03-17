/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

typedef enum gl_object_type {
    GL_OBJECT_NONE = 0,
    GL_OBJECT_SIMPLE,
    GL_OBJECT_BONED,
    GL_OBJECT_INSTANCED_SIMPLE,
    GL_OBJECT_INSTANCED_BONED,
} gl_object_type;

typedef struct gl_object_instances {
    size_t count;
    mat4* trans;
    mat4* data;
} gl_object_instances;

typedef struct gl_object {
    gl_object_type type;
    int32_t vao;
    int32_t vbo;
    int32_t ebo;
    texture_set texture;
    shader_model shader;
    vertex vertex;
    int32_t instances_trans_vbo;
    int32_t instances_data_vbo;
    gl_object_instances instances;
} gl_object;

typedef struct gl_object_update {
    uint64_t texture;
    uint64_t shader;
    uint64_t vert;
    gl_object_instances instances;
} gl_object_update;

extern gl_object* gl_object_init();
extern void gl_object_update_vert(gl_object* obj, vertex* vert);
extern void gl_object_update_texture(gl_object* obj, texture_set* texture);
extern void gl_object_update_shader(gl_object* obj, shader_model* shader);
extern void gl_object_update_instances(gl_object* obj, gl_object_instances* instances);
extern void gl_object_draw_c(gl_object* obj);
extern void gl_object_draw_g(gl_object* obj);
extern void gl_object_dispose(gl_object* obj);
