/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

extern bool task_mask_screen_open();
extern bool task_mask_screen_close();

extern void task_mask_screen_fade_in(float_t duration, int32_t index);
extern void task_mask_screen_fade_out(float_t duration, int32_t index);
