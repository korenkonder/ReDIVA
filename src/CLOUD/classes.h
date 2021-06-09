/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../CRE/lock.h"

typedef enum classes_enum {
    CLASSES_INIT_AT_STARTUP = 0x01,
    CLASSES_IN_CONTEXT_MENU = 0x02,
} classes_enum;

typedef struct classes_struct classes_struct;

struct classes_struct {
    const char* name;
    const classes_enum flags;
    bool* enabled;
    const void(FASTCALL* dispose)();
    const void(FASTCALL* init)();
    const void(FASTCALL* control)();
    const void(FASTCALL* draw)();
    const void(FASTCALL* drop)(size_t count, wchar_t** paths);
    const void(FASTCALL* imgui)();
    const void(FASTCALL* input)();
    const void(FASTCALL* render)();
    const void(FASTCALL* sound)();
    lock_val(lock);
    classes_struct* sub_classes;
    size_t sub_classes_count;
};

extern void classes_process_dispose(classes_struct* classes, size_t classes_count);
extern void classes_process_init   (classes_struct* classes, size_t classes_count);
extern void classes_process_control(classes_struct* classes, size_t classes_count);
extern void classes_process_draw   (classes_struct* classes, size_t classes_count);
extern void classes_process_drop   (classes_struct* classes, size_t classes_count, size_t count, wchar_t** paths);
extern void classes_process_imgui  (classes_struct* classes, size_t classes_count);
extern void classes_process_input  (classes_struct* classes, size_t classes_count);
extern void classes_process_render (classes_struct* classes, size_t classes_count);
extern void classes_process_sound  (classes_struct* classes, size_t classes_count);
