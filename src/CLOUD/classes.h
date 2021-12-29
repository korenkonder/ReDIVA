/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../CRE/lock.h"
#include "../CRE/render_context.h"

#define CLASS_DATA_NO_DATA ((void*)0)
#define CLASSES_STRUCT_NO_FUNC ((void*)0)

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

typedef struct classes_struct classes_struct;

struct classes_struct {
    const char* name;
    const classes_enum flags;
    const bool(*    init)(class_data* data, render_context* rctx);
    const void(*    draw)(class_data* data);
    const void(*    drop)(class_data* data, size_t count, char** paths);
    const bool(*    hide)(class_data* data);
    const void(*   imgui)(class_data* data);
    const void(*   input)(class_data* data);
    const void(*  render)(class_data* data);
    const bool(*    show)(class_data* data);
    const void(*   sound)(class_data* data);
    const bool(* dispose)(class_data* data);
    class_data data;
    classes_struct* sub_classes;
    size_t sub_classes_count;
};

extern void classes_process_init   (classes_struct* classes, size_t classes_count, render_context* rctx);
extern void classes_process_draw   (classes_struct* classes, size_t classes_count);
extern void classes_process_drop   (classes_struct* classes, size_t classes_count, size_t count, char** paths);
extern void classes_process_imgui  (classes_struct* classes, size_t classes_count);
extern void classes_process_input  (classes_struct* classes, size_t classes_count);
extern void classes_process_render (classes_struct* classes, size_t classes_count);
extern void classes_process_sound  (classes_struct* classes, size_t classes_count);
extern void classes_process_dispose(classes_struct* classes, size_t classes_count);
