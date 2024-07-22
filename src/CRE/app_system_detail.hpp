/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

extern void finish_calc_process();
extern void finish_render_process();

extern float_t get_anim_frame_speed();
extern float_t get_delta_frame();
extern int32_t get_delta_frame_history_int();
extern float_t get_fps();
extern float_t get_frame_speed();
extern uint32_t get_main_timer();
extern float_t get_measured_fps();
extern bool get_pause();

extern void measure_fps_init(int32_t max_time_sec);
extern bool measure_fps_check_state();

extern void set_next_frame_speed(float_t value);
extern void set_vsync_emulation(bool value);

extern void start_frame_process();
extern void start_render_process();

extern void system_work_init(uint32_t a1);

extern bool sub_1401921D0();

extern int32_t sub_140192D00();
extern void sub_140194880(int32_t a1);
