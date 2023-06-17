/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion_test.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/rob/skin_param.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/hand_item.hpp"
#include "../../CRE/module_table.hpp"
#include "../../CRE/pv_db.hpp"
#include "../../CRE/resolution_mode.hpp"
#include "../../CRE/stage.hpp"
#include "auth_3d_test.hpp"
#include "equip_test.hpp"
#include "stage_test.hpp"

extern int32_t width;
extern int32_t height;
extern render_context* rctx_ptr;

DataTestMot* data_test_mot;
DataTestMotA3d* data_test_mot_a3d;

DtmMot* dtm_mot_array;

DataTestMotDw* data_test_mot_dw_array[ROB_CHARA_COUNT];
DataTestMotA3dDw* data_test_mot_a3d_dw;
DataTestMotCtrlDw* data_test_mot_ctrl_dw;
DataTestFaceMotDw* data_test_face_mot_dw_array[ROB_CHARA_COUNT];

std::vector<uint32_t> motion_test_objset;

static DataTestMot::Data* data_test_mot_data_get();

static bool data_test_mot_a3d_add_task();
static bool data_test_mot_a3d_del_task();
static const char* data_test_mot_a3d_get_state_text();
static DataTestMotA3d* data_test_mot_a3d_get();
static void data_test_mot_a3d_set_auth_3d(std::string& value);

static void data_test_mot_dw_array_init(int32_t chara_id, DtmMot* dtm_mot);
static DataTestMotDw* data_test_mot_dw_array_get(int32_t chara_id);

static void data_test_mot_a3d_dw_init();
static DataTestMotA3dDw* data_test_mot_a3d_dw_get();

static void data_test_mot_ctrl_dw_init();
static DataTestMotCtrlDw* data_test_mot_ctrl_dw_get();

static void data_test_face_mot_dw_array_init(int32_t chara_id);
static DataTestFaceMotDw* data_test_face_mot_dw_array_get(int32_t chara_id);
static void data_test_face_mot_dw_array_unload();

static bool motion_test_objset_check_not_read();
static void motion_test_objset_load();
static void motion_test_objset_unload();

DataTestMot::Data::Offset::Offset() {
    pre = 0.0f;
    post = 0.0f;
}

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

DataTestMot::Data::Data() : chara_index(), curr_chara_index(), module_index(), curr_module_index(),
motion_set_index(), curr_motion_set_index(), motion_index(), curr_motion_index(), rot_y(),
trans_x(), offset(), start_frame(), type(), reset_mot(), reset_cam(), reload_data(),
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

    data_test_mot_dw_array_init(0, &dtm_mot_array[0]);
    data_test_mot_dw_array_init(1, &dtm_mot_array[1]);
    data_test_mot_ctrl_dw_init();
    data_test_mot_a3d_dw_init();
    data.reset_cam = true;
    data.field_A8 = true;
    //dtm_eq_vs_array[0].AddTask(0, data.chara_index[0]);
    //dtm_eq_vs_array[1].AddTask(1, data.chara_index[1]);
    motion_test_objset_load();
    return true;
}

bool DataTestMot::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    //data_test_mot_dw_array_get(0)->sub_14028D8B0();

    bool v3 = false;
    bool v2 = false;

    if (data.reload_data) {
        data.reload_data = false;
        v3 = true;
        v2 = true;
    }

    if (data.chara_index[0] != data.curr_chara_index[0]) {
        data.curr_chara_index[0] = data.chara_index[0];
        //dtm_eq_vs_array[0].SetCharaIndexModuleIndex(data.chara_index[0], data.module_index[0]);
        v3 = true;
        v2 = true;
    }

    if (data.chara_index[1] != data.curr_chara_index[1]) {
        data.curr_chara_index[1] = data.chara_index[1];
        //dtm_eq_vs_array[1].SetCharaIndexModuleIndex(data.chara_index[1], data.module_index[1]);
        v3 = true;
        v2 = true;
    }

    if (data.module_index[0] != data.curr_module_index[0]) {
        data.curr_module_index[0] = data.module_index[0];
        //dtm_eq_vs_array[0].SetCharaIndexModuleIndex(data.chara_index[0], data.module_index[0]);
        v3 = true;
        v2 = true;
    }

    if (data.module_index[1] != data.curr_module_index[1]) {
        data.curr_module_index[1] = data.module_index[1];
        //dtm_eq_vs_array[1].SetCharaIndexModuleIndex(data.chara_index[1], data.module_index[1]);
        v3 = true;
        v2 = true;
    }

    if (data.motion_set_index[0] != data.curr_motion_set_index[0]) {
        data.curr_motion_set_index[0] = data.motion_set_index[0];

        data_test_mot_dw_array_get(0)->ClearIDs();

        const motion_set_info& set_info = aft_mot_db->motion_set[data.motion_set_index[0]];
        for (const motion_info& i : set_info.motion)
            data_test_mot_dw_array_get(0)->AddID(i.name.c_str());

        data_test_mot_dw_array_get(0)->ResetIDListBoxIndex();

        data.curr_motion_index[0] = 0;
        data.motion_index[0] = 0;
        v3 = true;
        v2 = true;
    }

    if (data.motion_set_index[1] != data.curr_motion_set_index[1]) {
        data.curr_motion_set_index[1] = data.motion_set_index[1];

        data_test_mot_dw_array_get(1)->ClearIDs();

        const motion_set_info& set_info = aft_mot_db->motion_set[data.motion_set_index[0]];
        for (const motion_info& i : set_info.motion)
            data_test_mot_dw_array_get(1)->AddID(i.name.c_str());

        data_test_mot_dw_array_get(1)->ResetIDListBoxIndex();

        data.curr_motion_index[1] = 0;
        data.motion_index[1] = 0;

        v3 = true;
        v2 = true;
    }

    bool frame_comp = false;
    if (data.motion_index[0] != data.curr_motion_index[0]) {
        data.curr_motion_index[0] = data.motion_index[0];
        dtm_mot_array[0].SetMotion(data.motion_set_index[0], data.motion_index[0]);
        dtm_mot_array[1].SetMotion(data.motion_set_index[1], data.motion_index[1]);
        dtm_mot_array[0].SetChangeMotion();
        dtm_mot_array[1].SetChangeMotion();
        frame_comp = true;
    }

    if (data.motion_index[1] != data.curr_motion_index[1]) {
        data.curr_motion_index[1] = data.motion_index[1];
        dtm_mot_array[0].SetMotion(data.motion_set_index[0], data.motion_index[0]);
        dtm_mot_array[1].SetMotion(data.motion_set_index[1], data.motion_index[1]);
        dtm_mot_array[0].SetChangeMotion();
        dtm_mot_array[1].SetChangeMotion();
    }

    dtm_mot_array[0].SetRotationY(data.rot_y[0]);
    dtm_mot_array[1].SetRotationY(data.rot_y[1]);
    dtm_mot_array[0].SetTrans({ data.trans_x[0], 0.0f, 0.0f });
    dtm_mot_array[1].SetTrans({ data.trans_x[1], 0.0f, 0.0f });
    dtm_mot_array[0].SetOffset(data.offset[0].pre, data.offset[0].post);
    dtm_mot_array[1].SetOffset(data.offset[1].pre, data.offset[1].post);
    dtm_mot_array[0].SetStartFrame(data.start_frame[0]);
    dtm_mot_array[1].SetStartFrame(data.start_frame[1]);

    for (int32_t i = 0; i < 3; i++) {
        dtm_mot_array[0].SetDivide(i, data.divide[0].array[i]);
        dtm_mot_array[1].SetDivide(i, data.divide[1].array[i]);
    }

    for (int32_t i = 0; i < 4; i++) {
        dtm_mot_array[0].SetStep(i, data.step[0].array[i]);
        dtm_mot_array[1].SetStep(i, data.step[1].array[i]);
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
        dtm_mot_array[0].AddTask(data.chara_index[0],
            data.module_index[0], data.motion_set_index[0], data.motion_index[0]);

    if (v2)
        dtm_mot_array[1].DelTask();
    else
        dtm_mot_array[1].AddTask(data.chara_index[1],
            data.module_index[1], data.motion_set_index[1], data.motion_index[1]);

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
        //rctx_ptr->camera->set_fov(data.field_A9 ? 32.2673416137695 : 32.8125);
    }

    if (data.running && dtm_mot_array[0].CheckFirstFrame() && !frame_comp
        && data.motion_index[0] < aft_mot_db->motion_set[data.motion_set_index[0]].motion.size()) {
        data.curr_motion_index[0] = data.motion_index[0];
        data.motion_index[0] = data.motion_index[0] + 1;
        dtm_mot_array[0].SetMotion(data.motion_set_index[0], data.motion_index[0]);
        dtm_mot_array[1].SetMotion(data.motion_set_index[1], data.motion_index[1]);
        dtm_mot_array[0].SetChangeMotion();
        dtm_mot_array[1].SetChangeMotion();

        data_test_mot_dw_array_get(0)->SetIDListBoxIndex(data.motion_index[0]);
    }

    motion_test_objset_check_not_read();
    sub_140286280();
    return false;
}

bool DataTestMot::Dest() {
    clear_color = { 0x00, 0x00, 0x00, 0xFF };
    set_clear_color = true;

    dtm_mot_array[0].DelTask();
    dtm_mot_array[1].DelTask();
    data_test_mot_dw_array_get(0)->Hide();
    data_test_mot_dw_array_get(1)->Hide();
    data_test_mot_ctrl_dw_get()->Hide();
    data_test_mot_a3d_dw_get()->Hide();
    data_test_face_mot_dw_array_unload();
    //dtm_eq_vs_array[0].DelTask();
    //dtm_eq_vs_array[1].DelTask();
    motion_test_objset_unload();
    data_test_mot_a3d_get()->DelTask();
    return true;
}

void DataTestMot::Disp() {

}

void DataTestMot::sub_140286280() {
    dtm_mot_array[0].sub_1402922C0(true);
    dtm_mot_array[0].sub_1402922C0(true);
    if (data.sync_frame && (!dtm_mot_array[0].sub_140291C10() || !dtm_mot_array[1].sub_140291C10())) {
        dtm_mot_array[0].sub_1402922C0(false);
        dtm_mot_array[0].sub_1402922C0(false);
    }
}

DataTestMotA3d::DataTestMotA3d() : state(), camera() {

}

DataTestMotA3d::~DataTestMotA3d() {

}

bool DataTestMotA3d::Init() {
    Reset();
    auth_3d.clear();
    state = 0;
    return true;
}

bool DataTestMotA3d::Ctrl() {
    switch (state) {
    case 0:
        state = 1;
        break;
    case 1:
        Sync1pFrame();
        break;
    case 2: {
        Reset();

        LoadAuth3d(std::string(auth_3d));

        data_struct* aft_data = &data_list[DATA_AFT];
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;

        for (uint32_t& i : obj_sets)
            object_storage_load_set(aft_data, aft_obj_db, i);

        state = 3;
    } break;
    case 3: {
        bool wait_load = false;

        for (uint32_t& i : obj_sets)
            if (object_storage_load_obj_set_check_not_read(i))
                return false;

        if (wait_load)
            break;

        for (std::string& i : categories)
            auth_3d_data_load_category(i.c_str());

        state = 4;
    } break;
    case 4: {
        bool wait_load = false;

        for (std::string& i : categories)
            if (!auth_3d_data_check_category_loaded(i.c_str()))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

        for (auth_3d_id& i : auth_3d_ids)
            i.read_file(aft_auth_3d_db);

        state = 5;
    } break;
    case 5: {
        bool wait_load = false;

        for (auth_3d_id& i : auth_3d_ids)
            if (!i.check_loaded())
                wait_load |= true;

        if (wait_load)
            break;

        for (auth_3d_id& i : auth_3d_ids)
            i.set_enable(true);

        state = 1;
    } break;
    case 6:
        return true;
    }
    return false;
}

bool DataTestMotA3d::Dest() {
    Reset();
    camera = false;
    return true;
}

void DataTestMotA3d::Disp() {

}

void DataTestMotA3d::Basic() {

}

const char* DataTestMotA3d::GetStateText() {
    switch (state) {
    case 0:
        return "A3D: 初期化中...";
    case 1:
        return "A3D: 命令待機中...";
    case 2:
        return "A3D: データリクエスト中...";
    case 3:
        return "A3D: PVステージ読込中...";
    case 4:
        return "A3D: A3Dカテゴリ読込中...";
    case 5:
        return "A3D: A3Dハンドル読込中...";
    default:
        return "A3D: 未知のモード";
    }
}

void DataTestMotA3d::LoadAuth3d(std::string&& name) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    int32_t uid = aft_auth_3d_db->get_uid(name.c_str());
    if (uid == -1)
        return;

    auth_3d_id id = auth_3d_data_load_uid(uid, aft_auth_3d_db);
    if (!id.check_not_empty())
        return;

    id.set_enable(false);
    id.set_repeat(true);

    auth_3d_ids.push_back(id);

    if (!name.find("CAM")) {
        if (!camera)
            camera = true;
    }
    else {
        if (camera)
            camera = false;
    }

    std::string category(aft_auth_3d_db->uid[uid].category.c_str());
    categories.push_back(category);
    auth_3d_data_get_obj_sets_from_category(category, obj_sets, aft_auth_3d_db, aft_obj_db);
}

void DataTestMotA3d::Reset() {
    for (std::string& i : categories)
        auth_3d_data_unload_category(i.c_str());

    categories.clear();

    for (auth_3d_id& i : auth_3d_ids)
        i.unload(rctx_ptr);

    auth_3d_ids.clear();
    
    for (uint32_t& i : obj_sets)
        object_storage_unload_set(i);

    obj_sets.clear();
}

void DataTestMotA3d::SetAuth3d(std::string& value) {
    if (state != 1)
        return;

    auth_3d.assign(value);
    state = 2;
}

void DataTestMotA3d::Sync1pFrame() {
    if (!data_test_mot_data_get()->sync_1p_frame)
        return;

    float_t frame = dtm_mot_array->GetFrame();

    for (auth_3d_id& i : auth_3d_ids)
        i.set_req_frame(frame);
}

DtmMot::DtmMot() : rob_bone_data(), field_7C(), rot_y(), pre_offset(), post_offset(), divide(),
loop(), change_motion(), use_opd(), field_D6(), reset_mot(), save_only_start_frame(), state(),
frame(), delta_frame(), looped(), start_frame(), ab_frame(), ab_loop(), set_motion_index() {
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
    delta_frame = 1.0f;
    state = 1;
    looped = 0;

    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX || module_index >= 502) {
        state = 0;
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

bool DtmMot::sub_140291C10() {
    if (state)
        return field_D6;
    return true;
}

void DtmMot::sub_1402922C0(bool value) {
    this->field_100 = value;
}

bool DtmMot::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    switch (state) {
    case 1: {
        if (pv_id >= 0) {
            const pv_db_pv_difficulty* diff = task_pv_db_get_pv_difficulty(pv_id, PV_DIFFICULTY_HARD, 0);
            if (diff) {
                dsc_file_handler.reset();
                dsc_file_handler.read_file(aft_data, diff->script_file_name.c_str());
            }
        }

        motion_set_load_motion(motion_set_id, "", aft_mot_db);
        motion_set_load_mothead(motion_set_id, "", aft_mot_db);
        state = 2;
    } break;
    case 2: {
        if (motion_storage_check_mot_file_not_ready(motion_set_id)
            || mothead_storage_check_mhd_file_not_ready(motion_set_id)
            || dsc_file_handler.check_not_ready())
            break;

        state = 3;
    } break;
    case 3: {
        if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT && task_rob_manager_get_wait(chara_id))
            break;

        rob_chara_pv_data pv_data;
        pv_data.type = ROB_CHARA_TYPE_3;

        module_data data;
        if (module_data_handler_data_get_module(chara_index, module_index, data)) {
            pv_data.sleeve_l = data.sleeve_l;
            pv_data.sleeve_r = data.sleeve_r;
        }

        chara_id = rob_chara_array_init_chara_index(chara_index, pv_data, module_index, true);
        state = 4;
    } break;
    case 4: {
        if (!task_rob_manager_check_chara_loaded(chara_id))
            break;

        rob_bone_data = rob_chara_array_get_bone_data(chara_id);
        state = 7;
    } break;
    case 5: {
        if (osage_play_data_manager_check_task_ready())
            break;

        osage_play_data_manager_append_chara_motion_id(rob_chara_array_get(chara_id), motion_id);
        osage_play_data_manager_add_task();
        state = 6;
    } break;
    case 6: {
        if (osage_play_data_manager_check_task_ready() || skin_param_manager_check_task_ready(chara_id))
            break;

        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        rob_chr->set_motion_id(motion_id, frame, 0.0f, true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chr->set_use_opd(true);
        state = 11;
    } break;
    case 7: {
        field_D6 = false;
        if (skin_param_manager_check_task_ready(chara_id)
            || pv_osage_manager_array_get_disp(chara_id)
            || osage_play_data_manager_check_task_ready())
            break;

        const pv_db_pv_difficulty* diff = task_pv_db_get_pv_difficulty(pv_id, PV_DIFFICULTY_HARD, 0);
        if (pv_id > 0 && diff && dsc_file_handler.get_data()) {
            if (diff) {
                pv_data.reset();
                pv_data.dsc.parse(dsc_file_handler.get_data(), dsc_file_handler.get_size(), DSC_FT);

                dsc_file_handler.reset();

                int32_t chara_id = 0;
                for (auto& i : diff->motion)
                    for (const pv_db_pv_motion& j : i)
                        if (j.id == motion_id) {
                            chara_id = (int32_t)(&i - diff->motion);
                            break;
                        }

                pv_data.find_playdata_set_motion(chara_id);
                pv_data.find_set_motion(diff);
                const std::vector<pv_data_set_motion>* set_motion = pv_data.get_set_motion(chara_id);
                if (set_motion) {
                    this->set_motion.assign(set_motion->begin(), set_motion->end());
                    const mothead_mot* mhdm = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db);
                    if (mhdm)
                        for (const mothead_data& i : mhdm->data) {
                            if (i.type != MOTHEAD_DATA_WIND_RESET)
                                continue;

                            auto j_begin = this->set_motion.begin();
                            auto j_end = this->set_motion.end();
                            for (auto j = j_begin; j != j_end; )
                                if ((float_t)i.frame == j->frame_stage_index.first) {
                                    std::move(j + 1, j_end, j);
                                    this->set_motion.pop_back();
                                    j_end = this->set_motion.end();
                                }
                                else
                                    j++;
                        }
                }
            }
        }

        task_rob_manager_hide_task();
        task_wind_hide_task();
        reset_mot = false;
        state = 8;
    } break;
    case 8: {
        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        rob_chr->set_use_opd(false);

        skin_param_manager_reset(chara_id);

        std::vector<osage_init_data> vec;
        vec.push_back({ rob_chr });
        vec.push_back({ rob_chr, pv_id, motion_id });

        const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
        if (pv) {
            for (const pv_db_pv_osage_init& i : pv->osage_init) {
                uint32_t mot_id = aft_mot_db->get_motion_id(i.motion.c_str());
                if (mot_id != -1 && mot_id == motion_id)
                    vec.push_back({ rob_chr, pv_id, mot_id, "", i.frame });
            }

            for (pv_data_set_motion& i : set_motion)
                vec.push_back({ rob_chr, pv->id, i.motion_id, "", (int32_t)roundf(i.frame_stage_index.first) });

        }
        skin_param_manager_add_task(chara_id, vec);

        state = 9;

        bool has_opd = false;
        const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
        while (*opd_motion_set_ids != -1) {
            if (*opd_motion_set_ids != motion_set_id) {
                opd_motion_set_ids++;
                continue;
            }

            if (use_opd)
                state = 5;
            break;
        }

    } break;
    case 9: {
        if (skin_param_manager_check_task_ready(chara_id))
            break;

        pv_osage_manager_array_reset(chara_id);
        if (save_only_start_frame) {
            pv_data_set_motion motion;
            motion.motion_id = motion_id;
            motion.frame_stage_index.first = start_frame;
            motion.frame_stage_index.second = task_stage_get_current_stage_index();

            std::vector<pv_data_set_motion> set_motion;
            set_motion.push_back(motion);
            pv_osage_manager_array_set_pv_set_motion(chara_id, set_motion);
        }
        else {
            std::vector<uint32_t> motion_ids;
            motion_ids.push_back(motion_id);
            pv_osage_manager_array_set_motion_ids(chara_id, motion_ids);

            if (pv_id >= 0)
                pv_osage_manager_array_set_pv_set_motion(chara_id, set_motion);
        }

        /*if (pv_id >= 0) {
            stage_param_data_coli_data_clear();
            stage_param_data_coli_data_load(pv_id);
        }*/

        pv_osage_manager_array_set_pv_id(chara_id, pv_id, 0);
        state = 10;
    } break;
    case 10: {
        if (pv_osage_manager_array_get_disp())
            break;

        /*int32_t stage_index = task_stage_get_current_stage_index();
        if (stage_index != -1)
            rob_chara_array_get(chara_id)->set_stage_data_ring(stage_index);*/
        state = 11;
    } break;
    case 11: {
        field_D6 = true;
        if (!field_100)
            break;

        task_rob_manager_run_task();
        task_wind_run_task();
        state = 12;
    } break;
    case 12: {
        frame = start_frame;

        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        rob_chr->set_step_motion_step(1.0f);
        rob_chr->set_motion_id(motion_id, frame, 0.0f, true, true, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);

        set_motion_index = 0;

        if (data_test_face_mot_dw_array_get(chara_id))
            data_test_face_mot_dw_array_get(chara_id)->Reset();
        state = 13;
    }
    case 13: {
        if (reset_mot) {
            state = 7;
            break;
        }

        rob_chara* rob_chr = rob_chara_array_get(chara_id);

        float_t frame = 0.0f;
        if (frame >= pre_offset)
            frame = min_def(rob_chr->data.field_1588.field_0.frame, frame - pre_offset);

        rob_bone_data->sub_14041DBA0();

        rob_chr->set_frame(frame);
        rob_chr->set_step_motion_step(delta_frame);

        if (looped) {
            rob_chr->sub_140551000();
            set_motion_index = 0;
        }

        rob_chr->sub_1405070E0(aft_bone_data, aft_mot_db);
        rob_chr->arm_adjust_ctrl();

        if (!partial_mot) {
            rob_bone_data->load_face_motion(-1, aft_mot_db);
            rob_bone_data->load_hand_l_motion(-1, aft_mot_db);
            rob_bone_data->load_hand_r_motion(-1, aft_mot_db);
            rob_bone_data->load_mouth_motion(-1, aft_mot_db);
            rob_bone_data->load_eyes_motion(-1, aft_mot_db);
            rob_bone_data->load_eyelid_motion(-1, aft_mot_db);
        }

        CtrlFaceMot();

        rob_bone_data->interpolate();

        mat4 mat;
        mat4_rotate_zyx(&rotation, &mat);
        mat4_rotate_y_mult(&mat, rot_y, &mat);
        mat4_set_translation(&mat, &trans);
        rob_bone_data->update(&mat);

        rob_chr->adjust_ctrl();
        rob_chr->set_data_adjust_mat(&rob_chr->data.adjust_data);
        rob_chr->rob_motion_modifier_ctrl();
        rob_chr->sub_140509D30();

        if (data_test_mot_dw_array_get(chara_id)) {
            data_test_mot_dw_array_get(chara_id)->SetFrameSlider(GetFrame(), GetFrameCount() + pre_offset + post_offset);
            data_test_mot_dw_array_get(chara_id)->SetFrameLabel(GetFrame(), GetFrameCount());
        }

        if (change_motion)
            change_motion = false;

        if (rob_chara_array_check_visibility(chara_id) != disp)
            rob_chara_array_set_visibility(chara_id, disp);

        rob_chr->rob_info_ctrl();

        while (set_motion_index < set_motion.size()) {
            if (set_motion.data()[set_motion_index].motion_id != motion_id) {
                set_motion_index++;
                continue;
            }

            float_t frame = (float_t)(int32_t)roundf(set_motion.data()[set_motion_index].frame_stage_index.first);
            if (frame > this->frame)
                break;

            rob_chr->set_motion_reset_data(motion_id, frame);
            rob_chr->set_motion_skin_param(motion_id, frame);
            set_motion_index++;
        }
    } break;
    }
    return false;
}

bool DtmMot::Dest() {
    if (!state)
        return true;

    pv_osage_manager_array_set_not_reset_true();
    if (skin_param_manager_check_task_ready(chara_id)
        || pv_osage_manager_array_get_disp()
        || osage_play_data_manager_check_task_ready())
        return false;

    task_rob_manager_run_task();
    task_wind_run_task();
    if (rob_chara_array_get(chara_id))
        rob_chara_array_free_chara_id(chara_id);
    motion_set_unload_motion(motion_set_id);
    motion_set_unload_mothead(motion_set_id);
    skin_param_manager_reset(chara_id);
    //stage_param_data_coli_data_clear();
    osage_play_data_manager_reset();
    set_motion.clear();
    set_motion_index = 0;
    dsc_file_handler.reset();
    pv_data.reset();
    rob_bone_data = 0;
    state = 1;
    return true;
}

void DtmMot::Basic() {
    if (state != 13)
        return;

    int32_t step_index = 3;
    for (int32_t i = 0; i < 4; i++)
        if (frame >= divide[i]) {
            step_index = i;
            break;
        }

    float_t frame = this->frame;

    delta_frame = get_delta_frame() * step[step_index];
    looped = false;

    if (play)
        frame += delta_frame;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);

    float_t begin = 0.0f;
    float_t end = rob_chr->data.field_1588.field_0.frame;
    if (ab_loop) {
        begin = ab_frame[0];
        end = ab_frame[1];
    }

    end += pre_offset + post_offset;
    if (frame < begin)
        if (loop || ab_loop)
            frame += end;
        else
            frame = begin;

    if (frame >= end) {
        if (ab_loop) {
            looped = true;
            frame = begin;
        }
        else if (loop) {
            looped = true;
            frame -= end;
            frame += rob_chr->data.field_1588.field_0.loop_begin;
        }
        else
            frame = end;
    }

    this->frame = frame;
}

bool DtmMot::AddTask(::chara_index chara_index,
    int32_t module_index, uint32_t motion_set_id, uint32_t motion_id) {
    return true;
}

bool DtmMot::AddTask(::chara_index chara_index,
    int32_t module_index, uint32_t motion_id) {
    return true;
}

bool DtmMot::DelTask() {
    if (app::TaskWork::CheckTaskReady(this))
        return app::Task::DelTask();
    return false;
}

bool DtmMot::CheckFirstFrame() {
    if (rob_bone_data)
        return false;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return false;

    float_t frame_count = rob_chr->data.field_1588.field_0.frame + pre_offset + post_offset;
    if (frame_count > 0.0f)
        return fabsf(frame_count - frame) <= 0.000001f;
    return false;
}

void DtmMot::CtrlFaceMot() {
    DataTestFaceMotDw* face_mot_dw = data_test_face_mot_dw_array_get(chara_id);
    if (!face_mot_dw)
        return;

    if (!face_mot_dw->GetEnable())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    DataTestFaceMotDw::Data data = face_mot_dw->GetData();
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    rob_chr->set_face_mottbl_motion(0, data.face.mottbl_index, data.face.frame * 0.01f,
        0, 0.0f, 0.0f, 1.0f, -1, 0.0f, false, aft_mot_db);
    rob_chr->set_eyelid_mottbl_motion(0, data.eyelid.mottbl_index, data.eyelid.frame * 0.01f,
        0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    rob_chr->set_eyes_mottbl_motion(0, data.eyes.mottbl_index, data.eyes.frame * 0.01f,
        0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    rob_chr->set_mouth_mottbl_motion(0, data.mouth.mottbl_index, data.mouth.frame * 0.01f,
        0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
}

void DtmMot::GetABLoop(bool& ab_loop, float_t& a_frame, float_t& b_frame) {
    ab_loop = this->ab_loop;
    a_frame = ab_frame[0];
    b_frame = ab_frame[1];
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

void DtmMot::ResetABLoop() {
    ab_loop = false;
    ab_frame[0] = 0.0f;
    ab_frame[1] = 0.0f;
}

void DtmMot::SetABFrame(bool a) {
    if (a)
        ab_frame[0] = frame;
    else
        ab_frame[1] = frame;

    UpdateABFrame();
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
    rotation.y = value;
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

void DtmMot::ToggleABLoop() {
    if (!ab_loop)
        UpdateABFrame();

    ab_loop = !ab_loop;
}

void DtmMot::UpdateABFrame() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    float_t v3 = rob_chr->data.field_1588.field_0.frame;
    for (float_t& i : ab_frame)
        i = clamp_def(i, 0.0f, v3);

    if (ab_frame[0] < ab_frame[1])
        return;

    bool found = false;
    float_t a_frame = ab_frame[0];
    int32_t b_frame = (int32_t)v3;

    for (pv_data_set_motion& i : set_motion)
        if (i.motion_id == motion_id && i.frame_stage_index.first > a_frame) {
            if ((int32_t)i.frame_stage_index.first) {
                b_frame = (int32_t)i.frame_stage_index.first;
                found = true;
            }
            break;
        }

    if (!found)
        b_frame = (int32_t)v3;

    ab_frame[1] = (float_t)b_frame;
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

DataTestFaceMotDw::DataTestFaceMotDw(int32_t chara_id) {
    this->chara_id = chara_id;

    SetText("Face motion");

    enable = new dw::Button(this, dw::CHECKBOX);
    enable->SetText("Enable");
    enable->SetValue(false);

    dw::Group* motion_group = new dw::Group(this);
    motion_group->SetLayout(new dw::GridLayout(3));
    motion_group->SetText("Motion");

    (new dw::Label(motion_group))->SetText("FACE  ");

    face = InitAddMottblMapMotions(motion_group, 0);

    face_frame = dw::Slider::make(motion_group);
    face_frame->SetText("Frame  ");
    face_frame->format = "%4.0f";
    face_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("FACE_CL  ");

    face_cl = new dw::Label(motion_group);
    face_cl->SetText("");

    face_cl_frame = dw::Slider::make(motion_group);
    face_cl_frame->SetText("Frame");
    face_cl_frame->format = "%4.0f";
    face_cl_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("EYES  ");

    eyes = InitAddMottblMapMotions(motion_group, 1);

    eyes_frame = dw::Slider::make(motion_group);
    eyes_frame->SetText("Frame");
    eyes_frame->format = "%4.0f";
    eyes_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("MOUTH ");

    mouth = InitAddMottblMapMotions(motion_group, 2);

    mouth_frame = dw::Slider::make(motion_group);
    mouth_frame->SetText("Frame");
    mouth_frame->format = "%4.0f";
    mouth_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    GetSetSize();
}

DataTestFaceMotDw::~DataTestFaceMotDw() {

}

void DataTestFaceMotDw::Hide() {
    data_test_face_mot_dw_array[chara_id] = 0;
    dw::Shell::Hide();
}

void DataTestFaceMotDw::AddMottblMapMotions(dw::ListBox* list_box, int32_t type) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    size_t selected_item = list_box->list->selected_item;
    if (selected_item >= list_box->list->items.size())
        selected_item = 0;

    list_box->ClearItems();

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    switch (type) {
    case 0:
        for (int32_t i = 6; i <= 130; i++) {
            uint32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(i);
            if (motion_id != -1) {
                if (motion_id_mottbl_map.insert({ motion_id, i }).second) {
                    std::string motion_name(aft_mot_db->get_motion_name(motion_id));
                    if (motion_name.find("_CL") != motion_name.size() - 3)
                        list_box->AddItem(motion_name);
                }
            }
        }
        break;
    case 1:
        for (int32_t i = 165; i <= 191; i++) {
            uint32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(i);
            if (motion_id != -1) {
                motion_id_mottbl_map.insert({ motion_id, i });
                list_box->AddItem(aft_mot_db->get_motion_name(motion_id));
            }
        }
        break;
    case 2:
        for (int32_t i = 131; i <= 164; ++i) {
            uint32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(i);
            if (motion_id != -1) {
                motion_id_mottbl_map.insert({ motion_id, i });
                list_box->AddItem(aft_mot_db->get_motion_name(motion_id));
            }
        }
        break;
    }

    if (selected_item >= list_box->list->items.size())
        selected_item = 0;

    list_box->list->SetItemIndex(selected_item);
}

DataTestFaceMotDw::Data DataTestFaceMotDw::GetData() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    DataTestFaceMotDw::Data data = {
        {   6, 0.0f },
        {   6, 0.0f },
        { 165, 0.0f },
        { 131, 0.0f },
    };

    std::string face_mot = face->GetSelectedItem();
    auto elem_face = motion_id_mottbl_map.find(aft_mot_db->get_motion_id(face_mot.c_str()));
    if (elem_face != motion_id_mottbl_map.end())
        data.face.mottbl_index = elem_face->second;
    data.face.frame = face_frame->scroll_bar->value;

    std::string face_cl_mot;
    if (face_mot.find("FACE_WINK_OLD") != -1)
        face_cl_mot.assign("FACE_WINK_FT_OLD_CL");
    else {
        face_cl_mot.assign(face_mot);
        face_cl_mot.append("_CL");
    }

    uint32_t face_cl_mot_id = aft_mot_db->get_motion_id(face_cl_mot.c_str());
    if (face_cl_mot_id != -1)
        face_cl->SetText(face_cl_mot);
    else
        face_cl->SetText("CLモーションなし");

    auto elem_eyelid = motion_id_mottbl_map.find(face_cl_mot_id);
    if (elem_eyelid != motion_id_mottbl_map.end())
        data.eyelid.mottbl_index = elem_eyelid->second;
    data.eyelid.frame = face_cl_frame->scroll_bar->value;
    
    auto elem_eyes = motion_id_mottbl_map.find(aft_mot_db->get_motion_id(eyes->GetSelectedItem().c_str()));
    if (elem_eyes != motion_id_mottbl_map.end())
        data.eyes.mottbl_index = elem_eyes->second;
    data.eyes.frame = eyes_frame->scroll_bar->value;

    auto elem_mouth = motion_id_mottbl_map.find(aft_mot_db->get_motion_id(mouth->GetSelectedItem().c_str()));
    if (elem_mouth != motion_id_mottbl_map.end())
        data.mouth.mottbl_index = elem_mouth->second;
    data.mouth.frame = mouth_frame->scroll_bar->value;

    return data;
}

bool DataTestFaceMotDw::GetEnable() {
    return enable->value;
}

dw::ListBox* DataTestFaceMotDw::InitAddMottblMapMotions(dw::Composite* parent, int32_t type) {
    dw::ListBox* list_box = new dw::ListBox(parent);
    //list_box->list->sub_1402F9930(20);
    AddMottblMapMotions(list_box, type);
    return list_box;
}

void DataTestFaceMotDw::Reset() {
    motion_id_mottbl_map.clear();
    AddMottblMapMotions(face, 0);
    AddMottblMapMotions(eyes, 1);
    AddMottblMapMotions(mouth, 2);
}

DataTestMotDw::StepSliderProc::StepSliderProc() : index() {

}

DataTestMotDw::StepSliderProc::~StepSliderProc() {

}

void DataTestMotDw::StepSliderProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        test_mot_data->step[slider->callback_data.i32].array[index] = slider->scroll_bar->value;
}

DataTestMotDw::CharaListBoxProc::CharaListBoxProc() : list_box() {

}

DataTestMotDw::CharaListBoxProc::~CharaListBoxProc() {

}

void DataTestMotDw::CharaListBoxProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        int32_t chara_id = list_box->callback_data.i32;
        ::chara_index chara_index = (::chara_index)list_box->list->selected_item;
        test_mot_data->chara_index[chara_id] = chara_index;

        DataTestMotDw* test_mot_dw = data_test_mot_dw_array_get(chara_id);
        if (test_mot_dw->disp_button_proc.button->value) {
            data_struct* aft_data = &data_list[DATA_AFT];
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            uint32_t set_id = chara_init_data_get(chara_index)->motion_set;
            uint32_t set_index = -1;
            for (const motion_set_info& i : aft_mot_db->motion_set)
                if (i.id == set_id) {
                    set_index = (uint32_t)(&i - aft_mot_db->motion_set.data());
                    break;
                }

            test_mot_dw->c_type_list_box_proc.list_box->SetItemIndex(set_index);
            test_mot_data->motion_set_index[chara_id] = set_index;
        }

        test_mot_dw->AddModules(chara_id, test_mot_dw->chara_list_box_proc.list_box);
    }
}

DataTestMotDw::CTypeListBoxProc::CTypeListBoxProc() : list_box() {

}

DataTestMotDw::CTypeListBoxProc::~CTypeListBoxProc() {

}

void DataTestMotDw::CTypeListBoxProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        std::string str = list_box->GetSelectedItem();
        size_t start = str.find('(');
        if (start != -1) {
            start += 4;

            size_t end = str.find(')');
            if (end != -1)
                test_mot_data->module_index[list_box->callback_data.i32] =
                    atoi(str.substr(start, end - start).c_str()) - 1;
        }
    }
}

DataTestMotDw::SetListBoxProc::SetListBoxProc() : list_box() {

}

DataTestMotDw::SetListBoxProc::~SetListBoxProc() {

}

void DataTestMotDw::SetListBoxProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        int32_t chara_id = list_box->callback_data.i32;
        test_mot_data->motion_set_index[chara_id] = (int32_t)list_box->list->selected_item;
        data_test_mot_dw_array_get(chara_id)->ResetFrame();
    }
}

DataTestMotDw::IdListBoxProc::IdListBoxProc() : slider() {

}

DataTestMotDw::IdListBoxProc::~IdListBoxProc() {

}

void DataTestMotDw::IdListBoxProc::Callback(dw::Widget* data) {

}

DataTestMotDw::RotateSliderProc::RotateSliderProc() : slider() {

}

DataTestMotDw::RotateSliderProc::~RotateSliderProc() {

}

void DataTestMotDw::RotateSliderProc::Callback(dw::Widget* data) {

}

DataTestMotDw::PositionSliderProc::PositionSliderProc() {

}

DataTestMotDw::PositionSliderProc::~PositionSliderProc() {

}

void DataTestMotDw::PositionSliderProc::Callback(dw::Widget* data) {

}

DataTestMotDw::FrameSliderProc::FrameSliderProc() : slider() {

}

DataTestMotDw::FrameSliderProc::~FrameSliderProc() {

}

void DataTestMotDw::FrameSliderProc::Callback(dw::Widget* data) {

}

DataTestMotDw::StartFrameSliderProc::StartFrameSliderProc() : button() {

}

DataTestMotDw::StartFrameSliderProc::~StartFrameSliderProc() {

}

void DataTestMotDw::StartFrameSliderProc::Callback(dw::Widget* data) {

}

DataTestMotDw::DispButtonProc::DispButtonProc() : button() {

}

DataTestMotDw::DispButtonProc::~DispButtonProc() {

}

void DataTestMotDw::DispButtonProc::Callback(dw::Widget* data) {

}

DataTestMotDw::UseOpdButtonProc::UseOpdButtonProc() {

}

DataTestMotDw::UseOpdButtonProc::~UseOpdButtonProc() {

}

void DataTestMotDw::UseOpdButtonProc::Callback(dw::Widget* data) {

}

DataTestMotDw::PartialMotButtonProc::PartialMotButtonProc() {

}

DataTestMotDw::PartialMotButtonProc::~PartialMotButtonProc() {

}

void DataTestMotDw::PartialMotButtonProc::Callback(dw::Widget* data) {

}

DataTestMotDw::SaveOnlyStartFrameButtonProc::SaveOnlyStartFrameButtonProc() {

}

DataTestMotDw::SaveOnlyStartFrameButtonProc::~SaveOnlyStartFrameButtonProc() {

}

void DataTestMotDw::SaveOnlyStartFrameButtonProc::Callback(dw::Widget* data) {

}

DataTestMotDw::CreateFaceMotDwProc::CreateFaceMotDwProc() {

}

DataTestMotDw::CreateFaceMotDwProc::~CreateFaceMotDwProc() {

}

void DataTestMotDw::CreateFaceMotDwProc::Callback(dw::Widget* data) {

}

DataTestMotDw::CreateEqDwProc::CreateEqDwProc() {

}

DataTestMotDw::CreateEqDwProc::~CreateEqDwProc() {

}

void DataTestMotDw::CreateEqDwProc::Callback(dw::Widget* data) {

}

DataTestMotDw::CreateDebugCamProc::CreateDebugCamProc() {

}

DataTestMotDw::CreateDebugCamProc::~CreateDebugCamProc() {

}

void DataTestMotDw::CreateDebugCamProc::Callback(dw::Widget* data) {

}

DataTestMotDw::DataTestMotDw(int32_t chara_id, DtmMot* dtm_mot) {
    this->dtm_mot = dtm_mot;

    DataTestMot::Data* test_mot_data = data_test_mot_data_get();

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "MOTION TEST %dP", chara_id + 1);
    SetText(buf);

    dw::Composite* chara_comp = new dw::Composite(this);
    chara_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* chara_label = new dw::Label(chara_comp);
    chara_label->SetText("CHARA   ");
    //chara_label->SetFont(&p_dw__font_type_6x12);

    dw::ListBox* chara = new dw::ListBox(chara_comp);
    //chara->list->sub_1402F9930(20);
    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++)
        chara->AddItem(chara_index_get_name((chara_index)i));
    chara->callback_data.i32 = chara_id;
    chara->SetItemIndex(test_mot_data->chara_index[chara_id]);
    chara->AddSelectionListener(&chara_list_box_proc);
    //chara->SetFont(&p_dw__font_type_6x12);

    dw::Composite* chara_list_box_comp = new dw::Composite(this);
    chara_list_box_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    chara_list_box_proc.list_box = new dw::ListBox(chara_list_box_comp);
    //chara_list_box_proc.list_box->list->sub_1402F9930(40);
    //chara_list_box_proc.list_box->SetFont(&p_dw__font_type_6x12);
    AddModules(chara_id, chara_list_box_proc.list_box);
    chara_list_box_proc.list_box->callback_data.i32 = chara_id;

    chara_list_box_proc.list_box->SetItemIndex(test_mot_data->module_index[chara_id]);
    chara_list_box_proc.list_box->AddSelectionListener(&c_type_list_box_proc);
    //chara_list_box_proc.list_box->SetFont(&p_dw__font_type_6x12);

    dw::Composite* set_comp = new dw::Composite(this);
    set_comp->SetLayout( new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* set_label = new dw::Label(set_comp);
    set_label->SetText("SET     ");
    //set_label->SetFont(&p_dw__font_type_6x12);

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    c_type_list_box_proc.list_box = new dw::ListBox(set_comp);
    for (motion_set_info& i : aft_mot_db->motion_set)
        c_type_list_box_proc.list_box->AddItem(i.name);
    //c_type_list_box_proc.list_box->list->sub_1402F9930(40);
    c_type_list_box_proc.list_box->callback_data.i32 = chara_id;

    c_type_list_box_proc.list_box->SetItemIndex(test_mot_data->motion_set_index[chara_id]);
    c_type_list_box_proc.list_box->AddSelectionListener(&set_list_box_proc);
    //c_type_list_box_proc.list_box->SetFont(&p_dw__font_type_6x12);

    dw::Composite* id_comp = new dw::Composite(this);
    id_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* id_label = new dw::Label(id_comp);
    id_label->SetText("ID      ");
    //id_label->SetFont(&p_dw__font_type_6x12);

    set_list_box_proc.list_box = new dw::ListBox(id_comp);
    //set_list_box_proc.list_box->list->sub_1402F9930(40);
    set_list_box_proc.list_box->callback_data.i32 = chara_id;
    set_list_box_proc.list_box->AddSelectionListener(&id_list_box_proc);
    //set_list_box_proc.list_box->SetFont(&p_dw__font_type_6x12);

    id_list_box_proc.slider = dw::Slider::make(this);
    id_list_box_proc.slider->SetText("Y ROT");
    id_list_box_proc.slider->format = "%3.0f";
    id_list_box_proc.slider->SetParams(test_mot_data->rot_y[chara_id], -180.0f, 180.0f, 36.0f, 1.0f, 10.0f);
    id_list_box_proc.slider->callback_data.i32 = chara_id;
    id_list_box_proc.slider->AddSelectionListener(&rotate_slider_proc);

    rotate_slider_proc.slider = dw::Slider::make(this);
    rotate_slider_proc.slider->dw::Widget::SetText("X POS");
    rotate_slider_proc.slider->format = "%3.2f";
    rotate_slider_proc.slider->SetParams(test_mot_data->trans_x[chara_id], -2.0f, 2.0f, 0.4f, 0.01f, 0.1f);
    rotate_slider_proc.slider->callback_data.i32 = chara_id;
    rotate_slider_proc.slider->AddSelectionListener(&position_slider_proc);

    dw::Group* frame_group = new dw::Group(this);
    frame_group->SetText("FRAME");
    //frame_group->SetFont(&p_dw__font_type_6x12);

    for (int32_t i = 3; i < 4; i++) {
        step_slider = dw::Slider::make(frame_group);
        char buf[0x10];
        sprintf_s(buf, sizeof(buf), "STEP %02d", i);
        step_slider->SetText(buf);
        step_slider->format = "%4.4f";
        step_slider->SetParams(test_mot_data->step[chara_id].array[i], -3.0f, 3.0f, 0.4f, 0.01f, 0.1f);
        step_slider->callback_data.i32 = chara_id;
        step_slider_proc[i].index = i;
        step_slider->AddSelectionListener(&step_slider_proc[i]);
    }

    dw::Composite* frame_comp = new dw::Composite(frame_group);
    frame_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(frame_comp))->SetText("FRAME ");

    frame = new dw::Label(frame_comp, dw::FLAG_4000);
    frame->SetText("00000.0000");

    dw::Label* frame_separator = new dw::Label(frame_comp);
    frame_separator->SetText(" / ");

    frame_count = new dw::Label(frame_comp, dw::FLAG_4000);
    frame_count->SetText("00000.0000");

    current = dw::Slider::make(frame_group);
    current->SetText("Current");
    current->format = "%4.0f";
    current->SetParams(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    current->callback_data.i32 = chara_id;
    current->AddSelectionListener(&frame_slider_proc);

    frame_slider_proc.slider = dw::Slider::make(frame_group);
    frame_slider_proc.slider->SetText("Start  ");
    frame_slider_proc.slider->format = "%4.0f";
    frame_slider_proc.slider->SetParams(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    frame_slider_proc.slider->callback_data.i32 = chara_id;
    frame_slider_proc.slider->AddSelectionListener(&start_frame_slider_proc);

    dw::Composite* start_ctrl_comp = new dw::Composite(frame_group);
    start_ctrl_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(start_ctrl_comp))->SetText("Start Ctrl : ");

    dw::Button* start_ctrl_left = new dw::Button(start_ctrl_comp, dw::FLAG_8);
    start_ctrl_left->SetText(" < ");
    start_ctrl_left->callback_data.v64 = this;
    start_ctrl_left->callback = DataTestMotDw::StartCtrlLeftRightCallback;

    dw::Button* start_ctrl_reset = new dw::Button(start_ctrl_comp, dw::FLAG_8);
    start_ctrl_reset->SetText(" RESET ");
    start_ctrl_reset->callback = DataTestMotDw::StartCtrlResetCallback;

    dw::Button* start_ctrl_right = new dw::Button(start_ctrl_comp, dw::FLAG_8);
    start_ctrl_right->SetText(" > ");
    start_ctrl_right->callback_data.v64 = this;
    start_ctrl_right->callback = DataTestMotDw::StartCtrlLeftRightCallback;

    dw::Group* ab_loop_group = new dw::Group(this);
    ab_loop_group->SetLayout(new dw::RowLayout(dw::HORIZONTAL));
    ab_loop_group->SetText("AB LOOP");
    //ab_loop_group->SetFont(&p_dw__font_type_6x12);

    dw::Button* a_label = new dw::Button(ab_loop_group, dw::FLAG_8);
    a_label->SetText(" A ");
    a_label->callback_data.i64 = (int64_t)this;
    a_label->callback = DataTestMotDw::ACallback;
    //a_label->SetFont(&p_dw__font_type_6x12);

    dw::Button* b_label = new dw::Button(ab_loop_group, dw::FLAG_8);
    b_label->SetText(" B ");
    b_label->callback_data.i64 = (int64_t)this;
    b_label->callback = DataTestMotDw::BCallback;
    //b_label->SetFont(&p_dw__font_type_6x12);

    dw::Button* ab_toggle_button = new dw::Button(ab_loop_group, dw::CHECKBOX);
    ab_toggle_button->SetText("");
    ab_toggle_button->callback_data.v64 = this;
    ab_toggle_button->callback = DataTestMotDw::ABToggleCallback;
    //ab_toggle->SetFont(&p_dw__font_type_6x12);
    start_frame_slider_proc.button = ab_toggle_button;

    ab_loop = new dw::Label(ab_loop_group, dw::FLAG_8);
    //ab_loop->SetFont(&p_dw__font_type_6x12);
    ab_loop->SetText("     00000:00000");

    dw::Composite* rob_comp = new dw::Composite(this);
    rob_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* disp_button = new dw::Button(rob_comp, dw::CHECKBOX);
    disp_button->SetText("DISP  ");
    disp_button->SetValue(true);
    disp_button->AddSelectionListener(&disp_button_proc);
    disp_button->callback_data.i32 = chara_id;
    //disp_button->SetFont(&p_dw__font_type_6x12);

    disp_button_proc.button = new dw::Button(rob_comp, dw::CHECKBOX);
    disp_button_proc.button->SetText("SET CHANGE  ");
    disp_button_proc.button->SetValue(false);
    //disp_button_proc.button->SetFont(&p_dw__font_type_6x12);

    dw::Composite* var_comp = new dw::Composite(this);
    var_comp->Composite::SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* reate_face_mot_dw = new dw::Button(var_comp, dw::FLAG_8);
    reate_face_mot_dw->SetText(" FACE MOT ");
    reate_face_mot_dw->AddSelectionListener(&create_face_mot_dw_proc);
    reate_face_mot_dw->callback_data.i32 = chara_id;
    //reate_face_mot_dw->SetFont(&p_dw__font_type_6x12);

    dw::Button* create_eq_dw = new dw::Button(var_comp, dw::FLAG_8);
    create_eq_dw->SetText("  EQUIP  ");
    create_eq_dw->AddSelectionListener(&create_eq_dw_proc);
    //create_eq_dw->SetFont(&p_dw__font_type_6x12);

    dw::Button* create_debug_cam = new dw::Button(var_comp, dw::FLAG_8);
    create_debug_cam->SetText(" LOCK CAM ");
    create_debug_cam->AddSelectionListener(&create_debug_cam_proc);
    //create_debug_cam->SetFont(&p_dw__font_type_6x12);

    dw::Button* use_opd_button = new dw::Button(this, dw::CHECKBOX);
    use_opd_button->SetText("USE OSAGE PLAY DATA");
    use_opd_button->SetValue(false);
    use_opd_button->AddSelectionListener(&use_opd_button_proc);
    use_opd_button->callback_data.i32 = chara_id;
    //use_opd_button->SetFont(&p_dw__font_type_6x12);

    dtm_mot->SetPartialMot(false);

    dw::Button* save_only_start_frame_button = new dw::Button(this, dw::CHECKBOX);
    save_only_start_frame_button->SetText("Initialize only Start Frame Osage");
    save_only_start_frame_button->SetValue(false);
    save_only_start_frame_button->AddSelectionListener(&save_only_start_frame_button_proc);
    save_only_start_frame_button->callback_data.i32 = chara_id;
    //save_only_start_frame_button->SetFont(&p_dw__font_type_6x12);

    GetSetSize();
}

DataTestMotDw::~DataTestMotDw() {

}

void DataTestMotDw::Draw() {
    bool ab_loop;
    float_t a_frame;
    float_t b_frame;
    dtm_mot->GetABLoop(ab_loop, a_frame, b_frame);
    start_frame_slider_proc.button->SetValue(ab_loop);

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "     %5d:%5d", (int32_t)roundf(a_frame), (int32_t)roundf(b_frame));
    this->ab_loop->SetText(buf);

    dw::Shell::Draw();
}

void DataTestMotDw::Hide() {
    SetDisp(false);
}

void DataTestMotDw::ClearIDs() {
    set_list_box_proc.list_box->ClearItems();
}

void DataTestMotDw::AddID(const char* str) {
    set_list_box_proc.list_box->AddItem(str);
}

void DataTestMotDw::SetFrameSlider(float_t frame, float_t frame_count) {
    float_t last_frame = frame_count - 1.0f;
    current->SetParams(min_def(frame, last_frame), 0.0f, last_frame, last_frame * 0.1f, 1.0f, 10.0f);

    float_t _frame = frame_slider_proc.slider->scroll_bar->value;
    if (_frame >= 0.0)
        _frame = 0.0;
    else if (_frame > last_frame)
        _frame = last_frame;
    frame_slider_proc.slider->SetParams(_frame, 0.0f, last_frame, last_frame * 0.1f, 1.0f, 10.0f);

    data_test_mot_data_get()->start_frame[frame_slider_proc.slider->callback_data.i32] = _frame;
}

void DataTestMotDw::SetFrameLabel(float_t frame, float_t frame_count) {
    char buf[0x40];
    sprintf_s(buf, sizeof(buf), "%10.4f", frame);
    this->frame->SetText(buf);

    sprintf_s(buf, sizeof(buf), "%7.1f", frame_count);
    this->frame_count->SetText(buf);
}

void DataTestMotDw::AddModules(int32_t chara_id, dw::ListBox* list_box) {
    if (!list_box)
        return;

    DataTestMot::Data* test_mot_data = data_test_mot_data_get();

    list_box->ClearItems();

    chara_index chara_index = test_mot_data->chara_index[chara_id];
    const char* chara_name = chara_index_get_chara_name(chara_index);

    const std::vector<module_data>& modules = module_data_handler_data_get_modules();
    for (auto& i : item_table_handler_array_get_table(chara_index)->cos) {
        bool found = false;
        for (const module_data& j : modules)
            if (chara_index == j.chara_index && i.first == j.cos) {
                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "%03d:(%s%03d)%s", j.id, chara_name, i.first + 1, j.name.c_str());
                list_box->AddItem(buf);

                found = true;
                break;
            }

        if (!found && i.second.data.outer) {
            const item_table_item* item = item_table_handler_array_get_item(chara_index, i.second.data.outer);
            if (item) {
                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "---:(%s%03d)%s", chara_name, i.first + 1, item->name.c_str());
                list_box->AddItem(buf);
            }
        }
    }

    int32_t module_index = test_mot_data->module_index[chara_id];
    list_box->SetItemIndex(module_index < list_box->GetItemCount() ? module_index : 0);
}

void DataTestMotDw::ResetFrame() {
    frame_slider_proc.slider->SetValue(0.0f);
    dtm_mot->ResetABLoop();
}

void DataTestMotDw::ResetIDListBoxIndex() {
    GetSetSize();

    set_list_box_proc.list_box->SetItemIndex(0);
}

void DataTestMotDw::SetIDListBoxIndex(uint32_t index) {
    set_list_box_proc.list_box->SetItemIndex(index);
}

void DataTestMotDw::ABToggleCallback(dw::Widget* data) {
    DataTestMotDw* test_mot_dw = (DataTestMotDw*)data->callback_data.v64;
    if (test_mot_dw)
        test_mot_dw->dtm_mot->ToggleABLoop();
}

void DataTestMotDw::ACallback(dw::Widget* data) {
    DataTestMotDw* test_mot_dw = (DataTestMotDw*)data->callback_data.v64;
    if (test_mot_dw)
        test_mot_dw->dtm_mot->SetABFrame(true);
}

void DataTestMotDw::BCallback(dw::Widget* data) {
    DataTestMotDw* test_mot_dw = (DataTestMotDw*)data->callback_data.v64;
    if (test_mot_dw)
        test_mot_dw->dtm_mot->SetABFrame(false);
}

void DataTestMotDw::StartCtrlLeftRightCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (!button)
        return;

    DataTestMotDw* test_mot_dw = (DataTestMotDw*)button->callback_data.v64;
    if (!test_mot_dw)
        return;

    float_t frame = test_mot_dw->frame_slider_proc.slider->scroll_bar->value;

    std::vector<pv_data_set_motion> set_motion(test_mot_dw->dtm_mot->set_motion);

    auto i_begin = set_motion.begin();
    auto i_end = set_motion.end();
    for (auto i = i_begin; i != i_end; )
        if (i->motion_id != test_mot_dw->dtm_mot->motion_id) {
            for (auto j = i, k = i - 1; k != i_end; j++, k++)
                *j = *k;

            set_motion.pop_back();
            i_end = set_motion.end();
        }
        else
            i++;

    if (!set_motion.size())
        return;

    float_t last = 0.0f;
    float_t first = -1.0f;
    for (pv_data_set_motion& i : set_motion) {
        if (i.motion_id == test_mot_dw->dtm_mot->motion_id)
            if (&i != set_motion.data())
                last = i.frame_stage_index.first;
            if (frame < i.frame_stage_index.first) {
                first = i.frame_stage_index.first;
                break;
            }
    }

    if (frame > set_motion.back().frame_stage_index.first)
        last = set_motion.back().frame_stage_index.first;

    if (button->GetText().compare(" < "))
        test_mot_dw->frame_slider_proc.slider->SetValue(last);
    else if (first > 0.0f)
        test_mot_dw->frame_slider_proc.slider->SetValue(first);
}

void DataTestMotDw::StartCtrlResetCallback(dw::Widget* data) {
    data_test_mot_data_get()->reset_mot = true;
}

DataTestMotA3dDw::PvListBoxProc::PvListBoxProc() {

}

DataTestMotA3dDw::PvListBoxProc::~PvListBoxProc() {

}

void DataTestMotA3dDw::PvListBoxProc::Callback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        data_test_mot_a3d_dw->SetPvId(atoi(list_box->GetSelectedItem().c_str()));

    data_test_mot_a3d_dw->GetSetSize();
}

DataTestMotA3dDw::A3dListBoxProc::A3dListBoxProc() {

}

DataTestMotA3dDw::A3dListBoxProc::~A3dListBoxProc() {

}

void DataTestMotA3dDw::A3dListBoxProc::Callback(dw::Widget* data) {

}

DataTestMotCtrlDw::TypeListBoxProc::TypeListBoxProc() {

}

DataTestMotCtrlDw::TypeListBoxProc::~TypeListBoxProc() {

}

void DataTestMotCtrlDw::TypeListBoxProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        test_mot_data->type = (int32_t)list_box->list->selected_item;
}

DataTestMotCtrlDw::SyncFrameButtonProc::SyncFrameButtonProc() {

}

DataTestMotCtrlDw::SyncFrameButtonProc::~SyncFrameButtonProc() {

}

void DataTestMotCtrlDw::SyncFrameButtonProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        test_mot_data->sync_frame = button->value;
}

DataTestMotCtrlDw::GameCameraButtonProc::GameCameraButtonProc() {

}

DataTestMotCtrlDw::GameCameraButtonProc::~GameCameraButtonProc() {

}

void DataTestMotCtrlDw::GameCameraButtonProc::Callback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        test_mot_data->field_A8 = true;
        test_mot_data->field_A9 = button->value;
    }
}

DataTestMotA3dDw::DataTestMotA3dDw() {
    SetText("MOTION TEST");

    dw::Composite* pv_comp = new dw::Composite(this);
    pv_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* pv_label = new dw::Label(pv_comp);
    pv_label->SetText("PV");
    //pv_label->SetFont(&p_dw__font_type_6x12);

    pv = new dw::ListBox(pv_comp);
    //pv->list->sub_1402F9930(20);
    //pv->SetFont(&p_dw__font_type_6x12);

    for (int32_t i = 0; i < 1000; i++) {
        if (!task_pv_db_get_pv(i))
            continue;

        char buf[0x10];
        sprintf_s(buf, sizeof(buf), "%03d", i);
        pv->AddItem(buf);
    }

    pv->AddSelectionListener(&pv_list_box_proc);

    dw::Composite* a3d_comp = new dw::Composite(this);
    a3d_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* a3d_label = new dw::Label(a3d_comp);
    a3d_label->SetText("A3d");
    //a3d_label->SetFont(&p_dw__font_type_6x12);

    a3d = new dw::ListBox(a3d_comp);
    //a3d->list->sub_1402F9930(20);
    a3d->AddItem("PVを選択して下さい");

    a3d->Reset();
    a3d->AddSelectionListener(&a3d_list_box_proc);
    //a3d->SetFont(&p_dw__font_type_6x12);

    dw::Composite* button_comp = new dw::Composite(this);
    button_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    play_a3d = new dw::Button(button_comp, dw::FLAG_8);
    play_a3d->SetText(" PLAY A3D ");

    play_a3d->callback_data.v64 = this;
    play_a3d->callback = DataTestMotA3dDw::PlayA3dCallback;
    //play_a3d->SetFont(&p_dw__font_type_6x12);

    sync_1p_frame = new dw::Button(button_comp, dw::CHECKBOX);
    sync_1p_frame->SetText("SYNC 1P FRAME");
    sync_1p_frame->callback_data.v64 = this;
    sync_1p_frame->callback = DataTestMotA3dDw::Sync1pFrameCallback;
    //sync_1p_frame->SetFont(&p_dw__font_type_6x12);

    GetSetSize();

    rect.pos.x = (float_t)res_window_get()->width * 0.3f - rect.size.x * 0.3f;

    SetSize(rect.size);
}

DataTestMotA3dDw::~DataTestMotA3dDw() {

}

void DataTestMotA3dDw::Draw() {
    SetText(data_test_mot_a3d_get_state_text());
}

void DataTestMotA3dDw::Hide() {
    SetDisp(false);
    data_test_mot_a3d_del_task();
}

void DataTestMotA3dDw::SetPvId(int32_t pv_id) {
    a3d->ClearItems();

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

    for (const auth_3d_database_category& i : aft_auth_3d_db->category) {
        if (i.name.find(buf) == -1 || !i.uid.size())
            continue;

        std::vector<std::string> auth_3d_names;
        auth_3d_names.reserve(i.uid.size());

        for (const int32_t& j : i.uid)
            auth_3d_names.push_back(aft_auth_3d_db->uid[j].name);

        prj::sort(auth_3d_names);

        for (const std::string& i : auth_3d_names)
            a3d->AddItem(i);
    }

    if (!a3d->GetItemCount())
        a3d->AddItem("No 3D Auth");

    a3d->SetItemIndex(0);
}

void DataTestMotA3dDw::PlayA3dCallback(dw::Widget* data) {
    std::string auth_3d = data_test_mot_a3d_dw_get()->a3d->GetSelectedItem();
    data_test_mot_a3d_set_auth_3d(auth_3d);
}

void DataTestMotA3dDw::Sync1pFrameCallback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        test_mot_data->sync_1p_frame = button->value;
}

DataTestMotCtrlDw::DataTestMotCtrlDw() {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();

    SetText("MOTION TEST");

    type_list = new dw::ListBox(this);
    type_list->AddItem("ONCE");
    type_list->AddItem("LOOP");
    type_list->AddItem("STEP");
    type_list->SetItemIndex(test_mot_data->type);
    type_list->AddSelectionListener(&type_list_box_proc);

    reset_mot = new dw::Button(this, dw::FLAG_8);
    reset_mot->SetText("RESET MOT");
    reset_mot->callback_data.v64 = this;
    reset_mot->callback = DataTestMotCtrlDw::ResetMotCallback;

    reset_cam = new dw::Button(this, dw::FLAG_8);
    reset_cam->SetText("RESET CAM");
    reset_cam->callback_data.v64 = this;
    reset_cam->callback = DataTestMotCtrlDw::ResetCamCallback;

    reload_data = new dw::Button(this, dw::FLAG_8);
    reload_data->SetText("RELOAD DATA");
    reload_data->callback_data.v64 = this;
    reload_data->callback = DataTestMotCtrlDw::ReloadDataCallback;

    dw::Button* sync_frame_button = new dw::Button(this, dw::CHECKBOX);
    sync_frame_button->SetText("SYNC FRAME");
    sync_frame_button->AddSelectionListener(&sync_frame_button_proc);
    //sync_frame_button->SetFont(&p_dw__font_type_6x12);

    running = new dw::Button(this, dw::CHECKBOX);
    running->SetText("RUNNING");
    running->SetValue(false);
    running->callback = DataTestMotCtrlDw::RunningCallback;
    //running->SetFont(&p_dw__font_type_6x12);

    stage = new dw::Button(this, dw::FLAG_8);
    stage->SetText("STAGE");
    stage->callback = DataTestMotCtrlDw::StageCallback;

    GetSetSize();

    rect.pos.x = (float_t)res_window_get()->width - rect.size.x;

    SetSize(rect.size);
}

DataTestMotCtrlDw::~DataTestMotCtrlDw() {

}

void DataTestMotCtrlDw::Draw() {

}

void DataTestMotCtrlDw::Hide() {

}

void DataTestMotCtrlDw::ReloadDataCallback(dw::Widget* data) {
    data_test_mot_data_get()->reload_data = true;
}

void DataTestMotCtrlDw::ResetCamCallback(dw::Widget* data) {
    data_test_mot_data_get()->reset_cam = true;
}

void DataTestMotCtrlDw::ResetMotCallback(dw::Widget* data) {
    data_test_mot_data_get()->reset_mot = true;
}

void DataTestMotCtrlDw::RunningCallback(dw::Widget* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        test_mot_data->running = button->value;
}

void DataTestMotCtrlDw::StageCallback(dw::Widget* data) {
    dtw_stg_load(false);
}

void motion_test_init() {
    if (!data_test_mot)
        data_test_mot = new DataTestMot;

    if (!data_test_mot_a3d)
        data_test_mot_a3d = new DataTestMotA3d;

    if (!dtm_mot_array)
        dtm_mot_array = new DtmMot[2];

    motion_test_objset = {};
}

void motion_test_free() {
    for (DataTestFaceMotDw*& i : data_test_face_mot_dw_array)
        if (i) {
            delete i;
            i = 0;
        }

    if (data_test_mot_ctrl_dw) {
        delete data_test_mot_ctrl_dw;
        data_test_mot_ctrl_dw = 0;
    }

    if (data_test_mot_a3d_dw) {
        delete data_test_mot_a3d_dw;
        data_test_mot_a3d_dw = 0;
    }

    for (DataTestMotDw*& i : data_test_mot_dw_array)
        if (i) {
            delete i;
            i = 0;
        }

    motion_test_objset.clear();
    motion_test_objset.shrink_to_fit();

    if (dtm_mot_array) {
        delete[] dtm_mot_array;
        dtm_mot_array = 0;
    }

    if (data_test_mot_a3d) {
        delete data_test_mot_a3d;
        data_test_mot_a3d = 0;
    }

    if (data_test_mot) {
        delete data_test_mot;
        data_test_mot = 0;
    }
}

void dtm_mot_array_set_reset_mot() {
    dtm_mot_array[0].SetResetMot();
    dtm_mot_array[1].SetResetMot();
}

static DataTestMot::Data* data_test_mot_data_get() {
    return &data_test_mot->data;
}

static bool data_test_mot_a3d_add_task() {
    return app::TaskWork::AddTask(data_test_mot_a3d_get(), "DATA_TEST_MOT_A3D", 0);
}

static bool data_test_mot_a3d_del_task() {
    return data_test_mot_a3d_get()->DelTask();
}

static const char* data_test_mot_a3d_get_state_text() {
    return data_test_mot_a3d_get()->GetStateText();
}

static DataTestMotA3d* data_test_mot_a3d_get() {
    return data_test_mot_a3d;
}

static void data_test_mot_a3d_set_auth_3d(std::string& value) {
    data_test_mot_a3d_get()->SetAuth3d(value);
}

static void data_test_mot_dw_array_init(int32_t chara_id, DtmMot* dtm_mot) {
    if (data_test_mot_dw_array[chara_id]) {
        data_test_mot_dw_array[chara_id]->Disp();
        return;
    }

    DataTestMotDw* test_mot_dw = new DataTestMotDw(chara_id, dtm_mot);
    data_test_mot_dw_array[chara_id] = test_mot_dw;

    test_mot_dw->sub_1402F38B0();

    switch (chara_id) {
    case 0: {
        test_mot_dw->rect.pos.y += 14.0f;
    } break;
    case 1: {
        resolution_struct* res_wind = res_window_get();
        test_mot_dw->rect.pos.x = (float_t)res_wind->width - test_mot_dw->rect.size.x;
        test_mot_dw->rect.pos.y = (float_t)res_wind->height - 24.0f;
    } break;
    }
}

static DataTestMotDw* data_test_mot_dw_array_get(int32_t chara_id) {
    return data_test_mot_dw_array[chara_id];
}

static void data_test_mot_a3d_dw_init() {
    if (!data_test_mot_a3d_dw) {
        data_test_mot_a3d_dw = new DataTestMotA3dDw;
        data_test_mot_a3d_dw->sub_1402F38B0();
    }
    else
        data_test_mot_a3d_dw->Disp();
}

static DataTestMotA3dDw* data_test_mot_a3d_dw_get() {
    return data_test_mot_a3d_dw;
}

static void data_test_mot_ctrl_dw_init() {
    if (!data_test_mot_ctrl_dw) {
        data_test_mot_ctrl_dw = new DataTestMotCtrlDw;
        data_test_mot_ctrl_dw->sub_1402F38B0();
    }
    else
        data_test_mot_ctrl_dw->Disp();
}

static DataTestMotCtrlDw* data_test_mot_ctrl_dw_get() {
    return data_test_mot_ctrl_dw;
}

static void data_test_face_mot_dw_array_init(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || !rob_chara_array_get(chara_id))
        return;

    if (!data_test_face_mot_dw_array[chara_id]) {
        data_test_face_mot_dw_array[chara_id] = new DataTestFaceMotDw(chara_id);
        data_test_face_mot_dw_array[chara_id]->sub_1402F38B0();
    }
    else
        data_test_face_mot_dw_array[chara_id]->Disp();
}

static DataTestFaceMotDw* data_test_face_mot_dw_array_get(int32_t chara_id) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        return data_test_face_mot_dw_array[chara_id];
    return 0;
}

static void data_test_face_mot_dw_array_unload() {
    for (DataTestFaceMotDw*& i : data_test_face_mot_dw_array)
        if (i) {
            i->Hide();
            i = 0;
        }
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
