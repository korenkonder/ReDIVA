/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../../KKdLib/default.hpp"
#include "../../KKdLib/database/item_table.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../../CRE/task.hpp"

class Auth3dTestTask : public app::Task {
public:
    int32_t field_1C0;
    int32_t field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
    bool field_1D0;
    int32_t state;
    ::auth_3d_id auth_3d_id;
    int32_t auth_3d_uid;
    bool repeat;
    bool left_right_reverse;
    bool pos;
    bool snap_shot;
    int32_t snap_shot_state;
    bool aet_index;
    bool auth_2d;
    bool black_mask;
    bool stage_link_change;
    int32_t effcmn_obj_set_state;
    uint32_t effcmn_obj_set;
    int32_t aet_state;
    //int32_t aet_id;
    bool plane_above_floor;
    bool stg_auth_display;
    bool stg_display;
    int32_t stage_index;
    int32_t load_stage_index;
    vec3 trans_value;
    float_t rot_y_value;
    prj::vector_pair<std::string, std::string> field_348;
    bool field_388;
    int32_t field_38C;
    bool save;
    int32_t load_state;
    std::string load_category;
    std::string category;
    std::vector<uint32_t> obj_sets;

    struct Window {
        bool stage_link_change;
        bool obj_link;

        Window();
    } window;

    Auth3dTestTask();
    virtual ~Auth3dTestTask() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void DispAuth3dChara(::auth_3d_id& id);
    void DispChara();
    void SetAuth3dId();
    void SetAuth3dUid(int32_t value);
    void SetAuth3dUidByName(const char* str);
    void SetLoadCategory(const char* str);
    void SetRotY(float_t value);
    void SetStage();
    void SetTransAxis(float_t value, int32_t axis);

    void sub_140244610();
};

extern Auth3dTestTask* auth_3d_test_task;

extern void auth_3d_test_task_init();
extern void auth_3d_test_task_disp_chara();
extern void auth_3d_test_task_free();

extern void auth_3d_test_window_init();
