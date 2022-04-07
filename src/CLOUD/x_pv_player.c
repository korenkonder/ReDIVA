/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(CLOUD_DEV)
#include <set>
#include "x_pv_player.h"
#include "../CRE/Glitter/glitter.h"
#include "../CRE/data.h"
#include "../CRE/object.h"
#include "../CRE/stage_modern.h"
#include "../KKdLib/dsc.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/sort.h"

x_pv_player x_pv_player_data;

extern render_context* rctx_ptr;

x_pv_player::x_pv_player() : pp(), sr(), state(), frame(),
rob_chara_ids(), pv_glitter(), stage_glitter(), curr_frame_data() {
    pv_id = -1;
    stage_id = -1;
    effpv_objset = hash_murmurhash_empty;
    stgpv_objset = hash_murmurhash_empty;
    stgpvhrc_objset = hash_murmurhash_empty;
    stage_category_hash = hash_murmurhash_empty;
    pv_category_hash = hash_murmurhash_empty;
    light_auth_3d_id = -1;
    camera_auth_3d_id = -1;
}

x_pv_player::~x_pv_player() {

}

bool x_pv_player::Init() {
    task_stage_modern_load("X_PV_PLAYER_STAGE");
    return true;
}

static int x_pv_player_frame_data_quicksort_compare_func(void const* src1, void const* src2) {
    int32_t frame1 = ((x_pv_player_frame_data*)src1)->frame;
    int32_t frame2 = ((x_pv_player_frame_data*)src2)->frame;
    return frame1 > frame2 ? 1 : (frame1 < frame2 ? -1 : 0);
}

bool x_pv_player::Ctrl() {
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
        if ((!pv_glt || GPM_VAL.CheckNoFileReaders(pv_glitter->hash))
            && (!stg_glt || GPM_VAL.CheckNoFileReaders(stage_glitter->hash))) {
            data_struct* x_data = &data_list[DATA_X];

            bool wait_load = false;
            if (pv_glt) {
                GlitterEffectGroup* pv_eff_group = GPM_VAL.GetEffectGroup(pv_glitter->hash);
                if (pv_eff_group && pv_eff_group->CheckModel()) {
                    pv_eff_group->LoadModel(x_data);
                    wait_load = true;
                }
            }

            if (stg_glt) {
                GlitterEffectGroup* stage_eff_group = GPM_VAL.GetEffectGroup(stage_glitter->hash);
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
        GlitterEffectGroup* pv_eff_group = GPM_VAL.GetEffectGroup(pv_glitter->hash);
        if (pv_eff_group && pv_eff_group->CheckLoadModel())
            wait_load = true;

        GlitterEffectGroup* stage_eff_group = GPM_VAL.GetEffectGroup(stage_glitter->hash);
        if (stage_eff_group && stage_eff_group->CheckLoadModel())
            wait_load = true;

        if (!wait_load)
            state = 4;
    }
    else if (state == 4) {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            if (rob_chara_ids[i] == -1)
                continue;

            int32_t chara_id = rob_chara_ids[i];
            int32_t motion_id = aft_mot_db->get_motion_id(pp->chara[i].motion.front().name.c_str());
            rob_chara_set_motion_id(&rob_chara_array[chara_id], motion_id, 0.0f,
                motion_storage_get_mot_data_frame_count(motion_id, aft_mot_db),
                false, true, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chara_set_frame(&rob_chara_array[chara_id], 0.0f);
            rob_chara_item_equip* rob_item_equip = rob_chara_array[chara_id].item_equip;
            for (int32_t j = rob_item_equip->first_item_equip_object;
                j < rob_item_equip->max_item_equip_object; j++) {
                rob_chara_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[j];
                itm_eq_obj->osage_iterations = 60;
                for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
                    if (i)
                        i->rob.osage_reset = true;
            }
        }
        state = 5;
    }
    else if (state == 5) {
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

        auth_3d_data_load_category(x_data, stage_category.c_str(), stage_category_hash);
        auth_3d_data_load_category(x_data, pv_category.c_str(), pv_category_hash);
        auth_3d_data_load_category(light_category.c_str());
        auth_3d_data_load_category(camera_category.c_str());
        state = 9;
    }
    else if (state == 9) {
        if (!auth_3d_data_check_category_loaded(stage_category_hash)
            || !auth_3d_data_check_category_loaded(pv_category_hash)
            || !auth_3d_data_check_category_loaded(light_category.c_str())
            || !auth_3d_data_check_category_loaded(camera_category.c_str()))
            return false;

        data_struct* x_data = &data_list[DATA_X];

        for (std::string i : stage_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, stage_category_hash,
                stage_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_visibility(&id, false);
            stage_auth_3d_ids.insert({ hash, id });
        }

        for (std::string i : pv_auth_3d_names) {
            uint32_t hash = hash_string_murmurhash(&i);
            int32_t id = auth_3d_data_load_hash(hash, pv_category_hash,
                pv_category.c_str(), x_data, &obj_db, &tex_db);
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_visibility(&id, false);
            pv_auth_3d_ids.insert({ hash, id });
        }

        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];

            light_auth_3d_id = -1;
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000", pv_id);
            uint32_t light_auth_3d_hash = hash_utf8_murmurhash(buf);
            for (auth_3d_database_uid& i : auth_3d_db->uid)
                if (hash_string_murmurhash(&i.name) == light_auth_3d_hash) {
                    light_auth_3d_id = auth_3d_data_load_uid(
                        (int32_t)(&i - auth_3d_db->uid.data()), auth_3d_db);
                    auth_3d_data_read_file(&light_auth_3d_id, auth_3d_db);
                    auth_3d_data_set_enable(&light_auth_3d_id, true);
                    auth_3d_data_set_req_frame(&light_auth_3d_id, 0.0f);
                    auth_3d_data_set_max_frame(&light_auth_3d_id, -1.0f);
                    auth_3d_data_set_paused(&light_auth_3d_id, false);
                    auth_3d_data_set_visibility(&light_auth_3d_id, true);
                    break;
                }

            camera_auth_3d_id = -1;
            sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
            uint32_t camera_auth_3d_hash = hash_utf8_murmurhash(buf);
            for (auth_3d_database_uid& i : auth_3d_db->uid)
                if (hash_string_murmurhash(&i.name) == camera_auth_3d_hash) {
                    camera_auth_3d_id = auth_3d_data_load_uid(
                        (int32_t)(&i - auth_3d_db->uid.data()), auth_3d_db);
                    auth_3d_data_read_file(&camera_auth_3d_id, auth_3d_db);
                    auth_3d_data_set_enable(&camera_auth_3d_id, true);
                    auth_3d_data_set_req_frame(&camera_auth_3d_id, 0.0f);
                    auth_3d_data_set_max_frame(&camera_auth_3d_id, -1.0f);
                    auth_3d_data_set_paused(&camera_auth_3d_id, false);
                    auth_3d_data_set_visibility(&camera_auth_3d_id, true);
                    break;
                }
        }

        dsc dsc_m;
        {
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "rom/pv_script/pv%03d/", pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
            x_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

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

            dsc_m.merge(3, &dsc_scene, &dsc_system, &dsc_easy);
        }

        frame_data.clear();
        frame_data.shrink_to_fit();

        int32_t end_func_id = dsc_x_get_func_id("END");
        int32_t time_func_id = dsc_x_get_func_id("TIME");
        int32_t pv_end_func_id = dsc_x_get_func_id("PV_END");
        int32_t bar_point_func_id = dsc_x_get_func_id("BAR_POINT");
        int32_t stage_effect_func_id = dsc_x_get_func_id("STAGE_EFFECT");
        int32_t song_effect_func_id = dsc_x_get_func_id("SONG_EFFECT");

        std::vector<int32_t> bar_frames;
        bar_frames.push_back(-1);
        bar_frames.push_back(0);

        int32_t time = -1;
        int32_t frame = -1;
        int32_t bar = 1;
        bool bar_set = false;
        int32_t prev_bar_point_time = -1;
        int32_t prev_bpm = -1;
        int32_t prev_stage_effect = -1;
        for (dsc_data& i : dsc_m.data) {
            uint32_t* data = dsc_m.get_func_data(&i);
            if (i.func == end_func_id)
                break;
            else if (i.func == time_func_id) {
                time = (int32_t)data[0];
                frame = (int32_t)roundf((float_t)time * (float_t)(60.0f / 100000.0f));
            }
            else if (i.func == pv_end_func_id) {
                x_pv_player_frame_data frame_data = {};
                frame_data.frame = frame;
                frame_data.type = X_PV_PLAYER_FRAME_DATA_PV_END;
                this->frame_data.push_back(frame_data);
                break;
            }
            else if (i.func == bar_point_func_id) {
                bar = data[0];
                if (bar > bar_frames.size()) {
                    int32_t frame = bar_frames.back();
                    int32_t delta_frame = (int32_t)roundf(120.0f / (float_t)prev_bpm * 120.0f);
                    for (size_t i = bar - bar_frames.size(); i; i--)
                        bar_frames.push_back(frame += delta_frame);
                }
                bar_frames.push_back(frame);

                if (prev_bar_point_time != -1) {
                    float_t frame_speed = 200000.0f / (float_t)(time - prev_bar_point_time);
                    int32_t bpm = (int32_t)roundf(frame_speed * 120.0f);
                    if (bpm != prev_bpm) {
                        x_pv_player_frame_data frame_data = {};
                        frame_data.frame = frame;
                        frame_data.type = X_PV_PLAYER_FRAME_DATA_BPM;
                        frame_data.bpm = bpm;
                        this->frame_data.push_back(frame_data);
                    }
                    prev_bpm = bpm;
                }
                prev_bar_point_time = time;
            }
            else if (i.func == stage_effect_func_id) {
                int32_t stage_effect = (int32_t)data[0];
                stage_effect--;

                if (stage_effect >= 0 && stage_effect <= 6) {
                    if (prev_stage_effect != -1 && stage_effect != prev_stage_effect
                        && sr->stage_change_effect_init[prev_stage_effect][stage_effect]) {
                        pvsr_stage_effect& stg_eff = sr->stage_change_effect[prev_stage_effect][stage_effect];
                        uint8_t& bar_count = stg_eff.bar_count;

                        int32_t prev_bar = bar - 2;
                        if (bar_count && bar_count != 0xFF)
                            prev_bar = bar - bar_count;
                        prev_bar = max(prev_bar, 1);

                        if (bar_frames.size() == 2 && prev_bpm == -1) {
                            int32_t bpm = (int32_t)roundf(120.0f * (120.0f / frame));
                            if (bpm != prev_bpm) {
                                x_pv_player_frame_data frame_data = {};
                                frame_data.frame = 0;
                                frame_data.type = X_PV_PLAYER_FRAME_DATA_BPM;
                                frame_data.bpm = bpm;
                                this->frame_data.push_back(frame_data);
                            }
                            prev_bpm = bpm;

                            for (x_pv_player_frame_data& i : this->frame_data) {
                                if (i.frame != 0 || i.type != X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT)
                                    continue;

                                x_pv_player_frame_data& frame_data = i;
                                frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT;
                                frame_data.stage_effect.prev = prev_stage_effect;
                                frame_data.stage_effect.next = stage_effect;
                                break;
                            }
                        }
                        else {
                            x_pv_player_frame_data frame_data = {};
                            frame_data.frame = bar_frames[prev_bar] + (frame - bar_frames[bar]);
                            frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT;
                            frame_data.stage_effect.prev = prev_stage_effect;
                            frame_data.stage_effect.next = stage_effect;
                            this->frame_data.push_back(frame_data);
                        }

                        x_pv_player_frame_data frame_data = {};
                        frame_data.frame = frame;
                        frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT_END;
                        frame_data.stage_effect.prev = prev_stage_effect;
                        frame_data.stage_effect.next = stage_effect;
                        this->frame_data.push_back(frame_data);
                        prev_stage_effect = stage_effect;
                    }
                    else {
                        x_pv_player_frame_data frame_data = {};
                        frame_data.frame = frame;
                        frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT;
                        frame_data.stage_effect.prev = prev_stage_effect;
                        frame_data.stage_effect.next = stage_effect;
                        this->frame_data.push_back(frame_data);
                        prev_stage_effect = stage_effect;
                    }
                }
                else if (stage_effect >= 7 && stage_effect <= 8) {
                    x_pv_player_frame_data frame_data = {};
                    frame_data.frame = frame;
                    frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT_ONE_SHOT;
                    frame_data.stage_effect_one_shot = stage_effect;
                    this->frame_data.push_back(frame_data);
                }
            }
            else if (i.func == song_effect_func_id) {
                x_pv_player_frame_data frame_data = {};
                frame_data.frame = frame;
                frame_data.type = X_PV_PLAYER_FRAME_DATA_SONG_EFFECT;
                frame_data.song_effect.enable = data[0] ? true : false;
                frame_data.song_effect.id = (int32_t)data[1];
                this->frame_data.push_back(frame_data);
            }
        }

        quicksort_custom(frame_data.data(), frame_data.size(),
            sizeof(x_pv_player_frame_data), x_pv_player_frame_data_quicksort_compare_func);

        curr_frame_data = frame_data.size() ? &frame_data.front() : 0;

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                rob_chara_set_visibility(&rob_chara_array[rob_chara_ids[i]], true);
        state = 10;
    }
    else if (state == 10) {
        while (curr_frame_data && state == 10) {
            if (curr_frame_data->frame > frame)
                break;
            else if (curr_frame_data->frame < frame) {
                curr_frame_data++;
                continue;
            }

            switch (curr_frame_data->type) {
            case X_PV_PLAYER_FRAME_DATA_PV_END: {
                curr_frame_data = 0;
                state = 99;
                SetDest();
            } break;
            case X_PV_PLAYER_FRAME_DATA_BPM: {
                diva_stage_frame_rate.SetFrameSpeed((float_t)curr_frame_data->bpm / 120.0f);
            } break;
            case X_PV_PLAYER_FRAME_DATA_SONG_EFFECT: {
                x_pv_player_song_effect& song_effect = curr_frame_data->song_effect;
                uint32_t hash = pv_glitter->hash;

                if (song_effect.id >= pp->effect.size())
                    break;

                pvpp_effect& effect = pp->effect[song_effect.id];
                if (song_effect.enable) {
                    GPM_VAL.SetSceneName(pv_glitter->hash, pv_glitter->name.c_str());
                    for (pvpp_glitter& i : effect.glitter) {
                        GPM_VAL.LoadScene(hash_murmurhash_empty, hash_string_murmurhash(&i.name), true);
                        GPM_VAL.SetSceneEffectName(hash_murmurhash_empty,
                            hash_string_murmurhash(&i.name), i.name.c_str());
                        GPM_VAL.SetSceneFrameRate(hash, &diva_pv_frame_rate);
                    }
                }
                else
                    for (pvpp_glitter& i : effect.glitter)
                        GPM_VAL.FreeSceneEffect(0, hash_string_murmurhash(&i.name), false);
            } break;
            case X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT: {
                x_pv_player_stage_effect& stage_effect = curr_frame_data->stage_effect;
                uint32_t hash = stage_glitter->hash;

                if (stage_effect.prev != -1) {
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect.prev];
                    for (pvsr_a3da& i : effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_visibility(&id, false);
                        auth_3d_data_set_enable(&id, false);
                    }

                    for (pvsr_glitter& i : effect.glitter)
                        GPM_VAL.FreeSceneEffect(0, hash_string_murmurhash(&i.name), false);
                }

                if (stage_effect.next != -1) {
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect.next];
                    for (pvsr_a3da& i : effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_enable(&id, true);
                        auth_3d_data_set_req_frame(&id, 0.0f);
                        auth_3d_data_set_max_frame(&id, -1.0f);
                        auth_3d_data_set_paused(&id, false);
                        auth_3d_data_set_visibility(&id, true);
                        auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
                    }

                    GPM_VAL.SetSceneName(stage_glitter->hash, stage_glitter->name.c_str());
                    for (pvsr_glitter& i : effect.glitter) {
                        GPM_VAL.LoadScene(hash_murmurhash_empty, hash_string_murmurhash(&i.name), true);
                        GPM_VAL.SetSceneEffectName(hash_murmurhash_empty,
                            hash_string_murmurhash(&i.name), i.name.c_str());
                        GPM_VAL.SetSceneFrameRate(hash, &diva_stage_frame_rate);
                    }
                }
            } break;
            case X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT_ONE_SHOT: {
                int32_t& stage_effect = curr_frame_data->stage_effect_one_shot;
                uint32_t hash = stage_glitter->hash;

                pvsr_stage_effect& effect = sr->stage_effect[stage_effect];
                for (pvsr_a3da& i : effect.a3da) {
                    int32_t& id = stage_auth_3d_ids[i.hash];
                    auth_3d_data_set_enable(&id, true);
                    auth_3d_data_set_req_frame(&id, 0.0f);
                    auth_3d_data_set_max_frame(&id, -1.0f);
                    auth_3d_data_set_paused(&id, false);
                    auth_3d_data_set_visibility(&id, true);
                    auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
                }

                GPM_VAL.SetSceneName(stage_glitter->hash, stage_glitter->name.c_str());
                for (pvsr_glitter& i : effect.glitter) {
                    GPM_VAL.LoadScene(hash_murmurhash_empty, hash_string_murmurhash(&i.name), true);
                    GPM_VAL.SetSceneEffectName(hash_murmurhash_empty,
                        hash_string_murmurhash(&i.name), i.name.c_str());
                    GPM_VAL.SetSceneFrameRate(hash, &diva_stage_frame_rate);
                }
            } break;
            case X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT: {
                x_pv_player_stage_effect& stage_effect = curr_frame_data->stage_effect;
                uint32_t hash = stage_glitter->hash;

                if (sr->stage_change_effect_init[stage_effect.prev][stage_effect.next]) {
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect.prev];
                    for (pvsr_a3da& i : effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_visibility(&id, false);
                        auth_3d_data_set_enable(&id, false);
                    }

                    for (pvsr_glitter& i : effect.glitter)
                        GPM_VAL.FreeSceneEffect(0, hash_string_murmurhash(&i.name), false);

                    pvsr_stage_effect& change_effect = sr->stage_change_effect[stage_effect.prev][stage_effect.next];
                    for (pvsr_a3da& i : change_effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_enable(&id, true);
                        auth_3d_data_set_req_frame(&id, 0.0f);
                        auth_3d_data_set_max_frame(&id, -1.0f);
                        auth_3d_data_set_paused(&id, false);
                        auth_3d_data_set_visibility(&id, true);
                        auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
                    }

                    GPM_VAL.SetSceneName(stage_glitter->hash, stage_glitter->name.c_str());
                    for (pvsr_glitter& i : change_effect.glitter) {
                        GPM_VAL.LoadScene(hash_murmurhash_empty, hash_string_murmurhash(&i.name), true);
                        GPM_VAL.SetSceneEffectName(hash_murmurhash_empty,
                            hash_string_murmurhash(&i.name), i.name.c_str());
                        GPM_VAL.SetSceneFrameRate(hash, &diva_stage_frame_rate);
                    }
                }
            } break;
            case X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT_END: {
                x_pv_player_stage_effect& stage_effect = curr_frame_data->stage_effect;
                uint32_t hash = stage_glitter->hash;

                if (sr->stage_change_effect_init[stage_effect.prev][stage_effect.next]) {
                    pvsr_stage_effect& change_effect = sr->stage_change_effect[stage_effect.prev][stage_effect.next];
                    for (pvsr_a3da& i : change_effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_visibility(&id, false);
                        auth_3d_data_set_enable(&id, false);
                    }

                    for (pvsr_glitter& i : change_effect.glitter)
                        GPM_VAL.FreeSceneEffect(0, hash_string_murmurhash(&i.name), false);
                }
                else {
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect.prev];
                    for (pvsr_a3da& i : effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_visibility(&id, false);
                        auth_3d_data_set_enable(&id, false);
                    }

                    for (pvsr_glitter& i : effect.glitter)
                        GPM_VAL.FreeSceneEffect(0, hash_string_murmurhash(&i.name), false);
                }

                if (stage_effect.next != -1) {
                    pvsr_stage_effect& effect = sr->stage_effect[stage_effect.next];
                    for (pvsr_a3da& i : effect.a3da) {
                        int32_t& id = stage_auth_3d_ids[i.hash];
                        auth_3d_data_set_enable(&id, true);
                        auth_3d_data_set_req_frame(&id, 0.0f);
                        auth_3d_data_set_max_frame(&id, -1.0f);
                        auth_3d_data_set_paused(&id, false);
                        auth_3d_data_set_visibility(&id, true);
                        auth_3d_data_set_frame_rate(&id, &diva_stage_frame_rate);
                    }

                    GPM_VAL.SetSceneName(stage_glitter->hash, stage_glitter->name.c_str());
                    for (pvsr_glitter& i : effect.glitter) {
                        GPM_VAL.LoadScene(hash_murmurhash_empty, hash_string_murmurhash(&i.name), true);
                        GPM_VAL.SetSceneEffectName(hash_murmurhash_empty,
                            hash_string_murmurhash(&i.name), i.name.c_str());
                        GPM_VAL.SetSceneFrameRate(hash, &diva_stage_frame_rate);
                    }
                }
            } break;
            }
            curr_frame_data++;
        }

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                rob_chara_set_frame(&rob_chara_array[i], (float_t)frame);
        frame++;
    }
    return false;
}

void x_pv_player::Disp() {
    if (state != 9)
        return;



}

bool x_pv_player::Dest() {
    Unload();
    task_stage_modern_unload();
    return true;
}

void x_pv_player::Load(int32_t pv_id, int32_t stage_id) {
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
    x_data->load_file(pp, "rom/pv/", file_buf, pvpp::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "stgpv%03d_param.pvsr", stage_id);
    sr = new pvsr;
    x_data->load_file(sr, "rom/pv_stage_rsrc/", file_buf, pvsr::load_file);
    
    sprintf_s(file_buf, sizeof(file_buf), "pv_stgpv%03d.stg", stage_id);
    stage_data.modern = true;
    x_data->load_file(&stage_data, "rom/stage/", file_buf, stage_database::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "eff_pv%03d_main", pv_id);
    pv_glitter = new x_pv_player_glitter(file_buf);

    sprintf_s(file_buf, sizeof(file_buf), "eff_stgpv%03d_main", stage_id);
    stage_glitter = new x_pv_player_glitter(file_buf);

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
    stage_category = std::string(buf);
    stage_category_hash = hash_string_murmurhash(&stage_category);

    sprintf_s(buf, sizeof(buf), "A3D_EFFPV%03d", pv_id);
    pv_category = std::string(buf);
    pv_category_hash = hash_string_murmurhash(&pv_category);

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = std::string(buf);

    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    camera_category = std::string(buf);

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

    pv_auth_3d_names.clear();
    for (pvpp_effect i : pp->effect)
        for (pvpp_a3da j : i.a3da)
            if (pv_auth_3d_names.find(j.name) == pv_auth_3d_names.end())
                pv_auth_3d_names.insert(j.name);

    stage_hashes.clear();
    stages_data.clear();
    for (stage_data_modern& i : stage_data.stage_modern) {
        stage_hashes.push_back(i.hash);
        stages_data.push_back(&i);
    }

    for (int32_t& i : rob_chara_ids)
        i = -1;

    int32_t chara_index = 0;
    for (pvpp_chara& i : pp->chara) {
        if (i.motion.size()) {
            rob_chara_pv_data pv_data;
            int32_t chara_id = rob_chara_array_init_chara_index(CHARA_MIKU, &pv_data, 0, true);
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
}

void x_pv_player::Unload() {
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

    for (int32_t& i : rob_chara_ids)
        if (i != -1) {
            rob_chara_array_free_chara_id(i);
            i = -1;
        }

    delete pv_glitter;
    delete stage_glitter;
    pv_glitter = 0;
    stage_glitter = 0;

    frame_data.clear();
    frame_data.shrink_to_fit();
    curr_frame_data = 0;

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

    stage_category.clear();
    stage_category.shrink_to_fit();

    pv_category.clear();
    pv_category.shrink_to_fit();
    light_category.clear();
    light_category.shrink_to_fit();
    camera_category.clear();
    camera_category.shrink_to_fit();

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

    auth_3d_data_unload_category(stage_category_hash);
    auth_3d_data_unload_category(pv_category_hash);

    stage_category_hash = hash_murmurhash_empty;
    pv_category_hash = hash_murmurhash_empty;

    object_storage_unload_set(effpv_objset);
    object_storage_unload_set(stgpv_objset);
    object_storage_unload_set(stgpvhrc_objset);

    effpv_objset = hash_murmurhash_empty;
    stgpv_objset = hash_murmurhash_empty;
    stgpvhrc_objset = hash_murmurhash_empty;

}

x_pv_player_glitter::x_pv_player_glitter(char* name) {
    this->name = std::string(name);
    hash = (uint32_t)GPM_VAL.LoadFile(GLITTER_X, &data_list[DATA_X], name, 0, -1.0f, false, 0);
}

x_pv_player_glitter::~x_pv_player_glitter() {
    GPM_VAL.UnloadEffectGroup(hash);
}
#endif
