/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "bone_matrix.h"
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
    vertex* vertex;
    material* material;
    shader_model* shader;
    bone_matrix* bone_mat;
    gl_object_cull_face cull_face;
} gl_object;

typedef struct gl_object_data {
    hash vert;
    hash material;
    hash shader;
    hash bone_mat;
    gl_object_cull_face cull_face;
} gl_object_data;

extern const gl_object_cull_face gl_object_cull_face_default;

extern void gl_object_init(gl_object* obj);
extern void gl_object_load_vert(gl_object* obj, vertex* vert);
extern void gl_object_load_material(gl_object* obj, material* material);
extern void gl_object_load_shader(gl_object* obj, shader_model* shader);
extern void gl_object_load_cull_face(gl_object* obj, gl_object_cull_face* cull_face);
extern void gl_object_load_bone_matrix(gl_object* obj, bone_matrix* bone_mat);
extern void gl_object_draw(gl_object* obj);
extern void gl_object_draw_translucent_first_part(gl_object* obj);
extern void gl_object_draw_translucent_second_part(gl_object* obj);
extern void gl_object_free(gl_object* obj);
