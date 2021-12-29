/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_test.h"
#include "data_test/auth_3d_test.h"
#include "data_test/glitter_test.h"
#include "data_test/stage_test.h"

classes_struct data_test_classes[] = {
    [DATA_TEST_AUTH_3D_TEST] = {
        .name    = "Auth 3D Test##Data Test",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = data_test_auth_3d_test_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = data_test_auth_3d_test_hide,
        .input   = data_test_auth_3d_test_input,
        .imgui   = data_test_auth_3d_test_imgui,
        .render  = data_test_auth_3d_test_render,
        .show    = data_test_auth_3d_test_show,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = data_test_auth_3d_test_dispose,
        .data    = {
            .lock = 0,
            .flags = CLASS_DISPOSED | CLASS_HIDDEN,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
    [DATA_TEST_GLITTER_TEST] = {
        .name    = "Glitter Test##Data Test",
#if defined(CLOUD_DEV)
        .flags   = CLASSES_DISPOSE_AT_HIDE | CLASSES_IN_CONTEXT_MENU,
#else
        .flags   = CLASSES_SHOW_AT_STARTUP | CLASSES_DISPOSE_AT_HIDE | CLASSES_IN_CONTEXT_MENU,
#endif
        .init    = data_test_glitter_test_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .input   = data_test_glitter_test_input,
        .imgui   = data_test_glitter_test_imgui,
        .render  = data_test_glitter_test_render,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = data_test_glitter_test_dispose,
        .data    = {
            .lock = 0,
            .flags = CLASS_DISPOSED | CLASS_HIDDEN,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
    [DATA_TEST_STAGE_TEST] = {
        .name    = "Stage Test##Data Test",
        .flags   = CLASSES_INIT_AT_STARTUP | CLASSES_IN_CONTEXT_MENU,
        .init    = data_test_stage_test_init,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = data_test_stage_test_hide,
        .input   = data_test_stage_test_input,
        .imgui   = data_test_stage_test_imgui,
        .render  = data_test_stage_test_render,
        .show    = data_test_stage_test_show,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = data_test_stage_test_dispose,
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
