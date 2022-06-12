/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

class FrameRateControl {
public:
    float_t frame_speed;

    FrameRateControl();
    virtual ~FrameRateControl();
    virtual float_t GetDeltaFrame() = 0;
    virtual void SetFrameSpeed(float_t frame_speed);
};

class SysFrameRate : public FrameRateControl {
public:
    SysFrameRate();
    virtual ~SysFrameRate() override;
    virtual float_t GetDeltaFrame() override;
};

class DivaPvFrameRate : public FrameRateControl {
public:
    DivaPvFrameRate();
    virtual ~DivaPvFrameRate() override;
    virtual float_t GetDeltaFrame() override;
};

class DivaStageFrameRate : public FrameRateControl {
public:
    DivaStageFrameRate();
    virtual ~DivaStageFrameRate() override;
    virtual float_t GetDeltaFrame() override;
};

extern SysFrameRate sys_frame_rate;
extern DivaPvFrameRate diva_pv_frame_rate;
extern DivaStageFrameRate diva_stage_frame_rate;
