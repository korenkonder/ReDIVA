/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/object.h"
#include "../KKdLib/database/texture.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/obj.h"
#include "../KKdLib/string.h"
#include "../KKdLib/vec.h"
#include "static_var.h"
#include "texture.h"

typedef enum object_index_format : uint32_t {
    OBJECT_INDEX_U8  = 0x00,
    OBJECT_INDEX_U16 = 0x01,
    OBJECT_INDEX_U32 = 0x02,
} object_index_format;

typedef enum object_material_aniso_direction : uint32_t {
    OBJECT_MATERIAL_ANISO_DIRECTION_NORMAL = 0,
    OBJECT_MATERIAL_ANISO_DIRECTION_U      = 1,
    OBJECT_MATERIAL_ANISO_DIRECTION_V      = 2,
    OBJECT_MATERIAL_ANISO_DIRECTION_RADIAL = 3,
} object_material_aniso_direction;

typedef enum object_material_blend_factor : uint32_t {
    OBJECT_MATERIAL_BLEND_ZERO              = 0,
    OBJECT_MATERIAL_BLEND_ONE               = 1,
    OBJECT_MATERIAL_BLEND_SRC_COLOR         = 2,
    OBJECT_MATERIAL_BLEND_INVERSE_SRC_COLOR = 3,
    OBJECT_MATERIAL_BLEND_SRC_ALPHA         = 4,
    OBJECT_MATERIAL_BLEND_INVERSE_SRC_ALPHA = 5,
    OBJECT_MATERIAL_BLEND_DST_ALPHA         = 6,
    OBJECT_MATERIAL_BLEND_INVERSE_DST_ALPHA = 7,
    OBJECT_MATERIAL_BLEND_DST_COLOR         = 8,
    OBJECT_MATERIAL_BLEND_INVERSE_DST_COLOR = 9,
    OBJECT_MATERIAL_BLEND_ALPHA_SATURATE    = 10,
} object_material_blend_factor;

typedef enum object_material_bump_map_type : uint32_t {
    OBJECT_MATERIAL_BUMP_MAP_NONE = 0,
    OBJECT_MATERIAL_BUMP_MAP_DOT  = 1,
    OBJECT_MATERIAL_BUMP_MAP_ENV  = 2,
} object_material_bump_map_type;

typedef enum object_material_color_source_type : uint32_t {
    OBJECT_MATERIAL_COLOR_SOURCE_MATERIAL_COLOR = 0,
    OBJECT_MATERIAL_COLOR_SOURCE_VERTEX_COLOR   = 1,
    OBJECT_MATERIAL_COLOR_SOURCE_VERTEX_MORPH   = 2,
} object_material_color_source_type;

typedef enum object_material_flags : uint32_t {
    OBJECT_MATERIAL_COLOR          = 0x0001,
    OBJECT_MATERIAL_COLOR_ALPHA    = 0x0002,
    OBJECT_MATERIAL_COLOR_L1       = 0x0004,
    OBJECT_MATERIAL_COLOR_L1_ALPHA = 0x0008,
    OBJECT_MATERIAL_COLOR_L2       = 0x0010,
    OBJECT_MATERIAL_COLOR_L2_ALPHA = 0x0020,
    OBJECT_MATERIAL_TRANSPARENCY   = 0x0040,
    OBJECT_MATERIAL_SPECULAR       = 0x0080,
    OBJECT_MATERIAL_NORMAL         = 0x0100,
    OBJECT_MATERIAL_NORMALALT      = 0x0200,
    OBJECT_MATERIAL_ENVIRONMENT    = 0x0400,
    OBJECT_MATERIAL_COLOR_L3       = 0x0800,
    OBJECT_MATERIAL_COLOR_L3_ALPHA = 0x1000,
    OBJECT_MATERIAL_TRANSLUCENCY   = 0x2000,
    OBJECT_MATERIAL_FLAG_14        = 0x4000,
    OBJECT_MATERIAL_OVERRIDE_IBL   = 0x8000,
} object_material_flags;

typedef enum object_material_specular_quality : uint32_t {
    OBJECT_MATERIAL_SPECULAR_QUALITY_LOW  = 0,
    OBJECT_MATERIAL_SPECULAR_QUALITY_HIGH = 1,
} object_material_specular_quality;

typedef enum object_material_texture_type : uint32_t {
    OBJECT_MATERIAL_TEXTURE_NONE               = 0x00,
    OBJECT_MATERIAL_TEXTURE_COLOR              = 0x01,
    OBJECT_MATERIAL_TEXTURE_NORMAL             = 0x02,
    OBJECT_MATERIAL_TEXTURE_SPECULAR           = 0x03,
    OBJECT_MATERIAL_TEXTURE_HEIGHT             = 0x04,
    OBJECT_MATERIAL_TEXTURE_REFLECTION         = 0x05,
    OBJECT_MATERIAL_TEXTURE_TRANSLUCENCY       = 0x06,
    OBJECT_MATERIAL_TEXTURE_TRANSPARENCY       = 0x07,
    OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE = 0x08,
    OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_CUBE   = 0x09,
} object_material_texture_type;

typedef enum object_material_texture_coordinate_translation_type : uint32_t {
    OBJECT_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_NONE   = 0x00,
    OBJECT_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_UV     = 0x01,
    OBJECT_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_SPHERE = 0x02,
    OBJECT_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_CUBE   = 0x03
} object_material_texture_coordinate_translation_type;

typedef enum object_material_shader_lighting_type : uint32_t {
    OBJECT_MATERIAL_SHADER_LIGHTING_LAMBERT  = 0x0,
    OBJECT_MATERIAL_SHADER_LIGHTING_CONSTANT = 0x1,
    OBJECT_MATERIAL_SHADER_LIGHTING_PHONG    = 0x2,
} object_material_shader_lighting_type;

typedef enum object_material_vertex_translation_type : uint32_t {
    OBJECT_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0,
    OBJECT_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 1,
    OBJECT_MATERIAL_VERTEX_TRANSLATION_MORPHING = 2,
} object_material_vertex_translation_type;

typedef enum object_mesh_flags : uint32_t {
    OBJECT_MESH_FLAG_1                = 0x01,
    OBJECT_MESH_BILLBOARD_Y_AXIS      = 0x02,
    OBJECT_MESH_TRANSLUCENT_NO_SHADOW = 0x04,
    OBJECT_MESH_BILLBOARD             = 0x08,
} object_mesh_flags;

typedef enum object_primitive_type : uint32_t {
    OBJECT_PRIMITIVE_POINTS         = 0x00,
    OBJECT_PRIMITIVE_LINES          = 0x01,
    OBJECT_PRIMITIVE_LINE_STRIP     = 0x02,
    OBJECT_PRIMITIVE_LINE_LOOP      = 0x03,
    OBJECT_PRIMITIVE_TRIANGLES      = 0x04,
    OBJECT_PRIMITIVE_TRIANGLE_STRIP = 0x05,
    OBJECT_PRIMITIVE_TRIANGLE_FAN   = 0x06,
    OBJECT_PRIMITIVE_QUADS          = 0x07,
    OBJECT_PRIMITIVE_QUAD_STRIP     = 0x08,
    OBJECT_PRIMITIVE_POLYGON        = 0x09,
} object_primitive_type;

typedef enum object_skin_block_constraint_type : uint32_t {
    OBJECT_SKIN_BLOCK_CONSTRAINT_NONE = 0,
    OBJECT_SKIN_BLOCK_CONSTRAINT_ORIENTATION,
    OBJECT_SKIN_BLOCK_CONSTRAINT_DIRECTION,
    OBJECT_SKIN_BLOCK_CONSTRAINT_POSITION,
    OBJECT_SKIN_BLOCK_CONSTRAINT_DISTANCE,
} object_skin_block_constraint_type;

typedef enum object_skin_block_constraint_coupling : uint32_t {
    OBJECT_SKIN_BLOCK_CONSTRAINT_COUPLING_RIGID = 0x01,
    OBJECT_SKIN_BLOCK_CONSTRAINT_COUPLING_SOFT  = 0x03,
} object_skin_block_constraint_coupling;

typedef enum object_skin_block_type : uint32_t {
    OBJECT_SKIN_BLOCK_NONE = 0,
    OBJECT_SKIN_BLOCK_CLOTH,
    OBJECT_SKIN_BLOCK_CONSTRAINT,
    OBJECT_SKIN_BLOCK_EXPRESSION,
    OBJECT_SKIN_BLOCK_MOTION,
    OBJECT_SKIN_BLOCK_OSAGE,
} object_skin_block_type;

typedef enum object_sub_mesh_flags : uint32_t {
    OBJECT_SUB_MESH_RECIEVE_SHADOW = 0x01,
    OBJECT_SUB_MESH_CAST_SHADOW    = 0x02,
    OBJECT_SUB_MESH_TRANSPARENT    = 0x04,
    OBJECT_SUB_MESH_FLAG_8         = 0x08,
    OBJECT_SUB_MESH_FLAG_10        = 0x10,
    OBJECT_SUB_MESH_FLAG_20        = 0x20,
    OBJECT_SUB_MESH_FLAG_40        = 0x40,
    OBJECT_SUB_MESH_FLAG_80        = 0x80,
} object_sub_mesh_flags;

typedef enum object_vertex_flags : uint32_t {
    OBJECT_VERTEX_NONE      = 0x000,
    OBJECT_VERTEX_POSITION  = 0x001,
    OBJECT_VERTEX_NORMAL    = 0x002,
    OBJECT_VERTEX_TANGENT   = 0x004,
    OBJECT_VERTEX_BINORMAL  = 0x008,
    OBJECT_VERTEX_TEXCOORD0 = 0x010,
    OBJECT_VERTEX_TEXCOORD1 = 0x020,
    OBJECT_VERTEX_TEXCOORD2 = 0x040,
    OBJECT_VERTEX_TEXCOORD3 = 0x080,
    OBJECT_VERTEX_COLOR0    = 0x100,
    OBJECT_VERTEX_COLOR1    = 0x200,
    OBJECT_VERTEX_BONE_DATA = 0x400,
    OBJECT_VERTEX_UNKNOWN   = 0x800,
} object_vertex_flags;

typedef struct object_axis_aligned_bounding_box {
    vec3 center;
    vec3 size;
} object_axis_aligned_bounding_box;

typedef struct object_bounding_box {
    vec3 center;
    vec3 size;
} object_bounding_box;

typedef struct object_bounding_sphere {
    vec3 center;
    float_t radius;
} object_bounding_sphere;

typedef struct object_material_blend_flags {
    uint32_t alpha_texture : 1;
    uint32_t alpha_material : 1;
    uint32_t punch_through : 1;
    uint32_t double_sided : 1;
    uint32_t normal_direction_light : 1;
    object_material_blend_factor src_blend_factor : 4;
    object_material_blend_factor dst_blend_factor : 4;
    uint32_t blend_operation : 3;
    uint32_t z_bias : 4;
    uint32_t no_fog : 1;
    uint32_t translucent_priority : 6;
    uint32_t has_fog_height : 1;
    uint32_t flag_28 : 1;
    uint32_t fog_height : 1;
    uint32_t flag_30 : 1;
    uint32_t flag_31 : 1;
} object_material_blend_flags;

typedef struct object_material_shader_flags {
    object_material_vertex_translation_type vertex_translation_type : 2;
    object_material_color_source_type color_source_type : 2;
    uint32_t lambert_shading : 1;
    uint32_t phong_shading : 1;
    uint32_t per_pixel_shading : 1;
    uint32_t double_shading : 1;
    object_material_bump_map_type bump_map_type : 2;
    uint32_t fresnel : 4;
    uint32_t line_light : 4;
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    object_material_specular_quality specular_quality : 1;
    object_material_aniso_direction aniso_direction : 2;
} object_material_shader_flags;

typedef struct object_material_texture_sampler_flags {
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
} object_material_texture_sampler_flags;

typedef struct object_material_texture_texture_flags {
    object_material_texture_type type : 4;
    uint32_t tex_coord_index : 4;
    object_material_texture_coordinate_translation_type tex_coord_trans_type : 3;
} object_material_texture_texture_flags;

typedef struct object_material_texture {
    object_material_texture_sampler_flags sampler_flags;
    uint32_t texture_id;
    int32_t texture_index;
    object_material_texture_texture_flags texture_flags;
    char shader_name[8];
    float_t weight;
    mat4u tex_coord_mat;
} object_material_texture;

typedef struct object_material {
    object_material_flags flags;
    int32_t shader_index;
    object_material_shader_flags shader_flags;
    object_material_texture textures[8];
    vec4u diffuse;
    vec4u ambient;
    vec4u specular;
    vec4u emission;
    object_material_blend_flags blend_flags;
    float_t shininess;
    float_t intensity;
    object_bounding_sphere reserved_sphere;
    char name[64];
    float_t bump_depth;
} object_material;

typedef struct object_material_data {
    uint32_t textures_count;
    object_material material;
} object_material_data;

typedef struct object_sub_mesh {
    object_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t texcoord_indices[8];
    uint16_t* bone_indices;
    int32_t bone_indices_count;
    uint32_t bones_per_vertex;
    object_primitive_type primitive_type;
    object_index_format index_format;
    int32_t indices_count;
    object_sub_mesh_flags flags;
    object_axis_aligned_bounding_box axis_aligned_bounding_box;
    uint16_t first_index;
    uint16_t last_index;
    uint32_t indices_offset;
} object_sub_mesh;

typedef struct object_mesh {
    object_bounding_sphere bounding_sphere;
    object_sub_mesh* sub_meshes;
    int32_t sub_meshes_count;
    bool compressed;
    void* vertex;
    int32_t vertex_count;
    int32_t vertex_size;
    uint16_t* indices;
    int32_t indices_count;
    object_vertex_flags vertex_flags;
    object_mesh_flags flags;
    char name[0x40];
} object_mesh;

typedef struct object_skin_block_cloth_field_1C_sub {
    string bone_name;
    float_t weight;
    uint32_t matrix_index;
    uint32_t field_0C;
} object_skin_block_cloth_field_1C_sub;

typedef struct object_skin_block_cloth_field_1C {
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
    object_skin_block_cloth_field_1C_sub sub_data_0;
    object_skin_block_cloth_field_1C_sub sub_data_1;
    object_skin_block_cloth_field_1C_sub sub_data_2;
    object_skin_block_cloth_field_1C_sub sub_data_3;
} object_skin_block_cloth_field_1C;

typedef struct object_skin_block_cloth_field_20_sub {
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
} object_skin_block_cloth_field_20_sub;

typedef struct object_skin_block_cloth_field_20 {
    object_skin_block_cloth_field_20_sub sub_data_0;
    object_skin_block_cloth_field_20_sub sub_data_1;
    object_skin_block_cloth_field_20_sub sub_data_2;
    object_skin_block_cloth_field_20_sub sub_data_3;
    object_skin_block_cloth_field_20_sub sub_data_4;
    object_skin_block_cloth_field_20_sub sub_data_5;
    object_skin_block_cloth_field_20_sub sub_data_6;
    object_skin_block_cloth_field_20_sub sub_data_7;
    object_skin_block_cloth_field_20_sub sub_data_8;
    object_skin_block_cloth_field_20_sub sub_data_9;
} object_skin_block_cloth_field_20;

typedef struct object_skin_block_node {
    string parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
} object_skin_block_node;

typedef struct object_skin_block_cloth {
    string mesh_name;
    string backface_mesh_name;
    uint32_t field_08;
    uint32_t count;
    uint32_t field_10;
    uint32_t field_14;
    mat4u field_18[32];
    object_skin_block_cloth_field_1C* field_1C;
    object_skin_block_cloth_field_20* field_20;
    uint16_t* field_24;
    uint32_t field_24_count;
    uint16_t* field_28;
    uint32_t field_28_count;
    uint32_t field_2C;
    uint32_t field_30;
} object_skin_block_cloth;

typedef struct object_skin_block_constraint_attach_point  {
    bool affected_by_orientation;
    bool affected_by_scaling;
    vec3 offset;
} object_skin_block_constraint_attach_point;

typedef struct object_skin_block_constraint_up_vector_old {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    string name;
} object_skin_block_constraint_up_vector_old;

typedef struct object_skin_block_constraint_direction {
    object_skin_block_constraint_up_vector_old up_vector_old;
    vec3 align_axis;
    vec3 target_offset;
} object_skin_block_constraint_direction;

typedef struct object_skin_block_constraint_distance {
    object_skin_block_constraint_up_vector_old up_vector_old;
    float_t distance;
    object_skin_block_constraint_attach_point constrained_object;
    object_skin_block_constraint_attach_point constraining_object;
} object_skin_block_constraint_distance;

typedef struct object_skin_block_constraint_orientation {
    vec3 offset;
} object_skin_block_constraint_orientation;

typedef struct object_skin_block_constraint_position {
    object_skin_block_constraint_up_vector_old up_vector_old;
    object_skin_block_constraint_attach_point constrained_object;
    object_skin_block_constraint_attach_point constraining_object;
} object_skin_block_constraint_position;

typedef struct object_skin_block_constraint {
    object_skin_block_node base;
    object_skin_block_constraint_coupling coupling;
    uint32_t name_index;
    string source_node_name;
    object_skin_block_constraint_type type;
    union {
        object_skin_block_constraint_direction direction;
        object_skin_block_constraint_distance distance;
        object_skin_block_constraint_orientation orientation;
        object_skin_block_constraint_position position;
    };
} object_skin_block_constraint;

typedef struct object_skin_block_expression {
    object_skin_block_node base;
    uint32_t name_index;
    int32_t expressions_count;
    string expressions[9];
} object_skin_block_expression;

typedef struct object_skin_motion_node {
    uint32_t name_index;
    mat4u transformation;
} object_skin_motion_node;

typedef struct object_skin_block_motion {
    object_skin_block_node base;
    union {
        string name;
        uint32_t name_index;
    };
    object_skin_motion_node* nodes;
    int32_t nodes_count;
} object_skin_block_motion;

typedef struct object_skin_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;
} object_skin_osage_node;

typedef struct object_skin_block_osage {
    object_skin_block_node base;
    int32_t start_index;
    int32_t count;
    object_skin_osage_node* nodes;
    int32_t nodes_count;
    uint32_t external_name_index;
    uint32_t name_index;
    string motion_node_name;
} object_skin_block_osage;

typedef struct object_skin_block {
    object_skin_block_type type;
    union {
        object_skin_block_node base;
        object_skin_block_cloth cloth;
        object_skin_block_constraint constraint;
        object_skin_block_expression expression;
        object_skin_block_motion motion;
        object_skin_block_osage osage;
    };
} object_skin_block;

typedef struct object_skin_osage_sibling_info {
    uint32_t name_index;
    uint32_t sibling_name_index;
    float_t max_distance;
} object_skin_osage_sibling_info;

typedef struct object_skin_ex_data {
    object_skin_osage_node* osage_nodes;
    int32_t osage_nodes_count;
    object_skin_block* blocks;
    int32_t blocks_count;
    char* bone_names_buf;
    char** bone_names;
    int32_t bone_names_count;
    object_skin_osage_sibling_info* osage_sibling_infos;
    int32_t osage_sibling_infos_count;
} object_skin_ex_data;

typedef struct object_skin_bone {
    uint32_t id;
    mat4u inv_bind_pose_mat;
    string name;
    uint32_t parent;
} object_skin_bone;

typedef struct object_skin {
    object_skin_bone* bones;
    int32_t bones_count;
    object_skin_ex_data ex_data;
    bool ex_data_init;
} object_skin;

typedef struct object {
    object_bounding_sphere bounding_sphere;
    object_mesh* meshes;
    int32_t meshes_count;
    object_material_data* materials;
    int32_t materials_count;
    uint8_t flags;
    object_skin skin;
    bool skin_init;
    string name;
    uint32_t id;
    uint32_t hash;
} object;

typedef GLuint object_mesh_index_buffer;

typedef struct object_mesh_vertex_buffer {
    int32_t count;
    GLuint buffers[3];
    int32_t index;
} object_mesh_vertex_buffer;

typedef struct object_index_buffer {
    int32_t meshes_count;
    object_mesh_index_buffer* meshes;
} object_index_buffer;

typedef struct object_vertex_buffer {
    int32_t meshes_count;
    object_mesh_vertex_buffer* meshes;
} object_vertex_buffer;

typedef struct object_set {
    string name;
    uint32_t id;
    uint32_t hash;
    object* objects;
    int32_t objects_count;
    uint32_t* texture_ids;
    int32_t texture_ids_count;
    string* texture_names;
    int32_t texture_names_count;
    uint32_t* textures;
    int32_t textures_count;
    texture** texture_data;
    object_vertex_buffer* vertex_buffers;
    int32_t vertex_buffers_count;
    object_index_buffer* index_buffers;
    int32_t index_buffers_count;
    bool is_x;
} object_set;

extern void object_set_init(object_set* set);
extern void object_set_load(object_set* set, obj_set* obj_set_file, txp_set* txp_set_file,
    texture_database* tex_db, const char* name, uint32_t id, bool compressed);
extern bool object_set_load_db_entry(object_set_info** set_info,
    void* data, object_database* obj_db, char* name);
extern bool object_set_load_db_entry(object_set_info** set_info,
    void* data, object_database* obj_db, const char* name);
extern bool object_set_load_by_db_entry(object_set_info* set_info,
    void* data, object_database* obj_db);
extern bool object_set_load_by_db_index(object_set_info** set_info,
    void* data, object_database* obj_db, uint32_t set_id);
extern bool object_set_load_by_hash(void* data,
    object_database* obj_db, texture_database* tex_db, uint32_t hash);
extern void object_set_free(object_set* set);

extern object_material_shader_lighting_type object_material_shader_get_lighting_type(
    object_material_shader_flags* flags);
extern int32_t object_material_texture_get_blend(object_material_texture* tex);
extern int32_t object_material_texture_type_get_texcoord_index(
    object_material_texture_type type, int32_t index);
extern int32_t object_material_texture_type_get_texture_index(
    object_material_texture_type type, int32_t base_index);
extern GLuint object_mesh_vertex_buffer_get_buffer(object_mesh_vertex_buffer* buffer);
extern int32_t object_mesh_vertex_buffer_get_size(object_mesh_vertex_buffer* buffer);
extern void object_skin_set_matrix_buffer(object_skin* s, mat4* matrices,
    mat4* ex_data_matrices, mat4* matrix_buffer, mat4* global_mat, mat4* mat);

extern void object_storage_init();
extern void object_storage_append_object_set(uint32_t set_id);
extern void object_storage_insert_object_set(object_set* set, uint32_t set_id);
extern object* object_storage_get_object(object_info obj_info);
extern object_mesh* object_storage_get_object_mesh(object_info obj_info, char* mesh_name);
extern object_mesh* object_storage_get_object_mesh_by_index(object_info obj_info, int32_t index);
extern object_mesh* object_storage_get_object_mesh_by_object_hash(uint32_t hash, char* mesh_name);
extern object_mesh* object_storage_get_object_mesh_by_object_hash_index(uint32_t hash, int32_t index);
extern int32_t object_storage_get_object_mesh_index(object_info obj_info, char* mesh_name);
extern int32_t object_storage_get_object_mesh_index_by_hash(uint32_t hash, char* mesh_name);
extern object_set* object_storage_get_object_set(uint32_t set_id);
extern ssize_t object_storage_get_object_set_count();
extern int32_t object_storage_get_object_set_load_count(uint32_t set_id);
extern object_set* object_storage_get_object_set_by_index(ssize_t index);
extern ssize_t object_storage_get_object_set_index(uint32_t set_id);
extern int32_t object_storage_get_object_set_load_count_by_index(ssize_t index);
extern object_skin* object_storage_get_object_skin(object_info obj_info);
extern object_index_buffer* object_storage_get_object_index_buffers(uint32_t set_id);
extern object_mesh_index_buffer* object_storage_get_object_mesh_index_buffer(object_info obj_info);
extern object_vertex_buffer* object_storage_get_object_vertex_buffers(uint32_t set_id);
extern object_mesh_vertex_buffer* object_storage_get_object_mesh_vertex_buffer(object_info obj_info);
extern GLuint* object_storage_get_textures(uint32_t set_id);
extern int32_t object_storage_get_textures_count(uint32_t set_id);
extern uint32_t* object_storage_get_texture_ids(uint32_t set_id);
extern int32_t object_storage_get_texture_ids_count(uint32_t set_id);
extern void object_storage_delete_object_set(uint32_t set_id);
extern void object_storage_free();
