/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <list>
#include <map>
#include <vector>
#include "object.hpp"
#include "data.hpp"
#include "gl_state.h"
#include "shader_ft.h"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.h"

struct material_change_handler {
    int32_t load_count;
    material_change data;
};

static bool obj_mesh_index_buffer_load(obj_mesh_index_buffer* buffer, obj_mesh* mesh);
static GLuint obj_mesh_index_buffer_load_data(size_t size, void* data);
static void obj_mesh_index_buffer_free(obj_mesh_index_buffer* buffer);
static bool obj_mesh_vertex_buffer_load(obj_mesh_vertex_buffer* buffer, obj_mesh* mesh);
static bool obj_mesh_vertex_buffer_load_data(obj_mesh_vertex_buffer* buffer,
    size_t size, void* data, int32_t count);
static void obj_mesh_vertex_buffer_free(obj_mesh_vertex_buffer* buffer);
static void obj_set_handler_calc_axis_aligned_bounding_box(obj_set_handler* handler);
static void obj_set_handler_get_shader_index_texture_index(obj_set_handler* handler);
static bool obj_set_handler_index_buffer_load(obj_set_handler* handler);
static void obj_set_handler_index_buffer_free(obj_set_handler* handler);
static bool obj_set_handler_load_textures(obj_set_handler* handler, void* data, bool big_endian);
static bool obj_set_handler_load_textures_modern(obj_set_handler* handler, void* data, size_t size);
static bool obj_set_handler_vertex_buffer_load(obj_set_handler* handler);
static void obj_set_handler_vertex_buffer_free(obj_set_handler* handler);
static void obj_skin_block_constraint_attach_point_load(
    obj_skin_block_constraint_attach_point* attach_point,
    obj_skin_block_constraint_attach_point* attach_point_file);
static void obj_skin_block_constraint_up_vector_load(
    obj_skin_block_constraint_up_vector* up_vector,
    obj_skin_block_constraint_up_vector* up_vector_file);
static void obj_skin_block_node_load(obj_skin_block_node* node,
    obj_skin_block_node* node_file);
static void obj_skin_block_node_free(obj_skin_block_node* node);
static size_t obj_vertex_format_get_vertex_size(obj_vertex_format format);
static size_t obj_vertex_format_get_vertex_size_comp(obj_vertex_format format);

std::vector<obj_set_handler> object_storage_data;
std::list<obj_set_handler> object_storage_data_modern;

inline int32_t obj_material_texture_type_get_texcoord_index(
    obj_material_texture_type type, int32_t index) {
    switch (type) {
    case OBJ_MATERIAL_TEXTURE_COLOR:
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (index < 2)
            return index;
    case OBJ_MATERIAL_TEXTURE_NORMAL:
    case OBJ_MATERIAL_TEXTURE_SPECULAR:
        return 0;
    case OBJ_MATERIAL_TEXTURE_TRANSLUCENCY:
    case OBJ_MATERIAL_TEXTURE_TRANSPARENCY:
        return 1;
    }
    return -1;
}

inline int32_t obj_material_texture_type_get_texture_index(
    obj_material_texture_type type, int32_t base_index) {
    switch (type) {
    case OBJ_MATERIAL_TEXTURE_COLOR:
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (base_index < 2)
            return base_index;
    case OBJ_MATERIAL_TEXTURE_NORMAL:
        return 2;
    case OBJ_MATERIAL_TEXTURE_SPECULAR:
        return 3;
    case OBJ_MATERIAL_TEXTURE_TRANSLUCENCY:
        return 1;
    case OBJ_MATERIAL_TEXTURE_TRANSPARENCY:
        return 4;
    //case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // AFT
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE:
        return 5;
    }
    return -1;
}

inline GLuint obj_mesh_vertex_buffer_get_buffer(obj_mesh_vertex_buffer* buffer) {
    if (buffer->index < buffer->count)
        return buffer->buffers[buffer->index];
    else
        return 0;
}

inline int32_t obj_mesh_vertex_buffer_get_size(obj_mesh_vertex_buffer* buffer) {
    if (!buffer->buffers[0])
        return 0;

    int32_t size = 0;
    gl_state_bind_array_buffer(buffer->buffers[0]);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_state_bind_array_buffer(buffer->buffers[0]);
    return size;
}

void obj_skin_set_matrix_buffer(obj_skin* s, mat4* matrices,
    mat4* ex_data_matrices, mat4* matrix_buffer, mat4* mat, mat4* global_mat) {
    if (!s->bones_count)
        return;

    if (mat)
        for (uint32_t i = 0; i < s->bones_count; i++) {
            mat4 temp;
            int32_t bone_id = s->bones[i].id;
            if (bone_id & 0x8000)
                mat4_mult(mat, &ex_data_matrices[bone_id & 0x7FFF], &temp);
            else
                mat4_mult(mat, &matrices[bone_id], &temp);

            mat4_mult(&temp, global_mat, &temp);

            mat4 inv_bind_pose_mat = s->bones[i].inv_bind_pose_mat;
            mat4_mult(&inv_bind_pose_mat, &temp, &temp);
            matrix_buffer[i] = temp;
        }
    else
        for (uint32_t i = 0; i < s->bones_count; i++) {
            mat4 temp;
            int32_t bone_id = s->bones[i].id;
            if (bone_id & 0x8000)
                temp = ex_data_matrices[bone_id & 0x7FFF];
            else
                temp = matrices[bone_id];

            mat4_mult(&temp, global_mat, &temp);

            mat4 inv_bind_pose_mat = s->bones[i].inv_bind_pose_mat;
            mat4_mult(&inv_bind_pose_mat, &temp, &temp);
            matrix_buffer[i] = temp;
        }
}

inline void object_storage_init(object_database* obj_db) {
    object_set_info* obj_set = obj_db->object_set.data();
    size_t count = obj_db->object_set.size();
    object_storage_data.resize(count);
    for (size_t i = 0; i < count; i++) {
        object_storage_data[i].set_id = obj_set[i].id;
        object_storage_data[i].name = obj_set[i].name;
    }
    object_storage_data_modern.clear();
}

inline obj* object_storage_get_obj(object_info obj_info) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j].id == obj_info.id)
                return &set->obj_data[j];
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j].id == obj_info.id)
                return &set->obj_data[j];
        return 0;
    }
    return 0;
}

inline obj_set_handler* object_storage_get_obj_set_handler(uint32_t set_id) {
    for (obj_set_handler& i : object_storage_data)
        if (i.set_id == set_id)
            return &i;

    for (obj_set_handler& i : object_storage_data_modern)
        if (i.set_id == set_id)
            return &i;
    return 0;
}

inline obj_set_handler* object_storage_get_obj_set_handler_by_index(size_t index) {
    if (index >= 0 && index < object_storage_data.size())
        return &object_storage_data[index];

    index -= object_storage_data.size();
    if (index >= 0 && index < object_storage_data_modern.size()) {
        auto elem = object_storage_data_modern.begin();
        std::advance(elem, index);
        return &*elem;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh(object_info obj_info, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return &obj->mesh_array[k];
            return 0;
        }
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return &obj->mesh_array[k];
            return 0;
        }
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_index(object_info obj_info, uint32_t index) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            obj* obj = &set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            obj* obj = &set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_object_hash(uint32_t hash, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash != hash)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return &obj->mesh_array[k];
            return 0;
        }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash == hash)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return &obj->mesh_array[k];
            return 0;
        }
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_object_hash_index(uint32_t hash, uint32_t index) {
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash != hash)
                continue;

            obj* obj = &set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash != hash)
                continue;

            obj* obj = &set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
    }
    return 0;
}

inline uint32_t object_storage_get_obj_mesh_index(object_info obj_info, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return k;
            return -1;
        }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].id != obj_info.id)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return k;
            return -1;
        }
    }
    return -1;
}

inline uint32_t object_storage_get_obj_mesh_index_by_hash(uint32_t hash, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash != hash)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return k;
            return -1;
        }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j].hash != hash)
                continue;

            uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
            obj* obj = &set->obj_data[j];
            for (uint32_t k = 0; k < obj->num_mesh; k++)
                if (hash_utf8_murmurhash(obj->mesh_array[k].name) == mesh_name_hash)
                    return k;
            return -1;
        }
    }
    return -1;
}

inline obj_set* object_storage_get_obj_set(uint32_t set_id) {
    for (obj_set_handler& i : object_storage_data)
        if (i.set_id == set_id)
            return i.obj_set;

    for (obj_set_handler& i : object_storage_data_modern)
        if (i.set_id == set_id)
            return i.obj_set;
    return 0;
}

inline size_t object_storage_get_obj_set_count() {
    return object_storage_data.size() + object_storage_data_modern.size();
}

inline int32_t object_storage_get_obj_storage_load_count(uint32_t set_id) {
    for (obj_set_handler& i : object_storage_data)
        if (i.set_id == set_id)
            return i.load_count;

    for (obj_set_handler& i : object_storage_data_modern)
        if (i.set_id == set_id)
            return i.load_count;
    return 0;
}

inline obj_set* object_storage_get_obj_set_by_index(size_t index) {
    if (index >= 0 && index < object_storage_data.size())
        return object_storage_data[index].obj_set;

    index -= object_storage_data.size();
    if (index >= 0 && index < object_storage_data_modern.size()) {
        auto elem = object_storage_data_modern.begin();
        std::advance(elem, index);
        return elem->obj_set;
    }
    return 0;
}

inline int32_t object_storage_get_obj_storage_load_count_by_index(size_t index) {
    if (index >= 0 && index < object_storage_data.size())
        return object_storage_data[index].load_count;

    index -= object_storage_data.size();
    if (index >= 0 && index < object_storage_data_modern.size()) {
        auto elem = object_storage_data_modern.begin();
        std::advance(elem, index);
        return elem->load_count;
    }
    return 0;
}

inline size_t object_storage_get_obj_set_index(uint32_t set_id) {
    for (obj_set_handler& i : object_storage_data)
        if (i.set_id == set_id)
            return &i - object_storage_data.data();

    size_t index = 0;
    for (obj_set_handler& i : object_storage_data_modern)
        if (i.set_id == set_id)
            return object_storage_data.size() + index;
        else
            index++;
    return -1;
}

inline obj_skin* object_storage_get_obj_skin(object_info obj_info) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j].id == obj_info.id)
                return set->obj_data[j].skin_init ? &set->obj_data[j].skin : 0;
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j].id == obj_info.id)
                return set->obj_data[j].skin_init ? &set->obj_data[j].skin : 0;
        return 0;
    }
    return 0;
}

inline obj_index_buffer* object_storage_get_obj_index_buffers(uint32_t set_id) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (handler && handler->index_buffer_data)
        return handler->index_buffer_data;
    return 0;
}

inline obj_mesh_index_buffer* object_storage_get_obj_mesh_index_buffer(object_info obj_info) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(obj_info.set_id);
    if (handler && handler->obj_set && handler->index_buffer_data) {
        obj_set* set = handler->obj_set;
        for (uint32_t i = 0; i < set->obj_num; i++)
            if (set->obj_data[i].id == obj_info.id)
                return handler->index_buffer_data[i].mesh_data;
    }
    return 0;
}

inline obj_vertex_buffer* object_storage_get_obj_vertex_buffers(uint32_t set_id) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (handler && handler->vertex_buffer_data)
        return handler->vertex_buffer_data;
    return 0;
}

inline obj_mesh_vertex_buffer* object_storage_get_obj_mesh_vertex_buffer(object_info obj_info) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(obj_info.set_id);
    if (handler && handler->obj_set && handler->vertex_buffer_data) {
        obj_set* set = handler->obj_set;
        for (uint32_t i = 0; i < set->obj_num; i++)
            if (set->obj_data[i].id == obj_info.id)
                return handler->vertex_buffer_data[i].mesh_data;
    }
    return 0;
}

GLuint obj_database_get_obj_set_texture(int32_t set, uint32_t tex_id) {
    std::vector<GLuint>* textures = object_storage_get_obj_set_textures(set);
    if (!textures)
        return -1;

    obj_set_handler* handler = object_storage_get_obj_set_handler(set);
    if (!handler)
        return -1;

    std::pair<uint32_t, uint32_t>* texture = handler->tex_id_data.data();
    size_t length = handler->tex_id_data.size();
    size_t temp;
    while (length > 0)
        if (tex_id < texture[temp = length / 2].first)
            length /= 2;
        else {
            texture += temp + 1;
            length -= temp + 1;
        }

    if (texture != handler->tex_id_data.data() + handler->tex_id_data.size())
        return (*textures)[texture->second];
    return -1;
}

inline std::vector<GLuint>* object_storage_get_obj_set_textures(int32_t set) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set);
    if (handler)
        return &handler->gentex;
    return 0;
}

int32_t object_storage_load_set(void* data, object_database* obj_db, const char* name) {
    object_set_info* set_info = 0;
    if (!obj_db->get_object_set_info(name, &set_info))
        return 1;

    obj_set_handler* handler = object_storage_get_obj_set_handler(set_info->id);
    if (!handler)
        return 1;

    if (handler->load_count >= 0) {
        handler->load_count++;
        return 1;
    }

    std::string* archive_file_name = &set_info->archive_file_name;
    std::string* object_file_name = &set_info->object_file_name;
    std::string* texture_file_name = &set_info->texture_file_name;
    if (!object_file_name->size() || !texture_file_name->size())
        return 1;

    if (archive_file_name->size()) {
        handler->obj_file_handler.read_file(data, "rom/objset/",
            archive_file_name->c_str(), object_file_name->c_str(), false);
        handler->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name->c_str(), texture_file_name->c_str(), false);
    }
    else {
        handler->obj_file_handler.read_file(data, "rom/objset/", object_file_name->c_str());
        handler->tex_file_handler.read_file(data, "rom/objset/", texture_file_name->c_str());
    }

    handler->load_count = 1;
    handler->obj_loaded = false;
    handler->tex_loaded = false;
    return 0;
}

int32_t object_storage_load_set(void* data, object_database* obj_db, uint32_t set_id) {
    object_set_info* set_info = 0;
    if (!obj_db->get_object_set_info(set_id, &set_info))
        return 1;

    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (!handler)
        return 1;

    if (handler->load_count > 0) {
        handler->load_count++;
        return 1;
    }

    handler->modern = false;

    std::string* archive_file_name = &set_info->archive_file_name;
    std::string* object_file_name = &set_info->object_file_name;
    std::string* texture_file_name = &set_info->texture_file_name;
    if (!object_file_name->size() || !texture_file_name->size())
        return 1;

    if (archive_file_name->size()) {
        handler->obj_file_handler.read_file(data, "rom/objset/",
            archive_file_name->c_str(), object_file_name->c_str(), false);
        handler->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name->c_str(), texture_file_name->c_str(), false);
    }
    else {
        handler->obj_file_handler.read_file(data, "rom/objset/", object_file_name->c_str());
        handler->tex_file_handler.read_file(data, "rom/objset/", texture_file_name->c_str());
    }

    handler->load_count = 1;
    handler->obj_loaded = false;
    handler->tex_loaded = false;
    return 0;
}

int32_t object_storage_load_set_hash(void* data, uint32_t hash) {
    if (!hash || hash == hash_murmurhash_empty)
        return 1;

    std::string file;
    if (!((data_struct*)data)->get_file("root+/objset/", hash, ".farc", &file))
        return 1;

    obj_set_handler* handler = object_storage_get_obj_set_handler(hash);
    if (!handler) {
        object_storage_data_modern.push_back({});
        handler = &object_storage_data_modern.back();
        handler->set_id = hash;
    }

    if (handler->load_count > 0) {
        handler->load_count++;
        return 1;
    }

    handler->modern = true;

    handler->farc_file_handler.read_file(data, "root+/objset/", file.c_str());

    handler->load_count = 1;
    handler->obj_loaded = false;
    handler->tex_loaded = false;
    return 0;
}

bool object_storage_load_obj_set_check_not_read(uint32_t set_id,
    object_database* obj_db, texture_database* tex_db) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (!handler)
        return true;

    if (!handler->modern) {
        if (!handler->obj_loaded && !handler->obj_file_handler.check_not_ready()) {
            void* data = handler->obj_file_handler.get_data();
            size_t size = handler->obj_file_handler.get_size();
            if (!data || !size)
                return false;

            obj_set* set = new obj_set;
            handler->obj_set = set;
            set->unpack_file(data, size, false);
            if (!set->ready)
                return false;

            handler->obj_file_handler.free_data();
            handler->obj_id_data.reserve(set->obj_num);
            for (uint32_t i = 0; i < set->obj_num; i++)
                handler->obj_id_data.push_back({ set->obj_data[i].id, i });

            if (!obj_set_handler_vertex_buffer_load(handler)
                || !obj_set_handler_index_buffer_load(handler))
                return false;

            obj_set_handler_calc_axis_aligned_bounding_box(handler);
            handler->obj_loaded = true;
        }

        if (!handler->obj_loaded)
            return true;

        if (!handler->tex_loaded && !handler->tex_file_handler.check_not_ready()) {
            if (!handler->tex_file_handler.get_data())
                return false;
            else if (obj_set_handler_load_textures(handler,
                handler->tex_file_handler.get_data(), false))
                return false;

            obj_set_handler_get_shader_index_texture_index(handler);
            handler->tex_file_handler.free_data();
            handler->tex_loaded = true;
        }
    }
    else if (!handler->obj_loaded && !handler->farc_file_handler.check_not_ready()) {
        void* data = handler->farc_file_handler.get_data();
        size_t size = handler->farc_file_handler.get_size();
        if (!data || !size)
            return false;

        farc f;
        f.read(data, size, true);

        std::string* file = &handler->farc_file_handler.ptr->file;

        size_t file_len = file->size();
        if (file_len >= 0x100)
            return false;

        const char* t = strrchr(file->c_str(), '.');
        if (t)
            file_len = t - file->c_str();

        char buf[0x100];
        memcpy(buf, file->c_str(), file_len);
        char* ext = buf + file_len;
        size_t ext_len = sizeof(buf) - file_len;

        memcpy_s(ext, ext_len, ".osd", 5);
        farc_file* osd = f.read_file(buf);
        if (!osd)
            return false;

        memcpy_s(ext, ext_len, ".txd", 5);
        farc_file* txd = f.read_file(buf);
        if (!txd)
            return false;

        memcpy_s(ext, ext_len, ".osi", 5);
        farc_file* osi = f.read_file(buf);
        if (!osi)
            return false;

        memcpy_s(ext, ext_len, ".txi", 5);
        farc_file* txi = f.read_file(buf);
        if (!txi)
            return false;

        object_database local_obj_db;
        local_obj_db.read(osi->data, osi->size, true);

        texture_database local_tex_db;
        local_tex_db.read(txi->data, txi->size, true);

        std::string name;
        if (local_obj_db.ready)
            for (object_set_info& m : local_obj_db.object_set)
                if (m.id == handler->set_id) {
                    name = m.name;
                    break;
                }

        if (obj_db)
            obj_db->merge_mdata(obj_db, &local_obj_db);

        if (tex_db)
            tex_db->merge_mdata(tex_db, &local_tex_db);

        if (!name.size())
            return false;

        handler->name = name;

        obj_set* set = new obj_set;
        handler->obj_set = set;
        set->unpack_file(osd->data, osd->size, true);
        if (!set->ready)
            return false;

        handler->obj_file_handler.free_data();
        handler->obj_id_data.reserve(set->obj_num);
        for (uint32_t i = 0; i < set->obj_num; i++)
            handler->obj_id_data.push_back({ set->obj_data[i].id, i });

        if (!obj_set_handler_vertex_buffer_load(handler)
            || !obj_set_handler_index_buffer_load(handler))
            return false;

        obj_set_handler_calc_axis_aligned_bounding_box(handler);
        handler->obj_loaded = true;

        if (obj_set_handler_load_textures_modern(handler, txd->data, txd->size))
            return false;

        obj_set_handler_get_shader_index_texture_index(handler);
        handler->tex_loaded = true;
        handler->farc_file_handler.free_data();
    }

    if (handler->obj_loaded && handler->tex_loaded)
        return false;
    return true;
}

inline void object_storage_unload_set(uint32_t set_id) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (!handler || handler->load_count <= 0)
        return;

    if (--handler->load_count > 0)
        return;

    handler->obj_id_data.clear();
    handler->tex_id_data.clear();
    handler->gentex.clear();

    texture** tex_data = handler->tex_data;
    int32_t tex_num = handler->tex_num;
    for (int32_t i = 0; i < tex_num; i++)
        texture_free(tex_data[i]);
    free(tex_data);
    handler->tex_data = 0;
    handler->tex_num = 0;

    obj_set_handler_index_buffer_free(handler);
    obj_set_handler_vertex_buffer_free(handler);
    handler->load_count = 0;
    handler->tex_loaded = false;
    handler->obj_loaded = false;
    delete handler->obj_set;
    handler->obj_set = 0;
    handler->tex_file_handler.free_data();
    handler->obj_file_handler.free_data();
    handler->farc_file_handler.free_data();
    if (handler->modern)
        for (auto i = object_storage_data_modern.begin(); i != object_storage_data_modern.end(); i++)
            if (i->set_id = set_id) {
                i = object_storage_data_modern.erase(i);
                break;
            }
}

inline void object_storage_free() {
    object_storage_data.clear();
    object_storage_data.shrink_to_fit();
    object_storage_data_modern.clear();
}

static bool obj_mesh_index_buffer_load(obj_mesh_index_buffer* buffer, obj_mesh* mesh) {
    int32_t indices_count = 0;
    for (uint32_t i = 0; i < mesh->num_submesh; i++)
        indices_count += mesh->submesh_array[i].indices_count;

    if (!indices_count) {
        buffer->buffer = 0;
        return true;
    }

    uint16_t* indices = force_malloc_s(uint16_t, indices_count);
    uint16_t* _indices = indices;
    for (uint32_t k = 0; k < mesh->num_submesh; k++) {
        obj_sub_mesh* sub_mesh = &mesh->submesh_array[k];
        int32_t indices_count = sub_mesh->indices_count;
        uint32_t* sub_mesh_indices = sub_mesh->indices;
        for (int32_t l = 0; l < indices_count; l++)
            *_indices++ = (uint16_t)sub_mesh_indices[l];
    }

    _indices = indices;
    for (uint32_t i = 0, offset = 0; i < mesh->num_submesh; i++) {
        obj_sub_mesh* sub_mesh = &mesh->submesh_array[i];

        sub_mesh->first_index = 0;
        sub_mesh->last_index = 0;
        sub_mesh->indices_offset = 0;
        if (sub_mesh->index_format != OBJ_INDEX_U16)
            continue;

        uint16_t first_index = 0xFFFF;
        uint16_t last_index = 0;
        int32_t indices_count = sub_mesh->indices_count;
        uint32_t* sub_mesh_indices = sub_mesh->indices;
        for (int32_t j = 0; j < indices_count; j++) {
            uint16_t index = *_indices++;
            if (index == 0xFFFF)
                continue;

            if (index < first_index)
                first_index = index;
            if (index > last_index)
                last_index = index;
        }

        sub_mesh->first_index = first_index;
        sub_mesh->last_index = last_index;
        sub_mesh->indices_offset = (int32_t)(sizeof(uint16_t) * offset);
        offset += sub_mesh->indices_count;
    }

    buffer->buffer = obj_mesh_index_buffer_load_data((size_t)indices_count * sizeof(uint16_t), indices);
    free(indices);
    return true;
}

static GLuint obj_mesh_index_buffer_load_data(size_t size, void* data) {
    if (!size)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    gl_state_bind_element_array_buffer(buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    gl_state_bind_element_array_buffer(0);
    return buffer;
}

static void obj_mesh_index_buffer_free(obj_mesh_index_buffer* buffer) {
    glDeleteBuffers(1, &buffer->buffer);
}

static bool obj_mesh_vertex_buffer_load(obj_mesh_vertex_buffer* buffer, obj_mesh* mesh) {
    if (!mesh->num_vertex || !mesh->vertex)
        return false;

    size_t size_vertex;
    if (~mesh->attrib.m.compressed)
        size_vertex = obj_vertex_format_get_vertex_size(mesh->vertex_format);
    else
        size_vertex = obj_vertex_format_get_vertex_size_comp(mesh->vertex_format);

    void* vertex = force_malloc(size_vertex * mesh->num_vertex);
    if (vertex) {
        obj_vertex_format vertex_format = mesh->vertex_format;
        obj_vertex_data* vertex_file = mesh->vertex;
        int32_t num_vertex = mesh->num_vertex;
        size_t d = (size_t)vertex;
        if (~mesh->attrib.m.compressed) {
            for (int32_t i = 0; i < num_vertex; i++) {
                if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
                    *(vec3*)d = vertex_file[i].position;
                    d += 12;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
                    *(vec3*)d = vertex_file[i].normal;
                    d += 12;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
                    *(vec4u*)d = vertex_file[i].tangent;
                    d += 16;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL) {
                    *(vec3*)d = vertex_file[i].binormal;
                    d += 12;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
                    *(vec2*)d = vertex_file[i].texcoord0;
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
                    *(vec2*)d = vertex_file[i].texcoord1;
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2) {
                    *(vec2*)d = vertex_file[i].texcoord2;
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3) {
                    *(vec2*)d = vertex_file[i].texcoord3;
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
                    *(vec4u*)d = vertex_file[i].color0;
                    d += 16;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1) {
                    *(vec4u*)d = vertex_file[i].color1;
                    d += 16;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
                    *(vec4u*)d = vertex_file[i].bone_weight;
                    d += 16;
                    *(vec4iu*)d = vertex_file[i].bone_index;
                    d += 16;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN) {
                    *(vec4u*)d = vertex_file[i].unknown;
                    d += 16;
                }
            }
        }
        else {
            for (int32_t i = 0; i < num_vertex; i++) {
                if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
                    *(vec3*)d = vertex_file[i].position;
                    d += 12;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
                    vec3 normal;
                    vec3_mult_scalar(vertex_file[i].normal, 32727.0f, normal);
                    vec3_to_vec3i16(normal, *(vec3i16*)d);
                    *(int16_t*)(d + 6) = 0;
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
                    vec4 tangent = vertex_file[i].tangent;
                    vec4_mult_scalar(tangent, 32727.0f, tangent);
                    vec4_to_vec4i16(tangent, *(vec4i16*)d);
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
                    vec2_to_vec2h(vertex_file[i].texcoord0, *(vec2h*)d);
                    d += 4;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
                    vec2_to_vec2h(vertex_file[i].texcoord1, *(vec2h*)d);
                    d += 4;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2) {
                    vec2_to_vec2h(vertex_file[i].texcoord2, *(vec2h*)d);
                    d += 4;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3) {
                    vec2_to_vec2h(vertex_file[i].texcoord3, *(vec2h*)d);
                    d += 4;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
                    vec4 color0 = vertex_file[i].color0;
                    vec4_to_vec4h(color0, *(vec4h*)d);
                    d += 8;
                }

                if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
                    vec4 bone_weight = vertex_file[i].bone_weight;
                    vec4_mult_scalar(bone_weight, 65535.0f, bone_weight);
                    vec4_to_vec4u16(bone_weight, *(vec4u16*)d);
                    d += 8;

                    vec4i bone_index = vertex_file[i].bone_index;
                    vec4i_to_vec4u16(bone_index, *(vec4u16*)d);
                    d += 8;
                }
            }
        }
    }
    mesh->size_vertex = (int32_t)size_vertex;

    bool ret = obj_mesh_vertex_buffer_load_data(buffer, size_vertex * mesh->num_vertex,
        vertex, mesh->attrib.m.flag_0 ? 2 : 1);
    free(vertex);
    return ret;
}

static bool obj_mesh_vertex_buffer_load_data(obj_mesh_vertex_buffer* buffer,
    size_t size, void* data, int32_t count) {
    if (!size || count > 3)
        return false;

    buffer->count = count;

    glGenBuffers(count, buffer->buffers);
    for (int32_t i = 0; i < count; i++) {
        gl_state_bind_array_buffer(buffer->buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    }
    gl_state_bind_array_buffer(0);
    return true;
}

static void obj_mesh_vertex_buffer_free(obj_mesh_vertex_buffer* buffer) {
    glDeleteBuffers(buffer->count, buffer->buffers);
}

static void obj_set_handler_calc_axis_aligned_bounding_box(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = &set->obj_data[i];
        for (uint32_t j = 0; j < obj->num_mesh; j++) {
            obj_mesh* mesh = &obj->mesh_array[j];
            for (uint32_t k = 0; k < mesh->num_submesh; k++) {
                vec3 _min = { 9999999.0f, 9999999.0f, 9999999.0f };
                vec3 _max = { -100000000.0f, -100000000.0f, -100000000.0f };

                obj_sub_mesh* sub_mesh = &mesh->submesh_array[k];
                uint32_t* indices = (uint32_t*)sub_mesh->indices;
                uint32_t indices_count = sub_mesh->indices_count;
                obj_vertex_data* vertex = mesh->vertex;
                if (sub_mesh->index_format == OBJ_INDEX_U16)
                    for (uint32_t l = 0; l < indices_count; l++) {
                        if (indices[l] == 0xFFFFFFFF)
                            continue;

                        vec3 pos = vertex[indices[l]].position;
                        vec3_min(_min, pos, _min);
                        vec3_max(_max, pos, _max);
                    }
                else
                    for (uint32_t l = 0; l < indices_count; l++) {
                        vec3 pos = vertex[indices[l]].position;
                        vec3_min(_min, pos, _min);
                        vec3_max(_max, pos, _max);
                    }

                vec3 center;
                vec3 size;
                vec3_add(_max, _min, center);
                vec3_mult_scalar(center, 0.5f, center);
                vec3_sub(_max, center, size);
                sub_mesh->axis_aligned_bounding_box.center = center;
                sub_mesh->axis_aligned_bounding_box.size = size;
            }
        }
    }
}

static void obj_set_handler_get_shader_index_texture_index(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = &set->obj_data[i];
        uint32_t num_material = obj->num_material;
        for (uint32_t j = 0; j < num_material; j++) {
            obj_material_data* material_data = &obj->material_array[j];
            obj_material* material = &material_data->material;

            if (*(int32_t*)&material->shader.name[4] != 0xDEADFF) {
                material->shader.index = shader_get_index_by_name(&shaders_ft, material->shader.name);
                *(int32_t*)&material->shader.name[4] = 0xDEADFF;
            }

            for (obj_material_texture_data& k : material->texdata) {
                obj_material_texture_data* texture = &k;
                uint32_t texture_id = texture->tex_index;
                texture->tex_index = 0;
                texture->texture_index = 0;

                std::pair<uint32_t, uint32_t>* tex_id_data = handler->tex_id_data.data();
                uint32_t tex_id_num = (uint32_t)handler->tex_id_data.size();
                for (uint32_t l = tex_id_num; l; l--, tex_id_data++)
                    if (tex_id_data->first == texture_id) {
                        texture->tex_index = texture_id;
                        texture->texture_index = tex_id_data->second;
                        break;
                    }
            }
        }
    }
}

static bool obj_set_handler_index_buffer_load(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    handler->index_buffer_num = set->obj_num;
    handler->index_buffer_data = force_malloc_s(obj_index_buffer, set->obj_num);
    if (!handler->index_buffer_data)
        return true;

    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = &set->obj_data[i];
        obj_index_buffer* buffer = &handler->index_buffer_data[i];

        buffer->mesh_num = obj->num_mesh;
        buffer->mesh_data = force_malloc_s(obj_mesh_index_buffer, obj->num_mesh);
        if (!buffer->mesh_data)
            return false;

        for (uint32_t j = 0; j < buffer->mesh_num; j++)
            if (!obj_mesh_index_buffer_load(&buffer->mesh_data[j], &obj->mesh_array[j]))
                return false;
    }
    return true;
}

static void obj_set_handler_index_buffer_free(obj_set_handler* handler) {
    if (!handler->index_buffer_data)
        return;

    for (uint32_t i = 0; i < handler->index_buffer_num; i++) {
        obj_index_buffer* buffer = &handler->index_buffer_data[i];
        if (buffer->mesh_data)
            for (uint32_t j = 0; j < buffer->mesh_num; j++)
                obj_mesh_index_buffer_free(&buffer->mesh_data[j]);
        free(buffer->mesh_data);
    }
    free(handler->index_buffer_data);
    handler->index_buffer_data = 0;
    handler->index_buffer_num = 0;
}

static int32_t obj_set_tex_id_data_sort(void const* src1, void const* src2) {
    std::pair<uint32_t, uint32_t>* p1 = (std::pair<uint32_t, uint32_t>*)src1;
    std::pair<uint32_t, uint32_t>* p2 = (std::pair<uint32_t, uint32_t>*)src2;
    return p1->first - p2->first;
}

static bool obj_set_handler_load_textures(obj_set_handler* handler, void* data, bool big_endian) {
    obj_set* set = handler->obj_set;
    if (!set || !data)
        return true;
    else if (!set->tex_id_num)
        return false;

    {
        txp_set txp;
        txp.unpack_file(data, big_endian);
        handler->tex_num = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &handler->tex_data, set->tex_id_data);
    }

    handler->tex_id_data.reserve(handler->tex_num);
    handler->gentex.reserve(handler->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    int32_t tex_num = handler->tex_num;
    texture** tex_data = handler->tex_data;
    for (int32_t i = 0; i < tex_num; i++) {
        handler->tex_id_data.push_back({ tex_id_data[i], i });
        handler->gentex.push_back(tex_data[i]->tex);
    }
    return false;
}

static bool obj_set_handler_load_textures_modern(obj_set_handler* handler, void* data, size_t size) {
    obj_set* set = handler->obj_set;
    if (!set || !data || !size)
        return true;
    else if (!set->tex_id_num)
        return false;

    {
        txp_set txp;
        txp.unpack_file_modern(data, size);
        handler->tex_num = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &handler->tex_data, set->tex_id_data);
    }

    handler->tex_id_data.reserve(handler->tex_num);
    handler->gentex.reserve(handler->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    uint32_t tex_num = handler->tex_num;
    texture** tex_data = handler->tex_data;
    for (uint32_t i = 0; i < tex_num; i++) {
        handler->tex_id_data.push_back({ tex_id_data[i], i });
        handler->gentex.push_back(tex_data[i]->tex);
    }
    return false;
}

static bool obj_set_handler_vertex_buffer_load(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    handler->vertex_buffer_num = set->obj_num;
    handler->vertex_buffer_data = force_malloc_s(obj_vertex_buffer, set->obj_num);
    if (!handler->vertex_buffer_data)
        return true;

    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = &set->obj_data[i];
        obj_vertex_buffer* buffer = &handler->vertex_buffer_data[i];

        buffer->mesh_num = obj->num_mesh;
        buffer->mesh_data = force_malloc_s(obj_mesh_vertex_buffer, obj->num_mesh);
        if (!buffer->mesh_data)
            return false;

        for (uint32_t j = 0; j < buffer->mesh_num; j++)
            if (!obj_mesh_vertex_buffer_load(&buffer->mesh_data[j], &obj->mesh_array[j]))
                return false;
    }
    return true;
}

static void obj_set_handler_vertex_buffer_free(obj_set_handler* handler) {
    if (!handler->vertex_buffer_data)
        return;

    for (uint32_t i = 0; i < handler->vertex_buffer_num; i++) {
        obj_vertex_buffer* buffer = &handler->vertex_buffer_data[i];
        if (buffer->mesh_data)
            for (uint32_t j = 0; j < buffer->mesh_num; j++)
                obj_mesh_vertex_buffer_free(&buffer->mesh_data[j]);
        free(buffer->mesh_data);
    }
    free(handler->vertex_buffer_data);
    handler->vertex_buffer_data = 0;
    handler->vertex_buffer_num = 0;
}

inline static void obj_skin_block_constraint_attach_point_load(
    obj_skin_block_constraint_attach_point* attach_point,
    obj_skin_block_constraint_attach_point* attach_point_file) {
    attach_point->affected_by_orientation = attach_point_file->affected_by_orientation;
    attach_point->affected_by_scaling = attach_point_file->affected_by_scaling;
    attach_point->offset = attach_point_file->offset;
}
inline static void obj_skin_block_constraint_up_vector_load(
    obj_skin_block_constraint_up_vector* up_vector,
    obj_skin_block_constraint_up_vector* up_vector_file) {
    up_vector->active = up_vector_file->active;
    up_vector->roll = up_vector_file->roll;
    up_vector->affected_axis = up_vector_file->affected_axis;
    up_vector->point_at = up_vector_file->point_at;
    string_copy(&up_vector_file->name, &up_vector->name);
}

inline static void obj_skin_block_node_load(obj_skin_block_node* node,
    obj_skin_block_node* node_file) {
    string_copy(&node_file->parent_name, &node->parent_name);
    node->position = node_file->position;
    node->rotation = node_file->rotation;
    node->scale = node_file->scale;
}

inline static void obj_skin_block_node_free(obj_skin_block_node* node) {
    string_free(&node->parent_name);
}

inline static size_t obj_vertex_format_get_vertex_size(obj_vertex_format format) {
    size_t size = 0;
    if (format & OBJ_VERTEX_FORMAT_POSITION)
        size += 12;
    if (format & OBJ_VERTEX_FORMAT_NORMAL)
        size += 12;
    if (format & OBJ_VERTEX_FORMAT_TANGENT)
        size += 16;
    if (format & OBJ_VERTEX_FORMAT_BINORMAL)
        size += 12;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_COLOR0)
        size += 16;
    if (format & OBJ_VERTEX_FORMAT_COLOR1)
        size += 16;
    if (format & OBJ_VERTEX_FORMAT_BONE_DATA)
        size += 32;
    if (format & OBJ_VERTEX_FORMAT_UNKNOWN)
        size += 16;
    return size;
}

inline static size_t obj_vertex_format_get_vertex_size_comp(obj_vertex_format format) {
    size_t size = 0;
    if (format & OBJ_VERTEX_FORMAT_POSITION)
        size += 12;
    if (format & OBJ_VERTEX_FORMAT_NORMAL)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_TANGENT)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        size += 4;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        size += 4;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        size += 4;
    if (format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        size += 4;
    if (format & OBJ_VERTEX_FORMAT_COLOR0)
        size += 8;
    if (format & OBJ_VERTEX_FORMAT_BONE_DATA)
        size += 16;
    return size;
}

obj_set_handler::obj_set_handler(): obj_loaded(), tex_loaded(), obj_set(),
tex_num(), tex_data(), set_id(), vertex_buffer_num(), vertex_buffer_data(),
index_buffer_num(), index_buffer_data(), load_count(), modern() {

}

obj_set_handler::~obj_set_handler() {
    for (uint32_t i = 0; i < tex_num; i++)
        texture_free(tex_data[i]);
    free(tex_data);

    obj_set_handler_index_buffer_free(this);
    obj_set_handler_vertex_buffer_free(this);
    delete obj_set;
    while (tex_file_handler.ptr && tex_file_handler.ptr->count)
        tex_file_handler.free_data();
    while (obj_file_handler.ptr && obj_file_handler.ptr->count)
        obj_file_handler.free_data();
    while (farc_file_handler.ptr && farc_file_handler.ptr->count)
        farc_file_handler.free_data();
}
