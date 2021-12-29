/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_view.h"
#include "data_view/auth_3d.h"
#include "data_view/draw_task.h"

classes_struct data_view_classes[] = {
    [DATA_VIEW_AUTH_3D] = {
        .name    = "Auth 3D##Data View",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = data_view_auth_3d_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .input   = data_view_auth_3d_input,
        .imgui   = data_view_auth_3d_imgui,
        .render  = data_view_auth_3d_render,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = data_view_auth_3d_dispose,
        .data    = {
            .lock = 0,
            .flags = CLASS_DISPOSED | CLASS_HIDDEN,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
    [DATA_VIEW_DRAW_TASK] = {
        .name    = "Draw Task##Data View",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = data_view_draw_task_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .input   = data_view_draw_task_input,
        .imgui   = data_view_draw_task_imgui,
        .render  = data_view_draw_task_render,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = data_view_draw_task_dispose,
        .data    = {
            .lock = 0,
            .flags = CLASS_DISPOSED | CLASS_HIDDEN,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
};
