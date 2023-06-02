/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/vec.hpp"

extern vec2 target_pos_scale;
extern vec2 target_pos_offset;

extern void target_pos_scale_offset_apply(const vec2* src, vec2* dst);
extern void target_pos_scale_offset_get();
