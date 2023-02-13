/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#if defined(CRE_DEV)
#include "../KKdLib/default.hpp"
#include "shader.hpp"

enum shader_dev_enum {
    SHADER_DEV_FFP = 0,
    SHADER_DEV_GRID,      // Added
    SHADER_DEV_END,
};

extern const shader_table shader_dev_table[];
extern const size_t shader_dev_table_size;

extern shader_set_data shaders_dev;
#endif
