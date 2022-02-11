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
#include "../../KKdLib/str_utils.h"
#include "../auth_3d.h"
#include "../rob.h"

static void glitter_x_effect_inst_ctrl_init(glitter_effect_inst* a1, float_t emission);
static int32_t glitter_x_effect_inst_get_ext_anim_bone_index(GPM,
    glitter_effect_ext_anim_chara_node node);
static void glitter_x_effect_inst_get_ext_anim(glitter_effect_inst* a1);
static void glitter_x_effect_inst_get_value(glitter_effect_inst* a1);

glitter_effect_inst* glitter_x_effect_inst_init(GPM, glitter_effect* a1,
    size_t id, float_t emission, bool appear_now) {
    glitter_effect_inst_ext_anim_x* inst_ext_anim;
    glitter_effect_ext_anim_x* ext_anim;
    glitter_emitter** i;
    glitter_emitter_inst* emitter_inst;

    glitter_effect_inst* ei = force_malloc_s(glitter_effect_inst, 1);
    ei->effect = a1;
    ei->data = a1->data;
    ei->color = vec4u_identity;
    ei->scale_all = 1.0f;
    ei->id = id;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    glitter_x_random_reset(&ei->random_shared);

    if (ei->data.flags & GLITTER_EFFECT_USE_SEED)
        ei->random = ei->data.seed;
    else
        ei->random = glitter_x_counter_get(GPM_VAL);
    ei->random_shared.value = ei->random;

    ext_anim = ei->data.ext_anim_x;

    if (appear_now)
        ei->data.appear_time = 0;
    else
        ei->frame0 = (float_t)-ei->data.appear_time;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && ei->data.ext_anim_x) {
        inst_ext_anim = force_malloc_s(glitter_effect_inst_ext_anim_x, 1);
        inst_ext_anim->object_index = -1;
        inst_ext_anim->mesh_index = -1;
        inst_ext_anim->a3da_id = -1;
        inst_ext_anim->object_is_hrc = false;
        inst_ext_anim->file_name_hash = hash_murmurhash_empty;
        inst_ext_anim->object_hash = hash_murmurhash_empty;
        inst_ext_anim->instance_id = 0;
        inst_ext_anim->mesh_name = 0;
        inst_ext_anim->mat = mat4_identity;
        inst_ext_anim->translation = vec3_null;

        if (inst_ext_anim) {
            ext_anim = ei->data.ext_anim_x;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                enum_or(ei->flags, GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                enum_or(ei->flags, GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_X)
                enum_or(ei->flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_X);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Y)
                enum_or(ei->flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Y);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Z)
                enum_or(ei->flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Z);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_GET_THEN_UPDATE)
                enum_or(ei->flags, GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);


            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->chara_index;
                inst_ext_anim->bone_index =
                    glitter_x_effect_inst_get_ext_anim_bone_index(GPM_VAL, ext_anim->node_index);
                enum_or(ei->flags, GLITTER_EFFECT_INST_CHARA_ANIM);
            }
            else {
                inst_ext_anim->object_hash = (uint32_t)ext_anim->object_hash;
                inst_ext_anim->file_name_hash = (uint32_t)ext_anim->file_name_hash;
                inst_ext_anim->instance_id = ext_anim->instance_id;
            }

            if (ext_anim->mesh_name[0])
                inst_ext_anim->mesh_name = ext_anim->mesh_name;
            else
                inst_ext_anim->mesh_name = 0;
        }
        ei->ext_anim_x = inst_ext_anim;
        enum_or(ei->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM);
        enum_or(ei->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
        enum_or(ei->flags, GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT);
    }

    vector_ptr_glitter_emitter_inst_reserve(&ei->emitters, vector_length(a1->emitters));
    for (i = a1->emitters.begin; i != a1->emitters.end; i++) {
        if (!*i)
            continue;

        emitter_inst = glitter_x_emitter_inst_init(*i, ei, emission);
        vector_ptr_glitter_emitter_inst_push_back(&ei->emitters, &emitter_inst);
    }
    glitter_x_effect_inst_ctrl_init(ei, emission);
    return ei;
}

void glitter_x_effect_inst_calc_disp(GPM, glitter_effect_inst* a1) {
    glitter_x_render_scene_calc_disp(GPM_VAL, &a1->render_scene);
}

void glitter_x_effect_inst_disp(GPM, glitter_effect_inst* a1, draw_pass_3d_type alpha) {
    glitter_x_render_scene_disp(GPM_VAL, &a1->render_scene, alpha);
}

draw_pass_3d_type glitter_x_effect_inst_get_alpha(glitter_effect_inst* a1) {
    return a1->data.flags & GLITTER_EFFECT_ALPHA ? DRAW_PASS_3D_TRANSPARENT : DRAW_PASS_3D_TRANSLUCENT;
}

fog_id glitter_x_effect_inst_get_fog(glitter_effect_inst* a1) {
    if (a1->data.flags & GLITTER_EFFECT_FOG)
        return FOG_DEPTH;
    else if (a1->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        return FOG_HEIGHT;
    else
        return (fog_id)-1;
}

bool glitter_x_effect_inst_has_ended(glitter_effect_inst* effect, bool a2) {
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

void glitter_x_effect_inst_reset(glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;

    a1->frame0 = (float_t)-a1->data.appear_time;
    a1->flags = (glitter_effect_inst_flag)0;
    for (i = a1->emitters.begin; i != a1->emitters.end; i++)
        if (*i)
            glitter_x_emitter_inst_reset(*i);
    glitter_x_effect_inst_ctrl_init(a1, emission);
}

void glitter_x_effect_inst_ctrl(GPM, glitter_effect_inst* effect,
    float_t delta_frame, float_t emission) {
    glitter_effect_inst_ext_anim_x* ext_anim;
    glitter_emitter_inst** i;
    mat4 mat;
    mat4 mat_rot;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    glitter_x_effect_inst_get_ext_anim(effect);
    if (effect->flags & GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE && !GPM_VAL->draw_all)
        return;

    glitter_x_effect_inst_get_value(effect);
    trans = effect->translation;
    rot = effect->rotation;
    vec3_mult_scalar(effect->scale, effect->scale_all, scale);

    if (effect->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && effect->ext_anim_x) {
        vec3 ext_trans;
        ext_anim = effect->ext_anim_x;
        ext_trans = ext_anim->translation;
        mat4_translate_mult(&ext_anim->mat, ext_trans.x, ext_trans.y, ext_trans.z, &mat);
        mat4_normalize_rotation(&mat, &mat_rot);
        mat4_clear_trans(&mat_rot, &mat_rot);
        effect->mat_rot = mat_rot;
        mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &effect->mat_rot_eff_rot);
        mat4_translate_mult(&mat, trans.x, trans.y, trans.z, &mat);
    }
    else {
        effect->mat_rot = mat4_identity;
        mat4_rotate(rot.x, rot.y, rot.z, &effect->mat_rot_eff_rot);
        mat4_translate(trans.x, trans.y, trans.z, &mat);
    }

    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &effect->mat);
    for (i = effect->emitters.begin; i != effect->emitters.end; i++)
        if (*i)
            glitter_x_emitter_inst_ctrl(GPM_VAL, *i, effect, delta_frame);

    if (effect->frame0 >= 0.0f)
        if (effect->frame0 < (float_t)effect->data.life_time) {
            for (i = effect->emitters.begin; i != effect->emitters.end; i++)
                if (*i)
                    glitter_x_emitter_inst_emit(*i, delta_frame, emission);
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOOP)
            effect->frame0 -= (float_t)effect->data.life_time;
        else {
            enum_or(effect->flags, GLITTER_EFFECT_INST_FREE);
            for (i = effect->emitters.begin; i != effect->emitters.end; i++)
                if (*i)
                    glitter_x_emitter_inst_free(*i, emission);
        }

    glitter_x_render_scene_ctrl(&effect->render_scene, delta_frame, true);
    effect->frame0 += delta_frame;
}

void glitter_x_effect_inst_dispose(glitter_effect_inst* ei) {
    if (ei->ext_anim_x)
        free(ei->ext_anim_x);

    glitter_x_render_scene_free(&ei->render_scene);
    vector_ptr_glitter_emitter_inst_free(&ei->emitters, glitter_x_emitter_inst_dispose);
    free(ei);
}

static void glitter_x_effect_inst_ctrl_init(glitter_effect_inst* a1, float_t emission) {
    glitter_emitter_inst** i;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    mat4 mat;
    float_t delta_frame;
    float_t start_time;

    if (a1->data.start_time < 1)
        return;

    delta_frame = 2.0f;
    start_time = a1->data.start_time - a1->frame1;
    while (start_time > 0.0f) {
        enum_or(a1->flags, GLITTER_EFFECT_INST_JUST_INIT);
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
                    glitter_x_emitter_inst_ctrl_init(*i, a1, delta_frame);

            if (a1->frame0 < (float_t)a1->data.life_time) {
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_x_emitter_inst_emit(*i, delta_frame, emission);
            }
            else if (a1->data.flags & GLITTER_EFFECT_LOOP)
                a1->frame0 -= (float_t)a1->data.life_time;
            else {
                enum_or(a1->flags, GLITTER_EFFECT_INST_FREE);
                for (i = a1->emitters.begin; i != a1->emitters.end; i++)
                    if (*i)
                        glitter_x_emitter_inst_free(*i, emission);
            }

            glitter_x_render_scene_ctrl(&a1->render_scene, delta_frame, false);
        }

        a1->frame0 += delta_frame;
        a1->frame1 += delta_frame;
        start_time -= delta_frame;
    }
}

static int32_t glitter_x_effect_inst_get_ext_anim_bone_index(GPM,
    glitter_effect_ext_anim_chara_node node) {
    if (node < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD || node > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
        return -1;

    bone_database* bone_data = (bone_database*)GPM_VAL->bone_data;
    vector_string* motion_bone_names = 0;
    if (!bone_data || vector_length(bone_data->skeleton) < 1
        || !bone_database_get_skeleton_motion_bones(bone_data,
            (char*)bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), &motion_bone_names))
        return -1;

    static const char* bone_names[] = {
        "face_root",
        "n_kuti_u",
        "n_hara_cp",
        "e_mune_cp",
        "c_kata_l",
        "j_ude_l_wj",
        "e_ude_l_cp",
        "n_naka_l_ex",
        "c_kata_r",
        "j_ude_r_wj",
        "e_ude_r_cp",
        "n_naka_r_ex",
        "n_momo_c_l_wj_ex",
        "n_hiza_l_wj_ex",
        "kl_toe_l_wj",
        "n_momo_c_r_wj_ex",
        "n_hiza_r_wj_ex",
        "kl_toe_r_wj"
    };

    char* bone_name = (char*)bone_names[node];
    for (string* i = motion_bone_names->begin; i != motion_bone_names->end; i++)
        if (!str_utils_compare(bone_name, string_data(i)))
            return (int32_t)(i - motion_bone_names->begin);
    return -1;
}

static void glitter_x_effect_inst_get_ext_anim(glitter_effect_inst* a1) {
    glitter_effect_ext_anim_x* ext_anim;
    glitter_effect_inst_ext_anim_x* inst_ext_anim;
    vec3 scale;
    mat4 temp;
    mat4* mat;
    vec3* trans;
    bool set_flags;

    if (!a1->ext_anim_x || !a1->data.ext_anim_x)
        return;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    ext_anim = a1->data.ext_anim_x;
    inst_ext_anim = a1->ext_anim_x;

    mat = 0;
    trans = 0;
    set_flags = false;

    if (ext_anim->flags & GLITTER_EFFECT_INST_CHARA_ANIM)
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        rob_chara* rob_chr = rob_chara_array_get(inst_ext_anim->chara_index);
        if (!rob_chr)
            return;

        temp = mat4_identity/*chara root mat*/;
        mat4_get_scale(&temp, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE);

        if (rob_chr->data.field_0 & 0x01 || ~ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA)
            set_flags = true;
        else
            set_flags = false;

        int32_t bone_index = inst_ext_anim->bone_index;
        if (bone_index != -1) {
            mat4* bone_mat = rob_chara_get_bone_data_mat(rob_chr, bone_index);
            mat4_mult(bone_mat, &temp, &temp);
        }

        mat = &temp;
        goto SetMat;
    }
    else if (a1->flags & GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT) {
        if (inst_ext_anim->a3da_id != -1)
            mat = auth_3d_data_struct_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
                inst_ext_anim->object_index, inst_ext_anim->object_is_hrc, &temp);
        
        if (!mat) {
            inst_ext_anim->a3da_id = auth_3d_data_get_auth_3d_id_by_hash(
                inst_ext_anim->file_name_hash, inst_ext_anim->object_hash,
                &inst_ext_anim->object_index, &inst_ext_anim->object_is_hrc, inst_ext_anim->instance_id);
            if (inst_ext_anim->a3da_id == -1)
                return;

            inst_ext_anim->mesh_index = -1;
            mat = auth_3d_data_struct_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
                inst_ext_anim->object_index, inst_ext_anim->object_is_hrc, &temp);
            if (!mat)
                return;
        }

        mat4_get_scale(mat, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE);

        if (!inst_ext_anim->mesh_name)
            goto SetMat;

        if (inst_ext_anim->mesh_index == -1)
            inst_ext_anim->mesh_index = object_storage_get_object_mesh_index_by_hash(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_name);

        if (inst_ext_anim->mesh_index != -1) {
            object_mesh* mesh = object_storage_get_object_mesh_by_object_hash_index(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_index);
            if (mesh) {
                trans = &mesh->bounding_sphere.center;
                goto SetMat;
            }
        }
    }
    else if (inst_ext_anim->object_hash != hash_murmurhash_empty) {
        if (inst_ext_anim->mesh_index == -1) {
            if (!inst_ext_anim->mesh_name)
                return;

            inst_ext_anim->mesh_index = object_storage_get_object_mesh_index_by_hash(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_name);
        }

        if (inst_ext_anim->mesh_index != -1) {
            object_mesh* mesh = object_storage_get_object_mesh_by_object_hash_index(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_index);
            if (mesh) {
                trans = &mesh->bounding_sphere.center;
                goto SetMat;
            }
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
        enum_and(a1->flags, ~GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);
        enum_and(a1->flags, ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
    }
    enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS);
}

static void glitter_x_effect_inst_get_value(glitter_effect_inst* a1) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = vector_length(a1->effect->animation);
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
