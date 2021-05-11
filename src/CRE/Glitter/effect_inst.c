/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst.h"
#include "counter.h"
#include "curve.h"
#include "emitter_inst.h"
#include "random.h"
#include "render_group.h"

static void FASTCALL glitter_effect_inst_get_ext_anim(GPM, glitter_effect_inst* a1);
static int32_t FASTCALL glitter_aft_effect_inst_get_ext_anim_bone_index(int32_t index);
static int32_t FASTCALL glitter_f2_effect_inst_get_ext_anim_bone_index(int32_t index);
static void FASTCALL glitter_effect_inst_get_value(GPM, glitter_effect_inst* a1, float_t frame);
static void FASTCALL glitter_effect_inst_reset_sub(GPM, glitter_effect_inst* a1, float_t emission);

glitter_effect_inst* FASTCALL glitter_effect_inst_init(GPM,
    glitter_effect* a1, size_t id, float_t emission, bool appear_now) {
    glitter_effect_inst_ext_anim* inst_ext_anim;
    glitter_effect_ext_anim* ext_anim;
    glitter_emitter** i;
    glitter_emitter_inst* emitter_inst;

    glitter_effect_inst* ei = force_malloc(sizeof(glitter_effect_inst));
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = vec4_identity;
    ei->scale_all = 1.0f;
    ei->id = id;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    glitter_random_reset(&ei->random_shared);
    glitter_random_reset(&ei->random);

    if (appear_now)
        ei->frame = 0.0f;
    else
        ei->frame = (float_t)-ei->data.appear_time;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && ei->data.ext_anim) {
        inst_ext_anim = force_malloc(sizeof(glitter_effect_inst_ext_anim));
        inst_ext_anim->dword00 = -1;
        inst_ext_anim->object_hash = glt_type != GLITTER_AFT
            ? hash_murmurhash_empty : hash_fnv1a64_empty;
        inst_ext_anim->chara_index = -1;
        inst_ext_anim->node_index = -1;
        inst_ext_anim->mat = mat4_identity;

        if (inst_ext_anim) {
            ext_anim = ei->data.ext_anim;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_EXT_ANIM_ONCE)
                ei->flags |= GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_EXT_ANIM_TRANS_ONLY)
                ei->flags |= GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->index;
                inst_ext_anim->node_index = glt_type == GLITTER_AFT
                    ? glitter_aft_effect_inst_get_ext_anim_bone_index(ext_anim->node_index)
                    : glitter_f2_effect_inst_get_ext_anim_bone_index(ext_anim->node_index);
                ei->flags |= GLITTER_EFFECT_INST_CHARA_ANIM;
            }
            else {
                inst_ext_anim->object_hash = ext_anim->object_hash;
                inst_ext_anim->some_hash = ext_anim->some_hash;
                inst_ext_anim->instance_id = ext_anim->instance_id;
            }

            size_t name_len = strlen(ext_anim->mesh_name);
            if (name_len) {
                inst_ext_anim->name = force_malloc(name_len + 1);
                memcpy(inst_ext_anim->name, ext_anim->mesh_name, name_len);
            }
            else
                inst_ext_anim->name = 0;
        }
        ei->ext_anim = inst_ext_anim;
        ei->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM;
        ei->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
        ei->flags |= GLITTER_EFFECT_INST_SET_EXT_ANIM_BY_OBJECT_NAME;
    }

    glitter_random_set_value(&ei->random_shared, ei->data.name_hash % glitter_random_get_max(GPM_VAL));
    ei->random.value = glitter_random_get_value(&ei->random_shared);

    ei->mat = mat4_identity;
    vector_ptr_glitter_emitter_inst_append(&ei->emitters, a1->emitters.end - a1->emitters.begin);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter_inst = glitter_emitter_inst_init(GPM_VAL, *i, ei, emission);
        vector_ptr_glitter_emitter_inst_push_back(&ei->emitters, &emitter_inst);
    }
    glitter_effect_inst_reset_sub(GPM_VAL, ei, emission);
    return ei;
}

void FASTCALL glitter_effect_inst_calc_draw(GPM, glitter_effect_inst* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    glitter_render_group** i;
    glitter_render_group* render_group;

    for (i = a1->render_groups.begin; i != a1->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        if (render_group && (!glitter_render_group_cannot_draw(render_group) || gpm->draw_all))
            glitter_render_group_calc_draw(GPM_VAL, render_group, render_add_list_func);
    }
}

void FASTCALL glitter_effect_inst_draw(GPM, glitter_effect_inst* a1, int32_t alpha) {
    glitter_render_group** i;
    glitter_render_group* render_group;

    for (i = a1->render_groups.begin; i != a1->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        if (render_group && (render_group)->alpha == alpha
            && (!glitter_render_group_cannot_draw(render_group) || gpm->draw_all))
            glitter_render_group_draw(GPM_VAL, render_group);
    }
}

int32_t FASTCALL glitter_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return (a1->data.flags & GLITTER_EFFECT_ALPHA) ? 2 : 1;
}

size_t FASTCALL glitter_effect_inst_get_ctrl_count(glitter_effect_inst* a1, glitter_particle_type type) {
    glitter_render_group** i;
    glitter_render_group* render_group;
    size_t ctrl;

    ctrl = 0;
    for (i = a1->render_groups.begin; i != a1->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        if (render_group->type == type)
            ctrl += render_group->ctrl;
    }
    return ctrl;
}

size_t FASTCALL glitter_effect_inst_get_disp_count(glitter_effect_inst* a1, glitter_particle_type type) {
    glitter_render_group** i;
    glitter_render_group* render_group;
    size_t disp;

    disp = 0;
    for (i = a1->render_groups.begin; i != a1->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        if (render_group->type == type)
            disp += render_group->disp;
    }
    return disp;
}

int32_t FASTCALL glitter_effect_inst_get_fog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FOG)
        return 1;
    else if (a1->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        return 2;
    else
        return 0;
}

bool FASTCALL glitter_effect_inst_has_ended(glitter_effect_inst* effect, bool a2) {
    glitter_emitter_inst** i;

    if (~effect->flags & GLITTER_EFFECT_INST_FREE)
        return false;
    else if (!a2)
        return true;

    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (!glitter_emitter_inst_has_ended(*i, a2))
            return false;
    return true;
}

void FASTCALL glitter_effect_inst_reset(GPM, glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;

    a1->frame = (float_t)-a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        glitter_emitter_inst_reset(*i);
    glitter_effect_inst_reset_sub(GPM_VAL, a1, emission);
}

void FASTCALL glitter_effect_inst_update_value_frame(GPM,
    glitter_effect_inst* effect, float_t delta_frame, float_t emission) {
    vec3 scale;
    glitter_emitter_inst** i;
    glitter_render_group** j;
    mat4 mat;
    vec3 translation;
    vec3 rotation;

    glitter_effect_inst_get_ext_anim(GPM_VAL, effect);
    glitter_effect_inst_get_value(GPM_VAL, effect, effect->frame);
    vec3_mult_scalar(effect->scale, effect->scale_all, scale);
    rotation = effect->rotation;
    if (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && effect->ext_anim) {
        vec3_add(effect->translation, effect->ext_anim->translation, translation);
        mat4_translate_mult(&effect->ext_anim->mat, translation.x, translation.y, translation.z, &mat);
    }
    else {
        translation = effect->translation;
        mat4_translate(translation.x, translation.y, translation.z, &mat);
    }

    mat4_rot(&mat, rotation.x, rotation.y, rotation.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &effect->mat);
    for (i = effect->emitters.begin; i != effect->emitters.end; ++i)
        if (*i)
            glitter_emitter_inst_update_value_frame(GPM_VAL, *i, effect, delta_frame);

    if (effect->frame >= 0.0f)
        if (effect->frame < (float_t)effect->data.life_time) {
            for (i = effect->emitters.begin; i != effect->emitters.end; i++)
                if (*i)
                    glitter_emitter_inst_emit(GPM_VAL, *i, delta_frame, emission);
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOOP)
            effect->frame -= (float_t)effect->data.life_time;
        else {
            effect->flags |= GLITTER_EFFECT_INST_FREE;
            for (i = effect->emitters.begin; i != effect->emitters.end; ++i)
                if (*i)
                    glitter_emitter_inst_free(GPM_VAL, *i, emission);
        }

    for (j = effect->render_groups.begin; j != effect->render_groups.end; j++)
        if (*j)
            glitter_render_group_get_value(GPM_VAL, *j, delta_frame, true);
    effect->frame += delta_frame;
}

void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->ext_anim) {
        free(ei->ext_anim->name);
        free(ei->ext_anim);
    }

    vector_ptr_glitter_render_group_free(&ei->render_groups, glitter_render_group_dispose);
    vector_ptr_glitter_emitter_inst_free(&ei->emitters, glitter_emitter_inst_dispose);
    free(ei);
}

static void FASTCALL glitter_effect_inst_get_ext_anim(GPM, glitter_effect_inst* a1) {
    glitter_effect_inst_ext_anim* ext_anim;

    if (!a1->ext_anim)
        return;

    ext_anim = a1->ext_anim;
    ext_anim->mat = mat4_identity;
    ext_anim->translation = vec3_null;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    /*char* v4;
    int64_t v6;
    mat4* v15;
    int64_t v24;
    int32_t chara_index;
    int64_t v43;
    vec3 a2;
    mat4 mat;

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

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        v4 = sub_1405320E0();
        if (v4 && sub_1405327B0(v4, ext_anim->chara_index) && (v6 = sub_140532030(v4, ext_anim->chara_index))) {
            mat = *sub_140516750(v6);
            vec4_length(mat.row0, a2.x);
            vec4_length(mat.row1, a2.y);
            vec2_sub(*(vec2*)&a2, vec2_identity, *(vec2*)&a2);
            a2.z = 0.0f;
            a1->ext_anim_scale = a2;
            a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;
            if (ext_anim->node_index == 201)
                goto set_mat;
            else if (v15 = sub_140516730(v6, ext_anim->node_index)) {
                mat4_mult(&mat, v15, &mat);
                goto set_mat;
            }
        }
    }
    else if (~a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_BY_OBJECT_NAME) {
        if (ext_anim->node_index < 0) {
            if (!ext_anim->name)
                return;

            ext_anim->node_index = sub_140459DE0(ext_anim->object_hash, ext_anim->name);
        }

        if (ext_anim->node_index >= 0) {
            v43 = sub_140459D40(ext_anim->object_hash, ext_anim->node_index);
            if (v43) {
                ext_anim->translation = *(vec3*)(v43 + 4);
                goto set_flags;
            }
        }
    }
    else {
        if (ext_anim->dword00 == -1 || !(v24 = sub_1401D5BC0(ext_anim->dword00,
            ext_anim->dword08, ext_anim->object_is_hrc))) {
            ext_anim->dword00 = sub_1401D6090(ext_anim->object_hash,
                &ext_anim->dword08, &ext_anim->object_is_hrc);
            if (ext_anim->dword00 == -1)
                return;

            ext_anim->node_index = -1;
            v24 = sub_1401D5BC0(ext_anim->dword00, ext_anim->dword08, ext_anim->object_is_hrc);
            if (!v24)
                return;
        }

        mat = mat4_identity;
        chara_index = sub_1401D5010(ext_anim->dword00);
        if (chara_index >= 0 && chara_index <= 5) {
            v4 = sub_1405320E0();
            if (sub_1405327B0(v4, chara_index) && (v6 = sub_140532030(v4, chara_index))) {
                mat = *sub_140516750(v6);
                vec4_length(mat.row0, a2.x);
                vec4_length(mat.row1, a2.y);
                vec2_sub(*(vec2*)&a2, vec2_identity, *(vec2*)&a2);
                a2.z = 0.0f;
                a1->ext_anim_scale = a2;
                a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;
            }
        }

        if (ext_anim->name) {
            if (ext_anim->node_index < 0)
                ext_anim->node_index = sub_140459DE0(ext_anim->object_hash, ext_anim->name);

            if (ext_anim->node_index >= 0) {
                v43 = sub_140459D40(ext_anim->object_hash, ext_anim->node_index);
                if (v43) {
                    mat4_mult(&mat, (mat4*)v24, &ext_anim->mat);
                    ext_anim->translation = *(vec3*)(v43 + 4);
                    goto set_flags;
                }
            }
        }
        else {
            mat4_mult(&mat, (mat4*)v24, &mat);
            goto set_mat;
        }
    }
    return;

set_mat:
    if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
        ext_anim->mat = mat4_identity;
        mat4_get_translation(&mat, ext_anim->translation);
    }
    else
        ext_anim->mat = mat;

set_flags:
    if (a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) {
        a1->flags &= ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS;
    }*/
}

static int32_t FASTCALL glitter_aft_effect_inst_get_ext_anim_bone_index(int32_t index) {
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

static int32_t FASTCALL glitter_f2_effect_inst_get_ext_anim_bone_index(int32_t index) {
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
        return 98;
    case 5:
        return 100;
    case 6:
        return 101;
    case 7:
        return 115;
    case 8:
        return 132;
    case 9:
        return 136;
    case 10:
        return 137;
    case 11:
        return 151;
    case 12:
        return 186;
    case 13:
        return 176;
    case 14:
        return 175;
    case 15:
        return 189;
    case 16:
        return 183;
    case 17:
        return 182;
    default:
        return 193;
    }
}

static void FASTCALL glitter_effect_inst_get_value(GPM, glitter_effect_inst* a1, float_t frame) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->effect->curve.end - a1->effect->curve.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->curve.begin[i];
        if (!glitter_curve_get_value(GPM_VAL, curve, frame, &value, a1->random.value + i, &a1->random_shared))
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
        /*case GLITTER_CURVE_COLOR_R:
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
            break;*/
        }
    }
}

static void FASTCALL glitter_effect_inst_reset_sub(GPM, glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;
    glitter_render_group** j;
    vec3 scale;
    float_t delta_frame;
    float_t start_time;

    if (a1->data.start_time <= 0)
        return;

    delta_frame = 2.0f;
    for (start_time = (float_t)a1->data.start_time; start_time > 0.0f; start_time -= delta_frame) {
        if (start_time < delta_frame)
            delta_frame -= start_time;

        glitter_effect_inst_get_value(GPM_VAL, a1, a1->frame);
        mat4_translate(a1->translation.x, a1->translation.y, a1->translation.z, &a1->mat);
        mat4_rot(&a1->mat, a1->rotation.x, a1->rotation.y, a1->rotation.z, &a1->mat);
        vec3_mult_scalar(a1->scale, a1->scale_all, scale);
        mat4_scale_rot(&a1->mat, scale.x, scale.y, scale.z, &a1->mat);

        for (i = a1->emitters.begin; i != a1->emitters.end; ++i) {
            if (!*i)
                continue;

            glitter_emitter_inst_update_value_init(GPM_VAL, *i, a1, delta_frame);
            glitter_emitter_inst_emit(GPM_VAL, *i, delta_frame, emission);
        }

        for (j = a1->render_groups.begin; j != a1->render_groups.end; j++)
            if (*j)
                glitter_render_group_get_value(GPM_VAL, *j, delta_frame, true);
        a1->frame += delta_frame;
    }
}
