/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <list>
#include <map>
#include <vector>
#include "object.h"
#include "data.h"
#include "gl_state.h"
#include "shader_ft.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"

typedef struct material_change_handler {
    int32_t load_count;
    material_change data;
} material_change_handler;

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
static size_t obj_vertex_flags_get_vertex_size(obj_vertex_flags flags);
static size_t obj_vertex_flags_get_vertex_size_comp(obj_vertex_flags flags);

std::map<uint32_t, material_change_handler> material_change_storage_data;
std::vector<obj_set_handler> object_storage_data;
std::list<obj_set_handler> object_storage_data_modern;

inline obj_material_shader_lighting_type obj_material_shader_get_lighting_type(
    obj_material_shader_flags* flags) {
    if (!flags->lambert_shading && !flags->phong_shading)
        return OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT;
    else if (!flags->phong_shading)
        return OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT;
    else
        return OBJ_MATERIAL_SHADER_LIGHTING_PHONG;
}

inline int32_t obj_material_texture_get_blend(obj_material_texture* tex) {
    switch (tex->sampler_flags.blend) {
    case 4:
        return 2;
    case 6:
        return 1;
    case 16:
        return 3;
    default:
        return 0;
    }
}

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
        for (int32_t i = 0; i < s->bones_count; i++) {
            mat4 temp;
            int32_t bone_id = s->bones[i].id;
            if (bone_id & 0x8000)
                if (ex_data_matrices)
                    mat4_mult(mat, &ex_data_matrices[bone_id & 0x7FFF], &temp);
                else
                    temp = *mat;
            else
                mat4_mult(mat, &matrices[bone_id], &temp);

            mat4_mult(&temp, global_mat, &temp);

            mat4 inv_bind_pose_mat = s->bones[i].inv_bind_pose_mat;
            mat4_mult(&inv_bind_pose_mat, &temp, &matrix_buffer[i]);
        }
    else
        for (int32_t i = 0; i < s->bones_count; i++) {
            mat4 temp;
            int32_t bone_id = s->bones[i].id;
            if (bone_id & 0x8000)
                if (ex_data_matrices)
                    temp = ex_data_matrices[bone_id & 0x7FFF];
                else
                    temp = mat4_identity;
            else
                temp = matrices[bone_id];

            mat4_mult(&temp, global_mat, &temp);

            mat4 inv_bind_pose_mat = s->bones[i].inv_bind_pose_mat;
            mat4_mult(&inv_bind_pose_mat, &temp, &matrix_buffer[i]);
        }
}

void material_change_storage_load(const char* material_name) {
    uint32_t name_hash = hash_utf8_murmurhash(material_name);
    auto elem = material_change_storage_data.find(name_hash);
    if (elem != material_change_storage_data.end()) {
        elem->second.load_count++;
        return;
    }

    elem = material_change_storage_data.insert({ name_hash, { } }).first;

    material_change_handler* handler = &elem->second;
    handler->data.blend_color = vec4u_identity;
    handler->data.glow_intensity = 1.0f;
    handler->data.incandescence = vec4u_identity;
}

material_change* material_change_storage_get(const char* material_name) {
    uint32_t name_hash = hash_utf8_murmurhash(material_name);
    auto elem = material_change_storage_data.find(name_hash);
    if (elem != material_change_storage_data.end())
        return &elem->second.data;
    return 0;
}

void material_change_storage_unload(const char* material_name) {
    uint32_t name_hash = hash_utf8_murmurhash(material_name);
    auto elem = material_change_storage_data.find(name_hash);
    if (elem == material_change_storage_data.end())
        return;

    material_change_handler* handler = &elem->second;
    if (--handler->load_count > 0)
        return;

    material_change_storage_data.erase(elem);
}

inline void object_storage_init(object_database* obj_db) {
    material_change_storage_data.clear();
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
    for (obj_set_handler& i : object_storage_data)
        if (i.set_id == obj_info.set_id) {
            obj_set* set = i.obj_set;
            if (!set)
                return 0;

            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id)
                    return &set->objects[j];
            return 0;
        }

    for (obj_set_handler& i : object_storage_data_modern)
        if (i.set_id == obj_info.set_id) {
            obj_set* set = i.obj_set;
            if (!set)
                return 0;

            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id)
                    return &set->objects[j];
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

inline obj_material* object_storage_get_material(const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->objects_count; j++) {
            obj* obj = &set->objects[j];
            for (int32_t k = 0; k < obj->materials_count; k++)
                if (hash_utf8_murmurhash(obj->materials[k].material.name) == name_hash)
                    return &obj->materials[k].material;
        }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->objects_count; j++) {
            obj* obj = &set->objects[j];
            for (int32_t k = 0; k < obj->materials_count; k++)
                if (hash_utf8_murmurhash(obj->materials[k].material.name) == name_hash)
                    return &obj->materials[k].material;
        }
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

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return &obj->meshes[k];
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

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return &obj->meshes[k];
                return 0;
            }
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_index(object_info obj_info, int32_t index) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                obj* obj = &set->objects[j];
                if (index > -1 && index < obj->meshes_count)
                    return &obj->meshes[index];
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

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                obj* obj = &set->objects[j];
                if (index > -1 && index < obj->meshes_count)
                    return &obj->meshes[index];
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
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return &obj->meshes[k];
                return 0;
            }
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return &obj->meshes[k];
                return 0;
            }
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_object_hash_index(uint32_t hash, int32_t index) {
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                obj* obj = &set->objects[j];
                if (index > -1 && index < obj->meshes_count)
                    return &obj->meshes[index];
                return 0;
            }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                obj* obj = &set->objects[j];
                if (index > -1 && index < obj->meshes_count)
                    return &obj->meshes[index];
                return 0;
            }
    }
    return 0;
}

inline int32_t object_storage_get_obj_mesh_index(object_info obj_info, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return k;
                return -1;
            }
        return -1;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return k;
                return -1;
            }
        return -1;
    }
    return -1;
}

inline int32_t object_storage_get_obj_mesh_index_by_hash(uint32_t hash, const char* mesh_name) {
    for (obj_set_handler& i : object_storage_data) {
        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
                        return k;
                return -1;
            }
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        obj_set* set = i.obj_set;
        if (!set)
            return -1;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                uint32_t mesh_name_hash = hash_utf8_murmurhash(mesh_name);
                obj* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (hash_utf8_murmurhash(obj->meshes[k].name) == mesh_name_hash)
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

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id)
                return set->objects[j].skin_init ? &set->objects[j].skin : 0;
        return 0;
    }

    for (obj_set_handler& i : object_storage_data_modern) {
        if (i.set_id != obj_info.set_id)
            continue;

        obj_set* set = i.obj_set;
        if (!set)
            return 0;

        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].id == obj_info.id)
                return set->objects[j].skin_init ? &set->objects[j].skin : 0;
        return 0;
    }
    return 0;
}

inline obj_index_buffer* object_storage_get_obj_index_buffers(uint32_t set_id) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (handler && handler->index_buffers)
        return handler->index_buffers;
    return 0;
}

inline obj_mesh_index_buffer* object_storage_get_obj_mesh_index_buffer(object_info obj_info) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(obj_info.set_id);
    if (handler && handler->obj_set && handler->index_buffers) {
        obj_set* set = handler->obj_set;
        for (int32_t i = 0; i < set->objects_count; i++)
            if (set->objects[i].id == obj_info.id)
                return handler->index_buffers[i].meshes;
    }
    return 0;
}

inline object_vertex_buffer* object_storage_get_obj_vertex_buffers(uint32_t set_id) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
    if (handler && handler->vertex_buffers)
        return handler->vertex_buffers;
    return 0;
}

inline obj_mesh_vertex_buffer* object_storage_get_obj_mesh_vertex_buffer(object_info obj_info) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(obj_info.set_id);
    if (handler && handler->obj_set && handler->vertex_buffers) {
        obj_set* set = handler->obj_set;
        for (int32_t i = 0; i < set->objects_count; i++)
            if (set->objects[i].id == obj_info.id)
                return handler->vertex_buffers[i].meshes;
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

    std::pair<uint32_t, uint32_t>* texture = handler->texture_ids.data();
    size_t length = handler->texture_ids.size();
    size_t temp;
    while (length > 0)
        if (tex_id < texture[temp = length / 2].first)
            length /= 2;
        else {
            texture += temp + 1;
            length -= temp + 1;
        }

    if (texture != handler->texture_ids.data() + handler->texture_ids.size())
        return (*textures)[texture->second];
    return -1;
}

inline std::vector<GLuint>* object_storage_get_obj_set_textures(int32_t set) {
    obj_set_handler* handler = object_storage_get_obj_set_handler(set);
    if (handler)
        return &handler->textures;
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
            handler->object_ids.reserve(set->objects_count);
            for (int32_t i = 0; i < set->objects_count; i++)
                handler->object_ids.push_back({ set->objects[i].id, i });

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
        handler->object_ids.reserve(set->objects_count);
        for (int32_t i = 0; i < set->objects_count; i++)
            handler->object_ids.push_back({ set->objects[i].id, i });

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

    handler->object_ids.clear();
    handler->texture_ids.clear();
    handler->textures.clear();

    texture** texture_data = handler->texture_data;
    int32_t textures_count = handler->textures_count;
    for (int32_t i = 0; i < textures_count; i++)
        texture_free(texture_data[i]);
    free(texture_data);
    handler->texture_data = 0;
    handler->textures_count = 0;

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
    material_change_storage_data.clear();
    object_storage_data.clear();
    object_storage_data.shrink_to_fit();
    object_storage_data_modern.clear();
}

static bool obj_mesh_index_buffer_load(obj_mesh_index_buffer* buffer, obj_mesh* mesh) {
    int32_t indices_count = 0;
    for (int32_t i = 0; i < mesh->sub_meshes_count; i++)
        indices_count += mesh->sub_meshes[i].indices_count;

    if (!indices_count) {
        *buffer = 0;
        return true;
    }

    uint16_t* indices = force_malloc_s(uint16_t, indices_count);
    uint16_t* _indices = indices;
    for (int32_t k = 0; k < mesh->sub_meshes_count; k++) {
        obj_sub_mesh* sub_mesh = &mesh->sub_meshes[k];
        int32_t indices_count = sub_mesh->indices_count;
        uint32_t* sub_mesh_indices = sub_mesh->indices;
        for (int32_t l = 0; l < indices_count; l++)
            *_indices++ = (uint16_t)sub_mesh_indices[l];
    }

    _indices = indices;
    for (int32_t i = 0, offset = 0; i < mesh->sub_meshes_count; i++) {
        obj_sub_mesh* sub_mesh = &mesh->sub_meshes[i];

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

    *buffer = obj_mesh_index_buffer_load_data((size_t)indices_count * sizeof(uint16_t), indices);
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
    glDeleteBuffers(1, buffer);
}

static bool obj_mesh_vertex_buffer_load(obj_mesh_vertex_buffer* buffer, obj_mesh* mesh) {
    if (!mesh->vertex_count || !mesh->vertex)
        return false;

    size_t vertex_size;
    if (~mesh->flags & OBJ_MESH_COMPRESSED)
        vertex_size = obj_vertex_flags_get_vertex_size(mesh->vertex_flags);
    else
        vertex_size = obj_vertex_flags_get_vertex_size_comp(mesh->vertex_flags);

    void* vertex = force_malloc(vertex_size * mesh->vertex_count);
    if (vertex) {
        obj_vertex_flags flags = mesh->vertex_flags;
        obj_vertex_data* vertex_file = mesh->vertex;
        int32_t vertex_count = mesh->vertex_count;
        size_t d = (size_t)vertex;
        if (~mesh->flags & OBJ_MESH_COMPRESSED) {
            for (int32_t i = 0; i < vertex_count; i++) {
                if (flags & OBJ_VERTEX_POSITION) {
                    *(vec3*)d = vertex_file[i].position;
                    d += 12;
                }

                if (flags & OBJ_VERTEX_NORMAL) {
                    *(vec3*)d = vertex_file[i].normal;
                    d += 12;
                }

                if (flags & OBJ_VERTEX_TANGENT) {
                    *(vec4u*)d = vertex_file[i].tangent;
                    d += 16;
                }

                if (flags & OBJ_VERTEX_BINORMAL) {
                    *(vec3*)d = vertex_file[i].binormal;
                    d += 12;
                }

                if (flags & OBJ_VERTEX_TEXCOORD0) {
                    *(vec2*)d = vertex_file[i].texcoord0;
                    d += 8;
                }

                if (flags & OBJ_VERTEX_TEXCOORD1) {
                    *(vec2*)d = vertex_file[i].texcoord1;
                    d += 8;
                }

                if (flags & OBJ_VERTEX_TEXCOORD2) {
                    *(vec2*)d = vertex_file[i].texcoord2;
                    d += 8;
                }

                if (flags & OBJ_VERTEX_TEXCOORD3) {
                    *(vec2*)d = vertex_file[i].texcoord3;
                    d += 8;
                }

                if (flags & OBJ_VERTEX_COLOR0) {
                    *(vec4u*)d = vertex_file[i].color0;
                    d += 16;
                }

                if (flags & OBJ_VERTEX_COLOR1) {
                    *(vec4u*)d = vertex_file[i].color1;
                    d += 16;
                }

                if (flags & OBJ_VERTEX_BONE_DATA) {
                    *(vec4u*)d = vertex_file[i].bone_weight;
                    d += 16;
                    *(vec4iu*)d = vertex_file[i].bone_index;
                    d += 16;
                }

                if (flags & OBJ_VERTEX_UNKNOWN) {
                    *(vec4u*)d = vertex_file[i].unknown;
                    d += 16;
                }
            }
        }
        else {
            for (int32_t i = 0; i < vertex_count; i++) {
                if (flags & OBJ_VERTEX_POSITION) {
                    *(vec3*)d = vertex_file[i].position;
                    d += 12;
                }

                if (flags & OBJ_VERTEX_NORMAL) {
                    vec3 normal;
                    vec3_mult_scalar(vertex_file[i].normal, 32727.0f, normal);
                    vec3_to_vec3i16(normal, *(vec3i16*)d);
                    *(int16_t*)(d + 6) = 0;
                    d += 8;
                }

                if (flags & OBJ_VERTEX_TANGENT) {
                    vec4 tangent = vertex_file[i].tangent;
                    vec4_mult_scalar(tangent, 32727.0f, tangent);
                    vec4_to_vec4i16(tangent, *(vec4i16*)d);
                    d += 8;
                }

                if (flags & OBJ_VERTEX_TEXCOORD0) {
                    vec2_to_vec2h(vertex_file[i].texcoord0, *(vec2h*)d);
                    d += 4;
                }

                if (flags & OBJ_VERTEX_TEXCOORD1) {
                    vec2_to_vec2h(vertex_file[i].texcoord1, *(vec2h*)d);
                    d += 4;
                }

                if (flags & OBJ_VERTEX_TEXCOORD2) {
                    vec2_to_vec2h(vertex_file[i].texcoord2, *(vec2h*)d);
                    d += 4;
                }

                if (flags & OBJ_VERTEX_TEXCOORD3) {
                    vec2_to_vec2h(vertex_file[i].texcoord3, *(vec2h*)d);
                    d += 4;
                }

                if (flags & OBJ_VERTEX_COLOR0) {
                    vec4 color0 = vertex_file[i].color0;
                    vec4_to_vec4h(color0, *(vec4h*)d);
                    d += 8;
                }

                if (flags & OBJ_VERTEX_BONE_DATA) {
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
    mesh->vertex_size = (int32_t)vertex_size;

    bool ret = obj_mesh_vertex_buffer_load_data(buffer, vertex_size * mesh->vertex_count,
        vertex, mesh->flags & OBJ_MESH_FLAG_1 ? 2 : 1);
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
    for (int32_t i = 0; i < set->objects_count; i++) {
        obj* obj = &set->objects[i];
        for (int32_t j = 0; j < obj->meshes_count; j++) {
            obj_mesh* mesh = &obj->meshes[j];
            for (int32_t k = 0; k < mesh->sub_meshes_count; k++) {
                vec3 _min = { 9999999.0f, 9999999.0f, 9999999.0f };
                vec3 _max = { -100000000.0f, -100000000.0f, -100000000.0f };

                obj_sub_mesh* sub_mesh = &mesh->sub_meshes[k];
                uint32_t* indices = (uint32_t*)sub_mesh->indices;
                int32_t indices_count = sub_mesh->indices_count;
                obj_vertex_data* vertex = mesh->vertex;
                if (sub_mesh->index_format == OBJ_INDEX_U16)
                    for (int32_t l = 0; l < indices_count; l++) {
                        if (indices[l] == 0xFFFFFFFF)
                            continue;

                        vec3 pos = vertex[indices[l]].position;
                        vec3_min(_min, pos, _min);
                        vec3_max(_max, pos, _max);
                    }
                else
                    for (int32_t l = 0; l < indices_count; l++) {
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
    for (int32_t i = 0; i < set->objects_count; i++) {
        obj* obj = &set->objects[i];
        int32_t materials_count = obj->materials_count;
        for (int32_t j = 0; j < materials_count; j++) {
            obj_material_data* material_data = &obj->materials[j];
            obj_material* material = &material_data->material;
            int32_t textures_count = material_data->textures_count;

            if (*(int32_t*)&material->shader.name[4] != 0xDEADFF) {
                material->shader.index = shader_get_index_by_name(&shaders_ft, material->shader.name);
                *(int32_t*)&material->shader.name[4] = 0xDEADFF;
            }

            for (int32_t k = 0; k < 8; k++) {
                obj_material_texture* texture = &material->textures[k];
                uint32_t texture_id = texture->texture_id;
                texture->texture_id = 0;
                texture->texture_index = 0;

                std::pair<uint32_t, uint32_t>* texture_ids = handler->texture_ids.data();
                int32_t texture_ids_count = (int32_t)handler->texture_ids.size();
                for (int32_t l = texture_ids_count; l; l--, texture_ids++)
                    if (texture_ids->first == texture_id) {
                        texture->texture_id = texture_id;
                        texture->texture_index = texture_ids->second;
                        break;
                    }
            }
        }
    }
}

static bool obj_set_handler_index_buffer_load(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    handler->index_buffers_count = set->objects_count;
    handler->index_buffers = force_malloc_s(obj_index_buffer, set->objects_count);
    if (!handler->index_buffers)
        return true;

    for (int32_t i = 0; i < set->objects_count; i++) {
        obj* obj = &set->objects[i];
        obj_index_buffer* buffer = &handler->index_buffers[i];

        buffer->meshes_count = obj->meshes_count;
        buffer->meshes = force_malloc_s(obj_mesh_index_buffer, obj->meshes_count);
        if (!buffer->meshes)
            return false;

        for (int32_t j = 0; j < buffer->meshes_count; j++)
            if (!obj_mesh_index_buffer_load(&buffer->meshes[j], &obj->meshes[j]))
                return false;
    }
    return true;
}

static void obj_set_handler_index_buffer_free(obj_set_handler* handler) {
    if (!handler->index_buffers)
        return;

    for (int32_t i = 0; i < handler->index_buffers_count; i++) {
        obj_index_buffer* buffer = &handler->index_buffers[i];
        if (buffer->meshes)
            for (int32_t j = 0; j < buffer->meshes_count; j++)
                obj_mesh_index_buffer_free(&buffer->meshes[j]);
        free(buffer->meshes);
    }
    free(handler->index_buffers);
    handler->index_buffers = 0;
    handler->index_buffers_count = 0;
}

static int32_t obj_set_texture_ids_sort(void const* src1, void const* src2) {
    std::pair<uint32_t, uint32_t>* p1 = (std::pair<uint32_t, uint32_t>*)src1;
    std::pair<uint32_t, uint32_t>* p2 = (std::pair<uint32_t, uint32_t>*)src2;
    return p1->first - p2->first;
}

static bool obj_set_handler_load_textures(obj_set_handler* handler, void* data, bool big_endian) {
    obj_set* set = handler->obj_set;
    if (!set || !data)
        return true;
    else if (!set->texture_ids_count)
        return false;

    {
        txp_set txp;
        txp.unpack_file(data, big_endian);
        handler->textures_count = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &handler->texture_data, set->texture_ids);
    }

    uint32_t* texture_ids = set->texture_ids;
    int32_t textures_count = handler->textures_count;
    texture** texture_data = handler->texture_data;
    for (int32_t i = 0; i < textures_count; i++) {
        handler->texture_ids.push_back({ texture_ids[i], i });
        handler->textures.push_back(texture_data[i]->tex);
    }
    return false;
}

static bool obj_set_handler_load_textures_modern(obj_set_handler* handler, void* data, size_t size) {
    obj_set* set = handler->obj_set;
    if (!set || !data || !size)
        return true;
    else if (!set->texture_ids_count)
        return false;

    {
        txp_set txp;
        txp.unpack_file_modern(data, size);
        handler->textures_count = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &handler->texture_data, set->texture_ids);
    }

    uint32_t* texture_ids = set->texture_ids;
    int32_t textures_count = handler->textures_count;
    texture** texture_data = handler->texture_data;
    for (int32_t i = 0; i < textures_count; i++) {
        handler->texture_ids.push_back({ texture_ids[i], i });
        handler->textures.push_back(texture_data[i]->tex);
    }
    return false;
}

static bool obj_set_handler_vertex_buffer_load(obj_set_handler* handler) {
    obj_set* set = handler->obj_set;
    handler->vertex_buffers_count = set->objects_count;
    handler->vertex_buffers = force_malloc_s(object_vertex_buffer, set->objects_count);
    if (!handler->vertex_buffers)
        return true;

    for (int32_t i = 0; i < set->objects_count; i++) {
        obj* obj = &set->objects[i];
        object_vertex_buffer* buffer = &handler->vertex_buffers[i];

        buffer->meshes_count = obj->meshes_count;
        buffer->meshes = force_malloc_s(obj_mesh_vertex_buffer, obj->meshes_count);
        if (!buffer->meshes)
            return false;

        for (int32_t j = 0; j < buffer->meshes_count; j++)
            if (!obj_mesh_vertex_buffer_load(&buffer->meshes[j], &obj->meshes[j]))
                return false;
    }
    return true;
}

static void obj_set_handler_vertex_buffer_free(obj_set_handler* handler) {
    if (!handler->vertex_buffers)
        return;

    for (int32_t i = 0; i < handler->vertex_buffers_count; i++) {
        object_vertex_buffer* buffer = &handler->vertex_buffers[i];
        if (buffer->meshes)
            for (int32_t j = 0; j < buffer->meshes_count; j++)
                obj_mesh_vertex_buffer_free(&buffer->meshes[j]);
        free(buffer->meshes);
    }
    free(handler->vertex_buffers);
    handler->vertex_buffers = 0;
    handler->vertex_buffers_count = 0;
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

inline static size_t obj_vertex_flags_get_vertex_size(obj_vertex_flags flags) {
    size_t size = 0;
    if (flags & OBJ_VERTEX_POSITION)
        size += 12;
    if (flags & OBJ_VERTEX_NORMAL)
        size += 12;
    if (flags & OBJ_VERTEX_TANGENT)
        size += 16;
    if (flags & OBJ_VERTEX_BINORMAL)
        size += 12;
    if (flags & OBJ_VERTEX_TEXCOORD0)
        size += 8;
    if (flags & OBJ_VERTEX_TEXCOORD1)
        size += 8;
    if (flags & OBJ_VERTEX_TEXCOORD2)
        size += 8;
    if (flags & OBJ_VERTEX_TEXCOORD3)
        size += 8;
    if (flags & OBJ_VERTEX_COLOR0)
        size += 16;
    if (flags & OBJ_VERTEX_COLOR1)
        size += 16;
    if (flags & OBJ_VERTEX_BONE_DATA)
        size += 32;
    if (flags & OBJ_VERTEX_UNKNOWN)
        size += 16;
    return size;
}

inline static size_t obj_vertex_flags_get_vertex_size_comp(obj_vertex_flags flags) {
    size_t size = 0;
    if (flags & OBJ_VERTEX_POSITION)
        size += 12;
    if (flags & OBJ_VERTEX_NORMAL)
        size += 8;
    if (flags & OBJ_VERTEX_TANGENT)
        size += 8;
    if (flags & OBJ_VERTEX_TEXCOORD0)
        size += 4;
    if (flags & OBJ_VERTEX_TEXCOORD1)
        size += 4;
    if (flags & OBJ_VERTEX_TEXCOORD2)
        size += 4;
    if (flags & OBJ_VERTEX_TEXCOORD3)
        size += 4;
    if (flags & OBJ_VERTEX_COLOR0)
        size += 8;
    if (flags & OBJ_VERTEX_BONE_DATA)
        size += 16;
    return size;
}

obj_set_handler::obj_set_handler(): obj_loaded(), tex_loaded(), obj_set(),
textures_count(), texture_data(), set_id(), vertex_buffers_count(), vertex_buffers(),
index_buffers_count(), index_buffers(), load_count(), modern() {

}

obj_set_handler::~obj_set_handler() {
    for (int32_t i = 0; i < textures_count; i++)
        texture_free(texture_data[i]);
    free(texture_data);

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
