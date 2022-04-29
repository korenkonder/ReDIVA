/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(CLOUD_DEV)
#include <set>
#include "x_pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.h"
#include "../CRE/data.h"
#include "../CRE/object.h"
#include "../CRE/stage_modern.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/sort.h"
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
static bool x_pv_game_dsc_process(x_pv_game* xpvgm, int64_t curr_time);
static void x_pv_game_stage_effect_ctrl(x_pv_game* xpvgm);
static void x_pv_game_stage_effect_start(x_pv_game* xpvgm, pvsr_stage_effect* stage_effect);
static void x_pv_game_stage_effect_stop(x_pv_game* xpvgm,
    pvsr_stage_effect* stage_effect, bool stop_a3da = true, bool stop_glitter = true);
static bool x_pv_game_stage_effects_process(x_pv_game* xpvgm, int32_t frame);

static dsc_data* sub_1401216D0(x_pv_game* a1, dsc_x_func func_name,
    int32_t* time, int32_t* pv_branch_mode, dsc_data* start, dsc_data* end);
static void sub_140121A80(x_pv_game* a1, int32_t chara_id);
static void sub_140122770(x_pv_game* a1, int32_t chara_id);

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
    if (state == 0)
        return false;
    else if (state == 1) {
        bool wait_load = false;
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                wait_load |= !task_rob_manager_check_chara_loaded(rob_chara_ids[i]);

        if (!wait_load)
            state = 2;
    }
    else if (state == 2) {
        bool pv_glt = pv_glitter->hash != hash_murmurhash_empty;
        bool stg_glt = stage_glitter->hash != hash_murmurhash_empty;
        if ((!pv_glt || Glitter::glt_particle_manager.CheckNoFileReaders(pv_glitter->hash))
            && (!stg_glt || Glitter::glt_particle_manager.CheckNoFileReaders(stage_glitter->hash))) {
            data_struct* x_data = &data_list[DATA_X];

            bool wait_load = false;
            if (pv_glt) {
                Glitter::EffectGroup* pv_eff_group
                    = Glitter::glt_particle_manager.GetEffectGroup(pv_glitter->hash);
                if (pv_eff_group && pv_eff_group->CheckModel()) {
                    pv_eff_group->LoadModel(x_data);
                    wait_load = true;
                }
            }

            if (stg_glt) {
                Glitter::EffectGroup* stage_eff_group
                    = Glitter::glt_particle_manager.GetEffectGroup(stage_glitter->hash);
                if (stage_eff_group && stage_eff_group->CheckModel()) {
                    stage_eff_group->LoadModel(x_data);
                    wait_load = true;
                }
            }

            state = wait_load ? 3 : 4;
        }
    }
    else if (state == 3) {
        bool wait_load = false;
        Glitter::EffectGroup* pv_eff_group
            = Glitter::glt_particle_manager.GetEffectGroup(pv_glitter->hash);
        if (pv_eff_group && pv_eff_group->CheckLoadModel())
            wait_load = true;

        Glitter::EffectGroup* stage_eff_group
            = Glitter::glt_particle_manager.GetEffectGroup(stage_glitter->hash);
        if (stage_eff_group && stage_eff_group->CheckLoadModel())
            wait_load = true;

        if (!wait_load)
            state = 4;
    }
    else if (state == 4) {
        if (!object_storage_get_obj_set_handler(effpv_objset)
            || !object_storage_load_obj_set_check_not_read(
            effpv_objset, &obj_db, &tex_db))
            state = 6;
    }
    else if (state == 6) {
        if (!object_storage_get_obj_set_handler(stgpv_objset)
            || !object_storage_load_obj_set_check_not_read(
            stgpv_objset, &obj_db, &tex_db))
            state = 7;
    }
    else if (state == 7) {
        if (!object_storage_get_obj_set_handler(stgpvhrc_objset)
            || !object_storage_load_obj_set_check_not_read(
            stgpvhrc_objset, &obj_db, &tex_db))
            state = 8;
    }
    else if (state == 8) {
        obj_set* effpv_objset = object_storage_get_obj_set(this->effpv_objset);
        if (effpv_objset)
            for (int32_t i = 0; i < effpv_objset->objects_count; i++) {
                obj* obj = &effpv_objset->objects[i];
                for (int32_t j = 0; j < obj->materials_count; j++) {
                    obj_material* material = &obj->materials[j].material;
                    vec3 spec = { 0.5f, 0.5f, 0.5f };
                    if (!memcmp(&material->specular, &spec, sizeof(vec3)))
                        *(vec3*)&material->specular = vec3_null;
                    else if (memcmp(&material->specular, &vec3_null, sizeof(vec3)))
                        *(vec3*)&material->specular = { 0.1f, 0.1f, 0.1f };
                }
            }

        obj_set* stgpv_objset = object_storage_get_obj_set(this->stgpv_objset);
        if (stgpv_objset)
            for (int32_t i = 0; i < stgpv_objset->objects_count; i++) {
                obj* obj = &stgpv_objset->objects[i];
                for (int32_t j = 0; j < obj->materials_count; j++) {
                    obj_material* material = &obj->materials[j].material;
                    vec3 spec = { 0.5f, 0.5f, 0.5f };
                    if (!memcmp(&material->specular, &spec, sizeof(vec3)))
                        *(vec3*)&material->specular = vec3_null;
                    else if (memcmp(&material->specular, &vec3_null, sizeof(vec3)))
                        *(vec3*)&material->specular = { 0.1f, 0.1f, 0.1f };
                }
            }

        obj_set* stgpvhrc_objset = object_storage_get_obj_set(this->stgpvhrc_objset);
        if (stgpvhrc_objset)
            for (int32_t i = 0; i < stgpvhrc_objset->objects_count; i++) {
                obj* obj = &stgpvhrc_objset->objects[i];
                for (int32_t j = 0; j < obj->materials_count; j++) {
                    obj_material* material = &obj->materials[j].material;
                    vec3 spec = { 0.5f, 0.5f, 0.5f };
                    if (!memcmp(&material->specular, &spec, sizeof(vec3)))
                        *(vec3*)&material->specular = vec3_null;
                    else if (memcmp(&material->specular, &vec3_null, sizeof(vec3)))
                        *(vec3*)&material->specular = { 0.1f, 0.1f, 0.1f };
                }
            }

        data_struct* x_data = &data_list[DATA_X];

        task_stage_modern_set_data(x_data, &obj_db, &tex_db, &stage_data);
        task_stage_modern_set_stage_hashes(&stage_hashes, &stages_data);

        auth_3d_data_load_category(x_data, pv_category.c_str(), pv_category_hash);
        auth_3d_data_load_category(x_data, stage_category.c_str(), stage_category_hash);
        auth_3d_data_load_category(light_category.c_str());
        auth_3d_data_load_category(x_data, camera_category.c_str(), camera_category_hash);
        state = 9;
    }
    else if (state == 9) {
        if (!auth_3d_data_check_category_loaded(pv_category_hash)
            || !auth_3d_data_check_category_loaded(stage_category_hash)
            || !auth_3d_data_check_category_loaded(light_category.c_str())
            || !auth_3d_data_check_category_loaded(camera_category_hash))
            return false;

        data_struct* x_data = &data_list[DATA_X];

        for (std::string i : pv_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, pv_category_hash,
                pv_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_visibility(&id, false);
            pv_auth_3d_ids.insert({ hash, id });
        }

        for (std::string i : stage_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, stage_category_hash,
                stage_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
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
                    auth_3d_data_set_camera_root_update(&light_auth_3d_id, false);
                    auth_3d_data_set_enable(&light_auth_3d_id, true);
                    auth_3d_data_set_req_frame(&light_auth_3d_id, 0.0f);
                    auth_3d_data_set_max_frame(&light_auth_3d_id, -1.0f);
                    auth_3d_data_set_paused(&light_auth_3d_id, false);
                    auth_3d_data_set_visibility(&light_auth_3d_id, true);
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
            auth_3d_data_set_repeat(&camera_auth_3d_id, false);
            auth_3d_data_set_enable(&camera_auth_3d_id, true);
            auth_3d_data_set_req_frame(&camera_auth_3d_id, 0.0f);
            auth_3d_data_set_max_frame(&camera_auth_3d_id, -1.0f);
            auth_3d_data_set_paused(&camera_auth_3d_id, false);
            auth_3d_data_set_visibility(&camera_auth_3d_id, true);
        }

        {
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
            io_open(&s, file_buf, "wb");
            io_write(&s, data, size);
            io_free(&s);
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
                rob_chr->data.adjust.step_data.step = anim_frame_speed;
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

        frame_float = 0.0;
        this->frame = 0;
        this->time = 0;
        while (dsc_data_ptr != dsc_data_ptr_end
            && x_pv_game_dsc_process(this, this->time))
            dsc_data_ptr++;

        while (stage_effects_ptr != stage_effects_ptr_end
            && x_pv_game_stage_effects_process(this, this->frame))
            stage_effects_ptr++;

        x_pv_game_stage_effect_ctrl(this);

        pause = true;
    }
    else if (state == 10) {
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

        frame_float += get_delta_frame();
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        while (dsc_data_ptr != dsc_data_ptr_end
            && x_pv_game_dsc_process(this, time))
            dsc_data_ptr++;

        while (stage_effects_ptr != stage_effects_ptr_end
            && x_pv_game_stage_effects_process(this, this->frame))
            stage_effects_ptr++;

        stage_effect.frame += diva_stage_frame_rate.GetDeltaFrame();
        x_pv_game_stage_effect_ctrl(this);

        if (!play || pv_end)
            state = 11;
    }
    else if (state == 11) {
        Glitter::glt_particle_manager.FreeScenes();
        SetDest();
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

void x_pv_game::Load(int32_t pv_id, int32_t stage_id) {
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
    motion_set_load_motion(aft_mot_db->get_motion_set_id(buf), 0, aft_mot_db);

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
    pv_category = std::string(buf);
    pv_category_hash = hash_string_murmurhash(&pv_category);

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
    stage_category = std::string(buf);
    stage_category_hash = hash_string_murmurhash(&stage_category);

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = std::string(buf);

    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    camera_category = std::string(buf);
    camera_category_hash = hash_string_murmurhash(&camera_category);

    pv_auth_3d_names.clear();
    for (pvpp_effect i : pp->effect)
        for (pvpp_a3da j : i.a3da) {
            if (pv_auth_3d_names.find(j.name) == pv_auth_3d_names.end())
                pv_auth_3d_names.insert(j.name);
        }

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
    for (pv_play_data& i : playdata)
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

    int32_t chara_index = 0;
    for (pvpp_chara& i : pp->chara) {
        if (i.motion.size()) {
            ::chara_index chr_idx = CHARA_MIKU;
            if (i.chara_effect.size())
                chr_idx = (::chara_index)i.chara_effect.front().base_chara;

            rob_chara_pv_data pv_data;
            int32_t chara_id = rob_chara_array_init_chara_index(chr_idx, &pv_data, 0, true);
            if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                rob_chara_ids[chara_index] = chara_id;
        }

        if (++chara_index >= ROB_CHARA_COUNT)
            break;
    }

    object_storage_load_set_hash(x_data, effpv_objset);
    object_storage_load_set_hash(x_data, stgpv_objset);
    object_storage_load_set_hash(x_data, stgpvhrc_objset);

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

    dsc_time = 0;
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    Glitter::glt_particle_manager.draw_all = false;
}

void x_pv_game::Unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    motion_set_unload_motion(aft_mot_db->get_motion_set_id(buf));

    pv_id = -1;
    stage_id = -1;
    delete pp;
    delete sr;
    pp = 0;
    sr = 0;

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

    for (std::pair<uint32_t, int32_t> i : pv_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    for (std::pair<uint32_t, int32_t> i : stage_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    auth_3d_data_unload_id(light_auth_3d_id, rctx_ptr);
    auth_3d_data_unload_id(camera_auth_3d_id, rctx_ptr);

    pv_auth_3d_ids.clear();
    stage_auth_3d_ids.clear();
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;

    auth_3d_data_unload_category(pv_category_hash);
    auth_3d_data_unload_category(stage_category_hash);
    auth_3d_data_unload_category(light_category.c_str());
    auth_3d_data_unload_category(camera_category_hash);

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

    object_storage_unload_set(effpv_objset);
    object_storage_unload_set(stgpv_objset);
    object_storage_unload_set(stgpvhrc_objset);

    effpv_objset = hash_murmurhash_empty;
    stgpv_objset = hash_murmurhash_empty;
    stgpvhrc_objset = hash_murmurhash_empty;

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

    dsc_time = 0;
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4u_identity;
    branch_mode = 0;

    Glitter::glt_particle_manager.draw_all = true;
}

x_pv_game_glitter::x_pv_game_glitter(const char* name) {
    this->name = std::string(name);
    hash = (uint32_t)Glitter::glt_particle_manager
        .LoadFile(Glitter::X, &data_list[DATA_X], name, 0, -1.0f, false, 0);
}

x_pv_game_glitter::~x_pv_game_glitter() {
    Glitter::glt_particle_manager.UnloadEffectGroup(hash);
}

struc_104::struc_104() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

struc_104::~struc_104() {

}

void struc_104::reset() {
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

pv_play_data::pv_play_data() : rob_chr(), disp() {

}

pv_play_data::~pv_play_data() {

}

void pv_play_data::reset() {
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

    pv_play_data* playdata = &a1->playdata[chara_id];
    int64_t dsc_time = a1->dsc_time;
    for (dsc_set_motion& i : playdata->field_38.set_motion) {
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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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
        rob_chara_set_osage_reset(rob_chr);
    } break;
    case DSC_X_MIKU_ROT: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t rot_y = (float_t)(int32_t)data[0] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + a1->scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chara_set_osage_reset(rob_chr);
    } break;
    case DSC_X_MIKU_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t disp = (int32_t)data[1];

        playdata->disp = disp == 1;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chara_set_visibility(rob_chr, true);
            /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index,
                rob_chr->module_index)) {
                sub_1405430F0(rob_chr->chara_id, 1);
                sub_1405430F0(rob_chr->chara_id, 2);
            }*/

            //pv_game::set_data_itmpv_visibility(a1->pv_game, a1->chara_id, true);
            for (pv_play_data_set_motion& i : playdata->set_motion) {
                bool v45 = rob_chara_set_motion_id(rob_chr, i.motion_id, i.frame,
                    i.duration, i.field_10, 0, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chara_set_motion_reset_data(rob_chr, i.motion_id, i.dsc_frame);
                rob_chr->bone_data->field_758 = i.field_18;
                rob_chr->data.adjust.step_data.step = i.frame_speed;
                //if (v45)
                //    pv_expression_array_set_motion(a1->chara_id, i.motion_id);
                //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                    sub_140122B60(a1, a1->chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else {
            rob_chara_set_visibility(rob_chr, false);
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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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
        pv_play_data_motion* v56 = 0;
        for (pv_play_data_motion& i : playdata->motion)
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
                motion_id = rob_cmn_mottbl_get_motion_id(rob_chr, 0);
        }
        else if (motion_id == -1) {
            motion_id = rob_cmn_mottbl_get_motion_id(rob_chr, 5);
            if (motion_id == -1)
                motion_id = rob_cmn_mottbl_get_motion_id(rob_chr, 0);
        }

        duration /= a1->anim_frame_speed;
        if (v11)
            duration = 0.0f;

        pv_play_data_set_motion v519;
        v519.frame_speed = frame_speed * a1->anim_frame_speed;
        v519.motion_id = motion_id;
        v519.frame = frame;
        v519.duration = duration;
        v519.field_10 = v11;
        v519.blend_type = MOTION_BLEND_CROSS;
        v519.field_18 = true;
        v519.motion_index = motion_index;
        v519.dsc_time = v56 ? v56->time : a1->dsc_time;
        v519.dsc_frame = dsc_frame;

        //a1->field_2C560[a1->chara_id] = true;
        //a1->field_2C568[a1->chara_id] = v519;
        if (playdata->disp) {
            bool v84 = rob_chara_set_motion_id(rob_chr, motion_id, frame,
                duration, v11, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chara_set_motion_reset_data(rob_chr, motion_id, dsc_frame);
            rob_chara_set_motion_skin_param(rob_chr, motion_id, dsc_frame);
            rob_chr->bone_data->field_758 = true;
            rob_chr->data.adjust.step_data.step = v519.frame_speed;
            //if (v84)
            //    pv_expression_array_set_motion(a1->chara_id, motion_id);
            //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                sub_140122B60(a1, a1->chara_id, motion_index, v56 ? v56->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(v519);
            rob_chr->data.adjust.step_data.step = v519.frame_speed;
        }
    } break;
    case DSC_X_SET_PLAYDATA: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t motion_index = (int32_t)data[1];
        if (motion_index < 0) {
            playdata->motion.clear();
            break;
        }

        pv_play_data_motion* v92 = 0;
        for (pv_play_data_motion& i : playdata->motion)
            if (i.motion_index == motion_index) {
                v92 = &i;
                break;
            }

        if (v92) {
            v92->enable = true;
            v92->motion_index = motion_index;
            v92->time = a1->dsc_time;
        }
        else {
            pv_play_data_motion v531;
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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_eyelid_mottbl_motion_from_face(rob_chr,
            v115, duration, -1.0f, offset, aft_mot_db);
    } break;
    case DSC_X_MOUTH_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_mouth_mottbl_motion(rob_chr, 0, mottbl_index,
            value, 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_X_HAND_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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
            rob_chara_set_hand_l_mottbl_motion(rob_chr, 0, mottbl_index,
                value, 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        case 1:
            rob_chara_set_hand_r_mottbl_motion(rob_chr, 0, mottbl_index,
                value, 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        }
    } break;
    case DSC_X_LOOK_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_eyes_mottbl_motion(rob_chr, 0, mottbl_index,
            value, mottbl_index == 224 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
        return 1;
    } break;
    case DSC_X_EXPRESSION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_face_mottbl_motion(rob_chr, 0, mottbl_index, value,
            mottbl_index >= 214 && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v168, aft_mot_db);

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

        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_face_mottbl_motion(rob_chr, 0, mottbl_index, 1.0f,
            mottbl_index >= 214 && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v237, aft_mot_db);

        /*if (!a1->has_perf_id) {
            int32_t mottbl_index = mouth_anim_id_to_mottbl_index(v234);

            float_t value = 1.0f;
            //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
            //    value = 0.0f;

            rob_chara_set_mouth_mottbl_motion(rob_chr, 0, mottbl_index,
                value, 0, a1->target_anim_fps * 0.1f, 0.0f, 1.0f, -1, offset, aft_mot_db);
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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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

        rob_chara_set_mouth_mottbl_motion(rob_chr, 0, mottbl_index,
            value, 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t value = (float_t)(int32_t)data[0] * 0.001f;

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

    } break;
    case DSC_X_SATURATE: {

    } break;
    case DSC_X_FADE_MODE: {

    } break;
    case DSC_X_AUTO_BLINK: {

    } break;
    case DSC_X_PARTS_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        item_id id = (item_id)data[1];
        int32_t disp = (int32_t)data[2];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chara_set_parts_disp(rob_chr, id, disp == 1);
    } break;
    case DSC_X_TARGET_FLYING_TIME: {

    } break;
    case DSC_X_CHARA_SIZE: {

    } break;
    case DSC_X_CHARA_HEIGHT_ADJUST: {

    } break;
    case DSC_X_ITEM_ANIM: {

    } break;
    case DSC_X_CHARA_POS_ADJUST: {

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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

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
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_step_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chara_set_osage_step(rob_chr, osage_step_outer);
    } break;
    case DSC_X_OSAGE_MV_CCL: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_mv_ccl_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chara_set_osage_move_cancel(rob_chr, 1, osage_mv_ccl_kami);
        rob_chara_set_osage_move_cancel(rob_chr, 2, osage_mv_ccl_outer);
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
                Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash, true);
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
                Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash, true);
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

        uint32_t hash = a1->pv_glitter->hash;

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
        auth_3d_data_set_req_frame(&id, 0.0f);
        auth_3d_data_set_max_frame(&id, -1.0f);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_visibility(&id, true);
        auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
    }

    for (pvsr_glitter& i : stage_effect->glitter) {
        uint32_t effect_hash = hash_string_murmurhash(&i.name);
        Glitter::glt_particle_manager.LoadScene(hash_murmurhash_empty, effect_hash, true);
        Glitter::glt_particle_manager.SetSceneName(hash, xpvgm->stage_glitter->name.c_str());
        Glitter::glt_particle_manager.SetSceneEffectName(
            hash_murmurhash_empty, effect_hash, i.name.c_str());
        Glitter::glt_particle_manager.SetSceneFrameRate(hash, &diva_stage_frame_rate);
    }
}

static void x_pv_game_stage_effect_stop(x_pv_game* xpvgm,
    pvsr_stage_effect* stage_effect, bool stop_a3da, bool stop_glitter) {
        for (pvsr_a3da& i : stage_effect->a3da) {
            int32_t& id = xpvgm->stage_auth_3d_ids[i.hash];
            auth_3d_data_set_visibility(&id, false);
            auth_3d_data_set_enable(&id, false);
        }

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
            dsc_set_item v14;
            v14.item_index = data[1];
            v14.time = time;
            v14.pv_branch_mode = pv_branch_mode;
            a1->playdata[chara_id].field_38.set_item.push_back(v14);
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
            dsc_set_motion v14;
            v14.motion_index = data[1];
            v14.time = time;
            v14.pv_branch_mode = pv_branch_mode;
            a1->playdata[chara_id].field_38.set_motion.push_back(v14);
        }
        prev_time = time;
        i++;
    }
}
#endif
