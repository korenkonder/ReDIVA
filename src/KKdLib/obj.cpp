/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "obj.hpp"
#include <vector>
#include "io/memory_stream.hpp"
#include "f2/struct.hpp"
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
    uint32_t num_osage;
    int64_t osage_node_array_offset;
    int64_t osage_name_array_offset;
    int64_t block_array_offset;
    uint32_t num_bone_name;
    int64_t bone_name_array_offset;
    int64_t osage_sibling_info_array_offset;
    uint32_t cloth_count;
};

struct obj_skin_header {
    int64_t bone_id_array_offset;
    int64_t bone_matrix_array_offset;
    int64_t bone_name_array_offset;
    int64_t ex_data_offset;
    int64_t bone_parent_id_array_offset;
};

struct obj_sub_mesh_header {
    int64_t bone_index_array_offset;
    int64_t index_array_offset;
};

struct obj_mesh_header {
    int64_t submesh_array;
    obj_vertex_format_file format;
    uint32_t size_vertex;
    uint32_t num_vertex;
    int64_t vertex[20];
    uint32_t vertex_format_index;
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

const uint8_t obj_material_texture_enrs_table_bin[] =
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

static void obj_vertex_add_bone_weight(vec4& bone_weight, vec4i16& bone_index, int16_t index, float_t weight);
static void obj_vertex_generate_tangents(obj_mesh* mesh);
static void obj_vertex_validate_bone_data(vec4& bone_weight, vec4i16& bone_index);

static void obj_move_data(obj* obj_dst, const obj* obj_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_mesh(obj_mesh* mesh_dst, const obj_mesh* mesh_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_sub_mesh(obj_sub_mesh* sub_mesh_dst, const obj_sub_mesh* sub_mesh_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin* obj_move_data_skin(const obj_skin* sk_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_skin_bone(obj_skin_bone* bone_dst, const obj_skin_bone* bone_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_ex_data* obj_move_data_skin_ex_data(const obj_skin_ex_data* ex_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_block_cloth* obj_move_data_skin_block_cloth(const obj_skin_block_cloth* cls_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root_dst,
    const obj_skin_block_cloth_root* cloth_root_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight_dst,
    const obj_skin_block_cloth_root_bone_weight* bone_weight_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_block_constraint* obj_move_data_skin_block_constraint(const obj_skin_block_constraint* cns_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector_dst,
    const obj_skin_block_constraint_up_vector* up_vector_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_block_expression* obj_move_data_skin_block_expression(const obj_skin_block_expression* exp_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_block_motion* obj_move_data_skin_block_motion(const obj_skin_block_motion* mot_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void obj_move_data_skin_block_node(obj_skin_block_node* node_dst, const obj_skin_block_node* node_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_block_osage* obj_move_data_skin_block_osage(const obj_skin_block_osage* osg_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static obj_skin_skin_param* obj_move_data_skin_param(const obj_skin_skin_param* skp_src,
    prj::shared_ptr<prj::stack_allocator> alloc);

static void obj_set_classic_read_inner(obj_set* set, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void obj_set_classic_write_inner(obj_set* set, stream& s);
static void obj_classic_read_index(obj_sub_mesh* sub_mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static void obj_classic_write_index(obj_sub_mesh* sub_mesh, stream& s);
static void obj_classic_read_model(obj* obj, prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset);
static void obj_classic_write_model(obj* obj, stream& s, int64_t base_offset);
static void obj_classic_read_model_mesh(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset);
static void obj_classic_read_model_sub_mesh(obj_sub_mesh* sub_mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset);
static obj_skin* obj_classic_read_skin(prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset);
static void obj_classic_write_skin(obj_skin* sk, stream& s, int64_t base_offset);
static obj_skin_ex_data* obj_classic_read_skin_ex_data(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset);
static obj_skin_block_cloth* obj_classic_read_skin_block_cloth(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* cls,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    int64_t* mat_array_offset, int64_t* root_array_offset, int64_t* node_array_offset,
    int64_t* mesh_index_array_offset, int64_t* backface_mesh_index_array_offset);
static void obj_classic_read_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static obj_skin_block_constraint* obj_classic_read_skin_block_constraint(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* cns,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, int64_t* offsets);
static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, const char** str);
static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static obj_skin_block_expression* obj_classic_read_skin_block_expression(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_expression(obj_skin_block_expression* exp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array);
static obj_skin_block_motion* obj_classic_read_skin_block_motion(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_motion(obj_skin_block_motion* mot,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, int64_t* bone_name_array_offset, int64_t* bone_matrix_array_offset);
static void obj_classic_read_skin_block_node(obj_skin_block_node* node,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_node(obj_skin_block_node* node,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static obj_skin_block_osage* obj_classic_read_skin_block_osage(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_block_osage(obj_skin_block_osage* osg,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, int64_t* node_array_offset);
static obj_skin_skin_param* obj_classic_read_skin_param(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str);
static void obj_classic_write_skin_param(obj_skin_skin_param* skp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets);
static void obj_classic_read_vertex(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t* vertex, int64_t base_offset, uint32_t num_vertex,
    obj_vertex_format_file vertex_format_file);
static void obj_classic_write_vertex(obj_mesh* mesh,
    stream& s, int64_t* vertex, int64_t base_offset, uint32_t* num_vertex,
    obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex);

static void obj_set_modern_read_inner(obj_set* set, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void obj_set_modern_write_inner(obj_set* set, stream& s);
static void obj_modern_read_index(obj_sub_mesh* sub_mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static void obj_modern_write_index(obj_sub_mesh* sub_mesh, stream& s, bool is_x, f2_struct* ovtx);
static void obj_modern_read_model(obj* obj, prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx);
static void obj_modern_write_model(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* omdl);
static void obj_modern_read_model_mesh(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx);
static void obj_modern_read_model_sub_mesh(obj_sub_mesh* sub_mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx);
static obj_skin* obj_modern_read_skin(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset, uint32_t header_length, bool is_x);
static void obj_modern_write_skin(obj_skin* sk, stream& s,
    int64_t base_offset, bool is_x, f2_struct* oskn);
static obj_skin_ex_data* obj_modern_read_skin_ex_data(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset, uint32_t header_length, bool is_x);
static obj_skin_block_cloth* obj_modern_read_skin_block_cloth(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* cls,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* mat_array_offset, int64_t* root_array_offset, int64_t* node_array_offset,
    int64_t* mesh_index_array_offset, int64_t* backface_mesh_index_array_offset);
static void obj_modern_read_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static obj_skin_block_constraint* obj_modern_read_skin_block_constraint(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* cns,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, const char** bone_name_array,
    bool is_x, int64_t* offsets);
static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static obj_skin_block_expression* obj_modern_read_skin_block_expression(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_expression(obj_skin_block_expression* exp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, bool is_x);
static obj_skin_block_motion* obj_modern_read_skin_block_motion(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_motion(obj_skin_block_motion* mot,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    const char** bone_name_array, int64_t* bone_name_array_offset, int64_t* bone_matrix_array_offset);
static void obj_modern_read_skin_block_node(obj_skin_block_node* node,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_node(obj_skin_block_node* node,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static obj_skin_block_osage* obj_modern_read_skin_block_osage(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x);
static void obj_modern_write_skin_block_osage(obj_skin_block_osage* osg,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x);
static void obj_modern_read_vertex(obj_mesh* mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    int64_t* vertex, const uint32_t vertex_format_index, uint32_t num_vertex, uint32_t size_vertex);
static void obj_modern_write_vertex(obj_mesh* mesh, stream& s, int64_t* vertex,
    uint32_t* vertex_format_index, uint32_t* num_vertex, uint32_t* size_vertex, f2_struct* ovtx);

static const char* obj_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc);

static const char* obj_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

static uint32_t obj_skin_strings_get_string_index(std::vector<string_hash>& vec, const char* str);
static int64_t obj_skin_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, const char* str);
static int64_t obj_skin_strings_get_string_offset_by_index(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, const char** strings, uint32_t index);
static void obj_skin_strings_push_back_check(std::vector<string_hash>& vec, const char* str);
static void obj_skin_strings_push_back_check_by_index(std::vector<string_hash>& vec,
    const char** strings, uint32_t index);

obj_axis_aligned_bounding_box::obj_axis_aligned_bounding_box() {

}

obj_bounding_box::obj_bounding_box() {

}

obj_bounding_sphere::obj_bounding_sphere() : radius() {

}

obj_bounding_sphere::obj_bounding_sphere(vec3 center, float_t radius) {
    this->center = center;
    this->radius = radius;
}

obj_material_shader_lighting_type obj_material_shader_attrib::get_lighting_type() const {
    if (!m.is_lgt_diffuse && !m.is_lgt_specular)
        return OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT;
    else if (!m.is_lgt_specular)
        return OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT;
    else
        return OBJ_MATERIAL_SHADER_LIGHTING_PHONG;
}

int32_t obj_texture_attrib::get_blend() const {
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

obj_material_texture_data::obj_material_texture_data() : attrib(),
tex_index(), shader_info(), ex_shader(), weight(), reserved() {

}

obj_material_color::obj_material_color() : shininess(), intensity() {

}

obj_material::obj_material() : shader_compo(), shader(), shader_info(),
attrib(), center(), radius(), name(), bump_depth(), reserved() {
}

obj_material_data::obj_material_data() : num_of_textures() {

}

obj_sub_mesh::obj_sub_mesh() : flags(), bounding_sphere(), material_index(), uv_index(), bone_index_array(),
num_bone_index(), bones_per_vertex(), primitive_type(), index_format(), index_array(), num_index(),
attrib(), axis_aligned_bounding_box(), first_index(), last_index(), index_offset() {

}

obj_vertex_data::obj_vertex_data() {

}

obj_mesh::obj_mesh() : flags(), num_submesh(), submesh_array(), vertex_format(),
size_vertex(), num_vertex(), vertex_array(), attrib(), reserved(), name() {

}

obj_skin_block_node::obj_skin_block_node() : parent_name() {

}

obj_skin_skin_param_coli::obj_skin_skin_param_coli() : type(), bone0_index(), bone1_index(), radius() {

}

obj_skin_skin_param::obj_skin_skin_param() : unk0(), force(), force_gain(), air_res(), rot_y(), rot_z(),
hinge_y(), hinge_z(), name(), coli(), coli_count(), coli_r(), friction(), wind_afc(), unk44() {

}

obj_skin_block_cloth_root_bone_weight::obj_skin_block_cloth_root_bone_weight() :
bone_name(), weight(), matrix_index(), reserved() {

}

obj_skin_block_cloth_root::obj_skin_block_cloth_root() : field_18(), field_1C(), field_20(), field_24() {

}

obj_skin_block_cloth_node::obj_skin_block_cloth_node() : flags(),
dist_top(), dist_bottom(), dist_right(), dist_left() {

}

obj_skin_block_cloth::obj_skin_block_cloth() : mesh_name(), backface_mesh_name(), field_8(),
num_root(), num_node(), field_14(), mat_array(), num_mat(), root_array(), node_array(), mesh_index_array(),
num_mesh_index(), backface_mesh_index_array(), num_backface_mesh_index(), skin_param(), reserved() {

}

obj_skin_block_constraint_attach_point::obj_skin_block_constraint_attach_point()
    : affected_by_orientation(), affected_by_scaling() {

}

obj_skin_block_constraint_up_vector::obj_skin_block_constraint_up_vector() : active(), roll(), name() {

}

obj_skin_block_constraint_direction::obj_skin_block_constraint_direction() {

}

obj_skin_block_constraint_distance::obj_skin_block_constraint_distance() : distance() {

}

obj_skin_block_constraint_orientation::obj_skin_block_constraint_orientation() {

}

obj_skin_block_constraint_position::obj_skin_block_constraint_position() {

}

obj_skin_block_constraint::obj_skin_block_constraint()
    : name_index(), source_node_name(), coupling(), type(), data() {

}

obj_skin_block_expression::obj_skin_block_expression()
    : name_index(), num_expression(), expression_array() {

}

obj_skin_motion_node::obj_skin_motion_node() : name_index() {

}

obj_skin_block_motion::obj_skin_block_motion() : name(), node_array(), num_node() {

}

obj_skin_osage_node::obj_skin_osage_node() : name_index(), length() {

}

obj_skin_block_osage::obj_skin_block_osage() : start_index(), count(), node_array(),
num_node(), skin_param(), external_name_index(), name_index(), motion_node_name() {

}

obj_skin_block::obj_skin_block() : type(), node() {

}

obj_skin_osage_sibling_info::obj_skin_osage_sibling_info()
    : name_index(), sibling_name_index(), max_distance() {

}

obj_skin_ex_data::obj_skin_ex_data() : osage_node_array(), num_osage_node(), block_array(), num_block(),
bone_name_array(), num_bone_name(), osage_sibling_info_array(), num_osage_sibling_info(), reserved() {

}

obj_skin_bone::obj_skin_bone() : id(), parent(), name() {

}

obj_skin::obj_skin() : bone_array(), num_bone(), ex_data() {

}

obj::obj() : mesh_array(), num_mesh(), material_array(),
num_material(), flags(), reserved(), skin(), name(), id() {
    hash = hash_murmurhash_empty;
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

obj_set::obj_set() : ready(), modern(), big_endian(), is_x(), obj_data(),
obj_num(), tex_id_data(), tex_id_num(), reserved() {

}

void obj_set::move_data(obj_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!set_src->ready) {
        ready = false;
        modern = false;
        big_endian = false;
        is_x = false;
        return;
    }

    ready = true;
    modern = set_src->modern;
    big_endian = set_src->big_endian;
    is_x = set_src->is_x;

    uint32_t obj_num = set_src->obj_num;
    obj* obj_data_src = set_src->obj_data;
    obj* obj_data_dst = alloc->allocate<obj>(obj_num);

    for (uint32_t i = 0; i < obj_num; i++)
        obj_move_data(&obj_data_dst[i], &obj_data_src[i], alloc);

    this->obj_data = obj_data_dst;
    this->obj_num = obj_num;

    uint32_t tex_id_num = set_src->tex_id_num;
    this->tex_id_data = alloc->allocate<uint32_t>(set_src->tex_id_data, tex_id_num);
    this->tex_id_num = tex_id_num;

    memcpy(this->reserved, set_src->reserved, sizeof(uint32_t) * 2);
}

void obj_set::pack_file(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        obj_set_classic_write_inner(this, s);
    else
        obj_set_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

void obj_set::unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (!modern)
        obj_set_classic_read_inner(this, alloc, s);
    else
        obj_set_modern_read_inner(this, alloc, s);
}

static void obj_material_texture_enrs_table_init() {
    if (!obj_material_texture_enrs_table_initialized) {
        memory_stream s;
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

static void obj_vertex_add_bone_weight(vec4& bone_weight, vec4i16& bone_index, int16_t index, float_t weight) {
    if (bone_index.x < 0) {
        bone_index.x = index;
        bone_weight.x = weight;
    }
    else if (bone_index.y < 0) {
        bone_index.y = index;
        bone_weight.y = weight;
    }
    else if (bone_index.z < 0) {
        bone_index.z = index;
        bone_weight.z = weight;
    }
    else if (bone_index.w < 0) {
        bone_index.w = index;
        bone_weight.w = weight;
    }
}

static void calculate_tangent(obj_vertex_data* vtx,
    vec3* tangents, vec3* bitangents, uint32_t a, uint32_t b, uint32_t c) {
    obj_vertex_data* vtx_a = &vtx[a];
    obj_vertex_data* vtx_b = &vtx[b];
    obj_vertex_data* vtx_c = &vtx[c];

    vec3 pos_a = vtx_c->position - vtx_a->position;
    vec3 pos_b = vtx_b->position - vtx_a->position;

    vec2 uv_a = vtx_c->texcoord0 - vtx_a->texcoord0;
    vec2 uv_b = vtx_b->texcoord0 - vtx_a->texcoord0;

    vec3 tangent = pos_a * uv_b.y - pos_b * uv_a.y;
    vec3 bitangent = pos_b * uv_a.x - pos_a * uv_b.x;

    if (uv_a.x * uv_b.y - uv_a.y * uv_b.x <= 0.0f) {
        tangent = -tangent;
        bitangent = -bitangent;
    }

    tangents[a] += tangent;
    tangents[b] += tangent;
    tangents[c] += tangent;

    bitangents[a] += bitangent;
    bitangents[b] += bitangent;
    bitangents[c] += bitangent;
}

static void obj_vertex_generate_tangents(obj_mesh* mesh) {
    if (!(mesh->vertex_format & OBJ_VERTEX_NORMAL)
        || mesh->vertex_format & OBJ_VERTEX_TANGENT
        || !(mesh->vertex_format & OBJ_VERTEX_TEXCOORD0))
        return;

    uint32_t num_vertex = mesh->num_vertex;

    vec3* tangents = force_malloc_s(vec3, num_vertex);
    vec3* bitangents = force_malloc_s(vec3, num_vertex);

    obj_vertex_data* vtx = mesh->vertex_array;
    uint32_t num_submesh = mesh->num_submesh;
    for (uint32_t i = 0; i < num_submesh; i++) {
        obj_sub_mesh* sub_mesh = &mesh->submesh_array[i];
        if (!sub_mesh->index_array || !sub_mesh->num_index
            || (sub_mesh->primitive_type != OBJ_PRIMITIVE_TRIANGLES
                && sub_mesh->primitive_type != OBJ_PRIMITIVE_TRIANGLE_STRIP))
            continue;

        uint32_t* start = sub_mesh->index_array;
        uint32_t* end = sub_mesh->index_array + sub_mesh->num_index;

        bool triangle_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
        if (!triangle_strip) {
            while (start < end) {
                uint32_t a = *start++;
                uint32_t b = *start++;
                uint32_t c = *start++;
                calculate_tangent(vtx, tangents, bitangents, a, b, c);
            }
        }
        else {
            bool direction = true;

            uint32_t a = *start++;
            uint32_t b = *start++;

            while (start < end) {
                uint32_t c = *start++;

                if (c == -1) {
                    a = *start++;
                    b = *start++;
                    direction = true;
                }
                else {
                    direction ^= true;
                    if (a != b && b != c && c != a) {
                        if (!direction)
                            calculate_tangent(vtx, tangents, bitangents, a, b, c);
                        else
                            calculate_tangent(vtx, tangents, bitangents, a, c, b);
                    }

                    a = b;
                    b = c;
                }
            }
        }
    }

    for (uint32_t i = 0; i < num_vertex; i++) {
        vec3 normal = vtx[i].normal;

        vec3 tangent = vec3::normalize(tangents[i]);
        vec3 bitangent = vec3::normalize(bitangents[i]);

        tangent = vec3::normalize(tangent - normal * vec3::dot(tangent, normal));
        bitangent = vec3::normalize(bitangent - normal * vec3::dot(bitangent, normal));

        vec3 binormal = vec3::normalize(vec3::cross(normal, tangent));

        float_t dir_check = vec3::dot(binormal, bitangent);

        *(vec3*)&vtx[i].tangent = tangent;
        vtx[i].tangent.w = dir_check > 0.0f ? 1.0f : -1.0f;
        tangents[i] = tangent;
        bitangents[i] = bitangent;
    }

    for (uint32_t i = 0; i < num_vertex; i++) {
        vec3 position = vtx[i].position;
        vec3 tangent = tangents[i];

        if (tangent != 0.0f)
            continue;

        int32_t nearest_vtx_idx = -1;
        float_t current_distance = 3.40282346638529e38f;

        for (uint32_t j = 0; j < num_vertex; j++) {
            vec3 position_to_compare = vtx[j].position;
            vec3 tangent_to_compare = tangents[j];

            if (i == j || tangent_to_compare == 0.0f)
                continue;

            float_t distance = vec3::distance_squared(position, position_to_compare);

            if (current_distance >= distance) {
                nearest_vtx_idx = j;
                current_distance = distance;
            }
        }

        if (nearest_vtx_idx != -1)
            vtx[i].tangent = vtx[nearest_vtx_idx].tangent;
        else {
            vec3 normal = vtx[i].normal;

            vec3 temp1 = vec3::cross(normal, { 0.0f, 1.0f, 0.0f });
            vec3 temp2 = vec3::cross(normal, { 1.0f, 0.0f, 0.0f });

            float_t temp3 = vec3::length_squared(temp1);
            float_t temp4 = vec3::length_squared(temp2);

            vec3 tangent = vec3::normalize(temp3 > temp4 ? temp1 : temp2);

            *(vec3*)&vtx[i].tangent = tangent;
            vtx[i].tangent.w = 1.0f;
        }
    }

    free_def(tangents);
    free_def(bitangents);

    enum_or(mesh->vertex_format, OBJ_VERTEX_TANGENT);
}

static void obj_vertex_validate_bone_data(vec4& bone_weight, vec4i16& bone_index) {
    vec4 _bone_weight = { 0.0f, 0.0f, 0.0f, 0.0f };
    vec4i16 _bone_index = { -1, -1, -1, -1 };

    if (bone_index.x >= 0 && bone_weight.x > 0.0f)
        obj_vertex_add_bone_weight(_bone_weight, _bone_index, bone_index.x, bone_weight.x);

    if (bone_index.y >= 0 && bone_weight.y > 0.0f)
        obj_vertex_add_bone_weight(_bone_weight, _bone_index, bone_index.y, bone_weight.y);

    if (bone_index.z >= 0 && bone_weight.z > 0.0f)
        obj_vertex_add_bone_weight(_bone_weight, _bone_index, bone_index.z, bone_weight.z);

    if (bone_index.w >= 0 && bone_weight.w > 0.0f)
        obj_vertex_add_bone_weight(_bone_weight, _bone_index, bone_index.w, bone_weight.w);

    float_t sum = _bone_weight.x + _bone_weight.y + _bone_weight.z + _bone_weight.w;
    if (sum > 0.0f && fabsf(sum - 1.0f) > 0.000001f)
        _bone_weight *= 1.0f / sum;

    bone_weight = _bone_weight;
    bone_index = _bone_index;
}

static void obj_move_data(obj* obj_dst, const obj* obj_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_dst->bounding_sphere = obj_src->bounding_sphere;

    uint32_t num_mesh = obj_src->num_mesh;
    obj_mesh* mesh_array_src = obj_src->mesh_array;
    obj_mesh* mesh_array_dst = alloc->allocate<obj_mesh>(num_mesh);

    for (uint32_t i = 0; i < num_mesh; i++)
        obj_move_data_mesh(&mesh_array_dst[i], &mesh_array_src[i], alloc);

    obj_dst->mesh_array = mesh_array_dst;
    obj_dst->num_mesh = num_mesh;

    uint32_t num_material = obj_src->num_material;
    obj_dst->material_array = alloc->allocate<obj_material_data>(obj_src->material_array, num_material);
    obj_dst->num_material = num_material;

    obj_dst->flags = obj_src->flags;
    memcpy(obj_dst->reserved, obj_src->reserved, sizeof(uint32_t) * 10);

    obj_dst->skin = obj_move_data_skin(obj_src->skin, alloc);

    obj_dst->name = obj_move_data_string(obj_src->name, alloc);
    obj_dst->id = obj_src->id;
    obj_dst->hash = obj_src->hash;
}

static void obj_move_data_mesh(obj_mesh* mesh_dst, const obj_mesh* mesh_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    mesh_dst->flags = mesh_src->flags;
    mesh_dst->bounding_sphere = mesh_src->bounding_sphere;

    uint32_t num_submesh = mesh_src->num_submesh;
    obj_sub_mesh* submesh_array_src = mesh_src->submesh_array;
    obj_sub_mesh* submesh_array_dst = alloc->allocate<obj_sub_mesh>(num_submesh);

    for (uint32_t i = 0; i < num_submesh; i++)
        obj_move_data_sub_mesh(&submesh_array_dst[i], &submesh_array_src[i], alloc);

    mesh_dst->submesh_array = submesh_array_dst;
    mesh_dst->num_submesh = num_submesh;

    mesh_dst->vertex_format = mesh_src->vertex_format;
    mesh_dst->size_vertex = mesh_src->size_vertex;

    uint32_t num_vertex = mesh_src->num_vertex;
    mesh_dst->vertex_array = alloc->allocate<obj_vertex_data>(mesh_src->vertex_array, num_vertex);
    mesh_dst->num_vertex = num_vertex;

    mesh_dst->attrib = mesh_src->attrib;
    memcpy(mesh_dst->reserved, mesh_src->reserved, sizeof(uint32_t) * 6);
    memcpy(mesh_dst->name, mesh_src->name, 0x40);
}

static void obj_move_data_sub_mesh(obj_sub_mesh* sub_mesh_dst, const obj_sub_mesh* sub_mesh_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    sub_mesh_dst->flags = sub_mesh_src->flags;
    sub_mesh_dst->bounding_sphere = sub_mesh_src->bounding_sphere;
    sub_mesh_dst->material_index = sub_mesh_src->material_index;
    memcpy(sub_mesh_dst->uv_index, sub_mesh_src->uv_index, 8);

    uint32_t num_bone_index = sub_mesh_src->num_bone_index;
    sub_mesh_dst->bone_index_array = alloc->allocate<uint16_t>(sub_mesh_src->bone_index_array, num_bone_index);
    sub_mesh_dst->num_bone_index = num_bone_index;

    sub_mesh_dst->bones_per_vertex = sub_mesh_src->bones_per_vertex;
    sub_mesh_dst->primitive_type = sub_mesh_src->primitive_type;
    sub_mesh_dst->index_format = sub_mesh_src->index_format;

    uint32_t num_index = sub_mesh_src->num_index;
    sub_mesh_dst->index_array = alloc->allocate<uint32_t>(sub_mesh_src->index_array, num_index);
    sub_mesh_dst->num_index = num_index;

    sub_mesh_dst->attrib = sub_mesh_src->attrib;
    sub_mesh_dst->bounding_box = sub_mesh_src->bounding_box;

    sub_mesh_dst->first_index = sub_mesh_src->first_index;
    sub_mesh_dst->last_index = sub_mesh_src->last_index;
    sub_mesh_dst->index_offset = sub_mesh_src->index_offset;
}

static obj_skin* obj_move_data_skin(const obj_skin* sk_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!sk_src)
        return 0;

    obj_skin* sk_dst = alloc->allocate<obj_skin>();

    uint32_t num_bone = sk_src->num_bone;
    obj_skin_bone* bone_array_src = sk_src->bone_array;
    obj_skin_bone* bone_array_dst = alloc->allocate<obj_skin_bone>(num_bone);

    for (uint32_t i = 0; i < num_bone; i++)
        obj_move_data_skin_bone(&bone_array_dst[i], &bone_array_src[i], alloc);

    sk_dst->bone_array = bone_array_dst;
    sk_dst->num_bone = num_bone;

    sk_dst->ex_data = obj_move_data_skin_ex_data(sk_src->ex_data, alloc);
    return sk_dst;
}

static void obj_move_data_skin_bone(obj_skin_bone* bone_dst, const obj_skin_bone* bone_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    bone_dst->id = bone_src->id;
    bone_dst->parent = bone_src->parent;
    bone_dst->inv_bind_pose_mat = bone_src->inv_bind_pose_mat;
    bone_dst->name = obj_move_data_string(bone_src->name, alloc);
}

static obj_skin_ex_data* obj_move_data_skin_ex_data(const obj_skin_ex_data* ex_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_ex_data* ex_dst = alloc->allocate<obj_skin_ex_data>();

    uint32_t num_osage_node = ex_src->num_osage_node;
    ex_dst->osage_node_array = alloc->allocate<obj_skin_osage_node>(
        ex_src->osage_node_array, num_osage_node);
    ex_dst->num_osage_node = num_osage_node;

    uint32_t num_block = ex_src->num_block;
    obj_skin_block* block_array_src = ex_src->block_array;
    obj_skin_block* block_array_dst = alloc->allocate<obj_skin_block>(num_block);

    for (uint32_t i = 0; i < num_block; i++) {
        obj_skin_block* block_src = &block_array_src[i];
        obj_skin_block* block_dst = &block_array_dst[i];
        switch (block_src->type) {
        default:
            block_dst->type = OBJ_SKIN_BLOCK_NONE;
            block_dst->node = 0;
            break;
        case OBJ_SKIN_BLOCK_CLOTH:
            block_dst->type = OBJ_SKIN_BLOCK_CLOTH;
            block_dst->cloth = obj_move_data_skin_block_cloth(block_src->cloth, alloc);
            break;
        case OBJ_SKIN_BLOCK_CONSTRAINT:
            block_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT;
            block_dst->constraint = obj_move_data_skin_block_constraint(block_src->constraint, alloc);
            break;
        case OBJ_SKIN_BLOCK_EXPRESSION:
            block_dst->type = OBJ_SKIN_BLOCK_EXPRESSION;
            block_dst->expression = obj_move_data_skin_block_expression(block_src->expression, alloc);
            break;
        case OBJ_SKIN_BLOCK_MOTION:
            block_dst->type = OBJ_SKIN_BLOCK_MOTION;
            block_dst->motion = obj_move_data_skin_block_motion(block_src->motion, alloc);
            break;
        case OBJ_SKIN_BLOCK_OSAGE:
            block_dst->type = OBJ_SKIN_BLOCK_OSAGE;
            block_dst->osage = obj_move_data_skin_block_osage(block_src->osage, alloc);
            break;
        }
    }

    ex_dst->block_array = block_array_dst;
    ex_dst->num_block = num_block;

    uint32_t num_bone_name = ex_src->num_bone_name;
    const char** bone_array_src = ex_src->bone_name_array;
    const char** bone_array_dst = alloc->allocate<const char*>(num_bone_name);

    for (uint32_t i = 0; i < num_bone_name; i++)
        bone_array_dst[i] = obj_move_data_string(bone_array_src[i], alloc);

    ex_dst->bone_name_array = bone_array_dst;
    ex_dst->num_bone_name = num_bone_name;

    uint32_t num_osage_sibling_info = ex_src->num_osage_sibling_info;
    ex_dst->osage_sibling_info_array = alloc->allocate<obj_skin_osage_sibling_info>(
        ex_src->osage_sibling_info_array, num_osage_sibling_info);
    ex_dst->num_osage_sibling_info = num_osage_sibling_info;

    memcpy(ex_dst->reserved, ex_src->reserved, sizeof(int64_t) * 7);
    return ex_dst;
}

static obj_skin_block_cloth* obj_move_data_skin_block_cloth(const obj_skin_block_cloth* cls_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_block_cloth* cls_dst = alloc->allocate<obj_skin_block_cloth>();
    cls_dst->mesh_name = obj_move_data_string(cls_src->mesh_name, alloc);
    cls_dst->backface_mesh_name = obj_move_data_string(cls_src->backface_mesh_name, alloc);
    cls_dst->field_8 = cls_src->field_8;

    uint32_t num_root = cls_src->num_root;
    uint32_t num_node = cls_src->num_node;
    cls_dst->num_root = num_root;
    cls_dst->num_node = num_node;
    cls_dst->field_14 = cls_src->field_14;

    uint32_t num_mat = cls_src->num_mat;
    cls_dst->mat_array = alloc->allocate<mat4>(cls_src->mat_array, num_mat);
    cls_dst->num_mat = num_mat;

    obj_skin_block_cloth_root* root_array_src = cls_src->root_array;
    obj_skin_block_cloth_root* root_array_dst = alloc->allocate<obj_skin_block_cloth_root>(num_root);

    for (uint32_t i = 0; i < num_root; i++)
        obj_move_data_skin_block_cloth_root(&root_array_dst[i], &root_array_src[i], alloc);

    cls_dst->root_array = root_array_dst;

    cls_dst->node_array = alloc->allocate<obj_skin_block_cloth_node>(
        cls_src->node_array, num_root * (num_node - 1ULL));

    uint32_t num_mesh_index = cls_src->num_mesh_index;
    cls_dst->mesh_index_array = alloc->allocate<uint16_t>(
        cls_src->mesh_index_array, num_mesh_index);
    cls_dst->num_mesh_index = num_mesh_index;

    uint32_t num_backface_mesh_index = cls_src->num_backface_mesh_index;
    cls_dst->backface_mesh_index_array = alloc->allocate<uint16_t>(
        cls_src->backface_mesh_index_array, num_backface_mesh_index);
    cls_dst->num_backface_mesh_index = num_backface_mesh_index;

    cls_dst->skin_param = obj_move_data_skin_param(cls_src->skin_param, alloc);
    cls_dst->reserved = cls_src->reserved;
    return cls_dst;
}

static void obj_move_data_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root_dst,
    const obj_skin_block_cloth_root* cloth_root_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    cloth_root_dst->trans = cloth_root_src->trans;
    cloth_root_dst->normal = cloth_root_src->normal;
    cloth_root_dst->field_18 = cloth_root_src->field_18;
    cloth_root_dst->field_1C = cloth_root_src->field_1C;
    cloth_root_dst->field_20 = cloth_root_src->field_20;
    cloth_root_dst->field_24 = cloth_root_src->field_24;

    for (uint32_t i = 0; i < 4; i++)
        obj_move_data_skin_block_cloth_root_bone_weight(&cloth_root_dst->bone_weights[i],
            &cloth_root_src->bone_weights[i], alloc);
}

static void obj_move_data_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight_dst,
    const obj_skin_block_cloth_root_bone_weight* bone_weight_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    bone_weight_dst->bone_name = obj_move_data_string(bone_weight_src->bone_name, alloc);
    bone_weight_dst->weight = bone_weight_src->weight;
    bone_weight_dst->matrix_index = bone_weight_src->matrix_index;
    bone_weight_dst->reserved = bone_weight_src->reserved;
}

static obj_skin_block_constraint* obj_move_data_skin_block_constraint(const obj_skin_block_constraint* cns_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_block_constraint* cns_dst = alloc->allocate<obj_skin_block_constraint>();
    obj_move_data_skin_block_node(&cns_dst->node, &cns_src->node, alloc);
    cns_dst->name_index = cns_src->name_index;
    cns_dst->source_node_name = obj_move_data_string(cns_src->source_node_name, alloc);
    cns_dst->coupling = cns_src->coupling;

    switch (cns_src->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
        cns_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;

        obj_skin_block_constraint_orientation* orientation_src = cns_src->orientation;
        obj_skin_block_constraint_orientation* orientation_dst
            = alloc->allocate<obj_skin_block_constraint_orientation>();
        orientation_dst->offset = orientation_src->offset;
        cns_dst->orientation = orientation_dst;
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
        cns_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;

        obj_skin_block_constraint_direction* direction_src = cns_src->direction;
        obj_skin_block_constraint_direction* direction_dst
            = alloc->allocate<obj_skin_block_constraint_direction>();
        obj_move_data_skin_block_constraint_up_vector(
            &direction_dst->up_vector, &direction_src->up_vector, alloc);
        direction_dst->align_axis = direction_src->align_axis;
        direction_dst->target_offset = direction_src->target_offset;
        cns_dst->direction = direction_dst;
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION: {
        cns_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;

        obj_skin_block_constraint_position* position_src = cns_src->position;
        obj_skin_block_constraint_position* position_dst
            = alloc->allocate<obj_skin_block_constraint_position>();
        obj_move_data_skin_block_constraint_up_vector(
            &position_dst->up_vector, &position_src->up_vector, alloc);
        position_dst->constrained_object = position_src->constrained_object;
        position_dst->constraining_object = position_src->constraining_object;
        cns_dst->position = position_dst;
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE: {
        cns_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;

        obj_skin_block_constraint_distance* distance_src = cns_src->distance;
        obj_skin_block_constraint_distance* distance_dst
            = alloc->allocate<obj_skin_block_constraint_distance>();
        obj_move_data_skin_block_constraint_up_vector(
            &distance_dst->up_vector, &distance_src->up_vector, alloc);
        distance_dst->distance = distance_src->distance;
        distance_dst->constrained_object = distance_src->constrained_object;
        distance_dst->constraining_object = distance_src->constraining_object;
        cns_dst->distance = distance_dst;
    } break;
    default:
        cns_dst->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
        cns_dst->data = 0;
    }
    return cns_dst;
}

static void obj_move_data_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector_dst,
    const obj_skin_block_constraint_up_vector* up_vector_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    up_vector_dst->active = up_vector_src->active;
    up_vector_dst->roll = up_vector_src->roll;
    up_vector_dst->affected_axis = up_vector_src->affected_axis;
    up_vector_dst->point_at = up_vector_src->point_at;
    up_vector_dst->name = obj_move_data_string(up_vector_src->name, alloc);
}

static obj_skin_block_expression* obj_move_data_skin_block_expression(const obj_skin_block_expression* exp_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_block_expression* exp_dst = alloc->allocate<obj_skin_block_expression>();
    obj_move_data_skin_block_node(&exp_dst->node, &exp_src->node, alloc);
    exp_dst->name_index = exp_src->name_index;

    uint32_t num_expression = exp_src->num_expression;
    num_expression = min_def(num_expression, 9);
    exp_dst->num_expression = num_expression;
    for (uint32_t i = 0; i < num_expression; i++)
        exp_dst->expression_array[i] = obj_move_data_string(exp_src->expression_array[i], alloc);

    for (uint32_t i = num_expression; i < 9; i++)
        exp_dst->expression_array[i] = 0;
    return exp_dst;
}

static obj_skin_block_motion* obj_move_data_skin_block_motion(const obj_skin_block_motion* mot_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_block_motion* mot_dst = alloc->allocate<obj_skin_block_motion>();
    obj_move_data_skin_block_node(&mot_dst->node, &mot_src->node, alloc);

    mot_dst->name = 0;
    if ((mot_src->name_index & ~0x7FFF) == 0x8000)
        mot_dst->name_index = mot_src->name_index;
    else
        mot_dst->name = obj_move_data_string(mot_src->name, alloc);

    uint32_t num_node = mot_src->num_node;
    mot_dst->node_array = alloc->allocate<obj_skin_motion_node>(mot_src->node_array, num_node);
    mot_dst->num_node = num_node;
    return mot_dst;
}

static void obj_move_data_skin_block_node(obj_skin_block_node* node_dst, const obj_skin_block_node* node_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    node_dst->parent_name = obj_move_data_string(node_src->parent_name, alloc);
    node_dst->position = node_src->position;
    node_dst->rotation = node_src->rotation;
    node_dst->scale = node_src->scale;
}

static obj_skin_block_osage* obj_move_data_skin_block_osage(const obj_skin_block_osage* osg_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    obj_skin_block_osage* osg_dst = alloc->allocate<obj_skin_block_osage>();
    obj_move_data_skin_block_node(&osg_dst->node, &osg_src->node, alloc);
    osg_dst->start_index = osg_src->start_index;
    osg_dst->count = osg_src->count;

    uint32_t num_node = osg_src->num_node;
    osg_dst->node_array = alloc->allocate<obj_skin_osage_node>(osg_src->node_array, num_node);
    osg_dst->num_node = num_node;

    osg_dst->skin_param = obj_move_data_skin_param(osg_src->skin_param, alloc);
    osg_dst->external_name_index = osg_src->external_name_index;
    osg_dst->name_index = osg_src->name_index;
    osg_dst->motion_node_name = obj_move_data_string(osg_src->motion_node_name, alloc);
    return osg_dst;
}

static obj_skin_skin_param* obj_move_data_skin_param(const obj_skin_skin_param* skp_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!skp_src)
        return 0;

    obj_skin_skin_param* skp_dst = alloc->allocate<obj_skin_skin_param>();
    skp_dst->unk0 = skp_src->unk0;
    skp_dst->force = skp_src->force;
    skp_dst->force_gain = skp_src->force_gain;
    skp_dst->air_res = skp_src->air_res;
    skp_dst->rot_y = skp_src->rot_y;
    skp_dst->rot_z = skp_src->rot_z;
    skp_dst->hinge_y = skp_src->hinge_y;
    skp_dst->hinge_z = skp_src->hinge_z;
    skp_dst->name = obj_move_data_string(skp_src->name, alloc);

    uint32_t coli_count = skp_src->coli_count;
    skp_dst->coli = alloc->allocate<obj_skin_skin_param_coli>(skp_src->coli, coli_count);
    skp_dst->coli_count = coli_count;

    skp_dst->coli_r = skp_src->coli_r;
    skp_dst->friction = skp_src->friction;
    skp_dst->wind_afc = skp_src->wind_afc;
    skp_dst->unk44 = skp_src->unk44;
    return skp_dst;
}

static void obj_set_classic_read_inner(obj_set* set, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    uint32_t version = s.read_uint32_t();
    if (version != 0x05062500) {
        set->ready = false;
        set->modern = false;
        set->big_endian = false;
        set->is_x = false;
        return;
    }

    obj_set_header osh = {};
    set->obj_num = s.read_int32_t();
    osh.last_obj_id = s.read_int32_t();
    osh.obj_data = s.read_int32_t();
    osh.obj_skin_data = s.read_int32_t();
    osh.obj_name_data = s.read_int32_t();
    osh.obj_id_data = s.read_int32_t();
    osh.tex_id_data = s.read_int32_t();
    set->tex_id_num = s.read_int32_t();
    set->reserved[0] = s.read_uint32_t();
    set->reserved[1] = s.read_uint32_t();

    uint32_t obj_num = set->obj_num;
    set->obj_data = alloc->allocate<obj>(obj_num);

    int32_t* data = force_malloc_s(int32_t, obj_num * 3ULL);

    int32_t* obj_data = 0;
    if (osh.obj_data) {
        obj_data = data;
        s.set_position(osh.obj_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_data[i] = s.read_int32_t();
    }

    int32_t* obj_skin_data = 0;
    if (osh.obj_skin_data) {
        obj_skin_data = data + obj_num;
        s.set_position(osh.obj_skin_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_skin_data[i] = s.read_int32_t();
    }

    int32_t* obj_name_data = 0;
    if (osh.obj_name_data) {
        obj_name_data = data + obj_num * 2ULL;
        s.set_position(osh.obj_name_data, SEEK_SET);
        for (uint32_t i = 0; i < obj_num; i++)
            obj_name_data[i] = s.read_int32_t();
    }

    if (osh.obj_data) {
        for (uint32_t i = 0; i < obj_num; i++) {
            obj* obj = &set->obj_data[i];
            if (obj_data[i])
                obj_classic_read_model(obj, alloc, s, obj_data[i]);

            if (osh.obj_skin_data && obj_skin_data[i])
                obj->skin = obj_classic_read_skin(alloc, s, obj_skin_data[i]);

            if (osh.obj_name_data && obj_name_data[i]) {
                obj->name = obj_read_utf8_string_null_terminated_offset(alloc, s, obj_name_data[i]);
                obj->hash = hash_utf8_murmurhash(obj->name);
            }
            else
                obj->hash = hash_murmurhash_empty;
        }

        s.set_position(osh.obj_id_data, SEEK_SET);
        obj* obj_data = set->obj_data;
        for (uint32_t i = 0; i < obj_num; i++)
            obj_data[i].id = s.read_uint32_t();
    }

    if (osh.tex_id_data) {
        s.set_position(osh.tex_id_data, SEEK_SET);
        uint32_t tex_id_num = set->tex_id_num;
        uint32_t* tex_id_data = alloc->allocate<uint32_t>(tex_id_num);
        set->tex_id_data = tex_id_data;
        for (uint32_t i = 0; i < tex_id_num; i++)
            tex_id_data[i] = s.read_uint32_t();
    }

    free_def(data);

    set->ready = true;
    set->modern = false;
    set->big_endian = false;
    set->is_x = false;
}

static void obj_set_classic_write_inner(obj_set* set, stream& s) {
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

    uint32_t obj_num = set->obj_num;

    osh.obj_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(0);
    s.align_write(0x10);

    int32_t* data = force_malloc_s(int32_t, obj_num);

    int32_t* obj_data = data;
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_data[i] = (int32_t)s.get_position();
        obj_classic_write_model(&set->obj_data[i], s, obj_data[i]);
    }
    s.align_write(0x10);

    s.position_push(osh.obj_data, SEEK_SET);
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_uint32_t(obj_data[i]);
    s.position_pop();

    osh.obj_id_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++) {
        int32_t object_id = set->obj_data[i].id;
        s.write_uint32_t(object_id);
        if (osh.last_obj_id < object_id)
            osh.last_obj_id = object_id;
    }
    s.align_write(0x10);

    int32_t* obj_name_data = data;
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_name_data[i] = (int32_t)s.get_position();
        s.write_utf8_string_null_terminated(set->obj_data[i].name);
    }
    s.align_write(0x10);

    osh.obj_name_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(obj_name_data[i]);
    s.align_write(0x10);

    osh.tex_id_data = s.get_position();
    for (uint32_t i = 0; i < set->tex_id_num; i++)
        s.write_uint32_t(set->tex_id_data[i]);
    s.align_write(0x10);

    osh.obj_skin_data = s.get_position();
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(0);
    s.align_write(0x10);

    int32_t* obj_skin_data = data;
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_skin* sk = set->obj_data[i].skin;
        if (!sk) {
            obj_skin_data[i] = 0;
            continue;
        }

        obj_skin_data[i] = (int32_t)s.get_position();
        obj_classic_write_skin(sk, s, obj_skin_data[i]);
    }
    s.align_write(0x10);

    s.position_push(osh.obj_skin_data, SEEK_SET);
    for (uint32_t i = 0; i < obj_num; i++)
        s.write_int32_t(obj_skin_data[i]);
    s.position_pop();

    free_def(data);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(0x05062500);
    s.write_int32_t(set->obj_num);
    s.write_uint32_t(osh.last_obj_id);
    s.write_uint32_t((uint32_t)osh.obj_data);
    s.write_uint32_t((uint32_t)osh.obj_skin_data);
    s.write_uint32_t((uint32_t)osh.obj_name_data);
    s.write_uint32_t((uint32_t)osh.obj_id_data);
    s.write_uint32_t((uint32_t)osh.tex_id_data);
    s.write_int32_t(set->tex_id_num);
    s.write_uint32_t(set->reserved[0]);
    s.write_uint32_t(set->reserved[1]);
    s.position_pop();
}

static void obj_classic_read_index(obj_sub_mesh* sub_mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t num_index = sub_mesh->num_index;
    uint32_t* index_array = alloc->allocate<uint32_t>(num_index);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < num_index; i++) {
            uint8_t idx = s.read_uint8_t();
            index_array[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < num_index; i++) {
            uint16_t idx = s.read_uint16_t();
            index_array[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < num_index; i++)
            index_array[i] = s.read_uint32_t();
        break;
    }
    sub_mesh->index_array = index_array;
}

static void obj_classic_write_index(obj_sub_mesh* sub_mesh, stream& s) {
    uint32_t* index_array = sub_mesh->index_array;
    uint32_t num_index = sub_mesh->num_index;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint8_t((uint8_t)index_array[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint16_t((uint16_t)index_array[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint32_t(index_array[i]);
        break;
    }
    s.align_write(0x04);
}

static void obj_classic_read_model(obj* obj, prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset) {
    const size_t mesh_size = 0xD8;

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
        uint32_t num_mesh = obj->num_mesh;
        obj->mesh_array = alloc->allocate<obj_mesh>(num_mesh);
        for (uint32_t i = 0; i < num_mesh; i++) {
            s.set_position(base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);
            obj_classic_read_model_mesh(&obj->mesh_array[i],
                alloc, s, base_offset);
        }
    }

    if (oh.material_array > 0) {
        s.set_position(base_offset + oh.material_array, SEEK_SET);
        obj->material_array = alloc->allocate<obj_material_data>(obj->num_material);
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data& mat_data = obj->material_array[i];
            s.read(&mat_data, sizeof(obj_material_data));

            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
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

    uint32_t num_mesh = obj->num_mesh;

    obj_header oh = {};
    if (num_mesh) {
        oh.mesh_array = s.get_position() - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, num_mesh);
        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

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

            for (int64_t& i : mh->vertex)
                s.write_int32_t(0);

            s.write_int32_t(0);
            s.write_uint32_t(0);
            s.write(0x18);
            s.write(sizeof(mesh->name));
        }

        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            uint32_t num_submesh = mesh->num_submesh;
            if (num_submesh) {
                mh->submesh_array = s.get_position() - base_offset;
                for (uint32_t j = 0; j < num_submesh; j++) {
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

            obj_classic_write_vertex(mesh, s, mh->vertex,
                base_offset, &mh->num_vertex, &mh->format, &mh->size_vertex);

            for (uint32_t j = 0; j < num_submesh; j++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                obj_sub_mesh_header smh = {};
                if (sub_mesh->bones_per_vertex == 4 && sub_mesh->num_bone_index) {
                    smh.bone_index_array_offset = s.get_position() - base_offset;
                    s.write(sub_mesh->bone_index_array, sub_mesh->num_bone_index * sizeof(uint16_t));
                    s.align_write(0x04);
                }

                smh.index_array_offset = s.get_position() - base_offset;
                obj_classic_write_index(sub_mesh, s);

                s.position_push(base_offset + mh->submesh_array + sub_mesh_size * j, SEEK_SET);
                s.write_uint32_t(sub_mesh->flags);
                s.write_float_t(sub_mesh->bounding_sphere.center.x);
                s.write_float_t(sub_mesh->bounding_sphere.center.y);
                s.write_float_t(sub_mesh->bounding_sphere.center.z);
                s.write_float_t(sub_mesh->bounding_sphere.radius);
                s.write_uint32_t(sub_mesh->material_index);
                s.write(&sub_mesh->uv_index, 0x08);
                s.write_int32_t(sub_mesh->num_bone_index);
                s.write_uint32_t((uint32_t)smh.bone_index_array_offset);
                s.write_uint32_t(sub_mesh->bones_per_vertex);
                s.write_uint32_t(sub_mesh->primitive_type);
                s.write_uint32_t(sub_mesh->index_format);
                s.write_int32_t(sub_mesh->num_index);
                s.write_uint32_t((uint32_t)smh.index_array_offset);
                s.write_uint32_t(sub_mesh->attrib.w);
                s.write(0x18);
                s.write_uint32_t(sub_mesh->index_offset);
                s.position_pop();
            }
        }

        s.position_push(base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < num_mesh; i++) {
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

            for (int64_t& j : mh->vertex)
                s.write_uint32_t((uint32_t)j);

            s.write_uint32_t(mesh->attrib.w);
            s.write_uint32_t(mh->vertex_format_index);
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
        free_def(mhs);
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

static void obj_classic_read_model_mesh(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset) {
    const size_t sub_mesh_size = 0x5C;

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

    for (int64_t& i : mh.vertex)
        i = s.read_int32_t();

    mesh->attrib.w = s.read_uint32_t();
    mh.vertex_format_index = s.read_uint32_t();
    mesh->reserved[0] = s.read_uint32_t();
    mesh->reserved[1] = s.read_uint32_t();
    mesh->reserved[2] = s.read_uint32_t();
    mesh->reserved[3] = s.read_uint32_t();
    mesh->reserved[4] = s.read_uint32_t();
    mesh->reserved[5] = s.read_uint32_t();
    s.read(&mesh->name, sizeof(mesh->name));
    mesh->name[sizeof(mesh->name) - 1] = 0;

    if (mh.submesh_array) {
        uint32_t num_submesh = mesh->num_submesh;
        mesh->submesh_array = alloc->allocate<obj_sub_mesh>(num_submesh);
        for (uint32_t i = 0; i < num_submesh; i++) {
            s.set_position(base_offset + mh.submesh_array + sub_mesh_size * i, SEEK_SET);
            obj_classic_read_model_sub_mesh(&mesh->submesh_array[i],
                alloc, s, base_offset);
        }
    }

    obj_classic_read_vertex(mesh, alloc, s, mh.vertex,
        base_offset, mh.num_vertex, mh.format);
    obj_vertex_generate_tangents(mesh);
}

static void obj_classic_read_model_sub_mesh(obj_sub_mesh* sub_mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset) {
    obj_sub_mesh_header smh = {};
    sub_mesh->flags = s.read_uint32_t();
    sub_mesh->bounding_sphere.center.x = s.read_float_t();
    sub_mesh->bounding_sphere.center.y = s.read_float_t();
    sub_mesh->bounding_sphere.center.z = s.read_float_t();
    sub_mesh->bounding_sphere.radius = s.read_float_t();
    sub_mesh->material_index = s.read_uint32_t();
    s.read(&sub_mesh->uv_index, 0x08);
    sub_mesh->num_bone_index = s.read_int32_t();
    smh.bone_index_array_offset = s.read_uint32_t();
    sub_mesh->bones_per_vertex = s.read_uint32_t();
    sub_mesh->primitive_type = (obj_primitive_type)s.read_uint32_t();
    sub_mesh->index_format = (obj_index_format)s.read_uint32_t();
    sub_mesh->num_index = s.read_int32_t();
    smh.index_array_offset = s.read_uint32_t();
    sub_mesh->attrib.w = s.read_uint32_t();
    sub_mesh->bounding_box.center = sub_mesh->bounding_sphere.center;
    sub_mesh->bounding_box.size.x = sub_mesh->bounding_sphere.radius * 2.0f;
    sub_mesh->bounding_box.size.y = sub_mesh->bounding_sphere.radius * 2.0f;
    sub_mesh->bounding_box.size.z = sub_mesh->bounding_sphere.radius * 2.0f;
    s.read(0, 0x18);
    sub_mesh->index_offset = s.read_uint32_t();

    if (sub_mesh->bones_per_vertex == 4 && smh.bone_index_array_offset) {
        sub_mesh->bone_index_array = alloc->allocate<uint16_t>(sub_mesh->num_bone_index);
        s.set_position(base_offset + smh.bone_index_array_offset, SEEK_SET);
        s.read(sub_mesh->bone_index_array, sub_mesh->num_bone_index * sizeof(uint16_t));
    }

    s.set_position(base_offset + smh.index_array_offset, SEEK_SET);
    obj_classic_read_index(sub_mesh, alloc, s);
}

static obj_skin* obj_classic_read_skin(prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset) {
    obj_skin* sk = alloc->allocate<obj_skin>();
    s.set_position(base_offset, SEEK_SET);

    obj_skin_header sh = {};
    sh.bone_id_array_offset = s.read_uint32_t();
    sh.bone_matrix_array_offset = s.read_uint32_t();
    sh.bone_name_array_offset = s.read_uint32_t();
    sh.ex_data_offset = s.read_uint32_t();
    sk->num_bone = s.read_int32_t();
    sh.bone_parent_id_array_offset = s.read_uint32_t();
    s.read(0, 0x0C);

    if (sh.bone_id_array_offset) {
        sk->bone_array = alloc->allocate<obj_skin_bone>(sk->num_bone);

        s.set_position(sh.bone_id_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < sk->num_bone; i++)
            sk->bone_array[i].id = s.read_uint32_t();

        if (sh.bone_matrix_array_offset) {
            s.set_position(sh.bone_matrix_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                mat4& mat = sk->bone_array[i].inv_bind_pose_mat;
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

        if (sh.bone_parent_id_array_offset) {
            s.set_position(sh.bone_parent_id_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->num_bone; i++)
                sk->bone_array[i].parent = s.read_uint32_t();
        }

        if (sh.bone_name_array_offset) {
            s.set_position(sh.bone_name_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->num_bone; i++)
                sk->bone_array[i].name = obj_read_utf8_string_null_terminated_offset(
                    alloc, s, s.read_int32_t());
        }
    }

    if (sh.ex_data_offset)
        sk->ex_data = obj_classic_read_skin_ex_data(alloc, s, sh.ex_data_offset);
    return sk;
}

static void obj_classic_write_skin(obj_skin* sk, stream& s, int64_t base_offset) {
    obj_skin_header sh = {};
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write(0x0C);
    s.align_write(0x10);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;
    std::vector<string_hash> bone_names;
    if (sk->bone_array) {
        sh.bone_id_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++)
            s.write_int32_t(sk->bone_array[i].id);
        s.align_write(0x10);

        sh.bone_parent_id_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++)
            s.write_int32_t(sk->bone_array[i].parent);
        s.align_write(0x10);

        sh.bone_name_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++) {
            s.write_int32_t(0);
            obj_skin_strings_push_back_check(strings, sk->bone_array[i].name);
        }
        s.align_write(0x10);

        sh.bone_matrix_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++) {
            mat4& mat = sk->bone_array[i].inv_bind_pose_mat;
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
    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

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

        if (ex->num_osage_node) {
            exh.osage_node_array_offset = s.get_position();
            for (uint32_t i = 0; i < ex->num_osage_node; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.align_write(0x10);

            exh.osage_sibling_info_array_offset = s.get_position();
            for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++) {
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
        exh.num_osage = 0;
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];
            if (block->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (block->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.num_osage++;
        }

        if (exh.num_osage || exh.cloth_count) {
            exh.osage_name_array_offset = s.get_position();
            for (uint32_t i = 0; i < exh.num_osage; i++)
                s.write_int32_t(0);

            for (uint32_t i = 0; i < exh.cloth_count; i++)
                s.write_int32_t(0);
            s.align_write(0x10);
        }

        const char** bone_name_array = ex->bone_name_array;
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = block->cloth;
                obj_skin_strings_push_back_check(strings, cloth->mesh_name);
                obj_skin_strings_push_back_check(strings, cloth->backface_mesh_name);
                for (uint32_t k = 0; k < cloth->num_root; k++) {
                    obj_skin_block_cloth_root* root_array = &cloth->root_array[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings, root_array->bone_weights[l].bone_name);
                }

                if (cloth->skin_param)
                    obj_skin_strings_push_back_check(strings, cloth->skin_param->name);
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = block->constraint;
                obj_skin_strings_push_back_check(strings, constraint->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, constraint->name_index);
                obj_skin_strings_push_back_check(strings, constraint->source_node_name);
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings, constraint->direction->up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings, constraint->position->up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings, constraint->distance->up_vector.name);
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = block->expression;
                for (uint32_t j = 0; j < expression->num_expression; j++)
                    obj_skin_strings_push_back_check(strings, expression->expression_array[j]);
                obj_skin_strings_push_back_check(strings, expression->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = block->motion;
                obj_skin_strings_push_back_check(strings, motion->node.parent_name);
                obj_skin_strings_push_back_check(strings, motion->name);
                for (uint32_t j = 0; j < motion->num_node; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_name_array, motion->node_array[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                obj_skin_strings_push_back_check(bone_names, motion->name);
                for (uint32_t j = 0; j < motion->num_node; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_name_array, motion->node_array[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = block->osage;
                obj_skin_strings_push_back_check(strings, osage->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, osage->name_index);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, osage->external_name_index);
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_osage_node* osage_node = &ex->osage_node_array[j];
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_name_array, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_info = ex->osage_sibling_info_array;
                    for (uint32_t k = 0; k < ex->num_osage_sibling_info; k++) {
                        if (name_index == osage_sibling_info->name_index) {
                            obj_skin_strings_push_back_check_by_index(strings,
                                bone_name_array, osage_sibling_info->sibling_name_index);
                            break;
                        }
                        osage_sibling_info++;
                    }
                }

                if (osage->skin_param)
                    obj_skin_strings_push_back_check(strings, osage->skin_param->name);
                obj_skin_strings_push_back_check(strings, "OSG");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, osage->external_name_index);
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_osage_node* osage_node = &ex->osage_node_array[j];
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_name_array, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_info = ex->osage_sibling_info_array;
                    for (uint32_t k = 0; k < ex->num_osage_sibling_info; k++) {
                        if (name_index == osage_sibling_info->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_name_array, osage_sibling_info->sibling_name_index);
                            break;
                        }
                        osage_sibling_info++;
                    }
                }

                obj_skin_osage_node* child_osage_node = &ex->osage_node_array[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_name_array, child_osage_node->name_index);

                    int32_t name_index = child_osage_node->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_info = ex->osage_sibling_info_array;
                    for (uint32_t k = 0; k < ex->num_osage_sibling_info; k++) {
                        if (name_index == osage_sibling_info->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_name_array, osage_sibling_info->sibling_name_index);
                            break;
                        }
                        osage_sibling_info++;
                    }
                    child_osage_node++;
                }
                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, osage->name_index);

                if (osage->skin_param)
                    obj_skin_strings_push_back_check(strings, osage->skin_param->name);
            } break;
            }
        }

        exh.num_bone_name = (int32_t)bone_names.size();
        exh.bone_name_array_offset = s.get_position();
        for (string_hash& i : bone_names)
            s.write_int32_t(0);
        s.align_write(0x10);

        if (ex->num_block > 0) {
            exh.block_array_offset = s.get_position();
            for (uint32_t i = 0; i < ex->num_block; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.align_write(0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->num_block);
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                obj_skin_block_osage* osage = block->osage;
                if (osage->skin_param) {
                    obj_skin_skin_param* skin_param = osage->skin_param;
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

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x2C);
            }
            s.align_write(0x10);

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                obj_skin_block_cloth* cloth = block->cloth;
                if (cloth->skin_param) {
                    obj_skin_skin_param* skin_param = cloth->skin_param;
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

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x10);
                switch (block->constraint->type) {
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
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                bhs[i].block_offset = s.get_position();
                s.write(0x28);
                s.write(0x10);
                motion_block_count++;
            }
            s.align_write(0x10);

            if (exh.num_osage) {
                osage_block_node = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = block->osage;
                    s.write(osage->num_node * (sizeof(uint32_t) + sizeof(float_t) * 4));
                }
                s.align_write(0x10);
            }

            if (motion_block_count) {
                motion_block_node_mats = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = block->motion;
                    s.write(motion->num_node * sizeof(mat4));
                }
                s.align_write(0x10);

                motion_block_node_name_offset = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = block->motion;
                    s.write(motion->num_node * sizeof(uint32_t));
                }
                s.align_write(0x10);
            }

            if (exh.cloth_count) {
                cloth_mats = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    if (cloth->mat_array)
                        s.write(cloth->num_mat * sizeof(mat4));
                }
                s.align_write(0x10);

                cloth_root = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write((sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                        * cloth->num_root * (cloth->num_node - 1ULL));
                }
                s.align_write(0x10);

                cloth_nodes = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write((11 * sizeof(int32_t)) * cloth->num_root * (cloth->num_node - 1ULL));
                }
                s.align_write(0x10);

                cloth_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write(sizeof(uint16_t) + cloth->num_mesh_index * sizeof(uint16_t));
                }
                s.align_write(0x10);

                cloth_backface_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write(sizeof(uint16_t) + cloth->num_backface_mesh_index * sizeof(uint16_t));
                }
                s.align_write(0x10);
            }
        }
    }

    if (sk->bone_array || sk->ex_data) {
        quicksort_string_hash(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (string_hash& i : strings) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.str);
        }
    }
    s.align_write(0x10);

    std::vector<string_hash> osage_names;
    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

        int64_t cls_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CLS");
        int64_t cns_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CNS");
        int64_t exp_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "EXP");
        int64_t mot_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "MOT");
        int64_t osg_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "OSG");

        if (ex->num_block > 0) {
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_cloth(block->cloth,
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

            const char** bone_name_array = ex->bone_name_array;
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_constraint(block->constraint,
                    s, strings, string_offsets, bone_name_array, constraint_type_name_offsets);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_expression(block->expression,
                    s, strings, string_offsets, bone_name_array);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_motion(block->motion,
                    s, strings, string_offsets, bone_name_array,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_classic_write_skin_block_osage(block->osage,
                    s, strings, string_offsets, &osage_block_node);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = block->cloth;
                    obj_skin_strings_push_back_check(osage_names, cloth->mesh_name);
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_name_array, osage->external_name_index);
                }
            }

            s.position_push(exh.block_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
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
            free_def(bhs);
        }
    }

    if (sk->bone_array) {
        s.position_push(sh.bone_name_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < sk->num_bone; i++) {
            size_t str_offset = obj_skin_strings_get_string_offset(
                strings, string_offsets, sk->bone_array[i].name);
            s.write_uint32_t((uint32_t)str_offset);
        }
        s.position_pop();
    }

    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

        if (ex->num_block > 0) {
            s.position_push(exh.bone_name_array_offset, SEEK_SET);
            for (string_hash& i : bone_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                s.write_uint32_t((uint32_t)str_offset);
            }
            s.position_pop();

            s.position_push(exh.osage_node_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->num_osage_node; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_node_array[i];
                s.write_uint32_t(osage_node->name_index);
                s.write_float_t(osage_node->length);
                s.write_uint32_t(osage_node->name_index & 0x7FFF);
            }
            s.position_pop();

            s.position_push(exh.osage_sibling_info_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_info_array[i];
                s.write_uint32_t(osage_sibling_info->name_index);
                s.write_uint32_t(osage_sibling_info->sibling_name_index);
                s.write_float_t(osage_sibling_info->max_distance);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.position_pop();

            exh.num_osage = (int32_t)osage_names.size();
            exh.num_osage -= exh.cloth_count;
            s.position_push(exh.osage_name_array_offset, SEEK_SET);
            for (string_hash& i : osage_names) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, i.c_str());
                s.write_uint32_t((uint32_t)str_offset);
            }
            s.position_pop();
        }

        s.position_push(sh.ex_data_offset, SEEK_SET);
        s.write_int32_t(exh.num_osage);
        s.write_int32_t(ex->num_osage_node);
        s.write(0x04);
        s.write_uint32_t((uint32_t)exh.osage_node_array_offset);
        s.write_uint32_t((uint32_t)exh.osage_name_array_offset);
        s.write_uint32_t((uint32_t)exh.block_array_offset);
        s.write_int32_t(exh.num_bone_name);
        s.write_uint32_t((uint32_t)exh.bone_name_array_offset);
        s.write_uint32_t((uint32_t)exh.osage_sibling_info_array_offset);
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
    s.write_uint32_t((uint32_t)sh.bone_id_array_offset);
    s.write_uint32_t((uint32_t)sh.bone_matrix_array_offset);
    s.write_uint32_t((uint32_t)sh.bone_name_array_offset);
    s.write_uint32_t((uint32_t)sh.ex_data_offset);
    s.write_int32_t(sk->num_bone);
    s.write_uint32_t((uint32_t)sh.bone_parent_id_array_offset);
    s.write(0x0C);
    s.position_pop();
}

static obj_skin_ex_data* obj_classic_read_skin_ex_data(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset) {
    obj_skin_ex_data* ex = alloc->allocate<obj_skin_ex_data>();
    s.set_position(base_offset, SEEK_SET);

    obj_skin_ex_data_header exh = {};
    exh.num_osage = s.read_int32_t();
    ex->num_osage_node = s.read_int32_t();
    s.read(0, 0x04);
    exh.osage_node_array_offset = s.read_uint32_t();
    exh.osage_name_array_offset = s.read_uint32_t();
    exh.block_array_offset = s.read_uint32_t();
    exh.num_bone_name = s.read_int32_t();
    exh.bone_name_array_offset = s.read_uint32_t();
    exh.osage_sibling_info_array_offset = s.read_uint32_t();
    exh.cloth_count = s.read_uint32_t();
    ex->reserved[0] = s.read_uint32_t();
    ex->reserved[1] = s.read_uint32_t();
    ex->reserved[2] = s.read_uint32_t();
    ex->reserved[3] = s.read_uint32_t();
    ex->reserved[4] = s.read_uint32_t();
    ex->reserved[5] = s.read_uint32_t();
    ex->reserved[6] = s.read_uint32_t();

    ex->bone_name_array = 0;
    ex->num_bone_name = 0;
    if (!exh.bone_name_array_offset)
        return ex;

    uint32_t num_bone_name = exh.num_bone_name;
    const char** bone_name_array = alloc->allocate<const char*>(num_bone_name);
    if (!bone_name_array)
        return ex;

    ex->bone_name_array = bone_name_array;
    ex->num_bone_name = num_bone_name;

    s.set_position(exh.bone_name_array_offset, SEEK_SET);
    for (uint32_t i = 0; i < num_bone_name; i++) {
        int32_t string_offset = s.read_int32_t();
        if (string_offset)
            bone_name_array[i] = obj_read_utf8_string_null_terminated_offset(alloc, s, string_offset);
        else
            bone_name_array[i] = 0;
    }

    if (exh.osage_node_array_offset) {
        ex->osage_node_array = alloc->allocate<obj_skin_osage_node>(ex->num_osage_node);

        s.set_position(exh.osage_node_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < ex->num_osage_node; i++) {
            obj_skin_osage_node* osage_node = &ex->osage_node_array[i];

            osage_node->name_index = s.read_uint32_t();
            osage_node->length = s.read_float_t();
            s.read(0, 0x04);
        }
    }

    if (exh.block_array_offset) {
        ex->num_block = 0;
        s.set_position(exh.block_array_offset, SEEK_SET);
        while (s.read_int32_t()) {
            s.read(0, 0x04);
            ex->num_block++;
        }

        obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->num_block);
        s.set_position(exh.block_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < ex->num_block; i++) {
            bhs[i].block_signature_offset = s.read_uint32_t();
            bhs[i].block_offset = s.read_uint32_t();
        }

        ex->block_array = alloc->allocate<obj_skin_block>(ex->num_block);
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];

            std::string block_signature = s.read_string_null_terminated_offset(
                bhs[i].block_signature_offset);
            if (block_signature.size() != 3)
                continue;

            uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
            switch (signature) {
            case 'CLS\0':
                block->type = OBJ_SKIN_BLOCK_CLOTH;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->cloth = obj_classic_read_skin_block_cloth(
                    alloc, s, bone_name_array);
                break;
            case 'CNS\0':
                block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->constraint = obj_classic_read_skin_block_constraint(
                    alloc, s, bone_name_array);
                break;
            case 'EXP\0':
                block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->expression = obj_classic_read_skin_block_expression(
                    alloc, s, bone_name_array);
                break;
            case 'MOT\0':
                block->type = OBJ_SKIN_BLOCK_MOTION;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->motion = obj_classic_read_skin_block_motion(
                    alloc, s, bone_name_array);
                break;
            case 'OSG\0':
                block->type = OBJ_SKIN_BLOCK_OSAGE;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->osage = obj_classic_read_skin_block_osage(
                    alloc, s, bone_name_array);
                break;
            }
        }
        free_def(bhs);
    }

    if (exh.osage_sibling_info_array_offset) {
        ex->num_osage_sibling_info = 0;
        s.set_position(exh.osage_sibling_info_array_offset, SEEK_SET);
        while (s.read_uint32_t()) {
            s.read(0, 0x08);
            ex->num_osage_sibling_info++;
        }

        obj_skin_osage_sibling_info* osi = alloc->allocate<
            obj_skin_osage_sibling_info>(ex->num_osage_sibling_info);
        ex->osage_sibling_info_array = osi;
        s.set_position(exh.osage_sibling_info_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++, osi++) {
            osi->name_index = s.read_uint32_t();
            osi->sibling_name_index = s.read_uint32_t();
            osi->max_distance = s.read_float_t();
        }
    }

    for (uint32_t i = 0; i < ex->num_block; i++) {
        obj_skin_block* block = &ex->block_array[i];
        if (block->type != OBJ_SKIN_BLOCK_OSAGE)
            continue;

        obj_skin_block_osage* osage = block->osage;
        if (!osage->node_array)
            continue;

        int32_t count = osage->count;
        obj_skin_osage_node* child_osage_node = &ex->osage_node_array[osage->start_index];
        obj_skin_osage_node* osage_node = osage->node_array;
        for (int32_t j = 0; j < count; j++) {
            osage_node->name_index = child_osage_node->name_index;
            osage_node->length = child_osage_node->length;

            child_osage_node++;
            osage_node++;
        }
    }
    return ex;
}

static obj_skin_block_cloth* obj_classic_read_skin_block_cloth(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_block_cloth* cls = alloc->allocate<obj_skin_block_cloth>();
    uint32_t mesh_name_offset = s.read_uint32_t();
    uint32_t backface_mesh_name_offset = s.read_uint32_t();
    cls->field_8 = s.read_int32_t();
    cls->num_root = s.read_int32_t();
    cls->num_node = s.read_int32_t();
    cls->field_14 = s.read_int32_t();
    uint32_t mat_array_offset = s.read_uint32_t();
    uint32_t root_array_offset = s.read_uint32_t();
    uint32_t node_array_offset = s.read_uint32_t();
    uint32_t mesh_index_array_offset = s.read_uint32_t();
    uint32_t backface_mesh_index_array_offset = s.read_uint32_t();
    uint32_t skin_param_offset = s.read_uint32_t();
    cls->reserved = s.read_uint32_t();

    cls->mesh_name = obj_read_utf8_string_null_terminated_offset(alloc, s, mesh_name_offset);
    cls->backface_mesh_name = obj_read_utf8_string_null_terminated_offset(alloc, s, backface_mesh_name_offset);

    if (mat_array_offset) {
        int32_t max_matrix_index = -1;

        s.position_push(root_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < cls->num_root; i++) {
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

        cls->num_mat = max_matrix_index > -1 ? max_matrix_index + 1 : 0;
        cls->mat_array = alloc->allocate<mat4>(cls->num_mat);
        s.position_push(mat_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < cls->num_mat; i++) {
            mat4& mat = cls->mat_array[i];
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

    if (root_array_offset) {
        s.position_push(root_array_offset, SEEK_SET);
        cls->root_array = alloc->allocate<obj_skin_block_cloth_root>(cls->num_root);
        for (uint32_t i = 0; i < cls->num_root; i++)
            obj_classic_read_skin_block_cloth_root(&cls->root_array[i], alloc, s, str);
        s.position_pop();
    }

    if (node_array_offset) {
        s.position_push(node_array_offset, SEEK_SET);
        cls->node_array = alloc->allocate<obj_skin_block_cloth_node>(
            cls->num_root * (cls->num_node - 1ULL));
        for (uint32_t i = 0; i < cls->num_node - 1; i++)
            for (uint32_t j = 0; j < cls->num_root; j++) {
                obj_skin_block_cloth_node* f = &cls->node_array[i * cls->num_root + j];
                f->flags = s.read_uint32_t();
                f->trans.x = s.read_float_t();
                f->trans.y = s.read_float_t();
                f->trans.z = s.read_float_t();
                f->trans_diff.x = s.read_float_t();
                f->trans_diff.y = s.read_float_t();
                f->trans_diff.z = s.read_float_t();
                f->dist_top = s.read_float_t();
                f->dist_bottom = s.read_float_t();
                f->dist_right = s.read_float_t();
                f->dist_left = s.read_float_t();
            }
        s.position_pop();
    }

    if (mesh_index_array_offset) {
        s.position_push(mesh_index_array_offset, SEEK_SET);
        cls->num_mesh_index = s.read_uint16_t_reverse_endianness();
        cls->mesh_index_array = alloc->allocate<uint16_t>(cls->num_mesh_index);
        s.read(cls->mesh_index_array, cls->num_mesh_index * sizeof(uint16_t));
        s.position_pop();
    }

    if (backface_mesh_index_array_offset) {
        s.position_push(backface_mesh_index_array_offset, SEEK_SET);
        cls->num_backface_mesh_index = s.read_uint16_t_reverse_endianness();
        cls->backface_mesh_index_array = alloc->allocate<uint16_t>(cls->num_backface_mesh_index);
        s.read(cls->backface_mesh_index_array, cls->num_backface_mesh_index * sizeof(uint16_t));
        s.position_pop();
    }

    if (skin_param_offset)
        cls->skin_param = obj_classic_read_skin_param(alloc, s, str);
    return cls;
}

static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* cls,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    int64_t* mat_array_offset, int64_t* root_array_offset, int64_t* node_array_offset,
    int64_t* mesh_index_array_offset, int64_t* backface_mesh_index_array_offset) {
    int64_t skin_param_offset = 0;
    if (cls->skin_param) {
        skin_param_offset = s.get_position();
        obj_classic_write_skin_param(cls->skin_param, s, strings, string_offsets);
    }

    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cls->mesh_name);
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cls->backface_mesh_name);

    s.write_uint32_t((uint32_t)mesh_name_offset);
    s.write_uint32_t((uint32_t)backface_mesh_name_offset);
    s.write_int32_t(cls->field_8);
    s.write_int32_t(cls->num_root);
    s.write_int32_t(cls->num_node);
    s.write_int32_t(cls->field_14);
    s.write_uint32_t((uint32_t)*mat_array_offset);
    s.write_uint32_t((uint32_t)*root_array_offset);
    s.write_uint32_t((uint32_t)*node_array_offset);
    s.write_uint32_t((uint32_t)*mesh_index_array_offset);
    s.write_uint32_t((uint32_t)*backface_mesh_index_array_offset);
    s.write_uint32_t((uint32_t)skin_param_offset);
    s.write_uint32_t(cls->reserved);

    if (cls->mat_array) {
        s.position_push(*mat_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < cls->num_mat; i++) {
            mat4& mat = cls->mat_array[i];
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
            *mat_array_offset += sizeof(mat4);
        }
        s.position_pop();
    }

    if (cls->root_array) {
        s.position_push(*root_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        obj_skin_block_cloth_root* root_array = cls->root_array;
        for (uint32_t i = 0; i < num_root; i++)
            obj_classic_write_skin_block_cloth_root(&root_array[i],
                s, strings, string_offsets);
        s.position_pop();
        *root_array_offset += (sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
            * cls->num_root * (cls->num_node - 1ULL);
    }

    if (cls->node_array) {
        s.position_push(*node_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        uint32_t num_node = cls->num_node;
        obj_skin_block_cloth_node* node_array = cls->node_array;
        for (uint32_t i = 0; i < num_node - 1; i++)
            for (uint32_t j = 0; j < num_root; j++) {
                obj_skin_block_cloth_node* f = &node_array[i * cls->num_root + j];
                s.write_uint32_t(f->flags);
                s.write_float_t(f->trans.x);
                s.write_float_t(f->trans.y);
                s.write_float_t(f->trans.z);
                s.write_float_t(f->trans_diff.x);
                s.write_float_t(f->trans_diff.y);
                s.write_float_t(f->trans_diff.z);
                s.write_float_t(f->dist_top);
                s.write_float_t(f->dist_bottom);
                s.write_float_t(f->dist_right);
                s.write_float_t(f->dist_left);
            }
        s.position_pop();
        *node_array_offset += (11 * sizeof(int32_t)) * cls->num_root * (cls->num_node - 1ULL);
    }

    if (cls->mesh_index_array) {
        s.position_push(*mesh_index_array_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)cls->num_mesh_index);
        s.write(cls->mesh_index_array, cls->num_mesh_index * sizeof(uint16_t));
        s.position_pop();
        *mesh_index_array_offset += sizeof(uint16_t) + cls->num_mesh_index * sizeof(uint16_t);
    }

    if (cls->backface_mesh_index_array) {
        s.position_push(*backface_mesh_index_array_offset, SEEK_SET);
        s.write_uint16_t((uint16_t)cls->num_backface_mesh_index);
        s.write(cls->backface_mesh_index_array, cls->num_backface_mesh_index * sizeof(uint16_t));
        s.position_pop();
        *backface_mesh_index_array_offset += sizeof(uint16_t) + cls->num_backface_mesh_index * sizeof(uint16_t);
    }
}

static void obj_classic_read_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    cloth_root->trans.x = s.read_float_t();
    cloth_root->trans.y = s.read_float_t();
    cloth_root->trans.z = s.read_float_t();
    cloth_root->normal.x = s.read_float_t();
    cloth_root->normal.y = s.read_float_t();
    cloth_root->normal.z = s.read_float_t();
    cloth_root->field_18 = s.read_float_t();
    cloth_root->field_1C = s.read_int32_t();
    cloth_root->field_20 = s.read_int32_t();
    cloth_root->field_24 = s.read_int32_t();

    for (uint32_t i = 0; i < 4; i++)
        obj_classic_read_skin_block_cloth_root_bone_weight(&cloth_root->bone_weights[i],
            alloc, s, str);
}

static void obj_classic_write_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
    s.write_float_t(cloth_root->trans.x);
    s.write_float_t(cloth_root->trans.y);
    s.write_float_t(cloth_root->trans.z);
    s.write_float_t(cloth_root->normal.x);
    s.write_float_t(cloth_root->normal.y);
    s.write_float_t(cloth_root->normal.z);
    s.write_float_t(cloth_root->field_18);
    s.write_int32_t(cloth_root->field_1C);
    s.write_int32_t(cloth_root->field_20);
    s.write_int32_t(cloth_root->field_24);

    for (uint32_t j = 0; j < 4; j++)
        obj_classic_write_skin_block_cloth_root_bone_weight(&cloth_root->bone_weights[j],
            s, strings, string_offsets);
}

static void obj_classic_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    int32_t bone_name_offset = s.read_uint32_t();
    bone_weight->bone_name = obj_read_utf8_string_null_terminated_offset(alloc, s, bone_name_offset);
    bone_weight->weight = s.read_float_t();
    bone_weight->matrix_index = s.read_uint32_t();
    bone_weight->reserved = s.read_uint32_t();
}

static void obj_classic_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, bone_weight->bone_name);
    s.write_uint32_t((uint32_t)bone_name_offset);
    s.write_float_t(bone_weight->weight);
    s.write_uint32_t(bone_weight->matrix_index);
    s.write_uint32_t(bone_weight->reserved);
}

static obj_skin_block_constraint* obj_classic_read_skin_block_constraint(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_block_constraint* cns = alloc->allocate<obj_skin_block_constraint>();
    obj_classic_read_skin_block_node(&cns->node, alloc, s, str);

    uint32_t type_offset = s.read_uint32_t();
    char* type = s.read_utf8_string_null_terminated_offset(type_offset);

    uint32_t name_offset = s.read_uint32_t();
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    cns->name_index = 0;
    for (const char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            cns->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free_def(name);

    cns->coupling = (obj_skin_block_constraint_coupling)s.read_int32_t();

    uint32_t source_node_name_offset = s.read_uint32_t();
    cns->source_node_name = obj_read_utf8_string_null_terminated_offset(alloc, s, source_node_name_offset);

    if (!str_utils_compare(type, "Orientation")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;

        obj_skin_block_constraint_orientation* orientation
            = alloc->allocate<obj_skin_block_constraint_orientation>();
        orientation->offset.x = s.read_float_t();
        orientation->offset.y = s.read_float_t();
        orientation->offset.z = s.read_float_t();
        cns->orientation = orientation;
    }
    else if (!str_utils_compare(type, "Direction")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;

        obj_skin_block_constraint_direction* direction
            = alloc->allocate<obj_skin_block_constraint_direction>();
        obj_classic_read_skin_block_constraint_up_vector(&direction->up_vector,
            alloc, s, str);
        direction->align_axis.x = s.read_float_t();
        direction->align_axis.y = s.read_float_t();
        direction->align_axis.z = s.read_float_t();
        direction->target_offset.x = s.read_float_t();
        direction->target_offset.y = s.read_float_t();
        direction->target_offset.z = s.read_float_t();
        cns->direction = direction;
    }
    else if (!str_utils_compare(type, "Position")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;

        obj_skin_block_constraint_position* position
            = alloc->allocate<obj_skin_block_constraint_position>();
        obj_classic_read_skin_block_constraint_up_vector(&position->up_vector,
            alloc, s, str);
        obj_classic_read_skin_block_constraint_attach_point(&position->constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&position->constraining_object,
            s, str);
        cns->position = position;
    }
    else if (!str_utils_compare(type, "Distance")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;

        obj_skin_block_constraint_distance* distance
            = alloc->allocate<obj_skin_block_constraint_distance>();
        obj_classic_read_skin_block_constraint_up_vector(&distance->up_vector,
            alloc, s, str);
        distance->distance = s.read_float_t();
        obj_classic_read_skin_block_constraint_attach_point(&distance->constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&distance->constraining_object,
            s, str);
        cns->distance = distance;
    }
    else {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
        cns->data = 0;
    }
    free_def(type);
    return cns;
}

static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* cns,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, int64_t* offsets) {
    obj_classic_write_skin_block_node(&cns->node, s, strings, string_offsets);

    int64_t type_offset = 0;
    switch (cns->type) {
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
        string_offsets, bone_name_array, cns->name_index);
    s.write_uint32_t((uint32_t)name_offset);

    s.write_int32_t(cns->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cns->source_node_name);
    s.write_uint32_t((uint32_t)source_node_name_offset);

    switch (cns->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        s.write_float_t(cns->orientation->offset.x);
        s.write_float_t(cns->orientation->offset.y);
        s.write_float_t(cns->orientation->offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_classic_write_skin_block_constraint_up_vector(&cns->direction->up_vector,
            s, strings, string_offsets);
        s.write_float_t(cns->direction->align_axis.x);
        s.write_float_t(cns->direction->align_axis.y);
        s.write_float_t(cns->direction->align_axis.z);
        s.write_float_t(cns->direction->target_offset.x);
        s.write_float_t(cns->direction->target_offset.y);
        s.write_float_t(cns->direction->target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_classic_write_skin_block_constraint_up_vector(&cns->position->up_vector,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&cns->position->constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&cns->position->constraining_object,
            s, strings, string_offsets);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_classic_write_skin_block_constraint_up_vector(&cns->distance->up_vector,
            s, strings, string_offsets);
        s.write_float_t(cns->distance->distance);
        obj_classic_write_skin_block_constraint_attach_point(&cns->distance->constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&cns->distance->constraining_object,
            s, strings, string_offsets);
        break;
    }
}

static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, const char** str) {
    attach_point->affected_by_orientation = s.read_int32_t() != 0;
    attach_point->affected_by_scaling = s.read_int32_t() != 0;
    attach_point->offset.x = s.read_float_t();
    attach_point->offset.y = s.read_float_t();
    attach_point->offset.z = s.read_float_t();
}

static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
    s.write_int32_t(attach_point->affected_by_orientation ? 1 : 0);
    s.write_int32_t(attach_point->affected_by_scaling ? 1 : 0);
    s.write_float_t(attach_point->offset.x);
    s.write_float_t(attach_point->offset.y);
    s.write_float_t(attach_point->offset.z);
}

static void obj_classic_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    up_vector->active = !!s.read_int32_t();
    up_vector->roll = s.read_float_t();
    up_vector->affected_axis.x = s.read_float_t();
    up_vector->affected_axis.y = s.read_float_t();
    up_vector->affected_axis.z = s.read_float_t();
    up_vector->point_at.x = s.read_float_t();
    up_vector->point_at.y = s.read_float_t();
    up_vector->point_at.z = s.read_float_t();

    uint32_t name_offset = s.read_uint32_t();
    up_vector->name = obj_read_utf8_string_null_terminated_offset(alloc, s, name_offset);
}

static void obj_classic_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
    s.write_int32_t(up_vector->active ? 1 : 0);
    s.write_float_t(up_vector->roll);
    s.write_float_t(up_vector->affected_axis.x);
    s.write_float_t(up_vector->affected_axis.y);
    s.write_float_t(up_vector->affected_axis.z);
    s.write_float_t(up_vector->point_at.x);
    s.write_float_t(up_vector->point_at.y);
    s.write_float_t(up_vector->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, up_vector->name);
    s.write_uint32_t((uint32_t)name_offset);
}

static obj_skin_block_expression* obj_classic_read_skin_block_expression(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_block_expression* exp = alloc->allocate<obj_skin_block_expression>();
    obj_classic_read_skin_block_node(&exp->node, alloc, s, str);

    uint32_t name_offset = s.read_uint32_t();
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    exp->name_index = 0;
    for (const char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            exp->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free_def(name);

    uint32_t num_expression = s.read_int32_t();
    num_expression = min_def(num_expression, 9);
    exp->num_expression = num_expression;
    for (uint32_t i = 0; i < num_expression; i++) {
        uint32_t expression_offset = s.read_uint32_t();
        if (expression_offset)
            exp->expression_array[i]
                = obj_read_utf8_string_null_terminated_offset(alloc, s, expression_offset);
    }

    for (uint32_t i = num_expression; i < 9; i++)
        exp->expression_array[i] = 0;
    return exp;
}

static void obj_classic_write_skin_block_expression(obj_skin_block_expression* exp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array) {
    obj_classic_write_skin_block_node(&exp->node, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_name_array, exp->name_index);
    s.write_uint32_t((uint32_t)name_offset);

    uint32_t num_expression = exp->num_expression;
    num_expression = min_def(num_expression, 9);
    s.write_int32_t(num_expression);
    for (uint32_t i = 0; i < num_expression; i++) {
        int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, exp->expression_array[i]);
        s.write_uint32_t((uint32_t)expression_offset);
    }

    for (uint32_t i = num_expression; i < 9; i++)
        s.write_int32_t(0);
}

static obj_skin_block_motion* obj_classic_read_skin_block_motion(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_block_motion* mot = alloc->allocate<obj_skin_block_motion>();
    obj_classic_read_skin_block_node(&mot->node, alloc, s, str);

    uint32_t name_offset = s.read_uint32_t();
    mot->num_node = s.read_int32_t();
    uint32_t bone_name_array_offset = s.read_uint32_t();
    uint32_t bone_matrix_array_offset = s.read_uint32_t();

    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    mot->name_index = 0;
    for (const char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            mot->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free_def(name);

    mot->node_array = 0;

    if (!mot->num_node)
        return mot;

    mot->node_array = alloc->allocate<obj_skin_motion_node>(mot->num_node);

    if (bone_name_array_offset) {
        s.position_push(bone_name_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++)
            mot->node_array[i].name_index = s.read_uint32_t();
        s.position_pop();
    }

    if (bone_matrix_array_offset) {
        s.position_push(bone_matrix_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++) {
            mat4& mat = mot->node_array[i].inv_bind_pose_mat;
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
    return mot;
}

static void obj_classic_write_skin_block_motion(obj_skin_block_motion* mot,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, int64_t* bone_name_array_offset, int64_t* bone_matrix_array_offset) {
    obj_classic_write_skin_block_node(&mot->node, s, strings, string_offsets);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_name_array, mot->name_index);
    s.write_uint32_t((uint32_t)name_offset);
    s.write_int32_t(mot->num_node);
    s.write_uint32_t((uint32_t)*bone_name_array_offset);
    s.write_uint32_t((uint32_t)*bone_matrix_array_offset);

    if (mot->node_array) {
        s.position_push(*bone_name_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++)
            s.write_uint32_t(mot->node_array[i].name_index);
        s.position_pop();
        *bone_name_array_offset += mot->num_node * sizeof(uint32_t);

        s.position_push(*bone_matrix_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++) {
            mat4& mat = mot->node_array[i].inv_bind_pose_mat;
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
        *bone_matrix_array_offset += mot->num_node * sizeof(mat4);
    }
}

static void obj_classic_read_skin_block_node(obj_skin_block_node* node,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    uint32_t parent_name_offset = s.read_uint32_t();
    node->parent_name = obj_read_utf8_string_null_terminated_offset(alloc, s, parent_name_offset);

    node->position.x = s.read_float_t();
    node->position.y = s.read_float_t();
    node->position.z = s.read_float_t();
    node->rotation.x = s.read_float_t();
    node->rotation.y = s.read_float_t();
    node->rotation.z = s.read_float_t();
    node->scale.x = s.read_float_t();
    node->scale.y = s.read_float_t();
    node->scale.z = s.read_float_t();
}

static void obj_classic_write_skin_block_node(obj_skin_block_node* node,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, node->parent_name);
    s.write_uint32_t((uint32_t)parent_name_offset);

    s.write_float_t(node->position.x);
    s.write_float_t(node->position.y);
    s.write_float_t(node->position.z);
    s.write_float_t(node->rotation.x);
    s.write_float_t(node->rotation.y);
    s.write_float_t(node->rotation.z);
    s.write_float_t(node->scale.x);
    s.write_float_t(node->scale.y);
    s.write_float_t(node->scale.z);
}

static obj_skin_block_osage* obj_classic_read_skin_block_osage(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_block_osage* osg = alloc->allocate<obj_skin_block_osage>();
    obj_classic_read_skin_block_node(&osg->node, alloc, s, str);

    osg->start_index = s.read_int32_t();
    osg->count = s.read_int32_t();
    osg->external_name_index = s.read_uint32_t();
    osg->name_index = s.read_uint32_t();

    int32_t offset = s.read_int32_t();
    s.read(0, 0x14);

    osg->node_array = 0;

    if (!osg->count || !offset)
        return osg;

    osg->num_node = osg->count;
    osg->node_array = alloc->allocate<obj_skin_osage_node>(osg->count);

    s.position_push(offset, SEEK_SET);
    if (!(s.read_uint32_t() & 0x8000))
        osg->skin_param = obj_classic_read_skin_param( alloc, s, str);
    else {
        s.set_position(offset, SEEK_SET);
        for (uint32_t i = 0; i < osg->num_node; i++) {
            obj_skin_osage_node* node = &osg->node_array[i];
            node->name_index = s.read_uint32_t();
            node->length = s.read_float_t();
            node->rotation.x = s.read_float_t();
            node->rotation.y = s.read_float_t();
            node->rotation.z = s.read_float_t();
        }
    }
    s.position_pop();
    return osg;
}

static void obj_classic_write_skin_block_osage(obj_skin_block_osage* osg,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, int64_t* node_array_offset) {
    int64_t skin_param_offset = 0;
    if (osg->skin_param) {
        skin_param_offset = s.get_position();
        obj_classic_write_skin_param(osg->skin_param, s, strings, string_offsets);
    }

    obj_classic_write_skin_block_node(&osg->node, s, strings, string_offsets);

    s.write_int32_t(osg->start_index);
    s.write_int32_t(osg->count);
    s.write_uint32_t(osg->external_name_index);
    s.write_uint32_t(osg->name_index);

    if (osg->skin_param)
        s.write_uint32_t((uint32_t)skin_param_offset);
    else
        s.write_uint32_t((uint32_t)*node_array_offset);
    s.write(0x14);

    if (!osg->skin_param && osg->node_array) {
        s.position_push(*node_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < osg->num_node; i++) {
            obj_skin_osage_node* node = &osg->node_array[i];
            s.write_uint32_t(node->name_index);
            s.write_float_t(node->length);
            s.write_float_t(node->rotation.x);
            s.write_float_t(node->rotation.y);
            s.write_float_t(node->rotation.z);
        }
        s.position_pop();
        *node_array_offset += osg->num_node * (sizeof(uint32_t) + sizeof(float_t) * 4);
    }
}

static obj_skin_skin_param* obj_classic_read_skin_param(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, const char** str) {
    obj_skin_skin_param* skp = alloc->allocate<obj_skin_skin_param>();
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

    skp->name = obj_read_utf8_string_null_terminated_offset(alloc, s, name_offset);

    if (coli_offset) {
        s.set_position(coli_offset, SEEK_SET);
        int32_t coli_count = 0;
        while (s.read_int32_t()) {
            s.read(44);
            coli_count++;
        }

        skp->coli = alloc->allocate<obj_skin_skin_param_coli>(coli_count);
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
    return skp;
}

static void obj_classic_write_skin_param(obj_skin_skin_param* skp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets) {
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

static void obj_classic_read_vertex(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t* vertex, int64_t base_offset, uint32_t num_vertex,
    obj_vertex_format_file vertex_format_file) {
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

    obj_vertex_data* vtx = alloc->allocate<obj_vertex_data>(num_vertex);
    for (uint32_t i = 0; i < 20; i++) {
        obj_vertex_format_file attribute = (obj_vertex_format_file)(1 << i);
        if (!(vertex_format_file & attribute))
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
                int32_t bone_index_x = (int32_t)s.read_float_t();
                int32_t bone_index_y = (int32_t)s.read_float_t();
                int32_t bone_index_z = (int32_t)s.read_float_t();
                int32_t bone_index_w = (int32_t)s.read_float_t();
                vtx[j].bone_index.x = (int16_t)(bone_index_x >= 0 ? bone_index_x / 3 : -1);
                vtx[j].bone_index.y = (int16_t)(bone_index_y >= 0 ? bone_index_y / 3 : -1);
                vtx[j].bone_index.z = (int16_t)(bone_index_z >= 0 ? bone_index_z / 3 : -1);
                vtx[j].bone_index.w = (int16_t)(bone_index_w >= 0 ? bone_index_w / 3 : -1);
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

    if (vertex_format_file & (OBJ_VERTEX_FILE_BONE_WEIGHT | OBJ_VERTEX_FILE_BONE_INDEX))
        for (uint32_t i = 0; i < num_vertex; i++)
            obj_vertex_validate_bone_data(vtx[i].bone_weight, vtx[i].bone_index);

    mesh->vertex_array = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
}

static void obj_classic_write_vertex(obj_mesh* mesh,
    stream& s, int64_t* vertex, int64_t base_offset, uint32_t* num_vertex,
    obj_vertex_format_file* vertex_format_file, uint32_t* size_vertex) {
    obj_vertex_data* vtx = mesh->vertex_array;
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

static void obj_set_modern_read_inner(obj_set* set, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOSD') || !st.data.size())
        return;

    bool big_endian = st.header.use_big_endian;

    memory_stream s_mosd;
    s_mosd.open(st.data);
    s_mosd.big_endian = big_endian;

    uint32_t version = s_mosd.read_uint32_t_reverse_endianness();
    if (version != 0x05062501) {
        set->ready = false;
        set->modern = false;
        set->big_endian = false;
        set->is_x = false;
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
        set->obj_num = s_mosd.read_int32_t_reverse_endianness();
        osh.last_obj_id = s_mosd.read_int32_t_reverse_endianness();
        osh.obj_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_skin_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_name_data = s_mosd.read_offset_f2(st.header.length);
        osh.obj_id_data = s_mosd.read_offset_f2(st.header.length);
        osh.tex_id_data = s_mosd.read_offset_f2(st.header.length);
        set->tex_id_num = s_mosd.read_int32_t_reverse_endianness();
        set->reserved[0] = s_mosd.read_uint32_t_reverse_endianness();
        set->reserved[1] = s_mosd.read_uint32_t_reverse_endianness();
    }
    else {
        set->obj_num = s_mosd.read_int32_t_reverse_endianness();
        osh.last_obj_id = s_mosd.read_int32_t_reverse_endianness();
        osh.obj_data = s_mosd.read_offset_x();
        osh.obj_skin_data = s_mosd.read_offset_x();
        osh.obj_name_data = s_mosd.read_offset_x();
        osh.obj_id_data = s_mosd.read_offset_x();
        osh.tex_id_data = s_mosd.read_offset_x();
        set->tex_id_num = s_mosd.read_int32_t_reverse_endianness();
        set->reserved[0] = s_mosd.read_uint32_t_reverse_endianness();
        set->reserved[1] = s_mosd.read_uint32_t_reverse_endianness();
    }

    uint32_t obj_num = set->obj_num;
    set->obj_data = alloc->allocate<obj>(obj_num);

    int64_t* data = force_malloc_s(int64_t, obj_num * 3ULL);

    int64_t* obj_data = 0;
    if (osh.obj_data) {
        obj_data = data;
        s_mosd.set_position(osh.obj_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < obj_num; i++)
                obj_data[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < obj_num; i++)
                obj_data[i] = s_mosd.read_offset_x();
    }

    int64_t* obj_skin_data = 0;
    if (osh.obj_skin_data) {
        obj_skin_data = data + obj_num;
        s_mosd.set_position(osh.obj_skin_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < obj_num; i++)
                obj_skin_data[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < obj_num; i++)
                obj_skin_data[i] = s_mosd.read_offset_x();
    }

    int64_t* obj_name_data = 0;
    if (osh.obj_name_data) {
        obj_name_data = data + obj_num * 2ULL;
        s_mosd.set_position(osh.obj_name_data, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < obj_num; i++)
                obj_name_data[i] = s_mosd.read_offset_f2(st.header.length);
        else
            for (uint32_t i = 0; i < obj_num; i++)
                obj_name_data[i] = s_mosd.read_offset_x();
    }

    if (osh.obj_data)
        for (uint32_t i = 0; i < obj_num; i++) {
            obj* obj = &set->obj_data[i];
            if (osh.obj_name_data && obj_name_data[i]) {
                obj->name = obj_read_utf8_string_null_terminated_offset(alloc, s_mosd, obj_name_data[i]);
                obj->hash = hash_utf8_murmurhash(obj->name);
            }
            else
                obj->hash = hash_murmurhash_empty;
        }

    if (osh.obj_id_data) {
        s_mosd.set_position(osh.obj_id_data, SEEK_SET);
        obj* obj_data = set->obj_data;
        for (uint32_t i = 0; i < obj_num; i++)
            obj_data[i].id = s_mosd.read_uint32_t_reverse_endianness();
    }

    free_def(data);

    if (osh.tex_id_data) {
        s_mosd.set_position(osh.tex_id_data, SEEK_SET);
        uint32_t tex_id_num = set->tex_id_num;
        uint32_t* tex_id_data = alloc->allocate<uint32_t>(tex_id_num);
        set->tex_id_data = tex_id_data;
        for (uint32_t i = 0; i < tex_id_num; i++)
            tex_id_data[i] = s_mosd.read_uint32_t_reverse_endianness();
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

        memory_stream s_oskn;
        memory_stream s_oidx;
        memory_stream s_ovtx;
        stream* s_oskn_ptr = 0;
        stream* s_oidx_ptr = 0;
        stream* s_ovtx_ptr = 0;
        if (oskn) {
            s_oskn.open(oskn->data);
            s_oskn.big_endian = oskn->header.use_big_endian;
            s_oskn_ptr = &s_oskn;
        }

        if (oidx) {
            s_oidx.open(oidx->data);
            s_oidx.big_endian = oidx->header.use_big_endian;
            s_oidx_ptr = &s_oidx;
        }

        if (ovtx) {
            s_ovtx.open(ovtx->data);
            s_ovtx.big_endian = ovtx->header.use_big_endian;
            s_ovtx_ptr = &s_ovtx;
        }

        obj* obj = &set->obj_data[omdl_index];
        memory_stream s_omdl;
        s_omdl.open(i.data);
        s_omdl.big_endian = i.header.use_big_endian;
        obj_modern_read_model(obj, alloc,
            s_omdl, 0, i.header.length, is_x, s_oidx_ptr, s_ovtx_ptr);
        s_omdl.close();

        if (s_oskn_ptr)
            obj->skin = obj_modern_read_skin(alloc,
                *s_oskn_ptr, 0, oskn->header.length, is_x);
        omdl_index++;
    }

    set->ready = true;
    set->modern = true;
    set->big_endian = big_endian;
    set->is_x = is_x;
}

static void obj_set_modern_write_inner(obj_set* set, stream& s) {
    bool big_endian = s.big_endian;

    memory_stream s_mosd;
    s_mosd.open();
    s_mosd.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = set->is_x;

    obj_set_header osh = {};
    osh.last_obj_id = -1;

    uint32_t obj_num = set->obj_num;

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

    ee = { off, 1, 4, (uint32_t)set->tex_id_num };
    ee.append(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    off = (uint32_t)(set->tex_id_num * 4ULL);
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
        s_mosd.write_uint32_t_reverse_endianness(set->obj_data[i].id);
    s_mosd.align_write(0x10);

    osh.tex_id_data = s_mosd.get_position();
    for (uint32_t i = 0; i < set->tex_id_num; i++)
        s_mosd.write_uint32_t_reverse_endianness(set->tex_id_data[i]);
    s_mosd.align_write(0x10);

    int64_t* obj_name_data = force_malloc_s(int64_t, obj_num);
    for (uint32_t i = 0; i < obj_num; i++) {
        obj_name_data[i] = (int32_t)s_mosd.get_position();
        s_mosd.write_utf8_string_null_terminated(set->obj_data[i].name);
    }
    s_mosd.align_write(0x10);

    s_mosd.position_push(osh.obj_name_data, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_f2(obj_name_data[i], 0x20);
    else
        for (uint32_t i = 0; i < obj_num; i++)
            s_mosd.write_offset_x(obj_name_data[i]);
    s_mosd.position_pop();
    free_def(obj_name_data);

    s_mosd.position_push(0x00, SEEK_SET);
    if (!is_x) {
        s_mosd.write_uint32_t_reverse_endianness(0x05062501);
        s_mosd.write_int32_t_reverse_endianness(set->obj_num);
        s_mosd.write_int32_t_reverse_endianness(-1);
        s_mosd.write_offset_f2(osh.obj_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_skin_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_name_data, 0x20);
        s_mosd.write_offset_f2(osh.obj_id_data, 0x20);
        s_mosd.write_offset_f2(osh.tex_id_data, 0x20);
        s_mosd.write_int32_t_reverse_endianness(set->tex_id_num);
        s_mosd.write_uint32_t_reverse_endianness(set->reserved[0]);
        s_mosd.write_uint32_t_reverse_endianness(set->reserved[1]);
    }
    else {
        s_mosd.write_uint32_t_reverse_endianness(0x05062501);
        s_mosd.write_int32_t_reverse_endianness(set->obj_num);
        s_mosd.write_int32_t_reverse_endianness(-1);
        s_mosd.write_offset_x(osh.obj_data);
        s_mosd.write_offset_x(osh.obj_skin_data);
        s_mosd.write_offset_x(osh.obj_name_data);
        s_mosd.write_offset_x(osh.obj_id_data);
        s_mosd.write_offset_x(osh.tex_id_data);
        s_mosd.write_int32_t_reverse_endianness(set->tex_id_num);
        s_mosd.write_uint32_t_reverse_endianness(set->reserved[0]);
        s_mosd.write_uint32_t_reverse_endianness(set->reserved[1]);
    }
    s_mosd.position_pop();

    f2_struct st;
    for (uint32_t i = 0; i < obj_num; i++) {
        obj* obj = &set->obj_data[i];

        st.sub_structs.push_back({});
        f2_struct* omdl = &st.sub_structs.back();

        memory_stream s_omdl;
        s_omdl.open();
        s_omdl.big_endian = big_endian;

        if (obj->skin) {
            st.sub_structs.push_back({});
            f2_struct* oskn = &st.sub_structs.back();

            memory_stream s_oskn;
            s_oskn.open();
            s_oskn.big_endian = big_endian;

            obj_modern_write_skin(obj->skin, s_oskn, 0, is_x, oskn);

            s_oskn.align_write(0x10);
            s_oskn.copy(oskn->data);
            s_oskn.close();

            oskn->header.signature = reverse_endianness_uint32_t('OSKN');
            oskn->header.length = 0x20;
            oskn->header.use_big_endian = big_endian;
            oskn->header.use_section_size = true;
        }

        obj_modern_write_model(obj, s_omdl, 0, is_x, omdl);

        s_omdl.align_write(0x10);
        s_omdl.copy(omdl->data);
        s_omdl.close();

        omdl->header.signature = reverse_endianness_uint32_t('OMDL');
        omdl->header.length = 0x20;
        omdl->header.use_big_endian = big_endian;
        omdl->header.use_section_size = true;
    }

    s_mosd.align_write(0x10);
    s_mosd.copy(st.data);
    s_mosd.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSD');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, set->is_x);
}

static void obj_modern_read_index(obj_sub_mesh* sub_mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    bool tri_strip = sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP;
    uint32_t* index_array = alloc->allocate<uint32_t>(sub_mesh->num_index);
    uint32_t num_index = sub_mesh->num_index;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < num_index; i++) {
            uint8_t idx = s.read_uint8_t();
            index_array[i] = tri_strip && idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < num_index; i++) {
            uint16_t idx = s.read_uint16_t_reverse_endianness();
            index_array[i] = tri_strip && idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < num_index; i++)
            index_array[i] = s.read_uint32_t_reverse_endianness();
        break;
    }
    sub_mesh->index_array = index_array;
}

static void obj_modern_write_index(obj_sub_mesh* sub_mesh,
    stream& s, bool is_x, f2_struct* oidx) {
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
            e->vec.back().repeat_count += sub_mesh->num_index;
            add_enrs = false;
        }
    }

    if (add_enrs)
        if (type != ENRS_INVALID) {
            ee = { off, 1, (uint32_t)size, (uint32_t)sub_mesh->num_index };
            ee.append(0, 1, type);
            e->vec.push_back(ee);
        }
        else {
            ee = { off, 0, (uint32_t)size, (uint32_t)sub_mesh->num_index };
            e->vec.push_back(ee);
        }

    uint32_t* index_array = sub_mesh->index_array;
    uint32_t num_index = sub_mesh->num_index;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint8_t((uint8_t)index_array[i]);
        break;
    case OBJ_INDEX_U16:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint16_t_reverse_endianness((uint16_t)index_array[i]);
        break;
    case OBJ_INDEX_U32:
        for (uint32_t i = 0; i < num_index; i++)
            s.write_uint32_t_reverse_endianness(index_array[i]);
        break;
    }
    s.align_write(0x04);
}

static void obj_modern_read_model(obj* obj, prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;

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
        uint32_t num_mesh = obj->num_mesh;
        obj->mesh_array = alloc->allocate<obj_mesh>(num_mesh);
        for (uint32_t i = 0; i < num_mesh; i++) {
            s.set_position(base_offset + oh.mesh_array + mesh_size * i, SEEK_SET);
            obj_modern_read_model_mesh(&obj->mesh_array[i],
                alloc, s, base_offset,
                header_length, is_x, s_oidx, s_ovtx);
        }
    }

    if (oh.material_array > 0) {
        obj_material_texture_enrs_table_init();

        s.set_position(base_offset + oh.material_array, SEEK_SET);
        obj->material_array = alloc->allocate<obj_material_data>(obj->num_material);
        for (uint32_t i = 0; i < obj->num_material; i++) {
            obj_material_data mat_data;
            s.read(&mat_data, sizeof(obj_material_data));

            if (s.big_endian)
                obj_material_texture_enrs_table.apply(&mat_data);

            for (obj_material_texture_data& j : mat_data.material.texdata)
                mat4_transpose(&j.tex_coord_mat, &j.tex_coord_mat);
            obj->material_array[i] = mat_data;
        }
    }
}

static void obj_modern_write_model(obj* obj, stream& s,
    int64_t base_offset, bool is_x, f2_struct* omdl) {
    bool big_endian = s.big_endian;

    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    omdl->sub_structs.push_back({});
    omdl->sub_structs.push_back({});
    f2_struct* oidx = &omdl->sub_structs.end()[-2];
    f2_struct* ovtx = &omdl->sub_structs.back();

    memory_stream s_oidx;
    memory_stream s_ovtx;
    s_oidx.open();
    s_ovtx.open();
    s_oidx.big_endian = big_endian;
    s_ovtx.big_endian = big_endian;

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

    uint32_t num_mesh = obj->num_mesh;

    if (!is_x) {
        ee = { off, 1, 216, (uint32_t)num_mesh };
        ee.append(0, 32, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(num_mesh * 216ULL);
    }
    else {
        ee = { off, 5, 304, (uint32_t)num_mesh };
        ee.append(0, 6, ENRS_DWORD);
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 3, ENRS_DWORD);
        ee.append(4, 20, ENRS_QWORD);
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(num_mesh * 304ULL);
    }

    uint32_t total_sub_meshes = 0;
    for (uint32_t i = 0; i < num_mesh; i++)
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
        off = (uint32_t)(num_mesh * 112ULL);
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
        off = (uint32_t)(num_mesh * 128ULL);
    }

    int32_t total_bone_indices_count = 0;
    for (uint32_t i = 0; i < num_mesh; i++) {
        obj_mesh* mesh = &obj->mesh_array[i];
        uint32_t num_submesh = mesh->num_submesh;
        for (uint32_t j = 0; j < num_submesh; j++) {
            obj_sub_mesh* sub_mesh = &mesh->submesh_array[i];
            if (sub_mesh->bones_per_vertex == 4)
                total_bone_indices_count += sub_mesh->num_bone_index;
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

    if (num_mesh) {
        oh.mesh_array = s.get_position() - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, num_mesh);
        obj_sub_mesh_header** smhss = force_malloc_s(obj_sub_mesh_header*, num_mesh);
        for (uint32_t i = 0; i < num_mesh; i++) {
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

        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            mh->format = OBJ_VERTEX_FILE_MODERN_STORAGE;
            uint32_t num_submesh = mesh->num_submesh;
            if (num_submesh) {
                mh->submesh_array = s.get_position() - base_offset;
                for (uint32_t j = 0; j < num_submesh; j++) {
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

            obj_modern_write_vertex(mesh, s_ovtx, mh->vertex,
                &mh->vertex_format_index, &mh->num_vertex, &mh->size_vertex, ovtx);
        }

        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = 0;

            uint32_t num_submesh = mesh->num_submesh;
            if (num_submesh) {
                smhs = force_malloc_s(obj_sub_mesh_header, num_submesh);
                smhss[i] = smhs;
                for (uint32_t j = 0; j < num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    if (sub_mesh->bones_per_vertex == 4 && sub_mesh->num_bone_index) {
                        smh->bone_index_array_offset = s.get_position() - base_offset;
                        s.write(sub_mesh->bone_index_array, sub_mesh->num_bone_index * sizeof(uint16_t));
                    }

                    smh->index_array_offset = s_oidx.get_position();
                    obj_modern_write_index(sub_mesh, s_oidx, is_x, oidx);
                }
            }
        }
        s.align_write(is_x ? 0x10 : 0x04);

        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = smhss[i];

            uint32_t num_submesh = mesh->num_submesh;
            if (num_submesh) {
                s.position_push(base_offset + mh->submesh_array, SEEK_SET);
                for (uint32_t j = 0; j < num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    s.write_uint32_t_reverse_endianness(sub_mesh->flags);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_sphere.center.x);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_sphere.center.y);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_sphere.center.z);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_sphere.radius);
                    s.write_uint32_t_reverse_endianness(sub_mesh->material_index);
                    s.write(&sub_mesh->uv_index, 0x08);
                    s.write_int32_t_reverse_endianness(sub_mesh->num_bone_index);
                    s.write_offset(smh->bone_index_array_offset, 0x20, is_x);
                    s.write_uint32_t_reverse_endianness(sub_mesh->bones_per_vertex);
                    s.write_uint32_t_reverse_endianness(sub_mesh->primitive_type);
                    s.write_uint32_t_reverse_endianness(sub_mesh->index_format);
                    s.write_int32_t_reverse_endianness(sub_mesh->num_index);
                    s.write_offset(smh->index_array_offset, 0, is_x);
                    s.write_uint32_t_reverse_endianness(sub_mesh->attrib.w);
                    s.write(0x10);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.center.x);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.center.y);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.center.z);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.size.x);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.size.y);
                    s.write_float_t_reverse_endianness(sub_mesh->bounding_box.size.z);

                    uint32_t index_max = 0;
                    uint32_t* index = sub_mesh->index_array;
                    uint32_t num_index = sub_mesh->num_index;
                    for (uint32_t k = 0; k < num_index; k++, index++)
                        if (index_max < *index)
                            index_max = *index;

                    switch (sub_mesh->index_format) {
                    case OBJ_INDEX_U8:
                        s.write_uint32_t_reverse_endianness(index_max << 24);
                        break;
                    case OBJ_INDEX_U16:
                        s.write_uint32_t_reverse_endianness(index_max << 16);
                        break;
                    case OBJ_INDEX_U32:
                        s.write_uint32_t_reverse_endianness(index_max);
                        break;
                    }
                    s.write_uint32_t_reverse_endianness(sub_mesh->index_offset);

                    if (is_x)
                        s.write(0x04);
                }
                s.position_pop();
            }
            free_def(smhs);
        }

        s.position_push(base_offset + oh.mesh_array, SEEK_SET);
        for (uint32_t i = 0; i < num_mesh; i++) {
            obj_mesh* mesh = &obj->mesh_array[i];
            obj_mesh_header* mh = &mhs[i];

            s.write_uint32_t_reverse_endianness(mesh->flags);
            s.write_float_t_reverse_endianness(mesh->bounding_sphere.center.x);
            s.write_float_t_reverse_endianness(mesh->bounding_sphere.center.y);
            s.write_float_t_reverse_endianness(mesh->bounding_sphere.center.z);
            s.write_float_t_reverse_endianness(mesh->bounding_sphere.radius);
            s.write_int32_t_reverse_endianness(mesh->num_submesh);
            if (mh->submesh_array && !is_x)
                mh->submesh_array += 0x20;
            s.write_offset(mh->submesh_array, 0, is_x);
            s.write_uint32_t_reverse_endianness(mh->format);
            s.write_int32_t_reverse_endianness(mh->size_vertex);
            s.write_int32_t_reverse_endianness(mh->num_vertex);

            if (!is_x)
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_f2(mh->vertex[j], 0);
            else
                for (uint32_t j = 0; j < 20; j++)
                    s.write_offset_x(mh->vertex[j]);

            s.write_uint32_t_reverse_endianness(mesh->attrib.w);
            s.write_uint32_t_reverse_endianness(mh->vertex_format_index);
            s.write_uint32_t_reverse_endianness(mesh->reserved[0]);
            s.write_uint32_t_reverse_endianness(mesh->reserved[1]);
            s.write_uint32_t_reverse_endianness(mesh->reserved[2]);
            s.write_uint32_t_reverse_endianness(mesh->reserved[3]);
            s.write_uint32_t_reverse_endianness(mesh->reserved[4]);
            s.write_uint32_t_reverse_endianness(mesh->reserved[5]);
            s.write(&mesh->name, sizeof(mesh->name) - 1);
            s.write_char('\0');
        }
        s.position_pop();
        free_def(mhs);
        free_def(smhss);
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
        s.write_uint32_t_reverse_endianness(0x10000); // version
        s.write_uint32_t_reverse_endianness(0x00); // flags
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.x);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.y);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.z);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.radius);
        s.write_int32_t_reverse_endianness(obj->num_mesh);
        s.write_offset_f2(oh.mesh_array, 0x20);
        s.write_int32_t_reverse_endianness(obj->num_material);
        s.write_offset_f2(oh.material_array, 0x20);
        s.write_uint32_t_reverse_endianness(obj->reserved[0]);
        s.write_uint32_t_reverse_endianness(obj->reserved[1]);
        s.write_uint32_t_reverse_endianness(obj->reserved[2]);
        s.write_uint32_t_reverse_endianness(obj->reserved[3]);
        s.write_uint32_t_reverse_endianness(obj->reserved[4]);
        s.write_uint32_t_reverse_endianness(obj->reserved[5]);
        s.write_uint32_t_reverse_endianness(obj->reserved[6]);
        s.write_uint32_t_reverse_endianness(obj->reserved[7]);
        s.write_uint32_t_reverse_endianness(obj->reserved[8]);
        s.write_uint32_t_reverse_endianness(obj->reserved[9]);
    }
    else {
        s.write_uint32_t_reverse_endianness(0x10000); // version
        s.write_uint32_t_reverse_endianness(0x00); // flags
        s.write_int32_t_reverse_endianness(obj->num_mesh);
        s.write_int32_t_reverse_endianness(obj->num_material);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.x);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.y);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.center.z);
        s.write_float_t_reverse_endianness(obj->bounding_sphere.radius);
        s.write_offset_x(oh.mesh_array);
        s.write_offset_x(oh.material_array);
        s.write(0x10);
        s.write_uint8_t(obj->flags);
        s.write(0x07);
        s.write_uint32_t_reverse_endianness(obj->reserved[0]);
        s.write_uint32_t_reverse_endianness(obj->reserved[1]);
        s.write_uint32_t_reverse_endianness(obj->reserved[2]);
        s.write_uint32_t_reverse_endianness(obj->reserved[3]);
        s.write_uint32_t_reverse_endianness(obj->reserved[4]);
        s.write_uint32_t_reverse_endianness(obj->reserved[5]);
        s.write_uint32_t_reverse_endianness(obj->reserved[6]);
        s.write_uint32_t_reverse_endianness(obj->reserved[7]);
        s.write_uint32_t_reverse_endianness(obj->reserved[8]);
        s.write_uint32_t_reverse_endianness(obj->reserved[9]);
    }
    s.position_pop();

    omdl->enrs = e;
    omdl->pof = pof;

    s_oidx.align_write(0x10);
    s_oidx.copy(oidx->data);
    s_oidx.close();

    oidx->header.signature = reverse_endianness_uint32_t('OIDX');
    oidx->header.length = 0x20;
    oidx->header.use_big_endian = big_endian;
    oidx->header.use_section_size = true;

    s_ovtx.align_write(0x10);
    s_ovtx.copy(ovtx->data);
    s_ovtx.close();

    ovtx->header.signature = reverse_endianness_uint32_t('OVTX');
    ovtx->header.length = 0x20;
    ovtx->header.use_big_endian = big_endian;
    ovtx->header.use_section_size = true;
}

static void obj_modern_read_model_mesh(obj_mesh* mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx) {
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

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
        for (int64_t& i : mh.vertex)
            i = s.read_offset_f2(0);
    else
        for (int64_t& i : mh.vertex)
            i = s.read_offset_x();

    mesh->attrib.w = s.read_uint32_t_reverse_endianness();
    mh.vertex_format_index = s.read_uint32_t_reverse_endianness();
    mesh->reserved[0] = s.read_uint32_t_reverse_endianness();
    mesh->reserved[1] = s.read_uint32_t_reverse_endianness();
    mesh->reserved[2] = s.read_uint32_t_reverse_endianness();
    mesh->reserved[3] = s.read_uint32_t_reverse_endianness();
    mesh->reserved[4] = s.read_uint32_t_reverse_endianness();
    mesh->reserved[5] = s.read_uint32_t_reverse_endianness();
    s.read(&mesh->name, sizeof(mesh->name));
    mesh->name[sizeof(mesh->name) - 1] = 0;

    if (mh.submesh_array) {
        uint32_t num_submesh = mesh->num_submesh;
        mesh->submesh_array = alloc->allocate<obj_sub_mesh>(num_submesh);
        for (uint32_t i = 0; i < num_submesh; i++) {
            s.set_position(base_offset + mh.submesh_array + sub_mesh_size * i, SEEK_SET);
            obj_modern_read_model_sub_mesh(&mesh->submesh_array[i],
                alloc, s, base_offset, header_length, is_x, s_oidx);
        }
    }

    obj_modern_read_vertex(mesh, alloc, *s_ovtx, mh.vertex,
        mh.vertex_format_index, mh.num_vertex, mh.size_vertex);
    obj_vertex_generate_tangents(mesh);
}

static void obj_modern_read_model_sub_mesh(obj_sub_mesh* sub_mesh,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, int64_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx) {
    obj_sub_mesh_header smh = {};
    sub_mesh->flags = s.read_uint32_t_reverse_endianness();
    sub_mesh->bounding_sphere.center.x = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_sphere.center.y = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_sphere.center.z = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_sphere.radius = s.read_float_t_reverse_endianness();
    sub_mesh->material_index = s.read_uint32_t_reverse_endianness();
    s.read(&sub_mesh->uv_index, 0x08);
    sub_mesh->num_bone_index = s.read_int32_t_reverse_endianness();
    smh.bone_index_array_offset = s.read_offset(header_length, is_x);
    sub_mesh->bones_per_vertex = s.read_uint32_t_reverse_endianness();
    sub_mesh->primitive_type = (obj_primitive_type)s.read_uint32_t_reverse_endianness();
    sub_mesh->index_format = (obj_index_format)s.read_uint32_t_reverse_endianness();
    sub_mesh->num_index = s.read_int32_t_reverse_endianness();
    smh.index_array_offset = s.read_offset(0, is_x);
    sub_mesh->attrib.w = s.read_uint32_t_reverse_endianness();
    s.read(0, 0x10);
    sub_mesh->bounding_box.center.x = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_box.center.y = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_box.center.z = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_box.size.x = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_box.size.y = s.read_float_t_reverse_endianness();
    sub_mesh->bounding_box.size.z = s.read_float_t_reverse_endianness();
    s.read(0, 0x04);
    sub_mesh->index_offset = s.read_uint32_t_reverse_endianness();

    if (is_x)
        s.read(0, 0x04);

    if (sub_mesh->bones_per_vertex == 4 && smh.bone_index_array_offset) {
        sub_mesh->bone_index_array = alloc->allocate<uint16_t>(sub_mesh->num_bone_index);
        s.set_position(base_offset + smh.bone_index_array_offset, SEEK_SET);
        s.read(sub_mesh->bone_index_array, sub_mesh->num_bone_index * sizeof(uint16_t));
        if (s.big_endian) {
            uint16_t* bone_index_array = sub_mesh->bone_index_array;
            for (uint32_t k = 0; k < sub_mesh->num_bone_index; k++)
                bone_index_array[k] = reverse_endianness_uint16_t(bone_index_array[k]);
        }
    }

    s_oidx->set_position(smh.index_array_offset, SEEK_SET);
    obj_modern_read_index(sub_mesh, alloc, *s_oidx);
}

static obj_skin* obj_modern_read_skin(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset, uint32_t header_length, bool is_x) {
    obj_skin* sk = alloc->allocate<obj_skin>();
    s.set_position(base_offset, SEEK_SET);

    obj_skin_header sh = {};
    if (!is_x) {
        sh.bone_id_array_offset = s.read_offset_f2(header_length);
        sh.bone_matrix_array_offset = s.read_offset_f2(header_length);
        sh.bone_name_array_offset = s.read_offset_f2(header_length);
        sh.ex_data_offset = s.read_offset_f2(header_length);
        sk->num_bone = s.read_int32_t_reverse_endianness();
        sh.bone_parent_id_array_offset = s.read_offset_f2(header_length);
        s.read(0, 0x0C);
    }
    else {
        sh.bone_id_array_offset = s.read_offset_x();
        sh.bone_matrix_array_offset = s.read_offset_x();
        sh.bone_name_array_offset = s.read_offset_x();
        sh.ex_data_offset = s.read_offset_x();
        sk->num_bone = s.read_int32_t_reverse_endianness();
        sh.bone_parent_id_array_offset = s.read_offset_x();
        s.read(0, 0x18);
    }

    if (sh.bone_id_array_offset) {
        sk->bone_array = alloc->allocate<obj_skin_bone>(sk->num_bone);

        s.set_position(sh.bone_id_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < sk->num_bone; i++)
            sk->bone_array[i].id = s.read_uint32_t_reverse_endianness();

        if (sh.bone_matrix_array_offset) {
            s.set_position(sh.bone_matrix_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                mat4& mat = sk->bone_array[i].inv_bind_pose_mat;
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

        if (sh.bone_name_array_offset) {
            s.set_position(sh.bone_name_array_offset, SEEK_SET);
            if (!is_x)
                for (uint32_t i = 0; i < sk->num_bone; i++)
                    sk->bone_array[i].name = obj_read_utf8_string_null_terminated_offset(
                        alloc, s, s.read_offset_f2(header_length));
            else
                for (uint32_t i = 0; i < sk->num_bone; i++)
                    sk->bone_array[i].name = obj_read_utf8_string_null_terminated_offset(
                        alloc, s, s.read_offset_x());
        }

        if (sh.bone_parent_id_array_offset) {
            s.set_position(sh.bone_parent_id_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < sk->num_bone; i++)
                sk->bone_array[i].parent = s.read_uint32_t_reverse_endianness();
        }
    }

    if (sh.ex_data_offset)
        sk->ex_data = obj_modern_read_skin_ex_data(alloc,
            s, sh.ex_data_offset, header_length, is_x);
    return sk;
}

static void obj_modern_write_skin(obj_skin* sk, stream& s,
    int64_t base_offset, bool is_x, f2_struct* oskn) {
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
    if (sk->num_bone) {
        if (sk->num_bone % 4) {
            ee = { off, 1, 4, (uint32_t)sk->num_bone };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->num_bone * 4ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 4, (uint32_t)sk->num_bone };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->num_bone * 4ULL);
        }
        else {
            ee = { off, 1, 4, (uint32_t)(sk->num_bone * 2ULL) };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->num_bone * 2 * 4ULL);
        }
        off = align_val(off, 0x10);

        if (!is_x) {
            ee = { off, 1, 4, (uint32_t)sk->num_bone };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->num_bone * 4ULL);
        }
        else {
            ee = { off, 1, 8, (uint32_t)sk->num_bone };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(sk->num_bone * 8ULL);
        }
        off = align_val(off, 0x10);

        ee = { off, 1, 64, (uint32_t)sk->num_bone };
        ee.append(0, 16, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sk->num_bone * 64ULL);
        off = align_val(off, 0x10);
    }

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;
    std::vector<string_hash> bone_names;

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

    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

        const char** bone_name_array = ex->bone_name_array;
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];
            switch (block->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = block->cloth;
                obj_skin_strings_push_back_check(strings, cloth->mesh_name);
                obj_skin_strings_push_back_check(strings, cloth->backface_mesh_name);

                for (uint32_t k = 0; k < cloth->num_root; k++) {
                    obj_skin_block_cloth_root* root_array = &cloth->root_array[k];
                    for (uint32_t l = 0; l < 4; l++)
                        obj_skin_strings_push_back_check(strings, root_array->bone_weights[l].bone_name);
                }
                obj_skin_strings_push_back_check(strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = block->constraint;
                obj_skin_strings_push_back_check(strings, constraint->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, constraint->name_index);
                obj_skin_strings_push_back_check(strings, constraint->source_node_name);
                switch (constraint->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(strings, "Direction");
                    obj_skin_strings_push_back_check(strings, constraint->direction->up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(strings, "Position");
                    obj_skin_strings_push_back_check(strings, constraint->position->up_vector.name);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(strings, "Distance");
                    obj_skin_strings_push_back_check(strings, constraint->distance->up_vector.name);
                    break;
                }
                obj_skin_strings_push_back_check(strings, "CNS");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, constraint->name_index);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = block->expression;
                for (uint32_t j = 0; j < expression->num_expression; j++)
                    obj_skin_strings_push_back_check(strings, expression->expression_array[j]);
                obj_skin_strings_push_back_check(strings, expression->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, expression->name_index);
                obj_skin_strings_push_back_check(strings, "EXP");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, expression->name_index);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = block->motion;
                obj_skin_strings_push_back_check(strings, motion->node.parent_name);
                obj_skin_strings_push_back_check(strings, motion->name);
                for (uint32_t j = 0; j < motion->num_node; j++)
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_name_array, motion->node_array[j].name_index);
                obj_skin_strings_push_back_check(strings, "MOT");

                if (!is_x)
                    obj_skin_strings_push_back_check(bone_names, motion->name);
                for (uint32_t j = 0; j < motion->num_node; j++)
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_name_array, motion->node_array[j].name_index);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = block->osage;
                obj_skin_strings_push_back_check(strings, osage->node.parent_name);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, osage->name_index);
                obj_skin_strings_push_back_check_by_index(strings,
                    bone_name_array, osage->external_name_index);
                obj_skin_osage_node* osage_node = &ex->osage_node_array[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(strings,
                        bone_name_array, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_info = ex->osage_sibling_info_array;
                    for (uint32_t k = 0; k < ex->num_osage_sibling_info; k++) {
                        if (name_index == osage_sibling_info->name_index) {
                            obj_skin_strings_push_back_check_by_index(strings,
                                bone_name_array, osage_sibling_info->sibling_name_index);
                            break;
                        }
                        osage_sibling_info++;
                    }
                    osage_node++;
                }
                obj_skin_strings_push_back_check(strings, "OSG");

                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, osage->external_name_index);
                osage_node = &ex->osage_node_array[osage->start_index];
                for (uint32_t j = 0; j < osage->count; j++) {
                    obj_skin_strings_push_back_check_by_index(bone_names,
                        bone_name_array, osage_node->name_index);

                    int32_t name_index = osage->name_index;
                    obj_skin_osage_sibling_info* osage_sibling_info = ex->osage_sibling_info_array;
                    for (uint32_t k = 0; k < ex->num_osage_sibling_info; k++) {
                        if (name_index == osage_sibling_info->name_index) {
                            obj_skin_strings_push_back_check_by_index(bone_names,
                                bone_name_array, osage_sibling_info->sibling_name_index);
                            break;
                        }
                        osage_sibling_info++;
                    }
                    osage_node++;
                }
                obj_skin_strings_push_back_check_by_index(bone_names,
                    bone_name_array, osage->name_index);
            } break;
            }
        }

        exh.cloth_count = 0;
        exh.num_osage = 0;
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];
            if (block->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (block->type == OBJ_SKIN_BLOCK_EXPRESSION)
                expressions_count++;
            else if (block->type == OBJ_SKIN_BLOCK_MOTION) {
                motion_count++;
                motion_nodes_count += block->motion->num_node;
            }
            else if (block->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.num_osage++;
        }

        exh.num_bone_name = (int32_t)bone_names.size();
    }

    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

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

        if (ex->num_osage_node) {
            ee = { off, 1, 12, (uint32_t)ex->num_osage_node };
            ee.append(0, 2, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(ex->num_osage_node * 12ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 12, (uint32_t)ex->num_osage_sibling_info };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(ex->num_osage_sibling_info * 12ULL);
            off = align_val(off, 0x10);
        }

        if (exh.num_osage || exh.cloth_count) {
            int32_t count = exh.num_osage + exh.cloth_count;
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
            ee = { off, 1, 4, (uint32_t)exh.num_bone_name };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(exh.num_bone_name * 4ULL);
        }
        else {
            ee = { off, 1, 8, (uint32_t)exh.num_bone_name };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(exh.num_bone_name * 8ULL);
        }
        off = align_val(off, 0x10);

        if (ex->num_block > 0) {
            if (!is_x) {
                ee = { off, 1, 8, (uint32_t)ex->num_block };
                ee.append(0, 2, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(ex->num_block * 8ULL);
            }
            else {
                ee = { off, 1, 16, (uint32_t)ex->num_block };
                ee.append(0, 2, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(ex->num_block * 16ULL);
            }
            off = align_val(off, 0x10);

            if (exh.num_osage) {
                if (!is_x) {
                    ee = { off, 2, 76, (uint32_t)exh.num_osage };
                    ee.append(0, 14, ENRS_DWORD);
                    ee.append(4, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.num_osage * 76ULL);
                }
                else {
                    ee = { off, 4, 104, (uint32_t)exh.num_osage };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 9, ENRS_DWORD);
                    ee.append(4, 4, ENRS_DWORD);
                    ee.append(8, 1, ENRS_QWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(exh.num_osage * 104ULL);
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
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                if (cns_type == block->constraint->type) {
                    constraint_count++;
                    continue;
                }

                if (cns_type) {
                    ee.repeat_count = constraint_count;
                    e.vec.push_back(ee);
                    off = ee.size * ee.repeat_count;
                }

                cns_type = block->constraint->type;
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
                uint32_t num_mat = 0;
                uint32_t num_root = 0;
                uint32_t num_node = 0;
                uint32_t num_mesh_index = 0;
                uint32_t num_backface_mesh_index = 0;
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                        num_mat += block->cloth->mat_array ? block->cloth->num_mat : 0;
                        num_root += block->cloth->root_array ? block->cloth->num_root : 0;
                        num_node += block->cloth->node_array
                            ? block->cloth->num_root * (block->cloth->num_node - 1) : 0;
                        num_mesh_index += block->cloth->mesh_index_array
                            ? block->cloth->num_mesh_index + 1 : 0;
                        num_backface_mesh_index += block->cloth->backface_mesh_index_array
                            ? block->cloth->num_backface_mesh_index + 1 : 0;
                    }
                }

                if (num_mat) {
                    ee = { off, 1, sizeof(mat4), num_mat };
                    ee.append(0, 16, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(num_mat * sizeof(mat4));
                    off = align_val(off, 0x10);
                }

                if (num_root) {
                    if (!is_x) {
                        ee = { off, 1, 104, num_root };
                        ee.append(0, 26, ENRS_DWORD);
                        e.vec.push_back(ee);
                        off = (uint32_t)(num_root * 104ULL);
                    }
                    else {
                        ee = { off, 9, 136, num_root };
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
                        off = (uint32_t)(num_root * 136ULL);
                    }
                    off = align_val(off, 0x10);
                }

                if (num_node) {
                    ee = { off, 1, 44, num_node };
                    ee.append(0, 11, ENRS_DWORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(num_node * 44ULL);
                    off = align_val(off, 0x10);
                }

                if (num_mesh_index) {
                    ee = { off, 1, 2, num_mesh_index };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(num_mesh_index * 2ULL);
                    off = align_val(off, 0x10);
                }

                if (num_backface_mesh_index) {
                    ee = { off, 1, 2, num_backface_mesh_index };
                    ee.append(0, 1, ENRS_WORD);
                    e.vec.push_back(ee);
                    off = (uint32_t)(num_backface_mesh_index * 2ULL);
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

    if (sk->num_bone) {
        sh.bone_id_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++)
            s.write_int32_t_reverse_endianness(sk->bone_array[i].id);
        s.align_write(0x10);

        sh.bone_parent_id_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++)
            s.write_int32_t_reverse_endianness(sk->bone_array[i].parent);
        s.align_write(0x10);

        sh.bone_name_array_offset = s.get_position();
        if (!is_x)
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                obj_skin_strings_push_back_check(strings, sk->bone_array[i].name);
            }
        else
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                io_write_offset_x_pof_add(s, 0, &pof);
                obj_skin_strings_push_back_check(strings, sk->bone_array[i].name);
            }
        s.align_write(0x10);

        sh.bone_matrix_array_offset = s.get_position();
        for (uint32_t i = 0; i < sk->num_bone; i++) {
            mat4& mat = sk->bone_array[i].inv_bind_pose_mat;
            s.write_float_t_reverse_endianness(mat.row0.x);
            s.write_float_t_reverse_endianness(mat.row1.x);
            s.write_float_t_reverse_endianness(mat.row2.x);
            s.write_float_t_reverse_endianness(mat.row3.x);
            s.write_float_t_reverse_endianness(mat.row0.y);
            s.write_float_t_reverse_endianness(mat.row1.y);
            s.write_float_t_reverse_endianness(mat.row2.y);
            s.write_float_t_reverse_endianness(mat.row3.y);
            s.write_float_t_reverse_endianness(mat.row0.z);
            s.write_float_t_reverse_endianness(mat.row1.z);
            s.write_float_t_reverse_endianness(mat.row2.z);
            s.write_float_t_reverse_endianness(mat.row3.z);
            s.write_float_t_reverse_endianness(mat.row0.w);
            s.write_float_t_reverse_endianness(mat.row1.w);
            s.write_float_t_reverse_endianness(mat.row2.w);
            s.write_float_t_reverse_endianness(mat.row3.w);
        }
        s.align_write(0x10);
    }

    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

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

        if (ex->num_osage_node) {
            exh.osage_node_array_offset = s.get_position();
            for (uint32_t i = 0; i < ex->num_osage_node; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.align_write(0x10);

            exh.osage_sibling_info_array_offset = s.get_position();
            for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++) {
                s.write_int32_t(0);
                s.write_int32_t(0);
                s.write_int32_t(0);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.align_write(0x10);
        }

        if (exh.num_osage || exh.cloth_count) {
            exh.osage_name_array_offset = s.get_position();
            if (!is_x) {
                for (uint32_t i = 0; i < exh.num_osage; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                for (uint32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            }
            else {
                for (uint32_t i = 0; i < exh.num_osage; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);

                for (uint32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);
            }
            s.align_write(0x10);
        }

        exh.bone_name_array_offset = s.get_position();
        if (!is_x)
            for (string_hash& i : bone_names)
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        else
            for (string_hash& i : bone_names)
                io_write_offset_x_pof_add(s, 0, &pof);
        s.align_write(0x10);

        if (ex->num_block > 0) {
            exh.block_array_offset = s.get_position();
            if (!is_x) {
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
                s.write_offset_f2(0, 0x20);
                s.write_offset_f2(0, 0x20);
            }
            else {
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
                s.write_offset_x(0);
                s.write_offset_x(0);
            }
            s.align_write(0x10);

            bhs = force_malloc_s(obj_skin_block_header, ex->num_block);
            if (!is_x)
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    s.write(0x24);
                }
            else
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                        continue;

                    obj_skin_block_osage* osage = block->osage;

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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;

                    obj_skin_block_expression* expression = block->expression;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x04);

                    for (uint32_t j = 0; j < expression->num_expression && j < 9; j++)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                    for (uint32_t j = expression->num_expression; j < 9; j++)
                        s.write(0x04);
                }
            else
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                        continue;
                    obj_skin_block_expression* expression = block->expression;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x08);

                    for (uint32_t j = 0; j < expression->num_expression && j < 9; j++)
                        io_write_offset_x_pof_add(s, 0, &pof);

                    for (uint32_t j = expression->num_expression; j < 9; j++)
                        s.write(0x08);
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x10);

                    if (cloth->num_node)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    if (cloth->num_root) {
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    }
                    else
                        s.write(0x08);

                    if (cloth->num_mesh_index)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    if (cloth->num_backface_mesh_index)
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    else
                        s.write(0x04);

                    s.write(0x08);
                }
            else
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x10);

                    if (cloth->num_node)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    if (cloth->num_root) {
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                    }
                    else
                        s.write(0x10);

                    if (cloth->num_mesh_index)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    if (cloth->num_backface_mesh_index)
                        io_write_offset_x_pof_add(s, 0, &pof);
                    else
                        s.write(0x08);

                    s.write(0x10);
                }
            s.align_write(0x10);

            if (!is_x)
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x24);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    s.write(0x04);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    switch (block->constraint->type) {
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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                        continue;

                    bhs[i].block_offset = s.get_position();
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x28);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    s.write(0x08);
                    io_write_offset_x_pof_add(s, 0, &pof);
                    switch (block->constraint->type) {
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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = block->motion;
                    s.write(motion->num_node * sizeof(mat4));
                }
                s.align_write(0x10);

                motion_block_node_name_offset = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_MOTION)
                        continue;

                    obj_skin_block_motion* motion = block->motion;
                    s.write(motion->num_node * sizeof(uint32_t));
                }
                s.align_write(0x10);
            }

            if (exh.cloth_count) {
                cloth_mats = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    if (cloth->mat_array)
                        s.write(cloth->num_mat * sizeof(mat4));
                }
                s.align_write(0x10);

                cloth_root = s.get_position();
                if (!is_x)
                    for (uint32_t i = 0; i < ex->num_block; i++) {
                        obj_skin_block* block = &ex->block_array[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = block->cloth;
                        s.write((sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                            * cloth->num_root * (cloth->num_node - 1ULL));
                    }
                else
                    for (uint32_t i = 0; i < ex->num_block; i++) {
                        obj_skin_block* block = &ex->block_array[i];
                        if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                            continue;

                        obj_skin_block_cloth* cloth = block->cloth;
                        s.write((sizeof(int32_t) + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t)))
                            * cloth->num_root* (cloth->num_node - 1ULL));
                    }
                s.align_write(0x10);

                cloth_nodes = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write((11 * sizeof(int32_t)) * cloth->num_root * (cloth->num_node - 1ULL));
                }
                s.align_write(0x10);

                cloth_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write(sizeof(uint16_t) + cloth->num_mesh_index * sizeof(uint16_t));
                }
                s.align_write(0x10);

                cloth_backface_mesh_indices = s.get_position();
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
                    if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                        continue;

                    obj_skin_block_cloth* cloth = block->cloth;
                    s.write(sizeof(uint16_t) + cloth->num_backface_mesh_index * sizeof(uint16_t));
                }
                s.align_write(0x10);
            }
        }
    }

    if (sk->bone_array || sk->ex_data) {
        quicksort_string_hash(strings.data(), strings.size());
        string_offsets.reserve(strings.size());
        for (string_hash& i : strings) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.str);
        }
    }
    s.align_write(0x10);

    std::vector<string_hash> osage_names;
    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

        int64_t cls_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CLS");
        int64_t cns_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "CNS");
        int64_t exp_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "EXP");
        int64_t mot_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "MOT");
        int64_t osg_offset = obj_skin_strings_get_string_offset(strings, string_offsets, "OSG");

        if (ex->num_block > 0) {
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_cloth(block->cloth,
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

            const char** bone_name_array = ex->bone_name_array;
            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_constraint(block->constraint,
                    s, strings, string_offsets, bone_name_array, is_x, constraint_type_name_offsets);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_expression(block->expression,
                    s, strings, string_offsets, bone_name_array, is_x);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_motion(block->motion,
                    s, strings, string_offsets, is_x, bone_name_array,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                s.position_push(bhs[i].block_offset, SEEK_SET);
                obj_modern_write_skin_block_osage(block->osage,
                    s, strings, string_offsets, is_x);
                s.position_pop();
            }

            for (uint32_t i = 0; i < ex->num_block; i++) {
                obj_skin_block* block = &ex->block_array[i];
                if (block->type == OBJ_SKIN_BLOCK_CLOTH) {
                    obj_skin_block_cloth* cloth = block->cloth;
                    obj_skin_strings_push_back_check(osage_names, cloth->mesh_name);
                }
                else if (block->type == OBJ_SKIN_BLOCK_OSAGE) {
                    obj_skin_block_osage* osage = block->osage;
                    obj_skin_strings_push_back_check_by_index(osage_names,
                        bone_name_array, osage->external_name_index);
                }
            }

            s.position_push(exh.block_array_offset, SEEK_SET);
            if (!is_x) {
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
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
                for (uint32_t i = 0; i < ex->num_block; i++) {
                    obj_skin_block* block = &ex->block_array[i];
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
            free_def(bhs);
        }
    }

    if (sk->bone_array) {
        s.position_push(sh.bone_name_array_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, sk->bone_array[i].name);
                s.write_offset_f2(str_offset, 0x20);
            }
        else
            for (uint32_t i = 0; i < sk->num_bone; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(strings,
                    string_offsets, sk->bone_array[i].name);
                s.write_offset_x(str_offset);
            }
        s.position_pop();
    }

    if (sk->ex_data) {
        obj_skin_ex_data* ex = sk->ex_data;

        if (ex->num_block > 0) {
            s.position_push(exh.bone_name_array_offset, SEEK_SET);
            if (!is_x)
                for (string_hash& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_f2(str_offset, 0x20);
                }
            else
                for (string_hash& i : bone_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_x(str_offset);
                }
            s.position_pop();

            s.position_push(exh.osage_node_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->num_osage_node; i++) {
                obj_skin_osage_node* osage_node = &ex->osage_node_array[i];
                s.write_uint32_t_reverse_endianness(osage_node->name_index);
                s.write_float_t_reverse_endianness(osage_node->length);
                s.write_uint32_t_reverse_endianness(0);
            }
            s.position_pop();

            s.position_push(exh.osage_sibling_info_array_offset, SEEK_SET);
            for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++) {
                obj_skin_osage_sibling_info* osage_sibling_info = &ex->osage_sibling_info_array[i];
                s.write_uint32_t_reverse_endianness(osage_sibling_info->name_index);
                s.write_uint32_t_reverse_endianness(osage_sibling_info->sibling_name_index);
                s.write_float_t_reverse_endianness(osage_sibling_info->max_distance);
            }
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.write_int32_t(0);
            s.position_pop();

            exh.num_osage = (int32_t)osage_names.size();
            exh.num_osage -= exh.cloth_count;
            s.position_push(exh.osage_name_array_offset, SEEK_SET);
            if (!is_x)
                for (string_hash& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_f2(str_offset, 0x20);
                }
            else
                for (string_hash& i : osage_names) {
                    size_t str_offset = obj_skin_strings_get_string_offset(strings,
                        string_offsets, i.c_str());
                    s.write_offset_x(str_offset);
                }
            s.position_pop();
        }

        s.position_push(sh.ex_data_offset, SEEK_SET);
        if (!is_x) {
            s.write_int32_t_reverse_endianness(exh.num_osage);
            s.write_int32_t_reverse_endianness(ex->num_osage_node);
            s.write(0x04);
            s.write_offset_f2(exh.osage_node_array_offset, 0x20);
            s.write_offset_f2(exh.osage_name_array_offset, 0x20);
            s.write_offset_f2(exh.block_array_offset, 0x20);
            s.write_int32_t_reverse_endianness(exh.num_bone_name);
            s.write_offset_f2(exh.bone_name_array_offset, 0x20);
            s.write_offset_f2(exh.osage_sibling_info_array_offset, 0x20);
            s.write_int32_t_reverse_endianness(exh.cloth_count);
            s.write_offset_f2(ex->reserved[0], 0x20);
            s.write_offset_f2(ex->reserved[1], 0x20);
            s.write_offset_f2(ex->reserved[2], 0x20);
            s.write_offset_f2(ex->reserved[3], 0x20);
            s.write_offset_f2(ex->reserved[4], 0x20);
            s.write_offset_f2(ex->reserved[5], 0x20);
            s.write_offset_f2(ex->reserved[6], 0x20);
        }
        else {
            s.write_int32_t_reverse_endianness(exh.num_osage);
            s.write_int32_t_reverse_endianness(ex->num_osage_node);
            s.write(0x08);
            s.write_offset_x(exh.osage_node_array_offset);
            s.write_offset_x(exh.osage_name_array_offset);
            s.write_offset_x(exh.block_array_offset);
            s.write_int32_t_reverse_endianness(exh.num_bone_name);
            s.write_offset_x(exh.bone_name_array_offset);
            s.write_offset_x(exh.osage_sibling_info_array_offset);
            s.write_int32_t_reverse_endianness(exh.cloth_count);
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
        s.write_offset_f2(sh.bone_id_array_offset, 0x20);
        s.write_offset_f2(sh.bone_matrix_array_offset, 0x20);
        s.write_offset_f2(sh.bone_name_array_offset, 0x20);
        s.write_offset_f2(sh.ex_data_offset, 0x20);
        s.write_int32_t_reverse_endianness(sk->num_bone);
        s.write_offset_f2(sh.bone_parent_id_array_offset, 0x20);
        s.write(0x0C);
    }
    else {
        s.write_offset_x(sh.bone_id_array_offset);
        s.write_offset_x(sh.bone_matrix_array_offset);
        s.write_offset_x(sh.bone_name_array_offset);
        s.write_offset_x(sh.ex_data_offset);
        s.write_int32_t_reverse_endianness(sk->num_bone);
        s.write_offset_x(sh.bone_parent_id_array_offset);
        s.write(0x18);
    }
    s.position_pop();

    oskn->enrs = e;
    oskn->pof = pof;
}

static obj_skin_ex_data* obj_modern_read_skin_ex_data(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t base_offset, uint32_t header_length, bool is_x) {
    obj_skin_ex_data* ex = alloc->allocate<obj_skin_ex_data>();
    s.set_position(base_offset, SEEK_SET);

    obj_skin_ex_data_header exh = {};
    if (!is_x) {
        exh.num_osage = s.read_int32_t_reverse_endianness();
        ex->num_osage_node = s.read_int32_t_reverse_endianness();
        s.read(0, 0x04);
        exh.osage_node_array_offset = s.read_offset_f2(header_length);
        exh.osage_name_array_offset = s.read_offset_f2(header_length);
        exh.block_array_offset = s.read_offset_f2(header_length);
        exh.num_bone_name = s.read_int32_t_reverse_endianness();
        exh.bone_name_array_offset = s.read_offset_f2(header_length);
        exh.osage_sibling_info_array_offset = s.read_offset_f2(header_length);
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
        exh.num_osage = s.read_int32_t_reverse_endianness();
        ex->num_osage_node = s.read_int32_t_reverse_endianness();
        s.read(0, 0x08);
        exh.osage_node_array_offset = s.read_offset_x();
        exh.osage_name_array_offset = s.read_offset_x();
        exh.block_array_offset = s.read_offset_x();
        exh.num_bone_name = s.read_int32_t_reverse_endianness();
        exh.bone_name_array_offset = s.read_offset_x();
        exh.osage_sibling_info_array_offset = s.read_offset_x();
        exh.cloth_count = s.read_int32_t_reverse_endianness();
        ex->reserved[0] = s.read_offset_x();
        ex->reserved[1] = s.read_offset_x();
        ex->reserved[2] = s.read_offset_x();
        ex->reserved[3] = s.read_offset_x();
        ex->reserved[4] = s.read_offset_x();
        ex->reserved[5] = s.read_offset_x();
        ex->reserved[6] = s.read_offset_x();
    }

    ex->bone_name_array = 0;
    ex->num_bone_name = 0;
    if (!exh.bone_name_array_offset) {
        return ex;
    }

    uint32_t num_bone_name = exh.num_bone_name;

    const char** bone_name_array = alloc->allocate<const char*>(num_bone_name);
    ex->bone_name_array = bone_name_array;
    ex->num_bone_name = num_bone_name;

    s.set_position(exh.bone_name_array_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < num_bone_name; i++) {
            int64_t string_offset = s.read_offset_f2(header_length);
            if (string_offset)
                bone_name_array[i] = obj_read_utf8_string_null_terminated_offset(alloc, s, string_offset);
            else
                bone_name_array[i] = 0;
        }
    else
        for (uint32_t i = 0; i < num_bone_name; i++) {
            int64_t string_offset = s.read_offset_x();
            if (string_offset)
                bone_name_array[i] = obj_read_utf8_string_null_terminated_offset(alloc, s, string_offset);
            else
                bone_name_array[i] = 0;
        }

    if (exh.osage_node_array_offset) {
        ex->osage_node_array = alloc->allocate<obj_skin_osage_node>(ex->num_osage_node);

        s.set_position(exh.osage_node_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < ex->num_osage_node; i++) {
            obj_skin_osage_node* osage_node = &ex->osage_node_array[i];

            osage_node->name_index = s.read_uint32_t_reverse_endianness();
            osage_node->length = s.read_float_t_reverse_endianness();
            s.read(0, 0x04);
        }
    }

    if (exh.block_array_offset) {
        ex->num_block = 0;
        s.set_position(exh.block_array_offset, SEEK_SET);
        if (!is_x)
            while (s.read_int32_t()) {
                s.read(0, 0x04);
                ex->num_block++;
            }
        else
            while (s.read_int64_t()) {
                s.read(0, 0x08);
                ex->num_block++;
            }

        obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, ex->num_block);
        s.set_position(exh.block_array_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t i = 0; i < ex->num_block; i++) {
                bhs[i].block_signature_offset = s.read_offset_f2(header_length);
                bhs[i].block_offset = s.read_offset_f2(header_length);
            }
        else
            for (uint32_t i = 0; i < ex->num_block; i++) {
                bhs[i].block_signature_offset = s.read_offset_x();
                bhs[i].block_offset = s.read_offset_x();
            }

        ex->block_array = alloc->allocate<obj_skin_block>(ex->num_block);
        for (uint32_t i = 0; i < ex->num_block; i++) {
            obj_skin_block* block = &ex->block_array[i];

            std::string block_signature = s.read_string_null_terminated_offset(
                bhs[i].block_signature_offset);
            if (block_signature.size() != 3)
                continue;

            uint32_t signature = load_reverse_endianness_uint32_t(block_signature.c_str());
            switch (signature) {
            case 'CLS\0':
                block->type = OBJ_SKIN_BLOCK_CLOTH;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->cloth = obj_modern_read_skin_block_cloth(
                    alloc, s, header_length, bone_name_array, is_x);
                break;
            case 'CNS\0':
                block->type = OBJ_SKIN_BLOCK_CONSTRAINT;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->constraint = obj_modern_read_skin_block_constraint(
                    alloc, s, header_length, bone_name_array, is_x);
                break;
            case 'EXP\0':
                block->type = OBJ_SKIN_BLOCK_EXPRESSION;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->expression = obj_modern_read_skin_block_expression(
                    alloc, s, header_length, bone_name_array, is_x);
                break;
            case 'MOT\0':
                block->type = OBJ_SKIN_BLOCK_MOTION;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->motion = obj_modern_read_skin_block_motion(
                    alloc, s, header_length, bone_name_array, is_x);
                break;
            case 'OSG\0':
                block->type = OBJ_SKIN_BLOCK_OSAGE;
                s.set_position(bhs[i].block_offset, SEEK_SET);
                block->osage = obj_modern_read_skin_block_osage(
                    alloc, s, header_length, bone_name_array, is_x);
                break;
            default:
                break;
            }
        }
        free_def(bhs);
    }

    if (exh.osage_sibling_info_array_offset) {
        ex->num_osage_sibling_info = 0;
        s.set_position(exh.osage_sibling_info_array_offset, SEEK_SET);
        while (s.read_uint32_t()) {
            s.read(0, 0x08);
            ex->num_osage_sibling_info++;
        }

        obj_skin_osage_sibling_info* osi = alloc->allocate<
            obj_skin_osage_sibling_info>(ex->num_osage_sibling_info);
        ex->osage_sibling_info_array = osi;
        s.set_position(exh.osage_sibling_info_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < ex->num_osage_sibling_info; i++, osi++) {
            osi->name_index = s.read_uint32_t_reverse_endianness();
            osi->sibling_name_index = s.read_uint32_t_reverse_endianness();
            osi->max_distance = s.read_float_t_reverse_endianness();
        }
    }
    return ex;
}

static obj_skin_block_cloth* obj_modern_read_skin_block_cloth(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    obj_skin_block_cloth* cls = alloc->allocate<obj_skin_block_cloth>();
    int64_t mesh_name_offset = s.read_offset(header_length, is_x);
    int64_t backface_mesh_name_offset = s.read_offset(header_length, is_x);
    cls->field_8 = s.read_int32_t_reverse_endianness();
    cls->num_root = s.read_int32_t_reverse_endianness();
    cls->num_node = s.read_int32_t_reverse_endianness();
    cls->field_14 = s.read_int32_t_reverse_endianness();
    int64_t mat_array_offset = s.read_offset(header_length, is_x);
    int64_t root_array_offset = s.read_offset(header_length, is_x);
    int64_t node_array_offset = s.read_offset(header_length, is_x);
    int64_t mesh_index_array_offset = s.read_offset(header_length, is_x);
    int64_t backface_mesh_index_array_offset = s.read_offset(header_length, is_x);
    int64_t skp_root_offset = s.read_uint32_t_reverse_endianness();
    cls->reserved = s.read_uint32_t_reverse_endianness();

    cls->mesh_name = obj_read_utf8_string_null_terminated_offset(alloc, s, mesh_name_offset);
    cls->backface_mesh_name = obj_read_utf8_string_null_terminated_offset(alloc, s, backface_mesh_name_offset);

    if (mat_array_offset) {
        int32_t max_matrix_index = -1;

        s.position_push(root_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        for (uint32_t i = 0; i < num_root; i++) {
            s.read(0x28);
            for (uint32_t j = 0; j < 4; j++) {
                uint32_t name_offset = s.read_uint32_t();
                int32_t matrix_index = s.read_int32_t();
                if (name_offset&& max_matrix_index < matrix_index)
                    max_matrix_index = matrix_index;
            }
        }
        s.position_pop();

        uint32_t num_mat = max_matrix_index > -1 ? max_matrix_index + 1 : 0;
        cls->num_mat = num_mat;
        cls->mat_array = alloc->allocate<mat4>(num_mat);
        s.position_push(mat_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < num_mat; i++) {
            mat4& mat = cls->mat_array[i];
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

    if (root_array_offset) {
        s.position_push(root_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        obj_skin_block_cloth_root* root_array = alloc->allocate<obj_skin_block_cloth_root>(num_root);
        cls->root_array = root_array;
        for (uint32_t i = 0; i < num_root; i++)
            obj_modern_read_skin_block_cloth_root(&root_array[i],
                alloc, s, header_length, str, is_x);
        s.position_pop();
    }

    if (node_array_offset) {
        s.position_push(node_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        uint32_t num_node = cls->num_node;
        obj_skin_block_cloth_node* node_array = alloc->allocate<obj_skin_block_cloth_node>(
            num_root * (cls->num_node - 1ULL));
        cls->node_array = node_array;
        for (uint32_t i = 0; i < num_node - 1; i++)
            for (uint32_t j = 0; j < num_root; j++) {
                obj_skin_block_cloth_node* f = &node_array[(size_t)i * num_root + j];
                f->flags = s.read_uint32_t_reverse_endianness();
                f->trans.x = s.read_float_t_reverse_endianness();
                f->trans.y = s.read_float_t_reverse_endianness();
                f->trans.z = s.read_float_t_reverse_endianness();
                f->trans_diff.x = s.read_float_t_reverse_endianness();
                f->trans_diff.y = s.read_float_t_reverse_endianness();
                f->trans_diff.z = s.read_float_t_reverse_endianness();
                f->dist_top = s.read_float_t_reverse_endianness();
                f->dist_bottom = s.read_float_t_reverse_endianness();
                f->dist_right = s.read_float_t_reverse_endianness();
                f->dist_left = s.read_float_t_reverse_endianness();
            }
        s.position_pop();
    }

    if (mesh_index_array_offset) {
        s.position_push(mesh_index_array_offset, SEEK_SET);
        cls->num_mesh_index = s.read_uint16_t_reverse_endianness();
        cls->mesh_index_array = alloc->allocate<uint16_t>(cls->num_mesh_index);
        s.read(cls->mesh_index_array, cls->num_mesh_index * sizeof(uint16_t));
        if (s.big_endian) {
            uint16_t* mesh_index_array = cls->mesh_index_array;
            for (int32_t i = cls->num_mesh_index; i > 0; i--, mesh_index_array++)
                *mesh_index_array = reverse_endianness_uint16_t(*mesh_index_array);
        }
        s.position_pop();
    }

    if (backface_mesh_index_array_offset) {
        s.position_push(backface_mesh_index_array_offset, SEEK_SET);
        cls->num_backface_mesh_index = s.read_uint16_t_reverse_endianness();
        cls->backface_mesh_index_array = alloc->allocate<uint16_t>(cls->num_backface_mesh_index);
        s.read(cls->backface_mesh_index_array, cls->num_backface_mesh_index * sizeof(uint16_t));
        if (s.big_endian) {
            uint16_t* backface_mesh_index_array = cls->backface_mesh_index_array;
            for (int32_t i = cls->num_mesh_index; i > 0; i--, backface_mesh_index_array++)
                *backface_mesh_index_array = reverse_endianness_uint16_t(*backface_mesh_index_array);
        }
        s.position_pop();
    }
    return cls;
}

static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* cls,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    int64_t* mat_array_offset, int64_t* root_array_offset, int64_t* node_array_offset,
    int64_t* mesh_index_array_offset, int64_t* backface_mesh_index_array_offset) {
    int64_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cls->mesh_name);
    int64_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cls->backface_mesh_name);

    if (!is_x) {
        s.write_offset_f2(mesh_name_offset, 0x20);
        s.write_offset_f2(backface_mesh_name_offset, 0x20);
        s.write_int32_t_reverse_endianness(cls->field_8);
        s.write_int32_t_reverse_endianness(cls->num_root);
        s.write_int32_t_reverse_endianness(cls->num_node);
        s.write_int32_t_reverse_endianness(cls->field_14);
        s.write_offset_f2(cls->num_node ? *mat_array_offset : 0, 0x20);
        s.write_offset_f2(cls->root_array ? *root_array_offset : 0, 0x20);
        s.write_offset_f2(cls->node_array ? *node_array_offset : 0, 0x20);
        s.write_offset_f2(cls->mesh_index_array ? *mesh_index_array_offset : 0, 0x20);
        s.write_offset_f2(cls->backface_mesh_index_array ? *backface_mesh_index_array_offset : 0, 0x20);
        s.write_uint32_t_reverse_endianness(0);
        s.write_uint32_t_reverse_endianness(cls->reserved);
    }
    else {
        s.write_offset_x(mesh_name_offset);
        s.write_offset_x(backface_mesh_name_offset);
        s.write_int32_t_reverse_endianness(cls->field_8);
        s.write_int32_t_reverse_endianness(cls->num_root);
        s.write_int32_t_reverse_endianness(cls->num_node);
        s.write_int32_t_reverse_endianness(cls->field_14);
        s.write_offset_x(cls->num_node ? *mat_array_offset : 0);
        s.write_offset_x(cls->root_array ? *root_array_offset : 0);
        s.write_offset_x(cls->node_array ? *node_array_offset : 0);
        s.write_offset_x(cls->mesh_index_array ? *mesh_index_array_offset : 0);
        s.write_offset_x(cls->backface_mesh_index_array ? *backface_mesh_index_array_offset : 0);
        s.write_uint32_t_reverse_endianness(0);
        s.write_uint32_t_reverse_endianness(cls->reserved);
    }

    if (cls->mat_array) {
        s.position_push(*mat_array_offset, SEEK_SET);
        uint32_t num_mat = cls->num_mat;
        for (uint32_t i = 0; i < num_mat; i++) {
            mat4& mat = cls->mat_array[i];
            s.write_float_t_reverse_endianness(mat.row0.x);
            s.write_float_t_reverse_endianness(mat.row1.x);
            s.write_float_t_reverse_endianness(mat.row2.x);
            s.write_float_t_reverse_endianness(mat.row3.x);
            s.write_float_t_reverse_endianness(mat.row0.y);
            s.write_float_t_reverse_endianness(mat.row1.y);
            s.write_float_t_reverse_endianness(mat.row2.y);
            s.write_float_t_reverse_endianness(mat.row3.y);
            s.write_float_t_reverse_endianness(mat.row0.z);
            s.write_float_t_reverse_endianness(mat.row1.z);
            s.write_float_t_reverse_endianness(mat.row2.z);
            s.write_float_t_reverse_endianness(mat.row3.z);
            s.write_float_t_reverse_endianness(mat.row0.w);
            s.write_float_t_reverse_endianness(mat.row1.w);
            s.write_float_t_reverse_endianness(mat.row2.w);
            s.write_float_t_reverse_endianness(mat.row3.w);
            *mat_array_offset += sizeof(mat4);
        }
        s.position_pop();
    }

    if (cls->root_array) {
        s.position_push(*root_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        uint32_t num_node = cls->num_node;
        obj_skin_block_cloth_root* root_array = cls->root_array;
        for (uint32_t i = 0; i < num_root; i++)
            obj_modern_write_skin_block_cloth_root(&root_array[i],
                s, strings, string_offsets, is_x);
        s.position_pop();
        if (!is_x)
            *root_array_offset += (sizeof(int32_t) + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t)))
                * cls->num_root * (cls->num_node - 1ULL);
        else
            *root_array_offset += (sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                * cls->num_root * (cls->num_node - 1ULL);
    }

    if (cls->node_array) {
        s.position_push(*node_array_offset, SEEK_SET);
        uint32_t num_root = cls->num_root;
        uint32_t num_node = cls->num_node;
        obj_skin_block_cloth_node* node_array = cls->node_array;
        for (uint32_t i = 0; i < num_node - 1; i++)
            for (uint32_t j = 0; j < num_root; j++) {
                obj_skin_block_cloth_node* f = &node_array[(size_t)i * num_root + j];
                s.write_uint32_t_reverse_endianness(f->flags);
                s.write_float_t_reverse_endianness(f->trans.x);
                s.write_float_t_reverse_endianness(f->trans.y);
                s.write_float_t_reverse_endianness(f->trans.z);
                s.write_float_t_reverse_endianness(f->trans_diff.x);
                s.write_float_t_reverse_endianness(f->trans_diff.y);
                s.write_float_t_reverse_endianness(f->trans_diff.z);
                s.write_float_t_reverse_endianness(f->dist_top);
                s.write_float_t_reverse_endianness(f->dist_bottom);
                s.write_float_t_reverse_endianness(f->dist_right);
                s.write_float_t_reverse_endianness(f->dist_left);
            }
        s.position_pop();
        *node_array_offset += (11 * sizeof(int32_t)) * cls->num_root * (cls->num_node - 1ULL);
    }

    if (cls->mesh_index_array) {
        s.position_push(*mesh_index_array_offset, SEEK_SET);
        s.write_uint16_t_reverse_endianness((uint16_t)cls->num_mesh_index);
        uint16_t* mesh_index_array = cls->mesh_index_array;
        for (uint32_t i = cls->num_mesh_index; i > 0; i--, mesh_index_array++)
            s.write_uint16_t_reverse_endianness(*mesh_index_array);
        s.position_pop();
        *mesh_index_array_offset += sizeof(uint16_t) + cls->num_mesh_index * sizeof(uint16_t);
    }

    if (cls->backface_mesh_index_array) {
        s.position_push(*backface_mesh_index_array_offset, SEEK_SET);
        s.write_uint16_t_reverse_endianness((uint16_t)cls->num_backface_mesh_index);
        uint16_t* backface_mesh_index_array = cls->backface_mesh_index_array;
        for (uint32_t i = cls->num_backface_mesh_index; i > 0; i--, backface_mesh_index_array++)
            s.write_uint16_t_reverse_endianness(*backface_mesh_index_array);
        s.position_pop();
        *backface_mesh_index_array_offset += sizeof(uint16_t) + cls->num_backface_mesh_index * sizeof(uint16_t);
    }
}

static void obj_modern_read_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    cloth_root->trans.x = s.read_float_t();
    cloth_root->trans.y = s.read_float_t();
    cloth_root->trans.z = s.read_float_t();
    cloth_root->normal.x = s.read_float_t();
    cloth_root->normal.y = s.read_float_t();
    cloth_root->normal.z = s.read_float_t();
    cloth_root->field_18 = s.read_float_t();
    cloth_root->field_1C = s.read_int32_t();
    cloth_root->field_20 = s.read_int32_t();
    cloth_root->field_24 = s.read_int32_t();

    for (uint32_t i = 0; i < 4; i++)
        obj_modern_read_skin_block_cloth_root_bone_weight(&cloth_root->bone_weights[i],
            alloc, s, header_length, str, is_x);
}

static void obj_modern_write_skin_block_cloth_root(obj_skin_block_cloth_root* cloth_root,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    s.write_float_t(cloth_root->trans.x);
    s.write_float_t(cloth_root->trans.y);
    s.write_float_t(cloth_root->trans.z);
    s.write_float_t(cloth_root->normal.x);
    s.write_float_t(cloth_root->normal.y);
    s.write_float_t(cloth_root->normal.z);
    s.write_float_t(cloth_root->field_18);
    s.write_int32_t(cloth_root->field_1C);
    s.write_int32_t(cloth_root->field_20);
    s.write_int32_t(cloth_root->field_24);

    for (uint32_t j = 0; j < 4; j++)
        obj_modern_write_skin_block_cloth_root_bone_weight(&cloth_root->bone_weights[j],
            s, strings, string_offsets, is_x);
}

static void obj_modern_read_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    int64_t bone_name_offset = s.read_offset(header_length, is_x);
    bone_weight->bone_name = obj_read_utf8_string_null_terminated_offset(alloc, s, bone_name_offset);
    bone_weight->weight = s.read_float_t_reverse_endianness();
    bone_weight->matrix_index = s.read_uint32_t_reverse_endianness();
    bone_weight->reserved = s.read_uint32_t_reverse_endianness();
    if (is_x)
        s.read(0, 0x04);
}

static void obj_modern_write_skin_block_cloth_root_bone_weight(obj_skin_block_cloth_root_bone_weight* bone_weight,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, bone_weight->bone_name);
    s.write_offset(bone_name_offset, 0x20, is_x);
    s.write_float_t_reverse_endianness(bone_weight->weight);
    s.write_uint32_t_reverse_endianness(bone_weight->matrix_index);
    s.write_uint32_t_reverse_endianness(bone_weight->reserved);
    if (is_x)
        s.write(0x04);
}

static obj_skin_block_constraint* obj_modern_read_skin_block_constraint(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    obj_skin_block_constraint* cns = alloc->allocate<obj_skin_block_constraint>();
    obj_modern_read_skin_block_node(&cns->node, alloc, s, header_length, str, is_x);

    int64_t type_offset = s.read_offset(header_length, is_x);
    char* type = s.read_utf8_string_null_terminated_offset(type_offset);

    int64_t name_offset = s.read_offset(header_length, is_x);
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    uint32_t name_index = 0;
    for (const char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            cns->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free_def(name);

    cns->coupling = (obj_skin_block_constraint_coupling)s.read_int32_t_reverse_endianness();

    int64_t source_node_name_offset = s.read_offset(header_length, is_x);
    cns->source_node_name = obj_read_utf8_string_null_terminated_offset(alloc, s, source_node_name_offset);

    if (!str_utils_compare(type, "Orientation")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;

        obj_skin_block_constraint_orientation* orientation
            = alloc->allocate<obj_skin_block_constraint_orientation>();
        orientation->offset.x = s.read_float_t_reverse_endianness();
        orientation->offset.y = s.read_float_t_reverse_endianness();
        orientation->offset.z = s.read_float_t_reverse_endianness();
        cns->orientation = orientation;
    }
    else if (!str_utils_compare(type, "Direction")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;

        obj_skin_block_constraint_direction* direction
            = alloc->allocate<obj_skin_block_constraint_direction>();
        obj_modern_read_skin_block_constraint_up_vector(&direction->up_vector,
            alloc, s, header_length, str, is_x);
        direction->align_axis.x = s.read_float_t_reverse_endianness();
        direction->align_axis.y = s.read_float_t_reverse_endianness();
        direction->align_axis.z = s.read_float_t_reverse_endianness();
        direction->target_offset.x = s.read_float_t_reverse_endianness();
        direction->target_offset.y = s.read_float_t_reverse_endianness();
        direction->target_offset.z = s.read_float_t_reverse_endianness();
        cns->direction = direction;
    }
    else if (!str_utils_compare(type, "Position")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;

        obj_skin_block_constraint_position* position
            = alloc->allocate<obj_skin_block_constraint_position>();
        obj_modern_read_skin_block_constraint_up_vector(&position->up_vector,
            alloc, s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&position->constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&position->constraining_object,
            s, header_length, str, is_x);
        cns->position = position;
    }
    else if (!str_utils_compare(type, "Distance")) {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;

        obj_skin_block_constraint_distance* distance
            = alloc->allocate<obj_skin_block_constraint_distance>();
        obj_modern_read_skin_block_constraint_up_vector(&distance->up_vector,
            alloc, s, header_length, str, is_x);
        distance->distance = s.read_float_t_reverse_endianness();
        obj_modern_read_skin_block_constraint_attach_point(&distance->constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&distance->constraining_object,
            s, header_length, str, is_x);
        cns->distance = distance;
    }
    else {
        cns->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
        cns->data = 0;
    }
    free_def(type);
    return cns;
}

static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* cns,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, const char** bone_name_array,
    bool is_x, int64_t* offsets) {
    obj_modern_write_skin_block_node(&cns->node, s, strings, string_offsets, is_x);

    int64_t type_offset = 0;
    switch (cns->type) {
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
        string_offsets, bone_name_array, cns->name_index);
    s.write_offset(name_offset, 0x20, is_x);

    s.write_int32_t_reverse_endianness(cns->coupling);

    int64_t source_node_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, cns->source_node_name);
    s.write_offset(source_node_name_offset, 0x20, is_x);

    switch (cns->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        s.write_float_t_reverse_endianness(cns->orientation->offset.x);
        s.write_float_t_reverse_endianness(cns->orientation->offset.y);
        s.write_float_t_reverse_endianness(cns->orientation->offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_modern_write_skin_block_constraint_up_vector(&cns->direction->up_vector,
            s, strings, string_offsets, is_x);
        s.write_float_t_reverse_endianness(cns->direction->align_axis.x);
        s.write_float_t_reverse_endianness(cns->direction->align_axis.y);
        s.write_float_t_reverse_endianness(cns->direction->align_axis.z);
        s.write_float_t_reverse_endianness(cns->direction->target_offset.x);
        s.write_float_t_reverse_endianness(cns->direction->target_offset.y);
        s.write_float_t_reverse_endianness(cns->direction->target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_modern_write_skin_block_constraint_up_vector(&cns->position->up_vector,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&cns->position->constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&cns->position->constraining_object,
            s, strings, string_offsets, is_x);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_modern_write_skin_block_constraint_up_vector(&cns->distance->up_vector,
            s, strings, string_offsets, is_x);
        s.write_float_t_reverse_endianness(cns->distance->distance);
        obj_modern_write_skin_block_constraint_attach_point(&cns->distance->constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&cns->distance->constraining_object,
            s, strings, string_offsets, is_x);
        break;
    }
}

static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, uint32_t header_length, const char** str, bool is_x) {
    attach_point->affected_by_orientation = s.read_int32_t_reverse_endianness() != 0;
    attach_point->affected_by_scaling = s.read_int32_t_reverse_endianness() != 0;
    attach_point->offset.x = s.read_float_t_reverse_endianness();
    attach_point->offset.y = s.read_float_t_reverse_endianness();
    attach_point->offset.z = s.read_float_t_reverse_endianness();
}

static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* attach_point,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    s.write_int32_t_reverse_endianness(attach_point->affected_by_orientation ? 1 : 0);
    s.write_int32_t_reverse_endianness(attach_point->affected_by_scaling ? 1 : 0);
    s.write_float_t_reverse_endianness(attach_point->offset.x);
    s.write_float_t_reverse_endianness(attach_point->offset.y);
    s.write_float_t_reverse_endianness(attach_point->offset.z);
}

static void obj_modern_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    up_vector->active = !!s.read_int32_t_reverse_endianness();
    up_vector->roll = s.read_float_t_reverse_endianness();
    up_vector->affected_axis.x = s.read_float_t_reverse_endianness();
    up_vector->affected_axis.y = s.read_float_t_reverse_endianness();
    up_vector->affected_axis.z = s.read_float_t_reverse_endianness();
    up_vector->point_at.x = s.read_float_t_reverse_endianness();
    up_vector->point_at.y = s.read_float_t_reverse_endianness();
    up_vector->point_at.z = s.read_float_t_reverse_endianness();

    int64_t name_offset = s.read_offset(header_length, is_x);
    up_vector->name = obj_read_utf8_string_null_terminated_offset(alloc, s, name_offset);
}

static void obj_modern_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up_vector,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    s.write_int32_t_reverse_endianness(up_vector->active ? 1 : 0);
    s.write_float_t_reverse_endianness(up_vector->roll);
    s.write_float_t_reverse_endianness(up_vector->affected_axis.x);
    s.write_float_t_reverse_endianness(up_vector->affected_axis.y);
    s.write_float_t_reverse_endianness(up_vector->affected_axis.z);
    s.write_float_t_reverse_endianness(up_vector->point_at.x);
    s.write_float_t_reverse_endianness(up_vector->point_at.y);
    s.write_float_t_reverse_endianness(up_vector->point_at.z);

    int64_t name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, up_vector->name);
    s.write_offset(name_offset, 0x20, is_x);
}

static obj_skin_block_expression* obj_modern_read_skin_block_expression(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    obj_skin_block_expression* exp = alloc->allocate<obj_skin_block_expression>();
    obj_modern_read_skin_block_node(&exp->node, alloc, s, header_length, str, is_x);

    int64_t name_offset = s.read_offset(header_length, is_x);
    char* name = s.read_utf8_string_null_terminated_offset(name_offset);

    exp->name_index = 0;
    for (const char** i = str; *i; i++)
        if (!str_utils_compare(name, *i)) {
            exp->name_index = 0x8000 | (int32_t)(i - str);
            break;
        }
    free_def(name);

    uint32_t num_expression = s.read_int32_t_reverse_endianness();
    num_expression = min_def(num_expression, 9);
    exp->num_expression = num_expression;
    if (!is_x)
        for (uint32_t i = 0; i < num_expression; i++) {
            int64_t expression_offset = s.read_offset_f2(header_length);
            if (expression_offset)
                exp->expression_array[i]
                    = obj_read_utf8_string_null_terminated_offset(alloc, s, expression_offset);
        }
    else
        for (uint32_t i = 0; i < num_expression; i++) {
            int64_t expression_offset = s.read_offset_x();
            if (expression_offset)
                exp->expression_array[i]
                    = obj_read_utf8_string_null_terminated_offset(alloc, s, expression_offset);
        }

    for (uint32_t i = num_expression; i < 9; i++)
        exp->expression_array[i] = 0;
    return exp;
}

static void obj_modern_write_skin_block_expression(obj_skin_block_expression* exp,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets,
    const char** bone_name_array, bool is_x) {
    obj_modern_write_skin_block_node(&exp->node, s, strings, string_offsets, is_x);

    int64_t name_offset = obj_skin_strings_get_string_offset_by_index(strings,
        string_offsets, bone_name_array, exp->name_index);
    s.write_offset(name_offset, 0x20, is_x);

    uint32_t num_expression = exp->num_expression;
    num_expression = min_def(num_expression, 9);
    s.write_int32_t_reverse_endianness(num_expression);
    if (!is_x) {
        for (uint32_t i = 0; i < num_expression; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, exp->expression_array[i]);
            s.write_offset_f2((int32_t)expression_offset, 0x20);
        }

        for (uint32_t i = num_expression; i < 9; i++)
            s.write_offset_f2(0, 0x20);
    }
    else {
        for (uint32_t i = 0; i < num_expression; i++) {
            int64_t expression_offset = obj_skin_strings_get_string_offset(strings,
                string_offsets, exp->expression_array[i]);
            s.write_offset_x(expression_offset);
        }

        for (uint32_t i = num_expression; i < 9; i++)
            s.write_offset_x(0);
    }
}

static obj_skin_block_motion* obj_modern_read_skin_block_motion(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    obj_skin_block_motion* mot = alloc->allocate<obj_skin_block_motion>();
    obj_modern_read_skin_block_node(&mot->node, alloc, s, header_length, str, is_x);

    int64_t name_offset = s.read_offset(header_length, is_x);
    mot->num_node = s.read_int32_t_reverse_endianness();
    int64_t bone_name_array_offset = s.read_offset(header_length, is_x);
    int64_t bone_matrix_array_offset = s.read_offset(header_length, is_x);

    if (!is_x) {
        mot->name_index = 0;
        char* name = s.read_utf8_string_null_terminated_offset(name_offset);
        for (const char** i = str; *i; i++)
            if (!str_utils_compare(name, *i)) {
                mot->name_index = 0x8000 | (int32_t)(i - str);
                break;
            }
        free_def(name);
    }
    else
        mot->name = obj_read_utf8_string_null_terminated_offset(alloc, s, name_offset);

    mot->node_array = 0;

    if (!mot->num_node)
        return mot;

    mot->node_array = alloc->allocate<obj_skin_motion_node>(mot->num_node);

    if (bone_name_array_offset) {
        s.position_push(bone_name_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++)
            mot->node_array[i].name_index = s.read_uint32_t_reverse_endianness();
        s.position_pop();
    }

    if (bone_matrix_array_offset) {
        s.position_push(bone_matrix_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++) {
            mat4& mat = mot->node_array[i].inv_bind_pose_mat;
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
    return mot;
}

static void obj_modern_write_skin_block_motion(obj_skin_block_motion* mot,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x,
    const char** bone_name_array, int64_t* bone_name_array_offset, int64_t* bone_matrix_array_offset) {
    obj_modern_write_skin_block_node(&mot->node, s, strings, string_offsets, is_x);

    int64_t name_offset;
    if (!is_x)
        name_offset = obj_skin_strings_get_string_offset_by_index(strings,
            string_offsets, bone_name_array, mot->name_index);
    else
        name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, mot->name);
    s.write_offset(name_offset, 0x20, is_x);
    s.write_int32_t_reverse_endianness(mot->num_node);
    s.write_offset(mot->node_array ? *bone_name_array_offset : 0, 0x20, is_x);
    s.write_offset(mot->node_array ? *bone_matrix_array_offset : 0, 0x20, is_x);

    if (mot->node_array) {
        s.position_push(*bone_name_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++)
            s.write_uint32_t_reverse_endianness(mot->node_array[i].name_index);
        s.position_pop();
        *bone_name_array_offset += mot->num_node * sizeof(uint32_t);

        s.position_push(*bone_matrix_array_offset, SEEK_SET);
        for (uint32_t i = 0; i < mot->num_node; i++) {
            mat4& mat = mot->node_array[i].inv_bind_pose_mat;
            s.write_float_t_reverse_endianness(mat.row0.x);
            s.write_float_t_reverse_endianness(mat.row1.x);
            s.write_float_t_reverse_endianness(mat.row2.x);
            s.write_float_t_reverse_endianness(mat.row3.x);
            s.write_float_t_reverse_endianness(mat.row0.y);
            s.write_float_t_reverse_endianness(mat.row1.y);
            s.write_float_t_reverse_endianness(mat.row2.y);
            s.write_float_t_reverse_endianness(mat.row3.y);
            s.write_float_t_reverse_endianness(mat.row0.z);
            s.write_float_t_reverse_endianness(mat.row1.z);
            s.write_float_t_reverse_endianness(mat.row2.z);
            s.write_float_t_reverse_endianness(mat.row3.z);
            s.write_float_t_reverse_endianness(mat.row0.w);
            s.write_float_t_reverse_endianness(mat.row1.w);
            s.write_float_t_reverse_endianness(mat.row2.w);
            s.write_float_t_reverse_endianness(mat.row3.w);
        }
        s.position_pop();
        *bone_matrix_array_offset += mot->num_node * sizeof(mat4);
    }
}

static void obj_modern_read_skin_block_node(obj_skin_block_node* node,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    int64_t parent_name = s.read_offset(header_length, is_x);
    node->parent_name = obj_read_utf8_string_null_terminated_offset(alloc, s, parent_name);

    node->position.x = s.read_float_t_reverse_endianness();
    node->position.y = s.read_float_t_reverse_endianness();
    node->position.z = s.read_float_t_reverse_endianness();
    node->rotation.x = s.read_float_t_reverse_endianness();
    node->rotation.y = s.read_float_t_reverse_endianness();
    node->rotation.z = s.read_float_t_reverse_endianness();
    node->scale.x = s.read_float_t_reverse_endianness();
    node->scale.y = s.read_float_t_reverse_endianness();
    node->scale.z = s.read_float_t_reverse_endianness();
    if (is_x)
        s.read(0, 0x04);
}

static void obj_modern_write_skin_block_node(obj_skin_block_node* node,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    int64_t parent_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, node->parent_name);
    s.write_offset(parent_name_offset, 0x20, is_x);

    s.write_float_t_reverse_endianness(node->position.x);
    s.write_float_t_reverse_endianness(node->position.y);
    s.write_float_t_reverse_endianness(node->position.z);
    s.write_float_t_reverse_endianness(node->rotation.x);
    s.write_float_t_reverse_endianness(node->rotation.y);
    s.write_float_t_reverse_endianness(node->rotation.z);
    s.write_float_t_reverse_endianness(node->scale.x);
    s.write_float_t_reverse_endianness(node->scale.y);
    s.write_float_t_reverse_endianness(node->scale.z);
    if (is_x)
        s.write(0x04);
}

static obj_skin_block_osage* obj_modern_read_skin_block_osage(
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s, uint32_t header_length, const char** str, bool is_x) {
    obj_skin_block_osage* osg = alloc->allocate<obj_skin_block_osage>();
    obj_modern_read_skin_block_node(&osg->node, alloc, s, header_length, str, is_x);

    osg->start_index = s.read_int32_t_reverse_endianness();
    osg->count = s.read_int32_t_reverse_endianness();
    osg->external_name_index = s.read_uint32_t_reverse_endianness();
    osg->name_index = s.read_uint32_t_reverse_endianness();

    if (!is_x)
        s.read(0, 0x14);
    else {
        s.read(0, 0x08);
        int64_t motion_node_name_offset = s.read_offset_x();
        osg->motion_node_name = obj_read_utf8_string_null_terminated_offset(alloc, s, motion_node_name_offset);
        s.read(0, 0x18);
    }

    osg->node_array = 0;
    osg->num_node = 0;
    return osg;
}

static void obj_modern_write_skin_block_osage(obj_skin_block_osage* osg,
    stream& s, std::vector<string_hash>& strings, std::vector<int64_t>& string_offsets, bool is_x) {
    obj_modern_write_skin_block_node(&osg->node, s, strings, string_offsets, is_x);

    s.write_int32_t_reverse_endianness(osg->start_index);
    s.write_int32_t_reverse_endianness(osg->count);
    s.write_uint32_t_reverse_endianness(osg->external_name_index);
    s.write_uint32_t_reverse_endianness(osg->name_index);

    if (!is_x)
        s.write(0x14);
    else {
        s.write(0x08);
        int64_t motion_node_name_offset = obj_skin_strings_get_string_offset(strings,
            string_offsets, osg->motion_node_name);
        s.write_offset_x(motion_node_name_offset);
        s.write(0x18);
    }
}

static int32_t vtx_fmt_map[] = {
     -1, 1, 2, 3, 4, 5, 6, 7, 8, 9,
     10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     20
};

static void obj_modern_read_vertex(obj_mesh* mesh, prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    int64_t* vertex, const uint32_t vertex_format_index, uint32_t num_vertex, uint32_t size_vertex) {
    int32_t vtx_fmt = vertex_format_index >= 0 && vertex_format_index <= 20
        ? vtx_fmt_map[vertex_format_index] : -1;

    bool tex2 = false;
    bool tex3 = false;
    bool tex4 = false;
    bool tex5 = false;
    bool bone = false;

    switch (vtx_fmt) {
    default:
        return;
    case 2:
        break;
    case 4:
        bone = true;
        break;
    case 6:
        tex2 = true;
        bone = true;
    case 8:
        tex2 = true;
        bone = true;
        break;
    case 10:
        tex2 = true;
        tex3 = true;
        break;
    case 12:
        tex2 = true;
        tex3 = true;
        bone = true;
        break;
    case 14:
        tex2 = true;
        tex3 = true;
        tex4 = true;
        break;
    case 16:
        tex2 = true;
        tex3 = true;
        tex4 = true;
        bone = true;
        break;
    case 18:
        tex2 = true;
        tex3 = true;
        tex4 = true;
        tex5 = true;
        break;
    case 20:
        tex2 = true;
        tex3 = true;
        tex4 = true;
        tex5 = true;
        bone = true;
        break;
    }

    obj_vertex_format vertex_format = (obj_vertex_format)(OBJ_VERTEX_POSITION
        | OBJ_VERTEX_NORMAL
        | OBJ_VERTEX_TANGENT
        | OBJ_VERTEX_TEXCOORD0
        | OBJ_VERTEX_TEXCOORD1
        | OBJ_VERTEX_COLOR0);

    if (tex2)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD2);
    if (tex3)
        enum_or(vertex_format, OBJ_VERTEX_TEXCOORD3);
    if (bone)
        enum_or(vertex_format, OBJ_VERTEX_BONE_DATA);

    int64_t vtx_offset = vertex[13];

    bool has_tangents = false;
    obj_vertex_data* vtx = alloc->allocate<obj_vertex_data>(num_vertex);
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
        vtx[i].normal = vec3::div_min_max(normal, -32768.0f, 32767.0f);

        vec4 tangent;
        tangent.x = (float_t)s.read_int16_t_reverse_endianness();
        tangent.y = (float_t)s.read_int16_t_reverse_endianness();
        tangent.z = (float_t)s.read_int16_t_reverse_endianness();
        tangent.w = (float_t)s.read_int16_t_reverse_endianness();
        vtx[i].tangent = vec4::div_min_max(tangent, -32768.0f, 32767.0f);

        vec2h texcoord0;
        texcoord0.x = s.read_half_t_reverse_endianness();
        texcoord0.y = s.read_half_t_reverse_endianness();
        vec2h_to_vec2(texcoord0, vtx[i].texcoord0);

        vec2h texcoord1;
        texcoord1.x = s.read_half_t_reverse_endianness();
        texcoord1.y = s.read_half_t_reverse_endianness();
        vec2h_to_vec2(texcoord1, vtx[i].texcoord1);

        if (tex2) {
            vec2h texcoord2;
            texcoord2.x = s.read_half_t_reverse_endianness();
            texcoord2.y = s.read_half_t_reverse_endianness();
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);
        }

        if (tex3) {
            vec2h texcoord3;
            texcoord3.x = s.read_half_t_reverse_endianness();
            texcoord3.y = s.read_half_t_reverse_endianness();
            vec2h_to_vec2(texcoord3, vtx[i].texcoord3);
        }

        if (tex4)
            s.read(sizeof(half_t) * 2);

        if (tex5)
            s.read(sizeof(half_t) * 2);

        vec4h color;
        color.x = s.read_half_t_reverse_endianness();
        color.y = s.read_half_t_reverse_endianness();
        color.z = s.read_half_t_reverse_endianness();
        color.w = s.read_half_t_reverse_endianness();
        vec4h_to_vec4(color, vtx[i].color0);

        if (bone) {
            vec4 bone_weight;
            bone_weight.x = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.y = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.z = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight.w = (float_t)s.read_int16_t_reverse_endianness();
            bone_weight = vec4::div_min_max(bone_weight, -32768.0f, 32767.0f);

            vec4i16 bone_index;
            bone_index.x = s.read_uint8_t();
            bone_index.y = s.read_uint8_t();
            bone_index.z = s.read_uint8_t();
            bone_index.w = s.read_uint8_t();
            bone_index.x = bone_index.x != 0xFF ? bone_index.x / 3 : -1;
            bone_index.y = bone_index.y != 0xFF ? bone_index.y / 3 : -1;
            bone_index.z = bone_index.z != 0xFF ? bone_index.z / 3 : -1;
            bone_index.w = bone_index.w != 0xFF ? bone_index.w / 3 : -1;

            obj_vertex_validate_bone_data(bone_weight, bone_index);

            vtx[i].bone_weight = bone_weight;
            vtx[i].bone_index = bone_index;
        }

        if (vtx[i].tangent != 0.0f)
            has_tangents = true;
    }

    if (!has_tangents)
        enum_and(vertex_format, ~OBJ_VERTEX_TANGENT);

    mesh->vertex_array = vtx;
    mesh->num_vertex = num_vertex;
    mesh->vertex_format = vertex_format;
    mesh->attrib.m.compressed = 1;
}

static void obj_modern_write_vertex(obj_mesh* mesh, stream& s, int64_t* vertex,
    uint32_t* vertex_format_index, uint32_t* num_vertex, uint32_t* size_vertex, f2_struct* ovtx) {
    obj_vertex_data* vtx = mesh->vertex_array;
    uint32_t _num_vertex = mesh->num_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    bool tex2 = false;
    bool tex3 = false;
    bool bone = false;

    uint32_t _vertex_format_index;
    if ((vertex_format & OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3 | OBJ_VERTEX_BONE_DATA)
        == (OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3 | OBJ_VERTEX_BONE_DATA))
        _vertex_format_index = 12;
    else if ((vertex_format & OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3)
        == (OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3))
        _vertex_format_index = 10;
    else if ((vertex_format & OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_BONE_DATA)
        == (OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_BONE_DATA))
        _vertex_format_index = 8;
    else if ((vertex_format & OBJ_VERTEX_TEXCOORD2) == OBJ_VERTEX_TEXCOORD2)
        _vertex_format_index = 6;
    else if ((vertex_format & OBJ_VERTEX_BONE_DATA) == OBJ_VERTEX_BONE_DATA)
        _vertex_format_index = 4;
    else
        _vertex_format_index = 2;

    uint32_t _size_vertex = 0x2C;
    uint32_t enrs_se3_rc = 12;
    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        _size_vertex += 0x0C;
        enrs_se3_rc += 4;
        bone = true;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
        _size_vertex += 0x04;
        enrs_se3_rc += 2;
        tex3 = true;
    }

    if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
        _size_vertex += 0x04;
        enrs_se3_rc += 2;
        tex2 = true;
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
        s.write_float_t_reverse_endianness(position.x);
        s.write_float_t_reverse_endianness(position.y);
        s.write_float_t_reverse_endianness(position.z);

        vec3 normal = vec3::mult_min_max(vtx[i].normal, -32768.0f, 32767.0f);
        s.write_int16_t_reverse_endianness((int16_t)roundf(normal.x));
        s.write_int16_t_reverse_endianness((int16_t)roundf(normal.y));
        s.write_int16_t_reverse_endianness((int16_t)roundf(normal.z));
        s.write(0x02);

        vec4 tangent = vec4::mult_min_max(vtx[i].tangent, -32768.0f, 32767.0f);
        s.write_int16_t_reverse_endianness((int16_t)roundf(tangent.x));
        s.write_int16_t_reverse_endianness((int16_t)roundf(tangent.y));
        s.write_int16_t_reverse_endianness((int16_t)roundf(tangent.z));
        s.write_int16_t_reverse_endianness((int16_t)roundf(tangent.w));

        vec2h texcoord0;
        vec2_to_vec2h(vtx[i].texcoord0, texcoord0);
        s.write_half_t_reverse_endianness(texcoord0.x);
        s.write_half_t_reverse_endianness(texcoord0.y);

        vec2h texcoord1;
        vec2_to_vec2h(vtx[i].texcoord1, texcoord1);
        s.write_half_t_reverse_endianness(texcoord1.x);
        s.write_half_t_reverse_endianness(texcoord1.y);

        if (tex2) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            s.write_half_t_reverse_endianness(texcoord2.x);
            s.write_half_t_reverse_endianness(texcoord2.y);
        }

        if (tex3) {
            vec2h texcoord3;
            vec2_to_vec2h(vtx[i].texcoord3, texcoord3);
            s.write_half_t_reverse_endianness(texcoord3.x);
            s.write_half_t_reverse_endianness(texcoord3.y);
        }

        vec4h color;
        vec4_to_vec4h(vtx[i].color0, color);
        s.write_half_t_reverse_endianness(color.x);
        s.write_half_t_reverse_endianness(color.y);
        s.write_half_t_reverse_endianness(color.z);
        s.write_half_t_reverse_endianness(color.w);

        if (bone) {
            vec4 bone_weight = vec4::mult_min_max(vtx[i].bone_weight, -32768.0f, 32767.0f);
            s.write_int16_t_reverse_endianness((int16_t)bone_weight.x);
            s.write_int16_t_reverse_endianness((int16_t)bone_weight.y);
            s.write_int16_t_reverse_endianness((int16_t)bone_weight.z);
            s.write_int16_t_reverse_endianness((int16_t)bone_weight.w);

            s.write_uint8_t((uint8_t)vtx[i].bone_index.x);
            s.write_uint8_t((uint8_t)vtx[i].bone_index.y);
            s.write_uint8_t((uint8_t)vtx[i].bone_index.z);
            s.write_uint8_t((uint8_t)vtx[i].bone_index.w);
        }
    }

    *vertex_format_index = _vertex_format_index;
    *num_vertex = _num_vertex;
    *size_vertex = _size_vertex;
}

inline static const char* obj_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc) {
    if (str)
        return alloc->allocate<char>(str, utf8_length(str) + 1);
    return 0;
}

inline static const char* obj_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset) {
    size_t len = s.read_utf8_string_null_terminated_offset_length(offset);
    char* str = alloc->allocate<char>(len + 1);
    s.position_push(offset, SEEK_SET);
    s.read(str, len);
    s.position_pop();
    str[len] = 0;
    return str;
}

inline static uint32_t obj_skin_strings_get_string_index(std::vector<string_hash>& vec, const char* str) {
    size_t len = utf8_length(str);
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return (uint32_t)(0x8000 | &i - vec.data());
    return 0x8000;
}

inline static int64_t obj_skin_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, const char* str) {
    size_t len = utf8_length(str);
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static int64_t obj_skin_strings_get_string_offset_by_index(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, const char** strings, uint32_t index) {
    if (!(index & 0x8000))
        return 0;

    const char* str = strings[index & 0x7FFF];
    size_t len = utf8_length(str);
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static void obj_skin_strings_push_back_check(std::vector<string_hash>& vec, const char* str) {
    size_t len = utf8_length(str);
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return;

    vec.push_back(str);
}

inline static void obj_skin_strings_push_back_check_by_index(std::vector<string_hash>& vec,
    const char** strings, uint32_t index) {
    if (!(index & 0x8000))
        return;

    const char* str = strings[index & 0x7FFF];
    size_t len = utf8_length(str);
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return;

    vec.push_back(str);
}
