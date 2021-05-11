/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_object.h"
#include "static_var.h"

extern const gl_object_cull_face gl_object_cull_face_default = {
    .enable = true,
    .mode = GL_BACK,
};

static void gl_object_generate_vbo(gl_object* obj);
static void gl_object_bind_vbo(gl_object* obj);
static void gl_object_generate_ebo(gl_object* obj);
static void gl_object_bind_ebo(gl_object* obj);
static void gl_object_generate_vao(gl_object* obj);

gl_object* gl_object_init() {
    gl_object* obj = force_malloc(sizeof(gl_object));
    gl_object_generate_vbo(obj);
    gl_object_generate_ebo(obj);
    gl_object_generate_vao(obj);
    return obj;
}

void gl_object_update_vert(gl_object* obj, vertex* vert) {
    switch (vert->type) {
    case VERTEX_SIMPLE:
        obj->type = GL_OBJECT_SIMPLE;
        obj->vertex = *vert;
        gl_object_bind_vbo(obj);
        gl_object_bind_ebo(obj);
        break;
    case VERTEX_BONED:
        obj->type = GL_OBJECT_BONED;
        obj->vertex = *vert;
        gl_object_bind_vbo(obj);
        gl_object_bind_ebo(obj);
        break;
    }
}

void gl_object_update_material(gl_object* obj, material* mat) {
    if (mat)
        obj->material = *mat;
    else
        memset(&obj->material, 0, sizeof(material));
}

void gl_object_update_shader(gl_object* obj, shader_model* shader) {
    if (shader)
        obj->shader = *shader;
    else
        memset(&obj->shader, 0, sizeof(shader_model));
}

void gl_object_update_bone_mat(gl_object* obj, texture_bone_mat* bone_mat_tex) {
    if (bone_mat_tex)
        obj->bone_mat_tex = *bone_mat_tex;
    else
        memset(&obj->bone_mat_tex, 0, sizeof(texture_bone_mat));
}

void gl_object_update_cull_face(gl_object* obj, gl_object_cull_face cull_face) {
    obj->cull_face = cull_face;
}

static void gl_object_generate_vbo(gl_object* obj) {
    glGenBuffers(1, &obj->vbo);
    gl_object_bind_vbo(obj);
}

static void gl_object_bind_vbo(gl_object* obj) {
    bind_array_buffer(obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(vertex_data) * obj->vertex.vert_count),
        (void*)obj->vertex.vert, GL_DYNAMIC_DRAW);
    bind_array_buffer(0);
}

static void gl_object_generate_ebo(gl_object* obj) {
    glGenBuffers(1, &obj->ebo);
    gl_object_bind_ebo(obj);
}

static void gl_object_bind_ebo(gl_object* obj) {
    bind_element_array_buffer(obj->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(uint32_t) * obj->vertex.ind_count),
        obj->vertex.ind, GL_DYNAMIC_DRAW);
    bind_element_array_buffer(0);
}

static void gl_object_generate_vao(gl_object* obj) {
    static const GLsizei data_size = sizeof(vertex_data);

    glGenVertexArrays(1, &obj->vao);
    bind_vertex_array(obj->vao);
    bind_array_buffer(obj->vbo);
    bind_element_array_buffer(obj->ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, data_size,
        (void*)offsetof(vertex_data, pos));         // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 4, GL_UNSIGNED_INT, data_size,
        (void*)offsetof(vertex_data, uv));          // UV/Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, data_size,
        (void*)offsetof(vertex_data, normal));      // Normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_SHORT, GL_TRUE, data_size,
        (void*)offsetof(vertex_data, tangent));     // Tangent
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 4, GL_UNSIGNED_SHORT, data_size,
        (void*)offsetof(vertex_data, bone_index));  // Bone Index
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, data_size,
        (void*)offsetof(vertex_data, bone_weight)); // Bone Weight
    bind_vertex_array(0);
    bind_array_buffer(0);
    bind_element_array_buffer(0);
}

void gl_object_draw_c(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    if (obj->material.blend.enable) {
        glEnablei(GL_BLEND, 0);
        glBlendFuncSeparate(obj->material.blend.src_factor_rgb, obj->material.blend.dst_factor_rgb,
            obj->material.blend.src_factor_alpha, obj->material.blend.dst_factor_alpha);
        glBlendEquationSeparate(obj->material.blend.mode_rgb, obj->material.blend.mode_alpha);
    }
    else
        glDisable(GL_BLEND);

    if (obj->cull_face.enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(obj->cull_face.mode);
    }
    else
        glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    bool use_bones;
    switch (obj->type) {
    case GL_OBJECT_BONED:
        use_bones = true;
        break;
    default:
        use_bones = false;
        break;
    }

    GLint tex_mode[8];
    for (int32_t i = 0; i < 8; i++)
        tex_mode[i] = obj->material.texture.tex[i].mode;

    shader_model_c_use(&obj->shader);
    shader_model_c_set_int_array(&obj->shader, "tex_mode", 8, tex_mode);
    shader_model_c_set_bool(&obj->shader, "use_bones", use_bones);
    shader_model_c_set_bool(&obj->shader, "write_only_depth", false);
    texture_set_bind(&obj->material.texture);

    texture_bone_mat_bind(&obj->bone_mat_tex, 8);
    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count, GL_UNSIGNED_INT, 0);
    texture_set_reset(&obj->material.texture);
    texture_bone_mat_reset(&obj->bone_mat_tex, 8);
}

void gl_object_draw_c_translucent_first_part(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    if (obj->cull_face.enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(obj->cull_face.mode);
    }

    bool use_bones;
    switch (obj->type) {
    case GL_OBJECT_BONED:
        use_bones = true;
        break;
    default:
        use_bones = false;
        break;
    }

    shader_model_c_use(&obj->shader);
    shader_model_c_set_bool(&obj->shader, "use_bones", use_bones);
    shader_model_c_set_bool(&obj->shader, "write_only_depth", true);
    texture_set_reset(&obj->material.texture);

    texture_bone_mat_bind(&obj->bone_mat_tex, 8);
    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count, GL_UNSIGNED_INT, 0);
    texture_bone_mat_reset(&obj->bone_mat_tex, 8);
}

void gl_object_draw_c_translucent_second_part(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    glBlendFuncSeparate(obj->material.blend.src_factor_rgb, obj->material.blend.dst_factor_rgb,
        obj->material.blend.src_factor_alpha, obj->material.blend.dst_factor_alpha);
    glBlendEquationSeparate(obj->material.blend.mode_rgb, obj->material.blend.mode_alpha);

    if (obj->cull_face.enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(obj->cull_face.mode);
    }
    else
        glDisable(GL_CULL_FACE);

    bool use_bones;
    switch (obj->type) {
    case GL_OBJECT_BONED:
        use_bones = true;
        break;
    default:
        use_bones = false;
        break;
    }

    GLint tex_mode[8];
    for (int32_t i = 0; i < 8; i++)
        tex_mode[i] = obj->material.texture.tex[i].mode;

    shader_model_c_use(&obj->shader);
    shader_model_c_set_int_array(&obj->shader, "tex_mode", 8, tex_mode);
    shader_model_c_set_bool(&obj->shader, "use_bones", use_bones);
    shader_model_c_set_bool(&obj->shader, "write_only_depth", false);
    texture_set_bind(&obj->material.texture);

    texture_bone_mat_bind(&obj->bone_mat_tex, 8);
    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count, GL_UNSIGNED_INT, 0);
    texture_set_reset(&obj->material.texture);
    texture_bone_mat_reset(&obj->bone_mat_tex, 8);
}

void gl_object_draw_g(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    if (obj->cull_face.enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(obj->cull_face.mode);
    }
    else
        glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    bool use_bones;
    switch (obj->type) {
    case GL_OBJECT_BONED:
        use_bones = true;
        break;
    default:
        use_bones = false;
        break;
    }

    GLint tex_mode[8];
    for (int32_t i = 0; i < 8; i++)
        tex_mode[i] = obj->material.texture.tex[i].mode;

    shader_model_g_use(&obj->shader);
    shader_model_g_set_int_array(&obj->shader, "tex_mode", 8, tex_mode);
    shader_model_g_set_bool(&obj->shader, "use_bones", use_bones);
    texture_set_bind(&obj->material.texture);
    texture_bone_mat_bind(&obj->bone_mat_tex, 8);
    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)obj->vertex.ind_count, GL_UNSIGNED_INT, 0);
    texture_set_reset(&obj->material.texture);
    texture_bone_mat_reset(&obj->bone_mat_tex, 8);
}

void gl_object_dispose(gl_object* obj) {
    if (obj->vbo)
        glDeleteBuffers(1, &obj->vbo);
    if (obj->ebo)
        glDeleteBuffers(1, &obj->ebo);
    if (obj->vao)
        glDeleteVertexArrays(1, &obj->vao);
    free(obj);
}
