/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../gl_state.hpp"

namespace Glitter {
    RenderGroup::RenderGroup() : flags(), type(), draw_type(), pivot(),
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
        blend_mode = PARTICLE_BLEND_TYPICAL;
        mask_blend_mode = PARTICLE_BLEND_TYPICAL;
    }

    RenderGroup::~RenderGroup() {

    }

    RenderElement* RenderGroup::AddElement(RenderElement* rend_elem) {
        if (!rend_elem)
            rend_elem = elements;

        size_t left_count = count - (rend_elem - elements);
        size_t index = 0;
        if (left_count < 1)
            return 0;

        while (rend_elem->alive) {
            index++;
            rend_elem++;
            if (index >= left_count)
                return 0;
        }

        rend_elem->alive = true;
        ctrl++;
        return rend_elem;
    }

    F2RenderGroup::F2RenderGroup(F2ParticleInst* a1) : particle() {
        switch (a1->data.data.type) {
        case PARTICLE_QUAD:
        case PARTICLE_LINE:
        case PARTICLE_LOCUS:
            break;
        default:
            return;
        }

        if (a1->data.data.count > 0)
            count = a1->data.data.count;
        else
            count = a1->data.data.type == PARTICLE_LOCUS ? 30 : 250;
        max_count = count * 4;

        random_ptr = a1->data.random_ptr;
        particle = a1;

        elements = new RenderElement[count];
        if (!elements) {
            count = 0;
            max_count = 0;
            return;
        }

        memset(elements, 0, sizeof(RenderElement) * count);
        if (!max_count)
            return;

        bool is_quad = a1->data.data.type == PARTICLE_QUAD;

        glGenBuffers(1, &vbo);
        gl_state_bind_array_buffer(vbo);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(Buffer) * max_count), 0, GL_DYNAMIC_DRAW);
        gl_state_bind_array_buffer(0);

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

        if (!is_quad)
            draw_list.reserve(count);
    }

    F2RenderGroup::~F2RenderGroup() {
        DeleteBuffers(false);
    }

    bool F2RenderGroup::CannotDisp() {
        if (!particle)
            return true;
        else if (particle->data.effect)
            return (particle->data.effect->flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
        else if ((particle = particle->data.parent) && particle->data.effect)
            return (particle->data.effect->flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
        return true;
    }

    void F2RenderGroup::Copy(F2RenderGroup* dst) {
        dst->flags = flags;
        dst->type = type;
        dst->draw_type = draw_type;
        dst->blend_mode = blend_mode;
        dst->mask_blend_mode = mask_blend_mode;
        dst->pivot = pivot;
        dst->split_u = split_u;
        dst->split_v = split_v;
        dst->split_uv = split_uv;
        dst->z_offset = z_offset;
        if (dst->count > count)
            dst->count = count;
        dst->ctrl = ctrl;
        dst->texture = texture;
        dst->mask_texture = mask_texture;
        dst->frame = frame;
        dst->mat = mat;
        dst->mat_rot = mat_rot;
        dst->mat_draw = mat_draw;
        if (dst->count == count) {
            memmove(dst->elements, elements, sizeof(RenderElement) * dst->count);
            dst->ctrl = ctrl;
        }
    }

    void F2RenderGroup::Ctrl(GLT, float_t delta_frame, bool copy_mats) {
        if (!particle)
            return;

        F2ParticleInst::Data* data = &particle->data;
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

        if (copy_mats && particle->data.emitter) {
            F2EmitterInst* emitter = particle->data.emitter;
            mat = emitter->mat;
            mat_rot = emitter->mat_rot;
        }

        for (size_t ctrl = this->ctrl, i = 0; ctrl; i++) {
            if (!elements[i].alive)
                continue;

            CtrlParticle(GLT_VAL, &elements[i], delta_frame);
            ctrl--;
        }
        frame += delta_frame;
    }

    void F2RenderGroup::CtrlParticle(GLT, Glitter::RenderElement* rend_elem, float_t delta_frame) {
        if (!particle || (particle->data.data.flags & PARTICLE_LOOP
            && particle->HasEnded(false)) || rend_elem->frame >= rend_elem->life_time) {
            rend_elem->alive = false;
            delete rend_elem->locus_history;
            rend_elem->locus_history = 0;
            ctrl--;
            return;
        }

        particle->AccelerateParticle(GLT_VAL, rend_elem,
            rend_elem->frame * (float_t)(1.0 / 60.0), delta_frame, random_ptr);
        if (particle->data.data.draw_type == DIRECTION_PARTICLE_ROTATION) {
            rend_elem->rotation.x += rend_elem->rotation_add.x * delta_frame;
            rend_elem->rotation.y += rend_elem->rotation_add.y * delta_frame;
        }
        rend_elem->rotation.z += rend_elem->rotation_add.z * delta_frame;

        vec2 uv_scroll;
        vec2_mult_scalar(particle->data.data.uv_scroll_add,
            particle->data.data.uv_scroll_add_scale * delta_frame, uv_scroll);
        vec2_add(rend_elem->uv_scroll, uv_scroll, rend_elem->uv_scroll);

        particle->StepUVParticle(GLT_VAL, rend_elem, delta_frame, random_ptr);
        rend_elem->disp = true;
        rend_elem->color = { -1.0f, -1.0f, -1.0f, -1.0f };

        bool disp = true;
        if (particle->data.data.sub_flags & PARTICLE_SUB_USE_CURVE)
            disp = particle->GetValue(GLT_VAL, rend_elem, rend_elem->frame, random_ptr);

        if (disp)
            particle->GetColor(rend_elem);

        if (particle->data.data.type == PARTICLE_LOCUS)
            rend_elem->locus_history->Append(rend_elem, particle);

        rend_elem->frame += delta_frame;
        if (particle->data.data.flags & PARTICLE_LOOP && rend_elem->frame >= rend_elem->life_time)
            rend_elem->frame -= rend_elem->life_time;
    }

    void F2RenderGroup::DeleteBuffers(bool free) {
        if (particle) {
            if (!free)
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

        if (!free && elements) {
            Free();
            delete[] elements;
            elements = 0;
        }
    }

    void F2RenderGroup::Emit(GPM, GLT, F2ParticleInst::Data* ptcl_inst_data,
        F2EmitterInst* emit_inst, int32_t dup_count, int32_t count) {
        RenderElement* element = 0;
        for (int32_t i = 0; i < dup_count; i++)
            for (int32_t index = 0; index < count; index++, element++) {
                element = AddElement(element);
                if (!element)
                    break;

                particle->EmitParticle(GPM_VAL, GLT_VAL,
                    element, emit_inst, ptcl_inst_data, index, random_ptr);
            }
    }

    void F2RenderGroup::Free() {
        if (count <= 0) {
            ctrl = 0;
            return;
        }

        RenderElement* elem = elements;
        for (size_t i = count; i; i--, elem++) {
            elem->alive = false;
            if (elem->locus_history) {
                delete elem->locus_history;
                elem->locus_history = 0;
            }
        }
        ctrl = 0;
    }

    void F2RenderGroup::FreeData() {
        DeleteBuffers(true);
    }

    bool F2RenderGroup::GetExtAnimScale(vec3* ext_anim_scale, float_t* some_scale) {
        if (!particle)
            return false;

        F2EffectInst* effect_inst;
        if (particle->data.effect)
            effect_inst = particle->data.effect;
        else {
            if (!particle->data.parent)
                return false;

            effect_inst = particle->data.parent->data.effect;
            if (!effect_inst)
                return false;
        }

        if (~effect_inst->flags & EFFECT_INST_HAS_EXT_ANIM_SCALE)
            return false;

        if (ext_anim_scale)
            *ext_anim_scale = effect_inst->ext_anim_scale;
        if (some_scale)
            *some_scale = effect_inst->some_scale;
        return true;
    }

    mat4 F2RenderGroup::RotateToEmitPosition(F2RenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec) {
        vec3 vec2;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL)
            vec2 = rend_elem->translation;
        else {
            mat4_get_translation(&rend_group->mat, &vec2);
            vec3_sub(vec2, rend_elem->translation, vec2);
        }
        vec3_normalize(vec2, vec2);

        mat4 mat;
        if (fabsf(vec2.y) >= 0.000001f) {
            mat = mat4_identity;
            vec3 vec1 = *vec;

            vec3 axis;
            float_t angle;
            axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
            mat4_mult_axis_angle(&mat, &mat, &axis, angle);
        }
        else
            mat4_rotate_z((float_t)M_PI, &mat);
        return mat;
    }

    mat4 F2RenderGroup::RotateToPrevPosition(F2RenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec) {
        vec3 vec2;
        vec3_sub(rend_elem->translation, rend_elem->translation_prev, vec2);

        float_t length;
        vec3_length_squared(vec2, length);
        if (length < 0.000001f)
            vec2 = rend_elem->translation;
        vec3_normalize(vec2, vec2);

        mat4 mat;
        mat4_rotate_z((float_t)M_PI, &mat);
        if (fabsf(vec2.y) >= 0.000001f) {
            vec3 vec1 = *vec;

            vec3 axis;
            float_t angle;
            axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
            mat4_mult_axis_angle(&mat, &mat, &axis, angle);
        }
        return mat;
    }

    XRenderGroup::XRenderGroup(XParticleInst* a1) : particle(), use_culling() {
        object_name_hash = hash_murmurhash_empty;

        switch (a1->data.data.type) {
        case PARTICLE_QUAD:
            if (a1->data.data.count > 0)
                count = a1->data.data.count;
            else
                count = 2500;
            max_count = 4 * count;
            break;
        case PARTICLE_LINE:
            count = (size_t)a1->data.data.locus_history_size
                + a1->data.data.locus_history_size_random;
            max_count = count;
            break;
        case PARTICLE_LOCUS:
            count = (size_t)a1->data.data.locus_history_size
                + a1->data.data.locus_history_size_random;
            max_count = 2 * count;
            break;
        case PARTICLE_MESH:
            count = 1280;
            max_count = 0;
            break;
        default:
            return;
        }

        random_ptr = a1->data.random_ptr;
        particle = a1;

        elements = new RenderElement[count];
        if (!elements) {
            count = 0;
            max_count = 0;
            return;
        }

        memset(elements, 0, sizeof(RenderElement) * count);
        if (!max_count)
            return;

        bool is_quad = a1->data.data.type == PARTICLE_QUAD;

        if (a1->data.data.type != PARTICLE_MESH) {
            glGenBuffers(1, &vbo);
            gl_state_bind_array_buffer(vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(Buffer) * max_count), 0, GL_DYNAMIC_DRAW);
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

        if (!is_quad)
            draw_list.reserve(count);
    }

    XRenderGroup::~XRenderGroup() {
        DeleteBuffers(false);
    }

    bool XRenderGroup::CannotDisp() {
        XEffectInst* effect;
        if (!particle)
            return true;
        else if (particle->data.effect)
            return (particle->data.effect->flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
        else if ((particle = particle->data.parent) && (effect = particle->data.effect))
            return (effect->flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT) != 0;
        else
            return true;
    }

    void XRenderGroup::Copy(XRenderGroup* dst) {
        dst->flags = flags;
        dst->type = type;
        dst->draw_type = draw_type;
        dst->blend_mode = blend_mode;
        dst->mask_blend_mode = mask_blend_mode;
        dst->pivot = pivot;
        dst->split_u = split_u;
        dst->split_v = split_v;
        dst->split_uv = split_uv;
        dst->z_offset = z_offset;
        if (dst->count > count)
            dst->count = count;
        dst->ctrl = ctrl;
        dst->texture = texture;
        dst->mask_texture = mask_texture;
        dst->frame = frame;
        dst->mat = mat;
        dst->mat_rot = mat_rot;
        dst->mat_draw = mat_draw;
        if (dst->count == count) {
            memmove(dst->elements, elements, sizeof(RenderElement) * dst->count);
            dst->ctrl = ctrl;
        }
    }

    void XRenderGroup::Ctrl(float_t delta_frame, bool copy_mats) {
        if (!particle)
            return;

        XParticleInst::Data* data = &particle->data;
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

        if (copy_mats && particle->data.emitter) {
            XEmitterInst* emitter = particle->data.emitter;
            mat = emitter->mat;
            mat_rot = emitter->mat_rot;
        }

        for (size_t ctrl = this->ctrl, i = 0; ctrl; i++) {
            if (!elements[i].alive)
                continue;

            CtrlParticle(&elements[i], delta_frame);
            ctrl--;
        }
        frame += delta_frame;
    }

    void XRenderGroup::CtrlParticle(RenderElement* rend_elem, float_t delta_frame) {
        random_ptr->XSetStep(rend_elem->step);
        if (!particle || (particle->data.data.flags & PARTICLE_LOOP
            && particle->HasEnded(false)) || rend_elem->frame >= rend_elem->life_time) {
            rend_elem->alive = false;
            delete rend_elem->locus_history;
            rend_elem->locus_history = 0;
            ctrl--;
            return;
        }

        particle->AccelerateParticle(rend_elem, delta_frame, random_ptr);

        if (particle->data.data.draw_type == DIRECTION_PARTICLE_ROTATION
            || particle->data.data.type == PARTICLE_MESH) {
            rend_elem->rotation.x += rend_elem->rotation_add.x * delta_frame;
            rend_elem->rotation.y += rend_elem->rotation_add.y * delta_frame;
        }
        rend_elem->rotation.z += rend_elem->rotation_add.z * delta_frame;

        vec2 uv_scroll;
        vec2_mult_scalar(particle->data.data.uv_scroll_add,
            particle->data.data.uv_scroll_add_scale * delta_frame, uv_scroll);
        if (uv_scroll.x != 0.0f)
            rend_elem->uv_scroll.x = fmodf(rend_elem->uv_scroll.x + uv_scroll.x, 1.0f);
        if (uv_scroll.y != 0.0f)
            rend_elem->uv_scroll.y = fmodf(rend_elem->uv_scroll.y + uv_scroll.y, 1.0f);

        if (particle->data.data.sub_flags & PARTICLE_SUB_UV_2ND_ADD) {
            vec2 uv_scroll_2nd;
            vec2_mult_scalar(particle->data.data.uv_scroll_2nd_add,
                particle->data.data.uv_scroll_2nd_add_scale * delta_frame, uv_scroll_2nd);
            if (uv_scroll_2nd.x != 0.0f)
                rend_elem->uv_scroll_2nd.x = fmodf(rend_elem->uv_scroll_2nd.x + uv_scroll_2nd.x, 1.0f);
            if (uv_scroll_2nd.y != 0.0f)
                rend_elem->uv_scroll_2nd.y = fmodf(rend_elem->uv_scroll_2nd.y + uv_scroll_2nd.y, 1.0f);
        }

        particle->StepUVParticle(rend_elem, delta_frame, random_ptr);
        rend_elem->disp = true;
        rend_elem->color = { -1.0f, -1.0f, -1.0f, -1.0f };

        bool disp = true;
        float_t color_scale = -1.0f;
        if (particle->data.data.sub_flags & PARTICLE_SUB_USE_CURVE)
            disp = particle->GetValue(rend_elem, rend_elem->frame, random_ptr, &color_scale);

        if (disp)
            particle->GetColor(rend_elem, color_scale);

        if (particle->data.data.type == PARTICLE_LOCUS)
            rend_elem->locus_history->Append(rend_elem, particle);

        rend_elem->frame += delta_frame;
        if (particle->data.data.flags & PARTICLE_LOOP && rend_elem->frame >= rend_elem->life_time)
            rend_elem->frame -= rend_elem->life_time;
    }

    void XRenderGroup::Emit(XParticleInst::Data* ptcl_inst_data,
        XEmitterInst* emit_inst, int32_t dup_count, int32_t count) {
        RenderElement* element;
        int64_t i;
        int32_t index;
        uint8_t step;

        step = emit_inst->RandomGetStep();
        random_ptr->XSetStep(1);
        for (element = 0, i = 0; i < dup_count; i++)
            for (index = 0; index < count; index++, element++) {
                element = AddElement(element);
                if (!element)
                    break;

                emit_inst->RandomStepValue();
                particle->EmitParticle(element, emit_inst, ptcl_inst_data, index, step, random_ptr);
            }
    }

    void XRenderGroup::Free() {
        if (count <= 0) {
            ctrl = 0;
            return;
        }

        RenderElement* elem = elements;
        for (size_t i = count; i; i--, elem++) {
            elem->alive = false;
            if (elem->locus_history) {
                delete elem->locus_history;
                elem->locus_history = 0;
            }
        }
        ctrl = 0;
    }

    void XRenderGroup::FreeData() {
        DeleteBuffers(true);
    }

    bool XRenderGroup::GetEmitterScale(vec3* emitter_scale) {
        *emitter_scale = vec3_identity;

        if (particle) {
            XEmitterInst* emitter = particle->data.emitter;
            if (emitter) {
                vec3_mult_scalar(emitter->scale, emitter->scale_all, *emitter_scale);

                float_t length;
                vec3_length(*emitter_scale, length);
                return length > 0.000001f;
            }
        }
        return true;
    }

    bool XRenderGroup::GetExtAnimScale(vec3* ext_anim_scale, float_t* some_scale) {
        if (!particle)
            return false;

        XEffectInst* effect_inst;
        if (particle->data.effect)
            effect_inst = particle->data.effect;
        else {
            if (!particle->data.parent)
                return false;

            effect_inst = particle->data.parent->data.effect;
            if (!effect_inst)
                return false;
        }

        if (~effect_inst->flags & EFFECT_INST_HAS_EXT_ANIM_SCALE)
            return false;

        if (ext_anim_scale)
            *ext_anim_scale = effect_inst->ext_anim_scale;
        if (some_scale)
            *some_scale = effect_inst->some_scale;
        return true;
    }

    mat4 XRenderGroup::RotateMeshToEmitPosition(XRenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec, vec3* trans) {
        vec3 vec2;
        mat4_get_translation(&rend_group->mat, &vec2);
        vec3_sub(vec2, *trans, vec2);

        float_t length;
        vec3_length_squared(vec2, length);
        if (length < 0.000001f)
            vec2 = { 0.0f, 1.0f, 0.0f };
        vec3_normalize(vec2, vec2);

        vec3 vec1 = *vec;

        vec3 axis;
        float_t angle;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);

        mat4 mat;
        mat4_mult_axis_angle(&mat4_identity, &mat, &axis, angle);
        return mat;
    }

    mat4 XRenderGroup::RotateMeshToPrevPosition(XRenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec, vec3* trans) {
        vec3 vec2;
        vec3_sub(rend_elem->translation, rend_elem->translation_prev, vec2);

        float_t length;
        vec3_length_squared(vec2, length);
        if (length < 0.000001f)
            vec2 = rend_elem->base_direction;
        vec3_normalize(vec2, vec2);

        vec3 vec1 = *vec;

        vec3 axis;
        float_t angle;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);

        mat4 mat;
        mat4_mult_axis_angle(&mat4_identity, &mat, &axis, angle);
        return mat;
    }

    mat4 XRenderGroup::RotateToEmitPosition(XRenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec) {
        vec3 vec2;
        mat4_get_translation(&rend_elem->mat, &vec2);
        vec3_sub(vec2, rend_elem->translation, vec2);

        float_t length;
        vec3_length_squared(vec2, length);
        if (length < 0.000001f)
            vec2 = { 0.0f, 1.0f, 0.0f };
        vec3_normalize(vec2, vec2);

        vec3 vec1 = *vec;

        vec3 axis;
        float_t angle;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);

        mat4 mat;
        mat4_mult_axis_angle(&mat4_identity, &mat, &axis, angle);
        return mat;
    }

    mat4 XRenderGroup::RotateToPrevPosition(XRenderGroup* rend_group,
        RenderElement* rend_elem, vec3* vec) {
        vec3 vec2;
        vec3_sub(rend_elem->translation, rend_elem->translation_prev, vec2);

        float_t length;
        vec3_length_squared(vec2, length);
        if (length < 0.000001f)
            vec2 = rend_elem->base_direction;
        vec3_normalize(vec2, vec2);

        vec3 vec1 = *vec;

        vec3 axis;
        float_t angle;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);

        mat4 mat;
        mat4_mult_axis_angle(&mat4_identity, &mat, &axis, angle);
        return mat;
    }
}