/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "graphics.h"

classes_struct graphics_classes[] = {
    {
        "Background Color##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_background_color_init,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_background_color_imgui,
        graphics_background_color_input,
        graphics_background_color_render,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_background_color_dispose,
        {
            {},
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Post Process##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_post_process_init,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_post_process_imgui,
        graphics_post_process_input,
        graphics_post_process_render,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_post_process_dispose,
        {
            {},
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Glitter##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_glitter_init,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_glitter_imgui,
        graphics_glitter_input,
        graphics_glitter_render,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_glitter_dispose,
        {
            {},
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Render Settings##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_render_settings_init,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_render_settings_imgui,
        graphics_render_settings_input,
        graphics_render_settings_render,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        graphics_render_settings_dispose,
        {
            {},
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
};
