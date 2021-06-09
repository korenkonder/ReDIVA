/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/mat.h"

typedef enum uniform_name {
    U_NONE          = 0x00,
    U01             = 0x01,
    U_ALPHA_TEST    = 0x02,
    U_ANISO         = 0x03,
    U_AET_BACK      = 0x04,
    U_TEXTURE_BLEND = 0x05,
    U_CHARA_COLOR   = 0x06,
    U07             = 0x07,
    U08             = 0x08,
    U_DEPTH_PEEL    = 0x09,
    U0A             = 0x0A,
    U0B             = 0x0B,
    U_ALPHA_BLEND   = 0x0C,
    U0D             = 0x0D,
    U_ESM_FILTER    = 0x0E,
    U_EXPOSURE      = 0x0F,
    U_SCENE_FADE    = 0x10,
    U_FADE          = 0x11,
    U12             = 0x12,
    U_FLARE         = 0x13,
    U_FOG_HEIGHT    = 0x14,
    U_FOG           = 0x15,
    U_BUFFER        = 0x16,
    U_GAUSS         = 0x17,
    U18             = 0x18,
    U_IMAGE_FILTER  = 0x19,
    U1A             = 0x1A,
    U_TONE_CURVE    = 0x1B,
    U1C             = 0x1C,
    U_MAGNIFY       = 0x1D,
    U1E             = 0x1E,
    U1F             = 0x1F,
    U20             = 0x20,
    U_MORPH_COLOR   = 0x21,
    U_MORPH         = 0x22,
    U_MOVIE         = 0x23,
    U24             = 0x24,
    U25             = 0x25,
    U26             = 0x26,
    U_NPR           = 0x27,
    U_LIGHT_1       = 0x28,
    U29             = 0x29,
    U2A             = 0x2A,
    U_SELF_SHADOW   = 0x2B,
    U2C             = 0x2C,
    U2D             = 0x2D,
    U2E             = 0x2E,
    U2F             = 0x2F,
    U_BONE_MAT      = 0x30,
    U_SNOW_PARTICLE = 0x31,
    U_SPECULAR_IBL  = 0x32,
    U_SPRITE_BLEND  = 0x33,
    U_TEX_0_TYPE    = 0x34,
    U_TEX_1_TYPE    = 0x35,
    U36             = 0x36,
    U37             = 0x37,
    U_STAR          = 0x38,
    U_TEXTURE_COUNT = 0x39,
    U_ENV_MAP       = 0x3A,
    U3B             = 0x3B,
    U_TRANSLUCENCY  = 0x3C,
    U_NORMAL        = 0x3D,
    U_TRANSPARENCY  = 0x3E,
    U3F             = 0x3F,
    U40             = 0x40,
    U41             = 0x41,
    U_LIGHT_0       = 0x42,
    U_SPECULAR      = 0x43,
    U_TONE_MAP      = 0x44,
    U45             = 0x45,
    U_MAX           = 0x46,
    U_INVALID       = 0xFFFFFFFF,
} uniform_name;

extern int32_t sv_max_texture_buffer_size;
extern int32_t sv_max_texture_size;
extern int32_t sv_max_texture_max_anisotropy;

extern bool sv_anisotropy_changed;
extern int32_t sv_anisotropy;

extern bool sv_fxaa_changed;
extern bool sv_fxaa;

extern bool sv_fxaa_preset_changed;
extern int32_t sv_fxaa_preset;

extern int32_t uniform_value[];

extern const vec3 sv_rgb_to_luma;
extern const mat3 sv_rgb_to_ypbpr;
extern const mat3 sv_ypbpr_to_rgb;

extern void sv_anisotropy_set(int32_t value);
extern void sv_fxaa_set(bool value);
extern void sv_fxaa_preset_set(int32_t value);

extern void bind_index_tex1d(int32_t index, int32_t id);
extern void bind_index_tex2d(int32_t index, int32_t id);
extern void bind_index_tex3d(int32_t index, int32_t id);
extern void bind_index_texcube(int32_t index, int32_t id);
extern void active_texture(int32_t index);
extern void bind_framebuffer(int32_t framebuffer);
extern void bind_vertex_array(int32_t array);
extern void bind_array_buffer(int32_t buffer);
extern void bind_element_array_buffer(int32_t buffer);
extern void bind_uniform_buffer(int32_t buffer);
extern void bind_shader_storage_buffer(int32_t buffer);
extern void bind_uniform_buffer_base(int32_t index, int32_t buffer);
extern void bind_shader_storage_buffer_base(int32_t index, int32_t buffer);
extern void bind_uniform_buffer_range(int32_t index, int32_t buffer, size_t offset, size_t size);
extern void bind_shader_storage_buffer_range(int32_t index, int32_t buffer, size_t offset, size_t size);
extern void bind_tex1d(int32_t id);
extern void bind_tex2d(int32_t id);
extern void bind_tex3d(int32_t id);
extern void bind_texcube(int32_t id);
extern bool check_index_tex1d_set(int32_t index);
extern bool check_index_tex2d_set(int32_t index);
extern bool check_index_tex3d_set(int32_t index);
extern bool check_index_texcube_set(int32_t index);
