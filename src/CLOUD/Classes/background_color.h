/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"

typedef struct background_color_struct {
    bool enabled;
    bool dispose;
    bool disposed;
    vec3 color;
} background_color_struct;

extern background_color_struct background_color;

extern void background_color_dispose();
extern void background_color_init();
extern void background_color_control();
extern void background_color_input();
extern void background_color_mui();
extern void background_color_render();
extern void background_color_sound();
