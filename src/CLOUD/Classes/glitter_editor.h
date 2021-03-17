/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../../CRE/Glitter/glitter.h"

typedef struct glitter_editor_struct {
    bool enabled;
    bool dispose;
    bool disposed;
    bool load;
    bool save;
    bool save_big_endian;
    bool input_play;
    bool input_stop;
    bool input_pause;
    bool input_reload;
    float_t frame_counter;
    float_t old_frame_counter;
    float_t start_frame;
    float_t end_frame;
    int32_t random;
    int32_t counter;
    glitter_effect_group* effect_group;
    glitter_scene* scene;
    wchar_t file[MAX_PATH];
} glitter_editor_struct;

extern glitter_editor_struct glitter_editor;

extern void glitter_editor_dispose();
extern void glitter_editor_init();
extern void glitter_editor_control();
extern void glitter_editor_input();
extern void glitter_editor_mui();
extern void glitter_editor_render();
extern void glitter_editor_sound();
