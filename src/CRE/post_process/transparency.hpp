/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../fbo.hpp"
#include "../render_texture.hpp"
#include "../shared.hpp"

struct post_process_transparency {
    fbo fbo;
    GLuint sampler;
    GLuint vao;

    post_process_transparency();
    ~post_process_transparency();

    void combine(RenderTexture* rt, RenderTexture* buf_rt, float_t alpha, void* rctx);
    void copy(RenderTexture* rt);
    void init_fbo(GLuint color_texture, GLuint depth_texture, int32_t width, int32_t height);
};
