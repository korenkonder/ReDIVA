/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "graphics.hpp"
#include "../graphics/light.hpp"
#include "graphics/background_color.hpp"
#include "graphics/face_light.hpp"
#include "graphics/fog.hpp"
#include "graphics/glitter.hpp"
#include "graphics/post_process.hpp"
#include "graphics/render_settings.hpp"

classes_data graphics_classes[] = {
    {
        "Background Color##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_background_color_init,
        graphics_background_color_imgui,
        graphics_background_color_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Light##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)light_dw_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        {
            CLASS_DW,
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Face Light##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_face_light_init,
        graphics_face_light_imgui,
        graphics_face_light_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Fog##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        graphics_fog_init,
        graphics_fog_imgui,
        graphics_fog_dispose,
        {
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
        graphics_post_process_imgui,
        graphics_post_process_dispose,
        {
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
        graphics_glitter_imgui,
        graphics_glitter_dispose,
        {
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
        graphics_render_settings_imgui,
        graphics_render_settings_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
};
