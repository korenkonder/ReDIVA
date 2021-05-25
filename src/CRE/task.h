/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/char_buffer.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "../CRE/gl_object.h"
#include "../CRE/light.h"
#include "../CRE/shader.h"
#include "../CRE/material.h"
#include "../CRE/vertex.h"
#include <xmmintrin.h>

typedef enum task_render_free_type {
    TASK_RENDER_FREE_NONE = 0,
    TASK_RENDER_FREE_GL_OBJECT,
    TASK_RENDER_FREE_LIGHT_DIR,
    TASK_RENDER_FREE_LIGHT_POINT,
    TASK_RENDER_FREE_SHADER,
    TASK_RENDER_FREE_MATERIAL,
    TASK_RENDER_FREE_TEXTURE,
    TASK_RENDER_FREE_TEXTURE_SET,
    TASK_RENDER_FREE_VERT,
} task_render_free_type;

typedef enum task_render_update_type {
    TASK_RENDER_UPDATE_NONE = 0,
    TASK_RENDER_UPDATE_GL_OBJECT,
    TASK_RENDER_UPDATE_LIGHT_DIR,
    TASK_RENDER_UPDATE_LIGHT_POINT,
    TASK_RENDER_UPDATE_SHADER,
    TASK_RENDER_UPDATE_MATERIAL,
    TASK_RENDER_UPDATE_TEXTURE,
    TASK_RENDER_UPDATE_TEXTURE_SET,
    TASK_RENDER_UPDATE_VERT,
} task_render_update_type;

typedef enum task_render_uniform_type {
    TASK_RENDER_UNIFORM_NONE = 0,
    TASK_RENDER_UNIFORM_BOOL,
    TASK_RENDER_UNIFORM_INT32,
    TASK_RENDER_UNIFORM_FLOAT32,
    TASK_RENDER_UNIFORM_VEC2,
    TASK_RENDER_UNIFORM_VEC2I,
    TASK_RENDER_UNIFORM_VEC3,
    TASK_RENDER_UNIFORM_VEC3I,
    TASK_RENDER_UNIFORM_VEC4,
    TASK_RENDER_UNIFORM_VEC4I,
    TASK_RENDER_UNIFORM_MAT3,
    TASK_RENDER_UNIFORM_MAT4,
    TASK_RENDER_UNIFORM_INT32_ARRAY,
    TASK_RENDER_UNIFORM_FLOAT32_ARRAY,
    TASK_RENDER_UNIFORM_VEC2_ARRAY,
    TASK_RENDER_UNIFORM_VEC2I_ARRAY,
    TASK_RENDER_UNIFORM_VEC3_ARRAY,
    TASK_RENDER_UNIFORM_VEC3I_ARRAY,
    TASK_RENDER_UNIFORM_VEC4_ARRAY,
    TASK_RENDER_UNIFORM_VEC4I_ARRAY,
    TASK_RENDER_UNIFORM_MAT3_ARRAY,
    TASK_RENDER_UNIFORM_MAT4_ARRAY,
} task_render_uniform_type;

typedef enum task_render_type {
    TASK_RENDER_NONE = 0,
    TASK_RENDER_UPDATE,
    TASK_RENDER_FREE,
    TASK_RENDER_UNIFORM,
} task_render_type;

typedef enum task_render_draw2d_type {
    TASK_RENDER_DRAW2D_NONE = 0,
    TASK_RENDER_DRAW2D_UI,
    TASK_RENDER_DRAW2D_FRONT,
    TASK_RENDER_DRAW2D_BACK,
} task_render_draw2d_type;

typedef enum task_render_draw3d_type {
    TASK_RENDER_DRAW3D_NONE = 0,
    TASK_RENDER_DRAW3D_G_FRONT,
    TASK_RENDER_DRAW3D_C_FRONT,
    TASK_RENDER_DRAW3D_BACK,
} task_render_draw3d_type;

typedef struct task_render_update {
    hash hash;
    task_render_update_type type;
    union {
        gl_object_update gl_obj;
        light_dir_update light_dir;
        light_point_update light_point;
        shader_model_update shad;
        material_update mat;
        texture_data tex;
        texture_set_data tex_set;
        vertex_update vert;
    };
} task_render_update;

typedef struct task_render_free {
    hash hash;
    task_render_free_type type;
} task_render_free;

typedef struct task_render_uniform_bool {
    bool value;
} task_render_uniform_bool;

typedef struct task_render_uniform_int32 {
    int32_t value;
} task_render_uniform_int32;

typedef struct task_render_uniform_float32 {
    float_t value;
} task_render_uniform_float32;

typedef struct task_render_uniform_vec2 {
    vec2 value;
} task_render_uniform_vec2;

typedef struct task_render_uniform_vec2i {
    vec2i value;
} task_render_uniform_vec2i;

typedef struct task_render_uniform_vec3 {
    vec3 value;
} task_render_uniform_vec3;

typedef struct task_render_uniform_vec3i {
    vec3i value;
} task_render_uniform_vec3i;

typedef struct task_render_uniform_vec4 {
    vec4 value;
} task_render_uniform_vec4;

typedef struct task_render_uniform_vec4i {
    vec4i value;
} task_render_uniform_vec4i;

typedef struct task_render_uniform_mat3 {
    mat3 value;
    bool transpose;
} task_render_uniform_mat3;

typedef struct task_render_uniform_mat4 {
    mat4 value;
    bool transpose;
} task_render_uniform_mat4;

typedef struct task_render_uniform_int32_array {
    int32_t* value;
    size_t count;
} task_render_uniform_int32_array;

typedef struct task_render_uniform_float32_array {
    float_t* value;
    size_t count;
} task_render_uniform_float32_array;

typedef struct task_render_uniform_vec2_array {
    vec2* value;
    size_t count;
} task_render_uniform_vec2_array;

typedef struct task_render_uniform_vec2i_array {
    vec2i* value;
    size_t count;
} task_render_uniform_vec2i_array;

typedef struct task_render_uniform_vec3_array {
    vec3* value;
    size_t count;
} task_render_uniform_vec3_array;

typedef struct task_render_uniform_vec3i_array {
    vec3i* value;
    size_t count;
} task_render_uniform_vec3i_array;

typedef struct task_render_uniform_vec4_array {
    vec4* value;
    size_t count;
} task_render_uniform_vec4_array;

typedef struct task_render_uniform_vec4i_array {
    vec4i* value;
    size_t count;
} task_render_uniform_vec4i_array;

typedef struct task_render_uniform_mat3_array {
    mat3* value;
    size_t count;
    bool transpose;
} task_render_uniform_mat3_array;

typedef struct task_render_uniform_mat4_array {
    mat4* value;
    size_t count;
    bool transpose;
} task_render_uniform_mat4_array;

typedef struct task_render_uniform {
    hash shader_hash;
    char_buffer name;
    task_render_uniform_type type;
    union {
        task_render_uniform_bool boolean;
        task_render_uniform_int32 int32;
        task_render_uniform_float32 float32;
        task_render_uniform_vec2 vec2;
        task_render_uniform_vec2i vec2i;
        task_render_uniform_vec3 vec3;
        task_render_uniform_vec3i vec3i;
        task_render_uniform_vec4 vec4;
        task_render_uniform_vec4i vec4i;
        task_render_uniform_mat3 mat3;
        task_render_uniform_mat4 mat4;
        task_render_uniform_int32_array int32_array;
        task_render_uniform_float32_array float32_array;
        task_render_uniform_vec2_array vec2_array;
        task_render_uniform_vec2i_array vec2i_array;
        task_render_uniform_vec3_array vec3_array;
        task_render_uniform_vec3i_array vec3i_array;
        task_render_uniform_vec4_array vec4_array;
        task_render_uniform_vec4i_array vec4i_array;
        task_render_uniform_mat3_array mat3_array;
        task_render_uniform_mat4_array mat4_array;
    };
} task_render_uniform;

typedef struct task_render {
    task_render_type type;
    union {
        task_render_update update;
        task_render_free free;
        task_render_uniform uniform;
    };
} task_render;

vector(task_render_uniform)

typedef struct task_render_draw2d {
    hash hash;
    task_render_draw2d_type type;
    vector_task_render_uniform uniforms;
} task_render_draw2d;

typedef struct task_render_draw3d {
    hash hash;
    task_render_draw3d_type type;
    bool translucent;
    mat4 model;
    mat3 model_normal;
    mat4 uv_mat[8];
    vec4 color;
    vector_task_render_uniform uniforms;
} task_render_draw3d;

vector(task_render)
vector(task_render_draw2d)
vector(task_render_draw3d)
