/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst.h"
#include "curve.h"
#include "effect_val.h"
#include "emitter_inst.h"
#include "scene.h"

glitter_effect_inst* FASTCALL glitter_effect_inst_init(glitter_effect* a1,
    glitter_scene* a2, int32_t id, bool a4) {
    glitter_emitter** i;
    glitter_effect_inst_a3da* inst_a3da;
    glitter_effect_a3da* a3da;
    glitter_emitter_inst* emitter;

    glitter_effect_inst* ei = force_malloc(sizeof(glitter_effect_inst));
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = (vec4){ 1.0f, 1.0f, 1.0f, 1.0f };
    ei->scale_all = 1.0;
    ei->id = id;
    Glitter__EffectVal__Set(ei->data.name_hash % Glitter__EffectVal__GetMax());
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = *(vec3*)&a1->scale;

    if (a4)
        ei->data.appear_time = 0.0;
    else
        ei->frame0 = -ei->data.appear_time;

    if (ei->data.a3da) {
        inst_a3da = force_malloc(sizeof(glitter_effect_inst_a3da));
        inst_a3da->dword00 = -1;
        inst_a3da->dword10 = -1;
        inst_a3da->object_index = -1;
        inst_a3da->index = -1;
        mat4_identity(&inst_a3da->mat);

        if (inst_a3da) {
            a3da = ei->data.a3da;
            if (a3da->flags & 1)
                ei->flags |= 0x80;
            if (a3da->flags & 2)
                ei->flags |= 0x100;
            if (a3da->flags & 0x10000) {
                inst_a3da->object_index = a3da->object_index;
                inst_a3da->index = a3da->index;
                ei->flags |= 0x20;
            }
            else
                inst_a3da->dword10 = a3da->object_index;

            size_t mesh_name_len = strlen(a3da->mesh_name);
            if (mesh_name_len) {
                inst_a3da->mesh_name = force_malloc(mesh_name_len + 1);
                memcpy(inst_a3da->mesh_name, a3da->mesh_name, mesh_name_len);
            }
            else
                inst_a3da->mesh_name = 0;
        }
        ei->a3da = inst_a3da;
        ei->flags |= 0x54;
    }

    mat4_identity(&ei->mat);
    ei->effect_val = Glitter__EffectVal__Get();
    vector_ptr_glitter_emitter_inst_expand(&ei->emitters, a1->emitters.end - a1->emitters.begin);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter = glitter_emitter_inst_init(*i, a2, ei);
        vector_ptr_glitter_emitter_inst_append_element(&ei->emitters, &emitter);
    }
    Glitter__EffectInst__ResetSub(ei, a2);
    return ei;
}

void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->a3da) {
        free(ei->a3da->mesh_name);
        free(ei->a3da);
    }

    vector_ptr_glitter_emitter_inst_clear(&ei->emitters, (void*)&glitter_emitter_inst_dispose);
    vector_ptr_glitter_emitter_inst_dispose(&ei->emitters);
    free(ei);
}

void FASTCALL Glitter__EffectInst__GetValue(glitter_effect_inst* a1, float_t frame, int32_t efct_val) {
    int64_t length; // rbp
    glitter_curve* curve; // r14
    float_t value; // [rsp+20h] [rbp-58h] BYREF

    length = a1->effect->curve.end - a1->effect->curve.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->curve.begin[i];
        if (!Glitter__Curve__GetValue(curve, frame, &value, efct_val + i))
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

void FASTCALL Glitter__EffectInst__Emit(glitter_effect_inst* a1, glitter_scene* a2, float_t delta_frame) {
    glitter_emitter_inst** i; // rbx

    if (a1->frame0 >= 0.0) {
        if (a1->frame0 < a1->data.life_time)
            for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
                Glitter__EmitterInst__EmitStep(*i, a2, a1, delta_frame);
        else if (a1->data.flags & GLITTER_EFFECT_FLAG_LOOP)
            a1->frame0 -= a1->data.life_time;
        else
            Glitter__EffectInst__Free(a1, a2, false);
    }
    a1->frame0 += delta_frame;
}

void FASTCALL Glitter__EffectInst__EmitInit(glitter_effect_inst* a1, glitter_scene* a2, float_t delta_frame) {
    glitter_emitter_inst** i;

    if (a1->frame0 >= 0.0) {
        if (a1->frame0 < a1->data.life_time)
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                Glitter__EmitterInst__EmitInit(*i, a2, a1, delta_frame);
        else if (a1->data.flags & GLITTER_EFFECT_FLAG_LOOP)
            a1->frame0 -= a1->data.life_time;
        else
            Glitter__EffectInst__Free(a1, a2, 0);

        for (i = a1->emitters.begin; i != a1->emitters.end; i++)
            Glitter__EmitterInst__RenderGroupInit(*i, delta_frame);
    }
    a1->frame0 += delta_frame;
}

void FASTCALL Glitter__EffectInst__UpdateValueFrame(glitter_effect_inst* effect, float_t delta_frame) {
    vec3 scale;
    glitter_emitter_inst** i; // rbx
    mat4 mat; // [rsp+20h] [rbp-98h] BYREF

    Glitter__EffectInst__GetValue(effect, effect->frame0, effect->effect_val);
    vec3_mult_scalar(effect->scale, effect->scale_all, scale);
    mat4_identity(&effect->mat);
    Glitter__EffectInst__GetA3DA(effect);
    if (Glitter__EffectInst__GetA3DAMat(effect, &mat))
        mat4_mult(&effect->mat, &mat, &effect->mat);

    mat = effect->mat;
    mat4_translate_mult(&mat, effect->translation.x, effect->translation.y, effect->translation.z, &mat);
    mat4_rot(&mat, effect->rotation.x, effect->rotation.y, effect->rotation.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    effect->mat = mat;
    for (i = effect->emitters.begin; i != effect->emitters.end; ++i)
        Glitter__EmitterInst__UpdateValueFrame(*i, effect, delta_frame);
}

void FASTCALL Glitter__EffectInst__UpdateValueInit(glitter_effect_inst* a1, float_t delta_frame) {
    glitter_emitter_inst** i;

    if (a1->frame0 < 0.0f)
        return;

    Glitter__EffectInst__GetValue(a1, a1->frame0, a1->effect_val);
    for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
        Glitter__EmitterInst__UpdateValueInit(*i, a1->frame0, delta_frame);
}

void FASTCALL Glitter__EffectInst__Copy(glitter_effect_inst* a1, glitter_effect_inst* a2, glitter_scene* a3) {
    int64_t emitters_count; // rbp
    int64_t i; // rbx

    a2->frame0 = a1->frame0;
    a2->frame1 = a1->frame1;
    a2->color = a1->color;
    a2->translation = a1->translation;
    a2->rotation = a1->rotation;
    a2->scale = a1->scale;
    a2->mat = a1->mat;
    a2->effect_val = a1->effect_val;

    emitters_count = a1->emitters.end - a1->emitters.begin;
    if (emitters_count != a2->emitters.end - a2->emitters.begin)
        return;

    for (i = 0; i < emitters_count; i++)
        Glitter__EmitterInst__Copy(a1->emitters.begin[i], a2->emitters.begin[i], a3);
}

void FASTCALL Glitter__EffectInst__UpdateMat(glitter_effect_inst* a1) {
    glitter_emitter_inst** i;
    vec3 scale;

    if (a1->flags & 2) {
        mat4_translate(a1->translation.x, a1->translation.y, a1->translation.z, &a1->mat);
        mat4_rot(&a1->mat, a1->rotation.x, a1->rotation.y, a1->rotation.z, &a1->mat);
        vec3_mult_scalar(a1->scale, a1->scale_all, scale);
        mat4_scale_rot(&a1->mat, scale.x, scale.y, scale.z, &a1->mat);
        for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
            Glitter__EmitterInst__UpdateMat(*i, a1);
        a1->flags &= ~2;
    }
}

bool FASTCALL Glitter__EffectInst__GetA3DAMat(glitter_effect_inst* effect, mat4* a2) {
    if (!(effect->flags & 8) || !effect->a3da)
        return false;

    mat4_translate_mult(a2, effect->a3da->translation.x,
        effect->a3da->translation.y, effect->a3da->translation.z, a2);
    return true;
}

int32_t FASTCALL Glitter__EffectInst__GetAlpha(glitter_effect_inst* a1) {
    return (a1->data.flags & GLITTER_EFFECT_FLAG_ALPHA) ? 2 : 1;
}

int32_t FASTCALL Glitter__EffectInst__GetFog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FLAG_FOG)
        return 1;
    else if (a1->data.flags & GLITTER_EFFECT_FLAG_FOG_HEIGHT)
        return 2;
    else
        return 0;
}

bool FASTCALL Glitter__EffectInst__HasEnded(glitter_effect_inst* effect, bool a2) {
    glitter_emitter_inst** i;

    if (!(effect->flags & 1))
        return false;
    else if (!a2)
        return true;

    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (!Glitter__EmitterInst__HasEnded(*i, a2))
            return false;
    return true;
}

void FASTCALL Glitter__EffectInst__SetA3DAScale(glitter_effect_inst* a1, vec3* a2) {
    a1->a3da_scale = *a2;
    a1->flags |= 0x800;
}

void FASTCALL Glitter__EffectInst__Free(glitter_effect_inst* a1, glitter_scene* a2, bool free) {
    glitter_emitter_inst** i;

    a1->flags |= 0x01;
    for (i = a1->emitters.begin; i != a1->emitters.end; ++i)
        Glitter__EmitterInst__Free(*i, a2, free);
}

void FASTCALL Glitter__EffectInst__GetA3DA(glitter_effect_inst* a1) {
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

    if (!(a1->flags & 4) || !a1->a3da || (a1->flags & 0x80 && a1->flags & 8))
        return;

    return;

    int32_t (FASTCALL * sub_1401D5010)(int32_t a1)
        = (void*)0x00000001401D5010;
    int64_t (FASTCALL * sub_1401D5BC0)(int32_t a1, int64_t a2, bool object_is_hrc)
        = (void*)0x00000001401D5BC0;
    int32_t (FASTCALL * sub_1401D6090)(int32_t a1, int64_t* a2, bool* a3)
        = (void*)0x00000001401D6090;
    int64_t(FASTCALL * sub_140459D40)(int32_t a1, int32_t a2)
        = (void*)0x0000000140459D40;
    int32_t (FASTCALL * sub_140459DE0)(int32_t a1, char* a2)
        = (void*)0x0000000140459DE0;
    mat4* (FASTCALL * sub_140516730)(int64_t a1, int32_t a2)
        = (void*)0x0000000140516730;
    int64_t (FASTCALL * sub_140532030)(char* a1, int32_t a2)
        = (void*)0x0000000140532030;
    char* (FASTCALL * sub_1405320E0)()
        = (void*)0x00000001405320E0;
    int64_t (FASTCALL * sub_1405327B0)(char* a1, int32_t a2)
        = (void*)0x00000001405327B0;
    mat4* (FASTCALL * sub_140516750)(int64_t a1)
        = (void*)0x0000000140516750;

    v3 = a1->a3da;
    if (a1->flags & 0x20) {
        v4 = sub_1405320E0();
        if (v4 && sub_1405327B0(v4, v3->object_index) && (v6 = sub_140532030(v4, v3->object_index))) {
            mat0 = *sub_140516750(v6);
            vec4_length(mat0.row0, a2.x);
            vec4_length(mat0.row1, a2.y);
            vec2_sub(*(vec2*)&a2, ((vec2) { 1.0f, 1.0f }), *(vec2*)&a2);
            a2.z = 0.0f;
            Glitter__EffectInst__SetA3DAScale(a1, &a2);
            if (v3->index != 201) {
                v15 = sub_140516730(v6, v3->index);
                if (v15) {
                    mat1 = *v15;
                    mat4_mult(&mat0, &mat1, &mat0);
                    if (a1->flags & 0x100) {
                        mat4_identity(&v3->mat);
                        v3->translation = *(vec3*)&mat0.row3;
                    }
                    else
                        v3->mat = mat0;

                    if (a1->flags & 0x10)
                        a1->flags = (a1->flags & 0xFFFFFFEF) | 8;
                }
            }
            else {
                if (a1->flags & 0x100) {
                    mat4_identity(&v3->mat);
                    v3->translation = *(vec3*)&mat0.row3;
                }
                else
                    v3->mat = mat0;

                if (a1->flags & 0x10)
                    a1->flags = (a1->flags & 0xFFFFFFEF) | 8;
            }
        }
    }
    else if (!(a1->flags & 0x40)) {
        if (v3->index < 0) {
            if (!v3->mesh_name)
                return;

            v3->index = sub_140459DE0(v3->dword10, v3->mesh_name);
        }

        if (v3->index >= 0) {
            v43 = sub_140459D40(v3->dword10, v3->index);
            if (v43) {
                v3->translation = *(vec3*)(v43 + 4);

                if (a1->flags & 0x10)
                    a1->flags = (a1->flags & 0xFFFFFFEF) | 8;
            }
        }
    }
    else {
        if (v3->dword00 == -1 || !(v24 = sub_1401D5BC0(v3->dword00, v3->dword08, v3->object_is_hrc))) {
            v3->dword00 = sub_1401D6090(v3->dword10, &v3->dword08, &v3->object_is_hrc);
            if (v3->dword00 == -1)
                return;

            v3->index = -1;
            v24 = sub_1401D5BC0(v3->dword00, v3->dword08, v3->object_is_hrc);
            if (!v24)
                return;
        }

        mat4_identity(&mat0);
        v26 = sub_1401D5010(v3->dword00);
        if (v26 >= 0 && v26 <= 5) {
            v27 = sub_1405320E0();
            if (sub_1405327B0(v27, v26)) {
                mat0 = *sub_140516750(sub_140532030(v27, v26));
                vec4_length(mat0.row0, a2.x);
                vec4_length(mat0.row1, a2.y);
                vec2_sub(*(vec2*)&a2, ((vec2) { 1.0f, 1.0f }), *(vec2*)&a2);
                a2.z = 0.0f;
                Glitter__EffectInst__SetA3DAScale(a1, &a2);
            }
        }

        mat1 = *(mat4*)v24;
        if (v3->mesh_name) {
            if (v3->index < 0) {
                v3->index = sub_140459DE0(v3->dword10, v3->mesh_name);
            }

            if (v3->index >= 0) {
                v43 = sub_140459D40(v3->dword10, v3->index);
                if (v43) {
                    mat4_mult(&mat0, &mat1, &v3->mat);
                    v3->translation = *(vec3*)(v43 + 4);

                    if (a1->flags & 0x10)
                        a1->flags = (a1->flags & 0xFFFFFFEF) | 8;
                }
            }
        }
        else {
            mat4_mult(&mat0, &mat1, &mat0);
            if (a1->flags & 0x100) {
                mat4_identity(&v3->mat);
                v3->translation = *(vec3*)&mat0.row3;
            }
            else
                v3->mat = mat0;

            if (a1->flags & 0x10)
                a1->flags = (a1->flags & 0xFFFFFFEF) | 8;
        }
    }
}

void FASTCALL Glitter__EffectInst__ResetSub(glitter_effect_inst* a1, glitter_scene* a2) {
    float_t delta_frame; // xmm7_4
    float_t v11; // xmm6_4

    if (a1->data.start_time <= 0.0)
        return;

    if (a2->effect_group && a2->effect_group->scene
        && Glitter__Scene__Copy(a2->effect_group->scene, a1, a2))
        a1->frame1 = a1->data.start_time;
    else {
        delta_frame = 2.0f;
        v11 = a1->data.start_time - a1->frame1;
        for (; v11 > 0.0f; v11 -= delta_frame) {
            a1->flags |= 2;
            if (v11 < delta_frame)
                delta_frame -= v11;
            Glitter__EffectInst__UpdateValueInit(a1, delta_frame);
            Glitter__EffectInst__EmitInit(a1, a2, delta_frame);
            a1->frame1 += delta_frame;
        }
        Glitter__EffectInst__UpdateMat(a1);
    }
}

void FASTCALL Glitter__EffectInst__Reset(glitter_effect_inst* a1, glitter_scene* a2) {
    glitter_emitter_inst** i;

    a1->frame0 = -a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        Glitter__EmitterInst__Reset(*i);
    a1->frame1 = 0.0;
    Glitter__EffectInst__ResetSub(a1, a2);
}
