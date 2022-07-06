/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "obj.hpp"
#include <vector>
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/stream.hpp"
#include "half_t.hpp"
#include "hash.hpp"
#include "sort.hpp"
#include "str_utils.hpp"

enum obj_vertex_format_file {
    OBJ_VERTEX_FILE_POSITION       = 0x00000001,
    OBJ_VERTEX_FILE_NORMAL         = 0x00000002,
    OBJ_VERTEX_FILE_TANGENT        = 0x00000004,
    OBJ_VERTEX_FILE_BINORMAL       = 0x00000008,
    OBJ_VERTEX_FILE_TEXCOORD0      = 0x00000010,
    OBJ_VERTEX_FILE_TEXCOORD1      = 0x00000020,
    OBJ_VERTEX_FILE_TEXCOORD2      = 0x00000040,
    OBJ_VERTEX_FILE_TEXCOORD3      = 0x00000080,
    OBJ_VERTEX_FILE_COLOR0         = 0x00000100,
    OBJ_VERTEX_FILE_COLOR1         = 0x00000200,
    OBJ_VERTEX_FILE_BONE_WEIGHT    = 0x00000400,
    OBJ_VERTEX_FILE_BONE_INDEX     = 0x00000800,
    OBJ_VERTEX_FILE_UNKNOWN        = 0x00001000,
    OBJ_VERTEX_FILE_MODERN_STORAGE = 0x80000000,
};

struct obj_skin_block_header {
    int64_t block_signature_offset;
    int64_t block_offset;
};

struct obj_skin_ex_data_header {
    uint32_t osage_count;
    int64_t osage_nodes_offset;
    int64_t osage_names_offset;
    int64_t blocks_offset;
    uint32_t bone_names_count;
    int64_t bone_names_offset;
    int64_t osage_sibling_infos_offset;
    uint32_t cloth_count;
};

struct obj_skin_header {
    int64_t bone_ids_offset;
    int64_t bone_matrices_offset;
    int64_t bone_names_offset;
    int64_t ex_data_offset;
    int64_t bone_parent_ids_offset;
};

struct obj_sub_mesh_header {
    int64_t bone_indices_offset;
    int64_t indices_offset;
};

struct obj_mesh_header {
    int64_t submesh_array;
    obj_vertex_format_file format;
    uint32_t size_vertex;
    uint32_t num_vertex;
    int64_t vertex[20];
    uint32_t vertex_flags;
};

struct obj_header {
    int64_t mesh_array;
    int64_t material_array;
};

struct obj_set_header {
    int32_t last_obj_id;
    int64_t obj_data;
    int64_t obj_skin_data;
    int64_t obj_name_data;
    int64_t obj_id_data;
    int64_t tex_id_data;
};

const char obj_material_texture_enrs_table_bin[] =
"\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x40\xBA\x44\xB0\x01\x10\x02\x18\x01\x10\x01\x10\x01\x10\x01"
"\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01"
"\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01"
"\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50"
"\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x50\x20\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x50\x40\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

static enrs obj_material_texture_enrs_table;
static bool obj_material_texture_enrs_table_initialized;

static void obj_material_texture_enrs_table_init();
static void obj_material_texture_enrs_table_free(void);
static void obj_set_classic_read_inner(obj_set* os, stream& s);
static void obj_set_classic_write_inner(obj_set* os, stream& s);
static void obj_classic_read_index(obj* obj, stream& s, obj_sub_mesh* sub_mesh);
static void obj_classic_write_index(obj* obj, stream& s, obj_sub_mesh* sub_mesh);
static void obj_classic_read_model(obj* obj, stream& s, int64_t base_offset);
static void obj_classic_write_model(obj* obj, stream& s, int64_t base_offset);
static void obj_classic_read_skin(obj* obj, stream& s, int64_t base_offset);
static void obj_classic_write_skin(obj* obj, stream& s, int64_t base_offset);
static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    int64_t* mats_offset, int64_t* root_offset, int64_t* nodes_offset,
    int64_t* mesh_indices_offset, int64_t* backface_mesh_indices_offset);
static void obj_classic_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, char** str);
static void obj_classic_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* offsets);
static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, char** str);
static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, char** str);
static void obj_classic_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names);
static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset);
static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream& s, char** str);
static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, int64_t* nodes_offset);
static void obj_classic_read_skin_param(obj_skin_skin_param* skp,
    stream& s, char** str);
static void obj_classic_write_skin_param(obj_skin_skin_param* skp,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t num_vertex, obj_vertex_format_file vertex_format_file);
static void obj_classic_write_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t* num_vertex, obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex);
static void obj_set_modern_read_inner(obj_set* os, stream& s);
static void obj_set_modern_write_inner(obj_set* os, stream& s);
static void obj_modern_read_index(obj* obj, stream& s,
    obj_sub_mesh* sub_mesh);
static void obj_modern_write_index(obj* obj, stream& s, bool is_x,
    obj_sub_mesh* sub_mesh, f2_struct* ovtx);
static void obj_modern_read_model(obj* obj, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx);
static void obj_modern_write_model(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* omdl);
static void obj_modern_read_skin(obj* obj, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x);
static void obj_modern_write_skin(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* oskn);
static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* mats_offset, int64_t* root_offset, int64_t* nodes_offset,
    int64_t* mesh_indices_offset, int64_t* backface_mesh_indices_offset);
static void obj_modern_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names,
    bool is_x, int64_t* offsets);
static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, bool is_x);
static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset);
static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream& s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    const uint32_t attrib_flags, uint32_t num_vertex, uint32_t size_vertex);
static void obj_modern_write_vertex(obj* obj, stream& s, bool is_x,
    int64_t* vertex, obj_mesh* mesh, uint32_t* attrib_flags,
    uint32_t* num_vertex, uint32_t* size_vertex, f2_struct* ovtx);
static void obj_skin_block_node_free(obj_skin_block_node* b);
static uint32_t obj_skin_strings_get_string_index(std::vector<std::string>& vec, const char* str);
static int64_t obj_skin_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, const char* str);
static int64_t obj_skin_strings_get_string_offset_by_index(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, char** strings, uint32_t index);
static void obj_skin_strings_push_back_check(std::vector<std::string>& vec, const char* str);
static void obj_skin_strings_push_back_check_by_index(std::vector<std::string>& vec,
    char** strings, uint32_t index);

obj_bounding_sphere::obj_bounding_sphere() : radius() {

}

obj_material_shader_lighting_type obj_material_shader_attrib::get_lighting_type() {
    if (!m.is_lgt_diffuse && !m.is_lgt_specular)
        return OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT;
    else if (!m.is_lgt_specular)
        return OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT;
    else
        return OBJ_MATERIAL_SHADER_LIGHTING_PHONG;
}

int32_t obj_texture_attrib::get_blend() {
    switch (m.blend) {
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

obj_sub_mesh::obj_sub_mesh() : flags(), bounding_sphere(), material_index(), uv_index(), bone_indices(),
bone_indices_count(), bones_per_vertex(), primitive_type(), index_format(), indices(), indices_count(),
attrib(), axis_aligned_bounding_box(), first_index(), last_index(), indices_offset() {

}

obj_mesh* obj::get_obj_mesh(const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (uint32_t k = 0; k < num_mesh; k++)
        if (hash_utf8_murmurhash(mesh_array[k].name) == name_hash)
            return &mesh_array[k];
    return 0;
}

 uint32_t obj::get_obj_mesh_index(const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (uint32_t k = 0; k < num_mesh; k++)
        if (hash_utf8_murmurhash(mesh_array[k].name) == name_hash)
            return k;
    return -1;
}

obj_set::obj_set() : ready(), modern(), is_x(), obj_data(),
obj_num(), tex_id_data(), tex_id_num(), reserved() {

}

obj_set::~obj_set() {
    for (uint32_t i = 0; i < obj_num; i++) {
        obj* obj = &obj_data[i];
        for (uint32_t j = 0; j < obj->skin.bones_count; j++)
            free(obj->skin.bones[j].name);
        free(obj->skin.bones);

        for (uint32_t j = 0; j < obj->skin.ex_data.blocks_count; j++) {
            obj_skin_block* block = &obj->skin.ex_data.blocks[j];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &block->cloth;
                free(cloth->mesh_name);
                free(cloth->backface_mesh_name);

                for (uint32_t k = 0; k < cloth->root_count; k++) {
                    obj_skin_block_cloth_root* sub = &cloth->root[k];
                    for (uint32_t l = 0; l < 4; l++)
                        free(sub->bone_weights[l].bone_name);
                }
                free(cloth->root);
                free(cloth->nodes);
                free(cloth->mesh_indices);
                free(cloth->backface_mesh_indices);
                free(cloth->skin_param.coli);
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_block_node_free(&constraint->base);
                free(constraint->source_node_name);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                obj_skin_block_node_free(&expression->base);
                for (uint32_t k = 0; k < 9; k++)
                    free(expression->expressions[k]);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_block_node_free(&motion->base);
                if (is_x)
                    free(motion->name);
                free(motion->nodes);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_block_node_free(&osage->base);
                free(osage->skin_param.coli);
                free(osage->nodes);
            } break;
            }
        }
        free(obj->skin.ex_data.blocks);
        free(obj->skin.ex_data.osage_nodes);
        free(obj->skin.ex_data.bone_names_buf);
        free(obj->skin.ex_data.bone_names);
        free(obj->skin.ex_data.osage_sibling_infos);

        if (obj->mesh_array)
            for (uint32_t j = 0; j < obj->num_mesh; j++) {
                obj_mesh* mesh = &obj->mesh_array[j];
                if (mesh->submesh_array)
                    for (uint32_t k = 0; k < mesh->num_submesh; k++) {
                        obj_sub_mesh* sub_mesh = &mesh->submesh_array[k];
                        free(sub_mesh->bone_indices);
                        free(sub_mesh->indices);
                    }
                free(mesh->vertex);
                free(mesh->submesh_array);
            }
        free(obj->mesh_array);
        free(obj->material_array);
        free(obj->name);
    }
    free(obj_data);
    free(tex_id_data);
}

void obj_set::pack_file(void** data, size_t* size) {
    if (!data || !ready)
        return;

    stream s;
    s.open();
    if (!modern)
        obj_set_classic_write_inner(this, s);
    else
        obj_set_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

void obj_set::unpack_file(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    stream s;
    s.open(data, size);
    if (!modern)
        obj_set_classic_read_inner(this, s);
    else
        obj_set_modern_read_inner(this, s);
}

static void obj_material_texture_enrs_table_init() {
    if (!obj_material_texture_enrs_table_initialized) {
        stream s;
        s.open((void*)&obj_material_texture_enrs_table_bin,
            sizeof(obj_material_texture_enrs_table_bin));
        obj_material_texture_enrs_table.read(s);
        atexit(obj_material_texture_enrs_table_free);
        obj_material_texture_enrs_table_initialized = true;
    }
}

static void obj_material_texture_enrs_table_free(void) {
    obj_material_texture_enrs_table.vec.clear();
    obj_material_texture_enrs_table.vec.shrink_to_fit();
    obj_material_texture_enrs_table_initialized = false;
}

static void obj_set_classic_read_inner(obj_set* os, stream& s) {
    uint32_t version = s.read_uint32_t();
    if (version != 0x05062500) {
        os->is_x = false;
        os->modern = false;
        os->ready = false;
        return;
    }

    obj_set_header osh = {};
    os->obj_num = s.read_int32_t();
    osh.last_obj_id = s.read_int32_t();
    osh.obj_data = s.read_int32_t();
    osh.obj_skin_data = s.read_int32_t();
    osh.obj_name_data = s.read_int32_t();
    osh.obj_id_data = s.read_int32_t();
    osh.tex_id_data = s.read_int32_t();
    os->tex_id_num = s.read_int32_t();
    os->reserved[0] = s.read_uint32_t();
    os->reserved[1] = s.read_uint32_t();

    uint32_t obj_num = os->obj_num;
    os->obj_data = force_malloc_s(obj, os->obj_num);

    int32_t* obj_datas = 0;
    if (osh.obj_data) {
        obj_datas = force_malloc_s(int32_t, obj_num);
        s.set_position(osh.obj_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_datas[i] = s.read_int32_t();
    }

    int32_t* obj_skin_datas = 0;
    if (osh.obj_skin_data) {
        obj_skin_datas = force_malloc_s(int32_t, obj_num);
        s.set_position(osh.obj_skin_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_skin_datas[i] = s.read_int32_t();
    }

    int32_t* obj_name_datas = 0;
    if (osh.obj_name_data) {
        obj_name_datas = force_malloc_s(int32_t, obj_num);
        s.set_position(osh.obj_name_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_name_datas[i] = s.read_int32_t();
    }

    if (osh.obj_data) {
        for (uint32_t i = 0; i < obj_num; i++) {
            obj* obj = &os->obj_data[i];
            if (obj_datas[i])
                obj_classic_read_model(obj, s, obj_datas[i]);

            if (osh.obj_skin_data && obj_skin_datas[i])
                obj_classic_read_skin(obj, s, obj_skin_datas[i]);

            if (osh.obj_name_data && obj_name_datas[i]) {
                obj->name = s.read_utf8_string_null_terminated_offset(obj_name_datas[i]);
                obj->hash = hash_utf8_murmurhash(obj->name);
            }
            else
                obj->hash = hash_murmurhash_empty;
        }

        s.set_position(osh.obj_id_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            os->obj_data[i].id = s.read_uint32_t();
    }

    if (osh.tex_id_data) {
        os->tex_id_data = force_malloc_s(uint32_t, os->tex_id_num);
        s.set_position(osh.tex_id_data, SEEK_SET);
        for (uint32_t i = 0; i < os->tex_id_num; i++)
            os->tex_id_data[i] = s.read_uint32_t();
    }

    free(obj_datas);
    free(obj_skin_datas);
    free(obj_name_datas);

    os->is_x = false;
    os->modern = false;
    os->ready = true;
}

static void obj_set_classic_write_inner(obj_set* os, stream& s) {
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.align_write(0x10);

    obj_set_header osh = {};
    osh.last_obj_id = -1;

    uint32_t obj_num = os->obj_num;

    osh.obj_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(0);
    s.align_write(0x10);

    uint32_t* obj_datas = force_malloc_s(uint32_t, obj_num);
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_datas[i] = (uint32_t)s.get_position();
        obj_classic_write_model(&os->obj_data[i], s, obj_datas[i]);
    }
    s.align_write(0x10);

    s.position_push(osh.obj_data, SEEK_SET);
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_uint32_t(obj_datas[i]);
    s.position_pop();
    free(obj_datas);

    osh.obj_id_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++) {
        int32_t object_id = os->obj_data[i].id;
        s.write_uint32_t(object_id);
        if (osh.last_obj_id < object_id)
            osh.last_obj_id = object_id;
    }
    s.align_write(0x10);

    int32_t* obj_name_datas = force_malloc_s(int32_t, obj_num);
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_name_datas[i] = (int32_t)s.get_position();
        s.write_utf8_string_null_terminated(os->obj_data[i].name);
    }
    s.align_write(0x10);

    osh.obj_name_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(obj_name_datas[i]);
    s.align_write(0x10);
    free(obj_name_datas);

    osh.tex_id_data = s.get_position();
    for (uint32_t i = 0; i < os->tex_id_num; i++)
        s.write_uint32_t(os->tex_id_data[i]);
    s.align_write(0x10);

    osh.obj_skin_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(0);
    s.align_write(0x10);

    int32_t* obj_skin_datas = force_malloc_s(int32_t, obj_num);
    for (uint32_t i = 0; i < obj_num; i++) {
        if (!os->obj_data[i].skin_init) {
            obj_skin_datas[i] = 0;
            continue;
        }

        obj_skin_datas[i] = (int32_t)s.get_position();
        obj_classic_write_skin(&os->obj_data[i], s, obj_skin_datas[i]);
    }
    s.align_write(0x10);

    s.position_push(osh.obj_skin_data, SEEK_SET);
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(obj_skin_datas[i]);
    s.position_pop();
    free(obj_skin_datas);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(0x05062500);
    s.write_int32_t(os->obj_num);
    s.write_uint32_t(osh.last_obj_id);
    s.write_uint32_t((uint32_t)osh.obj_data);
    s.write_uint32_t((uint32_t)osh.obj_skin_data);
    s.write_uint32_t((uint32_t)osh.obj_name_data);
    s.write_uint32_t((uint32_t)osh.obj_id_data);
    s.write_uint32_t((uint32_t)osh.tex_id_data);
    s.write_int32_t(os->tex_id_num);
    s.write_uint32_t(os->reserved[0]);
    s.write_uint32_t(os->reserved[1]);
    s.position_pop();
}

static void obj_classic_read_index(obj* obj, stream& s,
    obj_sub_mesh* sub_mesh) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t indices_count = sub_mesh->indices_count;
    uint32_t* indices = force_malloc_s(uint32_t, indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < indices_count; i++) {
            uint8_t idx = s.read_uint8_t();
            indices[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < indices_count; i++) {
            uint16_t idx = s.read_uint16_t();
            indices[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < indices_count; i++)
            indices[i] = s.read_uint32_t();
        break;
    }
    sub_mesh->indices = indices;
}

static void obj_classic_write_index(obj* obj, stream& s, obj_sub_mesh* sub_mesh) {
    uint32_t* indices = sub_mesh->indices;
    uint32_t indices_count = sub_mesh->indices_count;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < indices_count; i++)
            s.write_uint8_t((uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < indices_count; i++)
            s.write_uint16_t((uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < indices_count; i++)
            s.write_uint32_t(indices[i]);
        break;
    }
    s.align_write(0x04);
}

static void obj_classic_read_model(obj* obj, stream& s, int64_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    s.set_position(base_offset, SEEK_SET);

    obj_header oh = {};
    s.read_uint32_t(); // version
    s.read_uint32_t(); // flags
    obj->bounding_sphere.center.x = s.read_float_t();
    obj->bounding_sphere.center.y = s.read_float_t();
    obj->bounding_sphere.center.z = s.read_float_t();
    obj->bounding_sphere.radius = s.read_float_t();
    obj->num_mesh = s.read_int32_t();
    oh.mesh_array = s.read_int32_t();
    obj->num_material = s.read_int32_t();
    oh.material_array = s.read_int32_t();
    obj->reserved[0] = s.read_uint32_t();
    obj->reserved[1] = s.read_uint32_t();
    obj->reserved[2] = s.read_uint32_t();
    obj->reserved[3] = s.read_uint32_t();
    obj->reserved[4] = s.read_uint32_t();
    obj->reserved[5] = s.read_uint32_t();
    obj->reserved[6] = s.read_uint32_t();
    obj->reserved[7] = s.read_uint32_t();
    obj->reserved[8] = s.read_uint32_t();
    obj->reserved[9] = s.read_uint32_t();

    if (oh.mesh_array > 0) {
        obj->mesh_array = force_malloc_s(obj_mesh, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            s.set_position(base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);

            obj_mesh_header mh = {};
            mesh->flags = s.read_uint32_t();
            mesh->bounding_sphere.center.x = s.read_float_t();
            mesh->bounding_sphere.center.y = s.read_float_t();
            mesh->bounding_sphere.center.z = s.read_float_t();
            mesh->bounding_sphere.radius = s.read_float_t();
            mesh->num_submesh = s.read_int32_t();
            mh.submesh_array = s.read_int32_t();
            mh.format = (obj_vertex_format_file)s.read_uint32_t();
            mh.size_vertex = s.read_int32_t();
            mh.num_vertex = s.read_int32_t();

            for (uint32_t j = 0; j < 20; j++)
                mh.vertex[j] = s.read_int32_t();

            mesh->attrib.w = s.read_uint32_t();
            mh.vertex_flags = s.read_uint32_t();
            mesh->reserved[0] = s.read_uint32_t();
            mesh->reserved[1] = s.read_uint32_t();
            mesh->reserved[2] = s.read_uint32_t();
            mesh->reserved[3] = s.read_uint32_t();
            mesh->reserved[4] = s.read_uint32_t();
            mesh->reserved[5] = s.read_uint32_t();
            s.read(&mesh->name, sizeof(mesh->name));
            mesh->name[sizeof(mesh->name) - 1] = 0;

            if (mh.submesh_array) {
                mesh->submesh_array = force_malloc_s(obj_sub_mesh, mesh->num_submesh);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                    s.set_position(base_offset + mh.submesh_array + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh = {};
                    sub_mesh->flags = s.read_uint32_t();
                    sub_mesh->bounding_sphere.center.x = s.read_float_t();
                    sub_mesh->bounding_sphere.center.y = s.read_float_t();
                    sub_mesh->bounding_sphere.center.z = s.read_float_t();
                    sub_mesh->bounding_sphere.radius = s.read_float_t();
                    sub_mesh->material_index = s.read_uint32_t();
                    s.read(&sub_mesh->uv_index, 0x08);
                    sub_mesh->bone_indices_count = s.read_int32_t();
                    smh.bone_indices_offset = s.read_uint32_t();
                    sub_mesh->bones_per_vertex = s.read_uint32_t();
                    sub_mesh->primitive_type = (obj_primitive_type)s.read_uint32_t();
                    sub_mesh->index_format = (obj_index_format)s.read_uint32_t();
                    sub_mesh->indices_count = s.read_int32_t();
                    smh.indices_offset = s.read_uint32_t();
                    sub_mesh->attrib.w = s.read_uint32_t();
                    sub_mesh->bounding_box.center = sub_mesh->bounding_sphere.center;
                    sub_mesh->bounding_box.size.x = sub_mesh->bounding_sphere.radius * 2.0f;
                    sub_mesh->bounding_box.size.y = sub_mesh->bounding_sphere.radius * 2.0f;
                    sub_mesh->bounding_box.size.z = sub_mesh->bounding_sphere.radius * 2.0f;
                    s.read(0, 0x18);
                    sub_mesh->indices_offset = s.read_uint32_t();

                    if (sub_mesh->bones_per_vertex == 4 && smh.bone_indices_offset) {
                        sub_mesh->bone_indices = force_malloc_s(uint16_t, sub_mesh->bone_indices_count);
                        s.set_position(base_offset + smh.bone_indices_offset, SEEK_SET);
                        s.read(sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    }

                    s.set_position(base_offset + smh.indices_offset, SEEK_SET);
                    obj_classic_read_index(obj, s, sub_mesh);
                }
            }

            obj_classic_read_vertex(obj, s, mh.vertex, mesh,
                base_offset, mh.num_vertex, mh.format);
        }
    }

    if (oh.material_array > 0) {
        s.set_position(base_offset + oh.material_array, SEEK_SET);
        obj->material_array = force_malloc_s(obj_material_data, obj->num_material);
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data mat_data;
            s.read(&mat_data, sizeof(obj_material_data));

            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
            obj->material_array[i] = mat_data;
        }
    }
}

static void obj_classic_write_model(obj* obj, stream& s, int64_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_float_t(0);
    s.write_float_t(0);
    s.write_float_t(0);
    s.write_float_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.write_uint32_t(0);
    s.align_write(0x10);

    obj_header oh = {};
    if (obj->num_mesh) {
        oh.mesh_array = s.get_position() - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            s.write(0x04);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_uint32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);

            for (uint32_t j = 0; j < 20; j++)
                s.write_int32_t(0);

            s.write_int32_t(0);
            s.write_uint32_t(0);
            s.write(0x18);
            s.write(sizeof(mesh->name));
        }

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            if (mesh->num_submesh) {
                mh->submesh_array = s.get_position() - base_offset;
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    s.write(0x04);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_uint32_t(0);
                    s.write(0x08);
                    s.write_int32_t(0);
                    s.write_int32_t(0);
                    s.write_uint32_t(0);
                    s.write_uint32_t(0);
                    s.write_uint32_t(0);
                    s.write_int32_t(0);
                    s.write_int32_t(0);
                    s.write_uint32_t(0);
                    s.write(0x18);
                    s.write_uint32_t(0);
                }
            }

            obj_classic_write_vertex(obj, s, mh->vertex,
                mesh, base_offset, &mh->num_vertex, &mh->format, &mh->size_vertex);

            for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                obj_sub_mesh_header smh = {};
                if (sub_mesh->bones_per_vertex == 4 && sub_mesh->bone_indices_count) {
                    smh.bone_indices_offset = s.get_position() - base_offset;
                    s.write(sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    s.align_write(0x04);
                }

                smh.indices_offset = s.get_position() - base_offset;
                obj_classic_write_index(obj, s, sub_mesh);

                s.position_push(base_offset + mh->submesh_array + sub_mesh_size * j, SEEK_SET);
                s.write_uint32_t(sub_mesh->flags);
                s.write_float_t(sub_mesh->bounding_sphere.center.x);
                s.write_float_t(sub_mesh->bounding_sphere.center.y);
                s.write_float_t(sub_mesh->bounding_sphere.center.z);
                s.write_float_t(sub_mesh->bounding_sphere.radius);
                s.write_uint32_t(sub_mesh->material_index);
                s.write(&sub_mesh->uv_index, 0x08);
                s.write_int32_t(sub_mesh->bone_indices_count);
                s.write_uint32_t((uint32_t)smh.bone_indices_offset);
                s.write_uint32_t(sub_mesh->bones_per_vertex);
                s.write_uint32_t(sub_mesh->primitive_type);
                s.write_uint32_t(sub_mesh->index_format);
                s.write_int32_t(sub_mesh->indices_count);
                s.write_uint32_t((uint32_t)smh.indices_offset);
                s.write_uint32_t(sub_mesh->attrib.w);
                s.write(0x18);
                s.write_uint32_t(sub_mesh->indices_offset);
                s.position_pop();
            }
        }

        s.position_push(base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            s.write_uint32_t(mesh->flags);
            s.write_float_t(mesh->bounding_sphere.center.x);
            s.write_float_t(mesh->bounding_sphere.center.y);
            s.write_float_t(mesh->bounding_sphere.center.z);
            s.write_float_t(mesh->bounding_sphere.radius);
            s.write_int32_t(mesh->num_submesh);
            s.write_uint32_t((uint32_t)mh->submesh_array);
            s.write_uint32_t(mh->format);
            s.write_int32_t(mh->size_vertex);
            s.write_int32_t(mh->num_vertex);

            for (uint32_t j = 0; j < 20; j++)
                s.write_uint32_t((uint32_t)mh->vertex[j]);

            s.write_uint32_t(mesh->attrib.w);
            s.write_uint32_t(mh->vertex_flags);
            s.write_uint32_t(mesh->reserved[0]);
            s.write_uint32_t(mesh->reserved[1]);
            s.write_uint32_t(mesh->reserved[2]);
            s.write_uint32_t(mesh->reserved[3]);
            s.write_uint32_t(mesh->reserved[4]);
            s.write_uint32_t(mesh->reserved[5]);
            s.write(&mesh->name, sizeof(mesh->name) - 1);
            s.write_char('\0');
        }
        s.position_pop();
        free(mhs);
    }

    if (obj->num_material) {
        oh.material_array = s.get_position() - base_offset;
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data mat_data = obj->material_array[i];
            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
            s.write(&mat_data, sizeof(obj_material_data));
        }
    }
    s.align_write(0x10);

    s.position_push(base_offset, SEEK_SET);
    s.write_uint32_t(0x10000);
    s.write_uint32_t(0x00);
    s.write_float_t(obj->bounding_sphere.center.x);
    s.write_float_t(obj->bounding_sphere.center.y);
    s.write_float_t(obj->bounding_sphere.center.z);
    s.write_float_t(obj->bounding_sphere.radius);
    s.write_int32_t(obj->num_mesh);
    s.write_uint32_t((uint32_t)oh.mesh_array);
    s.write_int32_t(obj->num_material);
    s.write_uint32_t((uint32_t)oh.material_array);
    s.write_uint32_t(obj->reserved[0]);
    s.write_uint32_t(obj->reserved[1]);
    s.write_uint32_t(obj->reserved[2]);
    s.write_uint32_t(obj->reserved[3]);
    s.write_uint32_t(obj->reserved[4]);
    s.write_uint32_t(obj->reserved[5]);
    s.write_uint32_t(obj->reserved[6]);
    s.write_uint32_t(obj->reserved[7]);
    s.write_uint32_t(obj->reserved[8]);
    s.write_uint32_t(obj->reserved[9]);
    s.position_pop();
}

static void obj_classic_read_skin(obj* obj, stream& s, int64_t base_offset) {
    obj_skin* sk = &obj->skin;
    obj->skin_init = true;
    s.set_position(base_offset, SEEK_SET);

    obj_skin_header sh = {};
    sh.bone_ids_offset = s.read_uint32_t();
    sh.bone_matrices_offset = s.read_uint32_t();
    sh.bone_names_offset = s.read_uint32_t();
    sh.ex_data_offset = s.read_uint32_t();
    sk->bones_count = s.read_int32_t();
    sh.bone_parent_ids_offset = s.read_uint32_t();
    s.read(0, 0x0C);

    if (sh.bone_ids_offset) {
        sk->bones = force_malloc_s(obj_skin_bone, sk->bones_count);

        int32_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(int32_t, sk->bones_count);
            s.set_position(sh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                bone_names_offsets[i] = s.read_int32_t();
        }

        if (sh.bone_ids_offset) {
            s.set_position(sh.bone_ids_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                sk->bones[i].id = s.read_uint32_t();

            if (sh.bone_matrices_offset) {
                s.set_position(sh.bone_matrices_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++) {
                    mat4& mat = sk->bones[i].inv_bind_pose_mat;
                    mat.row0.x = s.read_float_t();
                    mat.row1.x = s.read_float_t();
                    mat.row2.x = s.read_float_t();
                    mat.row3.x = s.read_float_t();
                    mat.row0.y = s.read_float_t();
                    mat.row1.y = s.read_float_t();
                    mat.row2.y = s.read_float_t();
                    mat.row3.y = s.read_float_t();
                    mat.row0.z = s.read_float_t();
                    mat.row1.z = s.read_float_t();
                    mat.row2.z = s.read_float_t();
                    mat.row3.z = s.read_float_t();
                    mat.row0.w = s.read_float_t();
                    mat.row1.w = s.read_float_t();
                    mat.row2.w = s.read_float_t();
                    mat.row3.w = s.read_float_t();
                }
            }

            if (sh.bone_names_offset)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].name = s.read_utf8_string_null_terminated_offset(
                        bone_names_offsets[i]);

            if (sh.bone_parent_ids_offset) {
                s.set_position(sh.bone_parent_ids_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].parent = s.read_uint32_t();
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        s.set_position(sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh = {};
        exh.osage_count = s.read_int32_t();
        ex->osage_nodes_count = s.read_int32_t();
        s.read(0, 0x04);
        exh.osage_nodes_offset = s.read_uint32_t();
        exh.osage_names_offset = s.read_uint32_t();
        exh.blocks_offset = s.read_uint32_t();
        exh.bone_names_count = s.read_int32_t();
        exh.bone_names_offset = s.read_uint32_t();
        exh.osage_sibling_infos_offset = s.read_uint32_t();
        exh.cloth_count = s.read_uint32_t();
        ex->reserved[0] = s.read_uint32_t();
        ex->reserved[1] = s.read_uint32_t();
        ex->reserved[2] = s.read_uint32_t();
        ex->reserved[3] = s.read_uint32_t();
        ex->reserved[4] = s.read_uint32_t();
        ex->reserved[5] = s.read_uint32_t();
        ex->reserved[6] = s.read_uint32_t();

        if (exh.bone_names_offset) {
            std::vector<std::string> bone_names;
            bone_names.reserve(exh.bone_names_count);
            int32_t* strings_offsets = force_malloc_s(int32_t, exh.bone_names_count);
            s.set_position(exh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < exh.bone_names_count; i++)
                strings_offsets[i] = s.read_int32_t();

            size_t buf_size = 0;
            for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                if (strings_offsets[i])
                    bone_names.push_back(s.read_string_null_terminated_offset(
                        strings_offsets[i]));
                else
                    bone_names.push_back({});
                buf_size += bone_names.back().size() + 1;
            }
            free(strings_offsets);

            if (bone_names.size()) {
                ex->bone_names_buf = force_malloc_s(char, buf_size);
                ex->bone_names = force_malloc_s(char*, buf_size + 1);
                ex->bone_names_count = exh.bone_names_count;

                char* bone_names_buf = ex->bone_names_buf;
                char** bone_names_ptr = ex->bone_names;
                for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                    std::string* str = &bone_names.data()[i];
                    *bone_names_ptr++ = bone_names_buf;
                    memcpy(bone_names_buf, str->c_str(), str->size());
                    bone_names_buf[str->size()] = 0;
                    bone_names_buf += str->size() + 1;
                }
                *bone_names_ptr = 0;
            }
        }
        else {
            ex->bone_names_buf = 0;
            ex->bone_names = 0;
            ex->bone_names_count = 0;
        }

        char** bone_names = ex->bone_names;
        if (!bone_names)
            return;

        if (exh.osage_nodes_offset) {
            ex->osage_nodes = force_malloc_s(obj_skin_osage_node, ex->osage_nodes_count);

            s.set_position(exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];

                osage_node->name_index = s.read_uint32_t();
                osage_node->length = s.read_float_t();
                s.read(0, 0x04);
            }
        }

        if (exh.blocks_offset) {
            ex->blocks_count = 0;
            s.set_position(exh.blocks_offset, SEEK_SET);
            while (s.read_int32_t()) {
                s.read(0, 0x04);
                ex->blocks_count++;
            }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            s.set_position(exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                bhs[i].block_signature_offset = s.read_uint32_t();
                bhs[i].block_offset = s.read_uint32_t();
            }

            ex->blocks = force_malloc_s(obj_skin_block, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];

                std::string block_signature = s.read_string_null_terminated_offset(
                    bhs[i].block_signature_offset);
                if (block_signature.size() != 3)
                    continue;

                uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
                switch (signature) {
                case 'CLS\0':
                    block->type = OBJ_SKIN_BLOCK_CLOTH;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_cloth(&block->cloth,
                        s, bone_names);
                    break;
                case 'CNS\0':
                    block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_constraint(&block->constraint,
                        s, bone_names);
                    break;
                case 'EXP\0':
                    block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_expression(&block->expression,
                        s, bone_names);
                    break;
                case 'MOT\0':
                    block->type = OBJ_SKIN_BLOCK_MOTION;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_motion(&block->motion,
                        s, bone_names);
                    break;
                case 'OSG\0':
                    block->type = OBJ_SKIN_BLOCK_OSAGE;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_osage(&block->osage,
                        s, bone_names);
                    break;
                }
            }
            free(bhs);
        }

        if (exh.osage_sibling_infos_offset) {
            ex->osage_sibling_infos_count = 0;
            s.set_position(exh.osage_sibling_infos_offset, SEEK_SET);
            while (s.read_uint32_t()) {
                s.read(0, 0x08);
                ex->osage_sibling_infos_count++;
            }

            obj_skin_osage_sibling_info* osis = force_malloc_s(
                obj_skin_osage_sibling_info, ex->osage_sibling_infos_count);
            ex->osage_sibling_infos = osis;
            s.set_position(exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                osis[i].name_index = s.read_uint32_t();
                osis[i].sibling_name_index = s.read_uint32_t();
                osis[i].max_distance = s.read_float_t();
            }

            s.set_position(exh.osage_sibling_infos_offset, SEEK_SET);
        }

        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                continue;

            obj_skin_block_osage* osage = &block->osage;
            if (!osage->nodes)
                continue;

            int32_t count = osage->count;
            obj_skin_osage_node* child_osage_node = &ex->osage_nodes[osage->start_index];
            obj_skin_osage_node* osage_node = osage->nodes;
            for (int32_t j = 0; j < count; j++) {
                osage_node->name_index = child_osage_node->name_index;
                osage_node->length = child_osage_node->length;

                child_osage_node++;
                osage_node++;
            }
        }
    }
}

static void obj_classic_write_skin(obj* obj, stream& s, int64_t base_offset) {
    obj_skin* sk = &obj->skin;

    obj_skin_header sh = {};
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write(0x0C);
    s.align_write(0x10);

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;
    std::vector<std::string> bone_names;
    if (sk->bones) {
        sh.bone_ids_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++)
            s.write_int32_t(sk->bones[i].id);
        s.align_write(0x10);

        sh.bone_parent_ids_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++)
            s.write_int32_t(sk->bones[i].parent);
        s.align_write(0x10);

        sh.bone_names_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            s.write_int32_t(0);
            obj_skin_strings_push_back_check(strings, sk->bones[i].name);
        }
        s.align_write(0x10);

        sh.bone_matrices_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            mat4& mat = sk->bones[i].inv_bind_pose_mat;
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
        }
        s.align_write(0x10);
    }

    obj_skin_block_header* bhs = 0;
    int64_t osage_block_node = 0;
    int64_t motion_block_node_mats = 0;
    int64_t motion_block_node_name_offset = 0;
    int64_t cloth_mats = 0;
    int64_t cloth_root = 0;
    int64_t cloth_nodes = 0;
    int64_t cloth_mesh_indices = 0;
    int64_t cloth_backface_mesh_indices = 0;

    obj_skin_ex_data_header exh = {};
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = s.get_position();
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write(0x04);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write(0x1C);
        s.align_write(0x10);

        if (ex->osage_nodes_count) {
            exh.osage_nodes_offset = s.get_position();
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.align_write(0x10);

            exh.osage_sibling_infos_offset = s.get_position();
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.align_write(0x10);
        }

        exh.cloth_count = 0;
        exh.osage_count = 0;
        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            if (block->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (block->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.osage_count++;
        }

        if (exh.osage_count || exh.cloth_count) {
            exh.osage_names_offset = s.get_position();
            for (uint32_t i = 0; i < exh.osage_count; i++)
                s.write_int32_t(0);

            for (uint32_t i = 0; i < exh.cloth_count; i++)
                s.write_int32_t(0);
            s.align_write(0x10);
        }

        char** bone_names_ptr = ex->bone_names;
        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &block->cloth;
                obj_skin_strings_push_back_check(strings, cloth->mesh_name);
                obj_skin_strings_push_back_check(strings, cloth->backface_mesh_name);
                for (uint32_t k = 0; k < cloth->root_count; k++) {
                    obj_skin_block_cloth_root* sub = &cloth->root[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings, sub->bone_weights[l].bone_name);
                }

                if (cloth->skin_param_init)
                    obj_skin_strings_push_back_check(strings, cloth->skin_param.name);
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_strings_push_back_check(strings, constraint->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, constraint->name_index);
                obj_skin_strings_push_back_check(strings, constraint->source_node_name);
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings, constraint->direction.up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings, constraint->position.up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings, constraint->distance.up_vector.name);
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                for (uint32_t j = 0; j < expression->expressions_count; j++)
                    obj_skin_strings_push_back_check(strings, expression->expressions[j]);
                obj_skin_strings_push_back_check(strings, expression->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_strings_push_back_check(strings, motion->base.parent_name);
                obj_skin_strings_push_back_check(strings, motion->name);
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, motion->nodes[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                obj_skin_strings_push_back_check(bone_names, motion->name);
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, motion->nodes[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_strings_push_back_check(strings, osage->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, osage->name_index);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, osage->external_name_index);
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_osage_node* osage_node = &ex->osage_nodes[j];
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_infos = ex->osage_sibling_infos;
                    for (uint32_t k = 0; k < ex->osage_sibling_infos_count; k++) {
                        if (name_index == osage_sibling_infos->name_index) {
                            obj_skin_strings_push_back_check_by_index(strings,
                                bone_names_ptr, osage_sibling_infos->sibling_name_index);
                            break;
                        }
                        osage_sibling_infos++;
                    }
                }

                if (osage->skin_param_init)
                    obj_skin_strings_push_back_check(strings, osage->skin_param.name);
                obj_skin_strings_push_back_check(strings, "OSG");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, osage->external_name_index);
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_osage_node* osage_node = &ex->osage_nodes[j];
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_infos = ex->osage_sibling_infos;
                    for (uint32_t k = 0; k < ex->osage_sibling_infos_count; k++) {
                        if (name_index == osage_sibling_infos->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_names_ptr, osage_sibling_infos->sibling_name_index);
                            break;
                        }
                        osage_sibling_infos++;
                    }
                }

                obj_skin_osage_node* child_osage_node = &ex->osage_nodes[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, child_osage_node->name_index);

                    int32_t name_index = child_osage_node->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_infos = ex->osage_sibling_infos;
                    for (uint32_t k = 0; k < ex->osage_sibling_infos_count; k++) {
                        if (name_index == osage_sibling_infos->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_names_ptr, osage_sibling_infos->sibling_name_index);
                            break;
                        }
                        osage_sibling_infos++;
                    }
                    child_osage_node++;
                }
                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, osage->name_index);

                if (osage->skin_param_init)
                    obj_skin_strings_push_back_check(strings, osage->skin_param.name);
            } break;
            }
        }

        exh.bone_names_count = (int32_t)bone_names.size();
        exh.bone_names_offset = s.get_position();
        for (std::string& i : bone_names)
            s.write_int32_t(0);
        s.align_write(0x10);

        if (ex->blocks_count > 0) {
            exh.blocks_offset = s.get_position();
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.align_write(0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                obj_skin_block_osage* osage = &block->osage;
                if (osage->skin_param_init) {
                    obj_skin_skin_param* skin_param = &osage->skin_param;
                    if (skin_param->coli) {
                        s.write(0x28ULL * skin_param->coli_count);
                        s.align_write(0x10);
                    }
                    s.write(0x38);
                    s.align_write(0x10);
                }

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x14);
                s.write(0x14);
            }
            s.align_write(0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x2C);
            }
            s.align_write(0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                obj_skin_block_cloth* cloth = &block->cloth;
                if (cloth->skin_param_init) {
                    obj_skin_skin_param* skin_param = &cloth->skin_param;
                    if (skin_param->coli) {
                        s.write(0x28ULL * skin_param->coli_count);
                        s.align_write(0x10);
                    }
                    s.write(0x38);
                    s.align_write(0x10);
                }

                bhs[i].block_offset = s.get_position();
                s.write(0x34);
            }
            s.align_write(0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x10);
                switch (block->constraint.type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    s.write(0x0C);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    s.write(0x24);
                    s.write(0x18);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    s.write(0x24);
                    s.write(0x14);
                    s.write(0x14);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    s.write(0x24);
                    s.write(0x04);
                    s.write(0x14);
                    s.write(0x14);
                    break;
                }
            }
            s.align_write(0x10);

            int32_t motion_block_count = 0;
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x10);
                motion_block_count++;
            }
            s.align_write(0x10);

            if (exh.osage_count) {
                osage_block_node = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = &block->osage;
                    s.write(osage->nodes_count * (sizeof(uint32_t) + sizeof(float_t) * 4));
                }
                s.align_write(0x10);
            }

            if (motion_block_count) {
                motion_block_node_mats = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    s.write(motion->nodes_count * sizeof(mat4));
                }
                s.align_write(0x10);

                motion_block_node_name_offset = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    s.write(motion->nodes_count * sizeof(uint32_t));
                }
                s.align_write(0x10);
            }

            if (exh.cloth_count) {
                cloth_mats = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    if (cloth->mats)
                        s.write(cloth->mats_count * sizeof(mat4));
                }
                s.align_write(0x10);

                cloth_root = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write((sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                        * cloth->root_count * (cloth->nodes_count - 1ULL));
                }
                s.align_write(0x10);

                cloth_nodes = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write((11 * sizeof(int32_t)) * cloth->root_count * (cloth->nodes_count - 1ULL));
                }
                s.align_write(0x10);

                cloth_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write(sizeof(uint16_t) + cloth->mesh_indices_count * sizeof(uint16_t));
                }
                s.align_write(0x10);

                cloth_backface_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write(sizeof(uint16_t) + cloth->backface_mesh_indices_count * sizeof(uint16_t));
                }
                s.align_write(0x10);
            }
        }
    }

    if (sk->bones || sk->ex_data_init) {
        quicksort_string(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (std::string& i : strings) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i);
        }
    }
    s.align_write(0x10);

    std::vector<std::string> osage_names;
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        int64_t cls_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CLS");
        int64_t cns_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CNS");
        int64_t exp_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "EXP");
        int64_t mot_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "MOT");
        int64_t osg_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "OSG");

        if (ex->blocks_count > 0) {
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_cloth(&block->cloth,
                    s, strings, string_offsets, &cloth_mats, &cloth_root,
                    &cloth_nodes, &cloth_mesh_indices, &cloth_backface_mesh_indices);
                s.position_pop();
            }

            int64_t constraint_type_name_offsets[4];
            constraint_type_name_offsets[0] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Orientation");
            constraint_type_name_offsets[1] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Direction");
            constraint_type_name_offsets[2] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Position");
            constraint_type_name_offsets[3] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Distance");

            char** bone_names_ptr = ex->bone_names;
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_constraint(&block->constraint,
                    s, strings, string_offsets, bone_names_ptr, constraint_type_name_offsets);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_expression(&block->expression,
                    s, strings, string_offsets, bone_names_ptr);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_motion(&block->motion,
                    s, strings, string_offsets, bone_names_ptr,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_osage(&block->osage,
                    s, strings, string_offsets, &osage_block_node);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = &block->cloth;
                    obj_skin_strings_push_back_check(osage_names, cloth->mesh_name);
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = &block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_names_ptr, osage->external_name_index);
                }
            }

            s.position_push(exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                switch (block->type) {
                case OBJ_SKIN_BLOCK_CLOTH:
                    s.write_uint32_t((uint32_t)cls_offset);
                    s.write_uint32_t((uint32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT:
                    s.write_uint32_t((uint32_t)cns_offset);
                    s.write_uint32_t((uint32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_EXPRESSION:
                    s.write_uint32_t((uint32_t)exp_offset);
                    s.write_uint32_t((uint32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_MOTION:
                    s.write_uint32_t((uint32_t)mot_offset);
                    s.write_uint32_t((uint32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_OSAGE:
                    s.write_uint32_t((uint32_t)osg_offset);
                    s.write_uint32_t((uint32_t)bhs[i].block_offset);
                    break;
                default:
                    s.write_uint32_t(0);
                    s.write_uint32_t(0);
                    break;
                }
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.position_pop();
            free(bhs);
        }
    }

    if (sk->bones) {
        s.position_push(sh.bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            size_t str_offset = obj_skin_strings_get_string_offset(
                strings, string_offsets, sk->bones[i].name);
            s.write_uint32_t((uint32_t)str_offset);
        }
        s.position_pop();
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (ex->blocks_count > 0) {
            s.position_push(exh.bone_names_offset, SEEK_SET);
            for (std::string& i : bone_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                s.write_uint32_t((uint32_t)str_offset);
            }
            s.position_pop();

            s.position_push(exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];
                s.write_uint32_t(osage_node->name_index);
                s.write_float_t(osage_node->length);
                s.write_uint32_t(osage_node->name_index & 0x7FFF);
            }
            s.position_pop();

            s.position_push(exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_infos[i];
                s.write_uint32_t(osage_sibling_info->name_index);
                s.write_uint32_t(osage_sibling_info->sibling_name_index);
                s.write_float_t(osage_sibling_info->max_distance);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.position_pop();

            exh.osage_count = (int32_t)osage_names.size();
            exh.osage_count -= exh.cloth_count;
            s.position_push(exh.osage_names_offset, SEEK_SET);
            for (std::string& i : osage_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                s.write_uint32_t((uint32_t)str_offset);
            }
            s.position_pop();
        }

        s.position_push(sh.ex_data_offset, SEEK_SET);
        s.write_int32_t(exh.osage_count);
        s.write_int32_t(ex->osage_nodes_count);
        s.write(0x04);
        s.write_uint32_t((uint32_t)exh.osage_nodes_offset);
        s.write_uint32_t((uint32_t)exh.osage_names_offset);
        s.write_uint32_t((uint32_t)exh.blocks_offset);
        s.write_int32_t(exh.bone_names_count);
        s.write_uint32_t((uint32_t)exh.bone_names_offset);
        s.write_uint32_t((uint32_t)exh.osage_sibling_infos_offset);
        s.write_int32_t(exh.cloth_count);
        s.write_uint32_t((uint32_t)ex->reserved[0]);
        s.write_uint32_t((uint32_t)ex->reserved[1]);
        s.write_uint32_t((uint32_t)ex->reserved[2]);
        s.write_uint32_t((uint32_t)ex->reserved[3]);
        s.write_uint32_t((uint32_t)ex->reserved[4]);
        s.write_uint32_t((uint32_t)ex->reserved[5]);
        s.write_uint32_t((uint32_t)ex->reserved[6]);
        s.position_pop();
    }

    s.position_push(base_offset, SEEK_SET);
    s.write_uint32_t((uint32_t)sh.bone_ids_offset);
    s.write_uint32_t((uint32_t)sh.bone_matrices_offset);
    s.write_uint32_t((uint32_t)sh.bone_names_offset);
    s.write_uint32_t((uint32_t)sh.ex_data_offset);
    s.write_int32_t(sk->bones_count);
    s.write_uint32_t((uint32_t)sh.bone_parent_ids_offset);
    s.write(0x0C);
    s.position_pop();
}

static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, char** str) {
    uint32_t mesh_name_offset = s.read_uint32_t();
    uint32_t backface_mesh_name_offset = s.read_uint32_t();
    b->field_8 = s.read_int32_t();
    b->root_count = s.read_int32_t();
    b->nodes_count = s.read_int32_t();
    b->field_14 = s.read_int32_t();
    uint32_t mats_offset = s.read_uint32_t();
    uint32_t root_offset = s.read_uint32_t();
    uint32_t nodes_offset = s.read_uint32_t();
    uint32_t mesh_indices_offset = s.read_uint32_t();
    uint32_t backface_mesh_indices_offset = s.read_uint32_t();
    uint32_t skin_param_offset = s.read_uint32_t();
    b->reserved = s.read_uint32_t();

    b->mesh_name = s.read_utf8_string_null_terminated_offset(mesh_name_offset);
    b->backface_mesh_name = s.read_utf8_string_null_terminated_offset(backface_mesh_name_offset);

    if (mats_offset) {
        int32_t max_matrix_index = -1;

        s.position_push(root_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->root_count; i++) {
            s.read(0x28);
            for (uint32_t j = 0; j < 4; j++) {
                uint32_t name_offset = s.read_uint32_t();
                s.read(0x04);
                int32_t matrix_index = s.read_int32_t();
                s.read(0x04);
                if (name_offset && max_matrix_index < matrix_index)
                    max_matrix_index = matrix_index;
            }
        }
        s.position_pop();

        b->mats_count = max_matrix_index > -1 ? max_matrix_index + 1 : 0;
        b->mats = force_malloc_s(mat4, b->mats_count);
        s.position_push(mats_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->mats_count; i++) {
            mat4& mat = b->mats[i];
            mat.row0.x = s.read_float_t();
            mat.row1.x = s.read_float_t();
            mat.row2.x = s.read_float_t();
            mat.row3.x = s.read_float_t();
            mat.row0.y = s.read_float_t();
            mat.row1.y = s.read_float_t();
            mat.row2.y = s.read_float_t();
            mat.row3.y = s.read_float_t();
            mat.row0.z = s.read_float_t();
            mat.row1.z = s.read_float_t();
            mat.row2.z = s.read_float_t();
            mat.row3.z = s.read_float_t();
            mat.row0.w = s.read_float_t();
            mat.row1.w = s.read_float_t();
            mat.row2.w = s.read_float_t();
            mat.row3.w = s.read_float_t();
        }
        s.position_pop();
    }

    if (root_offset) {
        s.position_push(root_offset, SEEK_SET);
        b->root = force_malloc_s(obj_skin_block_cloth_root, b->root_count);
        for (uint32_t i = 0; i < b->root_count; i++) {
            obj_skin_block_cloth_root* f = &b->root[i];
            f->trans.x = s.read_float_t();
            f->trans.y = s.read_float_t();
            f->trans.z = s.read_float_t();
            f->normal.x = s.read_float_t();
            f->normal.y = s.read_float_t();
            f->normal.z = s.read_float_t();
            f->field_18 = s.read_float_t();
            f->field_1C = s.read_int32_t();
            f->field_20 = s.read_int32_t();
            f->field_24 = s.read_int32_t();

            for (uint32_t j = 0; j < 4; j++)
                obj_classic_read_skin_block_cloth_root_bone_weight(&f->bone_weights[j], s, str);
        }
        s.position_pop();
    }

    if (nodes_offset) {
        s.position_push(nodes_offset, SEEK_SET);
        b->nodes = force_malloc_s(obj_skin_block_cloth_node,
            b->root_count * (b->nodes_count - 1ULL));
        for (uint32_t i = 0; i < b->nodes_count - 1; i++)
            for (uint32_t j = 0; j < b->root_count; j++) {
                obj_skin_block_cloth_node* f = &b->nodes[i * b->root_count + j];
                f->flags = s.read_uint32_t();
                f->trans.x = s.read_float_t();
                f->trans.y = s.read_float_t();
                f->trans.z = s.read_float_t();
                f->trans_diff.x = s.read_float_t();
                f->trans_diff.y = s.read_float_t();
                f->trans_diff.z = s.read_float_t();
                f->length = s.read_float_t();
                f->field_20 = s.read_float_t();
                f->field_24 = s.read_float_t();
                f->field_28 = s.read_float_t();
            }
        s.position_pop();
    }

    if (mesh_indices_offset) {
        s.position_push(mesh_indices_offset, SEEK_SET);
        b->mesh_indices_count = s.read_uint16_t_reverse_endianness();
        b->mesh_indices = force_malloc_s(uint16_t, b->mesh_indices_count);
        s.read(b->mesh_indices, b->mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
    }

    if (backface_mesh_indices_offset) {
        s.position_push(backface_mesh_indices_offset, SEEK_SET);
        b->backface_mesh_indices_count = s.read_uint16_t_reverse_endianness();
        b->backface_mesh_indices = force_malloc_s(uint16_t, b->backface_mesh_indices_count);
        s.read(b->backface_mesh_indices, b->backface_mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
    }

    if (skin_param_offset) {
        obj_classic_read_skin_param(&b->skin_param, s, str);
        b->skin_param_init = true;
    }
    else
        b->skin_param_init = false;
}

static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    int64_t* mats_offset, int64_t* root_offset, int64_t* nodes_offset,
    int64_t* mesh_indices_offset, int64_t* backface_mesh_indices_offset) {
    int64_t skin_param_offset = 0;
    if (b->skin_param_init) {
        skin_param_offset = s.get_position();
        obj_classic_write_skin_param(&b->skin_param, s, strings, string_offsets);
    }

    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->mesh_name);
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->backface_mesh_name);

    s.write_uint32_t((uint32_t)mesh_name_offset);
    s.write_uint32_t((uint32_t)backface_mesh_name_offset);
    s.write_int32_t(b->field_8);
    s.write_int32_t(b->root_count);
    s.write_int32_t(b->nodes_count);
    s.write_int32_t(b->field_14);
    s.write_uint32_t((uint32_t)*mats_offset);
    s.write_uint32_t((uint32_t)*root_offset);
    s.write_uint32_t((uint32_t)*nodes_offset);
    s.write_uint32_t((uint32_t)*mesh_indices_offset);
    s.write_uint32_t((uint32_t)*backface_mesh_indices_offset);
    s.write_uint32_t((uint32_t)skin_param_offset);
    s.write_uint32_t(b->reserved);

    if (b->mats) {
        s.position_push(*mats_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->mats_count; i++) {
            mat4& mat = b->mats[i];
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
            *mats_offset += sizeof(mat4);
        }
        s.position_pop();
    }

    if (b->root) {
        s.position_push(*root_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->root_count; i++) {
            obj_skin_block_cloth_root* f = &b->root[i];
            s.write_float_t(f->trans.x);
            s.write_float_t(f->trans.y);
            s.write_float_t(f->trans.z);
            s.write_float_t(f->normal.x);
            s.write_float_t(f->normal.y);
            s.write_float_t(f->normal.z);
            s.write_float_t(f->field_18);
            s.write_int32_t(f->field_1C);
            s.write_int32_t(f->field_20);
            s.write_int32_t(f->field_24);

            for (uint32_t j = 0; j < 4; j++)
                obj_classic_write_skin_block_cloth_root_bone_weight(&f->bone_weights[j],
                    s, strings, string_offsets);
        }
        s.position_pop();
        *root_offset += (sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
            * b->root_count * (b->nodes_count - 1ULL);
    }

    if (b->nodes) {
        s.position_push(*nodes_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count - 1; i++)
            for (uint32_t j = 0; j < b->root_count; j++) {
                obj_skin_block_cloth_node* f = &b->nodes[i * b->root_count + j];
                s.write_uint32_t(f->flags);
                s.write_float_t(f->trans.x);
                s.write_float_t(f->trans.y);
                s.write_float_t(f->trans.z);
                s.write_float_t(f->trans_diff.x);
                s.write_float_t(f->trans_diff.y);
                s.write_float_t(f->trans_diff.z);
                s.write_float_t(f->length);
                s.write_float_t(f->field_20);
                s.write_float_t(f->field_24);
                s.write_float_t(f->field_24);
            }
        s.position_pop();
        *nodes_offset += (11 * sizeof(int32_t)) * b->root_count * (b->nodes_count - 1ULL);
    }

    if (b->mesh_indices) {
        s.position_push(*mesh_indices_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)b->mesh_indices_count);
        s.write(b->mesh_indices, b->mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
        *mesh_indices_offset += sizeof(uint16_t) + b->mesh_indices_count * sizeof(uint16_t);
    }

    if (b->backface_mesh_indices) {
        s.position_push(*backface_mesh_indices_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)b->backface_mesh_indices_count);
        s.write(b->backface_mesh_indices, b->backface_mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
        *backface_mesh_indices_offset += sizeof(uint16_t) + b->backface_mesh_indices_count * sizeof(uint16_t);
    }
}

static void obj_classic_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, char** str) {
    int32_t bone_name_offset = s.read_uint32_t();
    sub->bone_name = s.read_utf8_string_null_terminated_offset(bone_name_offset);
    sub->weight = s.read_float_t();
    sub->matrix_index = s.read_uint32_t();
    sub->reserved = s.read_uint32_t();
}

static void obj_classic_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, sub->bone_name);
    s.write_uint32_t((uint32_t)bone_name_offset);
    s.write_float_t(sub->weight);
    s.write_uint32_t(sub->matrix_index);
    s.write_uint32_t(sub->reserved);
}

static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    uint32_t type_offset = s.read_uint32_t();
    char* type = s.read_utf8_string_null_terminated_offset(type_offset);

    uint32_t name_offset = s.read_uint32_t();
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->coupling = (obj_skin_block_constraint_coupling)s.read_int32_t();

    uint32_t source_node_name_offset = s.read_uint32_t();
    b->source_node_name = s.read_utf8_string_null_terminated_offset(source_node_name_offset);

    if (!str_utils_compare(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = s.read_float_t();
        b->orientation.offset.y = s.read_float_t();
        b->orientation.offset.z = s.read_float_t();
    }
    else if (!str_utils_compare(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_classic_read_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, str);
        b->direction.align_axis.x = s.read_float_t();
        b->direction.align_axis.y = s.read_float_t();
        b->direction.align_axis.z = s.read_float_t();
        b->direction.target_offset.x = s.read_float_t();
        b->direction.target_offset.y = s.read_float_t();
        b->direction.target_offset.z = s.read_float_t();
    }
    else if (!str_utils_compare(type, "Position")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        obj_classic_read_skin_block_constraint_up_vector_old(&b->position.up_vector,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, str);
    }
    else if (!str_utils_compare(type, "Distance")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        obj_classic_read_skin_block_constraint_up_vector_old(&b->distance.up_vector,
            s, str);
        b->distance.distance = s.read_float_t();
        obj_classic_read_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, str);
    }
    else
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    free(type);
}

static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* offsets) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    int64_t type_offset = 0;
    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        type_offset = offsets[0];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        type_offset = offsets[1];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        type_offset = offsets[2];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        type_offset = offsets[3];
        break;
    }
    s.write_uint32_t((uint32_t)type_offset);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    s.write_uint32_t((uint32_t)name_offset);

    s.write_int32_t(b->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->source_node_name);
    s.write_uint32_t((uint32_t)source_node_name_offset);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        s.write_float_t(b->orientation.offset.x);
        s.write_float_t(b->orientation.offset.y);
        s.write_float_t(b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_classic_write_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, strings, string_offsets);
        s.write_float_t(b->direction.align_axis.x);
        s.write_float_t(b->direction.align_axis.y);
        s.write_float_t(b->direction.align_axis.z);
        s.write_float_t(b->direction.target_offset.x);
        s.write_float_t(b->direction.target_offset.y);
        s.write_float_t(b->direction.target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_classic_write_skin_block_constraint_up_vector_old(&b->position.up_vector,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, strings, string_offsets);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_classic_write_skin_block_constraint_up_vector_old(&b->distance.up_vector,
            s, strings, string_offsets);
        s.write_float_t(b->distance.distance);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets);
        break;
    }
}

static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, char** str) {
    ap->affected_by_orientation = s.read_int32_t() != 0;
    ap->affected_by_scaling = s.read_int32_t() != 0;
    ap->offset.x = s.read_float_t();
    ap->offset.y = s.read_float_t();
    ap->offset.z = s.read_float_t();
}

static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    s.write_int32_t(ap->affected_by_orientation ? 1 : 0);
    s.write_int32_t(ap->affected_by_scaling ? 1 : 0);
    s.write_float_t(ap->offset.x);
    s.write_float_t(ap->offset.y);
    s.write_float_t(ap->offset.z);
}

static void obj_classic_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, char** str) {
    up->active = s.read_int32_t() != 0;
    up->roll = s.read_float_t();
    up->affected_axis.x = s.read_float_t();
    up->affected_axis.y = s.read_float_t();
    up->affected_axis.z = s.read_float_t();
    up->point_at.x = s.read_float_t();
    up->point_at.y = s.read_float_t();
    up->point_at.z = s.read_float_t();

    uint32_t name_offset = s.read_uint32_t();
    up->name = s.read_utf8_string_null_terminated_offset(name_offset);
}

static void obj_classic_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    s.write_int32_t(up->active ? 1 : 0);
    s.write_float_t(up->roll);
    s.write_float_t(up->affected_axis.x);
    s.write_float_t(up->affected_axis.y);
    s.write_float_t(up->affected_axis.z);
    s.write_float_t(up->point_at.x);
    s.write_float_t(up->point_at.y);
    s.write_float_t(up->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, up->name);
    s.write_uint32_t((uint32_t)name_offset);
}

static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream& s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    uint32_t name_offset = s.read_uint32_t();
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->expressions_count = s.read_int32_t();
    b->expressions_count = min(b->expressions_count, 9);
    for (uint32_t i = 0; i < b->expressions_count; i++) {
        uint32_t expression_offset = s.read_uint32_t();
        if (expression_offset)
            b->expressions[i] = s.read_utf8_string_null_terminated_offset(expression_offset);
    }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        b->expressions[i] = 0;
}

static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    s.write_uint32_t((uint32_t)name_offset);

    s.write_int32_t(min(b->expressions_count, 9));
    for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
        int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, b->expressions[i]);
        s.write_uint32_t((uint32_t)expression_offset);
    }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        s.write_int32_t(0);
}

static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream& s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    uint32_t name_offset = s.read_uint32_t();
    b->nodes_count = s.read_int32_t();
    uint32_t bone_names_offset = s.read_uint32_t();
    uint32_t bone_matrices_offset = s.read_uint32_t();

    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->nodes = 0;

    if (!b->nodes_count)
        return;

    b->nodes = force_malloc_s(obj_skin_motion_node, b->nodes_count);

    if (bone_names_offset) {
        s.position_push(bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            b->nodes[i].name_index = s.read_uint32_t();
        s.position_pop();
    }

    if (bone_matrices_offset) {
        s.position_push(bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4& mat = b->nodes[i].inv_bind_pose_mat;
            mat.row0.x = s.read_float_t();
            mat.row1.x = s.read_float_t();
            mat.row2.x = s.read_float_t();
            mat.row3.x = s.read_float_t();
            mat.row0.y = s.read_float_t();
            mat.row1.y = s.read_float_t();
            mat.row2.y = s.read_float_t();
            mat.row3.y = s.read_float_t();
            mat.row0.z = s.read_float_t();
            mat.row1.z = s.read_float_t();
            mat.row2.z = s.read_float_t();
            mat.row3.z = s.read_float_t();
            mat.row0.w = s.read_float_t();
            mat.row1.w = s.read_float_t();
            mat.row2.w = s.read_float_t();
            mat.row3.w = s.read_float_t();
        }
        s.position_pop();
    }
}

static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    s.write_uint32_t((uint32_t)name_offset);
    s.write_int32_t(b->nodes_count);
    s.write_uint32_t((uint32_t)*bone_names_offset);
    s.write_uint32_t((uint32_t)*bone_matrices_offset);

    if (b->nodes) {
        s.position_push(*bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            s.write_uint32_t(b->nodes[i].name_index);
        s.position_pop();
        *bone_names_offset += b->nodes_count * sizeof(uint32_t);

        s.position_push(*bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4& mat = b->nodes[i].inv_bind_pose_mat;
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
        }
        s.position_pop();
        *bone_matrices_offset += b->nodes_count * sizeof(mat4);
    }
}

static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream& s, char** str) {
    uint32_t parent_name_offset = s.read_uint32_t();
    b->parent_name = s.read_utf8_string_null_terminated_offset(parent_name_offset);

    b->position.x = s.read_float_t();
    b->position.y = s.read_float_t();
    b->position.z = s.read_float_t();
    b->rotation.x = s.read_float_t();
    b->rotation.y = s.read_float_t();
    b->rotation.z = s.read_float_t();
    b->scale.x = s.read_float_t();
    b->scale.y = s.read_float_t();
    b->scale.z = s.read_float_t();
}

static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->parent_name);
    s.write_uint32_t((uint32_t)parent_name_offset);

    s.write_float_t(b->position.x);
    s.write_float_t(b->position.y);
    s.write_float_t(b->position.z);
    s.write_float_t(b->rotation.x);
    s.write_float_t(b->rotation.y);
    s.write_float_t(b->rotation.z);
    s.write_float_t(b->scale.x);
    s.write_float_t(b->scale.y);
    s.write_float_t(b->scale.z);
}

static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream& s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    b->start_index = s.read_int32_t();
    b->count = s.read_int32_t();
    b->external_name_index = s.read_uint32_t();
    b->name_index = s.read_uint32_t();

    int32_t offset = s.read_int32_t();
    s.read(0, 0x14);

    b->nodes = 0;

    if (!b->count || !offset)
        return;

    b->nodes_count = b->count;
    b->nodes = force_malloc_s(obj_skin_osage_node, b->count);

    s.position_push(offset, SEEK_SET);
    if (~s.read_uint32_t() & 0x8000) {
        obj_classic_read_skin_param(&b->skin_param, s, str);
        b->skin_param_init = true;
    }
    else {
        s.set_position(offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            obj_skin_osage_node* node = &b->nodes[i];
            node->name_index = s.read_uint32_t();
            node->length = s.read_float_t();
            node->rotation.x = s.read_float_t();
            node->rotation.y = s.read_float_t();
            node->rotation.z = s.read_float_t();
        }
        b->skin_param_init = false;
    }
    s.position_pop();
}

static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, int64_t* nodes_offset) {
    int64_t skin_param_offset = 0;
    if (b->skin_param_init) {
        skin_param_offset = s.get_position();
        obj_classic_write_skin_param(&b->skin_param, s, strings, string_offsets);
    }

    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    s.write_int32_t(b->start_index);
    s.write_int32_t(b->count);
    s.write_uint32_t(b->external_name_index);
    s.write_uint32_t(b->name_index);

    if (b->skin_param_init)
        s.write_uint32_t((uint32_t)skin_param_offset);
    else
        s.write_uint32_t((uint32_t)*nodes_offset);
    s.write(0x14);

    if (!b->skin_param_init && b->nodes) {
        s.position_push(*nodes_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            obj_skin_osage_node* node = &b->nodes[i];
            s.write_uint32_t(node->name_index);
            s.write_float_t(node->length);
            s.write_float_t(node->rotation.x);
            s.write_float_t(node->rotation.y);
            s.write_float_t(node->rotation.z);
        }
        s.position_pop();
        *nodes_offset += b->nodes_count * (sizeof(uint32_t) + sizeof(float_t) * 4);
    }
}

static void obj_classic_read_skin_param(obj_skin_skin_param* skp,
    stream& s, char** str) {
    skp->coli = 0;
    skp->coli_count = 0;

    skp->unk0 = s.read_int32_t();
    skp->force = s.read_float_t();
    skp->force_gain = s.read_float_t();
    skp->air_res = s.read_float_t();
    skp->rot_y = s.read_float_t();
    skp->rot_z = s.read_float_t();
    skp->hinge_y = s.read_float_t();
    skp->hinge_z = s.read_float_t();
    uint32_t name_offset = s.read_uint32_t();
    uint32_t coli_offset = s.read_uint32_t();
    skp->coli_r = s.read_float_t();
    skp->friction = s.read_float_t();
    skp->wind_afc = s.read_float_t();
    skp->unk44 = s.read_int32_t();

    skp->name = s.read_utf8_string_null_terminated_offset(name_offset);

    if (coli_offset) {
        s.set_position(coli_offset, SEEK_SET);
        int32_t coli_count = 0;
        while (s.read_int32_t()) {
            s.read(44);
            coli_count++;
        }

        skp->coli = force_malloc_s(obj_skin_skin_param_coli, coli_count);
        skp->coli_count = coli_count;

        s.set_position(coli_offset, SEEK_SET);
        for (int32_t i = 0; i < skp->coli_count; i++) {
            obj_skin_skin_param_coli* coli = &skp->coli[i];
            coli->type = (obj_skin_skin_param_coli_type)s.read_int32_t();
            coli->bone0_index = s.read_int32_t();
            coli->bone1_index = s.read_int32_t();
            coli->radius = s.read_float_t();
            coli->bone0_pos.x = s.read_float_t();
            coli->bone0_pos.y = s.read_float_t();
            coli->bone0_pos.z = s.read_float_t();
            coli->bone1_pos.x = s.read_float_t();
            coli->bone1_pos.y = s.read_float_t();
            coli->bone1_pos.z = s.read_float_t();
        }
    }
}

static void obj_classic_write_skin_param(obj_skin_skin_param* skp,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    int64_t coli_offset = 0;
    if (skp->coli) {
        coli_offset = s.get_position();
        for (int32_t i = 0; i < skp->coli_count; i++) {
            obj_skin_skin_param_coli* coli = &skp->coli[i];
            s.write_int32_t(coli->type);
            s.write_int32_t(coli->bone0_index);
            s.write_int32_t(coli->bone1_index);
            s.write_float_t(coli->radius);
            s.write_float_t(coli->bone0_pos.x);
            s.write_float_t(coli->bone0_pos.y);
            s.write_float_t(coli->bone0_pos.z);
            s.write_float_t(coli->bone1_pos.x);
            s.write_float_t(coli->bone1_pos.y);
            s.write_float_t(coli->bone1_pos.z);
        }
        s.align_write(0x10);
    }

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, skp->name);

    s.write_int32_t(skp->unk0);
    s.write_float_t(skp->force);
    s.write_float_t(skp->force_gain);
    s.write_float_t(skp->air_res);
    s.write_float_t(skp->rot_y);
    s.write_float_t(skp->rot_z);
    s.write_float_t(skp->hinge_y);
    s.write_float_t(skp->hinge_z);
    s.write_uint32_t((uint32_t)name_offset);
    s.write_uint32_t((uint32_t)coli_offset);
    s.write_float_t(skp->coli_r);
    s.write_float_t(skp->friction);
    s.write_float_t(skp->wind_afc);
    s.write_int32_t(skp->unk44);
    s.write(0x08);
}

static void obj_classic_read_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t num_vertex, obj_vertex_format_file vertex_format_file) {
    obj_vertex_format vertex_format = OBJ_VERTEX_NONE;
    if (vertex_format_file & OBJ_VERTEX_FILE_POSITION)
        enum_or(vertex_format, OBJ_VERTEX_POSITION);

    if (vertex_format_file & OBJ_VERTEX_FILE_NORMAL)
        enum_or(vertex_format, OBJ_VERTEX_NORMAL);

    if (vertex_format_file & OBJ_VERTEX_FILE_TANGENT)
        enum_or(vertex_format, OBJ_VERTEX_TANGENT);

    if (vertex_format_file & OBJ_VERTEX_FILE_BINORMAL)
        enum_or(vertex_format, OBJ_VERTEX_BINORMAL);

    if (vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD0)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD0);

    if (vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD1)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD1);

    if (vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD2)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD2);

    if (vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD3)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD3);

    if (vertex_format_file & OBJ_VERTEX_FILE_COLOR0)
        enum_or(vertex_format, OBJ_VERTEX_COLOR0);

    if (vertex_format_file & OBJ_VERTEX_FILE_COLOR1)
        enum_or(vertex_format, OBJ_VERTEX_COLOR1);

    if ((vertex_format_file & (OBJ_VERTEX_FILE_BONE_WEIGHT | OBJ_VERTEX_FILE_BONE_INDEX))
        == (OBJ_VERTEX_FILE_BONE_WEIGHT | OBJ_VERTEX_FILE_BONE_INDEX))
        enum_or(vertex_format, OBJ_VERTEX_BONE_DATA);
    else
        enum_and(vertex_format_file, ~(OBJ_VERTEX_FILE_BONE_WEIGHT | OBJ_VERTEX_FILE_BONE_INDEX));

    if (vertex_format_file & OBJ_VERTEX_FILE_UNKNOWN)
        enum_or(vertex_format, OBJ_VERTEX_UNKNOWN);

    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, num_vertex);
    for (uint32_t i = 0; i < 20; i++) {
        obj_vertex_format_file attribute = (obj_vertex_format_file)(1 << i);
        if (~vertex_format_file & attribute)
            continue;

        s.set_position(base_offset + vertex[i], SEEK_SET);
        switch (attribute) {
        case OBJ_VERTEX_FILE_POSITION:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].position.x = s.read_float_t();
                vtx[j].position.y = s.read_float_t();
                vtx[j].position.z = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_NORMAL:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].normal.x = s.read_float_t();
                vtx[j].normal.y = s.read_float_t();
                vtx[j].normal.z = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_TANGENT:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].tangent.x = s.read_float_t();
                vtx[j].tangent.y = s.read_float_t();
                vtx[j].tangent.z = s.read_float_t();
                vtx[j].tangent.w = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_BINORMAL:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].binormal.x = s.read_float_t();
                vtx[j].binormal.y = s.read_float_t();
                vtx[j].binormal.z = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_TEXCOORD0:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord0.x = s.read_float_t();
                vtx[j].texcoord0.y = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_TEXCOORD1:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord1.x = s.read_float_t();
                vtx[j].texcoord1.y = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_TEXCOORD2:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord2.x = s.read_float_t();
                vtx[j].texcoord2.y = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_TEXCOORD3:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord3.x = s.read_float_t();
                vtx[j].texcoord3.y = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_COLOR0:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].color0.x = s.read_float_t();
                vtx[j].color0.y = s.read_float_t();
                vtx[j].color0.z = s.read_float_t();
                vtx[j].color0.w = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_COLOR1:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].color1.x = s.read_float_t();
                vtx[j].color1.y = s.read_float_t();
                vtx[j].color1.z = s.read_float_t();
                vtx[j].color1.w = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_BONE_WEIGHT:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].bone_weight.x = s.read_float_t();
                vtx[j].bone_weight.y = s.read_float_t();
                vtx[j].bone_weight.z = s.read_float_t();
                vtx[j].bone_weight.w = s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_BONE_INDEX:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].bone_index.x = (int32_t)s.read_float_t();
                vtx[j].bone_index.y = (int32_t)s.read_float_t();
                vtx[j].bone_index.z = (int32_t)s.read_float_t();
                vtx[j].bone_index.w = (int32_t)s.read_float_t();
            }
            break;
        case OBJ_VERTEX_FILE_UNKNOWN:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].unknown.x = s.read_float_t();
                vtx[j].unknown.y = s.read_float_t();
                vtx[j].unknown.z = s.read_float_t();
                vtx[j].unknown.w = s.read_float_t();
            }
            break;
        }
    }
    mesh->vertex = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
}

static void obj_classic_write_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t* num_vertex, obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex) {
    obj_vertex_data* vtx = mesh->vertex;
    uint32_t _num_vertex = mesh->num_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    obj_vertex_format_file _vertex_format_file = (obj_vertex_format_file)0;
    uint32_t _size_vertex = 0;
    if (vertex_format & OBJ_VERTEX_POSITION) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_POSITION);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_NORMAL) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_NORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_TANGENT) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_TANGENT);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_BINORMAL) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_BINORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_TEXCOORD0);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_TEXCOORD1);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_TEXCOORD2);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_TEXCOORD3);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_COLOR0) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_COLOR0);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_COLOR1) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_COLOR1);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_BONE_WEIGHT | OBJ_VERTEX_FILE_BONE_INDEX);
        _size_vertex += 0x20;
    }

    if (vertex_format & OBJ_VERTEX_UNKNOWN) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FILE_UNKNOWN);
        _size_vertex += 0x10;
    }
    *num_vertex = _num_vertex;
    *vertex_format_file = _vertex_format_file;
    *size_vertex = _size_vertex;

    if (_vertex_format_file & OBJ_VERTEX_FILE_POSITION) {
        vertex[0] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].position.x);
            s.write_float_t(vtx[i].position.y);
            s.write_float_t(vtx[i].position.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_NORMAL) {
        vertex[1] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].normal.x);
            s.write_float_t(vtx[i].normal.y);
            s.write_float_t(vtx[i].normal.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_TANGENT) {
        vertex[2] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].tangent.x);
            s.write_float_t(vtx[i].tangent.y);
            s.write_float_t(vtx[i].tangent.z);
            s.write_float_t(vtx[i].tangent.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_BINORMAL) {
        vertex[3] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].binormal.x);
            s.write_float_t(vtx[i].binormal.y);
            s.write_float_t(vtx[i].binormal.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD0) {
        vertex[4] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord0.x);
            s.write_float_t(vtx[i].texcoord0.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_BONE_WEIGHT) {
        vertex[10] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].bone_weight.x);
            s.write_float_t(vtx[i].bone_weight.y);
            s.write_float_t(vtx[i].bone_weight.z);
            s.write_float_t(vtx[i].bone_weight.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_BONE_INDEX) {
        vertex[11] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t((float_t)vtx[i].bone_index.x);
            s.write_float_t((float_t)vtx[i].bone_index.y);
            s.write_float_t((float_t)vtx[i].bone_index.z);
            s.write_float_t((float_t)vtx[i].bone_index.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD1) {
        vertex[5] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord1.x);
            s.write_float_t(vtx[i].texcoord1.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD2) {
        vertex[6] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord2.x);
            s.write_float_t(vtx[i].texcoord2.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_TEXCOORD3) {
        vertex[7] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].texcoord3.x);
            s.write_float_t(vtx[i].texcoord3.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_COLOR0) {
        vertex[8] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].color0.x);
            s.write_float_t(vtx[i].color0.y);
            s.write_float_t(vtx[i].color0.z);
            s.write_float_t(vtx[i].color0.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_COLOR1) {
        vertex[9] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].color1.x);
            s.write_float_t(vtx[i].color1.y);
            s.write_float_t(vtx[i].color1.z);
            s.write_float_t(vtx[i].color1.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FILE_UNKNOWN) {
        vertex[12] = s.get_position() - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            s.write_float_t(vtx[i].unknown.x);
            s.write_float_t(vtx[i].unknown.y);
            s.write_float_t(vtx[i].unknown.z);
            s.write_float_t(vtx[i].unknown.w);
        }
    }
}

static void obj_set_modern_read_inner(obj_set* os, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOSD') || !st.data.size())
        return;

    stream s_mosd;
    s_mosd.open(st.data);
    s_mosd.is_big_endian = st.header.use_big_endian;

    uint32_t version = s_mosd.read_uint32_t_reverse_endianness();
    if (version != 0x05062501) {
        os->is_x = false;
        os->modern = false;
        os->ready = false;
        return;
    }

    bool is_x = true;
    s_mosd.set_position(0x0C, SEEK_SET);
    is_x &= s_mosd.read_uint32_t_reverse_endianness() == 0;
    s_mosd.set_position(0x14, SEEK_SET);
    is_x &= s_mosd.read_uint32_t_reverse_endianness() == 0;
    s_mosd.set_position(0x1C, SEEK_SET);
    is_x &= s_mosd.read_uint32_t_reverse_endianness() == 0;
    s_mosd.set_position(0x24, SEEK_SET);
    is_x &= s_mosd.read_uint32_t_reverse_endianness() == 0;
    s_mosd.set_position(0x2C, SEEK_SET);
    is_x &= s_mosd.read_uint32_t_reverse_endianness() == 0;

    s_mosd.set_position(0x04, SEEK_SET);
    obj_set_header osh = {};
    if (!is_x) {
        os->obj_num = s_mosd.read_int32_t_reverse_endianness();
        osh.last_obj_id = s_mosd.read_int32_t_reverse_endianness();
        osh.obj_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_skin_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_name_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_id_data = s_mosd.read_offset_f2(st.header.length);
        osh.tex_id_data = s_mosd.read_offset_f2(st.header.length);
        os->tex_id_num = s_mosd.read_int32_t_reverse_endianness();
        os->reserved[0] = s_mosd.read_uint32_t_reverse_endianness();
        os->reserved[1] = s_mosd.read_uint32_t_reverse_endianness();
    }
    else {
        os->obj_num = s_mosd.read_int32_t_reverse_endianness();
        osh.last_obj_id = s_mosd.read_int32_t_reverse_endianness();
        osh.obj_data = s_mosd.read_offset_x();
        osh.obj_skin_data = s_mosd.read_offset_x();
        osh.obj_name_data = s_mosd.read_offset_x();
        osh.obj_id_data = s_mosd.read_offset_x();
        osh.tex_id_data = s_mosd.read_offset_x();
        os->tex_id_num = s_mosd.read_int32_t_reverse_endianness();
        os->reserved[0] = s_mosd.read_uint32_t_reverse_endianness();
        os->reserved[1] = s_mosd.read_uint32_t_reverse_endianness();
    }

    os->obj_data = force_malloc_s(obj, os->obj_num);

    int64_t* obj_datas = 0;
    if (osh.obj_data) {
        obj_datas = force_malloc_s(int64_t, os->obj_num);
        s_mosd.set_position(osh.obj_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_datas[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_datas[i] = s_mosd.read_offset_x();
    }

    int64_t* obj_skin_datas = 0;
    if (osh.obj_skin_data) {
        obj_skin_datas = force_malloc_s(int64_t, os->obj_num);
        s_mosd.set_position(osh.obj_skin_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_skin_datas[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_skin_datas[i] = s_mosd.read_offset_x();
    }

    int64_t* obj_name_datas = 0;
    if (osh.obj_name_data) {
        obj_name_datas = force_malloc_s(int64_t, os->obj_num);
        s_mosd.set_position(osh.obj_name_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_name_datas[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_name_datas[i] = s_mosd.read_offset_x();
    }

    if (osh.obj_data)
        for (uint32_t i = 0; i < os->obj_num; i++) {
            obj* obj = &os->obj_data[i];
            if (osh.obj_name_data && obj_name_datas[i]) {
                obj->name = s_mosd.read_utf8_string_null_terminated_offset(obj_name_datas[i]);
                obj->hash = hash_utf8_murmurhash(obj->name);
            }
            else
                obj->hash = hash_murmurhash_empty;
        }

    if (osh.obj_id_data) {
        s_mosd.set_position(osh.obj_id_data, SEEK_SET);
        for (uint32_t i = 0; i < os->obj_num; i++)
            os->obj_data[i].id = s_mosd.read_uint32_t_reverse_endianness();
    }

    free(obj_datas);
    free(obj_skin_datas);
    free(obj_name_datas);

    if (osh.tex_id_data) {
        s_mosd.set_position(osh.tex_id_data, SEEK_SET);
        os->tex_id_data = force_malloc_s(uint32_t, os->tex_id_num);
        for (uint32_t i = 0; i < os->tex_id_num; i++)
            os->tex_id_data[i] = s_mosd.read_uint32_t_reverse_endianness();
    }

    int32_t omdl_index = 0;
    for (f2_struct& i : st.sub_structs) {
        if (i.header.signature != reverse_endianness_uint32_t('OMDL'))
            continue;

        f2_struct* oskn = 0;
        f2_struct* oidx = 0;
        f2_struct* ovtx = 0;
        for (f2_struct& j : i.sub_structs)
            if (!oskn && j.header.signature == reverse_endianness_uint32_t('OSKN'))
                oskn = &j;
            else if (!oidx && j.header.signature == reverse_endianness_uint32_t('OIDX'))
                oidx = &j;
            else if (!ovtx && j.header.signature == reverse_endianness_uint32_t('OVTX'))
                ovtx = &j;

        stream s_oskn;
        stream s_oidx;
        stream s_ovtx;
        stream* s_oskn_ptr = 0;
        stream* s_oidx_ptr = 0;
        stream* s_ovtx_ptr = 0;
        if (oskn) {
            s_oskn.open(oskn->data);
            s_oskn.is_big_endian = oskn->header.use_big_endian;
            s_oskn_ptr = &s_oskn;
        }

        if (oidx) {
            s_oidx.open(oidx->data);
            s_oidx.is_big_endian = oidx->header.use_big_endian;
            s_oidx_ptr = &s_oidx;
        }

        if (ovtx) {
            s_ovtx.open(ovtx->data);
            s_ovtx.is_big_endian = ovtx->header.use_big_endian;
            s_ovtx_ptr = &s_ovtx;
        }

        obj* obj = &os->obj_data[omdl_index];
        stream s_omdl;
        s_omdl.open(i.data);
        s_omdl.is_big_endian = i.header.use_big_endian;
        obj_modern_read_model(obj, s_omdl, 0, i.header.length, is_x, s_oidx_ptr, s_ovtx_ptr);
        s_omdl.close();

        if (s_oskn_ptr)
            obj_modern_read_skin(obj, *s_oskn_ptr, 0, oskn->header.length, is_x);
        omdl_index++;
    }

    os->is_x = is_x;
    os->modern = true;
    os->ready = true;
}

static void obj_set_modern_write_inner(obj_set* os, stream& s) {
    stream s_mosd;
    s_mosd.open();
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = os->is_x;

    obj_set_header osh = {};
    osh.last_obj_id = -1;

    uint32_t obj_num = os->obj_num;

    if (!is_x) {
        ee = { 0, 1, 44, 1 };
        ee.append(0, 9, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 44;
    }
    else {
        ee = { 0, 3, 72, 1 };
        ee.append(0, 3, ENRS_DWORD);
        ee.append(4, 5, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 72;
        off = align_val(off, 0x10);
    }

    if (!is_x)
        off += (uint32_t)(obj_num * 4ULL);
    else
        off += (uint32_t)(obj_num * 8ULL);
    off = align_val(off, 0x10);

    if (!is_x)
        off += (uint32_t)(obj_num * 4ULL);
    else
        off += (uint32_t)(obj_num * 8ULL);
    off = align_val(off, 0x10);

    if (!is_x) {
        ee = { off, 1, 4, (uint32_t)obj_num };
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj_num * 4ULL);
    }
    else {
        ee = { off, 1, 8, (uint32_t)obj_num };
        ee.append(0, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj_num * 8ULL);
    }
    off = align_val(off, 0x10);

    ee = { off, 1, 4, (uint32_t)obj_num };
    ee.append(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    off = (uint32_t)(obj_num * 4ULL);
    off = align_val(off, 0x10);

    ee = { off, 1, 4, (uint32_t)os->tex_id_num };
    ee.append(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    off = (uint32_t)(os->tex_id_num * 4ULL);
    off = align_val(off, 0x10);

    if (!is_x) {
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
    }
    else {
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        io_write_offset_x_pof_add(s_mosd, 0, &pof);
        io_write_offset_x_pof_add(s_mosd, 0, &pof);
        io_write_offset_x_pof_add(s_mosd, 0, &pof);
        io_write_offset_x_pof_add(s_mosd, 0, &pof);
        io_write_offset_x_pof_add(s_mosd, 0, &pof);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        s_mosd.write_int32_t(0);
        s_mosd.align_write(0x10);
    }

    osh.obj_data = s_mosd.get_position();
    if (!is_x)
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_f2(0, 0x20);
    else
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_x(0);
    s_mosd.align_write(0x10);

    osh.obj_skin_data = s_mosd.get_position();
    if (!is_x)
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_f2(0, 0x20);
    else
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_x(0);
    s_mosd.align_write(0x10);

    osh.obj_name_data = s_mosd.get_position();
    if (!is_x)
        for (uint32_t i = 0; i < obj_num; i++)
            io_write_offset_f2_pof_add(s_mosd, 0, 0x20, &pof);
    else
        for (uint32_t i = 0; i < obj_num; i++)
            io_write_offset_x_pof_add(s_mosd, 0, &pof);
    s_mosd.align_write(0x10);

    osh.obj_id_data = s_mosd.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s_mosd.write_uint32_t(os->obj_data[i].id);
    s_mosd.align_write(0x10);

    osh.tex_id_data = s_mosd.get_position();
    for (uint32_t i = 0; i < os->tex_id_num; i++)
        s_mosd.write_uint32_t(os->tex_id_data[i]);
    s_mosd.align_write(0x10);

    int64_t* obj_name_datas = force_malloc_s(int64_t, obj_num);
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_name_datas[i] = (int32_t)s_mosd.get_position();
        s_mosd.write_utf8_string_null_terminated(os->obj_data[i].name);
    }
    s_mosd.align_write(0x10);

    s_mosd.position_push(osh.obj_name_data, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_f2(obj_name_datas[i], 0x20);
    else
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_x(obj_name_datas[i]);
    s_mosd.position_pop();
    free(obj_name_datas);

    s_mosd.position_push(0x00, SEEK_SET);
    if (!is_x) {
        s_mosd.write_uint32_t(0x05062501);
        s_mosd.write_int32_t(os->obj_num);
        s_mosd.write_int32_t(-1);
        s_mosd.write_offset_f2(osh.obj_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_skin_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_name_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_id_data, 0x20);
        s_mosd.write_offset_f2(osh.tex_id_data, 0x20);
        s_mosd.write_int32_t(os->tex_id_num);
        s_mosd.write_uint32_t(os->reserved[0]);
        s_mosd.write_uint32_t(os->reserved[1]);
    }
    else {
        s_mosd.write_uint32_t(0x05062501);
        s_mosd.write_int32_t(os->obj_num);
        s_mosd.write_int32_t(-1);
        s_mosd.write_offset_x(osh.obj_data);
        s_mosd.write_offset_x(osh.obj_skin_data);
        s_mosd.write_offset_x(osh.obj_name_data);
        s_mosd.write_offset_x(osh.obj_id_data);
        s_mosd.write_offset_x(osh.tex_id_data);
        s_mosd.write_int32_t(os->tex_id_num);
        s_mosd.write_uint32_t(os->reserved[0]);
        s_mosd.write_uint32_t(os->reserved[1]);
    }
    s_mosd.position_pop();

    f2_struct st;
    for (uint32_t i = 0; i < obj_num; i++) {
        obj* obj = &os->obj_data[i];

        st.sub_structs.push_back({});
        f2_struct* omdl = &st.sub_structs.back();

        stream s_omdl;
        s_omdl.open();

        if (obj->skin_init) {
            st.sub_structs.push_back({});
            f2_struct* oskn = &st.sub_structs.back();

            stream s_oskn;
            s_oskn.open();

            obj_modern_write_skin(obj, s_oskn, 0, is_x, oskn);

            s_oskn.align_write(0x10);
            s_oskn.copy(oskn->data);
            s_oskn.close();

            oskn->header.signature = reverse_endianness_uint32_t('OSKN');
            oskn->header.length = 0x20;
            oskn->header.use_big_endian = false;
            oskn->header.use_section_size = true;
        }

        obj_modern_write_model(obj, s_omdl, 0, is_x, omdl);

        s_omdl.align_write(0x10);
        s_omdl.copy(omdl->data);
        s_omdl.close();

        omdl->header.signature = reverse_endianness_uint32_t('OMDL');
        omdl->header.length = 0x20;
        omdl->header.use_big_endian = false;
        omdl->header.use_section_size = true;
    }

    s_mosd.align_write(0x10);
    s_mosd.copy(st.data);
    s_mosd.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSD');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    st.write(s, true, os->is_x);
}

static void obj_modern_read_index(obj* obj, stream& s, obj_sub_mesh* sub_mesh) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t* indices = force_malloc_s(uint32_t, sub_mesh->indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++) {
            uint8_t idx = s.read_uint8_t();
            indices[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++) {
            uint16_t idx = s.read_uint16_t_reverse_endianness();
            indices[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++)
            indices[i] = s.read_uint32_t_reverse_endianness();
        break;
    }
    sub_mesh->indices = indices;
}

static void obj_modern_write_index(obj* obj, stream& s, bool is_x,
    obj_sub_mesh* sub_mesh, f2_struct* oidx) {
    int32_t size = 1;
    enrs_type type = ENRS_INVALID;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U16:
        size = 2;
        type = ENRS_WORD;
        break;
    case OBJ_INDEX_U32:
        size = 4;
        type = ENRS_DWORD;
        break;
    }

    uint32_t off = 0;
    enrs* e = &oidx->enrs;
    enrs_entry ee;
    bool add_enrs = true;
    if (e->vec.size() > 0) {
        off = (uint32_t)((size_t)e->vec.back().size * e->vec.back().repeat_count);
        if (e->vec.back().count && e->vec.back().sub.front().type == type) {
            e->vec.back().repeat_count += sub_mesh->indices_count;
            add_enrs = false;
        }
    }

    if (add_enrs)
        if (type != ENRS_INVALID) {
            ee = { off, 1, (uint32_t)size, (uint32_t)sub_mesh->indices_count };
            ee.append(0, 1, type);
            e->vec.push_back(ee);
        }
        else {
            ee = { off, 0, (uint32_t)size, (uint32_t)sub_mesh->indices_count };
            e->vec.push_back(ee);
        }

    uint32_t* indices = sub_mesh->indices;
    uint32_t _indices_count = sub_mesh->indices_count;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < _indices_count; i++)
            s.write_uint8_t((uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < _indices_count; i++)
            s.write_uint16_t((uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < _indices_count; i++)
            s.write_uint32_t(indices[i]);
        break;
    }
    s.align_write(0x04);
}

static void obj_modern_read_model(obj* obj, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    s.set_position(base_offset, SEEK_SET);

    obj_header oh = {};
    if (!is_x) {
        s.read_uint32_t(); // version
        s.read_uint32_t(); // flags
        obj->bounding_sphere.center.x = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.center.y = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.center.z = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.radius = s.read_float_t_reverse_endianness();
        obj->num_mesh = s.read_int32_t_reverse_endianness();
        oh.mesh_array = s.read_offset_f2(header_length);
        obj->num_material = s.read_int32_t_reverse_endianness();
        oh.material_array = s.read_offset_f2(header_length);
        obj->reserved[0] = s.read_uint32_t_reverse_endianness();
        obj->reserved[1] = s.read_uint32_t_reverse_endianness();
        obj->reserved[2] = s.read_uint32_t_reverse_endianness();
        obj->reserved[3] = s.read_uint32_t_reverse_endianness();
        obj->reserved[4] = s.read_uint32_t_reverse_endianness();
        obj->reserved[5] = s.read_uint32_t_reverse_endianness();
        obj->reserved[6] = s.read_uint32_t_reverse_endianness();
        obj->reserved[7] = s.read_uint32_t_reverse_endianness();
        obj->reserved[8] = s.read_uint32_t_reverse_endianness();
        obj->reserved[9] = s.read_uint32_t_reverse_endianness();
    }
    else {
        s.read_uint32_t(); // version
        s.read_uint32_t(); // flags
        obj->num_mesh = s.read_int32_t_reverse_endianness();
        obj->num_material = s.read_int32_t_reverse_endianness();
        obj->bounding_sphere.center.x = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.center.y = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.center.z = s.read_float_t_reverse_endianness();
        obj->bounding_sphere.radius = s.read_float_t_reverse_endianness();
        oh.mesh_array = s.read_offset_x();
        oh.material_array = s.read_offset_x();
        s.read(0, 0x10);
        obj->flags = s.read_uint8_t();
        s.read(0, 0x07);
        obj->reserved[0] = s.read_uint32_t_reverse_endianness();
        obj->reserved[1] = s.read_uint32_t_reverse_endianness();
        obj->reserved[2] = s.read_uint32_t_reverse_endianness();
        obj->reserved[3] = s.read_uint32_t_reverse_endianness();
        obj->reserved[4] = s.read_uint32_t_reverse_endianness();
        obj->reserved[5] = s.read_uint32_t_reverse_endianness();
        obj->reserved[6] = s.read_uint32_t_reverse_endianness();
        obj->reserved[7] = s.read_uint32_t_reverse_endianness();
        obj->reserved[8] = s.read_uint32_t_reverse_endianness();
        obj->reserved[9] = s.read_uint32_t_reverse_endianness();
    }

    if (oh.mesh_array > 0) {
        obj->mesh_array = force_malloc_s(obj_mesh, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            s.set_position(base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);

            obj_mesh_header mh = {};
            mesh->flags = s.read_uint32_t_reverse_endianness();
            mesh->bounding_sphere.center.x = s.read_float_t_reverse_endianness();
            mesh->bounding_sphere.center.y = s.read_float_t_reverse_endianness();
            mesh->bounding_sphere.center.z = s.read_float_t_reverse_endianness();
            mesh->bounding_sphere.radius = s.read_float_t_reverse_endianness();
            mesh->num_submesh = s.read_int32_t_reverse_endianness();
            mh.submesh_array = s.read_offset(header_length, is_x);
            mh.format = (obj_vertex_format_file)s.read_uint32_t_reverse_endianness();
            mh.size_vertex = s.read_int32_t_reverse_endianness();
            mh.num_vertex = s.read_int32_t_reverse_endianness();

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    mh.vertex[j] = s.read_offset_f2(0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    mh.vertex[j] = s.read_offset_x();

            mesh->attrib.w = s.read_uint32_t_reverse_endianness();
            mh.vertex_flags = s.read_uint32_t_reverse_endianness();
            mesh->reserved[0] = s.read_uint32_t_reverse_endianness();
            mesh->reserved[1] = s.read_uint32_t_reverse_endianness();
            mesh->reserved[2] = s.read_uint32_t_reverse_endianness();
            mesh->reserved[3] = s.read_uint32_t_reverse_endianness();
            mesh->reserved[4] = s.read_uint32_t_reverse_endianness();
            mesh->reserved[5] = s.read_uint32_t_reverse_endianness();
            s.read(&mesh->name, sizeof(mesh->name));
            mesh->name[sizeof(mesh->name) - 1] = 0;

            if (mh.submesh_array) {
                mesh->submesh_array = force_malloc_s(obj_sub_mesh, mesh->num_submesh);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                    s.set_position(base_offset + mh.submesh_array + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh = {};
                    sub_mesh->flags = s.read_uint32_t_reverse_endianness();
                    sub_mesh->bounding_sphere.center.x = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_sphere.center.y = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_sphere.center.z = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_sphere.radius = s.read_float_t_reverse_endianness();
                    sub_mesh->material_index = s.read_uint32_t_reverse_endianness();
                    s.read(&sub_mesh->uv_index, 0x08);
                    sub_mesh->bone_indices_count = s.read_int32_t_reverse_endianness();
                    smh.bone_indices_offset = s.read_offset(header_length, is_x);
                    sub_mesh->bones_per_vertex = s.read_uint32_t_reverse_endianness();
                    sub_mesh->primitive_type = (obj_primitive_type)s.read_uint32_t_reverse_endianness();
                    sub_mesh->index_format = (obj_index_format)s.read_uint32_t_reverse_endianness();
                    sub_mesh->indices_count = s.read_int32_t_reverse_endianness();
                    smh.indices_offset = s.read_offset(0, is_x);
                    sub_mesh->attrib.w = s.read_uint32_t_reverse_endianness();
                    s.read(0, 0x10);
                    sub_mesh->bounding_box.center.x = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_box.center.y = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_box.center.z = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_box.size.x = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_box.size.y = s.read_float_t_reverse_endianness();
                    sub_mesh->bounding_box.size.z = s.read_float_t_reverse_endianness();
                    s.read(0, 0x04);
                    sub_mesh->indices_offset = s.read_uint32_t_reverse_endianness();

                    if (is_x)
                        s.read(0, 0x04);

                    if (sub_mesh->bones_per_vertex == 4 && smh.bone_indices_offset) {
                        sub_mesh->bone_indices = force_malloc_s(uint16_t, sub_mesh->bone_indices_count);
                        s.set_position(base_offset + smh.bone_indices_offset, SEEK_SET);
                        s.read(sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                        if (s.is_big_endian) {
                            uint16_t* bone_indices = sub_mesh->bone_indices;
                            for (uint32_t k = 0; k < sub_mesh->bone_indices_count; k++)
                                bone_indices[k] = reverse_endianness_uint16_t(bone_indices[k]);
                        }
                    }

                    s_oidx->set_position(smh.indices_offset, SEEK_SET);
                    obj_modern_read_index(obj, *s_oidx, sub_mesh);
                }
            }

            obj_modern_read_vertex(obj, *s_ovtx, mh.vertex, mesh,
                mh.vertex_flags, mh.num_vertex, mh.size_vertex);
        }
    }

    if (oh.material_array > 0) {
        obj_material_texture_enrs_table_init();

        s.set_position(base_offset + oh.material_array, SEEK_SET);
        obj->material_array = force_malloc_s(obj_material_data, obj->num_material);
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data mat_data;
            s.read(&mat_data, sizeof(obj_material_data));

            if (s.is_big_endian)
                obj_material_texture_enrs_table.apply(&mat_data);

            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
            obj->material_array[i] = mat_data;
        }
    }
}

static void obj_modern_write_model(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* omdl) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    omdl->sub_structs.push_back({});
    omdl->sub_structs.push_back({});
    f2_struct* oidx = &omdl->sub_structs.end()[-2];
    f2_struct* ovtx = &omdl->sub_structs.back();

    stream s_oidx;
    stream s_ovtx;
    s_oidx.open();
    s_ovtx.open();

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    obj_header oh = {};
    if (!is_x) {
        ee = { 0, 1, 80, 1 };
        ee.append(0, 10, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 80;
    }
    else {
        ee = { 0, 2, 112, 1 };
        ee.append(0, 8, ENRS_DWORD);
        ee.append(0, 2, ENRS_QWORD);
        e.vec.push_back(ee);
        off = 112;
    }

    if (!is_x) {
        ee = { off, 1, 216, (uint32_t)obj->num_mesh };
        ee.append(0, 32, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj->num_mesh * 216ULL);
    }
    else {
        ee = { off, 5, 304, (uint32_t)obj->num_mesh };
        ee.append(0, 6, ENRS_DWORD);
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 3, ENRS_DWORD);
        ee.append(4, 20, ENRS_QWORD);
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj->num_mesh * 304ULL);
    }

    uint32_t total_sub_meshes = 0;
    for (uint32_t i = 0; i < obj->num_mesh; i++)
        total_sub_meshes += obj->mesh_array[i].num_submesh;

    if (!is_x) {
        ee = { off, 17, 112, total_sub_meshes };
        ee.append(0, 6, ENRS_DWORD);
        ee.append(8, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(16, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_WORD);
        ee.append(0, 1, ENRS_WORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj->num_mesh * 112ULL);
    }
    else {
        ee = { off, 17, 128, total_sub_meshes };
        ee.append(0, 6, ENRS_DWORD);
        ee.append(8, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(16, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(0, 1, ENRS_WORD);
        ee.append(0, 1, ENRS_WORD);
        e.vec.push_back(ee);
        off = (uint32_t)(obj->num_mesh * 128ULL);
    }

    int32_t total_bone_indices_count = 0;
    for (uint32_t i = 0; i < obj->num_mesh; i++) {
        obj_mesh* mesh = &obj->mesh_array[i];
        for (uint32_t j = 0; j < mesh->num_submesh; j++) {
            obj_sub_mesh* sub_mesh = &mesh->submesh_array[i];
            if (sub_mesh->bones_per_vertex == 4)
                total_bone_indices_count += sub_mesh->bone_indices_count;
        }
    }

    ee = { off, 1, 2, (uint32_t)total_bone_indices_count };
    ee.append(0, 1, ENRS_WORD);
    e.vec.push_back(ee);
    off = (uint32_t)(2 * (size_t)total_bone_indices_count);
    off = align_val(off, is_x ? 0x10 : 0x04);

    if (obj->num_material) {
        obj_material_texture_enrs_table_init();

        enrs_entry* mte = &obj_material_texture_enrs_table.vec[0];
        ee = { off, 186, 1200, (uint32_t)obj->num_material, };
        ee.offset = off;
        ee.count = mte->count;
        ee.size = mte->size;
        ee.repeat_count = obj->num_material;
        ee.sub = mte->sub;
        e.vec.push_back(ee);
        off = (uint32_t)(ee.size * ee.repeat_count);
    }

    if (!is_x) {
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        s.write_int32_t(0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        s.write_int32_t(0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
    }
    else {
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_int32_t(0);
        s.write_int32_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        s.write_float_t(0);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        s.write(0x10);
        s.write_uint8_t(0);
        s.write(0x07);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
        s.write_uint32_t(0);
    }
    s.align_write(0x10);

    if (obj->num_mesh) {
        oh.mesh_array = s.get_position() - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, obj->num_mesh);
        obj_sub_mesh_header** smhss = force_malloc_s(obj_sub_mesh_header*, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            s.write(0x04);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_float_t(0.0f);
            s.write_int32_t(0);
            io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
            s.write_uint32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_f2(0, 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_x(0);

            s.write_int32_t(0);
            s.write_uint32_t(0);
            s.write(0x18);
            s.write(sizeof(mesh->name));
        }

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            mh->format = OBJ_VERTEX_FILE_MODERN_STORAGE;
            if (mesh->num_submesh) {
                mh->submesh_array = s.get_position() - base_offset;
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    s.write(0x04);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_float_t(0.0f);
                    s.write_uint32_t(0);
                    s.write(0x08);
                    s.write_int32_t(0);
                    io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
                    s.write_uint32_t(0);
                    s.write_uint32_t(0);
                    s.write_uint32_t(0);
                    s.write_int32_t(0);
                    s.write_offset(0, 0, is_x);
                    s.write_uint32_t(0);
                    s.write(0x10);
                    s.write_float_t(0);
                    s.write_float_t(0);
                    s.write_float_t(0);
                    s.write_float_t(0);
                    s.write_float_t(0);
                    s.write_float_t(0);
                    s.write(0x04);
                    s.write_uint32_t(0);

                    if (is_x)
                        s.write(0x04);
                }
            }

            obj_modern_write_vertex(obj, s_ovtx, is_x, mh->vertex, mesh,
                &mh->vertex_flags, &mh->num_vertex, &mh->size_vertex, ovtx);
        }

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = 0;

            if (mesh->num_submesh) {
                smhs = force_malloc_s(obj_sub_mesh_header, mesh->num_submesh);
                smhss[i] = smhs;
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    if (sub_mesh->bones_per_vertex == 4 && sub_mesh->bone_indices_count) {
                        smh->bone_indices_offset = s.get_position() - base_offset;
                        s.write(sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    }

                    smh->indices_offset = s_oidx.get_position();
                    obj_modern_write_index(obj, s_oidx, is_x, sub_mesh, oidx);
                }
            }
        }
        s.align_write(is_x ? 0x10 : 0x04);

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = smhss[i];

            if (mesh->num_submesh) {
                s.position_push(base_offset + mh->submesh_array, SEEK_SET);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    s.write_uint32_t(sub_mesh->flags);
                    s.write_float_t(sub_mesh->bounding_sphere.center.x);
                    s.write_float_t(sub_mesh->bounding_sphere.center.y);
                    s.write_float_t(sub_mesh->bounding_sphere.center.z);
                    s.write_float_t(sub_mesh->bounding_sphere.radius);
                    s.write_uint32_t(sub_mesh->material_index);
                    s.write(&sub_mesh->uv_index, 0x08);
                    s.write_int32_t(sub_mesh->bone_indices_count);
                    s.write_offset(smh->bone_indices_offset, 0x20, is_x);
                    s.write_uint32_t(sub_mesh->bones_per_vertex);
                    s.write_uint32_t(sub_mesh->primitive_type);
                    s.write_uint32_t(sub_mesh->index_format);
                    s.write_int32_t(sub_mesh->indices_count);
                    s.write_offset(smh->indices_offset, 0, is_x);
                    s.write_uint32_t(sub_mesh->attrib.w);
                    s.write(0x10);
                    s.write_float_t(sub_mesh->bounding_box.center.x);
                    s.write_float_t(sub_mesh->bounding_box.center.y);
                    s.write_float_t(sub_mesh->bounding_box.center.z);
                    s.write_float_t(sub_mesh->bounding_box.size.x);
                    s.write_float_t(sub_mesh->bounding_box.size.y);
                    s.write_float_t(sub_mesh->bounding_box.size.z);

                    uint32_t index_max = 0;
                    for (uint32_t k = 0; k < sub_mesh->indices_count; k++)
                        if (index_max < sub_mesh->indices[k])
                            index_max = sub_mesh->indices[k];

                    switch (sub_mesh->index_format) {
                    case OBJ_INDEX_U8:
                        s.write_uint32_t(index_max << 24);
                        break;
                    case OBJ_INDEX_U16:
                        s.write_uint32_t(index_max << 16);
                        break;
                    case OBJ_INDEX_U32:
                        s.write_uint32_t(index_max);
                        break;
                    }
                    s.write_uint32_t(sub_mesh->indices_offset);

                    if (is_x)
                        s.write(0x04);
                }
                s.position_pop();
            }
            free(smhs);
        }

        s.position_push(base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            s.write_uint32_t(mesh->flags);
            s.write_float_t(mesh->bounding_sphere.center.x);
            s.write_float_t(mesh->bounding_sphere.center.y);
            s.write_float_t(mesh->bounding_sphere.center.z);
            s.write_float_t(mesh->bounding_sphere.radius);
            s.write_int32_t(mesh->num_submesh);
            if (mh->submesh_array && !is_x)
                mh->submesh_array += 0x20;
            s.write_offset(mh->submesh_array, 0, is_x);
            s.write_uint32_t(mh->format);
            s.write_int32_t(mh->size_vertex);
            s.write_int32_t(mh->num_vertex);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_f2(mh->vertex[j], 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_x(mh->vertex[j]);

            s.write_uint32_t(mesh->attrib.w);
            s.write_uint32_t(mh->vertex_flags);
            s.write_uint32_t(mesh->reserved[0]);
            s.write_uint32_t(mesh->reserved[1]);
            s.write_uint32_t(mesh->reserved[2]);
            s.write_uint32_t(mesh->reserved[3]);
            s.write_uint32_t(mesh->reserved[4]);
            s.write_uint32_t(mesh->reserved[5]);
            s.write(&mesh->name, sizeof(mesh->name) - 1);
            s.write_char('\0');
        }
        s.position_pop();
        free(mhs);
        free(smhss);
    }

    if (obj->material_array) {
        oh.material_array = s.get_position() - base_offset;
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data mat_data = obj->material_array[i];
            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
            s.write(&mat_data, sizeof(obj_material_data));
        }
    }
    s.align_write(0x10);

    s.position_push(base_offset, SEEK_SET);
    if (!is_x) {
        s.write_uint32_t(0x10000); // version
        s.write_uint32_t(0x00); // flags
        s.write_float_t(obj->bounding_sphere.center.x);
        s.write_float_t(obj->bounding_sphere.center.y);
        s.write_float_t(obj->bounding_sphere.center.z);
        s.write_float_t(obj->bounding_sphere.radius);
        s.write_int32_t(obj->num_mesh);
        s.write_offset_f2(oh.mesh_array, 0x20);
        s.write_int32_t(obj->num_material);
        s.write_offset_f2(oh.material_array, 0x20);
        s.write_uint32_t(obj->reserved[0]);
        s.write_uint32_t(obj->reserved[1]);
        s.write_uint32_t(obj->reserved[2]);
        s.write_uint32_t(obj->reserved[3]);
        s.write_uint32_t(obj->reserved[4]);
        s.write_uint32_t(obj->reserved[5]);
        s.write_uint32_t(obj->reserved[6]);
        s.write_uint32_t(obj->reserved[7]);
        s.write_uint32_t(obj->reserved[8]);
        s.write_uint32_t(obj->reserved[9]);
    }
    else {
        s.write_uint32_t(0x10000); // version
        s.write_uint32_t(0x00); // flags
        s.write_int32_t(obj->num_mesh);
        s.write_int32_t(obj->num_material);
        s.write_float_t(obj->bounding_sphere.center.x);
        s.write_float_t(obj->bounding_sphere.center.y);
        s.write_float_t(obj->bounding_sphere.center.z);
        s.write_float_t(obj->bounding_sphere.radius);
        s.write_offset_x(oh.mesh_array);
        s.write_offset_x(oh.material_array);
        s.write(0x10);
        s.write_uint8_t(obj->flags);
        s.write(0x07);
        s.write_uint32_t(obj->reserved[0]);
        s.write_uint32_t(obj->reserved[1]);
        s.write_uint32_t(obj->reserved[2]);
        s.write_uint32_t(obj->reserved[3]);
        s.write_uint32_t(obj->reserved[4]);
        s.write_uint32_t(obj->reserved[5]);
        s.write_uint32_t(obj->reserved[6]);
        s.write_uint32_t(obj->reserved[7]);
        s.write_uint32_t(obj->reserved[8]);
        s.write_uint32_t(obj->reserved[9]);
    }
    s.position_pop();

    omdl->enrs = e;
    omdl->pof = pof;

    s_oidx.align_write(0x10);
    s_oidx.copy(oidx->data);
    s_oidx.close();

    oidx->header.signature = reverse_endianness_uint32_t('OIDX');
    oidx->header.length = 0x20;
    oidx->header.use_big_endian = false;
    oidx->header.use_section_size = true;

    s_ovtx.align_write(0x10);
    s_ovtx.copy(ovtx->data);
    s_ovtx.close();

    ovtx->header.signature = reverse_endianness_uint32_t('OVTX');
    ovtx->header.length = 0x20;
    ovtx->header.use_big_endian = false;
    ovtx->header.use_section_size = true;
}

static void obj_modern_read_skin(obj* obj, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x) {
    obj_skin* sk = &obj->skin;
    obj->skin_init = true;
    s.set_position(base_offset, SEEK_SET);

    obj_skin_header sh = {};
    if (!is_x) {
        sh.bone_ids_offset = s.read_offset_f2(header_length);
        sh.bone_matrices_offset = s.read_offset_f2(header_length);
        sh.bone_names_offset = s.read_offset_f2(header_length);
        sh.ex_data_offset = s.read_offset_f2(header_length);
        sk->bones_count = s.read_int32_t_reverse_endianness();
        sh.bone_parent_ids_offset = s.read_offset_f2(header_length);
        s.read(0, 0x0C);
    }
    else {
        sh.bone_ids_offset = s.read_offset_x();
        sh.bone_matrices_offset = s.read_offset_x();
        sh.bone_names_offset = s.read_offset_x();
        sh.ex_data_offset = s.read_offset_x();
        sk->bones_count = s.read_int32_t_reverse_endianness();
        sh.bone_parent_ids_offset = s.read_offset_x();
        s.read(0, 0x18);
    }

    if (sh.bone_ids_offset) {
        sk->bones = force_malloc_s(obj_skin_bone, sk->bones_count);

        int64_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(int64_t, sk->bones_count);
            s.set_position(sh.bone_names_offset, SEEK_SET);
            if (!is_x)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    bone_names_offsets[i] = s.read_offset_f2(header_length);
            else
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    bone_names_offsets[i] = s.read_offset_x();
        }

        if (sh.bone_ids_offset) {
            s.set_position(sh.bone_ids_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                sk->bones[i].id = s.read_uint32_t_reverse_endianness();

            if (sh.bone_matrices_offset) {
                s.set_position(sh.bone_matrices_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++) {
                    mat4& mat = sk->bones[i].inv_bind_pose_mat;
                    mat.row0.x = s.read_float_t_reverse_endianness();
                    mat.row1.x = s.read_float_t_reverse_endianness();
                    mat.row2.x = s.read_float_t_reverse_endianness();
                    mat.row3.x = s.read_float_t_reverse_endianness();
                    mat.row0.y = s.read_float_t_reverse_endianness();
                    mat.row1.y = s.read_float_t_reverse_endianness();
                    mat.row2.y = s.read_float_t_reverse_endianness();
                    mat.row3.y = s.read_float_t_reverse_endianness();
                    mat.row0.z = s.read_float_t_reverse_endianness();
                    mat.row1.z = s.read_float_t_reverse_endianness();
                    mat.row2.z = s.read_float_t_reverse_endianness();
                    mat.row3.z = s.read_float_t_reverse_endianness();
                    mat.row0.w = s.read_float_t_reverse_endianness();
                    mat.row1.w = s.read_float_t_reverse_endianness();
                    mat.row2.w = s.read_float_t_reverse_endianness();
                    mat.row3.w = s.read_float_t_reverse_endianness();
                }
            }

            if (sh.bone_names_offset)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].name = s.read_utf8_string_null_terminated_offset(
                        bone_names_offsets[i]);

            if (sh.bone_parent_ids_offset) {
                s.set_position(sh.bone_parent_ids_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].parent = s.read_uint32_t_reverse_endianness();
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        s.set_position(sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh = {};
        if (!is_x) {
            exh.osage_count = s.read_int32_t_reverse_endianness();
            ex->osage_nodes_count = s.read_int32_t_reverse_endianness();
            s.read(0, 0x04);
            exh.osage_nodes_offset = s.read_offset_f2(header_length);
            exh.osage_names_offset = s.read_offset_f2(header_length);
            exh.blocks_offset = s.read_offset_f2(header_length);
            exh.bone_names_count = s.read_int32_t_reverse_endianness();
            exh.bone_names_offset = s.read_offset_f2(header_length);
            exh.osage_sibling_infos_offset = s.read_offset_f2(header_length);
            exh.cloth_count = s.read_int32_t_reverse_endianness();
            ex->reserved[0] = s.read_offset_f2(header_length);
            ex->reserved[1] = s.read_offset_f2(header_length);
            ex->reserved[2] = s.read_offset_f2(header_length);
            ex->reserved[3] = s.read_offset_f2(header_length);
            ex->reserved[4] = s.read_offset_f2(header_length);
            ex->reserved[5] = s.read_offset_f2(header_length);
            ex->reserved[6] = s.read_offset_f2(header_length);
        }
        else {
            exh.osage_count = s.read_int32_t_reverse_endianness();
            ex->osage_nodes_count = s.read_int32_t_reverse_endianness();
            s.read(0, 0x08);
            exh.osage_nodes_offset = s.read_offset_x();
            exh.osage_names_offset = s.read_offset_x();
            exh.blocks_offset = s.read_offset_x();
            exh.bone_names_count = s.read_int32_t_reverse_endianness();
            exh.bone_names_offset = s.read_offset_x();
            exh.osage_sibling_infos_offset = s.read_offset_x();
            exh.cloth_count = s.read_int32_t_reverse_endianness();
            ex->reserved[0] = s.read_offset_x();
            ex->reserved[1] = s.read_offset_x();
            ex->reserved[2] = s.read_offset_x();
            ex->reserved[3] = s.read_offset_x();
            ex->reserved[4] = s.read_offset_x();
            ex->reserved[5] = s.read_offset_x();
            ex->reserved[6] = s.read_offset_x();
        }

        if (exh.bone_names_offset) {
            std::vector<std::string> bone_names;
            bone_names.reserve(exh.bone_names_count);
            int32_t* strings_offsets = force_malloc_s(int32_t, exh.bone_names_count);
            s.set_position(exh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < exh.bone_names_count; i++)
                strings_offsets[i] = s.read_int32_t();

            size_t buf_size = 0;
            for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                if (strings_offsets[i])
                    bone_names.push_back(s.read_string_null_terminated_offset(
                        strings_offsets[i]));
                else
                    bone_names.push_back({});
                buf_size += bone_names.back().size() + 1;
            }
            free(strings_offsets);

            if (bone_names.size()) {
                ex->bone_names_buf = force_malloc_s(char, buf_size);
                ex->bone_names = force_malloc_s(char*, buf_size + 1);
                ex->bone_names_count = exh.bone_names_count;

                char* bone_names_buf = ex->bone_names_buf;
                char** bone_names_ptr = ex->bone_names;
                for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                    std::string* str = &bone_names.data()[i];
                    *bone_names_ptr++ = bone_names_buf;
                    memcpy(bone_names_buf, str->c_str(), str->size());
                    bone_names_buf[str->size()] = 0;
                    bone_names_buf += str->size() + 1;
                }
                *bone_names_ptr = 0;
            }
        }
        else {
            ex->bone_names_buf = 0;
            ex->bone_names = 0;
            ex->bone_names_count = 0;
        }

        char** bone_names = ex->bone_names;
        if (!bone_names)
            return;

        if (exh.osage_nodes_offset) {
            ex->osage_nodes = force_malloc_s(obj_skin_osage_node, ex->osage_nodes_count);

            s.set_position(exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];

                osage_node->name_index = s.read_uint32_t_reverse_endianness();
                osage_node->length = s.read_float_t_reverse_endianness();
                s.read(0, 0x04);
            }
        }

        if (exh.blocks_offset) {
            ex->blocks_count = 0;
            s.set_position(exh.blocks_offset, SEEK_SET);
            if (!is_x)
                while (s.read_int32_t()) {
                    s.read(0, 0x04);
                    ex->blocks_count++;
                }
            else
                while (s.read_int64_t()) {
                    s.read(0, 0x08);
                    ex->blocks_count++;
                }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            s.set_position(exh.blocks_offset, SEEK_SET);
            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    bhs[i].block_signature_offset = s.read_offset_f2(header_length);
                    bhs[i].block_offset = s.read_offset_f2(header_length);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    bhs[i].block_signature_offset = s.read_offset_x();
                    bhs[i].block_offset = s.read_offset_x();
                }

            ex->blocks = force_malloc_s(obj_skin_block, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];

                std::string block_signature = s.read_string_null_terminated_offset(
                    bhs[i].block_signature_offset);
                if (block_signature.size() != 3)
                    continue;

                uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
                switch (signature) {
                case 'CLS\0':
                    block->type = OBJ_SKIN_BLOCK_CLOTH;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_cloth(&block->cloth, s,
                        header_length, bone_names, is_x);
                    break;
                case 'CNS\0':
                    block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_constraint(&block->constraint, s,
                        header_length, bone_names, is_x);
                    break;
                case 'EXP\0':
                    block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_expression(&block->expression, s,
                        header_length, bone_names, is_x);
                    break;
                case 'MOT\0':
                    block->type = OBJ_SKIN_BLOCK_MOTION;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_motion(&block->motion, s,
                        header_length, bone_names, is_x);
                    break;
                case 'OSG\0':
                    block->type = OBJ_SKIN_BLOCK_OSAGE;
                    s.set_position(bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_osage(&block->osage, s,
                        header_length, bone_names, is_x);
                    break;
                default:
                    break;
                }
            }
            free(bhs);
        }

        if (exh.osage_sibling_infos_offset) {
            ex->osage_sibling_infos_count = 0;
            s.set_position(exh.osage_sibling_infos_offset, SEEK_SET);
            while (s.read_uint32_t()) {
                s.read(0, 0x08);
                ex->osage_sibling_infos_count++;
            }

            obj_skin_osage_sibling_info* osis = force_malloc_s(
                obj_skin_osage_sibling_info, ex->osage_sibling_infos_count);
            ex->osage_sibling_infos = osis;
            s.set_position(exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                osis[i].name_index = s.read_uint32_t_reverse_endianness();
                osis[i].sibling_name_index = s.read_uint32_t_reverse_endianness();
                osis[i].max_distance = s.read_float_t_reverse_endianness();
            }

            s.set_position(exh.osage_sibling_infos_offset, SEEK_SET);
        }
    }
}

static void obj_modern_write_skin(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* oskn) {
    obj_skin* sk = &obj->skin;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    if (!is_x) {
        ee = { 0, 1, 48, 1 };
        ee.append(0, 9, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 48;
    }
    else {
        ee = { 0, 3, 72, 1 };
        ee.append(0, 4, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = 72;
    }
    off = align_val(off, 0x10);

    obj_skin_header sh = {};
    if (sk->bones_count) {
        if (sk->bones_count % 4) {
            ee = { off, 1, 4, (uint32_t)sk->bones_count };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->bones_count * 4ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 4, (uint32_t)sk->bones_count };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->bones_count * 4ULL);
        }
        else {
            ee = { off, 1, 4, (uint32_t)(sk->bones_count * 2ULL) };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->bones_count * 2 * 4ULL);
        }
        off = align_val(off, 0x10);

        if (!is_x) {
            ee = { off, 1, 4, (uint32_t)sk->bones_count };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->bones_count * 4ULL);
        }
        else {
            ee = { off, 1, 8, (uint32_t)sk->bones_count };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->bones_count * 8ULL);
        }
        off = align_val(off, 0x10);

        ee = { off, 1, 64, (uint32_t)sk->bones_count };
        ee.append(0, 16, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sk->bones_count * 64ULL);
        off = align_val(off, 0x10);
    }

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;
    std::vector<std::string> bone_names;

    obj_skin_ex_data_header exh = {};

    obj_skin_block_header* bhs = 0;
    int64_t motion_block_node_mats = 0;
    int64_t motion_block_node_name_offset = 0;
    int64_t cloth_mats = 0;
    int64_t cloth_root = 0;
    int64_t cloth_nodes = 0;
    int64_t cloth_mesh_indices = 0;
    int64_t cloth_backface_mesh_indices = 0;
    int32_t expressions_count = 0;
    int32_t motion_count = 0;
    int32_t motion_nodes_count = 0;

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        char** bone_names_ptr = ex->bone_names;
        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &block->cloth;
                obj_skin_strings_push_back_check(strings, cloth->mesh_name);
                obj_skin_strings_push_back_check(strings, cloth->backface_mesh_name);

                for (uint32_t k = 0; k < cloth->root_count; k++) {
                    obj_skin_block_cloth_root* sub = &cloth->root[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings, sub->bone_weights[l].bone_name);
                }
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_strings_push_back_check(strings, constraint->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, constraint->name_index);
                obj_skin_strings_push_back_check(strings, constraint->source_node_name);
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings, constraint->direction.up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings, constraint->position.up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings, constraint->distance.up_vector.name);
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                for (uint32_t j = 0; j < expression->expressions_count; j++)
                    obj_skin_strings_push_back_check(strings, expression->expressions[j]);
                obj_skin_strings_push_back_check(strings, expression->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_strings_push_back_check(strings, motion->base.parent_name);
                obj_skin_strings_push_back_check(strings, motion->name);
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, motion->nodes[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                if (!is_x)
                    obj_skin_strings_push_back_check(bone_names, motion->name);
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, motion->nodes[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_strings_push_back_check(strings, osage->base.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, osage->name_index);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, osage->external_name_index);
                obj_skin_osage_node* osage_node = &ex->osage_nodes[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_infos = ex->osage_sibling_infos;
                    for (uint32_t k = 0; k < ex->osage_sibling_infos_count; k++) {
                        if (name_index == osage_sibling_infos->name_index) {
                            obj_skin_strings_push_back_check_by_index(strings,
                                bone_names_ptr, osage_sibling_infos->sibling_name_index);
                            break;
                        }
                        osage_sibling_infos++;
                    }
                    osage_node++;
                }
                obj_skin_strings_push_back_check(strings, "OSG");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, osage->external_name_index);
                osage_node = &ex->osage_nodes[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_infos = ex->osage_sibling_infos;
                    for (uint32_t k = 0; k < ex->osage_sibling_infos_count; k++) {
                        if (name_index == osage_sibling_infos->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_names_ptr, osage_sibling_infos->sibling_name_index);
                            break;
                        }
                        osage_sibling_infos++;
                    }
                    osage_node++;
                }
                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, osage->name_index);
            } break;
            }
        }

        exh.cloth_count = 0;
        exh.osage_count = 0;
        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            if (block->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (block->type == OBJ_SKIN_BLOCK_EXPRESSION)
                expressions_count++;
            else if (block->type == OBJ_SKIN_BLOCK_MOTION) {
                motion_count++;
                motion_nodes_count += block->motion.nodes_count;
            }
            else if (block->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.osage_count++;
        }

        exh.bone_names_count = (int32_t)bone_names.size();
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (!is_x) {
            ee = { off, 1, 80, 1 };
            ee.append(0, 10, ENRS_DWORD);
            e.vec.push_back(ee);
            off = 80;
        }
        else {
            ee = { off, 5, 128, 1 };
            ee.append(0, 2, ENRS_DWORD);
            ee.append(0, 4, ENRS_QWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(4, 2, ENRS_QWORD);
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = 128;
        }
        off = align_val(off, 0x10);

        if (ex->osage_nodes_count) {
            ee = { off, 1, 12, (uint32_t)ex->osage_nodes_count };
            ee.append(0, 2, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(ex->osage_nodes_count * 12ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 12, (uint32_t)ex->osage_sibling_infos_count };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(ex->osage_sibling_infos_count * 12ULL);
            off = align_val(off, 0x10);
        }

        if (exh.osage_count || exh.cloth_count) {
            int32_t count = exh.osage_count + exh.cloth_count;
            if (!is_x) {
                ee = { off, 1, 4, (uint32_t)count };
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(count * 4ULL);
            }
            else {
                ee = { off, 1, 8, (uint32_t)count };
                ee.append(0, 1, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(count * 8ULL);
            }
            off = align_val(off, 0x10);
        }

        if (!is_x) {
            ee = { off, 1, 4, (uint32_t)exh.bone_names_count };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(exh.bone_names_count * 4ULL);
        }
        else {
            ee = { off, 1, 8, (uint32_t)exh.bone_names_count };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(exh.bone_names_count * 8ULL);
        }
        off = align_val(off, 0x10);

        if (ex->blocks_count > 0) {
            if (!is_x) {
                ee = { off, 1, 8, (uint32_t)ex->blocks_count };
                ee.append(0, 2, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(ex->blocks_count * 8ULL);
            }
            else {
                ee = { off, 1, 16, (uint32_t)ex->blocks_count };
                ee.append(0, 2, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(ex->blocks_count * 16ULL);
            }
            off = align_val(off, 0x10);

            if (exh.osage_count) {
                if (!is_x) {
                    ee = { off, 2, 76, (uint32_t)exh.osage_count };
                    ee.append(0, 14, ENRS_DWORD);
                    ee.append(4, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.osage_count * 76ULL);
                }
                else {
                    ee = { off, 4, 104, (uint32_t)exh.osage_count };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 9, ENRS_DWORD);
                    ee.append(4, 4, ENRS_DWORD);
                    ee.append(8, 1, ENRS_QWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.osage_count * 104ULL);
                }
                off = align_val(off, 0x10);
            }

            if (expressions_count) {
                if (!is_x) {
                    ee = { off, 1, 84, (uint32_t)expressions_count };
                    ee.append(0, 19, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(expressions_count * 84ULL);
                }
                else {
                    ee = { off, 5, 136, (uint32_t)expressions_count };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 9, ENRS_DWORD);
                    ee.append(4, 1, ENRS_QWORD);
                    ee.append(0, 1, ENRS_DWORD);
                    ee.append(4, 9, ENRS_QWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(expressions_count * 136ULL);
                }
                off = align_val(off, 0x10);
            }

            if (exh.cloth_count) {
                if (!is_x) {
                    ee = { off, 1, 52, (uint32_t)exh.cloth_count };
                    ee.append(0, 13, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.cloth_count * 52ULL);
                }
                else {
                    ee = { off, 4, 88, (uint32_t)exh.cloth_count };
                    ee.append(0, 2, ENRS_QWORD);
                    ee.append(0, 4, ENRS_DWORD);
                    ee.append(0, 6, ENRS_QWORD);
                    ee.append(0, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.cloth_count * 88ULL);
                }
                off = align_val(off, 0x10);
            }

            int32_t constraint_count = 0;
            obj_skin_block_constraint_type cns_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                if (cns_type == block->constraint.type) {
                    constraint_count++;
                    continue;
                }

                if (cns_type) {
                    ee.repeat_count = constraint_count;
                    e.vec.push_back(ee);
                    off = ee.size * ee.repeat_count;
                }

                cns_type = block->constraint.type;
                if (!is_x)
                    switch (cns_type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        ee = { off, 6, 68, 0 };
                        ee.append(0, 17, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        ee = { off, 8, 144, 0 };
                        ee.append(0, 29, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        ee = { off, 8, 132, 0 };
                        ee.append(0, 33, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        ee = { off, 8, 136, 0 };
                        ee.append(0, 34, ENRS_DWORD);
                        break;
                    }
                else
                    switch (cns_type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        ee = { off, 6, 96, 0 };
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 9, ENRS_DWORD);
                        ee.append(4, 2, ENRS_QWORD);
                        ee.append(0, 1, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 3, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        ee = { off, 8, 144, 0 };
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 9, ENRS_DWORD);
                        ee.append(4, 2, ENRS_QWORD);
                        ee.append(0, 1, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 8, ENRS_DWORD);
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 6, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        ee = { off, 8, 160, 0 };
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 9, ENRS_DWORD);
                        ee.append(4, 2, ENRS_QWORD);
                        ee.append(0, 1, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 8, ENRS_DWORD);
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 10, ENRS_DWORD);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        ee = { off, 8, 168, 0 };
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 9, ENRS_DWORD);
                        ee.append(4, 2, ENRS_QWORD);
                        ee.append(0, 1, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 8, ENRS_DWORD);
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 11, ENRS_DWORD);
                        break;
                    }
                constraint_count = 1;
            }

            if (constraint_count) {
                ee.repeat_count = constraint_count;
                e.vec.push_back(ee);
                off = ee.size * ee.repeat_count;
            }
            off = align_val(off, 0x10);

            if (!is_x) {
                ee = { off, 1, 56, (uint32_t)motion_count };
                ee.append(0, 14, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(motion_count * 56ULL);
            }
            else {
                ee = { off, 5, 80, (uint32_t)motion_count };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 9, ENRS_DWORD);
                ee.append(4, 1, ENRS_QWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(4, 2, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(motion_count * 80ULL);
            }
            off = align_val(off, 0x10);

            if (motion_count) {
                ee = { off, 1, 64, (uint32_t)motion_nodes_count };
                ee.append(0, 16, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(motion_nodes_count * 64ULL);
                off = align_val(off, 0x10);

                ee = { off, 1, 4, (uint32_t)motion_nodes_count };
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(motion_nodes_count * 4ULL);
                off = align_val(off, 0x10);
            }

            if (exh.cloth_count) {
                int32_t mats_count = 0;
                int32_t root_count = 0;
                int32_t nodes_count = 0;
                int32_t mesh_indices_count = 0;
                int32_t backface_mesh_indices_count = 0;
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                        mats_count += block->cloth.mats ? block->cloth.mats_count : 0;
                        root_count += block->cloth.root ? block->cloth.root_count : 0;
                        nodes_count += block->cloth.nodes
                            ? block->cloth.root_count * (block->cloth.nodes_count - 1) : 0;
                        mesh_indices_count += block->cloth.mesh_indices
                            ? block->cloth.mesh_indices_count + 1 : 0;
                        backface_mesh_indices_count += block->cloth.backface_mesh_indices
                            ? block->cloth.backface_mesh_indices_count + 1 : 0;
                    }
                }

                if (mats_count) {
                    ee = { off, 1, sizeof(mat4), (uint32_t)mats_count };
                    ee.append(0, 16, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(mats_count * sizeof(mat4));
                    off = align_val(off, 0x10);
                }

                if (root_count) {
                    if (!is_x) {
                        ee = { off, 1, 104, (uint32_t)root_count };
                        ee.append(0, 26, ENRS_DWORD);
                        e.vec.push_back(ee);
                        off = (uint32_t)(root_count * 104ULL);
                    }
                    else {
                        ee = { off, 9, 136, (uint32_t)root_count };
                        ee.append(0, 10, ENRS_DWORD);
                        ee.append(0, 1, ENRS_QWORD);
                        ee.append(0, 3, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 3, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 3, ENRS_DWORD);
                        ee.append(4, 1, ENRS_QWORD);
                        ee.append(0, 3, ENRS_DWORD);
                        e.vec.push_back(ee);
                        off = (uint32_t)(root_count * 136ULL);
                    }
                    off = align_val(off, 0x10);
                }

                if (nodes_count) {
                    ee = { off, 1, 44, (uint32_t)nodes_count };
                    ee.append(0, 11, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(nodes_count * 44ULL);
                    off = align_val(off, 0x10);
                }

                if (mesh_indices_count) {
                    ee = { off, 1, 2, (uint32_t)mesh_indices_count };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(mesh_indices_count * 2ULL);
                    off = align_val(off, 0x10);
                }

                if (backface_mesh_indices_count) {
                    ee = { off, 1, 2, (uint32_t)backface_mesh_indices_count };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(backface_mesh_indices_count * 2ULL);
                    off = align_val(off, 0x10);
                }
            }
        }
    }

    if (!is_x) {
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        s.write_int32_t(0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        s.write(0x0C);
    }
    else {
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        s.write_int32_t(0);
        io_write_offset_x_pof_add(s, 0, &pof);
        s.write(0x18);
    }
    s.align_write(0x10);

    if (sk->bones_count) {
        sh.bone_ids_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++)
            s.write_int32_t(sk->bones[i].id);
        s.align_write(0x10);

        sh.bone_parent_ids_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++)
            s.write_int32_t(sk->bones[i].parent);
        s.align_write(0x10);

        sh.bone_names_offset = s.get_position();
        if (!is_x)
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                obj_skin_strings_push_back_check(strings, sk->bones[i].name);
            }
        else
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                io_write_offset_x_pof_add(s, 0, &pof);
                obj_skin_strings_push_back_check(strings, sk->bones[i].name);
            }
        s.align_write(0x10);

        sh.bone_matrices_offset = s.get_position();
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            mat4& mat = sk->bones[i].inv_bind_pose_mat;
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
        }
        s.align_write(0x10);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = s.get_position();
        if (!is_x) {
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write(0x04);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            s.write_int32_t(0);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            s.write_int32_t(0);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        }
        else {
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write(0x08);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            s.write_int32_t(0);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            s.write_int32_t(0);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
        }
        s.align_write(0x10);

        if (ex->osage_nodes_count) {
            exh.osage_nodes_offset = s.get_position();
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.align_write(0x10);

            exh.osage_sibling_infos_offset = s.get_position();
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.align_write(0x10);
        }

        if (exh.osage_count || exh.cloth_count) {
            exh.osage_names_offset = s.get_position();
            if (!is_x) {
                for (uint32_t i = 0; i < exh.osage_count; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                for (uint32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            }
            else {
                for (uint32_t i = 0; i < exh.osage_count; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);

                for (uint32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);
            }
            s.align_write(0x10);
        }

        exh.bone_names_offset = s.get_position();
        if (!is_x)
            for (std::string& i : bone_names)
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        else
            for (std::string& i : bone_names)
                io_write_offset_x_pof_add(s, 0, &pof);
        s.align_write(0x10);

        if (ex->blocks_count > 0) {
            exh.blocks_offset = s.get_position();
            if (!is_x) {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
                s.write_offset_f2(0, 0x20);
                s.write_offset_f2(0, 0x20);
            }
            else {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
                s.write_offset_x(0);
                s.write_offset_x(0);
            }
            s.align_write(0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    s.write(0x24);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = &block->osage;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    if (osage->motion_node_name) {
                        s.write(0x18);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        s.write(0x18);
                    }
                    else
                        s.write(0x38);
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;

                    obj_skin_block_expression* expression = &block->expression;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x04);

                    for (uint32_t j = 0; j < expression->expressions_count && j < 9; j++)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                    for (uint32_t j = expression->expressions_count; j < 9; j++)
                        s.write(0x04);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;
                    obj_skin_block_expression* expression = &block->expression;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x08);

                    for (uint32_t j = 0; j < expression->expressions_count && j < 9; j++)
                        io_write_offset_x_pof_add(s, 0, &pof);

                    for (uint32_t j = expression->expressions_count; j < 9; j++)
                        s.write(0x08);
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x10);

                    if (cloth->nodes_count)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    if (cloth->root_count) {
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    }
                    else
                        s.write(0x08);

                    if (cloth->mesh_indices_count)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    if (cloth->backface_mesh_indices_count)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    s.write(0x08);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x10);

                    if (cloth->nodes_count)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    if (cloth->root_count) {
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                    }
                    else
                        s.write(0x10);

                    if (cloth->mesh_indices_count)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    if (cloth->backface_mesh_indices_count)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    s.write(0x10);
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x04);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    switch (block->constraint.type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        s.write(0x0C);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        s.write(0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        s.write(0x18);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        s.write(0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        s.write(0x14);
                        s.write(0x14);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        s.write(0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        s.write(0x04);
                        s.write(0x14);
                        s.write(0x14);
                        break;
                    }
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x08);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    switch (block->constraint.type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        s.write(0x0C);
                        s.write(0x04);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        s.write(0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        s.write(0x18);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        s.write(0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        s.write(0x14);
                        s.write(0x14);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        s.write(0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        s.write(0x04);
                        s.write(0x14);
                        s.write(0x14);
                        s.write(0x04);
                        break;
                    }
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x04);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x08);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
            s.align_write(0x10);

            if (motion_count) {
                motion_block_node_mats = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    s.write(motion->nodes_count * sizeof(mat4));
                }
                s.align_write(0x10);

                motion_block_node_name_offset = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    s.write(motion->nodes_count * sizeof(uint32_t));
                }
                s.align_write(0x10);
            }

            if (exh.cloth_count) {
                cloth_mats = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    if (cloth->mats)
                        s.write(cloth->mats_count * sizeof(mat4));
                }
                s.align_write(0x10);

                cloth_root = s.get_position();
                if (!is_x)
                    for (uint32_t i = 0; i < ex->blocks_count; i++) {
                        obj_skin_block* block = &ex->blocks[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = &block->cloth;
                        s.write((sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                            * cloth->root_count * (cloth->nodes_count - 1ULL));
                    }
                else
                    for (uint32_t i = 0; i < ex->blocks_count; i++) {
                        obj_skin_block* block = &ex->blocks[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = &block->cloth;
                        s.write((sizeof(int32_t) + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t)))
                            * cloth->root_count* (cloth->nodes_count - 1ULL));
                    }
                s.align_write(0x10);

                cloth_nodes = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write((11 * sizeof(int32_t)) * cloth->root_count * (cloth->nodes_count - 1ULL));
                }
                s.align_write(0x10);

                cloth_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write(sizeof(uint16_t) + cloth->mesh_indices_count * sizeof(uint16_t));
                }
                s.align_write(0x10);

                cloth_backface_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    s.write(sizeof(uint16_t) + cloth->backface_mesh_indices_count * sizeof(uint16_t));
                }
                s.align_write(0x10);
            }
        }
    }

    if (sk->bones || sk->ex_data_init) {
        quicksort_string(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (std::string& i : strings) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i);
        }
    }
    s.align_write(0x10);

    std::vector<std::string> osage_names;
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        int64_t cls_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CLS");
        int64_t cns_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CNS");
        int64_t exp_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "EXP");
        int64_t mot_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "MOT");
        int64_t osg_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "OSG");

        if (ex->blocks_count > 0) {
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_cloth(&block->cloth,
                    s, strings, string_offsets, is_x, &cloth_mats, &cloth_root,
                    &cloth_nodes, &cloth_mesh_indices, &cloth_backface_mesh_indices);
                s.position_pop();
            }

            int64_t constraint_type_name_offsets[4];
            constraint_type_name_offsets[0] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Orientation");
            constraint_type_name_offsets[1] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Direction");
            constraint_type_name_offsets[2] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Position");
            constraint_type_name_offsets[3] = obj_skin_strings_get_string_offset(strings,
                string_offsets, "Distance");

            char** bone_names_ptr = ex->bone_names;
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_constraint(&block->constraint,
                    s, strings, string_offsets, bone_names_ptr, is_x, constraint_type_name_offsets);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_expression(&block->expression,
                    s, strings, string_offsets, bone_names_ptr, is_x);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_motion(&block->motion,
                    s, strings, string_offsets, is_x, bone_names_ptr,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_osage(&block->osage,
                    s, strings, string_offsets, is_x);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = &block->cloth;
                    obj_skin_strings_push_back_check(osage_names, cloth->mesh_name);
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = &block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_names_ptr, osage->external_name_index);
                }
            }

            s.position_push(exh.blocks_offset, SEEK_SET);
            if (!is_x) {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    switch (block->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        s.write_offset_f2(cls_offset, 0x20);
                        s.write_offset_f2(bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        s.write_offset_f2(cns_offset, 0x20);
                        s.write_offset_f2(bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        s.write_offset_f2(exp_offset, 0x20);
                        s.write_offset_f2(bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        s.write_offset_f2(mot_offset, 0x20);
                        s.write_offset_f2(bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        s.write_offset_f2(osg_offset, 0x20);
                        s.write_offset_f2(bhs[i].block_offset, 0x20);
                        break;
                    default:
                        s.write_offset_f2(0, 0);
                        s.write_offset_f2(0, 0);
                        break;
                    }
                }
                s.write_offset_f2(0, 0);
                s.write_offset_f2(0, 0);
            }
            else {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    switch (block->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        s.write_offset_x(cls_offset);
                        s.write_offset_x(bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        s.write_offset_x(cns_offset);
                        s.write_offset_x(bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        s.write_offset_x(exp_offset);
                        s.write_offset_x(bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        s.write_offset_x(mot_offset);
                        s.write_offset_x(bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        s.write_offset_x(osg_offset);
                        s.write_offset_x(bhs[i].block_offset);
                        break;
                    default:
                        s.write_offset_x(0);
                        s.write_offset_x(0);
                        break;
                    }
                }
                s.write_offset_x(0);
                s.write_offset_x(0);
            }
            s.position_pop();
            free(bhs);
        }
    }

    if (sk->bones) {
        s.position_push(sh.bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, sk->bones[i].name);
                s.write_offset_f2(str_offset, 0x20);
            }
        else
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, sk->bones[i].name);
                s.write_offset_x(str_offset);
            }
        s.position_pop();
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (ex->blocks_count > 0) {
            s.position_push(exh.bone_names_offset, SEEK_SET);
            if (!is_x)
                for (std::string& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_f2(str_offset, 0x20);
                }
            else
                for (std::string& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_x(str_offset);
                }
            s.position_pop();

            s.position_push(exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];
                s.write_uint32_t(osage_node->name_index);
                s.write_float_t(osage_node->length);
                s.write_uint32_t(0);
            }
            s.position_pop();

            s.position_push(exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_infos[i];
                s.write_uint32_t(osage_sibling_info->name_index);
                s.write_uint32_t(osage_sibling_info->sibling_name_index);
                s.write_float_t(osage_sibling_info->max_distance);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.position_pop();

            exh.osage_count = (int32_t)osage_names.size();
            exh.osage_count -= exh.cloth_count;
            s.position_push(exh.osage_names_offset, SEEK_SET);
            if (!is_x)
                for (std::string& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_f2(str_offset, 0x20);
                }
            else
                for (std::string& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_x(str_offset);
                }
            s.position_pop();
        }

        s.position_push(sh.ex_data_offset, SEEK_SET);
        if (!is_x) {
            s.write_int32_t(exh.osage_count);
            s.write_int32_t(ex->osage_nodes_count);
            s.write(0x04);
            s.write_offset_f2(exh.osage_nodes_offset, 0x20);
            s.write_offset_f2(exh.osage_names_offset, 0x20);
            s.write_offset_f2(exh.blocks_offset, 0x20);
            s.write_int32_t(exh.bone_names_count);
            s.write_offset_f2(exh.bone_names_offset, 0x20);
            s.write_offset_f2(exh.osage_sibling_infos_offset, 0x20);
            s.write_int32_t(exh.cloth_count);
            s.write_offset_f2(ex->reserved[0], 0x20);
            s.write_offset_f2(ex->reserved[1], 0x20);
            s.write_offset_f2(ex->reserved[2], 0x20);
            s.write_offset_f2(ex->reserved[3], 0x20);
            s.write_offset_f2(ex->reserved[4], 0x20);
            s.write_offset_f2(ex->reserved[5], 0x20);
            s.write_offset_f2(ex->reserved[6], 0x20);
        }
        else {
            s.write_int32_t(exh.osage_count);
            s.write_int32_t(ex->osage_nodes_count);
            s.write(0x08);
            s.write_offset_x(exh.osage_nodes_offset);
            s.write_offset_x(exh.osage_names_offset);
            s.write_offset_x(exh.blocks_offset);
            s.write_int32_t(exh.bone_names_count);
            s.write_offset_x(exh.bone_names_offset);
            s.write_offset_x(exh.osage_sibling_infos_offset);
            s.write_int32_t(exh.cloth_count);
            s.write_offset_x(ex->reserved[0]);
            s.write_offset_x(ex->reserved[1]);
            s.write_offset_x(ex->reserved[2]);
            s.write_offset_x(ex->reserved[3]);
            s.write_offset_x(ex->reserved[4]);
            s.write_offset_x(ex->reserved[5]);
            s.write_offset_x(ex->reserved[6]);
        }
        s.position_pop();
    }

    s.position_push(base_offset, SEEK_SET);
    if (!is_x) {
        s.write_offset_f2(sh.bone_ids_offset, 0x20);
        s.write_offset_f2(sh.bone_matrices_offset, 0x20);
        s.write_offset_f2(sh.bone_names_offset, 0x20);
        s.write_offset_f2(sh.ex_data_offset, 0x20);
        s.write_int32_t(sk->bones_count);
        s.write_offset_f2(sh.bone_parent_ids_offset, 0x20);
        s.write(0x0C);
    }
    else {
        s.write_offset_x(sh.bone_ids_offset);
        s.write_offset_x(sh.bone_matrices_offset);
        s.write_offset_x(sh.bone_names_offset);
        s.write_offset_x(sh.ex_data_offset);
        s.write_int32_t(sk->bones_count);
        s.write_offset_x(sh.bone_parent_ids_offset);
        s.write(0x18);
    }
    s.position_pop();

    oskn->enrs = e;
    oskn->pof = pof;
}

static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    int64_t mesh_name_offset = s.read_offset(header_length, is_x);
    b->mesh_name = s.read_utf8_string_null_terminated_offset(mesh_name_offset);

    int64_t backface_mesh_name_offset = s.read_offset(header_length, is_x);
    b->backface_mesh_name = s.read_utf8_string_null_terminated_offset(backface_mesh_name_offset);

    b->field_8 = s.read_int32_t_reverse_endianness();
    b->root_count = s.read_int32_t_reverse_endianness();
    b->nodes_count = s.read_int32_t_reverse_endianness();
    b->field_14 = s.read_int32_t_reverse_endianness();
    int64_t mats_offset = s.read_offset(header_length, is_x);
    int64_t root_offset = s.read_offset(header_length, is_x);

    b->root = 0;
    b->nodes = 0;
    b->mesh_indices = 0;
    b->backface_mesh_indices = 0;
    int64_t nodes_offset = s.read_offset(header_length, is_x);
    int64_t mesh_indices_offset = s.read_offset(header_length, is_x);
    int64_t backface_mesh_indices_offset = s.read_offset(header_length, is_x);
    int64_t skp_root_offset = s.read_uint32_t_reverse_endianness();
    b->reserved = s.read_uint32_t_reverse_endianness();

    if (mats_offset) {
        int32_t max_matrix_index = -1;

        s.position_push(root_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->root_count; i++) {
            s.read(0x28);
            for (uint32_t j = 0; j < 4; j++) {
                uint32_t name_offset = s.read_uint32_t();
                int32_t matrix_index = s.read_int32_t();
                if (name_offset&& max_matrix_index < matrix_index)
                    max_matrix_index = matrix_index;
            }
        }
        s.position_pop();

        b->mats_count = max_matrix_index > -1 ? max_matrix_index + 1 : 0;
        b->mats = force_malloc_s(mat4, b->mats_count);
        s.position_push(mats_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->mats_count; i++) {
            mat4& mat = b->mats[i];
            mat.row0.x = s.read_float_t_reverse_endianness();
            mat.row1.x = s.read_float_t_reverse_endianness();
            mat.row2.x = s.read_float_t_reverse_endianness();
            mat.row3.x = s.read_float_t_reverse_endianness();
            mat.row0.y = s.read_float_t_reverse_endianness();
            mat.row1.y = s.read_float_t_reverse_endianness();
            mat.row2.y = s.read_float_t_reverse_endianness();
            mat.row3.y = s.read_float_t_reverse_endianness();
            mat.row0.z = s.read_float_t_reverse_endianness();
            mat.row1.z = s.read_float_t_reverse_endianness();
            mat.row2.z = s.read_float_t_reverse_endianness();
            mat.row3.z = s.read_float_t_reverse_endianness();
            mat.row0.w = s.read_float_t_reverse_endianness();
            mat.row1.w = s.read_float_t_reverse_endianness();
            mat.row2.w = s.read_float_t_reverse_endianness();
            mat.row3.w = s.read_float_t_reverse_endianness();
        }
        s.position_pop();
    }

    if (root_offset) {
        s.position_push(root_offset, SEEK_SET);
        b->root = force_malloc_s(obj_skin_block_cloth_root, b->root_count);
        for (uint32_t i = 0; i < b->root_count; i++) {
            obj_skin_block_cloth_root* f = &b->root[i];
            f->trans.x = s.read_float_t_reverse_endianness();
            f->trans.y = s.read_float_t_reverse_endianness();
            f->trans.z = s.read_float_t_reverse_endianness();
            f->normal.x = s.read_float_t_reverse_endianness();
            f->normal.y = s.read_float_t_reverse_endianness();
            f->normal.z = s.read_float_t_reverse_endianness();
            f->field_18 = s.read_float_t_reverse_endianness();
            f->field_1C = s.read_int32_t_reverse_endianness();
            f->field_20 = s.read_int32_t_reverse_endianness();
            f->field_24 = s.read_int32_t_reverse_endianness();

            for (uint32_t j = 0; j < 4; j++)
                obj_modern_read_skin_block_cloth_root_bone_weight(&f->bone_weights[j],
                    s, header_length, str, is_x);
        }
        s.position_pop();
    }

    if (nodes_offset) {
        s.position_push(nodes_offset, SEEK_SET);
        b->nodes = force_malloc_s(obj_skin_block_cloth_node,
            b->root_count * (b->nodes_count - 1ULL));
        for (uint32_t i = 0; i < b->nodes_count - 1; i++)
            for (uint32_t j = 0; j < b->root_count; j++) {
                obj_skin_block_cloth_node* f = &b->nodes[i * b->root_count + j];
                f->flags = s.read_uint32_t_reverse_endianness();
                f->trans.x = s.read_float_t_reverse_endianness();
                f->trans.y = s.read_float_t_reverse_endianness();
                f->trans.z = s.read_float_t_reverse_endianness();
                f->trans_diff.x = s.read_float_t_reverse_endianness();
                f->trans_diff.y = s.read_float_t_reverse_endianness();
                f->trans_diff.z = s.read_float_t_reverse_endianness();
                f->length = s.read_float_t_reverse_endianness();
                f->field_20 = s.read_float_t_reverse_endianness();
                f->field_24 = s.read_float_t_reverse_endianness();
                f->field_28 = s.read_float_t_reverse_endianness();
            }
        s.position_pop();
    }

    if (mesh_indices_offset) {
        s.position_push(mesh_indices_offset, SEEK_SET);
        b->mesh_indices_count = s.read_uint16_t_reverse_endianness();
        b->mesh_indices = force_malloc_s(uint16_t, b->mesh_indices_count);
        s.read(b->mesh_indices, b->mesh_indices_count * sizeof(uint16_t));
        if (s.is_big_endian) {
            uint16_t* mesh_indices = b->mesh_indices;
            for (int32_t i = 0; i < b->mesh_indices_count; i++)
                mesh_indices[i] = reverse_endianness_uint16_t(mesh_indices[i]);
        }
        s.position_pop();
    }

    if (backface_mesh_indices_offset) {
        s.position_push(backface_mesh_indices_offset, SEEK_SET);
        b->backface_mesh_indices_count = s.read_uint16_t_reverse_endianness();
        b->backface_mesh_indices = force_malloc_s(uint16_t, b->backface_mesh_indices_count);
        s.read(b->backface_mesh_indices, b->backface_mesh_indices_count * sizeof(uint16_t));
        if (s.is_big_endian) {
            uint16_t* backface_mesh_indices = b->backface_mesh_indices;
            for (int32_t i = 0; i < b->backface_mesh_indices_count; i++)
                backface_mesh_indices[i] = reverse_endianness_uint16_t(backface_mesh_indices[i]);
        }
        s.position_pop();
    }

}

static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* mats_offset, int64_t* root_offset, int64_t* nodes_offset,
    int64_t* mesh_indices_offset, int64_t* backface_mesh_indices_offset) {
    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->mesh_name);
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->backface_mesh_name);

    if (!is_x) {
        s.write_offset_f2(mesh_name_offset, 0x20);
        s.write_offset_f2(backface_mesh_name_offset, 0x20);
        s.write_int32_t(b->field_8);
        s.write_int32_t(b->root_count);
        s.write_int32_t(b->nodes_count);
        s.write_int32_t(b->field_14);
        s.write_offset_f2(b->nodes_count ? *mats_offset : 0, 0x20);
        s.write_offset_f2(b->root ? *root_offset : 0, 0x20);
        s.write_offset_f2(b->nodes ? *nodes_offset : 0, 0x20);
        s.write_offset_f2(b->mesh_indices ? *mesh_indices_offset : 0, 0x20);
        s.write_offset_f2(b->backface_mesh_indices ? *backface_mesh_indices_offset : 0, 0x20);
        s.write_uint32_t(0);
        s.write_uint32_t(b->reserved);
    }
    else {
        s.write_offset_x(mesh_name_offset);
        s.write_offset_x(backface_mesh_name_offset);
        s.write_int32_t(b->field_8);
        s.write_int32_t(b->root_count);
        s.write_int32_t(b->nodes_count);
        s.write_int32_t(b->field_14);
        s.write_offset_x(b->nodes_count ? *mats_offset : 0);
        s.write_offset_x(b->root ? *root_offset : 0);
        s.write_offset_x(b->nodes ? *nodes_offset : 0);
        s.write_offset_x(b->mesh_indices ? *mesh_indices_offset : 0);
        s.write_offset_x(b->backface_mesh_indices ? *backface_mesh_indices_offset : 0);
        s.write_uint32_t(0);
        s.write_uint32_t(b->reserved);
    }

    if (b->mats) {
        s.position_push(*mats_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->mats_count; i++) {
            mat4& mat = b->mats[i];
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
            *mats_offset += sizeof(mat4);
        }
        s.position_pop();
    }

    if (b->root) {
        s.position_push(*root_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->root_count; i++) {
            obj_skin_block_cloth_root* f = &b->root[i];
            s.write_float_t(f->trans.x);
            s.write_float_t(f->trans.y);
            s.write_float_t(f->trans.z);
            s.write_float_t(f->normal.x);
            s.write_float_t(f->normal.y);
            s.write_float_t(f->normal.z);
            s.write_float_t(f->field_18);
            s.write_int32_t(f->field_1C);
            s.write_int32_t(f->field_20);
            s.write_int32_t(f->field_24);

            for (uint32_t j = 0; j < 4; j++)
                obj_modern_write_skin_block_cloth_root_bone_weight(&f->bone_weights[j],
                    s, strings, string_offsets, is_x);
        }
        s.position_pop();
        if (!is_x)
            *root_offset += (sizeof(int32_t) + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t)))
                * b->root_count * (b->nodes_count - 1ULL);
        else
            *root_offset += (sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                * b->root_count * (b->nodes_count - 1ULL);
    }

    if (b->nodes) {
        s.position_push(*nodes_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count - 1; i++)
            for (uint32_t j = 0; j < b->root_count; j++) {
                obj_skin_block_cloth_node* f = &b->nodes[i * b->root_count + j];
                s.write_uint32_t(f->flags);
                s.write_float_t(f->trans.x);
                s.write_float_t(f->trans.y);
                s.write_float_t(f->trans.z);
                s.write_float_t(f->trans_diff.x);
                s.write_float_t(f->trans_diff.y);
                s.write_float_t(f->trans_diff.z);
                s.write_float_t(f->length);
                s.write_float_t(f->field_20);
                s.write_float_t(f->field_24);
                s.write_float_t(f->field_28);
            }
        s.position_pop();
        *nodes_offset += (11 * sizeof(int32_t)) * b->root_count * (b->nodes_count - 1ULL);
    }

    if (b->mesh_indices) {
        s.position_push(*mesh_indices_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)b->mesh_indices_count);
        s.write(b->mesh_indices, b->mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
        *mesh_indices_offset += sizeof(uint16_t) + b->mesh_indices_count * sizeof(uint16_t);
    }

    if (b->backface_mesh_indices) {
        s.position_push(*backface_mesh_indices_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)b->backface_mesh_indices_count);
        s.write(b->backface_mesh_indices, b->backface_mesh_indices_count * sizeof(uint16_t));
        s.position_pop();
        *backface_mesh_indices_offset += sizeof(uint16_t) + b->backface_mesh_indices_count * sizeof(uint16_t);
    }
}

static void obj_modern_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    int64_t bone_name_offset = s.read_offset(header_length, is_x);
    sub->bone_name = s.read_utf8_string_null_terminated_offset(bone_name_offset);
    sub->weight = s.read_float_t_reverse_endianness();
    sub->matrix_index = s.read_uint32_t_reverse_endianness();
    sub->reserved = s.read_uint32_t_reverse_endianness();
    if (is_x)
        s.read(0, 0x04);
}

static void obj_modern_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* sub,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, sub->bone_name);
    s.write_offset(bone_name_offset, 0x20, is_x);
    s.write_float_t(sub->weight);
    s.write_uint32_t(sub->matrix_index);
    s.write_uint32_t(sub->reserved);
    if (is_x)
        s.write(0x04);
}

static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t type_offset = s.read_offset(header_length, is_x);
    char* type = s.read_utf8_string_null_terminated_offset(type_offset);

    int64_t name_offset = s.read_offset(header_length, is_x);
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->coupling = (obj_skin_block_constraint_coupling)s.read_int32_t_reverse_endianness();

    int64_t source_node_name_offset = s.read_offset(header_length, is_x);
    b->source_node_name = s.read_utf8_string_null_terminated_offset(source_node_name_offset);

    if (!str_utils_compare(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = s.read_float_t_reverse_endianness();
        b->orientation.offset.y = s.read_float_t_reverse_endianness();
        b->orientation.offset.z = s.read_float_t_reverse_endianness();
    }
    else if (!str_utils_compare(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_modern_read_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, header_length, str, is_x);
        b->direction.align_axis.x = s.read_float_t_reverse_endianness();
        b->direction.align_axis.y = s.read_float_t_reverse_endianness();
        b->direction.align_axis.z = s.read_float_t_reverse_endianness();
        b->direction.target_offset.x = s.read_float_t_reverse_endianness();
        b->direction.target_offset.y = s.read_float_t_reverse_endianness();
        b->direction.target_offset.z = s.read_float_t_reverse_endianness();
    }
    else if (!str_utils_compare(type, "Position")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        obj_modern_read_skin_block_constraint_up_vector_old(&b->position.up_vector,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, header_length, str, is_x);
    }
    else if (!str_utils_compare(type, "Distance")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        obj_modern_read_skin_block_constraint_up_vector_old(&b->distance.up_vector,
            s, header_length, str, is_x);
        b->distance.distance = s.read_float_t_reverse_endianness();
        obj_modern_read_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, header_length, str, is_x);
    }
    else
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    free(type);
}

static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names,
    bool is_x, int64_t* offsets) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    int64_t type_offset = 0;
    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        type_offset = offsets[0];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        type_offset = offsets[1];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        type_offset = offsets[2];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        type_offset = offsets[3];
        break;
    }
    s.write_offset(type_offset, 0x20, is_x);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    s.write_offset(name_offset, 0x20, is_x);

    s.write_int32_t(b->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->source_node_name);
    s.write_offset(source_node_name_offset, 0x20, is_x);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        s.write_float_t(b->orientation.offset.x);
        s.write_float_t(b->orientation.offset.y);
        s.write_float_t(b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_modern_write_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, strings, string_offsets, is_x);
        s.write_float_t(b->direction.align_axis.x);
        s.write_float_t(b->direction.align_axis.y);
        s.write_float_t(b->direction.align_axis.z);
        s.write_float_t(b->direction.target_offset.x);
        s.write_float_t(b->direction.target_offset.y);
        s.write_float_t(b->direction.target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_modern_write_skin_block_constraint_up_vector_old(&b->position.up_vector,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, strings, string_offsets, is_x);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_modern_write_skin_block_constraint_up_vector_old(&b->distance.up_vector,
            s, strings, string_offsets, is_x);
        s.write_float_t(b->distance.distance);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets, is_x);
        break;
    }
}

static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    ap->affected_by_orientation = s.read_int32_t_reverse_endianness() != 0;
    ap->affected_by_scaling = s.read_int32_t_reverse_endianness() != 0;
    ap->offset.x = s.read_float_t_reverse_endianness();
    ap->offset.y = s.read_float_t_reverse_endianness();
    ap->offset.z = s.read_float_t_reverse_endianness();
}

static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    s.write_int32_t(ap->affected_by_orientation ? 1 : 0);
    s.write_int32_t(ap->affected_by_scaling ? 1 : 0);
    s.write_float_t(ap->offset.x);
    s.write_float_t(ap->offset.y);
    s.write_float_t(ap->offset.z);
}

static void obj_modern_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    up->active = s.read_int32_t_reverse_endianness() != 0;
    up->roll = s.read_float_t_reverse_endianness();
    up->affected_axis.x = s.read_float_t_reverse_endianness();
    up->affected_axis.y = s.read_float_t_reverse_endianness();
    up->affected_axis.z = s.read_float_t_reverse_endianness();
    up->point_at.x = s.read_float_t_reverse_endianness();
    up->point_at.y = s.read_float_t_reverse_endianness();
    up->point_at.z = s.read_float_t_reverse_endianness();

    int64_t name_offset = s.read_offset(header_length, is_x);
    up->name = s.read_utf8_string_null_terminated_offset(name_offset);
}

static void obj_modern_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    s.write_int32_t(up->active ? 1 : 0);
    s.write_float_t(up->roll);
    s.write_float_t(up->affected_axis.x);
    s.write_float_t(up->affected_axis.y);
    s.write_float_t(up->affected_axis.z);
    s.write_float_t(up->point_at.x);
    s.write_float_t(up->point_at.y);
    s.write_float_t(up->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, up->name);
    s.write_offset(name_offset, 0x20, is_x);
}

static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t name_offset = s.read_offset(header_length, is_x);
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->expressions_count = s.read_int32_t_reverse_endianness();
    b->expressions_count = min(b->expressions_count, 9);
    if (!is_x)
        for (uint32_t i = 0; i < b->expressions_count; i++) {
            int64_t expression_offset = s.read_offset_f2(header_length);
            if (expression_offset)
                b->expressions[i] = s.read_utf8_string_null_terminated_offset(expression_offset);
        }
    else
        for (uint32_t i = 0; i < b->expressions_count; i++) {
            int64_t expression_offset = s.read_offset_x();
            if (expression_offset)
                b->expressions[i] = s.read_utf8_string_null_terminated_offset(expression_offset);
        }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        b->expressions[i] = 0;
}

static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, bool is_x) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    s.write_offset(name_offset, 0x20, is_x);

    s.write_int32_t(min(b->expressions_count, 9));
    if (!is_x) {
        for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, b->expressions[i]);
            s.write_offset_f2((int32_t)expression_offset, 0x20);
        }

        for (uint32_t i = b->expressions_count; i < 9; i++)
            s.write_offset_f2(0, 0x20);
    }
    else {
        for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, b->expressions[i]);
            s.write_offset_x(expression_offset);
        }

        for (uint32_t i = b->expressions_count; i < 9; i++)
            s.write_offset_x(0);
    }
}

static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t name_offset = s.read_offset(header_length, is_x);
    b->nodes_count = s.read_int32_t_reverse_endianness();
    int64_t bone_names_offset = s.read_offset(header_length, is_x);
    int64_t bone_matrices_offset = s.read_offset(header_length, is_x);

    if (!is_x) {
        b->name_index = 0;
        char* name = s.read_utf8_string_null_terminated_offset(name_offset);
        for (char** i = str; *i; i++)
            if (!str_utils_compare(name, *i)) {
                b->name_index = 0x8000 | (int32_t)(i - str);
                break;
            }
        free(name);
    }
    else
        b->name = s.read_utf8_string_null_terminated_offset(name_offset);

    b->nodes = 0;

    if (!b->nodes_count)
        return;

    b->nodes = force_malloc_s(obj_skin_motion_node, b->nodes_count);

    if (bone_names_offset) {
        s.position_push(bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            b->nodes[i].name_index = s.read_uint32_t_reverse_endianness();
        s.position_pop();
    }

    if (bone_matrices_offset) {
        s.position_push(bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4& mat = b->nodes[i].inv_bind_pose_mat;
            mat.row0.x = s.read_float_t_reverse_endianness();
            mat.row1.x = s.read_float_t_reverse_endianness();
            mat.row2.x = s.read_float_t_reverse_endianness();
            mat.row3.x = s.read_float_t_reverse_endianness();
            mat.row0.y = s.read_float_t_reverse_endianness();
            mat.row1.y = s.read_float_t_reverse_endianness();
            mat.row2.y = s.read_float_t_reverse_endianness();
            mat.row3.y = s.read_float_t_reverse_endianness();
            mat.row0.z = s.read_float_t_reverse_endianness();
            mat.row1.z = s.read_float_t_reverse_endianness();
            mat.row2.z = s.read_float_t_reverse_endianness();
            mat.row3.z = s.read_float_t_reverse_endianness();
            mat.row0.w = s.read_float_t_reverse_endianness();
            mat.row1.w = s.read_float_t_reverse_endianness();
            mat.row2.w = s.read_float_t_reverse_endianness();
            mat.row3.w = s.read_float_t_reverse_endianness();
        }
        s.position_pop();
    }
}

static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    int64_t name_offset;
    if (!is_x)
        name_offset = obj_skin_strings_get_string_offset_by_index(strings,
            string_offsets, bone_names, b->name_index);
    else
        name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, b->name);
    s.write_offset(name_offset, 0x20, is_x);
    s.write_int32_t(b->nodes_count);
    s.write_offset(b->nodes ? *bone_names_offset : 0, 0x20, is_x);
    s.write_offset(b->nodes ? *bone_matrices_offset : 0, 0x20, is_x);

    if (b->nodes) {
        s.position_push(*bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            s.write_uint32_t(b->nodes[i].name_index);
        s.position_pop();
        *bone_names_offset += b->nodes_count * sizeof(uint32_t);

        s.position_push(*bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4& mat = b->nodes[i].inv_bind_pose_mat;
            s.write_float_t(mat.row0.x);
            s.write_float_t(mat.row1.x);
            s.write_float_t(mat.row2.x);
            s.write_float_t(mat.row3.x);
            s.write_float_t(mat.row0.y);
            s.write_float_t(mat.row1.y);
            s.write_float_t(mat.row2.y);
            s.write_float_t(mat.row3.y);
            s.write_float_t(mat.row0.z);
            s.write_float_t(mat.row1.z);
            s.write_float_t(mat.row2.z);
            s.write_float_t(mat.row3.z);
            s.write_float_t(mat.row0.w);
            s.write_float_t(mat.row1.w);
            s.write_float_t(mat.row2.w);
            s.write_float_t(mat.row3.w);
        }
        s.position_pop();
        *bone_matrices_offset += b->nodes_count * sizeof(mat4);
    }
}

static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    int64_t parent_name = s.read_offset(header_length, is_x);
    b->parent_name = s.read_utf8_string_null_terminated_offset(parent_name);

    b->position.x = s.read_float_t_reverse_endianness();
    b->position.y = s.read_float_t_reverse_endianness();
    b->position.z = s.read_float_t_reverse_endianness();
    b->rotation.x = s.read_float_t_reverse_endianness();
    b->rotation.y = s.read_float_t_reverse_endianness();
    b->rotation.z = s.read_float_t_reverse_endianness();
    b->scale.x = s.read_float_t_reverse_endianness();
    b->scale.y = s.read_float_t_reverse_endianness();
    b->scale.z = s.read_float_t_reverse_endianness();
    if (is_x)
        s.read(0, 0x04);
}

static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, b->parent_name);
    s.write_offset(parent_name_offset, 0x20, is_x);

    s.write_float_t(b->position.x);
    s.write_float_t(b->position.y);
    s.write_float_t(b->position.z);
    s.write_float_t(b->rotation.x);
    s.write_float_t(b->rotation.y);
    s.write_float_t(b->rotation.z);
    s.write_float_t(b->scale.x);
    s.write_float_t(b->scale.y);
    s.write_float_t(b->scale.z);
    if (is_x)
        s.write(0x04);
}

static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream& s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    b->start_index = s.read_int32_t_reverse_endianness();
    b->count = s.read_int32_t_reverse_endianness();
    b->external_name_index = s.read_uint32_t_reverse_endianness();
    b->name_index = s.read_uint32_t_reverse_endianness();

    if (!is_x)
        s.read(0, 0x14);
    else {
        s.read(0, 0x08);
        int64_t motion_node_name_offset = s.read_offset_x();
        b->motion_node_name = s.read_utf8_string_null_terminated_offset(motion_node_name_offset);
        s.read(0, 0x18);
    }

    b->nodes = 0;
    b->nodes_count = 0;
}

static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream& s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    s.write_int32_t(b->start_index);
    s.write_int32_t(b->count);
    s.write_uint32_t(b->external_name_index);
    s.write_uint32_t(b->name_index);

    if (!is_x)
        s.write(0x14);
    else {
        s.write(0x08);
        int64_t motion_node_name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, b->motion_node_name);
        s.write_offset_x(motion_node_name_offset);
        s.write(0x18);
    }
}

static void obj_modern_read_vertex(obj* obj, stream& s, int64_t* vertex, obj_mesh* mesh,
    const uint32_t vertex_flags, uint32_t num_vertex, uint32_t size_vertex) {
    obj_vertex_format vertex_format = (obj_vertex_format)(OBJ_VERTEX_POSITION
        | OBJ_VERTEX_NORMAL
        | OBJ_VERTEX_TANGENT
        | OBJ_VERTEX_TEXCOORD0
        | OBJ_VERTEX_TEXCOORD1
        | OBJ_VERTEX_COLOR0);

    if (vertex_flags == 0x06)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD2);
    else if (vertex_flags == 0x0A)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3);
    else if (vertex_flags == 0x04)
        enum_or(vertex_format, OBJ_VERTEX_BONE_DATA);

    int64_t vtx_offset = vertex[13];

    bool has_tangents = false;
    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, num_vertex);
    for (uint32_t i = 0; i < num_vertex; i++) {
        s.set_position(vtx_offset + (int64_t)size_vertex * i, SEEK_SET);

        vec3 position;
        position.x = s.read_float_t_reverse_endianness();
        position.y = s.read_float_t_reverse_endianness();
        position.z = s.read_float_t_reverse_endianness();
        vtx[i].position = position;

        vec3 normal;
        normal.x = (float_t)s.read_int16_t_reverse_endianness();
        normal.y = (float_t)s.read_int16_t_reverse_endianness();
        normal.z = (float_t)s.read_int16_t_reverse_endianness();
        s.read(0, 0x02);
        vec3_div_min_max_scalar(normal, -32768.0f, 32767.0f, vtx[i].normal);

        vec4 tangent;
        tangent.x = (float_t)s.read_int16_t_reverse_endianness();
        tangent.y = (float_t)s.read_int16_t_reverse_endianness();
        tangent.z = (float_t)s.read_int16_t_reverse_endianness();
        tangent.w = (float_t)s.read_int16_t_reverse_endianness();
        vec4_div_min_max_scalar(tangent, -32768.0f, 32767.0f, tangent);
        vtx[i].tangent = tangent;

        vec2h texcoord0;
        texcoord0.x = s.read_half_t_reverse_endianness();
        texcoord0.y = s.read_half_t_reverse_endianness();
        vec2h_to_vec2(texcoord0, vtx[i].texcoord0);

        vec2h texcoord1;
        texcoord1.x = s.read_half_t_reverse_endianness();
        texcoord1.y = s.read_half_t_reverse_endianness();
        vec2h_to_vec2(texcoord1, vtx[i].texcoord1);

        if (vertex_flags == 0x06) {
            vec2h texcoord2;
            texcoord2.x = s.read_half_t_reverse_endianness();
            texcoord2.y = s.read_half_t_reverse_endianness();
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);
        }
        else if (vertex_flags == 0x0A) {
            vec2h texcoord2;
            texcoord2.x = s.read_half_t_reverse_endianness();
            texcoord2.y = s.read_half_t_reverse_endianness();
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);

            vec2h texcoord3;
            texcoord3.x = s.read_half_t_reverse_endianness();
            texcoord3.y = s.read_half_t_reverse_endianness();
            vec2h_to_vec2(texcoord3, vtx[i].texcoord3);
        }

        vec4h color;
        color.x = s.read_half_t_reverse_endianness();
        color.y = s.read_half_t_reverse_endianness();
        color.z = s.read_half_t_reverse_endianness();
        color.w = s.read_half_t_reverse_endianness();
        vec4h_to_vec4(color, vtx[i].color0);

        if (vertex_flags == 0x04) {
            vec4 bone_weight;
            bone_weight.x = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.y = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.z = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.w = (float_t)s.read_int16_t_reverse_endianness();
            vec4_div_min_max_scalar(bone_weight, -32768.0f, 32767.0f, bone_weight);
            vtx[i].bone_weight = bone_weight;

            vec4i bone_index;
            bone_index.x = (int32_t)s.read_uint8_t();
            bone_index.y = (int32_t)s.read_uint8_t();
            bone_index.z = (int32_t)s.read_uint8_t();
            bone_index.w = (int32_t)s.read_uint8_t();
            if (bone_index.x == 0xFF)
                bone_index.x = -1;
            if (bone_index.y == 0xFF)
                bone_index.y = -1;
            if (bone_index.z == 0xFF)
                bone_index.z = -1;
            if (bone_index.w == 0xFF)
                bone_index.w = -1;
            vtx[i].bone_index = bone_index;
        }

        if (memcmp(&vtx[i].tangent, &vec4_null, sizeof(vec4)))
            has_tangents = true;
    }

    if (!has_tangents)
        enum_and(vertex_format, ~OBJ_VERTEX_TANGENT);

    mesh->vertex = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
}

static void obj_modern_write_vertex(obj* obj, stream& s, bool is_x,
    int64_t* vertex, obj_mesh* mesh, uint32_t* vertex_flags,
    uint32_t* num_vertex, uint32_t* size_vertex, f2_struct* ovtx) {

    obj_vertex_data* vtx = mesh->vertex;
    uint32_t _num_vertex = mesh->num_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    uint32_t _vertex_flags = 0x02;
    uint32_t _size_vertex = 0x2C;
    uint32_t enrs_se3_rc = 12;
    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        _vertex_flags = 0x04;
        _size_vertex += 0x0C;
        enrs_se3_rc += 4;
    }
    else if ((vertex_format & (OBJ_VERTEX_TEXCOORD2 & OBJ_VERTEX_TEXCOORD3))
        == (OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3)) {
        _vertex_flags = 0x0A;
        _size_vertex += 0x08;
        enrs_se3_rc += 4;
    }
    else if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
        _vertex_flags = 0x06;
        _size_vertex += 0x04;
        enrs_se3_rc += 2;
    }

    uint32_t off = 0;
    enrs* e = &ovtx->enrs;
    enrs_entry ee;
    bool add_enrs = true;
    if (e->vec.size() > 0) {
        off = (uint32_t)((size_t)e->vec.back().size * e->vec.back().repeat_count);
        if (e->vec.back().sub.begin()[2].repeat_count == enrs_se3_rc) {
            e->vec.back().repeat_count += _num_vertex;
            add_enrs = false;
        }
    }

    if (add_enrs) {
        ee = { off, 3, (uint32_t)_size_vertex, (uint32_t)_num_vertex };
        ee.append(0, 3, ENRS_DWORD);
        ee.append(0, 3, ENRS_WORD);
        ee.append(2, enrs_se3_rc, ENRS_WORD);
        e->vec.push_back(ee);
    }

    bool has_tangents = false;
    vertex[13] = s.get_position();
    for (uint32_t i = 0; i < _num_vertex; i++) {
        vec3 position = vtx[i].position;
        s.write_float_t(position.x);
        s.write_float_t(position.y);
        s.write_float_t(position.z);

        vec3 normal;
        vec3_mult_min_max_scalar(vtx[i].normal, -32768.0f, 32767.0f, normal);
        s.write_int16_t((int16_t)roundf(normal.x));
        s.write_int16_t((int16_t)roundf(normal.y));
        s.write_int16_t((int16_t)roundf(normal.z));
        s.write(0x02);

        vec4 tangent = vtx[i].tangent;
        vec4_mult_min_max_scalar(tangent, -32768.0f, 32767.0f, tangent);
        s.write_int16_t((int16_t)roundf(tangent.x));
        s.write_int16_t((int16_t)roundf(tangent.y));
        s.write_int16_t((int16_t)roundf(tangent.z));
        s.write_int16_t((int16_t)roundf(tangent.w));

        vec2h texcoord0;
        vec2_to_vec2h(vtx[i].texcoord0, texcoord0);
        s.write_half_t(texcoord0.x);
        s.write_half_t(texcoord0.y);

        vec2h texcoord1;
        vec2_to_vec2h(vtx[i].texcoord1, texcoord1);
        s.write_half_t(texcoord1.x);
        s.write_half_t(texcoord1.y);

        if (_vertex_flags == 0x06) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            s.write_half_t(texcoord2.x);
            s.write_half_t(texcoord2.y);
        }
        else if (_vertex_flags == 0x0A) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            s.write_half_t(texcoord2.x);
            s.write_half_t(texcoord2.y);

            vec2h texcoord3;
            vec2_to_vec2h(vtx[i].texcoord3, texcoord3);
            s.write_half_t(texcoord3.x);
            s.write_half_t(texcoord3.y);
        }

        vec4h color;
        vec4_to_vec4h(vtx[i].color0, color);
        s.write_half_t(color.x);
        s.write_half_t(color.y);
        s.write_half_t(color.z);
        s.write_half_t(color.w);

        if (_vertex_flags == 0x04) {
            vec4 bone_weight = vtx[i].bone_weight;
            vec4_mult_min_max_scalar(bone_weight, -32768.0f, 32767.0f, bone_weight);
            s.write_int16_t((int16_t)bone_weight.x);
            s.write_int16_t((int16_t)bone_weight.y);
            s.write_int16_t((int16_t)bone_weight.z);
            s.write_int16_t((int16_t)bone_weight.w);

            vec4u8 bone_index;
            vec4_to_vec4u8(vtx[i].bone_index, bone_index)
            s.write_uint8_t(bone_index.x);
            s.write_uint8_t(bone_index.y);
            s.write_uint8_t(bone_index.z);
            s.write_uint8_t(bone_index.w);
        }
    }

    *vertex_flags = _vertex_flags;
    *num_vertex = _num_vertex;
    *size_vertex = _size_vertex;
}

inline static void obj_skin_block_node_free(obj_skin_block_node* b) {
    free(b->parent_name);
}

inline static uint32_t obj_skin_strings_get_string_index(std::vector<std::string>& vec, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return (uint32_t)(0x8000 | &i - vec.data());
    return 0x8000;
}

inline static int64_t obj_skin_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return vec_off[&i - vec.data()];
    return 0;
}

inline static int64_t obj_skin_strings_get_string_offset_by_index(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, char** strings, uint32_t index) {
    if (~index & 0x8000)
        return 0;

    char* str = strings[index & 0x7FFF];
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return vec_off[&i - vec.data()];
    return 0;
}

inline static void obj_skin_strings_push_back_check(std::vector<std::string>& vec, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return;

    vec.push_back(std::string(str, len));
}

inline static void obj_skin_strings_push_back_check_by_index(std::vector<std::string>& vec,
    char** strings, uint32_t index) {
    if (~index & 0x8000)
        return;

    char* str = strings[index & 0x7FFF];
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return;

    vec.push_back(std::string(str, len));
}
