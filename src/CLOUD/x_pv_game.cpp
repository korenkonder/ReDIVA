/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(CLOUD_DEV)
#include <set>
#include "x_pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_param.hpp"
#include "../CRE/stage_modern.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/sort.hpp"
#include "input.hpp"
#include "classes/imgui_helper.h"

enum dsc_x_func {
    DSC_X_END = 0,
    DSC_X_TIME,
    DSC_X_MIKU_MOVE,
    DSC_X_MIKU_ROT,
    DSC_X_MIKU_DISP,
    DSC_X_MIKU_SHADOW,
    DSC_X_TARGET,
    DSC_X_SET_MOTION,
    DSC_X_SET_PLAYDATA,
    DSC_X_EFFECT,
    DSC_X_FADEIN_FIELD,
    DSC_X_EFFECT_OFF,
    DSC_X_SET_CAMERA,
    DSC_X_DATA_CAMERA,
    DSC_X_CHANGE_FIELD,
    DSC_X_HIDE_FIELD,
    DSC_X_MOVE_FIELD,
    DSC_X_FADEOUT_FIELD,
    DSC_X_EYE_ANIM,
    DSC_X_MOUTH_ANIM,
    DSC_X_HAND_ANIM,
    DSC_X_LOOK_ANIM,
    DSC_X_EXPRESSION,
    DSC_X_LOOK_CAMERA,
    DSC_X_LYRIC,
    DSC_X_MUSIC_PLAY,
    DSC_X_MODE_SELECT,
    DSC_X_EDIT_MOTION,
    DSC_X_BAR_TIME_SET,
    DSC_X_SHADOWHEIGHT,
    DSC_X_EDIT_FACE,
    DSC_X_DUMMY,
    DSC_X_PV_END,
    DSC_X_SHADOWPOS,
    DSC_X_EDIT_LYRIC,
    DSC_X_EDIT_TARGET,
    DSC_X_EDIT_MOUTH,
    DSC_X_SET_CHARA,
    DSC_X_EDIT_MOVE,
    DSC_X_EDIT_SHADOW,
    DSC_X_EDIT_EYELID,
    DSC_X_EDIT_EYE,
    DSC_X_EDIT_ITEM,
    DSC_X_EDIT_EFFECT,
    DSC_X_EDIT_DISP,
    DSC_X_EDIT_HAND_ANIM,
    DSC_X_AIM,
    DSC_X_HAND_ITEM,
    DSC_X_EDIT_BLUSH,
    DSC_X_NEAR_CLIP,
    DSC_X_CLOTH_WET,
    DSC_X_LIGHT_ROT,
    DSC_X_SCENE_FADE,
    DSC_X_TONE_TRANS,
    DSC_X_SATURATE,
    DSC_X_FADE_MODE,
    DSC_X_AUTO_BLINK,
    DSC_X_PARTS_DISP,
    DSC_X_TARGET_FLYING_TIME,
    DSC_X_CHARA_SIZE,
    DSC_X_CHARA_HEIGHT_ADJUST,
    DSC_X_ITEM_ANIM,
    DSC_X_CHARA_POS_ADJUST,
    DSC_X_SCENE_ROT,
    DSC_X_EDIT_MOT_SMOOTH_LEN,
    DSC_X_PV_BRANCH_MODE,
    DSC_X_DATA_CAMERA_START,
    DSC_X_MOVIE_PLAY,
    DSC_X_MOVIE_DISP,
    DSC_X_WIND,
    DSC_X_OSAGE_STEP,
    DSC_X_OSAGE_MV_CCL,
    DSC_X_CHARA_COLOR,
    DSC_X_SE_EFFECT,
    DSC_X_CHARA_SHADOW_QUALITY,
    DSC_X_STAGE_SHADOW_QUALITY,
    DSC_X_COMMON_LIGHT,
    DSC_X_TONE_MAP,
    DSC_X_IBL_COLOR,
    DSC_X_REFLECTION,
    DSC_X_CHROMATIC_ABERRATION,
    DSC_X_STAGE_SHADOW,
    DSC_X_REFLECTION_QUALITY,
    DSC_X_PV_END_FADEOUT,
    DSC_X_CREDIT_TITLE,
    DSC_X_BAR_POINT,
    DSC_X_BEAT_POINT,
    DSC_X_RESERVE1,
    DSC_X_PV_AUTH_LIGHT_PRIORITY,
    DSC_X_PV_CHARA_LIGHT,
    DSC_X_PV_STAGE_LIGHT,
    DSC_X_TARGET_EFFECT,
    DSC_X_FOG,
    DSC_X_BLOOM,
    DSC_X_COLOR_CORRECTION,
    DSC_X_DOF,
    DSC_X_CHARA_ALPHA,
    DSC_X_AUTO_CAPTURE_BEGIN,
    DSC_X_MANUAL_CAPTURE,
    DSC_X_TOON_EDGE,
    DSC_X_SHIMMER,
    DSC_X_ITEM_ALPHA,
    DSC_X_MOVIE_CUT,
    DSC_X_EDIT_CAMERA_BOX,
    DSC_X_EDIT_STAGE_PARAM,
    DSC_X_EDIT_CHANGE_FIELD,
    DSC_X_MIKUDAYO_ADJUST,
    DSC_X_LYRIC_2,
    DSC_X_LYRIC_READ,
    DSC_X_LYRIC_READ_2,
    DSC_X_ANNOTATION,
    DSC_X_STAGE_EFFECT,
    DSC_X_SONG_EFFECT,
    DSC_X_SONG_EFFECT_ATTACH,
    DSC_X_LIGHT_AUTH,
    DSC_X_FADE,
    DSC_X_SET_STAGE_EFFECT_ENV,
    DSC_X_RESERVE2,
    DSC_X_COMMON_EFFECT_AET_FRONT,
    DSC_X_COMMON_EFFECT_AET_FRONT_LOW,
    DSC_X_COMMON_EFFECT_PARTICLE,
    DSC_X_SONG_EFFECT_ALPHA_SORT,
    DSC_X_LOOK_CAMERA_FACE_LIMIT,
    DSC_X_ITEM_LIGHT,
    DSC_X_CHARA_EFFECT,
    DSC_X_MARKER,
    DSC_X_CHARA_EFFECT_CHARA_LIGHT,
    DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA,
    DSC_X_ENABLE_FXAA,
    DSC_X_ENABLE_TEMPORAL_AA,
    DSC_X_ENABLE_REFLECTION,
    DSC_X_BANK_BRANCH,
    DSC_X_BANK_END,
    DSC_X_NULL1,
    DSC_X_NULL2,
    DSC_X_NULL3,
    DSC_X_NULL4,
    DSC_X_NULL5,
    DSC_X_NULL6,
    DSC_X_NULL7,
    DSC_X_NULL8,
    DSC_X_VR_LIVE_MOVIE,
    DSC_X_VR_CHEER,
    DSC_X_VR_CHARA_PSMOVE,
    DSC_X_VR_MOVE_PATH,
    DSC_X_VR_SET_BASE,
    DSC_X_VR_TECH_DEMO_EFFECT,
    DSC_X_VR_TRANSFORM,
    DSC_X_GAZE,
    DSC_X_TECH_DEMO_GESUTRE,
    DSC_X_VR_CHEMICAL_LIGHT_COLOR,
    DSC_X_VR_LIVE_MOB,
    DSC_X_VR_LIVE_HAIR_OSAGE,
    DSC_X_VR_LIVE_LOOK_CAMERA,
    DSC_X_VR_LIVE_CHEER,
    DSC_X_VR_LIVE_GESTURE,
    DSC_X_VR_LIVE_CLONE,
    DSC_X_VR_LOOP_EFFECT,
    DSC_X_VR_LIVE_ONESHOT_EFFECT,
    DSC_X_VR_LIVE_PRESENT,
    DSC_X_VR_LIVE_TRANSFORM,
    DSC_X_VR_LIVE_FLY,
    DSC_X_VR_LIVE_CHARA_VOICE,
};

x_pv_game x_pv_game_data;

extern render_context* rctx_ptr;

static float_t dsc_time_to_frame(int64_t time);
static int32_t expression_id_to_mottbl_index(int32_t expression_id);
static int32_t hand_anim_id_to_mottbl_index(int32_t hand_anim_id);
static int32_t look_anim_id_to_mottbl_index(int32_t look_anim_id);
static int32_t mouth_anim_id_to_mottbl_index(int32_t mouth_anim_id);
static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time);
static bool x_pv_game_dsc_process(x_pv_game* xpvgm, int64_t curr_time);
static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys);
static void x_pv_game_reset_field(x_pv_game* xpvgm);
static void x_pv_game_stage_effect_ctrl(x_pv_game* xpvgm);
static void x_pv_game_stage_effect_start(x_pv_game* xpvgm, pvsr_stage_effect* stage_effect);
static void x_pv_game_stage_effect_stop(x_pv_game* xpvgm,
    pvsr_stage_effect* stage_effect, bool stop_a3da = true, bool stop_glitter = true);
static bool x_pv_game_stage_effects_process(x_pv_game* xpvgm, int32_t frame);

static dsc_data* sub_1401216D0(x_pv_game* a1, dsc_x_func func_name,
    int32_t* time, int32_t* pv_branch_mode, dsc_data* start, dsc_data* end);
static void sub_140121A80(x_pv_game* a1, int32_t chara_id);
static void sub_140122770(x_pv_game* a1, int32_t chara_id);

x_pv_game_a3da_to_mot_keys::x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot_keys::~x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot::x_pv_game_a3da_to_mot(int32_t auth_3d_id) {
    this->auth_3d_id = auth_3d_id;
    gblctr = -1;
    n_hara = -1;
    n_hara_y = -1;
    j_hara_wj = -1;
    n_kosi = -1;
    j_mune_wj = -1;
    n_mune_kl = -1;
    j_mune_b_wj = -1;
    j_kubi_wj = -1;
    n_kao = -1;
    j_kao_wj = -1;
    j_kao_normal_wj = -1;
    j_kao_close_wj = -1;
    j_kao_smile_wj = -1;
    j_kao_close_half_wj = -1;
    j_kao_smile_half_wj = -1;
    j_kuti_l_wj = -1;
    j_kuti_u_wj = -1;
    j_kuti_d_wj = -1;
    j_kuti_r_wj = -1;
    j_eye_r_wj = -1;
    j_eye_l_wj = -1;
    n_waki_l = -1;
    j_waki_l_wj = -1;
    n_kata_l = -1;
    j_kata_l_wj = -1;
    j_ude_l_wj = -1;
    j_te_l_wj = -1;
    j_te_sizen2_l_wj = -1;
    j_te_close_l_wj = -1;
    j_te_reset_l_wj = -1;
    n_waki_r = -1;
    j_waki_r_wj = -1;
    n_kata_r = -1;
    j_kata_r_wj = -1;
    j_ude_r_wj = -1;
    j_te_r_wj = -1;
    j_te_sizen2_r_wj = -1;
    j_te_close_r_wj = -1;
    j_te_reset_r_wj = -1;
    j_te_one_r_wj = -1;
    j_kosi_wj = -1;
    n_momo_l = -1;
    j_momo_l_wj = -1;
    j_sune_l_wj = -1;
    j_asi_l_wj = -1;
    n_momo_r = -1;
    j_momo_r_wj = -1;
    j_sune_r_wj = -1;
    j_asi_r_wj = -1;
}

x_pv_game_a3da_to_mot::~x_pv_game_a3da_to_mot() {

}
static int32_t get_bone_index(auth_3d_object_hrc* oh, const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (auth_3d_object_node& i : oh->node)
        if (hash_string_murmurhash(&i.name) == name_hash)
            return (int32_t)(&i - oh->node.data());
    return -1;
}

void x_pv_game_a3da_to_mot::get_bone_indices(auth_3d_object_hrc* oh) {
    gblctr = get_bone_index(oh, "gblctr");
    n_hara = get_bone_index(oh, "n_hara");
    n_hara_y = get_bone_index(oh, "n_hara_y");
    j_hara_wj = get_bone_index(oh, "j_hara_wj");
    n_kosi = get_bone_index(oh, "n_kosi");
    j_mune_wj = get_bone_index(oh, "j_mune_wj");
    n_mune_kl = get_bone_index(oh, "n_mune_kl");
    j_mune_b_wj = get_bone_index(oh, "j_mune_b_wj");
    j_kubi_wj = get_bone_index(oh, "j_kubi_wj");
    n_kao = get_bone_index(oh, "n_kao");
    j_kao_wj = get_bone_index(oh, "j_kao_wj");
    j_kao_normal_wj = get_bone_index(oh, "j_kao_normal_wj");
    j_kao_close_wj = get_bone_index(oh, "j_kao_close_wj");
    j_kao_smile_wj = get_bone_index(oh, "j_kao_smile_wj");
    j_kao_close_half_wj = get_bone_index(oh, "j_kao_close_half_wj");
    j_kao_smile_half_wj = get_bone_index(oh, "j_kao_smile_half_wj");
    j_kuti_l_wj = get_bone_index(oh, "j_kuti_l_wj");
    j_kuti_u_wj = get_bone_index(oh, "j_kuti_u_wj");
    j_kuti_d_wj = get_bone_index(oh, "j_kuti_d_wj");
    j_kuti_r_wj = get_bone_index(oh, "j_kuti_r_wj");
    j_eye_r_wj = get_bone_index(oh, "j_eye_r_wj");
    j_eye_l_wj = get_bone_index(oh, "j_eye_l_wj");
    n_waki_l = get_bone_index(oh, "n_waki_l");
    j_waki_l_wj = get_bone_index(oh, "j_waki_l_wj");
    n_kata_l = get_bone_index(oh, "n_kata_l");
    j_kata_l_wj = get_bone_index(oh, "j_kata_l_wj");
    j_ude_l_wj = get_bone_index(oh, "j_ude_l_wj");
    j_te_l_wj = get_bone_index(oh, "j_te_l_wj");
    j_te_sizen2_l_wj = get_bone_index(oh, "j_te_sizen2_l_wj");
    j_te_close_l_wj = get_bone_index(oh, "j_te_close_l_wj");
    j_te_reset_l_wj = get_bone_index(oh, "j_te_reset_l_wj");
    n_waki_r = get_bone_index(oh, "n_waki_r");
    j_waki_r_wj = get_bone_index(oh, "j_waki_r_wj");
    n_kata_r = get_bone_index(oh, "n_kata_r");
    j_kata_r_wj = get_bone_index(oh, "j_kata_r_wj");
    j_ude_r_wj = get_bone_index(oh, "j_ude_r_wj");
    j_te_r_wj = get_bone_index(oh, "j_te_r_wj");
    j_te_sizen2_r_wj = get_bone_index(oh, "j_te_sizen2_r_wj");
    j_te_close_r_wj = get_bone_index(oh, "j_te_close_r_wj");
    j_te_reset_r_wj = get_bone_index(oh, "j_te_reset_r_wj");
    j_te_one_r_wj = get_bone_index(oh, "j_te_one_r_wj");
    j_kosi_wj = get_bone_index(oh, "j_kosi_wj");
    n_momo_l = get_bone_index(oh, "n_momo_l");
    j_momo_l_wj = get_bone_index(oh, "j_momo_l_wj");
    j_sune_l_wj = get_bone_index(oh, "j_sune_l_wj");
    j_asi_l_wj = get_bone_index(oh, "j_asi_l_wj");
    n_momo_r = get_bone_index(oh, "n_momo_r");
    j_momo_r_wj = get_bone_index(oh, "j_momo_r_wj");
    j_sune_r_wj = get_bone_index(oh, "j_sune_r_wj");
    j_asi_r_wj = get_bone_index(oh, "j_asi_r_wj");
}

x_pv_game::x_pv_game() : pp(), sr(), state(), frame(), frame_float(), time(),
rob_chara_ids(), pv_glitter(), stage_glitter(), dsc_time(), dsc_data_ptr(), dsc_data_ptr_end(),
stage_effects_ptr(), stage_effects_ptr_end(), play(), success(), chara_id(),
pv_end(), playdata(), scene_rot_y(), branch_mode(), pause(), step_frame(), stage_effect() {
    pv_id = -1;
    stage_id = -1;
    effpv_objset = hash_murmurhash_empty;
    stgpv_objset = hash_murmurhash_empty;
    stgpvhrc_objset = hash_murmurhash_empty;
    stage_category_hash = hash_murmurhash_empty;
    pv_category_hash = hash_murmurhash_empty;
    camera_category_hash = hash_murmurhash_empty;
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4u_identity;
    prev_bar_point_time = -1;
    prev_bpm = -1;
}

x_pv_game::~x_pv_game() {

}

bool x_pv_game::Init() {
    task_stage_modern_load("X_PV_GAME_STAGE");
    return true;
}

bool x_pv_game::Ctrl() {
    switch (state) {
    case 0:
        return false;
    case 1: {
        bool wait_load = false;
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                wait_load |= !task_rob_manager_check_chara_loaded(rob_chara_ids[i]);

        if (!wait_load)
            state = 2;
    } break;
    case 2: {
        data_struct* x_data = &data_list[DATA_X];
        data_struct* xhd_data = &data_list[DATA_XHD];

        light_param_storage_data_set_pv_id(pv_id);
        task_stage_modern_set_data(x_data, &obj_db, &tex_db, &stage_data);
        task_stage_modern_set_stage_hashes(&stage_hashes, &stages_data);

        for (std::pair<std::string, uint32_t>& i : category_load)
            auth_3d_data_load_category(x_data, i.first.c_str(), i.second);

        for (std::pair<std::string, uint32_t>& i : effchrpv_category_load)
            auth_3d_data_load_category(xhd_data, i.first.c_str(), i.second);

        auth_3d_data_load_category(light_category.c_str());

        for (pvpp_chara& i : pp->chara)
            if (i.chara_effect_init)
                for (pvpp_chara_effect_a3da& j : i.chara_effect.effect_a3da)
                    auth_3d_data_load_category(x_data, camera_category.c_str(), camera_category_hash);

        bool pv_glt = pv_glitter->hash != hash_murmurhash_empty;
        bool stg_glt = stage_glitter->hash != hash_murmurhash_empty;
        if (pv_glt && Glitter::glt_particle_manager.CheckNoFileReaders(pv_glitter->hash)
            || stg_glt && Glitter::glt_particle_manager.CheckNoFileReaders(stage_glitter->hash)) {
            if (pv_glt) {
                Glitter::EffectGroup* pv_eff_group
                    = Glitter::glt_particle_manager.GetEffectGroup(pv_glitter->hash);
                if (pv_eff_group && pv_eff_group->CheckModel())
                    pv_eff_group->LoadModel(x_data);
            }

            if (stg_glt) {
                Glitter::EffectGroup* stage_eff_group
                    = Glitter::glt_particle_manager.GetEffectGroup(stage_glitter->hash);
                if (stage_eff_group && stage_eff_group->CheckModel())
                    stage_eff_group->LoadModel(x_data);
            }
        }
        state = 3;
    } break;
    case 3: {
        bool wait_load = task_stage_modern_check_not_loaded();

        for (uint32_t& i : objset_load)
            if (object_storage_get_obj_set_handler(i)
                && object_storage_load_obj_set_check_not_read(i, &obj_db, &tex_db))
                wait_load = true;

        for (std::pair<std::string, uint32_t>& i : category_load)
            if (!auth_3d_data_check_category_loaded(i.second))
                wait_load = true;

        for (std::pair<std::string, uint32_t>& i : effchrpv_category_load)
            if (!auth_3d_data_check_category_loaded(i.second))
                wait_load = true;

        if (!auth_3d_data_check_category_loaded(light_category.c_str()))
            wait_load = true;

        Glitter::EffectGroup* pv_eff_group
            = Glitter::glt_particle_manager.GetEffectGroup(pv_glitter->hash);
        if (pv_eff_group && pv_eff_group->CheckLoadModel())
            wait_load = true;

        Glitter::EffectGroup* stage_eff_group
            = Glitter::glt_particle_manager.GetEffectGroup(stage_glitter->hash);
        if (stage_eff_group && stage_eff_group->CheckLoadModel())
            wait_load = true;

        if (!wait_load)
            state = 6;
    } break;
    case 6: {
        data_struct* x_data = &data_list[DATA_X];

        for (uint32_t& i : objset_load) {
            obj_set* set = object_storage_get_obj_set(i);
            if (!set)
                continue;

            for (uint32_t i = 0; i < set->obj_num; i++) {
                obj* obj = &set->obj_data[i];
                for (uint32_t j = 0; j < obj->num_material; j++) {
                    obj_material* material = &obj->material_array[j].material;
                    vec3 spec = { 0.5f, 0.5f, 0.5f };
                    if (!memcmp(&material->color.specular, &spec, sizeof(vec3)))
                        *(vec3*)&material->color.specular = vec3_null;
                    else if (memcmp(&material->color.specular, &vec3_null, sizeof(vec3)))
                        *(vec3*)&material->color.specular = { 0.1f, 0.1f, 0.1f };
                }
            }
        }

        for (std::string i : pv_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, pv_category_hash,
                pv_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_enable(&id, false);
            auth_3d_data_set_visibility(&id, false);
            pv_auth_3d_ids.insert({ hash, id });
        }

        for (std::string i : stage_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, stage_category_hash,
                stage_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_enable(&id, false);
            auth_3d_data_set_visibility(&id, false);
            stage_auth_3d_ids.insert({ hash, id });
        }

        light_auth_3d_id = -1;
        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000", pv_id);
            uint32_t light_auth_3d_hash = hash_utf8_murmurhash(buf);
            for (auth_3d_database_uid& i : auth_3d_db->uid)
                if (hash_string_murmurhash(&i.name) == light_auth_3d_hash) {
                    light_auth_3d_id = auth_3d_data_load_uid(
                        (int32_t)(&i - auth_3d_db->uid.data()), auth_3d_db);
                    auth_3d_data_read_file(&light_auth_3d_id, auth_3d_db);
                    auth_3d_data_set_enable(&light_auth_3d_id, false);
                    auth_3d_data_set_visibility(&light_auth_3d_id, false);
                    break;
                }
        }

        camera_auth_3d_id = -1;
        {
            char buf[0x200];
            if ((pv_id % 100) >= 25 && (pv_id % 100) <= 30 && stage_id >= 25 && stage_id <= 30)
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_100", pv_id);
            else
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
            uint32_t camera_auth_3d_hash = hash_utf8_murmurhash(buf);
            camera_auth_3d_id = auth_3d_data_load_hash(camera_auth_3d_hash,
                camera_category_hash, camera_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&camera_auth_3d_id);
            auth_3d_data_set_enable(&camera_auth_3d_id, false);
            auth_3d_data_set_visibility(&camera_auth_3d_id, false);
        }

        for (auto i : effchrpv_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i.first);
            int32_t id = auth_3d_data_load_hash(hash, i.second.first,
                i.second.second.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_enable(&id, false);
            auth_3d_data_set_visibility(&id, false);
            effchrpv_auth_3d_ids.insert({ hash, id });
        }

        for (auto i : effchrpv_auth_3d_mot_names) {
            uint32_t hash = hash_string_murmurhash(&i.first);
            int32_t id = auth_3d_data_load_hash(hash, i.second.first,
                i.second.second.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_enable(&id, false);
            auth_3d_data_set_visibility(&id, false);
            effchrpv_auth_3d_mot_ids.insert({ hash, id });
        }

        state = 7;
    } break;
    case 7: {
        bool wait_load = false;

        for (auto i : pv_auth_3d_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        for (auto i : stage_auth_3d_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        if (!auth_3d_data_check_id_loaded(&light_auth_3d_id))
            wait_load = true;

        if (!auth_3d_data_check_id_loaded(&camera_auth_3d_id))
            wait_load = true;

        for (auto i : effchrpv_auth_3d_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        for (auto i : stage_auth_3d_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        for (auto i : effchrpv_auth_3d_mot_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        if (!wait_load)
            state = 8;
    } break;
    case 8: {
        app::TaskWork::AppendTask(&pv_param_task::post_process_task, "PV POST PROCESS TASK");

        state = 9;
    } break;
    case 9: {
        {
            data_struct* x_data = &data_list[DATA_X];

            dsc dsc_mouth;
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "root+/pv_script/pv%03d/", pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
            x_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_mouth.dsc", pv_id);
            farc_file* dsc_mouth_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_mouth_ff)
                dsc_mouth.parse(dsc_mouth_ff->data, dsc_mouth_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_scene.dsc", pv_id);
            farc_file* dsc_scene_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_scene_ff)
                dsc_scene.parse(dsc_scene_ff->data, dsc_scene_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_system.dsc", pv_id);
            farc_file* dsc_system_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_system_ff)
                dsc_system.parse(dsc_system_ff->data, dsc_system_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_id);
            dsc_easy.type = DSC_X;
            x_data->load_file(&dsc_easy, path_buf, file_buf, dsc::load_file);

            dsc_m.merge(4, &dsc_mouth, &dsc_scene, &dsc_system, &dsc_easy);

            struct miku_state {
                bool disp;
                int32_t disp_time;
                int32_t set_motion_time;
                uint32_t set_motion_data_offset;
            } state[6];

            for (miku_state& i : state) {
                i.disp = true;
                i.disp_time = -1;
                i.set_motion_time = -1;
                i.set_motion_data_offset = 0;
            }

            std::vector<std::pair<int32_t, int32_t>> bar_frames;
            std::vector<miku_state> state_vec;

            int32_t time = -1;
            int32_t frame = -1;
            int32_t prev_stage_effect = -1;
            for (dsc_data& i : dsc_m.data) {
                if (i.func == DSC_X_END)
                    break;

                uint32_t* data = dsc_m.get_func_data(&i);
                switch (i.func) {
                case DSC_X_TIME: {
                    for (miku_state& i : state)
                        if (state[chara_id].disp && i.disp_time == time && i.set_motion_time != time)
                            state_vec.push_back(i);
                    time = (int32_t)data[0];
                    frame = (int32_t)roundf((float_t)time * (float_t)(60.0f / 100000.0f));
                } break;
                case DSC_X_MIKU_DISP: {
                    int32_t chara_id = (int32_t)data[0];
                    state[chara_id].disp = (int32_t)data[1] == 1;
                    if (state[chara_id].disp)
                        state[chara_id].disp_time = time;
                } break;
                case DSC_X_SET_MOTION: {
                    int32_t chara_id = (int32_t)data[0];
                    state[chara_id].set_motion_time = time;
                    state[chara_id].set_motion_data_offset = i.data_offset;
                } break;
                case DSC_X_BAR_POINT: {
                    int32_t bar = data[0];
                    if (bar % 2)
                        bar_frames.push_back({ frame, bar });
                } break;
                case DSC_X_STAGE_EFFECT: {
                    int32_t stage_effect = (int32_t)data[0];
                    stage_effect--;

                    if (stage_effect < 0 || stage_effect > 6)
                        break;

                    int32_t bar_count = 2;
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect];
                    if (effect.bar_count && effect.bar_count != 0xFF)
                        bar_count = effect.bar_count;

                    int32_t bar_count_change = 2;
                    if (prev_stage_effect != -1) {
                        if (sr->stage_change_effect_init[prev_stage_effect][stage_effect]) {
                            pvsr_stage_effect& effect = sr->stage_change_effect[prev_stage_effect][stage_effect];
                            if (effect.bar_count && effect.bar_count != 0xFF)
                                bar_count_change = effect.bar_count;
                        }
                        else {
                            pvsr_stage_effect& effect = sr->stage_effect[prev_stage_effect];
                            if (effect.bar_count && effect.bar_count != 0xFF)
                                bar_count_change = effect.bar_count;
                        }
                    }
                    bar_count_change /= 2;

                    int32_t stage_effect_frame = frame;
                    int32_t bar = 1;
                    for (std::pair<int32_t, int32_t>& i : bar_frames)
                        if (&i - bar_frames.data() >= bar_count_change && frame >= i.first) {
                            stage_effect_frame = (&i)[-bar_count_change].first;
                            bar = (&i)[-bar_count_change].second;
                        }

                    stage_effects.push_back({ stage_effect_frame,
                        stage_effect, bar_count + bar_count_change, bar });
                    prev_stage_effect = stage_effect;
                } break;
                case DSC_X_SONG_EFFECT_ATTACH: {
                    int32_t effect_id = (int32_t)data[0];
                    int32_t chara_id = (int32_t)data[1];

                    auto elem = pv_auth_3d_chara_count.find(effect_id);
                    if (elem == pv_auth_3d_chara_count.end())
                        elem = pv_auth_3d_chara_count.insert({ effect_id, {} }).first;
                    elem->second.insert(chara_id);
                } break;
                }
            }

            if (stage_effects.size() > 1) {
                bool adjust = false;
                do {
                    adjust = false;
                    size_t count = stage_effects.size() - 1;
                    for (size_t i = 0; i < count; i++) {
                        if (stage_effects[i].bar == 1 || stage_effects[i].bar
                            + stage_effects[i + 1].bar_count <= stage_effects[i + 1].bar)
                            continue;

                        for (std::pair<int32_t, int32_t>& j : bar_frames) {
                            int32_t bar = stage_effects[i + 1].bar - stage_effects[i + 1].bar_count;
                            if (bar != j.second)
                                continue;

                            stage_effects[i].frame = j.first;
                            stage_effects[i].bar = j.second;
                            adjust = true;
                            break;
                        }
                    }
                } while (adjust);
            }

            if (state_vec.size()) {
                int32_t time = -1;
                for (miku_state& i : state_vec) {
                    if (i.disp_time != time) {
                        uint32_t* data = dsc_m.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.disp_time;
                        time = i.disp_time;
                    }

                    uint32_t* new_data = dsc_m.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                        DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                    uint32_t* data = dsc_m.data_buffer.data() + i.set_motion_data_offset;
                    memcpy(new_data, data, sizeof(uint32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                }
                dsc_m.rebuild();
            }

            /*sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_id);
            void* data = 0;
            size_t size = 0;
            dsc_m.unparse(&data, &size);
            stream s;
            s.open(file_buf, "wb");
            s.write(data, size);
            free(data);*/
        }

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            playdata[i].reset();
            if (rob_chara_ids[i] == -1)
                continue;

            rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i]);
            if (rob_chr) {
                playdata[i].rob_chr = rob_chr;
                rob_chr->frame_speed = anim_frame_speed;
                rob_chr->data.motion.step_data.step = anim_frame_speed;
                //sub_1404F3000(i, rob_chr, anim_frame_speed);
            }
            sub_140122770(this, i);
            sub_140121A80(this, i);
        }

        dsc_data_ptr = dsc_m.data.data();
        dsc_data_ptr_end = dsc_data_ptr + dsc_m.data.size();

        stage_effects_ptr = stage_effects.data();
        stage_effects_ptr_end = stage_effects_ptr + stage_effects.size();

        state = 10;

        x_pv_game_change_field(this, 1, -1, -1);

        Glitter::glt_particle_manager.SetPause(false);
        extern float_t frame_speed;
        frame_speed = 1.0f;

        prev_bar_point_time = -1;
        prev_bpm = -1;

        pause = false;
        step_frame = false;

        stage_effect.frame = -1.0f;
        stage_effect.last_frame = -1.0f;
        stage_effect.prev_stage_effect = -1;
        stage_effect.stage_effect = -1;
        stage_effect.next_stage_effect = -1;

        frame_float = -1.0f;
        this->frame = -1;
        this->time = (int64_t)round(-(100000.0 / 60.0) * 10000.0);
        while (dsc_data_ptr != dsc_data_ptr_end
            && x_pv_game_dsc_process(this, this->time))
            dsc_data_ptr++;

        while (stage_effects_ptr != stage_effects_ptr_end
            && x_pv_game_stage_effects_process(this, this->frame))
            stage_effects_ptr++;

        x_pv_game_stage_effect_ctrl(this);

        auth_3d_data_set_enable(&light_auth_3d_id, true);
        auth_3d_data_set_camera_root_update(&light_auth_3d_id, false);
        auth_3d_data_set_paused(&light_auth_3d_id, false);
        auth_3d_data_set_repeat(&camera_auth_3d_id, false);
        auth_3d_data_set_visibility(&light_auth_3d_id, true);

        auth_3d_data_set_enable(&camera_auth_3d_id, true);
        auth_3d_data_set_camera_root_update(&light_auth_3d_id, true);
        auth_3d_data_set_paused(&camera_auth_3d_id, false);
        auth_3d_data_set_repeat(&camera_auth_3d_id, false);
        auth_3d_data_set_visibility(&camera_auth_3d_id, true);

        if (pv_id == 826) {
            data_struct* aft_data = &data_list[DATA_AFT];
            bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
            for (int32_t& i : effchrpv_rob_mot_ids) {
                char buf[0x100];
                sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i + 1);
                int32_t motion_id = aft_mot_db->get_motion_id(buf);
                rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i]);
                rob_chr->set_motion_id(motion_id, 0.0f,
                    0.0f, false, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(motion_id, 0.0f);
                rob_chr->set_motion_skin_param(motion_id, 0.0f);

                sprintf_s(buf, sizeof(buf), "A3D_EFFCHRPV%03dMIK%03d", pv_id, i);
                uint32_t hash = hash_utf8_murmurhash(buf);
                bool found = false;
                for (auto j : effchrpv_auth_3d_names)
                    if (j.second.first == hash) {
                        uint32_t hash = hash_string_murmurhash(&j.first);
                        effchrpv_auth_3d_rob_mot_ids.insert({ i, { effchrpv_auth_3d_ids[hash] } });
                        found = true;
                        break;
                    }

                if (found)
                    continue;

                for (auto j : effchrpv_auth_3d_mot_names)
                    if (j.second.first == hash) {
                        uint32_t hash = hash_string_murmurhash(&j.first);
                        effchrpv_auth_3d_rob_mot_ids.insert({ i, { effchrpv_auth_3d_mot_ids[hash] } });
                        break;
                    }
            }
            for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
                x_pv_game_a3da_to_mot& a2m = i.second;
                auth_3d* auth = auth_3d_data_get_auth_3d(a2m.auth_3d_id);
                a2m.get_bone_indices(&auth->object_hrc[0]);
            }

            for (auto& i : effchrpv_auth_3d_mot_ids) {
                int32_t& id = i.second;
                auth_3d_data_set_repeat(&id, false);
                auth_3d_data_set_camera_root_update(&id, false);
                auth_3d_data_set_enable(&id, true);
                auth_3d_data_set_paused(&id, false);
                auth_3d_data_set_visibility(&id, true);
                auth_3d_data_set_req_frame(&id, -2.0f);
                auth_3d_data_set_frame_rate(&id, &diva_pv_frame_rate);
            }
        }

        if (pv_id == 826) {
            extern int32_t global_ctrl_frames;
            global_ctrl_frames = 5111;
        }
        else if (pv_id == 822) {
            extern int32_t global_ctrl_frames;
            global_ctrl_frames = 3600;
        }
        else
            pause = true;
    } break;
    case 10: {
        if (step_frame)
            if (pause)
                pause = false;
            else {
                pause = true;
                step_frame = false;
            }

        Glitter::glt_particle_manager.SetPause(pause);
        extern float_t frame_speed;
        frame_speed = pause ? 0.0f : 1.0f;

        float_t delta_frame = get_delta_frame();
        frame_float += delta_frame;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        if (pv_id == 826 && frame == 5110)
            pause = true;
        else if (pv_id == 822 && frame == 3600)
            pause = true;

        while (dsc_data_ptr != dsc_data_ptr_end
            && x_pv_game_dsc_process(this, time))
            dsc_data_ptr++;

        while (stage_effects_ptr != stage_effects_ptr_end
            && x_pv_game_stage_effects_process(this, this->frame))
            stage_effects_ptr++;

        stage_effect.frame += diva_stage_frame_rate.GetDeltaFrame();
        x_pv_game_stage_effect_ctrl(this);

        if (pv_id == 826)
            x_pv_game_map_auth_3d_to_mot(this, delta_frame != 0.0f && frame > 0);

        if (!play || pv_end)
            state = 11;
    } break;
    case 11: {
        Glitter::glt_particle_manager.FreeScenes();
        SetDest();
    } break;
    }
    return false;
}

void x_pv_game::Disp() {
    if (state != 10)
        return;

}

bool x_pv_game::Dest() {
    Unload();
    task_stage_modern_unload();

    light_param_storage_data_reset();
    rctx_ptr->post_process.tone_map->set_saturate_coeff(1.0f);
    rctx_ptr->post_process.tone_map->set_scene_fade(vec4_null);
    rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(0);
    rctx_ptr->post_process.dof->data.pv.enable = false;
    rctx_ptr->object_data.object_culling = true;
    rctx_ptr->draw_pass.shadow_ptr->range = 1.0f;

    Glitter::glt_particle_manager.SetPause(false);
    extern float_t frame_speed;
    frame_speed = 1.0f;
    return true;
}

void x_pv_game::Window() {
    if (state != 10)
        return;

    if (Input::IsKeyTapped(GLFW_KEY_K, GLFW_MOD_CONTROL))
        state = 11;
    else if (Input::IsKeyTapped(GLFW_KEY_K))
        pause ^= true;

    if (Input::IsKeyTapped(GLFW_KEY_L)) {
        pause = true;
        step_frame = true;
    }

    if (!pause)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 240.0f;
    float_t h = 86.0f;

    extern int32_t width;
    ImGui::SetNextWindowPos({ width - w, 0 }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    bool collapsed = !ImGui::Begin("X PV GAME", 0, window_flags);
    if (collapsed) {
        ImGui::End();
        return;
    }

    w = imguiGetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton(pause || (!pause && step_frame) ? "Play (K)" : "Pause (K)", { w, 0.0f }))
            pause ^= true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Step Frame (L)", { w, 0.0f })) {
            pause = true;
            step_frame = true;
        }

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Stop (Ctrl+K)", { w, 0.0f }))
            state = 11;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "%d", frame);
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
        imguiButtonEx(buf, { w, 0.0f }, ImGuiButtonFlags_DontClosePopups
            | ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoHoveredOnFocus);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();
    }

    extern bool input_locked;
    input_locked |= ImGui::IsWindowFocused();
    ImGui::End();
}

void x_pv_game::Load(int32_t pv_id, int32_t stage_id, chara_index charas[6], int32_t modules[6]) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    data_struct* x_data = &data_list[DATA_X];

    this->pv_id = pv_id;
    this->stage_id = stage_id;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "EFFPV%03d", pv_id);
    effpv_objset = hash_utf8_murmurhash(buf);

    sprintf_s(buf, sizeof(buf), "STGPV%03d", stage_id);
    stgpv_objset = hash_utf8_murmurhash(buf);

    sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", stage_id);
    stgpvhrc_objset = hash_utf8_murmurhash(buf);

    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_load_motion(mot_set, 0, aft_mot_db);
    motion_set_load_mothead(mot_set, 0, aft_mot_db);

    char file_buf[0x200];
    sprintf_s(file_buf, sizeof(file_buf), "pv%03d.pvpp", pv_id);
    pp = new pvpp;
    x_data->load_file(pp, "root+/pv/", file_buf, pvpp::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "stgpv%03d_param.pvsr", stage_id);
    sr = new pvsr;
    x_data->load_file(sr, "root+/pv_stage_rsrc/", file_buf, pvsr::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "pv_stgpv%03d.stg", stage_id);
    stage_data.modern = true;
    x_data->load_file(&stage_data, "root+/stage/", file_buf, stage_database::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "eff_pv%03d_main", pv_id);
    pv_glitter = new x_pv_game_glitter(file_buf);

    sprintf_s(file_buf, sizeof(file_buf), "eff_stgpv%03d_main", stage_id);
    stage_glitter = new x_pv_game_glitter(file_buf);

    sprintf_s(buf, sizeof(buf), "A3D_EFFPV%03d", pv_id);
    pv_category = buf;
    pv_category_hash = hash_string_murmurhash(&pv_category);
    category_load.push_back({ pv_category, pv_category_hash });

    pv_auth_3d_names.clear();
    for (pvpp_effect i : pp->effect)
        for (pvpp_a3da j : i.a3da) {
            if (pv_auth_3d_names.find(j.name) == pv_auth_3d_names.end())
                pv_auth_3d_names.insert(j.name);
        }

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
    stage_category = buf;
    stage_category_hash = hash_string_murmurhash(&stage_category);
    category_load.push_back({ stage_category, stage_category_hash });

    stage_auth_3d_names.clear();
    for (pvsr_stage_effect i : sr->stage_effect)
        for (pvsr_a3da j : i.a3da)
            if (stage_auth_3d_names.find(j.name) == stage_auth_3d_names.end())
                stage_auth_3d_names.insert(j.name);

    for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++) {
            if (!sr->stage_change_effect_init[i][j])
                continue;

            pvsr_stage_effect& stage_effect = sr->stage_change_effect[i][j];
            for (pvsr_a3da k : stage_effect.a3da)
                if (stage_auth_3d_names.find(k.name) == stage_auth_3d_names.end())
                    stage_auth_3d_names.insert(k.name);
        }

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = buf;

    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    camera_category = buf;
    camera_category_hash = hash_string_murmurhash(&camera_category);
    category_load.push_back({ camera_category, camera_category_hash });

    int32_t chara_index = 0;
    effchrpv_auth_3d_names.clear();
    if (pv_id == 826)
        effchrpv_auth_3d_mot_names.clear();

    for (pvpp_chara& i : pp->chara) {
        if (!i.chara_effect_init) {
            chara_index++;
            continue;
        }

        const char* src_name = chara_index_get_auth_3d_name((::chara_index)i.chara_effect.base_chara);
        const char* dst_name = chara_index_get_auth_3d_name(charas[chara_index++]);
        const char* mik_name = chara_index_get_auth_3d_name(CHARA_MIKU);

        for (pvpp_chara_effect_a3da& j : i.chara_effect.effect_a3da) {
            std::string name = j.a3da.name;
            if (name.find(src_name) == std::string::npos) {
                size_t pos = name.find(dst_name);
                if (pos != std::string::npos)
                    name.replace(pos, utf8_length(dst_name), src_name, utf8_length(src_name));
            }

            if (pv_id != 826) {
                std::string category = "A3D_" + name;
                uint32_t category_hash = hash_string_murmurhash(&category);
                category_load.push_back({ category, category_hash });

                if (effchrpv_auth_3d_names.find(name) == effchrpv_auth_3d_names.end())
                    effchrpv_auth_3d_names.insert({ name, { category_hash, category } });

                effchrpv_auth_3d_hashes.insert({ j.a3da.hash, hash_string_murmurhash(&name) });
            }
            else {
                sprintf_s(buf, sizeof(buf), "EFFCHRPV%03d", pv_id);
                if (j.a3da.name.find(buf) == std::string::npos)
                    continue;

                name = j.a3da.name;
                name.replace(utf8_length(buf), utf8_length(mik_name), mik_name, utf8_length(mik_name));

                std::string category = "A3D_" + name;
                uint32_t category_hash = hash_string_murmurhash(&category);

                effchrpv_category_load.push_back({ category, category_hash });
                effchrpv_auth_3d_mot_names.insert({ name, { category_hash, category } });
            }
        }
    }

    stage_hashes.clear();
    stages_data.clear();
    for (stage_data_modern& i : stage_data.stage_modern) {
        stage_hashes.push_back(i.hash);
        stages_data.push_back(&i);
    }

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (x_pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4u_identity;
    branch_mode = 0;

    prev_bar_point_time = -1;
    prev_bpm = -1;

    pause = false;
    step_frame = false;

    for (int32_t& i : rob_chara_ids)
        i = -1;

    chara_index = 0;
    for (pvpp_chara& i : pp->chara) {
        if (i.motion.size()) {
            rob_chara_pv_data pv_data;
            pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[chara_index]);
            int32_t chara_id = rob_chara_array_init_chara_index(
                charas[chara_index], &pv_data, modules[chara_index], true);
            if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                rob_chara_ids[chara_index] = chara_id;
        }

        if (++chara_index >= ROB_CHARA_COUNT)
            break;
    }

    if (pv_id == 826)
        for (int32_t i = (int32_t)pp->chara.size(); i < ROB_CHARA_COUNT; i++) {
            rob_chara_pv_data pv_data;
            pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[i]);
            int32_t chara_id = rob_chara_array_init_chara_index(charas[i], &pv_data, modules[i], true);
            if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                rob_chara_ids[i] = chara_id;
            effchrpv_rob_mot_ids.push_back(chara_id);
        }

    objset_load.push_back(effpv_objset);
    objset_load.push_back(stgpv_objset);
    objset_load.push_back(stgpvhrc_objset);

    chara_index = 0;
    for (pvpp_chara& i : pp->chara) {
        if (!i.chara_effect_init) {
            chara_index++;
            continue;
        }

        const char* src_name = chara_index_get_auth_3d_name((::chara_index)i.chara_effect.base_chara);
        const char* dst_name = chara_index_get_auth_3d_name(charas[chara_index++]);

        for (pvpp_chara_effect_a3da& j : i.chara_effect.effect_a3da) {
            std::string name = j.has_object_set ? j.object_set.name : j.a3da.name;
            if (name.find(src_name) == std::string::npos) {
                size_t pos = name.find(dst_name);
                if (pos != std::string::npos)
                    name.replace(pos, utf8_length(dst_name), src_name, utf8_length(src_name));
            }

            objset_load.push_back(hash_string_murmurhash(&name));

            std::string nam = j.has_object_set ? j.object_set.name : j.a3da.name;
            nam.replace(utf8_length(buf), utf8_length(dst_name), dst_name, utf8_length(dst_name));
            if (nam != name)
                objset_load.push_back(hash_string_murmurhash(&nam));
        }
    }

    for (uint32_t& i : objset_load)
        object_storage_load_set_hash(x_data, i);

    state = 1;
    this->frame = 0;
    frame_float = 0.0;
    time = 0;
    diva_stage_frame_rate.SetFrameSpeed(1.0f);

    obj_db.ready = true;
    obj_db.modern = true;
    obj_db.is_x = true;
    obj_db.object_set.clear();
    obj_db.object_set.shrink_to_fit();
    tex_db.ready = true;
    tex_db.modern = true;
    tex_db.is_x = true;
    tex_db.texture.clear();
    tex_db.texture.shrink_to_fit();

    dsc_m.type = DSC_NONE;
    dsc_m.signature = 0;
    dsc_m.id = 0;
    dsc_m.data.clear();
    dsc_m.data.shrink_to_fit();
    dsc_m.data_buffer.clear();
    dsc_m.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    stage_effects.clear();
    stage_effects.shrink_to_fit();
    stage_effects_ptr = 0;
    stage_effects_ptr_end = 0;

    pv_auth_3d_chara_count.clear();

    dsc_time = 0;
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    Glitter::glt_particle_manager.draw_all = false;
}

bool mot_write_motion(void* data, const char* path, const char* file, uint32_t hash) {
    x_pv_game* xpvgm = (x_pv_game*)data;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", xpvgm->pv_id);
    motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!set_info)
        return true;

    std::string mot_file = "mot_" + set_info->name + ".bin";

    mot_set* mot_set = new ::mot_set;
    {
        farc f;
        farc::load_file(&f, path, file, hash);

        farc_file* ff = f.read_file(mot_file.c_str());
        if (!ff) {
            delete mot_set;
            return true;
        }

        mot_set->unpack_file(ff->data, ff->size, false);
    }

    if (!mot_set->ready) {
        delete mot_set;
        return true;
    }

    for (auto& i : xpvgm->effchrpv_auth_3d_rob_mot_ids) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i.first + 1);

        int32_t motion_id = aft_mot_db->get_motion_id(buf);

        size_t motion_index = -1;
        for (motion_info& j : set_info->motion)
            if (j.id == motion_id) {
                motion_index = &j - set_info->motion.data();
                break;
            }

        mot_data* mot_data = &mot_set->vec[motion_index];

        uint16_t key_set_count = mot_data->key_set_count - 1;
        if (!key_set_count)
            continue;

        const char* name = bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON);
        std::string* bone_names = aft_mot_db->bone_name.data();
        std::vector<bone_database_bone>* bones = 0;
        if (!aft_bone_data->get_skeleton_bones(name, &bones))
            continue;

        x_pv_game_a3da_to_mot& a2m = i.second;
        const mot_bone_info* bone_info = mot_data->bone_info.data();
        for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
            motion_bone_index bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
                name, bone_names[bone_info[i].index].c_str());
            if (bone_index == -1) {
                i++;
                bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
                    name, bone_names[bone_info[i].index].c_str());
                if (bone_index == -1)
                    break;
            }

            bone_database_bone* bone = &(*bones)[bone_index];

            auto elem = a2m.bone_keys.find(bone_index);
            if (elem != a2m.bone_keys.end()) {
                x_pv_game_a3da_to_mot_keys& keys = elem->second;

                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = mot_set::fit_keys_into_curve(keys.x,
                    key_set_data_x.frames, key_set_data_x.values);
                key_set_data_x.keys_count = (uint16_t)key_set_data_x.frames.size();
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = mot_set::fit_keys_into_curve(keys.y,
                    key_set_data_y.frames, key_set_data_y.values);
                key_set_data_y.keys_count = (uint16_t)key_set_data_y.frames.size();
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = mot_set::fit_keys_into_curve(keys.z,
                    key_set_data_z.frames, key_set_data_z.values);
                key_set_data_z.keys_count = (uint16_t)key_set_data_z.frames.size();
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            elem = a2m.sec_bone_keys.find(bone_index);
            if (elem != a2m.sec_bone_keys.end()) {
                x_pv_game_a3da_to_mot_keys& keys = elem->second;

                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset + 3];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = mot_set::fit_keys_into_curve(keys.x,
                    key_set_data_x.frames, key_set_data_x.values);
                key_set_data_x.keys_count = (uint16_t)key_set_data_x.frames.size();
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 4];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = mot_set::fit_keys_into_curve(keys.y,
                    key_set_data_y.frames, key_set_data_y.values);
                key_set_data_y.keys_count = (uint16_t)key_set_data_y.frames.size();
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 5];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = mot_set::fit_keys_into_curve(keys.z,
                    key_set_data_z.frames, key_set_data_z.values);
                key_set_data_z.keys_count = (uint16_t)key_set_data_z.frames.size();
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            if (bone_index == MOTION_BONE_KL_AGO_WJ) {
                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.values.push_back(0.0491406508f);
                key_set_data_x.type = MOT_KEY_SET_STATIC;
                key_set_data_x.keys_count = 1;
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = MOT_KEY_SET_NONE;
                key_set_data_y.keys_count = 0;
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = MOT_KEY_SET_NONE;
                key_set_data_z.keys_count = 0;
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }
            else if (bone_index == MOTION_BONE_N_KUBI_WJ_EX) {
                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = MOT_KEY_SET_NONE;
                key_set_data_x.keys_count = 1;
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.values.push_back(0.0331281610f);
                key_set_data_y.type = MOT_KEY_SET_STATIC;
                key_set_data_y.keys_count = 0;
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = MOT_KEY_SET_NONE;
                key_set_data_z.keys_count = 0;
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            if (bone->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
                key_set_offset += 6;
            else
                key_set_offset += 3;
        }
    }

    {
        farc f;

        f.add_file(mot_file.c_str());
        farc_file* ff = &f.files.back();
        mot_set->pack_file(&ff->data, &ff->size);

        std::string mot_farc = "mot_" + set_info->name;
        f.write(mot_farc.c_str(), FARC_COMPRESS_FArC, false);
    }

    delete mot_set;
    return true;
}

void x_pv_game::Unload() {
    if (pv_id == 826) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
        motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
        if (set_info) {
            std::string farc_file = "mot_" + set_info->name + ".farc";
            aft_data->load_file(this, "rom/rob/", farc_file.c_str(), mot_write_motion);
        }
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_unload_motion(mot_set);
    motion_set_unload_mothead(mot_set);

    pv_id = -1;
    stage_id = -1;

    state = 0;
    frame = 0;
    frame_float = 0.0;
    time = 0;

    for (int32_t& i : rob_chara_ids)
        if (i != -1) {
            rob_chara_array_free_chara_id(i);
            i = -1;
        }

    delete pv_glitter;
    delete stage_glitter;
    pv_glitter = 0;
    stage_glitter = 0;

    obj_db.ready = false;
    obj_db.modern = false;
    obj_db.is_x = false;
    obj_db.object_set.clear();
    obj_db.object_set.shrink_to_fit();
    tex_db.ready = false;
    tex_db.modern = false;
    tex_db.is_x = false;
    tex_db.texture.clear();
    tex_db.texture.shrink_to_fit();
    stage_data.ready = false;
    stage_data.modern = false;
    stage_data.is_x = false;
    stage_data.format = STAGE_DATA_UNK;
    stage_data.stage_data.clear();
    stage_data.stage_data.shrink_to_fit();
    stage_data.stage_modern.clear();
    stage_data.stage_modern.shrink_to_fit();

    stage_hashes.clear();
    stage_hashes.shrink_to_fit();
    stages_data.clear();
    stages_data.shrink_to_fit();

    stage_auth_3d_names.clear();
    pv_auth_3d_names.clear();

    for (auto& i : pv_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    for (auto& i : stage_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    auth_3d_data_unload_id(light_auth_3d_id, rctx_ptr);
    auth_3d_data_unload_id(camera_auth_3d_id, rctx_ptr);

    for (auto& i : effchrpv_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    for (auto& i : effchrpv_auth_3d_mot_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    pv_auth_3d_ids.clear();
    stage_auth_3d_ids.clear();
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;
    effchrpv_auth_3d_ids.clear();
    effchrpv_auth_3d_mot_ids.clear();

    effchrpv_rob_mot_ids.clear();
    effchrpv_rob_mot_ids.shrink_to_fit();
    effchrpv_auth_3d_rob_mot_ids.clear();
    effchrpv_auth_3d_names.clear();
    effchrpv_auth_3d_mot_names.clear();

    effchrpv_auth_3d_hashes.clear();

    for (std::pair<std::string, uint32_t>& i : category_load)
        auth_3d_data_unload_category(i.second);

    for (std::pair<std::string, uint32_t>& i : effchrpv_category_load)
        auth_3d_data_unload_category(i.second);

    auth_3d_data_unload_category(light_category.c_str());

    category_load.clear();
    category_load.shrink_to_fit();
    effchrpv_category_load.clear();
    effchrpv_category_load.shrink_to_fit();

    pv_category_hash = hash_murmurhash_empty;
    stage_category_hash = hash_murmurhash_empty;
    camera_category_hash = hash_murmurhash_empty;

    pv_category.clear();
    pv_category.shrink_to_fit();
    stage_category.clear();
    stage_category.shrink_to_fit();
    light_category.clear();
    light_category.shrink_to_fit();
    camera_category.clear();
    camera_category.shrink_to_fit();

    for (uint32_t& i : objset_load)
        object_storage_unload_set(i);

    effpv_objset = hash_murmurhash_empty;
    stgpv_objset = hash_murmurhash_empty;
    stgpvhrc_objset = hash_murmurhash_empty;

    objset_load.clear();
    objset_load.shrink_to_fit();

    delete pp;
    delete sr;
    pp = 0;
    sr = 0;

    dsc_m.type = DSC_NONE;
    dsc_m.signature = 0;
    dsc_m.id = 0;
    dsc_m.data.clear();
    dsc_m.data.shrink_to_fit();
    dsc_m.data_buffer.clear();
    dsc_m.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    stage_effects.clear();
    stage_effects.shrink_to_fit();
    stage_effects_ptr = 0;
    stage_effects_ptr_end = 0;

    pv_auth_3d_chara_count.clear();

    dsc_time = 0;
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (x_pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4u_identity;
    branch_mode = 0;

    Glitter::glt_particle_manager.draw_all = true;
    pv_param_task::post_process_task.SetDest();
}

x_pv_game_glitter::x_pv_game_glitter(const char* name) {
    this->name = name;
    hash = (uint32_t)Glitter::glt_particle_manager
        .LoadFile(Glitter::X, &data_list[DATA_X], name, 0, -1.0f, false, 0);
}

x_pv_game_glitter::~x_pv_game_glitter() {
    Glitter::glt_particle_manager.UnloadEffectGroup(hash);
}

x_struc_104::x_struc_104() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

x_struc_104::~x_struc_104() {

}

void x_struc_104::reset() {
    rob_chr = 0;
    current_time = 0.0f;
    duration = 0.0f;
    start_pos = vec3_null;
    end_pos = vec3_null;
    start_rot = 0.0f;
    end_rot = 0.0f;
    mot_smooth_len = 12.0f;
    set_motion.clear();
    set_item.clear();
}

x_pv_play_data::x_pv_play_data() : rob_chr(), disp() {

}

x_pv_play_data::~x_pv_play_data() {

}

void x_pv_play_data::reset() {
    motion.clear();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    field_38.reset();
}

static float_t dsc_time_to_frame(int64_t time) {
    return (float_t)time / 1000000000.0f * 60.0f;
}

static int32_t expression_id_to_mottbl_index(int32_t expression_id) {
    static const int32_t expression_id_to_mottbl_index_table[] = {
         11,  15,  57,  19,  23,  25,  29,  33,
         37,  41,  45,  49,  53,  65,   7,  69,
         73,  77,  81,  85,  89,   7,  61,   6,
        214, 215, 216, 217, 218, 219, 220, 221,
        222, 223,  93,  95,  97,  99, 101, 103,
        105, 107, 109, 111, 113, 115, 117, 119,
        121, 123, 125, 127,  13,  21,  31,  39,
         43,  47,  51,  55,  67,  71,  79,  83,
        129,  59,  17,  91,  27,  87,  35,  75,
          9,  63, 236, 238, 240, 242,
    };

    if (expression_id >= 0 && expression_id
        < sizeof(expression_id_to_mottbl_index_table) / sizeof(int32_t))
        return expression_id_to_mottbl_index_table[expression_id];
    return 6;
}

static int32_t hand_anim_id_to_mottbl_index(int32_t hand_anim_id) {
    static const int32_t hand_anim_id_to_mottbl_index_table[] = {
        195, 196, 194, 197, 201, 198, 199, 202,
        203, 192, 200, 204, 204, 204, 193,
    };

    if (hand_anim_id >= 0 && hand_anim_id
        < sizeof(hand_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return hand_anim_id_to_mottbl_index_table[hand_anim_id];
    return 192;
}

static int32_t look_anim_id_to_mottbl_index(int32_t look_anim_id) {
    static const int32_t look_anim_id_to_mottbl_index_table[] = {
        168, 170, 174, 172, 178, 176, 182, 180,
        166, 165, 224, 169, 171, 175, 173, 179,
        177, 183, 181, 167,
    };

    if (look_anim_id >= 0 && look_anim_id
        < sizeof(look_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return look_anim_id_to_mottbl_index_table[look_anim_id];
    return 165;
}

static int32_t mouth_anim_id_to_mottbl_index(int32_t mouth_anim_id) {
    static const int32_t mouth_anim_id_to_mottbl_index_table[] = {
        134, 140, 142, 146, 144, 148, 150, 152,
        132, 131, 136, 138, 154, 155, 156, 157,
        158, 159, 160, 161, 162, 163, 164, 151,
        135, 143, 147, 145, 133, 137, 139, 141,
        149, 153, 244, 245, 246, 247, 248, 249,
        250, 251, 252,
    };

    if (mouth_anim_id >= 0 && mouth_anim_id
        < sizeof(mouth_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return mouth_anim_id_to_mottbl_index_table[mouth_anim_id];
    return 131;
}

static void sub_140122B60(x_pv_game* a1, int32_t chara_id, int32_t motion_index, int64_t disp_time) {
    if (chara_id < 0 || chara_id > ROB_CHARA_COUNT || motion_index < 0)
        return;

    x_pv_play_data* playdata = &a1->playdata[chara_id];
    int64_t dsc_time = a1->dsc_time;
    for (x_dsc_set_motion& i : playdata->field_38.set_motion) {
        int64_t time = (int64_t)i.time * 10000;
        if (time > dsc_time && i.motion_index == motion_index
            && (!a1->branch_mode || a1->branch_mode == i.pv_branch_mode)) {
            rob_chara_bone_data* rob_bone_data = playdata->rob_chr->bone_data;
            rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.field_14 =
                roundf(dsc_time_to_frame(time - disp_time)) - 1.0f;
            break;
        }
    }
}

static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time) {
    light_param_storage_data_set_pv_cut(field);
}

static bool x_pv_game_dsc_process(x_pv_game* a1, int64_t curr_time) {
    dsc_x_func func = (dsc_x_func)a1->dsc_data_ptr->func;
    uint32_t* data = a1->dsc_m.get_func_data(a1->dsc_data_ptr);
    if (a1->branch_mode) {
        bool v19;
        if (a1->success)
            v19 = a1->branch_mode == 2;
        else
            v19 = a1->branch_mode == 1;
        if (!v19) {
            if (func < 0 || func > DSC_X_VR_LIVE_CHARA_VOICE) {
                a1->play = false;
                return false;
            }
            else if ((data[0] < 0 || data[0] > 1)
                && func != DSC_X_PV_END && func != DSC_X_PV_BRANCH_MODE)
                return true;
        }
    }

    switch (func) {
    case DSC_X_END: {
        a1->play = false;
        a1->state = 11;
        return false;
    } break;
    case DSC_X_TIME: {
        a1->dsc_time = (int64_t)(int32_t)data[0] * 10000;
        if (a1->dsc_time > curr_time)
            return false;
    } break;
    case DSC_X_MIKU_MOVE: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 trans;
        trans.x = (float_t)(int32_t)data[1] * 0.001f;
        trans.y = (float_t)(int32_t)data[2] * 0.001f;
        trans.z = (float_t)(int32_t)data[3] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4 scene_rot_mat = a1->scene_rot_mat;
        mat4_mult_vec3_trans(&scene_rot_mat, &trans, &trans);
        rob_chr->data.miku_rot.position = trans;
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_ROT: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t rot_y = (float_t)(int32_t)data[0] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + a1->scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t disp = (int32_t)data[1];

        playdata->disp = disp == 1;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chr->set_visibility(true);
            /*if (rob_chara::check_for_ageageagain_module(chara_index, module_index)) {
                sub_1405430F0(chara_id, 1);
                sub_1405430F0(chara_id, 2);
            }*/

            //pv_game::set_data_itmpv_visibility(a1->pv_game, a1->chara_id, true);
            for (x_pv_play_data_set_motion& i : playdata->set_motion) {
                bool v45 = rob_chr->set_motion_id(i.motion_id, i.frame,
                    i.duration, i.field_10, 0, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->disable_eye_motion = i.disable_eye_motion;
                rob_chr->data.motion.step_data.step = i.frame_speed;
                //if (v45)
                //    pv_expression_array_set_motion(a1->chara_id, i.motion_id);
                //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                    sub_140122B60(a1, a1->chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else {
            rob_chr->set_visibility(false);
            //pv_game::set_data_itmpv_visibility(a1->pv_game, a1->chara_id, false);
        }
    } break;
    case DSC_X_MIKU_SHADOW: {
        a1->chara_id = data[0];
    } break;
    case DSC_X_TARGET: {

    } break;
    case DSC_X_SET_MOTION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t motion_index = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t frame_speed_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1){
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t frame_speed;
        if (frame_speed_int != -1) {
            frame_speed = (float_t)frame_speed_int * 0.001f;
            if (frame_speed < 0.0f)
                frame_speed = 0.0f;
        }
        else
            frame_speed = 1.0f;

        float_t frame = 0.0f;
        float_t dsc_frame = 0.0f;

        bool v11 = false;
        x_pv_play_data_motion* v56 = 0;
        for (x_pv_play_data_motion& i : playdata->motion)
            if (i.motion_index == motion_index) {
                v56 = &i;
                break;
            }

        if (v56 && v56->enable && (motion_index == v56->motion_index || !v56->motion_index)) {
            if (a1->dsc_time != v56->time) {
                frame = dsc_time_to_frame(curr_time - v56->time);
                dsc_frame = roundf(dsc_time_to_frame(a1->dsc_time - v56->time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                v11 = curr_time > v56->time;
            }
            else
                frame = 0.0f;
        }

        int32_t motion_id = -1;
        int32_t chara_id = 0;
        for (pvpp_chara& i : a1->pp->chara) {
            if (chara_id++ != a1->chara_id)
                continue;

            int32_t mot_idx = 1;
            for (pvpp_motion& j : i.motion) {
                if (mot_idx++ != motion_index)
                    continue;

                motion_id = aft_mot_db->get_motion_id(j.name.c_str());
                break;
            }
            break;
        }

        /*pv_db_pv_difficulty* v64 = 0i64;
        pv_db_pv* v65 = a1->pv_game->data.pv;
        pv_db_pv_difficulty* v66 = v65->difficulty[sub_14013C8C0()->difficulty].begin;
        pv_db_pv* v67 = a1->pv_game->data.pv;
        if (v66 == v67->difficulty[sub_14013C8C0()->difficulty].end)
            return 1;
        do {
            if (v66->edition == sub_14013C8C0()->edition)
                v64 = v66;
            ++v66;
            pv_db_pv* v68 = a1->pv_game->data.pv;
        } while (v66 != v68->difficulty[sub_14013C8C0()->difficulty].end);
        if (!v64)
            return 1;

        int32_t motion_id = vector_pv_db_pv_motion_get_element_by_index_or_null(&v64->motion[a1->chara_id], v52)->id;
        if (pv_game_data_get()->data.pv) {
            pv_db_pv_motion* v76 = vector_pv_db_pv_motion_get_element_by_index_or_null(&v64->motion[a1->chara_id], v52);
            motion_id = sub_1404EFA20(pv_game_data_get()->data.pv, rob_chr->chara_id, rob_chr->chara_index, v76);
        }*/

        if (motion_index) {
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }
        else if (motion_id == -1) {
            motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(5);
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }

        duration /= a1->anim_frame_speed;
        if (v11)
            duration = 0.0f;

        x_pv_play_data_set_motion v519;
        v519.frame_speed = frame_speed * a1->anim_frame_speed;
        v519.motion_id = motion_id;
        v519.frame = frame;
        v519.duration = duration;
        v519.field_10 = v11;
        v519.blend_type = MOTION_BLEND_CROSS;
        v519.disable_eye_motion = true;
        v519.motion_index = motion_index;
        v519.dsc_time = v56 ? v56->time : a1->dsc_time;
        v519.dsc_frame = dsc_frame;

        //a1->field_2C560[a1->chara_id] = true;
        //a1->field_2C568[a1->chara_id] = v519;
        if (playdata->disp) {
            bool v84 = rob_chr->set_motion_id(motion_id, frame, duration,
                v11, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, dsc_frame);
            rob_chr->set_motion_skin_param(motion_id, dsc_frame);
            rob_chr->bone_data->disable_eye_motion = true;
            rob_chr->data.motion.step_data.step = v519.frame_speed;
            //if (v84)
            //    pv_expression_array_set_motion(a1->chara_id, motion_id);
            //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                sub_140122B60(a1, a1->chara_id, motion_index, v56 ? v56->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(v519);
            rob_chr->data.motion.step_data.step = v519.frame_speed;
        }
    } break;
    case DSC_X_SET_PLAYDATA: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t motion_index = (int32_t)data[1];
        if (motion_index < 0) {
            playdata->motion.clear();
            break;
        }

        x_pv_play_data_motion* motion = 0;
        for (x_pv_play_data_motion& i : playdata->motion)
            if (i.motion_index == motion_index) {
                motion = &i;
                break;
            }

        if (motion) {
            motion->enable = true;
            motion->motion_index = motion_index;
            motion->time = a1->dsc_time;
        }
        else {
            x_pv_play_data_motion v531;
            v531.enable = true;
            v531.motion_index = motion_index;
            v531.time = a1->dsc_time;
            playdata->motion.push_back(v531);
        }
    } break;
    case DSC_X_EFFECT: {

    } break;
    case DSC_X_FADEIN_FIELD: {

    } break;
    case DSC_X_EFFECT_OFF: {

    } break;
    case DSC_X_SET_CAMERA: {

    } break;
    case DSC_X_DATA_CAMERA: {

    } break;
    case DSC_X_CHANGE_FIELD: {
        int32_t field = (int32_t)data[0];
        if (field > 0)
            x_pv_game_change_field(a1, field, a1->dsc_time, curr_time);
        else
            x_pv_game_reset_field(a1);
    } break;
    case DSC_X_HIDE_FIELD: {

    } break;
    case DSC_X_MOVE_FIELD: {

    } break;
    case DSC_X_FADEOUT_FIELD: {

    } break;
    case DSC_X_EYE_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t v115 = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];

        float_t duration;
        if (duration_int != -1)
            duration = (float_t)duration_int * 0.001f * 60.0f;
        else
            duration = 6.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyelid_mottbl_motion_from_face(v115, duration, -1.0f, offset, aft_mot_db);
    } break;
    case DSC_X_MOUTH_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t mouth_anim_id = (int32_t)data[2];
        int32_t duration_int = (int32_t)data[3];
        int32_t value_int = (int32_t)data[4];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        switch (mouth_anim_id) {
            case  4: mouth_anim_id = 34; break;
            case  5: mouth_anim_id = 35; break;
            case  6: mouth_anim_id = 36; break;
            case  7: mouth_anim_id = 37; break;
            case  8: mouth_anim_id = 38; break;
            case  9: mouth_anim_id = 39; break;
            case 10: mouth_anim_id =  4; break;
            case 11: mouth_anim_id =  5; break;
            case 12: mouth_anim_id =  6; break;
            case 13: mouth_anim_id =  7; break;
            case 14: mouth_anim_id =  8; break;
            case 15: mouth_anim_id =  9; break;
            case 16: mouth_anim_id = 10; break;
            case 17: mouth_anim_id = 11; break;
            case 18: mouth_anim_id = 12; break;
            case 19: mouth_anim_id = 13; break;
            case 20: mouth_anim_id = 14; break;
            case 21: mouth_anim_id = 15; break;
            case 22: mouth_anim_id = 16; break;
            case 23: mouth_anim_id = 17; break;
            case 24: mouth_anim_id = 18; break;
            case 25: mouth_anim_id = 19; break;
            case 26: mouth_anim_id = 20; break;
            case 27: mouth_anim_id = 21; break;
            case 28: mouth_anim_id = 22; break;
            case 29: mouth_anim_id = 40; break;
            case 30: mouth_anim_id = 41; break;
            case 31: mouth_anim_id = 42; break;
            default: printf(""); break;
        }

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        duration /= a1->anim_frame_speed;

        //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_X_HAND_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t hand_index = (int32_t)data[1];
        int32_t hand_anim_id = (int32_t)data[2];
        int32_t duration_int = (int32_t)data[3];
        int32_t value_int = (int32_t)data[4];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(hand_anim_id);
        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        switch (hand_index) {
        case 0:
            rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, value,
                0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        case 1:
            rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, value,
                0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        }
    } break;
    case DSC_X_LOOK_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t look_anim_id = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t value_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = look_anim_id_to_mottbl_index(look_anim_id);
        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value,
            mottbl_index == 224 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
        return 1;
    } break;
    case DSC_X_EXPRESSION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t expression_id = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t value_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        duration /= a1->anim_frame_speed;

        bool v168 = true;
        //if (a1->has_perf_id && (a1->pv_game->data.pv->edit - 1) <= 1)
        //    v168 = false;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v168, aft_mot_db);

    } break;
    case DSC_X_LOOK_CAMERA: {

    } break;
    case DSC_X_LYRIC: {

    } break;
    case DSC_X_MUSIC_PLAY: {

    } break;
    case DSC_X_MODE_SELECT: {

    } break;
    case DSC_X_EDIT_MOTION: {

    } break;
    case DSC_X_BAR_TIME_SET: {
        int32_t bpm = (int32_t)data[0];
        if (bpm != a1->prev_bpm)
            diva_stage_frame_rate.SetFrameSpeed((float_t)bpm / 120.0f);
        a1->prev_bpm = bpm;
        a1->prev_bar_point_time = a1->dsc_time;
    } break;
    case DSC_X_SHADOWHEIGHT: {
        a1->chara_id = data[0];
    } break;
    case DSC_X_EDIT_FACE: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t expression_id = (int32_t)data[0];

        float_t duration = 0.0f;
        //if (func == DSC_EDIT_EXPRESSION)
        //    duration = (float_t)(int32_t)data[1] * 0.001f * 60.0f;

        int32_t v234 = -1;
        //if (!a1->has_perf_id)
        //    v234 = data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        duration /= a1->anim_frame_speed;

        bool v237 = true;
        //if (a1->has_perf_id)
        //    v237 = (a1->pv_game->data.pv->edit - 1) > 1;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v237, aft_mot_db);

        /*if (!a1->has_perf_id) {
            int32_t mottbl_index = mouth_anim_id_to_mottbl_index(v234);

            float_t value = 1.0f;
            //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
            //    value = 0.0f;

            rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0,
            a1->target_anim_fps * 0.1f, 0.0f, 1.0f, -1, offset, aft_mot_db);
        }*/
    } break;
    case DSC_X_DUMMY: {

    } break;
    case DSC_X_PV_END: {
        a1->pv_end = true;
        break;
    } break;
    case DSC_X_SHADOWPOS: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_EDIT_LYRIC: {

    } break;
    case DSC_X_EDIT_TARGET: {

    } break;
    case DSC_X_EDIT_MOUTH: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t mouth_anim_id = (int32_t)data[1];
        float_t duration = 0.1f;
        //if (func == DSC_EDIT_MOUTH_ANIM)
        //    duration = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        duration *= a1->target_anim_fps;

        float_t value = 1.0f;
        //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_X_SET_CHARA: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_EDIT_MOVE: {

    } break;
    case DSC_X_EDIT_SHADOW: {

    } break;
    case DSC_X_EDIT_EYELID: {

    } break;
    case DSC_X_EDIT_EYE: {

    } break;
    case DSC_X_EDIT_ITEM: {

    } break;
    case DSC_X_EDIT_EFFECT: {

    } break;
    case DSC_X_EDIT_DISP: {

    } break;
    case DSC_X_EDIT_HAND_ANIM: {

    } break;
    case DSC_X_AIM: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_HAND_ITEM: {

    } break;
    case DSC_X_EDIT_BLUSH: {

    } break;
    case DSC_X_NEAR_CLIP: {

    } break;
    case DSC_X_CLOTH_WET: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t value = (float_t)(int32_t)data[1] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->item_equip->wet = clamp(value, 0.0f, 1.0f);
    } break;
    case DSC_X_LIGHT_ROT: {

    } break;
    case DSC_X_SCENE_FADE: {

    } break;
    case DSC_X_TONE_TRANS: {
        vec3 start;
        vec3 end;
        start.x = (float_t)(int32_t)data[0] * 0.001f;
        start.y = (float_t)(int32_t)data[1] * 0.001f;
        start.z = (float_t)(int32_t)data[2] * 0.001f;
        end.x = (float_t)(int32_t)data[3] * 0.001f;
        end.y = (float_t)(int32_t)data[4] * 0.001f;
        end.z = (float_t)(int32_t)data[5] * 0.001f;
        rctx_ptr->post_process.tone_map->set_tone_trans(start, end);
    } break;
    case DSC_X_SATURATE: {
        float_t value = (float_t)(int32_t)data[0] * 0.001f;
        rctx_ptr->post_process.tone_map->set_saturate_coeff(value);
    } break;
    case DSC_X_FADE_MODE: {
        int32_t value = data[0];
        rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(value);
    } break;
    case DSC_X_AUTO_BLINK: {

    } break;
    case DSC_X_PARTS_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        item_id id = (item_id)data[1];
        int32_t disp = (int32_t)data[2];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_parts_disp(id, disp == 1);
    } break;
    case DSC_X_TARGET_FLYING_TIME: {

    } break;
    case DSC_X_CHARA_SIZE: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t chara_size = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            return 1;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1) {
            chara_index chara_index = CHARA_MIKU;
            if (a1->chara_id < a1->pp->chara.size())
                chara_index = (::chara_index)a1->pp->chara[a1->chara_id].chara_effect.base_chara;
            else if (a1->pv_id == 826)
                chara_index = rob_chara_array_get(a1->chara_id)->chara_index;

            //chara_index chara_index = pv_db_pv::get_performer_chara(a1->pv_game->data.pv, a1->chara_id);
            chara_size_index = chara_init_data_get_chara_size_index(chara_index);
        }
        else if (chara_size == 2)
            chara_size_index = 1;
        else if (chara_size == 3)
            chara_size_index = rob_chr->pv_data.chara_size_index;
        else {
            rob_chr->set_chara_size((float_t)chara_size / 1000.0f/*a1->field_2C54C*/);
            break;
        }

        if (chara_size_index < 0 || chara_size_index > 4)
            break;

        rob_chr->set_chara_size(chara_size_table_get_value(chara_size_index));
        rob_chr->set_chara_pos_adjust_y(chara_pos_adjust_y_table_get_value(chara_size_index));
    } break;
    case DSC_X_CHARA_HEIGHT_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t height_adjust = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_X_ITEM_ANIM: {

    } break;
    case DSC_X_CHARA_POS_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 pos;
        pos.x = (float_t)(int32_t)data[2] * 0.001f;
        pos.y = (float_t)(int32_t)data[3] * 0.001f;
        pos.z = (float_t)(int32_t)data[4] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4 scene_rot_mat = a1->scene_rot_mat;
        mat4_mult_vec3_trans(&scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(&pos);

        /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
            sub_1405430F0(rob_chr->chara_id, 1);
            sub_1405430F0(rob_chr->chara_id, 2);
        }*/
    } break;
    case DSC_X_SCENE_ROT: {
        float_t scene_rot_y = (float_t)(int32_t)data[0] * 0.001f;
        a1->scene_rot_y = scene_rot_y;

        mat4 scene_rot_mat = a1->scene_rot_mat;
        mat4_rotate_y(scene_rot_y * DEG_TO_RAD_FLOAT, &scene_rot_mat);
        a1->scene_rot_mat = scene_rot_mat;

    } break;
    case DSC_X_EDIT_MOT_SMOOTH_LEN: {

    } break;
    case DSC_X_PV_BRANCH_MODE: {
        int32_t branch_mode = (int32_t)data[0];
        if (branch_mode >= 0 && branch_mode <= 2)
            a1->branch_mode = branch_mode;
    } break;
    case DSC_X_DATA_CAMERA_START: {

    } break;
    case DSC_X_MOVIE_PLAY: {

    } break;
    case DSC_X_MOVIE_DISP: {

    } break;
    case DSC_X_WIND: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t wind_strength_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t wind_strength_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->item_equip->wind_strength = wind_strength_outer;
    } break;
    case DSC_X_OSAGE_STEP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_step_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_X_OSAGE_MV_CCL: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_mv_ccl_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_move_cancel(1, osage_mv_ccl_kami);
        rob_chr->set_osage_move_cancel(2, osage_mv_ccl_outer);
    } break;
    case DSC_X_CHARA_COLOR: {

    } break;
    case DSC_X_SE_EFFECT: {

    } break;
    case DSC_X_CHARA_SHADOW_QUALITY: {

    } break;
    case DSC_X_STAGE_SHADOW_QUALITY: {

    } break;
    case DSC_X_COMMON_LIGHT: {

    } break;
    case DSC_X_TONE_MAP: {

    } break;
    case DSC_X_IBL_COLOR: {

    } break;
    case DSC_X_REFLECTION: {

    } break;
    case DSC_X_CHROMATIC_ABERRATION: {

    } break;
    case DSC_X_STAGE_SHADOW: {

    } break;
    case DSC_X_REFLECTION_QUALITY: {

    } break;
    case DSC_X_PV_END_FADEOUT: {

    } break;
    case DSC_X_CREDIT_TITLE: {

    } break;
    case DSC_X_BAR_POINT: {
        if (a1->prev_bar_point_time != -1) {
            float_t frame_speed = 200000.0f / ((float_t)(a1->dsc_time - a1->prev_bar_point_time) / 10000.0f);
            int32_t bpm = (int32_t)roundf(frame_speed * 120.0f);
            if (bpm != a1->prev_bpm)
                diva_stage_frame_rate.SetFrameSpeed((float_t)bpm / 120.0f);
            a1->prev_bpm = bpm;
        }
        a1->prev_bar_point_time = a1->dsc_time;
    } break;
    case DSC_X_BEAT_POINT: {

    } break;
    case DSC_X_RESERVE1: {

    } break;
    case DSC_X_PV_AUTH_LIGHT_PRIORITY: {

    } break;
    case DSC_X_PV_CHARA_LIGHT: {

    } break;
    case DSC_X_PV_STAGE_LIGHT: {

    } break;
    case DSC_X_TARGET_EFFECT: {

    } break;
    case DSC_X_FOG: {

    } break;
    case DSC_X_BLOOM: {

    } break;
    case DSC_X_COLOR_CORRECTION: {

    } break;
    case DSC_X_DOF: {

    } break;
    case DSC_X_CHARA_ALPHA: {
        a1->chara_id = (int32_t)data[0];

        float_t alpha = (float_t)(int32_t)data[1] * 0.001f;
        float_t duration = (float_t)(int32_t)data[2];
        int32_t type = (int32_t)data[3];

        if (a1->chara_id >= 0 && a1->chara_id < ROB_CHARA_COUNT) {
            pv_param::chara_alpha& chara_alpha
                = pv_param_task::post_process_task.chara_alpha.data.data[a1->chara_id];
            chara_alpha.type = type;
            chara_alpha.frame = 0.0f;
            chara_alpha.alpha = alpha;
            chara_alpha.duration = duration;
        }

    } break;
    case DSC_X_AUTO_CAPTURE_BEGIN: {

    } break;
    case DSC_X_MANUAL_CAPTURE: {

    } break;
    case DSC_X_TOON_EDGE: {

    } break;
    case DSC_X_SHIMMER: {

    } break;
    case DSC_X_ITEM_ALPHA: {
        a1->chara_id = (int32_t)data[0];

        float_t alpha = (float_t)(int32_t)data[1] * 0.001f;
        float_t duration = (float_t)(int32_t)data[2];
        int32_t type = (int32_t)data[3];

        if (a1->chara_id >= 0 && a1->chara_id < ROB_CHARA_COUNT) {
            pv_param::chara_alpha& chara_item_alpha
                = pv_param_task::post_process_task.chara_item_alpha.data.data[a1->chara_id];
            chara_item_alpha.type = type;
            chara_item_alpha.frame = 0.0f;
            chara_item_alpha.alpha = alpha;
            chara_item_alpha.duration = duration;
        }
    } break;
    case DSC_X_MOVIE_CUT: {

    } break;
    case DSC_X_EDIT_CAMERA_BOX: {

    } break;
    case DSC_X_EDIT_STAGE_PARAM: {

    } break;
    case DSC_X_EDIT_CHANGE_FIELD: {

    } break;
    case DSC_X_MIKUDAYO_ADJUST: {

    } break;
    case DSC_X_LYRIC_2: {

    } break;
    case DSC_X_LYRIC_READ: {

    } break;
    case DSC_X_LYRIC_READ_2: {

    } break;
    case DSC_X_ANNOTATION: {

    } break;
    case DSC_X_STAGE_EFFECT: {
        int32_t effect_id = (int32_t)data[0];
        effect_id--;
        uint32_t hash = a1->stage_glitter->hash;

        if (effect_id < 0 || effect_id >= a1->sr->stage_effect.size())
            break;

        if (effect_id >= 7 && effect_id <= 8) {
            pvsr_stage_effect& effect = a1->sr->stage_effect[effect_id];
            for (pvsr_a3da& i : effect.a3da) {
                int32_t& id = a1->stage_auth_3d_ids[i.hash];
                auth_3d_data_set_camera_root_update(&id, false);
                auth_3d_data_set_enable(&id, true);
                auth_3d_data_set_req_frame(&id, 0.0f);
                auth_3d_data_set_max_frame(&id, -1.0f);
                auth_3d_data_set_paused(&id, false);
                auth_3d_data_set_visibility(&id, true);
                auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
            }

            for (pvsr_glitter& i : effect.glitter) {
                uint32_t effect_hash = hash_string_murmurhash(&i.name);
                Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash);
                Glitter::glt_particle_manager.SetSceneName(hash, a1->stage_glitter->name.c_str());
                Glitter::glt_particle_manager.SetSceneEffectName(
                    hash_murmurhash_empty, effect_hash, i.name.c_str());
                Glitter::glt_particle_manager.SetSceneFrameRate(hash, &diva_stage_frame_rate);
            }
        }
    } break;
    case DSC_X_SONG_EFFECT: {
        bool enable = data[0] ? true : false;
        int32_t effect_id = (int32_t)data[1];

        if (effect_id < 0 || effect_id >= a1->pp->effect.size())
            break;

        uint32_t hash = a1->pv_glitter->hash;

        pvpp_effect& effect = a1->pp->effect[effect_id];
        if (enable) {
            for (pvpp_a3da& i : effect.a3da) {
                int32_t& id = a1->pv_auth_3d_ids[i.hash];
                auth_3d_data_set_repeat(&id, false);
                auth_3d_data_set_camera_root_update(&id, false);
                auth_3d_data_set_enable(&id, true);
                auth_3d_data_set_req_frame(&id, 0.0f);
                auth_3d_data_set_max_frame(&id, -1.0f);
                auth_3d_data_set_paused(&id, false);
                auth_3d_data_set_visibility(&id, true);
                auth_3d_data_set_frame_rate(&id, &diva_pv_frame_rate);
                if (effect.chara_index != PVPP_CHARA_STAGE)
                    auth_3d_data_set_chara_id(&id, effect.chara_index);
            }

            for (pvpp_glitter& i : effect.glitter) {
                uint32_t effect_hash = hash_string_murmurhash(&i.name);
                Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash);
                Glitter::glt_particle_manager.SetSceneName(hash, a1->pv_glitter->name.c_str());
                Glitter::glt_particle_manager.SetSceneEffectName(
                    hash_murmurhash_empty, effect_hash, i.name.c_str());
                Glitter::glt_particle_manager.SetSceneFrameRate(hash, &diva_pv_frame_rate);
            }
        }
        else {
            for (pvpp_a3da& i : effect.a3da) {
                int32_t& id = a1->pv_auth_3d_ids[i.hash];
                auth_3d_data_set_visibility(&id, false);
                auth_3d_data_set_enable(&id, false);
            }

            for (pvpp_glitter& i : effect.glitter)
                Glitter::glt_particle_manager.FreeSceneEffect(
                    0, hash_string_murmurhash(&i.name), false);
        }
    } break;
    case DSC_X_SONG_EFFECT_ATTACH: {
        int32_t effect_id = (int32_t)data[0];
        int32_t chara_id = (int32_t)data[1];

        if (effect_id >= a1->pp->effect.size())
            break;

        auto elem = a1->pv_auth_3d_chara_count.find(effect_id);
        if (elem != a1->pv_auth_3d_chara_count.end() && elem->second.size() > 1)
            chara_id = -1;

        pvpp_effect& effect = a1->pp->effect[effect_id];
        for (pvpp_a3da& i : effect.a3da) {
            int32_t& id = a1->pv_auth_3d_ids[i.hash];
            auth_3d_data_set_chara_id(&id, chara_id);
        }
    } break;
    case DSC_X_LIGHT_AUTH: {

    } break;
    case DSC_X_FADE: {

    } break;
    case DSC_X_SET_STAGE_EFFECT_ENV: {

    } break;
    case DSC_X_RESERVE2: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT_LOW: {

    } break;
    case DSC_X_COMMON_EFFECT_PARTICLE: {

    } break;
    case DSC_X_SONG_EFFECT_ALPHA_SORT: {

    } break;
    case DSC_X_LOOK_CAMERA_FACE_LIMIT: {

    } break;
    case DSC_X_ITEM_LIGHT: {

    } break;
    case DSC_X_CHARA_EFFECT: {
        return true;
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        bool enable = data[1] ? true : false;
        int32_t chara_effect_id = (int32_t)data[2];

        pvpp_chara& chara = a1->pp->chara[a1->chara_id];
        if (!chara.chara_effect_init)
            break;
        else if (chara_effect_id < 0 || chara_effect_id >= chara.chara_effect.effect_a3da.size())
            break;

        uint32_t hash = chara.chara_effect.effect_a3da[chara_effect_id].a3da.hash;
        hash = a1->effchrpv_auth_3d_hashes[hash];
        if (enable) {
            int32_t& id = a1->effchrpv_auth_3d_ids[hash];
            auth_3d_data_set_repeat(&id, false);
            auth_3d_data_set_camera_root_update(&id, false);
            auth_3d_data_set_enable(&id, true);
            auth_3d_data_set_req_frame(&id, (float_t)a1->frame_float);
            auth_3d_data_set_max_frame(&id, -1.0f);
            auth_3d_data_set_paused(&id, false);
            auth_3d_data_set_visibility(&id, true);
            auth_3d_data_set_frame_rate(&id, &diva_pv_frame_rate);
            auth_3d_data_set_chara_id(&id, a1->chara_id);
        }
        else {
            int32_t& id = a1->effchrpv_auth_3d_ids[hash];
            auth_3d_data_set_visibility(&id, false);
            auth_3d_data_set_enable(&id, false);
        }
    } break;
    case DSC_X_MARKER: {

    } break;
    case DSC_X_CHARA_EFFECT_CHARA_LIGHT: {

    } break;
    case DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA: {

    } break;
    case DSC_X_ENABLE_FXAA: {

    } break;
    case DSC_X_ENABLE_TEMPORAL_AA: {

    } break;
    case DSC_X_ENABLE_REFLECTION: {

    } break;
    case DSC_X_BANK_BRANCH: {

    } break;
    case DSC_X_BANK_END: {

    } break;
    case DSC_X_NULL1: {

    } break;
    case DSC_X_NULL2: {

    } break;
    case DSC_X_NULL3: {

    } break;
    case DSC_X_NULL4: {

    } break;
    case DSC_X_NULL5: {

    } break;
    case DSC_X_NULL6: {

    } break;
    case DSC_X_NULL7: {

    } break;
    case DSC_X_NULL8: {

    } break;
    case DSC_X_VR_LIVE_MOVIE: {

    } break;
    case DSC_X_VR_CHEER: {

    } break;
    case DSC_X_VR_CHARA_PSMOVE: {

    } break;
    case DSC_X_VR_MOVE_PATH: {

    } break;
    case DSC_X_VR_SET_BASE: {

    } break;
    case DSC_X_VR_TECH_DEMO_EFFECT: {

    } break;
    case DSC_X_VR_TRANSFORM: {

    } break;
    case DSC_X_GAZE: {

    } break;
    case DSC_X_TECH_DEMO_GESUTRE: {

    } break;
    case DSC_X_VR_CHEMICAL_LIGHT_COLOR: {

    } break;
    case DSC_X_VR_LIVE_MOB: {

    } break;
    case DSC_X_VR_LIVE_HAIR_OSAGE: {

    } break;
    case DSC_X_VR_LIVE_LOOK_CAMERA: {

    } break;
    case DSC_X_VR_LIVE_CHEER: {

    } break;
    case DSC_X_VR_LIVE_GESTURE: {

    } break;
    case DSC_X_VR_LIVE_CLONE: {

    } break;
    case DSC_X_VR_LOOP_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_ONESHOT_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_PRESENT: {

    } break;
    case DSC_X_VR_LIVE_TRANSFORM: {

    } break;
    case DSC_X_VR_LIVE_FLY: {

    } break;
    case DSC_X_VR_LIVE_CHARA_VOICE: {

    } break;
    }
    return true;
}

static void set_bone_key_set_data(bone_data* bone_data,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& bone_keys,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& second_bone_keys, bool add_keys,
    motion_bone_index motion_bone_index, mot_key_set* key_set, vec3* data, int32_t count = 1) {
    if (add_keys) {
        auto elem = bone_keys.find(motion_bone_index);
        if (elem == bone_keys.end())
            elem = bone_keys.insert({ motion_bone_index, {} }).first;

        x_pv_game_a3da_to_mot_keys& keys = elem->second;
        keys.x.push_back(data[0].x);
        keys.y.push_back(data[0].y);
        keys.z.push_back(data[0].z);

        if (count == 2) {
            auto elem = second_bone_keys.find(motion_bone_index);
            if (elem == second_bone_keys.end())
                elem = second_bone_keys.insert({ motion_bone_index, {} }).first;

            x_pv_game_a3da_to_mot_keys& keys = elem->second;
            keys.x.push_back(data[1].x);
            keys.y.push_back(data[1].y);
            keys.z.push_back(data[1].z);
        }
    }

    bone_data += motion_bone_index;
    key_set += bone_data->key_set_offset;
    while (count > 0) {
        if (key_set[0].type == MOT_KEY_SET_STATIC && key_set[0].values)
            *(float_t*)&key_set[0].values[0] = data->x;
        if (key_set[1].type == MOT_KEY_SET_STATIC && key_set[1].values)
            *(float_t*)&key_set[1].values[0] = data->y;
        if (key_set[2].type == MOT_KEY_SET_STATIC && key_set[2].values)
            *(float_t*)&key_set[2].values[0] = data->z;
        key_set += 3;
        data++;
        count--;
    }
}

static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys) {
    for (auto& i : xpvgm->effchrpv_auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(xpvgm->rob_chara_ids[i.first]);
        x_pv_game_a3da_to_mot& a2m = i.second;
        auth_3d* auth = auth_3d_data_get_auth_3d(a2m.auth_3d_id);
        auth_3d_object_hrc* oh = &auth->object_hrc[0];

        rob_chr->set_visibility(oh->node[0].model_transform.visible);

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        ::bone_data* bone_data = mot->bone_data.bones.data();
        mot_key_set* key_set = mot->mot_key_data.mot.key_sets;

        vec3 data[2];
        mat4 mat;
        data[0].x = oh->node[a2m.gblctr].model_transform.translation_value.x;
        data[0].y = oh->node[a2m.n_hara].model_transform.translation_value.y;
        data[0].z = oh->node[a2m.gblctr].model_transform.translation_value.z;
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA_CP, key_set, data, 2);

        data[0] = oh->node[a2m.n_hara_y].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KG_HARA_Y, key_set, data);

        data[0] = oh->node[a2m.j_hara_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_HARA_XZ, key_set, data);

        data[0] = oh->node[a2m.n_kosi].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA, key_set, data);

        mat = oh->node[a2m.j_mune_wj].model_transform.mat;
        data[0] = { 0.0f, 0.945f, 0.0f };
        mat4_mult_vec3(&mat, &data[0], &data[1]);
        mat4_get_translation(&mat, &data[0]);
        vec3_add(data[0], data[1], data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MUNE, key_set, data, 2);

        data[0] = oh->node[a2m.j_mune_b_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_MUNE_B_WJ, key_set, data);

        data[0] = oh->node[a2m.j_kubi_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_KUBI, key_set, data);

        data[0] = oh->node[a2m.n_kao].model_transform.rotation_value;
        data[0].z -= (float_t)M_PI;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KAO, key_set, data);

        mat = oh->node[a2m.j_kao_wj].model_transform.mat;
        data[0] = { 0.0f, 0.40f, 0.0f };
        mat4_mult_vec3(&mat, &data[0], &data[1]);
        mat4_get_translation(&mat, &data[0]);
        vec3_add(data[0], data[1], data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_KAO, key_set, data, 2);

        data[0] = oh->node[a2m.j_eye_r_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_R, key_set, data);

        data[0] = oh->node[a2m.j_eye_l_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_L, key_set, data);

        data[0] = oh->node[a2m.n_waki_l].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_L, key_set, data);

        data[1] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        data[0].x = data[1].x;
        data[0].y = data[1].z;
        data[0].z = -data[1].y;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_L_WJ, key_set, data);

        {
            vec3 pos_j_kata_l_wj;
            vec3 pos_j_ude_l_wj;
            vec3 pos_j_te_l_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_l_wj].model_transform.mat, &pos_j_kata_l_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_l_wj].model_transform.mat, &pos_j_ude_l_wj);
            mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &pos_j_te_l_wj);

            vec3 pos_middle;
            vec3_lerp_scalar(pos_j_kata_l_wj, pos_j_te_l_wj, pos_middle, 0.5f);

            vec3 tl_up_kata_dir;
            vec3_sub(pos_j_ude_l_wj, pos_middle, tl_up_kata_dir);

            float_t pos_middle_dist;
            vec3_length(tl_up_kata_dir, pos_middle_dist);
            if (pos_middle_dist == 0.0f) {
                mat = oh->node[a2m.j_kata_l_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_mult_vec3(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                vec3_mult_scalar(tl_up_kata_dir, 0.3f / pos_middle_dist, tl_up_kata_dir);

            vec3 tl_up_kata_pos;
            vec3_add(tl_up_kata_dir, pos_j_ude_l_wj, tl_up_kata_pos);

            mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_mult_vec3_inv_trans(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_L, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &data[0]);
        data[1].x = -(float_t)(M_PI / 2.0);
        data[1].y = -(float_t)(M_PI / 2.0);
        data[1].z = -(float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_L, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_L_WJ, key_set, data);

        data[0] = oh->node[a2m.n_waki_r].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_R, key_set, data);

        data[1] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        data[0].x = data[1].x;
        data[0].y = data[1].z;
        data[0].z = data[1].y;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_R_WJ, key_set, data);

        {
            vec3 pos_j_kata_r_wj;
            vec3 pos_j_ude_r_wj;
            vec3 pos_j_te_r_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_r_wj].model_transform.mat, &pos_j_kata_r_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_r_wj].model_transform.mat, &pos_j_ude_r_wj);
            mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &pos_j_te_r_wj);

            vec3 pos_middle;
            vec3_lerp_scalar(pos_j_kata_r_wj, pos_j_te_r_wj, pos_middle, 0.5f);

            vec3 tl_up_kata_dir;
            vec3_sub(pos_j_ude_r_wj, pos_middle, tl_up_kata_dir);

            float_t pos_middle_dist;
            vec3_length(tl_up_kata_dir, pos_middle_dist);
            if (pos_middle_dist == 0.0f) {
                mat = oh->node[a2m.j_kata_r_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_mult_vec3(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                vec3_mult_scalar(tl_up_kata_dir, 0.3f / pos_middle_dist, tl_up_kata_dir);

            vec3 tl_up_kata_pos;
            vec3_add(tl_up_kata_dir, pos_j_ude_r_wj, tl_up_kata_pos);

            mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_mult_vec3_inv_trans(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_R, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &data[0]);
        data[1].x = -(float_t)(M_PI / 2.0);
        data[1].y = (float_t)(M_PI / 2.0);
        data[1].z = (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_R_WJ, key_set, data);

        mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_L, key_set, data, 2);

        mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_R, key_set, data, 2);

        data[0] = { 0.0491406508f, 0.0f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_AGO_WJ, key_set, data);

        data[0] = { 0.0f, 0.0331281610f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KUBI_WJ_EX, key_set, data);
    }
}

static void x_pv_game_reset_field(x_pv_game* xpvgm) {
    task_stage_modern_info v14;
    task_stage_modern_set_stage(&v14);
    Glitter::glt_particle_manager.FreeScenes();
}

static void x_pv_game_stage_effect_ctrl(x_pv_game* xpvgm) {
    x_pv_game_stage_effect& stage_effect = xpvgm->stage_effect;

    if (stage_effect.next_stage_effect != -1 && stage_effect.prev_stage_effect == -1) {
        if (stage_effect.stage_effect == -1) {
            pvsr_stage_effect& effect = xpvgm->sr->stage_effect[stage_effect.next_stage_effect];

            x_pv_game_stage_effect_start(xpvgm, &effect);

            stage_effect.frame = 0.0f;
            if (effect.bar_count && effect.bar_count != 0xFF)
                stage_effect.last_frame = (float_t)(effect.bar_count * 120);
            else
                stage_effect.last_frame = 240.0f;
            stage_effect.stage_effect = stage_effect.next_stage_effect;
            stage_effect.next_stage_effect = -1;
        }
        else if (stage_effect.frame >= stage_effect.last_frame) {
            pvsr_stage_effect& effect = xpvgm->sr->stage_effect[stage_effect.stage_effect];
            pvsr_stage_effect& change_effect = xpvgm->sr->stage_change_effect
                [stage_effect.stage_effect][stage_effect.next_stage_effect];

            x_pv_game_stage_effect_stop(xpvgm, &effect,
                !!change_effect.a3da.size(), !!change_effect.glitter.size());
            x_pv_game_stage_effect_start(xpvgm, &change_effect);

            stage_effect.frame = 0.0f;
            if (change_effect.bar_count && change_effect.bar_count != 0xFF)
                stage_effect.last_frame = (float_t)(change_effect.bar_count * 120);
            else
                stage_effect.last_frame = 240.0f;
            stage_effect.prev_stage_effect = stage_effect.stage_effect;
            stage_effect.stage_effect = stage_effect.next_stage_effect;
            stage_effect.next_stage_effect = -1;
        }
    }
    else if (stage_effect.prev_stage_effect != -1) {
        if (stage_effect.stage_effect != -1 && stage_effect.frame >= stage_effect.last_frame) {
            if (xpvgm->sr->stage_change_effect_init[stage_effect.prev_stage_effect][stage_effect.stage_effect]) {
                pvsr_stage_effect& effect = xpvgm->sr->stage_effect[stage_effect.prev_stage_effect];
                pvsr_stage_effect& change_effect = xpvgm->sr->stage_change_effect
                    [stage_effect.prev_stage_effect][stage_effect.stage_effect];

                x_pv_game_stage_effect_stop(xpvgm, &change_effect);
                x_pv_game_stage_effect_stop(xpvgm, &effect,
                    !change_effect.a3da.size(), !change_effect.glitter.size());
            }
            else
                x_pv_game_stage_effect_stop(xpvgm,
                    &xpvgm->sr->stage_effect[stage_effect.prev_stage_effect], 0);

            pvsr_stage_effect& effect = xpvgm->sr->stage_effect[stage_effect.stage_effect];
            x_pv_game_stage_effect_start(xpvgm, &effect);

            stage_effect.frame = 0.0f;
            if (effect.bar_count && effect.bar_count != 0xFF)
                stage_effect.last_frame = (float_t)(effect.bar_count * 120);
            else
                stage_effect.last_frame = 240.0f;
            stage_effect.prev_stage_effect = -1;
            stage_effect.next_stage_effect = -1;
        }
    }

    if (stage_effect.frame >= stage_effect.last_frame)
        stage_effect.frame -= stage_effect.last_frame;
}

static void x_pv_game_stage_effect_start(x_pv_game* xpvgm, pvsr_stage_effect* stage_effect) {
    uint32_t hash = xpvgm->stage_glitter->hash;
    for (pvsr_a3da& i : stage_effect->a3da) {
        int32_t& id = xpvgm->stage_auth_3d_ids[i.hash];
        auth_3d_data_set_camera_root_update(&id, false);
        auth_3d_data_set_enable(&id, true);
        auth_3d_data_set_repeat(&id, true);
        auth_3d_data_set_req_frame(&id, 0.0f);
        auth_3d_data_set_max_frame(&id, -1.0f);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_visibility(&id, true);
        auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
    }

    for (pvsr_glitter& i : stage_effect->glitter) {
        uint32_t effect_hash = hash_string_murmurhash(&i.name);
        Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash);
        Glitter::glt_particle_manager.SetSceneName(hash, xpvgm->stage_glitter->name.c_str());
        Glitter::glt_particle_manager.SetSceneEffectName(
            hash_murmurhash_empty, effect_hash, i.name.c_str());
        Glitter::glt_particle_manager.SetSceneFrameRate(hash, &diva_stage_frame_rate);
    }
}

static void x_pv_game_stage_effect_stop(x_pv_game* xpvgm,
    pvsr_stage_effect* stage_effect, bool stop_a3da, bool stop_glitter) {
    if (stop_a3da)
        for (pvsr_a3da& i : stage_effect->a3da) {
            int32_t& id = xpvgm->stage_auth_3d_ids[i.hash];
            auth_3d_data_set_visibility(&id, false);
            auth_3d_data_set_enable(&id, false);
        }

    if (stop_glitter)
        for (pvsr_glitter& i : stage_effect->glitter)
            Glitter::glt_particle_manager.FreeSceneEffect(
                0, hash_string_murmurhash(&i.name), false);
}

static bool x_pv_game_stage_effects_process(x_pv_game* xpvgm, int32_t frame) {
    if (xpvgm->stage_effects_ptr->frame > xpvgm->frame)
        return false;
    else if (xpvgm->stage_effects_ptr->frame < xpvgm->frame)
        return true;

    xpvgm->stage_effect.next_stage_effect = xpvgm->stage_effects_ptr->effect_id;
    return true;
}

static dsc_data* sub_1401216D0(x_pv_game* a1, dsc_x_func func_name,
    int32_t* time, int32_t* pv_branch_mode, dsc_data* start, dsc_data* end) {
    int32_t _time = -1;
    for (dsc_data* i = start; i != end; i++)
        if (i->func == DSC_X_END)
            break;
        else if (i->func == DSC_X_TIME) {
            uint32_t* data = a1->dsc_m.get_func_data(i);
            _time = (int32_t)data[0];
        }
        else if (i->func == DSC_X_PV_END)
            break;
        else if (i->func == DSC_X_PV_BRANCH_MODE) {
            if (pv_branch_mode) {
                uint32_t* data = a1->dsc_m.get_func_data(i);
                *pv_branch_mode = (int32_t)data[0];
            }
        }
        else if (i->func == func_name) {
            if (time)
                *time = _time;
            return i;
        }
    return 0;
}

static void sub_140121A80(x_pv_game* a1, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;
    a1->playdata[chara_id].field_38.set_item.clear();

    dsc_data* i = a1->dsc_m.data.data();
    dsc_data* i_end = a1->dsc_m.data.data() + a1->dsc_m.data.size();
    while(true) {
        i = sub_1401216D0(a1, DSC_X_ITEM_ANIM, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = a1->dsc_m.get_func_data(i);
        if (chara_id == data[0] && data[2] == 2) {
            x_dsc_set_item item;
            item.item_index = data[1];
            item.time = time;
            item.pv_branch_mode = pv_branch_mode;
            a1->playdata[chara_id].field_38.set_item.push_back(item);
        }
        prev_time = time;
        i++;
    }
}

static void sub_140122770(x_pv_game* a1, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;
    a1->playdata[chara_id].field_38.set_motion.clear();

    dsc_data* i = a1->dsc_m.data.data();
    dsc_data* i_end = a1->dsc_m.data.data() + a1->dsc_m.data.size();
    while (true) {
        i = sub_1401216D0(a1, DSC_X_SET_MOTION, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = a1->dsc_m.get_func_data(i);
        if (chara_id == data[0]) {
            x_dsc_set_motion motion;
            motion.motion_index = data[1];
            motion.time = time;
            motion.pv_branch_mode = pv_branch_mode;
            a1->playdata[chara_id].field_38.set_motion.push_back(motion);
        }
        prev_time = time;
        i++;
    }
}
#endif
