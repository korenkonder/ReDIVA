/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "GL/shader_storage_buffer.hpp"
#include "GL/uniform_buffer.hpp"
#include "rob/rob.hpp"
#include "frame_rate_control.hpp"
#include "render_texture.hpp"
#include "stage.hpp"
#include "stage_param.hpp"
#include "task.hpp"

enum TaskEffectType {
    TASK_EFFECT_INVALID  = -1,
    TASK_EFFECT_AUTH_3D  = 0x00,
    TASK_EFFECT_TYPE_1   = 0x01,
    TASK_EFFECT_LEAF     = 0x02,
    TASK_EFFECT_TYPE_3   = 0x03,
    TASK_EFFECT_SNOW     = 0x04,
    TASK_EFFECT_TYPE_5   = 0x05,
    TASK_EFFECT_RIPPLE   = 0x06,
    TASK_EFFECT_RAIN     = 0x07,
    TASK_EFFECT_TYPE_8   = 0x08,
    TASK_EFFECT_TYPE_9   = 0x09,
    TASK_EFFECT_TYPE_10  = 0x0A,
    TASK_EFFECT_TYPE_11  = 0x0B,
    TASK_EFFECT_SPLASH   = 0x0C,
    TASK_EFFECT_TYPE_13  = 0x0D,
    TASK_EFFECT_FOG_ANIM = 0x0E,
    TASK_EFFECT_TYPE_15  = 0x0F,
    TASK_EFFECT_FOG_RING = 0x10,
    TASK_EFFECT_TYPE_17  = 0x11,
    TASK_EFFECT_PARTICLE = 0x12,
    TASK_EFFECT_LITPROJ  = 0x13,
    TASK_EFFECT_STAR     = 0x14,
};

struct particle_event_data {
    float_t type;
    float_t count;
    float_t size;
    vec3 trans;
    float_t force;

    particle_event_data();
};

extern void leaf_particle_draw();
extern void rain_particle_draw();
extern void particle_draw();
extern void snow_particle_draw();
extern void star_catalog_draw();

extern void task_effect_init();
extern void task_effect_free();

extern void task_effect_parent_event(TaskEffectType type, int32_t event_type, void* data);
extern void task_effect_parent_dest();
extern bool task_effect_parent_load();
extern void task_effect_parent_reset();
extern void task_effect_parent_set_current_stage_hash(uint32_t stage_hash);
extern void task_effect_parent_set_current_stage_index(int32_t stage_index);
extern void task_effect_parent_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data);
extern void task_effect_parent_set_enable(bool value);
extern void task_effect_parent_set_frame(int32_t value);
extern void task_effect_parent_set_frame_rate_control(FrameRateControl* value);
extern void task_effect_parent_set_stage_hashes(std::vector<uint32_t>& stage_hashes);
extern void task_effect_parent_set_stage_indices(std::vector<int32_t>& stage_indices);
extern bool task_effect_parent_unload();
