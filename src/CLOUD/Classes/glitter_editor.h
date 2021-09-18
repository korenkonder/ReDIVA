/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#if defined(CLOUD_DEV)
#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../../CRE/Glitter/glitter.h"

extern bool glitter_editor_enabled;

extern void glitter_editor_dispose();
extern void glitter_editor_init();
extern void glitter_editor_draw();
extern void glitter_editor_drop(size_t count, char** paths);
extern void glitter_editor_imgui();
extern void glitter_editor_input();
extern void glitter_editor_render();
#endif
