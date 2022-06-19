/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "classes.hpp"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

enum render_state {
    RENDER_UNINITIALIZED =  0,
    RENDER_INITIALIZING  =  1,
    RENDER_INITIALIZED   =  2,
    RENDER_DISPOSING     =  3,
    RENDER_DISPOSED      =  4,
};

enum thread_flag {
    THREAD_RENDER = 0x01,
    THREAD_SOUND  = 0x02,
    THREAD_INPUT  = 0x04,
};

extern classes_data classes[];
extern const size_t classes_count;
extern render_state state;
extern thread_flag thread_flags;
extern lock state_lock;
