/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.hpp"
#include "../information/dw_console.hpp"
#include "information/frame_speed.hpp"
#include "information/task.hpp"

classes_data information_classes[] = {
    {
        "Console##Information",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)dw_console_init,
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
        "FrameSpeed##Information",
        CLASSES_IN_CONTEXT_MENU,
        information_frame_speed_init,
        information_frame_speed_imgui,
        information_frame_speed_dispose,
        {
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
        information_task_imgui,
        information_task_dispose,
        {
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
};
