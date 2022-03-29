/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst_x.h"
#include "../draw_task.h"
#include "../gl_state.h"
#include "../render_context.h"
#include "../shader.h"
#include "../shader_ft.h"
#include "../static_var.h"
#include "random_x.h"
#include "render_group_x.h"
#include "render_element_x.h"
#include <glad/glad.h>

static glitter_render_element* GlitterXRenderGroup_add_render_element(GlitterXRenderGroup* a1,
    glitter_render_element* a2);
static void GlitterXRenderGroup_calc_disp_line(GlitterXRenderGroup* a1);
static void GlitterXRenderGroup_calc_disp_locus(GPM, GlitterXRenderGroup* a1);
static void GlitterXRenderGroup_calc_disp_mesh(GPM, GlitterXRenderGroup* a1);
static void GlitterXRenderGroup_calc_disp_quad(GPM, GlitterXRenderGroup* a1);
static void GlitterXRenderGroup_calc_draw_quad_normal(GPM, GlitterXRenderGroup* a1,
    mat4* model_mat, mat4* dir_mat);
static void GlitterXRenderGroup_calc_draw_quad_direction_rotation(GlitterXRenderGroup* a1,
    mat4* model_mat);
static void GlitterXRenderGroup_calc_draw_locus_set_pivot(glitter_pivot pivot,
    float_t w, float_t* v00, float_t* v01);
static void GlitterXRenderGroup_calc_draw_set_pivot(glitter_pivot pivot,
    float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11);
static bool GlitterXRenderGroup_get_ext_anim_scale(GlitterXRenderGroup* a1, vec3* a2);
static bool GlitterXRenderGroup_get_emitter_scale(GlitterXRenderGroup* a1, vec3* scale);

GlitterXRenderGroup::GlitterXRenderGroup(GlitterXParticleInst* a1) : particle(), use_culling() {
    object_name_hash = hash_murmurhash_empty;

    switch (a1->data.data.type) {
    case GLITTER_PARTICLE_QUAD:
        if (a1->data.data.count > 0)
            count = a1->data.data.count;
        else
            count = 2500;
        max_count = 4 * count;
        break;
    case GLITTER_PARTICLE_LINE:
        count = (size_t)a1->data.data.locus_history_size
            + a1->data.data.locus_history_size_random;
        max_count = count;
        break;
    case GLITTER_PARTICLE_LOCUS:
        count = (size_t)a1->data.data.locus_history_size
            + a1->data.data.locus_history_size_random;
        max_count = 2 * count;
        break;
    case GLITTER_PARTICLE_MESH:
        count = 1280;
        max_count = 0;
        break;
    default:
        return;
    }

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

    if (a1->data.data.type != GLITTER_PARTICLE_MESH) {
        glGenBuffers(1, &vbo);
        gl_state_bind_array_buffer(vbo);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(glitter_buffer) * max_count), 0, GL_DYNAMIC_DRAW);
        gl_state_bind_array_buffer(vbo);
    }
    else
        vbo = 0;

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

    static const GLsizei buffer_size = sizeof(glitter_buffer);

    if (!is_quad) {
        vec_key.reserve(count);
        vec_val.reserve(count);
    }
}

GlitterXRenderGroup::~GlitterXRenderGroup()  {
    DeleteBuffers(false);
    free(elements);
}

void GlitterXRenderGroup::CalcDisp(GPM) {
    switch (type) {
    case GLITTER_PARTICLE_MESH:
        break;
    default:
        return;
    }

    disp = 0;
    switch (type) {
    case GLITTER_PARTICLE_MESH:
        GlitterXRenderGroup_calc_disp_mesh(GPM_VAL, this);
        break;
    }
}

bool GlitterXRenderGroup::CannotDisp() {
    GlitterXEffectInst* effect;
    if (!particle)
        return true;
    else if (particle->data.effect)
        return (particle->data.effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else if ((particle = particle->data.parent) && (effect = particle->data.effect))
        return (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else
        return true;
}

void GlitterXRenderGroup::Ctrl(float_t delta_frame, bool copy_mats) {
    if (!particle)
        return;

    GlitterXParticleInstData* data = &particle->data;
    blend_mode = data->data.blend_mode;
    mask_blend_mode = data->data.mask_blend_mode;
    texture = data->data.texture;
    mask_texture = data->data.mask_texture;
    object_name_hash = data->data.mesh.object_name_hash;
    split_u = data->data.split_u;
    split_v = data->data.split_v;
    split_uv = data->data.split_uv;
    type = data->data.type;
    draw_type = data->data.draw_type;
    z_offset = data->data.z_offset;
    pivot = data->data.pivot;
    flags = data->data.flags;

    GlitterXEmitterInst* emitter;
    if (copy_mats && (emitter = particle->data.emitter)) {
        mat = emitter->mat;
        mat_rot = emitter->mat_rot;
    }

    for (size_t ctrl = this->ctrl, i = 0; ctrl; i++) {
        if (!elements[i].alive)
            continue;

        glitter_x_render_element_ctrl(this, &elements[i], delta_frame);
        ctrl--;
    }
    frame += delta_frame;
}

void GlitterXRenderGroup::Disp(GPM) {
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
        GlitterXRenderGroup_calc_disp_quad(GPM_VAL, this);
        break;
    case GLITTER_PARTICLE_LINE:
        GlitterXRenderGroup_calc_disp_line(this);
        break;
    case GLITTER_PARTICLE_LOCUS:
        GlitterXRenderGroup_calc_disp_locus(GPM_VAL, this);
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

    if (blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
        uniform_value[U_ALPHA_BLEND] = alpha != DRAW_PASS_3D_OPAQUE ? 3 : 1;
    else
        uniform_value[U_ALPHA_BLEND] = alpha != DRAW_PASS_3D_OPAQUE ? 2 : 0;

    if (~particle->data.flags & GLITTER_PARTICLE_DEPTH_TEST) {
        gl_state_enable_depth_test();
        gl_state_set_depth_func(GL_LESS);
    }
    else
        gl_state_disable_depth_test();

    if (blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
        gl_state_set_depth_mask(GL_TRUE);
    else
        gl_state_set_depth_mask(GL_FALSE);

    if (draw_type == GLITTER_DIRECTION_BILLBOARD && !use_culling) {
        gl_state_enable_cull_face();
        gl_state_set_cull_face_mode(GL_BACK);
    }
    else
        gl_state_disable_cull_face();

    float_t emission = 1.0f;
    if (flags & GLITTER_PARTICLE_EMISSION || blend_mode == GLITTER_PARTICLE_BLEND_TYPICAL)
        emission = this->emission;

    shader_state_material_set_emission(&shaders_ft, false, emission, emission, emission, 1.0f);
    shader_state_matrix_set_mvp_separate(&shaders_ft,
        &mat_draw, &GPM_VAL->cam_view, &GPM_VAL->cam_projection);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, (mat4*)&mat4_identity);

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
    gl_state_disable_cull_face();
    gl_state_disable_depth_test();
}

void GlitterXRenderGroup::DeleteBuffers(bool a2) {
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

void GlitterXRenderGroup::Emit(GlitterXParticleInstData* a2,
    GlitterXEmitterInst* a3, int32_t dup_count, int32_t count) {
    glitter_render_element* element;
    int64_t i;
    int32_t index;
    uint8_t step;

    step = a3->RandomGetStep();
    glitter_x_random_set_step(random_ptr, 1);
    for (element = 0, i = 0; i < dup_count; i++)
        for (index = 0; index < count; index++, element++) {
            element = GlitterXRenderGroup_add_render_element(this, element);
            if (!element)
                break;

            a3->RandomSetValue();
            glitter_x_render_element_emit(element, a2, a3, index, step, random_ptr);
        }
}

void GlitterXRenderGroup::Free() {
    glitter_render_element* elem = elements;
    for (size_t i = count; i; i--, elem++)
        glitter_x_render_element_free(elem);
    ctrl = 0;
}

static glitter_render_element* GlitterXRenderGroup_add_render_element(GlitterXRenderGroup* a1,
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

static void GlitterXRenderGroup_calc_disp_line(GlitterXRenderGroup* a1) {
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
                count += length;
        }
    }

    if (!count || count > a1->max_count)
        return;

    has_scale = false;
    scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        GlitterXRenderGroup_get_emitter_scale(a1, &scale);
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

static void GlitterXRenderGroup_calc_disp_locus(GPM, GlitterXRenderGroup* a1) {
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
    mat4 mat;
    mat3 model_mat;

    if (!a1->elements || !a1->vbo || a1->ctrl < 1)
        return;

    for (count = 0, i = a1->ctrl, elem = a1->elements; i > 0; i--, elem++) {
        if (!elem->alive)
            continue;

        if (elem->locus_history) {
            size_t length = elem->locus_history->data.size();
            if (length > 1)
                count += 2 * length;
        }
    }

    if (!count || count > a1->max_count)
        return;

    scale = vec3_null;
    x_vec = { 1.0f, 0.0f, 0.0f };
    if (a1->flags & GLITTER_PARTICLE_LOCAL) {
        mat4_mult(&GPM_VAL->cam_inv_view, &a1->mat, &mat);
        mat4_mult(&GPM_VAL->cam_view, &mat, &mat);
        mat4_mult(&mat, &GPM_VAL->cam_inv_view, &a1->mat_draw);
    }
    else
        a1->mat_draw = mat4_identity;

    has_scale = false;
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

        j = 0;
        if (has_scale)
            for (GlitterLocusHistory::Data& hist_data : hist->data) {
                pos = hist_data.translation;
                vec3_sub(pos, elem->base_translation, pos_diff);
                vec3_mult(pos_diff, scale, pos_diff);
                mat3_mult_vec(&model_mat, &pos_diff, &pos_diff);
                vec3_add(pos, pos_diff, pos);

                GlitterXRenderGroup_calc_draw_locus_set_pivot(a1->pivot,
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
                GlitterXRenderGroup_calc_draw_locus_set_pivot(a1->pivot,
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

static void GlitterXRenderGroup_calc_disp_mesh(GPM, GlitterXRenderGroup* a1) {
    vec3 up_vec;
    vec3 ext_anim_scale;

    size_t i;
    size_t j;
    size_t j_max;
    glitter_render_element* elem;
    size_t disp;
    bool billboard;
    bool emitter_rotation;
    bool has_scale;
    bool emitter_local;
    bool draw_local;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    vec3 emit_scale;
    mat4 uv_mat[2];
    mat4 model_mat;
    mat4 dir_mat;
    mat4 view_mat;
    mat4 inv_view_mat;
    mat4 mat;
    void(*rotate_func)(mat4*, GlitterXRenderGroup*, glitter_render_element*, vec3*, vec3*);

    if (a1->object_name_hash == hash_murmurhash_empty || a1->object_name_hash == 0xFFFFFFFF)
        return;

    has_scale = false;
    emitter_local = false;
    emit_scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        model_mat = a1->mat;
        mat4_normalize_rotation(&model_mat, &view_mat);
        mat4_mult(&view_mat, &GPM_VAL->cam_view, &view_mat);
        mat4_inverse(&view_mat, &inv_view_mat);

        emitter_local = true;
        if (a1->flags & GLITTER_PARTICLE_SCALE) {
            GlitterXRenderGroup_get_emitter_scale(a1, &emit_scale);
            if (fabsf(emit_scale.x) > 0.000001f || fabsf(emit_scale.y) > 0.000001f || fabsf(emit_scale.z) > 0.000001f)
                has_scale = true;
        }
    }
    else {
        model_mat = mat4_identity;
        view_mat = GPM_VAL->cam_view;
        inv_view_mat = GPM_VAL->cam_inv_view;
    }

    draw_local = false;
    if (a1->flags & GLITTER_PARTICLE_LOCAL) {
        mat4_mult(&inv_view_mat, &a1->mat, &inv_view_mat);
        mat4_mult(&view_mat, &inv_view_mat, &view_mat);
        mat4_inverse(&view_mat, &inv_view_mat);
        draw_local = true;
    }
    mat4_mult(&view_mat, &GPM_VAL->cam_inv_view, &a1->mat_draw);

    dir_mat = mat4_identity;
    up_vec = { 0.0f, 0.0f, 1.0f };
    rotate_func = 0;
    billboard = false;
    emitter_rotation = false;
    switch (a1->draw_type) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat4_clear_trans(&model_mat, &dir_mat);
        mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
        mat4_clear_trans(&dir_mat, &dir_mat);
        billboard = true;
        break;
    case GLITTER_DIRECTION_EMITTER_DIRECTION:
        dir_mat = a1->mat_rot;
        break;
    case GLITTER_DIRECTION_PREV_POSITION:
    case GLITTER_DIRECTION_PREV_POSITION_DUP:
        rotate_func = glitter_x_render_element_rotate_mesh_to_prev_position;
        break;
    case GLITTER_DIRECTION_EMIT_POSITION:
        rotate_func = glitter_x_render_element_rotate_mesh_to_emit_position;
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_AXIS:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    case GLITTER_DIRECTION_EMITTER_ROTATION:
        emitter_rotation = true;
        break;
    }

    if (GlitterXRenderGroup_get_ext_anim_scale(a1, &ext_anim_scale)) {
        if (!has_scale) {
            emit_scale = vec3_identity;
            has_scale = true;
        }
        vec3_add(emit_scale, ext_anim_scale, emit_scale);
    }

    render_context* rctx = (render_context*)GPM_VAL->rctx;
    object_data* object_data = &rctx->object_data;

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
            trans = elem->translation;
            rot = elem->rotation;
            vec3_mult_scalar(elem->scale, elem->scale_all, scale);
            if (has_scale)
                vec3_mult(scale, emit_scale, scale);

            if (emitter_local)
                mat4_mult_vec3_trans(&model_mat, &trans, &trans);

            if (billboard) {
                if (draw_local)
                    mat = mat4_identity;
                else
                    mat = dir_mat;
            }
            else if (rotate_func)
                rotate_func(&mat, a1, elem, &up_vec, &trans);
            else if (emitter_rotation)
                mat = elem->mat;
            else
                mat = dir_mat;

            mat4_set_translation(&mat, &trans);
            mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
            mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);

            mat4_translate(elem->uv_scroll.x, -elem->uv_scroll.y, 0.0f, &uv_mat[0]);
            mat4_translate(elem->uv_scroll_2nd.x, -elem->uv_scroll_2nd.y, 0.0f, &uv_mat[1]);

            glitter_particle* particle = a1->particle->data.particle;
            object_info object_info;
            object_info.set_id = (uint32_t)particle->data.mesh.object_set_name_hash;
            object_info.id = (uint32_t)particle->data.mesh.object_name_hash;
            obj* obj = object_storage_get_obj(object_info);
            if (!obj)
                continue;

            object_data->texture_pattern_count = 0;
            int32_t ttc = 0;
            texture_transform_struct* tt = object_data->texture_transform_array;
            for (int32_t i = 0; i < obj->meshes_count; i++) {
                obj_mesh* mesh = &obj->meshes[i];
                for (int32_t j = 0; j < mesh->sub_meshes_count; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->sub_meshes[j];
                    obj_material* material = &obj->materials[sub_mesh->material_index].material;

                    for (int32_t k = 0, l = 0; k < 8; k++) {
                        obj_material_texture* texture = &material->textures[k];
                        if (texture->texture_flags.type != OBJ_MATERIAL_TEXTURE_COLOR)
                            continue;

                        tt->id = texture->texture_id;
                        tt->mat = uv_mat[l];
                        ttc++;
                        tt++;

                        if (++l == 2 || ttc == 24)
                            break;
                    }

                    if (ttc == 24)
                        break;
                }

                if (ttc == 24)
                    break;
            }
            object_data->texture_transform_count = ttc;

            if (a1->flags & GLITTER_PARTICLE_LOCAL)
                mat4_mult(&mat, &GPM_VAL->cam_inv_view, &mat);

            vec4u color = elem->color;
            if (draw_task_add_draw_object_by_object_info_color(rctx, &mat,
                object_info, color.x, color.y, color.z, color.w))
                disp++;
            elem->mat_draw = mat;

            object_data->texture_pattern_count = 0;
            object_data->texture_transform_count = 0;
        }
    }
    a1->disp = disp;
}

static void GlitterXRenderGroup_calc_disp_quad(GPM, GlitterXRenderGroup* a1) {
    mat4 model_mat;
    mat4 dir_mat;
    mat4 view_mat;
    mat4 inv_view_mat;

    if (!a1->elements || !a1->vbo || a1->ctrl < 1)
        return;

    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        model_mat = a1->mat;
        if (a1->draw_type == GLITTER_DIRECTION_EMITTER_ROTATION
            || a1->draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION)
            mat4_clear_rot(&a1->mat, &view_mat);
        else
            mat4_normalize_rotation(&a1->mat, &view_mat);
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
        dir_mat = a1->mat_rot;
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
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
        GlitterXRenderGroup_calc_draw_quad_direction_rotation(a1, &model_mat);
        break;
    default:
        GlitterXRenderGroup_calc_draw_quad_normal(GPM_VAL, a1, &model_mat, &dir_mat);
        break;
    }
}

static void GlitterXRenderGroup_calc_draw_quad_normal(GPM,
    GlitterXRenderGroup* a1, mat4* model_mat, mat4* dir_mat) {
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
        GlitterXRenderGroup_get_emitter_scale(a1, &scale);
        if (a1->flags & GLITTER_PARTICLE_SCALE) {
            x_vec.x = scale.x;
            y_vec.y = scale.y;
        }
        if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        emitter_local = true;
    }

    if (GlitterXRenderGroup_get_ext_anim_scale(a1, &ext_scale)) {
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
                if (has_scale)
                    vec3_mult(pos, scale, pos);

                GlitterXRenderGroup_calc_draw_set_pivot(a1->pivot,
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
                if (has_scale)
                    vec3_mult(pos, scale, pos);

                GlitterXRenderGroup_calc_draw_set_pivot(a1->pivot,
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

static void GlitterXRenderGroup_calc_draw_quad_direction_rotation(GlitterXRenderGroup* a1,
    mat4* model_mat) {
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
    mat3 mat;
    mat4 inv_model_mat;
    vec3 scale;
    bool use_scale;
    void(* rotate_func)(mat3*, GlitterXRenderGroup*, glitter_render_element*, vec3*);

    mat4_inverse(model_mat, &inv_model_mat);
    mat4_clear_trans(&inv_model_mat, &inv_model_mat);

    x_vec_base = { 1.0f, 0.0f, 0.0f };
    y_vec_base = { 0.0f, 1.0f, 0.0f };

    mat4_mult_vec3(&inv_model_mat, &x_vec_base, &x_vec_base);
    mat4_mult_vec3(&inv_model_mat, &y_vec_base, &y_vec_base);

    if (a1->draw_type == GLITTER_DIRECTION_EMIT_POSITION) {
        up_vec = { 0.0f, 0.0f, 1.0f };
        rotate_func = glitter_x_render_element_rotate_to_emit_position;
    }
    else {
        up_vec = { 0.0f, 1.0f, 0.0f };
        rotate_func = glitter_x_render_element_rotate_to_prev_position;
    }

    scale = vec3_identity;

    use_scale = false;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL
        && GlitterXRenderGroup_get_emitter_scale(a1, &scale))
            use_scale = a1->flags & GLITTER_PARTICLE_SCALE ? true : false;

    gl_state_bind_array_buffer(a1->vbo);
    buf = (glitter_buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
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

            if (use_scale)
                vec3_mult(pos, scale, pos);

            GlitterXRenderGroup_calc_draw_set_pivot(a1->pivot,
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
            mat3_mult_vec(&mat, &x_vec_base, &x_vec);
            mat3_mult_vec(&mat, &y_vec_base, &y_vec);

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

static void GlitterXRenderGroup_calc_draw_locus_set_pivot(glitter_pivot pivot,
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

static void GlitterXRenderGroup_calc_draw_set_pivot(glitter_pivot pivot,
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

static bool GlitterXRenderGroup_get_ext_anim_scale(GlitterXRenderGroup* a1, vec3* a2) {
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
            GlitterXEffectInst* effect_inst_x = dynamic_cast<GlitterXEffectInst*>(effect_inst);
            if (effect_inst_x)
                *a2 = effect_inst_x->ext_anim_scale;
        }
        return true;
    }
    return false;
}

static bool GlitterXRenderGroup_get_emitter_scale(GlitterXRenderGroup* a1, vec3* scale) {
    *scale = vec3_identity;

    GlitterXParticleInst* particle = a1->particle;
    if (particle) {
        GlitterXEmitterInst* emitter = particle->data.emitter;
        if (emitter) {
            vec3_mult_scalar(emitter->scale, emitter->scale_all, *scale);

            float_t length;
            vec3_length(*scale, length);
            return length > 0.000001f;
        }
    }
    return true;
}