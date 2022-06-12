/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../shared.h"
#include "../render_texture.h"

struct post_process_aa {
    int32_t width;
    int32_t height;
    render_texture mlaa_buffer[2];
    GLuint mlaa_area_texture;

    post_process_aa();
    virtual ~post_process_aa();

    void apply_mlaa(render_texture* rt,
        render_texture* buf_rt, GLuint* samplers, int32_t a4);
    void init_fbo(int32_t width, int32_t height);
};
