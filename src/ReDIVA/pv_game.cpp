/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(ReDIVA_DEV)
#include <set>
#include "pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_param.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/sort.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"

enum dsc_ft_func {
    DSC_FT_END,
    DSC_FT_TIME,
    DSC_FT_MIKU_MOVE,
    DSC_FT_MIKU_ROT,
    DSC_FT_MIKU_DISP,
    DSC_FT_MIKU_SHADOW,
    DSC_FT_TARGET,
    DSC_FT_SET_MOTION,
    DSC_FT_SET_PLAYDATA,
    DSC_FT_EFFECT,
    DSC_FT_FADEIN_FIELD,
    DSC_FT_EFFECT_OFF,
    DSC_FT_SET_CAMERA,
    DSC_FT_DATA_CAMERA,
    DSC_FT_CHANGE_FIELD,
    DSC_FT_HIDE_FIELD,
    DSC_FT_MOVE_FIELD,
    DSC_FT_FADEOUT_FIELD,
    DSC_FT_EYE_ANIM,
    DSC_FT_MOUTH_ANIM,
    DSC_FT_HAND_ANIM,
    DSC_FT_LOOK_ANIM,
    DSC_FT_EXPRESSION,
    DSC_FT_LOOK_CAMERA,
    DSC_FT_LYRIC,
    DSC_FT_MUSIC_PLAY,
    DSC_FT_MODE_SELECT,
    DSC_FT_EDIT_MOTION,
    DSC_FT_BAR_TIME_SET,
    DSC_FT_SHADOWHEIGHT,
    DSC_FT_EDIT_FACE,
    DSC_FT_MOVE_CAMERA,
    DSC_FT_PV_END,
    DSC_FT_SHADOWPOS,
    DSC_FT_EDIT_LYRIC,
    DSC_FT_EDIT_TARGET,
    DSC_FT_EDIT_MOUTH,
    DSC_FT_SET_CHARA,
    DSC_FT_EDIT_MOVE,
    DSC_FT_EDIT_SHADOW,
    DSC_FT_EDIT_EYELID,
    DSC_FT_EDIT_EYE,
    DSC_FT_EDIT_ITEM,
    DSC_FT_EDIT_EFFECT,
    DSC_FT_EDIT_DISP,
    DSC_FT_EDIT_HAND_ANIM,
    DSC_FT_AIM,
    DSC_FT_HAND_ITEM,
    DSC_FT_EDIT_BLUSH,
    DSC_FT_NEAR_CLIP,
    DSC_FT_CLOTH_WET,
    DSC_FT_LIGHT_ROT,
    DSC_FT_SCENE_FADE,
    DSC_FT_TONE_TRANS,
    DSC_FT_SATURATE,
    DSC_FT_FADE_MODE,
    DSC_FT_AUTO_BLINK,
    DSC_FT_PARTS_DISP,
    DSC_FT_TARGET_FLYING_TIME,
    DSC_FT_CHARA_SIZE,
    DSC_FT_CHARA_HEIGHT_ADJUST,
    DSC_FT_ITEM_ANIM,
    DSC_FT_CHARA_POS_ADJUST,
    DSC_FT_SCENE_ROT,
    DSC_FT_EDIT_MOT_SMOOTH_LEN,
    DSC_FT_PV_BRANCH_MODE,
    DSC_FT_DATA_CAMERA_START,
    DSC_FT_MOVIE_PLAY,
    DSC_FT_MOVIE_DISP,
    DSC_FT_WIND,
    DSC_FT_OSAGE_STEP,
    DSC_FT_OSAGE_MV_CCL,
    DSC_FT_CHARA_COLOR,
    DSC_FT_SE_EFFECT,
    DSC_FT_EDIT_MOVE_XYZ,
    DSC_FT_EDIT_EYELID_ANIM,
    DSC_FT_EDIT_INSTRUMENT_ITEM,
    DSC_FT_EDIT_MOTION_LOOP,
    DSC_FT_EDIT_EXPRESSION,
    DSC_FT_EDIT_EYE_ANIM,
    DSC_FT_EDIT_MOUTH_ANIM,
    DSC_FT_EDIT_CAMERA,
    DSC_FT_EDIT_MODE_SELECT,
    DSC_FT_PV_END_FADEOUT,
    DSC_FT_TARGET_FLAG,
    DSC_FT_ITEM_ANIM_ATTACH,
    DSC_FT_SHADOW_RANGE,
    DSC_FT_HAND_SCALE,
    DSC_FT_LIGHT_POS,
    DSC_FT_FACE_TYPE,
    DSC_FT_SHADOW_CAST,
    DSC_FT_EDIT_MOTION_F,
    DSC_FT_FOG,
    DSC_FT_BLOOM,
    DSC_FT_COLOR_COLLE,
    DSC_FT_DOF,
    DSC_FT_CHARA_ALPHA,
    DSC_FT_AOTO_CAP,
    DSC_FT_MAN_CAP,
    DSC_FT_TOON,
    DSC_FT_SHIMMER,
    DSC_FT_ITEM_ALPHA,
    DSC_FT_MOVIE_CUT_CHG,
    DSC_FT_CHARA_LIGHT,
    DSC_FT_STAGE_LIGHT,
    DSC_FT_AGEAGE_CTRL,
    DSC_FT_MAX,
};

pv_game_old pv_game_old_data;
TaskPvGame task_pv_game;

extern render_context* rctx_ptr;

static float_t dsc_time_to_frame(int64_t time);
static void pv_game_change_field(pv_game_old* pvgm, int32_t field, int64_t dsc_time, int64_t curr_time);
static bool pv_game_dsc_process(pv_game_old* pvgm, int64_t curr_time);
static void pv_game_reset_field(pv_game_old* pvgm);

static dsc_data* pv_game_dsc_data_find_func(pv_game_old* pvgm, dsc_ft_func func_name,
    int32_t* time, int32_t* pv_branch_mode, dsc_data* start, dsc_data* end);
static void pv_game_dsc_data_find_playdata_item_anim(pv_game_old* pvgm, int32_t chara_id);
static void pv_game_dsc_data_find_playdata_set_motion(pv_game_old* pvgm, int32_t chara_id);

pv_play_data_motion_data::pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

pv_play_data_motion_data::~pv_play_data_motion_data() {

}

void pv_play_data_motion_data::reset() {
    rob_chr = 0;
    current_time = 0.0f;
    duration = 0.0f;
    start_pos = vec3_null;
    end_pos = vec3_null;
    start_rot = 0.0f;
    end_rot = 0.0f;
    mot_smooth_len = 12.0f;
    set_motion.clear();
    set_motion.shrink_to_fit();
    set_item.clear();
    set_item.shrink_to_fit();
}

pv_play_data::pv_play_data() : rob_chr(), disp(), field_31(), field_34() {

}

pv_play_data::~pv_play_data() {

}

void pv_play_data::reset() {
    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

pv_disp2d::pv_disp2d() : pv_id(), title_start_2d_field(), title_end_2d_field(),
title_start_2d_low_field(), title_end_2d_low_field(), title_start_3d_field(), title_end_3d_field(),
target_shadow_type(), pv_spr_set_id(), pv_aet_set_id(), pv_main_aet_id() {

}

pv_disp2d::~pv_disp2d() {

}

pv_game_dsc_data::pv_game_dsc_data() : field_0(), state(), play(), dsc_buffer(), dsc_buffer_counter(),
field_2BF30(), field_2BF38(), curr_time(), curr_time_float(), prev_time_float(), field_2BF50(), field_2BF58(),
field_2BF60(), field_2BF64(), field_2BF68(), chara_id(), pv_game()/*, field_2BF80(), field_2BF88()*/,
dsc_file_handler(), target_anim_fps(), anim_frame_speed(), target_flying_time(), time_signature(), dsc_time(),
field_2BFB0(), field_2BFB4(), field_2BFB8(), field_2BFBC(), field_2BFC0(), music_play(), field_2BFC4(), pv_end(),
fov(), min_dist(), field_2BFD4(), field_2BFD5(), data_camera(), change_field_branch_time(), scene_fade_enable(),
scene_fade_frame(), scene_fade_duration(), scene_fade_start_alpha(), scene_fade_end_alpha(), scene_fade_color(),
has_signature(), has_perf_id(), field_2C4E4(), field_2C500(), scene_rot_y(), scene_rot_mat(), field_2C54C(),
field_2C550(), branch_mode(), first_challenge_note(), last_challenge_note(), field_2C560() {

}

pv_game_dsc_data::~pv_game_dsc_data() {

}

pv_game_data_chara::pv_game_data_chara() : chara_index(), cos(), chara_id(), pv_data(), rob_chr() {

}

pv_game_data_chara::~pv_game_data_chara() {

}

pv_game_data_field::pv_game_data_field() : stage_index(), light_auth_3d_uid(), light_auth_3d_id(),
light_frame(), spr_set_back_id(), stage_flag(), npr_type(), cam_blur(), sdw_off() {

}

pv_game_data_field::~pv_game_data_field() {

}

struc_78::struc_78() : field_30(), field_34() {

}

struc_78::~struc_78() {

}

pv_game_data_chreff::pv_game_data_chreff() : src_chara_index(),
dst_chara_index(), src_auth_3d_uid(), dst_auth_3d_uid() {

}

pv_game_data_chreff::~pv_game_data_chreff() {

}

pv_effect_resource::pv_effect_resource() : emission() {

}

pv_effect_resource::~pv_effect_resource() {

}

pv_game_data::pv_game_data() : field_2CE98(), field_2CE9C(), field_2CF1C(), field_2CF20(), field_2CF24(),
field_2CF28(), field_2CF2C(), field_2CF30(), field_2CF34(), field_2CF38(), life_gauge(), score_final(),
challenge_time_total_bonus(), combo(), challenge_time_combo_count(), max_combo(), total_hit_count(), hit_count(),
current_reference_score(), target_count(), field_2CF84(), field_2CF88(), field_2CF8C(), score_slide_chain_bonus(),
slide_chain_length(), field_2CF98(), field_2CF9C(), field_2CFA0(), field_2CFA4(), pv(), field_2CFB0(), field_2CFB4(),
reference_score(), field_2CFBC(), field_2CFC0(), field_2CFC4(), field_2CFE0(), field_2CFE4(), notes_passed(),
field_2CFF0(), score_percentage(), score_percentage_clear(), score_percentage_border(), life_gauge_safety_time(),
life_gauge_border(), field_2D008(), field_2D00C(), no_fail(), field_2D00E(), field_2D00F(), challenge_time_start(),
challenge_time_end(), max_time(), max_time_float(), current_time_float(), current_time(), current_time_float_dup(),
field_2D03C(), score_hold_multi(), score_hold(), score_slide(), field_2D04C(), chance_time_ended(), pv_disp2d(),
life_gauge_final(), hit_border(), field_2D05D(), field_2D05E(), field_2D05F(), title_image_init(), field_2D061(),
field_2D062(), field_2D063(), has_auth_3d_frame(), has_light_frame(), field_2D066(), field_2D068(), field_2D069(),
field_2D06A(), field_2D06C(), field_index(), edit_effect_index(), slidertouch_counter(),
change_field_branch_success_counter(), field_2D080(), field_2D084(), life_gauge_bonus(), life_gauge_total_bonus(),
field_2D090(), field_2D091(), field_2D092(), field_2D093(), field_2D094(), field_2D095(), field_2D096(), success(),
field_2D098(), field_2D09C(), field_2D0A0(), pv_end_fadeout(), rival_percentage(), field_2D0A8(), field_2D0AC(),
field_2D0B0(), field_2D0BC(), field_2D0BD(), field_2D0BE(), field_2D0BF(), field_2D0C0(), field_2D0C4(),
field_2D7E8(), field_2D7EC(), field_2D808(), field_2D80C(), field_2D858(), camera_auth_3d_uid(), field_2D874(),
field_2D878(), field_2D87C(), current_field(), field_2D8A0(), field_2D8A4(), data_camera_id(),
field_2D954(), field_2DAC8(), field_2DACC(), field_2DB28(), field_2DB2C(), field_2DB34() {

}

pv_game_data::~pv_game_data() {

}

pv_game::pv_game() : loaded(), field_1(), field_2(), end_pv(), field_4(),
state(), field_C(), pv_id(), field_14(), modules(), items(), field_90() {

}

pv_game::~pv_game() {

}

void pv_game_init_data::reset() {
    pv_id = 0;
    difficulty = 0;
    field_C = 0;
    score_percentage_clear = 0;
    life_gauge_safety_time = 0;
    life_gauge_border = 0;
    field_18 = 0;
    for (int32_t& i : modules)
        i = 0;
    for (rob_chara_pv_data_item& i : items)
        i = {};
    for (struc_653& i : field_94)
        i = {};
}

TaskPvGame::InitData::InitData() : data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), field_197(), field_198(), field_19C() {
    Reset();
}

TaskPvGame::InitData::~InitData() {

}

void TaskPvGame::InitData::Clear() {
    field_B0.clear();
    field_D0.clear();
    field_F0.clear();
    field_110.clear();
    field_130.clear();
    field_150.clear();
    field_170.clear();
}

void TaskPvGame::InitData::Reset() {
    data.reset();
    Clear();
    field_190 = false;
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    field_197 = false;
    field_198 = false;
    field_19C = 0;
}

TaskPvGame::Data::Data() : field_0(), data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), field_198() {
    Reset();
}

TaskPvGame::Data::~Data() {

}

void TaskPvGame::Data::Clear() {
    field_B0.clear();
    field_D0.clear();
    field_F0.clear();
    field_110.clear();
    field_130.clear();
    field_150.clear();
    field_170.clear();
}

void TaskPvGame::Data::Reset() {
    field_0 = 1;
    data.reset();
    Clear();
    field_190 = false;
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    field_198 = 0;
}

TaskPvGame::TaskPvGame() {

}

TaskPvGame::~TaskPvGame() {

}

bool TaskPvGame::Init() {
    return true;
}

bool TaskPvGame::Ctrl() {
    return true;
    //return false;
}

bool TaskPvGame::Dest() {
    return true;
}

void TaskPvGame::Disp() {

}

pv_game_old::pv_game_old() : state(), frame(), frame_float(), time(), rob_chara_ids(),
dsc_time(), dsc_data_ptr(), dsc_data_ptr_end(), play(), success(), chara_id(),
pv_end(), playdata(), scene_rot_y(), branch_mode(), pause(), step_frame() {
    pv_id = -1;
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;
}

pv_game_old::~pv_game_old() {

}

bool pv_game_old::Init() {
    task_stage_load("PV_STAGE");
    return true;
}

bool pv_game_old::Ctrl() {
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
        data_struct* aft_data = &data_list[DATA_AFT];

        light_param_data_storage_data_set_pv_id(pv_id);

        for (std::string& i : category_load)
            auth_3d_data_load_category(i.c_str());

        state = 3;
    } break;
    case 3: {
        bool wait_load = task_stage_check_not_loaded();

        for (uint32_t& i : objset_load)
            if (object_storage_get_obj_set_handler(i)
                && object_storage_load_obj_set_check_not_read(i))
                wait_load = true;

        for (std::string& i : category_load)
            if (!auth_3d_data_check_category_loaded(i.c_str()))
                wait_load = true;

        if (!wait_load)
            state = 6;
    } break;
    case 6: {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* auth_3d_db = &aft_data->data_ft.auth_3d_db;

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

        light_auth_3d_id = -1;
        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000", pv_id);
            uint32_t light_auth_3d_uid = auth_3d_db->get_uid(buf);
            if (light_auth_3d_uid != -1) {
                light_auth_3d_id = auth_3d_data_load_uid(light_auth_3d_uid, auth_3d_db);
                auth_3d_data_read_file(&light_auth_3d_id, auth_3d_db);
                auth_3d_data_set_enable(&light_auth_3d_id, false);
                auth_3d_data_set_visibility(&light_auth_3d_id, false);
                break;
            }
        }

        camera_auth_3d_id = -1;
        {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
            uint32_t camera_auth_3d_uid = auth_3d_db->get_uid(buf);
            camera_auth_3d_id = auth_3d_data_load_uid(camera_auth_3d_uid, auth_3d_db);
            auth_3d_data_read_file_modern(&camera_auth_3d_id);
            auth_3d_data_set_enable(&camera_auth_3d_id, false);
            auth_3d_data_set_visibility(&camera_auth_3d_id, false);
        }

        state = 7;
    } break;
    case 7: {
        bool wait_load = false;

        for (auto i : pv_auth_3d_ids)
            if (!auth_3d_data_check_id_loaded(&i.second))
                wait_load = true;

        if (!auth_3d_data_check_id_loaded(&light_auth_3d_id))
            wait_load = true;

        if (!auth_3d_data_check_id_loaded(&camera_auth_3d_id))
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
            data_struct* aft_data = &data_list[DATA_AFT];

            dsc dsc_mouth;
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "root+/pv_script/pv%03d/", pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
            aft_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

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
            aft_data->load_file(&dsc_easy, path_buf, file_buf, dsc::load_file);

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
            pv_game_dsc_data_find_playdata_set_motion(this, i);
            pv_game_dsc_data_find_playdata_item_anim(this, i);
        }

        dsc_data_ptr = dsc_m.data.data();
        dsc_data_ptr_end = dsc_data_ptr + dsc_m.data.size();

        state = 10;

        pv_game_change_field(this, 1, -1, -1);

        Glitter::glt_particle_manager->SetPause(false);
        extern float_t frame_speed;
        frame_speed = 1.0f;

        pause = false;
        step_frame = false;

        frame_float = -1.0f;
        this->frame = -1;
        this->time = (int64_t)round(-(100000.0 / 60.0) * 10000.0);
        while (dsc_data_ptr != dsc_data_ptr_end
            && pv_game_dsc_process(this, this->time))
            dsc_data_ptr++;

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

        pause = true;
    } break;
    case 10: {
        float_t delta_frame = get_delta_frame();
        frame_float += delta_frame;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        while (dsc_data_ptr != dsc_data_ptr_end
            && pv_game_dsc_process(this, time))
            dsc_data_ptr++;

        if (!play || pv_end)
            state = 11;
    } break;
    case 11: {
        Glitter::glt_particle_manager->FreeScenes();
        SetDest();
    } break;
    }
    return false;
}

bool pv_game_old::Dest() {
    Unload();
    task_stage_unload();

    light_param_data_storage_data_reset();
    rctx_ptr->post_process.tone_map->set_saturate_coeff(1.0f);
    rctx_ptr->post_process.tone_map->set_scene_fade(vec4_null);
    rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(0);
    rctx_ptr->post_process.dof->data.pv.enable = false;
    rctx_ptr->object_data.object_culling = true;
    rctx_ptr->draw_pass.shadow_ptr->range = 1.0f;

    Glitter::glt_particle_manager->SetPause(false);
    extern float_t frame_speed;
    frame_speed = 1.0f;
    return true;
}

void pv_game_old::Disp() {
    if (state != 10)
        return;

}

void pv_game_old::Basic() {
    if (state != 10)
        return;

    if (step_frame)
        if (pause)
            pause = false;
        else {
            pause = true;
            step_frame = false;
        }

    Glitter::glt_particle_manager->SetPause(pause);
    extern float_t frame_speed;
    frame_speed = pause ? 0.0f : 1.0f;
}

void pv_game_old::Window() {
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

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed(pause || (!pause && step_frame) ? "Play (K)" : "Pause (K)", { w, 0.0f }))
            pause ^= true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Step Frame (L)", { w, 0.0f })) {
            pause = true;
            step_frame = true;
        }

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Stop (Ctrl+K)", { w, 0.0f }))
            state = 11;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "%d", frame);
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::ButtonExEnterKeyPressed(buf, { w, 0.0f }, ImGuiButtonFlags_DontClosePopups
            | ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoHoveredOnFocus);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();
    }

    extern bool input_locked;
    input_locked |= ImGui::IsWindowFocused();
    ImGui::End();
}

void pv_game_old::Load(int32_t pv_id, chara_index charas[6], int32_t modules[6]) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    this->pv_id = pv_id;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_load_motion(mot_set, 0, aft_mot_db);
    motion_set_load_mothead(mot_set, 0, aft_mot_db);

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", pv_id);
    stage_category = buf;
    category_load.push_back(stage_category);


    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    camera_category = buf;
    category_load.push_back(camera_category);

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;
    branch_mode = 0;

    pause = false;
    step_frame = false;

    for (int32_t& i : rob_chara_ids)
        i = -1;

    for (uint32_t& i : objset_load)
        object_storage_load_set(aft_data, aft_obj_db, i);

    state = 1;
    this->frame = 0;
    frame_float = 0.0;
    time = 0;

    dsc_m.type = DSC_NONE;
    dsc_m.signature = 0;
    dsc_m.id = 0;
    dsc_m.data.clear();
    dsc_m.data.shrink_to_fit();
    dsc_m.data_buffer.clear();
    dsc_m.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    dsc_time = 0;
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

    Glitter::glt_particle_manager->draw_all = false;
}

void pv_game_old::Unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_unload_motion(mot_set);
    motion_set_unload_mothead(mot_set);

    pv_id = -1;

    state = 0;
    frame = 0;
    frame_float = 0.0;
    time = 0;

    for (int32_t& i : rob_chara_ids)
        if (i != -1) {
            rob_chara_array_free_chara_id(i);
            i = -1;
        }

    for (auto& i : pv_auth_3d_ids)
        auth_3d_data_unload_id(i.second, rctx_ptr);

    auth_3d_data_unload_id(light_auth_3d_id, rctx_ptr);
    auth_3d_data_unload_id(camera_auth_3d_id, rctx_ptr);

    pv_auth_3d_ids.clear();
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;

    for (std::string& i : category_load)
        auth_3d_data_unload_category(i.c_str());

    category_load.clear();
    category_load.shrink_to_fit();

    pv_category.clear();
    pv_category.shrink_to_fit();
    stage_category.clear();
    stage_category.shrink_to_fit();
    camera_category.clear();
    camera_category.shrink_to_fit();

    for (uint32_t& i : objset_load)
        object_storage_unload_set(i);

    objset_load.clear();
    objset_load.shrink_to_fit();

    dsc_m.type = DSC_NONE;
    dsc_m.signature = 0;
    dsc_m.id = 0;
    dsc_m.data.clear();
    dsc_m.data.shrink_to_fit();
    dsc_m.data_buffer.clear();
    dsc_m.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;

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
    scene_rot_mat = mat4_identity;
    branch_mode = 0;

    Glitter::glt_particle_manager->draw_all = true;
    pv_param_task::post_process_task.SetDest();
}

bool task_pv_game_append_task(TaskPvGame::InitData* init_data) {
    task_pv_game.data.Reset();

    task_pv_game.data.field_0 = 1;
    task_pv_game.data.data = init_data->data;
    task_pv_game.data.field_B0 = init_data->field_B0;
    task_pv_game.data.field_D0 = init_data->field_D0;
    task_pv_game.data.field_F0 = init_data->field_F0;
    task_pv_game.data.field_110 = init_data->field_110;
    task_pv_game.data.field_130 = init_data->field_130;
    task_pv_game.data.field_150 = init_data->field_150;
    task_pv_game.data.field_170 = init_data->field_170;
    task_pv_game.data.field_190 = init_data->field_190;
    task_pv_game.data.field_191 = !init_data->field_195;
    task_pv_game.data.no_fail = init_data->no_fail;
    if (init_data->field_191) {
        task_pv_game.data.field_0 = 2;
        task_pv_game.data.field_193 = init_data->field_193;
    }
    else
        task_pv_game.data.field_193 = 1;
    task_pv_game.data.field_194 = init_data->field_195;
    task_pv_game.data.field_195 = init_data->field_196;
    task_pv_game.data.field_196 = init_data->field_197;
    task_pv_game.data.field_198 = init_data->field_19C;

    if (init_data->field_198)
        return app::TaskWork::AppendTask(&task_pv_game, "PVGAME");
    else
        return app::TaskWork::AppendTask(&task_pv_game, 0, "PVGAME");
}

bool task_pv_game_check_task_ready() {
    return app::TaskWork::CheckTaskReady(&task_pv_game);
}

bool task_pv_game_free_task() {
    if (!app::TaskWork::CheckTaskReady(&task_pv_game))
        return true;

    task_pv_game.SetDest();
    return false;
}

static float_t dsc_time_to_frame(int64_t time) {
    return (float_t)time / 1000000000.0f * 60.0f;
}

static void sub_140122B60(pv_game_old* a1, int32_t chara_id, int32_t motion_index, int64_t disp_time) {
    if (chara_id < 0 || chara_id > ROB_CHARA_COUNT || motion_index < 0)
        return;

    pv_play_data* playdata = &a1->playdata[chara_id];
    int64_t dsc_time = a1->dsc_time;
    for (dsc_set_motion& i : playdata->motion_data.set_motion) {
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

static void pv_game_change_field(pv_game_old* pvgm, int32_t field, int64_t dsc_time, int64_t curr_time) {
    light_param_data_storage_data_set_pv_cut(field);
}

static bool pv_game_dsc_process(pv_game_old* a1, int64_t curr_time) {
    dsc_ft_func func = (dsc_ft_func)a1->dsc_data_ptr->func;
    uint32_t* data = a1->dsc_m.get_func_data(a1->dsc_data_ptr);
    if (a1->branch_mode) {
        bool v19;
        if (a1->success)
            v19 = a1->branch_mode == 2;
        else
            v19 = a1->branch_mode == 1;
        if (!v19) {
            if (func < 0 || func > DSC_FT_AGEAGE_CTRL) {
                a1->play = false;
                return false;
            }
            else if ((data[0] < 0 || data[0] > 1)
                && func != DSC_FT_PV_END && func != DSC_FT_PV_BRANCH_MODE)
                return true;
        }
    }



    switch (func) {
    case DSC_FT_END: {
        a1->play = false;
        a1->state = 11;
        return false;
    } break;
    case DSC_FT_TIME: {
        a1->dsc_time = (int64_t)(int32_t)data[0] * 10000;
        if (a1->dsc_time > curr_time)
            return false;
    } break;
    case DSC_FT_MIKU_MOVE: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 trans;
        trans.x = (float_t)(int32_t)data[1] * 0.001f;
        trans.y = (float_t)(int32_t)data[2] * 0.001f;
        trans.z = (float_t)(int32_t)data[3] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4_mult_vec3_trans(&a1->scene_rot_mat,
            &trans, &rob_chr->data.miku_rot.position);
        rob_chr->set_osage_reset();
    } break;
    case DSC_FT_MIKU_ROT: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t rot_y = (float_t)(int32_t)data[0] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + a1->scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chr->set_osage_reset();
    } break;
    case DSC_FT_MIKU_DISP: {
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
            rob_chr->set_visibility(true);
            /*if (rob_chara::check_for_ageageagain_module(chara_index, module_index)) {
                sub_1405430F0(chara_id, 1);
                sub_1405430F0(chara_id, 2);
            }*/

            //pv_game_old::set_data_itmpv_visibility(a1->pv_game_old, a1->chara_id, true);
            for (pv_play_data_set_motion& i : playdata->set_motion) {
                bool v45 = rob_chr->set_motion_id(i.motion_id, i.frame,
                    i.duration, i.field_10, 0, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->disable_eye_motion = i.disable_eye_motion;
                rob_chr->data.motion.step_data.step = i.frame_speed;
                //if (v45)
                //    pv_expression_array_set_motion(a1->chara_id, i.motion_id);
                //if (!a1->pv_game_old->data.pv->disable_calc_motfrm_limit)
                sub_140122B60(a1, a1->chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else {
            rob_chr->set_visibility(false);
            //pv_game_old::set_data_itmpv_visibility(a1->pv_game_old, a1->chara_id, false);
        }
    } break;
    case DSC_FT_MIKU_SHADOW: {
        a1->chara_id = data[0];
    } break;
    case DSC_FT_TARGET: {

    } break;
    case DSC_FT_SET_MOTION: {
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
        if (duration_int != -1) {
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

        pv_play_data_set_motion v519;
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
            //if (!a1->pv_game_old->data.pv->disable_calc_motfrm_limit)
                sub_140122B60(a1, a1->chara_id, motion_index, v56 ? v56->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(v519);
            rob_chr->data.motion.step_data.step = v519.frame_speed;
        }
    } break;
    case DSC_FT_SET_PLAYDATA: {
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
    case DSC_FT_EFFECT: {

    } break;
    case DSC_FT_FADEIN_FIELD: {

    } break;
    case DSC_FT_EFFECT_OFF: {

    } break;
    case DSC_FT_SET_CAMERA: {

    } break;
    case DSC_FT_DATA_CAMERA: {

    } break;
    case DSC_FT_CHANGE_FIELD: {
        int32_t field = (int32_t)data[0];
        if (field > 0)
            pv_game_change_field(a1, field, a1->dsc_time, curr_time);
        else
            pv_game_reset_field(a1);
    } break;
    case DSC_FT_HIDE_FIELD: {

    } break;
    case DSC_FT_MOVE_FIELD: {

    } break;
    case DSC_FT_FADEOUT_FIELD: {

    } break;
    case DSC_FT_EYE_ANIM: {
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
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyelid_mottbl_motion_from_face(v115, duration, -1.0f, offset, aft_mot_db);
    } break;
    case DSC_FT_MOUTH_ANIM: {
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
        case 10: mouth_anim_id = 4; break;
        case 11: mouth_anim_id = 5; break;
        case 12: mouth_anim_id = 6; break;
        case 13: mouth_anim_id = 7; break;
        case 14: mouth_anim_id = 8; break;
        case 15: mouth_anim_id = 9; break;
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

        //if (a1->pv_game_old->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_FT_HAND_ANIM: {
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
        //if (a1->pv_game_old->data.pv->is_old_pv)
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
    case DSC_FT_LOOK_ANIM: {
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
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value,
            mottbl_index == 224 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
        return 1;
    } break;
    case DSC_FT_EXPRESSION: {
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
        //if (a1->has_perf_id && (a1->pv_game_old->data.pv->edit - 1) <= 1)
        //    v168 = false;

        float_t offset = 0.0f;
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v168, aft_mot_db);
    } break;
    case DSC_FT_LOOK_CAMERA: {

    } break;
    case DSC_FT_LYRIC: {

    } break;
    case DSC_FT_MUSIC_PLAY: {

    } break;
    case DSC_FT_MODE_SELECT: {

    } break;
    case DSC_FT_EDIT_MOTION: {

    } break;
    case DSC_FT_BAR_TIME_SET: {
        int32_t bpm = (int32_t)data[0];
    } break;
    case DSC_FT_SHADOWHEIGHT: {
        a1->chara_id = data[0];
    } break;
    case DSC_FT_EDIT_FACE:
    case DSC_FT_EDIT_EXPRESSION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t expression_id = (int32_t)data[0];

        float_t duration = 0.0f;
        if (func == DSC_FT_EDIT_EXPRESSION)
            duration = (float_t)(int32_t)data[1] * 0.001f * 60.0f;

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
        //    v237 = (a1->pv_game_old->data.pv->edit - 1) > 1;

        float_t offset = 0.0f;
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v237, aft_mot_db);

        /*if (!a1->has_perf_id) {
            int32_t mottbl_index = mouth_anim_id_to_mottbl_index(v234);

            float_t value = 1.0f;
            //if (a1->pv_game_old->data.field_2D090 && mottbl_index != 144)
            //    value = 0.0f;

            rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0,
            a1->target_anim_fps * 0.1f, 0.0f, 1.0f, -1, offset, aft_mot_db);
        }*/
    } break;
    case DSC_FT_MOVE_CAMERA: {

    } break;
    case DSC_FT_PV_END: {
        a1->pv_end = true;
    } break;
    case DSC_FT_SHADOWPOS: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_FT_EDIT_LYRIC: {

    } break;
    case DSC_FT_EDIT_TARGET: {

    } break;
    case DSC_FT_EDIT_MOUTH: {
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
        //if (a1->pv_game_old->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game_old->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_FT_SET_CHARA: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_FT_EDIT_MOVE: {

    } break;
    case DSC_FT_EDIT_SHADOW: {

    } break;
    case DSC_FT_EDIT_EYELID: {

    } break;
    case DSC_FT_EDIT_EYE: {

    } break;
    case DSC_FT_EDIT_ITEM: {

    } break;
    case DSC_FT_EDIT_EFFECT: {

    } break;
    case DSC_FT_EDIT_DISP: {

    } break;
    case DSC_FT_EDIT_HAND_ANIM: {

    } break;
    case DSC_FT_AIM: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_FT_HAND_ITEM: {

    } break;
    case DSC_FT_EDIT_BLUSH: {

    } break;
    case DSC_FT_NEAR_CLIP: {

    } break;
    case DSC_FT_CLOTH_WET: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t value = (float_t)(int32_t)data[1] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->item_equip->wet = clamp_def(value, 0.0f, 1.0f);
    } break;
    case DSC_FT_LIGHT_ROT: {

    } break;
    case DSC_FT_SCENE_FADE: {

    } break;
    case DSC_FT_TONE_TRANS: {
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
    case DSC_FT_SATURATE: {
        float_t value = (float_t)(int32_t)data[0] * 0.001f;
        rctx_ptr->post_process.tone_map->set_saturate_coeff(value);
    } break;
    case DSC_FT_FADE_MODE: {
        int32_t value = data[0];
        rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(value);
    } break;
    case DSC_FT_AUTO_BLINK: {

    } break;
    case DSC_FT_PARTS_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        item_id id = (item_id)data[1];
        int32_t disp = (int32_t)data[2];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_parts_disp(id, disp == 1);
    } break;
    case DSC_FT_TARGET_FLYING_TIME: {

    } break;
    case DSC_FT_CHARA_SIZE: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t chara_size = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            return 1;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1) {
            chara_index chara_index = CHARA_MIKU;
            //if (a1->chara_id < a1->pp->chara.size())
            //    chara_index = (::chara_index)a1->pp->chara[a1->chara_id].chara_effect.base_chara;

            //chara_index chara_index = pv_db_pv::get_performer_chara(a1->pv_game_old->data.pv, a1->chara_id);
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
    case DSC_FT_CHARA_HEIGHT_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t height_adjust = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_FT_ITEM_ANIM: {

    } break;
    case DSC_FT_CHARA_POS_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 pos;
        pos.x = (float_t)(int32_t)data[2] * 0.001f;
        pos.y = (float_t)(int32_t)data[3] * 0.001f;
        pos.z = (float_t)(int32_t)data[4] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4_mult_vec3_trans(&a1->scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(&pos);

        /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
            sub_1405430F0(rob_chr->chara_id, 1);
            sub_1405430F0(rob_chr->chara_id, 2);
        }*/
    } break;
    case DSC_FT_SCENE_ROT: {
        float_t scene_rot_y = (float_t)(int32_t)data[0] * 0.001f;
        a1->scene_rot_y = scene_rot_y;

        mat4_rotate_y(scene_rot_y * DEG_TO_RAD_FLOAT, &a1->scene_rot_mat);
    } break;
    case DSC_FT_EDIT_MOT_SMOOTH_LEN: {

    } break;
    case DSC_FT_PV_BRANCH_MODE: {
        int32_t branch_mode = (int32_t)data[0];
        if (branch_mode >= 0 && branch_mode <= 2)
            a1->branch_mode = branch_mode;
    } break;
    case DSC_FT_DATA_CAMERA_START: {

    } break;
    case DSC_FT_MOVIE_PLAY: {

    } break;
    case DSC_FT_MOVIE_DISP: {

    } break;
    case DSC_FT_WIND: {
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
    case DSC_FT_OSAGE_STEP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_step_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_FT_OSAGE_MV_CCL: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_mv_ccl_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_move_cancel(1, osage_mv_ccl_kami);
        rob_chr->set_osage_move_cancel(2, osage_mv_ccl_outer);
    } break;
    case DSC_FT_CHARA_COLOR: {

    } break;
    case DSC_FT_SE_EFFECT: {

    } break;
    case DSC_FT_EDIT_MOVE_XYZ: {

    } break;
    case DSC_FT_EDIT_EYELID_ANIM: {

    } break;
    case DSC_FT_EDIT_INSTRUMENT_ITEM: {

    } break;
    case DSC_FT_EDIT_MOTION_LOOP: {

    } break;
    case DSC_FT_EDIT_EYE_ANIM: {

    } break;
    case DSC_FT_EDIT_MOUTH_ANIM: {

    } break;
    case DSC_FT_EDIT_CAMERA: {

    } break;
    case DSC_FT_EDIT_MODE_SELECT: {

    } break;
    case DSC_FT_PV_END_FADEOUT: {

    } break;
    case DSC_FT_TARGET_FLAG: {

    } break;
    case DSC_FT_ITEM_ANIM_ATTACH: {

    } break;
    case DSC_FT_SHADOW_RANGE: {

    } break;
    case DSC_FT_HAND_SCALE: {

    } break;
    case DSC_FT_LIGHT_POS: {

    } break;
    case DSC_FT_FACE_TYPE: {

    } break;
    case DSC_FT_SHADOW_CAST: {

    } break;
    case DSC_FT_EDIT_MOTION_F: {

    } break;
    case DSC_FT_FOG: {

    } break;
    case DSC_FT_BLOOM: {

    } break;
    case DSC_FT_COLOR_COLLE: {

    } break;
    case DSC_FT_DOF: {

    } break;
    case DSC_FT_CHARA_ALPHA: {
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
    case DSC_FT_AOTO_CAP: {

    } break;
    case DSC_FT_MAN_CAP: {

    } break;
    case DSC_FT_TOON: {

    } break;
    case DSC_FT_SHIMMER: {

    } break;
    case DSC_FT_ITEM_ALPHA: {
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
    case DSC_FT_MOVIE_CUT_CHG: {

    } break;
    case DSC_FT_CHARA_LIGHT: {

    } break;
    case DSC_FT_STAGE_LIGHT: {

    } break;
    case DSC_FT_AGEAGE_CTRL: {

    } break;
    }
    return true;
}

static void pv_game_reset_field(pv_game_old* pvgm) {
    task_stage_info v14;
    task_stage_set_stage(&v14);
    Glitter::glt_particle_manager->FreeScenes();
}

static dsc_data* pv_game_dsc_data_find_func(pv_game_old* a1, dsc_ft_func func_name,
    int32_t* time, int32_t* pv_branch_mode, dsc_data* start, dsc_data* end) {
    int32_t _time = -1;
    for (dsc_data* i = start; i != end; i++)
        if (i->func == DSC_FT_END)
            break;
        else if (i->func == DSC_FT_TIME) {
            uint32_t* data = a1->dsc_m.get_func_data(i);
            _time = (int32_t)data[0];
        }
        else if (i->func == DSC_FT_PV_END)
            break;
        else if (i->func == DSC_FT_PV_BRANCH_MODE) {
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

static void pv_game_dsc_data_find_playdata_item_anim(pv_game_old* a1, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;
    a1->playdata[chara_id].motion_data.set_item.clear();

    dsc_data* i = a1->dsc_m.data.data();
    dsc_data* i_end = a1->dsc_m.data.data() + a1->dsc_m.data.size();
    while(true) {
        i = pv_game_dsc_data_find_func(a1, DSC_FT_ITEM_ANIM, &time, &pv_branch_mode, i, i_end);
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
            a1->playdata[chara_id].motion_data.set_item.push_back(v14);
        }
        prev_time = time;
        i++;
    }
}

static void pv_game_dsc_data_find_playdata_set_motion(pv_game_old* pvgm, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;
    pvgm->playdata[chara_id].motion_data.set_motion.clear();

    dsc_data* i = pvgm->dsc_m.data.data();
    dsc_data* i_end = pvgm->dsc_m.data.data() + pvgm->dsc_m.data.size();
    while (true) {
        i = pv_game_dsc_data_find_func(pvgm, DSC_FT_SET_MOTION, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = pvgm->dsc_m.get_func_data(i);
        if (chara_id == data[0]) {
            dsc_set_motion v14;
            v14.motion_index = data[1];
            v14.time = time;
            v14.pv_branch_mode = pv_branch_mode;
            pvgm->playdata[chara_id].motion_data.set_motion.push_back(v14);
        }
        prev_time = time;
        i++;
    }
}
#endif
