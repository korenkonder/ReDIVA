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

enum MaterialAttributeAnisoDirection : uint32_t {
    MAT_SHDATTR_ANISO_DIRECTION_DEFAULT = 0,

    MAT_SHDATTR_ANISO_DIRECTION_NORMAL = 0,
    MAT_SHDATTR_ANISO_DIRECTION_U,
    MAT_SHDATTR_ANISO_DIRECTION_V,
    MAT_SHDATTR_ANISO_DIRECTION_RADIAL,
    MAT_SHDATTR_ANISO_DIRECTION_MAX,
};

enum MaterialAttributeBlendFactor : uint32_t {
    MAT_ATTR_BLEND_DEFAULT = 0,

    MAT_ATTR_BLEND_ZERO = 0,
    MAT_ATTR_BLEND_ONE,
    MAT_ATTR_BLEND_SRC_COLOR,
    MAT_ATTR_BLEND_INVERSE_SRC_COLOR,
    MAT_ATTR_BLEND_SRC_ALPHA,
    MAT_ATTR_BLEND_INVERSE_SRC_ALPHA,
    MAT_ATTR_BLEND_DST_ALPHA,
    MAT_ATTR_BLEND_INVERSE_DST_ALPHA,
    MAT_ATTR_BLEND_DST_COLOR,
    MAT_ATTR_BLEND_INVERSE_DST_COLOR,
    MAT_ATTR_BLEND_ALPHA_SATURATE,
    MAT_ATTR_BLEND_MAX,
};

enum MaterialAttributeBumpMapType : uint32_t {
    MAT_SHDATTR_BUMP_DEFAULT = 0,

    MAT_SHDATTR_BUMP_NONE = 0,
    MAT_SHDATTR_BUMP_DOT,
    MAT_SHDATTR_BUMP_ENV,
    MAT_SHDATTR_BUMP_MAX,
};

enum MaterialAttributeColorSourceType : uint32_t {
    MAT_SHDATTR_COL_SRC_DEFAULT = 0,

    MAT_SHDATTR_COL_SRC_MATCOL = 0,
    MAT_SHDATTR_COL_SRC_VTXCOL,
    MAT_SHDATTR_COL_SRC_VTXMORPH,
    MAT_SHDATTR_COL_SRC_MAX,
};

enum MaterialAttributeFresnelType : uint32_t {
    MAT_SHDATTR_FRESNEL_DEFAULT = 0,

    MAT_SHDATTR_FRESNEL_NONE = 0,
    MAT_SHDATTR_FRESNEL_TYPE1,
    MAT_SHDATTR_FRESNEL_TYPE2,
    MAT_SHDATTR_FRESNEL_TYPE3,
    MAT_SHDATTR_FRESNEL_TYPE4,
    MAT_SHDATTR_FRESNEL_TYPE5,
    MAT_SHDATTR_FRESNEL_TYPE6,
    MAT_SHDATTR_FRESNEL_TYPE7,
    MAT_SHDATTR_FRESNEL_TYPE8,
    MAT_SHDATTR_FRESNEL_TYPE9,
    MAT_SHDATTR_FRESNEL_TYPE10,
    MAT_SHDATTR_FRESNEL_TYPE11,
    MAT_SHDATTR_FRESNEL_TYPE12,
    MAT_SHDATTR_FRESNEL_TYPE13,
    MAT_SHDATTR_FRESNEL_TYPE14,
    MAT_SHDATTR_FRESNEL_TYPE15,
    MAT_SHDATTR_FRESNEL_MAX,
};

enum MaterialAttributeLineLightType : uint32_t {
    MAT_SHDATTR_LINELIGHT_DEFAULT = 0,

    MAT_SHDATTR_LINELIGHT_NONE = 0,
    MAT_SHDATTR_LINELIGHT_TYPE1,
    MAT_SHDATTR_LINELIGHT_TYPE2,
    MAT_SHDATTR_LINELIGHT_TYPE3,
    MAT_SHDATTR_LINELIGHT_TYPE4,
    MAT_SHDATTR_LINELIGHT_TYPE5,
    MAT_SHDATTR_LINELIGHT_TYPE6,
    MAT_SHDATTR_LINELIGHT_TYPE7,
    MAT_SHDATTR_LINELIGHT_TYPE8,
    MAT_SHDATTR_LINELIGHT_TYPE9,
    MAT_SHDATTR_LINELIGHT_MAX,
};

enum MaterialAttributeSpecularQuality : uint32_t {
    MAT_SHDATTR_SPECULAR_QUALITY_DEFAULT = 0,

    MAT_SHDATTR_SPECULAR_QUALITY_LOW = 0,
    MAT_SHDATTR_SPECULAR_QUALITY_HIGH,
    MAT_SHDATTR_SPECULAR_QUALITY_MAX,
};

enum TextureAttributeTextureCoordTransType : uint32_t {
    TEX_SHDATTR_TEXCOORD_TRANS_DEFAULT = 0,

    TEX_SHDATTR_TEXCOORD_TRANS_NONE = 0,
    TEX_SHDATTR_TEXCOORD_TRANS_UV,
    TEX_SHDATTR_TEXCOORD_TRANS_ENV_SPHERE,
    TEX_SHDATTR_TEXCOORD_TRANS_ENV_CUBE,
    TEX_SHDATTR_TEXCOORD_TRANS_MAX,
};

enum TextureAttributeTextureType : uint32_t {
    TEX_SHDATTR_TEXTURE_DEFAULT = 0,

    TEX_SHDATTR_TEXTURE_NONE = 0,
    TEX_SHDATTR_TEXTURE_COLOR,
    TEX_SHDATTR_TEXTURE_NORMAL_MAP,
    TEX_SHDATTR_TEXTURE_SPECULAR_MAP,
    TEX_SHDATTR_TEXTURE_HEIGHT_MAP,
    TEX_SHDATTR_TEXTURE_REFLECT_MAP,
    TEX_SHDATTR_TEXTURE_TRNSL_MAP,
    TEX_SHDATTR_TEXTURE_TRNSP_MAP,
    TEX_SHDATTR_TEXTURE_ENV_SPHERE,
    TEX_SHDATTR_TEXTURE_ENV_CUBE,
    TEX_SHDATTR_TEXTURE_TRANS_MAX,
};

enum MaterialAttributeVertexTransType : uint32_t {
    MAT_SHDATTR_VERTEX_TRANS_DEFAULT = 0,
    MAT_SHDATTR_VERTEX_TRANS_ENVELOPE,
    MAT_SHDATTR_VERTEX_TRANS_MORPHING,
    MAT_SHDATTR_VERTEX_TRANS_MAX,
};

enum obj_index_format : uint32_t {
    OBJ_INDEX_U8  = 0x00,
    OBJ_INDEX_U16 = 0x01,
    OBJ_INDEX_U32 = 0x02,
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

enum obj_skin_ex_node_constraint_type : uint32_t {
    OBJ_SKIN_EX_NODE_CONSTRAINT_NONE = 0,
    OBJ_SKIN_EX_NODE_CONSTRAINT_ORIENTATION,
    OBJ_SKIN_EX_NODE_CONSTRAINT_DIRECTION,
    OBJ_SKIN_EX_NODE_CONSTRAINT_POSITION,
    OBJ_SKIN_EX_NODE_CONSTRAINT_DISTANCE,
};

enum obj_skin_ex_node_type : uint32_t {
    OBJ_SKIN_EX_NODE_NONE = 0,
    OBJ_SKIN_EX_NODE_CLOTH,
    OBJ_SKIN_EX_NODE_CONSTRAINT,
    OBJ_SKIN_EX_NODE_EXPRESSION,
    OBJ_SKIN_EX_NODE_MOTION,
    OBJ_SKIN_EX_NODE_OSAGE,
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

enum ROTTYPE {
    ROTTYPE_YZ = 0x00,
    ROTTYPE_ZY = 0x01,
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
    uint32_t env_sphere : 1;
    uint32_t env_cube : 1;
    uint32_t dummy : 16;
};

union obj_shader_compo {
    obj_shader_compo_member m;
    uint32_t w;
};

struct obj_material_shader_attrib_member {
    MaterialAttributeVertexTransType vtx_trans_type : 2;
    MaterialAttributeColorSourceType col_src : 2;
    uint32_t is_lgt_diffuse : 1;
    uint32_t is_lgt_specular : 1;
    uint32_t is_lgt_per_pixel : 1;
    uint32_t is_lgt_double : 1;
    MaterialAttributeBumpMapType bump_map_type : 2;
    MaterialAttributeFresnelType fresnel_type : 4;
    MaterialAttributeLineLightType line_light : 4;
    uint32_t receive_shadow : 1;
    uint32_t cast_shadow : 1;
    MaterialAttributeSpecularQuality specular_quality : 1;
    MaterialAttributeAnisoDirection aniso_direction : 2;
    uint32_t dummy : 9;
};

union obj_material_shader_attrib {
    obj_material_shader_attrib_member m;
    uint32_t w;
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
    uint32_t ignore : 1;
    uint32_t aniso : 2;
};

union obj_texture_attrib {
    obj_texture_attrib_member m;
    uint32_t w;
};

struct obj_texture_shader_attrib_member {
    TextureAttributeTextureType tex_type : 4;
    int32_t uv_idx : 4;
    TextureAttributeTextureCoordTransType texcoord_trans : 3;
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
    uint32_t alpha_tex : 1;
    uint32_t alpha_mat : 1;
    uint32_t punch_through : 1;
    uint32_t double_sided : 1;
    uint32_t normal_dir_light : 1;
    MaterialAttributeBlendFactor src_blend_factor : 4;
    MaterialAttributeBlendFactor dst_blend_factor : 4;
    uint32_t blend_operation : 3;
    uint32_t zbias : 4;
    uint32_t no_z_fog : 1;
    uint32_t alpha_prio : 6;
    uint32_t y_fog : 1;
    uint32_t ignore_alpha : 1;
    uint32_t y_fogmap : 1;
    uint32_t use_mat_center : 1;
    uint32_t dummy : 1;
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
    uint32_t receive_shadow : 1;
    uint32_t cast_shadow : 1;
    uint32_t vertex_alpha : 1;
    uint32_t hide : 1;
    uint32_t poly_offset : 3;
    uint32_t use_restart_index : 1;
    uint32_t dummy : 24;
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
    uint16_t min_index;
    uint16_t max_index;
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
    uint32_t soft_body : 1;
    uint32_t billboard : 1;
    uint32_t around_obj : 1;
    uint32_t billboard_view : 1;
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

    void generate_tangents();
};

struct obj_skin_ex_node_transform {
    const char* parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;

    obj_skin_ex_node_transform();
};

struct obj_skin_skin_param_coli {
    obj_skin_skin_param_coli_type type;
    int32_t nid0;
    int32_t nid1;
    float_t radius;
    vec3 pos0;
    vec3 pos1;

    obj_skin_skin_param_coli();
};

struct obj_skin_skin_param {
    int32_t root_nid;
    float_t force;
    float_t force_gain;
    float_t air_res;
    float_t angle[2];
    float_t limit[2];
    const char* name;
    obj_skin_skin_param_coli* coli_array;
    int32_t num_coli;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    ROTTYPE rottype;

    obj_skin_skin_param();
};

struct obj_skin_ex_node_cloth_weight {
    const char* bone_name;
    float_t weight;
    int32_t mat_idx;
    int32_t dummy;

    obj_skin_ex_node_cloth_weight();
};

struct obj_skin_ex_node_cloth_root {
    vec3 pos;
    vec3 normal;
    vec4 tangent;
    obj_skin_ex_node_cloth_weight weight[4];

    obj_skin_ex_node_cloth_root();
};

struct obj_skin_ex_node_cloth_point {
    uint32_t link_flag;
    vec3 pos;
    vec3 vec;
    float_t length[4];

    obj_skin_ex_node_cloth_point();
};

struct obj_skin_ex_node_cloth {
    const char* omote_name;
    const char* ura_name;
    uint32_t version;
    uint32_t width;
    uint32_t height;
    uint32_t ring_flag;
    mat4* mat_array;
    uint32_t num_mat;
    obj_skin_ex_node_cloth_root* fix_point;
    obj_skin_ex_node_cloth_point* move_point;
    uint16_t* omote_index_array;
    uint32_t num_omote_index;
    uint16_t* ura_index_array;
    uint32_t num_ura_index;
    obj_skin_skin_param* param;
    uint32_t dummy;

    obj_skin_ex_node_cloth();
};

struct obj_skin_ex_node_constraint_attach_point {
    bool affected_by_orientation;
    bool affected_by_scaling;
    vec3 offset;

    obj_skin_ex_node_constraint_attach_point();
};

struct obj_skin_ex_node_constraint_up_vector {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    const char* name;

    obj_skin_ex_node_constraint_up_vector();
};

struct obj_skin_ex_node_constraint_direction {
    obj_skin_ex_node_constraint_up_vector up_vector;
    vec3 align_axis;
    vec3 target_offset;

    obj_skin_ex_node_constraint_direction();
};

struct obj_skin_ex_node_constraint_distance {
    obj_skin_ex_node_constraint_up_vector up_vector;
    float_t distance;
    obj_skin_ex_node_constraint_attach_point constrained_object;
    obj_skin_ex_node_constraint_attach_point constraining_object;

    obj_skin_ex_node_constraint_distance();
};

struct obj_skin_ex_node_constraint_orientation {
    vec3 offset;

    obj_skin_ex_node_constraint_orientation();
};

struct obj_skin_ex_node_constraint_position {
    obj_skin_ex_node_constraint_up_vector up_vector;
    obj_skin_ex_node_constraint_attach_point constrained_object;
    obj_skin_ex_node_constraint_attach_point constraining_object;

    obj_skin_ex_node_constraint_position();
};

struct obj_skin_ex_node_constraint {
    obj_skin_ex_node_transform transform;
    uint32_t node_name;
    uint32_t nb_src;
    const char* src_name;
    obj_skin_ex_node_constraint_type type;
    union {
        void* data;
        obj_skin_ex_node_constraint_direction* direction;
        obj_skin_ex_node_constraint_distance* distance;
        obj_skin_ex_node_constraint_orientation* orientation;
        obj_skin_ex_node_constraint_position* position;
    };

    obj_skin_ex_node_constraint();
};

struct obj_skin_ex_node_expression {
    obj_skin_ex_node_transform transform;
    uint32_t node_name;
    int32_t nb_src;
    const char* script[9];

    obj_skin_ex_node_expression();
};

struct obj_skin_motion_node {
    uint32_t node_name;
    mat4 inv_bind_pose_mat;

    obj_skin_motion_node();
};

struct obj_skin_ex_node_motion {
    obj_skin_ex_node_transform transform;
    union {
        const char* name;
        uint32_t node_name;
    };
    obj_skin_motion_node* node_array;
    int32_t num_node;

    obj_skin_ex_node_motion();
};

struct obj_skin_osage_joint_rotation {
    uint32_t nid;
    float_t length;
    vec3 rotation;

    obj_skin_osage_joint_rotation();
};

struct obj_skin_ex_node_osage {
    obj_skin_ex_node_transform transform;
    uint32_t joint_ofs;
    uint32_t nb_joint;
    obj_skin_osage_joint_rotation* joint_rotation_array;
    int32_t num_joint_rotation;
    obj_skin_skin_param* skin_param;
    uint32_t root_idx;
    uint32_t efc_idx;
    const char* motion_node_name;

    obj_skin_ex_node_osage();
};

struct obj_skin_ex_node {
    obj_skin_ex_node_type type;
    union {
        obj_skin_ex_node_transform* transform;
        obj_skin_ex_node_cloth* cloth;
        obj_skin_ex_node_constraint* constraint;
        obj_skin_ex_node_expression* expression;
        obj_skin_ex_node_motion* motion;
        obj_skin_ex_node_osage* osage;
    };

    obj_skin_ex_node();
};

struct obj_skin_osage_constraint_info {
    uint32_t dst_joint;
    uint32_t src_joint;
    float_t length;

    obj_skin_osage_constraint_info();
};

struct obj_skin_osage_joint {
    uint32_t nid;
    float_t length;

    obj_skin_osage_joint();
};

struct obj_skin_ex_data {
    int32_t nb_jointX;
    const char* osage_root;
    obj_skin_osage_joint* osage_joint;
    obj_skin_ex_node* ex_node_table;
    int32_t num_ex_node;
    uint32_t nb_node_name;
    const char** ex_node_name;
    obj_skin_osage_constraint_info* osage_constraint_tbl;
    int32_t num_osage_constraint;
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
