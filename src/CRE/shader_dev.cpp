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
    SHADER_DEV_SUB_GLITTER_PT_WIREFRAME,
    SHADER_DEV_SUB_SHADER_END,
};

static const int32_t glitter_particle_wireframe_vpt_unival_max[] = {
    -1,
};

static const int32_t glitter_particle_wireframe_fpt_unival_max[] = {
    -1,
};

static const uniform_name GLITTER_PT_WIREFRAME_uniform[] = {
    U_INVALID,
};

#define shader_sub_table_struct(sub_index, vp, fp) \
{ \
    SHADER_DEV_SUB_##sub_index, \
    vp##_vpt_unival_max, \
    fp##_fpt_unival_max, \
    #vp, \
    #fp, \
}

static const shader_sub_table GLITTER_PT_WIREFRAME_table[] = {
    shader_sub_table_struct(GLITTER_PT_WIREFRAME,
        glitter_particle_wireframe, glitter_particle_wireframe),
};

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
    shader_table_struct(GLITTER_PT_WIREFRAME),
};

#undef shader_table_struct

const size_t shader_dev_table_size =
    sizeof(shader_dev_table) / sizeof(shader_table);

shader_set_data shaders_dev;
