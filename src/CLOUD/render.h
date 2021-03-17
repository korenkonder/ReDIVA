/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "../CRE/gl_object.h"
#include "../CRE/hash.h"
#include "../CRE/shader.h"
#include "../CRE/task.h"
#include "../CRE/texture.h"
#include "../CRE/vertex.h"

typedef enum render_state {
    RENDER_CLOSED        = -1,
    RENDER_UNINITIALIZED =  0,
    RENDER_INITIALIZING  =  1,
    RENDER_INITIALIZED   =  2,
} render_state;

typedef struct render_init_struct {
    vec2i res;
    vec2i internal_res;
    float_t scale;
} render_init_struct;

hash_ptr(gl_object)
hash(shader_model)
hash(texture)
hash(texture_set)
hash_ptr(vertex)

extern shader_fbo particle_shader;

vector(hash_ptr_gl_object)
vector(hash_shader_model)
vector(hash_texture)
vector(hash_texture_set)
vector(hash_ptr_vertex)

extern int32_t render_main(void* arg);
extern void render_set_scale(double_t value);
