/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_texture.hpp"
#include "gl_state.hpp"
#include "texture.hpp"

static shader_glsl render_texture_shader;
static GLuint render_texture_vao;
static GLuint render_texture_vbo;
static bool render_texture_data_initialized;
static uint32_t render_texture_counter;

static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level);
static int32_t render_texture_framebuffer_set_texture(render_texture* rrt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil);

render_texture::render_texture() : color_texture(),
depth_texture(), binding(), max_level(), fbos(), rbo(), field_2C() {

}

render_texture::~render_texture() {
    free();
}

int32_t render_texture::bind(int32_t index) {
    if (index < 0 || index > max_level)
        return -1;

    gl_state_bind_framebuffer(fbos[index]);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;

    glGetError();
    return 0;
}

void render_texture::draw(bool depth) {
    gl_state_active_bind_texture_2d(0, color_texture->tex);
    if (depth && depth_texture->tex)
        gl_state_active_bind_texture_2d(1, depth_texture->tex);
    gl_state_bind_vertex_array(render_texture_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    gl_state_bind_vertex_array(0);
}

void render_texture::free() {
    if (depth_texture) {
        texture_free(depth_texture);
        depth_texture = 0;
    }

    if (color_texture) {
        texture_free(color_texture);
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

    if (fbos[0]) {
        glDeleteFramebuffers(max_level + 1, fbos);
        memset(fbos, 0, sizeof(fbos));
    }
    max_level = 0;
}

int32_t render_texture::init(int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format) {
    if (max_level < 0)
        return -1;

    max_level = min_def(max_level, 15);
    if (max_level < 0)
        return 0;
    free();

    GLuint color_texture;
    if (color_format) {
        this->color_texture = texture_load_tex_2d(texture_id(0x23, render_texture_counter),
            color_format, width, height, max_level, 0, 0);
        if (!this->color_texture)
            return -1;

        render_texture_counter++;
        color_texture = this->color_texture->tex;
        gl_state_bind_texture_2d(this->color_texture->tex);
        if (color_format == GL_RGBA32F) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
        depth_texture = this->depth_texture->tex;
        stencil = depth_format == GL_DEPTH24_STENCIL8;
    }
    else {
        this->depth_texture = 0;
        depth_texture = 0;
        stencil = false;
    }
    this->max_level = max_level;

    if (render_texture_framebuffer_init(this, max_level) >= 0) {
        for (int32_t i = 0; i <= max_level; i++)
            if (render_texture_framebuffer_set_texture(this, color_texture, i, depth_texture, stencil) < 0)
                return -1;
    }
    return 0;
}

int32_t render_texture::set_color_depth_textures(GLuint color_texture,
    int32_t max_level, GLuint depth_texture, bool stencil) {
    int32_t error = 0;
    this->max_level = max_level;
    if (!fbos[0])
        error = render_texture_framebuffer_init(this, max_level);
    render_texture_framebuffer_set_texture(this, color_texture, max_level, depth_texture, stencil);
    return error;
}

void render_texture::draw_custom(shader_set_data* set) {
    gl_state_bind_vertex_array(render_texture_vao);
    set->draw_arrays(GL_TRIANGLE_STRIP, 0, 3);
    gl_state_bind_vertex_array(0);
}

void render_texture::draw_custom_glsl() {
    gl_state_bind_vertex_array(render_texture_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    gl_state_bind_vertex_array(0);
}

void render_texture::draw_params(shader_set_data* set, int32_t width, int32_t height,
    float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w) {
    float_t w = (float_t)max_def(width, 1);
    float_t h = (float_t)max_def(height, 1);
    set->local_vert_set(0, scale / w, scale / h, w, h);
    set->local_frag_set(0, scale / w, scale / h, w, h);
    set->local_vert_set(3, param_x, param_y, param_z, param_w);
    set->local_frag_set(3, param_x, param_y, param_z, param_w);

    gl_state_bind_vertex_array(render_texture_vao);
    set->draw_arrays(GL_TRIANGLE_STRIP, 0, 3);
    gl_state_bind_vertex_array(0);
}

void render_texture::shader_set(shader_glsl* shader) {
    if (shader)
        shader->use();
    else
        render_texture_shader.use();
}

void render_texture_data_init() {
    if (render_texture_data_initialized)
        return;

    static const char* render_texture_vert_shader =
        "#version 430 core\n"
        "out VertexData {\n"
        "    vec2 texcoord;\n"
        "} result;\n"
        "\n"
        "void main() {\n"
        "    gl_Position.x = -1.0 + float(gl_VertexID / 2) * 4.0;\n"
        "    gl_Position.y = 1.0 - float(gl_VertexID % 2) * 4.0;\n"
        "    gl_Position.z = 0.0;\n"
        "    gl_Position.w = 1.0;\n"
        "    result.texcoord = gl_Position.xy * 0.5 + 0.5;\n"
        "}\n";

    static const char* render_texture_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "in VertexData {\n"
        "    vec2 texcoord;\n"
        "} frg;\n"
        "\n"
        "layout(binding = 0) uniform sampler2D g_color;\n"
        "\n"
        "void main() {\n"
        "    result = texture(g_color, frg.texcoord);\n"
        "}\n";

    shader_glsl_param param = {};
    param.name = "Render Texture";
    render_texture_shader.load(render_texture_vert_shader,
        render_texture_frag_shader, 0, &param);

    const float_t verts_quad[] = {
        -1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -3.0f,  0.0f, -1.0f,
         3.0f,  1.0f,  2.0f,  1.0f,
    };

    glGenVertexArrays(1, &render_texture_vao);
    gl_state_bind_vertex_array(render_texture_vao, true);

    glGenBuffers(1, &render_texture_vbo);
    gl_state_bind_array_buffer(render_texture_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, 0);
    else
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);  // Pos
    glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color0
    glVertexAttrib4f(4, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord0
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord1
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord2
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord3
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord4
    glEnableVertexAttribArray(13);
    glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord5
    glEnableVertexAttribArray(14);
    glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord6
    glEnableVertexAttribArray(15);
    glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord7
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    render_texture_data_initialized = true;
}

void render_texture_data_free() {
    if (!render_texture_data_initialized)
        return;

    render_texture_shader.unload();

    glDeleteBuffers(1, &render_texture_vbo);
    glDeleteVertexArrays(1, &render_texture_vao);

    render_texture_data_initialized = false;
}

static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level) {
    glGenFramebuffers(max_level + 1, rt->fbos);
    return -(glGetError() != GL_ZERO);
}

static int32_t render_texture_framebuffer_set_texture(render_texture* rt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil) {
    if (level < 0 || level > rt->max_level)
        return -1;

    gl_state_bind_framebuffer(rt->fbos[level]);
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
    gl_state_bind_framebuffer(0);
    glGetError();
    return ret;
}
