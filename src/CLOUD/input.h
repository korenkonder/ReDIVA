/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.h"

extern int32_t input_main(void* arg);
extern bool input_is_down(uint8_t keycode);
extern bool input_is_up(uint8_t keycode);
extern bool input_is_tapped(uint8_t keycode);
extern bool input_is_released(uint8_t keycode);
extern bool input_was_down(uint8_t keycode);
extern bool input_was_up(uint8_t keycode);
extern POINT input_mouse_position();
