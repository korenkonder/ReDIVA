/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.h"

mu_Context* muctx;

const classes_struct classes[] = {
    {
        .name    = "BACKGROUND COLOR",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &background_color.enabled,
        .dispose = (void*)background_color_dispose,
        .init    = (void*)background_color_init,
        .control = (void*)background_color_control,
        .input   = (void*)background_color_input,
        .mui     = (void*)background_color_mui,
        .render  = (void*)background_color_render,
        .sound   = (void*)background_color_sound,
    },
    {
        .name    = "GLITTER TEST",
        .flags   = CLASSES_INIT_AT_STARTUP | CLASSES_IN_CONTEXT_MENU,
        .enabled = &glitter_test.enabled,
        .dispose = (void*)glitter_test_dispose,
        .init    = (void*)glitter_test_init,
        .control = (void*)glitter_test_control,
        .input   = (void*)glitter_test_input,
        .mui     = (void*)glitter_test_mui,
        .render  = (void*)glitter_test_render,
        .sound   = (void*)glitter_test_sound,
    },
    {
        .name    = "POST PROCESS",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &post_process.enabled,
        .dispose = (void*)post_process_dispose,
        .init    = (void*)post_process_init,
        .control = (void*)post_process_control,
        .input   = (void*)post_process_input,
        .mui     = (void*)post_process_mui,
        .render  = (void*)post_process_render,
        .sound   = (void*)post_process_sound,
    },
};

const size_t classes_count = sizeof(classes) / sizeof(classes_struct);
