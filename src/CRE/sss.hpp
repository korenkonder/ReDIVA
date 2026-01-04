/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "render_texture.hpp"

struct sss_data {
    bool init_data;
    bool enable;
    bool downsample;
    RenderTexture textures[4];
    vec4 param;

    sss_data();
    ~sss_data();

    void apply_filter(struct render_data_context& rend_data_ctx);
    void free();
    void init();
    void pre_proc(const struct cam_data& cam); // Added
    void reset(struct render_data_context& rend_data_ctx);
    bool set(struct render_data_context& rend_data_ctx);
    bool set(struct render_data_context& rend_data_ctx, RenderTexture& rt); // Added
    void set_texture(struct p_gl_rend_state& p_gl_rend_st, int32_t texture_index);
};

extern vec4 sss_param;
extern vec4 sss_param_reflect;
