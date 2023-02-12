/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../CRE/rob/rob.hpp"
#include "../../KKdLib/default.hpp"
#include "../task_window.hpp"

class DataTestMot : public app::Task {
public:
    struct Data {
        struct Divide {
            float_t array[3];

            Divide();
        };

        struct Step {
            float_t array[4];

            Step();
        };

        ::chara_index chara_index_p1;
        ::chara_index chara_index_p2;
        ::chara_index curr_chara_index_p1;
        ::chara_index curr_chara_index_p2;
        uint32_t module_index_p1;
        uint32_t module_index_p2;
        uint32_t curr_module_index_p1;
        uint32_t curr_module_index_p2;
        uint32_t motion_set_index_p1;
        uint32_t motion_set_index_p2;
        uint32_t curr_motion_set_index_p1;
        uint32_t curr_motion_set_index_p2;
        uint32_t motion_index_p1;
        uint32_t motion_index_p2;
        uint32_t curr_motion_index_p1;
        uint32_t curr_motion_index_p2;
        float_t rot_y_p1;
        float_t rot_y_p2;
        float_t trans_x_p1;
        float_t trans_x_p2;
        float_t pre_offset_p1;
        float_t post_offset_p1;
        float_t pre_offset_p2;
        float_t post_offset_p2;
        Divide divide_p1;
        Divide divide_p2;
        Step step_p1;
        Step step_p2;
        float_t start_frame_p1;
        float_t start_frame_p2;
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

class DtmMot : public app::Task {
public:
    rob_chara_bone_data* rob_bone_data;
    int32_t chara_id;
    int32_t field_7C;
    ::chara_index chara_index;
    int32_t module_index;
    union {
        int32_t motion_set_id;
        int32_t motion_set_index;
    };
    union {
        int32_t motion_id;
        int32_t motion_index;
    };
    vec3 trans;
    vec3 rotatation;
    float_t field_A8;
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
    float_t field_E4;
    bool field_E8;
    float_t start_frame;
    float_t a_frame;
    float_t b_frame;
    bool ab_loop;
    int32_t pv_id;
    bool field_100;
    p_file_handler field_108;
    //pv_game_dsc_data field_110;
    std::vector<pv_data_set_motion> set_motion;
    int32_t field_2C960;

    DtmMot();
    virtual ~DtmMot() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Basic() override;

    virtual bool AddTask(::chara_index chara_index,
        int32_t module_index, int32_t motion_set_id, int32_t motion_id);
    virtual bool AddTask(::chara_index chara_index,
        int32_t module_index, int32_t motion_id);
    virtual bool DelTask();

    float_t GetFrame();
    float_t GetFrameCount();
    float_t GetStep();
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

    static int32_t ConvertMotionSetNameToPVID(const char* set_name);
};

class DtmEqVs : public app::Task {
public:
    int32_t state;
    int32_t chara_id;
    ::chara_index chara_index;
    ::chara_index curr_chara_index;
    int32_t module_index;
    int32_t curr_module_index;
    int32_t field_80;

    DtmEqVs();
    virtual ~DtmEqVs() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;

    virtual bool AddTask(int32_t chara_id, ::chara_index chara_index);
    virtual bool DelTask();

    void SetCharaIndexModuleIndex(::chara_index chara_index, int32_t module_index);

    void sub_140261FF0();
    void sub_140262050();
};

extern DataTestMot* data_test_mot;
extern DtmMot* dtm_mot_array;
extern DtmEqVs* dtm_eq_vs_array;
