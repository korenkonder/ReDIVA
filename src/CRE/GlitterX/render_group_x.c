/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst_x.h"
#include "random_x.h"
#include "render_group_x.h"
#include "render_element_x.h"
#include "../shader.h"
#include "../shared.h"
#include "../static_var.h"

extern shader_fbo particle_shader;

static glitter_render_element* FASTCALL glitter_x_render_group_add_render_element(glitter_render_group* a1,
    glitter_render_element* a2);
static void FASTCALL glitter_x_render_group_calc_draw_line(glitter_render_group* a1);
static void FASTCALL glitter_x_render_group_calc_draw_locus(GPM, glitter_render_group* a1);
static void FASTCALL glitter_x_render_group_calc_draw_mesh(GPM, glitter_render_group* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*));
static void FASTCALL glitter_x_render_group_calc_draw_quad(GPM, glitter_render_group* a1);
static void FASTCALL glitter_x_render_group_calc_draw_quad_normal(GPM, glitter_render_group* a1,
    mat4* model_mat, mat4* dir_mat);
static void FASTCALL glitter_x_render_group_calc_draw_quad_direction_rotation(glitter_render_group* a1,
    mat4* model_mat);
static void FASTCALL glitter_x_render_group_calc_draw_set_pivot(glitter_pivot pivot,
    float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11);
static bool FASTCALL glitter_x_render_group_get_ext_anim_scale(glitter_render_group* a1, vec3* a2);
static bool FASTCALL glitter_x_render_group_get_emitter_scale(glitter_render_group* a1, vec3* scale);

glitter_render_group* FASTCALL glitter_x_render_group_init(glitter_particle_inst* a1) {
    size_t count = 0;
    size_t max_count = 0;

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
        return 0;
    }

    glitter_render_group* rg = force_malloc(sizeof(glitter_render_group));
    rg->split_u = 1;
    rg->split_v = 1;
    rg->split_uv = vec2_identity;
    rg->count = count;
    rg->object_name_hash = hash_murmurhash_empty;
    rg->mat = mat4_identity;
    rg->mat_ext_anim = mat4_identity;
    rg->mat_draw = mat4_identity;
    rg->max_count = max_count;
    rg->particle = a1;
    rg->alpha = 1;
    rg->emission = 1.0f;
    rg->blend_mode0 = GLITTER_PARTICLE_BLEND_TYPICAL;
    rg->blend_mode1 = GLITTER_PARTICLE_BLEND_TYPICAL;
    rg->random_ptr = a1->data.random_ptr;
    rg->buffer = 0;
    rg->vao = 0;
    rg->vbo = 0;
    rg->ebo = 0;
    rg->vec_key = (vector_int32_t){ 0, 0, 0 };
    rg->vec_val = (vector_int32_t){ 0, 0, 0 };

    rg->elements = force_malloc_s(glitter_render_element, rg->count);
    if (!rg->elements) {
        rg->count = 0;
        rg->max_count = 0;
        return rg;
    }
    else if (!rg->max_count)
        return rg;

    bool is_quad = a1->data.data.type == GLITTER_PARTICLE_QUAD;

    rg->buffer = force_malloc_s(glitter_buffer, rg->max_count);
    if (!rg->buffer)
        rg->max_count = 0;

    glGenVertexArrays(1, &rg->vao);
    glGenBuffers(1, &rg->vbo);

    if (is_quad) {
        size_t count = rg->max_count / 4 * 6;
        int32_t* ebo_data = force_malloc_s(int32_t, count);
        for (size_t i = 0, j = 0; i < count; i += 6, j += 4) {
            ebo_data[i] = (int32_t)j;
            ebo_data[i + 1] = (int32_t)(j + 1);
            ebo_data[i + 2] = (int32_t)(j + 2);
            ebo_data[i + 3] = (int32_t)(j + 0);
            ebo_data[i + 4] = (int32_t)(j + 2);
            ebo_data[i + 5] = (int32_t)(j + 3);
        }
        glGenBuffers(1, &rg->ebo);
        bind_element_array_buffer(rg->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * count, ebo_data, GL_STATIC_DRAW);
        bind_element_array_buffer(0);
        free(ebo_data);
    }

    static const GLsizei buffer_size = sizeof(glitter_buffer);

    bind_vertex_array(rg->vao);
    bind_array_buffer(rg->vbo);
    if (is_quad)
        bind_element_array_buffer(rg->ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(glitter_buffer, position)); // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(glitter_buffer, uv));       // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(glitter_buffer, color));    // Color
    bind_vertex_array(0);
    bind_array_buffer(0);
    if (is_quad)
        bind_element_array_buffer(0);

    if (!is_quad) {
        vector_int32_t_append(&rg->vec_key, rg->count);
        vector_int32_t_append(&rg->vec_val, rg->count);
    }
    rg->update_data = true;
    return rg;
}

void FASTCALL glitter_x_render_group_calc_draw(GPM, glitter_render_group* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    switch (a1->type) {
    case GLITTER_PARTICLE_QUAD:
    case GLITTER_PARTICLE_LINE:
    case GLITTER_PARTICLE_LOCUS:
        if (!a1->update_data && !GPM_VAL->updated)
            return;
        break;
    case GLITTER_PARTICLE_MESH:
        break;
    default:
        a1->update_data = false;
        return;
    }

    a1->update_data = false;
    a1->disp = 0;
    switch (a1->type) {
    case GLITTER_PARTICLE_QUAD:
        glitter_x_render_group_calc_draw_quad(GPM_VAL, a1);
        break;
    case GLITTER_PARTICLE_LINE:
        glitter_x_render_group_calc_draw_line(a1);
        break;
    case GLITTER_PARTICLE_LOCUS:
        glitter_x_render_group_calc_draw_locus(GPM_VAL, a1);
        break;
    case GLITTER_PARTICLE_MESH:
        glitter_x_render_group_calc_draw_mesh(GPM_VAL, a1, render_add_list_func);
        break;
    }
}

bool FASTCALL glitter_x_render_group_cannot_draw(glitter_render_group* a1) {
    glitter_effect_inst* effect;
    glitter_particle_inst* particle;

    if (!(particle = a1->particle))
        return true;
    else if (particle->data.effect)
        return (particle->data.effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else if ((particle = particle->data.parent) && (effect = particle->data.effect))
        return (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
    else
        return true;
}

void FASTCALL glitter_x_render_group_draw(glitter_render_group* a1) {
    int32_t glitter_shader_flags[4];

    if (a1->disp < 1)
        return;

    size_t disp = 0;
    switch (a1->type) {
    case GLITTER_PARTICLE_QUAD:
        disp = a1->disp * 4;
        break;
    case GLITTER_PARTICLE_LINE:
        disp = a1->disp;
        break;
    case GLITTER_PARTICLE_LOCUS:
        disp = a1->disp * 2;
        break;
    default:
        return;
    }

    glEnablei(GL_BLEND, 0);
    switch (a1->blend_mode0) {
    case GLITTER_PARTICLE_BLEND_ADD:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
        break;
    case GLITTER_PARTICLE_BLEND_MULTIPLY:
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    default:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    float_t emission;
    if (a1->flags & GLITTER_PARTICLE_EMISSION || a1->blend_mode0 == GLITTER_PARTICLE_BLEND_TYPICAL)
        emission = a1->emission;
    else
        emission = 1.0f;

    if (a1->type != GLITTER_PARTICLE_LINE && a1->texture0) {
        bind_index_tex2d(0, a1->texture0);
        if (a1->texture1) {
            bind_index_tex2d(1, a1->texture1);

            glitter_shader_flags[0] = 2;
            switch (a1->blend_mode1) {
            default:
                glitter_shader_flags[1] = 0;
                break;
            case GLITTER_PARTICLE_BLEND_MULTIPLY:
                glitter_shader_flags[1] = 1;
                break;
            case GLITTER_PARTICLE_BLEND_ADD:
                glitter_shader_flags[1] = 2;
                break;
            }
        }
        else {
            bind_index_tex2d(1, 0);
            glitter_shader_flags[0] = 1;
            glitter_shader_flags[1] = 0;
        }
    }
    else {
        bind_index_tex2d(0, 0);
        bind_index_tex2d(1, 0);
        glitter_shader_flags[0] = 0;
        glitter_shader_flags[1] = 0;
    }

    switch (a1->fog) {
    default:
        glitter_shader_flags[2] = 0;
        break;
    case 1:
        glitter_shader_flags[2] = 1;
        break;
    case 2:
        glitter_shader_flags[2] = 2;
        break;
    }

    if (a1->blend_mode0 == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
        glitter_shader_flags[3] = a1->alpha ? 3 : 1;
    else
        glitter_shader_flags[3] = a1->alpha ? 2 : 0;

    if (~a1->particle->data.flags & GLITTER_PARTICLE_DEPTH_TEST) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
        glDisable(GL_DEPTH_TEST);

    if (a1->blend_mode0 == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
        glDepthMask(true);
    else
        glDepthMask(false);

    if (a1->draw_type == GLITTER_DIRECTION_BILLBOARD && !a1->use_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
        glDisable(GL_CULL_FACE);

    shader_fbo_use(&particle_shader);
    shader_fbo_set_int_array(&particle_shader, "mode", 4, glitter_shader_flags);
    shader_fbo_set_float(&particle_shader, "emission", emission);
    shader_fbo_set_mat4(&particle_shader, "model", false, &a1->mat_draw);

    bind_array_buffer(a1->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(glitter_buffer) * disp), a1->buffer, GL_DYNAMIC_DRAW);
    bind_array_buffer(0);

    bind_vertex_array(a1->vao);
    switch (a1->type) {
    case GLITTER_PARTICLE_QUAD:
        glDrawElements(GL_TRIANGLES, (GLsizei)(6 * a1->disp), GL_UNSIGNED_INT, 0);
        break;
    case GLITTER_PARTICLE_LINE:
    case GLITTER_PARTICLE_LOCUS: {
        const GLenum mode = a1->type == GLITTER_PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
        const size_t count = a1->vec_key.end - a1->vec_key.begin;
        for (size_t i = 0; i < count; i++)
            glDrawArrays(mode, a1->vec_key.begin[i], a1->vec_val.begin[i]);
    } break;
    }
    bind_vertex_array(0);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    shader_fbo_use(0);
}

void FASTCALL glitter_x_render_group_delete_buffers(glitter_render_group* a1, bool a2) {
    if (a1->particle) {
        if (!a2)
            a1->particle->data.render_group = 0;
        a1->particle = 0;
    }

    if (a1->ebo) {
        glDeleteBuffers(1, &a1->ebo);
        a1->ebo = 0;
    }

    if (a1->vbo) {
        glDeleteBuffers(1, &a1->vbo);
        a1->vbo = 0;
    }

    if (a1->vao) {
        glDeleteVertexArrays(1, &a1->vao);
        a1->vao = 0;
    }
    free(a1->buffer);

    if (!a2 && a1->elements) {
        glitter_x_render_group_free(a1);
        free(a1->elements);
    }
}

void FASTCALL glitter_x_render_group_emit(glitter_render_group* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3,
    int32_t dup_count, int32_t count, float_t frame) {
    glitter_render_element* element;
    int64_t i;
    int32_t index;
    uint8_t step;

    step = glitter_x_emitter_inst_random_get_step(a3);
    glitter_x_random_set_step(a1->random_ptr, 1);
    for (element = 0, i = 0; i < dup_count; i++)
        for (index = 0; index < count; index++, element++) {
            element = glitter_x_render_group_add_render_element(a1, element);
            if (!element)
                break;

            glitter_x_emitter_inst_random_set_value(a3);
            glitter_x_render_element_emit(element, a2, a3, index, step, a1->random_ptr);
            element->frame = frame;
        }
}

void FASTCALL glitter_x_render_group_free(glitter_render_group* a1) {
    glitter_render_element* sub;
    size_t i;

    sub = a1->elements;
    for (i = 0; i < a1->count; i++, sub++)
        glitter_x_render_element_free(sub);
    a1->ctrl = 0;
}

void FASTCALL glitter_x_render_group_update(glitter_render_group* rg,
    float_t delta_frame, bool copy_mats) {
    glitter_particle_inst* particle_inst;
    glitter_particle_inst_data* data;
    glitter_emitter_inst* emitter;
    size_t ctrl;
    size_t i;

    if (!rg->particle)
        return;

    particle_inst = rg->particle;
    data = &particle_inst->data;
    rg->blend_mode0 = data->data.blend_mode0;
    rg->blend_mode1 = data->data.blend_mode1;
    rg->texture0 = data->data.texture0;
    rg->texture1 = data->data.texture1;
    rg->object_name_hash = data->data.mesh.object_file_hash;
    rg->split_u = data->data.split_u;
    rg->split_v = data->data.split_v;
    rg->split_uv = data->data.split_uv;
    rg->type = data->data.type;
    rg->draw_type = data->data.draw_type;
    rg->z_offset = data->data.z_offset;
    rg->pivot = data->data.pivot;
    rg->flags = data->data.flags;

    if (copy_mats && (emitter = particle_inst->data.emitter)) {
        rg->mat = emitter->mat;
        rg->mat_ext_anim = emitter->mat_ext_anim;
    }

    for (ctrl = rg->ctrl, i = 0; ctrl > 0; i++) {
        if (!rg->elements[i].alive)
            continue;

        glitter_x_render_element_update(rg, &rg->elements[i], delta_frame);
        ctrl--;
    }
    rg->frame += delta_frame;
    rg->update_data |= delta_frame != 0.0f;
}

void FASTCALL glitter_x_render_group_dispose(glitter_render_group* rg) {
    vector_int32_t_free(&rg->vec_key);
    vector_int32_t_free(&rg->vec_val);
    glitter_x_render_group_delete_buffers(rg, false);
    free(rg);
}

static glitter_render_element* FASTCALL glitter_x_render_group_add_render_element(glitter_render_group* a1,
    glitter_render_element* a2) {
    size_t v3;
    size_t v4;

    if (!a2)
        a2 = a1->elements;

    v3 = a1->count - (a2 - a1->elements);
    v4 = 0;
    if (v3 <= 0)
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

static void FASTCALL glitter_x_render_group_calc_draw_line(glitter_render_group* a1) {
    size_t count;
    size_t i;
    size_t j;
    vec3 scale;
    vec3 pos;
    vec3 pos_diff;
    bool has_scale_x;
    bool has_scale_y;
    bool has_scale_z;
    bool has_scale;
    glitter_buffer* buf;
    size_t disp;
    glitter_render_element* elem;
    glitter_locus_history* hist;
    glitter_locus_history_data* hist_data;
    size_t index;
    uint32_t temp;

    if (!a1->elements || !a1->buffer || a1->ctrl <= 0)
        return;

    for (count = 0, i = a1->ctrl, j = 0, elem = a1->elements; j < i; j++, elem++) {
        if (!elem->alive)
            continue;

        if (elem->locus_history) {
            size_t length = elem->locus_history->data.end - elem->locus_history->data.begin;
            if (length > 1)
                count += length;
        }
        i--;
    }

    if (!count || count > a1->max_count)
        return;

    scale = vec3_null;
    has_scale_x = false;
    has_scale_y = false;
    has_scale_z = false;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        glitter_x_render_group_get_emitter_scale(a1, &scale);
        if (!(has_scale_x = fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale_y = fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale_z = fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        mat4_normalize_rotation(&a1->mat, &a1->mat_draw);
    }
    else
        a1->mat_draw = mat4_identity;

    has_scale = has_scale_x || has_scale_y || has_scale_z;

    buf = a1->buffer;
    elem = a1->elements;
    disp = 0;
    vector_int32_t_clear(&a1->vec_key);
    vector_int32_t_clear(&a1->vec_val);
    for (i = a1->ctrl, index = 0; i > 0; elem++) {
        if (!elem->alive)
            continue;

        i--;
        hist = elem->locus_history;
        if (!elem->draw || !hist || hist->data.end - hist->data.begin < 2)
            continue;

        if (has_scale)
            for (j = 0, hist_data = hist->data.begin; hist_data != hist->data.end; j++, buf++, hist_data++) {
                pos = hist_data->translation;
                vec3_sub(pos, elem->base_translation, pos_diff);
                vec3_mult(pos_diff, scale, pos_diff);
                vec3_add(pos, pos_diff, buf->position);
                buf->uv = vec2_null;
                buf->color = *(vec3*)&hist_data->color;
                buf->alpha = hist_data->color.w;
            }
        else
            for (j = 0, hist_data = hist->data.begin; hist_data != hist->data.end; j++, buf++, hist_data++) {
                buf->position = hist_data->translation;
                buf->uv = vec2_null;
                buf->color = *(vec3*)&hist_data->color;
                buf->alpha = hist_data->color.w;
            }

        if (j > 0) {
            disp += j;
            temp = (uint32_t)index;
            vector_int32_t_push_back(&a1->vec_key, &temp);
            temp = (uint32_t)j;
            vector_int32_t_push_back(&a1->vec_val, &temp);
            index += j;
        }
    }
    a1->disp = disp;
}

static void FASTCALL glitter_x_render_group_calc_draw_locus(GPM, glitter_render_group* a1) {
    size_t i;
    size_t j;
    size_t count;
    vec3 scale;
    bool has_scale_x;
    bool has_scale_y;
    bool has_scale_z;
    bool has_scale;
    glitter_render_element* elem;
    glitter_locus_history* hist;
    float_t uv_u;
    float_t uv_u_2nd;
    float_t uv_v_2nd;
    float_t uv_v_scale;
    glitter_buffer* buf;
    size_t disp;
    vec3 pos;
    vec3 pos_diff;
    size_t index;
    uint32_t temp;
    float_t v00;
    float_t v01;
    float_t v10;
    float_t v11;
    vec3 x_vec;
    glitter_locus_history_data* hist_data;
    mat4 mat;
    mat3 model_mat;

    if (!a1->elements || !a1->buffer || a1->ctrl <= 0)
        return;

    for (count = 0, i = a1->ctrl, j = 0, elem = a1->elements; j < i; j++, elem++) {
        if (!elem->alive)
            continue;

        if (elem->locus_history) {
            size_t length = elem->locus_history->data.end - elem->locus_history->data.begin;
            if (length > 1)
                count += 2 * length;
        }
        i--;
    }

    if (!count || count > a1->max_count)
        return;

    scale = vec3_null;
    x_vec = (vec3){ 1.0f, 0.0f, 0.0f };
    if (a1->flags & GLITTER_PARTICLE_LOCAL) {
        mat4_mult(&GPM_VAL->cam_inv_view, &a1->mat, &mat);
        mat4_mult(&GPM_VAL->cam_view, &mat, &mat);
        mat4_mult(&mat, &GPM_VAL->cam_inv_view, &a1->mat_draw);
    }
    else
        a1->mat_draw = mat4_identity;

    has_scale_x = false;
    has_scale_y = false;
    has_scale_z = false;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        mat4_get_scale(&a1->mat, &scale);
        if (a1->flags & GLITTER_PARTICLE_SCALE)
            x_vec.x = scale.x;
        vec3_sub(scale, vec3_identity, scale);
        if (!(has_scale_x = fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale_y = fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale_z = fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        mat4_normalize_rotation(&a1->mat, &mat);
        mat3_from_mat4(&mat, &model_mat);
    }
    else
        model_mat = mat3_identity;

    has_scale = has_scale_x || has_scale_y || has_scale_z;

    mat3_mult_vec(&GPM_VAL->cam_inv_view_mat3, &x_vec, &x_vec);

    buf = a1->buffer;
    elem = a1->elements;
    disp = 0;
    vector_int32_t_clear(&a1->vec_key);
    vector_int32_t_clear(&a1->vec_val);
    for (i = a1->ctrl, index = 0; i > 0; elem++) {
        if (!elem->alive)
            continue;

        i--;
        hist = elem->locus_history;
        if (!elem->draw || !hist || hist->data.end - hist->data.begin < 2)
            continue;

        uv_u = elem->uv.x + elem->uv_scroll.x;
        uv_u_2nd = elem->uv.x + elem->uv_scroll.x + a1->split_uv.x;
        uv_v_2nd = elem->uv.y + elem->uv_scroll.y + a1->split_uv.y;
        uv_v_scale = a1->split_uv.y / (float_t)(hist->data.end - hist->data.begin - 1);

        uv_v_2nd = 1.0f - uv_v_2nd;

        for (j = 0, hist_data = hist->data.begin; hist_data != hist->data.end; j++, buf += 2, hist_data++) {
            pos = hist_data->translation;
            if (has_scale) {
                vec3_sub(pos, elem->base_translation, pos_diff);
                vec3_mult(pos_diff, scale, pos_diff);
                mat3_mult_vec(&model_mat, &pos_diff, &pos_diff);
                vec3_add(pos, pos_diff, pos);
            }

            glitter_x_render_group_calc_draw_set_pivot(a1->pivot,
                hist_data->scale * elem->scale.x * elem->scale_all,
                elem->scale.y * elem->scale_particle.y * elem->scale_all,
                &v00, &v01, &v10, &v11);

            vec3_mult_scalar(x_vec, v00, buf[0].position);
            vec3_add(buf[0].position, pos, buf[0].position);
            buf[0].uv.x = uv_u;
            buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
            buf[0].color = *(vec3*)&hist_data->color;
            buf[0].alpha = hist_data->color.w;

            vec3_mult_scalar(x_vec, v01, buf[1].position);
            vec3_add(buf[1].position, pos, buf[1].position);
            buf[1].uv.x = uv_u_2nd;
            buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
            buf[1].color = *(vec3*)&hist_data->color;
            buf[1].alpha = hist_data->color.w;
        }

        if (j > 0) {
            disp += j;
            temp = (uint32_t)index;
            vector_int32_t_push_back(&a1->vec_key, &temp);
            temp = (uint32_t)(j * 2);
            vector_int32_t_push_back(&a1->vec_val, &temp);
            index += j * 2;
        }
    }
    a1->disp = disp;
}

static void FASTCALL glitter_x_render_group_calc_draw_mesh(GPM, glitter_render_group* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
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
    void(FASTCALL* rotate_func)(mat4*, glitter_render_group*, glitter_render_element*, vec3*);

    if (a1->object_name_hash == hash_murmurhash_empty || a1->object_name_hash == 0xFFFFFFFF)
        return;

    elem = a1->elements;
    emit_scale = vec3_null;

    emitter_local = false;
    has_scale = false;

    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        model_mat = a1->mat;
        mat4_normalize_rotation(&a1->mat, &view_mat);
        mat4_mult(&view_mat, &GPM_VAL->cam_view, &view_mat);
        mat4_inverse(&view_mat, &inv_view_mat);

        emitter_local = true;
        if (a1->flags & GLITTER_PARTICLE_SCALE) {
            glitter_x_render_group_get_emitter_scale(a1, &emit_scale);
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

    dir_mat = mat4_identity;
    up_vec = (vec3){ 0.0f, 0.0f, 1.0f };
    rotate_func = 0;
    billboard = false;
    emitter_rotation = false;
    switch (a1->draw_type) {
    case GLITTER_DIRECTION_BILLBOARD:
        dir_mat = model_mat;
        dir_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
        dir_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        billboard = true;
        break;
    case GLITTER_DIRECTION_EMITTER_DIRECTION:
        dir_mat = a1->mat_ext_anim;
        break;
    case GLITTER_DIRECTION_PREV_POSITION:
    case GLITTER_DIRECTION_PREV_POSITION_DUP:
        rotate_func = glitter_x_render_element_rotate_mesh_to_prev_position;
        break;
    case GLITTER_DIRECTION_POSITION:
        rotate_func = glitter_x_render_element_rotate_mesh_to_position;
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_ONLY:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    case GLITTER_DIRECTION_EMITTER_ROTATION:
        emitter_rotation = true;
        break;
    }

    if (glitter_x_render_group_get_ext_anim_scale(a1, &ext_anim_scale)) {
        if (!has_scale) {
            emit_scale = vec3_identity;
            has_scale = true;
        }
        vec3_add(emit_scale, ext_anim_scale, emit_scale);
    }

    elem = a1->elements;
    disp = 0;
    for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
        j_max = min(i, j_max);
        for (j = j_max; j > 0; elem++) {
            if (!elem->alive)
                continue;
            j--;

            if (!elem->draw)
                continue;
            trans = elem->translation;
            rot = elem->rotation;
            vec3_mult_scalar(elem->scale, elem->scale_all, scale);
            if (has_scale)
                vec3_mult(scale, emit_scale, scale);

            if (emitter_local)
                mat4_mult_vec3_trans(&model_mat, &trans, &trans);

            mat = mat4_identity;
            if (billboard) {
                if (draw_local)
                    mat = mat4_identity;
                else
                    mat = dir_mat;
            }
            else if (rotate_func) {
                mat4_set_translation(&mat, &trans);
                rotate_func(&mat, a1, elem, &up_vec);
            }
            else if (emitter_rotation)
                mat = elem->mat;

            mat4_set_translation(&mat, &trans);
            mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
            mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);

            mat4_translate(elem->uv_scroll.x, elem->uv_scroll.y, 0.0f, &uv_mat[0]);
            mat4_translate(elem->uv_scroll_2nd.x, elem->uv_scroll_2nd.y, 0.0f, &uv_mat[1]);
            render_add_list_func(&a1->particle->data.data.mesh, &elem->color, &mat, uv_mat);
            disp++;
        }
    }
    a1->disp = disp;
}

static void FASTCALL glitter_x_render_group_calc_draw_quad(GPM, glitter_render_group* a1) {
    mat4 model_mat;
    mat4 dir_mat;
    mat4 view_mat;
    mat4 inv_view_mat;

    if (!a1->elements || !a1->buffer || a1->ctrl <= 0)
        return;

    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        model_mat = a1->mat;
        mat4_normalize_rotation(&a1->mat, &view_mat);
        if (a1->draw_type == GLITTER_DIRECTION_EMITTER_ROTATION
            || a1->draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION)
            mat4_clear_rot(&view_mat, &view_mat);
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
        dir_mat = model_mat;
        dir_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
        dir_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        break;
    case GLITTER_DIRECTION_EMITTER_DIRECTION:
    case GLITTER_DIRECTION_EMITTER_ROTATION:
    case GLITTER_DIRECTION_PARTICLE_ROTATION:
        dir_mat = a1->mat_ext_anim;
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_ONLY:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    default:
        dir_mat = mat4_identity;
        break;
    }

    switch (a1->draw_type) {
    case GLITTER_DIRECTION_PREV_POSITION:
    case GLITTER_DIRECTION_PREV_POSITION_DUP:
        glitter_x_render_group_calc_draw_quad_direction_rotation(a1, &model_mat);
        break;
    case GLITTER_DIRECTION_POSITION:
        glitter_x_render_group_calc_draw_quad_direction_rotation(a1, &model_mat);
        break;
    default:
        glitter_x_render_group_calc_draw_quad_normal(GPM_VAL, a1, &model_mat, &dir_mat);
        break;
    }
}

static void FASTCALL glitter_x_render_group_calc_draw_quad_normal(GPM,
    glitter_render_group* a1, mat4* model_mat, mat4* dir_mat) {
    size_t i;
    size_t j;
    size_t j_max;
    size_t k;
    glitter_render_element* elem;
    size_t disp;
    glitter_buffer* buf;
    vec3 z_offset_dir;
    bool has_scale_x;
    bool has_scale_y;
    bool has_scale_z;
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
    inv_model_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };

    x_vec = (vec3){ 1.0f, 0.0f, 0.0f };
    y_vec = (vec3){ 0.0f, 1.0f, 0.0f };
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

    has_scale_x = false;
    has_scale_y = false;
    has_scale_z = false;
    emitter_local = false;
    scale = vec3_null;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        glitter_x_render_group_get_emitter_scale(a1, &scale);
        if (a1->flags & GLITTER_PARTICLE_SCALE) {
            x_vec.x = scale.x;
            y_vec.y = scale.y;
        }
        if (!(has_scale_x = fabsf(scale.x) > 0.000001f ? true : false))
            scale.x = 0.0f;
        if (!(has_scale_y = fabsf(scale.y) > 0.000001f ? true : false))
            scale.y = 0.0f;
        if (!(has_scale_z = fabsf(scale.z) > 0.000001f ? true : false))
            scale.z = 0.0f;
        emitter_local = true;
    }

    has_scale = has_scale_x || has_scale_y || has_scale_z;

    if (glitter_x_render_group_get_ext_anim_scale(a1, &ext_scale)) {
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

    buf = a1->buffer;
    elem = a1->elements;
    disp = 0;
    if (a1->draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION)
        for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->draw)
                    continue;

                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                pos = elem->translation;
                if (has_scale)
                    vec3_mult(pos, scale, pos);

                glitter_x_render_group_calc_draw_set_pivot(a1->pivot,
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
                    buf->color = *(vec3*)&elem->color;
                    buf->alpha = elem->color.w;
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

                if (!elem->draw)
                    continue;

                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                pos = elem->translation;
                if (has_scale)
                    vec3_mult(pos, scale, pos);

                glitter_x_render_group_calc_draw_set_pivot(a1->pivot,
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
                    buf->color = *(vec3*)&elem->color;
                    buf->alpha = elem->color.w;
                }
                disp++;
            }
        }
    a1->disp = disp;
}

static void FASTCALL glitter_x_render_group_calc_draw_quad_direction_rotation(glitter_render_group* a1,
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
    void(FASTCALL * rotate_func)(mat3*, glitter_render_group*, glitter_render_element*, vec3*);

    mat4_inverse(model_mat, &inv_model_mat);
    inv_model_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };

    x_vec_base = (vec3){ 1.0f, 0.0f, 0.0f };
    y_vec_base = (vec3){ 0.0f, 1.0f, 0.0f };

    mat4_mult_vec3(&inv_model_mat, &x_vec_base, &x_vec_base);
    mat4_mult_vec3(&inv_model_mat, &y_vec_base, &y_vec_base);

    if (a1->draw_type == GLITTER_DIRECTION_POSITION) {
        up_vec = (vec3){ 0.0f, 0.0f, 1.0f };
        rotate_func = glitter_x_render_element_rotate_to_position;
    }
    else {
        up_vec = (vec3){ 0.0f, 1.0f, 0.0f };
        rotate_func = glitter_x_render_element_rotate_to_prev_position;
    }

    scale = vec3_identity;

    use_scale = false;
    if (a1->flags & GLITTER_PARTICLE_EMITTER_LOCAL
        && glitter_x_render_group_get_emitter_scale(a1, &scale))
            use_scale = a1->flags & GLITTER_PARTICLE_SCALE ? true : false;

    buf = a1->buffer;
    elem = a1->elements;
    disp = 0;
    for (i = a1->ctrl, j_max = 1024; i > 0; i -= j_max) {
        j_max = min(i, j_max);
        for (j = j_max; j > 0; elem++) {
            if (!elem->alive)
                continue;
            j--;

            if (!elem->draw)
                continue;

            vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
            vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

            if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                continue;

            pos = elem->translation;

            if (use_scale)
                vec3_mult(pos, scale, pos);

            glitter_x_render_group_calc_draw_set_pivot(a1->pivot,
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
                buf->color = *(vec3*)&elem->color;
                buf->alpha = elem->color.w;
            }
            disp++;
        }
    }
    a1->disp = disp;
}

static void FASTCALL glitter_x_render_group_calc_draw_set_pivot(glitter_pivot pivot,
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

static bool FASTCALL glitter_x_render_group_get_ext_anim_scale(glitter_render_group* a1, vec3* a2) {
    glitter_effect_inst* effect_inst;

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
        if (a2)
            *a2 = effect_inst->ext_anim_scale;
        return true;
    }
    return false;
}

static bool FASTCALL glitter_x_render_group_get_emitter_scale(glitter_render_group* a1, vec3* scale) {
    *scale = vec3_identity;

    glitter_particle_inst* particle = a1->particle;
    if (particle) {
        glitter_emitter_inst* emitter = particle->data.emitter;
        if (emitter) {
            vec3_mult_scalar(emitter->scale, emitter->scale_all, *scale);

            float_t length;
            vec3_length(*scale, length);
            return length > 0.000001f;
        }
    }
    return true;
}