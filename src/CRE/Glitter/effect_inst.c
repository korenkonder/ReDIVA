/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst.h"
#include "curve.h"
#include "emitter_inst.h"
#include "random.h"
#include "scene.h"

static int32_t FASTCALL glitter_effect_inst_get_a3da_index(int32_t index);

glitter_effect_inst* FASTCALL glitter_effect_inst_init(glitter_effect* a1,
    glitter_scene* a2, size_t id, bool appear_now) {
    glitter_emitter** i;
    glitter_effect_inst_a3da* inst_a3da;
    glitter_effect_a3da* a3da;
    glitter_emitter_inst* emitter;

    glitter_effect_inst* ei = force_malloc(sizeof(glitter_effect_inst));
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = vec4_identity;
    ei->scale_all = 1.0f;
    ei->id = id;
    glitter_random_set(ei->data.name_hash % glitter_random_get_max());
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;

    if (appear_now)
        ei->data.appear_time = 0.0f;
    else
        ei->frame = -ei->data.appear_time;

    if (ei->data.a3da) {
        inst_a3da = force_malloc(sizeof(glitter_effect_inst_a3da));
        inst_a3da->dword00 = -1;
        inst_a3da->object_index = -1;
        inst_a3da->index = -1;
        inst_a3da->mat = mat4_identity;

        if (inst_a3da) {
            a3da = ei->data.a3da;
            if (a3da->flags & GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_ONCE)
                ei->flags |= GLITTER_EFFECT_INST_FLAG_SET_A3DA_ONCE;
            if (a3da->flags & GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_TRANS_ONLY)
                ei->flags |= GLITTER_EFFECT_INST_FLAG_SET_A3DA_TRANS_ONLY;
            if (a3da->flags & GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_BY_OBJECT_INDEX) {
                inst_a3da->index = glitter_effect_inst_get_a3da_index(a3da->index);
                ei->flags |= GLITTER_EFFECT_INST_FLAG_SET_A3DA_BY_OBJECT_INDEX;
            }
            
            inst_a3da->object_index = a3da->object_index;

            size_t mesh_name_len = strlen(a3da->mesh_name);
            if (mesh_name_len) {
                inst_a3da->mesh_name = force_malloc(mesh_name_len + 1);
                memcpy(inst_a3da->mesh_name, a3da->mesh_name, mesh_name_len);
            }
            else
                inst_a3da->mesh_name = 0;
        }
        ei->a3da = inst_a3da;
        ei->flags |= GLITTER_EFFECT_INST_FLAG_HAS_A3DA;
        ei->flags |= GLITTER_EFFECT_INST_FLAG_HAS_A3DA_NON_INIT;
        ei->flags |= GLITTER_EFFECT_INST_FLAG_SET_A3DA_BY_OBJECT_NAME;
    }

    ei->mat = mat4_identity;
    ei->random = glitter_random_get();
    vector_ptr_glitter_emitter_inst_resize(&ei->emitters, a1->emitters.end - a1->emitters.begin);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter = glitter_emitter_inst_init(*i, a2, ei);
        vector_ptr_glitter_emitter_inst_push_back(&ei->emitters, &emitter);
    }
    glitter_effect_inst_reset_sub(ei, a2);
    return ei;
}

void FASTCALL glitter_effect_inst_copy(glitter_effect_inst* a1,
    glitter_effect_inst* a2, glitter_scene* a3) {
    int64_t emitters_count; // rbp
    int64_t i; // rbx

    a2->frame = a1->frame;
    a2->color = a1->color;
    a2->translation = a1->translation;
    a2->rotation = a1->rotation;
    a2->scale = a1->scale;
    a2->mat = a1->mat;
    a2->random = a1->random;

    emitters_count = a1->emitters.end - a1->emitters.begin;
    if (emitters_count != a2->emitters.end - a2->emitters.begin)
        return;

    for (i = 0; i < emitters_count; i++)
        glitter_emitter_inst_copy(a1->emitters.begin[i], a2->emitters.begin[i], a3);
}

void FASTCALL glitter_effect_inst_emit(glitter_effect_inst* a1, glitter_scene* a2, float_t delta_frame) {
    glitter_emitter_inst** i; // rbx

    if (a1->frame >= 0.0f) {
        if (a1->frame < a1->data.life_time)
            for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
                glitter_emitter_inst_emit_step(*i, a2, a1, delta_frame);
        else if (a1->data.flags & GLITTER_EFFECT_FLAG_LOOP)
            a1->frame -= a1->data.life_time;
        else
            glitter_effect_inst_free(a1, a2, false);
    }
    a1->frame += delta_frame;
}

void FASTCALL glitter_effect_inst_emit_init(glitter_effect_inst* a1, glitter_scene* a2, float_t delta_frame) {
    glitter_emitter_inst** i;

    if (a1->frame >= 0.0f) {
        if (a1->frame < a1->data.life_time)
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                glitter_emitter_inst_emit_init(*i, a2, a1, delta_frame);
        else if (a1->data.flags & GLITTER_EFFECT_FLAG_LOOP)
            a1->frame -= a1->data.life_time;
        else
            glitter_effect_inst_free(a1, a2, 0);

        for (i = a1->emitters.begin; i != a1->emitters.end; i++)
            glitter_emitter_inst_render_group_init(*i, delta_frame);
    }
    a1->frame += delta_frame;
}

void FASTCALL glitter_effect_inst_free(glitter_effect_inst* a1, glitter_scene* a2, bool free) {
    glitter_emitter_inst** i;

    a1->flags |= GLITTER_EFFECT_INST_FLAG_FREE;
    for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
        glitter_emitter_inst_free(*i, a2, free);
}

void FASTCALL glitter_effect_inst_get_a3da(glitter_effect_inst* a1) {
    glitter_effect_inst_a3da* v3; // rax
    char* v4; // rax
    int64_t v6; // rbx
    mat4* v15; // r8
    int64_t v24; // rsi
    int32_t v26; // ebx
    char* v27; // r14
    int64_t v43; // rbx
    vec3 a2;
    mat4 mat0;
    mat4 mat1;

    if (~a1->flags & GLITTER_EFFECT_INST_FLAG_HAS_A3DA || !a1->a3da
        || (a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_ONCE
            && a1->flags & GLITTER_EFFECT_INST_FLAG_HAS_A3DA_TRANS))
        return;

    return;

    int32_t(FASTCALL * sub_1401D5010)(int32_t a1)
        = (void*)0x00000001401D5010;
    int64_t(FASTCALL * sub_1401D5BC0)(int32_t a1, int64_t a2, bool object_is_hrc)
        = (void*)0x00000001401D5BC0;
    int32_t(FASTCALL * sub_1401D6090)(int32_t a1, int64_t * a2, bool* a3)
        = (void*)0x00000001401D6090;
    int64_t(FASTCALL * sub_140459D40)(int32_t a1, int32_t a2)
        = (void*)0x0000000140459D40;
    int32_t(FASTCALL * sub_140459DE0)(int32_t a1, char* a2)
        = (void*)0x0000000140459DE0;
    mat4* (FASTCALL * sub_140516730)(int64_t a1, int32_t a2)
        = (void*)0x0000000140516730;
    int64_t(FASTCALL * sub_140532030)(char* a1, int32_t a2)
        = (void*)0x0000000140532030;
    char* (FASTCALL * sub_1405320E0)()
        = (void*)0x00000001405320E0;
    int64_t(FASTCALL * sub_1405327B0)(char* a1, int32_t a2)
        = (void*)0x00000001405327B0;
    mat4* (FASTCALL * sub_140516750)(int64_t a1)
        = (void*)0x0000000140516750;

    v3 = a1->a3da;
    if (a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_BY_OBJECT_INDEX) {
        v4 = sub_1405320E0();
        if (v4 && sub_1405327B0(v4, v3->object_index) && (v6 = sub_140532030(v4, v3->object_index))) {
            mat0 = *sub_140516750(v6);
            vec4_length(mat0.row0, a2.x);
            vec4_length(mat0.row1, a2.y);
            vec2_sub(*(vec2*)&a2, vec2_identity, *(vec2*)&a2);
            a2.z = 0.0f;
            a1->a3da_scale = a2;
            a1->flags |= GLITTER_EFFECT_INST_FLAG_HAS_A3DA_SCALE;
            if (v3->index != 201) {
                v15 = sub_140516730(v6, v3->index);
                if (v15) {
                    mat1 = *v15;
                    mat4_mult(&mat0, &mat1, &mat0);
                    if (a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_TRANS_ONLY) {
                        v3->mat = mat4_identity;
                        v3->translation = *(vec3*)&mat0.row3;
                    }
                    else
                        v3->mat = mat0;
                    goto SetFlags;
                }
            }
            else {
                if (a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_TRANS_ONLY) {
                    v3->mat = mat4_identity;
                    v3->translation = *(vec3*)&mat0.row3;
                }
                else
                    v3->mat = mat0;
                goto SetFlags;
            }
        }
    }
    else if (~a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_BY_OBJECT_NAME) {
        if (v3->index < 0) {
            if (!v3->mesh_name)
                return;

            v3->index = sub_140459DE0(v3->object_index, v3->mesh_name);
        }

        if (v3->index >= 0) {
            v43 = sub_140459D40(v3->object_index, v3->index);
            if (v43) {
                v3->translation = *(vec3*)(v43 + 4);
                goto SetFlags;
            }
        }
    }
    else {
        if (v3->dword00 == -1 || !(v24 = sub_1401D5BC0(v3->dword00, v3->dword08, v3->object_is_hrc))) {
            v3->dword00 = sub_1401D6090(v3->object_index, &v3->dword08, &v3->object_is_hrc);
            if (v3->dword00 == -1)
                return;

            v3->index = -1;
            v24 = sub_1401D5BC0(v3->dword00, v3->dword08, v3->object_is_hrc);
            if (!v24)
                return;
        }

        mat0 = mat4_identity;
        v26 = sub_1401D5010(v3->dword00);
        if (v26 >= 0 && v26 <= 5) {
            v27 = sub_1405320E0();
            if (sub_1405327B0(v27, v26)) {
                mat0 = *sub_140516750(sub_140532030(v27, v26));
                vec4_length(mat0.row0, a2.x);
                vec4_length(mat0.row1, a2.y);
                vec2_sub(*(vec2*)&a2, vec2_identity, *(vec2*)&a2);
                a2.z = 0.0f;
                a1->a3da_scale = a2;
                a1->flags |= GLITTER_EFFECT_INST_FLAG_HAS_A3DA_SCALE;
            }
        }

        mat1 = *(mat4*)v24;
        if (v3->mesh_name) {
            if (v3->index < 0) {
                v3->index = sub_140459DE0(v3->object_index, v3->mesh_name);
            }

            if (v3->index >= 0) {
                v43 = sub_140459D40(v3->object_index, v3->index);
                if (v43) {
                    mat4_mult(&mat0, &mat1, &v3->mat);
                    v3->translation = *(vec3*)(v43 + 4);
                    goto SetFlags;
                }
            }
        }
        else {
            mat4_mult(&mat0, &mat1, &mat0);
            if (a1->flags & GLITTER_EFFECT_INST_FLAG_SET_A3DA_TRANS_ONLY) {
                v3->mat = mat4_identity;
                v3->translation = *(vec3*)&mat0.row3;
            }
            else
                v3->mat = mat0;
            goto SetFlags;
        }
    }
    return;

SetFlags:
    if (a1->flags & GLITTER_EFFECT_INST_FLAG_HAS_A3DA_NON_INIT) {
        a1->flags &= ~GLITTER_EFFECT_INST_FLAG_HAS_A3DA_NON_INIT;
        a1->flags |= GLITTER_EFFECT_INST_FLAG_HAS_A3DA_TRANS;
    }
}

int32_t FASTCALL glitter_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return (a1->data.flags & GLITTER_EFFECT_FLAG_ALPHA) ? 2 : 1;
}

int32_t FASTCALL glitter_effect_inst_get_fog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FLAG_FOG)
        return 1;
    else if (a1->data.flags & GLITTER_EFFECT_FLAG_FOG_HEIGHT)
        return 2;
    else
        return 0;
}

void FASTCALL glitter_effect_inst_get_value(glitter_effect_inst* a1, float_t frame, int32_t random) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->effect->curve.end - a1->effect->curve.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->curve.begin[i];
        if (!glitter_curve_get_value(curve, frame, &value, random + i))
            continue;

        switch (curve->type) {
        case GLITTER_CURVE_TRANSLATION_X:
            a1->translation.x = value;
            break;
        case GLITTER_CURVE_TRANSLATION_Y:
            a1->translation.y = value;
            break;
        case GLITTER_CURVE_TRANSLATION_Z:
            a1->translation.z = value;
            break;
        case GLITTER_CURVE_ROTATION_X:
            a1->rotation.x = value;
            break;
        case GLITTER_CURVE_ROTATION_Y:
            a1->rotation.y = value;
            break;
        case GLITTER_CURVE_ROTATION_Z:
            a1->rotation.z = value;
            break;
        case GLITTER_CURVE_SCALE_X:
            a1->scale.x = value;
            break;
        case GLITTER_CURVE_SCALE_Y:
            a1->scale.y = value;
            break;
        case GLITTER_CURVE_SCALE_Z:
            a1->scale.z = value;
            break;
        case GLITTER_CURVE_SCALE_ALL:
            a1->scale_all = value;
            break;
        case GLITTER_CURVE_COLOR_R:
            a1->color.x = value;
            break;
        case GLITTER_CURVE_COLOR_G:
            a1->color.y = value;
            break;
        case GLITTER_CURVE_COLOR_B:
            a1->color.z = value;
            break;
        case GLITTER_CURVE_COLOR_A:
            a1->color.w = value;
            break;
        }
    }
}

bool FASTCALL glitter_effect_inst_has_ended(glitter_effect_inst* effect, bool a2) {
    glitter_emitter_inst** i;

    if (~effect->flags & GLITTER_EFFECT_INST_FLAG_FREE)
        return false;
    else if (!a2)
        return true;

    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (!glitter_emitter_inst_has_ended(*i, a2))
            return false;
    return true;
}

void FASTCALL glitter_effect_inst_reset(glitter_effect_inst* a1, glitter_scene* a2) {
    glitter_emitter_inst** i;

    a1->frame = -a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        glitter_emitter_inst_reset(*i);
    glitter_effect_inst_reset_sub(a1, a2);
}

void FASTCALL glitter_effect_inst_reset_sub(glitter_effect_inst* a1, glitter_scene* a2) {
    float_t delta_frame;
    float_t start_time;

    if (a1->data.start_time <= 0.0f)
        return;

    delta_frame = 2.0f;
    for (start_time = a1->data.start_time; start_time > 0.0f; start_time -= delta_frame) {
        a1->flags |= GLITTER_EFFECT_INST_FLAG_JUST_INIT;
        if (start_time < delta_frame)
            delta_frame -= start_time;
        glitter_effect_inst_update_value_init(a1, delta_frame);
        glitter_effect_inst_emit_init(a1, a2, delta_frame);
    }
    glitter_effect_inst_update_mat(a1);
}

void FASTCALL glitter_effect_inst_update_mat(glitter_effect_inst* a1) {
    glitter_emitter_inst** i;
    vec3 scale;

    if (a1->flags & GLITTER_EFFECT_INST_FLAG_JUST_INIT) {
        mat4_translate(a1->translation.x, a1->translation.y, a1->translation.z, &a1->mat);
        mat4_rot(&a1->mat, a1->rotation.x, a1->rotation.y, a1->rotation.z, &a1->mat);
        vec3_mult_scalar(a1->scale, a1->scale_all, scale);
        mat4_scale_rot(&a1->mat, scale.x, scale.y, scale.z, &a1->mat);
        for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
            glitter_emitter_inst_update_mat(*i, a1);
        a1->flags &= ~GLITTER_EFFECT_INST_FLAG_JUST_INIT;
    }
}

void FASTCALL glitter_effect_inst_update_value_frame(glitter_effect_inst* effect, float_t delta_frame) {
    vec3 scale;
    glitter_emitter_inst** i;
    mat4 mat;
    vec3 translation;

    glitter_effect_inst_get_value(effect, effect->frame, effect->random);
    vec3_mult_scalar(effect->scale, effect->scale_all, scale);
    glitter_effect_inst_get_a3da(effect);
    if (~effect->flags & GLITTER_EFFECT_INST_FLAG_HAS_A3DA_TRANS || !effect->a3da)
        translation = effect->translation;
    else
        vec3_add(effect->translation, effect->a3da->translation, translation);

    mat4_translate(translation.x, translation.y, translation.z, &mat);
    mat4_rot(&mat, effect->rotation.x, effect->rotation.y, effect->rotation.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    effect->mat = mat;
    for (i = effect->emitters.begin; i != effect->emitters.end; ++i)
        glitter_emitter_inst_update_value_frame(*i, effect, delta_frame);
}

void FASTCALL glitter_effect_inst_update_value_init(glitter_effect_inst* a1, float_t delta_frame) {
    glitter_emitter_inst** i;

    if (a1->frame < 0.0f)
        return;

    glitter_effect_inst_get_value(a1, a1->frame, a1->random);
    for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
        glitter_emitter_inst_update_value_init(*i, a1->frame, delta_frame);
}

void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->a3da) {
        free(ei->a3da->mesh_name);
        free(ei->a3da);
    }

    vector_ptr_glitter_emitter_inst_free(&ei->emitters, (void*)glitter_emitter_inst_dispose);
    free(ei);
}

static int32_t FASTCALL glitter_effect_inst_get_a3da_index(int32_t index) {
    switch (index) {
    case 0:
        return 15;
    case 1:
        return 54;
    case 2:
        return 0;
    case 3:
        return 7;
    case 4:
        return 106;
    case 5:
        return 108;
    case 6:
        return 109;
    case 7:
        return 123;
    case 8:
        return 142;
    case 9:
        return 144;
    case 10:
        return 145;
    case 11:
        return 159;
    case 12:
        return 194;
    case 13:
        return 184;
    case 14:
        return 183;
    case 15:
        return 197;
    case 16:
        return 191;
    case 17:
        return 190;
    default:
        return 201;
    }
}
