/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../pv_game/pv_game_pv_data.hpp"
#include "../dw.hpp"
#include "../task_window.hpp"

class DataTestMot : public app::Task {
public:
    struct Data {
        struct Offset {
            float_t pre;
            float_t post;

            Offset();
        };

        struct Divide {
            float_t array[3];

            Divide();
        };

        struct Step {
            float_t array[4];

            Step();
        };

        ::chara_index chara_index[2];
        ::chara_index curr_chara_index[2];
        uint32_t module_index[2];
        uint32_t curr_module_index[2];
        uint32_t motion_set_index[2];
        uint32_t curr_motion_set_index[2];
        uint32_t motion_index[2];
        uint32_t curr_motion_index[2];
        float_t rot_y[2];
        float_t trans_x[2];
        Offset offset[2];
        Divide divide[2];
        Step step[2];
        float_t start_frame[2];
        int32_t type;
        bool reset_mot;
        bool reset_cam;
        bool reload_data;
        bool sync_frame;
        bool field_A8;
        bool field_A9;
        bool field_AA;
        bool running;
        int32_t field_AC;
        int32_t field_B0;
        bool sync_1p_frame;

        Data();
    };

    Data data;

    DataTestMot();
    virtual ~DataTestMot() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void sub_140286280();
};

class DataTestMotA3d : public app::Task {
public:
    int32_t state;
    std::string auth_3d;
    std::vector<std::string> categories;
    std::vector<auth_3d_id> auth_3d_ids;
    std::vector<uint32_t> obj_sets;
    bool camera;
    
    DataTestMotA3d();
    virtual ~DataTestMotA3d() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    const char* GetStateText();
    void LoadAuth3d(std::string&& name);
    void Reset();
    void SetAuth3d(std::string& value);
    void Sync1pFrame();
};

class DtmMot : public app::Task {
public:
    rob_chara_bone_data* rob_bone_data;
    int32_t chara_id;
    int32_t field_7C;
    ::chara_index chara_index;
    int32_t module_index;
    union {
        uint32_t motion_set_id;
        int32_t motion_set_index;
    };
    union {
        uint32_t motion_id;
        int32_t motion_index;
    };
    vec3 trans;
    vec3 rotation;
    float_t rot_y;
    float_t pre_offset;
    float_t post_offset;
    float_t step[4];
    float_t divide[3];
    bool loop;
    bool play;
    bool field_D2;
    bool change_motion;
    bool disp;
    bool use_opd;
    bool field_D6;
    bool reset_mot;
    bool partial_mot;
    bool save_only_start_frame;
    int32_t state;
    float_t frame;
    float_t delta_frame;
    bool looped;
    float_t start_frame;
    float_t ab_frame[2];
    bool ab_loop;
    int32_t pv_id;
    bool field_100;
    p_file_handler dsc_file_handler;
    pv_game_pv_data pv_data;
    std::vector<pv_data_set_motion> set_motion;
    int32_t set_motion_index;

    DtmMot();
    virtual ~DtmMot() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Basic() override;

    virtual bool AddTask(::chara_index chara_index,
        int32_t module_index, uint32_t motion_set_id, uint32_t motion_id);
    virtual bool AddTask(::chara_index chara_index,
        int32_t module_index, uint32_t motion_id);
    virtual bool DelTask();

    bool CheckFirstFrame();
    void CtrlFaceMot();
    void GetABLoop(bool& ab_loop, float_t& a_frame, float_t& b_frame);
    float_t GetFrame();
    float_t GetFrameCount();
    float_t GetStep();
    void ResetABLoop();
    void SetABFrame(bool a);
    void SetChangeMotion();
    void SetDisp(bool value);
    void SetDivide(int32_t index, float_t value);
    void SetFrame(float_t value);
    void SetLoop(bool value);
    void SetMotion(uint32_t motion_set_index, uint32_t motion_index);
    void SetOffset(float_t pre_offset, float_t post_offset);
    void SetPartialMot(bool value);
    void SetPlay(bool value);
    void SetResetMot();
    void SetRotationY(float_t value);
    void SetSaveOnlyStartFrame(bool value);
    void SetStartFrame(float_t value);
    void SetStep(int32_t index, float_t value);
    void SetTrans(const vec3& value);
    void SetUseOpd(bool value);
    void ToggleABLoop();
    void UpdateABFrame();

    static int32_t ConvertMotionSetNameToPVID(const char* set_name);

    bool sub_140291C10();
    void sub_1402922C0(bool value);
};

class DataTestFaceMotDw : public dw::Shell {
public:
    struct MotionData
    {
        int32_t mottbl_index;
        float_t frame;
    };

    struct Data {
        MotionData face;
        MotionData eyelid;
        MotionData eyes;
        MotionData mouth;
    };

    int32_t chara_id;
    std::map<uint32_t, int32_t> motion_id_mottbl_map;
    dw::Button* enable;
    dw::ListBox* face;
    dw::Slider* face_frame;
    dw::Label* face_cl;
    dw::Slider* face_cl_frame;
    dw::ListBox* eyes;
    dw::Slider* eyes_frame;
    dw::ListBox* mouth;
    dw::Slider* mouth_frame;

    DataTestFaceMotDw(int32_t chara_id);
    virtual ~DataTestFaceMotDw() override;

    virtual void Hide() override;

    void AddMottblMapMotions(dw::ListBox* list_box, int32_t type);
    Data GetData();
    bool GetEnable();
    dw::ListBox* InitAddMottblMapMotions(dw::Composite* parent, int32_t type);
    void Reset();
};

class DataTestMotDw : public dw::Shell {
public:
    class StepSliderProc : public dw::SelectionAdapter {
    public:
        int32_t index;

        StepSliderProc();
        virtual ~StepSliderProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class CharaListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        CharaListBoxProc();
        virtual ~CharaListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class CTypeListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        CTypeListBoxProc();
        virtual ~CTypeListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class SetListBoxProc : public dw::SelectionAdapter {
    public:
        dw::ListBox* list_box;

        SetListBoxProc();
        virtual ~SetListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class IdListBoxProc : public dw::SelectionAdapter {
    public:
        dw::Slider* slider;

        IdListBoxProc();
        virtual ~IdListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class RotateSliderProc : public dw::SelectionAdapter {
    public:
        dw::Slider* slider;

        RotateSliderProc();
        virtual ~RotateSliderProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class PositionSliderProc : public dw::SelectionAdapter {
    public:
        PositionSliderProc();
        virtual ~PositionSliderProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class FrameSliderProc : public dw::SelectionAdapter {
    public:
        dw::Slider* slider;

        FrameSliderProc();
        virtual ~FrameSliderProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class StartFrameSliderProc : public dw::SelectionAdapter {
    public:
        dw::Button* button;

        StartFrameSliderProc();
        virtual ~StartFrameSliderProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class DispButtonProc : public dw::SelectionAdapter {
    public:
        dw::Button* button;

        DispButtonProc();
        virtual ~DispButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class UseOpdButtonProc : public dw::SelectionAdapter {
    public:
        UseOpdButtonProc();
        virtual ~UseOpdButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class PartialMotButtonProc : public dw::SelectionAdapter {
    public:
        PartialMotButtonProc();
        virtual ~PartialMotButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class SaveOnlyStartFrameButtonProc : public dw::SelectionAdapter {
    public:
        SaveOnlyStartFrameButtonProc();
        virtual ~SaveOnlyStartFrameButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class CreateFaceMotDwProc : public dw::SelectionAdapter {
    public:
        CreateFaceMotDwProc();
        virtual ~CreateFaceMotDwProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    class CreateEqDwProc : public dw::SelectionAdapter {
    public:
        CreateEqDwProc();
        virtual ~CreateEqDwProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    struct CreateDebugCamProc : public dw::SelectionAdapter {
    public:
        CreateDebugCamProc();
        virtual ~CreateDebugCamProc() override;

        virtual void Callback(dw::Widget* data) override;
    };

    DtmMot* dtm_mot;
    dw::ListBox* chara;
    CharaListBoxProc chara_list_box_proc;
    CTypeListBoxProc c_type_list_box_proc;
    SetListBoxProc set_list_box_proc;
    IdListBoxProc id_list_box_proc;
    RotateSliderProc rotate_slider_proc;
    PositionSliderProc position_slider_proc;
    //__int64 field_1E8;
    //__int64 field_1F0;
    //__int64 field_1F8;
    dw::Slider* step_slider;
    StepSliderProc step_slider_proc[4];
    dw::Slider* current;
    FrameSliderProc frame_slider_proc;
    StartFrameSliderProc start_frame_slider_proc;
    dw::Label* ab_loop;
    dw::Label* frame;
    dw::Label* frame_count;
    DispButtonProc disp_button_proc;
    UseOpdButtonProc use_opd_button_proc;
    PartialMotButtonProc partial_mot_button_proc;
    SaveOnlyStartFrameButtonProc save_only_start_frame_button_proc;
    CreateFaceMotDwProc create_face_mot_dw_proc;
    CreateEqDwProc create_eq_dw_proc;
    CreateDebugCamProc create_debug_cam_proc;

    DataTestMotDw(int32_t chara_id, DtmMot* dtm_mot);
    virtual ~DataTestMotDw() override;

    virtual void Draw() override;
    virtual void Hide() override;

    virtual void ClearIDs();
    virtual void AddID(const char* str);
    virtual void SetFrameSlider(float_t frame, float_t frame_count);
    virtual void SetFrameLabel(float_t frame, float_t frame_count);

    void AddModules(int32_t chara_id, dw::ListBox* list_box);
    void ResetFrame();
    void ResetIDListBoxIndex();
    void SetIDListBoxIndex(uint32_t index);

    static void ABToggleCallback(dw::Widget* data);
    static void ACallback(dw::Widget* data);
    static void BCallback(dw::Widget* data);
    static void StartCtrlLeftRightCallback(dw::Widget* data);
    static void StartCtrlResetCallback(dw::Widget* data);
};

class DataTestMotA3dDw : public dw::Shell {
public:
    class PvListBoxProc : public dw::SelectionAdapter {
    public:
        PvListBoxProc();
        virtual ~PvListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
    };
    
    class A3dListBoxProc : public dw::SelectionAdapter {
    public:
        A3dListBoxProc();
        virtual ~A3dListBoxProc() override;

        virtual void Callback(dw::Widget* data) override;
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

        virtual void Callback(dw::Widget* data) override;
    };

    class SyncFrameButtonProc : public dw::SelectionAdapter {
    public:
        SyncFrameButtonProc();
        virtual ~SyncFrameButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
    };


    class GameCameraButtonProc : public dw::SelectionAdapter {
    public:
        GameCameraButtonProc();
        virtual ~GameCameraButtonProc() override;

        virtual void Callback(dw::Widget* data) override;
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

extern DataTestMot* data_test_mot;
extern DtmMot* dtm_mot_array;

extern void motion_test_init();
extern void motion_test_free();

extern void dtm_mot_array_set_reset_mot();
