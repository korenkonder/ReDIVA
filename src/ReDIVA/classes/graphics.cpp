/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "graphics.hpp"
#include "../graphics/color_change.hpp"
#include "../graphics/face_light.hpp"
#include "../graphics/fog.hpp"
#include "../graphics/glitter.hpp"
#include "../graphics/global_material.hpp"
#include "../graphics/light.hpp"
#include "../graphics/material.hpp"
#include "../graphics/post_process.hpp"
#include "../graphics/reflect_refract.hpp"
#include "../graphics/render_settings.hpp"
#include "../graphics/shadow.hpp"
#include "graphics/background_color.hpp"

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
        (classes_init_func)dw_face_light_init,
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
        "Fog##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)fog_dw_init,
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
        "Post Process##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)dw_post_process_init,
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
        "Color Change##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)color_change_dw_init,
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
        "Shadow##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)shadow_dw_init,
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
        "Global Material##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)global_material_dw_init,
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
        "Material##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)material_dw_init,
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
        "Ripple Effect##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
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
        "Rain Effect##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
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
        "Snow Effect##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
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
        "Reflect/Refract##Graphics",
        CLASSES_IN_CONTEXT_MENU,
       (classes_init_func)reflect_refract_dw_init,
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
        "Glitter##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)glitter_dw_init,
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
        "Render Settings##Graphics",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)render_settings_dw_init,
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
};
