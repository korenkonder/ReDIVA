/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_test.h"
#include "data_test/auth_3d_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/stage_test.hpp"

classes_data data_test_classes[] = {
    {
        "Auth 3D Test##Data Test",
        CLASSES_IN_CONTEXT_MENU,
        data_test_auth_3d_test_init,
        data_test_auth_3d_test_ctrl,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_test_auth_3d_test_hide,
        data_test_auth_3d_test_imgui,
        CLASSES_DATA_NO_FUNC,
        data_test_auth_3d_test_show,
        CLASSES_DATA_NO_FUNC,
        data_test_auth_3d_test_dispose,
        &data_test_shared_lock,
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
        "Glitter Test##Data Test",
#if defined(CLOUD_DEV)
        (classes_enum)(CLASSES_DISPOSE_AT_HIDE | CLASSES_IN_CONTEXT_MENU),
#else
        (classes_enum)(CLASSES_SHOW_AT_STARTUP | CLASSES_DISPOSE_AT_HIDE | CLASSES_IN_CONTEXT_MENU),
#endif
        data_test_glitter_test_init,
        data_test_glitter_test_ctrl,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_test_glitter_test_imgui,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_test_glitter_test_dispose,
        &data_test_shared_lock,
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
        "Stage Test##Data Test",
        CLASSES_IN_CONTEXT_MENU,
        data_test_stage_test_init,
        data_test_stage_test_ctrl,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        data_test_stage_test_hide,
        data_test_stage_test_imgui,
        CLASSES_DATA_NO_FUNC,
        data_test_stage_test_show,
        CLASSES_DATA_NO_FUNC,
        data_test_stage_test_dispose,
        &data_test_shared_lock,
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
