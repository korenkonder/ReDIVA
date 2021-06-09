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

static void gl_object_update_vao(gl_object* obj);

void gl_object_init(gl_object* obj) {
    glGenVertexArrays(1, &obj->vao);

    obj->type = GL_OBJECT_NONE;
    obj->vertex = 0;
    obj->material = 0;
    obj->shader = 0;
    obj->bone_mat = 0;
    obj->cull_face = gl_object_cull_face_default;
}

void gl_object_load_vert(gl_object* obj, vertex* vert) {
    switch (vert->type) {
    case VERTEX_SIMPLE:
        obj->type = GL_OBJECT_SIMPLE;
        obj->vertex = vert;
        gl_object_update_vao(obj);
        break;
    case VERTEX_BONED:
        obj->type = GL_OBJECT_BONED;
        obj->vertex = vert;
        gl_object_update_vao(obj);
        break;
    }
}

void gl_object_load_material(gl_object* obj, material* mat) {
    obj->material = mat;
}

void gl_object_load_shader(gl_object* obj, shader_model* shader) {
    obj->shader = shader;
}

void gl_object_load_cull_face(gl_object* obj, gl_object_cull_face* cull_face) {
    obj->cull_face = *cull_face;
}

void gl_object_load_bone_matrix(gl_object* obj, bone_matrix* bone_mat) {
    obj->bone_mat = bone_mat;
}

void gl_object_draw(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    bone_matrix* bone_mat = obj->bone_mat;
    material* mat = obj->material;
    shader_model* shad = obj->shader;
    vertex* vert = obj->vertex;
    texture_set* tex = mat->texture;

    if (mat->blend.enable) {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(mat->blend.src_factor_rgb, mat->blend.dst_factor_rgb,
            mat->blend.src_factor_alpha, mat->blend.dst_factor_alpha);
        glBlendEquationSeparate(mat->blend.mode_rgb, mat->blend.mode_alpha);
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

    GLint tex_mode[TEXTURE_SET_COUNT];
    for (int32_t i = 0; i < TEXTURE_SET_COUNT; i++)
        tex_mode[i] = tex->tex[i].mode;

    shader_model_use(shad);
    shader_model_set_int_array(shad, "tex_mode", TEXTURE_SET_COUNT, tex_mode);
    shader_model_set_bool(shad, "use_bones", use_bones);
    shader_model_set_bool(shad, "write_only_depth", false);
    shader_model_set_vec4(shad, "material_param.ambient", mat->param.ambient);
    shader_model_set_vec4(shad, "material_param.diffuse", mat->param.diffuse);
    shader_model_set_vec4(shad, "material_param.specular", mat->param.specular);
    shader_model_set_vec4(shad, "material_param.emission", mat->param.emission);
    shader_model_set_float(shad, "material_param.shininess", mat->param.shininess);
    texture_set_bind(mat->texture);
    bind_shader_storage_buffer_base(0, bone_mat->ssbo);

    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)vert->ind_count, GL_UNSIGNED_INT, 0);
    texture_set_reset(mat->texture);
    bind_shader_storage_buffer(0);
    bind_vertex_array(0);
}

void gl_object_draw_translucent_first_part(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    bone_matrix* bone_mat = obj->bone_mat;
    shader_model* shad = obj->shader;
    material* mat = obj->material;
    vertex* vert = obj->vertex;

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

    shader_model_use(shad);
    shader_model_set_bool(shad, "use_bones", use_bones);
    shader_model_set_bool(shad, "write_only_depth", true);
    texture_set_reset(mat->texture);
    bind_shader_storage_buffer_base(0, bone_mat->ssbo);

    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)vert->ind_count, GL_UNSIGNED_INT, 0);
    bind_shader_storage_buffer(0);
    bind_vertex_array(0);
}

void gl_object_draw_translucent_second_part(gl_object* obj) {
    if (obj->type == GL_OBJECT_NONE)
        return;

    bone_matrix* bone_mat = obj->bone_mat;
    material* mat = obj->material;
    shader_model* shad = obj->shader;
    vertex* vert = obj->vertex;
    texture_set* tex = mat->texture;

    glBlendFuncSeparate(mat->blend.src_factor_rgb, mat->blend.dst_factor_rgb,
        mat->blend.src_factor_alpha, mat->blend.dst_factor_alpha);
    glBlendEquationSeparate(mat->blend.mode_rgb, mat->blend.mode_alpha);

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

    GLint tex_mode[TEXTURE_SET_COUNT];
    for (int32_t i = 0; i < TEXTURE_SET_COUNT; i++)
        tex_mode[i] = tex->tex[i].mode;

    shader_model_use(shad);
    shader_model_set_int_array(shad, "tex_mode", TEXTURE_SET_COUNT, tex_mode);
    shader_model_set_bool(shad, "use_bones", use_bones);
    shader_model_set_bool(shad, "write_only_depth", false);
    shader_model_set_vec4(shad, "material_param.ambient", mat->param.ambient);
    shader_model_set_vec4(shad, "material_param.diffuse", mat->param.diffuse);
    shader_model_set_vec4(shad, "material_param.specular", mat->param.specular);
    shader_model_set_vec4(shad, "material_param.emission", mat->param.emission);
    shader_model_set_float(shad, "material_param.shininess", mat->param.shininess);
    texture_set_bind(mat->texture);
    bind_shader_storage_buffer_base(0, bone_mat->ssbo);

    bind_vertex_array(obj->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)vert->ind_count, GL_UNSIGNED_INT, 0);
    texture_set_reset(mat->texture);
    bind_shader_storage_buffer(0);
    bind_vertex_array(0);
}

void gl_object_free(gl_object* obj) {
    if (!obj)
        return;

    if (obj->vao)
        glDeleteVertexArrays(1, &obj->vao);

    obj->type = GL_OBJECT_NONE;
    obj->vertex = 0;
    obj->material = 0;
    obj->shader = 0;
    obj->bone_mat = 0;
    obj->cull_face = gl_object_cull_face_default;
}

static void gl_object_update_vao(gl_object* obj) {
    static const GLsizei data_size = sizeof(vertex_struct);
    vertex* vert = obj->vertex;

    bind_vertex_array(obj->vao);
    bind_array_buffer(vert->vbo);
    bind_element_array_buffer(vert->ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, data_size,
        (void*)offsetof(vertex_struct, pos));       // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 4, GL_UNSIGNED_INT, data_size,
        (void*)offsetof(vertex_struct, texcoord));  // TexCoord[0-3]
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 4, GL_UNSIGNED_INT, data_size,
        (void*)offsetof(vertex_struct, color));     // Color / Normal / Tangent (X)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, data_size,
        (void*)offsetof(vertex_struct, tangent.y)); // Tangent (YZ) / Bone Index / Bone Weight
    bind_vertex_array(0);
    bind_array_buffer(0);
    bind_element_array_buffer(0);
}
