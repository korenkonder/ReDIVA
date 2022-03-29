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

static void GlitterXEffectInst__ctrl_init(GlitterXEffectInst* a1, float_t emission);
static int32_t GlitterXEffectInst__get_ext_anim_bone_index(GPM,
    glitter_effect_ext_anim_chara_node node);
static void GlitterXEffectInst__get_ext_anim(GlitterXEffectInst* a1);
static void GlitterXEffectInst__get_value(GlitterXEffectInst* a1);

GlitterXEffectInst::GlitterXEffectInst(GPM, glitter_effect* a1,
    size_t id, float_t emission, bool appear_now)
    : GlitterEffectInst(GPM_VAL, GLITTER_X, a1, id, emission, appear_now) {
    mat_rot = mat4_identity;
    mat_rot_eff_rot = mat4_identity;
    glitter_x_random_reset(&random_shared);
    ext_anim = 0;
    ext_anim_scale = vec3_null;
    render_scene = {};

    if (data.flags & GLITTER_EFFECT_USE_SEED)
        random = data.seed;
    else
        random = glitter_x_counter_get(GPM_VAL);
    random_shared.value = random;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && data.ext_anim_x) {
        GlitterXEffectInst::ExtAnim* inst_ext_anim = new GlitterXEffectInst::ExtAnim;
        if (inst_ext_anim) {
            glitter_effect_ext_anim_x* ext_anim = data.ext_anim_x;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                enum_or(flags, GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                enum_or(flags, GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_X)
                enum_or(flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_X);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Y)
                enum_or(flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Y);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Z)
                enum_or(flags, GLITTER_EFFECT_INST_NO_EXT_ANIM_TRANS_Z);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_GET_THEN_UPDATE)
                enum_or(flags, GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);


            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->chara_index;
                inst_ext_anim->bone_index =
                    GlitterXEffectInst__get_ext_anim_bone_index(GPM_VAL, ext_anim->node_index);
                enum_or(flags, GLITTER_EFFECT_INST_CHARA_ANIM);
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
        ext_anim = inst_ext_anim;
        enum_or(flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM);
        enum_or(flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
        enum_or(flags, GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT);
    }

    emitters.reserve(a1->emitters.size());
    for (glitter_emitter*& i : a1->emitters) {
        if (!i)
            continue;

        GlitterXEmitterInst* emitter= new GlitterXEmitterInst(i, this, emission);
        if (emitter)
            emitters.push_back(emitter);
    }
    GlitterXEffectInst__ctrl_init(this, emission);
}

GlitterXEffectInst::~GlitterXEffectInst() {
    for (GlitterXEmitterInst*& i : emitters)
        delete i;

    delete ext_anim;
}

void GlitterXEffectInst::Ctrl(GPM, GLT, float_t delta_frame, float_t emission) {
    mat4 mat;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    GlitterXEffectInst__get_ext_anim(this);
    if (flags & GLITTER_EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE && !GPM_VAL->draw_all)
        return;

    GlitterXEffectInst__get_value(this);
    trans = translation;
    rot = rotation;
    vec3_mult_scalar(this->scale, scale_all, scale);

    if (flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && ext_anim) {
        vec3 ext_trans;
        ext_trans = ext_anim->translation;
        mat4_translate_mult(&ext_anim->mat, ext_trans.x, ext_trans.y, ext_trans.z, &mat);
        mat4_normalize_rotation(&mat, &mat_rot);
        mat4_clear_trans(&mat_rot, &mat_rot);
        mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &mat_rot_eff_rot);
        mat4_translate_mult(&mat, trans.x, trans.y, trans.z, &mat);
    }
    else {
        mat_rot = mat4_identity;
        mat4_rotate(rot.x, rot.y, rot.z, &mat_rot_eff_rot);
        mat4_translate(trans.x, trans.y, trans.z, &mat);
    }

    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &this->mat);
    for (GlitterXEmitterInst*& i : emitters)
        if (i)
            i->Ctrl(GPM_VAL, this, delta_frame);

    if (frame0 >= 0.0f)
        if (frame0 < (float_t)effect->data.life_time) {
            for (GlitterXEmitterInst*& i : emitters)
                if (i)
                    i->Emit(delta_frame, emission);
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOOP)
            frame0 -= (float_t)effect->data.life_time;
        else {
            enum_or(flags, GLITTER_EFFECT_INST_FREE);
            for (GlitterXEmitterInst*& i : emitters)
                if (i)
                    i->Free(emission, false);
        }

    render_scene.Ctrl(delta_frame, true);
    frame0 += delta_frame;
}

void GlitterXEffectInst::Disp(GPM, draw_pass_3d_type alpha) {
    render_scene.Disp(GPM_VAL, alpha);
}

void GlitterXEffectInst::Free(GPM, GLT, float_t emission, bool free) {
    enum_or(flags, GLITTER_EFFECT_INST_FREE);
    for (GlitterXEmitterInst*& i : emitters)
        if (i)
            i->Free(emission, free);
}

size_t GlitterXEffectInst::GetCtrlCount(glitter_particle_type type) {
    return render_scene.GetCtrlCount(type);
}

size_t GlitterXEffectInst::GetDispCount(glitter_particle_type type) {
    return render_scene.GetDispCount(type);
}

bool GlitterXEffectInst::HasEnded(bool a2) {
    if (~flags & GLITTER_EFFECT_INST_FREE)
        return false;
    else if (!a2)
        return true;

    for (GlitterXEmitterInst*& i : emitters)
        if (i)
            if (!i->HasEnded(a2))
                return false;
    return true;
}

/*
size_t GlitterXEffectInst::GetCtrlCount(glitter_particle_type type) override;
size_t GlitterXEffectInst::GetDispCount(glitter_particle_type type) override;

*/

void GlitterXEffectInst::Reset(GPM, GLT, float_t emission) {
    frame0 = -(float_t)data.appear_time;
    flags = (glitter_effect_inst_flag)0;
    for (GlitterXEmitterInst*& i : emitters)
        if (i)
            i->Reset();
    GlitterXEffectInst__ctrl_init(this, emission);
}

void GlitterXEffectInst::CalcDisp(GPM) {
    render_scene.CalcDisp(GPM_VAL);
}

GlitterXEffectInst::ExtAnim::ExtAnim() {
    object_index = -1;
    mesh_index = -1;
    a3da_id = -1;
    object_is_hrc = false;
    file_name_hash = hash_murmurhash_empty;
    object_hash = hash_murmurhash_empty;
    instance_id = 0;
    mesh_name = 0;
    mat = mat4_identity;
    translation = vec3_null;
}

GlitterXEffectInst::ExtAnim::~ExtAnim() {

}

static void GlitterXEffectInst__ctrl_init(GlitterXEffectInst* a1, float_t emission) {
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
            GlitterXEffectInst__get_value(a1);
            trans = a1->translation;
            rot = a1->rotation;
            vec3_mult_scalar(a1->scale, a1->scale_all, scale);

            mat4_translate(trans.x, trans.y, trans.z, &mat);
            mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
            mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
            for (GlitterXEmitterInst*& i : a1->emitters)
                if (i)
                    i->CtrlInit(a1, delta_frame);

            if (a1->frame0 < (float_t)a1->data.life_time) {
                for (GlitterXEmitterInst*& i : a1->emitters)
                    if (i)
                        i->Emit(delta_frame, emission);
            }
            else if (a1->data.flags & GLITTER_EFFECT_LOOP)
                a1->frame0 -= (float_t)a1->data.life_time;
            else {
                enum_or(a1->flags, GLITTER_EFFECT_INST_FREE);
                for (GlitterXEmitterInst*& i : a1->emitters)
                    if (i)
                        i->Emit(emission, false);
            }

            a1->render_scene.Ctrl(delta_frame, false);
        }

        a1->frame0 += delta_frame;
        a1->frame1 += delta_frame;
        start_time -= delta_frame;
    }
}

static int32_t GlitterXEffectInst__get_ext_anim_bone_index(GPM,
    glitter_effect_ext_anim_chara_node node) {
    if (node < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD || node > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
        return -1;

    bone_database* bone_data = (bone_database*)GPM_VAL->bone_data;
    std::vector<std::string>* motion_bone_names = 0;
    if (!bone_data || bone_data->skeleton.size() < 1
        || !bone_data->get_skeleton_motion_bones(
            bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), &motion_bone_names))
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

    const char* bone_name = bone_names[node];
    for (std::string& i : *motion_bone_names)
        if (!str_utils_compare(bone_name, i.c_str()))
            return (int32_t)(&i - motion_bone_names->data());
    return -1;
}

static void GlitterXEffectInst__get_ext_anim(GlitterXEffectInst* a1) {
    glitter_effect_ext_anim_x* ext_anim;
    GlitterXEffectInst::ExtAnim* inst_ext_anim;
    vec3 scale;
    mat4 temp;
    mat4* mat;
    vec3* trans;
    bool set_flags;

    if (!a1->ext_anim || !a1->data.ext_anim_x)
        return;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    ext_anim = a1->data.ext_anim_x;
    inst_ext_anim = a1->ext_anim;

    mat = 0;
    trans = 0;
    set_flags = false;

    if (ext_anim->flags & GLITTER_EFFECT_INST_CHARA_ANIM)
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        rob_chara* rob_chr = rob_chara_array_get(inst_ext_anim->chara_index);
        if (!rob_chr)
            return;

        temp = rob_chr->data.adjust_data.mat;
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
            mat = auth_3d_data_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
                inst_ext_anim->object_index, inst_ext_anim->object_is_hrc, &temp);
        
        if (!mat) {
            inst_ext_anim->a3da_id = auth_3d_data_get_auth_3d_id_by_hash(
                inst_ext_anim->file_name_hash, inst_ext_anim->object_hash,
                &inst_ext_anim->object_index, &inst_ext_anim->object_is_hrc, inst_ext_anim->instance_id);
            if (inst_ext_anim->a3da_id == -1)
                return;

            inst_ext_anim->mesh_index = -1;
            mat = auth_3d_data_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
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
            inst_ext_anim->mesh_index = object_storage_get_obj_mesh_index_by_hash(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_name);

        if (inst_ext_anim->mesh_index != -1) {
            obj_mesh* mesh = object_storage_get_obj_mesh_by_object_hash_index(
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

            inst_ext_anim->mesh_index = object_storage_get_obj_mesh_index_by_hash(
                inst_ext_anim->object_hash, inst_ext_anim->mesh_name);
        }

        if (inst_ext_anim->mesh_index != -1) {
            obj_mesh* mesh = object_storage_get_obj_mesh_by_object_hash_index(
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

static void GlitterXEffectInst__get_value(GlitterXEffectInst* a1) {
    int64_t length;
    GlitterCurve* curve;
    float_t value;

    length = a1->effect->animation.curves.size();
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->animation.curves.data()[i];
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
