/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "config.hpp"
#include "../KKdLib/default.hpp"

// Added
extern bool init_data_edit();
extern bool ctrl_data_edit();
extern bool dest_data_edit();

// Added
extern bool init_data_edit_main_md();
extern bool ctrl_data_edit_main_md();
extern bool dest_data_edit_main_md();

// Added
extern bool init_data_edit_glitter_editor_md();
extern bool ctrl_data_edit_glitter_editor_md();
extern bool dest_data_edit_glitter_editor_md();

#if FACE_ANIM
// Added
extern bool init_data_edit_face_anim_md();
extern bool ctrl_data_edit_face_anim_md();
extern bool dest_data_edit_face_anim_md();
#endif
