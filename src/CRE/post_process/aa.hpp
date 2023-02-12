/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../shared.hpp"
#include "../render_texture.hpp"

struct post_process_aa {
    int32_t width;
    int32_t height;
    render_texture mlaa_buffer;
    render_texture temp_buffer;
    GLuint mlaa_area_texture;

    post_process_aa();
    ~post_process_aa();

    void apply_mlaa(render_texture* rt,
        render_texture* buf_rt, GLuint* samplers, int32_t ss_alpha_mask);
    void init_fbo(int32_t width, int32_t height);
};
