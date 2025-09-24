/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "config.hpp"
#include "shader.hpp"

enum shader_dev_enum {
    SHADER_DEV_FFP = 0,
    SHADER_DEV_CONVERT_YCBCR_BT709,
    SHADER_DEV_CONVERT_ALPHA,
    SHADER_DEV_CONVERT_RGB,
#if DISPLAY_IBL
    SHADER_DEV_CUBEMAP_DISPLAY,
#endif
    SHADER_DEV_END,
    SHADER_DEV_MAX,
};

extern const shader_table shader_dev_table[];
extern const size_t shader_dev_table_size;

extern shader_set_data shaders_dev;
