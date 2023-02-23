/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include "../KKdLib/default.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "resolution_mode.hpp"
#include "texture.hpp"
#include "gl_state.hpp"

namespace spr {
    enum SprKind {
        SPR_KIND_NORMAL = 0,
        SPR_KIND_LINE,
        SPR_KIND_LINES,
        SPR_KIND_RECT,
        SPR_KIND_MULTI,
        SPR_KIND_ARROW_A,
        SPR_KIND_ARROW_B,
        SPR_KIND_ARROW_AB,
        SPR_KIND_TRIANGLE,
        SPR_KIND_CIRCLE,
    };
    
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

        SPR_ATTR_FLIP_H  = 0x00010000,
        SPR_ATTR_FLIP_V  = 0x00020000,
        SPR_ATTR_CTR_LT  = 0x00040000,
        SPR_ATTR_CTR_CT  = 0x00080000,
        SPR_ATTR_CTR_RT  = 0x00100000,
        SPR_ATTR_CTR_LC  = 0x00200000,
        SPR_ATTR_CTR_CC  = 0x00400000,
        SPR_ATTR_CTR_RC  = 0x00800000,
        SPR_ATTR_CTR_LB  = 0x01000000,
        SPR_ATTR_CTR_CB  = 0x02000000,
        SPR_ATTR_CTR_RB  = 0x04000000,
        SPR_ATTR_MATRIX  = 0x08000000,
        SPR_ATTR_NOBLEND = 0x20000000,

        SPR_ATTR_CTR     = 0x07FC0000,
    };

    struct SprArgs {
        enum Flags : uint32_t {
            SPRITE_SIZE      = 0x01,
            TEXTURE_POS_SIZE = 0x02,
        };

        spr::SprKind kind;
        union {
            int32_t index;
            //spr_info info;
        } id;
        vec4 color;
        SprAttr attr;
        int32_t blend;
        int32_t index;
        int32_t layer;
        int32_t prio;
        resolution_mode resolution_mode_screen;
        resolution_mode resolution_mode_sprite;
        vec3 center;
        vec3 trans;
        vec3 scale;
        vec3 rot;
        vec2 skew_angle;
        mat4 mat;
        texture* texture;
        int32_t shader;
        int32_t field_AC;
        //sprite_vertex* vertex_array;
        size_t num_vertex;
        Flags flags;
        vec2 sprite_size;
        int32_t field_CC;
        vec2 texture_pos;
        vec2 texture_size;
        spr::SprArgs* next;
    };

    struct SprInfo {
        uint32_t texid;
        int32_t rotate;
        float_t su;
        float_t sv;
        float_t eu;
        float_t ev;
        float_t px;
        float_t py;
        float_t width;
        float_t height;
    };

    struct SpriteVertex {
        vec3 pos;
        vec2 uv;
        vec4u8 color;
    };

    struct TexCoord {
        struct UV {
            float_t u;
            float_t v;

            inline UV() : u(), v() {

            }
        };

        UV uv[4];

        inline TexCoord() {

        }
    };

    struct TexParam {
        texture* texture;
        TexCoord texcoord;
        //int32_t pad[2];

        inline TexParam() : texture()/*, pad()*/ {

        }
    };

    void calc_sprite_vertex(spr::SprArgs* args, vec3* vtx, mat4* mat, bool font);
}

extern void sprite_manager_init();
extern void sprite_manager_load_set(int32_t set_id, std::string* mdata_dir);
extern void sprite_manager_reset_res_data();
extern void sprite_manager_free();
