/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(CRE_DEV)
#include "shader_dev.hpp"
#include "../KKdLib/str_utils.hpp"
#include "gl_state.hpp"
#include "random.hpp"
#include "render_context.hpp"

enum shader_dev_sub_enum {
    SHADER_DEV_SUB_SHADER_FFP = 0,
    SHADER_DEV_SUB_CLAMP_COLORS,
    SHADER_DEV_SUB_SHADER_END,
};

static const int32_t clamp_colors_vpt_unival_max[] = {
    -1,
};

static const int32_t clamp_colors_fpt_unival_max[] = {
    -1,
};

static const uniform_name CLAMP_COLORS_uniform[] = {
    U_INVALID,
};

static const bool CLAMP_COLORS_permut[] = {
    false,
};

static const shader_sub_table CLAMP_COLORS_table[] = {
    {
        SHADER_DEV_SUB_CLAMP_COLORS,
        clamp_colors_vpt_unival_max,
        clamp_colors_fpt_unival_max,
        "clamp_colors",
        "clamp_colors",
    },
};


#define shader_table_struct(n) \
{ \
    #n, \
    SHADER_DEV_##n, \
    sizeof(n##_table) / sizeof(shader_sub_table), \
    n##_table, \
    sizeof(n##_uniform) / sizeof(uniform_name), \
    n##_uniform, \
    n##_permut, \
}

const shader_table shader_dev_table[] = {
    {
        "SHADER_FFP",
        SHADER_DEV_FFP,
        0,
        0,
        0,
        0,
        0,
    },
    shader_table_struct(CLAMP_COLORS),
};

#undef shader_table_struct

const size_t shader_dev_table_size =
    sizeof(shader_dev_table) / sizeof(shader_table);

shader_set_data shaders_dev;
#endif
