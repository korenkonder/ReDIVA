/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.hpp"
#include "classes/data_view.hpp"
#include "classes/graphics.hpp"
#include "classes/information.hpp"

classes_data classes[] = {
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
        5,
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
        7,
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
        3,
    }
};

const size_t classes_count = sizeof(classes) / sizeof(classes_data);
