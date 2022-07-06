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

extern SysFrameRate sys_frame_rate;
