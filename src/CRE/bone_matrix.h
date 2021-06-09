/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"

typedef struct bone_matrix {
    size_t length;
    int32_t ssbo;
} bone_matrix;

typedef struct bone_matrix_data {
    size_t index;
    size_t count;
    union {
        mat4 mat;
        mat4* data;
    };
} bone_matrix_data;

extern void bone_matrix_init(bone_matrix* bmd, size_t length);
extern void bone_matrix_load(bone_matrix* bmd, size_t index, size_t count, mat4* mat);
extern void bone_matrix_free(bone_matrix* bmd);
