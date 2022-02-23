/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../CRE/lock.h"
#include "../CRE/render_context.h"

#define CLASS_DATA_NO_DATA (0)
#define CLASSES_DATA_NO_FUNC (0)

typedef enum class_flags {
    CLASS_INIT     = 0x01,
    CLASS_DISPOSE  = 0x02,
    CLASS_DISPOSED = 0x04,
    CLASS_HIDE     = 0x08,
    CLASS_HIDDEN   = 0x10,
} class_flags;

typedef enum classes_enum {
    CLASSES_INIT_AT_STARTUP = 0x01,
    CLASSES_SHOW_AT_STARTUP = 0x02,
    CLASSES_DISPOSE_AT_HIDE = 0x04,
    CLASSES_IN_CONTEXT_MENU = 0x08,
} classes_enum;

typedef struct class_data {
    lock lock;
    class_flags flags;
    bool imgui_focus;
    void* data;
} class_data;

typedef struct classes_data classes_data;

struct classes_data {
    const char* name;
    const classes_enum flags;
    bool(*    init)(class_data* data, render_context* rctx);
    void(*    ctrl)(class_data* data);
    void(*    disp)(class_data* data);
    void(*    drop)(class_data* data, size_t count, char** paths);
    bool(*    hide)(class_data* data);
    void(*   imgui)(class_data* data);
    void(*   input)(class_data* data);
    bool(*    show)(class_data* data);
    void(*   sound)(class_data* data);
    bool(* dispose)(class_data* data);
    class_data data;
    classes_data* sub_classes;
    size_t sub_classes_count;
};

extern void classes_process_init   (classes_data* classes, size_t classes_count, render_context* rctx);
extern void classes_process_draw   (classes_data* classes, size_t classes_count);
extern void classes_process_drop   (classes_data* classes, size_t classes_count, size_t count, char** paths);
extern void classes_process_imgui  (classes_data* classes, size_t classes_count);
extern void classes_process_input  (classes_data* classes, size_t classes_count);
extern void classes_process_ctrl   (classes_data* classes, size_t classes_count);
extern void classes_process_sound  (classes_data* classes, size_t classes_count);
extern void classes_process_dispose(classes_data* classes, size_t classes_count);
