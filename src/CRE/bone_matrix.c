/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone_matrix.h"
#include "static_var.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

typedef struct bone_matrix_struct {
    mat4 mat;
    vec4 mat_normal_row0;
    vec4 mat_normal_row1;
    vec4 mat_normal_row2;
} bone_matrix_struct;

void bone_matrix_init(bone_matrix* bm, size_t length) {
    glGenBuffers(1, &bm->ssbo);
    bm->length = length;
    bind_shader_storage_buffer(bm->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        (GLsizeiptr)(sizeof(bone_matrix_struct) * length), 0, GL_DYNAMIC_DRAW);
    bind_shader_storage_buffer(0);
}

void bone_matrix_load(bone_matrix* bm, size_t index, size_t count, mat4* mat) {
    if (!bm || !bm->ssbo || !bm->length)
        return;
    else if (bm->length <= index || !count || bm->length - index < count)
        return;
    else if (count == 1) {
        mat4 temp;
        mat3 mn;
        mat4_invtrans(mat, &temp);
        mat3_from_mat4(&temp, &mn);

        bind_shader_storage_buffer(bm->ssbo);
        bone_matrix_struct* data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        bone_matrix_struct bms;
        bms.mat = *mat;
        *(vec3*)&bms.mat_normal_row0 = mn.row0;
        *(vec3*)&bms.mat_normal_row1 = mn.row1;
        *(vec3*)&bms.mat_normal_row2 = mn.row2;
        bms.mat_normal_row0.w = 0.0f;
        bms.mat_normal_row1.w = 0.0f;
        bms.mat_normal_row2.w = 0.0f;
        data[index] = bms;
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        return;
    }

    bind_shader_storage_buffer(bm->ssbo);
    bone_matrix_struct* data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    for (size_t i = 0; i < count; i++) {
        mat4 temp;
        mat3 mn;
        mat4_invtrans(&mat[i], &temp);
        mat3_from_mat4(&temp, &mn);

        bone_matrix_struct bms;
        bms.mat = mat[i];
        *(vec3*)&bms.mat_normal_row0 = mn.row0;
        *(vec3*)&bms.mat_normal_row1 = mn.row1;
        *(vec3*)&bms.mat_normal_row2 = mn.row2;
        bms.mat_normal_row0.w = 0.0f;
        bms.mat_normal_row1.w = 0.0f;
        bms.mat_normal_row2.w = 0.0f;
        data[index + i] = bms;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    bind_shader_storage_buffer(0);
}

void bone_matrix_free(bone_matrix* bm) {
    glDeleteBuffers(1, &bm->ssbo);
    bm->ssbo = 0;
    bm->length = 0;
}
