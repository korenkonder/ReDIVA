/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_object.h"

static void gl_object_generate_vbo(gl_object* obj);
static void gl_object_bind_vbo(gl_object* obj);
static void gl_object_generate_ivbo(gl_object* obj);
static void gl_object_bind_ivbo(gl_object* obj);
static void gl_object_generate_ebo(gl_object* obj);
static void gl_object_bind_ebo(gl_object* obj);
static void gl_object_generate_vao(gl_object* obj);
static void gl_object_draw(gl_object* obj);

gl_object* gl_object_init() {
    gl_object* obj = force_malloc(sizeof(gl_object));
    gl_object_generate_vbo(obj);
    gl_object_generate_ivbo(obj);
    gl_object_generate_ebo(obj);
    gl_object_generate_vao(obj);
    return obj;
}

void gl_object_update_vert(gl_object* obj, vertex* vert) {
    switch (vert->type) {
    case VERTEX_SIMPLE:
        obj->type = obj->instances.count > 0 ? GL_OBJECT_INSTANCED_SIMPLE : GL_OBJECT_SIMPLE;
        obj->vertex = *vert;
        gl_object_bind_vbo(obj);
        gl_object_bind_ebo(obj);
        break;
    case VERTEX_BONED:
        obj->type = obj->instances.count > 0 ? GL_OBJECT_INSTANCED_BONED : GL_OBJECT_BONED;
        obj->vertex = *vert;
        gl_object_bind_vbo(obj);
        gl_object_bind_ebo(obj);
        break;
    }
}

void gl_object_update_texture(gl_object* obj, texture_set* texture) {
    if (texture)
        obj->texture = *texture;
    else
        memset(&obj->texture, 0, sizeof(texture_set));
}

void gl_object_update_shader(gl_object* obj, shader_model* shader) {
    if (shader)
        obj->shader = *shader;
    else
        memset(&obj->shader, 0, sizeof(shader_model));
}

void gl_object_update_instances(gl_object* obj, gl_object_instances* instances) {
    if (instances->count > 0)
        switch (obj->type) {
        case GL_OBJECT_SIMPLE:
            obj->type = GL_OBJECT_INSTANCED_SIMPLE;
            break;
        case GL_OBJECT_BONED:
            obj->type = GL_OBJECT_INSTANCED_BONED;
            break;
        }

    switch (obj->type) {
    case GL_OBJECT_INSTANCED_SIMPLE:
    case GL_OBJECT_INSTANCED_BONED:
        obj->instances = *instances;
        gl_object_bind_ivbo(obj);
        break;
    }
}

static void gl_object_generate_vbo(gl_object* obj) {
    glGenBuffers(1, &obj->vbo);
    gl_object_bind_vbo(obj);
}

static void gl_object_bind_vbo(gl_object* obj) {
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * obj->vertex.vert_count, obj->vertex.vert, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void gl_object_generate_ivbo(gl_object* obj) {
    glGenBuffers(1, &obj->instances_trans_vbo);
    glGenBuffers(1, &obj->instances_data_vbo);
    gl_object_bind_ivbo(obj);
}

static void gl_object_bind_ivbo(gl_object* obj) {
    glBindBuffer(GL_ARRAY_BUFFER, obj->instances_trans_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * obj->instances.count, obj->instances.trans, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, obj->instances_data_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * obj->instances.count, obj->instances.data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void gl_object_generate_ebo(gl_object* obj) {
    glGenBuffers(1, &obj->ebo);
    gl_object_bind_ebo(obj);
}

static void gl_object_bind_ebo(gl_object* obj) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * obj->vertex.ind_count, obj->vertex.ind, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void gl_object_generate_vao(gl_object* obj) {
    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);
    switch (obj->type) {
    case GL_OBJECT_SIMPLE:
    case GL_OBJECT_INSTANCED_SIMPLE:
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 40, (void*)0);             // Pos
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_HALF_FLOAT, false, 40, (void*)12);       // UV
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_HALF_FLOAT, false, 40, (void*)16);       // UV2
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, false, 40, (void*)20);       // Color
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_HALF_FLOAT, false, 40, (void*)28);       // Normal
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_HALF_FLOAT, false, 40, (void*)34);       // Tangent
        break;
    case GL_OBJECT_BONED:
    case GL_OBJECT_INSTANCED_BONED:
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 56, (void*)0);             // Pos
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_HALF_FLOAT, false, 56, (void*)12);       // UV
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_HALF_FLOAT, false, 56, (void*)16);       // UV2
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, false, 56, (void*)20);       // Color
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_HALF_FLOAT, false, 56, (void*)28);       // Normal
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_HALF_FLOAT, false, 56, (void*)34);       // Tangent
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_UNSIGNED_SHORT, false, 56, (void*)40);   // Bone Index
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_HALF_FLOAT, false, 56, (void*)48);       // Bone Weight
        break;
    }

    switch (obj->type) {
    case GL_OBJECT_INSTANCED_SIMPLE:
    case GL_OBJECT_INSTANCED_BONED:
        glBindBuffer(GL_ARRAY_BUFFER, obj->instances_trans_vbo);
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, false, 64, (void*)0);             // Trans[0]
        glVertexAttribDivisor(8, 1);
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, false, 64, (void*)16);            // Trans[1]
        glVertexAttribDivisor(9, 1);
        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 4, GL_FLOAT, false, 64, (void*)32);           // Trans[2]
        glVertexAttribDivisor(10, 1);
        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 4, GL_FLOAT, false, 64, (void*)48);           // Trans[3]
        glVertexAttribDivisor(11, 1);
        glBindBuffer(GL_ARRAY_BUFFER, obj->instances_data_vbo);
        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 4, GL_FLOAT, false, 64, (void*)0);            // Data[0]
        glVertexAttribDivisor(12, 1);
        glEnableVertexAttribArray(13);
        glVertexAttribPointer(13, 4, GL_FLOAT, false, 64, (void*)16);           // Data[1]
        glVertexAttribDivisor(13, 1);
        glEnableVertexAttribArray(14);
        glVertexAttribPointer(14, 4, GL_FLOAT, false, 64, (void*)32);           // Data[2]
        glVertexAttribDivisor(14, 1);
        glEnableVertexAttribArray(15);
        glVertexAttribPointer(15, 4, GL_FLOAT, false, 64, (void*)48);           // Data[3]
        glVertexAttribDivisor(15, 1);
        break;
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gl_object_draw_c(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    shader_model_c_use(&obj->shader);
    gl_object_draw(obj);
}

void gl_object_draw_g(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    shader_model_g_use(&obj->shader);
    gl_object_draw(obj);
}

static void gl_object_draw(gl_object* obj) {
    texture_set_bind(&obj->texture);
    glBindVertexArray(obj->vao);
    switch (obj->type) {
    case GL_OBJECT_SIMPLE:
    case GL_OBJECT_BONED:
        glDrawElements(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count, GL_UNSIGNED_INT, 0);
        break;
    case GL_OBJECT_INSTANCED_SIMPLE:
    case GL_OBJECT_INSTANCED_BONED:
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count,
            GL_UNSIGNED_INT, 0, (GLsizei)obj->instances.count);
        break;
    }
    glUseProgram(0);
    texture_set_reset(&obj->texture);
    glBindVertexArray(0);
}

void gl_object_dispose(gl_object* obj) {
    if (obj->vbo)
        glDeleteBuffers(1, &obj->vbo);
    if (obj->ebo)
        glDeleteBuffers(1, &obj->ebo);
    if (obj->vao)
        glDeleteVertexArrays(1, &obj->vao);
    if (obj->instances_trans_vbo)
        glDeleteBuffers(1, &obj->instances_trans_vbo);
    if (obj->instances_data_vbo)
        glDeleteBuffers(1, &obj->instances_data_vbo);
    free(obj);
}
