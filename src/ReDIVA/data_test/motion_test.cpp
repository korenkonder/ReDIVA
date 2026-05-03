/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion_test.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/rob/skin_param.hpp"
#include "../../CRE/app_system_detail.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/debug_print.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/hand_item.hpp"
#include "../../CRE/module_table.hpp"
#include "../../CRE/pv_db.hpp"
#include "../../CRE/screen_param.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/stage_param.hpp"
#include "../dw.hpp"
#include "../input_state.hpp"
#include "../print_work.hpp"
#include "auth_3d_test.hpp"
#include "equip_test.hpp"
#include "stage_test.hpp"

class DataTestFaceMotDw : public dw::Shell {
public:
    struct MotionData {
        MOTTABLE_TYPE mottbl_type;
        float_t frame;
    };

    struct Data {
        MotionData face;
        MotionData eyelid;
        MotionData eyes;
        MotionData mouth;
    };

    ROB_ID rob_id;
    std::map<uint32_t, MOTTABLE_TYPE> motion_id_mottbl_map;
    dw::Button* enable;
    dw::ListBox* face;
    dw::Slider* face_frame;
    dw::Label* face_cl;
    dw::Slider* face_cl_frame;
    dw::ListBox* eyes;
    dw::Slider* eyes_frame;
    dw::ListBox* mouth;
    dw::Slider* mouth_frame;

    DataTestFaceMotDw(ROB_ID id);
    virtual ~DataTestFaceMotDw() override;

    virtual void Hide() override;

    void AddMottblMapMotions(dw::ListBox* list_box, int32_t type);
    Data GetData();
    bool GetEnable();
    void GetMottblIndexFrame(DataTestFaceMotDw::MotionData& data, uint32_t uid, float_t value);
    dw::ListBox* InitAddMottblMapMotions(dw::Composite* parent, int32_t type);
    void ResetMot();
};

class DataTestMotDw : public dw::Shell {
public:
    class StepSliderProc : public dw::SelectionAdapter {
    public:
        int32_t index;

        StepSliderProc();
        virtual ~StepSliderProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CharaListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        CharaListBoxProc();
        virtual ~CharaListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CTypeListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        CTypeListBoxProc();
        virtual ~CTypeListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SetListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        SetListBoxProc();
        virtual ~SetListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class IdListBoxProc : public dw::SelectionAdapter {
    public:
        IdListBoxProc();
        virtual ~IdListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class RotateSliderProc : public dw::SelectionAdapter {
    public:
        RotateSliderProc();
        virtual ~RotateSliderProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class PositionSliderProc : public dw::SelectionAdapter {
    public:
        PositionSliderProc();
        virtual ~PositionSliderProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class FrameSliderProc : public dw::SelectionAdapter {
    public:
        FrameSliderProc();
        virtual ~FrameSliderProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class StartFrameSliderProc : public dw::SelectionAdapter {
    public:
        StartFrameSliderProc();
        virtual ~StartFrameSliderProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class DispButtonProc : public dw::SelectionAdapter {
    public:
        DispButtonProc();
        virtual ~DispButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class UseOpdButtonProc : public dw::SelectionAdapter {
    public:
        UseOpdButtonProc();
        virtual ~UseOpdButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class PartialMotButtonProc : public dw::SelectionAdapter {
    public:
        PartialMotButtonProc();
        virtual ~PartialMotButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SaveOnlyStartFrameButtonProc : public dw::SelectionAdapter {
    public:
        SaveOnlyStartFrameButtonProc();
        virtual ~SaveOnlyStartFrameButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CreateFaceMotDwProc : public dw::SelectionAdapter {
    public:
        CreateFaceMotDwProc();
        virtual ~CreateFaceMotDwProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CreateEqDwProc : public dw::SelectionAdapter {
    public:
        CreateEqDwProc();
        virtual ~CreateEqDwProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    struct CreateDebugCamProc : public dw::SelectionAdapter {
    public:
        CreateDebugCamProc();
        virtual ~CreateDebugCamProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    DtmMot* dtm_mot;
    dw::ListBox* chara;
    CharaListBoxProc chara_list_box_proc;
    CTypeListBoxProc c_type_list_box_proc;
    SetListBoxProc set_list_box_proc;
    IdListBoxProc id_list_box_proc;
    dw::Slider* rotate_slider;
    RotateSliderProc rotate_slider_proc;
    dw::Slider* position_slider;
    PositionSliderProc position_slider_proc;
    //__int64 field_1E8;
    //__int64 field_1F0;
    //__int64 field_1F8;
    dw::Slider* step_slider;
    StepSliderProc step_slider_proc[4];
    dw::Slider* current;
    FrameSliderProc frame_slider_proc;
    dw::Slider* start_frame_slider;
    StartFrameSliderProc start_frame_slider_proc;
    dw::Button* ab_toggle_button;
    dw::Label* ab_loop;
    dw::Label* frame;
    dw::Label* frame_max;
    DispButtonProc disp_button_proc;
    dw::Button* set_change_button;
    UseOpdButtonProc use_opd_button_proc;
    PartialMotButtonProc partial_mot_button_proc;
    SaveOnlyStartFrameButtonProc save_only_start_frame_button_proc;
    CreateFaceMotDwProc create_face_mot_dw_proc;
    CreateEqDwProc create_eq_dw_proc;
    CreateDebugCamProc create_debug_cam_proc;

    DataTestMotDw(ROB_ID id, DtmMot* dtm_mot);
    virtual ~DataTestMotDw() override;

    virtual void Draw() override;
    virtual void Hide() override;

    virtual void ClearIDs();
    virtual void AddID(const char* str);
    virtual void SetFrameSlider(float_t frame, float_t frame_max);
    virtual void SetFrameLabel(float_t frame, float_t frame_max);

    void AddModules(ROB_ID id, dw::ListBox* list_box);
    void ResetFrame();
    void ResetIDListBoxIndex();
    void SetIDListBoxIndex(uint32_t index);

    static void ABToggleCallback(dw::Widget* data);
    static void ACallback(dw::Widget* data);
    static void BCallback(dw::Widget* data);
    static void StartCtrlLeftRightCallback(dw::Widget* data);
    static void StartCtrlResetCallback(dw::Widget* data);

    void sub_14028D8B0();
};

class DataTestMotA3dDw : public dw::Shell {
public:
    class PvListBoxProc : public dw::SelectionAdapter {
    public:
        PvListBoxProc();
        virtual ~PvListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class A3dListBoxProc : public dw::SelectionAdapter {
    public:
        A3dListBoxProc();
        virtual ~A3dListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    dw::ListBox* pv;
    DataTestMotA3dDw::PvListBoxProc pv_list_box_proc;
    dw::ListBox* a3d;
    DataTestMotA3dDw::A3dListBoxProc a3d_list_box_proc;
    dw::Button* play_a3d;
    dw::Button* sync_1p_frame;

    DataTestMotA3dDw();
    virtual ~DataTestMotA3dDw();

    virtual void Draw() override;
    virtual void Hide() override;

    void SetPvId(int32_t pv_id);

    static void PlayA3dCallback(dw::Widget* data);
    static void Sync1pFrameCallback(dw::Widget* data);
};

class DataTestMotCtrlDw : public dw::Shell {
public:
    class TypeListBoxProc : public dw::SelectionAdapter {
    public:
        TypeListBoxProc();
        virtual ~TypeListBoxProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SyncFrameButtonProc : public dw::SelectionAdapter {
    public:
        SyncFrameButtonProc();
        virtual ~SyncFrameButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class GameCameraButtonProc : public dw::SelectionAdapter {
    public:
        GameCameraButtonProc();
        virtual ~GameCameraButtonProc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    dw::ListBox* type_list;
    TypeListBoxProc type_list_box_proc;
    dw::Button* reset_mot;
    dw::Button* reset_cam;
    dw::Button* reload_data;
    dw::Button* stage;
    dw::Button* running;
    DataTestMotCtrlDw::SyncFrameButtonProc sync_frame_button_proc;
    DataTestMotCtrlDw::GameCameraButtonProc game_camera_button_proc;

    DataTestMotCtrlDw();
    virtual ~DataTestMotCtrlDw();

    virtual void Draw() override;
    virtual void Hide() override;

    static void ReloadDataCallback(dw::Widget* data);
    static void ResetCamCallback(dw::Widget* data);
    static void ResetMotCallback(dw::Widget* data);
    static void RunningCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
};

extern render_context* rctx_ptr;

DataTestMot* data_test_mot;
DataTestMotA3d* data_test_mot_a3d;

DtmMot* dtm_mot_array;

DataTestMotDw* data_test_mot_dw_array[ROB_ID_MAX];
DataTestMotA3dDw* data_test_mot_a3d_dw;
DataTestMotCtrlDw* data_test_mot_ctrl_dw;
DataTestFaceMotDw* data_test_face_mot_dw_array[ROB_ID_MAX];

std::vector<uint32_t> motion_test_objset;

static DataTestMot::Data* data_test_mot_data_get();

static bool data_test_mot_a3d_open();
static bool data_test_mot_a3d_close();
static const char* data_test_mot_a3d_get_state_text();
static DataTestMotA3d* data_test_mot_a3d_get();
static void data_test_mot_a3d_set_auth_3d(std::string& value);

static void data_test_mot_dw_array_init(ROB_ID id, DtmMot* dtm_mot);
static DataTestMotDw* data_test_mot_dw_array_get(ROB_ID id);

static void data_test_mot_a3d_dw_init();
static DataTestMotA3dDw* data_test_mot_a3d_dw_get();

static void data_test_mot_ctrl_dw_init();
static DataTestMotCtrlDw* data_test_mot_ctrl_dw_get();

static void data_test_face_mot_dw_array_init(ROB_ID id);
static DataTestFaceMotDw* data_test_face_mot_dw_array_get(ROB_ID id);
static void data_test_face_mot_dw_array_unload();

static bool motion_test_objset_check_not_read();
static void motion_test_objset_load();
static void motion_test_objset_unload();

DataTestMot::Data::Off::Off() {
    array[0] = 0.0f;
    array[1] = 0.0f;
}

DataTestMot::Data::Div::Div() {
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

DataTestMot::Data::Data() : chr(), curr_chr(), ptr(), curr_ptr(), set(),
curr_set(), uid(), curr_uid(), rot_y(), trans_x(), off(), start_frame(),
type(), reset_mot(), reset_cam(), reload_data(), sync_frame(), field_A8(),
field_A9(), running(), field_AC(), field_B0(), sync_1p_frame() {
    for (uint32_t& i : set)
        i = -1;
    for (uint32_t& i : curr_set)
        i = -1;
    field_AA = true;
}

DataTestMot::DataTestMot() {

}

DataTestMot::~DataTestMot() {

}

bool DataTestMot::init() {
    clear_color = 0xFF606060;
    set_clear_color = true;

    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    cam->set_roll(0.0f);

    data_test_mot_dw_array_init(ROB_ID_1P, &dtm_mot_array[ROB_ID_1P]);
    data_test_mot_dw_array_init(ROB_ID_2P, &dtm_mot_array[ROB_ID_2P]);
    data_test_mot_ctrl_dw_init();
    data_test_mot_a3d_dw_init();
    data.reset_cam = true;
    data.field_A8 = true;
    dtm_eq_vs_array[ROB_ID_1P].open(ROB_ID_1P, data.chr[ROB_ID_1P]);
    dtm_eq_vs_array[ROB_ID_2P].open(ROB_ID_2P, data.chr[ROB_ID_2P]);
    motion_test_objset_load();
    return true;
}

bool DataTestMot::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

   data_test_mot_dw_array_get(ROB_ID_1P)->sub_14028D8B0();

    bool v3 = false;
    bool v2 = false;

    if (data.reload_data) {
        data.reload_data = false;
        v3 = true;
        v2 = true;
    }

    if (data.chr[ROB_ID_1P] != data.curr_chr[ROB_ID_1P]) {
        data.curr_chr[ROB_ID_1P] = data.chr[ROB_ID_1P];
        dtm_eq_vs_array[ROB_ID_1P].SetCharaNumCosId(data.chr[ROB_ID_1P], data.ptr[ROB_ID_1P]);
        v3 = true;
        v2 = true;
    }

    if (data.chr[ROB_ID_2P] != data.curr_chr[ROB_ID_2P]) {
        data.curr_chr[ROB_ID_2P] = data.chr[ROB_ID_2P];
        dtm_eq_vs_array[ROB_ID_2P].SetCharaNumCosId(data.chr[ROB_ID_2P], data.ptr[ROB_ID_2P]);
        v3 = true;
        v2 = true;
    }

    if (data.ptr[ROB_ID_1P] != data.curr_ptr[ROB_ID_1P]) {
        data.curr_ptr[ROB_ID_1P] = data.ptr[ROB_ID_1P];
        dtm_eq_vs_array[ROB_ID_1P].SetCharaNumCosId(data.chr[ROB_ID_1P], data.ptr[ROB_ID_1P]);
        v3 = true;
        v2 = true;
    }

    if (data.ptr[ROB_ID_2P] != data.curr_ptr[ROB_ID_2P]) {
        data.curr_ptr[ROB_ID_2P] = data.ptr[ROB_ID_2P];
        dtm_eq_vs_array[ROB_ID_2P].SetCharaNumCosId(data.chr[ROB_ID_2P], data.ptr[ROB_ID_2P]);
        v3 = true;
        v2 = true;
    }

    if (data.set[ROB_ID_1P] != data.curr_set[ROB_ID_1P]) {
        data.curr_set[ROB_ID_1P] = data.set[ROB_ID_1P];

        data_test_mot_dw_array_get(ROB_ID_1P)->ClearIDs();

        const motion_set_info& set_info = aft_mot_db->motion_set[data.set[ROB_ID_1P]];
        for (const motion_info& i : set_info.motion)
            data_test_mot_dw_array_get(ROB_ID_1P)->AddID(i.name.c_str());

        data_test_mot_dw_array_get(ROB_ID_1P)->ResetIDListBoxIndex();

        data.curr_uid[ROB_ID_1P] = 0;
        data.uid[ROB_ID_1P] = 0;

        v3 = true;
        v2 = true;
    }

    if (data.set[ROB_ID_2P] != data.curr_set[ROB_ID_2P]) {
        data.curr_set[ROB_ID_2P] = data.set[ROB_ID_2P];

        data_test_mot_dw_array_get(ROB_ID_2P)->ClearIDs();

        const motion_set_info& set_info = aft_mot_db->motion_set[data.set[ROB_ID_2P]];
        for (const motion_info& i : set_info.motion)
            data_test_mot_dw_array_get(ROB_ID_2P)->AddID(i.name.c_str());

        data_test_mot_dw_array_get(ROB_ID_2P)->ResetIDListBoxIndex();

        data.curr_uid[ROB_ID_2P] = 0;
        data.uid[ROB_ID_2P] = 0;

        v3 = true;
        v2 = true;
    }

    bool frame_comp = false;
    if (data.uid[ROB_ID_1P] != data.curr_uid[ROB_ID_1P]) {
        data.curr_uid[ROB_ID_1P] = data.uid[ROB_ID_1P];
        dtm_mot_array[ROB_ID_1P].SetMotion(data.set[ROB_ID_1P], data.uid[ROB_ID_1P]);
        dtm_mot_array[ROB_ID_2P].SetMotion(data.set[ROB_ID_2P], data.uid[ROB_ID_2P]);
        dtm_mot_array[ROB_ID_1P].SetChangeMotion();
        dtm_mot_array[ROB_ID_2P].SetChangeMotion();
        frame_comp = true;
    }

    if (data.uid[ROB_ID_2P] != data.curr_uid[ROB_ID_2P]) {
        data.curr_uid[ROB_ID_2P] = data.uid[ROB_ID_2P];
        dtm_mot_array[ROB_ID_1P].SetMotion(data.set[ROB_ID_1P], data.uid[ROB_ID_1P]);
        dtm_mot_array[ROB_ID_2P].SetMotion(data.set[ROB_ID_2P], data.uid[ROB_ID_2P]);
        dtm_mot_array[ROB_ID_1P].SetChangeMotion();
        dtm_mot_array[ROB_ID_2P].SetChangeMotion();
    }

    dtm_mot_array[ROB_ID_1P].SetRotationY(data.rot_y[ROB_ID_1P]);
    dtm_mot_array[ROB_ID_2P].SetRotationY(data.rot_y[ROB_ID_2P]);
    dtm_mot_array[ROB_ID_1P].SetTrans({ data.trans_x[ROB_ID_1P], 0.0f, 0.0f });
    dtm_mot_array[ROB_ID_2P].SetTrans({ data.trans_x[ROB_ID_2P], 0.0f, 0.0f });
    dtm_mot_array[ROB_ID_1P].SetOffset(data.off[ROB_ID_1P].array[0], data.off[ROB_ID_1P].array[1]);
    dtm_mot_array[ROB_ID_2P].SetOffset(data.off[ROB_ID_2P].array[0], data.off[ROB_ID_2P].array[1]);
    dtm_mot_array[ROB_ID_1P].SetStartFrame(data.start_frame[ROB_ID_1P]);
    dtm_mot_array[ROB_ID_2P].SetStartFrame(data.start_frame[ROB_ID_2P]);

    for (int32_t i = 0; i < 3; i++) {
        dtm_mot_array[ROB_ID_1P].SetDivide(i, data.div[ROB_ID_1P].array[i]);
        dtm_mot_array[ROB_ID_2P].SetDivide(i, data.div[ROB_ID_2P].array[i]);
    }

    for (int32_t i = 0; i < 4; i++) {
        dtm_mot_array[ROB_ID_1P].SetStep(i, data.step[ROB_ID_1P].array[i]);
        dtm_mot_array[ROB_ID_2P].SetStep(i, data.step[ROB_ID_2P].array[i]);
    }

    switch (data.type) {
    case 0: {
        dtm_mot_array[ROB_ID_1P].SetLoop(false);
        dtm_mot_array[ROB_ID_2P].SetLoop(false);

        dtm_mot_array[ROB_ID_1P].SetPlay(true);
        dtm_mot_array[ROB_ID_2P].SetPlay(true);
    } break;
    case 1: {
        dtm_mot_array[ROB_ID_1P].SetLoop(true);
        dtm_mot_array[ROB_ID_2P].SetLoop(true);

        dtm_mot_array[ROB_ID_1P].SetPlay(true);
        dtm_mot_array[ROB_ID_2P].SetPlay(true);
    } break;
    case 2: {
        dtm_mot_array[ROB_ID_1P].SetLoop(true);
        dtm_mot_array[ROB_ID_2P].SetLoop(true);

        bool move = input_state_get(0)->CheckIntervalTapped(INPUT_BUTTON_SPACE);
        dtm_mot_array[ROB_ID_1P].SetPlay(move);
        dtm_mot_array[ROB_ID_2P].SetPlay(move);
    } break;
    }

    if (data.reset_mot) {
        data.reset_mot = false;

        dtm_mot_array[ROB_ID_1P].SetResetMot();
        dtm_mot_array[ROB_ID_2P].SetResetMot();
    }

    if (data.sync_frame) {
        ROB_ID rob_id = (ROB_ID)dtm_mot_array[ROB_ID_2P].GetFrameCount() > dtm_mot_array[ROB_ID_1P].GetFrameCount()
            ? ROB_ID_2P : ROB_ID_1P;
        float_t frame = dtm_mot_array[rob_id].GetFrame();
        dtm_mot_array[ROB_ID_1P].SetFrame(frame);
        dtm_mot_array[ROB_ID_2P].SetFrame(frame);
    }

    if (v3)
        dtm_mot_array[ROB_ID_1P].close();
    else
        dtm_mot_array[ROB_ID_1P].open(data.chr[ROB_ID_1P],
            data.ptr[ROB_ID_1P], data.set[ROB_ID_1P], data.uid[ROB_ID_1P]);

    if (v2)
        dtm_mot_array[ROB_ID_2P].close();
    else
        dtm_mot_array[ROB_ID_2P].open(data.chr[ROB_ID_2P],
            data.ptr[ROB_ID_2P], data.set[ROB_ID_2P], data.uid[ROB_ID_2P]);

    if (data.reset_cam) {
        data.reset_cam = false;

        cam_struct cam;
        if (data.field_A9) {
            cam.view_point = { -3.79f, 0.71f, 1.0f };
            cam.interest = { 0.0f, 0.96f, 0.0f };
            cam.fov = 0.57268613576889f;
        }
        else {
            cam.view_point = { 0.0f, 1.0f, 3.45f };
            cam.interest = { 0.0f, 1.0f, 0.0f };
            cam.fov = 0.563171327114105f;
        }
        cam.set(rctx_ptr->camera);
    }

    if (data.field_A8) {
        data.field_A8 = false;

        //sub_1401F9510(data.field_A9 ? 32.2673416137695f : 32.8125f);
    }

    if (data.running && dtm_mot_array[ROB_ID_1P].CheckFirstFrame() && !frame_comp
        && data.uid[ROB_ID_1P] < aft_mot_db->motion_set[data.set[ROB_ID_1P]].motion.size()) {
        data.curr_uid[ROB_ID_1P] = data.uid[ROB_ID_1P];
        data.uid[ROB_ID_1P] = data.uid[ROB_ID_1P] + 1;
        dtm_mot_array[ROB_ID_1P].SetMotion(data.set[ROB_ID_1P], data.uid[ROB_ID_1P]);
        dtm_mot_array[ROB_ID_2P].SetMotion(data.set[ROB_ID_2P], data.uid[ROB_ID_2P]);
        dtm_mot_array[ROB_ID_1P].SetChangeMotion();
        dtm_mot_array[ROB_ID_2P].SetChangeMotion();

        data_test_mot_dw_array_get(ROB_ID_1P)->SetIDListBoxIndex(data.uid[ROB_ID_1P]);
    }

    motion_test_objset_check_not_read();
    sub_140286280();
    return false;
}

bool DataTestMot::dest() {
    clear_color = 0xFF000000;
    set_clear_color = true;

    dtm_mot_array[ROB_ID_1P].close();
    dtm_mot_array[ROB_ID_2P].close();
    data_test_mot_dw_array_get(ROB_ID_1P)->Hide();
    data_test_mot_dw_array_get(ROB_ID_2P)->Hide();
    data_test_mot_ctrl_dw_get()->Hide();
    data_test_mot_a3d_dw_get()->Hide();
    data_test_face_mot_dw_array_unload();
    dtm_eq_vs_array[ROB_ID_1P].close();
    dtm_eq_vs_array[ROB_ID_2P].close();
    motion_test_objset_unload();
    data_test_mot_a3d_get()->close();
    return true;
}

void DataTestMot::disp() {

}

void DataTestMot::sub_140286280() {
    dtm_mot_array[ROB_ID_1P].sub_1402922C0(true);
    dtm_mot_array[ROB_ID_1P].sub_1402922C0(true);
    if (data.sync_frame && (!dtm_mot_array[ROB_ID_1P].GetLoaded()
        || !dtm_mot_array[ROB_ID_2P].GetLoaded())) {
        dtm_mot_array[ROB_ID_1P].sub_1402922C0(false);
        dtm_mot_array[ROB_ID_1P].sub_1402922C0(false);
    }
}

DataTestMotA3d::DataTestMotA3d() : state(), camera() {

}

DataTestMotA3d::~DataTestMotA3d() {

}

bool DataTestMotA3d::init() {
    Reset();
    auth_3d.clear();
    state = 0;
    return true;
}

bool DataTestMotA3d::ctrl() {
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
            objset_info_storage_load_set(aft_data, aft_obj_db, i);

        state = 3;
    } break;
    case 3: {
        bool wait_load = false;

        for (uint32_t& i : obj_sets)
            if (objset_info_storage_load_obj_set_check_not_read(i))
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

bool DataTestMotA3d::dest() {
    Reset();
    camera = false;
    return true;
}

void DataTestMotA3d::disp() {

}

void DataTestMotA3d::post() {

}

const char* DataTestMotA3d::GetStateText() {
    if (dw::translate)
        switch (state) {
        case 0:
            return u8"A3D: Initializing...";
        case 1:
            return u8"A3D: Awaiting command...";
        case 2:
            return u8"A3D: Requesting data...";
        case 3:
            return u8"A3D: Loading PV Stage...";
        case 4:
            return u8"A3D: Loading A3D Category...";
        case 5:
            return u8"A3D: Loading A3D Handle...";
        default:
            return u8"A3D: Unknown mode";
        }
    else
        switch (state) {
        case 0:
            return u8"A3D: 初期化中...";
        case 1:
            return u8"A3D: 命令待機中...";
        case 2:
            return u8"A3D: データリクエスト中...";
        case 3:
            return u8"A3D: PVステージ読込中...";
        case 4:
            return u8"A3D: A3Dカテゴリ読込中...";
        case 5:
            return u8"A3D: A3Dハンドル読込中...";
        default:
            return u8"A3D: 未知のモード";
        }
}

void DataTestMotA3d::LoadAuth3d(std::string&& name) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    int32_t uid = aft_auth_3d_db->get_uid(name.c_str());
    if (uid == -1)
        return;

    auth_3d_id id = auth_3d_id(uid, aft_auth_3d_db);
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
        objset_info_storage_unload_set(i);

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

    float_t frame = dtm_mot_array[ROB_ID_1P].GetFrame();
    if (get_pause())
        frame -= dtm_mot_array[ROB_ID_1P].GetStep();

    for (auth_3d_id& i : auth_3d_ids)
        i.set_req_frame(frame);
}

DtmMot::DtmMot() : rob_man(), motion(), assign(), rot(), pre_offset(), post_offset(), div(),
loop(), reset_osage_flag(), use_opd(), loaded(), reset_mot(), save_only_start_frame(), state(),
frame(), delta_frame(), looped(), start_frame(), ab_frame(), ab_loop(), set_motion_index() {
    rctrl = ROB_ID_NULL;
    chr = CN_MAX;
    ptr = 502;
    set = -1;
    uid = -1;
    step[0] = 0.0f;
    step[1] = 0.0f;
    step[2] = 0.0f;
    step[3] = 1.0f;
    move = true;
    item = true;
    display = true;
    partial_mot = true;
    pv_id = -1;
    field_100 = true;
}

DtmMot::~DtmMot() {

}

bool DtmMot::init() {
    frame = 0.0f;
    delta_frame = 1.0f;
    state = 1;
    looped = false;

    if (chr < 0 || chr >= CN_MAX || ptr >= 502) {
        state = 0;
        return true;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    if (assign == IDX) {
        uint32_t set = this->set;
        uint32_t uid = this->uid;
        if (aft_mot_db->motion_set.size() <= set) {
            state = 0;
            return true;
        }

        this->set = aft_mot_db->motion_set[set].id;
        if (aft_mot_db->motion_set[set].motion.size() <= uid) {
            state = 0;
            return true;
        }
        this->uid = aft_mot_db->motion_set[set].motion[uid].id;
    }

    pv_id = DtmMot::ConvertMotionSetNameToPVID(aft_mot_db->get_motion_set_name(set));
    loaded = false;
    return true;
}

bool DtmMot::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    switch (state) {
    case 1: {
        if (pv_id >= 0) {
            const pv_db_pv_difficulty* diff = task_pv_db_get_pv_difficulty(
                pv_id, PV_DIFFICULTY_HARD, PV_EDITION_ORIGINAL);
            if (diff) {
                dsc_file_handler.reset();
                dsc_file_handler.read_file(aft_data, diff->script_file_name.c_str(), prj::MemCMax);
            }
        }

        motion_set_load_motion(set, "", aft_mot_db);
        motion_set_load_mothead(set, "", aft_mot_db);
        state = 2;
    } break;
    case 2: {
        if (motion_storage_check_mot_file_not_ready(set)
            || mothead_storage_check_mhd_file_not_ready(set)
            || dsc_file_handler.check_not_ready())
            break;

        const motion_set_info* set_info = aft_mot_db->get_motion_set_by_id(set);
        const size_t motion_count = set_info->motion.size();
        for (size_t i = 0; i < motion_count; i++)
            get_mot_frame_max(set_info->motion.data()[i].id, aft_mot_db);

        state = 3;
    } break;
    case 3: {
        if (rctrl >= 0 && rctrl < ROB_ID_MAX && task_rob_manager_get_wait(rctrl))
            break;

        RobInit rob_init;
        rob_init.rob_type = ROB_TYPE_DATA_TEST;

        module_data data;
        if (module_data_handler_data_get_module(chr, ptr, data)) {
            rob_init.sleeve_l = data.sleeve_l;
            rob_init.sleeve_r = data.sleeve_r;
        }

        rctrl = rob_man->create_rob(chr, rob_init, ptr, true);
        state = 4;
    } break;
    case 4: {
        if (!task_rob_manager_check_chara_loaded(rctrl))
            break;

        motion = rob_man->get_rob_motion_work(rctrl);
        state = 7;
    } break;
    case 5: {
        if (osage_play_data_manager_check_alive())
            break;

        osage_play_data_manager_append_chara_motion_id(rob_man->get_rob(rctrl), uid);
        osage_play_data_manager_open();
        state = 6;
    } break;
    case 6: {
        if (osage_play_data_manager_check_alive() || skin_param_manager_check_alive(rctrl))
            break;

        rob_chara* rob_chr = rob_man->get_rob(rctrl);
        rob_chr->replace_rob_motion(uid, frame, 0.0f, true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chr->set_use_opd(true);
        state = 11;
    } break;
    case 7: {
        loaded = false;
        if (skin_param_manager_check_alive(rctrl)
            || pv_osage_manager_array_get_disp(rctrl)
            || osage_play_data_manager_check_alive())
            break;

        const pv_db_pv_difficulty* diff = task_pv_db_get_pv_difficulty(
            pv_id, PV_DIFFICULTY_HARD, PV_EDITION_ORIGINAL);
        if (pv_id > 0 && diff && dsc_file_handler.get_data()) {
            pv_data.reset();
            pv_data.dsc.parse(dsc_file_handler.get_data(), dsc_file_handler.get_size(), DSC_FT);

            int32_t rctrl = 0;
            for (auto& i : diff->motion)
                for (const pv_db_pv_motion& j : i)
                    if (j.id == uid) {
                        rctrl = (int32_t)(&i - diff->motion);
                        break;
                    }

            pv_data.find_playdata_set_motion(rctrl);
            pv_data.find_set_motion(diff);

            const std::vector<pv_data_set_motion>* set_motion = pv_data.get_set_motion(rctrl);
            if (set_motion) {
                this->set_motion.assign(set_motion->begin(), set_motion->end());

                const MhpList* pp_list = mothead_storage_get_mot_by_motion_id(uid, aft_mot_db)->pp_list;
                if (pp_list && pp_list->type >= 0) {
                    int32_t type = pp_list->type;
                    while (type != MHP_OSAGE_RESET) {
                        pp_list++;
                        type = pp_list->type;
                        if (type < 0)
                            break;
                    }

                    while (true) {
                        if (type < 0)
                            break;

                        auto j_begin = this->set_motion.begin();
                        auto j_end = this->set_motion.end();
                        for (auto j = j_begin; j != j_end; )
                            if ((float_t)pp_list->frame == j->frame_stage_index.first) {
                                std::move(j + 1, j_end, j);
                                this->set_motion.pop_back();
                                j_end = this->set_motion.end();
                            }
                            else
                                j++;

                        pp_list++;
                        type = pp_list->type;
                        if (type < 0)
                            break;

                        while (type != MHP_OSAGE_RESET) {
                            pp_list++;
                            type = pp_list->type;
                            if (type < 0)
                                break;
                        }
                    }
                }
            }
        }

        task_rob_manager_suspend();
        task_wind_suspend();
        reset_mot = false;
        state = 8;
    } break;
    case 8: {
        rob_chara* rob_chr = rob_man->get_rob(rctrl);
        rob_chr->set_use_opd(false);

        skin_param_manager_reset(rctrl);

        std::vector<osage_init_data> vec;
        vec.push_back({ rob_chr });
        vec.push_back({ rob_chr, pv_id, uid });

        const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
        if (pv) {
            for (const pv_db_pv_osage_init& i : pv->osage_init) {
                uint32_t mot_id = aft_mot_db->get_motion_id(i.motion.c_str());
                if (mot_id != -1 && mot_id == uid)
                    vec.push_back({ rob_chr, pv_id, mot_id, "", i.frame });
            }

            for (pv_data_set_motion& i : set_motion)
                vec.push_back({ rob_chr, pv->id, i.motnum, "", (int32_t)prj::roundf(i.frame_stage_index.first) });
        }
        skin_param_manager_open(rctrl, vec);

        state = 9;

        const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
        while (*opd_motion_set_ids != -1) {
            if (*opd_motion_set_ids != set) {
                opd_motion_set_ids++;
                continue;
            }

            if (use_opd)
                state = 5;
            break;
        }
    } break;
    case 9: {
        if (skin_param_manager_check_alive(rctrl))
            break;

        pv_osage_manager_array_reset(rctrl);
        if (save_only_start_frame) {
            pv_data_set_motion motion;
            motion.motnum = uid;
            motion.frame_stage_index.first = start_frame;
            motion.frame_stage_index.second = task_stage_get_current_stage_index();

            std::vector<pv_data_set_motion> set_motion;
            set_motion.push_back(motion);
            pv_osage_manager_array_set_pv_set_motion(rctrl, set_motion);
        }
        else {
            std::vector<uint32_t> motion_ids;
            motion_ids.push_back(uid);
            pv_osage_manager_array_set_motion_ids(rctrl, motion_ids);

            if (pv_id >= 0)
                pv_osage_manager_array_set_pv_set_motion(rctrl, set_motion);
        }

        if (pv_id >= 0) {
            stage_param_data_coli_data_reset();
            stage_param_data_coli_data_load(pv_id);
        }

        pv_osage_manager_array_set_pv_id(rctrl, pv_id, 0);
        state = 10;
    } break;
    case 10: {
        if (pv_osage_manager_array_get_disp())
            break;

        int32_t stage_index = task_stage_get_current_stage_index();
        if (stage_index != -1)
            rob_man->get_rob(rctrl)->set_stage_data_ring(stage_index);
        state = 11;
    } break;
    case 11: {
        loaded = true;
        if (!field_100)
            break;

        task_rob_manager_restart();
        task_wind_restart();
        state = 12;
    } break;
    case 12: {
        frame = start_frame;

        rob_chara* rob_chr = rob_man->get_rob(rctrl);
        rob_chr->set_step_motion_step(1.0f);
        rob_chr->replace_rob_motion(uid, frame, 0.0f, true, true, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);

        set_motion_index = 0;

        if (data_test_face_mot_dw_array_get(rctrl))
            data_test_face_mot_dw_array_get(rctrl)->Reset();
        state = 13;
    }
    case 13: {
        if (reset_mot) {
            state = 7;
            break;
        }
        else if (get_pause())
            break;

        rob_chara* rob_chr = rob_man->get_rob(rctrl);

        float_t frame = this->frame;
        if (frame >= pre_offset)
            frame = min_def(rob_chr->rob_base.motdata.frame, frame - pre_offset);

        motion->motion_step();

        rob_chr->set_frame(frame);
        rob_chr->set_step_motion_step(delta_frame);

        if (looped) {
            rob_chr->sub_140551000();
            set_motion_index = 0;
        }

        rob_chr->sub_1405070E0(aft_bone_data, aft_mot_db);
        rob_chr->arm_adjust_ctrl();

        if (!partial_mot) {
            motion->load_face_motion(-1, aft_mot_db);
            motion->load_hand_l_motion(-1, aft_mot_db);
            motion->load_hand_r_motion(-1, aft_mot_db);
            motion->load_mouth_motion(-1, aft_mot_db);
            motion->load_eyes_motion(-1, aft_mot_db);
            motion->load_eyelid_motion(-1, aft_mot_db);
        }

        CtrlFaceMot();

        motion->interpolate();

        mat4 mat;
        mat4_rotate_zyx(rot[0], rot[1], rot[2], &mat);
        mat4_mul_rotate_y(&mat, rot[3], &mat);
        mat4_set_translation(&mat, &pos);
        motion->update(&mat);

        rob_chr->adjust_ctrl();
        rob_chr->set_data_adjust_mat(&rob_chr->rob_base.adjust);
        rob_chr->rob_motion_modifier_ctrl();
        rob_chr->calc_rob_colli_matrix();

        if (data_test_mot_dw_array_get(rctrl)) {
            data_test_mot_dw_array_get(rctrl)->SetFrameSlider(GetFrame(),
                GetFrameCount() + pre_offset + post_offset);
            data_test_mot_dw_array_get(rctrl)->SetFrameLabel(GetFrame(), GetFrameCount());
        }

        if (reset_osage_flag)
            reset_osage_flag = false;

        if (rob_man->get_disp_on(rctrl) != display)
            rob_man->set_disp_on(rctrl, display);

        rob_chr->rob_info_ctrl();

        while (set_motion_index < set_motion.size()) {
            if (set_motion.data()[set_motion_index].motnum != uid) {
                set_motion_index++;
                continue;
            }

            float_t frame = (float_t)(int32_t)prj::roundf(
                set_motion.data()[set_motion_index].frame_stage_index.first);
            if (frame > this->frame)
                break;

            rob_chr->set_motion_reset_data(uid, frame);
            rob_chr->set_motion_skin_param(uid, frame);
            set_motion_index++;
        }
    } break;
    }
    return false;
}

bool DtmMot::dest() {
    if (!state)
        return true;

    pv_osage_manager_array_set_not_reset_true();
    if (skin_param_manager_check_alive(rctrl)
        || pv_osage_manager_array_get_disp()
        || osage_play_data_manager_check_alive())
        return false;

    task_rob_manager_restart();
    task_wind_restart();
    if (rob_man->get_rob(rctrl))
        rob_man->dest_rob(rctrl);
    motion_set_unload_motion(set);
    motion_set_unload_mothead(set);
    skin_param_manager_reset(rctrl);
    stage_param_data_coli_data_reset();
    osage_play_data_manager_reset();
    set_motion.clear();
    set_motion_index = 0;
    dsc_file_handler.reset();
    pv_data.reset();
    motion = 0;
    state = 1;
    return true;
}

inline void print_string(PrintWork& print_work, vec2 position, vec2 offset, const char* fmt, const char* str) {
    position += offset;
    print_work.set_position(position.x, position.y + 8.0f);
    print_work.printf_align_left(fmt, str);
}

inline void print_vector(PrintWork& print_work, vec2 position, vec2 offset, const char* fmt, const vec3 vec) {
    position += offset;
    print_work.set_position(position.x, position.y + 8.0f);
    print_work.printf_align_left(fmt, vec.x, vec.y, vec.z);
}

void rob_block_disp(int32_t rctrl, RobBlock* block, BONE_BLK blk, int32_t motion_body_type) {
    return;
    static bool none = false;

    font_info font;
    font.init_font_data(0);
    font.set_glyph_size(font.glyph.x * 0.75f, font.glyph.y * 0.75f);

    PrintWork print_work;
    print_work.set_font(&font);
    print_work.set_prio(spr::SPR_PRIO_DW);
    print_work.set_screen_mode(SCREEN_MODE_MAX);
    print_work.set_color(none ? 0 : color_red);

    switch (block->ik_type) {
    case IKT_0:
    case IKT_0N:
    case IKT_0T: {
        mat4 v92 = *block->node[0].mat_ptr;
        mat4_scale_rot(&v92, 0.1f, &v92);
        debug_put_line_axis(v92);

        vec3 v93;
        vec3 v91;
        mat4_get_translation(&v92, &v93);
        mat4_get_translation(&v92, &v91);

        vec2 v80 = project_screen(v91, true);
        print_string(print_work, v80, vec2(0.0f, 16.0f * 0), "%s", block->node[0].name);
        print_vector(print_work, v80, vec2(0.0f, 16.0f * 1),
            "gbl:%.3f %.3f %.3f", v91);
        //print_vector(print_work, v80, vec2(0.0f, 16.0f * 2),
        //    "lcl:%.4f %.4f %.4f", block->chain_pos[0]);
        print_vector(print_work, v80, vec2(0.0f, 16.0f * 2),
            "rot:%.3f %.3f %.3f", block->node[0].transform.rot * RAD_TO_DEG_FLOAT);

        if (blk == BLK_TL_UP_KATA_L || blk == BLK_TL_UP_KATA_R) {
            vec3 v87;
            mat4_get_translation(block[BLK_C_KATA_L - BLK_TL_UP_KATA_L].node[2].mat_ptr, &v87);

            vec2 v76 = project_screen(v87, true);
            vec2 v75 = project_screen(v93, true);
            spr::putLine(v76, v75, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_cyan);
        }
    } break;
    case IKT_1: {
        mat4 v92 = *block->node[1].mat_ptr;

        mat4 v84 = *block->node[0].mat_ptr;
        mat4 v84a;
        mat4_scale_rot(&v84, 0.15f, &v84a);
        debug_put_line_axis(v84a, color_dark_red, color_dark_green, color_dark_blue);

        vec3 v30;
        mat4_inverse_transform_point(&v84, &block->leaf_pos[0], &v30);
        mat4_clear_rot(&v84, &v84);
        mat4_translate(&v30, &v84);
        mat4_scale_rot(&v84, 0.1f, &v84);
        debug_put_line_axis(v84, color_green, color_cyan, color_white);

        vec2 v76a = project_screen(0.0f, true);
        vec2 v75a = project_screen(v30, true);
        spr::putLine(v76a, v75a, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_yellow);

        mat4 v88;
        mat4_mul_translate_x(&v92, block->len[0][motion_body_type], &v88);

        vec3 v91;
        vec3 v90;
        vec3 v93;
        mat4_get_translation(&v88, &v91);
        mat4_get_translation(&v92, &v90);
        mat4_get_translation(&v88, &v93);

        vec2 v79 = project_screen(v90, true);
        vec2 v78 = project_screen(v91, true);
        spr::putRect({ v78 - 2.0f, 4.0f }, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_red);
        spr::putLine(v79, v78, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_white);

        mat4_scale_rot(&v92, 0.1f, &v92);
        debug_put_line_axis(v92);

        vec2 v77 = project_screen(v90, true);

        print_work.set_color(color_yellow);
        print_string(print_work, v77, vec2(0.0f, 16.0f * 0), "%s", block->node[0].name);
        print_vector(print_work, v77, vec2(0.0f, 16.0f * 1),
            "gbl:%.4f %.4f %.4f", v90);
        //print_vector(print_work, v77, vec2(0.0f, 16.0f * 2),
        //    "lcl:%.4f %.4f %.4f", block->chain_pos[0]);
        print_vector(print_work, v77, vec2(0.0f, 16.0f * 2),
            "rot:%.3f %.3f %.3f", block->node[0].transform.rot * RAD_TO_DEG_FLOAT);
        print_work.set_color(none ? 0 : color_red);

        v77 = project_screen((v91 + v90) * 0.5f, true);
        print_string(print_work, v77, vec2(0.0f, 16.0f * 0), "%s", block->node[1].name);
        print_vector(print_work, v77, vec2(0.0f, 16.0f * 1),
            "rot:%.3f %.3f %.3f", block->node[1].transform.rot * RAD_TO_DEG_FLOAT);

        v93 = block->leaf_pos[0];
        v77 = project_screen(v93, true);
        print_string(print_work, v77, vec2(0.0f, 16.0f * 0), "%s", block->node[2].name);
        print_vector(print_work, v77, vec2(0.0f, 16.0f * 1), "gbl:%.3f %.3f %.3f", v93);
        spr::putRect({ v77 - 2.0f, 4.0f }, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_blue);
    } break;
    case IKT_2:
    case IKT_2R: {
        mat4 v84 = *block->node[0].mat_ptr;
        vec3 v30;
        mat4_inverse_transform_point(&v84, &block->leaf_pos[0], &v30);

        mat4 v84a;
        mat4_scale_rot(&v84, 0.15f, &v84a);
        debug_put_line_axis(v84a, color_dark_red, color_dark_green, color_dark_blue);

        mat4_clear_rot(&v84, &v84);
        mat4_translate(&v30, &v84);
        mat4_scale_rot(&v84, 0.1f, &v84);
        debug_put_line_axis(v84, color_green, color_cyan, color_white);

        vec2 v76a = project_screen(0.0f, true);
        vec2 v75a = project_screen(v30, true);
        spr::putLine(v76a, v75a, SCREEN_MODE_MAX, spr::SPR_PRIO_DW,
            block->ik_type == IKT_2 ? color_cyan : color_magenta);

        mat4 v92 = *block->inherit_mat_ptr;
        mat4 v88 = *block->node[1].mat_ptr;
        mat4 v82 = *block->node[2].mat_ptr;
        mat4 v81;
        mat4_mul_translate_x(&v82, block->len[1][motion_body_type], &v81);

        vec3 v87;
        vec3 v90;
        vec3 v93;
        mat4_get_translation(&v88, &v87);
        mat4_get_translation(&v82, &v90);
        mat4_get_translation(&v81, &v93);

        vec2 v74 = project_screen(v87, true);

        print_work.set_color(color_green);
        print_string(print_work, v74, vec2(0.0f, 16.0f * 0), "%s", block->node[0].name);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 1),
            "gbl:%.4f %.4f %.4f", v87);
        //print_vector(print_work, v74, vec2(0.0f, 16.0f * 2),
        //    "lcl:%.4f %.4f %.4f", block->chain_pos[0]);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 2),
            "lcl:%.4f %.4f %.4f", v30);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 3),
            "rot:%.3f %.3f %.3f", block->node[0].transform.rot * RAD_TO_DEG_FLOAT);
        print_work.set_color(none ? 0 : color_red);

        v74 = project_screen((v90 + v87) * 0.5f, true);

        print_string(print_work, v74, vec2(0.0f, 16.0f * 0), "%s", block->node[1].name);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 1),
            "rot:%.3f %.3f %.3f", block->node[1].transform.rot * RAD_TO_DEG_FLOAT);

        v74 = project_screen((v93 + v90) * 0.5f, true);
        print_string(print_work, v74, vec2(0.0f, 16.0f * 0), "%s", block->node[2].name);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 1),
            "rot:%.3f %.3f %.3f", block->node[2].transform.rot * RAD_TO_DEG_FLOAT);

        vec2 v76 = project_screen(v87, true);
        vec2 v75 = project_screen(v90, true);
        spr::putLine(v76, v75, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_white);

        mat4_get_translation(&v81, &v87);

        v76 = project_screen(v87, true);
        spr::putLine(v76, v75, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_white);
        spr::putRect({ v76 - 2.0f, 4.0f }, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_red);

        mat4_scale_rot(&v88, 0.1f, &v88);
        debug_put_line_axis(v88);

        mat4_scale_rot(&v82, 0.1f, &v82);
        debug_put_line_axis(v82);

        v93 = block->leaf_pos[0];
        v74 = project_screen(v93, true);
        print_string(print_work, v74, vec2(0.0f, 16.0f * 0), "%s", block->node[3].name);
        print_vector(print_work, v74, vec2(0.0f, 16.0f * 1), "gbl:%.4f %.4f %.4f", v93);
        spr::putRect({ v74 - 2.0f, 4.0f }, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, color_blue);
    } break;
    }

    font = font_info(16);
    font.set_glyph_size(font.glyph.x * 0.7f, font.glyph.y * 0.7f);
    print_work.set_font(&font);
    print_work.set_position({ 0.0f, 720.0f - (rctrl + 1) * font.glyph.y * 4 });
    print_work.set_color(color_white);
    if (blk == BLK_CL_MUNE) {
        vec3 leaf_pos;
        mat4_inverse_transform_point(block->node[0].mat_ptr, &block->leaf_pos[0], &leaf_pos);
        print_work.printf_align_left("%dP %.5f\n", rctrl + 1, vec3::length(leaf_pos));
    }
    else if (blk == BLK_CL_KAO) {
        vec3 leaf_pos;
        mat4_inverse_transform_point(block->node[0].mat_ptr, &block->leaf_pos[0], &leaf_pos);
        print_work.printf_align_left("           %.5f\n", vec3::length(leaf_pos));
    }
    else if (blk == BLK_C_KATA_L || blk == BLK_C_KATA_R) {
        vec3 tl_up_kata_pos;
        mat4_get_translation(block[-1].node[0].mat_ptr, &tl_up_kata_pos);

        vec3 leaf_pos;
        mat4_inverse_transform_point(block->node[0].mat_ptr, &block->leaf_pos[0], &leaf_pos);

        vec3 pos_j_kata_wj;
        vec3 pos_j_ude_wj;
        vec3 pos_j_te_wj;
        mat4_get_translation(block->node[1].mat_ptr, &pos_j_kata_wj);
        mat4_get_translation(block->node[2].mat_ptr, &pos_j_ude_wj);
        mat4_get_translation(block->node[3].mat_ptr, &pos_j_te_wj);

        float_t arm_length = block->len[0][1] + block->len[1][1];
        vec3 pos_j_ude_wj_mid = vec3::lerp(pos_j_kata_wj, pos_j_te_wj,
            block->len[0][1] / arm_length);

        float_t len1 = vec3::distance(pos_j_ude_wj, tl_up_kata_pos);
        float_t len2 = vec3::distance(pos_j_ude_wj_mid, tl_up_kata_pos);
        float_t len3 = vec3::distance(pos_j_ude_wj, pos_j_ude_wj_mid);
        float_t len4 = vec3::distance(pos_j_kata_wj, pos_j_te_wj);
        float_t len5 = vec3::length(leaf_pos);

        if (blk == BLK_C_KATA_L) {
            print_work.printf_align_left("\nL: %.5f %.5f %.5f %.5f", len1, len2, len3, len4);
            if (len5 >= arm_length)
                print_work.set_color(color_red);
            print_work.printf_align_left(" %.5f\n\n", len5);
        }
        else {
            print_work.printf_align_left("\n\nR: %.5f %.5f %.5f %.5f", len1, len2, len3, len4);
            if (len5 >= arm_length)
                print_work.set_color(color_red);
            print_work.printf_align_left(" %.5f\n", len5);
        }
        print_work.set_color(color_white);
    }
    else if (blk == BLK_CL_MOMO_L || blk == BLK_CL_MOMO_R) {
        vec3 pos_j_momo_wj;
        vec3 pos_j_sune_l_wj;
        vec3 pos_e_sune_l_cp;
        mat4_get_translation(block->node[1].mat_ptr, &pos_j_momo_wj);
        mat4_get_translation(block->node[2].mat_ptr, &pos_j_sune_l_wj);
        mat4_get_translation(block->node[3].mat_ptr, &pos_e_sune_l_cp);

        float_t leg_length = block->len[0][1] + block->len[1][1];
        vec3 pos_j_sune_l_wj_mid = vec3::lerp(pos_j_momo_wj, pos_e_sune_l_cp,
            block->len[0][1] / leg_length);

        float_t len1 = vec3::distance(pos_j_sune_l_wj, pos_j_sune_l_wj_mid);
        float_t len2 = vec3::distance(pos_j_momo_wj, pos_e_sune_l_cp);
        float_t len3 = vec3::distance(pos_j_momo_wj, block->leaf_pos[0]);

        if (blk == BLK_CL_MOMO_L) {
            print_work.printf_align_left("\n\n\nL: %.5f %.5f", len1, len2);
            if (len3 >= leg_length)
                print_work.set_color(color_red);
            print_work.printf_align_left(" %.5f\n\n", len3);
        }
        else {
            print_work.printf_align_left("\n\n\n                           R: %.5f %.5f", len1, len2);
            if (len3 >= leg_length)
                print_work.set_color(color_red);
            print_work.printf_align_left(" %.5f\n", len3);
        }
        print_work.set_color(color_white);
    }
}

void DtmMot::post() {
    if (state != 13)
        return;

    int32_t step_index = 3;
    for (int32_t i = 0; i < 4; i++)
        if (frame < div[i]) {
            step_index = i;
            break;
        }

    float_t frame = this->frame;

    delta_frame = get_delta_frame() * step[step_index];
    looped = false;

    if (move)
        frame += delta_frame;

    rob_chara* rob_chr = rob_man->get_rob(rctrl);

    float_t begin = 0.0f;
    float_t end = rob_chr->rob_base.motdata.frame;
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
            frame += rob_chr->rob_base.motdata.loop_begin;
        }
        else
            frame = end;
    }

    this->frame = frame;
}

bool DtmMot::open(CHARA_NUM chr, int32_t ptr, uint32_t set, uint32_t uid) {
    if (check_entry() || check_alive())
        return true;

    this->chr = chr;
    this->ptr = ptr;
    this->uid = uid;
    this->set = set;
    assign = IDX;
    return app::Task::open("DATA_TEST_MOTION_MANAGER", app::TASK_PRIO_HIGH);
}

bool DtmMot::open(CHARA_NUM chr, int32_t ptr, uint32_t uid) {
    if (check_entry() || check_alive())
        return true;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    this->chr = chr;
    this->ptr = ptr;
    this->uid = uid;
    this->set = aft_mot_db->get_motion_set_id_by_motion_id(uid);
    assign = UID;
    return app::Task::open("DATA_TEST_MOTION_MANAGER", app::TASK_PRIO_HIGH);
}

bool DtmMot::close() {
    if (check_alive())
        return app::Task::close();
    return false;
}

bool DtmMot::CheckFirstFrame() {
    if (motion)
        return false;

    rob_chara* rob_chr = rob_man->get_rob(rctrl);
    if (!rob_chr)
        return false;

    float_t frame_max = rob_chr->rob_base.motdata.frame + pre_offset + post_offset;
    if (frame_max > 0.0f)
        return fabsf(frame_max - frame) <= 0.000001f;
    return false;
}

void DtmMot::CtrlFaceMot() {
    DataTestFaceMotDw* face_mot_dw = data_test_face_mot_dw_array_get(rctrl);
    if (!face_mot_dw || !face_mot_dw->GetEnable())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    DataTestFaceMotDw::Data data = face_mot_dw->GetData();
    rob_chara* rob_chr = rob_man->get_rob(rctrl);
    rob_chr->set_face_mottbl_motion(0, data.face.mottbl_type, data.face.frame * 0.01f,
        ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, false, aft_mot_db);
    rob_chr->set_eyelid_mottbl_motion(0, data.eyelid.mottbl_type, data.eyelid.frame * 0.01f,
        ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    rob_chr->set_eyes_mottbl_motion(0, data.eyes.mottbl_type, data.eyes.frame * 0.01f,
        ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    rob_chr->set_mouth_mottbl_motion(0, data.mouth.mottbl_type, data.mouth.frame * 0.01f,
        ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
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
    if (state == 13 && motion)
        return motion->get_frame_max();
    return 0.0f;
}

bool DtmMot::GetLoaded() {
    if (state)
        return loaded;
    return true;
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
    reset_osage_flag = true;
}

void DtmMot::SetDisp(bool value) {
    display = value;
}

void DtmMot::SetDivide(int32_t index, float_t value) {
    if (index < 3)
        div[index] = value;
}

void DtmMot::SetFrame(float_t value) {
    frame = value;
}

void DtmMot::SetLoop(bool value) {
    loop = value;
}

void DtmMot::SetMotion(uint32_t set, uint32_t uid) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    state = 0;

    if (set >= aft_mot_db->motion_set.size())
        return;

    const motion_set_info* set_info = &aft_mot_db->motion_set[set];
    this->set = set_info->id;

    if (uid >= set_info->motion.size())
        return;

    state = 7;
    frame = 0.0f;
    this->uid = set_info->motion[uid].id;
}

void DtmMot::SetOffset(float_t pre_offset, float_t post_offset) {
    this->pre_offset = pre_offset;
    this->post_offset = post_offset;
}

void DtmMot::SetPlay(bool value) {
    move = value;
}

void DtmMot::SetPartialMot(bool value) {
    partial_mot = value;
}

void DtmMot::SetResetMot() {
    reset_mot = true;
}

void DtmMot::SetRotationY(float_t value) {
    rot[1] = value;
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
    pos = value;
}

void DtmMot::SetUseOpd(bool value) {
    use_opd = value;
    SetResetMot();
}

void DtmMot::ToggleABLoop() {
    if (!ab_loop)
        UpdateABFrame();

    ab_loop = !ab_loop;
}

void DtmMot::UpdateABFrame() {
    rob_chara* rob_chr = rob_man->get_rob(rctrl);
    if (!rob_chr)
        return;

    float_t frame = rob_chr->rob_base.motdata.frame;
    for (float_t& i : ab_frame)
        i = clamp_def(i, 0.0f, frame);

    if (ab_frame[0] < ab_frame[1])
        return;

    bool found = false;
    float_t a_frame = ab_frame[0];
    int32_t b_frame = (int32_t)frame;

    for (pv_data_set_motion& i : set_motion)
        if (i.motnum == uid && i.frame_stage_index.first > a_frame) {
            if ((int32_t)i.frame_stage_index.first) {
                b_frame = (int32_t)i.frame_stage_index.first;
                found = true;
            }
            break;
        }

    if (!found)
        b_frame = (int32_t)frame;

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

void DtmMot::sub_1402922C0(bool value) {
    field_100 = value;
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

bool dtm_mot_array_get_loaded() {
    return dtm_mot_array[ROB_ID_1P].GetLoaded() && dtm_mot_array[ROB_ID_2P].GetLoaded();
}

void dtm_mot_array_set_reset_mot() {
    dtm_mot_array[ROB_ID_1P].SetResetMot();
    dtm_mot_array[ROB_ID_2P].SetResetMot();
}

DataTestFaceMotDw::DataTestFaceMotDw(ROB_ID id) {
    rob_id = id;

    SetText("Face motion");

    enable = new dw::Button(this, dw::CHECKBOX);
    enable->SetText("Enable");
    enable->SetValue(false);

    dw::Group* motion_group = new dw::Group(this);
    motion_group->SetLayout(new dw::GridLayout(3));
    motion_group->SetText("Motion");

    (new dw::Label(motion_group))->SetText("FACE  ");

    face = InitAddMottblMapMotions(motion_group, 0);

    face_frame = dw::Slider::Create(motion_group);
    face_frame->SetText("Frame");
    face_frame->format = "%4.0f";
    face_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("FACE_CL  ");

    face_cl = new dw::Label(motion_group);
    face_cl->SetText("");

    face_cl_frame = dw::Slider::Create(motion_group);
    face_cl_frame->SetText("Frame");
    face_cl_frame->format = "%4.0f";
    face_cl_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("EYES  ");

    eyes = InitAddMottblMapMotions(motion_group, 1);

    eyes_frame = dw::Slider::Create(motion_group);
    eyes_frame->SetText("Frame");
    eyes_frame->format = "%4.0f";
    eyes_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    (new dw::Label(motion_group))->SetText("MOUTH ");

    mouth = InitAddMottblMapMotions(motion_group, 2);

    mouth_frame = dw::Slider::Create(motion_group);
    mouth_frame->SetText("Frame");
    mouth_frame->format = "%4.0f";
    mouth_frame->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);

    UpdateLayout();
}

DataTestFaceMotDw::~DataTestFaceMotDw() {

}

void DataTestFaceMotDw::Hide() {
    data_test_face_mot_dw_array[rob_id] = 0;
    dw::Shell::Hide();
}

void DataTestFaceMotDw::AddMottblMapMotions(dw::ListBox* list_box, int32_t type) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    size_t selected_item = list_box->list->selected_item;
    if (selected_item >= list_box->list->items.size())
        selected_item = 0;

    list_box->ClearItems();

    rob_chara* rob_chr = get_rob_management()->get_rob(rob_id);
    if (!rob_chr)
        return;

    switch (type) {
    case 0:
        for (uint32_t i = MTP_FACE_NULL; i <= MTP_FACE_NEW_IKARI_OLD_CL; i++) {
            uint32_t uid = rob_chr->get_common_mot((MOTTABLE_TYPE)i);
            if (uid != -1) {
                if (motion_id_mottbl_map.insert({ uid, (MOTTABLE_TYPE)i }).second) {
                    std::string motion_name(aft_mot_db->get_motion_name(uid));
                    if (motion_name.rfind("_CL") != motion_name.size() - 3)
                        list_box->AddItem(motion_name);
                }
            }
        }
        break;
    case 1:
        for (uint32_t i = MTP_EYES_NULL; i <= MTP_EYES_MOVE_UR_DL_OLD; i++) {
            uint32_t uid = rob_chr->get_common_mot((MOTTABLE_TYPE)i);
            if (uid != -1) {
                motion_id_mottbl_map.insert({ uid, (MOTTABLE_TYPE)i });
                list_box->AddItem(aft_mot_db->get_motion_name(uid));
            }
        }
        break;
    case 2:
        for (uint32_t i = MTP_KUCHI_NULL; i <= MTP_KUCHI_NEUTRAL; ++i) {
            uint32_t uid = rob_chr->get_common_mot((MOTTABLE_TYPE)i);
            if (uid != -1) {
                motion_id_mottbl_map.insert({ uid, (MOTTABLE_TYPE)i });
                list_box->AddItem(aft_mot_db->get_motion_name(uid));
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
        { MTP_FACE_NULL , 0.0f },
        { MTP_FACE_NULL , 0.0f },
        { MTP_EYES_NULL , 0.0f },
        { MTP_KUCHI_NULL, 0.0f },
    };

    std::string face_mot = face->GetSelectedItemStr();
    GetMottblIndexFrame(data.face, aft_mot_db->get_motion_id(
        face_mot.c_str()), face_frame->GetValue());

    std::string face_cl_mot;
    if (face_mot.find("FACE_WINK_OLD") != -1)
        face_cl_mot.assign("FACE_WINK_FT_OLD_CL");
    else {
        face_cl_mot.assign(face_mot);
        face_cl_mot.append("_CL");
    }

    uint32_t face_cl_mot_id = aft_mot_db->get_motion_id(face_cl_mot.c_str());
    face_cl->SetText(face_cl_mot_id != -1 ? face_cl_mot : u8"CLモーションなし");

    GetMottblIndexFrame(data.eyelid, face_cl_mot_id, face_cl_frame->GetValue());
    GetMottblIndexFrame(data.eyes, aft_mot_db->get_motion_id(
        eyes->GetSelectedItemStr().c_str()), eyes_frame->GetValue());
    GetMottblIndexFrame(data.mouth, aft_mot_db->get_motion_id(
        mouth->GetSelectedItemStr().c_str()), mouth_frame->GetValue());

    return data;
}

bool DataTestFaceMotDw::GetEnable() {
    return enable->value;
}

inline void DataTestFaceMotDw::GetMottblIndexFrame(
    DataTestFaceMotDw::MotionData& data, uint32_t uid, float_t value) {
    auto elem = motion_id_mottbl_map.find(uid);
    if (elem != motion_id_mottbl_map.end())
        data.mottbl_type = elem->second;
    data.frame = value;
}

dw::ListBox* DataTestFaceMotDw::InitAddMottblMapMotions(dw::Composite* parent, int32_t type) {
    dw::ListBox* list_box = new dw::ListBox(parent);
    list_box->SetMaxItems(20);
    AddMottblMapMotions(list_box, type);
    return list_box;
}

void DataTestFaceMotDw::ResetMot() {
    motion_id_mottbl_map.clear();
    AddMottblMapMotions(face, 0);
    AddMottblMapMotions(eyes, 1);
    AddMottblMapMotions(mouth, 2);
}

DataTestMotDw::StepSliderProc::StepSliderProc() : index() {

}

DataTestMotDw::StepSliderProc::~StepSliderProc() {

}

void DataTestMotDw::StepSliderProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        test_mot_data->step[slider->callback_data.i32].array[index] = slider->GetValue();
}

DataTestMotDw::CharaListBoxProc::CharaListBoxProc() : list_box() {

}

DataTestMotDw::CharaListBoxProc::~CharaListBoxProc() {

}

void DataTestMotDw::CharaListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        ROB_ID rob_id = (ROB_ID)list_box->callback_data.i32;
        CHARA_NUM chr = (CHARA_NUM)list_box->list->selected_item;
        test_mot_data->chr[rob_id] = chr;

        DataTestMotDw* test_mot_dw = data_test_mot_dw_array_get(rob_id);
        if (test_mot_dw->set_change_button->value) {
            data_struct* aft_data = &data_list[DATA_AFT];
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            uint32_t set_id = get_rob_data(chr)->motfile;
            uint32_t set_index = -1;
            for (const motion_set_info& i : aft_mot_db->motion_set)
                if (i.id == set_id) {
                    set_index = (uint32_t)(&i - aft_mot_db->motion_set.data());
                    break;
                }

            test_mot_dw->c_type_list_box_proc.list_box->SetItemIndex(set_index);
            test_mot_data->set[rob_id] = set_index;
        }

        test_mot_dw->AddModules(rob_id, test_mot_dw->chara_list_box_proc.list_box);
    }
}

DataTestMotDw::CTypeListBoxProc::CTypeListBoxProc() : list_box() {

}

DataTestMotDw::CTypeListBoxProc::~CTypeListBoxProc() {

}

void DataTestMotDw::CTypeListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        std::string str = list_box->GetSelectedItemStr();
        size_t start = str.find('(');
        if (start != -1) {
            start += 4;

            size_t end = str.find(')');
            if (end != -1)
                test_mot_data->ptr[list_box->callback_data.i32] =
                atoi(str.substr(start, end - start).c_str()) - 1;
        }
    }
}

DataTestMotDw::SetListBoxProc::SetListBoxProc() : list_box() {

}

DataTestMotDw::SetListBoxProc::~SetListBoxProc() {

}

void DataTestMotDw::SetListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        ROB_ID rob_id = (ROB_ID)list_box->callback_data.i32;
        test_mot_data->set[rob_id] = (int32_t)list_box->list->selected_item;
        data_test_mot_dw_array_get(rob_id)->ResetFrame();
    }
}

DataTestMotDw::IdListBoxProc::IdListBoxProc() {

}

DataTestMotDw::IdListBoxProc::~IdListBoxProc() {

}

void DataTestMotDw::IdListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        ROB_ID rob_id = (ROB_ID)list_box->callback_data.i32;
        test_mot_data->uid[rob_id] = (int32_t)list_box->list->selected_item;
        data_test_mot_dw_array_get(rob_id)->ResetFrame();
    }
}

DataTestMotDw::RotateSliderProc::RotateSliderProc() {

}

DataTestMotDw::RotateSliderProc::~RotateSliderProc() {

}

void DataTestMotDw::RotateSliderProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        test_mot_data->rot_y[slider->callback_data.i32] = slider->GetValue() * DEG_TO_RAD_FLOAT;
}

DataTestMotDw::PositionSliderProc::PositionSliderProc() {

}

DataTestMotDw::PositionSliderProc::~PositionSliderProc() {

}

void DataTestMotDw::PositionSliderProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        test_mot_data->trans_x[slider->callback_data.i32] = slider->GetValue();
}

DataTestMotDw::FrameSliderProc::FrameSliderProc() {

}

DataTestMotDw::FrameSliderProc::~FrameSliderProc() {

}

void DataTestMotDw::FrameSliderProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        ROB_ID rob_id = (ROB_ID)slider->callback_data.i32;
        float_t frame = slider->GetValue();
        data_test_mot_dw_array_get(rob_id)->dtm_mot->SetFrame(frame);
        if (test_mot_data->sync_frame) {
            dtm_mot_array[ROB_ID_1P].SetFrame(frame);
            dtm_mot_array[ROB_ID_2P].SetFrame(frame);
        }
    }
}

DataTestMotDw::StartFrameSliderProc::StartFrameSliderProc() {

}

DataTestMotDw::StartFrameSliderProc::~StartFrameSliderProc() {

}

void DataTestMotDw::StartFrameSliderProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        test_mot_data->start_frame[slider->callback_data.i32] = slider->GetValue();
}

DataTestMotDw::DispButtonProc::DispButtonProc() {

}

DataTestMotDw::DispButtonProc::~DispButtonProc() {

}

void DataTestMotDw::DispButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        ROB_ID rob_id = (ROB_ID)button->callback_data.i32;
        data_test_mot_dw_array_get(rob_id)->dtm_mot->SetDisp(button->value);
    }
}

DataTestMotDw::UseOpdButtonProc::UseOpdButtonProc() {

}

DataTestMotDw::UseOpdButtonProc::~UseOpdButtonProc() {

}

void DataTestMotDw::UseOpdButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        ROB_ID rob_id = (ROB_ID)button->callback_data.i32;
        data_test_mot_dw_array_get(rob_id)->dtm_mot->SetUseOpd(button->value);
    }
}

DataTestMotDw::PartialMotButtonProc::PartialMotButtonProc() {

}

DataTestMotDw::PartialMotButtonProc::~PartialMotButtonProc() {

}

void DataTestMotDw::PartialMotButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        ROB_ID rob_id = (ROB_ID)button->callback_data.i32;
        data_test_mot_dw_array_get(rob_id)->dtm_mot->SetPartialMot(button->value);
    }
}

DataTestMotDw::SaveOnlyStartFrameButtonProc::SaveOnlyStartFrameButtonProc() {

}

DataTestMotDw::SaveOnlyStartFrameButtonProc::~SaveOnlyStartFrameButtonProc() {

}

void DataTestMotDw::SaveOnlyStartFrameButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        ROB_ID rob_id = (ROB_ID)button->callback_data.i32;
        data_test_mot_dw_array_get(rob_id)->dtm_mot->SetSaveOnlyStartFrame(button->value);
    }
}

DataTestMotDw::CreateFaceMotDwProc::CreateFaceMotDwProc() {

}

DataTestMotDw::CreateFaceMotDwProc::~CreateFaceMotDwProc() {

}

void DataTestMotDw::CreateFaceMotDwProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        ROB_ID rob_id = (ROB_ID)button->callback_data.i32;
        data_test_face_mot_dw_array_init(rob_id);
    }
}

DataTestMotDw::CreateEqDwProc::CreateEqDwProc() {

}

DataTestMotDw::CreateEqDwProc::~CreateEqDwProc() {

}

void DataTestMotDw::CreateEqDwProc::Callback(dw::SelectionListener::CallbackData* data) {
    data_test_equip_dw_init();
}

DataTestMotDw::CreateDebugCamProc::CreateDebugCamProc() {

}

DataTestMotDw::CreateDebugCamProc::~CreateDebugCamProc() {

}

void DataTestMotDw::CreateDebugCamProc::Callback(dw::SelectionListener::CallbackData* data) {
    //set_debug_camera_set_mode_lock();
}

DataTestMotDw::DataTestMotDw(ROB_ID id, DtmMot* dtm_mot) {
    this->dtm_mot = dtm_mot;

    DataTestMot::Data* test_mot_data = data_test_mot_data_get();

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "MOTION TEST %dP", id + 1);
    SetText(buf);

    dw::Composite* chara_comp = new dw::Composite(this);
    chara_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* chara_label = new dw::Label(chara_comp);
    chara_label->SetText("CHARA   ");
    chara_label->SetFont(dw::p_font_type_6x12);

    dw::ListBox* chara = new dw::ListBox(chara_comp);
    chara->SetMaxItems(20);
    for (int32_t i = 0; i < CN_MAX; i++)
        chara->AddItem(get_chara_name_full((CHARA_NUM)i));
    chara->callback_data.i32 = id;
    chara->SetItemIndex(test_mot_data->chr[id]);
    chara->AddSelectionListener(&chara_list_box_proc);
    chara->SetFont(dw::p_font_type_6x12);

    dw::Composite* chara_list_box_comp = new dw::Composite(this);
    chara_list_box_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    chara_list_box_proc.list_box = new dw::ListBox(chara_list_box_comp);
    chara_list_box_proc.list_box->SetMaxItems(40);
    chara_list_box_proc.list_box->SetFont(dw::p_font_type_6x12);
    AddModules(id, chara_list_box_proc.list_box);
    chara_list_box_proc.list_box->callback_data.i32 = id;

    chara_list_box_proc.list_box->SetItemIndex(test_mot_data->ptr[id]);
    chara_list_box_proc.list_box->AddSelectionListener(&c_type_list_box_proc);
    chara_list_box_proc.list_box->SetFont(dw::p_font_type_6x12);

    dw::Composite* set_comp = new dw::Composite(this);
    set_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* set_label = new dw::Label(set_comp);
    set_label->SetText("SET     ");
    set_label->SetFont(dw::p_font_type_6x12);

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    c_type_list_box_proc.list_box = new dw::ListBox(set_comp);
    for (motion_set_info& i : aft_mot_db->motion_set)
        c_type_list_box_proc.list_box->AddItem(i.name);
    c_type_list_box_proc.list_box->SetMaxItems(40);
    c_type_list_box_proc.list_box->callback_data.i32 = id;

    c_type_list_box_proc.list_box->SetItemIndex(test_mot_data->set[id]);
    c_type_list_box_proc.list_box->AddSelectionListener(&set_list_box_proc);
    c_type_list_box_proc.list_box->SetFont(dw::p_font_type_6x12);

    dw::Composite* id_comp = new dw::Composite(this);
    id_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* id_label = new dw::Label(id_comp);
    id_label->SetText("ID      ");
    id_label->SetFont(dw::p_font_type_6x12);

    set_list_box_proc.list_box = new dw::ListBox(id_comp);
    set_list_box_proc.list_box->SetMaxItems(40);
    set_list_box_proc.list_box->callback_data.i32 = id;
    set_list_box_proc.list_box->AddSelectionListener(&id_list_box_proc);
    set_list_box_proc.list_box->SetFont(dw::p_font_type_6x12);

    rotate_slider = dw::Slider::Create(this);
    rotate_slider->SetText("Y ROT");
    rotate_slider->format = "%3.0f";
    rotate_slider->SetParams(test_mot_data->rot_y[id], -180.0f, 180.0f, 36.0f, 1.0f, 10.0f);
    rotate_slider->callback_data.i32 = id;
    rotate_slider->AddSelectionListener(&rotate_slider_proc);

    position_slider = dw::Slider::Create(this);
    position_slider->dw::Widget::SetText("X POS");
    position_slider->format = "%3.2f";
    position_slider->SetParams(test_mot_data->trans_x[id], -2.0f, 2.0f, 0.4f, 0.01f, 0.1f);
    position_slider->callback_data.i32 = id;
    position_slider->AddSelectionListener(&position_slider_proc);

    dw::Group* frame_group = new dw::Group(this);
    frame_group->SetText("FRAME");
    frame_group->SetFont(dw::p_font_type_6x12);

    for (int32_t i = 3; i < 4; i++) {
        step_slider = dw::Slider::Create(frame_group);
        char buf[0x10];
        sprintf_s(buf, sizeof(buf), "STEP %02d", i);
        step_slider->SetText(buf);
        step_slider->format = "%4.4f";
        step_slider->SetParams(test_mot_data->step[id].array[i], -3.0f, 3.0f, 0.4f, 0.01f, 0.1f);
        step_slider->callback_data.i32 = id;
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

    frame_max = new dw::Label(frame_comp, dw::FLAG_4000);
    frame_max->SetText("00000.0000");

    current = dw::Slider::Create(frame_group);
    current->SetText("Current");
    current->format = "%4.0f";
    current->SetParams(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    current->callback_data.i32 = id;
    current->AddSelectionListener(&frame_slider_proc);

    start_frame_slider = dw::Slider::Create(frame_group);
    start_frame_slider->SetText("Start  ");
    start_frame_slider->format = "%4.0f";
    start_frame_slider->SetParams(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    start_frame_slider->callback_data.i32 = id;
    start_frame_slider->AddSelectionListener(&start_frame_slider_proc);

    dw::Composite* start_ctrl_comp = new dw::Composite(frame_group);
    start_ctrl_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(start_ctrl_comp))->SetText("Start Ctrl : ");

    dw::Button* start_ctrl_left = new dw::Button(start_ctrl_comp);
    start_ctrl_left->SetText(" < ");
    start_ctrl_left->callback_data.v64 = this;
    start_ctrl_left->callback = DataTestMotDw::StartCtrlLeftRightCallback;

    dw::Button* start_ctrl_reset = new dw::Button(start_ctrl_comp);
    start_ctrl_reset->SetText(" RESET ");
    start_ctrl_reset->callback = DataTestMotDw::StartCtrlResetCallback;

    dw::Button* start_ctrl_right = new dw::Button(start_ctrl_comp);
    start_ctrl_right->SetText(" > ");
    start_ctrl_right->callback_data.v64 = this;
    start_ctrl_right->callback = DataTestMotDw::StartCtrlLeftRightCallback;

    dw::Group* ab_loop_group = new dw::Group(frame_group);
    ab_loop_group->SetLayout(new dw::RowLayout(dw::HORIZONTAL));
    ab_loop_group->SetText("AB LOOP");
    ab_loop_group->SetFont(dw::p_font_type_6x12);

    dw::Button* a_label = new dw::Button(ab_loop_group);
    a_label->SetText(" A ");
    a_label->callback_data.i64 = (int64_t)this;
    a_label->callback = DataTestMotDw::ACallback;
    a_label->SetFont(dw::p_font_type_6x12);

    dw::Button* b_label = new dw::Button(ab_loop_group);
    b_label->SetText(" B ");
    b_label->callback_data.i64 = (int64_t)this;
    b_label->callback = DataTestMotDw::BCallback;
    b_label->SetFont(dw::p_font_type_6x12);

    ab_toggle_button = new dw::Button(ab_loop_group, dw::CHECKBOX);
    ab_toggle_button->SetText("");
    ab_toggle_button->callback_data.v64 = this;
    ab_toggle_button->callback = DataTestMotDw::ABToggleCallback;
    ab_toggle_button->SetFont(dw::p_font_type_6x12);

    ab_loop = new dw::Label(ab_loop_group);
    ab_loop->SetFont(dw::p_font_type_6x12);
    ab_loop->SetText("     00000:00000");

    dw::Composite* rob_comp = new dw::Composite(this);
    rob_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* disp_button = new dw::Button(rob_comp, dw::CHECKBOX);
    disp_button->SetText("DISP  ");
    disp_button->SetValue(true);
    disp_button->AddSelectionListener(&disp_button_proc);
    disp_button->callback_data.i32 = id;
    disp_button->SetFont(dw::p_font_type_6x12);

    set_change_button = new dw::Button(rob_comp, dw::CHECKBOX);
    set_change_button->SetText("SET CHANGE  ");
    set_change_button->SetValue(false);
    set_change_button->SetFont(dw::p_font_type_6x12);

    dw::Composite* var_comp = new dw::Composite(this);
    var_comp->Composite::SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* reate_face_mot_dw = new dw::Button(var_comp);
    reate_face_mot_dw->SetText(" FACE MOT ");
    reate_face_mot_dw->AddSelectionListener(&create_face_mot_dw_proc);
    reate_face_mot_dw->callback_data.i32 = id;
    reate_face_mot_dw->SetFont(dw::p_font_type_6x12);

    dw::Button* create_eq_dw = new dw::Button(var_comp);
    create_eq_dw->SetText("  EQUIP  ");
    create_eq_dw->AddSelectionListener(&create_eq_dw_proc);
    create_eq_dw->SetFont(dw::p_font_type_6x12);

    dw::Button* create_debug_cam = new dw::Button(var_comp);
    create_debug_cam->SetText(" LOCK CAM ");
    create_debug_cam->AddSelectionListener(&create_debug_cam_proc);
    create_debug_cam->SetFont(dw::p_font_type_6x12);

    dw::Button* use_opd_button = new dw::Button(this, dw::CHECKBOX);
    use_opd_button->SetText("USE OSAGE PLAY DATA");
    use_opd_button->SetValue(false);
    use_opd_button->AddSelectionListener(&use_opd_button_proc);
    use_opd_button->callback_data.i32 = id;
    use_opd_button->SetFont(dw::p_font_type_6x12);

    dtm_mot->SetPartialMot(false);

    const char* save_only_start_frame_button_text;
    if (dw::translate)
        save_only_start_frame_button_text = u8"Initialize only Start Frame Osage";
    else
        save_only_start_frame_button_text = u8"Startフレームだけおさげ初期化";

    dw::Button* save_only_start_frame_button = new dw::Button(this, dw::CHECKBOX);
    save_only_start_frame_button->SetText(save_only_start_frame_button_text);
    save_only_start_frame_button->SetValue(false);
    save_only_start_frame_button->AddSelectionListener(&save_only_start_frame_button_proc);
    save_only_start_frame_button->callback_data.i32 = id;
    save_only_start_frame_button->SetFont(dw::p_font_type_6x12);

    UpdateLayout();
}

DataTestMotDw::~DataTestMotDw() {

}

void DataTestMotDw::Draw() {
    bool ab_loop;
    float_t a_frame;
    float_t b_frame;
    dtm_mot->GetABLoop(ab_loop, a_frame, b_frame);
    ab_toggle_button->SetValue(ab_loop);

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "     %5d:%5d", (int32_t)prj::roundf(a_frame), (int32_t)prj::roundf(b_frame));
    this->ab_loop->SetText(buf);

    dw::Shell::Draw();
}

void DataTestMotDw::Hide() {
    SetDisp();
}

void DataTestMotDw::ClearIDs() {
    set_list_box_proc.list_box->ClearItems();
}

void DataTestMotDw::AddID(const char* str) {
    set_list_box_proc.list_box->AddItem(str);
}

void DataTestMotDw::SetFrameSlider(float_t frame, float_t frame_max) {
    float_t last_frame = frame_max - 1.0f;
    current->SetParams(min_def(frame, last_frame), 0.0f, last_frame, last_frame * 0.1f, 1.0f, 10.0f);

    float_t _frame = clamp_def(start_frame_slider->GetValue(), 0.0f, last_frame);
    start_frame_slider->SetParams(_frame, 0.0f, last_frame, last_frame * 0.1f, 1.0f, 10.0f);

    data_test_mot_data_get()->start_frame[start_frame_slider->callback_data.i32] = _frame;
}

void DataTestMotDw::SetFrameLabel(float_t frame, float_t frame_max) {
    char buf[0x40];
    sprintf_s(buf, sizeof(buf), "%10.4f", frame);
    this->frame->SetText(buf);

    sprintf_s(buf, sizeof(buf), "%7.1f", frame_max);
    this->frame_max->SetText(buf);
}

void DataTestMotDw::AddModules(ROB_ID id, dw::ListBox* list_box) {
    if (!list_box)
        return;

    DataTestMot::Data* test_mot_data = data_test_mot_data_get();

    list_box->ClearItems();

    CHARA_NUM chr = test_mot_data->chr[id];
    const char* chara_name = get_chara_name(chr);

    const std::vector<module_data>& modules = module_data_handler_data_get_modules();
    for (auto& i : get_rob_item_header(chr)->defset) {
        bool found = false;
        for (const module_data& j : modules)
            if (chr == j.chara_num && i.first == j.cos) {
                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "%03d:(%s%03d)%s", j.id, chara_name, i.first + 1, j.name.c_str());
                list_box->AddItem(buf);

                found = true;
                break;
            }

        if (!found && i.second.item_no[ROB_ITEM_EQUIP_SUB_ID_OUTER]) {
            const RobItemTable* tbl = get_rob_item_table(
                chr, i.second.item_no[ROB_ITEM_EQUIP_SUB_ID_OUTER]);
            if (tbl) {
                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "---:(%s%03d)%s", chara_name, i.first + 1, tbl->name.c_str());
                list_box->AddItem(buf);
            }
        }
    }

    int32_t ptr = test_mot_data->ptr[id];
    list_box->SetItemIndex(ptr < list_box->GetItemCount() ? ptr : 0);
}

void DataTestMotDw::ResetFrame() {
    start_frame_slider->SetValue(0.0f);
    dtm_mot->ResetABLoop();
}

void DataTestMotDw::ResetIDListBoxIndex() {
    UpdateLayout();

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

    float_t frame = test_mot_dw->start_frame_slider->GetValue();

    std::vector<pv_data_set_motion> set_motion(test_mot_dw->dtm_mot->set_motion);

    auto i_begin = set_motion.begin();
    auto i_end = set_motion.end();
    for (auto i = i_begin; i != i_end; )
        if (i->motnum != test_mot_dw->dtm_mot->uid) {
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
        if (i.motnum == test_mot_dw->dtm_mot->uid)
            if (&i != set_motion.data())
                last = i.frame_stage_index.first;
        if (frame < i.frame_stage_index.first) {
            first = i.frame_stage_index.first;
            break;
        }
    }

    if (frame > set_motion.back().frame_stage_index.first)
        last = set_motion.back().frame_stage_index.first;

    if (button->GetText().compare(L" < "))
        test_mot_dw->start_frame_slider->SetValue(last);
    else if (first > 0.0f)
        test_mot_dw->start_frame_slider->SetValue(first);
}

void DataTestMotDw::StartCtrlResetCallback(dw::Widget* data) {
    data_test_mot_data_get()->reset_mot = true;
}

void DataTestMotDw::sub_14028D8B0() {
    const InputState* input_state = input_state_get(0);

    int32_t chara_list_dir = 0;
    if (input_state->CheckTapped(INPUT_BUTTON_F5))
        chara_list_dir--;
    if (input_state->CheckTapped(INPUT_BUTTON_F6))
        chara_list_dir++;

    if (chara_list_dir) {
        dw::List* list = chara_list_box_proc.list_box->list;
        if (chara_list_dir >= 0) {
            if (list->selected_item < list->items.size() - 1)
                list->SetItemIndex(list->selected_item + 1);
        }
        else {
            if (list->selected_item)
                list->SetItemIndex(list->selected_item - 1);
        }

        dw::SelectionListener::CallbackData callback_data = chara_list_box_proc.list_box;
        list->Callback(&callback_data);
    }

    int32_t set_list_dir = 0;
    if (input_state->CheckTapped(INPUT_BUTTON_F7))
        set_list_dir--;
    if (input_state->CheckTapped(INPUT_BUTTON_F8))
        set_list_dir++;

    if (set_list_dir) {
        dw::List* list = set_list_box_proc.list_box->list;
        if (set_list_dir >= 0) {
            if (list->selected_item < list->items.size() - 1)
                list->SetItemIndex(list->selected_item + 1);
        }
        else {
            if (list->selected_item)
                list->SetItemIndex(list->selected_item - 1);
        }

        dw::SelectionListener::CallbackData callback_data = set_list_box_proc.list_box;
        list->Callback(&callback_data);
    }
}

DataTestMotA3dDw::PvListBoxProc::PvListBoxProc() {

}

DataTestMotA3dDw::PvListBoxProc::~PvListBoxProc() {

}

void DataTestMotA3dDw::PvListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box)
        data_test_mot_a3d_dw->SetPvId(atoi(list_box->GetSelectedItemStr().c_str()));

    data_test_mot_a3d_dw->UpdateLayout();
}

DataTestMotA3dDw::A3dListBoxProc::A3dListBoxProc() {

}

DataTestMotA3dDw::A3dListBoxProc::~A3dListBoxProc() {

}

void DataTestMotA3dDw::A3dListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {

}

DataTestMotCtrlDw::TypeListBoxProc::TypeListBoxProc() {

}

DataTestMotCtrlDw::TypeListBoxProc::~TypeListBoxProc() {

}

void DataTestMotCtrlDw::TypeListBoxProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box)
        test_mot_data->type = (int32_t)list_box->list->selected_item;
}

DataTestMotCtrlDw::SyncFrameButtonProc::SyncFrameButtonProc() {

}

DataTestMotCtrlDw::SyncFrameButtonProc::~SyncFrameButtonProc() {

}

void DataTestMotCtrlDw::SyncFrameButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button)
        test_mot_data->sync_frame = button->value;
}

DataTestMotCtrlDw::GameCameraButtonProc::GameCameraButtonProc() {

}

DataTestMotCtrlDw::GameCameraButtonProc::~GameCameraButtonProc() {

}

void DataTestMotCtrlDw::GameCameraButtonProc::Callback(dw::SelectionListener::CallbackData* data) {
    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
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
    pv_label->SetFont(dw::p_font_type_6x12);

    pv = new dw::ListBox(pv_comp);
    pv->SetMaxItems(20);
    pv->SetFont(dw::p_font_type_6x12);

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
    a3d_label->SetFont(dw::p_font_type_6x12);

    const char* please_select_a_pv;
    if (dw::translate)
        please_select_a_pv = u8"Please select a PV";
    else
        please_select_a_pv = u8"PVを選択して下さい";

    a3d = new dw::ListBox(a3d_comp);
    a3d->SetMaxItems(20);
    a3d->AddItem(please_select_a_pv);

    a3d->SetItemIndex(0);
    a3d->AddSelectionListener(&a3d_list_box_proc);
    a3d->SetFont(dw::p_font_type_6x12);

    dw::Composite* button_comp = new dw::Composite(this);
    button_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    play_a3d = new dw::Button(button_comp);
    play_a3d->SetText(" PLAY A3D ");

    play_a3d->callback_data.v64 = this;
    play_a3d->callback = DataTestMotA3dDw::PlayA3dCallback;
    play_a3d->SetFont(dw::p_font_type_6x12);

    sync_1p_frame = new dw::Button(button_comp, dw::CHECKBOX);
    sync_1p_frame->SetText("SYNC 1P FRAME");
    sync_1p_frame->callback_data.v64 = this;
    sync_1p_frame->callback = DataTestMotA3dDw::Sync1pFrameCallback;
    sync_1p_frame->SetFont(dw::p_font_type_6x12);

    UpdateLayout();

    rect.pos.x = (float_t)get_screen_param().width * 0.3f - rect.size.x * 0.3f;

    SetSize(rect.size);
}

DataTestMotA3dDw::~DataTestMotA3dDw() {

}

void DataTestMotA3dDw::Draw() {
    SetText(data_test_mot_a3d_get_state_text());
    Shell::Draw();
}

void DataTestMotA3dDw::Hide() {
    SetDisp();
    data_test_mot_a3d_close();
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
    std::string auth_3d = data_test_mot_a3d_dw_get()->a3d->GetSelectedItemStr();
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

    type_list = new dw::ListBox(this, dw::MULTISELECT);
    type_list->AddItem("ONCE");
    type_list->AddItem("LOOP");
    type_list->AddItem("STEP");
    type_list->SetItemIndex(test_mot_data->type);
    type_list->AddSelectionListener(&type_list_box_proc);

    reset_mot = new dw::Button(this);
    reset_mot->SetText("RESET MOT");
    reset_mot->callback_data.v64 = this;
    reset_mot->callback = DataTestMotCtrlDw::ResetMotCallback;

    reset_cam = new dw::Button(this);
    reset_cam->SetText("RESET CAM");
    reset_cam->callback_data.v64 = this;
    reset_cam->callback = DataTestMotCtrlDw::ResetCamCallback;

    reload_data = new dw::Button(this);
    reload_data->SetText("RELOAD DATA");
    reload_data->callback_data.v64 = this;
    reload_data->callback = DataTestMotCtrlDw::ReloadDataCallback;

    dw::Button* sync_frame_button = new dw::Button(this, dw::CHECKBOX);
    sync_frame_button->SetText("SYNC FRAME");
    sync_frame_button->AddSelectionListener(&sync_frame_button_proc);
    sync_frame_button->SetFont(dw::p_font_type_6x12);

    running = new dw::Button(this, dw::CHECKBOX);
    running->SetText("RUNNING");
    running->SetValue(false);
    running->callback = DataTestMotCtrlDw::RunningCallback;
    running->SetFont(dw::p_font_type_6x12);

    stage = new dw::Button(this);
    stage->SetText("STAGE");
    stage->callback = DataTestMotCtrlDw::StageCallback;

    UpdateLayout();

    rect.pos.x = (float_t)get_screen_param().width - rect.size.x;

    SetSize(rect.size);
}

DataTestMotCtrlDw::~DataTestMotCtrlDw() {

}

void DataTestMotCtrlDw::Draw() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    DataTestMot::Data* test_mot_data = data_test_mot_data_get();
    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_V))
        test_mot_data->reset_mot = true;
    else if (input_state->CheckTapped(INPUT_BUTTON_L))
        test_mot_data->reload_data = true;

    if (input_state->CheckTapped(INPUT_BUTTON_M))
        switch (test_mot_data->type) {
        case 0:
        case 1:
            test_mot_data->type = 2;
            break;
        case 2:
        default: {
            test_mot_data->type = 0;

            if (dtm_mot_array[ROB_ID_1P].set != -1) {
                std::string motion_set_name(aft_mot_db->get_motion_set_name(dtm_mot_array[ROB_ID_1P].set));

                const char* v13[] = {
                    "CMN",
                    "EDT",
                    "TEST_DESIGN",
                };

                for (const char* i : v13)
                    if (motion_set_name.find(i) != -1) {
                        test_mot_data->type = 1;
                        break;
                    }
            }
        } break;
        }

    if (type_list->list && type_list->list->selected_item != test_mot_data->type)
        type_list->list->SetItemIndex(test_mot_data->type);

    Shell::Draw();
}

void DataTestMotCtrlDw::Hide() {
    SetDisp();
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

static DataTestMot::Data* data_test_mot_data_get() {
    return &data_test_mot->data;
}

static bool data_test_mot_a3d_open() {
    return data_test_mot_a3d_get()->open("DATA_TEST_MOT_A3D", app::TASK_PRIO_HIGH);
}

static bool data_test_mot_a3d_close() {
    return data_test_mot_a3d_get()->close();
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

static void data_test_mot_dw_array_init(ROB_ID id, DtmMot* dtm_mot) {
    if (data_test_mot_dw_array[id]) {
        data_test_mot_dw_array[id]->Disp();
        return;
    }

    DataTestMotDw* test_mot_dw = new DataTestMotDw(id, dtm_mot);
    data_test_mot_dw_array[id] = test_mot_dw;

    test_mot_dw->LimitPosDisp();

    switch (id) {
    case ROB_ID_1P: {
        test_mot_dw->rect.pos.y += 14.0f;
    } break;
    case ROB_ID_2P: {
        ScreenParam& screen_param = get_screen_param();
        test_mot_dw->rect.pos.x = (float_t)screen_param.width - test_mot_dw->rect.size.x;
        test_mot_dw->rect.pos.y = (float_t)screen_param.height - 24.0f;
    } break;
    }
}

static DataTestMotDw* data_test_mot_dw_array_get(ROB_ID id) {
    return data_test_mot_dw_array[id];
}

static void data_test_mot_a3d_dw_init() {
    data_test_mot_a3d_open();
    if (!data_test_mot_a3d_dw) {
        data_test_mot_a3d_dw = new DataTestMotA3dDw;
        data_test_mot_a3d_dw->LimitPosDisp();
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
        data_test_mot_ctrl_dw->LimitPosDisp();
    }
    else
        data_test_mot_ctrl_dw->Disp();
}

static DataTestMotCtrlDw* data_test_mot_ctrl_dw_get() {
    return data_test_mot_ctrl_dw;
}

static void data_test_face_mot_dw_array_init(ROB_ID id) {
    if (id < 0 || id >= ROB_ID_MAX || !get_rob_management()->get_rob(id))
        return;

    if (!data_test_face_mot_dw_array[id]) {
        data_test_face_mot_dw_array[id] = new DataTestFaceMotDw(id);
        data_test_face_mot_dw_array[id]->LimitPosDisp();
    }
    else
        data_test_face_mot_dw_array[id]->Disp();
}

static DataTestFaceMotDw* data_test_face_mot_dw_array_get(ROB_ID id) {
    if (id >= 0 && id < ROB_ID_MAX)
        return data_test_face_mot_dw_array[id];
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
        if (objset_info_storage_load_obj_set_check_not_read(i))
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
        objset_info_storage_load_set(aft_data, aft_obj_db, i);
}

static void motion_test_objset_unload() {
    for (uint32_t i : motion_test_objset)
        objset_info_storage_unload_set(i);
    motion_test_objset.clear();
}
