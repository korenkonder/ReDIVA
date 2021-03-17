/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"

typedef enum glitter_curve_flags {
    GLITTER_CURVE_RANDOMIZE    = 0b0000001,
    GLITTER_CURVE_RANDOM_RANGE = 0b0000010,
    GLITTER_CURVE_NEGATE       = 0b0000100,
    GLITTER_CURVE_ABSOLUTE     = 0b0000100,
    GLITTER_CURVE_STEP         = 0b0001000,
    GLITTER_CURVE_INVERSE      = 0b0010000,
    GLITTER_CURVE_MULT         = 0b0100000,
    GLITTER_CURVE_PLAIN        = 0b1000000,
} glitter_curve_flags;

typedef enum glitter_curve_type {
    GLITTER_CURVE_TRANSLATION_X          = 0,
    GLITTER_CURVE_TRANSLATION_Y          = 1,
    GLITTER_CURVE_TRANSLATION_Z          = 2,
    GLITTER_CURVE_ROTATION_X             = 3,
    GLITTER_CURVE_ROTATION_Y             = 4,
    GLITTER_CURVE_ROTATION_Z             = 5,
    GLITTER_CURVE_SCALE_X                = 6,
    GLITTER_CURVE_SCALE_Y                = 7,
    GLITTER_CURVE_SCALE_Z                = 8,
    GLITTER_CURVE_SCALE_ALL              = 9,
    GLITTER_CURVE_COLOR_R                = 10,
    GLITTER_CURVE_COLOR_G                = 11,
    GLITTER_CURVE_COLOR_B                = 12,
    GLITTER_CURVE_COLOR_A                = 13,
    GLITTER_CURVE_UNK14                  = 14,
    GLITTER_CURVE_UNK15                  = 15,
    GLITTER_CURVE_UNK16                  = 16,
    GLITTER_CURVE_UNK17                  = 17,
    GLITTER_CURVE_UNK18                  = 18,
    GLITTER_CURVE_UNK19                  = 19,
    GLITTER_CURVE_EMISSION_INTERVAL      = 20,
    GLITTER_CURVE_PARTICLES_PER_EMISSION = 21,
    GLITTER_CURVE_U_SCROLL               = 22,
    GLITTER_CURVE_V_SCROLL               = 23,
} glitter_curve_type;

typedef enum glitter_direction {
    GLITTER_DIRECTION_BILLBOARD         = 0,
    GLITTER_DIRECTION_EMITTER_DIRECTION = 1,
    GLITTER_DIRECTION_Y_AXIS            = 4,
    GLITTER_DIRECTION_X_AXIS            = 5,
    GLITTER_DIRECTION_Z_AXIS            = 6,
    GLITTER_DIRECTION_BILLBOARD_Y_ONLY  = 7,
    GLITTER_DIRECTION_EMITTER_ROTATION  = 9,
    GLITTER_DIRECTION_PARTICLE_ROTATION = 11,
} glitter_direction;

typedef enum glitter_effect_flag {
    GLITTER_EFFECT_FLAG_NONE       = 0x00,
    GLITTER_EFFECT_FLAG_LOOP       = 0x01,
    GLITTER_EFFECT_FLAG_LOCAL      = 0x02,
    GLITTER_EFFECT_FLAG_ALPHA      = 0x04,
    GLITTER_EFFECT_FLAG_FOG        = 0x08,
    GLITTER_EFFECT_FLAG_FOG_HEIGHT = 0x10,
    GLITTER_EFFECT_FLAG_EMISSION   = 0x20,
} glitter_effect_flag;

typedef enum glitter_effect_file_flag {
    GLITTER_EFFECT_FILE_FLAG_ALPHA      = 0x01,
    GLITTER_EFFECT_FILE_FLAG_FOG        = 0x02,
    GLITTER_EFFECT_FILE_FLAG_FOG_HEIGHT = 0x04,
    GLITTER_EFFECT_FILE_FLAG_EMISSION   = 0x08,
} glitter_effect_file_flag;

typedef enum glitter_emitter_emission_direction {
    GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY = 0,
    GLITTER_EMITTER_EMISSION_DIRECTION_INWARD            = 1,
    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD           = 2,
} glitter_emitter_emission_direction;

typedef enum glitter_emitter_flag {
    GLITTER_EMITTER_FLAG_NONE = 0x00,
    GLITTER_EMITTER_FLAG_LOOP = 0x01,
    GLITTER_EMITTER_FLAG_FREE = 0x02,
} glitter_emitter_flag;

typedef enum glitter_emitter_type {
    GLITTER_EMITTER_BOX      = 0,
    GLITTER_EMITTER_CYLINDER = 1,
    GLITTER_EMITTER_SPHERE   = 2,
    GLITTER_EMITTER_MESH     = 3,
    GLITTER_EMITTER_POLYGON  = 4,
} glitter_emitter_type;

typedef enum glitter_key_type {
    GLITTER_KEY_CONSTANT = 0,
    GLITTER_KEY_LINEAR   = 1,
    GLITTER_KEY_HERMITE  = 2,
} glitter_key_type;

typedef enum glitter_particle_flag {
    GLITTER_PARTICLE_FLAG_NONE                 = 0x00000,
    GLITTER_PARTICLE_FLAG_LOOP                 = 0x00001,
    GLITTER_PARTICLE_FLAG_USE_MODEL_MAT        = 0x00004,
    GLITTER_PARTICLE_FLAG_POSITION_OFFSET_SAME = 0x00010,
    GLITTER_PARTICLE_FLAG_REBOUND_PLANE        = 0x00020,
    GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER = 0x00040,
    GLITTER_PARTICLE_FLAG_SCALE                = 0x00080,
    GLITTER_PARTICLE_FLAG_SECOND_TEXTURE       = 0x00100,
    GLITTER_PARTICLE_FLAG_LOCAL                = 0x10000,
    GLITTER_PARTICLE_FLAG_EMISSION             = 0x20000,
} glitter_particle_flag;

typedef enum glitter_particle_inst_flag {
    GLITTER_PARTICLE_INST_FLAG_NONE     = 0x00,
    GLITTER_PARTICLE_INST_FLAG_ENDED    = 0x01,
    GLITTER_PARTICLE_INST_FLAG_NO_CHILD = 0x02,
} glitter_particle_inst_flag;

typedef enum glitter_particle_sub_flag {
    GLITTER_PARTICLE_SUB_FLAG_NONE      = 0x00000000,
    GLITTER_PARTICLE_SUB_FLAG_GET_VALUE = 0x40000000,
} glitter_particle_sub_flag;

typedef enum glitter_particle_type {
    GLITTER_PARTICLE_QUAD  = 0,
    GLITTER_PARTICLE_LINE  = 1,
    GLITTER_PARTICLE_LOCUS = 2,
    GLITTER_PARTICLE_MESH  = 3,
} glitter_particle_type;

typedef enum glitter_pivot {
    GLITTER_PIVOT_TOP_LEFT      = 0,
    GLITTER_PIVOT_TOP_CENTER    = 1,
    GLITTER_PIVOT_TOP_RIGHT     = 2,
    GLITTER_PIVOT_MIDDLE_LEFT   = 3,
    GLITTER_PIVOT_MIDDLE_CENTER = 4,
    GLITTER_PIVOT_MIDDLE_RIGHT  = 5,
    GLITTER_PIVOT_BOTTOM_LEFT   = 6,
    GLITTER_PIVOT_BOTTOM_CENTER = 7,
    GLITTER_PIVOT_BOTTOM_RIGHT  = 8,
} glitter_pivot;

typedef enum glitter_uv_index_type {
    GLITTER_UV_INDEX_FIXED                  = 0,
    GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED   = 1,
    GLITTER_UV_INDEX_RANDOM                 = 2,
    GLITTER_UV_INDEX_FORWARD                = 3,
    GLITTER_UV_INDEX_REVERSE                = 4,
    GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD = 5,
    GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE = 6,
} glitter_uv_index_type;

typedef struct glitter_buffer glitter_buffer;
typedef struct glitter_curve_key glitter_curve_key;
typedef struct glitter_curve glitter_curve;
typedef struct glitter_effect_a3da glitter_effect_a3da;
typedef struct glitter_effect_data glitter_effect_data;
typedef struct glitter_effect glitter_effect;
typedef struct glitter_effect_group glitter_effect_group;
typedef struct glitter_effect_inst_a3da glitter_effect_inst_a3da;
typedef struct glitter_effect_inst glitter_effect_inst;
typedef struct glitter_file_reader glitter_file_reader;
typedef struct glitter_particle_manager glitter_particle_manager;
typedef struct glitter_locus_history glitter_locus_history;
typedef struct glitter_locus_history_data glitter_locus_history_data;
typedef struct glitter_particle_data glitter_particle_data;
typedef struct glitter_particle_sub glitter_particle_sub;
typedef struct glitter_particle glitter_particle;
typedef struct glitter_particle_inst_data glitter_particle_inst_data;
typedef struct glitter_particle_inst glitter_particle_inst;
typedef struct glitter_render_group_sub glitter_render_group_sub;
typedef struct glitter_render_group glitter_render_group;
typedef struct glitter_scene_sub glitter_scene_sub;
typedef struct glitter_scene glitter_scene;
typedef struct glitter_emitter_box glitter_emitter_box;
typedef struct glitter_emitter_cylinder glitter_emitter_cylinder;
typedef struct glitter_emitter_polygon glitter_emitter_polygon;
typedef struct glitter_emitter_sphere glitter_emitter_sphere;
typedef union glitter_emitter_union glitter_emitter_union;
typedef struct glitter_emitter_data glitter_emitter_data;
typedef struct glitter_emitter glitter_emitter;
typedef struct glitter_emitter_inst glitter_emitter_inst;

vector(glitter_curve_key)
vector_ptr(glitter_curve)
vector_ptr(glitter_effect)
vector_ptr(glitter_effect_group)
vector_ptr(glitter_effect_inst)
vector_ptr(glitter_emitter)
vector_ptr(glitter_emitter_inst)
vector_ptr(glitter_file_reader)
vector(glitter_locus_history_data)
vector_ptr(glitter_particle)
vector_ptr(glitter_particle_inst)
vector_ptr(glitter_render_group)
vector_ptr(glitter_scene)

struct glitter_buffer {
    vec3 position;
    vec2 uv;
    vec4 color;
};

struct glitter_curve_key {
    glitter_key_type type;
    float_t frame;
    float_t value;
    float_t tangent1;
    float_t tangent2;
    float_t random_range;
};

struct glitter_curve {
    glitter_curve_type type;
    bool repeat;
    float_t start_time;
    float_t end_time;
    glitter_curve_flags flags;
    float_t max_rand;
    vector_glitter_curve_key keys;
};

struct glitter_effect_a3da {
    int32_t object_index;
    int32_t flags;
    int32_t index;
    char mesh_name[128];
};

struct glitter_effect_data {
    uint64_t name_hash;
    float_t appear_time;
    float_t life_time;
    float_t start_time;
    uint32_t color;
    glitter_effect_a3da* a3da;
    glitter_effect_flag flags;
    float_t emission;
};

struct glitter_effect {
    char* name;
    vector_ptr_glitter_curve curve;
    vec3 translation;
    vec3 rotation;
    vec4 scale;
    glitter_effect_data data;
    vector_ptr_glitter_emitter emitters;
};

struct glitter_effect_group {
    vector_ptr_glitter_effect effects;
    uint64_t hash;
    int64_t* resource_hashes;
    uint32_t resources_count;
    int32_t* resources;
    glitter_scene* scene;
    float_t emission;
    bool scene_init;
    bool buffer_init;
};

struct glitter_effect_inst_a3da {
    int32_t dword00;
    int32_t dword04;
    int64_t dword08;
    int32_t dword10;
    int32_t object_index;
    int32_t index;
    mat4 mat;
    vec3 translation;
    char* mesh_name;
    bool object_is_hrc;
};

struct glitter_effect_inst {
    glitter_effect* effect;
    glitter_effect_data data;
    float_t frame0;
    float_t frame1;
    vec4 color;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    float_t scale_all;
    mat4 mat;
    vector_ptr_glitter_emitter_inst emitters;
    uint32_t effect_val;
    int32_t flags;
    int32_t id;
    glitter_effect_inst_a3da* a3da;
    vec4 min_color;
    vec3 a3da_scale;
};

struct glitter_emitter_box {
    vec3 size;
};

struct glitter_emitter_cylinder {
    float_t radius;
    float_t height;
    float_t start_angle;
    float_t end_angle;
    bool plain;
    glitter_emitter_emission_direction direction;
};

struct glitter_emitter_polygon {
    float_t scale;
    int32_t count;
};

struct glitter_emitter_sphere {
    float_t radius;
    float_t latitude;
    float_t longitude;
    bool plain;
    glitter_emitter_emission_direction direction;
};

union glitter_emitter_union {
    glitter_emitter_box box;
    glitter_emitter_cylinder cylinder;
    glitter_emitter_sphere sphere;
    glitter_emitter_polygon polygon;
};

struct glitter_emitter_data {
    float_t start_time;
    float_t life_time;
    float_t loop_start_time;
    float_t loop_life_time;
    glitter_emitter_flag flags;
    vec3 rotation_add;
    vec3 rotation_add_random;
    int32_t dword2C;
    int32_t dword30;
    float_t emission_interval;
    float_t particles_per_emission;
    glitter_direction direction;
    glitter_emitter_type type;
    glitter_emitter_union data;
};


struct glitter_emitter {
    char* name;
    vector_ptr_glitter_curve curve;
    vec3 translation;
    vec3 rotation;
    vec4 scale;
    glitter_emitter_data data;
    vector_ptr_glitter_particle particles;
};

struct glitter_emitter_inst {
    glitter_emitter* emitter;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    mat4 mat;
    mat4 mat_no_scale;
    float_t scale_all;
    float_t emission_timer;
    vector_ptr_glitter_particle_inst particles;
    glitter_emitter_data data;
    float_t emission_interval;
    float_t particles_per_emission;
    uint32_t effect_val;
    bool loop;
    int32_t dword154;
    float_t frame;
    bool ended;
};

struct glitter_file_reader {
    glitter_effect_group* effect_group;
    int32_t version;
    wchar_t* path;
    wchar_t* file;
    uint64_t hash;
};

struct glitter_particle_manager {
    vector_ptr_glitter_scene scenes;
    vector_ptr_glitter_file_reader file_readers;
    vector_ptr_glitter_effect_group effect_groups;
    float_t delta_frame;
    uint32_t effect_val;
    uint32_t counter;
    bool f2;
};

struct glitter_locus_history {
    vector_glitter_locus_history_data data;
};

struct glitter_locus_history_data {
    vec3 translation;
    vec4 color;
    float_t scale;
};

struct glitter_particle_data {
    glitter_particle_flag flags;
    float_t life_time;
    glitter_particle_type type;
    glitter_pivot pivot;
    glitter_direction draw_type;
    float_t z_offset;
    vec3 rotation;
    vec3 rotation_random;
    vec3 rotation_add;
    vec3 rotation_add_random;
    vec3 position_offset;
    vec3 position_offset_random;
    vec3 direction;
    vec3 direction_random;
    float_t speed;
    float_t speed_random;
    float_t deceleration;
    float_t deceleration_random;
    vec3 gravitational_acceleration;
    vec3 external_acceleration;
    vec3 external_acceleration_random;
    float_t reflection_coeff;
    float_t reflection_coeff_random;
    float_t rebound_plane_y;
    uint32_t color_int;
    vec4 color;
    glitter_uv_index_type uv_index_type;
    int32_t uv_index;
    float_t frame_step_uv;
    int32_t uv_index_start;
    int32_t uv_index_end;
    int32_t uv_index_count;
    vec2 uv_scroll_add;
    float_t uv_scroll_add_scale;
    vec2 split_uv;
    uint8_t split_u;
    uint8_t split_v;
    int32_t blend_mode0;
    int32_t blend_mode1;
    glitter_particle_sub_flag sub_flags;
    int32_t count;
    int32_t locus_history_size;
    int32_t locus_history_size_random;
    int32_t dword118;
    float_t emission;
    uint64_t tex_hash0;
    uint64_t tex_hash1;
    int32_t texture0;
    int32_t texture1;
    char dword138[0x20];
};

struct glitter_particle_sub {
    glitter_particle_data data;
    int32_t max_count;
};

struct glitter_particle {
    char* name;
    vector_ptr_glitter_curve curve;
    glitter_particle_sub data;
};

struct glitter_particle_inst_data {
    glitter_particle_data data;
    glitter_particle_inst_flag flags;
    glitter_render_group* render_group;
    glitter_effect_inst* effect_inst;
    glitter_particle_inst* parent;
    glitter_particle* particle;
    vector_ptr_glitter_particle_inst particle_insts;
};

struct glitter_particle_inst {
    glitter_particle* particle;
    mat4 mat;
    mat4 mat_no_scale;
    glitter_particle_inst_data sub;
};

struct glitter_render_group_sub {
    bool alive;
    uint8_t uv_index;
    uint32_t effect_val;
    float_t frame;
    float_t life_time;
    float_t rebound_time;
    float_t frame_step_uv;
    float_t speed;
    float_t deceleration;
    vec2 uv;
    vec4 color;
    vec3 base_translation;
    vec3 translation;
    vec3 translation_prev;
    vec3 direction;
    vec3 acceleration;
    vec2 position_offset;
    vec3 scale;
    vec3 rotation;
    vec3 rotation_add;
    float_t rot_z_cos;
    float_t rot_z_sin;
    float_t scale_all;
    vec2 uv_scroll;
    mat4 mat;
    glitter_locus_history* locus_history;
};

struct glitter_render_group {
    glitter_particle_flag flags;
    glitter_particle_type type;
    glitter_direction draw_type;
    int32_t blend_mode0;
    int32_t blend_mode1;
    glitter_pivot pivot;
    int32_t split_u;
    int32_t split_v;
    vec2 split_uv;
    float_t z_offset;
    int32_t count;
    int32_t ctrl;
    int32_t texture0;
    int32_t texture1;
    char dword48[32];
    float_t frame;
    mat4 mat;
    mat4 mat_no_scale;
    int32_t has_texture1;
    glitter_render_group_sub* sub;
    glitter_buffer* buffer;
    int32_t max_count;
    glitter_particle_inst* particle_inst;
    int32_t alpha;
    int32_t fog;
    int32_t buffer_index;
    int32_t vao;
    float_t emission;
};

struct glitter_scene_sub {
    vector_ptr_glitter_render_group render_groups;
    int32_t disp_quad;
    int32_t disp_locus;
    int32_t disp_line;
    int32_t ctrl_quad;
    int32_t ctrl_locus;
    int32_t ctrl_line;
};

struct glitter_scene {
    vector_ptr_glitter_effect_inst effects;
    uint64_t hash;
    float_t emission;
    glitter_effect_group* effect_group;
    glitter_scene_sub sub;
};
