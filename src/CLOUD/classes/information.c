/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.h"
#include "information/frame_speed.h"

classes_data information_classes[] = {
    {
        "FrameSpeed##Information",
        CLASSES_IN_CONTEXT_MENU,
        information_frame_speed_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        information_frame_speed_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
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
