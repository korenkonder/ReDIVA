/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_dev.hpp"
#include "../KKdLib/str_utils.hpp"
#include "gl_state.hpp"
#include "random.hpp"
#include "render_context.hpp"

enum shader_dev_sub_enum {
    SHADER_DEV_SUB_SHADER_FFP = 0,
#if DISPLAY_IBL
    SHADER_DEV_SUB_CUBEMAP_DISPLAY,
#endif
    SHADER_DEV_SUB_SHADER_END,
};

#if DISPLAY_IBL
static const int32_t cubemap_display_vpt_unival_max[] = {
    -1,
};

static const int32_t cubemap_display_fpt_unival_max[] = {
    -1,
};
#endif

#pragma region Shader Description
#define SHADER_DESCRIPTION_EFFECT_TEXTURE_SAMPLER \
    { SHADER_DESCRIPTION_SAMPLER, 14, 0, U_INVALID, }

#define SHADER_DESCRIPTION_COMMON_SCENE \
    { SHADER_DESCRIPTION_UNIFORM, 0, sizeof(obj_shader_shader_data), U_INVALID, }, \
    { SHADER_DESCRIPTION_UNIFORM, 1, sizeof(obj_scene_shader_data), U_INVALID, }, \
    { SHADER_DESCRIPTION_UNIFORM, 2, sizeof(obj_batch_shader_data), U_INVALID, }

#define SHADER_DESCRIPTION_COMMON_SKINNING \
    { SHADER_DESCRIPTION_VERTEX_INPUT,  1, 4, U_SKINNING, }, \
    { SHADER_DESCRIPTION_VERTEX_INPUT, 15, 4, U_SKINNING, }, \
    { SHADER_DESCRIPTION_STORAGE, 0, sizeof(obj_skinning_shader_data), U_SKINNING, }

#define SHADER_DESCRIPTION_COMMON_QUAD \
    { SHADER_DESCRIPTION_UNIFORM, 0, sizeof(quad_shader_data), U_INVALID, }

#if DISPLAY_IBL
static const shader_description cubemap_display_vpt_desc[] = {
    { SHADER_DESCRIPTION_VERTEX_INPUT, 0, 3, U_INVALID, },
    { SHADER_DESCRIPTION_UNIFORM, 3, sizeof(vec4) * 5, U_INVALID, },
    { SHADER_DESCRIPTION_END, -1, -1, U_INVALID, },
};

static const shader_description cubemap_display_fpt_desc[] = {
    { SHADER_DESCRIPTION_SAMPLER, 0, 0, U_INVALID, },
    { SHADER_DESCRIPTION_UNIFORM, 3, sizeof(vec4) * 5, U_INVALID, },
    { SHADER_DESCRIPTION_FRAGMENT_OUTPUT, 0, 4, U_INVALID, },
    { SHADER_DESCRIPTION_END, -1, -1, U_INVALID, },
};
#endif

#undef SHADER_DESCRIPTION_COMMON_QUAD
#undef SHADER_DESCRIPTION_COMMON_SKINNING
#undef SHADER_DESCRIPTION_COMMON_SCENE
#undef SHADER_DESCRIPTION_EFFECT_TEXTURE_SAMPLER
#pragma endregion

#if DISPLAY_IBL
static const uniform_name CUBEMAP_DISPLAY_uniform[] = {
    U_INVALID,
};
#endif

#define shader_sub_table_struct(sub_index, vp, fp) \
{ \
    SHADER_DEV_SUB_##sub_index, \
    vp##_vpt_unival_max, \
    fp##_fpt_unival_max, \
    #vp, \
    #fp, \
    vp##_vpt_desc, \
    fp##_fpt_desc, \
}

#if DISPLAY_IBL
static const shader_sub_table CUBEMAP_DISPLAY_table[] = {
    shader_sub_table_struct(CUBEMAP_DISPLAY,
        cubemap_display, cubemap_display),
};
#endif

#undef shader_sub_table_struct

#define shader_table_struct(n) \
{ \
    #n, \
    SHADER_DEV_##n, \
    sizeof(n##_table) / sizeof(shader_sub_table), \
    n##_table, \
    n##_uniform[0] > 0 ? sizeof(n##_uniform) / sizeof(uniform_name) : 0, \
    n##_uniform, \
}

const shader_table shader_dev_table[] = {
    {
        "SHADER_FFP",
        SHADER_DEV_FFP,
        0,
        0,
        0,
        0,
    },
#if DISPLAY_IBL
    shader_table_struct(CUBEMAP_DISPLAY),
#endif
};

#undef shader_table_struct

const size_t shader_dev_table_size =
    sizeof(shader_dev_table) / sizeof(shader_table);

shader_set_data shaders_dev;
