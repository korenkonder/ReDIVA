/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.h"
#include "information/frame_speed.h"

classes_struct information_classes[] = {
    {
        "FrameSpeed##Information",
        CLASSES_IN_CONTEXT_MENU,
        information_frame_speed_init,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        information_frame_speed_imgui,
        information_frame_speed_input,
        information_frame_speed_render,
        CLASSES_STRUCT_NO_FUNC,
        CLASSES_STRUCT_NO_FUNC,
        information_frame_speed_dispose,
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
