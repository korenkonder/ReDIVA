/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../../CRE/task.hpp"
#include "../pv_game/pv_game_pv_data.hpp"

class DataTestMot : public app::Task {
public:
    struct Data {
        struct Off {
            float_t array[2];

            Off();
        };

        struct Div {
            float_t array[3];

            Div();
        };

        struct Step {
            float_t array[4];

            Step();
        };

        CHARA_NUM chr[2];
        CHARA_NUM curr_chr[2];
        uint32_t ptr[2];
        uint32_t curr_ptr[2];
        uint32_t set[2];
        uint32_t curr_set[2];
        uint32_t uid[2];
        uint32_t curr_uid[2];
        float_t rot_y[2];
        float_t trans_x[2];
        Off off[2];
        Div div[2];
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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
    virtual void post() override;

    const char* GetStateText();
    void LoadAuth3d(std::string&& name);
    void Reset();
    void SetAuth3d(std::string& value);
    void Sync1pFrame();
};

class DtmMot : public app::Task {
public:
    enum ASSIGN {
        IDX = 0,
        UID,
    };

    RobManagement* rob_man;
    rob_chara_bone_data* motion;
    ROB_ID rctrl;
    ASSIGN assign;
    CHARA_NUM chr;
    int32_t ptr;
    uint32_t set;
    uint32_t uid;
    vec3 pos;
    float_t rot[4];
    float_t pre_offset;
    float_t post_offset;
    float_t step[4];
    float_t div[3];
    bool loop;
    bool move;
    bool item;
    bool reset_osage_flag;
    bool display;
    bool use_opd;
    bool loaded;
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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void post() override;

    virtual bool open(CHARA_NUM chr, int32_t ptr, uint32_t set, uint32_t uid);
    virtual bool open(CHARA_NUM chr, int32_t ptr, uint32_t uid);
    virtual bool close();

    bool CheckFirstFrame();
    void CtrlFaceMot();
    void GetABLoop(bool& ab_loop, float_t& a_frame, float_t& b_frame);
    float_t GetFrame();
    float_t GetFrameCount();
    bool GetLoaded();
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

    void sub_1402922C0(bool value);
};

extern DataTestMot* data_test_mot;
extern DtmMot* dtm_mot_array;

extern void motion_test_init();
extern void motion_test_free();

extern bool dtm_mot_array_get_loaded();
extern void dtm_mot_array_set_reset_mot();
