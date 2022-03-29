/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(CLOUD_DEV)
#include "x_pv_player.h"
#include "../KKdLib/dsc.h"
#include "../KKdLib/farc.h"
#include "../CRE/data.h"
#include "../CRE/Glitter/glitter.h"

x_pv_player::x_pv_player() : pv_id(), stage_id(), pp(), sr() {

}

x_pv_player::~x_pv_player() {

}

bool x_pv_player::Ctrl() {
    return false;
}

void x_pv_player::Disp() {

}

void x_pv_player::Load(int32_t pv_id, int32_t stage_id) {
    this->pv_id = pv_id;
    this->stage_id = stage_id;

    char path_buf[0x200];
    char file_buf[0x200];

    data_struct* x_data = &data_list[DATA_X];
    GPM_VAL.data = x_data;

    sprintf_s(file_buf, sizeof(file_buf), "pv%03d.pvpp", pv_id);
    pp = new pvpp;
    x_data->load_file(pp, "rom/pv/", file_buf, pvpp::load_file);

    sprintf_s(file_buf, sizeof(file_buf), "stgpv%03d_param.pvsr", stage_id);
    pvsr_init(&sr);
    x_data->load_file(&sr, "rom/pv_stage_rsrc/", file_buf, pvsr_load_file);

    glt_particle_manager.data = x_data;

    sprintf_s(file_buf, sizeof(file_buf), "eff_pv%03d_main", pv_id);
    pv_glitter = x_pv_player_glitter(file_buf);

    sprintf_s(file_buf, sizeof(file_buf), "eff_stgpv%03d_main", stage_id);
    stage_glitter = x_pv_player_glitter(file_buf);

    dsc dsc_scene;
    dsc dsc_system;
    dsc dsc_easy;

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

    dsc dsc_m;
    dsc_m.merge(3, &dsc_scene, &dsc_system, &dsc_easy);

    this->frame_data.clear();
    this->frame_data.shrink_to_fit();

    int32_t end_func_id = dsc_x_get_func_id("END");
    int32_t time_func_id = dsc_x_get_func_id("TIME");
    int32_t bar_time_set_func_id = dsc_x_get_func_id("BAR_TIME_SET");
    int32_t pv_end_func_id = dsc_x_get_func_id("PV_END");
    int32_t bar_point_func_id = dsc_x_get_func_id("BAR_POINT");
    int32_t stage_effect_func_id = dsc_x_get_func_id("STAGE_EFFECT");
    int32_t song_effect_func_id = dsc_x_get_func_id("SONG_EFFECT");

    int32_t time = -1;
    int32_t frame = -1;
    bool bar_set = false;
    int32_t prev_bar_point_time = -1;
    int32_t prev_bpm = -1;
    int32_t end_frame = -1;
    for (dsc_data& i : dsc_m.data) {
        uint32_t* data = dsc_m.get_func_data(&i);
        if (i.func == end_func_id)
            break;
        else if (i.func == time_func_id) {
            time = (int32_t)data[0];
            frame = (int32_t)roundf((float_t)time * (float_t)(60.0f / 100000.0f));
        }
        else if (i.func == bar_time_set_func_id) {
            if (prev_bar_point_time != -1)
                continue;

            int32_t bpm = (int32_t)data[0];
            int32_t time_signature = (int32_t)(data[1] + 1);

            if (bpm != prev_bpm) {
                x_pv_player_frame_data frame_data;
                frame_data.frame = frame;
                frame_data.type = X_PV_PLAYER_FRAME_DATA_BPM;
                frame_data.bpm = bpm;
                this->frame_data.push_back(frame_data);

            }
            prev_bpm = bpm;
        }
        else if (i.func == pv_end_func_id) {
            end_frame = frame;
            break;
        }
        else if (i.func == bar_point_func_id) {
            if (prev_bar_point_time != -1) {
                float_t frame_speed = 200000.0f / (float_t)(time - prev_bar_point_time);
                int32_t bpm = (int32_t)roundf(frame_speed * 120.0f);
                if (bpm != prev_bpm) {
                    x_pv_player_frame_data frame_data;
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
            x_pv_player_frame_data frame_data;
            frame_data.frame = frame;
            frame_data.type = X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT;
            frame_data.stage_effect = (int32_t)data[0];
            this->frame_data.push_back(frame_data);
        }
        else if (i.func == song_effect_func_id) {
            x_pv_player_frame_data frame_data;
            frame_data.frame = frame;
            frame_data.type = X_PV_PLAYER_FRAME_DATA_SONG_EFFECT;
            frame_data.song_effect.enable = data[0] ? true : false;
            frame_data.song_effect.id = (int32_t)data[1];
            this->frame_data.push_back(frame_data);
        }
    }
}

x_pv_player_glitter::x_pv_player_glitter(char* name) {
    this->name = std::string(name);
    hash = (uint32_t)GPM_VAL.LoadFile(GLITTER_X, name, 0, -1.0f, false);
}

x_pv_player_glitter::~x_pv_player_glitter() {

}
#endif
