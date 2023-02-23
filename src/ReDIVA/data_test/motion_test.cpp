/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion_test.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/hand_item.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "../task_window.hpp"
#include "auth_3d_test.hpp"

extern int32_t width;
extern int32_t height;
extern render_context* rctx_ptr;

DataTestMot* data_test_mot;
DtmMot* dtm_mot_array;
DtmEqVs* dtm_eq_vs_array;

std::vector<uint32_t> motion_test_objset;

static void dtm_mot_array_set_reset_mot();

static bool motion_test_objset_check_not_read();
static void motion_test_objset_load();
static void motion_test_objset_unload();

DataTestMot::Data::Divide::Divide() {
    array[0] = 0.0f;
    array[1] = 0.0f;
    array[2] = 0.0f;
}

DataTestMot::Data::Step::Step() {
    array[0] = 0.0f;
    array[1] = 0.0f;
    array[2] = 0.0f;
    array[3] = 1.0f;
}

DataTestMot::Data::Data() : chara_index_p1(), chara_index_p2(), curr_chara_index_p1(),
curr_chara_index_p2(), module_index_p1(), module_index_p2(), curr_module_index_p1(),
curr_module_index_p2(), motion_set_index_p1(), motion_set_index_p2(),
curr_motion_set_index_p1(), curr_motion_set_index_p2(), motion_index_p1(),
motion_index_p2(), curr_motion_index_p1(), curr_motion_index_p2(), rot_y_p1(), rot_y_p2(),
trans_x_p1(), trans_x_p2(), pre_offset_p1(), post_offset_p1(), pre_offset_p2(), post_offset_p2(),
start_frame_p1(), start_frame_p2(), type(), reset_mot(), reset_cam(), reload_data(),
sync_frame(), field_A8(), field_A9(), running(), field_AC(), field_B0(), sync_1p_frame() {
    field_AA = true;
}

DataTestMot::DataTestMot() {

}

DataTestMot::~DataTestMot() {

}

bool DataTestMot::Init() {
    clear_color = { 0x60, 0x60, 0x60, 0xFF };
    set_clear_color = true;

    camera* cam = rctx_ptr->camera;
    cam->reset();
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    //DataTestMotDw::InitByCharaID(0, &dtm_mot_array[-]);
    //DataTestMotDw::InitByCharaID(1, &dtm_mot_array[1]);
    //data_test_mot_ctrl_dw_init();
    //data_test_mot_a3d_dw_init();
    data.reset_cam = true;
    data.field_A8 = true;
    dtm_eq_vs_array[0].AddTask(0, data.chara_index_p1);
    dtm_eq_vs_array[1].AddTask(1, data.chara_index_p1);
    motion_test_objset_load();
    return true;
}

bool DataTestMot::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    //data_mot_test_dw_array_get(0)->sub_14028D8B0();

    bool v3 = false;
    bool v2 = false;

    if (data.reload_data) {
        data.reload_data = false;
        v3 = true;
        v2 = true;
    }

    if (data.chara_index_p1 != data.curr_chara_index_p1) {
        data.curr_chara_index_p1 = data.chara_index_p1;
        dtm_eq_vs_array[0].SetCharaIndexModuleIndex(data.chara_index_p1, data.module_index_p1);
        v3 = true;
        v2 = true;
    }

    if (data.chara_index_p2 != data.curr_chara_index_p2) {
        data.curr_chara_index_p2 = data.chara_index_p2;
        dtm_eq_vs_array[1].SetCharaIndexModuleIndex(data.chara_index_p2, data.module_index_p2);
        v3 = true;
        v2 = true;
    }

    if (data.module_index_p1 != data.curr_module_index_p1) {
        data.curr_module_index_p1 = data.module_index_p1;
        dtm_eq_vs_array[0].SetCharaIndexModuleIndex(data.chara_index_p1, data.module_index_p1);
        v3 = true;
        v2 = true;
    }

    if (data.module_index_p2 != data.curr_module_index_p2) {
        data.curr_module_index_p2 = data.module_index_p2;
        dtm_eq_vs_array[1].SetCharaIndexModuleIndex(data.chara_index_p2, data.module_index_p2);
        v3 = true;
        v2 = true;
    }

    if (data.motion_set_index_p1 != data.curr_motion_set_index_p1) {
        data.curr_motion_set_index_p1 = data.motion_set_index_p1;

        /*data_mot_test_dw_array_get(0)->field_D8();

        uint32_t v16 = motion_database_set_indices_get_id(data.motion_set_index_p1);
        uint32_t v18 = motion_database_get_set_motion_count(v16);
        for (uint32_t i = 0; i < v18; i++) {
            uint32_t v20 = motion_database_get_motion_id(v16, i);
            const char* v21 = motion_database_get_motion_name_by_motion_index(v20);
            data_mot_test_dw_array_get(0)->field_E0(v21);
        }

        data_mot_test_dw_array_get(0)->sub_14028EB80();*/

        data.curr_motion_index_p1 = 0;
        data.motion_index_p1 = 0;
        v3 = true;
        v2 = true;
    }

    if (data.motion_set_index_p2 != data.curr_motion_set_index_p2) {
        data.curr_motion_set_index_p2 = data.motion_set_index_p2;

        /*data_mot_test_dw_array_get(1)->field_D8();

        uint32_t v24 = motion_database_set_indices_get_id(data.motion_set_index_p2);
        uint32_t v18 = motion_database_get_set_motion_count(v24);
        for (uint32_t i = 0; i < v18; i++) {
            uint32_t v28 = motion_database_get_motion_id(v24, i);
            const char* v29 = motion_database_get_motion_name_by_motion_index(v28);
            data_mot_test_dw_array_get(0)->field_E0(v29);
        }

        data_mot_test_dw_array_get(1)->sub_14028EB80();*/

        data.curr_motion_index_p2 = 0;
        data.motion_index_p2 = 0;

        v3 = true;
        v2 = true;
    }

    bool v59 = false;
    if (data.motion_index_p1 != data.curr_motion_index_p1) {
        data.curr_motion_index_p1 = data.motion_index_p1;
        dtm_mot_array[0].SetMotion(data.motion_set_index_p1, data.motion_index_p1);
        dtm_mot_array[1].SetMotion(data.motion_set_index_p2, data.motion_index_p2);
        dtm_mot_array[0].SetChangeMotion();
        dtm_mot_array[1].SetChangeMotion();
        v59 = true;
    }

    if (data.motion_index_p2 != data.curr_motion_index_p2) {
        data.curr_motion_index_p2 = data.motion_index_p2;
        dtm_mot_array[0].SetMotion(data.motion_set_index_p1, data.motion_index_p1);
        dtm_mot_array[1].SetMotion(data.motion_set_index_p2, data.motion_index_p2);
        dtm_mot_array[0].SetChangeMotion();
        dtm_mot_array[1].SetChangeMotion();
    }

    dtm_mot_array[0].SetRotationY(data.rot_y_p1);
    dtm_mot_array[1].SetRotationY(data.rot_y_p2);
    dtm_mot_array[0].SetTrans({ data.trans_x_p1, 0.0f, 0.0f });
    dtm_mot_array[1].SetTrans({ data.trans_x_p2, 0.0f, 0.0f });
    dtm_mot_array[0].SetOffset(data.pre_offset_p1, data.post_offset_p1);
    dtm_mot_array[1].SetOffset(data.pre_offset_p2, data.post_offset_p2);
    dtm_mot_array[0].SetStartFrame(data.start_frame_p1);
    dtm_mot_array[1].SetStartFrame(data.start_frame_p2);

    for (int32_t i = 0; i < 3; i++) {
        dtm_mot_array[0].SetDivide(i, data.divide_p1.array[i]);
        dtm_mot_array[1].SetDivide(i, data.divide_p2.array[i]);
    }

    for (int32_t i = 0; i < 4; i++) {
        dtm_mot_array[0].SetStep(i, data.step_p1.array[i]);
        dtm_mot_array[1].SetStep(i, data.step_p2.array[i]);
    }

    switch (data.type) {
    case 0: {
        dtm_mot_array[0].SetLoop(false);
        dtm_mot_array[1].SetLoop(false);

        dtm_mot_array[0].SetPlay(true);
        dtm_mot_array[1].SetPlay(true);
    } break;
    case 1: {
        dtm_mot_array[0].SetLoop(true);
        dtm_mot_array[1].SetLoop(true);

        dtm_mot_array[0].SetPlay(true);
        dtm_mot_array[1].SetPlay(true);
    } break;
    case 2: {
        dtm_mot_array[0].SetLoop(true);
        dtm_mot_array[1].SetLoop(true);

        bool play = false;//input_state_get(0)->sub_14018D540(84) != 0;
        dtm_mot_array[0].SetPlay(play);
        dtm_mot_array[1].SetPlay(play);
    } break;
    }

    if (data.reset_mot) {
        data.reset_mot = false;

        dtm_mot_array[0].SetResetMot();
        dtm_mot_array[1].SetResetMot();
    }

    if (data.sync_frame) {
        int32_t chara_id = dtm_mot_array[1].GetFrameCount() > dtm_mot_array[0].GetFrameCount() ? 1 : 0;
        float_t frame = dtm_mot_array[chara_id].GetFrame();
        dtm_mot_array[0].SetFrame(frame);
        dtm_mot_array[1].SetFrame(frame);
    }

    if (v3)
        dtm_mot_array[0].DelTask();
    else
        dtm_mot_array[0].AddTask(data.chara_index_p1,
            data.module_index_p1, data.motion_set_index_p1, data.motion_index_p1);

    if (v2)
        dtm_mot_array[1].DelTask();
    else
        dtm_mot_array[1].AddTask(data.chara_index_p2,
            data.module_index_p2, data.motion_set_index_p2, data.motion_index_p2);

    if (data.reset_cam) {
        data.reset_cam = false;

        camera* cam = rctx_ptr->camera;
        cam->reset();

        if (data.field_A9) {
            cam->set_view_point({ 0.0f, 1.0f, 3.45f });
            cam->set_interest({ 0.0f, 1.0f, 0.0f });
            cam->set_view_point({ -3.79f, 0.71f, 1.0f });
            cam->set_interest({ 0.0f, 0.96f, 0.0f });
            cam->set_fov(32.8124985652134);
        }
        else {
            cam->set_view_point({ 0.0f, 1.0f, 3.45f });
            cam->set_interest({ 0.0f, 1.0f, 0.0f });
            cam->set_fov(32.8124985652134);
        }
    }

    if (data.field_A8) {
        data.field_A8 = false;
        //sub_1401F9510(data.field_A9 ? 32.2673416137695 : 32.8125);
    }

    /*if (data.running && dtm_mot_array[0].sub_140291B70() && !v59
        && data.motion_index_p1 < aft_mot_db->motion_set[data.motion_set_index_p1].motion.size()) {
        data.curr_motion_index_p1 = data.motion_index_p1;
        data.motion_index_p1 = data.motion_index_p1 + 1;
        dtm_mot_array[0].SetMotion(data.motion_set_index_p1, data.motion_index_p1);
        dtm_mot_array[1].SetMotion(data.motion_set_index_p2, data.motion_index_p2);
        dtm_mot_array[0].sub_140291F20();
        dtm_mot_array[1].sub_140291F20();

        //data_mot_test_dw_array_get(0)->sub_14028EF90(data.motion_index_p1);
    }*/

    motion_test_objset_check_not_read();
    sub_140286280();
    return false;
}

bool DataTestMot::Dest() {
    clear_color = { 0x00, 0x00, 0x00, 0xFF };
    set_clear_color = true;

    dtm_mot_array[0].DelTask();
    dtm_mot_array[1].DelTask();
    //data_mot_test_dw_array_get(0)->field_D0();
    //data_mot_test_dw_array_get(1)->field_D0();
    //data_test_mot_ctrl_dw_get()->field_D0();
    //data_test_mot_a3d_dw()->field_D0();
    //sub_140263340();
    dtm_eq_vs_array[0].DelTask();
    dtm_eq_vs_array[1].DelTask();
    motion_test_objset_unload();
    //data_mot_test_a3d_get()->DelTask();
    return true;
}

void DataTestMot::Disp() {

}

void DataTestMot::sub_140286280() {
    /*dtm_mot_array[0].sub_1402922C0(true);
    dtm_mot_array[0].sub_1402922C0(true);
    if (this->data.sync_frame && (!dtm_mot_array[0].sub_140291C10() || !dtm_mot_array[1].sub_140291C10())) {
        dtm_mot_array[0].sub_1402922C0(false);
        dtm_mot_array[0].sub_1402922C0(false);
    }*/
}

DtmMot::DtmMot() : rob_bone_data(), field_7C(), field_A8(), pre_offset(), post_offset(), divide(),
loop(), change_motion(), use_opd(), field_D6(), reset_mot(), save_only_start_frame(), state(),
frame(), field_E4(), field_E8(), start_frame(), a_frame(), b_frame(), ab_loop(), field_2C960() {
    chara_id = -1;
    chara_index = CHARA_MAX;
    module_index = 502;
    motion_set_id = -1;
    motion_id = -1;
    step[0] = 0.0f;
    step[1] = 0.0f;
    step[2] = 0.0f;
    step[3] = 1.0f;
    play = true;
    field_D2 = true;
    disp = true;
    partial_mot = true;
    pv_id = -1;
    field_100 = true;
}

DtmMot::~DtmMot() {

}

bool DtmMot::Init() {
    frame = 0.0f;
    field_E4 = 1.0f;
    state = 1;
    field_E8 = 0;

    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX || module_index >= 502) {
        this->state = 0;
        return true;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    if (!field_7C) {
        int32_t motion_set_index = this->motion_set_index;
        int32_t motion_index = this->motion_index;
        if (aft_mot_db->motion_set.size() <= motion_set_index) {
            state = 0;
            return true;
        }

        motion_set_id = aft_mot_db->motion_set[motion_set_index].id;
        if (aft_mot_db->motion_set[motion_set_index].motion.size() <= motion_index) {
            state = 0;
            return true;
        }
        motion_id = aft_mot_db->motion_set[motion_set_index].motion[motion_index].id;
    }

    pv_id = DtmMot::ConvertMotionSetNameToPVID(aft_mot_db->get_motion_set_name(motion_set_id));
    field_D6 = false;
    return true;
}

bool DtmMot::Ctrl() {
    return false;
}

bool DtmMot::Dest() {
    return true;
}

void DtmMot::Basic() {

}

bool DtmMot::AddTask(::chara_index chara_index,
    int32_t module_index, int32_t motion_set_id, int32_t motion_id) {
    return true;
}

bool DtmMot::AddTask(::chara_index chara_index,
    int32_t module_index, int32_t motion_id) {
    return true;
}

bool DtmMot::DelTask() {
    if (app::TaskWork::CheckTaskReady(this))
        return app::Task::DelTask();
    return false;
}

float_t DtmMot::GetFrame() {
    if (state == 13)
        return frame;
    return 0.0f;
}

float_t DtmMot::GetFrameCount() {
    if (state == 13 && rob_bone_data)
        return rob_bone_data->get_frame_count();
    return 0.0f;
}

float_t DtmMot::GetStep() {
    return step[3];
}

void DtmMot::SetChangeMotion() {
    change_motion = true;
}

void DtmMot::SetDisp(bool value) {
    disp = value;
}

void DtmMot::SetDivide(int32_t index, float_t value) {
    if (index < 3)
        divide[index] = value;
}

void DtmMot::SetFrame(float_t value) {
    frame = value;
}

void DtmMot::SetLoop(bool value) {
    loop = value;
}

void DtmMot::SetMotion(uint32_t motion_set_index, uint32_t motion_index) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    state = 0;

    if (motion_set_index >= aft_mot_db->motion_set.size())
        return;

    const motion_set_info* set_info = &aft_mot_db->motion_set[motion_set_index];
    motion_set_id = set_info->id;

    if (motion_index >= set_info->motion.size())
        return;

    state = 7;
    frame = 0.0f;
    motion_id = set_info->motion[motion_index].id;
}

void DtmMot::SetOffset(float_t pre_offset, float_t post_offset) {
    this->pre_offset = pre_offset;
    this->post_offset = post_offset;
}

void DtmMot::SetPlay(bool value) {
    partial_mot = value;
}

void DtmMot::SetPartialMot(bool value) {
    play = value;
}

void DtmMot::SetResetMot() {
    reset_mot = true;
}

void DtmMot::SetRotationY(float_t value) {
    rotatation.y = value;
}

void DtmMot::SetSaveOnlyStartFrame(bool value) {
    save_only_start_frame = value;
}

void DtmMot::SetStartFrame(float_t value) {
    start_frame = value;
}

void DtmMot::SetStep(int32_t index, float_t value) {
    if (index < 4)
        step[index] = value;
}

void DtmMot::SetTrans(const vec3& value) {
    trans = value;
}

void DtmMot::SetUseOpd(bool value) {
    use_opd = value;
}

int32_t DtmMot::ConvertMotionSetNameToPVID(const char* set_name) {
    if (!set_name)
        return -1;

    size_t length = utf8_length(set_name);
    if (length < 5)
        return -1;

    if (memcmp(set_name, "PV", 2))
        return -1;

    if (set_name[2] < '0' || set_name[2] > '9'
        || set_name[3] < '0' || set_name[3] > '9'
        || set_name[4] < '0' || set_name[4] > '9')
        return -1;

    int32_t pv_id = (set_name[2] - '0') * 100;
    pv_id += (set_name[3] - '0') * 10;
    pv_id += set_name[4] - '0';
    return pv_id;
}

DtmEqVs::DtmEqVs() : chara_id(), chara_index(), curr_chara_index(),
module_index(), curr_module_index(), field_80() {
    state = -1;
}

DtmEqVs::~DtmEqVs() {

}

bool DtmEqVs::Init() {
    state = 0;
    return true;
}

bool DtmEqVs::Ctrl() {
    rob_chara_item_cos_data* item_cos_data = rob_chara_array_get_item_cos_data(chara_id);
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    switch (state) {
    case 0:
        //data_test_equip_dw_sub_14025FCC0(this->chara_id);
        //data_test_equip_dw_sub_14025FA80(this->chara_id, item_cos_data->get_cos());
        state = 3;
    case 3:
        if (false /*dtm_mot_array_sub_140287740()*/) {
            sub_140262050();
            sub_140261FF0();

            int32_t item_no = ITEM_NONE;//data_test_equip_dw_sub_14025FB20(this->chara_id);
            ::item_cos_data temp_cos;
            if (rob_chr)
                temp_cos = *rob_chr->item_cos_data.get_cos();

            bool v2 = false;
            /*if (data_test_equip_dw_sub_14025FAF0(chara_id)) {
                item_cos_data->set_chara_index_item(chara_index, item_no);
                v2 = true;
            }*/

            /*if (data_test_equip_dw_sub_14025FB80(chara_id)) {
                item_cos_data->set_chara_index_item_zero(chara_index, item_no);
                v2 = true;
            }*/

            const ::item_cos_data* cos = 0;//data_test_equip_dw_get_cos(chara_id);
            if (cos) {
                item_cos_data->set_chara_index_item_nos(chara_index, cos->arr);
                v2 = true;
            }

            /*if (sub_1402CEEC0(chara_id)) {
                v2 = true;
                sub_1402CF8F0();
            }*/

            //data_test_equip_dw_sub_14025FBE0(chara_id);

            if (v2) {
                if (rob_chr)
                    task_rob_load_append_free_req_data_obj(rob_chr->chara_index, &temp_cos);
                rob_chara_array_reset_bone_data_item_equip(chara_id);
                task_rob_manager_hide_task();
                state = 1;
            }
        }
        break;
    case 1:
        if (rob_chr->chara_index) {
            task_rob_load_append_load_req_data_obj(rob_chr->chara_index, rob_chr->item_cos_data.get_cos());
            state = 2;
        }
        break;
    case 2:
        if (!task_rob_load_check_load_req_data()) {
            task_rob_manager_run_task();
            if (rob_chara_array_get(chara_id)) {
                data_struct* aft_data = &data_list[DATA_AFT];
                bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
                object_database* aft_obj_db = &aft_data->data_ft.obj_db;
                item_cos_data->reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);
            }
            auth_3d_test_task_disp_chara();
            dtm_mot_array_set_reset_mot();
            this->state = 3;
        }
        break;
    }
    return false;
}

bool DtmEqVs::Dest() {
    return true;
}

bool DtmEqVs::AddTask(int32_t chara_id, ::chara_index chara_index) {
    this->chara_id = chara_id;
    this->chara_index = chara_index;
    this->curr_chara_index = chara_index;
    module_index = 0;
    curr_module_index = 0;
    this->field_80 = 0;
    //data_test_equip_dw_set_chara_index(chara_id, chara_index);
    //data_test_equip_dw_set_module_index(chara_id, module_index);
    return app::TaskWork::AddTask(this, "DATA TEST EQUIP MANAGER FOR VS");
    return true;
}

bool DtmEqVs::DelTask() {
    return app::Task::DelTask();
}

void DtmEqVs::SetCharaIndexModuleIndex(::chara_index chara_index, int32_t module_index) {
    this->chara_index = chara_index;
    this->module_index = module_index;
    //data_test_equip_dw_set_chara_index(chara_id, chara_index);
    //data_test_equip_dw_set_module_index(chara_id, module_index);
}

void DtmEqVs::sub_140261FF0() {
    if (chara_index == curr_chara_index && module_index == curr_module_index)
        return;

    curr_chara_index = chara_index;
    curr_module_index = module_index;
    const item_cos_data* cos = item_table_handler_array_get_item_cos_data_by_module_index(chara_index, module_index);
    if (!check_module_index_is_501(curr_module_index) || !cos) {
        //data_test_equip_dw_sub_14025FCC0(chara_id);
        //data_test_equip_dw_sub_14025FA80(chara_id, cos);
    }
}

void DtmEqVs::sub_140262050() {
    return;

    int32_t v2 = 0;//data_test_equip_dw_sub_14025FAC0(chara_id);
    /*if (v2 == field_80)
        return;*/

    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    bool disp = false;
    item_id id;
    switch (v2) {
    default:
        id = ITEM_NONE;
        disp = true;
        break;
    case 1:
        id = ITEM_NONE;
        break;
    case 2:
        id = ITEM_OUTER;
        break;
    case 3:
        id = ITEM_PANTS;
        break;
    }

    for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++)
        if (rob_itm_equip->get_object_info((item_id)i).not_null())
            rob_itm_equip->set_disp((item_id)i, true);

    for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++)
        if (i != id && (!disp || i != rob_itm_equip->field_D4))
            rob_itm_equip->set_disp((item_id)i, disp);

    field_80 = v2;
}

void motion_test_init() {
    if (!data_test_mot)
        data_test_mot = new DataTestMot;

    if (!dtm_mot_array)
        dtm_mot_array = new DtmMot[2];

    if (!dtm_eq_vs_array)
        dtm_eq_vs_array = new DtmEqVs[2];

    motion_test_objset = {};
}

void motion_test_free() {
    motion_test_objset.clear();
    motion_test_objset.shrink_to_fit();

    if (dtm_eq_vs_array) {
        delete[] dtm_eq_vs_array;
        dtm_eq_vs_array = 0;
    }

    if (dtm_mot_array) {
        delete[] dtm_mot_array;
        dtm_mot_array = 0;
    }

    if (data_test_mot) {
        delete data_test_mot;
        data_test_mot = 0;
    }
}

static void dtm_mot_array_set_reset_mot() {
    dtm_mot_array[0].SetResetMot();
    dtm_mot_array[1].SetResetMot();
}

static bool motion_test_objset_check_not_read() {
    for (uint32_t i : motion_test_objset)
        if (object_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

static void motion_test_objset_load() {
    motion_test_objset.clear();

    auto& hand_items = hand_item_handler_data_get_hand_items();
    for (auto& i : hand_items) {
        const hand_item& hnd_itm = i.second;
        if (hnd_itm.obj_left.not_null())
            motion_test_objset.push_back(hnd_itm.obj_left.set_id);
        if (hnd_itm.obj_right.not_null())
            motion_test_objset.push_back(hnd_itm.obj_right.set_id);
    }

    prj::sort_unique(motion_test_objset);

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    for (uint32_t i : motion_test_objset)
        object_storage_load_set(aft_data, aft_obj_db, i);
}

static void motion_test_objset_unload() {
    for (uint32_t i : motion_test_objset)
        object_storage_unload_set(i);
    motion_test_objset.clear();
}
