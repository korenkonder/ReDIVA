/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_texture.h"
#include "gl_state.h"
#include "texture.h"

static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level);
static int32_t render_texture_framebuffer_set_texture(render_texture* rrt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil);

int32_t render_texture_init(render_texture* rt, int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format) {
    if (max_level < 0)
        return -1;

    if (rt->color_texture && rt->depth_texture
        && rt->color_texture->width == width && rt->color_texture->height == height
        && rt->color_texture->max_mipmap_level == max_level
        && rt->color_texture->internal_format == color_format
        && rt->depth_texture->internal_format == depth_format)
        return 0;
    render_texture_free(rt);

    GLuint color_texture;
    if (color_format) {
        texture* color_tex = texture_load_tex_2d(color_format, width, height, max_level, 0, 0);
        rt->color_texture = color_tex;
        if (!color_tex)
            return -1;

        color_texture = color_tex->texture;
        if (color_format == GL_RGBA32F) {
            gl_state_bind_texture_2d(rt->color_texture->texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl_state_bind_texture_2d(0);
        }
    }
    else {
        rt->color_texture = 0;
        color_texture = 0;
    }

    GLuint depth_texture;
    bool stencil;
    if (depth_format) {
        texture* depth_tex = texture_load_tex_2d(depth_format, width, height, 0, 0, 0);
        rt->depth_texture = depth_tex;
        if (!depth_tex)
            return -1;

        depth_texture = depth_tex->texture;
        stencil = depth_format == GL_DEPTH24_STENCIL8;
    }
    else {
        rt->depth_texture = 0;
        depth_texture = 0;
        stencil = false;
    }
    rt->max_level = max_level;

    if (render_texture_framebuffer_init(rt, max_level) >= 0) {
        for (int32_t i = 0; i <= max_level; i++)
            if (render_texture_framebuffer_set_texture(rt, color_texture, i, depth_texture, stencil) < 0)
                return -1;
    }
    return 0;
}

int32_t render_texture_bind(render_texture* rt, int32_t index) {
    if (index < 0 || index > rt->max_level)
        return -1;

    gl_state_bind_framebuffer(rt->fbos[index]);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;
    glGetError();
    return 0;
}

void render_texture_free(render_texture* rt) {
    if (rt->depth_texture) {
        texture_dispose(rt->depth_texture);
        rt->depth_texture = 0;
    }

    if (rt->color_texture) {
        texture_dispose(rt->color_texture);
        rt->color_texture = 0;
    }

    if (rt->rbo) {
        glDeleteRenderbuffers(1, &rt->rbo);
        rt->rbo = 0;
    }

    if (rt->field_2C) {
        glDeleteRenderbuffers(1, &rt->field_2C);
        rt->field_2C = 0;
    }

    if (rt->fbos) {
        glDeleteFramebuffers(rt->max_level + 1, rt->fbos);
        free(rt->fbos);
    }
}
static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level) {
    rt->fbos = force_malloc_s(GLuint, max_level + 1ULL);
    if (!rt->fbos)
        return -1;
    glGenFramebuffers(max_level + 1, rt->fbos);
    return -(glGetError() != GL_ZERO);
}

static int32_t render_texture_framebuffer_set_texture(render_texture* rt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil) {
    if (level < 0 || level > rt->max_level)
        return -1;

    glBindFramebuffer(GL_FRAMEBUFFER, rt->fbos[level]);
    glGetError();

    if (color_texture) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, level);
        glGetError();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }
    else {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        glGetError();
        glDrawBuffer(GL_ZERO);
        glReadBuffer(GL_ZERO);
    }
    glGetError();

    if (level == 0) {
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
        glGetError();
    }

    int32_t ret = 0;
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ret = -1;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGetError();
    return ret;
}