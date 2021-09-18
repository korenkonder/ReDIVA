/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(VIDEO)
#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"

extern bool data_player_enabled;

extern void data_player_dispose();
extern void data_player_draw();
extern void data_player_init();
extern void data_player_imgui();
extern void data_player_input();
extern void data_player_render();
extern void data_player_video();
#endif
