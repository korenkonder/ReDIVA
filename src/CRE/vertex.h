/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/obj.h"
#include "../KKdLib/vec.h"

typedef enum vertex_type {
    VERTEX_NONE = 0,
    VERTEX_SIMPLE,
    VERTEX_BONED,
} vertex_type;

typedef enum vertex_primitive_type {
    VERTEX_PRIMITIVE_NONE = 0,
    VERTEX_PRIMITIVE_POINT,
    VERTEX_PRIMITIVE_LINE,
    VERTEX_PRIMITIVE_TRIANGLE,
} vertex_primitive_type;

typedef struct vertex_bounding_box {
    vec3 min;
    vec3 max;
}vertex_bounding_box;

#pragma pack(push, 1)
typedef struct vertex_struct {
    vec3 pos;
    vec4i16 normal;
    vec4i16 tangent;
    vec2h texcoord[4];
    vec4h color[2];
    vec4u16 bone_index;
    vec4u8 bone_weight;
} vertex_struct;
#pragma pack(pop)

typedef struct vertex {
    vertex_type type;
    vertex_struct* vert;
    size_t vert_count;
    uint32_t* ind;
    size_t ind_count;
    vertex_bounding_box bound_box;
    size_t morph_count;
    uint32_t* vbo;
    uint32_t ebo;
    bool translucent;
} vertex;

typedef struct vertex_data {
    vertex_primitive_type primitive;
    obj_vertex_flags flags;
    obj_vertex_data* vert;
    size_t count;
    size_t morph_count;
} vertex_data;

extern void vertex_init(vertex* v);
extern void vertex_load(vertex* v, vertex_data* data);
extern void vertex_free(vertex* v);
