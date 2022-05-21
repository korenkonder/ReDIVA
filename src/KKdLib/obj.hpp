/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kf.h"
#include "mat.h"
#include "string.h"
#include "vec.h"

enum obj_index_format : uint32_t {
    OBJ_INDEX_U8  = 0x00,
    OBJ_INDEX_U16 = 0x01,
    OBJ_INDEX_U32 = 0x02,
};

enum obj_material_aniso_direction : uint32_t {
    OBJ_MATERIAL_ANISO_DIRECTION_NORMAL = 0,
    OBJ_MATERIAL_ANISO_DIRECTION_U      = 1,
    OBJ_MATERIAL_ANISO_DIRECTION_V      = 2,
    OBJ_MATERIAL_ANISO_DIRECTION_RADIAL = 3,
};

enum obj_material_blend_factor : uint32_t {
    OBJ_MATERIAL_BLEND_ZERO              = 0,
    OBJ_MATERIAL_BLEND_ONE               = 1,
    OBJ_MATERIAL_BLEND_SRC_COLOR         = 2,
    OBJ_MATERIAL_BLEND_INVERSE_SRC_COLOR = 3,
    OBJ_MATERIAL_BLEND_SRC_ALPHA         = 4,
    OBJ_MATERIAL_BLEND_INVERSE_SRC_ALPHA = 5,
    OBJ_MATERIAL_BLEND_DST_ALPHA         = 6,
    OBJ_MATERIAL_BLEND_INVERSE_DST_ALPHA = 7,
    OBJ_MATERIAL_BLEND_DST_COLOR         = 8,
    OBJ_MATERIAL_BLEND_INVERSE_DST_COLOR = 9,
    OBJ_MATERIAL_BLEND_ALPHA_SATURATE = 10,
};

enum obj_material_bump_map_type : uint32_t {
    OBJ_MATERIAL_BUMP_MAP_NONE = 0,
    OBJ_MATERIAL_BUMP_MAP_DOT  = 1,
    OBJ_MATERIAL_BUMP_MAP_ENV  = 2,
};

enum obj_material_color_source_type : uint32_t {
    OBJ_MATERIAL_COLOR_SOURCE_MATERIAL_COLOR = 0,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_COLOR   = 1,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_MORPH   = 2,
};

enum obj_material_shader_lighting_type : uint32_t {
    OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT  = 0x00,
    OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT = 0x01,
    OBJ_MATERIAL_SHADER_LIGHTING_PHONG    = 0x02,
};

enum obj_material_specular_quality : uint32_t {
    OBJ_MATERIAL_SPECULAR_QUALITY_LOW  = 0,
    OBJ_MATERIAL_SPECULAR_QUALITY_HIGH = 1,
};

enum obj_material_texture_type : uint32_t {
    OBJ_MATERIAL_TEXTURE_NONE               = 0x00,
    OBJ_MATERIAL_TEXTURE_COLOR              = 0x01,
    OBJ_MATERIAL_TEXTURE_NORMAL             = 0x02,
    OBJ_MATERIAL_TEXTURE_SPECULAR           = 0x03,
    OBJ_MATERIAL_TEXTURE_HEIGHT             = 0x04,
    OBJ_MATERIAL_TEXTURE_REFLECTION         = 0x05,
    OBJ_MATERIAL_TEXTURE_TRANSLUCENCY       = 0x06,
    OBJ_MATERIAL_TEXTURE_TRANSPARENCY       = 0x07,
    OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE = 0x08,
    OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE   = 0x09,
};

enum obj_material_texture_coordinate_translation_type : uint32_t {
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_NONE   = 0x00,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_UV     = 0x01,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_SPHERE = 0x02,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_CUBE   = 0x03
};

enum obj_material_vertex_translation_type : uint32_t {
    OBJ_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0,
    OBJ_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 1,
    OBJ_MATERIAL_VERTEX_TRANSLATION_MORPHING = 2,
};

enum obj_primitive_type : uint32_t {
    OBJ_PRIMITIVE_POINTS         = 0x00,
    OBJ_PRIMITIVE_LINES          = 0x01,
    OBJ_PRIMITIVE_LINE_STRIP     = 0x02,
    OBJ_PRIMITIVE_LINE_LOOP      = 0x03,
    OBJ_PRIMITIVE_TRIANGLES      = 0x04,
    OBJ_PRIMITIVE_TRIANGLE_STRIP = 0x05,
    OBJ_PRIMITIVE_TRIANGLE_FAN   = 0x06,
    OBJ_PRIMITIVE_QUADS          = 0x07,
    OBJ_PRIMITIVE_QUAD_STRIP     = 0x08,
    OBJ_PRIMITIVE_POLYGON        = 0x09,
};

enum obj_skin_block_constraint_type : uint32_t {
    OBJ_SKIN_BLOCK_CONSTRAINT_NONE = 0,
    OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION,
    OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION,
    OBJ_SKIN_BLOCK_CONSTRAINT_POSITION,
    OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE,
};

enum obj_skin_block_constraint_coupling : uint32_t {
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_RIGID = 0x01,
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_SOFT  = 0x03,
};

enum obj_skin_block_type : uint32_t {
    OBJ_SKIN_BLOCK_NONE = 0,
    OBJ_SKIN_BLOCK_CLOTH,
    OBJ_SKIN_BLOCK_CONSTRAINT,
    OBJ_SKIN_BLOCK_EXPRESSION,
    OBJ_SKIN_BLOCK_ITEM,
    OBJ_SKIN_BLOCK_MOTION,
    OBJ_SKIN_BLOCK_OSAGE,
};

enum obj_skin_osage_root_coli_type {
    OBJ_SKIN_OSAGE_ROOT_COLI_TYPE_NONE     = 0x00,
    OBJ_SKIN_OSAGE_ROOT_COLI_TYPE_BALL     = 0x01,
    OBJ_SKIN_OSAGE_ROOT_COLI_TYPE_CYLINDER = 0x02,
    OBJ_SKIN_OSAGE_ROOT_COLI_TYPE_PLANE    = 0x03,
    OBJ_SKIN_OSAGE_ROOT_COLI_TYPE_ELLIPSE  = 0x04,
};

enum obj_vertex_format : uint32_t {
    OBJ_VERTEX_FORMAT_NONE      = 0x000,
    OBJ_VERTEX_FORMAT_POSITION  = 0x001,
    OBJ_VERTEX_FORMAT_NORMAL    = 0x002,
    OBJ_VERTEX_FORMAT_TANGENT   = 0x004,
    OBJ_VERTEX_FORMAT_BINORMAL  = 0x008,
    OBJ_VERTEX_FORMAT_TEXCOORD0 = 0x010,
    OBJ_VERTEX_FORMAT_TEXCOORD1 = 0x020,
    OBJ_VERTEX_FORMAT_TEXCOORD2 = 0x040,
    OBJ_VERTEX_FORMAT_TEXCOORD3 = 0x080,
    OBJ_VERTEX_FORMAT_COLOR0    = 0x100,
    OBJ_VERTEX_FORMAT_COLOR1    = 0x200,
    OBJ_VERTEX_FORMAT_BONE_DATA = 0x400,
    OBJ_VERTEX_FORMAT_UNKNOWN   = 0x800,
};

struct obj_axis_aligned_bounding_box {
    vec3 center;
    vec3 size;
};

struct obj_bounding_box {
    vec3 center;
    vec3 size;
};

struct obj_bounding_sphere {
    vec3 center;
    float_t radius;
};

struct obj_shader_compo_member {
    uint32_t color : 1;
    uint32_t color_a : 1;
    uint32_t color_l1 : 1;
    uint32_t color_l1_a : 1;
    uint32_t color_l2 : 1;
    uint32_t color_l2_a : 1;
    uint32_t transparency : 1;
    uint32_t specular : 1;
    uint32_t normal_01 : 1;
    uint32_t normal_02 : 1;
    uint32_t envmap : 1;
    uint32_t color_l3 : 1;
    uint32_t color_l3_a : 1;
    uint32_t translucency : 1;
    uint32_t flag_14 : 1;
    uint32_t override_ibl : 1;
    uint32_t dummy : 16;
};

union obj_shader_compo {
    obj_shader_compo_member m;
    uint32_t w;
};

struct obj_material_shader_attrib_member {
    obj_material_vertex_translation_type vtx_trans_type : 2;
    obj_material_color_source_type col_src : 2;
    uint32_t is_lgt_diffuse : 1;
    uint32_t is_lgt_specular : 1;
    uint32_t is_lgt_per_pixel : 1;
    uint32_t is_lgt_double : 1;
    obj_material_bump_map_type bump_map_type : 2;
    uint32_t fresnel_type : 4;
    uint32_t line_light : 4;
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    obj_material_specular_quality specular_quality : 1;
    obj_material_aniso_direction aniso_direction : 2;
    uint32_t dummy : 9;
};

union obj_material_shader_attrib {
    obj_material_shader_attrib_member m;
    uint32_t w;

    obj_material_shader_lighting_type get_lighting_type();
};

struct obj_texture_attrib_member {
    uint32_t repeat_u : 1;
    uint32_t repeat_v : 1;
    uint32_t mirror_u : 1;
    uint32_t mirror_v : 1;
    uint32_t ignore_alpha : 1;
    uint32_t blend : 5;
    uint32_t alpha_blend : 5;
    uint32_t border : 1;
    uint32_t clamp2edge : 1;
    uint32_t filter : 3;
    uint32_t mipmap : 2;
    uint32_t mipmap_bias : 7;
    uint32_t flag_29 : 1;
    uint32_t anisotropic_filter : 2;
};

union obj_texture_attrib {
    obj_texture_attrib_member m;
    uint32_t w;

    int32_t get_blend();
};

struct obj_texture_shader_attrib_member {
    obj_material_texture_type tex_type : 4;
    int32_t uv_idx : 4;
    obj_material_texture_coordinate_translation_type texcoord_trans : 3;
    uint32_t dummy : 21;
};

union obj_texture_shader_attrib {
    obj_texture_shader_attrib_member m;
    uint32_t w;
};

struct obj_material_texture_data {
    obj_texture_attrib attrib;
    uint32_t tex_index;
    obj_texture_shader_attrib shader_info;
    char ex_shader[8];
    float_t weight;
    mat4u tex_coord_mat;
    union {
        uint32_t reserved[8];
        int32_t texture_index;
    };
};

struct obj_material_attrib_member {
    uint32_t alpha_texture : 1;
    uint32_t alpha_material : 1;
    uint32_t punch_through : 1;
    uint32_t double_sided : 1;
    uint32_t normal_dir_light : 1;
    obj_material_blend_factor src_blend_factor : 4;
    obj_material_blend_factor dst_blend_factor : 4;
    uint32_t blend_operation : 3;
    uint32_t zbias : 4;
    uint32_t no_fog : 1;
    uint32_t translucent_priority : 6;
    uint32_t has_fog_height : 1;
    uint32_t flag_28 : 1;
    uint32_t fog_height : 1;
    uint32_t flag_30 : 1;
    uint32_t flag_31 : 1;
};

union obj_material_attrib {
    obj_material_attrib_member m;
    uint32_t w;
};

struct obj_material_color {
    vec4u diffuse;
    vec4u ambient;
    vec4u specular;
    vec4u emission;
    float_t shininess;
    float_t intensity;
};

struct obj_material {
    obj_shader_compo shader_compo;
    union {
        char name[8];
        int32_t index;
    } shader;
    obj_material_shader_attrib shader_info;
    obj_material_texture_data texdata[8];
    obj_material_attrib attrib;
    obj_material_color color;
    vec3 center;
    float_t radius;
    char name[64];
    float_t bump_depth;
    uint32_t reserved[15];
};

struct obj_material_data {
    uint32_t num_of_texture;
    obj_material material;
};

struct obj_sub_mesh_attrib_member {
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    uint32_t transparent : 1;
    uint32_t flag_3 : 1;
    uint32_t dummy : 28;
};

union obj_sub_mesh_attrib {
    obj_sub_mesh_attrib_member m;
    uint32_t w;
};

struct obj_sub_mesh {
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t uv_index[8];
    uint16_t* bone_indices;
    uint32_t bone_indices_count;
    uint32_t bones_per_vertex;
    obj_primitive_type primitive_type;
    obj_index_format index_format;
    uint32_t* indices;
    uint32_t indices_count;
    obj_sub_mesh_attrib attrib;
    union {
        obj_axis_aligned_bounding_box axis_aligned_bounding_box;
        obj_bounding_box bounding_box;
    };
    uint16_t first_index;
    uint16_t last_index;
    uint32_t indices_offset;
};

struct obj_vertex_data {
    vec3 position;
    vec3 normal;
    vec4u tangent;
    vec3 binormal;
    vec2 texcoord0;
    vec2 texcoord1;
    vec2 texcoord2;
    vec2 texcoord3;
    vec4u color0;
    vec4u color1;
    vec4u bone_weight;
    vec4iu bone_index;
    vec4u unknown;
};

struct obj_mesh_attrib_member {
    uint32_t flag_0 : 1;
    uint32_t billboard_y_axis : 1;
    uint32_t translucent_no_shadow : 1;
    uint32_t billboard : 1;
    uint32_t dummy : 27; // 28
    uint32_t compressed : 1; // Own stuff
};

union obj_mesh_attrib {
    obj_mesh_attrib_member m;
    uint32_t w;
};

struct obj_mesh {
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    uint32_t num_submesh;
    obj_sub_mesh* submesh_array;
    obj_vertex_format vertex_format;
    uint32_t size_vertex;
    uint32_t num_vertex;
    obj_vertex_data* vertex;
    obj_mesh_attrib attrib;
    uint32_t reserved[6];
    char name[0x40];
};

struct obj_skin_block_cloth_field_1C_sub {
    string bone_name;
    float_t weight;
    uint32_t matrix_index;
    uint32_t field_C;
};

struct obj_skin_block_cloth_field_1C {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    uint32_t field_1C;
    uint32_t field_20;
    uint32_t field_24;
    obj_skin_block_cloth_field_1C_sub sub_data[4];
};

struct obj_skin_block_cloth_field_20_sub {
    uint32_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
    float_t field_28;
};

struct obj_skin_block_cloth_field_20 {
    obj_skin_block_cloth_field_20_sub sub_data[10];
};

struct obj_skin_block_node {
    string parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

struct obj_skin_block_cloth {
    string mesh_name;
    string backface_mesh_name;
    uint32_t field_8;
    uint32_t count;
    uint32_t field_10;
    uint32_t field_14;
    mat4u field_18[32];
    obj_skin_block_cloth_field_1C* field_1C;
    obj_skin_block_cloth_field_20* field_20;
    uint16_t* field_24;
    int32_t field_24_count;
    uint16_t* field_28;
    int32_t field_28_count;
    uint32_t field_2C;
    uint32_t field_30;
};

struct obj_skin_block_constraint_attach_point {
    bool affected_by_orientation;
    bool affected_by_scaling;
    vec3 offset;
};

struct obj_skin_block_constraint_up_vector {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    string name;
};

struct obj_skin_block_constraint_direction {
    obj_skin_block_constraint_up_vector up_vector;
    vec3 align_axis;
    vec3 target_offset;
};

struct obj_skin_block_constraint_distance {
    obj_skin_block_constraint_up_vector up_vector;
    float_t distance;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
};

struct obj_skin_block_constraint_orientation {
    vec3 offset;
};

struct obj_skin_block_constraint_position {
    obj_skin_block_constraint_up_vector up_vector;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
};

struct obj_skin_block_constraint {
    obj_skin_block_node base;
    uint32_t name_index;
    string source_node_name;
    obj_skin_block_constraint_coupling coupling;
    obj_skin_block_constraint_type type;
    union {
        obj_skin_block_constraint_direction direction;
        obj_skin_block_constraint_distance distance;
        obj_skin_block_constraint_orientation orientation;
        obj_skin_block_constraint_position position;
    };
};

struct obj_skin_block_expression {
    obj_skin_block_node base;
    uint32_t name_index;
    uint32_t expressions_count;
    string expressions[9];
};

struct obj_skin_motion_node {
    uint32_t name_index;
    mat4u transformation;
};

struct obj_skin_block_motion {
    obj_skin_block_node base;
    union {
        string name;
        uint32_t name_index;
    };
    obj_skin_motion_node* nodes;
    uint32_t nodes_count;
};

struct obj_skin_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;
};

struct obj_skin_osage_root_coli {
    obj_skin_osage_root_coli_type type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
};

struct obj_skin_osage_root_node {
    int32_t unk0;
    float_t force;
    float_t force_gain;
    float_t air_res;
    float_t rot_y;
    float_t rot_z;
    float_t hinge_y;
    float_t hinge_z;
    string name;
    obj_skin_osage_root_coli* coli;
    int32_t coli_count;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    int32_t unk44;
};

struct obj_skin_block_osage {
    obj_skin_block_node base;
    uint32_t start_index;
    uint32_t count;
    obj_skin_osage_node* nodes;
    uint32_t nodes_count;
    obj_skin_osage_root_node root;
    bool root_init;
    uint32_t external_name_index;
    uint32_t name_index;
    string motion_node_name;
};

struct obj_skin_block {
    obj_skin_block_type type;
    union {
        obj_skin_block_node base;
        obj_skin_block_cloth cloth;
        obj_skin_block_constraint constraint;
        obj_skin_block_expression expression;
        obj_skin_block_motion motion;
        obj_skin_block_osage osage;
    };
};

struct obj_skin_osage_sibling_info {
    uint32_t name_index;
    uint32_t sibling_name_index;
    float_t max_distance;
};

struct obj_skin_ex_data {
    obj_skin_osage_node* osage_nodes;
    uint32_t osage_nodes_count;
    obj_skin_block* blocks;
    uint32_t blocks_count;
    char* bone_names_buf;
    char** bone_names;
    uint32_t bone_names_count;
    obj_skin_osage_sibling_info* osage_sibling_infos;
    uint32_t osage_sibling_infos_count;
    int64_t reserved[7];
};

struct obj_skin_bone {
    uint32_t id;
    uint32_t parent;
    mat4u inv_bind_pose_mat;
    string name;
};

struct obj_skin {
    obj_skin_bone* bones;
    uint32_t bones_count;
    obj_skin_ex_data ex_data;
    bool ex_data_init;
};

struct obj {
    obj_bounding_sphere bounding_sphere;
    obj_mesh* mesh_array;
    uint32_t num_mesh;
    obj_material_data* material_array;
    uint32_t num_material;
    uint8_t flags;
    uint32_t reserved[10];
    obj_skin skin;
    bool skin_init;
    string name;
    uint32_t id;
    uint32_t hash;
};

struct obj_set {
    bool ready;
    bool modern;
    bool is_x;

    obj* obj_data;
    uint32_t obj_num;
    uint32_t* tex_id_data;
    uint32_t tex_id_num;
    uint32_t reserved[2];

    obj_set();
    virtual ~obj_set();

    void pack_file(void** data, size_t* size);
    void unpack_file(void* data, size_t size, bool modern);
};
