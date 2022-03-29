/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst.h"
#include "../gl_state.h"
#include "../shader.h"
#include "../shader_ft.h"
#include "../static_var.h"
#include "random.h"
#include "render_group.h"
#include "render_element.h"
#include <glad/glad.h>

static glitter_render_element* GlitterF2RenderGroup_add_render_element(GlitterF2RenderGroup* a1,
    glitter_render_element* a2);
static void GlitterF2RenderGroup_calc_disp_line(GlitterF2RenderGroup* a1);
static void GlitterF2RenderGroup_calc_disp_locus(GPM, GlitterF2RenderGroup* a1);
static void GlitterF2RenderGroup_calc_disp_quad(GPM, GlitterF2RenderGroup* a1);
static void GlitterF2RenderGroup_calc_disp_quad_normal(GPM,
    GlitterF2RenderGroup* a1, mat4* model_mat, mat4* dir_mat);
static void GlitterF2RenderGroup_calc_disp_quad_direction_rotation(GlitterF2RenderGroup* a1,
    mat4* model_mat, mat4* dir_mat);
static void GlitterF2RenderGroup_calc_disp_locus_set_pivot(glitter_pivot pivot,
    float_t w, float_t* v00, float_t* v01);
static void GlitterF2RenderGroup_calc_disp_set_pivot(glitter_pivot pivot,
    float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11);
static bool GlitterF2RenderGroup_get_ext_anim_scale(GlitterF2RenderGroup* a1, vec3* a2);

GlitterRenderGroup::GlitterRenderGroup() : flags(), type(), draw_type(), pivot(),
z_offset(), count(), ctrl(), disp(), texture(), mask_texture(), frame(),
elements(), max_count(), random_ptr(), alpha(), fog(), vbo(), ebo() {
    split_u = 1;
    split_v = 1;
    split_uv = vec2_identity;
    mat = mat4_identity;
    mat_rot = mat4_identity;
    mat_draw = mat4_identity;
    alpha = DRAW_PASS_3D_TRANSLUCENT;
    emission = 1.0f;
    blend_mode = GLITTER_PARTICLE_BLEND_TYPICAL;
    mask_blend_mode = GLITTER_PARTICLE_BLEND_TYPICAL;
}

GlitterRenderGroup::~GlitterRenderGroup() {

}

GlitterF2RenderGroup::GlitterF2RenderGroup(GlitterF2ParticleInst* a1) : particle() {
    switch (a1->data.data.type) {
    case GLITTER_PARTICLE_QUAD:
    case GLITTER_PARTICLE_LINE:
    case GLITTER_PARTICLE_LOCUS:
        break;
    default:
        return;
    }

    if (a1->data.data.count > 0)
        count = a1->data.data.count;
    else
        count = a1->data.data.type == GLITTER_PARTICLE_LOCUS ? 30 : 250;
    max_count = count * 4;

    random_ptr = a1->data.random_ptr;
    particle = a1;

    elements = force_malloc_s(glitter_render_element, count);
    if (!elements) {
        count = 0;
        max_count = 0;
        return;
    }
    else if (!max_count)
        return;

    bool is_quad = a1->data.data.type == GLITTER_PARTICLE_QUAD;

    glGenBuffers(1, &vbo);
    gl_state_bind_array_buffer(vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(glitter_buffer) * max_count), 0, GL_DYNAMIC_DRAW);
    gl_state_bind_array_buffer(vbo);

    if (is_quad) {
        size_t count = max_count / 4 * 6;
        int32_t* ebo_data = force_malloc_s(int32_t, count);
        for (size_t i = 0, j = 0; i < count; i += 6, j += 4) {
            ebo_data[i] = (int32_t)j;
            ebo_data[i + 1] = (int32_t)(j + 1);
            ebo_data[i + 2] = (int32_t)(j + 2);
            ebo_data[i + 3] = (int32_t)(j + 0);
            ebo_data[i + 4] = (int32_t)(j + 2);
            ebo_data[i + 5] = (int32_t)(j + 3);
        }

        glGenBuffers(1, &ebo);
        gl_state_bind_element_array_buffer(ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * count, ebo_data, GL_STATIC_DRAW);
        gl_state_bind_element_array_buffer(0);
        free(ebo_data);
    }

    if (!is_quad) {
        vec_key.reserve(count);
        vec_val.reserve(count);
    }
}

GlitterF2RenderGroup::~GlitterF2RenderGroup() {
    DeleteBuffers(false);
    free(elements);
}

bool GlitterF2RenderGroup::CannotDisp() {
    GlitterEffectInst* effect;
    if (!particle)
        return true;
    else if (particle->data.effect)
        return (particle->data.effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else if ((particle = particle->data.parent) && (effect = particle->data.effect))
        return (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else
        return true;
}

void GlitterF2RenderGroup::Ctrl(GLT, float_t delta_frame, bool copy_mats) {
    if (!particle)
        return;

    GlitterF2ParticleInstData* data = &particle->data;
    blend_mode = data->data.blend_mode;
    mask_blend_mode = data->data.mask_blend_mode;
    texture = data->data.texture;
    mask_texture = data->data.mask_texture;
    split_u = data->data.split_u;
    split_v = data->data.split_v;
    split_uv = data->data.split_uv;
    type = data->data.type;
    draw_type = data->data.draw_type;
    z_offset = data->data.z_offset;
    pivot = data->data.pivot;
    flags = data->data.flags;

    GlitterF2EmitterInst* emitter;
    if (copy_mats && (emitter = particle->data.emitter)) {
        mat = emitter->mat;
        mat_rot = emitter->mat_rot;
    }

    for (size_t ctrl = this->ctrl, i = 0; ctrl; i++) {
        if (!elements[i].alive)
            continue;

        glitter_render_element_ctrl(GLT_VAL, this, &elements[i], delta_frame);
        ctrl--;
    }
    frame += delta_frame;
}

void GlitterF2RenderGroup::DeleteBuffers(bool a2) {
    if (particle) {
        if (!a2)
            particle->data.render_group = 0;
        particle = 0;
    }

    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }

    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (!a2 && elements) {
        Free();
        free(elements);
    }
}

void GlitterF2RenderGroup::Disp(GPM) {
    switch (type) {
    case GLITTER_PARTICLE_QUAD:
    case GLITTER_PARTICLE_LINE:
    case GLITTER_PARTICLE_LOCUS:
        break;
    default:
        return;
    }

    disp = 0;
    switch (type) {
    case GLITTER_PARTICLE_QUAD:
        GlitterF2RenderGroup_calc_disp_quad(GPM_VAL, this);
        break;
    case GLITTER_PARTICLE_LINE:
        GlitterF2RenderGroup_calc_disp_line(this);
        break;
    case GLITTER_PARTICLE_LOCUS:
        GlitterF2RenderGroup_calc_disp_locus(GPM_VAL, this);
        break;
    }

    if (disp < 1)
        return;

    gl_state_enable_blend();
    switch (blend_mode) {
    case GLITTER_PARTICLE_BLEND_ADD:
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE);
        break;
    case GLITTER_PARTICLE_BLEND_MULTIPLY:
        gl_state_set_blend_func(GL_ZERO, GL_SRC_COLOR);
        break;
    default:
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
    gl_state_set_blend_equation(GL_FUNC_ADD);

    if (type != GLITTER_PARTICLE_LINE && texture) {
        gl_state_active_bind_texture_2d(0, texture);
        if (mask_texture) {
            gl_state_active_bind_texture_2d(1, mask_texture);

            uniform_value[U_TEXTURE_COUNT] = 2;
            switch (mask_blend_mode) {
            default:
                uniform_value[U_TEXTURE_BLEND] = 0;
                break;
            case GLITTER_PARTICLE_BLEND_MULTIPLY:
                uniform_value[U_TEXTURE_BLEND] = 1;
                break;
            case GLITTER_PARTICLE_BLEND_ADD:
                uniform_value[U_TEXTURE_BLEND] = 2;
                break;
            }
        }
        else {
            gl_state_active_bind_texture_2d(1, 0);
            uniform_value[U_TEXTURE_COUNT] = 1;
            uniform_value[U_TEXTURE_BLEND] = 0;
        }
    }
    else {
        gl_state_active_bind_texture_2d(0, 0);
        gl_state_active_bind_texture_2d(1, 0);
        uniform_value[U_TEXTURE_COUNT] = 0;
        uniform_value[U_TEXTURE_BLEND] = 0;
    }

    switch (type) {
    case GLITTER_PARTICLE_QUAD:
        switch (fog) {
        default:
            uniform_value[U_FOG_HEIGHT] = 0;
            break;
        case FOG_DEPTH:
            uniform_value[U_FOG_HEIGHT] = 1;
            break;
        case FOG_HEIGHT:
            uniform_value[U_FOG_HEIGHT] = 2;
            break;
        }

        if (blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH) {
            uniform_value[U_ALPHA_BLEND] = 1;
            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LESS);
            gl_state_set_depth_mask(GL_TRUE);
        }
        else {
            uniform_value[U_ALPHA_BLEND] = alpha != DRAW_PASS_3D_OPAQUE ? 2 : 0;
            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LESS);
            gl_state_set_depth_mask(GL_FALSE);
        }

        if (draw_type == GLITTER_DIRECTION_BILLBOARD) {
            gl_state_enable_cull_face();
            gl_state_set_cull_face_mode(GL_BACK);
        }
        else
            gl_state_disable_cull_face();
        break;
    case GLITTER_PARTICLE_LINE:
        uniform_value[U_FOG_HEIGHT] = 0;
        uniform_value[U_ALPHA_BLEND] = 2;

        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LESS);
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_enable_cull_face();
        gl_state_set_cull_face_mode(GL_BACK);
        break;
    case GLITTER_PARTICLE_LOCUS:
        uniform_value[U_FOG_HEIGHT] = 0;
        uniform_value[U_ALPHA_BLEND] = 2;

        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LESS);
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_disable_cull_face();
        break;
    }

    float_t emission = 1.0f;
    if (flags & GLITTER_PARTICLE_EMISSION || blend_mode == GLITTER_PARTICLE_BLEND_TYPICAL)
        emission = this->emission;

    shader_state_material_set_emission(&shaders_ft, false, emission, emission, emission, 1.0f);
    shader_state_matrix_set_mvp_separate(&shaders_ft,
        &mat_draw, &GPM_VAL->cam_view, &GPM_VAL->cam_projection);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, (mat4*)&mat4_identity);
    shader_env_vert_set_ptr(&shaders_ft, 3, (vec4*)&vec4_identity);

    shader_set(&shaders_ft, SHADER_FT_GLITTER_PT);
    switch (type) {
    case GLITTER_PARTICLE_QUAD: {
        static const GLsizei buffer_size = sizeof(glitter_buffer);

        gl_state_bind_array_buffer(vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, position)); // Pos
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, color));    // Color
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, uv));       // TexCoord0
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, uv));       // TexCoord1
        gl_state_bind_array_buffer(0);

        gl_state_bind_element_array_buffer(ebo);
        shader_draw_elements(&shaders_ft,
            GL_TRIANGLES, (GLsizei)(6 * disp), GL_UNSIGNED_INT, 0);
        gl_state_bind_element_array_buffer(0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(8);
        glDisableVertexAttribArray(9);

    } break;
    case GLITTER_PARTICLE_LINE:
    case GLITTER_PARTICLE_LOCUS: {
        static const GLsizei buffer_size = sizeof(glitter_buffer);

        gl_state_bind_array_buffer(vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, position)); // Pos
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, color));    // Color
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, uv));       // TexCoord0
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(glitter_buffer, uv));       // TexCoord1
        gl_state_bind_array_buffer(0);

        const GLenum mode = type == GLITTER_PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
        const size_t count = vec_key.size();
        for (size_t i = 0; i < count; i++)
            shader_draw_arrays(&shaders_ft,
                mode, vec_key.data()[i], vec_val.data()[i]);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(8);
        glDisableVertexAttribArray(9);

    } break;
    }
    gl_state_disable_blend();
    gl_state_enable_cull_face();
    gl_state_disable_depth_test();
}

void GlitterF2RenderGroup::Emit(GPM, GLT, GlitterF2ParticleInstData* a2,
    GlitterF2EmitterInst* a3, int32_t dup_count, int32_t count) {
    glitter_render_element* element;
    int32_t i;
    int32_t index;

    for (element = 0, i = 0; i < dup_count; i++)
        for (index = 0; index < count; index++, element++) {
            element = GlitterF2RenderGroup_add_render_element(this, element);
            if (!element)
                break;

            glitter_render_element_emit(GPM_VAL, GLT_VAL,
                element, a2, a3, index, random_ptr);
        }
}

void GlitterF2RenderGroup::Free() {
    glitter_render_element* elem = elements;
    for (size_t i = count; i; i--, elem++)
        glitter_render_element_free(elem);
    ctrl = 0;
}

static glitter_render_element* GlitterF2RenderGroup_add_render_element(GlitterF2RenderGroup* a1,
    glitter_render_element* a2) {
    size_t v3;
    size_t v4;

    if (!a2)
        a2 = a1->elements;

    v3 = a1->count - (a2 - a1->elements);
    v4 = 0;
    if (v3 < 1)
        return 0;

    while (a2->alive) {
        v4++;
        a2++;
        if (v4 >= v3)
            return 0;
    }

    a2->alive = true;
    a1->ctrl++;
    return a2;
}

static void GlitterF2RenderGroup_calc_disp_line(GlitterF2RenderGroup* a1) {
    size_t count;
    size_t i;
    size_t j;
    vec3 scale;
    vec3 pos;
    vec3 pos_diff;
    bool has_scale;
    glitter_buffer* buf;
    size_t disp;
    glitter_render_element* elem;
    GlitterLocusHistory* hist;
    size_t index;

    if (!a1->elements || !a1->vbo || a1->ctrl < 1)
        return;

    for (count = 0, i = a1->ctrl, elem = a1->elements; i > 0; i--, elem++) {
        if (!elem->alive)
            continue;

        if (elem->locus_history) {
            size_t length = elem->locus_history->data.size();
            if (length > 1)
                count +=  length;
        }
    }

    if (!count || count > a1->max_count)
        return;

    has_scale = false;
    scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        mat4_get_scale(&a1->mat, &scale);
        vec3_sub(scale, vec3_identity, scale);
        if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        mat4_normalize_rotation(&a1->mat, &a1->mat_draw);
    }
    else
        a1->mat_draw = mat4_identity;

    gl_state_bind_array_buffer(a1->vbo);
    buf = (glitter_buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    elem = a1->elements;
    disp = 0;
    a1->vec_key.clear();
    a1->vec_val.clear();
    for (i = a1->ctrl, index = 0; i > 0; elem++) {
        if (!elem->alive)
            continue;

        i--;
        hist = elem->locus_history;
        if (!elem->disp || !hist || hist->data.size() < 2)
            continue;

        j = 0;
        if (has_scale)
            for (GlitterLocusHistory::Data& hist_data : hist->data) {
                pos = hist_data.translation;
                vec3_sub(pos, elem->base_translation, pos_diff);
                vec3_mult(pos_diff, scale, pos_diff);
                vec3_add(pos, pos_diff, buf->position);
                buf->uv = vec2_null;
                buf->color = hist_data.color;
                j++;
                buf++;
            }
        else
            for (GlitterLocusHistory::Data& hist_data : hist->data) {
                buf->position = hist_data.translation;
                buf->uv = vec2_null;
                buf->color = hist_data.color;
                j++;
                buf++;
            }

        if (j > 0) {
            disp += j;
            a1->vec_key.push_back((uint32_t)index);
            a1->vec_val.push_back((uint32_t)j);
            index += j;
        }
    }
    a1->disp = disp;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    gl_state_bind_array_buffer(0);
}

static void GlitterF2RenderGroup_calc_disp_locus(GPM, GlitterF2RenderGroup* a1) {
    size_t i;
    size_t j;
    size_t count;
    vec3 scale;
    bool has_scale;
    glitter_render_element* elem;
    GlitterLocusHistory* hist;
    float_t uv_u;
    float_t uv_u_2nd;
    float_t uv_v_2nd;
    float_t uv_v_scale;
    glitter_buffer* buf;
    size_t disp;
    vec3 pos;
    vec3 pos_diff;
    size_t index;
    float_t v00;
    float_t v01;
    vec3 x_vec;

    mat3 model_mat;

    if (!a1->elements || !a1->vbo || a1->ctrl < 1)
        return;

    for (count = 0, i = a1->ctrl, elem = a1->elements; i > 0; i--, elem++) {
        if (!elem->alive)
            continue;

        if (elem->locus_history) {
            hist = elem->locus_history;
            size_t length = elem->locus_history->data.size();
            if (length > 1)
                count += 2 * length;
        }
    }

    if (!count || count > a1->max_count)
        return;

    x_vec = { 1.0f, 0.0f, 0.0f };
    if (a1->flags & GLITTER_PARTICLE_LOCAL) {
        mat4 mat;
        mat4_mult(&GPM_VAL->cam_inv_view, &a1->mat, &mat);
        mat4_mult(&GPM_VAL->cam_view, &mat, &mat);
        mat4_mult(&mat, &GPM_VAL->cam_inv_view, &a1->mat_draw);
    }
    else
        a1->mat_draw = mat4_identity;

    has_scale = false;
    scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        mat4_get_scale(&a1->mat, &scale);
        if (a1->flags & GLITTER_PARTICLE_SCALE)
            x_vec.x = scale.x;
        vec3_sub(scale, vec3_identity, scale);
        if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        mat4_mult_vec3(&a1->mat_rot, &scale, &scale);

        mat4 mat;
        mat4_normalize_rotation(&a1->mat, &mat);
        mat3_from_mat4(&mat, &model_mat);
    }
    else
        model_mat = mat3_identity;

    mat3_mult_vec(&GPM_VAL->cam_inv_view_mat3, &x_vec, &x_vec);

    gl_state_bind_array_buffer(a1->vbo);
    buf = (glitter_buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    elem = a1->elements;
    disp = 0;
    a1->vec_key.clear();
    a1->vec_val.clear();
    for (i = a1->ctrl, index = 0; i > 0; elem++) {
        if (!elem->alive)
            continue;

        i--;
        hist = elem->locus_history;
        if (!elem->disp || !hist || hist->data.size() < 2)
            continue;

        uv_u = elem->uv.x + elem->uv_scroll.x;
        uv_u_2nd = elem->uv.x + elem->uv_scroll.x + a1->split_uv.x;
        uv_v_2nd = elem->uv.y + elem->uv_scroll.y + a1->split_uv.y;
        uv_v_scale = a1->split_uv.y / (float_t)(hist->data.size() - 1);

        uv_v_2nd = 1.0f - uv_v_2nd;

        size_t len = elem->locus_history->data.size();
        j = 0;
        if (has_scale)
            for (GlitterLocusHistory::Data& hist_data : hist->data) {
                pos = hist_data.translation;
                vec3_sub(pos, elem->base_translation, pos_diff);
                vec3_mult(pos_diff, scale, pos_diff);
                mat3_mult_vec(&model_mat, &pos_diff, &pos_diff);
                vec3_add(pos, pos_diff, pos);

                GlitterF2RenderGroup_calc_disp_locus_set_pivot(a1->pivot,
                    hist_data.scale * elem->scale.x * elem->scale_all,
                    &v00, &v01);

                vec3_mult_scalar(x_vec, v00, buf[0].position);
                vec3_add(buf[0].position, pos, buf[0].position);
                buf[0].uv.x = uv_u;
                buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                buf[0].color = hist_data.color;

                vec3_mult_scalar(x_vec, v01, buf[1].position);
                vec3_add(buf[1].position, pos, buf[1].position);
                buf[1].uv.x = uv_u_2nd;
                buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                buf[1].color = hist_data.color;
                j++;
                buf += 2;
            }
        else
            for (GlitterLocusHistory::Data& hist_data : hist->data) {
                pos = hist_data.translation;

                GlitterF2RenderGroup_calc_disp_locus_set_pivot(a1->pivot,
                    hist_data.scale * elem->scale.x * elem->scale_all,
                    &v00, &v01);

                vec3_mult_scalar(x_vec, v00, buf[0].position);
                vec3_add(buf[0].position, pos, buf[0].position);
                buf[0].uv.x = uv_u;
                buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                buf[0].color = hist_data.color;

                vec3_mult_scalar(x_vec, v01, buf[1].position);
                vec3_add(buf[1].position, pos, buf[1].position);
                buf[1].uv.x = uv_u_2nd;
                buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                buf[1].color = hist_data.color;
                j++;
                buf += 2;
            }

        if (j > 0) {
            disp += j;
            a1->vec_key.push_back((uint32_t)index);
            a1->vec_val.push_back((uint32_t)(j * 2));
            index += j * 2;
        }
    }
    a1->disp = disp;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    gl_state_bind_array_buffer(0);
}

static void GlitterF2RenderGroup_calc_disp_quad(GPM, GlitterF2RenderGroup* a1) {
    mat4 model_mat;
    mat4 dir_mat;
    mat4 view_mat;
    mat4 inv_view_mat;

    if (!a1->elements || !a1->vbo || a1->ctrl < 1)
        return;

    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        model_mat = a1->mat;
        mat4_normalize_rotation(&model_mat, &view_mat);
        mat4_mult(&view_mat, &GPM_VAL->cam_view, &view_mat);
        mat4_inverse(&view_mat, &inv_view_mat);
    }
    else {
        model_mat = mat4_identity;
        view_mat = GPM_VAL->cam_view;
        inv_view_mat = GPM_VAL->cam_inv_view;
    }

    if (a1->flags & GLITTER_PARTICLE_LOCAL) {
        mat4_mult(&inv_view_mat, &a1->mat, &inv_view_mat);
        mat4_mult(&view_mat, &inv_view_mat, &view_mat);
        mat4_inverse(&view_mat, &inv_view_mat);
    }

    mat4_mult(&view_mat, &GPM_VAL->cam_inv_view, &a1->mat_draw);

    switch (a1->draw_type) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat4_clear_trans(&model_mat, &dir_mat);
        mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
        mat4_clear_trans(&dir_mat, &dir_mat);
        break;
    case GLITTER_DIRECTION_EMITTER_DIRECTION:
        mat4_clear_trans(&a1->mat_rot, &dir_mat);
        break;
    case GLITTER_DIRECTION_PREV_POSITION:
    case GLITTER_DIRECTION_EMIT_POSITION:
    case GLITTER_DIRECTION_PREV_POSITION_DUP:
        if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL)
            mat4_clear_trans(&a1->mat_rot, &dir_mat);
        else
            dir_mat = mat4_identity;
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_Z_AXIS:
        mat4_rotate_z((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_AXIS:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    default:
        dir_mat = mat4_identity;
        break;
    }

    switch (a1->draw_type) {
    case GLITTER_DIRECTION_PREV_POSITION:
    case GLITTER_DIRECTION_EMIT_POSITION:
    case GLITTER_DIRECTION_PREV_POSITION_DUP:
        GlitterF2RenderGroup_calc_disp_quad_direction_rotation(a1, &model_mat, &dir_mat);
        break;
    default:
        GlitterF2RenderGroup_calc_disp_quad_normal(GPM_VAL, a1, &model_mat, &dir_mat);
        break;
    }
}

static void GlitterF2RenderGroup_calc_disp_quad_normal(GPM,
    GlitterF2RenderGroup* a1, mat4* model_mat, mat4* dir_mat) {
    size_t i;
    size_t j;
    size_t j_max;
    size_t k;
    glitter_render_element* elem;
    size_t disp;
    glitter_buffer* buf;
    vec3 z_offset_dir;
    bool has_scale;
    vec3 pos;
    bool use_z_offset;
    bool emitter_local;
    vec2 base_uv;
    vec3 scale;
    vec3 ext_scale;
    vec3 x_vec;
    vec3 y_vec;
    vec3 dist_to_cam;
    vec3 pos_diff;
    vec2 scale_particle;
    float_t v00;
    float_t v01;
    float_t v11;
    float_t v10;
    vec2 pos_add[4];
    vec2 uv_add[4];
    vec3 y_vec_rot;
    vec3 x_vec_rot;
    mat4 inv_model_mat;
    mat4 z_offset_inv_mat;
    mat3 ptc_rot;
    float_t rot_z_cos;
    float_t rot_z_sin;

    mat4_inverse(model_mat, &inv_model_mat);
    mat4_clear_trans(&inv_model_mat, &inv_model_mat);

    x_vec = { 1.0f, 0.0f, 0.0f };
    y_vec = { 0.0f, 1.0f, 0.0f };
    use_z_offset = false;
    dist_to_cam = vec3_null;
    z_offset_inv_mat = mat4_identity;
    if (fabsf(a1->z_offset) > 0.000001f) {
        use_z_offset = true;
        mat4_get_translation(model_mat, &dist_to_cam);
        vec3_sub(GPM_VAL->cam_view_point, dist_to_cam, dist_to_cam);
        if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
            mat4_normalize_rotation(model_mat, &z_offset_inv_mat);
            mat4_inverse(&z_offset_inv_mat, &z_offset_inv_mat);
        }
    }

    has_scale = false;
    emitter_local = false;
    scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        mat4_get_scale(model_mat, &scale);
        if (a1->flags & GLITTER_PARTICLE_SCALE) {
            x_vec.x = scale.x;
            y_vec.y = scale.y;
        }
        vec3_sub(scale, vec3_identity, scale);
        if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        mat4_mult_vec3(&a1->mat_rot, &scale, &scale);
        emitter_local = true;
    }

    if (GlitterF2RenderGroup_get_ext_anim_scale(a1, &ext_scale)) {
        vec2_add(*(vec2*)&ext_scale, vec2_identity, *(vec2*)&ext_scale);
        x_vec.x *= ext_scale.x;
        y_vec.y *= ext_scale.y;
    }

    if (a1->draw_type != GLITTER_DIRECTION_BILLBOARD) {
        mat4_mult_vec3(dir_mat, &x_vec, &x_vec);
        mat4_mult_vec3(dir_mat, &y_vec, &y_vec);
    }
    else
        mat4_mult(&inv_model_mat, dir_mat, &inv_model_mat);

    mat4_mult_vec3(&inv_model_mat, &x_vec, &x_vec);
    mat4_mult_vec3(&inv_model_mat, &y_vec, &y_vec);

    gl_state_bind_array_buffer(a1->vbo);
    buf = (glitter_buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    elem = a1->elements;
    disp = 0;
    if (a1->draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION)
        for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->disp)
                    continue;

                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                pos = elem->translation;
                if (has_scale) {
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    mat4_mult_vec3(&inv_model_mat, &pos_diff, &pos_diff);
                    vec3_add(pos, pos_diff, pos);
                }
                GlitterF2RenderGroup_calc_disp_set_pivot(a1->pivot,
                    scale_particle.x,
                    scale_particle.y,
                    &v00, &v01, &v10, &v11);

                pos_add[0].x = v00;
                pos_add[0].y = v11;
                uv_add[0].x = 0.0f;
                uv_add[0].y = 0.0f;
                pos_add[1].x = v00;
                pos_add[1].y = v10;
                uv_add[1].x = 0.0f;
                uv_add[1].y = a1->split_uv.y;
                pos_add[2].x = v01;
                pos_add[2].y = v10;
                uv_add[2].x = a1->split_uv.x;
                uv_add[2].y = a1->split_uv.y;
                pos_add[3].x = v01;
                pos_add[3].y = v11;
                uv_add[3].x = a1->split_uv.x;
                uv_add[3].y = 0.0f;

                vec2_add(elem->uv_scroll, elem->uv, base_uv);
                if (use_z_offset) {
                    vec3_sub(dist_to_cam, pos, z_offset_dir);
                    vec3_normalize(z_offset_dir, z_offset_dir);

                    if (emitter_local)
                        mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                    vec3_mult_scalar(z_offset_dir, a1->z_offset, z_offset_dir);
                    vec3_add(pos, z_offset_dir, pos);
                }

                mat3_rotate(elem->rotation.x, elem->rotation.y, elem->rotation.z, &ptc_rot);
                for (k = 0; k < 4; k++, buf++) {
                    x_vec_rot.x = x_vec.x * pos_add[k].x;
                    x_vec_rot.y = y_vec.y * pos_add[k].y;
                    x_vec_rot.z = 0.0f;
                    mat3_mult_vec(&ptc_rot, &x_vec_rot, &x_vec_rot);
                    vec3_add(x_vec_rot, pos, buf->position);
                    vec2_add(base_uv, uv_add[k], buf->uv);
                    buf->color = elem->color;
                }
                disp++;
            }
        }
    else
        for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->disp)
                    continue;

                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                pos = elem->translation;
                if (has_scale) {
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    mat4_mult_vec3(&inv_model_mat, &pos_diff, &pos_diff);
                    vec3_add(pos, pos_diff, pos);
                }
                GlitterF2RenderGroup_calc_disp_set_pivot(a1->pivot,
                    scale_particle.x,
                    scale_particle.y,
                    &v00, &v01, &v10, &v11);

                pos_add[0].x = v00;
                pos_add[0].y = v11;
                uv_add[0].x = 0.0f;
                uv_add[0].y = 0.0f;
                pos_add[1].x = v00;
                pos_add[1].y = v10;
                uv_add[1].x = 0.0f;
                uv_add[1].y = a1->split_uv.y;
                pos_add[2].x = v01;
                pos_add[2].y = v10;
                uv_add[2].x = a1->split_uv.x;
                uv_add[2].y = a1->split_uv.y;
                pos_add[3].x = v01;
                pos_add[3].y = v11;
                uv_add[3].x = a1->split_uv.x;
                uv_add[3].y = 0.0f;

                vec2_add(elem->uv_scroll, elem->uv, base_uv);
                if (use_z_offset) {
                    vec3_sub(dist_to_cam, pos, z_offset_dir);
                    vec3_normalize(z_offset_dir, z_offset_dir);

                    if (emitter_local)
                        mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                    vec3_mult_scalar(z_offset_dir, a1->z_offset, z_offset_dir);
                    vec3_add(pos, z_offset_dir, pos);
                }

                rot_z_cos = cosf(elem->rotation.z);
                rot_z_sin = sinf(elem->rotation.z);
                for (k = 0; k < 4; k++, buf++) {
                    vec3_mult_scalar(x_vec, rot_z_cos * pos_add[k].x - rot_z_sin * pos_add[k].y, x_vec_rot);
                    vec3_mult_scalar(y_vec, rot_z_sin * pos_add[k].x + rot_z_cos * pos_add[k].y, y_vec_rot);
                    vec3_add(x_vec_rot, y_vec_rot, x_vec_rot);
                    vec3_add(x_vec_rot, pos, buf->position);
                    vec2_add(base_uv, uv_add[k], buf->uv);
                    buf->color = elem->color;
                }
                disp++;
            }
        }
    a1->disp = disp;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    gl_state_bind_array_buffer(0);
}

static void GlitterF2RenderGroup_calc_disp_quad_direction_rotation(GlitterF2RenderGroup* a1,
    mat4* model_mat, mat4* dir_mat) {
    size_t i;
    size_t j;
    size_t j_max;
    size_t k;
    glitter_render_element* elem;
    size_t disp;
    glitter_buffer* buf;
    float_t rot_z_cos;
    float_t rot_z_sin;
    vec3 x_vec_rot;
    vec3 y_vec_rot;
    vec3 pos_add_rot;
    vec3 pos;
    vec2 scale_particle;
    float_t v00;
    float_t v01;
    float_t v10;
    float_t v11;
    vec2 pos_add[4];
    vec2 uv_add[4];
    vec2 base_uv;
    vec3 x_vec;
    vec3 y_vec;
    vec3 x_vec_base;
    vec3 y_vec_base;
    vec3 up_vec;
    mat4 mat;
    mat4 inv_model_mat;
    vec3 scale;
    bool use_scale;
    void(* rotate_func)(mat4*, GlitterF2RenderGroup*, glitter_render_element*, vec3*);

    mat4_inverse(model_mat, &inv_model_mat);
    mat4_clear_trans(&inv_model_mat, &inv_model_mat);

    x_vec_base = { 1.0f, 0.0f, 0.0f };
    y_vec_base = { 0.0f, 1.0f, 0.0f };

    mat4_mult_vec3(&inv_model_mat, &x_vec_base, &x_vec_base);
    mat4_mult_vec3(&inv_model_mat, &y_vec_base, &y_vec_base);

    if (a1->draw_type == GLITTER_DIRECTION_EMIT_POSITION) {
        up_vec = { 0.0f, 0.0f, 1.0f };
        rotate_func = glitter_render_element_rotate_to_emit_position;
    }
    else {
        up_vec = { 0.0f, 1.0f, 0.0f };
        rotate_func = glitter_render_element_rotate_to_prev_position;
    }

    mat4_get_scale(model_mat, &scale);

    gl_state_bind_array_buffer(a1->vbo);
    buf = (glitter_buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    use_scale = a1->flags & GLITTER_PARTICLE_SCALE ? true : false;
    elem = a1->elements;
    disp = 0;
    for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
        j_max = min(i, j_max);
        for (j = j_max; j > 0; elem++) {
            if (!elem->alive)
                continue;
            j--;

            if (!elem->disp)
                continue;

            vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
            vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

            if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                continue;

            pos = elem->translation;

            GlitterF2RenderGroup_calc_disp_set_pivot(a1->pivot,
                scale_particle.x,
                scale_particle.y,
                &v00, &v01, &v10, &v11);

            pos_add[0].x = v00;
            pos_add[0].y = v11;
            uv_add[0].x = 0.0f;
            uv_add[0].y = 0.0f;
            pos_add[1].x = v00;
            pos_add[1].y = v10;
            uv_add[1].x = 0.0f;
            uv_add[1].y = a1->split_uv.y;
            pos_add[2].x = v01;
            pos_add[2].y = v10;
            uv_add[2].x = a1->split_uv.x;
            uv_add[2].y = a1->split_uv.y;
            pos_add[3].x = v01;
            pos_add[3].y = v11;
            uv_add[3].x = a1->split_uv.x;
            uv_add[3].y = 0.0f;

            vec2_add(elem->uv_scroll, elem->uv, base_uv);
            rotate_func(&mat, a1, elem, &up_vec);
            mat4_clear_trans(&mat, &mat);
            mat4_mult(dir_mat, &mat, &mat);
            mat4_clear_trans(&mat, &mat);
            mat4_mult_vec3(&mat, &x_vec_base, &x_vec);
            mat4_mult_vec3(&mat, &y_vec_base, &y_vec);

            if (use_scale) {
                vec3_mult(x_vec, scale, x_vec);
                vec3_mult(y_vec, scale, y_vec);
            }

            rot_z_cos = cosf(elem->rotation.z);
            rot_z_sin = sinf(elem->rotation.z);
            for (k = 0; k < 4; k++, buf++) {
                vec3_mult_scalar(x_vec, pos_add[k].x * rot_z_cos - pos_add[k].y * rot_z_sin, x_vec_rot);
                vec3_mult_scalar(y_vec, pos_add[k].x * rot_z_sin + pos_add[k].y * rot_z_cos, y_vec_rot);
                vec3_add(x_vec_rot, y_vec_rot, pos_add_rot);
                vec3_add(pos, pos_add_rot, buf->position);
                vec2_add(base_uv, uv_add[k], buf->uv);
                buf->color = elem->color;
            }
            disp++;
        }
    }
    a1->disp = disp;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    gl_state_bind_array_buffer(0);
}

static void GlitterF2RenderGroup_calc_disp_locus_set_pivot(glitter_pivot pivot,
    float_t w, float_t* v00, float_t* v01) {
    switch (pivot) {
    case GLITTER_PIVOT_TOP_LEFT:
    case GLITTER_PIVOT_MIDDLE_LEFT:
    case GLITTER_PIVOT_BOTTOM_LEFT:
        *v00 = 0.0f;
        *v01 = w;
        break;
    case GLITTER_PIVOT_TOP_CENTER:
    case GLITTER_PIVOT_MIDDLE_CENTER:
    case GLITTER_PIVOT_BOTTOM_CENTER:
    default:
        *v00 = w * -0.5f;
        *v01 = w * 0.5f;
        break;
    case GLITTER_PIVOT_TOP_RIGHT:
    case GLITTER_PIVOT_MIDDLE_RIGHT:
    case GLITTER_PIVOT_BOTTOM_RIGHT:
        *v00 = -w;
        *v01 = 0.0f;
        break;
    }
}

static void GlitterF2RenderGroup_calc_disp_set_pivot(glitter_pivot pivot,
    float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11) {
    switch (pivot) {
    case GLITTER_PIVOT_TOP_LEFT:
        *v00 = 0.0f;
        *v01 = w;
        *v10 = -h;
        *v11 = 0.0f;
        break;
    case GLITTER_PIVOT_TOP_CENTER:
        *v00 = w * -0.5f;
        *v01 = w * 0.5f;
        *v10 = -h;
        *v11 = 0.0f;
        break;
    case GLITTER_PIVOT_TOP_RIGHT:
        *v00 = -w;
        *v01 = 0.0f;
        *v10 = -h;
        *v11 = 0.0f;
        break;
    case GLITTER_PIVOT_MIDDLE_LEFT:
        *v00 = 0.0f;
        *v01 = w;
        *v10 = h * -0.5f;
        *v11 = h * 0.5f;
        break;
    case GLITTER_PIVOT_MIDDLE_CENTER:
    default:
        *v00 = w * -0.5f;
        *v01 = w * 0.5f;
        *v10 = h * -0.5f;
        *v11 = h * 0.5f;
        break;
    case GLITTER_PIVOT_MIDDLE_RIGHT:
        *v00 = -w;
        *v01 = 0.0f;
        *v10 = h * -0.5f;
        *v11 = h * 0.5f;
        break;
    case GLITTER_PIVOT_BOTTOM_LEFT:
        *v00 = 0.0f;
        *v01 = w;
        *v10 = 0.0f;
        *v11 = h;
        break;
    case GLITTER_PIVOT_BOTTOM_CENTER:
        *v00 = w * -0.5f;
        *v01 = w * 0.5f;
        *v10 = 0.0f;
        *v11 = h;
        break;
    case GLITTER_PIVOT_BOTTOM_RIGHT:
        *v00 = -w;
        *v01 = 0.0f;
        *v10 = 0.0f;
        *v11 = h;
        break;
    }
}

static bool GlitterF2RenderGroup_get_ext_anim_scale(GlitterF2RenderGroup* a1, vec3* a2) {
    GlitterEffectInst* effect_inst;

    if (!a1->particle)
        return false;

    if (a1->particle->data.effect)
        effect_inst = a1->particle->data.effect;
    else {
        if (!a1->particle->data.parent)
            return false;

        effect_inst = a1->particle->data.parent->data.effect;
        if (!effect_inst)
            return false;
    }

    if (effect_inst->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE) {
        if (a2) {
            GlitterF2EffectInst* effect_inst_f2 = dynamic_cast<GlitterF2EffectInst*>(effect_inst);
            if (effect_inst_f2)
                *a2 = effect_inst_f2->ext_anim_scale;
        }
        return true;
    }
    return false;
}
