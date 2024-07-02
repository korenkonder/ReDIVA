/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "prj/shared_ptr.hpp"
#include "prj/stack_allocator.hpp"
#include "mat.hpp"
#include "vec.hpp"

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
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_CUBE   = 0x03,
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
    OBJ_SKIN_BLOCK_MOTION,
    OBJ_SKIN_BLOCK_OSAGE,
};

enum obj_skin_skin_param_coli_type {
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_NONE     = 0x00,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_BALL     = 0x01,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_CYLINDER = 0x02,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_PLANE    = 0x03,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_ELLIPSE  = 0x04,
};

enum obj_vertex_format : uint32_t {
    OBJ_VERTEX_NONE      = 0x000,
    OBJ_VERTEX_POSITION  = 0x001,
    OBJ_VERTEX_NORMAL    = 0x002,
    OBJ_VERTEX_TANGENT   = 0x004,
    OBJ_VERTEX_BINORMAL  = 0x008,
    OBJ_VERTEX_TEXCOORD0 = 0x010,
    OBJ_VERTEX_TEXCOORD1 = 0x020,
    OBJ_VERTEX_TEXCOORD2 = 0x040,
    OBJ_VERTEX_TEXCOORD3 = 0x080,
    OBJ_VERTEX_COLOR0    = 0x100,
    OBJ_VERTEX_COLOR1    = 0x200,
    OBJ_VERTEX_BONE_DATA = 0x400,
    OBJ_VERTEX_UNKNOWN   = 0x800,
};

struct obj_axis_aligned_bounding_box {
    vec3 center;
    vec3 size;

    obj_axis_aligned_bounding_box();
};

struct obj_bounding_box {
    vec3 center;
    vec3 size;

    obj_bounding_box();
};

struct obj_bounding_sphere {
    vec3 center;
    float_t radius;

    obj_bounding_sphere();
    obj_bounding_sphere(vec3 center, float_t radius);
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

    obj_material_shader_lighting_type get_lighting_type() const;
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

    int32_t get_blend() const;
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
    mat4 tex_coord_mat;
    union {
        uint32_t reserved[8];
        int32_t texture_index;
    };

    obj_material_texture_data();
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
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emission;
    float_t shininess;
    float_t intensity;

    obj_material_color();
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

    obj_material();
};

struct obj_material_data {
    int32_t num_of_textures;
    obj_material material;

    obj_material_data();
};

struct obj_sub_mesh_attrib_member {
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    uint32_t transparent : 1;
    uint32_t cloth : 1;
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
    int32_t num_bone_index;
    uint16_t* bone_index_array;
    uint32_t bones_per_vertex;
    obj_primitive_type primitive_type;
    obj_index_format index_format;
    int32_t num_index;
    uint32_t* index_array;
    obj_sub_mesh_attrib attrib;
    union {
        obj_axis_aligned_bounding_box axis_aligned_bounding_box;
        obj_bounding_box bounding_box;
    };
    uint16_t first_index;
    uint16_t last_index;
    uint32_t index_offset;

    obj_sub_mesh();
};

struct obj_vertex_data {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec3 binormal;
    vec2 texcoord0;
    vec2 texcoord1;
    vec2 texcoord2;
    vec2 texcoord3;
    vec4 color0;
    vec4 color1;
    vec4 bone_weight;
    vec4i16 bone_index;
    vec4 unknown;

    obj_vertex_data();
};

struct obj_mesh_attrib_member {
    uint32_t double_buffer : 1;
    uint32_t billboard_y_axis : 1;
    uint32_t translucent_sort_by_radius : 1;
    uint32_t billboard : 1;
    uint32_t dummy : 26; // 28
    uint32_t compression : 2; // Own stuff
};

union obj_mesh_attrib {
    obj_mesh_attrib_member m;
    uint32_t w;
};

struct obj_mesh {
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    int32_t num_submesh;
    obj_sub_mesh* submesh_array;
    obj_vertex_format vertex_format;
    uint32_t size_vertex;
    int32_t num_vertex;
    obj_vertex_data* vertex_array;
    obj_mesh_attrib attrib;
    uint32_t reserved[6];
    char name[0x40];

    obj_mesh();
};

struct obj_skin_block_node {
    const char* parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;

    obj_skin_block_node();
};

struct obj_skin_skin_param_coli {
    obj_skin_skin_param_coli_type type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;

    obj_skin_skin_param_coli();
};

struct obj_skin_skin_param {
    int32_t unk0;
    float_t force;
    float_t force_gain;
    float_t air_res;
    float_t rot_y;
    float_t rot_z;
    float_t hinge_y;
    float_t hinge_z;
    const char* name;
    obj_skin_skin_param_coli* coli;
    int32_t coli_count;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    int32_t unk44;

    obj_skin_skin_param();
};

struct obj_skin_block_cloth_root_bone_weight {
    const char* bone_name;
    float_t weight;
    int32_t matrix_index;
    int32_t reserved;

    obj_skin_block_cloth_root_bone_weight();
};

struct obj_skin_block_cloth_root {
    vec3 pos;
    vec3 normal;
    float_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    obj_skin_block_cloth_root_bone_weight bone_weights[4];

    obj_skin_block_cloth_root();
};

struct obj_skin_block_cloth_node {
    uint32_t flags;
    vec3 pos;
    vec3 delta_pos;
    float_t dist_top;
    float_t dist_bottom;
    float_t dist_right;
    float_t dist_left;

    obj_skin_block_cloth_node();
};

struct obj_skin_block_cloth {
    const char* mesh_name;
    const char* backface_mesh_name;
    uint32_t field_8;
    int32_t num_root;
    int32_t num_node;
    uint32_t field_14;
    mat4* mat_array;
    int32_t num_mat;
    obj_skin_block_cloth_root* root_array;
    obj_skin_block_cloth_node* node_array;
    uint16_t* mesh_index_array;
    int32_t num_mesh_index;
    uint16_t* backface_mesh_index_array;
    int32_t num_backface_mesh_index;
    obj_skin_skin_param* skin_param;
    uint32_t reserved;

    obj_skin_block_cloth();
};

struct obj_skin_block_constraint_attach_point {
    bool affected_by_orientation;
    bool affected_by_scaling;
    vec3 offset;

    obj_skin_block_constraint_attach_point();
};

struct obj_skin_block_constraint_up_vector {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    const char* name;

    obj_skin_block_constraint_up_vector();
};

struct obj_skin_block_constraint_direction {
    obj_skin_block_constraint_up_vector up_vector;
    vec3 align_axis;
    vec3 target_offset;

    obj_skin_block_constraint_direction();
};

struct obj_skin_block_constraint_distance {
    obj_skin_block_constraint_up_vector up_vector;
    float_t distance;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;

    obj_skin_block_constraint_distance();
};

struct obj_skin_block_constraint_orientation {
    vec3 offset;

    obj_skin_block_constraint_orientation();
};

struct obj_skin_block_constraint_position {
    obj_skin_block_constraint_up_vector up_vector;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;

    obj_skin_block_constraint_position();
};

struct obj_skin_block_constraint {
    obj_skin_block_node node;
    uint32_t name_index;
    const char* source_node_name;
    obj_skin_block_constraint_coupling coupling;
    obj_skin_block_constraint_type type;
    union {
        void* data;
        obj_skin_block_constraint_direction* direction;
        obj_skin_block_constraint_distance* distance;
        obj_skin_block_constraint_orientation* orientation;
        obj_skin_block_constraint_position* position;
    };

    obj_skin_block_constraint();
};

struct obj_skin_block_expression {
    obj_skin_block_node node;
    uint32_t name_index;
    int32_t num_expression;
    const char* expression_array[9];

    obj_skin_block_expression();
};

struct obj_skin_motion_node {
    uint32_t name_index;
    mat4 inv_bind_pose_mat;

    obj_skin_motion_node();
};

struct obj_skin_block_motion {
    obj_skin_block_node node;
    union {
        const char* name;
        uint32_t name_index;
    };
    obj_skin_motion_node* node_array;
    int32_t num_node;

    obj_skin_block_motion();
};

struct obj_skin_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;

    obj_skin_osage_node();
};

struct obj_skin_block_osage {
    obj_skin_block_node node;
    uint32_t start_index;
    int32_t count;
    obj_skin_osage_node* node_array;
    int32_t num_node;
    obj_skin_skin_param* skin_param;
    uint32_t external_name_index;
    uint32_t name_index;
    const char* motion_node_name;

    obj_skin_block_osage();
};

struct obj_skin_block {
    obj_skin_block_type type;
    union {
        obj_skin_block_node* node;
        obj_skin_block_cloth* cloth;
        obj_skin_block_constraint* constraint;
        obj_skin_block_expression* expression;
        obj_skin_block_motion* motion;
        obj_skin_block_osage* osage;
    };

    obj_skin_block();
};

struct obj_skin_osage_sibling_info {
    uint32_t name_index;
    uint32_t sibling_name_index;
    float_t max_distance;

    obj_skin_osage_sibling_info();
};

struct obj_skin_ex_data {
    obj_skin_osage_node* osage_node_array;
    int32_t num_osage_node;
    obj_skin_block* block_array;
    int32_t num_block;
    const char** bone_name_array;
    int32_t num_bone_name;
    obj_skin_osage_sibling_info* osage_sibling_info_array;
    int32_t num_osage_sibling_info;
    int64_t reserved[7];

    obj_skin_ex_data();
};

struct obj_skin_bone {
    uint32_t id;
    uint32_t parent;
    mat4 inv_bind_pose_mat;
    const char* name;

    obj_skin_bone();
};

struct obj_skin {
    obj_skin_bone* bone_array;
    int32_t num_bone;
    obj_skin_ex_data* ex_data;

    obj_skin();
};

struct obj {
    obj_bounding_sphere bounding_sphere;
    int32_t num_mesh;
    obj_mesh* mesh_array;
    int32_t num_material;
    obj_material_data* material_array;
    uint8_t flags;
    uint32_t reserved[10];
    obj_skin* skin;
    const char* name;
    uint32_t id;
    uint32_t hash;

    obj();

    obj_mesh* get_obj_mesh(const char* name);
    int32_t get_obj_mesh_index(const char* name);
};

struct obj_set {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    obj** obj_data;
    int32_t obj_num;
    uint32_t* tex_id_data;
    int32_t tex_id_num;
    uint32_t reserved[2];

    obj_set();

    void move_data(obj_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc);
    void pack_file(void** data, size_t* size);
    void unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern);
};
