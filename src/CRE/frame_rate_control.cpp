/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "frame_rate_control.hpp"
#include "static_var.hpp"

SysFrameRate sys_frame_rate;

FrameRateControl::FrameRateControl() {
    frame_speed = 1.0f;
}

FrameRateControl::~FrameRateControl() {

}

void FrameRateControl::SetFrameSpeed(float_t frame_speed) {
    this->frame_speed = frame_speed;
}

SysFrameRate::SysFrameRate() {
    frame_speed = 1.0f;
}

SysFrameRate::~SysFrameRate() {

}

float_t SysFrameRate::GetDeltaFrame() {
    return get_delta_frame() * get_anim_frame_speed() * frame_speed;
}
