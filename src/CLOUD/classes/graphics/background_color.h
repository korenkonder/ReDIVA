/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../../KKdLib/default.h"
#include "../../classes.h"

extern bool graphics_background_color_init(class_data* data, render_context* rctx);
extern void graphics_background_color_imgui(class_data* data);
extern void graphics_background_color_input(class_data* data);
extern void graphics_background_color_render(class_data* data);
extern bool graphics_background_color_dispose(class_data* data);
