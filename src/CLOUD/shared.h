/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "classes.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

typedef enum render_state {
    RENDER_UNINITIALIZED =  0,
    RENDER_INITIALIZING  =  1,
    RENDER_INITIALIZED   =  2,
    RENDER_DISPOSING     =  3,
    RENDER_DISPOSED      =  4,
} render_state;

extern classes_data classes[];
extern const size_t classes_count;
extern render_state state;
extern lock state_lock;
extern lock_data glitter_data_lock;
extern lock_data stage_data_lock;
extern lock pv_lock;
