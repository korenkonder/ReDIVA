/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.h"
#include "information/frame_speed.h"

classes_struct information_classes[] = {
    [INFORMATION_FRAME_SPEED] = {
        .name    = "FrameSpeed##Information",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = information_frame_speed_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .input   = information_frame_speed_input,
        .imgui   = information_frame_speed_imgui,
        .render  = information_frame_speed_render,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = information_frame_speed_dispose,
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
