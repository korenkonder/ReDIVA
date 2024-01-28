/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../rob/rob.hpp"
#include "../auth_3d.hpp"

namespace Glitter {
    EffectInst::EffectInst(GPM, GLT, Effect* eff,
        size_t id, bool appear_now, bool init) : data(GLT_VAL) {
        this->init = init;
        effect = eff;
        data = eff->data;
        color = 1.0f;
        scale_all = 1.0f;
        this->id = id;
        translation = eff->translation;
        rotation = eff->rotation;
        scale = eff->scale;
        frame0 = 0.0f;
        frame1 = 0.0f;
        mat = mat4_identity;
        flags = (EffectInstFlag)0;
        random = 0;
#if defined(CRE_DEV)
        req_frame = 0.0f;
#endif
        ext_color = 0.0f;
        ext_anim_scale = 0.0f;
        some_scale = -1.0f;

        if (appear_now)
            data.appear_time = 0;
        else
            frame0 = -(float_t)data.appear_time;
    }

    EffectInst::~EffectInst() {

    }

    bool EffectInst::GetExtAnimScale(vec3* ext_anim_scale, float_t* some_scale) {
        if (!(flags & EFFECT_INST_HAS_EXT_ANIM_SCALE))
            return false;

        if (ext_anim_scale)
            *ext_anim_scale = this->ext_anim_scale;
        if (some_scale)
            *some_scale = this->some_scale;
        return true;
    }

    void EffectInst::SetExtColor(bool set, float_t r, float_t g, float_t b, float_t a) {
        ext_color.x = r;
        ext_color.y = g;
        ext_color.z = b;
        ext_color.w = a;
        if (set)
            enum_or(flags, EFFECT_INST_SET_EXT_COLOR);
        else
            enum_and(flags, ~EFFECT_INST_SET_EXT_COLOR);
        enum_or(flags, EFFECT_INST_EXT_COLOR);
    }

    int32_t EffectInst::GetExtAnimBoneIndex(GPM, EffectExtAnimCharaNode node) {
        if (node < EFFECT_EXT_ANIM_CHARA_HEAD || node > EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
            return -1;

        bone_database* bone_data = (bone_database*)GPM_VAL->bone_data;
        const std::vector<std::string>* motion_bone_names = 0;
        if (!bone_data || bone_data->skeleton.size() < 1)
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

        return bone_data->get_skeleton_motion_bone_index(
            bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone_names[node]);
    }

    F2EffectInst::ExtAnim::ExtAnim() {
        object_index = -1;
        mesh_index = -1;
        a3da_id = -1;
        object_is_hrc = false;
        mesh_name = 0;
        mat = mat4_identity;
        translation = 0.0f;
    }

    F2EffectInst::ExtAnim::~ExtAnim() {

    }

    F2EffectInst::F2EffectInst(GPM, GLT, Effect* eff, size_t id,
        Scene* sc, bool appear_now, bool init)
        : EffectInst(GPM_VAL, GLT_VAL, eff, id, appear_now, init) {
        random_ptr = &Glitter::random;
        ext_anim = 0;

        if (!init && !(eff->data.flags & EFFECT_LOCAL) && data.ext_anim) {
            F2EffectInst::ExtAnim* inst_ext_anim = new F2EffectInst::ExtAnim;
            if (inst_ext_anim) {
                Effect::ExtAnim* ext_anim = data.ext_anim;
                if (ext_anim->flags & EFFECT_EXT_ANIM_SET_ONCE)
                    enum_or(flags, EFFECT_INST_SET_EXT_ANIM_ONCE);
                if (ext_anim->flags & EFFECT_EXT_ANIM_TRANS_ONLY)
                    enum_or(flags, EFFECT_INST_EXT_ANIM_TRANS_ONLY);

                if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
                    inst_ext_anim->chara_index = ext_anim->chara_index;
                    inst_ext_anim->bone_index = GetExtAnimBoneIndex(GPM_VAL, ext_anim->node_index);
                    enum_or(flags, EFFECT_INST_CHARA_ANIM);
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
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM);
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
            enum_or(flags, EFFECT_INST_GET_EXT_ANIM_MAT);
        }

        random_ptr->SetValue(GLT_VAL == Glitter::FT ? (uint32_t)data.name_hash : 0);
        random = random_ptr->GetValue();

        emitters.reserve(eff->emitters.size());
        for (Emitter*& i : eff->emitters) {
            if (!i)
                continue;

            F2EmitterInst* emitter = new F2EmitterInst(i, this, sc->emission);
            if (emitter)
                emitters.push_back(emitter);
        }
        
        if (!init)
            ResetInit(GPM_VAL, GLT_VAL, sc);
    }

    F2EffectInst::~F2EffectInst() {
        for (F2EmitterInst*& i : emitters)
            delete i;

        delete ext_anim;
    }

    void F2EffectInst::CalcDisp(GPM) {
        render_scene.CalcDisp(GPM_VAL);
    }

    void F2EffectInst::Copy(EffectInst* dst, float_t emission) {
        dst->frame0 = frame0;
        dst->frame1 = frame1;
        dst->color = color;
        dst->translation = translation;
        dst->rotation = rotation;
        dst->scale = scale;
        dst->mat = mat;
        dst->random = random;

        F2EffectInst* f2_dst = dynamic_cast<F2EffectInst*>(dst);
        if (f2_dst && emitters.size() == f2_dst->emitters.size()) {
            size_t count = emitters.size();
            for (size_t i = 0; i < count; i++)
                emitters.data()[i]->Copy(f2_dst->emitters.data()[i], emission);
        }
    }

    void F2EffectInst::Ctrl(GPM, GLT, float_t delta_frame, float_t emission) {
        GetExtAnim();
        GetValue(GLT_VAL);

        mat4 mat;
        if (GetExtAnimMat(&mat))
            mat4_mul_translate(&mat, &translation, &mat);
        else
            mat4_translate(&translation, &mat);

        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_scale_rot(&mat, &scale, &this->mat);

        for (F2EmitterInst*& i : emitters)
            if (i)
                i->Ctrl(GPM_VAL, GLT_VAL, this, delta_frame);

        if (frame0 >= 0.0f)
            if (frame0 < (float_t)data.life_time) {
                for (F2EmitterInst*& i : emitters)
                    if (i)
                        i->Emit(GPM_VAL, GLT_VAL, delta_frame, emission);
            }
            else if (data.flags & EFFECT_LOOP)
                frame0 -= (float_t)data.life_time;
            else
                Free(GPM_VAL, GLT_VAL, emission, false);

        render_scene.Ctrl(GLT_VAL, delta_frame);
        frame0 += delta_frame;
    }

    void F2EffectInst::Disp(GPM, DispType disp_type) {
        render_scene.Disp(GPM_VAL, disp_type);
    }

    void F2EffectInst::Free(GPM, GLT, float_t emission, bool free) {
        enum_or(flags, EFFECT_INST_FREE);
        for (F2EmitterInst*& i : emitters)
            if (i)
                i->Free(GPM_VAL, GLT_VAL, emission, free);
    }

    size_t F2EffectInst::GetCtrlCount(ParticleType type) {
        return render_scene.GetCtrlCount(type);
    }

    size_t F2EffectInst::GetDispCount(ParticleType type) {
        return render_scene.GetDispCount(type);
    }

    bool F2EffectInst::HasEnded(bool a2) {
        if (!(flags & EFFECT_INST_FREE))
            return false;
        else if (!a2)
            return true;

        for (F2EmitterInst*& i : emitters)
            if (i && !i->HasEnded(a2))
                return false;
        return true;
    }

    void F2EffectInst::Reset(GPM, GLT, Scene* sc) {
        frame0 = -(float_t)data.appear_time;
        frame1 = 0.0;

        flags = (EffectInstFlag)0;
        if (!(data.flags & EFFECT_LOCAL) && data.ext_anim) {
            if (ext_anim) {
                Effect::ExtAnim* ext_anim = data.ext_anim;
                if (ext_anim->flags & EFFECT_EXT_ANIM_SET_ONCE)
                    enum_or(flags, EFFECT_INST_SET_EXT_ANIM_ONCE);
                if (ext_anim->flags & EFFECT_EXT_ANIM_TRANS_ONLY)
                    enum_or(flags, EFFECT_INST_EXT_ANIM_TRANS_ONLY);
                if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM)
                    enum_or(flags, EFFECT_INST_CHARA_ANIM);
            }
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM);
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
            enum_or(flags, EFFECT_INST_GET_EXT_ANIM_MAT);
        }

        for (F2EmitterInst*& i : emitters)
            if (i)
                i->Reset();

        ResetInit(GPM_VAL, GLT_VAL, sc);
    }

    bool F2EffectInst::ResetCheckInit(GPM, GLT, Scene* sc, float_t* a3) {
        if (init)
            return ResetInit(GPM_VAL, GLT_VAL, sc, a3);
        return false;
    }

    void F2EffectInst::CtrlMat(GPM, GLT) {
        if (!(flags & EFFECT_INST_JUST_INIT))
            return;

        vec3 trans = translation;
        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        mat4 mat;
        mat4_translate(&trans, &mat);
        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_scale_rot(&mat, &scale, &mat);
        this->mat = mat;

        for (F2EmitterInst*& i : emitters)
            if (i)
                i->CtrlMat(GPM_VAL, GLT_VAL, this);

        enum_and(flags, ~EFFECT_INST_JUST_INIT);
    }

    DispType F2EffectInst::GetDispType() {
        if (data.flags & EFFECT_ALPHA)
            return DISP_ALPHA;
        return DISP_NORMAL;
    }

    void F2EffectInst::GetExtAnim() {
        if (!ext_anim || !data.ext_anim)
            return;

        if (!(flags & EFFECT_INST_HAS_EXT_ANIM) || (flags & EFFECT_INST_SET_EXT_ANIM_ONCE
                && flags & EFFECT_INST_HAS_EXT_ANIM_TRANS))
            return;

        int32_t chara_id;

        if (flags & EFFECT_INST_CHARA_ANIM) {
            rob_chara* rob_chr = rob_chara_array_get(ext_anim->chara_index);
            if (!rob_chr)
                return;

            mat4 mat = rob_chr->data.adjust_data.mat;

            vec3 scale;
            mat4_get_scale(&mat, &scale);
            ext_anim_scale = scale - 1.0f;
            ext_anim_scale.z = 0.0f;
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_SCALE);

            int32_t bone_index = ext_anim->bone_index;
            if (bone_index != -1) {
                mat4* bone_mat = rob_chr->get_bone_data_mat(bone_index);
                if (!bone_mat)
                    return;

                mat4_mul(bone_mat, &mat, &mat);
            }

            if (flags & EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
                ext_anim->mat = mat4_identity;
                mat4_get_translation(&mat, &ext_anim->translation);
            }
            else
                ext_anim->mat = mat;
            goto SetFlags;
        }
        else if (flags & EFFECT_INST_GET_EXT_ANIM_MAT) {
            const mat4* obj_mat = 0;
            if (ext_anim->a3da_id != -1)
                obj_mat = auth_3d_id(ext_anim->a3da_id).get_auth_3d_object_mat(
                    ext_anim->object_index, ext_anim->object_is_hrc);

            if (!obj_mat) {
                ext_anim->a3da_id = auth_3d_data_get_auth_3d_id(ext_anim->object,
                    &ext_anim->object_index, &ext_anim->object_is_hrc);
                if (ext_anim->a3da_id == -1)
                    return;

                ext_anim->mesh_index = -1;
                obj_mat = auth_3d_id(ext_anim->a3da_id).get_auth_3d_object_mat(
                    ext_anim->object_index, ext_anim->object_is_hrc);
                if (!obj_mat)
                    return;
            }

            mat4 mat = mat4_identity;
            chara_id = auth_3d_id(ext_anim->a3da_id).get_chara_id();
            if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
                rob_chara* rob_chr = rob_chara_array_get(chara_id);
                if (rob_chr) {
                    mat = rob_chr->data.adjust_data.mat;

                    vec3 scale;
                    mat4_get_scale(&mat, &scale);
                    ext_anim_scale = scale - 1.0f;
                    ext_anim_scale.z = 0.0f;
                    enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_SCALE);
                }
            }

            if (ext_anim->mesh_name) {
                if (ext_anim->mesh_index == -1)
                    ext_anim->mesh_index = object_storage_get_obj_mesh_index(
                        ext_anim->object, ext_anim->mesh_name);

                if (ext_anim->mesh_index != -1) {
                    obj_mesh* mesh = object_storage_get_obj_mesh_by_index(
                        ext_anim->object, ext_anim->mesh_index);
                    if (mesh) {
                        mat4_mul(obj_mat, &mat, &mat);
                        ext_anim->mat = mat;
                        ext_anim->translation = mesh->bounding_sphere.center;
                        goto SetFlags;
                    }
                }
            }
            else {
                mat4_mul(obj_mat, &mat, &mat);
                if (flags & EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
                    ext_anim->mat = mat4_identity;
                    mat4_get_translation(&mat, &ext_anim->translation);
                }
                else
                    ext_anim->mat = mat;
                goto SetFlags;
            }
        }
        else {
            if (ext_anim->mesh_index == -1) {
                if (!ext_anim->mesh_name)
                    return;

                ext_anim->mesh_index = object_storage_get_obj_mesh_index(
                    ext_anim->object, ext_anim->mesh_name);
            }

            if (ext_anim->mesh_index != -1) {
                obj_mesh* mesh = object_storage_get_obj_mesh_by_index(ext_anim->object,
                    ext_anim->mesh_index);
                if (mesh) {
                    ext_anim->translation = mesh->bounding_sphere.center;
                    goto SetFlags;
                }
            }
        }
        return;

    SetFlags:
        if (flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT) {
            enum_and(flags, ~EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_TRANS);
        }
    }

    bool F2EffectInst::GetExtAnimMat(mat4* mat) {
        if (!(flags & EFFECT_INST_HAS_EXT_ANIM_TRANS) || !ext_anim)
            return false;

        mat4_mul_translate(&ext_anim->mat, &ext_anim->translation, mat);
        return true;
    }

    void F2EffectInst::GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a) {
        if (!(flags & EFFECT_INST_EXT_COLOR))
            return;

        if (flags & EFFECT_INST_SET_EXT_COLOR) {
            if (ext_color.x >= 0.0f)
                r = ext_color.x;
            if (ext_color.y >= 0.0f)
                g = ext_color.y;
            if (ext_color.z >= 0.0f)
                b = ext_color.z;
            if (ext_color.w >= 0.0f)
                a = ext_color.w;
        }
        else {
            r = ext_color.x + r;
            g = ext_color.y + g;
            b = ext_color.z + b;
            a = ext_color.w + a;
        }

        if (r < 0.0f)
            r = 0.0f;
        if (g < 0.0f)
            g = 0.0f;
        if (b < 0.0f)
            b = 0.0f;
        if (a < 0.0f)
            a = 0.0f;
    }

    FogType F2EffectInst::GetFog() {
        if (data.flags & EFFECT_FOG)
            return Glitter::FOG_DEPTH;
        else if (data.flags & EFFECT_FOG_HEIGHT)
            return Glitter::FOG_HEIGHT;
        return Glitter::FOG_NONE;
    }

    void F2EffectInst::GetValue(GLT) {
        Animation* anim = &effect->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            float_t value;
            if (!curve->F2GetValue(GLT_VAL, frame0,
                &value, random + i, random_ptr))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                break;
            case CURVE_ROTATION_X:
                rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rotation.z = value;
                break;
            case CURVE_SCALE_X:
                scale.x = value;
                break;
            case CURVE_SCALE_Y:
                scale.y = value;
                break;
            case CURVE_SCALE_Z:
                scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                scale_all = value;
                break;
            case CURVE_COLOR_R:
                color.x = value;
                break;
            case CURVE_COLOR_G:
                color.y = value;
                break;
            case CURVE_COLOR_B:
                color.z = value;
                break;
            case CURVE_COLOR_A:
                color.w = value;
                break;
            }
        }
    }

    bool F2EffectInst::ResetInit(GPM, GLT, Glitter::Scene* sc, float_t* a3) {
        if (data.start_time < 1)
            return false;

        if (!init && sc->effect_group && sc->effect_group->scene
            && sc->effect_group->scene->Copy(this, sc)) {
            frame1 = (float_t)data.start_time;
            return false;
        }

        float_t delta_frame = 2.0f;
        float_t start_time = (float_t)data.start_time - frame1;
        bool ret = false;
        if (a3) {
            if (start_time >= *a3) {
                start_time = *a3;
                *a3 = 0.0f;
                ret = true;
            }
            else
                *a3 -= start_time;
        }

        while (start_time > 0.0f) {
            enum_or(flags, EFFECT_INST_JUST_INIT);
            if (start_time < delta_frame)
                delta_frame -= start_time;

            if (frame0 >= 0.0f) {
                GetValue(GLT_VAL);

                vec3 trans = translation;
                vec3 rot = rotation;
                vec3 scale = this->scale * scale_all;

                mat4 mat;
                mat4_translate(&trans, &mat);
                mat4_mul_rotate_zyx(&mat, &rot, &mat);
                mat4_scale_rot(&mat, &scale, &this->mat);

                for (F2EmitterInst*& i : emitters)
                    if (i)
                        i->CtrlInit(GPM_VAL, GLT_VAL, this, delta_frame);

                if (frame0 < (float_t)data.life_time) {
                    for (F2EmitterInst*& i : emitters)
                        if (i)
                            i->Emit(GPM_VAL, GLT_VAL, delta_frame, sc->emission);
                }
                else if (data.flags & EFFECT_LOOP)
                    frame0 -= (float_t)data.life_time;
                else
                    Free(GPM_VAL, GLT_VAL, sc->emission, false);

                render_scene.Ctrl(GLT_VAL, delta_frame);
            }

            frame0 += delta_frame;
            frame1 += delta_frame;
            start_time -= delta_frame;
        }

        CtrlMat(GPM_VAL, GLT_VAL);
        return ret;
    }

    XEffectInst::ExtAnim::ExtAnim() {
        object_index = -1;
        mesh_index = -1;
        a3da_id = -1;
        object_is_hrc = false;
        file_name_hash = hash_murmurhash_empty;
        object_hash = hash_murmurhash_empty;
        instance_id = 0;
        mesh_name = 0;
        mat = mat4_identity;
        translation = 0.0f;
    }

    XEffectInst::ExtAnim::~ExtAnim() {

    }

    XEffectInst::XEffectInst(GPM, Effect* eff, size_t id,
        Scene* sc, bool appear_now, bool init, uint8_t load_flags)
        : EffectInst(GPM_VAL, Glitter::X, eff, id, appear_now, init) {
        mat_rot = mat4_identity;
        mat_rot_eff_rot = mat4_identity;
        random_shared.XReset();
        ext_anim = 0;
        render_scene = {};

        counter.Increment();

        if (load_flags & 0x01)
            enum_or(flags, EFFECT_INST_FLAG_23);

        if (data.flags & EFFECT_USE_SEED)
            random = data.seed;
        else
            random = counter.GetValue();
        random_shared.value = random;

        if (!init && !(eff->data.flags & EFFECT_LOCAL) && data.ext_anim_x) {
            XEffectInst::ExtAnim* inst_ext_anim = new XEffectInst::ExtAnim;
            if (inst_ext_anim) {
                Effect::ExtAnimX* ext_anim = data.ext_anim_x;
                if (ext_anim->flags & EFFECT_EXT_ANIM_SET_ONCE)
                    enum_or(flags, EFFECT_INST_SET_EXT_ANIM_ONCE);
                if (ext_anim->flags & EFFECT_EXT_ANIM_TRANS_ONLY)
                    enum_or(flags, EFFECT_INST_EXT_ANIM_TRANS_ONLY);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_X)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_X);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_Y)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_Y);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_Z)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_Z);
                if (ext_anim->flags & EFFECT_EXT_ANIM_GET_THEN_UPDATE)
                    enum_or(flags, EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);

                if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
                    inst_ext_anim->chara_index = ext_anim->chara_index;
                    inst_ext_anim->bone_index = GetExtAnimBoneIndex(GPM_VAL, ext_anim->node_index);
                    enum_or(flags, EFFECT_INST_CHARA_ANIM);
                }
                else {
                    inst_ext_anim->object_hash = ext_anim->object_hash;
                    inst_ext_anim->file_name_hash = ext_anim->file_name_hash;
                    inst_ext_anim->instance_id = ext_anim->instance_id;
                }

                if (ext_anim->mesh_name[0])
                    inst_ext_anim->mesh_name = ext_anim->mesh_name;
                else
                    inst_ext_anim->mesh_name = 0;
            }
            ext_anim = inst_ext_anim;
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM);
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
            enum_or(flags, EFFECT_INST_GET_EXT_ANIM_MAT);
        }

        emitters.reserve(eff->emitters.size());
        for (Emitter*& i : eff->emitters) {
            if (!i)
                continue;

            XEmitterInst* emitter = new XEmitterInst(i, this, sc->emission);
            if (emitter)
                emitters.push_back(emitter);
        }

        if (!init)
            ResetInit(GPM_VAL, sc);
    }

    XEffectInst::~XEffectInst() {
        for (XEmitterInst*& i : emitters)
            delete i;

        delete ext_anim;
    }

    void XEffectInst::CalcDisp(GPM) {
        render_scene.CalcDisp(GPM_VAL);
    }

    void XEffectInst::Copy(EffectInst* dst, float_t emission) {
        dst->frame0 = frame0;
        dst->frame1 = frame1;
        dst->color = color;
        dst->translation = translation;
        dst->rotation = rotation;
        dst->scale = scale;
        dst->mat = mat;
        dst->mat = mat_rot;
        dst->random = random;

        XEffectInst* x_dst = dynamic_cast<XEffectInst*>(dst);
        if (x_dst && emitters.size() == x_dst->emitters.size()) {
            x_dst->mat_rot_eff_rot = mat_rot_eff_rot;

            size_t count = emitters.size();
            for (size_t i = 0; i < count; i++)
                emitters.data()[i]->Copy(x_dst->emitters.data()[i], emission);
        }
    }

    void XEffectInst::Ctrl(GPM, GLT, float_t delta_frame, float_t emission) {
        GetExtAnim();
        if (flags & EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE && !GPM_VAL->draw_all)
            return;

        GetValue();

        vec3& rot = rotation;

        mat4 mat;
        if (GetExtAnimMat(&mat)) {
            mat4_normalize_rotation(&mat, &mat_rot);
            mat4_clear_trans(&mat_rot, &mat_rot);

            mat_rot_eff_rot = mat_rot;
            mat4_mul_rotate_zyx(&mat_rot, &rot, &mat_rot);

            mat4_mul_translate(&mat, &translation, &mat);
        }
        else {
            mat_rot = mat4_identity;
            mat_rot_eff_rot = mat4_identity;
            mat4_rotate_zyx(&rot, &mat_rot);

            mat4_translate(&translation, &mat);
        }

        vec3 scale = this->scale * scale_all;

        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_scale_rot(&mat, &scale, &this->mat);

        for (XEmitterInst*& i : emitters)
            if (i)
                i->Ctrl(GPM_VAL, this, delta_frame);

        if (frame0 >= 0.0f)
            if (frame0 < (float_t)data.life_time) {
                for (XEmitterInst*& i : emitters)
                    if (i)
                        i->Emit(delta_frame, emission);
            }
            else if (data.flags & EFFECT_LOOP)
                frame0 -= (float_t)data.life_time;
            else
                Free(GPM_VAL, Glitter::X, emission, false);

        render_scene.Ctrl(delta_frame, true);
        frame0 += delta_frame;
    }

    void XEffectInst::Disp(GPM, DispType disp_type) {
        render_scene.Disp(GPM_VAL, disp_type);
    }

    void XEffectInst::Free(GPM, GLT, float_t emission, bool free) {
        enum_or(flags, EFFECT_INST_FREE);
        for (XEmitterInst*& i : emitters)
            if (i)
                i->Free(emission, free);
    }

    size_t XEffectInst::GetCtrlCount(ParticleType type) {
        return render_scene.GetCtrlCount(type);
    }

    size_t XEffectInst::GetDispCount(ParticleType type) {
        return render_scene.GetDispCount(type);
    }

    bool XEffectInst::HasEnded(bool a2) {
        if (!(flags & EFFECT_INST_FREE))
            return false;
        else if (!a2)
            return true;

        for (XEmitterInst*& i : emitters)
            if (i && !i->HasEnded(a2))
                return false;
        return true;
    }

    void XEffectInst::Reset(GPM, GLT, Scene* sc) {
        frame0 = -(float_t)data.appear_time;
        frame1 = 0.0;

        flags = (EffectInstFlag)0;
        if (!(data.flags & EFFECT_LOCAL) && data.ext_anim_x) {
            if (ext_anim) {
                Effect::ExtAnimX* ext_anim = data.ext_anim_x;
                if (ext_anim->flags & EFFECT_EXT_ANIM_SET_ONCE)
                    enum_or(flags, EFFECT_INST_SET_EXT_ANIM_ONCE);
                if (ext_anim->flags & EFFECT_EXT_ANIM_TRANS_ONLY)
                    enum_or(flags, EFFECT_INST_EXT_ANIM_TRANS_ONLY);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_X)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_X);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_Y)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_Y);
                if (ext_anim->flags & EFFECT_EXT_ANIM_NO_TRANS_Z)
                    enum_or(flags, EFFECT_INST_NO_EXT_ANIM_TRANS_Z);
                if (ext_anim->flags & EFFECT_EXT_ANIM_GET_THEN_UPDATE)
                    enum_or(flags, EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);
                if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM)
                    enum_or(flags, EFFECT_INST_CHARA_ANIM);
            }
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM);
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
            enum_or(flags, EFFECT_INST_GET_EXT_ANIM_MAT);
        }

        for (XEmitterInst*& i : emitters)
            if (i)
                i->Reset();

        ResetInit(GPM_VAL, sc);
    }

    bool XEffectInst::ResetCheckInit(GPM, GLT, Scene* sc, float_t* a3) {
        if (init)
            return ResetInit(GPM_VAL, sc, a3);
        return false;
    }

    void XEffectInst::CtrlMat(GPM) {
        if (!(flags & EFFECT_INST_JUST_INIT))
            return;

        vec3 trans = translation;
        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        mat4 mat;
        mat4_translate(&trans, &mat);
        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_scale_rot(&mat, &scale, &mat);
        this->mat = mat;

        for (XEmitterInst*& i : emitters)
            if (i)
                i->CtrlMat(GPM_VAL, this);

        enum_and(flags, ~EFFECT_INST_JUST_INIT);
    }

    DispType XEffectInst::GetDispType() {
        if (data.flags & EFFECT_LOCAL)
            return DISP_LOCAL;
        else if (data.flags & EFFECT_ALPHA)
            return DISP_ALPHA;
        else if (flags & EFFECT_INST_FLAG_23)
            return DISP_TYPE_2;
        return DISP_NORMAL;
    }

    void XEffectInst::GetExtAnim() {
        if (!ext_anim || !data.ext_anim_x)
            return;

        if (!(flags & EFFECT_INST_HAS_EXT_ANIM) || (flags & EFFECT_INST_SET_EXT_ANIM_ONCE
                && flags & EFFECT_INST_HAS_EXT_ANIM_TRANS))
            return;

        bool set_flags = false;

        if (data.ext_anim_x->flags & EFFECT_INST_CHARA_ANIM)
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_NON_INIT);

        if (flags & EFFECT_INST_CHARA_ANIM) {
            rob_chara* rob_chr = rob_chara_array_get(ext_anim->chara_index);
            if (!rob_chr)
                return;

            mat4* mat = &rob_chr->data.adjust_data.mat;

            vec3 scale;
            mat4_get_scale(mat, &scale);
            ext_anim_scale = scale - 1.0f;
            ext_anim_scale.z = 0.0f;
            enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_SCALE);

            if (rob_chr->is_visible() || !(data.ext_anim_x->flags & EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA))
                set_flags = true;
            else
                set_flags = false;

            int32_t bone_index = ext_anim->bone_index;
            if (bone_index != -1) {
                mat4* bone_mat = rob_chr->get_bone_data_mat(bone_index);
                if (bone_mat)
                    SetExtAnim(mat, bone_mat, 0, set_flags);
            }
            else
                SetExtAnim(mat, 0, 0, set_flags);
        }
        else if (flags & EFFECT_INST_GET_EXT_ANIM_MAT) {
            const mat4* obj_mat = 0;
            if (ext_anim->a3da_id != -1)
                obj_mat = auth_3d_id(ext_anim->a3da_id).get_auth_3d_object_mat(
                    ext_anim->object_index, ext_anim->object_is_hrc);

            if (!obj_mat) {
                ext_anim->a3da_id = auth_3d_data_get_auth_3d_id(
                    ext_anim->file_name_hash, ext_anim->object_hash,
                    &ext_anim->object_index, &ext_anim->object_is_hrc, ext_anim->instance_id);
                if (ext_anim->a3da_id == -1)
                    return;

                ext_anim->mesh_index = -1;
                obj_mat = auth_3d_id(ext_anim->a3da_id).get_auth_3d_object_mat(
                    ext_anim->object_index, ext_anim->object_is_hrc);
                if (!obj_mat)
                    return;
            }

            mat4 mat = mat4_identity;

            int32_t chara_id = auth_3d_id(ext_anim->a3da_id).get_chara_id();
            if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
                rob_chara* rob_chr = rob_chara_array_get(chara_id);
                if (rob_chr) {
                    mat = rob_chr->data.adjust_data.mat;

                    vec3 scale;
                    mat4_get_scale(&mat, &scale);
                    ext_anim_scale = scale - 1.0f;
                    ext_anim_scale.z = 0.0f;
                    enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_SCALE);
                }
            }

            if (ext_anim->mesh_name) {
                if (ext_anim->mesh_index == -1)
                    ext_anim->mesh_index = object_storage_get_obj_mesh_index_by_hash(
                        ext_anim->object_hash, ext_anim->mesh_name);

                if (ext_anim->mesh_index != -1) {
                    obj_mesh* mesh = object_storage_get_obj_mesh_by_object_hash_index(
                        ext_anim->object_hash, ext_anim->mesh_index);
                    if (mesh) {
                        vec3* trans = &mesh->bounding_sphere.center;
                        SetExtAnim(&mat, obj_mat, trans, true);
                    }
                }
            }
            else
                SetExtAnim(&mat, obj_mat, 0, true);

        }
        else if (ext_anim->object_hash != hash_murmurhash_empty) {
            if (ext_anim->mesh_index == -1) {
                if (!ext_anim->mesh_name)
                    return;

                ext_anim->mesh_index = object_storage_get_obj_mesh_index_by_hash(
                    ext_anim->object_hash, ext_anim->mesh_name);
            }

            if (ext_anim->mesh_index != -1) {
                obj_mesh* mesh = object_storage_get_obj_mesh_by_object_hash_index(
                    ext_anim->object_hash, ext_anim->mesh_index);
                if (mesh) {
                    vec3* trans = &mesh->bounding_sphere.center;
                    SetExtAnim(0, 0, trans, true);
                }
            }
        }
    }

    bool XEffectInst::GetExtAnimMat(mat4* mat) {
        if (!(flags & EFFECT_INST_HAS_EXT_ANIM_TRANS) || !ext_anim)
            return false;

        mat4_mul_translate(&ext_anim->mat, &ext_anim->translation, mat);
        return true;
    }

    void XEffectInst::GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a) {
        if (!(flags & EFFECT_INST_EXT_COLOR))
            return;

        if (flags & EFFECT_INST_SET_EXT_COLOR) {
            if (ext_color.x >= 0.0f)
                r = ext_color.x;
            if (ext_color.y >= 0.0f)
                g = ext_color.y;
            if (ext_color.z >= 0.0f)
                b = ext_color.z;
            if (ext_color.w >= 0.0f)
                a = ext_color.w;
        }
        else {
            if (ext_color.x >= 0.0f)
                r *= ext_color.x;
            if (ext_color.y >= 0.0f)
                g *= ext_color.y;
            if (ext_color.z >= 0.0f)
                b *= ext_color.z;
            if (ext_color.w >= 0.0f)
                a *= ext_color.w;
        }

        if (r < 0.0f)
            r = 0.0f;
        if (g < 0.0f)
            g = 0.0f;
        if (b < 0.0f)
            b = 0.0f;
        if (a < 0.0f)
            a = 0.0f;
    }
    FogType XEffectInst::GetFog() {
        if (data.flags & EFFECT_FOG)
            return Glitter::FOG_DEPTH;
        else if (data.flags & EFFECT_FOG_HEIGHT)
            return Glitter::FOG_HEIGHT;
        return Glitter::FOG_NONE;
    }

    void XEffectInst::GetValue() {
        Animation* anim = &effect->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            float_t value;
            if (!curve->XGetValue(frame0, &value, random + i, &random_shared))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                break;
            case CURVE_ROTATION_X:
                rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rotation.z = value;
                break;
            case CURVE_SCALE_X:
                scale.x = value;
                break;
            case CURVE_SCALE_Y:
                scale.y = value;
                break;
            case CURVE_SCALE_Z:
                scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                scale_all = value;
                break;
            //case CURVE_COLOR_R:
            //    color.x = value;
            //    break;
            //case CURVE_COLOR_G:
            //    color.y = value;
            //    break;
            //case CURVE_COLOR_B:
            //    color.z = value;
            //    break;
            //case CURVE_COLOR_A:
            //    color.w = value;
            //    break;
            }
        }
    }
    
    bool XEffectInst::ResetInit(GPM, Scene* sc, float_t* a3) {
        if (data.start_time < 1)
            return false;

        if (!init && sc->effect_group && sc->effect_group->scene
            && sc->effect_group->scene->Copy(this, sc)) {
            frame1 = (float_t)data.start_time;
            return false;
        }

        float_t delta_frame = 2.0f;
        float_t start_time = (float_t)data.start_time - frame1;
        bool ret = false;
        if (a3) {
            if (start_time >= *a3) {
                start_time = *a3;
                *a3 = 0.0f;
                ret = true;
            }
            else
                *a3 -= start_time;
        }

        while (start_time > 0.0f) {
            enum_or(flags, EFFECT_INST_JUST_INIT);
            if (start_time < delta_frame)
                delta_frame -= start_time;

            if (frame0 >= 0.0f) {
                GetValue();

                vec3 trans = translation;
                vec3 rot = rotation;
                vec3 scale = this->scale * scale_all;

                mat4 mat;
                mat4_translate(&trans, &mat);
                mat4_mul_rotate_zyx(&mat, &rot, &mat);
                mat4_scale_rot(&mat, &scale, &this->mat);

                for (XEmitterInst*& i : emitters)
                    if (i)
                        i->CtrlInit(this, delta_frame);

                if (frame0 < (float_t)data.life_time) {
                    for (XEmitterInst*& i : emitters)
                        if (i)
                            i->Emit(delta_frame, sc->emission);
                }
                else if (data.flags & EFFECT_LOOP)
                    frame0 -= (float_t)data.life_time;
                else
                    Free(GPM_VAL, Glitter::X, sc->emission, false);

                render_scene.Ctrl(delta_frame, false);
            }

            frame0 += delta_frame;
            frame1 += delta_frame;
            start_time -= delta_frame;
        }

        CtrlMat(GPM_VAL);
        return ret;
    }

    void XEffectInst::SetExtAnim(const mat4* a2, const mat4* a3, const vec3* trans, bool set_flags) {
        EffectInstFlag flags = this->flags;
        if (a2) {
            mat4 mat = *a2;
            if (a3)
                mat4_mul(a3, &mat, &mat);

            if (flags & EFFECT_INST_EXT_ANIM_TRANS_ONLY) {
                ext_anim->mat = mat4_identity;
                mat4_get_translation(&mat, &ext_anim->translation);
            }
            else
                ext_anim->mat = mat;
        }

        if (flags & EFFECT_INST_NO_EXT_ANIM_TRANS_X) {
            ext_anim->mat.row3.x = 0.0f;
            ext_anim->translation.x = 0.0f;
        }
        else if (trans)
            ext_anim->translation.x = trans->x;

        if (flags & EFFECT_INST_NO_EXT_ANIM_TRANS_Y) {
            ext_anim->mat.row3.y = 0.0f;
            ext_anim->translation.y = 0.0f;
        }
        else if (trans)
            ext_anim->translation.y = trans->y;

        if (flags & EFFECT_INST_NO_EXT_ANIM_TRANS_Z) {
            ext_anim->mat.row3.z = 0.0f;
            ext_anim->translation.z = 0.0f;
        }
        else if (trans)
            ext_anim->translation.z = trans->z;

        if (set_flags) {
            if (flags & EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE)
                enum_and(flags, ~EFFECT_INST_GET_EXT_ANIM_THEN_UPDATE);
            if (flags & EFFECT_INST_HAS_EXT_ANIM_NON_INIT)
                enum_and(flags, ~EFFECT_INST_HAS_EXT_ANIM_NON_INIT);
        }
        enum_or(flags, EFFECT_INST_HAS_EXT_ANIM_TRANS);
        this->flags = flags;
    }
}
