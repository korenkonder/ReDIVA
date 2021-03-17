/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef enum vertex_type {
    VERTEX_NONE = 0,
    VERTEX_SIMPLE,
    VERTEX_BONED,
} vertex_type;

typedef struct vertex {
    vertex_type type;
    void* vert;
    size_t vert_count;
    uint32_t* ind;
    size_t ind_count;
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