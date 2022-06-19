/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../../KKdLib/default.hpp"
#include "../../classes.hpp"

extern bool graphics_post_process_init(class_data* data, render_context* rctx);
extern void graphics_post_process_imgui(class_data* data);
extern bool graphics_post_process_dispose(class_data* data);
