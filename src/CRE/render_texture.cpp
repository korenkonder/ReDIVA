/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_texture.hpp"
#include "gl_state.hpp"
#include "render_context.hpp"
#include "texture.hpp"

extern render_context* rctx_ptr;

static uint32_t render_texture_counter;

static int32_t render_texture_init_framebuffer(RenderTexture* rt, int32_t max_level);
static int32_t render_texture_set_framebuffer_texture(RenderTexture* rrt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil);

RenderTexture::RenderTexture() : color_texture(),
depth_texture(), binding(), max_level(), fbos(), rbo(), field_2C() {

}

RenderTexture::~RenderTexture() {
    Free();
}

int32_t RenderTexture::Bind(int32_t index) {
    if (index < 0 || index > max_level)
        return -1;

    gl_state_bind_framebuffer(fbos[index]);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;

    gl_state_get_error();
    return 0;
}

void RenderTexture::Free() {
    if (depth_texture) {
        texture_release(depth_texture);
        depth_texture = 0;
    }

    if (color_texture) {
        texture_release(color_texture);
        color_texture = 0;
    }

    if (rbo) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    if (field_2C) {
        glDeleteRenderbuffers(1, &field_2C);
        field_2C = 0;
    }

    if (fbos) {
        glDeleteFramebuffers(max_level + 1, fbos);
        free(fbos);
        fbos = 0;
    }
    max_level = 0;
}

int32_t RenderTexture::Init(int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format) {
    if (max_level < 0)
        return -1;

    max_level = min_def(max_level, 15);
    if (max_level < 0)
        return 0;
    Free();

    GLuint color_texture;
    if (color_format) {
        this->color_texture = texture_load_tex_2d(texture_id(0x23, render_texture_counter),
            color_format, width, height, max_level, 0, 0);
        if (!this->color_texture)
            return -1;

        render_texture_counter++;
        color_texture = GetColorTex();
        gl_state_bind_texture_2d(GetColorTex());
        if (color_format == GL_RGBA32F) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        gl_state_bind_texture_2d(0);
    }
    else {
        this->color_texture = 0;
        color_texture = 0;
    }

    GLuint depth_texture;
    bool stencil;
    if (depth_format) {
        this->depth_texture = texture_load_tex_2d(texture_id(0x23, render_texture_counter),
            depth_format, width, height, 0, 0, 0);
        if (!this->depth_texture)
            return -1;

        render_texture_counter++;
        depth_texture = this->GetDepthTex();
        stencil = depth_format == GL_DEPTH24_STENCIL8;
    }
    else {
        this->depth_texture = 0;
        depth_texture = 0;
        stencil = false;
    }
    this->max_level = max_level;

    if (render_texture_init_framebuffer(this, max_level) >= 0)
        for (int32_t i = 0; i <= max_level; i++)
            if (render_texture_set_framebuffer_texture(this, color_texture, i, depth_texture, stencil) < 0)
                return -1;
    return 0;
}

int32_t RenderTexture::SetColorDepthTextures(GLuint color_texture,
    int32_t max_level, GLuint depth_texture, bool stencil) {
    int32_t error = 0;
    this->max_level = max_level;
    if (!fbos)
        error = render_texture_init_framebuffer(this, max_level);
    render_texture_set_framebuffer_texture(this, color_texture, max_level, depth_texture, stencil);
    return error;
}

void render_texture_counter_reset() {
    render_texture_counter = 0;
}

static int32_t render_texture_init_framebuffer(RenderTexture* rt, int32_t max_level) {
    rt->fbos = force_malloc<GLuint>(max_level + 1LL);
    glGenFramebuffers(max_level + 1, rt->fbos);
    return -(gl_state_get_error() != GL_ZERO);
}

static int32_t render_texture_set_framebuffer_texture(RenderTexture* rt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil) {
    if (level < 0 || level > rt->max_level)
        return -1;

    gl_state_bind_framebuffer(rt->fbos[level]);
    gl_state_get_error();

    if (color_texture) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, level);
        gl_state_get_error();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }
    else {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        gl_state_get_error();
        glDrawBuffer(GL_ZERO);
        glReadBuffer(GL_ZERO);
    }
    gl_state_get_error();

    if (!level) {
        if (stencil) {
            if (depth_texture)
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, depth_texture, 0);
            else if (rt->rbo) {
                glBindRenderbuffer(GL_RENDERBUFFER, rt->rbo);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt->rbo);
            }
            else
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
        }
        else {
            if (depth_texture)
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, depth_texture, 0);
            else if (rt->rbo) {
                glBindRenderbuffer(GL_RENDERBUFFER, rt->rbo);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt->rbo);
            }
            else
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, 0, 0);
        }
        gl_state_get_error();
    }

    int32_t ret = 0;
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ret = -1;
    gl_state_bind_framebuffer(0);
    gl_state_get_error();
    return ret;
}
