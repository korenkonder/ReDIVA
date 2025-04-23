/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../gl_rend_state.hpp"
#include "../shared.hpp"

namespace renderer {
    struct FBO {
        enum Flags {
            DEPTH_ATTACHMENT = 0x01,
        };

        Flags flags;
        int32_t width;
        int32_t height;
        GLuint buffer;
        int32_t count;
        GLuint* textures;

        FBO();
        ~FBO();

        void init(int32_t width, int32_t height,
            GLuint* color_textures, int32_t count, GLuint depth_texture);
        void free();

        inline void bind_buffer() {
            gl_rend_state.bind_framebuffer(buffer);
        }
    };
}
