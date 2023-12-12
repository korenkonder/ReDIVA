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

void FrameRateControl::set_frame_speed(float_t value) {
    frame_speed = value;
}

SysFrameRate::SysFrameRate() {

}

SysFrameRate::~SysFrameRate() {

}

float_t SysFrameRate::get_delta_frame() {
    return ::get_delta_frame() * get_anim_frame_speed() * frame_speed;
}

FrameRateControl* get_sys_frame_rate() {
    return &sys_frame_rate;
}
