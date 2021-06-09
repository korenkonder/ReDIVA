/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"

typedef enum vertex_type {
    VERTEX_NONE = 0,
    VERTEX_SIMPLE,
    VERTEX_BONED,
} vertex_type;

typedef struct vertex_bounding_box {
    vec3 min;
    vec3 max;
}vertex_bounding_box;

#pragma pack(push, 1)
typedef struct vertex_struct {
    vec3 pos;
    vec2h texcoord[4];
    vec4h color;
    vec3i16 normal;
    vec3i16 tangent;
    vec4u16 bone_index;
    vec4u8 bone_weight;
} vertex_struct;
#pragma pack(pop)

typedef struct vertex {
    vertex_type type;
    bool translucent;
    vertex_struct* vert;
    size_t vert_count;
    uint32_t* ind;
    size_t ind_count;
    vertex_bounding_box bound_box;
    uint32_t vbo;
    uint32_t ebo;
} vertex;

typedef struct vertex_data {
    size_t length;
    vec3* position;
    vec2* texcoord0;
    vec2* texcoord1;
    vec2* texcoord2;
    vec2* texcoord3;
    vec4* color;
    vec3* normal;
    vec4i* bone_index;
    vec4* bone_weight;
} vertex_data;

extern void vertex_init(vertex* v);
extern void vertex_load(vertex* v, vertex_data* upd);
extern void vertex_free(vertex* v);