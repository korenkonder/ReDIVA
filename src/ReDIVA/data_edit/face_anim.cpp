/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face_anim.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/ogg_vorbis.hpp"
#include "../../CRE/sound.hpp"
#include "../../KKdLib/vec.hpp"
#include "../imgui_helper.hpp"

#if FACE_ANIM
FaceAnim face_anim;

extern render_context* rctx_ptr;

FaceAnim::Auth3D::Auth3D() {

}

FaceAnim::Auth3D::~Auth3D() {

}

inline static void set_data(auth_3d_key& key, const float_t value) {
    key.reset();
    key.type = value != 0.0f && value != -0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
    key.value = value;
}

inline static void set_data(auth_3d_object_model_transform& mt, const vec3 translation,
    const vec3 rotation, const vec3 scale = 1.0f, const float_t visibility = 1.0f) {
    set_data(mt.translation.x, translation.x);
    set_data(mt.translation.y, translation.y);
    set_data(mt.translation.z, translation.z);
    set_data(mt.rotation.x, rotation.x);
    set_data(mt.rotation.y, rotation.y);
    set_data(mt.rotation.z, rotation.z);
    set_data(mt.scale.x, scale.x);
    set_data(mt.scale.y, scale.y);
    set_data(mt.scale.z, scale.z);
    set_data(mt.visibility, visibility);
}

void FaceAnim::Auth3D::patch() {
    ::auth_3d* auth = id.get_auth_3d();
    auth_3d_object_hrc* objhrc = &auth->object_hrc[0];
    auth_3d_object_node* nodes = objhrc->node.data();

    set_data(nodes[ 0].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // OBJHRC_EFFCHRPV826MIK001_MOB
    set_data(nodes[ 1].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // gblctr
    set_data(nodes[ 2].model_transform,
        vec3(0.0f, 1.055f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // n_hara
    set_data(nodes[ 3].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // n_hara_y
    set_data(nodes[ 4].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hara_wj
    set_data(nodes[ 5].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // n_kosi
    set_data(nodes[ 6].model_transform,
        vec3(0.0f, 0.11f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_mune_wj
    set_data(nodes[ 7].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // n_mune_kl
    set_data(nodes[ 8].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_mune_b_wj
    set_data(nodes[ 9].model_transform,
        vec3(0.0f, 0.145f, -0.044f),
        vec3(-0.820574f, 0.0f, 0.24122f)); // j_kubi_wj
    set_data(nodes[10].model_transform,
        vec3(0.0f, 0.079f, 0.004f),
        vec3(2.376704f, 0.175657f, -0.166183f)); // n_kao
    set_data(nodes[11].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_wj
    set_data(nodes[12].model_transform,
        vec3(0.111f, 0.18f, -0.062f),
        vec3(0.0f, -1.570796f, 1.221731f)); // j_hair_twin_l_000_wj
    set_data(nodes[13].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_l_001_wj
    set_data(nodes[14].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_l_002_wj
    set_data(nodes[15].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_l_003_wj
    set_data(nodes[16].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_l_004_wj
    set_data(nodes[17].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_l_005_wj
    set_data(nodes[18].model_transform,
        vec3(-0.111f, 0.18f, -0.062f),
        vec3(0.0f, -1.570796f, -1.221731f)); // j_hair_twin_r_000_wj
    set_data(nodes[19].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_r_001_wj
    set_data(nodes[20].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_r_002_wj
    set_data(nodes[21].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_r_003_wj
    set_data(nodes[22].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_r_004_wj
    set_data(nodes[23].model_transform,
        vec3(0.0f, -0.222f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_hair_twin_r_005_wj
    /*set_data(nodes[24].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_normal_wj
    set_data(nodes[25].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_close_wj
    set_data(nodes[26].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_smile_wj
    set_data(nodes[27].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_close_half_wj
    set_data(nodes[28].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kao_smile_half_wj
    set_data(nodes[29].model_transform,
        vec3(0.0148f, -0.0018f, 0.079f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kuti_l_wj
    set_data(nodes[30].model_transform,
        vec3(0.0f, 0.00048f, 0.088f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kuti_u_wj
    set_data(nodes[31].model_transform,
        vec3(0.0f, -0.00672f, 0.082f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kuti_d_wj
    set_data(nodes[32].model_transform,
        vec3(-0.0148f, -0.0018f, 0.079f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kuti_r_wj
    set_data(nodes[33].model_transform,
        vec3(-0.027f, 0.061f, 0.015f),
        vec3(-1.570796f, 0.0f, 0.0f)); // j_eye_r_wj
    set_data(nodes[34].model_transform,
        vec3(0.027f, 0.061f, 0.015f),
        vec3(-1.570796f, 0.0f, 0.0f)); // j_eye_l_wj*/
    set_data(nodes[35].model_transform,
        vec3(0.02f, 0.129f, -0.053f),
        vec3(1.570796f, 0.0f, 1.570796f)); // n_waki_l
    set_data(nodes[36].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_waki_l_wj
    set_data(nodes[37].model_transform,
        vec3(0.083002f, -0.009998f, 0.0f),
        vec3(-0.037163f, -1.081817f, -3.108780f)); // n_kata_l
    set_data(nodes[38].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kata_l_wj
    set_data(nodes[39].model_transform,
        vec3(0.183625f, -0.09765f, -0.003205f),
        vec3(0.0f, 0.0f, 0.0f)); // j_ude_l_wj
    set_data(nodes[40].model_transform,
        vec3(0.18539f, -0.098589f, -0.003236f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_l_wj
    set_data(nodes[41].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_sizen2_l_wj
    set_data(nodes[42].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_close_l_wj
    set_data(nodes[43].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_reset_l_wj
    set_data(nodes[44].model_transform,
        vec3(-0.02f, 0.129f, -0.053f),
        vec3(1.570796f, 0.0f, 1.570796f)); // n_waki_r
    set_data(nodes[45].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_waki_r_wj
    set_data(nodes[46].model_transform,
        vec3(-0.083002f, -0.009998f, 0.0f),
        vec3(0.037163f, 1.081817f, -3.108780f)); // n_kata_r
    set_data(nodes[47].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_kata_r_wj
    set_data(nodes[48].model_transform,
        vec3(-0.183625f, -0.09765f, -0.003205f),
        vec3(0.0f, 0.0f, 0.0f)); // j_ude_r_wj
    set_data(nodes[49].model_transform,
        vec3(-0.18539f, -0.098589f, -0.003236f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_r_wj
    set_data(nodes[50].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_sizen2_r_wj
    set_data(nodes[51].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_close_r_wj
    set_data(nodes[52].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_reset_r_wj
    set_data(nodes[53].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_te_one_r_wj
    set_data(nodes[54].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.570796f, 0.0f)); // j_kosi_wj
    set_data(nodes[55].model_transform,
        vec3(0.065f, -0.15f, -0.046f),
        vec3(0.0f, 0.0f, -1.570796f)); // n_momo_l
    set_data(nodes[56].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_momo_l_wj
    set_data(nodes[57].model_transform,
        vec3(0.0f, -0.39f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_sune_l_wj
    set_data(nodes[58].model_transform,
        vec3(0.0f, -0.38f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_asi_l_wj
    set_data(nodes[59].model_transform,
        vec3(-0.065f, -0.15f, -0.046f),
        vec3(0.0f, 0.0f, -1.570796f)); // n_momo_r
    set_data(nodes[60].model_transform,
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_momo_r_wj
    set_data(nodes[61].model_transform,
        vec3(0.0f, -0.39f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_sune_r_wj
    set_data(nodes[62].model_transform,
        vec3(0.0f, -0.38f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f)); // j_asi_r_wj
    set_data(nodes[63].model_transform,
        vec3(0.0f, -0.052047f, 0.060104f),
        vec3(0.0f, 0.0f, -1.762782f)); // j_cloth_skirt_f_000_wj
    set_data(nodes[64].model_transform,
        vec3(0.05915f, -0.052048f, 0.04283f),
        vec3(-0.368869f, -0.220518f, -1.784135f)); // j_cloth_skirt_l_02_000_wj
    set_data(nodes[65].model_transform,
        vec3(0.10962f, -0.052048f, -0.013f),
        vec3(-1.710423f, -0.331613f, -1.588249f)); // j_cloth_skirt_l_04_000_wj
    set_data(nodes[66].model_transform,
        vec3(0.08239f, -0.052048f, -0.09275f),
        vec3(-2.356194f, -0.314159f, -1.308997f)); // j_cloth_skirt_l_06_000_wj
    set_data(nodes[67].model_transform,
        vec3(-0.05915f, -0.052048f, 0.04283f),
        vec3(0.368869f, 0.220518f, -1.784135f)); // j_cloth_skirt_r_02_000_wj
    set_data(nodes[68].model_transform,
        vec3(-0.10962f, -0.052048f, -0.013f),
        vec3(1.710423f, 0.331613f, -1.588249f)); // j_cloth_skirt_r_04_000_wj
    set_data(nodes[69].model_transform,
        vec3(-0.08239f, -0.052048f, -0.09275f),
        vec3(2.356194f, 0.314159f, -1.308997f)); // j_cloth_skirt_r_06_000_wj
    set_data(nodes[70].model_transform,
        vec3(0.0f, -0.052049f, -0.118331f),
        vec3(-3.141592f, 0.0f, -1.186823f)); // j_cloth_skirt_b_000_wj

    objhrc->interpolate(0.0f);
}

void FaceAnim::Auth3D::reset() {
    auth_3d_data_unload_category(category.hash_murmurhash);
    id.unload(rctx_ptr);
    objset_info_storage_unload_set(object_set.hash_murmurhash);

    category.clear();
    file.clear();
    object_set.clear();
    id = -1;

    obj_db.clear();
    tex_db.clear();
}

FaceAnim::FaceAnim() : state(), play(), sound_play(), frame(), frame_count() {

}

FaceAnim::~FaceAnim() {

}

bool FaceAnim::init() {
    reset();

    camera* cam = rctx_ptr->camera;
    cam->reset();
    cam->set_fast_change_hist0(true);
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f });

    state = 1;
    return true;
}

bool FaceAnim::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    data_struct* x_data = &data_list[DATA_X];

    switch (state) {
    case 1: {
        auth_3d.category.assign("A3D_EFFCHRPV826MIK001");
        auth_3d.file.assign("EFFCHRPV826MIK001");
        auth_3d.object_set.assign("EFFCHRPV826MIK001");

        auth_3d_data_load_category(x_data, auth_3d.category.c_str(), auth_3d.category.hash_murmurhash);
        objset_info_storage_load_set_hash(x_data, auth_3d.object_set.hash_murmurhash);

        state = 2;
    } break;
    case 2: {
        if (!auth_3d_data_check_category_loaded(auth_3d.category.hash_murmurhash)
            || (objset_info_storage_get_objset_info(auth_3d.object_set.hash_murmurhash)
            && objset_info_storage_load_obj_set_check_not_read(
                auth_3d.object_set.hash_murmurhash, &auth_3d.obj_db, &auth_3d.tex_db)))
            break;

        auth_3d.id = auth_3d_id(auth_3d.file.hash_murmurhash, x_data, &auth_3d.obj_db, &auth_3d.tex_db);
        auth_3d.id.read_file_modern();

        state = 3;
    } break;
    case 3: {
        if (!auth_3d.id.check_not_empty() || !auth_3d.id.check_loaded())
            break;

        auth_3d.id.set_enable(true);
        auth_3d.id.set_paused(false);
        auth_3d.id.set_repeat(false);
        auth_3d.id.set_req_frame(0.0f);
        auth_3d.id.set_visibility(true);
        auth_3d.patch();

        frame = 5000;
        frame_count = (int32_t)auth_3d.id.get_last_frame();

        ogg_path.assign("H:\\C\\Documents\\A\\pv826\\lipsync\\01_rin.ogg");
        //sound_work_play_stream(0, ogg_path.c_str(), (float_t)((double_t)frame / 60.0));
        state = 4;
    } break;
    case 4: {
        auth_3d.id.set_req_frame((float_t)frame);

        if (!play && sound_play) {
            sound_work_release_stream(0);
            sound_play = false;
        }
        else if (play && !sound_play) {
            sound_work_play_stream(0, ogg_path.c_str(), (float_t)((double_t)frame / 60.0));
            sound_play = true;
        }

        if (play)
            frame++;

        if (frame >= frame_count) {
            frame = 5000;

            sound_work_release_stream(0);
            sound_work_play_stream(0, ogg_path.c_str(), (float_t)((double_t)frame / 60.0));
        }
    } break;
    }
    return false;
}

bool FaceAnim::dest() {
    reset();
    return true;
}

void FaceAnim::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 200.0f;
    float_t h = (float_t)height;
    h = min_def(h, 232.0f);

    ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Face Anim", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        ImGui::End();
        return;
    }

    if (ImGui::Button(play ? "Pause" : "Play"))
        play ^= true;

    ImGui::Text("% 5d/% 5d", frame, frame_count);
    ImGui::End();
}

void FaceAnim::reset() {
    state = 0;
    auth_3d.reset();
    ogg_path.clear();
    ogg_path.shrink_to_fit();
    sound_work_release_stream(0);
    play = false;
    sound_play = false;
    frame = 0;
    frame_count = 0;
}
#endif
