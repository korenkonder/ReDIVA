/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"

typedef struct post_process_struct {
    bool enabled;
    bool dispose;
    bool disposed;
} post_process_struct;

extern post_process_struct post_process;

extern void post_process_dispose();
extern void post_process_init();
extern void post_process_control();
extern void post_process_input();
extern void post_process_mui();
extern void post_process_render();
extern void post_process_sound();
