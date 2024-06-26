/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_view.hpp"
#include "data_view/auth_3d.hpp"
#include "data_view/draw_task.hpp"
#include "data_view/glitter.hpp"
#include "data_view/object.hpp"
#include "data_view/texture.hpp"

classes_data data_view_classes[] = {
    {
        "Auth 3D##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_auth_3d_init,
        data_view_auth_3d_imgui,
        data_view_auth_3d_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Draw Task##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_draw_task_init,
        data_view_draw_task_imgui,
        data_view_draw_task_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Glitter##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_glitter_init,
        data_view_glitter_imgui,
        data_view_glitter_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Object##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_object_init,
        data_view_object_imgui,
        data_view_object_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
    {
        "Texture##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_texture_init,
        data_view_texture_imgui,
        data_view_texture_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
};
