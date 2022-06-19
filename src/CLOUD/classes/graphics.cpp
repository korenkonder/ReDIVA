/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "graphics.hpp"
#include "graphics/background_color.hpp"
#include "graphics/glitter.hpp"
#include "graphics/post_process.hpp"
#include "graphics/render_settings.hpp"

classes_data graphics_classes[] = {
    {
        "Background Color##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_background_color_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_background_color_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_background_color_dispose,
        0,
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
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_post_process_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_post_process_dispose,
        0,
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
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_glitter_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_glitter_dispose,
        0,
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
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_render_settings_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        graphics_render_settings_dispose,
        0,
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
