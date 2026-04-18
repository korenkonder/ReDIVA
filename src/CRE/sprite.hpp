/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/sprite.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/rectangle.hpp"
#include "../KKdLib/spr.hpp"
#include "../KKdLib/vec.hpp"
#include "color.hpp"
#include "gl_rend_state.hpp"
#include "screen_param.hpp"
#include "texture.hpp"

#define BREAK_SPRITE_VERTEX_LIMIT (1)

namespace spr {
    enum SprAttr : uint32_t {
        /*M_SPR_ATTR_SCALESIZE_X = 0x800,
        M_SPR_ATTR_SCALESIZE_Y = 0x1000,
        M_SPR_ATTR_RATIOCROP = 0x2000,
        M_SPR_ATTR_IGNR_FMTRC = 0x4000,
        M_SPR_ATTR_NOTCORRECT_P = 0x8000,
        M_SPR_ATTR_NOTCORRECT_S = 0x10000,
        M_SPR_ATTR_MULTI = 0x40000,
        M_SPR_ATTR_BACKGROUND = 0x80000,
        M_SPR_ATTR_POLY = 0x1000000,
        M_SPR_ATTR_MESH = 0x2000000,
        M_SPR_ATTR_LINEAR = 0x4000000,
        M_SPR_ATTR_TEXTURE = 0x8000000,
        M_SPR_ATTR_EDGELINE = 0x10000000,
        M_SPR_ATTR_CROPPED = 0x20000000,
        M_SPR_ATTR_REQUESTED = 0x40000000,
        M_SPR_ATTR_ALLOCATED = 0x80000000,
        M_SPR_ATTR_SCALESIZE = 0x1800,
        M_SPR_ATTR_NOTCORRECT = 0x18000,*/

        SPR_ATTR_SHAPED       = 0x00000001,
        SPR_ATTR_CROPPED      = 0x00000002,

        SPR_ATTR_FLIP_H       = 0x00010000,
        SPR_ATTR_FLIP_V       = 0x00020000,
        SPR_ATTR_CTR_LT       = 0x00040000,
        SPR_ATTR_CTR_CT       = 0x00080000,
        SPR_ATTR_CTR_RT       = 0x00100000,
        SPR_ATTR_CTR_LC       = 0x00200000,
        SPR_ATTR_CTR_CC       = 0x00400000,
        SPR_ATTR_CTR_RC       = 0x00800000,
        SPR_ATTR_CTR_LB       = 0x01000000,
        SPR_ATTR_CTR_CB       = 0x02000000,
        SPR_ATTR_CTR_RB       = 0x04000000,
        SPR_ATTR_NOTRANSFORM  = 0x08000000,
        SPR_ATTR_EDGELINE     = 0x10000000,
        SPR_ATTR_IGNORE_ALPHA = 0x20000000,

        SPR_ATTR_CTR_ALL      = 0x07FC0000,
    };

    enum SprBlend {
        SPR_BLEND_DEFAULT = 0,
        SPR_BLEND_SCREEN,
        SPR_BLEND_ADD,
        SPR_BLEND_MODULATE,
        SPR_BLEND_BASE,
        SPR_BLEND_OVERLAY,
        SPR_BLEND_MAX,
    };

    enum SprKind {
        SPR_KIND_NORMAL = 0,
        SPR_KIND_MULTI,
        SPR_KIND_LINE,
        SPR_KIND_RECT,
        SPR_KIND_LINE_BOX,
        SPR_KIND_GLUT,
        SPR_KIND_LINES,
        SPR_KIND_POLY_LINE,
        SPR_KIND_STRIP,
    };

    enum SprLayer {
        SPR_LAYER_DEFAULT = 0,
        SPR_LAYER_1,
        SPR_LAYER_MAX,
    };

    enum SprPrio {
        /*SPR_PRIO_INFORMATION = SPR_PRIO_11,
        SPR_PRIO_FRONTSIDE = SPR_PRIO_20,
        SPR_PRIO_FOREGROUND = SPR_PRIO_24,
        SPR_PRIO_SCREENSAVER = SPR_PRIO_30,
        SPR_PRIO_VISTAFRAME = SPR_PRIO_23,
        SPR_PRIO_PHASE_BACKGROUND_STA = SPR_PRIO_00,
        SPR_PRIO_PHASE_BACKGROUND_END = SPR_PRIO_04,
        SPR_PRIO_PHASE_DEFAULT_STA = SPR_PRIO_05,
        SPR_PRIO_PHASE_DEFAULT_END = SPR_PRIO_20,
        SPR_PRIO_PHASE_FOREGROUND_STA = SPR_PRIO_21,
        SPR_PRIO_PHASE_FOREGROUND_END = SPR_PRIO_26,
        SPR_PRIO_PHASE_DEBUG_STA = SPR_PRIO_27,
        SPR_PRIO_PHASE_DEBUG_END = SPR_PRIO_31,*/

        SPR_PRIO_00 = 0,
        SPR_PRIO_01,
        SPR_PRIO_02,
        SPR_PRIO_03,
        SPR_PRIO_04,
        SPR_PRIO_05,
        SPR_PRIO_06,
        SPR_PRIO_07,
        SPR_PRIO_08,
        SPR_PRIO_09,
        SPR_PRIO_10,
        SPR_PRIO_11,
        SPR_PRIO_12,
        SPR_PRIO_13,
        SPR_PRIO_14,
        SPR_PRIO_15,
        SPR_PRIO_16,
        SPR_PRIO_17,
        SPR_PRIO_18,
        SPR_PRIO_19,
        SPR_PRIO_20,
        SPR_PRIO_21,
        SPR_PRIO_22,
        SPR_PRIO_23,
        SPR_PRIO_24,
        SPR_PRIO_25,
        SPR_PRIO_26,
        SPR_PRIO_27,
        SPR_PRIO_28,
        SPR_PRIO_29,
        SPR_PRIO_30,
        SPR_PRIO_31,
        SPR_PRIO_MAX,

        SPR_PRIO_BACKGROUND   = SPR_PRIO_01,
        SPR_PRIO_DEFAULT      = SPR_PRIO_07,
        SPR_PRIO_VISTAFRAME   = SPR_PRIO_19,
        SPR_PRIO_FOREGROUND   = SPR_PRIO_25,
        SPR_PRIO_DW           = SPR_PRIO_29,
        SPR_PRIO_GAME_BACK    = SPR_PRIO_03,
        SPR_PRIO_GAME_DEFAULT = SPR_PRIO_05,
        SPR_PRIO_GAME_FRONT   = SPR_PRIO_07,
        SPR_PRIO_WAIT_SCREEN  = SPR_PRIO_21,
        SPR_PRIO_INFORMATION  = SPR_PRIO_25,
    };

    enum SprTarget {
        SPR_TARGET_DEFAULT = -1,

        SPR_TARGET_FRONT = 0,
        SPR_TARGET_FRONT_3D_SURF,
        SPR_TARGET_BACK,
        SPR_TARGET_3,
        SPR_TARGET_MAX,
    };

    struct SprArgs {
        enum Flags : uint32_t {
            SPRITE_SIZE      = 0x01,
            TEXTURE_POS_SIZE = 0x02,
        };

        union IDUnion {
            int32_t index;
            uint32_t id;
            spr_info info;

            IDUnion() {
                index = -1;
                id = -1;
                info = {};
            }
        };

        struct Rect {
            float_t x;
            float_t y;
            float_t width;
            float_t height;
        };

        struct Quad {
            vec3 pos;
            vec2 uv;
            color4u8 color;
        };

        SprKind kind;
        IDUnion id;
        color4u8 color;
        SprAttr attr;
        SprBlend blend;
        SprTarget target;
        SprLayer layer;
        SprPrio prio;
        SCREEN_MODE screen_trans;
        SCREEN_MODE screen_scale;
        vec3 anchor;
        vec3 trans;
        vec3 scale;
        vec3 rot;
        float_t slant;
        float_t shear;
        mat4 matrix;
        const texture* tex;
        int32_t shader_name;
        int32_t sprite_draw_param_index;
#if BREAK_SPRITE_VERTEX_LIMIT
        size_t quad;
#else
        Quad* quad;
#endif
        size_t nb_quad;
        Flags flags;
        vec3 size;
        Rect rect;
        SprArgs* next;

        SprArgs();

        SprArgs::Quad* GetVertexArray();
        void Reset();
        void SetRect(float_t x, float_t y, float_t width, float_t height);
        void SetQuadArgs(SprArgs::Quad* quad_arg, size_t len);
        void SetSize(vec2 size);

        static void SetChild(SprArgs* args, SprArgs* next);
    };

    vec2 proj_sprite_3d_line(vec3 vec, bool offset);

    void put_cross(const mat4& mat, color4u8 color_x, color4u8 color_y, color4u8 color_z);
    void put_rgb_cross(const mat4& mat);
    SprArgs* put_sprite(const SprArgs& args, const sprite_database* spr_db);
    void put_sprite_3d_line(vec3 p1, vec3 p2, color4u8 color);
    void put_sprite_line(vec2 p1, vec2 p2, SCREEN_MODE screen_mode,
        SprPrio prio, color4u8 color, SprLayer layer = spr::SPR_LAYER_DEFAULT);
    void put_sprite_poly_line(vec2* points, size_t count, SCREEN_MODE screen_mode,
        SprPrio prio, color4u8 color, SprLayer layer = spr::SPR_LAYER_DEFAULT);
    void put_sprite_line_box(rectangle rect, SCREEN_MODE screen_mode,
        SprPrio prio, color4u8 color, SprLayer layer = spr::SPR_LAYER_DEFAULT);
    void put_sprite_rect(rectangle rect, SCREEN_MODE screen_mode,
        SprPrio prio, color4u8 color, SprLayer layer = spr::SPR_LAYER_DEFAULT);
    void put_sprite_strip(SprArgs::Quad* vert, size_t num, SCREEN_MODE screen_mode,
        SprPrio prio, int32_t spr_id, SprLayer layer = spr::SPR_LAYER_DEFAULT, const sprite_database* spr_db = 0);
}

extern void sprite_manager_init();
extern void sprite_manager_add_spr_sets(const sprite_database* spr_db);
extern void sprite_manager_clear();
extern void sprite_manager_draw(struct render_data_context& rend_data_ctx,
    int32_t index, bool font, texture* overlay_tex, const mat4& vp);
extern spr::SprTarget sprite_manager_get_target();
extern size_t sprite_manager_get_reqlist_count(int32_t index);
extern ::spr_set* sprite_manager_get_set(uint32_t set_id, const sprite_database* spr_db);
extern bool sprite_manager_get_set_ready(uint32_t set_id, const sprite_database* spr_db);
extern uint32_t sprite_manager_get_set_sprite_num(uint32_t set_id, const sprite_database* spr_db);
extern uint32_t sprite_manager_get_set_texture_num(uint32_t set_id, const sprite_database* spr_db);
extern const char* sprite_manager_get_spr_name(uint32_t spr_id, const sprite_database* spr_db);
extern rectangle sprite_manager_get_spr_rectangle(uint32_t spr_id, const sprite_database* spr_db);
extern texture* sprite_manager_get_spr_texture(uint32_t spr_id, const sprite_database* spr_db);
extern bool sprite_manager_load_file(uint32_t set_id, const sprite_database* spr_db);
extern bool sprite_manager_load_file_modern(uint32_t set_hash, sprite_database* spr_db);
extern void sprite_manager_post_draw();
extern void sprite_manager_pre_draw();
extern void sprite_manager_read_file(uint32_t set_id,
    std::string& mdata_dir, void* data, const sprite_database* spr_db);
extern void sprite_manager_read_file(uint32_t set_id,
    std::string&& mdata_dir, void* data, const sprite_database* spr_db);
extern void sprite_manager_read_file_modern(uint32_t set_hash, void* data, sprite_database* spr_db);
extern void sprite_manager_remove_spr_sets(const sprite_database* spr_db);
extern void sprite_manager_reset_req_list();
extern void sprite_manager_reset_res_data();
extern void sprite_manager_set_target(spr::SprTarget value);
extern void sprite_manager_set_res(double_t aspect, int32_t width, int32_t height);
extern void sprite_manager_unload_set(uint32_t set_id, const sprite_database* spr_db);
extern void sprite_manager_unload_set_modern(uint32_t set_hash, sprite_database* spr_db);
extern void sprite_manager_free();
