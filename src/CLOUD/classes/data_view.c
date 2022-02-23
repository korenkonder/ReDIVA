/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_view.h"
#include "data_view/auth_3d.h"
#include "data_view/draw_task.h"
#include "data_view/object.h"
#include "data_view/texture.h"

classes_data data_view_classes[] = {
    {
        "Auth 3D##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_auth_3d_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_auth_3d_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_auth_3d_dispose,
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
        "Draw Task##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_draw_task_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_draw_task_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_draw_task_dispose,
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
        "Object##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_object_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_object_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_object_dispose,
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
        "Texture##Data View",
        CLASSES_IN_CONTEXT_MENU,
        data_view_texture_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_texture_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_view_texture_dispose,
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
