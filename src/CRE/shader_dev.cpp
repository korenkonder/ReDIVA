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
    SHADER_DEV_SUB_GLITTER_PT_WIREFRAME,
    SHADER_DEV_SUB_GRID,
    SHADER_DEV_SUB_SHADER_END,
};

static const int32_t glitter_particle_wireframe_vpt_unival_max[] = {
    -1,
};

static const int32_t glitter_particle_wireframe_fpt_unival_max[] = {
    -1,
};

static const int32_t grid_vpt_unival_max[] = {
    -1,
};

static const int32_t grid_fpt_unival_max[] = {
    -1,
};

static const uniform_name GLITTER_PT_WIREFRAME_uniform[] = {
    U_INVALID,
};

static const uniform_name GRID_uniform[] = {
    U_INVALID,
};

static const bool GLITTER_PT_WIREFRAME_permut[] = {
    false,
};

static const bool GRID_permut[] = {
    false,
};

static const shader_sub_table GLITTER_PT_WIREFRAME_table[] = {
    {
        SHADER_DEV_SUB_GLITTER_PT_WIREFRAME,
        glitter_particle_wireframe_vpt_unival_max,
        glitter_particle_wireframe_fpt_unival_max,
        "glitter_particle_wireframe",
        "glitter_particle_wireframe",
    },
};

static const shader_sub_table GRID_table[] = {
    {
        SHADER_DEV_SUB_GRID,
        grid_vpt_unival_max,
        grid_fpt_unival_max,
        "grid",
        "grid",
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
    shader_table_struct(GRID),
    shader_table_struct(GLITTER_PT_WIREFRAME),
};

#undef shader_table_struct

const size_t shader_dev_table_size =
    sizeof(shader_dev_table) / sizeof(shader_table);

shader_set_data shaders_dev;
#endif
