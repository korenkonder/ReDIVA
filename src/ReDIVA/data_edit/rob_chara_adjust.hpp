/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../task_window.hpp"

class RobCharaAdjust : public app::TaskWindow {
public:
    struct PartsData {
        int32_t transition_duration;
        uint8_t parts;
        int8_t type;
        uint8_t ignore_gravity;
        uint8_t cycle_type;
        vec3 external_force;
        vec3 external_force_cycle_strength;
        vec3 external_force_cycle;
        float_t cycle;
        float_t phase;
        float_t force;
        float_t strength;
        int32_t strength_transition;
    };

    struct GlobalData {
        int32_t transition_duration;
        int8_t type;
        uint8_t cycle_type;
        vec3 external_force;
        vec3 external_force_cycle_strength;
        vec3 external_force_cycle;
        float_t cycle;
        float_t phase;
    };

    bool apply;
    bool apply_wait;
    float_t apply_frame;
    bool save;
    bool track_frame;
    bool visible;

    int32_t chara_id;
    uint32_t motion_id;

    rob_osage_parts parts;

    void* data;

    RobCharaAdjust();
    virtual ~RobCharaAdjust() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

extern RobCharaAdjust* rob_chara_adjust;

extern void rob_chara_adjust_init();
extern void rob_chara_adjust_free();
