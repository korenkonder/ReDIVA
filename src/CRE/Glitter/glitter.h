/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"

typedef enum glitter_curve_flag {
    GLITTER_CURVE_RANDOM_RANGE        = 0x01,
    GLITTER_CURVE_KEY_RANDOM_RANGE    = 0x02,
    GLITTER_CURVE_RANDOM_RANGE_NEGATE = 0x04,
    GLITTER_CURVE_STEP                = 0x08,
    GLITTER_CURVE_NEGATE              = 0x10,
    GLITTER_CURVE_RANDOM_RANGE_MULT   = 0x20,
    GLITTER_CURVE_BAKED               = 0x40,
    GLITTER_CURVE_BAKED_FULL          = 0x80,
} glitter_curve_flag;

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
    GLITTER_CURVE_COLOR_RGB_SCALE        = 14,
    GLITTER_CURVE_COLOR_R_2ND            = 15,
    GLITTER_CURVE_COLOR_G_2ND            = 16,
    GLITTER_CURVE_COLOR_B_2ND            = 17,
    GLITTER_CURVE_COLOR_A_2ND            = 18,
    GLITTER_CURVE_COLOR_RGB_SCALE_2ND    = 19,
    GLITTER_CURVE_EMISSION_INTERVAL      = 20,
    GLITTER_CURVE_PARTICLES_PER_EMISSION = 21,
    GLITTER_CURVE_U_SCROLL               = 22,
    GLITTER_CURVE_V_SCROLL               = 23,
    GLITTER_CURVE_U_SCROLL_ALPHA         = 24,
    GLITTER_CURVE_V_SCROLL_ALPHA         = 25,
    GLITTER_CURVE_U_SCROLL_2ND           = 26,
    GLITTER_CURVE_V_SCROLL_2ND           = 27,
    GLITTER_CURVE_U_SCROLL_ALPHA_2ND     = 28,
    GLITTER_CURVE_V_SCROLL_ALPHA_2ND     = 29,
} glitter_curve_type;

typedef enum glitter_curve_type_flags {
    GLITTER_CURVE_TYPE_TRANSLATION_X          = 0x00000001,
    GLITTER_CURVE_TYPE_TRANSLATION_Y          = 0x00000002,
    GLITTER_CURVE_TYPE_TRANSLATION_Z          = 0x00000004,
    GLITTER_CURVE_TYPE_ROTATION_X             = 0x00000008,
    GLITTER_CURVE_TYPE_ROTATION_Y             = 0x00000010,
    GLITTER_CURVE_TYPE_ROTATION_Z             = 0x00000020,
    GLITTER_CURVE_TYPE_SCALE_X                = 0x00000040,
    GLITTER_CURVE_TYPE_SCALE_Y                = 0x00000080,
    GLITTER_CURVE_TYPE_SCALE_Z                = 0x00000100,
    GLITTER_CURVE_TYPE_SCALE_ALL              = 0x00000200,
    GLITTER_CURVE_TYPE_COLOR_R                = 0x00000400,
    GLITTER_CURVE_TYPE_COLOR_G                = 0x00000800,
    GLITTER_CURVE_TYPE_COLOR_B                = 0x00001000,
    GLITTER_CURVE_TYPE_COLOR_A                = 0x00002000,
    GLITTER_CURVE_TYPE_COLOR_RGB_SCALE        = 0x00004000,
    GLITTER_CURVE_TYPE_COLOR_R_2ND            = 0x00008000,
    GLITTER_CURVE_TYPE_COLOR_G_2ND            = 0x00010000,
    GLITTER_CURVE_TYPE_COLOR_B_2ND            = 0x00020000,
    GLITTER_CURVE_TYPE_COLOR_A_2ND            = 0x00040000,
    GLITTER_CURVE_TYPE_COLOR_RGB_SCALE_2ND    = 0x00080000,
    GLITTER_CURVE_TYPE_EMISSION_INTERVAL      = 0x00100000,
    GLITTER_CURVE_TYPE_PARTICLES_PER_EMISSION = 0x00200000,
    GLITTER_CURVE_TYPE_U_SCROLL               = 0x00400000,
    GLITTER_CURVE_TYPE_V_SCROLL               = 0x00800000,
    GLITTER_CURVE_TYPE_U_SCROLL_ALPHA         = 0x01000000,
    GLITTER_CURVE_TYPE_V_SCROLL_ALPHA         = 0x02000000,
    GLITTER_CURVE_TYPE_U_SCROLL_2ND           = 0x04000000,
    GLITTER_CURVE_TYPE_V_SCROLL_2ND           = 0x08000000,
    GLITTER_CURVE_TYPE_U_SCROLL_ALPHA_2ND     = 0x10000000,
    GLITTER_CURVE_TYPE_V_SCROLL_ALPHA_2ND     = 0x20000000,

    GLITTER_CURVE_TYPE_TRANSLATION_XYZ     = 0x00000001 | 0x00000002 | 0x00000004,
    GLITTER_CURVE_TYPE_ROTATION_XYZ        = 0x00000008 | 0x00000010 | 0x00000020,
    GLITTER_CURVE_TYPE_SCALE_XYZ           = 0x00000040 | 0x00000080 | 0x00000100,
    GLITTER_CURVE_TYPE_COLOR_RGBA          = 0x00000400 | 0x00000800 | 0x00001000 | 0x00002000,
    GLITTER_CURVE_TYPE_COLOR_RGBA_2ND      = 0x00008000 | 0x00010000 | 0x00020000 | 0x00040000,
    GLITTER_CURVE_TYPE_UV_SCROLL           = 0x00400000 | 0x00800000,
    GLITTER_CURVE_TYPE_UV_SCROLL_ALPHA     = 0x01000000 | 0x02000000,
    GLITTER_CURVE_TYPE_UV_SCROLL_2ND       = 0x04000000 | 0x08000000,
    GLITTER_CURVE_TYPE_UV_SCROLL_ALPHA_2ND = 0x10000000 | 0x20000000,
} glitter_curve_type_flags;

typedef enum glitter_direction {
    GLITTER_DIRECTION_BILLBOARD         = 0,
    GLITTER_DIRECTION_EMITTER_DIRECTION = 1,
    GLITTER_DIRECTION_PREV_POSITION     = 2,
    GLITTER_DIRECTION_EMIT_POSITION     = 3,
    GLITTER_DIRECTION_Y_AXIS            = 4,
    GLITTER_DIRECTION_X_AXIS            = 5,
    GLITTER_DIRECTION_Z_AXIS            = 6,
    GLITTER_DIRECTION_BILLBOARD_Y_AXIS  = 7,
    GLITTER_DIRECTION_PREV_POSITION_DUP = 8,
    GLITTER_DIRECTION_EMITTER_ROTATION  = 9,
    GLITTER_DIRECTION_EFFECT_ROTATION   = 10,
    GLITTER_DIRECTION_PARTICLE_ROTATION = 11,
} glitter_direction;

typedef enum glitter_effect_ext_anim_flag {
    GLITTER_EFFECT_EXT_ANIM_SET_ONCE            = 0x00001,
    GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY          = 0x00002,
    GLITTER_EFFECT_EXT_ANIM_NO_TRANS_X          = 0x00004,
    GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Y          = 0x00008,
    GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Z          = 0x00010,
    GLITTER_EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA  = 0x00020,
    GLITTER_EFFECT_EXT_ANIM_GET_THEN_UPDATE     = 0x00040,
    GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM          = 0x10000,
} glitter_effect_ext_anim_flag;

typedef enum glitter_effect_ext_anim_chara_node {
    GLITTER_EFFECT_EXT_ANIM_CHARA_NONE            = -1,
    GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD            = 0,
    GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH           = 1,
    GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY           = 2,
    GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST           = 3,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER   = 4,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW      = 5,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP  = 6,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND       = 7,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER  = 8,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW     = 9,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP = 10,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND      = 11,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH      = 12,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE       = 13,
    GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE        = 14,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH     = 15,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE      = 16,
    GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE       = 17,
    GLITTER_EFFECT_EXT_ANIM_CHARA_MAX             = 18,
} glitter_effect_ext_anim_chara_node;

typedef enum glitter_effect_flag {
    GLITTER_EFFECT_NONE       = 0x00,
    GLITTER_EFFECT_LOOP       = 0x01,
    GLITTER_EFFECT_LOCAL      = 0x02,
    GLITTER_EFFECT_ALPHA      = 0x04,
    GLITTER_EFFECT_FOG        = 0x08,
    GLITTER_EFFECT_FOG_HEIGHT = 0x10,
    GLITTER_EFFECT_EMISSION   = 0x20,
    GLITTER_EFFECT_USE_SEED   = 0x40,
} glitter_effect_flag;

typedef enum glitter_effect_file_flag {
    GLITTER_EFFECT_FILE_ALPHA      = 0x01,
    GLITTER_EFFECT_FILE_FOG        = 0x02,
    GLITTER_EFFECT_FILE_FOG_HEIGHT = 0x04,
    GLITTER_EFFECT_FILE_EMISSION   = 0x08,
    GLITTER_EFFECT_FILE_USE_SEED   = 0x10,
} glitter_effect_file_flag;

typedef enum glitter_effect_inst_flag {
    GLITTER_EFFECT_INST_NONE                        = 0x00000,
    GLITTER_EFFECT_INST_FREE                        = 0x00001,
    GLITTER_EFFECT_INST_JUST_INIT                   = 0x00002,
    GLITTER_EFFECT_INST_HAS_EXT_ANIM                = 0x00004,
    GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS          = 0x00008,
    GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT       = 0x00010,
    GLITTER_EFFECT_INST_CHARA_ANIM                  = 0x00020,
    GLITTER_EFFECT_INST_GET_EXT_ANIM_MAT            = 0x00040,
    GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE           = 0x00080,
    GLITTER_EFFECT_INST_MIN_COLOR_MULT              = 0x00100,
    GLITTER_EFFECT_INST_SET_MIN_COLOR               = 0x00200,
    GLITTER_EFFECT_INST_SET_ADD_MIN_COLOR           = 0x00400,
    GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE          = 0x00800,
    GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_X         = 0x01000,
    GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Y         = 0x02000,
    GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Z         = 0x04000,
    GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY         = 0x08000,
    GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE    = 0x20000,
} glitter_effect_inst_flag;

typedef enum glitter_emitter_direction {
    GLITTER_EMITTER_DIRECTION_BILLBOARD        = 0,
    GLITTER_EMITTER_DIRECTION_BILLBOARD_Y_AXIS = 1,
    GLITTER_EMITTER_DIRECTION_Y_AXIS           = 2,
    GLITTER_EMITTER_DIRECTION_X_AXIS           = 3,
    GLITTER_EMITTER_DIRECTION_Z_AXIS           = 4,
    GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION  = 5,
} glitter_emitter_direction;

typedef enum glitter_emitter_emission {
    GLITTER_EMITTER_EMISSION_ON_TIMER = 0,
    GLITTER_EMITTER_EMISSION_ON_START = 1,
    GLITTER_EMITTER_EMISSION_ON_END   = 2,
    GLITTER_EMITTER_EMISSION_EMITTED  = 3,
} glitter_emitter_emission;

typedef enum glitter_emitter_emission_direction {
    GLITTER_EMITTER_EMISSION_DIRECTION_NONE    = 0,
    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD = 1,
    GLITTER_EMITTER_EMISSION_DIRECTION_INWARD  = 2,
} glitter_emitter_emission_direction;

typedef enum glitter_emitter_flag {
    GLITTER_EMITTER_NONE        = 0x00,
    GLITTER_EMITTER_LOOP        = 0x01,
    GLITTER_EMITTER_KILL_ON_END = 0x02,
    GLITTER_EMITTER_USE_SEED    = 0x04,
} glitter_emitter_flag;

typedef enum glitter_emitter_inst_flag {
    GLITTER_EMITTER_INST_NONE         = 0x00,
    GLITTER_EMITTER_INST_ENDED        = 0x01,
    GLITTER_EMITTER_INST_HAS_DISTANCE = 0x02,
} glitter_emitter_inst_flag;

typedef enum glitter_emitter_timer_type {
    GLITTER_EMITTER_TIMER_BY_TIME     = 0,
    GLITTER_EMITTER_TIMER_BY_DISTANCE = 1,
} glitter_emitter_timer_type;

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

typedef enum glitter_particle_blend {
    GLITTER_PARTICLE_BLEND_ZERO          = 0,
    GLITTER_PARTICLE_BLEND_TYPICAL       = 1,
    GLITTER_PARTICLE_BLEND_ADD           = 2,
    GLITTER_PARTICLE_BLEND_SUBTRACT      = 3,
    GLITTER_PARTICLE_BLEND_MULTIPLY      = 4,
    GLITTER_PARTICLE_BLEND_PUNCH_THROUGH = 5,
} glitter_particle_blend;

typedef enum glitter_particle_blend_draw {
    GLITTER_PARTICLE_BLEND_DRAW_TYPICAL  = 0,
    GLITTER_PARTICLE_BLEND_DRAW_ADD      = 1,
    GLITTER_PARTICLE_BLEND_DRAW_MULTIPLY = 2,
} glitter_particle_blend_draw;

typedef enum glitter_particle_blend_mask {
    GLITTER_PARTICLE_BLEND_MASK_TYPICAL  = 0,
    GLITTER_PARTICLE_BLEND_MASK_ADD      = 1,
    GLITTER_PARTICLE_BLEND_MASK_MULTIPLY = 2,
} glitter_particle_blend_mask;

typedef enum glitter_particle_draw_flag {
    GLITTER_PARTICLE_DRAW_NONE              = 0x0,
    GLITTER_PARTICLE_DRAW_NO_BILLBOARD_CULL = 0x1,
} glitter_particle_draw_flag;

typedef enum glitter_particle_draw_type {
    GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD         = 0,
    GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD_Y_AXIS  = 1,
    GLITTER_PARTICLE_DRAW_TYPE_X_AXIS            = 2,
    GLITTER_PARTICLE_DRAW_TYPE_Y_AXIS            = 3,
    GLITTER_PARTICLE_DRAW_TYPE_Z_AXIS            = 4,
    GLITTER_PARTICLE_DRAW_TYPE_EMITTER_DIRECTION = 5,
    GLITTER_PARTICLE_DRAW_TYPE_EMITTER_ROTATION  = 6,
    GLITTER_PARTICLE_DRAW_TYPE_PARTICLE_ROTATION = 7,
    GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION     = 8,
    GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION_DUP = 9,
    GLITTER_PARTICLE_DRAW_TYPE_EMIT_POSITION     = 10,
} glitter_particle_draw_type;

typedef enum glitter_particle_flag {
    GLITTER_PARTICLE_NONE              = 0x00000,
    GLITTER_PARTICLE_LOOP              = 0x00001,
    GLITTER_PARTICLE_EMITTER_LOCAL     = 0x00004,
    GLITTER_PARTICLE_SCALE_Y_BY_X      = 0x00010,
    GLITTER_PARTICLE_REBOUND_PLANE     = 0x00020,
    GLITTER_PARTICLE_ROTATE_BY_EMITTER = 0x00040,
    GLITTER_PARTICLE_SCALE             = 0x00080,
    GLITTER_PARTICLE_TEXTURE_MASK      = 0x00100,
    GLITTER_PARTICLE_DEPTH_TEST        = 0x00200,
    GLITTER_PARTICLE_ROTATE_LOCUS      = 0x00800,
    GLITTER_PARTICLE_LOCAL             = 0x10000,
    GLITTER_PARTICLE_EMISSION          = 0x20000,
} glitter_particle_flag;

typedef enum glitter_particle_inst_flag {
    GLITTER_PARTICLE_INST_NONE     = 0x00,
    GLITTER_PARTICLE_INST_ENDED    = 0x01,
    GLITTER_PARTICLE_INST_NO_CHILD = 0x02,
} glitter_particle_inst_flag;

typedef enum glitter_particle_manager_flag {
    GLITTER_PARTICLE_MANAGER_PAUSE               = 0x01,
    GLITTER_PARTICLE_MANAGER_NOT_DISP            = 0x02,
    GLITTER_PARTICLE_MANAGER_RESET_SCENE_COUNTER = 0x04,
    GLITTER_PARTICLE_MANAGER_READ_FILES          = 0x08,
} glitter_particle_manager_flag;

typedef enum glitter_particle_sub_flag {
    GLITTER_PARTICLE_SUB_NONE       = 0x00000000,
    GLITTER_PARTICLE_SUB_UV_2ND_ADD = 0x00400000,
    GLITTER_PARTICLE_SUB_USE_CURVE  = 0x40000000,
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

typedef enum glitter_scene_flag {
    GLITTER_SCENE_NONE     = 0x00,
    GLITTER_SCENE_FLAG_1   = 0x01,
    GLITTER_SCENE_NOT_DISP = 0x02,
    GLITTER_SCENE_FLAG_4   = 0x04,
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    GLITTER_SCENE_EDITOR   = 0x80,
#endif
} glitter_scene_flag;

typedef enum glitter_type {
    GLITTER_FT = 0,
    GLITTER_F2 = 1,
    GLITTER_X  = 2,
} glitter_type;

typedef enum glitter_uv_index_type {
    GLITTER_UV_INDEX_FIXED                  = 0,
    GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED   = 1,
    GLITTER_UV_INDEX_RANDOM                 = 2,
    GLITTER_UV_INDEX_FORWARD                = 3,
    GLITTER_UV_INDEX_REVERSE                = 4,
    GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD = 5,
    GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE = 6,
    GLITTER_UV_INDEX_USER                   = 7,
} glitter_uv_index_type;

class GlitterAnimation;
typedef struct glitter_buffer glitter_buffer;
class GlitterCurve;
typedef struct glitter_effect_data glitter_effect_data;
typedef struct glitter_effect_ext_anim glitter_effect_ext_anim;
typedef struct glitter_effect_ext_anim_x glitter_effect_ext_anim_x;
class glitter_effect;
typedef struct glitter_emitter_box glitter_emitter_box;
typedef struct glitter_emitter_cylinder glitter_emitter_cylinder;
typedef struct glitter_emitter_polygon glitter_emitter_polygon;
typedef struct glitter_emitter_sphere glitter_emitter_sphere;
typedef struct glitter_emitter_data glitter_emitter_data;
class glitter_emitter;
class GlitterEmitterInst;
class GlitterF2EmitterInst;
class GlitterXEmitterInst;
class glitter_particle;
typedef struct glitter_particle_data glitter_particle_data;
class GlitterParticleInst;
class GlitterF2ParticleInst;
class GlitterF2ParticleInstData;
class GlitterXParticleInst;
class GlitterXParticleInstData;
typedef struct glitter_particle_mesh glitter_particle_mesh;
typedef struct glitter_random glitter_random;
typedef struct glitter_render_element glitter_render_element;
class GlitterRenderGroup;
class GlitterF2RenderGroup;
class GlitterXRenderGroup;
class GlitterRenderScene;
class GlitterF2RenderScene;
class GlitterXRenderScene;

class GltParticleManager;
class GlitterScene;

#define GPM GltParticleManager* glt_particle_manager
#define GPM_VAL (glt_particle_manager)

#define GLT glitter_type glt_type
#define GLT_VAL (glt_type)

extern const float_t glitter_min_emission;
extern const glitter_curve_type_flags glitter_effect_curve_flags;
extern const glitter_curve_type_flags glitter_emitter_curve_flags;
extern const glitter_curve_type_flags glitter_particle_curve_flags;
extern const glitter_curve_type_flags glitter_particle_x_curve_flags;
extern const glitter_direction glitter_emitter_direction_types[];
extern const size_t glitter_emitter_direction_types_count;
extern const glitter_direction glitter_emitter_direction_default_direction;
extern const glitter_emitter_direction glitter_emitter_direction_default;
extern const glitter_direction glitter_particle_draw_types[];
extern const size_t glitter_particle_draw_types_count;
extern const glitter_direction glitter_particle_draw_type_default_direction;
extern const glitter_particle_draw_type glitter_particle_draw_type_default;
extern const glitter_particle_blend glitter_particle_blend_draw_types[];
extern const size_t glitter_particle_blend_draw_types_count;
extern const glitter_particle_blend glitter_particle_blend_draw_default_blend;
extern const glitter_particle_blend_draw glitter_particle_blend_draw_default;
extern const glitter_particle_blend glitter_particle_blend_mask_types[];
extern const size_t glitter_particle_blend_mask_types_count;
extern const glitter_particle_blend glitter_particle_blend_mask_default_blend;
extern const glitter_particle_blend_mask glitter_particle_blend_mask_default;
extern const glitter_pivot glitter_pivot_reverse[];

struct glitter_random {
    uint32_t value;
    uint8_t step;
};

class GlitterAnimation {
public:
    std::vector<GlitterCurve*> curves;

    GlitterAnimation();
    ~GlitterAnimation();
};

struct glitter_buffer {
    vec3 position;
    vec2 uv;
    vec4u color;
};


class GlitterCurve {
public:
    struct Key {
        glitter_key_type type;
        int32_t frame;
        float_t value;
        float_t tangent1;
        float_t tangent2;
        float_t random_range;

        Key();
    };

    glitter_curve_type type;
    bool repeat;
    int32_t start_time;
    int32_t end_time;
    glitter_curve_flag flags;
    float_t random_range;
    std::vector<Key> keys;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    std::vector<Key> keys_rev;
#endif
    uint32_t version;
    uint32_t keys_version;

    GlitterCurve(GLT);
    virtual ~GlitterCurve();
};

struct glitter_effect_data {
    uint64_t name_hash;
    int32_t appear_time;
    int32_t life_time;
    int32_t start_time;
    union {
        glitter_effect_ext_anim* ext_anim;
        glitter_effect_ext_anim_x* ext_anim_x;
    };
    glitter_effect_flag flags;
    float_t emission;
    int32_t seed;
};

struct glitter_effect_ext_anim {
    glitter_effect_ext_anim_flag flags;
    union {
        struct {
            uint64_t object_hash;
            object_info object;
            char mesh_name[128];
        };
        struct {
            int32_t chara_index;
            glitter_effect_ext_anim_chara_node node_index;
        };
    };
};

struct glitter_effect_ext_anim_x {
    glitter_effect_ext_anim_flag flags;
    union {
        struct {
            uint32_t file_name_hash;
            uint32_t object_hash;
            int32_t instance_id;
            char mesh_name[128];
        };
        struct {
            int32_t chara_index;
            glitter_effect_ext_anim_chara_node node_index;
        };
    };
};

class glitter_effect {
public:
    char name[0x80];
    GlitterAnimation animation;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    glitter_effect_data data;
    std::vector<glitter_emitter*> emitters;
    uint32_t version;

    glitter_effect(GLT);
    virtual ~glitter_effect();
};

class GlitterEffectGroup {
public:
    std::vector<glitter_effect*> effects;
    int32_t load_count;
    uint64_t hash;
    GlitterScene* scene;
    float_t emission;
    uint32_t resources_count;
    std::vector<uint64_t> resource_hashes;
    txp_set resources_tex;
    texture** resources;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    std::vector<uint32_t> object_set_ids;
    std::string name;
#endif
    bool not_loaded;
    bool scene_init;
    uint32_t version;
    glitter_type type;

    GlitterEffectGroup(GLT);
    virtual ~GlitterEffectGroup();

#if defined(CRE_DEV) || defined(CLOUD_DEV)
    bool CheckModel();
    bool CheckLoadModel();
    void LoadModel(void* data);
    void FreeModel();
#endif
    bool ParseFile(f2_struct* st, object_database* obj_db);
    bool UnparseFile(GLT, f2_struct* st);
};

class GlitterRenderGroup {
public:
    glitter_particle_flag flags;
    glitter_particle_type type;
    glitter_direction draw_type;
    glitter_particle_blend blend_mode;
    glitter_particle_blend mask_blend_mode;
    glitter_pivot pivot;
    int32_t split_u;
    int32_t split_v;
    vec2 split_uv;
    float_t z_offset;
    size_t count;
    size_t ctrl;
    size_t disp;
    int32_t texture;
    int32_t mask_texture;
    float_t frame;
    mat4 mat;
    mat4 mat_rot;
    mat4 mat_draw;
    glitter_render_element* elements;
    size_t max_count;
    glitter_random* random_ptr;
    draw_pass_3d_type alpha;
    fog_id fog;
    GLuint vbo;
    GLuint ebo;
    float_t emission;
    std::vector<int32_t> vec_key;
    std::vector<int32_t> vec_val;

    GlitterRenderGroup();
    virtual ~GlitterRenderGroup();
};

class GlitterF2RenderGroup : public GlitterRenderGroup {
public:
    GlitterF2ParticleInst* particle;

    GlitterF2RenderGroup(GlitterF2ParticleInst* a1);
    virtual ~GlitterF2RenderGroup();

    bool CannotDisp();
    void Ctrl(GLT, float_t delta_frame, bool copy_mats);
    void DeleteBuffers(bool a2);
    void Disp(GPM);
    void Emit(GPM, GLT, GlitterF2ParticleInstData* a2,
        GlitterF2EmitterInst* a3, int32_t dup_count, int32_t count);
    void Free();
};

class GlitterXRenderGroup : public GlitterRenderGroup {
public:
    GlitterXParticleInst* particle;
    uint64_t object_name_hash;
    bool use_culling;

    GlitterXRenderGroup(GlitterXParticleInst* a1);
    virtual ~GlitterXRenderGroup();

    void CalcDisp(GPM);
    bool CannotDisp();
    void Ctrl(float_t delta_frame, bool copy_mats);
    void Disp(GPM);
    void DeleteBuffers(bool a2);
    void Emit(GlitterXParticleInstData* a2,
        GlitterXEmitterInst* a3, int32_t dup_count, int32_t count);
    void Free();
};

class GlitterF2RenderScene {
public:
    std::vector<GlitterF2RenderGroup*> groups;

    GlitterF2RenderScene();
    ~GlitterF2RenderScene();

    void Append(GlitterF2RenderGroup* rg);
    void Ctrl(GLT, float_t delta_frame);
    void Disp(GPM, draw_pass_3d_type alpha);
    size_t GetCtrlCount(glitter_particle_type type);
    size_t GetDispCount(glitter_particle_type type);
};

class GlitterXRenderScene {
public:
    std::vector<GlitterXRenderGroup*> groups;

    GlitterXRenderScene();
    ~GlitterXRenderScene();

    void Append(GlitterXRenderGroup* rg);
    void CalcDisp(GPM);
    void Ctrl(float_t delta_frame, bool copy_mats);
    void Disp(GPM, draw_pass_3d_type alpha);
    size_t GetCtrlCount(glitter_particle_type type);
    size_t GetDispCount(glitter_particle_type type);
};

class GlitterEffectInst {
public:
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    std::string name;
#endif
    glitter_effect* effect;
    glitter_effect_data data;
    float_t frame0;
    float_t frame1;
    vec4u color;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    float_t scale_all;
    mat4 mat;
    glitter_effect_inst_flag flags;
    size_t id;
    uint32_t random;

    GlitterEffectInst(GPM, GLT, glitter_effect* a1,
        size_t id, float_t emission, bool appear_now);
    virtual ~GlitterEffectInst();

    virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) = 0;
    virtual void Disp(GPM, draw_pass_3d_type alpha) = 0;
    virtual void Free(GPM, GLT, float_t emission, bool free) = 0;
    virtual size_t GetCtrlCount(glitter_particle_type type) = 0;
    virtual size_t GetDispCount(glitter_particle_type type) = 0;
    virtual bool HasEnded(bool a2) = 0;
    virtual void Reset(GPM, GLT, float_t emission) = 0;

    draw_pass_3d_type GetAlpha();
    fog_id GetFog();
};

class GlitterF2EffectInst : public GlitterEffectInst {
public:
    struct ExtAnim {
        union {
            struct {
                int32_t object_index;
                int32_t mesh_index;
                int32_t a3da_id;
                bool object_is_hrc;
                object_info object;
                const char* mesh_name;
            };
            struct {
                int32_t chara_index;
                int32_t bone_index;
            };
        };
        mat4 mat;
        vec3 translation;

        ExtAnim();
        ~ExtAnim();
    };

    std::vector<GlitterF2EmitterInst*> emitters;
    glitter_random* random_ptr;
    ExtAnim* ext_anim;
    vec3 ext_anim_scale;
    GlitterF2RenderScene render_scene;

    GlitterF2EffectInst(GPM, GLT, glitter_effect* a1,
        size_t id, float_t emission, bool appear_now);
    virtual ~GlitterF2EffectInst();

    virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) override;
    virtual void Disp(GPM, draw_pass_3d_type alpha) override;
    virtual void Free(GPM, GLT, float_t emission, bool free) override;
    virtual size_t GetCtrlCount(glitter_particle_type type) override;
    virtual size_t GetDispCount(glitter_particle_type type) override;
    virtual bool HasEnded(bool a2) override;
    virtual void Reset(GPM, GLT, float_t emission) override;
};

class GlitterXEffectInst : public GlitterEffectInst {
public:
    struct ExtAnim {
        union {
            struct {
                int32_t object_index;
                int32_t mesh_index;
                int32_t a3da_id;
                bool object_is_hrc;
                uint32_t file_name_hash;
                uint32_t object_hash;
                int32_t instance_id;
                const char* mesh_name;
            };
            struct {
                int32_t chara_index;
                int32_t bone_index;
            };
        };
        mat4 mat;
        vec3 translation;

        ExtAnim();
        ~ExtAnim();
    };

    std::vector<GlitterXEmitterInst*> emitters;
    mat4 mat_rot;
    mat4 mat_rot_eff_rot;
    glitter_random random_shared;
    ExtAnim* ext_anim;
    vec3 ext_anim_scale;
    GlitterXRenderScene render_scene;

    GlitterXEffectInst(GPM, glitter_effect* a1, size_t id, float_t emission, bool appear_now);
    virtual ~GlitterXEffectInst();

    virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) override;
    virtual void Disp(GPM, draw_pass_3d_type alpha) override;
    virtual void Free(GPM, GLT, float_t emission, bool free) override;
    virtual size_t GetCtrlCount(glitter_particle_type type) override;
    virtual size_t GetDispCount(glitter_particle_type type) override;
    virtual bool HasEnded(bool a2) override;
    virtual void Reset(GPM, GLT, float_t emission) override;

    void CalcDisp(GPM);
};

struct glitter_emitter_box {
    vec3 size;
};

struct glitter_emitter_cylinder {
    float_t radius;
    float_t height;
    float_t start_angle;
    float_t end_angle;
    bool on_edge;
    glitter_emitter_emission_direction direction;
};

struct glitter_emitter_polygon {
    float_t size;
    int32_t count;
    glitter_emitter_emission_direction direction;
};

struct glitter_emitter_sphere {
    float_t radius;
    float_t latitude;
    float_t longitude;
    bool on_edge;
    glitter_emitter_emission_direction direction;
};

struct glitter_emitter_data {
    int32_t start_time;
    int32_t life_time;
    int32_t loop_start_time;
    int32_t loop_end_time;
    glitter_emitter_flag flags;
    vec3 rotation_add;
    vec3 rotation_add_random;
    glitter_emitter_timer_type timer;
    float_t emission_interval;
    float_t particles_per_emission;
    glitter_direction direction;
    glitter_emitter_type type;
    glitter_emitter_box box;
    glitter_emitter_cylinder cylinder;
    glitter_emitter_sphere sphere;
    glitter_emitter_polygon polygon;
    int32_t seed;
};

class glitter_emitter {
public:
    GlitterAnimation animation;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    glitter_emitter_data data;
    std::vector<glitter_particle*> particles;
    uint32_t version;

    glitter_emitter(GLT);
    virtual ~glitter_emitter();
};

class GlitterEmitterInst {
public:
    glitter_emitter* emitter;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    mat4 mat;
    mat4 mat_rot;
    float_t scale_all;
    float_t emission_timer;
    glitter_emitter_data data;
    float_t emission_interval;
    float_t particles_per_emission;
    glitter_random* random_ptr;
    bool loop;
    glitter_emitter_emission emission;
    float_t frame;
    glitter_emitter_inst_flag flags;
    uint32_t random;

    GlitterEmitterInst(glitter_emitter* a1, glitter_random* random);
    virtual ~GlitterEmitterInst();
};

class GlitterF2EmitterInst : public GlitterEmitterInst {
public:
    std::vector<GlitterF2ParticleInst*> particles;

    GlitterF2EmitterInst(glitter_emitter* a1, GlitterF2EffectInst* a2, float_t emission);
    virtual ~GlitterF2EmitterInst();

    void Ctrl(GPM, GLT, GlitterF2EffectInst* a2, float_t delta_frame);
    void CtrlInit(GPM, GLT, GlitterF2EffectInst* a2, float_t delta_frame);
    void Emit(GPM, GLT, float_t delta_frame, float_t emission);
    void Free(GPM, GLT, float_t emission, bool free);
    bool HasEnded(bool a2);
    void Reset();
};

class GlitterXEmitterInst : public GlitterEmitterInst {
public:
    std::vector<GlitterXParticleInst*> particles;
    uint32_t counter;
    uint8_t step;

    GlitterXEmitterInst(glitter_emitter* a1, GlitterXEffectInst* a2, float_t emission);
    virtual ~GlitterXEmitterInst();

    void Ctrl(GPM, GlitterXEffectInst* a2, float_t delta_frame);
    void CtrlInit(GlitterXEffectInst* a2, float_t delta_frame);
    void Emit(float_t delta_frame, float_t emission);
    void Free(float_t emission, bool free);
    bool HasEnded(bool a2);
    uint8_t RandomGetStep();
    void RandomSetValue();
    void Reset();
};

class GlitterFileReader {
public:
    p_file_handler* file_handler;
    farc* farc;
    GlitterEffectGroup* effect_group;
    uint64_t hash;
    int32_t load_count;
    float_t emission;
    glitter_type type;
    std::string path;
    std::string file;
    int32_t state;
    bool init_scene;
    object_database* obj_db;

    GlitterFileReader(GLT);
    GlitterFileReader(GLT, const char* path, const char* file, float_t emission);
    GlitterFileReader(GLT, const wchar_t* path, const wchar_t* file, float_t emission);
    virtual ~GlitterFileReader();

    bool LoadFarc(void* data, const char* path, const char* file, uint64_t hash, object_database* obj_db);
    bool Read(GPM, object_database* obj_db);
    bool ReadFarc(GPM);
};

class GlitterLocusHistory {
public:
    struct Data {
        vec4u color;
        vec3 translation;
        float_t scale;
    };

    std::vector<Data> data;

    GlitterLocusHistory(size_t size);
    virtual ~GlitterLocusHistory();

    void Append(glitter_render_element* a2, GlitterF2ParticleInst* a3);
    void Append(glitter_render_element* a2, GlitterXParticleInst* a3);
};


struct glitter_particle_mesh {
    uint64_t object_name_hash;
    uint64_t object_set_name_hash;
    //char mesh_name[64];     // Unused
    //uint64_t sub_mesh_hash; // Unused
};

struct glitter_particle_data {
    glitter_particle_flag flags;
    int32_t life_time;
    int32_t life_time_random;
    int32_t fade_in;
    int32_t fade_in_random;
    int32_t fade_out;
    int32_t fade_out_random;
    glitter_particle_type type;
    glitter_pivot pivot;
    glitter_direction draw_type;
    float_t z_offset;
    vec3 rotation;
    vec3 rotation_random;
    vec3 rotation_add;
    vec3 rotation_add_random;
    vec3 scale;
    vec3 scale_random;
    vec3 direction;
    vec3 direction_random;
    float_t speed;
    float_t speed_random;
    float_t deceleration;
    float_t deceleration_random;
    vec3 gravity;
    vec3 acceleration;
    vec3 acceleration_random;
    float_t reflection_coeff;
    float_t reflection_coeff_random;
    float_t rebound_plane_y;
    vec4u color;
    glitter_uv_index_type uv_index_type;
    int32_t uv_index;
    int32_t frame_step_uv;
    int32_t uv_index_start;
    int32_t uv_index_end;
    int32_t uv_index_count;
    vec2 uv_scroll_add;
    float_t uv_scroll_add_scale;
    vec2 uv_scroll_2nd_add;
    float_t uv_scroll_2nd_add_scale;
    vec2 split_uv;
    uint8_t split_u;
    uint8_t split_v;
    glitter_particle_blend blend_mode;
    glitter_particle_blend mask_blend_mode;
    glitter_particle_sub_flag sub_flags;
    int32_t count;
    int32_t locus_history_size;
    int32_t locus_history_size_random;
    glitter_particle_draw_flag draw_flags;
    float_t emission;
    uint64_t tex_hash;
    uint64_t mask_tex_hash;
    int32_t texture;
    int32_t mask_texture;
    int32_t unk0;
    int32_t unk1;
    glitter_particle_mesh mesh;
};

class glitter_particle {
public:
    GlitterAnimation animation;
    glitter_particle_data data;
    int32_t version;

    glitter_particle(GLT);
    virtual ~glitter_particle();
};

class GlitterParticleInst {
public:
    glitter_particle* particle;

    GlitterParticleInst();
    virtual ~GlitterParticleInst();
};

class GlitterF2ParticleInstData {
public:
    glitter_particle_data data;
    glitter_particle_inst_flag flags;
    GlitterF2RenderGroup* render_group;
    glitter_random* random_ptr;
    GlitterF2EffectInst* effect;
    GlitterF2EmitterInst* emitter;
    GlitterF2ParticleInst* parent;
    glitter_particle* particle;
    std::vector<GlitterF2ParticleInst*> children;

    GlitterF2ParticleInstData();
    ~GlitterF2ParticleInstData();
};

class GlitterF2ParticleInst : public GlitterParticleInst {
public:
    GlitterF2ParticleInstData data;

    GlitterF2ParticleInst(glitter_particle* a1, GlitterF2EffectInst* a2,
        GlitterF2EmitterInst* a3, glitter_random* random, float_t emission);
    GlitterF2ParticleInst(GlitterF2ParticleInst* a1, float_t emission);
    virtual ~GlitterF2ParticleInst();

    void Emit(GPM, GLT, int32_t dup_count, int32_t count, float_t emission);
    void Free(bool free);
    bool HasEnded(bool a2);
    void Reset();
};

class GlitterXParticleInstData {
public:
    glitter_particle_data data;
    glitter_particle_inst_flag flags;
    GlitterXRenderGroup* render_group;
    glitter_random* random_ptr;
    GlitterXEffectInst* effect;
    GlitterXEmitterInst* emitter;
    GlitterXParticleInst* parent;
    glitter_particle* particle;
    std::vector<GlitterXParticleInst*> children;

    GlitterXParticleInstData();
    ~GlitterXParticleInstData();
};

class GlitterXParticleInst : public GlitterParticleInst {
public:
    GlitterXParticleInstData data;

    GlitterXParticleInst(glitter_particle* a1, GlitterXEffectInst* a2,
        GlitterXEmitterInst* a3, glitter_random* random, float_t emission);
    GlitterXParticleInst(GlitterXParticleInst* a1, float_t emission);
    virtual ~GlitterXParticleInst();

    void Emit(int32_t dup_count, int32_t count, float_t emission);
    void Free(bool free);
    bool HasEnded(bool a2);
    void Reset();
};

struct glitter_render_element {
    bool alive;
    uint8_t uv_index;
    bool disp;
    float_t frame;
    float_t life_time;
    union {
        float_t rebound_frame;
        float_t rebound_time;
    };
    float_t frame_step_uv;
    float_t base_speed;
    float_t speed;
    float_t deceleration;
    vec2 uv;
    vec4u color;
    vec3 base_translation;
    vec3 base_direction;
    vec3 translation;
    vec3 translation_prev;
    vec3 direction;
    vec3 acceleration;
    vec2 scale_particle;
    vec3 scale;
    vec3 rotation;
    vec3 rotation_add;
    float_t scale_all;
    vec2 uv_scroll;
    vec2 uv_scroll_2nd;
    float_t fade_out_frames;
    float_t fade_in_frames;
    mat4 mat;
    mat4 mat_draw;
    GlitterLocusHistory* locus_history;
    uint32_t random;
    uint8_t step;
};

typedef struct GlitterSceneEffect {
    GlitterEffectInst* ptr;
    bool disp;
} GlitterSceneEffect;

struct GlitterSceneCounter {
    uint32_t index : 8;
    uint32_t counter : 24;

    GlitterSceneCounter(uint32_t counter = 0);
    GlitterSceneCounter(uint32_t index, uint32_t counter);

    operator uint32_t() const { return (counter << 8) || index; }
};

class GlitterScene {
public:
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    std::string name;
#endif
    std::vector<GlitterSceneEffect> effects;
    GlitterSceneCounter counter;
    uint64_t hash;
    glitter_scene_flag flags;
    float_t emission;
    glitter_type type;
    GlitterEffectGroup* effect_group;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    float_t delta_frame_history;
    bool skip;
    FrameRateControl* frame_rate;
#endif

    GlitterScene(GlitterSceneCounter counter, uint64_t hash, GlitterEffectGroup* a4, bool a5);
    virtual ~GlitterScene();

#if defined(CRE_DEV) || defined(CLOUD_DEV)
    void CalcDisp(GPM);
#endif
    void Ctrl(GPM, float_t delta_frame);
    void Disp(GPM, draw_pass_3d_type alpha);
    size_t GetCtrlCount(glitter_particle_type ptcl_type);
    size_t GetDispCount(glitter_particle_type ptcl_type);
    void GetFrame(float_t* frame, int32_t* life_time);
    void GetStartEndFrame(int32_t* start_frame, int32_t* end_frame);
    bool FreeEffect(GPM, uint64_t effect_hash, bool free);
    bool HasEnded(bool a2);
    void InitEffect(GPM, glitter_effect* a2, size_t id, bool appear_now);
    bool ResetEffect(GPM, uint64_t effect_hash);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    void SetFrameRate(FrameRateControl* frame_rate);
#endif
};

class GltParticleManager : public Task {
public:
    std::vector<GlitterScene*> scenes;
    std::vector<GlitterFileReader*> file_readers;
    std::map<uint64_t, GlitterEffectGroup*> effect_groups;
    GlitterScene* scene;
    GlitterEffectInst* effect;
    GlitterEmitterInst* emitter;
    GlitterParticleInst* particle;
    void* rctx;
    void* bone_data;
    FrameRateControl* frame_rate;
    glitter_particle_manager_flag flags;
    int32_t scene_load_counter;
    float_t emission;
    float_t delta_frame;
    uint32_t texture_counter;
    glitter_random random;
    uint32_t counter;
    mat4 cam_projection;
    mat4 cam_view;
    mat4 cam_inv_view;
    mat3 cam_inv_view_mat3;
    vec3 cam_view_point;
    float_t cam_rotation_y;
    bool draw_all;
    bool draw_all_mesh;
    bool draw_selected;

    GltParticleManager();
    virtual ~GltParticleManager() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    virtual void Disp() override;
#endif
    virtual void Basic() override;

    bool AppendEffectGroup(uint64_t hash, GlitterEffectGroup* eff_group, GlitterFileReader* file_read);
    void BasicEffectGroups();
    uint64_t CalculateHash(const char* str);
    bool CheckNoFileReaders(uint64_t hash);
    void CtrlScenes();
    void Disp(draw_pass_3d_type draw_pass_type);
    void FreeEffects();
    void FreeSceneEffect(GlitterSceneCounter scene_counter, uint64_t hash, bool force_kill);
    void FreeScenes();
    size_t GetCtrlCount(glitter_particle_type type);
    size_t GetDispCount(glitter_particle_type type);
    GlitterEffectGroup* GetEffectGroup(uint64_t hash);
    const char* GetEffectName(uint64_t hash, int32_t index);
    size_t GetEffectsCount(uint64_t hash);
    bool GetPause();
    GlitterScene* GetScene(uint64_t hash);
    GlitterScene* GetScene(GlitterSceneCounter scene_counter);
    void GetStartEndFrame(int32_t* start_frame,
        int32_t* end_frame, uint64_t effect_group_hash);
    void GetStartEndFrame(int32_t* start_frame,
        int32_t* end_frame, GlitterSceneCounter scene_counter);
    float_t GetSceneFrameLifeTime(GlitterSceneCounter scene_counter, int32_t* life_time);
    GlitterSceneCounter GetSceneCounter(uint8_t index);
    GlitterSceneCounter Load(uint64_t effect_group_hash, uint64_t effect_hash, bool use_existing);
    uint64_t LoadFile(GLT, void* data, const char* file, const char* path,
        float_t emission, bool init_scene, object_database* obj_db);
    GlitterSceneCounter LoadScene(uint64_t effect_group_hash, uint64_t effect_hash, bool appear_now = true);
    GlitterSceneCounter LoadSceneEffect(uint64_t hash, bool appear_now = true);
    bool SceneHasNotEnded(GlitterSceneCounter load_counter);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    void SetFrame(GlitterEffectGroup* effect_group,
        GlitterScene** scene, float_t curr_frame, float_t prev_frame,
        uint32_t counter, glitter_random* random, bool reset);
    void SetSceneEffectName(uint64_t hash, uint64_t effect_hash, const char* name);
    void SetSceneFrameRate(uint64_t hash, FrameRateControl* frame_rate);
    void SetSceneName(uint64_t hash, const char* name);
#endif
    void SetPause(bool value);
    void UnloadEffectGroup(uint64_t hash);
    void sub_1403A53E0(float_t a2);
};

extern GltParticleManager glt_particle_manager;
