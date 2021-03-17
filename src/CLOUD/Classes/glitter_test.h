/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vector.h"

typedef struct glitter_test_struct {
    bool enabled;
    bool dispose;
    bool disposed;
    bool input_play;
    bool input_stop;
    bool input_auto;
    bool input_pause;
    float_t frame_counter;
    wchar_t* file;
    vector_ptr_wchar_t files;
} glitter_test_struct;

extern glitter_test_struct glitter_test;

extern void glitter_test_dispose();
extern void glitter_test_init();
extern void glitter_test_control();
extern void glitter_test_input();
extern void glitter_test_mui();
extern void glitter_test_render();
extern void glitter_test_sound();
