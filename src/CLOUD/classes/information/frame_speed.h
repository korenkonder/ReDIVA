/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../../KKdLib/default.h"
#include "../../classes.h"

extern bool information_frame_speed_init(class_data* data, render_context* rctx);
extern void information_frame_speed_imgui(class_data* data);
extern void information_frame_speed_input(class_data* data);
extern void information_frame_speed_render(class_data* data);
extern bool information_frame_speed_dispose(class_data* data);
