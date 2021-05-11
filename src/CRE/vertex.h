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
typedef struct vertex_data {
    vec3 pos;
    struct {
        half_t x;
        half_t y;
    } uv;
    struct {
        half_t x;
        half_t y;
    } uv2;
    struct {
        half_t r;
        half_t g;
        half_t b;
        half_t a;
    } color;
    struct {
        half_t x;
        half_t y;
        half_t z;
    } normal;
    struct {
        half_t x;
        half_t y;
        half_t z;
    } tangent;
    struct {
        half_t x;
        half_t y;
        half_t z;
        half_t w;
    } bone_index;
    struct {
        half_t x;
        half_t y;
        half_t z;
        half_t w;
    } bone_weight;
} vertex_data;
#pragma pack(pop)

typedef struct vertex {
    vertex_type type;
    bool translucent;
    vertex_data* vert;
    size_t vert_count;
    uint32_t* ind;
    size_t ind_count;
    vertex_bounding_box bound_box;
} vertex;

typedef struct vertex_update {
    float_t* data;
    size_t length;
    bool uv;
    bool uv2;
    bool color;
    bool normal;
    bool bones;
} vertex_update;

extern vertex* vertex_init();
extern void vertex_load(vertex* v, vertex_update* upd);
extern void vertex_dispose(vertex* v);