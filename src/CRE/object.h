/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/object.h"
#include "../KKdLib/database/texture.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/obj.h"
#include "../KKdLib/txp.h"
#include "../KKdLib/vec.h"
#include "file_handler.h"
#include "static_var.h"
#include "texture.h"

struct material_change {
    vec4u blend_color;
    float_t glow_intensity;
    vec4u incandescence;
};

struct obj_mesh_index_buffer {
    GLuint buffer;
};

struct obj_mesh_vertex_buffer {
    int32_t count;
    GLuint buffers[3];
    int32_t index;
};

struct obj_index_buffer {
    int32_t meshes_count;
    obj_mesh_index_buffer* meshes;
};

struct obj_vertex_buffer {
    int32_t meshes_count;
    obj_mesh_vertex_buffer* meshes;
};

struct obj_set_handler {
    p_file_handler obj_file_handler;
    bool obj_loaded;
    p_file_handler tex_file_handler;
    bool tex_loaded;
    obj_set* obj_set;
    std::vector<std::pair<uint32_t, uint32_t>> object_ids;
    int32_t textures_count;
    std::vector<GLuint> textures;
    std::vector<std::pair<uint32_t, uint32_t>> texture_ids;
    texture** texture_data;
    int32_t set_id;
    int32_t vertex_buffers_count;
    obj_vertex_buffer* vertex_buffers;
    int32_t index_buffers_count;
    obj_index_buffer* index_buffers;
    int32_t load_count;
    std::string name;
    p_file_handler farc_file_handler;
    bool modern;

    obj_set_handler();
    virtual ~obj_set_handler();
};

extern obj_material_shader_lighting_type obj_material_shader_get_lighting_type(
    obj_material_shader_flags* flags);
extern int32_t obj_material_texture_get_blend(obj_material_texture* tex);
extern int32_t obj_material_texture_type_get_texcoord_index(
    obj_material_texture_type type, int32_t index);
extern int32_t obj_material_texture_type_get_texture_index(
    obj_material_texture_type type, int32_t base_index);
extern GLuint obj_mesh_vertex_buffer_get_buffer(obj_mesh_vertex_buffer* buffer);
extern int32_t obj_mesh_vertex_buffer_get_size(obj_mesh_vertex_buffer* buffer);
extern void obj_skin_set_matrix_buffer(obj_skin* s, mat4* matrices,
    mat4* ex_data_matrices, mat4* matrix_buffer, mat4* global_mat, mat4* mat);

extern void material_change_storage_load(const char* material_name);
extern material_change* material_change_storage_get(const char* material_name);
extern void material_change_storage_unload(const char* material_name);

extern void object_storage_init(object_database* obj_db);
extern obj* object_storage_get_obj(object_info obj_info);
extern obj_set_handler* object_storage_get_obj_set_handler(uint32_t set_id);
extern obj_set_handler* object_storage_get_obj_set_handler_by_index(size_t index);
extern obj_material* object_storage_get_material(const char* name);
extern obj_mesh* object_storage_get_obj_mesh(object_info obj_info, const char* mesh_name);
extern obj_mesh* object_storage_get_obj_mesh_by_index(object_info obj_info, int32_t index);
extern obj_mesh* object_storage_get_obj_mesh_by_object_hash(uint32_t hash, const char* mesh_name);
extern obj_mesh* object_storage_get_obj_mesh_by_object_hash_index(uint32_t hash, int32_t index);
extern int32_t object_storage_get_obj_mesh_index(object_info obj_info, const char* mesh_name);
extern int32_t object_storage_get_obj_mesh_index_by_hash(uint32_t hash, const char* mesh_name);
extern obj_set* object_storage_get_obj_set(uint32_t set_id);
extern size_t object_storage_get_obj_set_count();
extern int32_t object_storage_get_obj_storage_load_count(uint32_t set_id);
extern obj_set* object_storage_get_obj_set_by_index(size_t index);
extern size_t object_storage_get_obj_set_index(uint32_t set_id);
extern int32_t object_storage_get_obj_storage_load_count_by_index(size_t index);
extern obj_skin* object_storage_get_obj_skin(object_info obj_info);
extern obj_index_buffer* object_storage_get_obj_index_buffers(uint32_t set_id);
extern obj_mesh_index_buffer* object_storage_get_obj_mesh_index_buffer(object_info obj_info);
extern obj_vertex_buffer* object_storage_get_obj_vertex_buffers(uint32_t set_id);
extern obj_mesh_vertex_buffer* object_storage_get_obj_mesh_vertex_buffer(object_info obj_info);
extern GLuint obj_database_get_obj_set_texture(int32_t set, uint32_t tex_id);
extern std::vector<GLuint>* object_storage_get_obj_set_textures(int32_t set);
extern int32_t object_storage_load_set(void* data, object_database* obj_db, const char* name);
extern int32_t object_storage_load_set(void* data, object_database* obj_db, uint32_t set_id);
extern int32_t object_storage_load_set_hash(void* data, uint32_t hash);
extern bool object_storage_load_obj_set_check_not_read(uint32_t set_id,
    object_database* obj_db = 0, texture_database* tex_db = 0);
extern void object_storage_unload_set(uint32_t set_id);
extern void object_storage_free();