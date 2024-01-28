/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/lock.hpp"
#include "../CRE/render_context.hpp"

#define CLASS_DATA_NO_DATA (0)
#define CLASSES_DATA_NO_FUNC (0)

enum class_flags {
    CLASS_INIT        = 0x01,
    CLASS_DISPOSE     = 0x02,
    CLASS_DISPOSED    = 0x04,
    CLASS_HIDE        = 0x08,
    CLASS_HIDDEN      = 0x10,
    CLASS_HIDE_WINDOW = 0x20,
    CLASS_HAS_PARENT  = 0x40,
    CLASS_DW          = 0x80,
};

enum classes_enum {
    CLASSES_INIT_AT_STARTUP = 0x01,
    CLASSES_SHOW_AT_STARTUP = 0x02,
    CLASSES_DISPOSE_AT_HIDE = 0x04,
    CLASSES_IN_CONTEXT_MENU = 0x08,
};

struct class_data {
    class_flags flags;
    bool imgui_focus;
    void* data;
};

typedef bool(*classes_init_func   )(class_data* data, render_context* rctx);
typedef void(*classes_imgui_func  )(class_data* data);
typedef bool(*classes_dispose_func)(class_data* data);

struct classes_data {
    const char* name;
    const classes_enum flags;
    classes_init_func       init;
    classes_imgui_func     imgui;
    classes_dispose_func dispose;
    class_data data;
    classes_data* sub_classes;
    size_t sub_classes_count;
};

extern void classes_process_init   (classes_data* classes, size_t classes_count, render_context* rctx);
extern void classes_process_ctrl   (classes_data* classes, size_t classes_count);
extern void classes_process_imgui  (classes_data* classes, size_t classes_count);
extern void classes_process_dispose(classes_data* classes, size_t classes_count);
