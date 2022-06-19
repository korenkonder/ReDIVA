/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.hpp"
#include "information/frame_speed.hpp"
#include "information/task.hpp"

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
        "Task##Information",
        CLASSES_IN_CONTEXT_MENU,
        information_task_init,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        information_task_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        information_task_dispose,
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