/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "information.hpp"
#include "../information/dw_console.hpp"
#include "../information/frame_speed.hpp"
#include "../information/task.hpp"

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
        (classes_init_func)frame_speed_window_init,
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
        "Task##Information",
        CLASSES_IN_CONTEXT_MENU,
        (classes_init_func)dw_task_init,
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
