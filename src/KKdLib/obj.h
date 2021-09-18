/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "kf.h"
#include "vec.h"
#include "vector.h"

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
    OBJ_SKIN_BLOCK_MOTION,
    OBJ_SKIN_BLOCK_OSAGE,
} obj_skin_block_type;

typedef enum obj_index_format {
    OBJ_INDEX_U8  = 0x00,
    OBJ_INDEX_U16 = 0x01,
    OBJ_INDEX_U32 = 0x02,
} obj_index_format;

typedef enum obj_mesh_flags {
    OBJ_MESH_FACE_CAMERA_POSITION  = 0x02,
    OBJ_MESH_TRANSLUCENT_NO_SHADOW = 0x04,
    OBJ_MESH_FACE_CAMERA_VIEW      = 0x08,
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

typedef enum obj_sub_mesh_flags {
    OBJ_SUB_MESH_NONE           = 0x01,
    OBJ_SUB_MESH_RECIEVE_SHADOW = 0x01,
    OBJ_SUB_MESH_CAST_SHADOW    = 0x02,
    OBJ_SUB_MESH_TRANSPARENT    = 0x04,
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
    OBJ_MATERIAL_FLAG14         = 0x4000,
    OBJ_MATERIAL_OVERRIDEIBL    = 0x8000,
} obj_material_flags;

typedef enum obj_material_specular_quality {
    OBJ_MATERIAL_SPECULAR_QUALITY_LOW  = 0,
    OBJ_MATERIAL_SPECULAR_QUALITY_HIGH = 1,
} obj_material_specular_quality;

typedef enum obj_material_vertex_translation_type {
    OBJ_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0,
    OBJ_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 1,
    OBJ_MATERIAL_VERTEX_TRANSLATION_MORPHING = 2,
} obj_material_vertex_translation_type;

typedef struct obj_bounding_box {
    vec3 center;
    vec3 size;
} obj_bounding_box;

typedef struct obj_bounding_sphere {
    vec3 center;
    float_t radius;
} obj_bounding_sphere;

typedef struct obj_material_texture {
    uint32_t sampler_flags;
    uint32_t texture_id;
    uint32_t texture_flags;
    char shader_name[8];
    float_t weight;
    float_t tex_coord_mat[16];
    int32_t reserved[8];
} obj_material_texture;

typedef struct obj_material {
    obj_material_flags flags;
    char shader_name[8];
    uint32_t shader_flags;
    obj_material_texture textures[8];
    uint32_t blend_flags;
    float_t diffuse[4];
    float_t ambient[4];
    float_t specular[4];
    float_t emission[4];
    float_t shininess;
    float_t intensity;
    obj_bounding_sphere reserved_sphere;
    char name[64];
    float_t bump_depth;
    int32_t reserved[15];
} obj_material;

typedef struct obj_material_parent {
    uint32_t textures_count;
    obj_material data;
} obj_material_parent;

vector(obj_material_parent)

typedef struct obj_sub_mesh {
    obj_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t texcoord_indices[8];
    uint16_t* bone_index;
    int32_t bone_index_count;
    uint32_t bones_per_vertex;
    obj_primitive_type primitive_type;
    obj_index_format index_format;
    uint32_t* indices;
    int32_t indices_count;
    obj_sub_mesh_flags flags;
    uint32_t index_offset;
    obj_bounding_box bounding_box;
} obj_sub_mesh;

vector(obj_sub_mesh)

typedef struct obj_vertex_data { // Optimized to memory
    vec3 position;
    vec3 normal;
    vec3 binormal;
    vec3 dummy;
    vec2 texcoord0;
    vec2 texcoord1;
    vec2 texcoord2;
    vec2 texcoord3;
    vec4 tangent;
    vec4 color0;
    vec4 color1;
    vec4 bone_weight;
    vec4i bone_index;
    vec4 unknown;
    /*vec3 position;
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
    vec4i bone_index;
    vec4 unknown;*/
} obj_vertex_data;

typedef struct obj_mesh {
    obj_bounding_sphere bounding_sphere;
    vector_obj_sub_mesh sub_meshes;
    obj_vertex_data* vertex;
    int32_t vertex_count;
    obj_vertex_flags vertex_flags;
    obj_mesh_flags flags;
    char name[0x40];
} obj_mesh;

vector(obj_mesh)

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

vector(obj_skin_block_cloth_field_1C)
vector(obj_skin_block_cloth_field_20)

typedef struct obj_skin_block_node {
    string parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
    string name;
} obj_skin_block_node;

typedef struct obj_skin_block_cloth {
    string mesh_name;
    string backface_mesh_name;
    uint32_t field_08;
    uint32_t field_10;
    uint32_t field_14;
    mat4 field_18[32];
    vector_obj_skin_block_cloth_field_1C field_1C;
    vector_obj_skin_block_cloth_field_20 field_20;
    vector_uint16_t field_24;
    vector_uint16_t field_28;
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
    obj_skin_block_constraint_coupling coupling;
    string source_node_name;
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
    int32_t expression_count;
    string expressions[9];
} obj_skin_block_expression;

typedef struct obj_skin_motion_node {
    string name;
    mat4 transformation;
} obj_skin_motion_node;

vector(obj_skin_motion_node)

typedef struct obj_skin_block_motion {
    obj_skin_block_node base;
    vector_obj_skin_motion_node nodes;
} obj_skin_block_motion;

typedef struct obj_skin_osage_node {
    string name;
    float_t length;
    vec3 rotation;
    string sibling_name;
    float_t sibling_max_distance;
} obj_skin_osage_node;

vector(obj_skin_osage_node)

typedef struct obj_skin_block_osage {
    obj_skin_block_node base;
    int32_t start_index;
    int32_t count;
    vector_obj_skin_osage_node nodes;
    string external_name;
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

vector(obj_skin_block)

typedef struct obj_skin_ex_data {
    vector_obj_skin_osage_node osage_nodes;
    vector_obj_skin_block blocks;
} obj_skin_ex_data;

typedef struct obj_skin_bone {
    uint32_t id;
    mat4 inv_bind_pose_mat;
    string name;
    uint32_t parent;
} obj_skin_bone;

vector(obj_skin_bone)

typedef struct obj_skin {
    vector_obj_skin_bone bones;
    obj_skin_ex_data ex_data;
    bool ex_data_init;
} obj_skin;

typedef struct obj {
    obj_bounding_sphere bounding_sphere;
    vector_obj_mesh meshes;
    vector_obj_material_parent materials;
    uint8_t flags;
    obj_skin skin;
    bool skin_init;
    string name;
    uint32_t id;
} obj;

vector(obj)

typedef struct obj_set {
    bool ready;
    bool modern;
    bool is_x;
    vector_obj vec;
    vector_uint32_t tex_ids;
} obj_set;

extern void obj_init(obj_set* os);
extern void obj_read(obj_set* os, char* path, bool modern);
extern void obj_wread(obj_set* os, wchar_t* path, bool modern);
extern void obj_mread(obj_set* os, void* data, size_t length, bool modern);
extern void obj_write(obj_set* os, char* path);
extern void obj_wwrite(obj_set* os, wchar_t* path);
extern void obj_mwrite(obj_set* os, void** data, size_t* length);
extern void obj_free(obj_set* os);
