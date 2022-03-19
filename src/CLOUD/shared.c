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

classes_data classes[] = {
    {
        "Data Test",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        {
            {},
            (class_flags)CLASS_INIT,
            false,
            CLASS_DATA_NO_DATA,
        },
        data_test_classes,
        3,
    },
    {
        "Data View",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        {
            {},
            (class_flags)CLASS_INIT,
            false,
            CLASS_DATA_NO_DATA,
        },
        data_view_classes,
        4,
    },
    {
        "Graphics",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        {
            {},
            (class_flags)CLASS_INIT,
            false,
            CLASS_DATA_NO_DATA,
        },
        graphics_classes,
        4,
    },
    {
        "Information",
        CLASSES_IN_CONTEXT_MENU,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        {
            {},
            (class_flags)CLASS_INIT,
            false,
            CLASS_DATA_NO_DATA,
        },
        information_classes,
        2,
    },
#if defined(CLOUD_DEV)
    {
        "Glitter Editor",
        CLASSES_IN_CONTEXT_MENU,
        glitter_editor_init,
        glitter_editor_ctrl,
        glitter_editor_draw,
        glitter_editor_drop,
        CLASSES_DATA_NO_FUNC,
        glitter_editor_imgui,
        glitter_editor_input,
        CLASSES_DATA_NO_FUNC,
        CLASSES_DATA_NO_FUNC,
        glitter_editor_dispose,
        {
            {},
            (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN),
            false,
            CLASS_DATA_NO_DATA,
        },
        0,
        0,
    },
#endif
};

const size_t classes_count = sizeof(classes) / sizeof(classes_data);

render_state state;
lock state_lock;

bool stage_control;
lock stage_control_lock;
