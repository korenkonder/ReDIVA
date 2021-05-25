/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.h"

classes_struct classes[] = {
#ifndef CLOUD_DEV
    {
        .name    = "Data Test",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &data_test_enabled,
        .dispose = (void*)0,
        .init    = (void*)0,
        .control = (void*)0,
        .drop    = (void*)0,
        .imgui   = (void*)0,
        .input   = (void*)0,
        .render  = (void*)0,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = data_test_classes,
        .sub_classes_count = 1,
    },
#endif
    {
        .name    = "Graphics",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &graphics_enabled,
        .dispose = (void*)0,
        .init    = (void*)0,
        .control = (void*)0,
        .drop    = (void*)0,
        .imgui   = (void*)0,
        .input   = (void*)0,
        .render  = (void*)0,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = graphics_classes,
        .sub_classes_count = 3,
    },
#ifdef CLOUD_DEV
    {
        .name    = "Glitter Editor",
        .flags   = CLASSES_INIT_AT_STARTUP,
        .enabled = &glitter_editor_enabled,
        .dispose = (void*)glitter_editor_dispose,
        .init    = (void*)glitter_editor_init,
        .control = (void*)0,
        .drop    = (void*)glitter_editor_drop,
        .imgui   = (void*)glitter_editor_imgui,
        .input   = (void*)glitter_editor_input,
        .render  = (void*)glitter_editor_render,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
#endif
};

const size_t classes_count = sizeof(classes) / sizeof(classes_struct);

render_state state;