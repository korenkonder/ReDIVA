/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "classes.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include "classes/data_player.h"
#include "classes/data_test.h"
#include "classes/glitter_editor.h"
#include "classes/graphics.h"

typedef enum render_state {
    RENDER_UNINITIALIZED =  0,
    RENDER_INITIALIZING  =  1,
    RENDER_INITIALIZED   =  2,
    RENDER_DISPOSING     =  3,
    RENDER_DISPOSED      =  4,
} render_state;

extern classes_struct classes[];
extern const size_t classes_count;
extern render_state state;
extern lock state_lock;
