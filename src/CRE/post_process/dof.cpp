/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dof.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../rob/rob.hpp"
#include "../gl_state.hpp"
#include "../shader_ft.hpp"

struct dof_common_shader_data {
    vec4 g_depth_params; //x=(n-f)/(nf), y=1/n, z=coc_from_z_scale, w=coc_from_z_offset
    vec4 g_spread_scale; //x=scale_from_meter_to_pixel, y=scale_from_meter_to_sample,
        //  z=scale_from_pixel_to_sample, w=scale_from_sample_to_pixel
    vec4 g_depth_params2; //x=distance_to_focus_m, y=focus_range,
        // z=k/(fuzzing_range*fuzzing_range), w=max_coc_radius_in_pixel   //yzw=for_f2
};

static const dof_debug dof_debug_default = {
    (dof_debug_flags)0,
    10.0f,
    0.04f,
    1.4f,
    {
        10.0f,
        1.0f,
        0.5f,
        1.0f,
    },
};

static const dof_pv dof_pv_default = {
    false,
    {
        10.0f,
        1.0f,
        0.5f,
        1.0f,
    },
};

namespace renderer {
    struct DOF3 {
        static void apply_f2(post_process_dof* dof, RenderTexture* rt,
            RenderTexture* buf, GLuint* samplers, GLuint color_texture,
            GLuint depth_texture, float_t min_distance, float_t max_distance, float_t fov,
            float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio);
        static void apply_physical(post_process_dof* dof, RenderTexture* rt,
            RenderTexture* buf, GLuint* samplers, GLuint color_texture,
            GLuint depth_texture, float_t min_distance, float_t max_distance,
            float_t focus, float_t focal_length, float_t fov, float_t f_number);

        static void render_tiles(post_process_dof* dof,
            GLuint* samplers, GLuint depth_texture, bool f2);
        static void downsample(post_process_dof* dof,
            GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2);
        static void main_filter(post_process_dof* dof, GLuint* samplers, bool f2);
        static void upsample(post_process_dof* dof, RenderTexture* rt, RenderTexture* buf,
            GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2);

        static void update_data(post_process_dof* dof, float_t min_dist,
            float_t max_dist, float_t fov, float_t dist, float_t focal_length,
            float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio);
    };
}

static void post_process_dof_calculate_texcoords(vec2* data, float_t size);
static void post_process_dof_free_fbo(post_process_dof* dof);

post_process_dof::post_process_dof() : data(), width(),
height(), textures(), fbo(), vao(), texcoords_ubo() {

}

post_process_dof::~post_process_dof() {
    if (!this)
        return;

    post_process_dof_free_fbo(this);
}

void post_process_dof::apply(RenderTexture* rt, RenderTexture* buf, GLuint* samplers, camera* cam) {
    if (!this)
        return;

    bool use_dof_f2 = false;
    if (data.debug.flags & DOF_DEBUG_USE_UI_PARAMS) {
        if (data.debug.flags & DOF_DEBUG_ENABLE_DOF) {
            if (data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF) {
                float_t focus = data.debug.focus;
                if (data.debug.flags & DOF_DEBUG_AUTO_FOCUS) {
                    rob_chara* rob_chr = 0;
                    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                        rob_chara* rob_chr = rob_chara_array_get(i);
                        if (!rob_chr || !rob_chr->is_visible())
                            continue;

                        mat4 mat = mat4_identity;
                        rob_chr->sub_1405163C0(4, mat);

                        vec3 chara_trans = 0.0f;
                        mat4_get_translation(&mat, &chara_trans);

                        mat4 view_transpose;
                        mat4_transpose(&cam->view, &view_transpose);
                        focus = -vec3::dot(*(vec3*)&view_transpose.row2, chara_trans)
                            - view_transpose.row2.w - 0.1f;
                        break;
                    }
                }

                focus = max_def(focus, (float_t)cam->min_distance);
                renderer::DOF3::apply_physical(this, rt, buf, samplers,
                    rt->color_texture->tex, rt->depth_texture->tex,
                    (float_t)cam->min_distance, (float_t)cam->max_distance, focus,
                    data.debug.focal_length, (float_t)cam->fov_rad, data.debug.f_number);
            }
            else {
                float_t fuzzing_range = max_def(data.debug.f2.fuzzing_range, 0.01f);
                renderer::DOF3::apply_f2(this, rt, buf, samplers,
                    rt->color_texture->tex, rt->depth_texture->tex,
                    (float_t)cam->min_distance, (float_t)cam->max_distance, (float_t)cam->fov_rad,
                    data.debug.f2.focus, data.debug.f2.focus_range, fuzzing_range, data.debug.f2.ratio);
                use_dof_f2 = true;
            }
        }
    }
    else if (data.pv.enable && data.pv.f2.ratio > 0.0f) {
        float_t fuzzing_range = max_def(data.pv.f2.fuzzing_range, 0.01f);
        renderer::DOF3::apply_f2(this, rt, buf, samplers,
            rt->color_texture->tex, rt->depth_texture->tex,
            (float_t)cam->min_distance, (float_t)cam->max_distance, (float_t)cam->fov_rad,
            data.pv.f2.focus, data.pv.f2.focus_range, fuzzing_range, data.pv.f2.ratio);
        enum_or(data.debug.flags, DOF_DEBUG_ENABLE_DOF);
        data.debug.f2 = data.pv.f2;
        use_dof_f2 = true;
    }
    else
        enum_and(data.debug.flags, ~DOF_DEBUG_ENABLE_DOF);
}

void post_process_dof::init_fbo(int32_t width, int32_t height) {
    if (!this || (this->width == width && this->height == height))
        return;

    post_process_dof_free_fbo(this);

    this->width = width;
    this->height = height;
    int32_t w20 = max_def(width / 20, 1);
    int32_t h20 = max_def(height / 20, 1);
    int32_t w2 = max_def(width / 2, 1);
    int32_t h2 = max_def(height / 2, 1);
    glGenTextures(6, textures);
    gl_state_bind_texture_2d(textures[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, w20, h20);
    fbo[0].init_data(w20, h20, &textures[0], 1, 0);
    gl_state_bind_texture_2d(textures[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, w20, h20);
    fbo[1].init_data(w20, h20, &textures[1], 1, 0);
    gl_state_bind_texture_2d(textures[2]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    gl_state_bind_texture_2d(textures[3]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    fbo[2].init_data(w2, h2, &textures[2], 2, 0);
    gl_state_bind_texture_2d(textures[4]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R11F_G11F_B10F, w2, h2);
    gl_state_bind_texture_2d(textures[5]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, w2, h2);
    fbo[3].init_data(w2, h2, &textures[4], 2, 0);

    vec2 data[50] = {};
    post_process_dof_calculate_texcoords(data, 3.0f);
    common_ubo.Create(sizeof(dof_common_shader_data));
    texcoords_ubo.Create(sizeof(data), data);

    glGenVertexArrays(1, &vao);
}

void post_process_dof::initialize_data(dof_debug* debug, dof_pv* pv) {
    set_dof_debug(debug);
    set_dof_pv(pv);
}

void post_process_dof::get_dof_debug(dof_debug* debug) {
    if (debug)
        *debug = data.debug;
}

void post_process_dof::set_dof_debug(dof_debug* debug) {
    if (debug)
        data.debug = *debug;
    else
        data.debug = dof_debug_default;
}

void post_process_dof::get_dof_pv(dof_pv* pv) {
    if (pv)
        *pv = data.pv;
}

void post_process_dof::set_dof_pv(dof_pv* pv) {
    if (pv)
        data.pv = *pv;
    else
        data.pv = dof_pv_default;
}

static void post_process_dof_calculate_texcoords(vec2* data, float_t size) {
    size_t i;
    size_t j;
    float_t v6;
    float_t v7;
    float_t v8;
    float_t v9;
    float_t v11;

    const float_t t = (float_t)(1.0 / 3.0);
    size *= 3.0f;
    for (i = 0; i < 7; i++) {
        v6 = (float_t)i * t - 1.0f;
        for (j = 0; j < 7; j++) {
            v7 = (float_t)j * t - 1.0f;
            if (-v6 >= v7) {
                if (v7 < v6) {
                    v8 = -v7;
                    v9 = (v6 / v7) + 4.0f;
                }
                else if (v6 == 0.0f) {
                    v8 = 0.0f;
                    v9 = 0.0f;
                }
                else {
                    v8 = -v6;
                    v9 = 6.0f - (v7 / v6);
                }
            }
            else if (v6 < v7) {
                v8 = (float_t)j * t - 1.0f;
                v9 = v6 / v7;
            }
            else {
                v8 = (float_t)i * t - 1.0f;
                v9 = 2.0f - (v7 / v6);
            }
            v8 *= size;
            v11 = v9 * (float_t)(M_PI * 0.25);

            *data++ = vec2(cosf(v11), sinf(v11)) * v8;
        }
    }
}

static void post_process_dof_free_fbo(post_process_dof* dof) {
    if (dof->vao) {
        glDeleteVertexArrays(1, &dof->vao);
        dof->vao = 0;
    }

    dof->texcoords_ubo.Destroy();
    dof->common_ubo.Destroy();

    if (dof->textures[0]) {
        glDeleteTextures(6, dof->textures);
        dof->textures[0] = 0;
    }
}

namespace renderer {
    void DOF3::apply_f2(post_process_dof* dof, RenderTexture* rt,
        RenderTexture* buf, GLuint* samplers, GLuint color_texture,
        GLuint depth_texture, float_t min_distance, float_t max_distance, float_t fov,
        float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio) {
        gl_state_begin_event("renderer::DOF3::apply_f2");
        gl_state_disable_blend();
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_set_depth_func(GL_ALWAYS);
        update_data(dof, min_distance, max_distance,
            fov, focus, 0.0f, 1.0f, focus_range, fuzzing_range, ratio);

        uniform_value[U_DOF] = 1;

        gl_state_bind_vertex_array(dof->vao);
        render_tiles(dof, samplers, depth_texture, true);
        downsample(dof, samplers, color_texture, depth_texture, true);
        main_filter(dof, samplers, true);
        upsample(dof, rt, buf, samplers, color_texture, depth_texture, true);

        shader::unbind();
        for (int32_t i = 0; i < 8; i++) {
            gl_state_bind_sampler(i, 0);
            gl_state_active_bind_texture_2d(i, 0);
        }
        gl_state_bind_vertex_array(0);
        gl_state_end_event();
    }

    void DOF3::apply_physical(post_process_dof* dof, RenderTexture* rt,
        RenderTexture* buf, GLuint* samplers, GLuint color_texture,
        GLuint depth_texture, float_t min_distance, float_t max_distance,
        float_t focus, float_t focal_length, float_t fov, float_t f_number) {
        gl_state_begin_event("renderer::DOF3::apply_physical");
        gl_state_disable_blend();
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_set_depth_func(GL_ALWAYS);
        update_data(dof, min_distance, max_distance,
            fov, focus, focal_length, f_number, 0.0f, 0.1f, 0.0f);

        uniform_value[U_DOF] = 0;

        gl_state_bind_vertex_array(dof->vao);
        render_tiles(dof, samplers, depth_texture, false);
        downsample(dof, samplers, color_texture, depth_texture, false);
        main_filter(dof, samplers, false);
        upsample(dof, rt, buf, samplers, color_texture, depth_texture, false);

        shader::unbind();
        for (int32_t i = 0; i < 8; i++) {
            gl_state_bind_sampler(i, 0);
            gl_state_active_bind_texture_2d(i, 0);
        }
        gl_state_bind_vertex_array(0);
        gl_state_end_event();
    }

    void DOF3::render_tiles(post_process_dof* dof,
        GLuint* samplers, GLuint depth_texture, bool f2) {
        gl_state_begin_event("renderer::DOF3::render_tiles");
        gl_state_bind_framebuffer(dof->fbo[0].buffer);
        glViewport(0, 0, dof->fbo[0].width, dof->fbo[0].height);
        uniform_value[U_DOF_STAGE] = 0;
        shaders_ft.set(SHADER_FT_DOF);
        dof->common_ubo.Bind(0);
        gl_state_active_bind_texture_2d(0, depth_texture);
        gl_state_bind_sampler(0, samplers[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        gl_state_bind_framebuffer(dof->fbo[1].buffer);
        glViewport(0, 0, dof->fbo[1].width, dof->fbo[1].height);
        uniform_value[U_DOF_STAGE] = 1;
        shaders_ft.set(SHADER_FT_DOF);
        gl_state_active_bind_texture_2d(0, dof->textures[0]);
        gl_state_bind_sampler(0, samplers[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        gl_state_end_event();

    }

    void DOF3::downsample(post_process_dof* dof,
        GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2) {
        gl_state_begin_event("renderer::DOF3::downsample");
        gl_state_bind_framebuffer(dof->fbo[2].buffer);
        glViewport(0, 0, dof->fbo[2].width, dof->fbo[2].height);
        uniform_value[U_DOF_STAGE] = 2;
        shaders_ft.set(SHADER_FT_DOF);
        dof->common_ubo.Bind(0);
        gl_state_active_bind_texture_2d(0, depth_texture);
        gl_state_bind_sampler(0, samplers[1]);
        gl_state_active_bind_texture_2d(1, color_texture);
        gl_state_bind_sampler(1, samplers[0]);
        gl_state_active_bind_texture_2d(2, dof->textures[1]);
        gl_state_bind_sampler(2, samplers[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        gl_state_end_event();
    }

    void DOF3::main_filter(post_process_dof* dof, GLuint* samplers, bool f2) {
        gl_state_begin_event("renderer::DOF3::main_filter");
        gl_state_bind_framebuffer(dof->fbo[3].buffer);
        glViewport(0, 0, dof->fbo[3].width, dof->fbo[3].height);
        uniform_value[U_DOF_STAGE] = 3;
        shaders_ft.set(SHADER_FT_DOF);
        dof->common_ubo.Bind(0);
        dof->texcoords_ubo.Bind(1);
        gl_state_active_bind_texture_2d(0, dof->textures[3]);
        gl_state_bind_sampler(0, samplers[1]);
        gl_state_active_bind_texture_2d(1, dof->textures[2]);
        gl_state_bind_sampler(1, samplers[1]);
        gl_state_active_bind_texture_2d(2, dof->textures[1]);
        gl_state_bind_sampler(2, samplers[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        gl_state_end_event();
    }

    void DOF3::upsample(post_process_dof* dof, RenderTexture* rt, RenderTexture* buf,
        GLuint* samplers, GLuint color_texture, GLuint depth_texture, bool f2) {
        gl_state_begin_event("renderer::DOF3::upsample");
        buf->Bind();
        glViewport(0, 0, dof->width, dof->height);
        uniform_value[U_DOF_STAGE] = 4;
        shaders_ft.set(SHADER_FT_DOF);
        dof->common_ubo.Bind(0);
        gl_state_active_bind_texture_2d(0, dof->textures[4]);
        gl_state_bind_sampler(0, samplers[1]);
        gl_state_active_bind_texture_2d(1, dof->textures[5]);
        gl_state_bind_sampler(1, samplers[1]);
        gl_state_active_bind_texture_2d(2, dof->textures[1]);
        gl_state_bind_sampler(2, samplers[1]);
        gl_state_active_bind_texture_2d(3, color_texture);
        gl_state_bind_sampler(3, samplers[1]);
        gl_state_active_bind_texture_2d(4, depth_texture);
        gl_state_bind_sampler(4, samplers[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        fbo::blit(buf->fbos[0], rt->fbos[0],
            0, 0, buf->color_texture->width, buf->color_texture->height,
            0, 0, rt->color_texture->width, rt->color_texture->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        gl_state_end_event();
    }

    void DOF3::update_data(post_process_dof* dof, float_t min_dist,
        float_t max_dist, float_t fov, float_t dist, float_t focal_length,
        float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio) {
        float_t fl = focal_length;
        if (dist <= focal_length)
            fl = dist + 0.1f;
        fl = fl / (dist - fl) * fl / f_number;

        dof_common_shader_data shader_data = {};
        shader_data.g_depth_params.x = 1.0f / (min_dist * max_dist) * (min_dist - max_dist);
        shader_data.g_depth_params.y = 1.0f / min_dist;
        shader_data.g_depth_params.z = -((fl * dist * (min_dist - max_dist)) * (1.0f / (min_dist * max_dist)));
        shader_data.g_depth_params.w = (1.0f - 1.0f / min_dist * dist) * fl;
        shader_data.g_spread_scale.x = 720.0f / (tanf(fov * 0.5f) * (min_dist * 2.0f));
        shader_data.g_spread_scale.y = shader_data.g_spread_scale.x * (float_t)(1.0 / 3.0);
        shader_data.g_spread_scale.z = (float_t)(1.0 / 3.0);
        shader_data.g_spread_scale.w = 3.0f;
        shader_data.g_depth_params2.x = dist;
        shader_data.g_depth_params2.y = focus_range;
        shader_data.g_depth_params2.z = -4.5f / (fuzzing_range * fuzzing_range);
        shader_data.g_depth_params2.w = ratio * 8.0f;
        dof->common_ubo.WriteMemory(shader_data);
    }
}
