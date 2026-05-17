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
#include "GL/buffer.hpp"
#include "config.hpp"
#include "file_handler.hpp"
#include "static_var.hpp"
#include "texture.hpp"

struct IndexBuffer;
struct VertexBuffer;

struct ObjIB {
    int32_t num_ib;
    IndexBuffer* ibhn_array;
};

struct ObjVB {
    int32_t num_vb;
    VertexBuffer* vbhn_array;
};

struct ObjsetInfo {
    p_file_handler obj_file_handler;
    bool obj_ready;
    p_file_handler tex_file_handler;
    bool tex_ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    obj_set* obj_set;
    prj::vector_pair<uint32_t, uint32_t> objdb_map;
    int32_t tex_num;
    std::vector<GLuint> gentex_vec;
    prj::vector_pair<uint32_t, uint32_t> texidx_map;
    texture** textures;
    int32_t id;
    int32_t objvb_num;
    ObjVB* objvb;
    int32_t objib_num;
    ObjIB* objib;
    int32_t req_cnt;

    // Added
#if SHARED_OBJECT_BUFFER
    GLuint vb;
    GLuint ib;
#endif

    std::string name;
    p_file_handler farc_file_handler;
    bool modern;

    ObjsetInfo();
    ~ObjsetInfo();
};

struct ObjsetInfoObject {
    ObjsetInfo* info;
    int32_t index;
};

extern int32_t obj_material_texture_type_get_texcoord_index(
    obj_material_texture_type type, int32_t index);
extern int32_t obj_material_texture_type_get_texture_index(
    obj_material_texture_type type, int32_t base_index);

extern void obj_skin_set_matrix_buffer(const obj_skin* s, const mat4* matrices,
    const mat4* ex_data_matrices, mat4* matrix_buffer, const mat4* mat, const mat4& global_mat);

extern void object_material_msgpack_read(const char* path, const char* set_name,
    obj_set* obj_set);
extern void object_material_msgpack_read(const char* path, const char* set_name,
    txp_set* txp_set, texture_database* tex_db, ObjsetInfo* info);
extern void object_material_msgpack_write(const char* path, const char* set_name, uint32_t set_id,
    obj_set* obj_set, txp_set* txp_set, texture_database* tex_db);

extern void objset_info_storage_init(const object_database* obj_db);
extern void objset_info_storage_free();

extern bool check_objset_ready(uint32_t objset_index);

extern bool create_mesh_index_buffer(IndexBuffer& ibhn, obj_mesh& mesh);
#if SHARED_OBJECT_BUFFER
extern void create_mesh_index_buffer(IndexBuffer& ibhn, obj_mesh& mesh, GLuint in_ib); // Added
#endif
extern bool create_mesh_vertex_buffer(VertexBuffer& vbhn,
    obj_mesh& mesh, GL::BufferUsage usage = GL::BUFFER_USAGE_STATIC);
#if SHARED_OBJECT_BUFFER
extern void create_mesh_vertex_buffer(VertexBuffer& vbhn,
    obj_mesh& mesh, GLuint in_vb, uint32_t& offset); // Added
#endif

extern bool create_objset_index_buffer(ObjsetInfo* info);
extern bool create_objset_vertex_buffer(ObjsetInfo* info);

extern bool find_objdata_index(ObjsetInfoObject& info_object, object_info obj_id);

extern void free_objset_index_buffer(ObjsetInfo* info);
extern void free_objset_vertex_buffer(ObjsetInfo* info);

extern obj_set* get_obj_data_header(uint32_t objset_index);

extern obj_bounding_sphere* get_object_bsphere(object_info obj_id);
inline obj* get_object_header(object_info obj_id);
extern IndexBuffer* get_object_index_buffer(object_info obj_id);
extern const char* get_object_name(object_info obj_id);
extern int32_t get_object_num(uint32_t objset_index);
extern obj_skin* get_object_skin(object_info obj_id);
extern obj_skin_ex_data* get_object_skin_osage_header(object_info obj_id);
extern VertexBuffer* get_object_vertex_buffer(object_info obj_id);

extern uint32_t get_objnum_idx2uid(uint32_t objset_index, int32_t obj_index, const object_database* obj_db);

extern GLuint get_objset_gen_textures_id(uint32_t objset_index, uint32_t uid);
extern std::vector<GLuint>* get_objset_gen_textures_vec(uint32_t objset_index);
extern ObjsetInfo* get_objset_info(uint32_t objset_index);
extern size_t get_objset_num();
extern int32_t get_objset_num_textures(uint32_t objset_index);
extern texture** get_objset_textures(uint32_t objset_index);

extern obj_mesh* get_mesh(object_info obj_id, const char* mesh_name);
extern obj_mesh* get_mesh(object_info obj_id, int32_t mesh_index);
extern obj_mesh* get_mesh_modern(uint32_t hash, const char* mesh_name); // Added
extern obj_mesh* get_mesh_modern(uint32_t hash, int32_t mesh_index); // Added
extern int32_t get_mesh_index(object_info obj_id, const char* mesh_name);
extern int32_t get_mesh_index_modern(uint32_t hash, const char* mesh_name);// Added
extern VertexBuffer* get_mesh_vertex_buffer(object_info obj_id, const char* mesh_name);

extern uint32_t get_texnum_idx2uid(uint32_t objset_index, int32_t tex_index);

extern int32_t request_objset(void* data, const object_database* obj_db, uint32_t objset_index);
extern int32_t request_objset(void* data, const object_database* obj_db, const char* name); // Added
extern int32_t request_objset_modern(void* data, uint32_t hash); // Added
extern bool wait_objset(uint32_t objset_index, object_database* obj_db = 0, texture_database* tex_db = 0);
extern void free_objset(uint32_t objset_index);
extern void free_objset(const object_database* obj_db, const char* name); // Added
