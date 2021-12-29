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
#include "../auth_3d.h"
#include "../rob.h"

static void glitter_effect_inst_get_ext_anim(glitter_effect_inst* a1);
static int32_t glitter_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node index);
static void glitter_effect_inst_get_value(GLT, glitter_effect_inst* a1);
static void glitter_effect_inst_update_init(GPM, GLT, glitter_effect_inst* a1, float_t emission);

glitter_effect_inst* glitter_effect_inst_init(GPM, GLT,
    glitter_effect* a1, size_t id, float_t emission, bool appear_now) {
    glitter_effect_inst_ext_anim* inst_ext_anim;
    glitter_effect_ext_anim* ext_anim;
    glitter_emitter** i;
    glitter_emitter_inst* emitter;

    glitter_effect_inst* ei = force_malloc(sizeof(glitter_effect_inst));
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = vec4u_identity;
    ei->scale_all = 1.0f;
    ei->id = id;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    ei->random_ptr = &GPM_VAL->random;

    if (appear_now)
        ei->data.appear_time = 0;
    else
        ei->frame0 = (float_t)-ei->data.appear_time;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && ei->data.ext_anim) {
        inst_ext_anim = force_malloc(sizeof(glitter_effect_inst_ext_anim));
        inst_ext_anim->a3da_id = -1;
        inst_ext_anim->object_hash = GLT_VAL != GLITTER_FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty;
        inst_ext_anim->chara_index = -1;
        inst_ext_anim->mesh_index = -1;
        inst_ext_anim->mat = mat4_identity;
        inst_ext_anim->translation = vec3_null;

        if (inst_ext_anim) {
            ext_anim = ei->data.ext_anim;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                ei->flags |= GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                ei->flags |= GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY;

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->bone_index
                    = glitter_effect_inst_get_ext_anim_bone_index(ext_anim->node_index);
                ei->flags |= GLITTER_EFFECT_INST_CHARA_ANIM;
            }
            else {
                inst_ext_anim->object = ext_anim->object;
                inst_ext_anim->object_hash = ext_anim->object_hash;
                inst_ext_anim->file_name_hash = ext_anim->file_name_hash;
                inst_ext_anim->instance_id = ext_anim->instance_id;
            }

            size_t mesh_name_len = utf8_length(ext_anim->mesh_name);
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
        GLT_VAL == GLITTER_FT ? (uint32_t)ei->data.name_hash : 0);
    ei->random = glitter_random_get_value(ei->random_ptr);

    ei->mat = mat4_identity;
    vector_ptr_glitter_emitter_inst_reserve(&ei->emitters, a1->emitters.end - a1->emitters.begin);
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

void glitter_effect_inst_calc_draw(GPM, glitter_effect_inst* a1) {
    glitter_render_scene_calc_draw(GPM_VAL, &a1->render_scene);
}

void glitter_effect_inst_draw(GPM, glitter_effect_inst* a1, draw_pass_3d_type alpha) {
    glitter_render_scene_draw(GPM_VAL, &a1->render_scene, alpha);
}

draw_pass_3d_type glitter_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return a1->data.flags & GLITTER_EFFECT_ALPHA ? DRAW_PASS_3D_TRANSPARENT : DRAW_PASS_3D_TRANSLUCENT;
}

size_t glitter_effect_inst_get_ctrl_count(glitter_effect_inst* a1, glitter_particle_type type) {
    return glitter_render_scene_get_ctrl_count(&a1->render_scene, type);
}

size_t glitter_effect_inst_get_disp_count(glitter_effect_inst* a1, glitter_particle_type type) {
    return glitter_render_scene_get_ctrl_count(&a1->render_scene, type);
}

fog_type glitter_effect_inst_get_fog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FOG)
        return FOG_DEPTH;
    else if (a1->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        return FOG_HEIGHT;
    else
        return -1;
}

bool glitter_effect_inst_has_ended(glitter_effect_inst* effect, bool a2) {
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

void glitter_effect_inst_reset(GPM, GLT, glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;

    a1->frame0 = (float_t)-a1->data.appear_time;
    a1->flags = 0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        if (*i)
            glitter_emitter_inst_reset(*i);
    glitter_effect_inst_update_init(GPM_VAL, GLT_VAL, a1, emission);
}

void glitter_effect_inst_update(GPM, GLT,
    glitter_effect_inst* effect, float_t delta_frame, float_t emission) {
    glitter_emitter_inst** i;
    mat4 mat;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    glitter_effect_inst_get_ext_anim(effect);
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

void glitter_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->ext_anim) {
        free(ei->ext_anim->mesh_name);
        free(ei->ext_anim);
    }

    glitter_render_scene_free(&ei->render_scene);
    vector_ptr_glitter_emitter_inst_free(&ei->emitters, glitter_emitter_inst_dispose);
    free(ei);
}

static void glitter_effect_inst_get_ext_anim(glitter_effect_inst* a1) {
    if (!a1->ext_anim || !a1->data.ext_anim)
        return;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    glitter_effect_inst_ext_anim* inst_ext_anim = a1->ext_anim;

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        rob_chara_data* rob_chr_data = rob_chara_data_array_get(inst_ext_anim->chara_index);
        if (!rob_chr_data)
            return;

        mat4 mat = mat4_identity/*chara root mat*/;

        vec3 scale;
        mat4_get_scale(&mat, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;

        int32_t bone_index = inst_ext_anim->bone_index;
        if (bone_index == -1) {
            if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
                inst_ext_anim->mat = mat4_identity;
                mat4_get_translation(&mat, &inst_ext_anim->translation);
            }
            else
                inst_ext_anim->mat = mat;
        }
        else {
            mat4* bone_mat = rob_chara_data_get_bone_data_mat(rob_chr_data, bone_index);
            if (!bone_mat)
                return;

            mat4_mult(bone_mat, &mat, &mat);
            if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
                inst_ext_anim->mat = mat4_identity;
                mat4_get_translation(&mat, &inst_ext_anim->translation);
            }
            else
                inst_ext_anim->mat = mat;

        }
        goto SetFlags;
    }

    if (~a1->flags & GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT) {
        if (inst_ext_anim->mesh_index < 0) {
            if (!inst_ext_anim->mesh_name)
                return;

            inst_ext_anim->mesh_index = object_storage_get_object_mesh_index(
                inst_ext_anim->object, inst_ext_anim->mesh_name);
        }

        int32_t v46 = inst_ext_anim->mesh_index;
        if (v46 >= 0) {
            object_mesh* mesh = object_storage_get_object_mesh_by_index(inst_ext_anim->object, v46);
            if (mesh) {
                inst_ext_anim->translation = mesh->bounding_sphere.center;
                goto SetFlags;
            }
        }
        return;
    }

    mat4* obj_mat = 0;
    mat4 temp;
    if (inst_ext_anim->a3da_id != -1)
        obj_mat = auth_3d_data_struct_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
            inst_ext_anim->a3da_index, inst_ext_anim->object_is_hrc, &temp);

    if (!obj_mat) {
        inst_ext_anim->a3da_id = auth_3d_data_get_auth_3d_id_by_object_info(inst_ext_anim->object,
            &inst_ext_anim->a3da_index, &inst_ext_anim->object_is_hrc);
        if (inst_ext_anim->a3da_id == -1)
            return;

        inst_ext_anim->mesh_index = -1;
        obj_mat = auth_3d_data_struct_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
            inst_ext_anim->a3da_index, inst_ext_anim->object_is_hrc, &temp);
        if (!obj_mat)
            return;
    }

    mat4 mat = mat4_identity;
    int32_t chara_id = -1;// auth_3d_data_get_chara_id(inst_ext_anim->a3da_id);
    if (chara_id >= 0 && chara_id <= 5) {
        rob_chara_data* rob_chr_data = rob_chara_data_array_get(chara_id);
        if (rob_chr_data) {
            mat = mat4_identity/*chara root mat*/;

            vec3 scale;
            mat4_get_scale(&mat, &scale);
            vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
            a1->ext_anim_scale.z = 0.0f;
            a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE;
        }
    }

    if (inst_ext_anim->mesh_name) {
        if (inst_ext_anim->mesh_index < 0)
            inst_ext_anim->mesh_index = object_storage_get_object_mesh_index(
                inst_ext_anim->object, inst_ext_anim->mesh_name);

        if (inst_ext_anim->mesh_index >= 0) {
            object_mesh* mesh = object_storage_get_object_mesh_by_index(
                inst_ext_anim->object, inst_ext_anim->mesh_index);
            if (mesh) {
                mat4_mult(obj_mat, &mat, &mat);
                inst_ext_anim->mat = mat;
                inst_ext_anim->translation = mesh->bounding_sphere.center;
                goto SetFlags;
            }
        }
    }
    else {
        mat4_mult(obj_mat, &mat, &mat);
        if (a1->flags & GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
            inst_ext_anim->mat = mat4_identity;
            mat4_get_translation(&mat, &inst_ext_anim->translation);
        }
        else
            inst_ext_anim->mat = mat;
        goto SetFlags;
    }
    return;

SetFlags:
    if (a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT) {
        a1->flags &= ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT;
        a1->flags |= GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS;
    }
}

static int32_t glitter_effect_inst_get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node node) {
    /*if (node < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD || node > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
        return -1;

    const char* bone_names[] = {
        [GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD]            = "face_root",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH]           = "n_kuti_u",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY]           = "n_hara_cp",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST]           = "e_mune_cp",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER]   = "c_kata_l",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW]      = "j_ude_l_wj",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP]  = "e_ude_l_cp",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND]       = "n_naka_l_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER]  = "c_kata_r",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW]     = "j_ude_r_wj",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP] = "e_ude_r_cp",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND]      = "n_naka_r_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH]      = "n_momo_c_l_wj_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE]       = "n_hiza_l_wj_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE]        = "kl_toe_l_wj",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH]     = "n_momo_c_r_wj_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE]      = "n_hiza_r_wj_ex",
        [GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE]       = "kl_toe_r_wj"
    };

    return -1;*/

    switch (node) {
    case GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD:
        return 0x0F;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH:
        return 0x36;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY:
        return 0x00;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST:
        return 0x07;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER:
        return 0x6A;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW:
        return 0x6C;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP:
        return 0x6D;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND:
        return 0x7B;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER:
        return 0x8E;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW:
        return 0x90;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP:
        return 0x91;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND:
        return 0x9F;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH:
        return 0xC2;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE:
        return 0xB8;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE:
        return 0xB7;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH:
        return 0xC5;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE:
        return 0xBF;
    case GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE:
        return 0xBE;
    default:
        return -1;
    }
}

static void glitter_effect_inst_get_value(GLT, glitter_effect_inst* a1) {
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

static void glitter_effect_inst_update_init(GPM, GLT,
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
