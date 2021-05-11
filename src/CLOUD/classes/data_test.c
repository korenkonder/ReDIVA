/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_test.h"

bool data_test_enabled = true;

classes_struct data_test_classes[1] = {
    {
        .name    = "Glitter Test",
        .flags   = CLASSES_INIT_AT_STARTUP | CLASSES_IN_CONTEXT_MENU,
        .enabled = &glitter_test_enabled,
        .dispose = (void*)glitter_test_dispose,
        .init    = (void*)glitter_test_init,
        .control = (void*)0,
        .drop    = (void*)0,
        .input   = (void*)glitter_test_input,
        .imgui   = (void*)glitter_test_imgui,
        .render  = (void*)glitter_test_render,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
};
