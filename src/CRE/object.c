/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.h"
#include "data.h"
#include "gl_state.h"
#include "shader_ft.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"

typedef obj object_file;
typedef obj_material_data object_material_data_file;
typedef obj_material object_material_file;
typedef obj_material_texture object_material_texture_file;
typedef obj_mesh object_mesh_file;
typedef obj_set object_set_file;
typedef obj_skin object_skin_file;
typedef obj_skin_block object_skin_block_file;
typedef obj_skin_block_cloth object_skin_block_cloth_file;
typedef obj_skin_block_constraint object_skin_block_constraint_file;
typedef obj_skin_block_constraint_attach_point object_skin_block_constraint_attach_point_file;
typedef obj_skin_block_constraint_direction object_skin_block_constraint_direction_file;
typedef obj_skin_block_constraint_distance object_skin_block_constraint_distance_file;
typedef obj_skin_block_constraint_orientation object_skin_block_constraint_orientation_file;
typedef obj_skin_block_constraint_position object_skin_block_constraint_position_file;
typedef obj_skin_block_constraint_up_vector_old object_skin_block_constraint_up_vector_old_file;
typedef obj_skin_block_expression object_skin_block_expression_file;
typedef obj_skin_block_motion object_skin_block_motion_file;
typedef obj_skin_block_node object_skin_block_node_file;
typedef obj_skin_block_osage object_skin_block_osage_file;
typedef obj_skin_bone object_skin_bone_file;
typedef obj_skin_ex_data object_skin_ex_data_file;
typedef obj_skin_osage_node object_skin_osage_node_file;
typedef obj_skin_osage_sibling_info  object_skin_osage_sibling_info_file;
typedef obj_skin_motion_node object_skin_motion_node_file;
typedef obj_sub_mesh object_sub_mesh_file;
typedef obj_vertex_data object_vertex_data_file;

typedef struct object_storage {
    uint32_t set_id;
    int32_t count;
    object_set set;
} object_storage;

vector_old(object_storage)

static bool object_mesh_index_buffer_load(object_mesh_index_buffer* buffer, object_mesh* mesh);
static GLuint object_mesh_index_buffer_load_data(size_t size, void* data);
static void object_mesh_index_buffer_free(object_mesh_index_buffer* buffer);
static bool object_mesh_vertex_buffer_load(object_mesh_vertex_buffer* buffer, object_mesh* mesh);
static bool object_mesh_vertex_buffer_load_data(object_mesh_vertex_buffer* buffer,
    size_t size, void* data, int32_t count);
static void object_mesh_vertex_buffer_free(object_mesh_vertex_buffer* buffer);
static bool object_set_index_buffer_load(object_set* set);
static void object_set_index_buffer_free(object_set* set);
static bool object_set_vertex_buffer_load(object_set* set);
static void object_set_vertex_buffer_free(object_set* set);
static void object_skin_block_constraint_attach_point_load(
    object_skin_block_constraint_attach_point* attach_point,
    object_skin_block_constraint_attach_point_file* attach_point_file);
static void object_skin_block_constraint_up_vector_old_load(
    object_skin_block_constraint_up_vector_old* up_vector_old,
    object_skin_block_constraint_up_vector_old_file* up_vector_old_file);
static void object_skin_block_node_load(object_skin_block_node* node,
    object_skin_block_node_file* node_file);
static void object_skin_block_node_free(object_skin_block_node* node);
static size_t obj_vertex_flags_get_vertex_size(obj_vertex_flags flags);
static size_t obj_vertex_flags_get_vertex_size_comp(obj_vertex_flags flags);
static size_t object_vertex_flags_get_vertex_size(object_vertex_flags flags);
static size_t object_vertex_flags_get_vertex_size_comp(object_vertex_flags flags);
static bool object_set_load_file(void* data, const char* path, const char* file, uint32_t hash);
static bool object_set_load_file_modern(void* data, const char* path, const char* file, uint32_t hash);

vector_old_func(object_storage)

vector_old_object_storage object_storage_data;

void object_set_init(object_set* set) {
    if (!set)
        return;

    memset(set, 0, sizeof(object_set));
}

void object_set_load(object_set* set, obj_set* obj_set_file, txp_set* txp_set_file,
    texture_database* tex_db, const char* name, uint32_t id, bool compressed) {
    if (!set || !obj_set_file || !txp_set_file || !tex_db || !name)
        return;

    bool is_x = obj_set_file->is_x;
    set->is_x = is_x;
    set->texture_ids_count = obj_set_file->texture_ids_count;
    set->texture_ids = force_malloc_s(uint32_t, obj_set_file->texture_ids_count);
    memcpy(set->texture_ids, obj_set_file->texture_ids, obj_set_file->texture_ids_count * sizeof(uint32_t));

    set->texture_names_count = obj_set_file->texture_ids_count;
    set->texture_names = force_malloc_s(string, obj_set_file->texture_ids_count);
    if (tex_db->ready)
        for (int32_t i = 0; i < set->texture_names_count; i++) {
            int32_t texture_id = set->texture_ids[i];
            string* name = &set->texture_names[i];
            *name = string_empty;

            for (texture_info& j : tex_db->texture)
                if (j.id == texture_id) {
                    string_init_length(name, j.name.c_str(), j.name.size());
                    break;
                }

            if (!name->length) {
                char buf[0x100];
                int32_t len = sprintf_s(buf, sizeof(buf), "TEXTURE_%d", i);
                string_init_length(name, buf, len);
            }
        }
    else
        for (int32_t i = 0; i < set->texture_names_count; i++) {
            string* name = &set->texture_names[i];

            char buf[0x100];
            int32_t len = sprintf_s(buf, sizeof(buf), "TEXTURE_%d", i);
            string_init_length(name, buf, len);
        }

    string_init(&set->name, name);
    set->id = id;
    set->hash = hash_string_murmurhash(&set->name, 0, false);

    int32_t objects_count = obj_set_file->objects_count;
    set->objects_count = objects_count;
    set->objects = force_malloc_s(object, objects_count);
    for (int32_t i = 0; i < objects_count; i++) {
        object* obj = &set->objects[i];
        object_file* obj_file = &obj_set_file->objects[i];
        obj->bounding_sphere.center = obj_file->bounding_sphere.center;
        obj->bounding_sphere.radius = obj_file->bounding_sphere.radius;

        int32_t meshes_count = obj_file->meshes_count;
        obj->meshes_count = meshes_count;
        obj->meshes = force_malloc_s(object_mesh, meshes_count);
        for (int32_t j = 0; j < meshes_count; j++) {
            object_mesh* mesh = &obj->meshes[j];
            object_mesh_file* mesh_file = &obj_file->meshes[j];

            mesh->bounding_sphere.center = mesh_file->bounding_sphere.center;
            mesh->bounding_sphere.radius = mesh_file->bounding_sphere.radius;

            int32_t sub_meshes_count = mesh_file->sub_meshes_count;
            mesh->sub_meshes_count = sub_meshes_count;
            mesh->sub_meshes = force_malloc_s(object_sub_mesh, sub_meshes_count);
            for (int32_t k = 0; k < sub_meshes_count; k++) {
                object_sub_mesh* sub_mesh = &mesh->sub_meshes[k];
                object_sub_mesh_file* sub_mesh_file = &mesh_file->sub_meshes[k];

                sub_mesh->bounding_sphere.center = sub_mesh_file->bounding_sphere.center;
                sub_mesh->bounding_sphere.radius = sub_mesh_file->bounding_sphere.radius;
                sub_mesh->material_index = sub_mesh_file->material_index;
                memcpy(sub_mesh->texcoord_indices, sub_mesh_file->texcoord_indices,
                    sizeof(sub_mesh_file->texcoord_indices));

                sub_mesh->bone_indices_count = sub_mesh_file->bone_indices_count;
                sub_mesh->bone_indices = force_malloc_s(uint16_t, sub_mesh_file->bone_indices_count);
                memcpy(sub_mesh->bone_indices, sub_mesh_file->bone_indices,
                    sub_mesh_file->bone_indices_count * sizeof(uint16_t));
                sub_mesh->bones_per_vertex = sub_mesh_file->bones_per_vertex;
                sub_mesh->primitive_type = (object_primitive_type)sub_mesh_file->primitive_type;
                sub_mesh->index_format = (object_index_format)sub_mesh_file->index_format;
                sub_mesh->flags = (object_sub_mesh_flags)sub_mesh_file->flags;

                bool u16 = sub_mesh_file->index_format == OBJ_INDEX_U16;
                vec3 _min = { 9999999.0f, 9999999.0f, 9999999.0f };
                vec3 _max = { -100000000.0f, -100000000.0f, -100000000.0f };

                uint32_t* indices = (uint32_t*)sub_mesh_file->indices;
                int32_t indices_count = sub_mesh_file->indices_count;
                object_vertex_data_file* vertex = mesh_file->vertex;
                if (u16)
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

                sub_mesh->indices_count = indices_count;
                sub_mesh->first_index = 0;
                sub_mesh->last_index = 0;
                sub_mesh->indices_offset = 0;
                if (u16) {
                    uint16_t first_index = 0xFFFF;
                    uint16_t last_index = 0;
                    for (int32_t i = 0; i < indices_count; i++) {
                        uint16_t index = (uint16_t)indices[i];
                        if (index == 0xFFFF)
                            continue;

                        if (index < first_index)
                            first_index = index;
                        if (index > last_index)
                            last_index = index;
                    }

                    sub_mesh->first_index = first_index;
                    sub_mesh->last_index = last_index;
                }
            }

            mesh->compressed = compressed;

            size_t vertex_size;
            if (!compressed)
                vertex_size = obj_vertex_flags_get_vertex_size(mesh_file->vertex_flags);
            else
                vertex_size = obj_vertex_flags_get_vertex_size_comp(mesh_file->vertex_flags);

            void* vertex = force_malloc(vertex_size * mesh_file->vertex_count);
            if (vertex) {
                obj_vertex_flags flags = mesh_file->vertex_flags;
                object_vertex_data_file* vertex_file = mesh_file->vertex;
                int32_t vertex_count = mesh_file->vertex_count;
                size_t d = (size_t)vertex;
                if (!compressed) {
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

                mesh->vertex = vertex;
                mesh->vertex_count = mesh_file->vertex_count;
                mesh->vertex_size = (int32_t)vertex_size;
            }
            else {
                mesh->vertex = 0;
                mesh->vertex_count = 0;
                mesh->vertex_size = 0;
            }

            size_t indices_count = 0;
            for (int32_t k = 0; k < mesh->sub_meshes_count; k++)
                if (mesh_file->sub_meshes[k].index_format == OBJ_INDEX_U16)
                    indices_count += mesh_file->sub_meshes[k].indices_count;

            if (indices_count) {
                uint16_t* indices = force_malloc_s(uint16_t, indices_count);
                size_t offset = 0;
                for (int32_t k = 0; k < mesh->sub_meshes_count; k++) {
                    object_sub_mesh* sub_mesh = &mesh->sub_meshes[k];
                    object_sub_mesh_file* sub_mesh_file = &mesh_file->sub_meshes[k];
                    if (sub_mesh->index_format != OBJ_INDEX_U16)
                        continue;

                    int32_t sub_mesh_indices_count = sub_mesh_file->indices_count;
                    uint32_t* sub_mesh_indices = sub_mesh_file->indices;
                    for (int32_t l = 0; l < sub_mesh_indices_count; l++)
                        indices[offset + l] = (uint16_t)sub_mesh_indices[l];
                    sub_mesh->indices_offset = (int32_t)(sizeof(uint16_t) * offset);
                    offset += sub_mesh_file->indices_count;
                }
                mesh->indices = indices;
                mesh->indices_count = (int32_t)indices_count;
            }
            else {
                mesh->indices = 0;
                mesh->indices_count = 0;
            }

            mesh->vertex_flags = (object_vertex_flags)mesh_file->vertex_flags;
            mesh->flags = (object_mesh_flags)mesh_file->flags;
            memcpy(mesh->name, mesh_file->name, sizeof(mesh_file->name));
            mesh->name[sizeof(mesh->name) - 1] = 0;
        }

        int32_t materials_count = obj_file->materials_count;
        obj->materials_count = materials_count;
        obj->materials = force_malloc_s(object_material_data, materials_count);
        for (int32_t j = 0; j < materials_count; j++) {
            obj->materials[j].textures_count = obj_file->materials[j].textures_count;

            object_material* material = &obj->materials[j].material;
            object_material_file* material_file = &obj_file->materials[j].material;

            material->flags = (object_material_flags)material_file->flags;
            material->shader_index = shader_get_index_by_name(&shaders_ft, material_file->shader_name);
            memcpy(&material->shader_flags, &material_file->shader_flags,
                sizeof(material_file->shader_flags));

            for (int32_t k = 0; k < 8; k++) {
                object_material_texture* textures = &material->textures[k];
                object_material_texture_file* textures_file = &material_file->textures[k];

                memcpy(&textures->sampler_flags, &textures_file->sampler_flags,
                    sizeof(textures_file->sampler_flags));
                textures->texture_id = -1;
                textures->texture_index = -1;
                memcpy(&textures->texture_flags, &textures_file->texture_flags,
                    sizeof(textures_file->texture_flags));
                memcpy(&textures->shader_name, &textures_file->shader_name,
                    sizeof(textures_file->shader_name));
                textures->weight = textures_file->weight;
                textures->tex_coord_mat = textures_file->tex_coord_mat;

                uint32_t* texture_ids = set->texture_ids;
                int32_t texture_ids_count = set->texture_ids_count;
                for (int32_t l = 0; l < texture_ids_count; l++) {
                    if (texture_ids[l] == textures_file->texture_id) {
                        textures->texture_id = textures_file->texture_id;
                        textures->texture_index = l;
                        break;
                    }
                }
            }

            material->diffuse = material_file->diffuse;
            material->ambient = material_file->ambient;
            material->specular = material_file->specular;
            material->emission = material_file->emission;
            memcpy(&material->blend_flags, &material_file->blend_flags,
                sizeof(material_file->blend_flags));
            material->shininess = material_file->shininess;
            material->intensity = material_file->intensity;
            material->reserved_sphere.center = material_file->reserved_sphere.center;
            material->reserved_sphere.radius = material_file->reserved_sphere.radius;
            memcpy(material->name, material_file->name, sizeof(material_file->name) - 1);
            material->name[sizeof(material->name) - 1] = 0;
            material->bump_depth = material_file->bump_depth;
        }

        obj->flags = obj_file->flags;
        string_copy(&obj_file->name, &obj->name);
        obj->id = obj_file->id;
        obj->hash = hash_string_murmurhash(&obj->name, 0, false);

        if (!obj_file->skin_init) {
            memset(&obj->skin, 0, sizeof(object_skin));
            obj->skin_init = false;
            continue;
        }

        object_skin* skin = &obj->skin;
        object_skin_file* skin_file = &obj_file->skin;

        int32_t bones_count = skin_file->bones_count;
        skin->bones_count = bones_count;
        skin->bones = force_malloc_s(object_skin_bone, bones_count);
        for (int32_t j = 0; j < bones_count; j++) {
            object_skin_bone* bone = &skin->bones[j];
            object_skin_bone_file* bone_file = &skin_file->bones[j];

            bone->id = bone_file->id;
            bone->inv_bind_pose_mat = bone_file->inv_bind_pose_mat;
            string_copy(&bone_file->name, &bone->name);
            bone->parent = bone_file->parent;
        }
        obj->skin_init = true;

        if (!skin_file->ex_data_init) {
            memset(&skin->ex_data, 0, sizeof(object_skin_ex_data));
            skin->ex_data_init = false;
            continue;
        }

        object_skin_ex_data* ex_data = &skin->ex_data;
        object_skin_ex_data_file* ex_data_file = &skin_file->ex_data;

        int32_t osage_nodes_count = ex_data_file->osage_nodes_count;
        ex_data->osage_nodes_count = osage_nodes_count;
        ex_data->osage_nodes = force_malloc_s(object_skin_osage_node, osage_nodes_count);
        for (int32_t j = 0; j < osage_nodes_count; j++) {
            object_skin_osage_node* node = &ex_data->osage_nodes[j];
            object_skin_osage_node_file* node_file = &ex_data_file->osage_nodes[j];

            node->name_index = node_file->name_index;
            node->length = node_file->length;
            node->rotation = node_file->rotation;
        }

        int32_t bone_names_count = ex_data_file->bone_names_count;

        size_t buf_size = 0;
        char** bone_names_file_ptr = ex_data_file->bone_names;
        for (int32_t i = 0; i < bone_names_count; i++)
            buf_size += utf8_length(*bone_names_file_ptr++) + 1;

        ex_data->bone_names_buf = force_malloc_s(char, buf_size);
        ex_data->bone_names = force_malloc_s(char*, buf_size + 1);
        ex_data->bone_names_count = bone_names_count;

        bone_names_file_ptr = ex_data_file->bone_names;
        char* bone_names_buf = ex_data->bone_names_buf;
        char** bone_names_ptr = ex_data->bone_names;
        for (int32_t i = 0; i < bone_names_count; i++) {
            *bone_names_ptr++ = bone_names_buf;
            size_t len = utf8_length(*bone_names_file_ptr);
            memcpy(bone_names_buf, *bone_names_file_ptr++, len);
            bone_names_buf[len] = 0;
            bone_names_buf += len + 1;
        }
        *bone_names_ptr = 0;

        int32_t blocks_count = ex_data_file->blocks_count;
        ex_data->blocks_count = blocks_count;
        ex_data->blocks = force_malloc_s(object_skin_block, blocks_count);
        for (int32_t j = 0; j < blocks_count; j++) {
            object_skin_block* block = &ex_data->blocks[j];
            object_skin_block_file* block_file = &ex_data_file->blocks[j];

            object_skin_block_node_load(&block->base, &block_file->base);

            switch (block_file->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                block->type = OBJECT_SKIN_BLOCK_CLOTH;

                object_skin_block_cloth* cloth = &block->cloth;
                object_skin_block_cloth_file* cloth_file = &block_file->cloth;

            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                block->type = OBJECT_SKIN_BLOCK_CONSTRAINT;

                object_skin_block_constraint* constraint = &block->constraint;
                object_skin_block_constraint_file* constraint_file = &block_file->constraint;

                constraint->coupling = (object_skin_block_constraint_coupling)constraint_file->coupling;
                constraint->name_index = constraint_file->name_index;
                string_copy(&constraint_file->source_node_name, &constraint->source_node_name);

                switch (constraint_file->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
                    constraint->type = OBJECT_SKIN_BLOCK_CONSTRAINT_ORIENTATION;

                    object_skin_block_constraint_orientation* orientation = &constraint->orientation;
                    object_skin_block_constraint_orientation_file* orientation_file
                        = &constraint_file->orientation;

                    orientation->offset = orientation_file->offset;
                } break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
                    constraint->type = OBJECT_SKIN_BLOCK_CONSTRAINT_DIRECTION;

                    object_skin_block_constraint_direction* direction = &constraint->direction;
                    object_skin_block_constraint_direction_file* direction_file
                        = &constraint_file->direction;

                    object_skin_block_constraint_up_vector_old_load(
                        &direction->up_vector_old, &direction_file->up_vector_old);
                    direction->align_axis = direction_file->align_axis;
                    direction->target_offset = direction_file->target_offset;
                } break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION: {
                    constraint->type = OBJECT_SKIN_BLOCK_CONSTRAINT_POSITION;

                    object_skin_block_constraint_position* position = &constraint->position;
                    object_skin_block_constraint_position_file* position_file
                        = &constraint_file->position;

                    object_skin_block_constraint_up_vector_old_load(
                        &position->up_vector_old, &position_file->up_vector_old);
                    string_copy(&position_file->up_vector_old.name, &position->up_vector_old.name);
                    object_skin_block_constraint_attach_point_load(
                        &position->constrained_object, &position_file->constrained_object);
                    object_skin_block_constraint_attach_point_load(
                        &position->constraining_object, &position_file->constraining_object);
                } break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE: {
                    constraint->type = OBJECT_SKIN_BLOCK_CONSTRAINT_DISTANCE;

                    object_skin_block_constraint_distance* distance = &constraint->distance;
                    object_skin_block_constraint_distance_file* distance_file
                        = &constraint_file->distance;

                    object_skin_block_constraint_up_vector_old_load(
                        &distance->up_vector_old, &distance_file->up_vector_old);
                    distance->distance = distance_file->distance;
                    object_skin_block_constraint_attach_point_load(
                        &distance->constrained_object, &distance_file->constrained_object);
                    object_skin_block_constraint_attach_point_load(
                        &distance->constraining_object, &distance_file->constraining_object);
                } break;
                }

            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                block->type = OBJECT_SKIN_BLOCK_EXPRESSION;

                object_skin_block_expression* expression = &block->expression;
                object_skin_block_expression_file* expression_file = &block_file->expression;

                expression->name_index = expression_file->name_index;
                int32_t expressions_count = min(expression_file->expressions_count, 9);
                expression->expressions_count = expressions_count;
                for (int32_t k = 0; k < expressions_count; k++)
                    string_copy(&expression_file->expressions[k], &expression->expressions[k]);

                for (int32_t k = expressions_count; k < 9; k++)
                    expression->expressions[k] = string_empty;

            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                block->type = OBJECT_SKIN_BLOCK_MOTION;

                object_skin_block_motion* motion = &block->motion;
                object_skin_block_motion_file* motion_file = &block_file->motion;

                int32_t nodes_count = motion_file->nodes_count;
                if (!is_x)
                    motion->name_index = motion_file->name_index;
                else
                    string_copy(&motion_file->name, &motion->name);
                motion->nodes_count = nodes_count;
                motion->nodes = force_malloc_s(object_skin_motion_node, nodes_count);
                for (int32_t k = 0; k < nodes_count; k++) {
                    object_skin_motion_node* node = &motion->nodes[j];
                    object_skin_motion_node_file* node_file = &motion_file->nodes[j];

                    node->name_index = node_file->name_index;
                    node->transformation = node_file->transformation;
                }

            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                block->type = OBJECT_SKIN_BLOCK_OSAGE;

                object_skin_block_osage* osage = &block->osage;
                object_skin_block_osage_file* osage_file = &block_file->osage;

                osage->start_index = osage_file->start_index;
                osage->count = osage_file->count;

                int32_t nodes_count = osage_file->nodes_count;
                osage->nodes_count = nodes_count;
                osage->nodes = force_malloc_s(object_skin_osage_node, nodes_count);
                for (int32_t k = 0; k < nodes_count; k++) {
                    object_skin_osage_node* node = &osage->nodes[k];
                    object_skin_osage_node_file* node_file = &osage_file->nodes[k];

                    node->name_index = node_file->name_index;
                    node->length = node_file->length;
                    node->rotation = node_file->rotation;
                }

                osage->external_name_index = osage_file->external_name_index;
                osage->name_index = osage_file->name_index;
                string_copy(&osage_file->motion_node_name, &osage->motion_node_name);
            } break;
            }
        }

        int32_t osage_sibling_infos_count = ex_data_file->osage_sibling_infos_count;
        ex_data->osage_sibling_infos_count = osage_sibling_infos_count;
        ex_data->osage_sibling_infos = force_malloc_s(
            object_skin_osage_sibling_info, osage_sibling_infos_count);
        for (int32_t j = 0; j < osage_sibling_infos_count; j++) {
            object_skin_osage_sibling_info* osage_sibling_info = &ex_data->osage_sibling_infos[j];
            object_skin_osage_sibling_info_file* osage_sibling_info_file = &ex_data_file->osage_sibling_infos[j];

            osage_sibling_info->name_index = osage_sibling_info_file->name_index;
            osage_sibling_info->sibling_name_index = osage_sibling_info_file->sibling_name_index;
            osage_sibling_info->max_distance = osage_sibling_info_file->max_distance;
        }
        skin->ex_data_init = true;
    }

    int32_t textures_count = (int32_t)txp_set_file->textures.size();
    set->textures_count = textures_count;
    set->textures = force_malloc_s(GLuint, textures_count);
    texture_txp_set_load(txp_set_file, &set->texture_data, set->texture_ids);
    if (set->texture_ids)
        for (int32_t i = 0; i < textures_count; i++)
            if (set->texture_data[i])
                set->textures[i] = set->texture_data[i]->texture;
            else
                set->textures[i] = 0;

    object_set_vertex_buffer_load(set);
    object_set_index_buffer_load(set);
}

bool object_set_load_db_entry(object_set_info** set_info,
    void* data, object_database* obj_db, char* name) {
    if (!obj_db->get_object_set_info(name, set_info))
        return false;
    else if (object_storage_get_object_set((*set_info)->id)) {
        object_storage_append_object_set((*set_info)->id);
        return true;
    }

    size_t temp[2];
    temp[0] = (size_t)data;
    temp[1] = (size_t)*set_info;
    return ((data_struct*)data)->load_file(temp, "rom/objset/",
        (*set_info)->archive_file_name.c_str(), object_set_load_file);
}

bool object_set_load_db_entry(object_set_info** set_info,
    void* data, object_database* obj_db, const char* name) {
    if (!obj_db->get_object_set_info(name, set_info))
        return false;
    else if (object_storage_get_object_set((*set_info)->id)) {
        object_storage_append_object_set((*set_info)->id);
        return true;
    }

    size_t temp[2];
    temp[0] = (size_t)data;
    temp[1] = (size_t)*set_info;
    return ((data_struct*)data)->load_file(temp, "rom/objset/",
        (*set_info)->archive_file_name.c_str(), object_set_load_file);
}

bool object_set_load_by_db_entry(object_set_info* set_info,
    void* data, object_database* obj_db) {
    if (object_storage_get_object_set(set_info->id)) {
        object_storage_append_object_set(set_info->id);
        return true;
    }

    size_t temp[2];
    temp[0] = (size_t)data;
    temp[1] = (size_t)set_info;
    return ((data_struct*)data)->load_file(temp, "rom/objset/",
        set_info->archive_file_name.c_str(), object_set_load_file);
}

bool object_set_load_by_db_index(object_set_info** set_info,
    void* data, object_database* obj_db, uint32_t set_id) {
    if (!obj_db->get_object_set_info_by_set_id(set_id, set_info))
        return false;
    else if (object_storage_get_object_set(set_id)) {
        object_storage_append_object_set(set_id);
        return true;
    }

    size_t temp[2];
    temp[0] = (size_t)data;
    temp[1] = (size_t)*set_info;
    return ((data_struct*)data)->load_file(temp, "rom/objset/",
        (*set_info)->archive_file_name.c_str(), object_set_load_file);
}

bool object_set_load_by_hash(void* data,
    object_database* obj_db, texture_database* tex_db, uint32_t hash) {
    size_t temp[2];
    temp[0] = (size_t)obj_db;
    temp[1] = (size_t)tex_db;
    return ((data_struct*)data)->load_file_by_hash(temp, "rom/objset/",
        hash, object_set_load_file_modern);
}

void object_set_free(object_set* set) {
    if (!set)
        return;

    string_free(&set->name);

    bool is_x = set->is_x;
    for (int32_t i = 0; i < set->objects_count; i++) {
        object* obj = &set->objects[i];
        for (int32_t j = 0; j < obj->skin.bones_count; j++)
            string_free(&obj->skin.bones[j].name);
        free(obj->skin.bones);
        obj->skin.bones_count = 0;

        for (int32_t j = 0; j < obj->skin.ex_data.blocks_count; j++) {
            object_skin_block* block = &obj->skin.ex_data.blocks[j];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                object_skin_block_cloth* cloth = &block->cloth;
                string_free(&cloth->mesh_name);
                string_free(&cloth->backface_mesh_name);

                for (uint32_t k = 0; k < cloth->count; k++) {
                    object_skin_block_cloth_field_1C* sub = &cloth->field_1C[k];
                    string_free(&sub->sub_data_0.bone_name);
                    string_free(&sub->sub_data_1.bone_name);
                    string_free(&sub->sub_data_2.bone_name);
                    string_free(&sub->sub_data_3.bone_name);
                }
                free(cloth->field_1C);
                free(cloth->field_20);
                free(cloth->field_24);
                free(cloth->field_28);
                cloth->count = 0;
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                object_skin_block_constraint* constraint = &block->constraint;
                object_skin_block_node_free(&constraint->base);
                string_free(&constraint->source_node_name);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                object_skin_block_expression* expression = &block->expression;
                object_skin_block_node_free(&expression->base);
                for (int32_t k = 0; k < 9; k++)
                    string_free(&expression->expressions[k]);
                expression->expressions_count = 0;
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                object_skin_block_motion* motion = &block->motion;
                object_skin_block_node_free(&motion->base);
                if (is_x)
                    string_free(&motion->name);
                free(motion->nodes);
                motion->nodes_count = 0;
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                object_skin_block_osage* osage = &block->osage;
                object_skin_block_node_free(&osage->base);
                free(osage->nodes);
                osage->nodes_count = 0;
            } break;
            }
        }
        free(obj->skin.ex_data.blocks);
        obj->skin.ex_data.blocks_count = 0;

        free(obj->skin.ex_data.osage_nodes);
        obj->skin.ex_data.osage_nodes_count = 0;

        free(obj->skin.ex_data.bone_names_buf);
        free(obj->skin.ex_data.bone_names);
        obj->skin.ex_data.bone_names_count = 0;

        free(obj->skin.ex_data.osage_sibling_infos);
        obj->skin.ex_data.osage_sibling_infos_count = 0;

        if (obj->meshes)
            for (int32_t j = 0; j < obj->meshes_count; j++) {
                object_mesh* mesh = &obj->meshes[j];
                if (mesh->sub_meshes)
                    for (int32_t k = 0; k < mesh->sub_meshes_count; k++) {
                        object_sub_mesh* sub_mesh = &mesh->sub_meshes[k];
                        free(sub_mesh->bone_indices);
                        sub_mesh->bone_indices_count = 0;
                    }
                free(mesh->vertex);
                mesh->vertex_count = 0;
                free(mesh->indices);
                mesh->indices_count = 0;
                free(mesh->sub_meshes);
                mesh->sub_meshes_count = 0;
            }
        free(obj->meshes);
        obj->meshes_count = 0;
        free(obj->materials);
        obj->materials_count = 0;
        string_free(&obj->name);
    }
    free(set->objects);
    set->objects_count = 0;
    free(set->texture_ids);
    set->texture_ids_count = 0;
    for (int32_t i = 0; i < set->texture_names_count; i++)
        string_free(&set->texture_names[i]);
    free(set->texture_names);
    set->texture_names_count = 0;

    free(set->textures);
    set->textures_count = 0;
    free(set->texture_data);

    object_set_vertex_buffer_free(set);
    object_set_index_buffer_free(set);
}

inline object_material_shader_lighting_type object_material_shader_get_lighting_type(
    object_material_shader_flags* flags) {
    if (!flags->lambert_shading && !flags->phong_shading)
        return OBJECT_MATERIAL_SHADER_LIGHTING_CONSTANT;
    else if (!flags->phong_shading)
        return OBJECT_MATERIAL_SHADER_LIGHTING_LAMBERT;
    else
        return OBJECT_MATERIAL_SHADER_LIGHTING_PHONG;
}

inline int32_t object_material_texture_get_blend(object_material_texture* tex) {
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

inline int32_t object_material_texture_type_get_texcoord_index(
    object_material_texture_type type, int32_t index) {
    switch (type) {
    case OBJECT_MATERIAL_TEXTURE_COLOR:
    case OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (index < 2)
            return index;
    case OBJECT_MATERIAL_TEXTURE_NORMAL:
    case OBJECT_MATERIAL_TEXTURE_SPECULAR:
        return 0;
    case OBJECT_MATERIAL_TEXTURE_TRANSLUCENCY:
    case OBJECT_MATERIAL_TEXTURE_TRANSPARENCY:
        return 1;
    }
    return -1;
}

inline int32_t object_material_texture_type_get_texture_index(
    object_material_texture_type type, int32_t base_index) {
    switch (type) {
    case OBJECT_MATERIAL_TEXTURE_COLOR:
    case OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (base_index < 2)
            return base_index;
    case OBJECT_MATERIAL_TEXTURE_NORMAL:
        return 2;
    case OBJECT_MATERIAL_TEXTURE_SPECULAR:
        return 3;
    case OBJECT_MATERIAL_TEXTURE_TRANSLUCENCY:
        return 1;
    case OBJECT_MATERIAL_TEXTURE_TRANSPARENCY:
        return 4;
    //case OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // AFT
    case OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_CUBE:
        return 5;
    }
    return -1;
}

inline GLuint object_mesh_vertex_buffer_get_buffer(object_mesh_vertex_buffer* buffer) {
    if (buffer->index < buffer->count)
        return buffer->buffers[buffer->index];
    else
        return 0;
}

inline int32_t object_mesh_vertex_buffer_get_size(object_mesh_vertex_buffer* buffer) {
    if (!buffer->buffers[0])
        return 0;

    int32_t size = 0;
    gl_state_bind_array_buffer(buffer->buffers[0]);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_state_bind_array_buffer(buffer->buffers[0]);
    return size;
}

void object_skin_set_matrix_buffer(object_skin* s, mat4* matrices,
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

inline void object_storage_init() {
    object_storage_data = vector_old_empty(object_storage);
}

inline void object_storage_append_object_set(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id) {
            i->count++;
            return;
        }
}

inline void object_storage_insert_object_set(object_set* set, uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id) {
            object_set_free(&i->set);
            i->count++;
            i->set = *set;
            return;
        }

    object_storage* obj_set_storage = vector_old_object_storage_reserve_back(&object_storage_data);
    obj_set_storage->set_id = set_id;
    obj_set_storage->count = 1;
    obj_set_storage->set = *set;
}

inline object* object_storage_get_object(object_info obj_info) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == obj_info.set_id) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id)
                    return &set->objects[j];
            return 0;
        }
    return 0;
}

inline object_mesh* object_storage_get_object_mesh(object_info obj_info, char* mesh_name) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == obj_info.set_id) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id) {
                    object* obj = &set->objects[j];
                    for (int32_t k = 0; k < obj->meshes_count; k++)
                        if (!str_utils_compare(obj->meshes[k].name, mesh_name))
                            return &obj->meshes[k];
                    return 0;
                }
            return 0;
        }
    return 0;
}

inline object_mesh* object_storage_get_object_mesh_by_index(object_info obj_info, int32_t index) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == obj_info.set_id) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id) {
                    object* obj = &set->objects[j];
                    if (index > -1 && index < obj->meshes_count)
                        return &obj->meshes[index];
                    return 0;
                }
            return 0;
        }
    return 0;
}

inline object_mesh* object_storage_get_object_mesh_by_object_hash(uint32_t hash, char* mesh_name) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].hash == hash) {
                    object* obj = &set->objects[j];
                    for (int32_t k = 0; k < obj->meshes_count; k++)
                        if (!str_utils_compare(obj->meshes[k].name, mesh_name))
                            return &obj->meshes[k];
                    return 0;
                }
            return 0;
        }
    return 0;
}

inline object_mesh* object_storage_get_object_mesh_by_object_hash_index(uint32_t hash, int32_t index) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++) {
        object_set* set = &i->set;
        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                object* obj = &set->objects[j];
                if (index > -1 && index < obj->meshes_count)
                    return &obj->meshes[index];
                return 0;
            }
    }
    return 0;
}

inline int32_t object_storage_get_object_mesh_index(object_info obj_info, char* mesh_name) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == obj_info.set_id) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id) {
                    object* obj = &set->objects[j];
                    for (int32_t k = 0; k < obj->meshes_count; k++)
                        if (!str_utils_compare(obj->meshes[k].name, mesh_name))
                            return k;
                    return -1;
                }
            return -1;
        }
    return -1;
}

inline int32_t object_storage_get_object_mesh_index_by_hash(uint32_t hash, char* mesh_name) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++) {
        object_set* set = &i->set;
        for (int32_t j = 0; j < set->objects_count; j++)
            if (set->objects[j].hash == hash) {
                object* obj = &set->objects[j];
                for (int32_t k = 0; k < obj->meshes_count; k++)
                    if (!str_utils_compare(obj->meshes[k].name, mesh_name))
                        return k;
                return -1;
            }
    }
    return -1;
}

inline object_set* object_storage_get_object_set(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return &i->set;
    return 0;
}

inline ssize_t object_storage_get_object_set_count() {
    return vector_old_length(object_storage_data);
}

inline int32_t object_storage_get_object_set_load_count(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i->count;
    return 0;
}

inline object_set* object_storage_get_object_set_by_index(ssize_t index) {
    if (index >= 0 && index < vector_old_length(object_storage_data))
        return &object_storage_data.begin[index].set;
    return 0;
}

inline int32_t object_storage_get_object_set_load_count_by_index(ssize_t index) {
    if (index >= 0 && index < vector_old_length(object_storage_data))
        return object_storage_data.begin[index].count;
    return 0;
}

inline ssize_t object_storage_get_object_set_index(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i - object_storage_data.begin;
    return -1;
}

inline object_skin* object_storage_get_object_skin(object_info obj_info) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == obj_info.set_id) {
            object_set* set = &i->set;
            for (int32_t j = 0; j < set->objects_count; j++)
                if (set->objects[j].id == obj_info.id)
                    return set->objects[j].skin_init ? &set->objects[j].skin : 0;
            return 0;
        }
    return 0;
}

inline object_index_buffer* object_storage_get_object_index_buffers(uint32_t set_id) {
    object_set* set = object_storage_get_object_set(set_id);
    if (!set || !set->index_buffers)
        return 0;

    return set->index_buffers;
}

inline object_mesh_index_buffer* object_storage_get_object_mesh_index_buffer(object_info obj_info) {
    object_set* set = object_storage_get_object_set(obj_info.set_id);
    if (!set || !set->index_buffers)
        return 0;

    for (int32_t i = 0; i < set->objects_count; i++)
        if (set->objects[i].id == obj_info.id)
            return set->index_buffers[i].meshes;

    return 0;
}

inline object_vertex_buffer* object_storage_get_object_vertex_buffers(uint32_t set_id) {
    object_set* set = object_storage_get_object_set(set_id);
    if (!set || !set->vertex_buffers)
        return 0;

    return set->vertex_buffers;
}

inline object_mesh_vertex_buffer* object_storage_get_object_mesh_vertex_buffer(object_info obj_info) {
    object_set* set = object_storage_get_object_set(obj_info.set_id);
    if (!set || !set->vertex_buffers)
        return 0;

    for (int32_t i = 0; i < set->objects_count; i++)
        if (set->objects[i].id == obj_info.id)
            return set->vertex_buffers[i].meshes;

    return 0;
}

inline GLuint* object_storage_get_textures(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i->set.textures;
    return 0;
}

inline int32_t object_storage_get_textures_count(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i->set.textures_count;
    return 0;
}

inline uint32_t* object_storage_get_texture_ids(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i->set.texture_ids;
    return 0;
}

inline int32_t object_storage_get_texture_ids_count(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id)
            return i->set.texture_ids_count;
    return 0;
}

inline void object_storage_delete_object_set(uint32_t set_id) {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        if (i->set_id == set_id) {
            i->count--;
            if (i->count > 0)
                break;

            object_set* set = &i->set;
            for (int32_t j = 0; j < set->texture_ids_count; j++)
                texture_storage_delete_texture(set->texture_ids[j]);
            object_set_free(set);

            vector_old_object_storage_erase(&object_storage_data,
                i - object_storage_data.begin, 0);
            break;
        }
}

inline void object_storage_free() {
    for (object_storage* i = object_storage_data.begin; i != object_storage_data.end; i++)
        object_set_free(&i->set);
    vector_old_object_storage_free(&object_storage_data, 0);
}

static bool object_mesh_index_buffer_load(object_mesh_index_buffer* buffer, object_mesh* mesh) {
    if (!mesh->indices_count || !mesh->vertex) {
        *buffer = 0;
        return true;
    }

    *buffer = object_mesh_index_buffer_load_data(sizeof(uint16_t) * mesh->indices_count, mesh->indices);
    return true;
}

static GLuint object_mesh_index_buffer_load_data(size_t size, void* data) {
    if (!size)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    gl_state_bind_element_array_buffer(buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    gl_state_bind_element_array_buffer(0);
    return buffer;
}

static void object_mesh_index_buffer_free(object_mesh_index_buffer* buffer) {
    glDeleteBuffers(1, (GLuint*)buffer);
}

static bool object_mesh_vertex_buffer_load(object_mesh_vertex_buffer* buffer, object_mesh* mesh) {
    if (!mesh->vertex_size || !mesh->vertex_count || !mesh->vertex)
        return false;

    return object_mesh_vertex_buffer_load_data(buffer, (size_t)mesh->vertex_size * mesh->vertex_count,
        mesh->vertex, mesh->flags & OBJECT_MESH_FLAG_1 ? 2 : 1);
}

static bool object_mesh_vertex_buffer_load_data(object_mesh_vertex_buffer* buffer,
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

static void object_mesh_vertex_buffer_free(object_mesh_vertex_buffer* buffer) {
    glDeleteBuffers(buffer->count, buffer->buffers);
}

static bool object_set_index_buffer_load(object_set* set) {
    set->index_buffers_count = set->objects_count;
    set->index_buffers = force_malloc_s(object_index_buffer, set->objects_count);
    if (!set->index_buffers)
        return false;

    for (int32_t i = 0; i < set->objects_count; i++) {
        object* obj = &set->objects[i];
        object_index_buffer* buffer = &set->index_buffers[i];

        buffer->meshes_count = obj->meshes_count;
        buffer->meshes = force_malloc_s(object_mesh_index_buffer, obj->meshes_count);
        if (!buffer->meshes)
            return false;

        for (int32_t j = 0; j < buffer->meshes_count; j++)
            if (!object_mesh_index_buffer_load(&buffer->meshes[j], &obj->meshes[j]))
                return false;
    }
    return true;
}

static void object_set_index_buffer_free(object_set* set) {
    if (!set->index_buffers)
        return;

    for (int32_t i = 0; i < set->index_buffers_count; i++) {
        object_index_buffer* buffer = &set->index_buffers[i];
        if (buffer->meshes)
            for (int32_t j = 0; j < buffer->meshes_count; j++)
                object_mesh_index_buffer_free(&buffer->meshes[j]);
        free(buffer->meshes);
    }
    free(set->index_buffers);
    set->index_buffers_count = 0;
}

static bool object_set_vertex_buffer_load(object_set* set) {
    set->vertex_buffers_count = set->objects_count;
    set->vertex_buffers = force_malloc_s(object_vertex_buffer, set->objects_count);
    if (!set->vertex_buffers)
        return false;

    for (int32_t i = 0; i < set->objects_count; i++) {
        object* obj = &set->objects[i];
        object_vertex_buffer* buffer = &set->vertex_buffers[i];

        buffer->meshes_count = obj->meshes_count;
        buffer->meshes = force_malloc_s(object_mesh_vertex_buffer, obj->meshes_count);
        if (!buffer->meshes)
            return false;

        for (int32_t j = 0; j < buffer->meshes_count; j++)
            if (!object_mesh_vertex_buffer_load(&buffer->meshes[j], &obj->meshes[j]))
                return false;
    }
    return true;
}

static void object_set_vertex_buffer_free(object_set* set) {
    if (!set->vertex_buffers)
        return;

    for (int32_t i = 0; i < set->vertex_buffers_count; i++) {
        object_vertex_buffer* buffer = &set->vertex_buffers[i];
        if (buffer->meshes)
            for (int32_t j = 0; j < buffer->meshes_count; j++)
                object_mesh_vertex_buffer_free(&buffer->meshes[j]);
        free(buffer->meshes);
    }
    free(set->vertex_buffers);
    set->vertex_buffers_count = 0;
}

inline static void object_skin_block_constraint_attach_point_load(
    object_skin_block_constraint_attach_point* attach_point,
    object_skin_block_constraint_attach_point_file* attach_point_file) {
    attach_point->affected_by_orientation = attach_point_file->affected_by_orientation;
    attach_point->affected_by_scaling = attach_point_file->affected_by_scaling;
    attach_point->offset = attach_point_file->offset;
}
inline static void object_skin_block_constraint_up_vector_old_load(
    object_skin_block_constraint_up_vector_old* up_vector_old,
    object_skin_block_constraint_up_vector_old_file* up_vector_old_file) {
    up_vector_old->active = up_vector_old_file->active;
    up_vector_old->roll = up_vector_old_file->roll;
    up_vector_old->affected_axis = up_vector_old_file->affected_axis;
    up_vector_old->point_at = up_vector_old_file->point_at;
    string_copy(&up_vector_old_file->name, &up_vector_old->name);
}


inline static void object_skin_block_node_load(object_skin_block_node* node,
    object_skin_block_node_file* node_file) {
    string_copy(&node_file->parent_name, &node->parent_name);
    node->position = node_file->position;
    node->rotation = node_file->rotation;
    node->scale = node_file->scale;
}

inline static void object_skin_block_node_free(object_skin_block_node* node) {
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

inline static size_t object_vertex_flags_get_vertex_size(object_vertex_flags flags) {
    size_t size = 0;
    if (flags & OBJECT_VERTEX_POSITION)
        size += 12;
    if (flags & OBJECT_VERTEX_NORMAL)
        size += 12;
    if (flags & OBJECT_VERTEX_TANGENT)
        size += 16;
    if (flags & OBJECT_VERTEX_BINORMAL)
        size += 12;
    if (flags & OBJECT_VERTEX_TEXCOORD0)
        size += 8;
    if (flags & OBJECT_VERTEX_TEXCOORD1)
        size += 8;
    if (flags & OBJECT_VERTEX_TEXCOORD2)
        size += 8;
    if (flags & OBJECT_VERTEX_TEXCOORD3)
        size += 8;
    if (flags & OBJECT_VERTEX_COLOR0)
        size += 16;
    if (flags & OBJECT_VERTEX_COLOR1)
        size += 16;
    if (flags & OBJECT_VERTEX_BONE_DATA)
        size += 32;
    if (flags & OBJECT_VERTEX_UNKNOWN)
        size += 16;
    return size;
}

inline static size_t object_vertex_flags_get_vertex_size_comp(object_vertex_flags flags) {
    size_t size = 0;
    if (flags & OBJECT_VERTEX_POSITION)
        size += 12;
    if (flags & OBJECT_VERTEX_NORMAL)
        size += 8;
    if (flags & OBJECT_VERTEX_TANGENT)
        size += 8;
    if (flags & OBJECT_VERTEX_TEXCOORD0)
        size += 4;
    if (flags & OBJECT_VERTEX_TEXCOORD1)
        size += 4;
    if (flags & OBJECT_VERTEX_TEXCOORD2)
        size += 4;
    if (flags & OBJECT_VERTEX_TEXCOORD3)
        size += 4;
    if (flags & OBJECT_VERTEX_COLOR0)
        size += 8;
    if (flags & OBJECT_VERTEX_BONE_DATA)
        size += 16;
    return size;
}

static bool object_set_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    data_struct* ds = (data_struct*)((size_t*)data)[0];
    data_ft* d = &ds->data_ft;

    object_set_info* set_info = (object_set_info*)((size_t*)data)[1];

    farc f;
    if (!farc::load_file(&f, path, file, hash))
        return false;

    farc_file* obj = f.read_file(set_info->object_file_name.c_str());
    farc_file* tex = f.read_file(set_info->texture_file_name.c_str());

    if (obj && tex) {
        obj_set obj_set;
        obj_set.unpack_file(obj->data, obj->size, false);

        txp_set txp_set;
        txp_set.unpack_file(tex->data, false);

        if (obj_set.ready) {
            object_set object_set;
            object_set_init(&object_set);
            object_set_load(&object_set, &obj_set, &txp_set, &d->tex_db, set_info->name.c_str(), set_info->id, false);
            object_storage_insert_object_set(&object_set, set_info->id);
        }
    }
    return true;
}

static bool object_set_load_file_modern(void* data, const char* path, const char* file, uint32_t hash) {
    object_database* obj_db = (object_database*)((size_t*)data)[0];
    texture_database* tex_db = (texture_database*)((size_t*)data)[1];

    farc f;
    if (!farc::load_file(&f, path, file, hash))
        return false;

    char buf[0x100];
    size_t file_len = utf8_length(file);
    if (file_len >= 0x100)
        return false;

    memcpy(buf, file, file_len);
    char* ext = buf + file_len - 5;

    memcpy(ext, ".osd", 5);
    farc_file* osd = f.read_file(buf);

    memcpy(ext, ".txd", 5);
    farc_file* txd = f.read_file(buf);

    memcpy(ext, ".osi", 5);
    farc_file* osi = f.read_file(buf);

    memcpy(ext, ".txi", 5);
    farc_file* txi = f.read_file(buf);

    if (osd && txd && osi && txi) {
        obj_set obj_set;
        obj_set.unpack_file(osd->data, osd->size, true);

        txp_set txp_set;
        txp_set.unpack_file_modern(txd->data, txd->size);

        obj_db->read(osi->data, osi->size, true);
        tex_db->read(txi->data, txi->size, true);

        std::string* name = 0;
        if (obj_db->ready)
            for (object_set_info& m : obj_db->object_set)
                if (m.id == hash) {
                    name = &m.name;
                    break;
                }

        if (name && obj_set.ready) 
            if (!object_storage_get_object_set(hash)) {
                object_set object_set;
                object_set_init(&object_set);
                object_set_load(&object_set, &obj_set, &txp_set, tex_db, name->c_str(), hash, false);
                object_storage_insert_object_set(&object_set, hash);
            }
            else
                object_storage_append_object_set(hash);
    }
    return true;
}
