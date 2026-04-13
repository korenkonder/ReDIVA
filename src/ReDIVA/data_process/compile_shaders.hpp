/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"

extern void compile_shaders(struct farc* f, struct farc* of,
    const struct shader_table* shaders_table, const size_t size, bool debug);
extern void compile_all_shaders(bool debug);
