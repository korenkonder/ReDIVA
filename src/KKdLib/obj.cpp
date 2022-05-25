/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "obj.hpp"
#include <vector>
#include "f2/struct.hpp"
#include "io/path.h"
#include "io/stream.h"
#include "half_t.h"
#include "hash.hpp"
#include "sort.hpp"
#include "str_utils.h"

enum obj_vertex_format_file {
    OBJ_VERTEX_FORMAT_FILE_POSITION       = 0x00000001,
    OBJ_VERTEX_FORMAT_FILE_NORMAL         = 0x00000002,
    OBJ_VERTEX_FORMAT_FILE_TANGENT        = 0x00000004,
    OBJ_VERTEX_FORMAT_FILE_BINORMAL       = 0x00000008,
    OBJ_VERTEX_FORMAT_FILE_TEXCOORD0      = 0x00000010,
    OBJ_VERTEX_FORMAT_FILE_TEXCOORD1      = 0x00000020,
    OBJ_VERTEX_FORMAT_FILE_TEXCOORD2      = 0x00000040,
    OBJ_VERTEX_FORMAT_FILE_TEXCOORD3      = 0x00000080,
    OBJ_VERTEX_FORMAT_FILE_COLOR0         = 0x00000100,
    OBJ_VERTEX_FORMAT_FILE_COLOR1         = 0x00000200,
    OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT    = 0x00000400,
    OBJ_VERTEX_FORMAT_FILE_BONE_INDEX     = 0x00000800,
    OBJ_VERTEX_FORMAT_FILE_UNKNOWN        = 0x00001000,
    OBJ_VERTEX_FORMAT_FILE_MODERN_STORAGE = 0x80000000,
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
static void obj_set_classic_read_inner(obj_set* os, stream* s);
static void obj_set_classic_write_inner(obj_set* os, stream* s);
static void obj_classic_read_index(obj* obj, stream* s, obj_sub_mesh* sub_mesh);
static void obj_classic_write_index(obj* obj, stream* s, obj_sub_mesh* sub_mesh);
static void obj_classic_read_model(obj* obj, stream* s, int64_t base_offset);
static void obj_classic_write_model(obj* obj, stream* s, int64_t base_offset);
static void obj_classic_read_skin(obj* obj, stream* s, int64_t base_offset);
static void obj_classic_write_skin(obj* obj, stream* s, int64_t base_offset);
static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    int64_t* field_18_offset, int64_t* field_1C_offset, int64_t* field_20_offset,
    int64_t* field_24_offset, int64_t* field_28_offset);
static void obj_classic_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, char** str);
static void obj_classic_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, char** str);
static void obj_classic_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, int64_t* offsets);
static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, char** str);
static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, char** str);
static void obj_classic_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names);
static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset);
static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, char** str);
static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, int64_t* nodes_offset);
static void obj_classic_read_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t num_vertex, obj_vertex_format_file vertex_format_file);
static void obj_classic_write_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t* num_vertex, obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex);
static void obj_set_modern_read_inner(obj_set* os, stream* s);
static void obj_set_modern_write_inner(obj_set* os, stream* s);
static void obj_modern_read_index(obj* obj, stream* s,
    obj_sub_mesh* sub_mesh);
static void obj_modern_write_index(obj* obj, stream* s, bool is_x,
    obj_sub_mesh* sub_mesh, f2_struct* ovtx);
static void obj_modern_read_model(obj* obj, stream* s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx);
static void obj_modern_write_model(obj* obj, stream* s,
    int64_t base_offset, bool is_x, f2_struct* omdl);
static void obj_modern_read_skin(obj* obj, stream* s, int64_t base_offset,
    uint32_t header_length, bool is_x);
static void obj_modern_write_skin(obj* obj, stream* s,
    int64_t base_offset, bool is_x, f2_struct* oskn);
static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* field_18_offset, int64_t* field_1C_offset, int64_t* field_20_offset,
    int64_t* field_24_offset, int64_t* field_28_offset);
static void obj_modern_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names,
    bool is_x, int64_t* offsets);
static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, bool is_x);
static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset);
static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, uint32_t header_length, char** str, bool is_x);
static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    const uint32_t attrib_flags, uint32_t num_vertex, uint32_t size_vertex);
static void obj_modern_write_vertex(obj* obj, stream* s, bool is_x,
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

obj_set::obj_set() : ready(), modern(), is_x(), obj_data(),
obj_num(), tex_id_data(), tex_id_num(), reserved() {

}

obj_set::~obj_set() {
    for (uint32_t i = 0; i < obj_num; i++) {
        obj* obj = &obj_data[i];
        for (uint32_t j = 0; j < obj->skin.bones_count; j++)
            string_free(&obj->skin.bones[j].name);
        free(obj->skin.bones);

        for (uint32_t j = 0; j < obj->skin.ex_data.blocks_count; j++) {
            obj_skin_block* block = &obj->skin.ex_data.blocks[j];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &block->cloth;
                string_free(&cloth->mesh_name);
                string_free(&cloth->backface_mesh_name);

                for (uint32_t k = 0; k < cloth->count; k++) {
                    obj_skin_block_cloth_field_1C* sub = &cloth->field_1C[k];
                    for (uint32_t l = 0; l < 4; l++)
                        string_free(&sub->sub_data[l].bone_name);
                }
                free(cloth->field_1C);
                free(cloth->field_20);
                free(cloth->field_24);
                free(cloth->field_28);
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_block_node_free(&constraint->base);
                string_free(&constraint->source_node_name);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                obj_skin_block_node_free(&expression->base);
                for (uint32_t k = 0; k < 9; k++)
                    string_free(&expression->expressions[k]);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_block_node_free(&motion->base);
                if (is_x)
                    string_free(&motion->name);
                free(motion->nodes);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_block_node_free(&osage->base);
                free(osage->root.coli);
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
        string_free(&obj->name);
    }
    free(obj_data);
    free(tex_id_data);
}

void obj_set::pack_file(void** data, size_t* size) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        obj_set_classic_write_inner(this, &s);
    else
        obj_set_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, size);
    io_free(&s);
}

void obj_set::unpack_file(void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    stream s;
    io_open(&s, data, size);
    if (!modern)
        obj_set_classic_read_inner(this, &s);
    else
        obj_set_modern_read_inner(this, &s);
    io_free(&s);
}

static void obj_material_texture_enrs_table_init() {
    if (!obj_material_texture_enrs_table_initialized) {
        stream s;
        io_open(&s, (void*)&obj_material_texture_enrs_table_bin,
            sizeof(obj_material_texture_enrs_table_bin));
        obj_material_texture_enrs_table.read(&s);
        io_free(&s);
        atexit(obj_material_texture_enrs_table_free);
        obj_material_texture_enrs_table_initialized = true;
    }
}

static void obj_material_texture_enrs_table_free(void) {
    obj_material_texture_enrs_table.vec.clear();
    obj_material_texture_enrs_table.vec.shrink_to_fit();
    obj_material_texture_enrs_table_initialized = false;
}

static void obj_set_classic_read_inner(obj_set* os, stream* s) {
    uint32_t version = io_read_uint32_t(s);
    if (version != 0x05062500) {
        os->is_x = false;
        os->modern = false;
        os->ready = false;
        return;
    }

    obj_set_header osh = {};
    os->obj_num = io_read_int32_t(s);
    osh.last_obj_id = io_read_int32_t(s);
    osh.obj_data = io_read_int32_t(s);
    osh.obj_skin_data = io_read_int32_t(s);
    osh.obj_name_data = io_read_int32_t(s);
    osh.obj_id_data = io_read_int32_t(s);
    osh.tex_id_data = io_read_int32_t(s);
    os->tex_id_num = io_read_int32_t(s);
    os->reserved[0] = io_read_uint32_t(s);
    os->reserved[1] = io_read_uint32_t(s);

    uint32_t count = os->obj_num;
    os->obj_data = force_malloc_s(obj, os->obj_num);

    int32_t* obj_datas = 0;
    if (osh.obj_data) {
        obj_datas = force_malloc_s(int32_t, count);
        io_set_position(s, osh.obj_data, SEEK_SET);
        for (uint32_t i = 0; i < count; i++)
            obj_datas[i] = io_read_int32_t(s);
    }

    int32_t* obj_skin_datas = 0;
    if (osh.obj_skin_data) {
        obj_skin_datas = force_malloc_s(int32_t, count);
        io_set_position(s, osh.obj_skin_data, SEEK_SET);
        for (uint32_t i = 0; i < count; i++)
            obj_skin_datas[i] = io_read_int32_t(s);
    }

    int32_t* obj_name_datas = 0;
    if (osh.obj_name_data) {
        obj_name_datas = force_malloc_s(int32_t, count);
        io_set_position(s, osh.obj_name_data, SEEK_SET);
        for (uint32_t i = 0; i < count; i++)
            obj_name_datas[i] = io_read_int32_t(s);
    }

    if (osh.obj_data) {
        for (uint32_t i = 0; i < count; i++) {
            obj* obj = &os->obj_data[i];
            if (obj_datas[i])
                obj_classic_read_model(obj, s, obj_datas[i]);

            if (osh.obj_skin_data && obj_skin_datas[i])
                obj_classic_read_skin(obj, s, obj_skin_datas[i]);

            if (osh.obj_name_data && obj_name_datas[i])
                io_read_string_null_terminated_offset(s, obj_name_datas[i], &obj->name);
        }

        io_set_position(s, osh.obj_id_data, SEEK_SET);
        for (uint32_t i = 0; i < count; i++)
            os->obj_data[i].id = io_read_uint32_t(s);
    }

    if (osh.tex_id_data) {
        os->tex_id_data = force_malloc_s(uint32_t, os->tex_id_num);
        io_set_position(s, osh.tex_id_data, SEEK_SET);
        for (uint32_t i = 0; i < os->tex_id_num; i++)
            os->tex_id_data[i] = io_read_uint32_t(s);
    }

    free(obj_datas);
    free(obj_skin_datas);
    free(obj_name_datas);

    os->is_x = false;
    os->modern = false;
    os->ready = true;
}

static void obj_set_classic_write_inner(obj_set* os, stream* s) {
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    obj_set_header osh = {};
    osh.last_obj_id = -1;

    uint32_t count = os->obj_num;

    osh.obj_data = io_get_position(s);
    for (uint32_t i = 0; i < count; i++)
        io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    int32_t* obj_datas = force_malloc_s(int32_t, count);
    for (uint32_t i = 0; i < count; i++) {
        obj_datas[i] = (int32_t)io_get_position(s);
        obj_classic_write_model(&os->obj_data[i], s, obj_datas[i]);
    }
    io_align_write(s, 0x10);

    io_position_push(s, osh.obj_data, SEEK_SET);
    for (uint32_t i = 0; i < count; i++)
        io_write_int32_t(s, (int32_t)obj_datas[i]);
    io_position_pop(s);
    free(obj_datas);

    osh.obj_id_data = io_get_position(s);
    for (uint32_t i = 0; i < count; i++) {
        int32_t object_id = os->obj_data[i].id;
        io_write_uint32_t(s, object_id);
        if (osh.last_obj_id < object_id)
            osh.last_obj_id = object_id;
    }
    io_align_write(s, 0x10);

    int32_t* obj_name_datas = force_malloc_s(int32_t, count);
    for (uint32_t i = 0; i < count; i++) {
        obj_name_datas[i] = (int32_t)io_get_position(s);
        io_write_string_null_terminated(s, &os->obj_data[i].name);
    }
    io_align_write(s, 0x10);

    osh.obj_name_data = io_get_position(s);
    for (uint32_t i = 0; i < count; i++)
        io_write_int32_t(s, obj_name_datas[i]);
    io_align_write(s, 0x10);
    free(obj_name_datas);

    osh.tex_id_data = io_get_position(s);
    for (uint32_t i = 0; i < os->tex_id_num; i++)
        io_write_uint32_t(s, os->tex_id_data[i]);
    io_align_write(s, 0x10);

    osh.obj_skin_data = io_get_position(s);
    for (uint32_t i = 0; i < count; i++)
        io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    int32_t* obj_skin_datas = force_malloc_s(int32_t, count);
    for (uint32_t i = 0; i < count; i++) {
        if (!os->obj_data[i].skin_init) {
            obj_skin_datas[i] = 0;
            continue;
        }

        obj_skin_datas[i] = (int32_t)io_get_position(s);
        obj_classic_write_skin(&os->obj_data[i], s, obj_skin_datas[i]);
    }
    io_align_write(s, 0x10);

    io_position_push(s, osh.obj_skin_data, SEEK_SET);
    for (uint32_t i = 0; i < count; i++)
        io_write_int32_t(s, obj_skin_datas[i]);
    io_position_pop(s);
    free(obj_skin_datas);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, 0x05062500);
    io_write_int32_t(s, os->obj_num);
    io_write_uint32_t(s, osh.last_obj_id);
    io_write_int32_t(s, (int32_t)osh.obj_data);
    io_write_int32_t(s, (int32_t)osh.obj_skin_data);
    io_write_int32_t(s, (int32_t)osh.obj_name_data);
    io_write_int32_t(s, (int32_t)osh.obj_id_data);
    io_write_int32_t(s, (int32_t)osh.tex_id_data);
    io_write_int32_t(s, os->tex_id_num);
    io_write_uint32_t(s, os->reserved[0]);
    io_write_uint32_t(s, os->reserved[1]);
    io_position_pop(s);
}

static void obj_classic_read_index(obj* obj, stream* s,
    obj_sub_mesh* sub_mesh) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t indices_count = sub_mesh->indices_count;
    uint32_t* indices = force_malloc_s(uint32_t, indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < indices_count; i++) {
            uint8_t idx = io_read_uint8_t(s);
            indices[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < indices_count; i++) {
            uint16_t idx = io_read_uint16_t(s);
            indices[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < indices_count; i++)
            indices[i] = io_read_uint32_t(s);
        break;
    }
    sub_mesh->indices = indices;
}

static void obj_classic_write_index(obj* obj, stream* s, obj_sub_mesh* sub_mesh) {
    uint32_t* indices = sub_mesh->indices;
    uint32_t indices_count = sub_mesh->indices_count;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < indices_count; i++)
            io_write_uint8_t(s, (uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < indices_count; i++)
            io_write_uint16_t(s, (uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < indices_count; i++)
            io_write_uint32_t(s, indices[i]);
        break;
    }
    io_align_write(s, 0x04);
}

static void obj_classic_read_model(obj* obj, stream* s, int64_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    io_set_position(s, base_offset, SEEK_SET);

    obj_header oh = {};
    io_read_uint32_t(s); // version
    io_read_uint32_t(s); // flags
    obj->bounding_sphere.center.x = io_read_float_t(s);
    obj->bounding_sphere.center.y = io_read_float_t(s);
    obj->bounding_sphere.center.z = io_read_float_t(s);
    obj->bounding_sphere.radius = io_read_float_t(s);
    obj->num_mesh = io_read_int32_t(s);
    oh.mesh_array = io_read_int32_t(s);
    obj->num_material = io_read_int32_t(s);
    oh.material_array = io_read_int32_t(s);
    obj->reserved[0] = io_read_uint32_t(s);
    obj->reserved[1] = io_read_uint32_t(s);
    obj->reserved[2] = io_read_uint32_t(s);
    obj->reserved[3] = io_read_uint32_t(s);
    obj->reserved[4] = io_read_uint32_t(s);
    obj->reserved[5] = io_read_uint32_t(s);
    obj->reserved[6] = io_read_uint32_t(s);
    obj->reserved[7] = io_read_uint32_t(s);
    obj->reserved[8] = io_read_uint32_t(s);
    obj->reserved[9] = io_read_uint32_t(s);

    if (oh.mesh_array > 0) {
        obj->mesh_array = force_malloc_s(obj_mesh, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            io_set_position(s, base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);

            obj_mesh_header mh = {};
            mesh->flags = io_read_uint32_t(s);
            mesh->bounding_sphere.center.x = io_read_float_t(s);
            mesh->bounding_sphere.center.y = io_read_float_t(s);
            mesh->bounding_sphere.center.z = io_read_float_t(s);
            mesh->bounding_sphere.radius = io_read_float_t(s);
            mesh->num_submesh = io_read_int32_t(s);
            mh.submesh_array = io_read_int32_t(s);
            mh.format = (obj_vertex_format_file)io_read_uint32_t(s);
            mh.size_vertex = io_read_int32_t(s);
            mh.num_vertex = io_read_int32_t(s);

            for (uint32_t j = 0; j < 20; j++)
                mh.vertex[j] = io_read_int32_t(s);

            mesh->attrib.w = io_read_uint32_t(s);
            mh.vertex_flags = io_read_uint32_t(s);
            mesh->reserved[0] = io_read_uint32_t(s);
            mesh->reserved[1] = io_read_uint32_t(s);
            mesh->reserved[2] = io_read_uint32_t(s);
            mesh->reserved[3] = io_read_uint32_t(s);
            mesh->reserved[4] = io_read_uint32_t(s);
            mesh->reserved[5] = io_read_uint32_t(s);
            io_read(s, &mesh->name, sizeof(mesh->name));
            mesh->name[sizeof(mesh->name) - 1] = 0;

            if (mh.submesh_array) {
                mesh->submesh_array = force_malloc_s(obj_sub_mesh, mesh->num_submesh);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                    io_set_position(s, base_offset + mh.submesh_array + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh = {};
                    sub_mesh->flags = io_read_uint32_t(s);
                    sub_mesh->bounding_sphere.center.x = io_read_float_t(s);
                    sub_mesh->bounding_sphere.center.y = io_read_float_t(s);
                    sub_mesh->bounding_sphere.center.z = io_read_float_t(s);
                    sub_mesh->bounding_sphere.radius = io_read_float_t(s);
                    sub_mesh->material_index = io_read_uint32_t(s);
                    io_read(s, &sub_mesh->uv_index, 0x08);
                    sub_mesh->bone_indices_count = io_read_int32_t(s);
                    smh.bone_indices_offset = io_read_int32_t(s);
                    sub_mesh->bones_per_vertex = io_read_uint32_t(s);
                    sub_mesh->primitive_type = (obj_primitive_type)io_read_uint32_t(s);
                    sub_mesh->index_format = (obj_index_format)io_read_uint32_t(s);
                    sub_mesh->indices_count = io_read_int32_t(s);
                    smh.indices_offset = io_read_int32_t(s);
                    sub_mesh->attrib.w = io_read_uint32_t(s);
                    sub_mesh->bounding_box.center = sub_mesh->bounding_sphere.center;
                    sub_mesh->bounding_box.size.x = sub_mesh->bounding_sphere.radius * 2.0f;
                    sub_mesh->bounding_box.size.y = sub_mesh->bounding_sphere.radius * 2.0f;
                    sub_mesh->bounding_box.size.z = sub_mesh->bounding_sphere.radius * 2.0f;
                    io_read(s, 0, 0x18);
                    sub_mesh->indices_offset = io_read_uint32_t(s);

                    if (sub_mesh->bones_per_vertex == 4 && smh.bone_indices_offset) {
                        sub_mesh->bone_indices = force_malloc_s(uint16_t, sub_mesh->bone_indices_count);
                        io_set_position(s, base_offset + smh.bone_indices_offset, SEEK_SET);
                        io_read(s, sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    }

                    io_set_position(s, base_offset + smh.indices_offset, SEEK_SET);
                    obj_classic_read_index(obj, s, sub_mesh);
                }
            }

            obj_classic_read_vertex(obj, s, mh.vertex, mesh,
                base_offset, mh.num_vertex, mh.format);
        }
    }

    if (oh.material_array > 0) {
        io_set_position(s, base_offset + oh.material_array, SEEK_SET);
        obj->material_array = force_malloc_s(obj_material_data, obj->num_material);
        io_read(s, obj->material_array, obj->num_material * sizeof(obj_material_data));
    }
}

static void obj_classic_write_model(obj* obj, stream* s, int64_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_align_write(s, 0x10);

    obj_header oh = {};
    if (obj->num_mesh) {
        oh.mesh_array = io_get_position(s) - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            io_write(s, 0x04);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);

            for (uint32_t j = 0; j < 20; j++)
                io_write_int32_t(s, 0);

            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write(s, 0x18);
            io_write(s, sizeof(mesh->name));
        }

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            if (mesh->num_submesh) {
                mh->submesh_array = io_get_position(s) - base_offset;
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    io_write(s, 0x04);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0x08);
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0x18);
                    io_write_uint32_t(s, 0);
                }
            }

            obj_classic_write_vertex(obj, s, mh->vertex,
                mesh, base_offset, &mh->num_vertex, &mh->format, &mh->size_vertex);

            for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                obj_sub_mesh_header smh = {};
                if (sub_mesh->bones_per_vertex == 4 && sub_mesh->bone_indices_count) {
                    smh.bone_indices_offset = io_get_position(s) - base_offset;
                    io_write(s, sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    io_align_write(s, 0x04);
                }

                smh.indices_offset = io_get_position(s) - base_offset;
                obj_classic_write_index(obj, s, sub_mesh);

                io_position_push(s, base_offset + mh->submesh_array + sub_mesh_size * j, SEEK_SET);
                io_write_uint32_t(s, sub_mesh->flags);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.x);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.y);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.z);
                io_write_float_t(s, sub_mesh->bounding_sphere.radius);
                io_write_uint32_t(s, sub_mesh->material_index);
                io_write(s, &sub_mesh->uv_index, 0x08);
                io_write_int32_t(s, sub_mesh->bone_indices_count);
                io_write_int32_t(s, (int32_t)smh.bone_indices_offset);
                io_write_uint32_t(s, sub_mesh->bones_per_vertex);
                io_write_uint32_t(s, sub_mesh->primitive_type);
                io_write_uint32_t(s, sub_mesh->index_format);
                io_write_int32_t(s, sub_mesh->indices_count);
                io_write_int32_t(s, (int32_t)smh.indices_offset);
                io_write_uint32_t(s, sub_mesh->attrib.w);
                io_write(s, 0x18);
                io_write_uint32_t(s, sub_mesh->indices_offset);
                io_position_pop(s);
            }
        }

        io_position_push(s, base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            io_write_uint32_t(s, mesh->flags);
            io_write_float_t(s, mesh->bounding_sphere.center.x);
            io_write_float_t(s, mesh->bounding_sphere.center.y);
            io_write_float_t(s, mesh->bounding_sphere.center.z);
            io_write_float_t(s, mesh->bounding_sphere.radius);
            io_write_int32_t(s, mesh->num_submesh);
            io_write_int32_t(s, (int32_t)mh->submesh_array);
            io_write_uint32_t(s, mh->format);
            io_write_int32_t(s, mh->size_vertex);
            io_write_int32_t(s, mh->num_vertex);

            for (uint32_t j = 0; j < 20; j++)
                io_write_int32_t(s, (int32_t)mh->vertex[j]);

            io_write_uint32_t(s, mesh->attrib.w);
            io_write_uint32_t(s, mh->vertex_flags);
            io_write_uint32_t(s, mesh->reserved[0]);
            io_write_uint32_t(s, mesh->reserved[1]);
            io_write_uint32_t(s, mesh->reserved[2]);
            io_write_uint32_t(s, mesh->reserved[3]);
            io_write_uint32_t(s, mesh->reserved[4]);
            io_write_uint32_t(s, mesh->reserved[5]);
            io_write(s, &mesh->name, sizeof(mesh->name) - 1);
            io_write_char(s, '\0');
        }
        io_position_pop(s);
        free(mhs);
    }

    if (obj->num_material) {
        oh.material_array = io_get_position(s) - base_offset;
        io_write(s, obj->material_array, obj->num_material * sizeof(obj_material_data));
    }
    io_align_write(s, 0x10);

    io_position_push(s, base_offset, SEEK_SET);
    io_write_uint32_t(s, 0x10000);
    io_write_uint32_t(s, 0x00);
    io_write_float_t(s, obj->bounding_sphere.center.x);
    io_write_float_t(s, obj->bounding_sphere.center.y);
    io_write_float_t(s, obj->bounding_sphere.center.z);
    io_write_float_t(s, obj->bounding_sphere.radius);
    io_write_int32_t(s, obj->num_mesh);
    io_write_int32_t(s, (int32_t)oh.mesh_array);
    io_write_int32_t(s, obj->num_material);
    io_write_int32_t(s, (int32_t)oh.material_array);
    io_write_uint32_t(s, obj->reserved[0]);
    io_write_uint32_t(s, obj->reserved[1]);
    io_write_uint32_t(s, obj->reserved[2]);
    io_write_uint32_t(s, obj->reserved[3]);
    io_write_uint32_t(s, obj->reserved[4]);
    io_write_uint32_t(s, obj->reserved[5]);
    io_write_uint32_t(s, obj->reserved[6]);
    io_write_uint32_t(s, obj->reserved[7]);
    io_write_uint32_t(s, obj->reserved[8]);
    io_write_uint32_t(s, obj->reserved[9]);
    io_position_pop(s);
}

static void obj_classic_read_skin(obj* obj, stream* s, int64_t base_offset) {
    obj_skin* sk = &obj->skin;
    obj->skin_init = true;
    io_set_position(s, base_offset, SEEK_SET);

    obj_skin_header sh = {};
    sh.bone_ids_offset = io_read_int32_t(s);
    sh.bone_matrices_offset = io_read_int32_t(s);
    sh.bone_names_offset = io_read_int32_t(s);
    sh.ex_data_offset = io_read_int32_t(s);
    sk->bones_count = io_read_int32_t(s);
    sh.bone_parent_ids_offset = io_read_int32_t(s);
    io_read(s, 0, 0x0C);

    if (sh.bone_ids_offset) {
        sk->bones = force_malloc_s(obj_skin_bone, sk->bones_count);

        int32_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(int32_t, sk->bones_count);
            io_set_position(s, sh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                bone_names_offsets[i] = io_read_int32_t(s);
        }

        if (sh.bone_ids_offset) {
            io_set_position(s, sh.bone_ids_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                sk->bones[i].id = io_read_uint32_t(s);

            if (sh.bone_matrices_offset) {
                io_set_position(s, sh.bone_matrices_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++) {
                    mat4u& mat = sk->bones[i].inv_bind_pose_mat;
                    mat.row0.x = io_read_float_t(s);
                    mat.row1.x = io_read_float_t(s);
                    mat.row2.x = io_read_float_t(s);
                    mat.row3.x = io_read_float_t(s);
                    mat.row0.y = io_read_float_t(s);
                    mat.row1.y = io_read_float_t(s);
                    mat.row2.y = io_read_float_t(s);
                    mat.row3.y = io_read_float_t(s);
                    mat.row0.z = io_read_float_t(s);
                    mat.row1.z = io_read_float_t(s);
                    mat.row2.z = io_read_float_t(s);
                    mat.row3.z = io_read_float_t(s);
                    mat.row0.w = io_read_float_t(s);
                    mat.row1.w = io_read_float_t(s);
                    mat.row2.w = io_read_float_t(s);
                    mat.row3.w = io_read_float_t(s);
                }
            }

            if (sh.bone_names_offset)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    io_read_string_null_terminated_offset(s,
                        bone_names_offsets[i], &sk->bones[i].name);

            if (sh.bone_parent_ids_offset) {
                io_set_position(s, sh.bone_parent_ids_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].parent = io_read_uint32_t(s);
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        io_set_position(s, sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh = {};
        exh.osage_count = io_read_int32_t(s);
        ex->osage_nodes_count = io_read_int32_t(s);
        io_read(s, 0, 0x04);
        exh.osage_nodes_offset = io_read_int32_t(s);
        exh.osage_names_offset = io_read_int32_t(s);
        exh.blocks_offset = io_read_int32_t(s);
        exh.bone_names_count = io_read_int32_t(s);
        exh.bone_names_offset = io_read_int32_t(s);
        exh.osage_sibling_infos_offset = io_read_int32_t(s);
        exh.cloth_count = io_read_uint32_t(s);
        ex->reserved[0] = io_read_uint32_t(s);
        ex->reserved[1] = io_read_uint32_t(s);
        ex->reserved[2] = io_read_uint32_t(s);
        ex->reserved[3] = io_read_uint32_t(s);
        ex->reserved[4] = io_read_uint32_t(s);
        ex->reserved[5] = io_read_uint32_t(s);
        ex->reserved[6] = io_read_uint32_t(s);

        if (exh.bone_names_offset) {
            std::vector<std::string> bone_names;
            bone_names.reserve(exh.bone_names_count);
            int32_t* strings_offsets = force_malloc_s(int32_t, exh.bone_names_count);
            io_set_position(s, exh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < exh.bone_names_count; i++)
                strings_offsets[i] = io_read_int32_t(s);

            size_t buf_size = 0;
            for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                if (strings_offsets[i]) {
                    std::string str;
                    io_read_string_null_terminated_offset(s, strings_offsets[i], &str);
                    bone_names.push_back(str);
                }
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

            io_set_position(s, exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];

                osage_node->name_index = io_read_uint32_t(s);
                osage_node->length = io_read_float_t(s);
                io_read(s, 0, 0x04);
            }
        }

        if (exh.blocks_offset) {
            ex->blocks_count = 0;
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            while (io_read_int32_t(s)) {
                io_read(s, 0, 0x04);
                ex->blocks_count++;
            }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                bhs[i].block_signature_offset = io_read_int32_t(s);
                bhs[i].block_offset = io_read_int32_t(s);
            }

            ex->blocks = force_malloc_s(obj_skin_block, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];

                std::string block_signature;
                io_read_string_null_terminated_offset(s,
                    bhs[i].block_signature_offset, &block_signature);
                if (block_signature.size() != 3)
                    continue;

                uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
                switch (signature) {
                case 'CLS\0':
                    block->type = OBJ_SKIN_BLOCK_CLOTH;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_cloth(&block->cloth,
                        s, bone_names);
                    break;
                case 'CNS\0':
                    block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_constraint(&block->constraint,
                        s, bone_names);
                    break;
                case 'EXP\0':
                    block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_expression(&block->expression,
                        s, bone_names);
                    break;
                case 'MOT\0':
                    block->type = OBJ_SKIN_BLOCK_MOTION;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_motion(&block->motion,
                        s, bone_names);
                    break;
                case 'OSG\0':
                    block->type = OBJ_SKIN_BLOCK_OSAGE;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_classic_read_skin_block_osage(&block->osage,
                        s, bone_names);
                    break;
                }
            }
            free(bhs);
        }

        if (exh.osage_sibling_infos_offset) {
            ex->osage_sibling_infos_count = 0;
            io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
            while (io_read_uint32_t(s)) {
                io_read(s, 0, 0x08);
                ex->osage_sibling_infos_count++;
            }

            obj_skin_osage_sibling_info* osis = force_malloc_s(
                obj_skin_osage_sibling_info, ex->osage_sibling_infos_count);
            ex->osage_sibling_infos = osis;
            io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                osis[i].name_index = io_read_uint32_t(s);
                osis[i].sibling_name_index = io_read_uint32_t(s);
                osis[i].max_distance = io_read_float_t(s);
            }

            io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
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

static void obj_classic_write_skin(obj* obj, stream* s, int64_t base_offset) {
    obj_skin* sk = &obj->skin;

    obj_skin_header sh = {};
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write(s, 0x0C);
    io_align_write(s, 0x10);

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;
    std::vector<std::string> bone_names;
    if (sk->bones) {
        sh.bone_ids_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++)
            io_write_int32_t(s, sk->bones[i].id);
        io_align_write(s, 0x10);

        sh.bone_parent_ids_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++)
            io_write_int32_t(s, sk->bones[i].parent);
        io_align_write(s, 0x10);

        sh.bone_names_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            io_write_int32_t(s, 0);
            obj_skin_strings_push_back_check(strings, string_data(&sk->bones[i].name));
        }
        io_align_write(s, 0x10);

        sh.bone_matrices_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            mat4u& mat = sk->bones[i].inv_bind_pose_mat;
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
        }
        io_align_write(s, 0x10);
    }

    obj_skin_block_header* bhs = 0;
    int64_t osage_block_node = 0;
    int64_t motion_block_node_mats = 0;
    int64_t motion_block_node_name_offset = 0;
    int64_t field_18 = 0;
    int64_t field_1C = 0;
    int64_t field_20 = 0;
    int64_t field_24 = 0;
    int64_t field_28 = 0;

    obj_skin_ex_data_header exh = {};
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = io_get_position(s);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write(s, 0x04);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write(s, 0x1C);
        io_align_write(s, 0x10);

        if (ex->osage_nodes_count) {
            exh.osage_nodes_offset = io_get_position(s);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_align_write(s, 0x10);

            exh.osage_sibling_infos_offset = io_get_position(s);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
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
            exh.osage_names_offset = io_get_position(s);
            for (uint32_t i = 0; i < exh.osage_count; i++)
                io_write_int32_t(s, 0);

            for (uint32_t i = 0; i < exh.cloth_count; i++)
                io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
        }

        char** bone_names_ptr = ex->bone_names;
        for (uint32_t i = 0; i < ex->blocks_count; i++) {
            obj_skin_block* block = &ex->blocks[i];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &block->cloth;
                obj_skin_strings_push_back_check(strings, string_data(&cloth->mesh_name));
                obj_skin_strings_push_back_check(strings, string_data(&cloth->backface_mesh_name));
                for (uint32_t k = 0; k < cloth->count; k++) {
                    obj_skin_block_cloth_field_1C* sub = &cloth->field_1C[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings,
                            string_data(&sub->sub_data[l].bone_name));
                }
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_strings_push_back_check(strings, string_data(&constraint->base.parent_name));
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, constraint->name_index);
                obj_skin_strings_push_back_check(strings, string_data(&constraint->source_node_name));
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->direction.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->position.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->distance.up_vector.name));
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                for (uint32_t j = 0; j < expression->expressions_count; j++)
                    obj_skin_strings_push_back_check(strings, string_data(&expression->expressions[j]));
                obj_skin_strings_push_back_check(strings, string_data(&expression->base.parent_name));
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_strings_push_back_check(strings, string_data(&motion->base.parent_name));
                obj_skin_strings_push_back_check(strings, string_data(&motion->name));
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, motion->nodes[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                obj_skin_strings_push_back_check(bone_names, string_data(&motion->name));
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, motion->nodes[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_strings_push_back_check(strings, string_data(&osage->base.parent_name));
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

                if (osage->root_init)
                    obj_skin_strings_push_back_check(strings, string_data(&osage->root.name));
            } break;
            }
        }

        exh.bone_names_count = (int32_t)bone_names.size();
        exh.bone_names_offset = io_get_position(s);
        for (std::string& i : bone_names)
            io_write_int32_t(s, 0);
        io_align_write(s, 0x10);

        if (ex->blocks_count > 0) {
            exh.blocks_offset = io_get_position(s);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                obj_skin_block_osage* osage = &block->osage;
                if (osage->root_init) {
                    obj_skin_osage_root_node* root = &osage->root;
                    if (root->coli) {
                        io_write(s, 0x28LL * root->coli_count);
                        io_align_write(s, 0x10);
                    }
                    io_write(s, 0x38);
                    io_align_write(s, 0x10);
                }

                bhs[i].block_offset = io_get_position(s);
                io_write(s, 0x28);
                io_write(s, 0x14);
                io_write(s, 0x14);
            }
            io_align_write(s, 0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                bhs[i].block_offset = io_get_position(s);
                io_write(s, 0x28);
                io_write(s, 0x2C);
            }
            io_align_write(s, 0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                bhs[i].block_offset = io_get_position(s);
                io_write(s, 0x34);
            }
            io_align_write(s, 0x10);

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                bhs[i].block_offset = io_get_position(s);
                io_write(s, 0x28);
                io_write(s, 0x10);
                switch (block->constraint.type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    io_write(s, 0x0C);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    io_write(s, 0x24);
                    io_write(s, 0x18);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    io_write(s, 0x24);
                    io_write(s, 0x14);
                    io_write(s, 0x14);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    io_write(s, 0x24);
                    io_write(s, 0x04);
                    io_write(s, 0x14);
                    io_write(s, 0x14);
                    break;
                }
            }
            io_align_write(s, 0x10);

            int32_t motion_block_count = 0;
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                bhs[i].block_offset = io_get_position(s);
                io_write(s, 0x28);
                io_write(s, 0x10);
                motion_block_count++;
            }
            io_align_write(s, 0x10);

            if (exh.osage_count) {
                osage_block_node = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = &block->osage;
                    io_write(s, osage->nodes_count * (sizeof(uint32_t) + sizeof(float_t) * 4));
                }
                io_align_write(s, 0x10);
            }

            if (motion_block_count) {
                motion_block_node_mats = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    io_write(s, motion->nodes_count * sizeof(mat4));
                }
                io_align_write(s, 0x10);

                motion_block_node_name_offset = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    io_write(s, motion->nodes_count * sizeof(uint32_t));
                }
                io_align_write(s, 0x10);
            }

            if (exh.cloth_count) {
                field_18 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    for (uint32_t j = 0; j < 32; j++)
                        if (cloth->field_10 & (1 << j))
                            io_write(s, sizeof(mat4));
                }
                io_align_write(s, 0x10);

                field_1C = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, (10 * sizeof(int32_t)
                        + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * cloth->count);
                }
                io_align_write(s, 0x10);

                field_20 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, 10 * (11 * sizeof(int32_t)) * cloth->count);
                }
                io_align_write(s, 0x10);

                field_24 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, sizeof(uint16_t) + cloth->field_24_count * sizeof(uint16_t));
                }
                io_align_write(s, 0x10);

                field_28 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, sizeof(uint16_t) + cloth->field_28_count * sizeof(uint16_t));
                }
                io_align_write(s, 0x10);
            }
        }
    }

    if (sk->bones || sk->ex_data_init) {
        quicksort_string(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (std::string& i : strings) {
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i);
        }
    }
    io_align_write(s, 0x10);

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

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_cloth(&block->cloth,
                    s, strings, string_offsets, &field_18, &field_1C, &field_20, &field_24, &field_28);
                io_position_pop(s);
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

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_constraint(&block->constraint,
                    s, strings, string_offsets, bone_names_ptr, constraint_type_name_offsets);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_expression(&block->expression,
                    s, strings, string_offsets, bone_names_ptr);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_motion(&block->motion,
                    s, strings, string_offsets, bone_names_ptr,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_osage(&block->osage,
                    s, strings, string_offsets, &osage_block_node);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = &block->cloth;
                    obj_skin_strings_push_back_check(osage_names, string_data(&cloth->mesh_name));
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = &block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_names_ptr, osage->external_name_index);
                }
            }

            io_position_push(s, exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                switch (block->type) {
                case OBJ_SKIN_BLOCK_CLOTH:
                    io_write_int32_t(s, (int32_t)cls_offset);
                    io_write_int32_t(s, (int32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT:
                    io_write_int32_t(s, (int32_t)cns_offset);
                    io_write_int32_t(s, (int32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_EXPRESSION:
                    io_write_int32_t(s, (int32_t)exp_offset);
                    io_write_int32_t(s, (int32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_MOTION:
                    io_write_int32_t(s, (int32_t)mot_offset);
                    io_write_int32_t(s, (int32_t)bhs[i].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_OSAGE:
                    io_write_int32_t(s, (int32_t)osg_offset);
                    io_write_int32_t(s, (int32_t)bhs[i].block_offset);
                    break;
                default:
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    break;
                }
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);
            free(bhs);
        }
    }

    if (sk->bones) {
        io_position_push(s, sh.bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            size_t str_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, string_data(&sk->bones[i].name));
            io_write_int32_t(s, (int32_t)str_offset);
        }
        io_position_pop(s);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (ex->blocks_count > 0) {
            io_position_push(s, exh.bone_names_offset, SEEK_SET);
            for (std::string& i : bone_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                io_write_int32_t(s, (int32_t)str_offset);
            }
            io_position_pop(s);

            io_position_push(s, exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];
                io_write_uint32_t(s, osage_node->name_index);
                io_write_float_t(s, osage_node->length);
                io_write_uint32_t(s, osage_node->name_index & 0x7FFF);
            }
            io_position_pop(s);

            io_position_push(s, exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_infos[i];
                io_write_uint32_t(s, osage_sibling_info->name_index);
                io_write_uint32_t(s, osage_sibling_info->sibling_name_index);
                io_write_float_t(s, osage_sibling_info->max_distance);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);

            exh.osage_count = (int32_t)osage_names.size();
            exh.osage_count -= exh.cloth_count;
            io_position_push(s, exh.osage_names_offset, SEEK_SET);
            for (std::string& i : osage_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                io_write_int32_t(s, (int32_t)str_offset);
            }
            io_position_pop(s);
        }

        io_position_push(s, sh.ex_data_offset, SEEK_SET);
        io_write_int32_t(s, exh.osage_count);
        io_write_int32_t(s, ex->osage_nodes_count);
        io_write(s, 0x04);
        io_write_uint32_t(s, (uint32_t)exh.osage_nodes_offset);
        io_write_uint32_t(s, (uint32_t)exh.osage_names_offset);
        io_write_uint32_t(s, (uint32_t)exh.blocks_offset);
        io_write_int32_t(s, exh.bone_names_count);
        io_write_uint32_t(s, (uint32_t)exh.bone_names_offset);
        io_write_uint32_t(s, (uint32_t)exh.osage_sibling_infos_offset);
        io_write_int32_t(s, exh.cloth_count);
        io_write_uint32_t(s, (uint32_t)ex->reserved[0]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[1]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[2]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[3]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[4]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[5]);
        io_write_uint32_t(s, (uint32_t)ex->reserved[6]);
        io_position_pop(s);
    }

    io_position_push(s, base_offset, SEEK_SET);
    io_write_int32_t(s, (int32_t)sh.bone_ids_offset);
    io_write_int32_t(s, (int32_t)sh.bone_matrices_offset);
    io_write_int32_t(s, (int32_t)sh.bone_names_offset);
    io_write_int32_t(s, (int32_t)sh.ex_data_offset);
    io_write_int32_t(s, sk->bones_count);
    io_write_int32_t(s, (int32_t)sh.bone_parent_ids_offset);
    io_write(s, 0x0C);
    io_position_pop(s);
}

static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, char** str) {
    int32_t mesh_name_offset = io_read_int32_t(s);
    int32_t backface_mesh_name_offset = io_read_int32_t(s);
    b->field_8 = io_read_int32_t(s);
    b->count = io_read_int32_t(s);
    b->field_10 = io_read_int32_t(s);
    b->field_14 = io_read_int32_t(s);
    int32_t field_18_offset = io_read_int32_t(s);
    int32_t field_1C_offset = io_read_int32_t(s);
    int32_t field_20_offset = io_read_int32_t(s);
    int32_t field_24_offset = io_read_int32_t(s);
    int32_t field_28_offset = io_read_int32_t(s);
    b->field_2C = io_read_uint32_t(s);
    b->field_30 = io_read_uint32_t(s);

    io_read_string_null_terminated_offset(s, mesh_name_offset, &b->mesh_name);
    io_read_string_null_terminated_offset(s, backface_mesh_name_offset, &b->backface_mesh_name);

    if (field_18_offset) {
        io_position_push(s, field_18_offset, SEEK_SET);
        for (uint32_t i = 0; i < 32; i++) {
            if (~b->field_10 & (1 << i))
                continue;

            mat4 mat;
            mat.row0.x = io_read_float_t(s);
            mat.row0.y = io_read_float_t(s);
            mat.row0.z = io_read_float_t(s);
            mat.row0.w = io_read_float_t(s);
            mat.row1.x = io_read_float_t(s);
            mat.row1.y = io_read_float_t(s);
            mat.row1.z = io_read_float_t(s);
            mat.row1.w = io_read_float_t(s);
            mat.row2.x = io_read_float_t(s);
            mat.row2.y = io_read_float_t(s);
            mat.row2.z = io_read_float_t(s);
            mat.row2.w = io_read_float_t(s);
            mat.row3.x = io_read_float_t(s);
            mat.row3.y = io_read_float_t(s);
            mat.row3.z = io_read_float_t(s);
            mat.row3.w = io_read_float_t(s);
            mat4_transpose(&mat, &mat);
            b->field_18[i] = mat;
        }
        io_position_pop(s);
    }

    if (field_1C_offset) {
        io_position_push(s, field_1C_offset, SEEK_SET);
        b->field_1C = force_malloc_s(obj_skin_block_cloth_field_1C, b->count);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_1C* f = &b->field_1C[i];
            f->field_0 = io_read_float_t(s);
            f->field_4 = io_read_float_t(s);
            f->field_8 = io_read_float_t(s);
            f->field_C = io_read_float_t(s);
            f->field_10 = io_read_float_t(s);
            f->field_14 = io_read_float_t(s);
            f->field_18 = io_read_float_t(s);
            f->field_1C = io_read_uint32_t(s);
            f->field_20 = io_read_uint32_t(s);
            f->field_24 = io_read_uint32_t(s);

            for (uint32_t j = 0; j < 4; j++)
                obj_classic_read_skin_block_cloth_field_1C_sub(&f->sub_data[j], s, str);
        }
        io_position_pop(s);
    }

    if (field_20_offset) {
        io_position_push(s, field_20_offset, SEEK_SET);
        b->field_20 = force_malloc_s(obj_skin_block_cloth_field_20, b->count);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_20* f = &b->field_20[i];
            for (uint32_t j = 0; j < 10; j++)
                obj_classic_read_skin_block_cloth_field_20_sub(&f->sub_data[j], s, str);
        }
        io_position_pop(s);
    }

    if (field_24_offset) {
        io_position_push(s, field_24_offset, SEEK_SET);
        b->field_24_count = io_read_uint16_t_stream_reverse_endianness(s);
        b->field_24 = force_malloc_s(uint16_t, b->field_24_count);
        io_read(s, b->field_24, b->field_24_count * sizeof(uint16_t));
        io_position_pop(s);
    }

    if (field_28_offset) {
        io_position_push(s, field_28_offset, SEEK_SET);
        b->field_28_count = io_read_uint16_t_stream_reverse_endianness(s);
        b->field_28 = force_malloc_s(uint16_t, b->field_28_count);
        io_read(s, b->field_28, b->field_28_count * sizeof(uint16_t));
        io_position_pop(s);
    }
}

static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    int64_t* field_18_offset, int64_t* field_1C_offset, int64_t* field_20_offset,
    int64_t* field_24_offset, int64_t* field_28_offset) {
    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->mesh_name));
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->backface_mesh_name));

    io_write_int32_t(s, (int32_t)mesh_name_offset);
    io_write_int32_t(s, (int32_t)backface_mesh_name_offset);
    io_write_int32_t(s, b->field_8);
    io_write_int32_t(s, b->count);
    io_write_int32_t(s, b->field_10);
    io_write_int32_t(s, b->field_14);
    io_write_int32_t(s, (int32_t)*field_18_offset);
    io_write_int32_t(s, (int32_t)*field_1C_offset);
    io_write_int32_t(s, (int32_t)*field_20_offset);
    io_write_int32_t(s, (int32_t)*field_24_offset);
    io_write_int32_t(s, (int32_t)*field_28_offset);
    io_write_uint32_t(s, b->field_2C);
    io_write_uint32_t(s, b->field_30);

    if (b->field_10) {
        io_position_push(s, *field_18_offset, SEEK_SET);
        for (uint32_t i = 0; i < 32; i++) {
            if (~b->field_10 & (1 << i))
                continue;

            mat4u& mat = b->field_18[i];
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
            *field_18_offset += sizeof(mat4);
        }
        io_position_pop(s);
    }

    if (b->field_1C) {
        io_position_push(s, *field_1C_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_1C* f = &b->field_1C[i];
            io_write_float_t(s, f->field_0);
            io_write_float_t(s, f->field_4);
            io_write_float_t(s, f->field_8);
            io_write_float_t(s, f->field_C);
            io_write_float_t(s, f->field_10);
            io_write_float_t(s, f->field_14);
            io_write_float_t(s, f->field_18);
            io_write_uint32_t(s, f->field_1C);
            io_write_uint32_t(s, f->field_20);
            io_write_uint32_t(s, f->field_24);

            for (uint32_t j = 0; j < 4; j++)
                obj_classic_write_skin_block_cloth_field_1C_sub(&f->sub_data[j],
                    s, strings, string_offsets);
        }
        io_position_pop(s);
        *field_1C_offset += (10 * sizeof(int32_t)
            + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * b->count;
    }

    if (b->field_20) {
        io_position_push(s, *field_20_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_20* f = &b->field_20[i];
            for (uint32_t j = 0; j < 10; j++)
                obj_classic_write_skin_block_cloth_field_20_sub(&f->sub_data[j],
                    s, strings, string_offsets);
        }
        io_position_pop(s);
        *field_20_offset += 10 * (11 * sizeof(int32_t)) * b->count;
    }

    if (b->field_24) {
        io_position_push(s, *field_24_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)b->field_24_count);
        io_write(s, b->field_24, b->field_24_count * sizeof(uint16_t));
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + b->field_24_count * sizeof(uint16_t);
    }

    if (b->field_28) {
        io_position_push(s, *field_28_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)b->field_28_count);
        io_write(s, b->field_28, b->field_28_count * sizeof(uint16_t));
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + b->field_28_count * sizeof(uint16_t);
    }
}

static void obj_classic_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, char** str) {
    int32_t bone_name_offset = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, bone_name_offset, &sub->bone_name);
    sub->weight = io_read_float_t(s);
    sub->matrix_index = io_read_uint32_t(s);
    sub->field_C = io_read_uint32_t(s);
}

static void obj_classic_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&sub->bone_name));
    io_write_int32_t(s, (int32_t)bone_name_offset);
    io_write_float_t(s, sub->weight);
    io_write_uint32_t(s, sub->matrix_index);
    io_write_uint32_t(s, sub->field_C);
}

static void obj_classic_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, char** str) {
    sub->field_0 = io_read_uint32_t(s);
    sub->field_4 = io_read_float_t(s);
    sub->field_8 = io_read_float_t(s);
    sub->field_C = io_read_float_t(s);
    sub->field_10 = io_read_float_t(s);
    sub->field_14 = io_read_float_t(s);
    sub->field_18 = io_read_float_t(s);
    sub->field_1C = io_read_float_t(s);
    sub->field_20 = io_read_float_t(s);
    sub->field_24 = io_read_float_t(s);
    sub->field_28 = io_read_float_t(s);
}

static void obj_classic_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    io_write_uint32_t(s, sub->field_0);
    io_write_float_t(s, sub->field_4);
    io_write_float_t(s, sub->field_8);
    io_write_float_t(s, sub->field_C);
    io_write_float_t(s, sub->field_10);
    io_write_float_t(s, sub->field_14);
    io_write_float_t(s, sub->field_18);
    io_write_float_t(s, sub->field_1C);
    io_write_float_t(s, sub->field_20);
    io_write_float_t(s, sub->field_24);
    io_write_float_t(s, sub->field_28);
}

static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t type_offset = io_read_int32_t(s);
    char* type = io_read_utf8_string_null_terminated_offset(s, type_offset);

    int32_t name_offset = io_read_int32_t(s);
    char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->coupling = (obj_skin_block_constraint_coupling)io_read_int32_t(s);

    int32_t source_node_name_offset = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, source_node_name_offset, &b->source_node_name);

    if (!str_utils_compare(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = io_read_float_t(s);
        b->orientation.offset.y = io_read_float_t(s);
        b->orientation.offset.z = io_read_float_t(s);
    }
    else if (!str_utils_compare(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_classic_read_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, str);
        b->direction.align_axis.x = io_read_float_t(s);
        b->direction.align_axis.y = io_read_float_t(s);
        b->direction.align_axis.z = io_read_float_t(s);
        b->direction.target_offset.x = io_read_float_t(s);
        b->direction.target_offset.y = io_read_float_t(s);
        b->direction.target_offset.z = io_read_float_t(s);
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
        b->distance.distance = io_read_float_t(s);
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
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, int64_t* offsets) {
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
    io_write_int32_t(s, (int32_t)type_offset);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    io_write_int32_t(s, (int32_t)name_offset);

    io_write_int32_t(s, b->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->source_node_name));
    io_write_int32_t(s, (int32_t)source_node_name_offset);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        io_write_float_t(s, b->orientation.offset.x);
        io_write_float_t(s, b->orientation.offset.y);
        io_write_float_t(s, b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_classic_write_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, strings, string_offsets);
        io_write_float_t(s, b->direction.align_axis.x);
        io_write_float_t(s, b->direction.align_axis.y);
        io_write_float_t(s, b->direction.align_axis.z);
        io_write_float_t(s, b->direction.target_offset.x);
        io_write_float_t(s, b->direction.target_offset.y);
        io_write_float_t(s, b->direction.target_offset.z);
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
        io_write_float_t(s, b->distance.distance);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets);
        break;
    }
}

static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, char** str) {
    ap->affected_by_orientation = io_read_int32_t(s) != 0;
    ap->affected_by_scaling = io_read_int32_t(s) != 0;
    ap->offset.x = io_read_float_t(s);
    ap->offset.y = io_read_float_t(s);
    ap->offset.z = io_read_float_t(s);
}

static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    io_write_int32_t(s, ap->affected_by_orientation ? 1 : 0);
    io_write_int32_t(s, ap->affected_by_scaling ? 1 : 0);
    io_write_float_t(s, ap->offset.x);
    io_write_float_t(s, ap->offset.y);
    io_write_float_t(s, ap->offset.z);
}

static void obj_classic_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, char** str) {
    up->active = io_read_int32_t(s) != 0;
    up->roll = io_read_float_t(s);
    up->affected_axis.x = io_read_float_t(s);
    up->affected_axis.y = io_read_float_t(s);
    up->affected_axis.z = io_read_float_t(s);
    up->point_at.x = io_read_float_t(s);
    up->point_at.y = io_read_float_t(s);
    up->point_at.z = io_read_float_t(s);

    int32_t name_offset = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, name_offset, &up->name);
}

static void obj_classic_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    io_write_int32_t(s, up->active ? 1 : 0);
    io_write_float_t(s, up->roll);
    io_write_float_t(s, up->affected_axis.x);
    io_write_float_t(s, up->affected_axis.y);
    io_write_float_t(s, up->affected_axis.z);
    io_write_float_t(s, up->point_at.x);
    io_write_float_t(s, up->point_at.y);
    io_write_float_t(s, up->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&up->name));
    io_write_int32_t(s, (int32_t)name_offset);
}

static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t name_offset = io_read_int32_t(s);
    char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->expressions_count = io_read_int32_t(s);
    b->expressions_count = min(b->expressions_count, 9);
    for (uint32_t i = 0; i < b->expressions_count; i++) {
        int32_t expression_offset = io_read_int32_t(s);
        if (expression_offset)
            io_read_string_null_terminated_offset(s, expression_offset, &b->expressions[i]);
    }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        b->expressions[i] = string_empty;
}

static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    io_write_int32_t(s, (int32_t)name_offset);

    io_write_int32_t(s, min(b->expressions_count, 9));
    for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
        int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, string_data(&b->expressions[i]));
        io_write_int32_t(s, (int32_t)expression_offset);
    }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        io_write_int32_t(s, 0);
}

static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t name_offset = io_read_int32_t(s);
    b->nodes_count = io_read_int32_t(s);
    int32_t bone_names_offset = io_read_int32_t(s);
    int32_t bone_matrices_offset = io_read_int32_t(s);

    char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);

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
        io_position_push(s, bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            b->nodes[i].name_index = io_read_uint32_t(s);
        io_position_pop(s);
    }

    if (bone_matrices_offset) {
        io_position_push(s, bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4u& mat = b->nodes[i].transformation;
            mat.row0.x = io_read_float_t(s);
            mat.row1.x = io_read_float_t(s);
            mat.row2.x = io_read_float_t(s);
            mat.row3.x = io_read_float_t(s);
            mat.row0.y = io_read_float_t(s);
            mat.row1.y = io_read_float_t(s);
            mat.row2.y = io_read_float_t(s);
            mat.row3.y = io_read_float_t(s);
            mat.row0.z = io_read_float_t(s);
            mat.row1.z = io_read_float_t(s);
            mat.row2.z = io_read_float_t(s);
            mat.row3.z = io_read_float_t(s);
            mat.row0.w = io_read_float_t(s);
            mat.row1.w = io_read_float_t(s);
            mat.row2.w = io_read_float_t(s);
            mat.row3.w = io_read_float_t(s);
        }
        io_position_pop(s);
    }
}

static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    io_write_int32_t(s, (int32_t)name_offset);
    io_write_int32_t(s, b->nodes_count);
    io_write_int32_t(s, (int32_t)*bone_names_offset);
    io_write_int32_t(s, (int32_t)*bone_matrices_offset);

    if (b->nodes) {
        io_position_push(s, *bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            io_write_uint32_t(s, b->nodes[i].name_index);
        io_position_pop(s);
        *bone_names_offset += b->nodes_count * sizeof(uint32_t);

        io_position_push(s, *bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4u& mat = b->nodes[i].transformation;
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
        }
        io_position_pop(s);
        *bone_matrices_offset += b->nodes_count * sizeof(mat4);
    }
}

static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream* s, char** str) {
    int32_t parent_name_offset = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, parent_name_offset, &b->parent_name);

    b->position.x = io_read_float_t(s);
    b->position.y = io_read_float_t(s);
    b->position.z = io_read_float_t(s);
    b->rotation.x = io_read_float_t(s);
    b->rotation.y = io_read_float_t(s);
    b->rotation.z = io_read_float_t(s);
    b->scale.x = io_read_float_t(s);
    b->scale.y = io_read_float_t(s);
    b->scale.z = io_read_float_t(s);
}

static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->parent_name));
    io_write_int32_t(s, (int32_t)parent_name_offset);

    io_write_float_t(s, b->position.x);
    io_write_float_t(s, b->position.y);
    io_write_float_t(s, b->position.z);
    io_write_float_t(s, b->rotation.x);
    io_write_float_t(s, b->rotation.y);
    io_write_float_t(s, b->rotation.z);
    io_write_float_t(s, b->scale.x);
    io_write_float_t(s, b->scale.y);
    io_write_float_t(s, b->scale.z);
}

static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, char** str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    b->start_index = io_read_int32_t(s);
    b->count = io_read_int32_t(s);
    b->external_name_index = io_read_uint32_t(s);
    b->name_index = io_read_uint32_t(s);

    int32_t offset = io_read_int32_t(s);
    io_read(s, 0, 0x14);

    b->nodes = 0;

    if (!b->count || !offset)
        return;

    b->nodes_count = b->count;
    b->nodes = force_malloc_s(obj_skin_osage_node, b->count);

    io_position_push(s, offset, SEEK_SET);
    if (~io_read_uint32_t(s) & 0x8000) {
        obj_skin_osage_root_node* root = &b->root;
        root->coli = 0;
        root->coli_count = 0;

        root->unk0 = io_read_int32_t(s);
        root->force = io_read_float_t(s);
        root->force_gain = io_read_float_t(s);
        root->air_res = io_read_float_t(s);
        root->rot_y = io_read_float_t(s);
        root->rot_z = io_read_float_t(s);
        root->hinge_y = io_read_float_t(s);
        root->hinge_z = io_read_float_t(s);
        io_read_string_null_terminated(s, &root->name);
        int32_t coli_offset = io_read_int32_t(s);
        root->coli_r = io_read_float_t(s);
        root->friction = io_read_float_t(s);
        root->wind_afc = io_read_float_t(s);
        root->unk44 = io_read_int32_t(s);

        if (coli_offset) {
            io_set_position(s, coli_offset, SEEK_SET);
            int32_t coli_count = 0;
            while (io_read_int32_t(s)) {
                io_read(s, 44);
                coli_count++;
            }

            root->coli = force_malloc_s(obj_skin_osage_root_coli, coli_count);
            root->coli_count = coli_count;

            io_set_position(s, coli_offset, SEEK_SET);
            for (int32_t i = 0; i < root->coli_count; i++) {
                obj_skin_osage_root_coli* coli = &root->coli[i];
                coli->type = (obj_skin_osage_root_coli_type)io_read_int32_t(s);
                coli->bone0_index = io_read_int32_t(s);
                coli->bone1_index = io_read_int32_t(s);
                coli->radius = io_read_float_t(s);
                coli->bone0_pos.x = io_read_float_t(s);
                coli->bone0_pos.y = io_read_float_t(s);
                coli->bone0_pos.z = io_read_float_t(s);
                coli->bone1_pos.x = io_read_float_t(s);
                coli->bone1_pos.y = io_read_float_t(s);
                coli->bone1_pos.z = io_read_float_t(s);
            }
        }
        b->root_init = true;
    }
    else {
        io_set_position(s, offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            obj_skin_osage_node* node = &b->nodes[i];
            node->name_index = io_read_uint32_t(s);
            node->length = io_read_float_t(s);
            node->rotation.x = io_read_float_t(s);
            node->rotation.y = io_read_float_t(s);
            node->rotation.z = io_read_float_t(s);
        }
        b->root_init = false;
    }
    io_position_pop(s);
}

static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, int64_t* nodes_offset) {
    int32_t nodes_offset_i32 = (int32_t)*nodes_offset;
    if (b->root_init) {
        nodes_offset_i32 = (int32_t)io_get_position(s);

        obj_skin_osage_root_node* root = &b->root;
        int32_t coli_offset = 0;
        if (root->coli) {
            coli_offset = (int32_t)io_get_position(s);
            for (int32_t i = 0; i < root->coli_count; i++) {
                obj_skin_osage_root_coli* coli = &root->coli[i];
                io_write_int32_t(s, coli->type);
                io_write_int32_t(s, coli->bone0_index);
                io_write_int32_t(s, coli->bone1_index);
                io_write_float_t(s, coli->radius);
                io_write_float_t(s, coli->bone0_pos.x);
                io_write_float_t(s, coli->bone0_pos.y);
                io_write_float_t(s, coli->bone0_pos.z);
                io_write_float_t(s, coli->bone1_pos.x);
                io_write_float_t(s, coli->bone1_pos.y);
                io_write_float_t(s, coli->bone1_pos.z);
            }
            io_align_write(s, 0x10);
        }

        io_write_int32_t(s, root->unk0);
        io_write_float_t(s, root->force);
        io_write_float_t(s, root->force_gain);
        io_write_float_t(s, root->air_res);
        io_write_float_t(s, root->rot_y);
        io_write_float_t(s, root->rot_z);
        io_write_float_t(s, root->hinge_y);
        io_write_float_t(s, root->hinge_z);
        io_write_string_null_terminated(s, &root->name);
        io_write_int32_t(s, coli_offset);
        io_write_float_t(s, root->coli_r);
        io_write_float_t(s, root->friction);
        io_write_float_t(s, root->wind_afc);
        io_write_int32_t(s, root->unk44);
        io_write(s, 0x08);
    }

    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    io_write_int32_t(s, b->start_index);
    io_write_int32_t(s, b->count);
    io_write_uint32_t(s, b->external_name_index);
    io_write_uint32_t(s, b->name_index);

    io_write_int32_t(s, nodes_offset_i32);
    io_write(s, 0x14);

    if (!b->root_init && b->nodes) {
        io_position_push(s, *nodes_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            obj_skin_osage_node* node = &b->nodes[i];
            io_write_uint32_t(s, node->name_index);
            io_write_float_t(s, node->length);
            io_write_float_t(s, node->rotation.x);
            io_write_float_t(s, node->rotation.y);
            io_write_float_t(s, node->rotation.z);
        }
        io_position_pop(s);
        *nodes_offset += b->nodes_count * (sizeof(uint32_t) + sizeof(float_t) * 4);
    }
}

static void obj_classic_read_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t num_vertex, obj_vertex_format_file vertex_format_file) {
    obj_vertex_format vertex_format = OBJ_VERTEX_FORMAT_NONE;
    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_POSITION)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_POSITION);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_NORMAL)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_NORMAL);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TANGENT)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TANGENT);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_BINORMAL)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_BINORMAL);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD0)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD0);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD1)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD1);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD2)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD2);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD3)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD3);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_COLOR0)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_COLOR0);

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_COLOR1)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_COLOR1);

    if ((vertex_format_file & (OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT | OBJ_VERTEX_FORMAT_FILE_BONE_INDEX))
        == (OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT | OBJ_VERTEX_FORMAT_FILE_BONE_INDEX))
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_BONE_DATA);
    else
        enum_and(vertex_format_file, ~(OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT | OBJ_VERTEX_FORMAT_FILE_BONE_INDEX));

    if (vertex_format_file & OBJ_VERTEX_FORMAT_FILE_UNKNOWN)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_UNKNOWN);

    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, num_vertex);
    for (uint32_t i = 0; i < 20; i++) {
        obj_vertex_format_file attribute = (obj_vertex_format_file)(1 << i);
        if (~vertex_format_file & attribute)
            continue;

        io_set_position(s, base_offset + vertex[i], SEEK_SET);
        switch (attribute) {
        case OBJ_VERTEX_FORMAT_FILE_POSITION:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].position.x = io_read_float_t(s);
                vtx[j].position.y = io_read_float_t(s);
                vtx[j].position.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_NORMAL:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].normal.x = io_read_float_t(s);
                vtx[j].normal.y = io_read_float_t(s);
                vtx[j].normal.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_TANGENT:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].tangent.x = io_read_float_t(s);
                vtx[j].tangent.y = io_read_float_t(s);
                vtx[j].tangent.z = io_read_float_t(s);
                vtx[j].tangent.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_BINORMAL:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].binormal.x = io_read_float_t(s);
                vtx[j].binormal.y = io_read_float_t(s);
                vtx[j].binormal.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_TEXCOORD0:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord0.x = io_read_float_t(s);
                vtx[j].texcoord0.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_TEXCOORD1:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord1.x = io_read_float_t(s);
                vtx[j].texcoord1.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_TEXCOORD2:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord2.x = io_read_float_t(s);
                vtx[j].texcoord2.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_TEXCOORD3:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].texcoord3.x = io_read_float_t(s);
                vtx[j].texcoord3.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_COLOR0:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].color0.x = io_read_float_t(s);
                vtx[j].color0.y = io_read_float_t(s);
                vtx[j].color0.z = io_read_float_t(s);
                vtx[j].color0.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_COLOR1:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].color1.x = io_read_float_t(s);
                vtx[j].color1.y = io_read_float_t(s);
                vtx[j].color1.z = io_read_float_t(s);
                vtx[j].color1.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].bone_weight.x = io_read_float_t(s);
                vtx[j].bone_weight.y = io_read_float_t(s);
                vtx[j].bone_weight.z = io_read_float_t(s);
                vtx[j].bone_weight.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_BONE_INDEX:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].bone_index.x = (int32_t)io_read_float_t(s);
                vtx[j].bone_index.y = (int32_t)io_read_float_t(s);
                vtx[j].bone_index.z = (int32_t)io_read_float_t(s);
                vtx[j].bone_index.w = (int32_t)io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_FORMAT_FILE_UNKNOWN:
            for (uint32_t j = 0; j < num_vertex; j++) {
                vtx[j].unknown.x = io_read_float_t(s);
                vtx[j].unknown.y = io_read_float_t(s);
                vtx[j].unknown.z = io_read_float_t(s);
                vtx[j].unknown.w = io_read_float_t(s);
            }
            break;
        }
    }
    mesh->vertex = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
}

static void obj_classic_write_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    int64_t base_offset, uint32_t* num_vertex, obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex) {
    obj_vertex_data* vtx = mesh->vertex;
    uint32_t _num_vertex = mesh->num_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    obj_vertex_format_file _vertex_format_file = (obj_vertex_format_file)0;
    uint32_t _size_vertex = 0;
    if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_POSITION);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_NORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_TANGENT);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_BINORMAL);
        _size_vertex += 0x0C;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_TEXCOORD0);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_TEXCOORD1);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_TEXCOORD2);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_TEXCOORD3);
        _size_vertex += 0x08;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_COLOR0);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_COLOR1);
        _size_vertex += 0x10;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT | OBJ_VERTEX_FORMAT_FILE_BONE_INDEX);
        _size_vertex += 0x20;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN) {
        enum_or(_vertex_format_file, OBJ_VERTEX_FORMAT_FILE_UNKNOWN);
        _size_vertex += 0x10;
    }
    *num_vertex = _num_vertex;
    *vertex_format_file = _vertex_format_file;
    *size_vertex = _size_vertex;

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_POSITION) {
        vertex[0] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].position.x);
            io_write_float_t(s, vtx[i].position.y);
            io_write_float_t(s, vtx[i].position.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_NORMAL) {
        vertex[1] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].normal.x);
            io_write_float_t(s, vtx[i].normal.y);
            io_write_float_t(s, vtx[i].normal.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TANGENT) {
        vertex[2] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].tangent.x);
            io_write_float_t(s, vtx[i].tangent.y);
            io_write_float_t(s, vtx[i].tangent.z);
            io_write_float_t(s, vtx[i].tangent.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_BINORMAL) {
        vertex[3] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].binormal.x);
            io_write_float_t(s, vtx[i].binormal.y);
            io_write_float_t(s, vtx[i].binormal.z);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD0) {
        vertex[4] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].texcoord0.x);
            io_write_float_t(s, vtx[i].texcoord0.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_BONE_WEIGHT) {
        vertex[10] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].bone_weight.x);
            io_write_float_t(s, vtx[i].bone_weight.y);
            io_write_float_t(s, vtx[i].bone_weight.z);
            io_write_float_t(s, vtx[i].bone_weight.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_BONE_INDEX) {
        vertex[11] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, (float_t)vtx[i].bone_index.x);
            io_write_float_t(s, (float_t)vtx[i].bone_index.y);
            io_write_float_t(s, (float_t)vtx[i].bone_index.z);
            io_write_float_t(s, (float_t)vtx[i].bone_index.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD1) {
        vertex[5] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].texcoord1.x);
            io_write_float_t(s, vtx[i].texcoord1.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD2) {
        vertex[6] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].texcoord2.x);
            io_write_float_t(s, vtx[i].texcoord2.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_TEXCOORD3) {
        vertex[7] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].texcoord3.x);
            io_write_float_t(s, vtx[i].texcoord3.y);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_COLOR0) {
        vertex[8] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].color0.x);
            io_write_float_t(s, vtx[i].color0.y);
            io_write_float_t(s, vtx[i].color0.z);
            io_write_float_t(s, vtx[i].color0.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_COLOR1) {
        vertex[9] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].color1.x);
            io_write_float_t(s, vtx[i].color1.y);
            io_write_float_t(s, vtx[i].color1.z);
            io_write_float_t(s, vtx[i].color1.w);
        }
    }

    if (_vertex_format_file & OBJ_VERTEX_FORMAT_FILE_UNKNOWN) {
        vertex[12] = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < _num_vertex; i++) {
            io_write_float_t(s, vtx[i].unknown.x);
            io_write_float_t(s, vtx[i].unknown.y);
            io_write_float_t(s, vtx[i].unknown.z);
            io_write_float_t(s, vtx[i].unknown.w);
        }
    }
}

static void obj_set_modern_read_inner(obj_set* os, stream* s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOSD') || !st.data.size())
        return;

    stream s_mosd;
    io_open(&s_mosd, &st.data);
    s_mosd.is_big_endian = st.header.use_big_endian;

    uint32_t version = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
    if (version != 0x05062501) {
        os->is_x = false;
        os->modern = false;
        os->ready = false;
        return;
    }

    bool is_x = true;
    io_set_position(&s_mosd, 0x0C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
    io_set_position(&s_mosd, 0x14, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
    io_set_position(&s_mosd, 0x1C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
    io_set_position(&s_mosd, 0x24, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
    io_set_position(&s_mosd, 0x2C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;

    io_set_position(&s_mosd, 0x04, SEEK_SET);
    obj_set_header osh = {};
    if (!is_x) {
        os->obj_num = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        osh.last_obj_id = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        osh.obj_data = io_read_offset_f2(&s_mosd, st.header.length);
        osh.obj_skin_data = io_read_offset_f2(&s_mosd, st.header.length);
        osh.obj_name_data = io_read_offset_f2(&s_mosd, st.header.length);
        osh.obj_id_data = io_read_offset_f2(&s_mosd, st.header.length);
        osh.tex_id_data = io_read_offset_f2(&s_mosd, st.header.length);
        os->tex_id_num = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        os->reserved[0] = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
        os->reserved[1] = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
    }
    else {
        os->obj_num = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        osh.last_obj_id = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        osh.obj_data = io_read_offset_x(&s_mosd);
        osh.obj_skin_data = io_read_offset_x(&s_mosd);
        osh.obj_name_data = io_read_offset_x(&s_mosd);
        osh.obj_id_data = io_read_offset_x(&s_mosd);
        osh.tex_id_data = io_read_offset_x(&s_mosd);
        os->tex_id_num = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        os->reserved[0] = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
        os->reserved[1] = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
    }

    os->obj_data = force_malloc_s(obj, os->obj_num);

    int64_t* obj_datas = 0;
    if (osh.obj_data) {
        obj_datas = force_malloc_s(int64_t, os->obj_num);
        io_set_position(&s_mosd, osh.obj_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_datas[i] = io_read_offset_f2(&s_mosd, st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_datas[i] = io_read_offset_x(&s_mosd);
    }

    int64_t* obj_skin_datas = 0;
    if (osh.obj_skin_data) {
        obj_skin_datas = force_malloc_s(int64_t, os->obj_num);
        io_set_position(&s_mosd, osh.obj_skin_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_skin_datas[i] = io_read_offset_f2(&s_mosd, st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_skin_datas[i] = io_read_offset_x(&s_mosd);
    }

    int64_t* obj_name_datas = 0;
    if (osh.obj_name_data) {
        obj_name_datas = force_malloc_s(int64_t, os->obj_num);
        io_set_position(&s_mosd, osh.obj_name_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_name_datas[i] = io_read_offset_f2(&s_mosd, st.header.length);
        else
            for (uint32_t i = 0; i < os->obj_num; i++)
                obj_name_datas[i] = io_read_offset_x(&s_mosd);
    }

    if (osh.obj_data)
        for (uint32_t i = 0; i < os->obj_num; i++) {
            obj* obj = &os->obj_data[i];
            if (osh.obj_name_data && obj_name_datas[i])
                io_read_string_null_terminated_offset(&s_mosd,
                    obj_name_datas[i], &obj->name);
        }

    if (osh.obj_id_data) {
        io_set_position(&s_mosd, osh.obj_id_data, SEEK_SET);
        for (uint32_t i = 0; i < os->obj_num; i++)
            os->obj_data[i].id = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
    }

    free(obj_datas);
    free(obj_skin_datas);
    free(obj_name_datas);

    if (osh.tex_id_data) {
        io_set_position(&s_mosd, osh.tex_id_data, SEEK_SET);
        os->tex_id_data = force_malloc_s(uint32_t, os->tex_id_num);
        for (uint32_t i = 0; i < os->tex_id_num; i++)
            os->tex_id_data[i] = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
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
            io_open(&s_oskn, &oskn->data);
            s_oskn.is_big_endian = oskn->header.use_big_endian;
            s_oskn_ptr = &s_oskn;
        }

        if (oidx) {
            io_open(&s_oidx, &oidx->data);
            s_oidx.is_big_endian = oidx->header.use_big_endian;
            s_oidx_ptr = &s_oidx;
        }

        if (ovtx) {
            io_open(&s_ovtx, &ovtx->data);
            s_ovtx.is_big_endian = ovtx->header.use_big_endian;
            s_ovtx_ptr = &s_ovtx;
        }

        obj* obj = &os->obj_data[omdl_index];
        stream s_omdl;
        io_open(&s_omdl, &i.data);
        s_omdl.is_big_endian = i.header.use_big_endian;
        obj_modern_read_model(obj, &s_omdl, 0, i.header.length, is_x, s_oidx_ptr, s_ovtx_ptr);
        io_free(&s_omdl);

        if (s_oskn_ptr)
            obj_modern_read_skin(obj, s_oskn_ptr, 0, oskn->header.length, is_x);

        if (s_oskn_ptr)
            io_free(&s_oskn);
        if (s_oidx_ptr)
            io_free(&s_oidx);
        if (s_ovtx_ptr)
            io_free(&s_ovtx);
        omdl_index++;
    }

    io_free(&s_mosd);

    os->is_x = is_x;
    os->modern = true;
    os->ready = true;
}

static void obj_set_modern_write_inner(obj_set* os, stream* s) {
    stream s_mosd;
    io_open(&s_mosd);
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = os->is_x;

    obj_set_header osh = {};
    osh.last_obj_id = -1;

    uint32_t count = os->obj_num;

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
        off += (uint32_t)(count * 4ULL);
    else
        off += (uint32_t)(count * 8ULL);
    off = align_val(off, 0x10);

    if (!is_x)
        off += (uint32_t)(count * 4ULL);
    else
        off += (uint32_t)(count * 8ULL);
    off = align_val(off, 0x10);

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

    ee = { off, 1, 4, (uint32_t)count };
    ee.append(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    off = (uint32_t)(count * 4ULL);
    off = align_val(off, 0x10);

    ee = { off, 1, 4, (uint32_t)os->tex_id_num };
    ee.append(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    off = (uint32_t)(os->tex_id_num * 4ULL);
    off = align_val(off, 0x10);

    if (!is_x) {
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
    }
    else {
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_align_write(&s_mosd, 0x10);
    }

    osh.obj_data = io_get_position(&s_mosd);
    if (!is_x)
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, 0, 0x20);
    else
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, 0);
    io_align_write(&s_mosd, 0x10);

    osh.obj_skin_data = io_get_position(&s_mosd);
    if (!is_x)
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, 0, 0x20);
    else
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, 0);
    io_align_write(&s_mosd, 0x10);

    osh.obj_name_data = io_get_position(&s_mosd);
    if (!is_x)
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
    else
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_x_pof_add(&s_mosd, 0, &pof);
    io_align_write(&s_mosd, 0x10);

    osh.obj_id_data = io_get_position(&s_mosd);
    for (uint32_t i = 0; i < count; i++)
        io_write_uint32_t(&s_mosd, os->obj_data[i].id);
    io_align_write(&s_mosd, 0x10);

    osh.tex_id_data = io_get_position(&s_mosd);
    for (uint32_t i = 0; i < os->tex_id_num; i++)
        io_write_uint32_t(&s_mosd, os->tex_id_data[i]);
    io_align_write(&s_mosd, 0x10);

    int64_t* obj_name_datas = force_malloc_s(int64_t, count);
    for (uint32_t i = 0; i < count; i++) {
        obj_name_datas[i] = (int32_t)io_get_position(&s_mosd);
        io_write_string_null_terminated(&s_mosd, &os->obj_data[i].name);
    }
    io_align_write(&s_mosd, 0x10);

    io_position_push(&s_mosd, osh.obj_name_data, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, obj_name_datas[i], 0x20);
    else
        for (uint32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, obj_name_datas[i]);
    io_position_pop(&s_mosd);
    free(obj_name_datas);

    io_position_push(&s_mosd, 0x00, SEEK_SET);
    if (!is_x) {
        io_write_uint32_t(&s_mosd, 0x05062501);
        io_write_int32_t(&s_mosd, os->obj_num);
        io_write_int32_t(&s_mosd, -1);
        io_write_offset_f2(&s_mosd, osh.obj_data, 0x20);
        io_write_offset_f2(&s_mosd, osh.obj_skin_data, 0x20);
        io_write_offset_f2(&s_mosd, osh.obj_name_data, 0x20);
        io_write_offset_f2(&s_mosd, osh.obj_id_data, 0x20);
        io_write_offset_f2(&s_mosd, osh.tex_id_data, 0x20);
        io_write_int32_t(&s_mosd, os->tex_id_num);
        io_write_uint32_t(&s_mosd, os->reserved[0]);
        io_write_uint32_t(&s_mosd, os->reserved[1]);
    }
    else {
        io_write_uint32_t(&s_mosd, 0x05062501);
        io_write_int32_t(&s_mosd, os->obj_num);
        io_write_int32_t(&s_mosd, -1);
        io_write_offset_x(&s_mosd, osh.obj_data);
        io_write_offset_x(&s_mosd, osh.obj_skin_data);
        io_write_offset_x(&s_mosd, osh.obj_name_data);
        io_write_offset_x(&s_mosd, osh.obj_id_data);
        io_write_offset_x(&s_mosd, osh.tex_id_data);
        io_write_int32_t(&s_mosd, os->tex_id_num);
        io_write_uint32_t(&s_mosd, os->reserved[0]);
        io_write_uint32_t(&s_mosd, os->reserved[1]);
    }
    io_position_pop(&s_mosd);

    f2_struct st;
    for (uint32_t i = 0; i < count; i++) {
        obj* obj = &os->obj_data[i];

        st.sub_structs.push_back({});
        f2_struct* omdl = &st.sub_structs.back();

        stream s_omdl;
        io_open(&s_omdl);

        if (obj->skin_init) {
            st.sub_structs.push_back({});
            f2_struct* oskn = &st.sub_structs.back();

            stream s_oskn;
            io_open(&s_oskn);

            obj_modern_write_skin(obj, &s_oskn, 0, is_x, oskn);

            io_align_write(&s_oskn, 0x10);
            io_copy(&s_oskn, &oskn->data);
            io_free(&s_oskn);

            oskn->header.signature = reverse_endianness_uint32_t('OSKN');
            oskn->header.length = 0x20;
            oskn->header.use_big_endian = false;
            oskn->header.use_section_size = true;
        }

        obj_modern_write_model(obj, &s_omdl, 0, is_x, omdl);

        io_align_write(&s_omdl, 0x10);
        io_copy(&s_omdl, &omdl->data);
        io_free(&s_omdl);

        omdl->header.signature = reverse_endianness_uint32_t('OMDL');
        omdl->header.length = 0x20;
        omdl->header.use_big_endian = false;
        omdl->header.use_section_size = true;
    }

    io_align_write(&s_mosd, 0x10);
    io_copy(&s_mosd, &st.data);
    io_free(&s_mosd);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSD');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    st.write(s, true, os->is_x);
}

static void obj_modern_read_index(obj* obj, stream* s, obj_sub_mesh* sub_mesh) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t* indices = force_malloc_s(uint32_t, sub_mesh->indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++) {
            uint8_t idx = io_read_uint8_t(s);
            indices[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++) {
            uint16_t idx = io_read_uint16_t_stream_reverse_endianness(s);
            indices[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < sub_mesh->indices_count; i++)
            indices[i] = io_read_uint32_t_stream_reverse_endianness(s);
        break;
    }
    sub_mesh->indices = indices;
}

static void obj_modern_write_index(obj* obj, stream* s, bool is_x,
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
            io_write_uint8_t(s, (uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < _indices_count; i++)
            io_write_uint16_t(s, (uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < _indices_count; i++)
            io_write_uint32_t(s, indices[i]);
        break;
    }
    io_align_write(s, 0x04);
}

static void obj_modern_read_model(obj* obj, stream* s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    io_set_position(s, base_offset, SEEK_SET);

    obj_header oh = {};
    if (!is_x) {
        io_read_uint32_t(s); // version
        io_read_uint32_t(s); // flags
        obj->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
        obj->num_mesh = io_read_int32_t_stream_reverse_endianness(s);
        oh.mesh_array = io_read_offset_f2(s, header_length);
        obj->num_material = io_read_int32_t_stream_reverse_endianness(s);
        oh.material_array = io_read_offset_f2(s, header_length);
        obj->reserved[0] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[1] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[2] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[3] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[4] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[5] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[6] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[7] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[8] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[9] = io_read_uint32_t_stream_reverse_endianness(s);
    }
    else {
        io_read_uint32_t(s); // version
        io_read_uint32_t(s); // flags
        obj->num_mesh = io_read_int32_t_stream_reverse_endianness(s);
        obj->num_material = io_read_int32_t_stream_reverse_endianness(s);
        obj->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
        obj->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
        oh.mesh_array = io_read_offset_x(s);
        oh.material_array = io_read_offset_x(s);
        io_read(s, 0, 0x10);
        obj->flags = io_read_uint8_t(s);
        io_read(s, 0, 0x07);
        obj->reserved[0] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[1] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[2] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[3] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[4] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[5] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[6] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[7] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[8] = io_read_uint32_t_stream_reverse_endianness(s);
        obj->reserved[9] = io_read_uint32_t_stream_reverse_endianness(s);
    }

    if (oh.mesh_array > 0) {
        obj->mesh_array = force_malloc_s(obj_mesh, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            io_set_position(s, base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);

            obj_mesh_header mh = {};
            mesh->flags = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
            mesh->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
            mesh->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
            mesh->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
            mesh->num_submesh = io_read_int32_t_stream_reverse_endianness(s);
            mh.submesh_array = io_read_offset(s, header_length, is_x);
            mh.format = (obj_vertex_format_file)io_read_uint32_t_stream_reverse_endianness(s);
            mh.size_vertex = io_read_int32_t_stream_reverse_endianness(s);
            mh.num_vertex = io_read_int32_t_stream_reverse_endianness(s);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    mh.vertex[j] = io_read_offset_f2(s, 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    mh.vertex[j] = io_read_offset_x(s);

            mesh->attrib.w = io_read_uint32_t_stream_reverse_endianness(s);
            mh.vertex_flags = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[0] = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[1] = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[2] = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[3] = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[4] = io_read_uint32_t_stream_reverse_endianness(s);
            mesh->reserved[5] = io_read_uint32_t_stream_reverse_endianness(s);
            io_read(s, &mesh->name, sizeof(mesh->name));
            mesh->name[sizeof(mesh->name) - 1] = 0;

            if (mh.submesh_array) {
                mesh->submesh_array = force_malloc_s(obj_sub_mesh, mesh->num_submesh);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                    io_set_position(s, base_offset + mh.submesh_array + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh = {};
                    sub_mesh->flags = io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->material_index = io_read_uint32_t_stream_reverse_endianness(s);
                    io_read(s, &sub_mesh->uv_index, 0x08);
                    sub_mesh->bone_indices_count = io_read_int32_t_stream_reverse_endianness(s);
                    smh.bone_indices_offset = io_read_offset(s, header_length, is_x);
                    sub_mesh->bones_per_vertex = io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh->primitive_type = (obj_primitive_type)io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh->index_format = (obj_index_format)io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh->indices_count = io_read_int32_t_stream_reverse_endianness(s);
                    smh.indices_offset = io_read_offset(s, 0, is_x);
                    sub_mesh->attrib.w = io_read_uint32_t_stream_reverse_endianness(s);
                    io_read(s, 0, 0x10);
                    sub_mesh->bounding_box.center.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_box.center.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_box.center.z = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_box.size.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_box.size.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh->bounding_box.size.z = io_read_float_t_stream_reverse_endianness(s);
                    io_read(s, 0, 0x04);
                    sub_mesh->indices_offset = io_read_uint32_t_stream_reverse_endianness(s);

                    if (is_x)
                        io_read(s, 0, 0x04);

                    if (sub_mesh->bones_per_vertex == 4 && smh.bone_indices_offset) {
                        sub_mesh->bone_indices = force_malloc_s(uint16_t, sub_mesh->bone_indices_count);
                        io_set_position(s, base_offset + smh.bone_indices_offset, SEEK_SET);
                        io_read(s, sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                        if (s->is_big_endian) {
                            uint16_t* bone_indices = sub_mesh->bone_indices;
                            for (uint32_t k = 0; k < sub_mesh->bone_indices_count; k++)
                                bone_indices[k] = reverse_endianness_uint16_t(bone_indices[k]);
                        }
                    }

                    io_set_position(s_oidx, smh.indices_offset, SEEK_SET);
                    obj_modern_read_index(obj, s_oidx, sub_mesh);
                }
            }

            obj_modern_read_vertex(obj, s_ovtx, mh.vertex, mesh,
                mh.vertex_flags, mh.num_vertex, mh.size_vertex);
        }
    }

    if (oh.material_array > 0) {
        obj_material_texture_enrs_table_init();

        io_set_position(s, base_offset + oh.material_array, SEEK_SET);
        obj->material_array = force_malloc_s(obj_material_data, obj->num_material);
        io_read(s, obj->material_array, obj->num_material * sizeof(obj_material_data));
        if (s->is_big_endian)
            for (uint32_t i = 0; i < obj->num_material; i++)
                obj_material_texture_enrs_table.apply(&obj->material_array[i]);
    }
}

static void obj_modern_write_model(obj* obj, stream* s,
    int64_t base_offset, bool is_x, f2_struct* omdl) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    omdl->sub_structs.push_back({});
    omdl->sub_structs.push_back({});
    f2_struct* oidx = &omdl->sub_structs.end()[-2];
    f2_struct* ovtx = &omdl->sub_structs.back();

    stream s_oidx;
    stream s_ovtx;
    io_open(&s_oidx);
    io_open(&s_ovtx);

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
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
    }
    else {
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write(s, 0x10);
        io_write_uint8_t(s, 0);
        io_write(s, 0x07);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
    }
    io_align_write(s, 0x10);

    if (obj->num_mesh) {
        oh.mesh_array = io_get_position(s) - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, obj->num_mesh);
        obj_sub_mesh_header** smhss = force_malloc_s(obj_sub_mesh_header*, obj->num_mesh);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];

            io_write(s, 0x04);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_int32_t(s, 0);
            io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
            io_write_uint32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    io_write_offset_f2(s, 0, 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    io_write_offset_x(s, 0);

            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write(s, 0x18);
            io_write(s, sizeof(mesh->name));
        }

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            mh->format = OBJ_VERTEX_FORMAT_FILE_MODERN_STORAGE;
            if (mesh->num_submesh) {
                mh->submesh_array = io_get_position(s) - base_offset;
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    io_write(s, 0x04);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0x08);
                    io_write_int32_t(s, 0);
                    io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_offset(s, 0, 0, is_x);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0x10);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write(s, 0x04);
                    io_write_uint32_t(s, 0);

                    if (is_x)
                        io_write(s, 0x04);
                }
            }

            obj_modern_write_vertex(obj, &s_ovtx, is_x, mh->vertex, mesh,
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
                        smh->bone_indices_offset = io_get_position(s) - base_offset;
                        io_write(s, sub_mesh->bone_indices, sub_mesh->bone_indices_count * sizeof(uint16_t));
                    }

                    smh->indices_offset = io_get_position(&s_oidx);
                    obj_modern_write_index(obj, &s_oidx, is_x, sub_mesh, oidx);
                }
            }
        }
        io_align_write(s, is_x ? 0x10 : 0x04);

        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = smhss[i];

            if (mesh->num_submesh) {
                io_position_push(s, base_offset + mh->submesh_array, SEEK_SET);
                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    io_write_uint32_t(s, sub_mesh->flags);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.x);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.y);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.z);
                    io_write_float_t(s, sub_mesh->bounding_sphere.radius);
                    io_write_uint32_t(s, sub_mesh->material_index);
                    io_write(s, &sub_mesh->uv_index, 0x08);
                    io_write_int32_t(s, sub_mesh->bone_indices_count);
                    io_write_offset(s, smh->bone_indices_offset, 0x20, is_x);
                    io_write_uint32_t(s, sub_mesh->bones_per_vertex);
                    io_write_uint32_t(s, sub_mesh->primitive_type);
                    io_write_uint32_t(s, sub_mesh->index_format);
                    io_write_int32_t(s, sub_mesh->indices_count);
                    io_write_offset(s, smh->indices_offset, 0, is_x);
                    io_write_uint32_t(s, sub_mesh->attrib.w);
                    io_write(s, 0x10);
                    io_write_float_t(s, sub_mesh->bounding_box.center.x);
                    io_write_float_t(s, sub_mesh->bounding_box.center.y);
                    io_write_float_t(s, sub_mesh->bounding_box.center.z);
                    io_write_float_t(s, sub_mesh->bounding_box.size.x);
                    io_write_float_t(s, sub_mesh->bounding_box.size.y);
                    io_write_float_t(s, sub_mesh->bounding_box.size.z);

                    uint32_t index_max = 0;
                    for (uint32_t k = 0; k < sub_mesh->indices_count; k++)
                        if (index_max < sub_mesh->indices[k])
                            index_max = sub_mesh->indices[k];

                    switch (sub_mesh->index_format) {
                    case OBJ_INDEX_U8:
                        io_write_uint32_t(s, index_max << 24);
                        break;
                    case OBJ_INDEX_U16:
                        io_write_uint32_t(s, index_max << 16);
                        break;
                    case OBJ_INDEX_U32:
                        io_write_uint32_t(s, index_max);
                        break;
                    }
                    io_write_uint32_t(s, sub_mesh->indices_offset);

                    if (is_x)
                        io_write(s, 0x04);
                }
                io_position_pop(s);
            }
            free(smhs);
        }

        io_position_push(s, base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            io_write_uint32_t(s, mesh->flags);
            io_write_float_t(s, mesh->bounding_sphere.center.x);
            io_write_float_t(s, mesh->bounding_sphere.center.y);
            io_write_float_t(s, mesh->bounding_sphere.center.z);
            io_write_float_t(s, mesh->bounding_sphere.radius);
            io_write_int32_t(s, mesh->num_submesh);
            if (mh->submesh_array && !is_x)
                mh->submesh_array += 0x20;
            io_write_offset(s, mh->submesh_array, 0, is_x);
            io_write_uint32_t(s, mh->format);
            io_write_int32_t(s, mh->size_vertex);
            io_write_int32_t(s, mh->num_vertex);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    io_write_offset_f2(s, mh->vertex[j], 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    io_write_offset_x(s, mh->vertex[j]);

            io_write_uint32_t(s, mesh->attrib.w);
            io_write_uint32_t(s, mh->vertex_flags);
            io_write_uint32_t(s, mesh->reserved[0]);
            io_write_uint32_t(s, mesh->reserved[1]);
            io_write_uint32_t(s, mesh->reserved[2]);
            io_write_uint32_t(s, mesh->reserved[3]);
            io_write_uint32_t(s, mesh->reserved[4]);
            io_write_uint32_t(s, mesh->reserved[5]);
            io_write(s, &mesh->name, sizeof(mesh->name) - 1);
            io_write_char(s, '\0');
        }
        io_position_pop(s);
        free(mhs);
        free(smhss);
    }

    if (obj->material_array) {
        oh.material_array = io_get_position(s) - base_offset;
        for (uint32_t i = 0; i < obj->num_material; i++)
            io_write(s, &obj->material_array[i], sizeof(obj_material_data));
    }
    io_align_write(s, 0x10);

    io_position_push(s, base_offset, SEEK_SET);
    if (!is_x) {
        io_write_uint32_t(s, 0x10000); // version
        io_write_uint32_t(s, 0x00); // flags
        io_write_float_t(s, obj->bounding_sphere.center.x);
        io_write_float_t(s, obj->bounding_sphere.center.y);
        io_write_float_t(s, obj->bounding_sphere.center.z);
        io_write_float_t(s, obj->bounding_sphere.radius);
        io_write_int32_t(s, obj->num_mesh);
        io_write_offset_f2(s, oh.mesh_array, 0x20);
        io_write_int32_t(s, obj->num_material);
        io_write_offset_f2(s, oh.material_array, 0x20);
        io_write_uint32_t(s, obj->reserved[0]);
        io_write_uint32_t(s, obj->reserved[1]);
        io_write_uint32_t(s, obj->reserved[2]);
        io_write_uint32_t(s, obj->reserved[3]);
        io_write_uint32_t(s, obj->reserved[4]);
        io_write_uint32_t(s, obj->reserved[5]);
        io_write_uint32_t(s, obj->reserved[6]);
        io_write_uint32_t(s, obj->reserved[7]);
        io_write_uint32_t(s, obj->reserved[8]);
        io_write_uint32_t(s, obj->reserved[9]);
    }
    else {
        io_write_uint32_t(s, 0x10000); // version
        io_write_uint32_t(s, 0x00); // flags
        io_write_int32_t(s, obj->num_mesh);
        io_write_int32_t(s, obj->num_material);
        io_write_float_t(s, obj->bounding_sphere.center.x);
        io_write_float_t(s, obj->bounding_sphere.center.y);
        io_write_float_t(s, obj->bounding_sphere.center.z);
        io_write_float_t(s, obj->bounding_sphere.radius);
        io_write_offset_x(s, oh.mesh_array);
        io_write_offset_x(s, oh.material_array);
        io_write(s, 0x10);
        io_write_uint8_t(s, obj->flags);
        io_write(s, 0x07);
        io_write_uint32_t(s, obj->reserved[0]);
        io_write_uint32_t(s, obj->reserved[1]);
        io_write_uint32_t(s, obj->reserved[2]);
        io_write_uint32_t(s, obj->reserved[3]);
        io_write_uint32_t(s, obj->reserved[4]);
        io_write_uint32_t(s, obj->reserved[5]);
        io_write_uint32_t(s, obj->reserved[6]);
        io_write_uint32_t(s, obj->reserved[7]);
        io_write_uint32_t(s, obj->reserved[8]);
        io_write_uint32_t(s, obj->reserved[9]);
    }
    io_position_pop(s);

    omdl->enrs = e;
    omdl->pof = pof;

    io_align_write(&s_oidx, 0x10);
    io_copy(&s_oidx, &oidx->data);
    io_free(&s_oidx);

    oidx->header.signature = reverse_endianness_uint32_t('OIDX');
    oidx->header.length = 0x20;
    oidx->header.use_big_endian = false;
    oidx->header.use_section_size = true;

    io_align_write(&s_ovtx, 0x10);
    io_copy(&s_ovtx, &ovtx->data);
    io_free(&s_ovtx);

    ovtx->header.signature = reverse_endianness_uint32_t('OVTX');
    ovtx->header.length = 0x20;
    ovtx->header.use_big_endian = false;
    ovtx->header.use_section_size = true;
}

static void obj_modern_read_skin(obj* obj, stream* s, int64_t base_offset,
    uint32_t header_length, bool is_x) {
    obj_skin* sk = &obj->skin;
    obj->skin_init = true;
    io_set_position(s, base_offset, SEEK_SET);

    obj_skin_header sh = {};
    if (!is_x) {
        sh.bone_ids_offset = io_read_offset_f2(s, header_length);
        sh.bone_matrices_offset = io_read_offset_f2(s, header_length);
        sh.bone_names_offset = io_read_offset_f2(s, header_length);
        sh.ex_data_offset = io_read_offset_f2(s, header_length);
        sk->bones_count = io_read_int32_t_stream_reverse_endianness(s);
        sh.bone_parent_ids_offset = io_read_offset_f2(s, header_length);
        io_read(s, 0, 0x0C);
    }
    else {
        sh.bone_ids_offset = io_read_offset_x(s);
        sh.bone_matrices_offset = io_read_offset_x(s);
        sh.bone_names_offset = io_read_offset_x(s);
        sh.ex_data_offset = io_read_offset_x(s);
        sk->bones_count = io_read_int32_t_stream_reverse_endianness(s);
        sh.bone_parent_ids_offset = io_read_offset_x(s);
        io_read(s, 0, 0x18);
    }

    if (sh.bone_ids_offset) {
        sk->bones = force_malloc_s(obj_skin_bone, sk->bones_count);

        int64_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(int64_t, sk->bones_count);
            io_set_position(s, sh.bone_names_offset, SEEK_SET);
            if (!is_x)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    bone_names_offsets[i] = io_read_offset_f2(s, header_length);
            else
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    bone_names_offsets[i] = io_read_offset_x(s);
        }

        if (sh.bone_ids_offset) {
            io_set_position(s, sh.bone_ids_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->bones_count; i++)
                sk->bones[i].id = io_read_uint32_t_stream_reverse_endianness(s);

            if (sh.bone_matrices_offset) {
                io_set_position(s, sh.bone_matrices_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++) {
                    mat4u& mat = sk->bones[i].inv_bind_pose_mat;
                    mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
                }
            }

            if (sh.bone_names_offset)
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    io_read_string_null_terminated_offset(s,
                        bone_names_offsets[i], &sk->bones[i].name);

            if (sh.bone_parent_ids_offset) {
                io_set_position(s, sh.bone_parent_ids_offset, SEEK_SET);
                for (uint32_t i = 0; i < sk->bones_count; i++)
                    sk->bones[i].parent = io_read_uint32_t_stream_reverse_endianness(s);
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        io_set_position(s, sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh = {};
        if (!is_x) {
            exh.osage_count = io_read_int32_t_stream_reverse_endianness(s);
            ex->osage_nodes_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x04);
            exh.osage_nodes_offset = io_read_offset_f2(s, header_length);
            exh.osage_names_offset = io_read_offset_f2(s, header_length);
            exh.blocks_offset = io_read_offset_f2(s, header_length);
            exh.bone_names_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.bone_names_offset = io_read_offset_f2(s, header_length);
            exh.osage_sibling_infos_offset = io_read_offset_f2(s, header_length);
            exh.cloth_count = io_read_int32_t_stream_reverse_endianness(s);
            ex->reserved[0] = io_read_offset_f2(s, header_length);
            ex->reserved[1] = io_read_offset_f2(s, header_length);
            ex->reserved[2] = io_read_offset_f2(s, header_length);
            ex->reserved[3] = io_read_offset_f2(s, header_length);
            ex->reserved[4] = io_read_offset_f2(s, header_length);
            ex->reserved[5] = io_read_offset_f2(s, header_length);
            ex->reserved[6] = io_read_offset_f2(s, header_length);
        }
        else {
            exh.osage_count = io_read_int32_t_stream_reverse_endianness(s);
            ex->osage_nodes_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x08);
            exh.osage_nodes_offset = io_read_offset_x(s);
            exh.osage_names_offset = io_read_offset_x(s);
            exh.blocks_offset = io_read_offset_x(s);
            exh.bone_names_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.bone_names_offset = io_read_offset_x(s);
            exh.osage_sibling_infos_offset = io_read_offset_x(s);
            exh.cloth_count = io_read_int32_t_stream_reverse_endianness(s);
            ex->reserved[0] = io_read_offset_x(s);
            ex->reserved[1] = io_read_offset_x(s);
            ex->reserved[2] = io_read_offset_x(s);
            ex->reserved[3] = io_read_offset_x(s);
            ex->reserved[4] = io_read_offset_x(s);
            ex->reserved[5] = io_read_offset_x(s);
            ex->reserved[6] = io_read_offset_x(s);
        }

        if (exh.bone_names_offset) {
            std::vector<std::string> bone_names;
            bone_names.reserve(exh.bone_names_count);
            int32_t* strings_offsets = force_malloc_s(int32_t, exh.bone_names_count);
            io_set_position(s, exh.bone_names_offset, SEEK_SET);
            for (uint32_t i = 0; i < exh.bone_names_count; i++)
                strings_offsets[i] = io_read_int32_t(s);

            size_t buf_size = 0;
            for (uint32_t i = 0; i < exh.bone_names_count; i++) {
                if (strings_offsets[i]) {
                    std::string str;
                    io_read_string_null_terminated_offset(s, strings_offsets[i], &str);
                    bone_names.push_back(str);
                }
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

            io_set_position(s, exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];

                osage_node->name_index = io_read_uint32_t_stream_reverse_endianness(s);
                osage_node->length = io_read_float_t_stream_reverse_endianness(s);
                io_read(s, 0, 0x04);
            }
        }

        if (exh.blocks_offset) {
            ex->blocks_count = 0;
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            if (!is_x)
                while (io_read_int32_t(s)) {
                    io_read(s, 0, 0x04);
                    ex->blocks_count++;
                }
            else
                while (io_read_int64_t(s)) {
                    io_read(s, 0, 0x08);
                    ex->blocks_count++;
                }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    bhs[i].block_signature_offset = io_read_offset_f2(s, header_length);
                    bhs[i].block_offset = io_read_offset_f2(s, header_length);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    bhs[i].block_signature_offset = io_read_offset_x(s);
                    bhs[i].block_offset = io_read_offset_x(s);
                }

            ex->blocks = force_malloc_s(obj_skin_block, ex->blocks_count);
            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];

                std::string block_signature;
                io_read_string_null_terminated_offset(s,
                    bhs[i].block_signature_offset, &block_signature);
                if (block_signature.size() != 3)
                    continue;

                uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
                switch (signature) {
                case 'CLS\0':
                    block->type = OBJ_SKIN_BLOCK_CLOTH;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_cloth(&block->cloth, s,
                        header_length, bone_names, is_x);
                    break;
                case 'CNS\0':
                    block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_constraint(&block->constraint, s,
                        header_length, bone_names, is_x);
                    break;
                case 'EXP\0':
                    block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_expression(&block->expression, s,
                        header_length, bone_names, is_x);
                    break;
                case 'MOT\0':
                    block->type = OBJ_SKIN_BLOCK_MOTION;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
                    obj_modern_read_skin_block_motion(&block->motion, s,
                        header_length, bone_names, is_x);
                    break;
                case 'OSG\0':
                    block->type = OBJ_SKIN_BLOCK_OSAGE;
                    io_set_position(s, bhs[i].block_offset, SEEK_SET);
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
            io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
            while (io_read_uint32_t(s)) {
                io_read(s, 0, 0x08);
                ex->osage_sibling_infos_count++;
            }

            obj_skin_osage_sibling_info* osis = force_malloc_s(
                obj_skin_osage_sibling_info, ex->osage_sibling_infos_count);
            ex->osage_sibling_infos = osis;
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                osis[i].name_index = io_read_uint32_t_stream_reverse_endianness(s);
                osis[i].sibling_name_index = io_read_uint32_t_stream_reverse_endianness(s);
                osis[i].max_distance = io_read_float_t_stream_reverse_endianness(s);
            }

            io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
        }
    }
}

static void obj_modern_write_skin(obj* obj, stream* s,
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
    int64_t field_18 = 0;
    int64_t field_1C = 0;
    int64_t field_20 = 0;
    int64_t field_24 = 0;
    int64_t field_28 = 0;
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
                obj_skin_strings_push_back_check(strings, string_data(&cloth->mesh_name));
                obj_skin_strings_push_back_check(strings, string_data(&cloth->backface_mesh_name));

                for (uint32_t k = 0; k < cloth->count; k++) {
                    obj_skin_block_cloth_field_1C* sub = &cloth->field_1C[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings,
                            string_data(&sub->sub_data[l].bone_name));
                }
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &block->constraint;
                obj_skin_strings_push_back_check(strings, string_data(&constraint->base.parent_name));
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, constraint->name_index);
                obj_skin_strings_push_back_check(strings, string_data(&constraint->source_node_name));
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->direction.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->position.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings,
                        string_data(&constraint->distance.up_vector.name));
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &block->expression;
                for (uint32_t j = 0; j < expression->expressions_count; j++)
                    obj_skin_strings_push_back_check(strings, string_data(&expression->expressions[j]));
                obj_skin_strings_push_back_check(strings, string_data(&expression->base.parent_name));
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_names_ptr, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_names_ptr, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &block->motion;
                obj_skin_strings_push_back_check(strings, string_data(&motion->base.parent_name));
                obj_skin_strings_push_back_check(strings, string_data(&motion->name));
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_names_ptr, motion->nodes[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                if (!is_x)
                    obj_skin_strings_push_back_check(bone_names, string_data(&motion->name));
                for (uint32_t j = 0; j < motion->nodes_count; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_names_ptr, motion->nodes[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &block->osage;
                obj_skin_strings_push_back_check(strings, string_data(&osage->base.parent_name));
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
                int32_t field_18_count = 0;
                int32_t field_1C_count = 0;
                int32_t field_20_count = 0;
                int32_t field_24_count = 0;
                int32_t field_28_count = 0;
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                        for (uint32_t j = 0; j < 32; j++)
                            if (block->cloth.field_10 & (1 << j))
                                field_18_count++;

                        field_1C_count += block->cloth.field_1C ? block->cloth.count : 0;
                        field_20_count += block->cloth.field_20 ? block->cloth.count : 0;
                        field_24_count += block->cloth.field_24 ? block->cloth.field_24_count + 1 : 0;
                        field_28_count += block->cloth.field_28 ? block->cloth.field_28_count + 1 : 0;
                    }
                }

                if (field_18_count) {
                    ee = { off, 1, 64, (uint32_t)field_18_count };
                    ee.append(0, 16, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(field_18_count * 64ULL);
                    off = align_val(off, 0x10);
                }

                if (field_1C_count) {
                    if (!is_x) {
                        ee = { off, 1, 104, (uint32_t)field_1C_count };
                        ee.append(0, 26, ENRS_DWORD);
                        e.vec.push_back(ee);
                        off = (uint32_t)(field_1C_count * 104ULL);
                    }
                    else {
                        ee = { off, 9, 136, (uint32_t)field_1C_count };
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
                        off = (uint32_t)(field_1C_count * 136ULL);
                    }
                    off = align_val(off, 0x10);
                }

                if (field_20_count) {
                    ee = { off, 1, 440, (uint32_t)field_20_count };
                    ee.append(0, 110, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(field_20_count * 440ULL);
                    off = align_val(off, 0x10);
                }

                if (field_24_count) {
                    ee = { off, 1, 2, (uint32_t)field_24_count };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(field_24_count * 2ULL);
                    off = align_val(off, 0x10);
                }

                if (field_28_count) {
                    ee = { off, 1, 2, (uint32_t)field_28_count };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(field_28_count * 2ULL);
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
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write(s, 0x0C);
    }
    else {
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_int32_t(s, 0);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write(s, 0x18);
    }
    io_align_write(s, 0x10);

    if (sk->bones_count) {
        sh.bone_ids_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++)
            io_write_int32_t(s, sk->bones[i].id);
        io_align_write(s, 0x10);

        sh.bone_parent_ids_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++)
            io_write_int32_t(s, sk->bones[i].parent);
        io_align_write(s, 0x10);

        sh.bone_names_offset = io_get_position(s);
        if (!is_x)
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                obj_skin_strings_push_back_check(strings, string_data(&sk->bones[i].name));
            }
        else
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                io_write_offset_x_pof_add(s, 0, &pof);
                obj_skin_strings_push_back_check(strings, string_data(&sk->bones[i].name));
            }
        io_align_write(s, 0x10);

        sh.bone_matrices_offset = io_get_position(s);
        for (uint32_t i = 0; i < sk->bones_count; i++) {
            mat4u& mat = sk->bones[i].inv_bind_pose_mat;
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
        }
        io_align_write(s, 0x10);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = io_get_position(s);
        if (!is_x) {
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write(s, 0x04);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        }
        else {
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write(s, 0x08);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
        }
        io_align_write(s, 0x10);

        if (ex->osage_nodes_count) {
            exh.osage_nodes_offset = io_get_position(s);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_align_write(s, 0x10);

            exh.osage_sibling_infos_offset = io_get_position(s);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
        }

        if (exh.osage_count || exh.cloth_count) {
            exh.osage_names_offset = io_get_position(s);
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
            io_align_write(s, 0x10);
        }

        exh.bone_names_offset = io_get_position(s);
        if (!is_x)
            for (std::string& i : bone_names)
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        else
            for (std::string& i : bone_names)
                io_write_offset_x_pof_add(s, 0, &pof);
        io_align_write(s, 0x10);

        if (ex->blocks_count > 0) {
            exh.blocks_offset = io_get_position(s);
            if (!is_x) {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
                io_write_offset_f2(s, 0, 0x20);
                io_write_offset_f2(s, 0, 0x20);
            }
            else {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
                io_write_offset_x(s, 0);
                io_write_offset_x(s, 0);
            }
            io_align_write(s, 0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->blocks_count);
            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x24);
                    io_write(s, 0x24);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = &block->osage;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x28);
                    if (osage->motion_node_name.length) {
                        io_write(s, 0x18);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0x18);
                    }
                    else
                        io_write(s, 0x38);
                }
            io_align_write(s, 0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;

                    obj_skin_block_expression* expression = &block->expression;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x04);

                    for (uint32_t j = 0; j < expression->expressions_count && j < 9; j++)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                    for (uint32_t j = expression->expressions_count; j < 9; j++)
                        io_write(s, 0x04);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;
                    obj_skin_block_expression* expression = &block->expression;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x08);

                    for (uint32_t j = 0; j < expression->expressions_count && j < 9; j++)
                        io_write_offset_x_pof_add(s, 0, &pof);

                    for (uint32_t j = expression->expressions_count; j < 9; j++)
                        io_write(s, 0x08);
                }
            io_align_write(s, 0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x10);

                    if (cloth->field_10)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        io_write(s, 0x04);

                    if (cloth->count) {
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    }
                    else
                        io_write(s, 0x08);

                    if (cloth->field_24_count)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        io_write(s, 0x04);

                    if (cloth->field_28_count)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        io_write(s, 0x04);

                    io_write(s, 0x08);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x10);

                    if (cloth->field_10)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        io_write(s, 0x08);

                    if (cloth->count) {
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                    }
                    else
                        io_write(s, 0x10);

                    if (cloth->field_24_count)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        io_write(s, 0x08);

                    if (cloth->field_28_count)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        io_write(s, 0x08);

                    io_write(s, 0x10);
                }
            io_align_write(s, 0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x04);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    switch (block->constraint.type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        io_write(s, 0x0C);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        io_write(s, 0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0x18);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        io_write(s, 0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0x14);
                        io_write(s, 0x14);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        io_write(s, 0x20);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0x04);
                        io_write(s, 0x14);
                        io_write(s, 0x14);
                        break;
                    }
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x08);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    switch (block->constraint.type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        io_write(s, 0x0C);
                        io_write(s, 0x04);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        io_write(s, 0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0x18);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        io_write(s, 0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0x14);
                        io_write(s, 0x14);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        io_write(s, 0x20);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0x04);
                        io_write(s, 0x14);
                        io_write(s, 0x14);
                        io_write(s, 0x04);
                        break;
                    }
                }
            io_align_write(s, 0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write(s, 0x04);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
            else
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    bhs[i].block_offset = io_get_position(s);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write(s, 0x08);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
            io_align_write(s, 0x10);

            if (motion_count) {
                motion_block_node_mats = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    io_write(s, motion->nodes_count * sizeof(mat4));
                }
                io_align_write(s, 0x10);

                motion_block_node_name_offset = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = &block->motion;
                    io_write(s, motion->nodes_count * sizeof(uint32_t));
                }
                io_align_write(s, 0x10);
            }

            if (exh.cloth_count) {
                field_18 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    for (uint32_t j = 0; j < 32; j++)
                        if (cloth->field_10 & (1 << j))
                            io_write(s, sizeof(mat4));
                }
                io_align_write(s, 0x10);

                field_1C = io_get_position(s);
                if (!is_x)
                    for (uint32_t i = 0; i < ex->blocks_count; i++) {
                        obj_skin_block* block = &ex->blocks[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = &block->cloth;
                        io_write(s, (10 * sizeof(int32_t)
                            + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * cloth->count);
                    }
                else
                    for (uint32_t i = 0; i < ex->blocks_count; i++) {
                        obj_skin_block* block = &ex->blocks[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = &block->cloth;
                        io_write(s, (10 * sizeof(int32_t) + 4 * (sizeof(int64_t)
                            + 4 * sizeof(int32_t))) * cloth->count);
                    }
                io_align_write(s, 0x10);

                field_20 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, 10 * (11 * sizeof(int32_t)) * cloth->count);
                }
                io_align_write(s, 0x10);

                field_24 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, sizeof(uint16_t) + cloth->field_24_count * sizeof(uint16_t));
                }
                io_align_write(s, 0x10);

                field_28 = io_get_position(s);
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = &block->cloth;
                    io_write(s, sizeof(uint16_t) + cloth->field_28_count * sizeof(uint16_t));
                }
                io_align_write(s, 0x10);
            }
        }
    }

    if (sk->bones || sk->ex_data_init) {
        quicksort_string(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (std::string& i : strings) {
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i);
        }
    }
    io_align_write(s, 0x10);

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

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_cloth(&block->cloth, s, strings, string_offsets,
                    is_x, &field_18, &field_1C, &field_20, &field_24, &field_28);
                io_position_pop(s);
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

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_constraint(&block->constraint,
                    s, strings, string_offsets, bone_names_ptr, is_x, constraint_type_name_offsets);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_expression(&block->expression,
                    s, strings, string_offsets, bone_names_ptr, is_x);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_motion(&block->motion,
                    s, strings, string_offsets, is_x, bone_names_ptr,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                io_position_push(s, bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_osage(&block->osage,
                    s, strings, string_offsets, is_x);
                io_position_pop(s);
            }

            for (uint32_t i = 0; i < ex->blocks_count; i++) {
                obj_skin_block* block = &ex->blocks[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = &block->cloth;
                    obj_skin_strings_push_back_check(osage_names, string_data(&cloth->mesh_name));
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = &block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_names_ptr, osage->external_name_index);
                }
            }

            io_position_push(s, exh.blocks_offset, SEEK_SET);
            if (!is_x) {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    switch (block->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        io_write_offset_f2(s, cls_offset, 0x20);
                        io_write_offset_f2(s, bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        io_write_offset_f2(s, cns_offset, 0x20);
                        io_write_offset_f2(s, bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        io_write_offset_f2(s, exp_offset, 0x20);
                        io_write_offset_f2(s, bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        io_write_offset_f2(s, mot_offset, 0x20);
                        io_write_offset_f2(s, bhs[i].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        io_write_offset_f2(s, osg_offset, 0x20);
                        io_write_offset_f2(s, bhs[i].block_offset, 0x20);
                        break;
                    default:
                        io_write_offset_f2(s, 0, 0);
                        io_write_offset_f2(s, 0, 0);
                        break;
                    }
                }
                io_write_offset_f2(s, 0, 0);
                io_write_offset_f2(s, 0, 0);
            }
            else {
                for (uint32_t i = 0; i < ex->blocks_count; i++) {
                    obj_skin_block* block = &ex->blocks[i];
                    switch (block->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        io_write_offset_x(s, cls_offset);
                        io_write_offset_x(s, bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        io_write_offset_x(s, cns_offset);
                        io_write_offset_x(s, bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        io_write_offset_x(s, exp_offset);
                        io_write_offset_x(s, bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        io_write_offset_x(s, mot_offset);
                        io_write_offset_x(s, bhs[i].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        io_write_offset_x(s, osg_offset);
                        io_write_offset_x(s, bhs[i].block_offset);
                        break;
                    default:
                        io_write_offset_x(s, 0);
                        io_write_offset_x(s, 0);
                        break;
                    }
                }
                io_write_offset_x(s, 0);
                io_write_offset_x(s, 0);
            }
            io_position_pop(s);
            free(bhs);
        }
    }

    if (sk->bones) {
        io_position_push(s, sh.bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, string_data(&sk->bones[i].name));
                io_write_offset_f2(s, str_offset, 0x20);
            }
        else
            for (uint32_t i = 0; i < sk->bones_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, string_data(&sk->bones[i].name));
                io_write_offset_x(s, str_offset);
            }
        io_position_pop(s);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (ex->blocks_count > 0) {
            io_position_push(s, exh.bone_names_offset, SEEK_SET);
            if (!is_x)
                for (std::string& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    io_write_offset_f2(s, str_offset, 0x20);
                }
            else
                for (std::string& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    io_write_offset_x(s, str_offset);
                }
            io_position_pop(s);

            io_position_push(s, exh.osage_nodes_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_nodes_count; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_nodes[i];
                io_write_uint32_t(s, osage_node->name_index);
                io_write_float_t(s, osage_node->length);
                io_write_uint32_t(s, 0);
            }
            io_position_pop(s);

            io_position_push(s, exh.osage_sibling_infos_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->osage_sibling_infos_count; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_infos[i];
                io_write_uint32_t(s, osage_sibling_info->name_index);
                io_write_uint32_t(s, osage_sibling_info->sibling_name_index);
                io_write_float_t(s, osage_sibling_info->max_distance);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);

            exh.osage_count = (int32_t)osage_names.size();
            exh.osage_count -= exh.cloth_count;
            io_position_push(s, exh.osage_names_offset, SEEK_SET);
            if (!is_x)
                for (std::string& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    io_write_offset_f2(s, str_offset, 0x20);
                }
            else
                for (std::string& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    io_write_offset_x(s, str_offset);
                }
            io_position_pop(s);
        }

        io_position_push(s, sh.ex_data_offset, SEEK_SET);
        if (!is_x) {
            io_write_int32_t(s, exh.osage_count);
            io_write_int32_t(s, ex->osage_nodes_count);
            io_write(s, 0x04);
            io_write_offset_f2(s, exh.osage_nodes_offset, 0x20);
            io_write_offset_f2(s, exh.osage_names_offset, 0x20);
            io_write_offset_f2(s, exh.blocks_offset, 0x20);
            io_write_int32_t(s, exh.bone_names_count);
            io_write_offset_f2(s, exh.bone_names_offset, 0x20);
            io_write_offset_f2(s, exh.osage_sibling_infos_offset, 0x20);
            io_write_int32_t(s, exh.cloth_count);
            io_write_offset_f2(s, ex->reserved[0], 0x20);
            io_write_offset_f2(s, ex->reserved[1], 0x20);
            io_write_offset_f2(s, ex->reserved[2], 0x20);
            io_write_offset_f2(s, ex->reserved[3], 0x20);
            io_write_offset_f2(s, ex->reserved[4], 0x20);
            io_write_offset_f2(s, ex->reserved[5], 0x20);
            io_write_offset_f2(s, ex->reserved[6], 0x20);
        }
        else {
            io_write_int32_t(s, exh.osage_count);
            io_write_int32_t(s, ex->osage_nodes_count);
            io_write(s, 0x08);
            io_write_offset_x(s, exh.osage_nodes_offset);
            io_write_offset_x(s, exh.osage_names_offset);
            io_write_offset_x(s, exh.blocks_offset);
            io_write_int32_t(s, exh.bone_names_count);
            io_write_offset_x(s, exh.bone_names_offset);
            io_write_offset_x(s, exh.osage_sibling_infos_offset);
            io_write_int32_t(s, exh.cloth_count);
            io_write_offset_x(s, ex->reserved[0]);
            io_write_offset_x(s, ex->reserved[1]);
            io_write_offset_x(s, ex->reserved[2]);
            io_write_offset_x(s, ex->reserved[3]);
            io_write_offset_x(s, ex->reserved[4]);
            io_write_offset_x(s, ex->reserved[5]);
            io_write_offset_x(s, ex->reserved[6]);
        }
        io_position_pop(s);
    }

    io_position_push(s, base_offset, SEEK_SET);
    if (!is_x) {
        io_write_offset_f2(s, sh.bone_ids_offset, 0x20);
        io_write_offset_f2(s, sh.bone_matrices_offset, 0x20);
        io_write_offset_f2(s, sh.bone_names_offset, 0x20);
        io_write_offset_f2(s, sh.ex_data_offset, 0x20);
        io_write_int32_t(s, sk->bones_count);
        io_write_offset_f2(s, sh.bone_parent_ids_offset, 0x20);
        io_write(s, 0x0C);
    }
    else {
        io_write_offset_x(s, sh.bone_ids_offset);
        io_write_offset_x(s, sh.bone_matrices_offset);
        io_write_offset_x(s, sh.bone_names_offset);
        io_write_offset_x(s, sh.ex_data_offset);
        io_write_int32_t(s, sk->bones_count);
        io_write_offset_x(s, sh.bone_parent_ids_offset);
        io_write(s, 0x18);
    }
    io_position_pop(s);

    oskn->enrs = e;
    oskn->pof = pof;
}

static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    int64_t mesh_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, mesh_name_offset, &b->mesh_name);

    int64_t backface_mesh_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, backface_mesh_name_offset, &b->backface_mesh_name);

    b->field_8 = io_read_int32_t_stream_reverse_endianness(s);
    b->count = io_read_int32_t_stream_reverse_endianness(s);
    b->field_10 = io_read_int32_t_stream_reverse_endianness(s);
    b->field_14 = io_read_int32_t_stream_reverse_endianness(s);
    int64_t field_18_offset = io_read_offset(s, header_length, is_x);
    int64_t field_1C_offset = io_read_offset(s, header_length, is_x);

    b->field_1C = 0;
    b->field_20 = 0;
    b->field_24 = 0;
    b->field_28 = 0;
    int64_t field_20_offset = io_read_offset(s, header_length, is_x);
    int64_t field_24_offset = io_read_offset(s, header_length, is_x);
    int64_t field_28_offset = io_read_offset(s, header_length, is_x);
    b->field_2C = io_read_uint32_t_stream_reverse_endianness(s);
    b->field_30 = io_read_uint32_t_stream_reverse_endianness(s);

    if (field_18_offset) {
        io_position_push(s, field_18_offset, SEEK_SET);
        for (uint32_t i = 0; i < 32; i++) {
            if (~b->field_10 & (1 << i))
                continue;

            mat4u& mat = b->field_18[i];
            mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
        }
        io_position_pop(s);
    }

    if (field_1C_offset) {
        io_position_push(s, field_1C_offset, SEEK_SET);
        b->field_1C = force_malloc_s(obj_skin_block_cloth_field_1C, b->count);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_1C* f = &b->field_1C[i];
            f->field_0 = io_read_float_t_stream_reverse_endianness(s);
            f->field_4 = io_read_float_t_stream_reverse_endianness(s);
            f->field_8 = io_read_float_t_stream_reverse_endianness(s);
            f->field_C = io_read_float_t_stream_reverse_endianness(s);
            f->field_10 = io_read_float_t_stream_reverse_endianness(s);
            f->field_14 = io_read_float_t_stream_reverse_endianness(s);
            f->field_18 = io_read_float_t_stream_reverse_endianness(s);
            f->field_1C = io_read_uint32_t_stream_reverse_endianness(s);
            f->field_20 = io_read_uint32_t_stream_reverse_endianness(s);
            f->field_24 = io_read_uint32_t_stream_reverse_endianness(s);

            for (uint32_t j = 0; j < 4; j++)
                obj_modern_read_skin_block_cloth_field_1C_sub(&f->sub_data[j],
                    s, header_length, str, is_x);
        }
        io_position_pop(s);
    }

    if (field_20_offset) {
        io_position_push(s, field_20_offset, SEEK_SET);
        b->field_20 = force_malloc_s(obj_skin_block_cloth_field_20, b->count);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_20* f = &b->field_20[i];
            for (uint32_t j = 0; j < 10; j++)
                obj_modern_read_skin_block_cloth_field_20_sub(&f->sub_data[j],
                    s, header_length, str, is_x);
        }
        io_position_pop(s);
    }

    if (field_24_offset) {
        io_position_push(s, field_24_offset, SEEK_SET);
        b->field_24_count = io_read_uint16_t_stream_reverse_endianness(s);
        b->field_24 = force_malloc_s(uint16_t, b->field_24_count);
        io_read(s, b->field_24, b->field_24_count * sizeof(uint16_t));
        if (s->is_big_endian) {
            uint16_t* field_24 = b->field_24;
            for (int32_t i = 0; i < b->field_24_count; i++)
                field_24[i] = reverse_endianness_uint16_t(field_24[i]);
        }
        io_position_pop(s);
    }

    if (field_28_offset) {
        io_position_push(s, field_28_offset, SEEK_SET);
        b->field_28_count = io_read_uint16_t_stream_reverse_endianness(s);
        b->field_28 = force_malloc_s(uint16_t, b->field_28_count);
        io_read(s, b->field_28, b->field_28_count * sizeof(uint16_t));
        if (s->is_big_endian) {
            uint16_t* field_28 = b->field_28;
            for (int32_t i = 0; i < b->field_28_count; i++)
                field_28[i] = reverse_endianness_uint16_t(field_28[i]);
        }
        io_position_pop(s);
    }

}

static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* field_18_offset, int64_t* field_1C_offset, int64_t* field_20_offset,
    int64_t* field_24_offset, int64_t* field_28_offset) {
    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->mesh_name));
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->backface_mesh_name));

    if (!is_x) {
        io_write_offset_f2(s, mesh_name_offset, 0x20);
        io_write_offset_f2(s, backface_mesh_name_offset, 0x20);
        io_write_int32_t(s, b->field_8);
        io_write_int32_t(s, b->count);
        io_write_int32_t(s, b->field_10);
        io_write_int32_t(s, b->field_14);
        io_write_offset_f2(s, b->field_10 ? *field_18_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_1C ? *field_1C_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_20 ? *field_20_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_24 ? *field_24_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_28 ? *field_28_offset : 0, 0x20);
        io_write_uint32_t(s, b->field_2C);
        io_write_uint32_t(s, b->field_30);
    }
    else {
        io_write_offset_x(s, mesh_name_offset);
        io_write_offset_x(s, backface_mesh_name_offset);
        io_write_int32_t(s, b->field_8);
        io_write_int32_t(s, b->count);
        io_write_int32_t(s, b->field_10);
        io_write_int32_t(s, b->field_14);
        io_write_offset_x(s, b->field_10 ? *field_18_offset : 0);
        io_write_offset_x(s, b->field_1C ? *field_1C_offset : 0);
        io_write_offset_x(s, b->field_20 ? *field_20_offset : 0);
        io_write_offset_x(s, b->field_24 ? *field_24_offset : 0);
        io_write_offset_x(s, b->field_28 ? *field_28_offset : 0);
        io_write_uint32_t(s, b->field_2C);
        io_write_uint32_t(s, b->field_30);
    }

    if (b->field_10) {
        io_position_push(s, *field_18_offset, SEEK_SET);
        for (uint32_t i = 0; i < 32; i++) {
            if (~b->field_10 & (1 << i))
                continue;

            mat4u& mat = b->field_18[i];
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
            *field_18_offset += sizeof(mat4);
        }
        io_position_pop(s);
    }

    if (b->field_1C) {
        io_position_push(s, *field_1C_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_1C* f = &b->field_1C[i];
            io_write_float_t(s, f->field_0);
            io_write_float_t(s, f->field_4);
            io_write_float_t(s, f->field_8);
            io_write_float_t(s, f->field_C);
            io_write_float_t(s, f->field_10);
            io_write_float_t(s, f->field_14);
            io_write_float_t(s, f->field_18);
            io_write_uint32_t(s, f->field_1C);
            io_write_uint32_t(s, f->field_20);
            io_write_uint32_t(s, f->field_24);

            for (uint32_t j = 0; j < 4; j++)
                obj_modern_write_skin_block_cloth_field_1C_sub(&f->sub_data[j],
                    s, strings, string_offsets, is_x);
        }
        io_position_pop(s);
        if (!is_x)
            *field_1C_offset += (10 * sizeof(int32_t)
                + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t))) * b->count;
        else
            *field_1C_offset += (10 * sizeof(int32_t)
                + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * b->count;
    }

    if (b->field_20) {
        io_position_push(s, *field_20_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->count; i++) {
            obj_skin_block_cloth_field_20* f = &b->field_20[i];
            for (uint32_t j = 0; j < 10; j++)
                obj_modern_write_skin_block_cloth_field_20_sub(&f->sub_data[j],
                    s, strings, string_offsets, is_x);
        }
        io_position_pop(s);
        *field_20_offset += 10 * (11 * sizeof(int32_t)) * b->count;
    }

    if (b->field_24) {
        io_position_push(s, *field_24_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)b->field_24_count);
        io_write(s, b->field_24, b->field_24_count * sizeof(uint16_t));
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + b->field_24_count * sizeof(uint16_t);
    }

    if (b->field_28) {
        io_position_push(s, *field_28_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)b->field_28_count);
        io_write(s, b->field_28, b->field_28_count * sizeof(uint16_t));
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + b->field_28_count * sizeof(uint16_t);
    }
}

static void obj_modern_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    int64_t bone_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, bone_name_offset, &sub->bone_name);
    sub->weight = io_read_float_t_stream_reverse_endianness(s);
    sub->matrix_index = io_read_uint32_t_stream_reverse_endianness(s);
    sub->field_C = io_read_uint32_t_stream_reverse_endianness(s);
    if (is_x)
        io_read(s, 0, 0x04);
}

static void obj_modern_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&sub->bone_name));
    io_write_offset(s, bone_name_offset, 0x20, is_x);
    io_write_float_t(s, sub->weight);
    io_write_uint32_t(s, sub->matrix_index);
    io_write_uint32_t(s, sub->field_C);
    if (is_x)
        io_write(s, 0x04);
}

static void obj_modern_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    sub->field_0 = io_read_uint32_t_stream_reverse_endianness(s);
    sub->field_4 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_8 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_C = io_read_float_t_stream_reverse_endianness(s);
    sub->field_10 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_14 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_18 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_1C = io_read_float_t_stream_reverse_endianness(s);
    sub->field_20 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_24 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_28 = io_read_float_t_stream_reverse_endianness(s);
}

static void obj_modern_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    io_write_uint32_t(s, sub->field_0);
    io_write_float_t(s, sub->field_4);
    io_write_float_t(s, sub->field_8);
    io_write_float_t(s, sub->field_C);
    io_write_float_t(s, sub->field_10);
    io_write_float_t(s, sub->field_14);
    io_write_float_t(s, sub->field_18);
    io_write_float_t(s, sub->field_1C);
    io_write_float_t(s, sub->field_20);
    io_write_float_t(s, sub->field_24);
    io_write_float_t(s, sub->field_28);
}

static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t type_offset = io_read_offset(s, header_length, is_x);
    char* type = io_read_utf8_string_null_terminated_offset(s, type_offset);

    int64_t name_offset = io_read_offset(s, header_length, is_x);
    char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->coupling = (obj_skin_block_constraint_coupling)io_read_int32_t_stream_reverse_endianness(s);

    int64_t source_node_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, source_node_name_offset, &b->source_node_name);

    if (!str_utils_compare(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = io_read_float_t_stream_reverse_endianness(s);
        b->orientation.offset.y = io_read_float_t_stream_reverse_endianness(s);
        b->orientation.offset.z = io_read_float_t_stream_reverse_endianness(s);
    }
    else if (!str_utils_compare(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_modern_read_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, header_length, str, is_x);
        b->direction.align_axis.x = io_read_float_t_stream_reverse_endianness(s);
        b->direction.align_axis.y = io_read_float_t_stream_reverse_endianness(s);
        b->direction.align_axis.z = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.x = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.y = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.z = io_read_float_t_stream_reverse_endianness(s);
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
        b->distance.distance = io_read_float_t_stream_reverse_endianness(s);
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
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names,
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
    io_write_offset(s, type_offset, 0x20, is_x);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    io_write_offset(s, name_offset, 0x20, is_x);

    io_write_int32_t(s, b->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->source_node_name));
    io_write_offset(s, source_node_name_offset, 0x20, is_x);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        io_write_float_t(s, b->orientation.offset.x);
        io_write_float_t(s, b->orientation.offset.y);
        io_write_float_t(s, b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_modern_write_skin_block_constraint_up_vector_old(&b->direction.up_vector,
            s, strings, string_offsets, is_x);
        io_write_float_t(s, b->direction.align_axis.x);
        io_write_float_t(s, b->direction.align_axis.y);
        io_write_float_t(s, b->direction.align_axis.z);
        io_write_float_t(s, b->direction.target_offset.x);
        io_write_float_t(s, b->direction.target_offset.y);
        io_write_float_t(s, b->direction.target_offset.z);
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
        io_write_float_t(s, b->distance.distance);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets, is_x);
        break;
    }
}

static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    ap->affected_by_orientation = io_read_int32_t_stream_reverse_endianness(s) != 0;
    ap->affected_by_scaling = io_read_int32_t_stream_reverse_endianness(s) != 0;
    ap->offset.x = io_read_float_t_stream_reverse_endianness(s);
    ap->offset.y = io_read_float_t_stream_reverse_endianness(s);
    ap->offset.z = io_read_float_t_stream_reverse_endianness(s);
}

static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    io_write_int32_t(s, ap->affected_by_orientation ? 1 : 0);
    io_write_int32_t(s, ap->affected_by_scaling ? 1 : 0);
    io_write_float_t(s, ap->offset.x);
    io_write_float_t(s, ap->offset.y);
    io_write_float_t(s, ap->offset.z);
}

static void obj_modern_read_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    up->active = io_read_int32_t_stream_reverse_endianness(s) != 0;
    up->roll = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.x = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.y = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.z = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.x = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.y = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.z = io_read_float_t_stream_reverse_endianness(s);

    int64_t name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, name_offset, &up->name);
}

static void obj_modern_write_skin_block_constraint_up_vector_old(obj_skin_block_constraint_up_vector* up,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    io_write_int32_t(s, up->active ? 1 : 0);
    io_write_float_t(s, up->roll);
    io_write_float_t(s, up->affected_axis.x);
    io_write_float_t(s, up->affected_axis.y);
    io_write_float_t(s, up->affected_axis.z);
    io_write_float_t(s, up->point_at.x);
    io_write_float_t(s, up->point_at.y);
    io_write_float_t(s, up->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&up->name));
    io_write_offset(s, name_offset, 0x20, is_x);
}

static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t name_offset = io_read_offset(s, header_length, is_x);
    char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);

    b->name_index = 0;
    for (char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            b->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free(name);

    b->expressions_count = io_read_int32_t_stream_reverse_endianness(s);
    b->expressions_count = min(b->expressions_count, 9);
    if (!is_x)
        for (uint32_t i = 0; i < b->expressions_count; i++) {
            int64_t expression_offset = io_read_offset_f2(s, header_length);
            if (expression_offset)
                io_read_string_null_terminated_offset(s, expression_offset, &b->expressions[i]);
        }
    else
        for (uint32_t i = 0; i < b->expressions_count; i++) {
            int64_t expression_offset = io_read_offset_x(s);
            if (expression_offset)
                io_read_string_null_terminated_offset(s, expression_offset, &b->expressions[i]);
        }

    for (uint32_t i = b->expressions_count; i < 9; i++)
        b->expressions[i] = string_empty;
}

static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, char** bone_names, bool is_x) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_names, b->name_index);
    io_write_offset(s, name_offset, 0x20, is_x);

    io_write_int32_t(s, min(b->expressions_count, 9));
    if (!is_x) {
        for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, string_data(&b->expressions[i]));
            io_write_offset_f2(s, (int32_t)expression_offset, 0x20);
        }

        for (uint32_t i = b->expressions_count; i < 9; i++)
            io_write_offset_f2(s, 0, 0x20);
    }
    else {
        for (uint32_t i = 0; i < b->expressions_count && i < 9; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, string_data(&b->expressions[i]));
            io_write_offset_x(s, expression_offset);
        }

        for (uint32_t i = b->expressions_count; i < 9; i++)
            io_write_offset_x(s, 0);
    }
}

static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    int64_t name_offset = io_read_offset(s, header_length, is_x);
    b->nodes_count = io_read_int32_t_stream_reverse_endianness(s);
    int64_t bone_names_offset = io_read_offset(s, header_length, is_x);
    int64_t bone_matrices_offset = io_read_offset(s, header_length, is_x);

    if (!is_x) {
        b->name_index = 0;
        char* name = io_read_utf8_string_null_terminated_offset(s, name_offset);
        for (char** i = str; *i; i++)
            if (!str_utils_compare(name, *i)) {
                b->name_index = 0x8000 | (int32_t)(i - str);
                break;
            }
        free(name);
    }
    else
        io_read_string_null_terminated_offset(s, name_offset, &b->name);

    b->nodes = 0;

    if (!b->nodes_count)
        return;

    b->nodes = force_malloc_s(obj_skin_motion_node, b->nodes_count);

    if (bone_names_offset) {
        io_position_push(s, bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            b->nodes[i].name_index = io_read_uint32_t_stream_reverse_endianness(s);
        io_position_pop(s);
    }

    if (bone_matrices_offset) {
        io_position_push(s, bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4u& mat = b->nodes[i].transformation;
            mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
        }
        io_position_pop(s);
    }
}

static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    char** bone_names, int64_t* bone_names_offset, int64_t* bone_matrices_offset) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    int64_t name_offset;
    if (!is_x)
        name_offset = obj_skin_strings_get_string_offset_by_index(strings,
            string_offsets, bone_names, b->name_index);
    else
        name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, string_data(&b->name));
    io_write_offset(s, name_offset, 0x20, is_x);
    io_write_int32_t(s, b->nodes_count);
    io_write_offset(s, b->nodes ? *bone_names_offset : 0, 0x20, is_x);
    io_write_offset(s, b->nodes ? *bone_matrices_offset : 0, 0x20, is_x);

    if (b->nodes) {
        io_position_push(s, *bone_names_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++)
            io_write_uint32_t(s, b->nodes[i].name_index);
        io_position_pop(s);
        *bone_names_offset += b->nodes_count * sizeof(uint32_t);

        io_position_push(s, *bone_matrices_offset, SEEK_SET);
        for (uint32_t i = 0; i < b->nodes_count; i++) {
            mat4u& mat = b->nodes[i].transformation;
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.w);
        }
        io_position_pop(s);
        *bone_matrices_offset += b->nodes_count * sizeof(mat4);
    }
}

static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    int64_t parent_name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, parent_name, &b->parent_name);

    b->position.x = io_read_float_t_stream_reverse_endianness(s);
    b->position.y = io_read_float_t_stream_reverse_endianness(s);
    b->position.z = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.x = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.y = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.z = io_read_float_t_stream_reverse_endianness(s);
    b->scale.x = io_read_float_t_stream_reverse_endianness(s);
    b->scale.y = io_read_float_t_stream_reverse_endianness(s);
    b->scale.z = io_read_float_t_stream_reverse_endianness(s);
    if (is_x)
        io_read(s, 0, 0x04);
}

static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->parent_name));
    io_write_offset(s, parent_name_offset, 0x20, is_x);

    io_write_float_t(s, b->position.x);
    io_write_float_t(s, b->position.y);
    io_write_float_t(s, b->position.z);
    io_write_float_t(s, b->rotation.x);
    io_write_float_t(s, b->rotation.y);
    io_write_float_t(s, b->rotation.z);
    io_write_float_t(s, b->scale.x);
    io_write_float_t(s, b->scale.y);
    io_write_float_t(s, b->scale.z);
    if (is_x)
        io_write(s, 0x04);
}

static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, uint32_t header_length, char** str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    b->start_index = io_read_int32_t_stream_reverse_endianness(s);
    b->count = io_read_int32_t_stream_reverse_endianness(s);
    b->external_name_index = io_read_uint32_t_stream_reverse_endianness(s);
    b->name_index = io_read_uint32_t_stream_reverse_endianness(s);

    if (!is_x)
        io_read(s, 0, 0x14);
    else {
        io_read(s, 0, 0x08);
        int64_t motion_node_name_offset = io_read_offset_x(s);
        io_read_string_null_terminated_offset(s, motion_node_name_offset, &b->motion_node_name);
        io_read(s, 0, 0x18);
    }

    b->nodes = 0;
    b->nodes_count = 0;
}

static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, std::vector<std::string>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    io_write_int32_t(s, b->start_index);
    io_write_int32_t(s, b->count);
    io_write_uint32_t(s, b->external_name_index);
    io_write_uint32_t(s, b->name_index);

    if (!is_x)
        io_write(s, 0x14);
    else {
        io_write(s, 0x08);
        int64_t motion_node_name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, string_data(&b->motion_node_name));
        io_write_offset_x(s, motion_node_name_offset);
        io_write(s, 0x18);
    }
}

static void obj_modern_read_vertex(obj* obj, stream* s, int64_t* vertex, obj_mesh* mesh,
    const uint32_t vertex_flags, uint32_t num_vertex, uint32_t size_vertex) {
    obj_vertex_format vertex_format = (obj_vertex_format)(OBJ_VERTEX_FORMAT_POSITION
        | OBJ_VERTEX_FORMAT_NORMAL
        | OBJ_VERTEX_FORMAT_TANGENT
        | OBJ_VERTEX_FORMAT_TEXCOORD0
        | OBJ_VERTEX_FORMAT_TEXCOORD1
        | OBJ_VERTEX_FORMAT_COLOR0);

    if (vertex_flags == 0x06)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD2);
    else if (vertex_flags == 0x0A)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_TEXCOORD2 | OBJ_VERTEX_FORMAT_TEXCOORD3);
    else if (vertex_flags == 0x04)
        enum_or(vertex_format, OBJ_VERTEX_FORMAT_BONE_DATA);

    int64_t vtx_offset = vertex[13];

    bool has_tangents = false;
    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, num_vertex);
    for (uint32_t i = 0; i < num_vertex; i++) {
        io_set_position(s, vtx_offset + (int64_t)size_vertex * i, SEEK_SET);

        vec3 position;
        position.x = io_read_float_t_stream_reverse_endianness(s);
        position.y = io_read_float_t_stream_reverse_endianness(s);
        position.z = io_read_float_t_stream_reverse_endianness(s);
        vtx[i].position = position;

        vec3 normal;
        normal.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        normal.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        normal.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        io_read(s, 0, 0x02);
        vec3_div_min_max_scalar(normal, -32768.0f, 32767.0f, vtx[i].normal);

        vec4 tangent;
        tangent.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.w = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        vec4_div_min_max_scalar(tangent, -32768.0f, 32767.0f, tangent);
        vtx[i].tangent = tangent;

        vec2h texcoord0;
        texcoord0.x = io_read_half_t_stream_reverse_endianness(s);
        texcoord0.y = io_read_half_t_stream_reverse_endianness(s);
        vec2h_to_vec2(texcoord0, vtx[i].texcoord0);

        vec2h texcoord1;
        texcoord1.x = io_read_half_t_stream_reverse_endianness(s);
        texcoord1.y = io_read_half_t_stream_reverse_endianness(s);
        vec2h_to_vec2(texcoord1, vtx[i].texcoord1);

        if (vertex_flags == 0x06) {
            vec2h texcoord2;
            texcoord2.x = io_read_half_t_stream_reverse_endianness(s);
            texcoord2.y = io_read_half_t_stream_reverse_endianness(s);
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);
        }
        else if (vertex_flags == 0x0A) {
            vec2h texcoord2;
            texcoord2.x = io_read_half_t_stream_reverse_endianness(s);
            texcoord2.y = io_read_half_t_stream_reverse_endianness(s);
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);

            vec2h texcoord3;
            texcoord3.x = io_read_half_t_stream_reverse_endianness(s);
            texcoord3.y = io_read_half_t_stream_reverse_endianness(s);
            vec2h_to_vec2(texcoord3, vtx[i].texcoord3);
        }

        vec4h color;
        color.x = io_read_half_t_stream_reverse_endianness(s);
        color.y = io_read_half_t_stream_reverse_endianness(s);
        color.z = io_read_half_t_stream_reverse_endianness(s);
        color.w = io_read_half_t_stream_reverse_endianness(s);
        vec4h_to_vec4(color, vtx[i].color0);

        if (vertex_flags == 0x04) {
            vec4 bone_weight;
            bone_weight.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.w = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            vec4_div_min_max_scalar(bone_weight, -32768.0f, 32767.0f, bone_weight);
            vtx[i].bone_weight = bone_weight;

            vec4i bone_index;
            bone_index.x = (int32_t)io_read_uint8_t(s);
            bone_index.y = (int32_t)io_read_uint8_t(s);
            bone_index.z = (int32_t)io_read_uint8_t(s);
            bone_index.w = (int32_t)io_read_uint8_t(s);
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
        enum_and(vertex_format, ~OBJ_VERTEX_FORMAT_TANGENT);

    mesh->vertex = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
}

static void obj_modern_write_vertex(obj* obj, stream* s, bool is_x,
    int64_t* vertex, obj_mesh* mesh, uint32_t* vertex_flags,
    uint32_t* num_vertex, uint32_t* size_vertex, f2_struct* ovtx) {

    obj_vertex_data* vtx = mesh->vertex;
    uint32_t _num_vertex = mesh->num_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    uint32_t _vertex_flags = 0x02;
    uint32_t _size_vertex = 0x2C;
    uint32_t enrs_se3_rc = 12;
    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        _vertex_flags = 0x04;
        _size_vertex += 0x0C;
        enrs_se3_rc += 4;
    }
    else if ((vertex_format & (OBJ_VERTEX_FORMAT_TEXCOORD2 & OBJ_VERTEX_FORMAT_TEXCOORD3))
        == (OBJ_VERTEX_FORMAT_TEXCOORD2 | OBJ_VERTEX_FORMAT_TEXCOORD3)) {
        _vertex_flags = 0x0A;
        _size_vertex += 0x08;
        enrs_se3_rc += 4;
    }
    else if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2) {
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
    vertex[13] = io_get_position(s);
    for (uint32_t i = 0; i < _num_vertex; i++) {
        vec3 position = vtx[i].position;
        io_write_float_t(s, position.x);
        io_write_float_t(s, position.y);
        io_write_float_t(s, position.z);

        vec3 normal;
        vec3_mult_min_max_scalar(vtx[i].normal, -32768.0f, 32767.0f, normal);
        io_write_int16_t(s, (int16_t)roundf(normal.x));
        io_write_int16_t(s, (int16_t)roundf(normal.y));
        io_write_int16_t(s, (int16_t)roundf(normal.z));
        io_write(s, 0x02);

        vec4 tangent = vtx[i].tangent;
        vec4_mult_min_max_scalar(tangent, -32768.0f, 32767.0f, tangent);
        io_write_int16_t(s, (int16_t)roundf(tangent.x));
        io_write_int16_t(s, (int16_t)roundf(tangent.y));
        io_write_int16_t(s, (int16_t)roundf(tangent.z));
        io_write_int16_t(s, (int16_t)roundf(tangent.w));

        vec2h texcoord0;
        vec2_to_vec2h(vtx[i].texcoord0, texcoord0);
        io_write_half_t(s, texcoord0.x);
        io_write_half_t(s, texcoord0.y);

        vec2h texcoord1;
        vec2_to_vec2h(vtx[i].texcoord1, texcoord1);
        io_write_half_t(s, texcoord1.x);
        io_write_half_t(s, texcoord1.y);

        if (_vertex_flags == 0x06) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            io_write_half_t(s, texcoord2.x);
            io_write_half_t(s, texcoord2.y);
        }
        else if (_vertex_flags == 0x0A) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            io_write_half_t(s, texcoord2.x);
            io_write_half_t(s, texcoord2.y);

            vec2h texcoord3;
            vec2_to_vec2h(vtx[i].texcoord3, texcoord3);
            io_write_half_t(s, texcoord3.x);
            io_write_half_t(s, texcoord3.y);
        }

        vec4h color;
        vec4_to_vec4h(vtx[i].color0, color);
        io_write_half_t(s, color.x);
        io_write_half_t(s, color.y);
        io_write_half_t(s, color.z);
        io_write_half_t(s, color.w);

        if (_vertex_flags == 0x04) {
            vec4 bone_weight = vtx[i].bone_weight;
            vec4_mult_min_max_scalar(bone_weight, -32768.0f, 32767.0f, bone_weight);
            io_write_int16_t(s, (int16_t)bone_weight.x);
            io_write_int16_t(s, (int16_t)bone_weight.y);
            io_write_int16_t(s, (int16_t)bone_weight.z);
            io_write_int16_t(s, (int16_t)bone_weight.w);

            vec4u8 bone_index;
            vec4_to_vec4u8(vtx[i].bone_index, bone_index)
            io_write_uint8_t(s, bone_index.x);
            io_write_uint8_t(s, bone_index.y);
            io_write_uint8_t(s, bone_index.z);
            io_write_uint8_t(s, bone_index.w);
        }
    }

    *vertex_flags = _vertex_flags;
    *num_vertex = _num_vertex;
    *size_vertex = _size_vertex;
}

inline static void obj_skin_block_node_free(obj_skin_block_node* b) {
    string_free(&b->parent_name);
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
