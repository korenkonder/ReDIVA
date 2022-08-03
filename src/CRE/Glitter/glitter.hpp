/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <map>
#include <vector>
#include "../../KKdLib/database/object.hpp"
#include "../../KKdLib/f2/header.hpp"
#include "../../KKdLib/f2/struct.hpp"
#include "../../KKdLib/default.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/hash.hpp"
#include "../../KKdLib/mat.hpp"
#include "../../KKdLib/txp.hpp"
#include "../../KKdLib/vec.hpp"
#include "../file_handler.hpp"
#include "../frame_rate_control.hpp"
#include "../static_var.hpp"
#include "../task.hpp"
#include "../texture.hpp"

namespace Glitter {
    enum CurveFlag {
        CURVE_RANDOM_RANGE        = 0x01,
        CURVE_KEY_RANDOM_RANGE    = 0x02,
        CURVE_RANDOM_RANGE_NEGATE = 0x04,
        CURVE_STEP                = 0x08,
        CURVE_NEGATE              = 0x10,
        CURVE_RANDOM_RANGE_MULT   = 0x20,
        CURVE_BAKED               = 0x40,
        CURVE_BAKED_FULL          = 0x80,
    };

    enum CurveType {
        CURVE_TRANSLATION_X          = 0,
        CURVE_TRANSLATION_Y          = 1,
        CURVE_TRANSLATION_Z          = 2,
        CURVE_ROTATION_X             = 3,
        CURVE_ROTATION_Y             = 4,
        CURVE_ROTATION_Z             = 5,
        CURVE_SCALE_X                = 6,
        CURVE_SCALE_Y                = 7,
        CURVE_SCALE_Z                = 8,
        CURVE_SCALE_ALL              = 9,
        CURVE_COLOR_R                = 10,
        CURVE_COLOR_G                = 11,
        CURVE_COLOR_B                = 12,
        CURVE_COLOR_A                = 13,
        CURVE_COLOR_RGB_SCALE        = 14,
        CURVE_COLOR_R_2ND            = 15,
        CURVE_COLOR_G_2ND            = 16,
        CURVE_COLOR_B_2ND            = 17,
        CURVE_COLOR_A_2ND            = 18,
        CURVE_COLOR_RGB_SCALE_2ND    = 19,
        CURVE_EMISSION_INTERVAL      = 20,
        CURVE_PARTICLES_PER_EMISSION = 21,
        CURVE_U_SCROLL               = 22,
        CURVE_V_SCROLL               = 23,
        CURVE_U_SCROLL_ALPHA         = 24,
        CURVE_V_SCROLL_ALPHA         = 25,
        CURVE_U_SCROLL_2ND           = 26,
        CURVE_V_SCROLL_2ND           = 27,
        CURVE_U_SCROLL_ALPHA_2ND     = 28,
        CURVE_V_SCROLL_ALPHA_2ND     = 29,
    };

    enum CurveTypeFlags {
        CURVE_TYPE_TRANSLATION_X          = 0x00000001,
        CURVE_TYPE_TRANSLATION_Y          = 0x00000002,
        CURVE_TYPE_TRANSLATION_Z          = 0x00000004,
        CURVE_TYPE_ROTATION_X             = 0x00000008,
        CURVE_TYPE_ROTATION_Y             = 0x00000010,
        CURVE_TYPE_ROTATION_Z             = 0x00000020,
        CURVE_TYPE_SCALE_X                = 0x00000040,
        CURVE_TYPE_SCALE_Y                = 0x00000080,
        CURVE_TYPE_SCALE_Z                = 0x00000100,
        CURVE_TYPE_SCALE_ALL              = 0x00000200,
        CURVE_TYPE_COLOR_R                = 0x00000400,
        CURVE_TYPE_COLOR_G                = 0x00000800,
        CURVE_TYPE_COLOR_B                = 0x00001000,
        CURVE_TYPE_COLOR_A                = 0x00002000,
        CURVE_TYPE_COLOR_RGB_SCALE        = 0x00004000,
        CURVE_TYPE_COLOR_R_2ND            = 0x00008000,
        CURVE_TYPE_COLOR_G_2ND            = 0x00010000,
        CURVE_TYPE_COLOR_B_2ND            = 0x00020000,
        CURVE_TYPE_COLOR_A_2ND            = 0x00040000,
        CURVE_TYPE_COLOR_RGB_SCALE_2ND    = 0x00080000,
        CURVE_TYPE_EMISSION_INTERVAL      = 0x00100000,
        CURVE_TYPE_PARTICLES_PER_EMISSION = 0x00200000,
        CURVE_TYPE_U_SCROLL               = 0x00400000,
        CURVE_TYPE_V_SCROLL               = 0x00800000,
        CURVE_TYPE_U_SCROLL_ALPHA         = 0x01000000,
        CURVE_TYPE_V_SCROLL_ALPHA         = 0x02000000,
        CURVE_TYPE_U_SCROLL_2ND           = 0x04000000,
        CURVE_TYPE_V_SCROLL_2ND           = 0x08000000,
        CURVE_TYPE_U_SCROLL_ALPHA_2ND     = 0x10000000,
        CURVE_TYPE_V_SCROLL_ALPHA_2ND     = 0x20000000,

        CURVE_TYPE_TRANSLATION_XYZ     = 0x00000001 | 0x00000002 | 0x00000004,
        CURVE_TYPE_ROTATION_XYZ        = 0x00000008 | 0x00000010 | 0x00000020,
        CURVE_TYPE_SCALE_XYZ           = 0x00000040 | 0x00000080 | 0x00000100,
        CURVE_TYPE_COLOR_RGBA          = 0x00000400 | 0x00000800 | 0x00001000 | 0x00002000,
        CURVE_TYPE_COLOR_RGBA_2ND      = 0x00008000 | 0x00010000 | 0x00020000 | 0x00040000,
        CURVE_TYPE_UV_SCROLL           = 0x00400000 | 0x00800000,
        CURVE_TYPE_UV_SCROLL_ALPHA     = 0x01000000 | 0x02000000,
        CURVE_TYPE_UV_SCROLL_2ND       = 0x04000000 | 0x08000000,
        CURVE_TYPE_UV_SCROLL_ALPHA_2ND = 0x10000000 | 0x20000000,
    };

    enum Direction {
        DIRECTION_BILLBOARD         = 0,
        DIRECTION_EMITTER_DIRECTION = 1,
        DIRECTION_PREV_POSITION     = 2,
        DIRECTION_EMIT_POSITION     = 3,
        DIRECTION_Y_AXIS            = 4,
        DIRECTION_X_AXIS            = 5,
        DIRECTION_Z_AXIS            = 6,
        DIRECTION_BILLBOARD_Y_AXIS  = 7,
        DIRECTION_PREV_POSITION_DUP = 8,
        DIRECTION_EMITTER_ROTATION  = 9,
        DIRECTION_EFFECT_ROTATION   = 10,
        DIRECTION_PARTICLE_ROTATION = 11,
    };

    enum DispType {
        DISP_OPAQUE = 0,
        DISP_NORMAL = 1,
        DISP_ALPHA  = 2, // 3 in X
        DISP_TYPE_2 = 3, // 2 in X
        DISP_LOCAL  = 4,
    };

    enum EffectExtAnimFlag {
        EFFECT_EXT_ANIM_SET_ONCE            = 0x00001,
        EFFECT_EXT_ANIM_TRANS_ONLY          = 0x00002,
        EFFECT_EXT_ANIM_NO_TRANS_X          = 0x00004,
        EFFECT_EXT_ANIM_NO_TRANS_Y          = 0x00008,
        EFFECT_EXT_ANIM_NO_TRANS_Z          = 0x00010,
        EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA  = 0x00020,
        EFFECT_EXT_ANIM_GET_THEN_UPDATE     = 0x00040,
        EFFECT_EXT_ANIM_CHARA_ANIM          = 0x10000,
    };

    enum EffectExtAnimCharaNode {
        EFFECT_EXT_ANIM_CHARA_NONE            = -1,
        EFFECT_EXT_ANIM_CHARA_HEAD            = 0,
        EFFECT_EXT_ANIM_CHARA_MOUTH           = 1,
        EFFECT_EXT_ANIM_CHARA_BELLY           = 2,
        EFFECT_EXT_ANIM_CHARA_CHEST           = 3,
        EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER   = 4,
        EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW      = 5,
        EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP  = 6,
        EFFECT_EXT_ANIM_CHARA_LEFT_HAND       = 7,
        EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER  = 8,
        EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW     = 9,
        EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP = 10,
        EFFECT_EXT_ANIM_CHARA_RIGHT_HAND      = 11,
        EFFECT_EXT_ANIM_CHARA_LEFT_THIGH      = 12,
        EFFECT_EXT_ANIM_CHARA_LEFT_KNEE       = 13,
        EFFECT_EXT_ANIM_CHARA_LEFT_TOE        = 14,
        EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH     = 15,
        EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE      = 16,
        EFFECT_EXT_ANIM_CHARA_RIGHT_TOE       = 17,
        EFFECT_EXT_ANIM_CHARA_MAX             = 18,
    };

    enum EffectFlag {
        EFFECT_NONE       = 0x00,
        EFFECT_LOOP       = 0x01,
        EFFECT_LOCAL      = 0x02,
        EFFECT_ALPHA      = 0x04,
        EFFECT_FOG        = 0x08,
        EFFECT_FOG_HEIGHT = 0x10,
        EFFECT_EMISSION   = 0x20,
        EFFECT_USE_SEED   = 0x40,
    };

    enum EffectFileFlag {
        EFFECT_FILE_ALPHA      = 0x01,
        EFFECT_FILE_FOG        = 0x02,
        EFFECT_FILE_FOG_HEIGHT = 0x04,
        EFFECT_FILE_EMISSION   = 0x08,
        EFFECT_FILE_USE_SEED   = 0x10,
    };

    enum EffectInstFlag {
        EFFECT_INST_NONE                        = 0x0000000,
        EFFECT_INST_FREE                        = 0x0000001,
        EFFECT_INST_JUST_INIT                   = 0x0000002,
        EFFECT_INST_HAS_EXT_ANIM                = 0x0000004,
        EFFECT_INST_HAS_EXT_ANIM_TRANS          = 0x0000008,
        EFFECT_INST_HAS_EXT_ANIM_NON_INIT       = 0x0000010,
        EFFECT_INST_CHARA_ANIM                  = 0x0000020,
        EFFECT_INST_GET_EXT_ANIM_MAT            = 0x0000040,
        EFFECT_INST_SET_EXT_ANIM_ONCE           = 0x0000080,
        EFFECT_INST_SET_EXT_COLOR               = 0x0000100,
        EFFECT_INST_EXT_COLOR                   = 0x0000200,
        EFFECT_INST_SET_ADD_EXT_COLOR           = 0x0000400,
        EFFECT_INST_HAS_EXT_ANIM_SCALE          = 0x0000800,
        EFFECT_INST_NO_EXT_ANIM_TRANS_X         = 0x0001000,
        EFFECT_INST_NO_EXT_ANIM_TRANS_Y         = 0x0002000,
        EFFECT_INST_NO_EXT_ANIM_TRANS_Z         = 0x0004000,
        EFFECT_INST_EXT_ANIM_TRANS_ONLY         = 0x0008000,
        EFFECT_INST_FLAG_17                     = 0x0010000,
        EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE    = 0x0020000,
        EFFECT_INST_FLAG_19                     = 0x0040000,
        EFFECT_INST_DISP                        = 0x0080000,
        EFFECT_INST_FLAG_21                     = 0x0100000,
        EFFECT_INST_HAS_MESH                    = 0x0200000,
        EFFECT_INST_FLAG_23                     = 0x0400000,
        EFFECT_INST_FLAG_24                     = 0x0800000,
        EFFECT_INST_NOT_ENDED                   = 0x1000000,
    };

    enum EmitterDirection {
        EMITTER_DIRECTION_BILLBOARD        = 0,
        EMITTER_DIRECTION_BILLBOARD_Y_AXIS = 1,
        EMITTER_DIRECTION_Y_AXIS           = 2,
        EMITTER_DIRECTION_X_AXIS           = 3,
        EMITTER_DIRECTION_Z_AXIS           = 4,
        EMITTER_DIRECTION_EFFECT_ROTATION  = 5,
    };

    enum EmitterEmission {
        EMITTER_EMISSION_ON_TIMER = 0,
        EMITTER_EMISSION_ON_START = 1,
        EMITTER_EMISSION_ON_END   = 2,
        EMITTER_EMISSION_EMITTED  = 3,
    };

    enum EmitterEmissionDirection {
        EMITTER_EMISSION_DIRECTION_NONE    = 0,
        EMITTER_EMISSION_DIRECTION_OUTWARD = 1,
        EMITTER_EMISSION_DIRECTION_INWARD  = 2,
    };

    enum EmitterFlag {
        EMITTER_NONE        = 0x00,
        EMITTER_LOOP        = 0x01,
        EMITTER_KILL_ON_END = 0x02,
        EMITTER_USE_SEED    = 0x04,
    };

    enum EmitterInstFlag {
        EMITTER_INST_NONE         = 0x00,
        EMITTER_INST_ENDED        = 0x01,
        EMITTER_INST_HAS_DISTANCE = 0x02,
    };

    enum EmitterTimerType {
        EMITTER_TIMER_BY_TIME     = 0,
        EMITTER_TIMER_BY_DISTANCE = 1,
    };

    enum EmitterType {
        EMITTER_BOX      = 0,
        EMITTER_CYLINDER = 1,
        EMITTER_SPHERE   = 2,
        EMITTER_MESH     = 3,
        EMITTER_POLYGON  = 4,
    };

    enum FogType {
        FOG_NONE = 0,
        FOG_DEPTH,
        FOG_HEIGHT,
    };

    enum KeyType {
        KEY_CONSTANT = 0,
        KEY_LINEAR   = 1,
        KEY_HERMITE  = 2,
    };

    enum ParticleBlend {
        PARTICLE_BLEND_ZERO          = 0,
        PARTICLE_BLEND_TYPICAL       = 1,
        PARTICLE_BLEND_ADD           = 2,
        PARTICLE_BLEND_SUBTRACT      = 3,
        PARTICLE_BLEND_MULTIPLY      = 4,
        PARTICLE_BLEND_PUNCH_THROUGH = 5,
    };

    enum ParticleBlendDraw {
        PARTICLE_BLEND_DRAW_TYPICAL  = 0,
        PARTICLE_BLEND_DRAW_ADD      = 1,
        PARTICLE_BLEND_DRAW_MULTIPLY = 2,
    };

    enum ParticleBlendMask {
        PARTICLE_BLEND_MASK_TYPICAL  = 0,
        PARTICLE_BLEND_MASK_ADD      = 1,
        PARTICLE_BLEND_MASK_MULTIPLY = 2,
    };

    enum ParticleDrawFlag {
        PARTICLE_DRAW_NONE              = 0x0,
        PARTICLE_DRAW_NO_BILLBOARD_CULL = 0x1,
    };

    enum ParticleDrawType {
        PARTICLE_DRAW_TYPE_BILLBOARD         = 0,
        PARTICLE_DRAW_TYPE_BILLBOARD_Y_AXIS  = 1,
        PARTICLE_DRAW_TYPE_X_AXIS            = 2,
        PARTICLE_DRAW_TYPE_Y_AXIS            = 3,
        PARTICLE_DRAW_TYPE_Z_AXIS            = 4,
        PARTICLE_DRAW_TYPE_EMITTER_DIRECTION = 5,
        PARTICLE_DRAW_TYPE_EMITTER_ROTATION  = 6,
        PARTICLE_DRAW_TYPE_PARTICLE_ROTATION = 7,
        PARTICLE_DRAW_TYPE_PREV_POSITION     = 8,
        PARTICLE_DRAW_TYPE_PREV_POSITION_DUP = 9,
        PARTICLE_DRAW_TYPE_EMIT_POSITION     = 10,
    };

    enum ParticleFlag {
        PARTICLE_NONE              = 0x00000,
        PARTICLE_LOOP              = 0x00001,
        PARTICLE_EMITTER_LOCAL     = 0x00004,
        PARTICLE_SCALE_Y_BY_X      = 0x00010,
        PARTICLE_REBOUND_PLANE     = 0x00020,
        PARTICLE_ROTATE_BY_EMITTER = 0x00040,
        PARTICLE_SCALE             = 0x00080,
        PARTICLE_TEXTURE_MASK      = 0x00100,
        PARTICLE_DEPTH_TEST        = 0x00200,
        PARTICLE_ROTATE_LOCUS      = 0x00800,
        PARTICLE_LOCAL             = 0x10000,
        PARTICLE_EMISSION          = 0x20000,
    };

    enum ParticleInstFlag {
        PARTICLE_INST_NONE     = 0x00,
        PARTICLE_INST_ENDED    = 0x01,
        PARTICLE_INST_NO_CHILD = 0x02,
    };

    enum ParticleManagerFlag {
        PARTICLE_MANAGER_PAUSE               = 0x01,
        PARTICLE_MANAGER_NOT_DISP            = 0x02,
        PARTICLE_MANAGER_RESET_SCENE_COUNTER = 0x04,
        PARTICLE_MANAGER_READ_FILES          = 0x08,
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        PARTICLE_MANAGER_LOCAL               = 0x20,
#endif
    };

    enum ParticleSubFlag {
        PARTICLE_SUB_NONE       = 0x00000000,
        PARTICLE_SUB_UV_2ND_ADD = 0x00400000,
        PARTICLE_SUB_USE_CURVE  = 0x40000000,
    };

    enum ParticleType {
        PARTICLE_QUAD  = 0,
        PARTICLE_LINE  = 1,
        PARTICLE_LOCUS = 2,
        PARTICLE_MESH  = 3,
    };

    enum Pivot {
        PIVOT_TOP_LEFT      = 0,
        PIVOT_TOP_CENTER    = 1,
        PIVOT_TOP_RIGHT     = 2,
        PIVOT_MIDDLE_LEFT   = 3,
        PIVOT_MIDDLE_CENTER = 4,
        PIVOT_MIDDLE_RIGHT  = 5,
        PIVOT_BOTTOM_LEFT   = 6,
        PIVOT_BOTTOM_CENTER = 7,
        PIVOT_BOTTOM_RIGHT  = 8,
    };

    enum SceneFlag {
        SCENE_NONE     = 0x00,
        SCENE_FLAG_1   = 0x01,
        SCENE_NOT_DISP = 0x02,
        SCENE_FLAG_3   = 0x04,
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        SCENE_ENDED    = 0x08,
        SCENE_EDITOR   = 0x80,
#endif
    };

    enum Type {
        FT = 0,
        F2 = 1,
        X  = 2,
    };

    enum UVIndexType {
        UV_INDEX_FIXED                  = 0,
        UV_INDEX_INITIAL_RANDOM_FIXED   = 1,
        UV_INDEX_RANDOM                 = 2,
        UV_INDEX_FORWARD                = 3,
        UV_INDEX_REVERSE                = 4,
        UV_INDEX_INITIAL_RANDOM_FORWARD = 5,
        UV_INDEX_INITIAL_RANDOM_REVERSE = 6,
        UV_INDEX_USER                   = 7,
    };

    struct Animation;
    struct Buffer;
    struct Camera;
    struct Curve;
    class Effect;
    class Emitter;
    class EmitterInst;
    class F2EmitterInst;
    class XEmitterInst;
    class ItemBase;
    class Node;
    class Particle;
    class ParticleInst;
    class F2ParticleInst;
    class XParticleInst;
    struct Random;
    struct RenderElement;
    class RenderGroup;
    class F2RenderGroup;
    class XRenderGroup;
    class RenderScene;
    class F2RenderScene;
    class XRenderScene;

    class GltParticleManager;
    struct Scene;

#define GPM Glitter::GltParticleManager* glt_particle_manager
#define GPM_VAL (glt_particle_manager)

#define GLT Glitter::Type glt_type
#define GLT_VAL (glt_type)

    extern const float_t min_emission;
    extern const CurveTypeFlags effect_curve_flags;
    extern const CurveTypeFlags emitter_curve_flags;
    extern const CurveTypeFlags particle_curve_flags;
    extern const CurveTypeFlags particle_x_curve_flags;
    extern const Direction emitter_direction_types[];
    extern const size_t emitter_direction_types_count;
    extern const Direction emitter_direction_default_direction;
    extern const EmitterDirection emitter_direction_default;
    extern const Direction particle_draw_types[];
    extern const size_t particle_draw_types_count;
    extern const Direction particle_draw_type_default_direction;
    extern const ParticleDrawType particle_draw_type_default;
    extern const ParticleBlend particle_blend_draw_types[];
    extern const size_t particle_blend_draw_types_count;
    extern const ParticleBlend particle_blend_draw_default_blend;
    extern const ParticleBlendDraw particle_blend_draw_default;
    extern const ParticleBlend particle_blend_mask_types[];
    extern const size_t particle_blend_mask_types_count;
    extern const ParticleBlend particle_blend_mask_default_blend;
    extern const ParticleBlendMask particle_blend_mask_default;
    extern const Pivot pivot_reverse[];

    extern const char* effect_ext_anim_index_name[];
    extern const char* effect_ext_anim_node_index_name[];
    extern const char* effect_type_name[];
    extern const char* emitter_name[];
    extern const char* emitter_emission_name[];
    extern const char* emitter_emission_direction_name[];
    extern const char* emitter_direction_name[];
    extern const char* emitter_timer_name[];
    extern const char* key_name[];
    extern const char* particle_name[];
    extern const char* particle_blend_name[];
    extern const char* particle_blend_draw_name[];
    extern const char* particle_blend_mask_name[];
    extern const char* particle_draw_type_name[];
    extern const char* pivot_name[];
    extern const char* uv_index_type_name[];

    struct Animation {
        std::vector<Curve*> curves;

        Animation();
        ~Animation();

#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void AddValue(GLT, float_t val, CurveTypeFlags flags);
#endif
    };

    struct Buffer {
        vec3 position;
        vec2 uv;
        vec4 color;
    };

    struct Camera {
        mat4 projection;
        mat4 view;
        mat4 inv_view;
        mat3 inv_view_mat3;
        vec3 view_point;
        float_t rotation_y;

        Camera();
    };

    struct Curve {
        struct Key {
            KeyType type;
            int32_t frame;
            float_t value;
            float_t tangent1;
            float_t tangent2;
            float_t random_range;

            Key();
        };

        CurveType type;
        bool repeat;
        int32_t start_time;
        int32_t end_time;
        CurveFlag flags;
        float_t random_range;
        std::vector<Key> keys;
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        std::vector<Key> keys_rev;
#endif
        uint32_t version;
        uint32_t keys_version;

        Curve(GLT);
        virtual ~Curve();

#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void AddValue(GLT, float_t val);
#endif
        bool F2GetValue(GLT, float_t frame,
            float_t* value, int32_t random_value, Random* random);
        float_t F2Interpolate(GLT, float_t frame, Curve::Key* curr,
            Curve::Key* next, KeyType key_type, Random* random);
        float_t F2Randomize(GLT, float_t value, Random* random);
        float_t F2RandomizeKey(GLT, Curve::Key* key, Random* random);
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void FitKeysIntoCurve(GLT);
        void Recalculate(GLT);
#endif
        bool XGetValue(float_t frame,
            float_t* value, int32_t random_value, Random* random);
        float_t XInterpolate(float_t frame, Curve::Key* curr,
            Curve::Key* next, KeyType key_type, Random* random);
        float_t XRandomize(float_t value, Random* random);
        float_t XRandomizeKey(Curve::Key* key, Random* random);

        static void GetKeyIndices(std::vector<Curve::Key>* keys,
            float_t frame, size_t* curr, size_t* next);
    };

    class ItemBase {
    public:
        std::string name;
        Animation animation;

        ItemBase();
        virtual ~ItemBase();
    };

    class Node : public ItemBase {
    public:
        vec3 translation;
        vec3 rotation;
        vec3 scale;
        float_t scale_all;

        Node();
        virtual ~Node() override;
    };

    class Effect : public Node {
    public:
        struct ExtAnim {
            EffectExtAnimFlag flags;
            union {
                struct {
                    uint64_t object_hash;
                    object_info object;
                    char mesh_name[0x80];
                };
                struct {
                    int32_t chara_index;
                    EffectExtAnimCharaNode node_index;
                };
            };

            static object_info GetObjectInfo(uint64_t hash, object_database* obj_db);
        };

        struct ExtAnimX {
            EffectExtAnimFlag flags;
            union {
                struct {
                    uint32_t file_name_hash;
                    uint32_t object_hash;
                    int32_t instance_id;
                    char mesh_name[128];
                };
                struct {
                    int32_t chara_index;
                    EffectExtAnimCharaNode node_index;
                };
            };
        };

        struct Data {
            uint64_t name_hash;
            int32_t appear_time;
            int32_t life_time;
            int32_t start_time;
            vec4u8 color;
            union {
                Effect::ExtAnim * ext_anim;
                Effect::ExtAnimX* ext_anim_x;
            };
            EffectFlag flags;
            float_t emission;
            int32_t seed;

            Data(GLT);
        };

        Effect::Data data;
        std::vector<Emitter*> emitters;
        uint32_t version;

        Effect(GLT);
        virtual ~Effect() override;
    };

    struct EffectGroup {
        std::vector<Effect*> effects;
        int32_t load_count;
        uint64_t hash;
        Scene* scene;
        float_t emission;
        uint32_t resources_count;
        std::vector<uint64_t> resource_hashes;
        txp_set resources_tex;
        texture** resources;
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        std::vector<uint32_t> object_set_ids;
        std::string name;
#endif
        bool not_loaded;
        bool scene_init;
        uint32_t version;
        Type type;

        EffectGroup(GLT);
        virtual ~EffectGroup();

#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        bool CheckLoadModel();
        bool CheckModel();
        void FreeModel();
        void LoadModel(void* data);
#endif
    };

    struct Random {
        uint32_t value;
        uint8_t step;

        Random();

        float_t F2GetFloat(GLT, float_t value);
        float_t F2GetFloat(GLT, float_t min, float_t max);
        int32_t F2GetInt(GLT, int32_t value);
        int32_t F2GetInt(GLT, int32_t min, int32_t max);
        void F2GetVec3(GLT, vec3& src, vec3& dst);
        void F2StepValue();
        int32_t GetValue();
        void SetValue(int32_t value);
        float_t XGetFloat(float_t value);
        float_t XGetFloat(float_t min, float_t max);
        int32_t XGetInt(int32_t value);
        int32_t XGetInt(int32_t min, int32_t max);
        void XGetVec3(vec3& src, vec3& dst);
        void XReset();
        void XSetStep(uint8_t step);
        void XStepValue();

        static int32_t F2GetMax(GLT);
        static int32_t XGetMax();
    };

    class RenderGroup {
    public:
        ParticleFlag flags;
        ParticleType type;
        Direction draw_type;
        ParticleBlend blend_mode;
        ParticleBlend mask_blend_mode;
        Pivot pivot;
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
        RenderElement* elements;
        size_t max_count;
        Random* random_ptr;
        DispType disp_type;
        FogType fog_type;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        float_t emission;
        std::vector<std::pair<GLint, GLsizei>> draw_list;

        RenderGroup();
        virtual ~RenderGroup();

        RenderElement* AddElement(RenderElement* rend_elem);
    };

    class RenderScene {
    public:
        size_t disp_quad;
        size_t disp_locus;
        size_t disp_line;
        size_t disp_mesh;
        size_t ctrl_quad;
        size_t ctrl_locus;
        size_t ctrl_line;
        size_t ctrl_mesh;

        RenderScene();
        ~RenderScene();

        size_t GetCtrlCount(ParticleType type);
        size_t GetDispCount(ParticleType type);

        static void CalcDispLocusSetPivot(Pivot pivot,
            float_t w, float_t* v00, float_t* v01);
        static void CalcDispQuadSetPivot(Pivot pivot,
            float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11);
    };

    class F2RenderScene : public RenderScene {
    public:
        std::vector<F2RenderGroup*> groups;

        F2RenderScene();
        ~F2RenderScene();

        void Append(F2RenderGroup* rend_group);
        void CalcDispLine(F2RenderGroup* rend_group);
        void CalcDispLocus(GPM, F2RenderGroup* rend_group);
        void CalcDispQuad(GPM, F2RenderGroup* rend_group);
        void CalcDispQuadDirectionRotation(
            F2RenderGroup* rend_group, mat4* model_mat, mat4* dir_mat);
        void CalcDispQuadNormal(GPM,
            F2RenderGroup* rend_group, mat4* model_mat, mat4* dir_mat);
        void Ctrl(GLT, float_t delta_frame);
        void Disp(GPM, DispType disp_type);
        void Disp(GPM, F2RenderGroup* rend_group);
    };

    class XRenderScene : public RenderScene {
    public:
        std::vector<XRenderGroup*> groups;

        XRenderScene();
        ~XRenderScene();

        void Append(XRenderGroup* rend_group);
        void CalcDisp(GPM);
        void CalcDisp(GPM, XRenderGroup* rend_group);
        void CalcDispLine(XRenderGroup* rend_group);
        void CalcDispLocus(GPM, XRenderGroup* rend_group);
        void CalcDispMesh(GPM, XRenderGroup* rend_group);
        void CalcDispQuad(GPM, XRenderGroup* rend_group);
        void CalcDispQuadDirectionRotation(XRenderGroup* rend_group, mat4* model_mat);
        void CalcDispQuadNormal(XRenderGroup* rend_group, mat4* model_mat, mat4* dir_mat);
        bool CanDisp(DispType disp_type, bool a3);
        void Ctrl(float_t delta_frame, bool copy_mats);
        void Disp(GPM, DispType disp_type);
        void Disp(GPM, XRenderGroup* rend_group);
    };

    class EffectInst {
    public:
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        std::string name;
#endif
        Effect* effect;
        Effect::Data data;
        float_t frame0;
        float_t frame1;
        vec4 color;
        vec3 translation;
        vec3 rotation;
        vec3 scale;
        float_t scale_all;
        mat4 mat;
        EffectInstFlag flags;
        size_t id;
        uint32_t random;
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        float_t req_frame;
#endif
        vec4 ext_color;
        vec3 ext_anim_scale;
        float_t some_scale;

        EffectInst(GPM, GLT, Effect* eff,
            size_t id, float_t emission, bool appear_now);
        virtual ~EffectInst();

        virtual void Copy(EffectInst* dst, float_t emission) = 0;
        virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) = 0;
        virtual void Disp(GPM, DispType disp_type) = 0;
        virtual void Free(GPM, GLT, float_t emission, bool free) = 0;
        virtual size_t GetCtrlCount(ParticleType type) = 0;
        virtual size_t GetDispCount(ParticleType type) = 0;
        virtual bool HasEnded(bool a2) = 0;
        virtual void Reset(GPM, GLT, float_t emission) = 0;

        void SetExtColor(bool set, float_t r, float_t g, float_t b, float_t a);

        static int32_t GetExtAnimBoneIndex(GPM, EffectExtAnimCharaNode node);
    };

    class F2EffectInst : public EffectInst {
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

        std::vector<F2EmitterInst*> emitters;
        Random* random_ptr;
        ExtAnim* ext_anim;
        F2RenderScene render_scene;

        F2EffectInst(GPM, GLT, Effect* eff, size_t id, float_t emission, bool appear_now);
        virtual ~F2EffectInst() override;

        virtual void Copy(EffectInst* dst, float_t emission) override;
        virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) override;
        virtual void Disp(GPM, DispType disp_type) override;
        virtual void Free(GPM, GLT, float_t emission, bool free) override;
        virtual size_t GetCtrlCount(ParticleType type) override;
        virtual size_t GetDispCount(ParticleType type) override;
        virtual bool HasEnded(bool a2) override;
        virtual void Reset(GPM, GLT, float_t emission) override;

        void CtrlInit(GPM, GLT, float_t emission);
        void CtrlMat(GPM, GLT);
        DispType GetDispType();
        void GetExtAnim();
        bool GetExtAnimMat(mat4* mat);
        void GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a);
        FogType GetFog();
        void GetValue(GLT);
    };

    class XEffectInst : public EffectInst {
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

        std::vector<XEmitterInst*> emitters;
        mat4 mat_rot;
        mat4 mat_rot_eff_rot;
        Random random_shared;
        ExtAnim* ext_anim;
        XRenderScene render_scene;

        XEffectInst(GPM, Effect* eff, size_t id, float_t emission, bool appear_now);
        virtual ~XEffectInst() override;

        virtual void Copy(EffectInst* dst, float_t emission) override;
        virtual void Ctrl(GPM, GLT, float_t delta_frame, float_t emission) override;
        virtual void Disp(GPM, DispType disp_type) override;
        virtual void Free(GPM, GLT, float_t emission, bool free) override;
        virtual size_t GetCtrlCount(ParticleType type) override;
        virtual size_t GetDispCount(ParticleType type) override;
        virtual bool HasEnded(bool a2) override;
        virtual void Reset(GPM, GLT, float_t emission) override;

        void CalcDisp(GPM);
        void CtrlInit(GPM, float_t emission);
        void CtrlMat(GPM);
        DispType GetDispType();
        void GetExtAnim();
        bool GetExtAnimMat(mat4* mat);
        void GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a);
        FogType GetFog();
        void GetValue();
        void SetExtAnim(mat4* a2, mat4* a3, vec3* trans, bool set_flags);
    };

    class Emitter : public Node {
    public:
        struct Box {
            vec3 size;

            Box();
        };

        struct Cylinder {
            float_t radius;
            float_t height;
            float_t start_angle;
            float_t end_angle;
            bool on_edge;
            EmitterEmissionDirection direction;

            Cylinder();
        };

        struct Polygon {
            float_t size;
            int32_t count;
            EmitterEmissionDirection direction;

            Polygon();
        };

        struct Sphere {
            float_t radius;
            float_t latitude;
            float_t longitude;
            bool on_edge;
            EmitterEmissionDirection direction;

            Sphere();
        };

        struct Data {
            int32_t start_time;
            int32_t life_time;
            int32_t loop_start_time;
            int32_t loop_end_time;
            EmitterFlag flags;
            vec3 rotation_add;
            vec3 rotation_add_random;
            EmitterTimerType timer;
            float_t emission_interval;
            float_t particles_per_emission;
            Direction direction;
            EmitterType type;
            Emitter::Box box;
            Emitter::Cylinder cylinder;
            Emitter::Sphere sphere;
            Emitter::Polygon polygon;
            int32_t seed;

            Data();
        };

        Emitter::Data data;
        std::vector<Particle*> particles;
        uint32_t version;

        Emitter(GLT);
        virtual ~Emitter() override;
    };

    class EmitterInst {
    public:
        Emitter* emitter;
        vec3 translation;
        vec3 rotation;
        vec3 scale;
        mat4 mat;
        mat4 mat_rot;
        float_t scale_all;
        float_t emission_timer;
        Emitter::Data data;
        float_t emission_interval;
        float_t particles_per_emission;
        Random* random_ptr;
        bool loop;
        EmitterEmission emission;
        float_t frame;
        EmitterInstFlag flags;
        uint32_t random;

        EmitterInst(Emitter* emit, Random* random);
        virtual ~EmitterInst();
    };

    class F2EmitterInst : public EmitterInst {
    public:
        std::vector<F2ParticleInst*> particles;

        F2EmitterInst(Emitter* emit, F2EffectInst* eff_inst, float_t emission);
        virtual ~F2EmitterInst() override;

        void Copy(F2EmitterInst* dst, float_t emission);
        void Ctrl(GPM, GLT, F2EffectInst* eff_inst, float_t delta_frame);
        void CtrlInit(GPM, GLT, F2EffectInst* eff_inst, float_t delta_frame);
        void CtrlMat(GPM, GLT, F2EffectInst* eff_inst);
        void Emit(GPM, GLT, float_t delta_frame, float_t emission);
        void EmitParticle(GPM, GLT, float_t emission);
        void Free(GPM, GLT, float_t emission, bool free);
        void GetValue(GLT);
        bool HasEnded(bool a2);
        void InitMesh(GLT, int32_t index, vec3& scale,
            vec3& position, vec3& direction, Random* random);
        void Reset();
    };

    class XEmitterInst : public EmitterInst {
    public:
        std::vector<XParticleInst*> particles;
        uint32_t counter;
        uint8_t step;

        XEmitterInst(Emitter* emit, XEffectInst* eff_inst, float_t emission);
        virtual ~XEmitterInst() override;

        void Copy(XEmitterInst* dst, float_t emission);
        void Ctrl(GPM, XEffectInst* eff_inst, float_t delta_frame);
        void CtrlInit(XEffectInst* eff_inst, float_t delta_frame);
        void CtrlMat(GPM, XEffectInst* eff_inst);
        void Emit(float_t delta_frame, float_t emission);
        void EmitParticle(float_t emission);
        void Free(float_t emission, bool free);
        void GetValue();
        bool HasEnded(bool a2);
        void InitMesh(int32_t index, vec3& scale,
            vec3& position, vec3& direction, Random* random);
        uint8_t RandomGetStep();
        void RandomStepValue();
        void Reset();
    };

    struct FileReader {
        p_file_handler* file_handler;
        farc* farc;
        EffectGroup* effect_group;
        uint64_t hash;
        int32_t load_count;
        float_t emission;
        Type type;
        std::string path;
        std::string file;
        int32_t state;
        bool init_scene;
        object_database* obj_db;

        FileReader(GLT);
        FileReader(GLT, const char* path, const char* file, float_t emission);
        FileReader(GLT, const wchar_t* path, const wchar_t* file, float_t emission);
        virtual ~FileReader();

        bool LoadFarc(void* data, const char* path, const char* file, uint64_t hash, object_database* obj_db);
        void ParseAnimation(f2_struct* st, Animation* anim);
        void ParseCurve(f2_struct* st, Animation* anim);
        bool ParseDivaEffect(GPM, f2_struct* st, object_database* obj_db);
        bool ParseDivaList(f2_struct* st, EffectGroup* eff_group);
        bool ParseDivaResource(GPM, f2_struct* st, EffectGroup* eff_group);
        bool ParseEffect(f2_struct* st, EffectGroup* eff_group, object_database* obj_db);
        bool ParseEffectGroup(f2_struct* st, std::vector<Effect*>* vec,
            EffectGroup* eff_group, object_database* obj_db);
        bool ParseEmitter(f2_struct* st, Effect* eff, EffectGroup* eff_group);
        bool ParseParticle(f2_struct* st, Emitter* emit, Effect* eff, EffectGroup* eff_group);
        bool Read(GPM, object_database* obj_db);
        bool ReadFarc(GPM);
        void UnpackCurve(void* data, Animation* anim, Curve* c,
            uint32_t count, uint32_t keys_version, bool big_endian);
        bool UnpackDivaList(f2_struct* st, EffectGroup* eff_group);
        bool UnpackDivaResource(GPM, f2_struct* st, EffectGroup* eff_group);
        bool UnpackDivaResourceHashes(f2_struct* st, EffectGroup* eff_group);
        bool UnpackEffect(void* data, Effect* eff,
            int32_t efct_version, bool big_endian, object_database* obj_db);
        bool UnpackEmitter(void* data, Emitter* emit, uint32_t emit_version, bool big_endian);
        bool UnpackParticle(void* data, Particle* ptcl,
            uint32_t ptcl_version, Effect* eff, bool big_endian, EffectGroup* eff_group);
    };

    struct FileWriter {
        Type type;

        FileWriter();

        void PackCurve(f2_struct* st, Curve* c);
        bool PackDivaList(EffectGroup* eff_group, f2_struct* st);
        bool PackDivaResource(EffectGroup* eff_group, f2_struct* st);
        bool PackDivaResourceHashes(EffectGroup* eff_group, f2_struct* st);
        bool PackEffect(f2_struct* st, Effect* eff);
        bool PackEmitter(f2_struct* st, Emitter* emit);
        bool PackParticle(EffectGroup* eff_group, f2_struct* st, Particle* ptcl, Effect* eff);
        bool UnparseAnimation(f2_struct* st, Animation* anim, CurveTypeFlags flags);
        bool UnparseCurve(f2_struct* st, Curve* c);
        bool UnparseDivaEffect(EffectGroup* eff_group, f2_struct* st);
        bool UnparseDivaList(EffectGroup* eff_group, f2_struct* st);
        bool UnparseDivaResource(EffectGroup* eff_group, f2_struct* st);
        bool UnparseEffect(EffectGroup* eff_group, f2_struct* st, Effect* eff);
        void UnparseEffectGroup(EffectGroup* eff_group, f2_struct* st);
        bool UnparseEmitter(EffectGroup* eff_group, f2_struct* st, Emitter* emit, Effect* eff);
        bool UnparseParticle(EffectGroup* eff_group, f2_struct* st, Particle* ptcl, Effect* eff);

        static void Write(GLT, EffectGroup* eff_group, const char* path, const char* file, bool compress);
    };

    struct LocusHistory {
        struct Data {
            vec4 color;
            vec3 translation;
            float_t scale;
        };

        std::vector<Data> data;

        LocusHistory(size_t size);
        virtual ~LocusHistory();

        void Append(RenderElement* rend_elem, F2ParticleInst* ptcl_inst);
        void Append(RenderElement* rend_elem, XParticleInst* ptcl_inst);
    };

    class Particle : public ItemBase {
    public:
        struct Mesh {
            uint64_t object_name_hash;
            uint64_t object_set_name_hash;
            //char mesh_name[0x40];   // Unused
            //uint64_t sub_mesh_hash; // Unused
        };

        struct Data {
            ParticleFlag flags;
            int32_t life_time;
            int32_t life_time_random;
            int32_t fade_in;
            int32_t fade_in_random;
            int32_t fade_out;
            int32_t fade_out_random;
            ParticleType type;
            Pivot pivot;
            Direction draw_type;
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
            vec4 color;
            UVIndexType uv_index_type;
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
            ParticleBlend blend_mode;
            ParticleBlend mask_blend_mode;
            ParticleSubFlag sub_flags;
            int32_t count;
            int32_t locus_history_size;
            int32_t locus_history_size_random;
            ParticleDrawFlag draw_flags;
            float_t emission;
            uint64_t tex_hash;
            uint64_t mask_tex_hash;
            int32_t texture;
            int32_t mask_texture;
            int32_t unk0;
            int32_t unk1;
            Particle::Mesh mesh;
        };

        Particle::Data data;
        int32_t version;

        Particle(GLT);
        virtual ~Particle() override;
    };

    class ParticleInst {
    public:
        Particle* particle;

        ParticleInst();
        virtual ~ParticleInst();
    };

    class F2ParticleInst : public ParticleInst {
    public:
        struct Data {
            Particle::Data data;
            ParticleInstFlag flags;
            F2RenderGroup* render_group;
            Random* random_ptr;
            F2EffectInst* effect;
            F2EmitterInst* emitter;
            F2ParticleInst* parent;
            Particle* particle;
            std::vector<F2ParticleInst*> children;

            Data();
            ~Data();
        } data;

        F2ParticleInst(Particle* ptcl, F2EffectInst* eff_inst,
            F2EmitterInst* emit_inst, Random* random, float_t emission);
        F2ParticleInst(F2ParticleInst* ptcl_inst, float_t emission);
        virtual ~F2ParticleInst() override;

        void AccelerateParticle(GLT, RenderElement* rend_elem,
            float_t time, float_t delta_frame, Random* random);
        void Copy(F2ParticleInst* dst, float_t emission);
        void Emit(GPM, GLT, int32_t dup_count, int32_t count, float_t emission);
        void EmitParticle(GPM, GLT, RenderElement* rend_elem, F2EmitterInst* emit_inst,
            F2ParticleInst::Data* ptcl_inst_data, int32_t index, Random* random);
        void GetColor(RenderElement* rend_elem);
        void GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a);
        bool GetValue(GLT, RenderElement* rend_elem, float_t frame, Random* random);
        void Free(bool free);
        bool HasEnded(bool a2);
        void Reset();
        void StepUVParticle(GLT, RenderElement* rend_elem, float_t delta_frame, Random* random);
    };

    class XParticleInst : public ParticleInst {
    public:
        struct Data {
            Particle::Data data;
            ParticleInstFlag flags;
            XRenderGroup* render_group;
            Random* random_ptr;
            XEffectInst* effect;
            XEmitterInst* emitter;
            XParticleInst* parent;
            Particle* particle;
            std::vector<XParticleInst*> children;

            Data();
            ~Data();
        } data;

        XParticleInst(Particle* ptcl, XEffectInst* eff_inst,
            XEmitterInst* emit_inst, Random* random, float_t emission);
        XParticleInst(XParticleInst* parent, float_t emission);
        virtual ~XParticleInst() override;

        void AccelerateParticle(RenderElement* rend_elem,
            float_t delta_frame, Random* random);
        void Copy(XParticleInst* dst, float_t emission);
        void Emit(int32_t dup_count, int32_t count, float_t emission);
        void EmitParticle(RenderElement* rend_elem, XEmitterInst* emit_inst,
            XParticleInst::Data* ptcl_inst_data, int32_t index, uint8_t step, Random* random);
        void GetColor(RenderElement* rend_elem, float_t color_scale);
        void GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a);
        bool GetValue(RenderElement* rend_elem, float_t frame, Random* random, float_t* color_scale);
        void Free(bool free);
        bool HasEnded(bool a2);
        void Reset();
        void StepUVParticle(RenderElement* rend_elem, float_t delta_frame, Random* random);
    };

    class F2RenderGroup : public RenderGroup {
    public:
        F2ParticleInst* particle;

        F2RenderGroup(F2ParticleInst* ptcl_inst);
        virtual ~F2RenderGroup();

        bool CannotDisp();
        void Copy(F2RenderGroup* dst);
        void Ctrl(GLT, float_t delta_frame, bool copy_mats);
        void CtrlParticle(GLT, RenderElement* rend_elem, float_t delta_frame);
        void DeleteBuffers(bool a2);
        void Emit(GPM, GLT, F2ParticleInst::Data* ptcl_inst_data,
            F2EmitterInst* emit_inst, int32_t dup_count, int32_t count);
        void Free();
        void FreeData();
        bool GetExtAnimScale(vec3* ext_anim_scale, float_t* some_scale);

        static mat4 RotateToEmitPosition(F2RenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec);
        static mat4 RotateToPrevPosition(F2RenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec);
    };

    class XRenderGroup : public RenderGroup {
    public:
        XParticleInst* particle;
        uint64_t object_name_hash;
        bool use_culling;

        XRenderGroup(XParticleInst* ptcl_inst);
        virtual ~XRenderGroup();

        bool CannotDisp();
        void Copy(XRenderGroup* dst);
        void Ctrl(float_t delta_frame, bool copy_mats);
        void CtrlParticle(RenderElement* rend_elem, float_t delta_frame);
        void DeleteBuffers(bool a2);
        void Emit(XParticleInst::Data* ptcl_inst_data,
            XEmitterInst* emit_inst, int32_t dup_count, int32_t count);
        void Free();
        void FreeData();
        bool GetEmitterScale(vec3& emitter_scale);
        bool GetExtAnimScale(vec3* ext_anim_scale, float_t* some_scale);
        bool HasEnded();

        static mat4 RotateMeshToEmitPosition(XRenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec, vec3* trans);
        static mat4 RotateMeshToPrevPosition(XRenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec, vec3* trans);
        static mat4 RotateToEmitPosition(XRenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec);
        static mat4 RotateToPrevPosition(XRenderGroup* rend_group,
            RenderElement* rend_elem, vec3* vec);
    };

    struct RenderElement {
        bool alive;
        uint8_t uv_index;
        bool disp;
        float_t frame;
        float_t life_time;
        float_t rebound_time;
        float_t frame_step_uv;
        float_t base_speed;
        float_t speed;
        float_t deceleration;
        vec2 uv;
        vec4 color;
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
        LocusHistory* locus_history;
        uint32_t random;
        uint8_t step;

        RenderElement();

        void InitLocusHistory(GLT, F2ParticleInst* ptcl_inst, Random* random);
        void InitLocusHistory(XParticleInst* ptcl_inst, Random* random);
        void InitMesh(GLT, F2EmitterInst* emit_inst,
            F2ParticleInst::Data* ptcl_inst_data, int32_t index, Random* random);
        void InitMesh(XEmitterInst* emit_inst,
            XParticleInst::Data* ptcl_inst_data, int32_t index, Random* random);
    };

    struct SceneEffect {
        EffectInst* ptr;
        bool disp;
    };

    struct SceneCounter {
        uint32_t index : 8;
        uint32_t counter : 24;

        SceneCounter(uint32_t counter = 0);
        SceneCounter(uint32_t index, uint32_t counter);

        operator uint32_t() const { return (counter << 8) || index; }
    };

    struct Scene {
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        std::string name;
#endif
        std::vector<SceneEffect> effects;
        SceneCounter counter;
        uint64_t hash;
        SceneFlag flags;
        float_t emission;
        Type type;
        EffectGroup* effect_group;
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        float_t fade_frame;
        float_t fade_frame_left;
        float_t delta_frame_history;
        bool skip;
        FrameRateControl* frame_rate;
#endif

        Scene(SceneCounter counter, uint64_t hash, EffectGroup* eff_group, bool a5);
        virtual ~Scene();

#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void CalcDisp(GPM);
        bool CanDisp(DispType disp_type, bool a3);
#endif
        bool Copy(EffectInst* eff_inst, Scene* dst);
        void Ctrl(GPM, float_t delta_frame);
        void Disp(GPM, DispType disp_type);
        size_t GetCtrlCount(ParticleType ptcl_type);
        size_t GetDispCount(ParticleType ptcl_type);
        float_t GeFrameLifeTime(int32_t* life_time, size_t id);
        void GetStartEndFrame(int32_t* start_frame, int32_t* end_frame);
        bool FreeEffect(GPM, uint64_t effect_hash, bool free);
        bool FreeEffectByID(GPM, size_t id, bool free);
        bool HasEnded(bool a2);
        bool HasEnded(size_t id, bool a3);
        void InitEffect(GPM, Effect* eff, size_t id, bool appear_now);
        bool ResetEffect(GPM, uint64_t effect_hash, size_t* id = 0);
        bool SetExtColor(bool set, uint64_t effect_hash, float_t r, float_t g, float_t b, float_t a);
        bool SetExtColorByID(bool set, size_t id, float_t r, float_t g, float_t b, float_t a);
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void SetFrameRate(FrameRateControl* frame_rate);
        void SetReqFrame(size_t id, float_t req_frame);
#endif
    };

    class GltParticleManager : public app::Task {
    public:
        std::vector<Scene*> scenes;
        std::vector<FileReader*> file_readers;
        std::map<uint64_t, EffectGroup*> effect_groups;
        Scene* selected_scene;
        EffectInst* selected_effect;
        EmitterInst* selected_emitter;
        ParticleInst* selected_particle;
        void* bone_data;
        FrameRateControl* frame_rate;
        Camera cam;
        ParticleManagerFlag flags;
        int32_t scene_load_counter;
        float_t emission;
        float_t delta_frame;
        uint32_t texture_counter;
        Random random;
        uint32_t counter;
        bool draw_all;
        bool draw_all_mesh;
        bool draw_selected;

        GltParticleManager();
        virtual ~GltParticleManager() override;

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        virtual void Disp() override;
#endif
        virtual void Basic() override;

        bool AppendEffectGroup(uint64_t hash, EffectGroup* eff_group, FileReader* file_read);
        void BasicEffectGroups();
        uint64_t CalculateHash(const char* str);
#if defined(CRE_DEV)
        bool CheckHasLocalEffect();
#endif
        bool CheckHasFileReader(uint64_t hash);
        bool CheckNoFileReaders(uint64_t hash);
        bool CheckSceneEnded(SceneCounter scene_counter);
#if defined(CRE_DEV)
        void CheckSceneHasLocalEffect(Scene* sc);
#endif
        int32_t CounterGet();
        void CounterIncrement();
        void CtrlScenes();
        void DispScenes(DispType disp_type);
        void FreeEffects();
        void FreeSceneEffect(SceneCounter scene_counter, bool force_kill = true);
        void FreeSceneEffect(uint64_t effect_group_hash, uint64_t effect_hash, bool force_kill = true);
        void FreeScene(uint64_t effect_group_hash);
        void FreeScenes();
        size_t GetCtrlCount(ParticleType type);
        size_t GetDispCount(ParticleType type);
        EffectGroup* GetEffectGroup(uint64_t hash);
        const char* GetEffectName(uint64_t hash, int32_t index);
        size_t GetEffectsCount(uint64_t hash);
        bool GetPause();
        Scene* GetScene(uint64_t hash);
        Scene* GetScene(SceneCounter scene_counter);
        void GetSceneStartEndFrame(int32_t* start_frame, int32_t* end_frame, SceneCounter scene_counter);
        float_t GetSceneFrameLifeTime(SceneCounter scene_counter, int32_t* life_time);
        SceneCounter GetSceneCounter(uint8_t index = 0);
        SceneCounter Load(uint64_t effect_group_hash, uint64_t effect_hash, bool use_existing);
        uint64_t LoadFile(GLT, void* data, const char* file, const char* path,
            float_t emission, bool init_scene, object_database* obj_db);
        SceneCounter LoadScene(uint64_t effect_group_hash, uint64_t effect_hash, bool appear_now = true);
        SceneCounter LoadSceneEffect(uint64_t hash, bool appear_now = true);
        SceneCounter LoadSceneEffect(uint64_t hash, const char* name, bool appear_now = true);
        bool SceneHasNotEnded(SceneCounter load_counter);
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void SetFrame(EffectGroup* effect_group,
            Scene** scene, float_t curr_frame, float_t prev_frame,
            uint32_t counter, Random* random, bool reset);
#endif
        void SetSceneEffectExtColor(SceneCounter scene_counter, bool set,
            uint64_t effect_hash, float_t r, float_t g, float_t b, float_t a);
#if defined(CRE_DEV) || defined(ReDIVA_DEV)
        void SetSceneEffectName(uint64_t effect_group_hash, uint64_t effect_hash, const char* name);
        void SetSceneEffectReqFrame(SceneCounter scene_counter, float_t req_frame);
        void SetSceneFrameRate(SceneCounter scene_counter, FrameRateControl* frame_rate);
        void SetSceneName(uint64_t hash, const char* name);
#endif
        void SetPause(bool value);
        void UnloadEffectGroup(uint64_t hash);
        void sub_1403A53E0(float_t a2);
    };

    extern GltParticleManager* glt_particle_manager;

    extern void axis_angle_from_vectors(vec3* axis, float_t* angle, const vec3* vec0, const vec3* vec1);

    extern void glt_particle_manager_init();
    extern bool glt_particle_manager_append_task();
    extern bool glt_particle_manager_free_task();
    extern void glt_particle_manager_free();
}

