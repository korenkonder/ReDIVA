/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.h"
#include "classes/data_test.h"
#include "classes/data_view.h"
#include "classes/glitter_editor.h"
#include "classes/graphics.h"
#include "classes/information.h"

classes_struct classes[] = {
    {
        .name    = "Data Test",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = CLASSES_STRUCT_NO_FUNC,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .imgui   = CLASSES_STRUCT_NO_FUNC,
        .input   = CLASSES_STRUCT_NO_FUNC,
        .render  = CLASSES_STRUCT_NO_FUNC,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = CLASSES_STRUCT_NO_FUNC,
        .data    = {
            .lock = 0,
            .flags = CLASS_INIT,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = data_test_classes,
        .sub_classes_count = 3,
    },
    {
        .name    = "Data View",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = CLASSES_STRUCT_NO_FUNC,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .imgui   = CLASSES_STRUCT_NO_FUNC,
        .input   = CLASSES_STRUCT_NO_FUNC,
        .render  = CLASSES_STRUCT_NO_FUNC,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = CLASSES_STRUCT_NO_FUNC,
        .data = {
            .lock = 0,
            .flags = CLASS_INIT,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = data_view_classes,
        .sub_classes_count = 4,
    },
    {
        .name    = "Graphics",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = CLASSES_STRUCT_NO_FUNC,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .imgui   = CLASSES_STRUCT_NO_FUNC,
        .input   = CLASSES_STRUCT_NO_FUNC,
        .render  = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = CLASSES_STRUCT_NO_FUNC,
        .data = {
            .lock = 0,
            .flags = CLASS_INIT,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = graphics_classes,
        .sub_classes_count = 4,
    },
    {
        .name    = "Information",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = CLASSES_STRUCT_NO_FUNC,
        .draw    = CLASSES_STRUCT_NO_FUNC,
        .drop    = CLASSES_STRUCT_NO_FUNC,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .imgui   = CLASSES_STRUCT_NO_FUNC,
        .input   = CLASSES_STRUCT_NO_FUNC,
        .render  = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = CLASSES_STRUCT_NO_FUNC,
        .data = {
            .lock = 0,
            .flags = CLASS_INIT,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = information_classes,
        .sub_classes_count = 1,
    },
#if defined(CLOUD_DEV)
    {
        .name    = "Glitter Editor",
        .flags   = CLASSES_IN_CONTEXT_MENU,
        .init    = glitter_editor_init,
        .draw    = glitter_editor_draw,
        .drop    = glitter_editor_drop,
        .hide    = CLASSES_STRUCT_NO_FUNC,
        .imgui   = glitter_editor_imgui,
        .input   = glitter_editor_input,
        .render  = glitter_editor_render,
        .show    = CLASSES_STRUCT_NO_FUNC,
        .sound   = CLASSES_STRUCT_NO_FUNC,
        .dispose = glitter_editor_dispose,
        .data = {
            .lock = 0,
            .flags = CLASS_DISPOSED | CLASS_HIDDEN,
            .imgui_focus = false,
            .data = CLASS_DATA_NO_DATA,
        },
        .sub_classes = 0,
        .sub_classes_count = 0,
    },
#endif
};

const size_t classes_count = sizeof(classes) / sizeof(classes_struct);

render_state state;
lock state_lock;

lock_data glitter_data_lock;
lock_data stage_data_lock;
lock pv_lock;
