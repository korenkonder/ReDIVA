/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../render_texture.hpp"
#include "fbo.hpp"

struct render_data_context;

namespace renderer {
    class Transparency {
    private:
        FBO m_fbo;
        GLuint m_sampler;
        GLuint m_vao;

    public:
        Transparency(GLuint color_tex, GLuint depth_tex, int32_t width, int32_t height);
        ~Transparency();

        void begin_render(render_data_context& rend_data_ctx, GLuint texture);
        void end_render(render_data_context& rend_data_ctx, RenderTexture* rt, float_t alpha);

        void resize(GLuint color_tex, GLuint depth_tex, int32_t width, int32_t height); // Added
    };
};
