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

typedef enum obj_index_format {
    OBJ_INDEX_U8  = 0x00,
    OBJ_INDEX_U16 = 0x01,
    OBJ_INDEX_U32 = 0x02,
} obj_index_format;

typedef enum obj_material_aniso_direction {
    OBJ_MATERIAL_ANISO_DIRECTION_NORMAL = 0,
    OBJ_MATERIAL_ANISO_DIRECTION_U      = 1,
    OBJ_MATERIAL_ANISO_DIRECTION_V      = 2,
    OBJ_MATERIAL_ANISO_DIRECTION_RADIAL = 3,
} obj_material_aniso_direction;

typedef enum obj_material_blend_factor {
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
} obj_material_blend_factor;

typedef enum obj_material_bump_map_type {
    OBJ_MATERIAL_BUMP_MAP_NONE = 0,
    OBJ_MATERIAL_BUMP_MAP_DOT  = 1,
    OBJ_MATERIAL_BUMP_MAP_ENV  = 2,
} obj_material_bump_map_type;

typedef enum obj_material_color_source_type {
    OBJ_MATERIAL_COLOR_SOURCE_MATERIAL_COLOR = 0,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_COLOR   = 1,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_MORPH   = 2,
} obj_material_color_source_type;

typedef enum obj_material_flags {
    OBJ_MATERIAL_COLOR          = 0x0001,
    OBJ_MATERIAL_COLOR_ALPHA    = 0x0002,
    OBJ_MATERIAL_COLOR_L1       = 0x0004,
    OBJ_MATERIAL_COLOR_L1_ALPHA = 0x0008,
    OBJ_MATERIAL_COLOR_L2       = 0x0010,
    OBJ_MATERIAL_COLOR_L2_ALPHA = 0x0020,
    OBJ_MATERIAL_TRANSPARENCY   = 0x0040,
    OBJ_MATERIAL_SPECULAR       = 0x0080,
    OBJ_MATERIAL_NORMAL         = 0x0100,
    OBJ_MATERIAL_NORMALALT      = 0x0200,
    OBJ_MATERIAL_ENVIRONMENT    = 0x0400,
    OBJ_MATERIAL_COLOR_L3       = 0x0800,
    OBJ_MATERIAL_COLOR_L3_ALPHA = 0x1000,
    OBJ_MATERIAL_TRANSLUCENCY   = 0x2000,
    OBJ_MATERIAL_FLAG_14        = 0x4000,
    OBJ_MATERIAL_OVERRIDE_IBL   = 0x8000,
} obj_material_flags;

typedef enum obj_material_specular_quality {
    OBJ_MATERIAL_SPECULAR_QUALITY_LOW  = 0,
    OBJ_MATERIAL_SPECULAR_QUALITY_HIGH = 1,
} obj_material_specular_quality;

typedef enum obj_material_texture_type {
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
} obj_material_texture_type;

typedef enum obj_material_texture_coordinate_translation_type {
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_NONE   = 0x00,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_UV     = 0x01,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_SPHERE = 0x02,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_CUBE   = 0x03
} obj_material_texture_coordinate_translation_type;

typedef enum obj_material_vertex_translation_type {
    OBJ_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0,
    OBJ_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 1,
    OBJ_MATERIAL_VERTEX_TRANSLATION_MORPHING = 2,
} obj_material_vertex_translation_type;

typedef enum obj_mesh_flags {
    OBJ_MESH_FLAG_1                = 0x01,
    OBJ_MESH_BILLBOARD_Y_AXIS      = 0x02,
    OBJ_MESH_TRANSLUCENT_NO_SHADOW = 0x04,
    OBJ_MESH_BILLBOARD             = 0x08,
} obj_mesh_flags;

typedef enum obj_primitive_type {
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
} obj_primitive_type;

typedef enum obj_skin_block_constraint_type {
    OBJ_SKIN_BLOCK_CONSTRAINT_NONE = 0,
    OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION,
    OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE,
    OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION,
    OBJ_SKIN_BLOCK_CONSTRAINT_POSITION,
} obj_skin_block_constraint_type;

typedef enum obj_skin_block_constraint_coupling {
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_RIGID = 0x01,
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_SOFT  = 0x03,
} obj_skin_block_constraint_coupling;

typedef enum obj_skin_block_type {
    OBJ_SKIN_BLOCK_NONE = 0,
    OBJ_SKIN_BLOCK_CLOTH,
    OBJ_SKIN_BLOCK_CONSTRAINT,
    OBJ_SKIN_BLOCK_EXPRESSION,
    OBJ_SKIN_BLOCK_ITEM,
    OBJ_SKIN_BLOCK_MOTION,
    OBJ_SKIN_BLOCK_OSAGE,
} obj_skin_block_type;

typedef enum obj_sub_mesh_flags {
    OBJ_SUB_MESH_RECIEVE_SHADOW = 0x01,
    OBJ_SUB_MESH_CAST_SHADOW    = 0x02,
    OBJ_SUB_MESH_TRANSPARENT    = 0x04,
    OBJ_SUB_MESH_FLAG_8         = 0x08,
    OBJ_SUB_MESH_FLAG_10        = 0x10,
    OBJ_SUB_MESH_FLAG_20        = 0x20,
    OBJ_SUB_MESH_FLAG_40        = 0x40,
    OBJ_SUB_MESH_FLAG_80        = 0x80,
} obj_sub_mesh_flags;

typedef enum obj_vertex_flags {
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
} obj_vertex_flags;

typedef struct obj_bounding_box {
    vec3 center;
    vec3 size;
} obj_bounding_box;

typedef struct obj_bounding_sphere {
    vec3 center;
    float_t radius;
} obj_bounding_sphere;

typedef struct objt_material_blend_flags {
    uint32_t alpha_texture : 1;
    uint32_t alpha_material : 1;
    uint32_t punch_through : 1;
    uint32_t double_sided : 1;
    uint32_t normal_direction_light : 1;
    obj_material_blend_factor src_blend_factor : 4;
    obj_material_blend_factor dst_blend_factor : 4;
    uint32_t blend_operation : 3;
    uint32_t z_bias : 4;
    uint32_t no_fog : 1;
    uint32_t flag_21 : 6;
    uint32_t flag_27 : 1;
    uint32_t flag_28 : 1;
    uint32_t flag_29 : 2;
    uint32_t flag_31 : 1;
} obj_material_blend_flags;

typedef struct obj_material_shader_flags {
    obj_material_vertex_translation_type vertex_translation_type : 2;
    obj_material_color_source_type color_source_type : 2;
    uint32_t lambert_shading : 1;
    uint32_t phong_shading : 1;
    uint32_t per_pixel_shading : 1;
    uint32_t double_shading : 1;
    obj_material_bump_map_type bump_map_type : 2;
    uint32_t fresnel : 4;
    uint32_t line_light : 4;
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    obj_material_specular_quality specular_quality : 1;
    obj_material_aniso_direction aniso_direction : 2;
} obj_material_shader_flags;

typedef struct obj_material_texture_flags {
    obj_material_texture_type type : 4;
    uint32_t tex_coord_index : 4;
    obj_material_texture_coordinate_translation_type tex_coord_trans_type : 4;
} obj_material_texture_flags;

typedef struct obj_material_texture_sampler_flags {
    uint32_t repeat_u : 1;
    uint32_t repeat_v : 1;
    uint32_t mirror_u : 1;
    uint32_t mirror_v : 1;
    uint32_t ignore_alpha : 1;
    uint32_t blend : 5;
    uint32_t alpha_blend : 5;
    uint32_t border : 1;
    uint32_t clamp_to_edge : 1;
    uint32_t filter : 3;
    uint32_t mip_map : 2;
    uint32_t mip_map_bias : 7;
    uint32_t flag_29 : 1;
    uint32_t anisotropic_filter : 2;
} obj_material_texture_sampler_flags;

typedef struct obj_material_texture_texture_flags {
    obj_material_texture_type type : 4;
    uint32_t tex_coord_index : 4;
    obj_material_texture_coordinate_translation_type tex_coord_trans_type : 3;
} obj_material_texture_texture_flags;

typedef struct obj_material_texture {
    obj_material_texture_sampler_flags sampler_flags;
    uint32_t texture_id;
    obj_material_texture_texture_flags texture_flags;
    char shader_name[8];
    float_t weight;
    mat4u tex_coord_mat;
    int32_t reserved[8];
} obj_material_texture;

typedef struct obj_material {
    obj_material_flags flags;
    char shader_name[8];
    obj_material_shader_flags shader_flags;
    obj_material_texture textures[8];
    obj_material_blend_flags blend_flags;
    vec4u diffuse;
    vec4u ambient;
    vec4u specular;
    vec4u emission;
    float_t shininess;
    float_t intensity;
    obj_bounding_sphere reserved_sphere;
    char name[64];
    float_t bump_depth;
    int32_t reserved[15];
} obj_material;

typedef struct obj_material_data {
    uint32_t textures_count;
    obj_material material;
} obj_material_data;

typedef struct obj_sub_mesh {
    obj_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t texcoord_indices[8];
    uint16_t* bone_indices;
    int32_t bone_indices_count;
    uint32_t bones_per_vertex;
    obj_primitive_type primitive_type;
    obj_index_format index_format;
    uint32_t* indices;
    int32_t indices_count;
    obj_sub_mesh_flags flags;
    uint32_t indices_offset;
    obj_bounding_box bounding_box;
} obj_sub_mesh;

typedef struct obj_vertex_data {
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
} obj_vertex_data;

typedef struct obj_mesh {
    obj_bounding_sphere bounding_sphere;
    obj_sub_mesh* sub_meshes;
    int32_t sub_meshes_count;
    obj_vertex_data* vertex;
    int32_t vertex_count;
    obj_vertex_flags vertex_flags;
    obj_mesh_flags flags;
    char name[0x40];
} obj_mesh;

typedef struct obj_skin_block_cloth_field_1C_sub {
    string bone_name;
    float_t weight;
    uint32_t matrix_index;
    uint32_t field_0C;
} obj_skin_block_cloth_field_1C_sub;

typedef struct obj_skin_block_cloth_field_1C {
    float_t field_00;
    float_t field_04;
    float_t field_08;
    float_t field_0C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    uint32_t field_1C;
    uint32_t field_20;
    uint32_t field_24;
    obj_skin_block_cloth_field_1C_sub sub_data_0;
    obj_skin_block_cloth_field_1C_sub sub_data_1;
    obj_skin_block_cloth_field_1C_sub sub_data_2;
    obj_skin_block_cloth_field_1C_sub sub_data_3;
} obj_skin_block_cloth_field_1C;

typedef struct obj_skin_block_cloth_field_20_sub {
    uint32_t field_00;
    float_t field_04;
    float_t field_08;
    float_t field_0C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
    float_t field_28;
} obj_skin_block_cloth_field_20_sub;

typedef struct obj_skin_block_cloth_field_20 {
    obj_skin_block_cloth_field_20_sub sub_data_0;
    obj_skin_block_cloth_field_20_sub sub_data_1;
    obj_skin_block_cloth_field_20_sub sub_data_2;
    obj_skin_block_cloth_field_20_sub sub_data_3;
    obj_skin_block_cloth_field_20_sub sub_data_4;
    obj_skin_block_cloth_field_20_sub sub_data_5;
    obj_skin_block_cloth_field_20_sub sub_data_6;
    obj_skin_block_cloth_field_20_sub sub_data_7;
    obj_skin_block_cloth_field_20_sub sub_data_8;
    obj_skin_block_cloth_field_20_sub sub_data_9;
} obj_skin_block_cloth_field_20;

typedef struct obj_skin_block_node {
    string parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
} obj_skin_block_node;

typedef struct obj_skin_block_cloth {
    string mesh_name;
    string backface_mesh_name;
    uint32_t field_08;
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
} obj_skin_block_cloth;

typedef struct obj_skin_block_constraint_attach_point {
    bool affected_by_orientation;
    bool affected_by_scaling;
    vec3 offset;
} obj_skin_block_constraint_attach_point;

typedef struct obj_skin_block_constraint_up_vector {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    string name;
} obj_skin_block_constraint_up_vector;

typedef struct obj_skin_block_constraint_direction {
    obj_skin_block_constraint_up_vector up_vector;
    vec3 align_axis;
    vec3 target_offset;
} obj_skin_block_constraint_direction;

typedef struct obj_skin_block_constraint_distance {
    obj_skin_block_constraint_up_vector up_vector;
    float_t distance;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
} obj_skin_block_constraint_distance;

typedef struct obj_skin_block_constraint_orientation {
    vec3 offset;
} obj_skin_block_constraint_orientation;

typedef struct obj_skin_block_constraint_position {
    obj_skin_block_constraint_up_vector up_vector;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
} obj_skin_block_constraint_position;

typedef struct obj_skin_block_constraint {
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
} obj_skin_block_constraint;

typedef struct obj_skin_block_expression {
    obj_skin_block_node base;
    uint32_t name_index;
    int32_t expressions_count;
    string expressions[9];
} obj_skin_block_expression;

typedef struct obj_skin_motion_node {
    uint32_t name_index;
    mat4u transformation;
} obj_skin_motion_node;

typedef struct obj_skin_block_motion {
    obj_skin_block_node base;
    union {
        string name;
        uint32_t name_index;
    };
    obj_skin_motion_node* nodes;
    int32_t nodes_count;
} obj_skin_block_motion;

typedef struct obj_skin_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;
} obj_skin_osage_node;

typedef struct obj_skin_block_osage {
    obj_skin_block_node base;
    int32_t start_index;
    int32_t count;
    obj_skin_osage_node* nodes;
    int32_t nodes_count;
    uint32_t external_name_index;
    uint32_t name_index;
    string motion_node_name;
} obj_skin_block_osage;

typedef struct obj_skin_block {
    obj_skin_block_type type;
    union {
        obj_skin_block_cloth cloth;
        obj_skin_block_constraint constraint;
        obj_skin_block_expression expression;
        obj_skin_block_motion motion;
        obj_skin_block_osage osage;
    };
} obj_skin_block;

typedef struct obj_skin_osage_sibling_info {
    uint32_t name_index;
    uint32_t sibling_name_index;
    float_t distance;
} obj_skin_osage_sibling_info;

typedef struct obj_skin_ex_data {
    obj_skin_osage_node* osage_nodes;
    int32_t osage_nodes_count;
    obj_skin_block* blocks;
    int32_t blocks_count;
    char* bone_names_buf;
    char** bone_names;
    int32_t bone_names_count;
    obj_skin_osage_sibling_info* osage_sibling_infos;
    int32_t osage_sibling_infos_count;
} obj_skin_ex_data;

typedef struct obj_skin_bone {
    uint32_t id;
    uint32_t parent;
    mat4u inv_bind_pose_mat;
    string name;
} obj_skin_bone;

typedef struct obj_skin {
    obj_skin_bone* bones;
    int32_t bones_count;
    obj_skin_ex_data ex_data;
    bool ex_data_init;
} obj_skin;

typedef struct obj {
    obj_bounding_sphere bounding_sphere;
    obj_mesh* meshes;
    int32_t meshes_count;
    obj_material_data* materials;
    int32_t materials_count;
    uint8_t flags;
    obj_skin skin;
    bool skin_init;
    string name;
    uint32_t id;
} obj;

typedef struct obj_set {
    bool ready;
    bool modern;
    bool is_x;

    obj* objects;
    int32_t objects_count;
    uint32_t* texture_ids;
    int32_t texture_ids_count;
} obj_set;

extern void obj_init(obj_set* os);
extern void obj_read(obj_set* os, char* path, bool modern);
extern void obj_wread(obj_set* os, wchar_t* path, bool modern);
extern void obj_mread(obj_set* os, void* data, size_t length, bool modern);
extern void obj_write(obj_set* os, char* path);
extern void obj_wwrite(obj_set* os, wchar_t* path);
extern void obj_mwrite(obj_set* os, void** data, size_t* length);
extern void obj_free(obj_set* os);
