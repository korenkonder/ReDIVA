/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "texture.hpp"

struct RenderTexture {
    texture* color_texture;
    texture* depth_texture;
    int32_t binding;
    int32_t max_level;
    GLuint* fbos;
    GLuint depth_rbo;
    GLuint stencil_rbo;

    RenderTexture();
    virtual ~RenderTexture();

    int32_t Bind(struct gl_state_struct& gl_st, int32_t index = 0);
    int32_t Bind(struct p_gl_rend_state& p_gl_rend_st, int32_t index = 0);
    void Free();
    int32_t Init(int32_t width, int32_t height,
        int32_t max_level, GLenum color_format, GLenum depth_format);
    /*int32_t InitDepthRenderbuffer(gl_state_struct& gl_st, GLenum internal_format, int32_t width, int32_t height);
    int32_t InitStencilRenderbuffer(gl_state_struct& gl_st, GLenum internal_format, int32_t width, int32_t height);*/
    int32_t SetColorDepthTextures(GLuint color_texture,
        int32_t max_level = 0, GLuint depth_texture = 0, bool stencil = false);
    void SetViewport(struct p_gl_rend_state& p_gl_rend_st);

    inline GLuint GetColorTex() {
        return color_texture->glid;
    };

    inline GLuint GetDepthTex() {
        return depth_texture->glid;
    };

    inline int32_t GetHeight() {
        return color_texture->height;
    };

    inline int32_t GetWidth() {
        return color_texture->width;
    };
};

extern void render_texture_counter_reset();
