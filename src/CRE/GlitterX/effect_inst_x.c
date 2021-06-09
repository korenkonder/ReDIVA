/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst_x.h"
#include "counter_x.h"
#include "curve_x.h"
#include "emitter_inst_x.h"
#include "random_x.h"
#include "render_scene_x.h"

static mot_bone_index_f FASTCALL glitter_x_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node node);
static void FASTCALL glitter_x_effect_inst_get_ext_anim(glitter_effect_inst* a1);
static void FASTCALL glitter_x_effect_inst_get_value(glitter_effect_inst* a1);
static void FASTCALL glitter_x_effect_inst_update_init(glitter_effect_inst* a1, float_t emission);

glitter_effect_inst* FASTCALL glitter_x_effect_inst_init(GPM, glitter_effect* a1,
    size_t id, float_t emission, bool appear_now) {
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
    glitter_x_random_reset(&ei->random_shared);

    if (ei->data.flags & GLITTER_EFFECT_USE_SEED)
        ei->random = glitter_x_counter_get(GPM_VAL);
    else
        ei->random = ei->data.seed;
    ei->random_shared.value = ei->random;

    ext_anim = ei->data.ext_anim;

    if (appear_now)
        ei->data.appear_time = 0;
    else
        ei->frame0 = (float_t)-ei->data.appear_time;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && ei->data.ext_anim) {
        inst_ext_anim = force_malloc(sizeof(glitter_effect_inst_ext_anim));
        inst_ext_anim->a3da_id = -1;
        inst_ext_anim->object_hash = hash_murmurhash_empty;
        inst_ext_anim->chara_index = -1;
        inst_ext_anim->index.data = -1;
        inst_ext_anim->mat = mat4_identity;
        inst_ext_anim = ei->ext_anim;
        if (inst_ext_anim != (glitter_effect_inst_ext_anim*)0x0) {
            ext_anim = ei->data.ext_anim;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                ei->flags |= GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                ei->flags |= GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_X)
                ei->flags |= GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_X;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Y)
                ei->flags |= GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Y;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Z)
                ei->flags |= GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Z;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_GET_THEN_UPDATE)
                ei->flags |= GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE;


            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->index;
                inst_ext_anim->index.f =
                    glitter_x_effect_inst_get_ext_anim_bone_index(ext_anim->node_index);
                ei->flags |= GLITTER_EFFECT_INST_CHARA_ANIM;
            }
            else {
                inst_ext_anim->object_hash = ext_anim->object_hash;
                inst_ext_anim->file_name_hash = ext_anim->file_name_hash;
                inst_ext_anim->instance_id = ext_anim->instance_id;
            }

            size_t mesh_name_len = strlen(ext_anim->mesh_name);
            if (mesh_name_len) {
                inst_ext_anim->mesh_name = force_malloc(mesh_name_len + 1);
                memcpy(inst_ext_anim->mesh_name, ext_anim->mesh_name, mesh_name_len);
            }
            else
                inst_ext_anim->mesh_name = 0;
        }
        ei->ext_anim = inst_ext_anim;
        ei->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM;
        ei->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
        ei->flags |= GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT;
    }

    vector_ptr_glitter_emitter_inst_append(&ei->emitters, a1->emitters.end - a1->emitters.begin);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter_inst = glitter_x_emitter_inst_init(*i, ei, emission);
        vector_ptr_glitter_emitter_inst_push_back(&ei->emitters, &emitter_inst);
    }
    glitter_x_effect_inst_update_init(ei, emission);
    return ei;
}

void FASTCALL glitter_x_effect_inst_calc_draw(GPM, glitter_effect_inst* a1,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    glitter_x_render_scene_calc_draw(GPM_VAL, &a1->render_scene, render_add_list_func);
}

void FASTCALL glitter_x_effect_inst_draw(GPM, glitter_effect_inst* a1, int32_t alpha) {
    glitter_x_render_scene_draw(GPM_VAL, &a1->render_scene, alpha);
}

int32_t FASTCALL glitter_x_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return a1->data.flags & GLITTER_EFFECT_ALPHA ? 2 : 1;
}

int32_t FASTCALL glitter_x_effect_inst_get_fog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FOG)
        return 1;
    else if (a1->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        return 2;
    else
        return 0;
}

bool FASTCALL glitter_x_effect_inst_has_ended(glitter_effect_inst* effect, bool a2) {
    glitter_emitter_inst** i;

    if (~effect->flags & GLITTER_EFFECT_INST_FREE)
        return false;
    else if (!a2)
        return true;

    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (*i)
            if (!glitter_x_emitter_inst_has_ended(*i, a2))
                return false;
    return true;
}

void FASTCALL glitter_x_effect_inst_reset(glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;

    a1->frame0 = (float_t)-a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        if (*i)
            glitter_x_emitter_inst_reset(*i);
    glitter_x_effect_inst_update_init(a1, emission);
}

void FASTCALL glitter_x_effect_inst_update(GPM, glitter_effect_inst* a1,
    float_t delta_frame, float_t emission) {
    glitter_effect_inst_ext_anim* ext_anim;
    glitter_emitter_inst** i;
    mat4 mat;
    mat4 mat_rot;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    glitter_x_effect_inst_get_ext_anim(a1);
    if (~a1->flags & GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE && !GPM_VAL->draw_all)
        return;

    glitter_x_effect_inst_get_value(a1);
    trans = a1->translation;
    rot = a1->rotation;
    vec3_mult_scalar(a1->scale, a1->scale_all, scale);

    if (a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && a1->ext_anim) {
        vec3 ext_trans;
        ext_anim = a1->ext_anim;
        ext_trans = ext_anim->translation;
        mat4_translate_mult(&ext_anim->mat, ext_trans.x, ext_trans.y, ext_trans.z, &mat);
        mat4_normalize_rotation(&mat, &mat_rot);
        mat4_clear_trans(&mat_rot, &mat_rot);
        a1->mat_rot = mat_rot;
        mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &a1->mat_rot_eff_rot);
        mat4_translate_mult(&mat, trans.x, trans.y, trans.z, &mat);
    }
    else {
        a1->mat_rot = mat4_identity;
        mat4_rotate(rot.x, rot.y, rot.z, &a1->mat_rot_eff_rot);
        mat4_translate(trans.x, trans.y, trans.z, &mat);
    }

    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        if (*i)
            glitter_x_emitter_inst_update(GPM_VAL, *i, a1, delta_frame);

    if (a1->frame0 >= 0.0f)
        if (a1->frame0 < (float_t)a1->data.life_time) {
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                if (*i)
                    glitter_x_emitter_inst_emit(*i, delta_frame, emission);
        }
        else if (a1->data.flags & GLITTER_EFFECT_LOOP)
            a1->frame0 -= (float_t)a1->data.life_time;
        else {
            a1->flags |= GLITTER_EFFECT_INST_FREE;
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                if (*i)
                    glitter_x_emitter_inst_free(*i, emission);
        }

    glitter_x_render_scene_update(&a1->render_scene, delta_frame, true);
    a1->frame0 += delta_frame;
}

void FASTCALL glitter_x_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->ext_anim) {
        free(ei->ext_anim->mesh_name);
        free(ei->ext_anim);
    }

    glitter_x_render_scene_free(&ei->render_scene);
    vector_ptr_glitter_emitter_inst_free(&ei->emitters, glitter_x_emitter_inst_dispose);
    free(ei);
}

static mot_bone_index_f FASTCALL glitter_x_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node node) {
    switch (node) {
    case GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD:
        return MOT_BONE_F_FACE_ROOT;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH:
        return MOT_BONE_F_N_KUTI_U;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY:
        return MOT_BONE_F_N_HARA_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST:
        return MOT_BONE_F_E_MUNE_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER:
        return MOT_BONE_F_C_KATA_L;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW:
        return MOT_BONE_F_J_UDE_L_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP:
        return MOT_BONE_F_E_UDE_L_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND:
        return MOT_BONE_F_N_NAKA_L_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER:
        return MOT_BONE_F_C_KATA_R;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW:
        return MOT_BONE_F_J_UDE_R_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP:
        return MOT_BONE_F_E_UDE_R_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND:
        return MOT_BONE_F_N_NAKA_R_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH:
        return MOT_BONE_F_N_MOMO_C_L_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE:
        return MOT_BONE_F_N_HIZA_L_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE:
        return MOT_BONE_F_KL_TOE_L_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH:
        return MOT_BONE_F_N_MOMO_C_R_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE:
        return MOT_BONE_F_N_HIZA_R_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE:
        return MOT_BONE_F_KL_TOE_R_WJ;
    default:
        return MOT_BONE_F_MAX;
    }
}

static void FASTCALL glitter_x_effect_inst_get_ext_anim(glitter_effect_inst* a1) {
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_inst_ext_anim* inst_ext_anim;
    vec3 scale;
    mat4 temp;
    mat4* mat;
    vec3* trans;
    bool set_flags;

    if (!a1->ext_anim || !a1->data.ext_anim)
        return;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    ext_anim = a1->data.ext_anim;
    inst_ext_anim = a1->ext_anim;

    mat = 0;
    trans = 0;
    set_flags = false;

    if (ext_anim->flags & GLITTER_EFFECT_INST_CHARA_ANIM)
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        if (inst_ext_anim->chara_index < 0 || inst_ext_anim->chara_index > 3)
            return;

        temp = mat4_identity/*chara root mat*/;
        mat4_get_scale((mat4*)&mat4_identity/*chara root mat*/, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;

        if (false/*check chara is visible*/
            || ~ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA)
            set_flags = true;
        else
            set_flags = false;

        if (inst_ext_anim->index.f < MOT_BONE_F_MAX)
            mat4_mult(&temp, (mat4*)&mat4_identity/*chara node mat*/, &temp);

        mat = &temp;
        goto SetMat;
    }
    else if (a1->flags & GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT) {
        if (inst_ext_anim->a3da_id == -1) {
            inst_ext_anim->a3da_id = -1;/*try get a3da id*/
            //fun(inst_ext_anim->file_name_hash, inst_ext_anim->object_hash,
            //    &inst_ext_anim->a3da_index, &inst_ext_anim->object_is_hrc)
            inst_ext_anim->index.data = -1;

            if (inst_ext_anim->a3da_id > -1)
                mat = (mat4*)&mat4_identity/*try get obj a3da mat*/;
        }
        else {
            mat = (mat4*)&mat4_identity/*try get obj a3da mat*/;
            if (!mat) {
                inst_ext_anim->a3da_id = -1;/*try get a3da id*/
                //fun(inst_ext_anim->file_name_hash, inst_ext_anim->object_hash,
                //    &inst_ext_anim->a3da_index, &inst_ext_anim->object_is_hrc)
                if (inst_ext_anim->a3da_id != -1)
                    mat = (mat4*)&mat4_identity/*try get obj a3da mat*/;
            }
        }

        if (!mat)
            return;

        mat4_get_scale(mat, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;

        if (!inst_ext_anim->mesh_name)
            goto SetMat;

        if (inst_ext_anim->index.data < 0)
            inst_ext_anim->index.data = -1/*try get mesh index*/;

        if (inst_ext_anim->index.data > -1) {
            trans = (vec3*)&vec3_null/*try get mesh trans*/;
            if (trans)
                goto SetMat;
        }
    }
    else if (inst_ext_anim->object_hash != hash_murmurhash_empty) {
        if (inst_ext_anim->index.data < 0) {
            if (!inst_ext_anim->mesh_name)
                return;

            inst_ext_anim->index.data = -1/*try get mesh index*/;
        }

        if (inst_ext_anim->index.data >= 0) {
            trans = (vec3*)&vec3_null/*try get mesh trans*/;
            if (trans)
                goto SetMat;
        }
    }
    return;

SetMat:
    inst_ext_anim->mat = mat4_identity;
    if (mat)
        if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY)
            mat4_get_translation(mat, &inst_ext_anim->translation);
        else
            inst_ext_anim->mat = *mat;

    if (a1->flags & GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_X) {
        inst_ext_anim->mat.row3.x = 0.0f;
        inst_ext_anim->translation.x = 0.0f;
    }
    else if (trans)
        inst_ext_anim->translation.x = trans->x;

    if (a1->flags & GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Y) {
        inst_ext_anim->mat.row3.y = 0.0f;
        inst_ext_anim->translation.y = 0.0f;
    }
    else if (trans)
        inst_ext_anim->translation.y = trans->y;

    if (a1->flags & GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Z) {
        inst_ext_anim->mat.row3.z = 0.0f;
        inst_ext_anim->translation.z = 0.0f;
    }
    else if (trans)
        inst_ext_anim->translation.z = trans->z;

    if (set_flags) {
        a1->flags &= ~GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE;
        a1->flags &= ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
    }
    a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS;
}

static void FASTCALL glitter_x_effect_inst_get_value(glitter_effect_inst* a1) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->effect->animation.end - a1->effect->animation.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->animation.begin[i];
        if (!glitter_x_curve_get_value(curve, a1->frame0,
            &value, a1->random + i, &a1->random_shared))
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

static void FASTCALL glitter_x_effect_inst_update_init(glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    mat4 mat;
    float_t delta_frame;
    float_t start_time;

    if (a1->data.start_time <= 0)
        return;

    delta_frame = 2.0f;
    start_time = a1->data.start_time - a1->frame1;
    while (start_time > 0.0f) {
        a1->flags |= GLITTER_EFFECT_INST_JUST_INIT;
        if (start_time < delta_frame)
            delta_frame -= start_time;

        if (a1->frame0 >= 0.0f) {
            glitter_x_effect_inst_get_value(a1);
            trans = a1->translation;
            rot = a1->rotation;
            vec3_mult_scalar(a1->scale, a1->scale_all, scale);

            mat4_translate(trans.x, trans.y, trans.z, &mat);
            mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
            mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                if (*i)
                    glitter_x_emitter_inst_update_init(*i, a1, delta_frame);

            if (a1->frame0 < (float_t)a1->data.life_time) {
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_x_emitter_inst_emit(*i, delta_frame, emission);
            }
            else if (a1->data.flags & GLITTER_EFFECT_LOOP)
                a1->frame0 -= (float_t)a1->data.life_time;
            else {
                a1->flags |= GLITTER_EFFECT_INST_FREE;
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_x_emitter_inst_free(*i, emission);
            }

            glitter_x_render_scene_update(&a1->render_scene, delta_frame, false);
        }

        a1->frame0 += delta_frame;
        a1->frame1 += delta_frame;
        start_time -= delta_frame;
    }
}