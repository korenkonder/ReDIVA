/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#if defined(CLOUD_DEV)
#include "../../KKdLib/default.h"
#include "../classes.h"

extern bool glitter_editor_init(class_data* data, render_context* rctx);
extern void glitter_editor_ctrl(class_data* data);
extern void glitter_editor_draw(class_data* data);
extern void glitter_editor_drop(class_data* data, size_t count, char** paths);
extern void glitter_editor_imgui(class_data* data);
extern void glitter_editor_input(class_data* data);
extern bool glitter_editor_dispose(class_data* data);
#endif
