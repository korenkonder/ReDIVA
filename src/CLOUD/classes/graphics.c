/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "graphics.h"

bool graphics_enabled = true;

classes_struct graphics_classes[3] = {
    {
        .name    = "Background Color",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &background_color_enabled,
        .dispose = (void*)background_color_dispose,
        .init    = (void*)background_color_init,
        .control = (void*)0,
        .drop    = (void*)0,
        .imgui   = (void*)background_color_imgui,
        .input   = (void*)background_color_input,
        .render  = (void*)background_color_render,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
    {
        .name    = "Post Process",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &post_process_enabled,
        .dispose = (void*)post_process_dispose,
        .init    = (void*)post_process_init,
        .control = (void*)0,
        .drop    = (void*)0,
        .imgui   = (void*)post_process_imgui,
        .input   = (void*)post_process_input,
        .render  = (void*)post_process_render,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
    {
        .name    = "Glitter##Counter",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .enabled = &glitter_counter_enabled,
        .dispose = (void*)glitter_counter_dispose,
        .init    = (void*)glitter_counter_init,
        .control = (void*)0,
        .drop    = (void*)0,
        .imgui   = (void*)glitter_counter_imgui,
        .input   = (void*)glitter_counter_input,
        .render  = (void*)glitter_counter_render,
        .sound   = (void*)0,
        .lock = 0,
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
};
