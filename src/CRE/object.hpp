/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/prj/vector_pair.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/obj.hpp"
#include "../KKdLib/txp.hpp"
#include "../KKdLib/vec.hpp"
#include "file_handler.hpp"
#include "static_var.hpp"
#include "texture.hpp"

#define SHARED_OBJECT_BUFFER (0)

struct obj_mesh_index_buffer {
    GLuint buffer;
    GLsizeiptr size;

    obj_mesh_index_buffer();

    bool load(obj_mesh& mesh);
    bool load_data(size_t size, const void* data);
    void unload();

    static void* fill_data(void* data, obj_mesh& mesh);
};

struct obj_mesh_vertex_buffer {
    uint32_t count;
    GLuint buffers[3];
    GLsizeiptr size;
#if SHARED_OBJECT_BUFFER
    size_t offset;
#endif
    uint32_t index;

    obj_mesh_vertex_buffer();

    void cycle_index();
    GLuint get_buffer();
#if SHARED_OBJECT_BUFFER
    size_t get_offset();
#endif
    GLsizeiptr get_size();
    bool load(obj_mesh& mesh, bool dynamic = false);
    bool load_data(size_t size, const void* data, int32_t count, bool dynamic);
    void unload();

    static void* fill_data(void* data, obj_mesh& mesh);
};

struct obj_index_buffer {
    uint32_t mesh_num;
    obj_mesh_index_buffer* mesh_data;
#if SHARED_OBJECT_BUFFER
    GLuint buffer;
#endif

    obj_index_buffer();

    bool load(obj& obj);
    void unload();
};

struct obj_vertex_buffer {
    uint32_t mesh_num;
    obj_mesh_vertex_buffer* mesh_data;
#if SHARED_OBJECT_BUFFER
    GLuint buffers[3];
#endif

    obj_vertex_buffer();

    bool load(obj& obj);
    void unload();
};

struct obj_set_handler {
    p_file_handler obj_file_handler;
    bool obj_loaded;
    p_file_handler tex_file_handler;
    bool tex_loaded;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    obj_set* obj_set;
    prj::vector_pair<uint32_t, uint32_t> obj_id_data;
    uint32_t tex_num;
    std::vector<texture*> gentex;
    prj::vector_pair<uint32_t, uint32_t> tex_id_data;
    texture** tex_data;
    int32_t set_id;
    uint32_t vertex_buffer_num;
    obj_vertex_buffer* vertex_buffer_data;
    uint32_t index_buffer_num;
    obj_index_buffer* index_buffer_data;
    uint32_t load_count;
    std::string name;
    p_file_handler farc_file_handler;
    bool modern;

    obj_set_handler();
    ~obj_set_handler();
};

extern int32_t obj_material_texture_type_get_texcoord_index(
    obj_material_texture_type type, int32_t index);
extern int32_t obj_material_texture_type_get_texture_index(
    obj_material_texture_type type, int32_t base_index);
extern void obj_skin_set_matrix_buffer(obj_skin* s, mat4* matrices,
    mat4* ex_data_matrices, mat4* matrix_buffer, const mat4* mat, const mat4* global_mat);

extern void object_material_msgpack_read(const char* path, const char* set_name,
    obj_set* obj_set, object_database* obj_db);
extern void object_material_msgpack_read(const char* path, const char* set_name,
    txp_set* txp_set, texture_database* tex_db, obj_set_handler* handler);
extern void object_material_msgpack_write(const char* path, const char* set_name, uint32_t set_id,
    obj_set* obj_set, txp_set* txp_set, object_database* obj_db, texture_database* tex_db);

extern void object_storage_init(const object_database* obj_db);
extern obj* object_storage_get_obj(object_info obj_info);
extern obj_set_handler* object_storage_get_obj_set_handler(uint32_t set_id);
extern obj_set_handler* object_storage_get_obj_set_handler_by_index(size_t index);
extern obj_mesh* object_storage_get_obj_mesh(object_info obj_info, const char* mesh_name);
extern obj_mesh* object_storage_get_obj_mesh_by_index(object_info obj_info, uint32_t index);
extern obj_mesh* object_storage_get_obj_mesh_by_object_hash(uint32_t hash, const char* mesh_name);
extern obj_mesh* object_storage_get_obj_mesh_by_object_hash_index(uint32_t hash, uint32_t index);
extern uint32_t object_storage_get_obj_mesh_index(object_info obj_info, const char* mesh_name);
extern uint32_t object_storage_get_obj_mesh_index_by_hash(uint32_t hash, const char* mesh_name);
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
extern texture* obj_database_get_obj_set_texture(int32_t set, uint32_t tex_id);
extern std::vector<texture*>* object_storage_get_obj_set_textures(int32_t set);
extern int32_t object_storage_load_set(void* data, const object_database* obj_db, const char* name);
extern int32_t object_storage_load_set(void* data, const object_database* obj_db, uint32_t set_id);
extern int32_t object_storage_load_set_hash(void* data, uint32_t hash);
extern bool object_storage_load_obj_set_check_not_read(uint32_t set_id,
    object_database* obj_db = 0, texture_database* tex_db = 0);
extern void object_storage_unload_set(const object_database* obj_db, const char* name);
extern void object_storage_unload_set(uint32_t set_id);
extern void object_storage_free();