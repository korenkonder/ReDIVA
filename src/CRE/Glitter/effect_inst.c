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
#include "../../KKdLib/str_utils.h"
#include "../auth_3d.h"
#include "../rob.h"

static void GlitterEffectInst__ctrl_init(GPM, GLT, GlitterF2EffectInst* a1, float_t emission);
static void GlitterEffectInst__get_ext_anim(GlitterF2EffectInst* a1);
static int32_t GlitterEffectInst__get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node index);
static void GlitterEffectInst__get_value(GLT, GlitterF2EffectInst* a1);

GlitterEffectInst::GlitterEffectInst(GPM, GLT, glitter_effect* a1,
    size_t id, float_t emission, bool appear_now) {
    effect = a1;
    data = a1->data;
    color = vec4u_identity;
    scale_all = 1.0f;
    this->id = id;
    translation = a1->translation;
    rotation = a1->rotation;
    scale = a1->scale;
    frame0 = 0.0f;
    frame1 = 0.0f;
    mat = mat4_identity;
    flags = (glitter_effect_inst_flag)0;
    random = 0;

    if (appear_now)
        data.appear_time = 0;
    else
        frame0 = -(float_t)data.appear_time;
}

GlitterEffectInst::~GlitterEffectInst() {

}

draw_pass_3d_type GlitterEffectInst::GetAlpha() {
    return data.flags & GLITTER_EFFECT_ALPHA ? DRAW_PASS_3D_TRANSPARENT : DRAW_PASS_3D_TRANSLUCENT;
}

fog_id GlitterEffectInst::GetFog() {
    if (data.flags & GLITTER_EFFECT_FOG)
        return FOG_DEPTH;
    else if (data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        return FOG_HEIGHT;
    else
        return (fog_id)-1;
}

GlitterF2EffectInst::GlitterF2EffectInst(GPM, GLT, glitter_effect* a1,
    size_t id, float_t emission, bool appear_now)
    : GlitterEffectInst(GPM_VAL, GLT_VAL, a1, id, emission, appear_now) {
    random_ptr = &GPM_VAL->random;
    ext_anim = 0;
    ext_anim_scale = vec3_null;

    if (~a1->data.flags & GLITTER_EFFECT_LOCAL && data.ext_anim) {
        GlitterF2EffectInst::ExtAnim* inst_ext_anim = new GlitterF2EffectInst::ExtAnim;
        if (inst_ext_anim) {
            glitter_effect_ext_anim* ext_anim = data.ext_anim;
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_SET_ONCE)
                enum_or(flags, GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE);
            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY)
                enum_or(flags, GLITTER_EFFECT_INST_EXT_ANIM_TRANS_ONLY);

            if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                inst_ext_anim->chara_index = ext_anim->chara_index;
                inst_ext_anim->bone_index
                    = GlitterEffectInst__get_ext_anim_bone_index(ext_anim->node_index);
                enum_or(flags, GLITTER_EFFECT_INST_CHARA_ANIM);
                inst_ext_anim->mesh_name = 0;
            }
            else {
                inst_ext_anim->object = ext_anim->object;

                if (ext_anim->mesh_name[0])
                    inst_ext_anim->mesh_name = ext_anim->mesh_name;
                else
                    inst_ext_anim->mesh_name = 0;
            }
        }
        ext_anim = inst_ext_anim;
        enum_or(flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM);
        enum_or(flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
        enum_or(flags, GLITTER_EFFECT_INST_FLAG_GET_EXT_ANIM_MAT);
    }

    glitter_random_set_value(random_ptr,
        GLT_VAL == GLITTER_FT ? (uint32_t)data.name_hash : 0);
    random = glitter_random_get_value(random_ptr);

    emitters.reserve(a1->emitters.size());
    for (glitter_emitter*& i : a1->emitters) {
        if (!i)
            continue;

        GlitterF2EmitterInst* emitter = new GlitterF2EmitterInst(i, this, emission);
        if (emitter)
            emitters.push_back(emitter);
    }
    GlitterEffectInst__ctrl_init(GPM_VAL, GLT_VAL, this, emission);
}

GlitterF2EffectInst::~GlitterF2EffectInst() {
    for (GlitterF2EmitterInst*& i : emitters)
        delete i;

    delete ext_anim;
}

void GlitterF2EffectInst::Ctrl(GPM, GLT, float_t delta_frame, float_t emission) {
    mat4 mat;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    GlitterEffectInst__get_ext_anim(this);
    GlitterEffectInst__get_value(GLT_VAL, this);
    if (flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS && ext_anim) {
        vec3_add(translation, ext_anim->translation, trans);
        mat4_translate_mult(&ext_anim->mat, trans.x, trans.y, trans.z, &mat);
    }
    else {
        trans = translation;
        mat4_translate(trans.x, trans.y, trans.z, &mat);
    }
    rot = rotation;
    vec3_mult_scalar(this->scale, scale_all, scale);

    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &this->mat);
    for (GlitterF2EmitterInst*& i : emitters)
        if (i)
            i->Ctrl(GPM_VAL, GLT_VAL, this, delta_frame);

    if (frame0 >= 0.0f)
        if (frame0 < (float_t)data.life_time) {
            for (GlitterF2EmitterInst*& i : emitters)
                if (i)
                    i->Emit(GPM_VAL, GLT_VAL, delta_frame, emission);
        }
        else if (data.flags & GLITTER_EFFECT_LOOP)
            frame0 -= (float_t)data.life_time;
        else {
            enum_or(flags, GLITTER_EFFECT_INST_FREE);
            for (GlitterF2EmitterInst*& i : emitters)
                if (i)
                    i->Free(GPM_VAL, GLT_VAL, emission, false);
        }

    render_scene.Ctrl(GLT_VAL, delta_frame);
    frame0 += delta_frame;
}

void GlitterF2EffectInst::Disp(GPM, draw_pass_3d_type alpha) {
    render_scene.Disp(GPM_VAL, alpha);
}

void GlitterF2EffectInst::Free(GPM, GLT, float_t emission, bool free) {
    enum_or(flags, GLITTER_EFFECT_INST_FREE);
    for (GlitterF2EmitterInst*& i : emitters)
        if (i)
            i->Free(GPM_VAL, GLT_VAL, emission, free);
}

size_t GlitterF2EffectInst::GetCtrlCount(glitter_particle_type type) {
    return render_scene.GetCtrlCount(type);
}

size_t GlitterF2EffectInst::GetDispCount(glitter_particle_type type) {
    return render_scene.GetDispCount(type);
}

bool GlitterF2EffectInst::HasEnded(bool a2) {
    if (~flags & GLITTER_EFFECT_INST_FREE)
        return false;
    else if (!a2)
        return true;

    for (GlitterF2EmitterInst*& i : emitters)
        if (i)
            if (!i->HasEnded(a2))
                return false;
    return true;
}

void GlitterF2EffectInst::Reset(GPM, GLT, float_t emission) {
    frame0 = -(float_t)data.appear_time;
    flags = (glitter_effect_inst_flag)0;
    for (GlitterF2EmitterInst*& i : emitters)
        if (i)
            i->Reset();
    GlitterEffectInst__ctrl_init(GPM_VAL, GLT_VAL, this, emission);
}

GlitterF2EffectInst::ExtAnim::ExtAnim() {
    object_index = -1;
    mesh_index = -1;
    a3da_id = -1;
    object_is_hrc = false;
    mesh_name = 0;
    mat = mat4_identity;
    translation = vec3_null;
}

GlitterF2EffectInst::ExtAnim::~ExtAnim() {

}

static void GlitterEffectInst__ctrl_init(GPM, GLT,
    GlitterF2EffectInst* a1, float_t emission) {
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
    for (; start_time > 0.0f; a1->frame0 += delta_frame, a1->frame1 += delta_frame, start_time -= delta_frame) {
        enum_or(a1->flags, GLITTER_EFFECT_INST_JUST_INIT);
        if (start_time < delta_frame)
            delta_frame -= start_time;

        if (a1->frame0 >= 0.0f) {
            GlitterEffectInst__get_value(GLT_VAL, a1);
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
            for (GlitterF2EmitterInst*& i : a1->emitters)
                if (i)
                    i->CtrlInit(GPM_VAL, GLT_VAL, a1, delta_frame);

            if (a1->frame0 < (float_t)a1->data.life_time) {
                for (GlitterF2EmitterInst*& i : a1->emitters)
                    if (i)
                        i->Emit(GPM_VAL, GLT_VAL, delta_frame, emission);
            }
            else if (a1->data.flags & GLITTER_EFFECT_LOOP)
                a1->frame0 -= (float_t)a1->data.life_time;
            else {
                enum_or(a1->flags, GLITTER_EFFECT_INST_FREE);
                for (GlitterF2EmitterInst*& i : a1->emitters)
                    if (i)
                        i->Free(GPM_VAL, GLT_VAL, emission, false);
            }

            a1->render_scene.Ctrl(GLT_VAL, delta_frame);
        }
    }
    a1->frame1 += delta_frame;
}

static void GlitterEffectInst__get_ext_anim(GlitterF2EffectInst* a1) {
    if (!a1->ext_anim || !a1->data.ext_anim)
        return;

    if (~a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM
        || (a1->flags & GLITTER_EFFECT_INST_SET_EXT_ANIM_ONCE
            && a1->flags & GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS))
        return;

    GlitterF2EffectInst::ExtAnim* inst_ext_anim = a1->ext_anim;

    mat4* obj_mat = 0;
    mat4 temp;
    mat4 mat;
    int32_t chara_id;

    if (a1->flags & GLITTER_EFFECT_INST_CHARA_ANIM) {
        rob_chara* rob_chr = rob_chara_array_get(inst_ext_anim->chara_index);
        if (!rob_chr)
            return;

        mat4 mat = rob_chr->data.adjust_data.mat;

        vec3 scale;
        mat4_get_scale(&mat, &scale);
        vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
        a1->ext_anim_scale.z = 0.0f;
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE);

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
            mat4* bone_mat = rob_chara_get_bone_data_mat(rob_chr, bone_index);
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
        if (inst_ext_anim->mesh_index == -1) {
            if (!inst_ext_anim->mesh_name)
                return;

            inst_ext_anim->mesh_index = object_storage_get_obj_mesh_index(
                inst_ext_anim->object, inst_ext_anim->mesh_name);
        }

        if (inst_ext_anim->mesh_index != -1) {
            obj_mesh* mesh = object_storage_get_obj_mesh_by_index(inst_ext_anim->object,
                inst_ext_anim->mesh_index);
            if (mesh) {
                inst_ext_anim->translation = mesh->bounding_sphere.center;
                goto SetFlags;
            }
        }
        return;
    }

    if (inst_ext_anim->a3da_id != -1)
        obj_mat = auth_3d_data_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
            inst_ext_anim->object_index, inst_ext_anim->object_is_hrc, &temp);

    if (!obj_mat) {
        inst_ext_anim->a3da_id = auth_3d_data_get_auth_3d_id_by_object_info(inst_ext_anim->object,
            &inst_ext_anim->object_index, &inst_ext_anim->object_is_hrc, 0);
        if (inst_ext_anim->a3da_id == -1)
            return;

        inst_ext_anim->mesh_index = -1;
        obj_mat = auth_3d_data_get_auth_3d_object_mat(inst_ext_anim->a3da_id,
            inst_ext_anim->object_index, inst_ext_anim->object_is_hrc, &temp);
        if (!obj_mat)
            return;
    }

    mat = mat4_identity;
    chara_id = auth_3d_data_get_chara_id(inst_ext_anim->a3da_id);
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        if (rob_chr) {
            mat = rob_chr->data.adjust_data.mat;

            vec3 scale;
            mat4_get_scale(&mat, &scale);
            vec3_sub_scalar(scale, 1.0f, a1->ext_anim_scale);
            a1->ext_anim_scale.z = 0.0f;
            enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_SCALE);
        }
    }

    if (inst_ext_anim->mesh_name) {
        if (inst_ext_anim->mesh_index == -1)
            inst_ext_anim->mesh_index = object_storage_get_obj_mesh_index(
                inst_ext_anim->object, inst_ext_anim->mesh_name);

        if (inst_ext_anim->mesh_index != -1) {
            obj_mesh* mesh = object_storage_get_obj_mesh_by_index(
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
        enum_and(a1->flags, ~GLITTER_EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
        enum_or(a1->flags, GLITTER_EFFECT_INST_HAS_EXT_ANIM_TRANS);
    }
}

static int32_t GlitterEffectInst__get_ext_anim_bone_index(
    glitter_effect_ext_anim_chara_node node) {
    if (node < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD || node > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
        return -1;

    bone_database* bone_data = (bone_database*)GPM_VAL.bone_data;
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

static void GlitterEffectInst__get_value(GLT, GlitterF2EffectInst* a1) {
    int64_t length;
    GlitterCurve* curve;
    float_t value;

    length = a1->effect->animation.curves.size();
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->effect->animation.curves.data()[i];
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
