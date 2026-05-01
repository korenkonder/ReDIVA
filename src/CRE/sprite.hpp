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

struct render_data_context;

namespace spr {
    enum SPR_ATTR : uint32_t {
        M_SPR_ATTR_SHAPED       = 0x00000001,
        M_SPR_ATTR_CROPPED      = 0x00000002,

        M_SPR_ATTR_FLIP_H       = 0x00010000,
        M_SPR_ATTR_FLIP_V       = 0x00020000,
        M_SPR_ATTR_CTR_LT       = 0x00040000,
        M_SPR_ATTR_CTR_CT       = 0x00080000,
        M_SPR_ATTR_CTR_RT       = 0x00100000,
        M_SPR_ATTR_CTR_LC       = 0x00200000,
        M_SPR_ATTR_CTR_CC       = 0x00400000,
        M_SPR_ATTR_CTR_RC       = 0x00800000,
        M_SPR_ATTR_CTR_LB       = 0x01000000,
        M_SPR_ATTR_CTR_CB       = 0x02000000,
        M_SPR_ATTR_CTR_RB       = 0x04000000,
        M_SPR_ATTR_NOTRANSFORM  = 0x08000000,
        M_SPR_ATTR_EDGELINE     = 0x10000000,
        M_SPR_ATTR_IGNORE_ALPHA = 0x20000000,

        M_SPR_ATTR_CTR_ALL      = 0x07FC0000,
    };

    enum SPR_BLEND {
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

    enum SPR_LAYER {
        SPR_LAYER_DEFAULT = 0,
        SPR_LAYER_1,
        SPR_LAYER_MAX,
    };

    enum SPR_PRIO {
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

    enum SPR_TARGET {
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

        struct Rect {
            float_t x;
            float_t y;
            float_t width;
            float_t height;

            inline Rect() : x(), y(), width(), height() {

            }

            inline Rect(float_t x, float_t y, float_t width, float_t height)
                : x(x), y(y), width(width), height(height) {

            }
        };

        struct Quad {
            vec3 pos;
            vec2 uv;
            color4u8 color;

            inline Quad() {

            }
        };

        struct TexParam {
            const ::texture* texture;
            vec2 uv[4];
            uint32_t attr;

            inline TexParam() : texture(), attr() {

            }
        };

        SprKind kind;
        SprId id;
        color4u8 color;
        SPR_ATTR attr;
        SPR_BLEND blend;
        SPR_TARGET target;
        SPR_LAYER layer;
        SPR_PRIO prio;
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

        static const GLenum blend_table[][4];

        SprArgs();

        void draw(render_data_context& rend_data_ctx, const mat4& mat,
            int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max, texture* overlay_tex);
        SprArgs::Quad* getQuadArgs();
        void init();
        void setChild(SprArgs* args);
        void setQuadArgs(const SprArgs::Quad* quad_arg, size_t len);
        void setRect(float_t x, float_t y, float_t width, float_t height);
        void setSize(vec2 in_size);

        static void addDraw(SprArgs* args_array[0x100],
            const int32_t args_count, bool in_scale, struct SprArgsDraw& args_draw);

    private:
        int32_t calcTexture(SprArgs::TexParam* param, vec3 vertex[4], bool in_scale);
        int32_t calcTextureVertex(vec3 vertex[4], SprArgs::TexParam* param, bool in_scale);
        void calcVertex(vec3 vertex[4], mat4* trans_matrix, bool in_scale);
        void correctResolution();
        void drawObject(render_data_context& rend_data_ctx, const mat4& mat,
            int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max, texture* overlay_tex);
        void drawObjectCopyOverlayTex(render_data_context& rend_data_ctx, const mat4& mat, const vec3 vertex[4],
            int32_t over_x_min, int32_t over_y_min, int32_t over_x_max, int32_t over_y_max);
    };

    extern void dest();
    extern void dest(const SprDb* spr_db);
    extern void flush(render_data_context& rend_data_ctx,
        SPR_TARGET target, bool in_scale, texture* overlay_tex, const mat4& vp);
    extern void free(const std::vector<uint32_t>& setids, const SprDb* spr_db);
    extern void free(uint32_t setid, const SprDb* spr_db);
    extern void freeModern(const std::vector<uint32_t>& sethashes, SprDb* spr_db); // Added
    extern void freeModern(uint32_t sethash, SprDb* spr_db); // Added
    extern const char* getName(uint32_t uid, const SprDb* spr_db);
    extern size_t getObjListCount(SPR_TARGET target);
    extern bool getReady(uint32_t setid, const SprDb* spr_db);
    extern const SprArgs::Rect getRect(uint32_t uid, const SprDb* spr_db);
    extern uint32_t getSprNum(uint32_t setid, const SprDb* spr_db);
    extern SPR_TARGET getTarget();
    extern uint32_t getTexNum(uint32_t setid, const SprDb* spr_db);
    extern const texture* getTexture(uint32_t uid, const SprDb* spr_db);
    extern void getViewport(SCREEN_MODE& screen, rectangle& rect, SPR_LAYER layer);
    extern void init();
    extern void init(const SprDb* spr_db);
    extern void postFlush();
    extern void preFlush();
    extern SprArgs* putLine(vec2 p0, vec2 p1, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT);
    extern void putLineBox(rectangle rect, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT);
    extern SprArgs* putLines(const vec2* pos_arg, size_t len, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT);
    extern void putMask();
    extern SprArgs* putObject(const SprArgs& args, const SprDb* spr_db);
    extern SprArgs* put(const SprArgs& args, const SprDb* spr_db);
    extern void putPolyLine(const vec2* pos_arg, size_t len, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT);
    extern void putRect(rectangle rect, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT);
    extern void putStrip(const SprArgs::Quad* quad_arg, size_t len, SCREEN_MODE screen,
        SPR_PRIO prio, uint32_t spr_id, SPR_LAYER layer = spr::SPR_LAYER_DEFAULT, const SprDb* spr_db = 0);
    extern void refresh();
    extern void request(const std::vector<uint32_t>& setids,
        const std::string mdata_dir, void* data, const SprDb* spr_db);
    extern void request(uint32_t setid, const std::string mdata_dir, void* data, const SprDb* spr_db);
    extern void requestModern(const std::vector<uint32_t>& sethashes, void* data, SprDb* spr_db); // Added
    extern void requestModern(uint32_t sethash, void* data, SprDb* spr_db); // Added
    extern void setProjection(SPR_LAYER layer, float_t aspect);
    extern void setTarget(SPR_TARGET in_target);
    extern void setViewport(SPR_LAYER layer, SCREEN_MODE screen, const rectangle rect);
    extern bool wait(const std::vector<uint32_t>& setids, const SprDb* spr_db);
    extern bool wait(uint32_t setid, const SprDb* spr_db);
    extern bool waitModern(const std::vector<uint32_t>& sethashes, SprDb* spr_db);
    extern bool waitModern(uint32_t sethash, SprDb* spr_db); // Added
}

extern void sprite_manager_init();
extern ::spr_set* sprite_manager_get_set(uint32_t set_id, const SprDb* spr_db);
extern void sprite_manager_set_res(double_t aspect, int32_t width, int32_t height);
extern void sprite_manager_free();
