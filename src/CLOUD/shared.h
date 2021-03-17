/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../CRE/microui.h"
#include "Classes/background_color.h"
#include "Classes/glitter_test.h"
#include "Classes/post_process.h"

typedef enum classes_enum {
    CLASSES_INIT_AT_STARTUP = 0x01,
    CLASSES_IN_CONTEXT_MENU = 0x02,
} classes_enum;

typedef struct classes_struct {
    char* name;
    classes_enum flags;
    bool* enabled;
    void(FASTCALL* dispose)();
    void(FASTCALL* init)();
    void(FASTCALL* control)();
    void(FASTCALL* input)();
    void(FASTCALL* mui)();
    void(FASTCALL* render)();
    void(FASTCALL* sound)();
} classes_struct;

extern mu_Context* muctx;

extern const classes_struct classes[];
extern const size_t classes_count;
