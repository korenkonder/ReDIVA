/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "hash.h"
#include "material.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

typedef struct gl_object_cull_face {
    bool enable;
    int16_t mode;
} gl_object_cull_face;

typedef enum gl_object_type {
    GL_OBJECT_NONE = 0,
    GL_OBJECT_SIMPLE,
    GL_OBJECT_BONED,
} gl_object_type;

typedef struct gl_object {
    gl_object_type type;
    int32_t vao;
    int32_t vbo;
    int32_t ebo;
    vertex vertex;
    material material;
    shader_model shader;
    texture_bone_mat bone_mat_tex;
    gl_object_cull_face cull_face;
} gl_object;

typedef struct gl_object_update {
    hash vert;
    hash material;
    hash shader;
    hash bone_mat;
    gl_object_cull_face cull_face;
} gl_object_update;

extern const gl_object_cull_face gl_object_cull_face_default;

extern gl_object* gl_object_init();
extern void gl_object_update_vert(gl_object* obj, vertex* vert);
extern void gl_object_update_material(gl_object* obj, material* material);
extern void gl_object_update_shader(gl_object* obj, shader_model* shader);
extern void gl_object_update_bone_mat(gl_object* obj, texture_bone_mat* bone_mat_tex);
extern void gl_object_update_cull_face(gl_object* obj, gl_object_cull_face cull_face);
extern void gl_object_draw_c(gl_object* obj);
extern void gl_object_draw_c_translucent_first_part(gl_object* obj);
extern void gl_object_draw_c_translucent_second_part(gl_object* obj);
extern void gl_object_draw_g(gl_object* obj);
extern void gl_object_dispose(gl_object* obj);
