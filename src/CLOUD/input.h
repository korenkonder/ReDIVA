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
extern double_t input_mouse_scroll_x();
extern double_t input_mouse_scroll_y();
extern void input_mouse_reset_scroll();
extern void input_mouse_add_scroll_x(double_t value);
extern void input_mouse_add_scroll_y(double_t value);
