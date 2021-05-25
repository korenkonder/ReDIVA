/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst.h"
#include "counter.h"
#include "curve.h"
#include "emitter_inst.h"
#include "random.h"
#include "render_scene.h"

static void FASTCALL glitter_effect_inst_get_ext_anim(GLT, glitter_effect_inst* a1);
static mot_bone_index_ft FASTCALL glitter_aft_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node index);
static mot_bone_index_f FASTCALL glitter_f2_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node index);
static void FASTCALL glitter_effect_inst_get_value(GLT, glitter_effect_inst* a1);
static void FASTCALL glitter_effect_inst_update_init(GPM, GLT, glitter_effect_inst* a1, float_t emission);

glitter_effect_inst* FASTCALL glitter_effect_inst_init(GPM, GLT, glitter_effect* a1,
    glitter_random* random, size_t id, float_t emission, bool appear_now) {
    glitter_effect_inst_ext_anim* inst_ext_anim;
    glitter_effect_ext_anim* ext_anim;
    glitter_emitter** i;
    glitter_emitter_inst* emitter;

    glitter_effect_inst* ei = force_malloc(sizeof(glitter_effect_inst));
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = vec4_identity;
    ei->scale_all = 1.0f;
    ei->id = id;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    ei->random_ptr = random;

    if (appear_now)
        ei->data.appear_time = 0;
    else
        ei->frame0 = (float_t)-ei->data.appear_time;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && ei->data.ext_anim) {
        inst_ext_anim = force_malloc(sizeof(glitter_effect_inst_ext_anim));
        inst_ext_anim->a3da_id = -1;
        inst_ext_anim->object_hash = glt_type != GLITTER_AFT
            ? hash_murmurhash_empty : hash_fnv1a64_empty;
        inst_ext_anim->chara_index = -1;
        inst_ext_anim->index.data = -1;
        inst_ext_anim->mat = mat4_identity;
        inst_ext_anim->translation = vec3_null;

        if (inst_ext_anim) {
            ext_anim = ei->data.ext_anim;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                ei->flags |= GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                ei->flags |= GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->index;
                if (glt_type == GLITTER_AFT)
                    inst_ext_anim->index.ft
                    = glitter_aft_effect_inst_get_ext_anim_bone_index(ext_anim->node_index);
                else
                    inst_ext_anim->index.f
                    = glitter_f2_effect_inst_get_ext_anim_bone_index(ext_anim->node_index);
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

    glitter_random_set_value(ei->random_ptr,
        GLT_VAL == GLITTER_AFT ? (uint32_t)ei->data.name_hash : 0);
    ei->random = glitter_random_get_value(ei->random_ptr);

    ei->mat = mat4_identity;
    vector_ptr_glitter_emitter_inst_append(&ei->emitters, a1->emitters.end - a1->emitters.begin);
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter = glitter_emitter_inst_init(*i, ei, emission);
        if (emitter)
            vector_ptr_glitter_emitter_inst_push_back(&ei->emitters, &emitter);
    }
    glitter_effect_inst_update_init(GPM_VAL, GLT_VAL, ei, emission);
    return ei;
}

void FASTCALL glitter_effect_inst_calc_draw(GPM, glitter_effect_inst* a1) {
    glitter_render_scene_calc_draw(GPM_VAL, &a1->render_scene);
}

void FASTCALL glitter_effect_inst_draw(GPM, glitter_effect_inst* a1, int32_t alpha) {
    glitter_render_scene_draw(GPM_VAL, &a1->render_scene, alpha);
}

int32_t FASTCALL glitter_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return a1->data.flags & GLITTER_EFFECT_ALPHA ? 2 : 1;
}

size_t FASTCALL glitter_effect_inst_get_ctrl_count(glitter_effect_inst* a1, glitter_particle_type type) {
    return glitter_render_scene_get_ctrl_count(&a1->render_scene, type);
}

size_t FASTCALL glitter_effect_inst_get_disp_count(glitter_effect_inst* a1, glitter_particle_type type) {
    return glitter_render_scene_get_ctrl_count(&a1->render_scene, type);
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
        if (*i)
            if (!glitter_emitter_inst_has_ended(*i, a2))
                return false;
    return true;
}

void FASTCALL glitter_effect_inst_reset(GPM, GLT, glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;

    a1->frame0 = (float_t)-a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        if (*i)
            glitter_emitter_inst_reset(*i);
    glitter_effect_inst_update_init(GPM_VAL, GLT_VAL, a1, emission);
}

void FASTCALL glitter_effect_inst_update(GPM, GLT,
    glitter_effect_inst* effect, float_t delta_frame, float_t emission) {
    glitter_emitter_inst** i;
    mat4 mat;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    glitter_effect_inst_get_ext_anim(GLT_VAL, effect);
    glitter_effect_inst_get_value(GLT_VAL, effect);
    if (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && effect->ext_anim) {
        vec3_add(effect->translation, effect->ext_anim->translation, trans);
        mat4_translate_mult(&effect->ext_anim->mat, trans.x, trans.y, trans.z, &mat);
    }
    else {
        trans = effect->translation;
        mat4_translate(trans.x, trans.y, trans.z, &mat);
    }
    rot = effect->rotation;
    vec3_mult_scalar(effect->scale, effect->scale_all, scale);

    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &effect->mat);
    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (*i)
            glitter_emitter_inst_update(GPM_VAL, GLT_VAL, *i, effect, delta_frame);

    if (effect->frame0 >= 0.0f)
        if (effect->frame0 < (float_t)effect->data.life_time) {
            for (i = effect->emitters.begin; i != effect->emitters.end; i++)
                if (*i)
                    glitter_emitter_inst_emit(GPM_VAL, GLT_VAL, *i, delta_frame, emission);
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOOP)
            effect->frame0 -= (float_t)effect->data.life_time;
        else {
            effect->flags |= GLITTER_EFFECT_INST_FREE;
            for (i = effect->emitters.begin; i != effect->emitters.end; i++)
                if (*i)
                    glitter_emitter_inst_free(GPM_VAL, GLT_VAL, *i, emission);
        }

    glitter_render_scene_update(GLT_VAL, &effect->render_scene, delta_frame);
    effect->frame0 += delta_frame;
}

void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->ext_anim) {
        free(ei->ext_anim->mesh_name);
        free(ei->ext_anim);
    }

    glitter_render_scene_free(&ei->render_scene);
    vector_ptr_glitter_emitter_inst_free(&ei->emitters, glitter_emitter_inst_dispose);
    free(ei);
}

static void FASTCALL glitter_effect_inst_get_ext_anim(GLT, glitter_effect_inst* a1) {
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_inst_ext_anim* inst_ext_anim;
    vec3 scale;
    mat4 temp;
    mat4* mat;
    vec3* trans;

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

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        if (inst_ext_anim->chara_index < 0
            || inst_ext_anim->chara_index > (GLT_VAL == GLITTER_AFT ? 5 : 2))
            return;


        temp = mat4_identity/*chara root mat*/;
        mat4_get_scale((mat4*)&mat4_identity/*chara root mat*/, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;

        if (glt_type == GLITTER_AFT
            ? (inst_ext_anim->index.ft < MOT_BONE_FT_MAX)
            : (inst_ext_anim->index.ft < MOT_BONE_F_MAX))
            mat4_mult(&temp, (mat4*)&mat4_identity/*chara node mat*/, &temp);

        mat = &temp;
        goto SetMat;
    }
    else if (a1->flags & GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT) {
        if (inst_ext_anim->a3da_id == -1) {
            inst_ext_anim->a3da_id = -1;/*try get a3da id*/
            //fun(inst_ext_anim->object_hash,
            //    &inst_ext_anim->a3da_index, &inst_ext_anim->object_is_hrc)
            inst_ext_anim->index.data = -1;

            if (inst_ext_anim->a3da_id > -1)
                mat = (mat4*)&mat4_identity/*try get obj a3da mat*/;
        }
        else {
            mat = (mat4*)&mat4_identity/*try get obj a3da mat*/;
            if (!mat) {
                inst_ext_anim->a3da_id = -1;/*try get a3da id*/
                //fun(inst_ext_anim->object_hash,
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
            if (trans) {
                inst_ext_anim->mat = *mat;
                inst_ext_anim->translation = *trans;
                goto SetFlags;
            }
        }
    }
    else if (inst_ext_anim->object_hash != (glt_type != GLITTER_AFT
        ? hash_murmurhash_empty : hash_fnv1a64_empty)) {
        if (inst_ext_anim->index.data < 0) {
            if (!inst_ext_anim->mesh_name)
                return;

            inst_ext_anim->index.data = -1/*try get mesh index*/;
        }

        if (inst_ext_anim->index.data >= 0) {
            trans = (vec3*)&vec3_null/*try get mesh trans*/;
            if (trans) {
                inst_ext_anim->translation = *trans;
                goto SetFlags;
            }
        }
    }
    return;

SetMat:
    if (mat)
        if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
            inst_ext_anim->mat = mat4_identity;
            mat4_get_translation(mat, &inst_ext_anim->translation);
        }
        else
            inst_ext_anim->mat = *mat;

SetFlags:
    a1->flags &= ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
    a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS;
}

static mot_bone_index_ft FASTCALL glitter_aft_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node node) {
    switch (node) {
    case GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD:
        return MOT_BONE_FT_FACE_ROOT;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH:
        return MOT_BONE_FT_N_KUTI_U;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY:
        return MOT_BONE_FT_N_HARA_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST:
        return MOT_BONE_FT_E_MUNE_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER:
        return MOT_BONE_FT_C_KATA_L;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW:
        return MOT_BONE_FT_J_UDE_L_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP:
        return MOT_BONE_FT_E_UDE_L_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND:
        return MOT_BONE_FT_N_NAKA_L_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER:
        return MOT_BONE_FT_C_KATA_R;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW:
        return MOT_BONE_FT_J_UDE_R_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP:
        return MOT_BONE_FT_E_UDE_R_CP;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND:
        return MOT_BONE_FT_N_NAKA_R_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH:
        return MOT_BONE_FT_N_MOMO_C_L_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE:
        return MOT_BONE_FT_N_HIZA_L_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE:
        return MOT_BONE_FT_KL_TOE_L_WJ;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH:
        return MOT_BONE_FT_N_MOMO_C_R_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE:
        return MOT_BONE_FT_N_HIZA_R_WJ_EX;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE:
        return MOT_BONE_FT_KL_TOE_R_WJ;
    default:
        return MOT_BONE_FT_MAX;
    }
}

static mot_bone_index_f FASTCALL glitter_f2_effect_inst_get_ext_anim_bone_index(
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

static void FASTCALL glitter_effect_inst_get_value(GLT, glitter_effect_inst* a1) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->effect->animation.end - a1->effect->animation.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->animation.begin[i];
        if (!glitter_curve_get_value(GLT_VAL, curve, a1->frame0,
            &value, a1->random + i, a1->random_ptr))
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

static void FASTCALL glitter_effect_inst_update_init(GPM, GLT,
    glitter_effect_inst* a1, float_t emission) {
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
    for (; start_time > 0.0f; a1->frame0 += delta_frame, a1->frame1 += delta_frame, start_time -= delta_frame) {
        a1->flags |= GLITTER_EFFECT_INST_JUST_INIT;
        if (start_time < delta_frame)
            delta_frame -= start_time;

        if (a1->frame0 >= 0.0f) {
            glitter_effect_inst_get_value(GLT_VAL, a1);
            if (a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && a1->ext_anim) {
                vec3_add(a1->translation, a1->ext_anim->translation, trans);
                mat4_translate_mult(&a1->ext_anim->mat, trans.x, trans.y, trans.z, &mat);
            }
            else {
                trans = a1->translation;
                mat4_translate(trans.x, trans.y, trans.z, &mat);
            }
            rot = a1->rotation;
            vec3_mult_scalar(a1->scale, a1->scale_all, scale);

            mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
            mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
            for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                if (*i)
                    glitter_emitter_inst_update_init(GPM_VAL, GLT_VAL, *i, a1, delta_frame);

            if (a1->frame0 < (float_t)a1->data.life_time) {
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_emitter_inst_emit(GPM_VAL, GLT_VAL, *i, delta_frame, emission);
            }
            else if (a1->data.flags & GLITTER_EFFECT_LOOP)
                a1->frame0 -= (float_t)a1->data.life_time;
            else {
                a1->flags |= GLITTER_EFFECT_INST_FREE;
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_emitter_inst_free(GPM_VAL, GLT_VAL, *i, emission);
            }

            glitter_render_scene_update(GLT_VAL, &a1->render_scene, delta_frame);
        }
    }
    a1->frame1 += delta_frame;
}
