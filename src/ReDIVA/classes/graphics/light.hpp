/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../../KKdLib/default.hpp"
#include "../../classes.hpp"

extern bool graphics_light_init(class_data* data, render_context* rctx);
extern void graphics_light_ctrl(class_data* data);
extern void graphics_light_imgui(class_data* data);
extern bool graphics_light_dispose(class_data* data);