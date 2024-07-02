/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/stage.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/vec.hpp"
#include "frame_rate_control.hpp"

enum EffectType {
    EFFECT_INVALID  = -1,
    EFFECT_AUTH_3D  = 0x00,
    EFFECT_TYPE_1   = 0x01,
    EFFECT_LEAF     = 0x02,
    EFFECT_TYPE_3   = 0x03,
    EFFECT_SNOW     = 0x04,
    EFFECT_TYPE_5   = 0x05,
    EFFECT_RIPPLE   = 0x06,
    EFFECT_RAIN     = 0x07,
    EFFECT_TYPE_8   = 0x08,
    EFFECT_TYPE_9   = 0x09,
    EFFECT_TYPE_10  = 0x0A,
    EFFECT_TYPE_11  = 0x0B,
    EFFECT_SPLASH   = 0x0C,
    EFFECT_TYPE_13  = 0x0D,
    EFFECT_FOG_ANIM = 0x0E,
    EFFECT_TYPE_15  = 0x0F,
    EFFECT_FOG_RING = 0x10,
    EFFECT_TYPE_17  = 0x11,
    EFFECT_PARTICLE = 0x12,
    EFFECT_LITPROJ  = 0x13,
    EFFECT_STAR     = 0x14,
    EFFECT_MAX      = 0x15,
};

struct particle_event_data {
    float_t type;
    float_t count;
    float_t size;
    vec3 pos;
    float_t force;

    particle_event_data();
};

extern void leaf_particle_draw();
extern void rain_particle_draw();
extern void particle_draw();
extern void snow_particle_draw();
extern void star_catalog_draw();

extern void effect_init();
extern void effect_free();

extern void effect_fog_ring_data_reset();
extern void effect_splash_data_reset();

extern void effect_manager_event(EffectType type, int32_t event_type, void* data);
extern void effect_manager_dest();
extern bool effect_manager_load();
extern void effect_manager_reset();
extern void effect_manager_set_current_stage_hash(uint32_t stage_hash);
extern void effect_manager_set_current_stage_index(int32_t stage_index);
extern void effect_manager_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data);
extern void effect_manager_set_enable(bool value);
extern void effect_manager_set_frame(int32_t value);
extern void effect_manager_set_frame_rate_control(FrameRateControl* value);
extern void effect_manager_set_stage_hashes(const std::vector<uint32_t>& stage_hashes);
extern void effect_manager_set_stage_indices(const std::vector<int32_t>& stage_indices);
extern bool effect_manager_unload();
