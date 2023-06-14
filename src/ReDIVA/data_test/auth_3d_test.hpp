/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../../KKdLib/default.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../task_window.hpp"

class Auth3dTestTask : public app::Task {
public:
    int32_t field_1C0;
    int32_t field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
    bool field_1D0;
    int32_t field_1D4;
    ::auth_3d_id auth_3d_id;
    int32_t auth_3d_uid;
    bool repeat;
    bool left_right_reverse;
    bool pos;
    bool field_1E3;
    int32_t field_1E4;
    bool field_1E8;
    bool field_1E9;
    bool black_mask_listener;
    bool field_1EB;
    int32_t field_1EC;
    uint32_t effcmn_obj_set;
    int32_t field_210;
    bool field_328;
    bool field_329;
    bool field_32A;
    int32_t stage_index;
    int32_t load_stage_index;
    vec3 trans_value;
    float_t rot_y_value;
    prj::vector_pair<std::string, std::string> field_348;
    bool field_388;
    int32_t field_38C;
    bool field_390;
    int32_t field_394;
    std::string category;
    std::string load_category;
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

    void DispAuth3dChara(::auth_3d_id& id);
    void DispChara();
    void SetAuth3dId();
    void SetStage();
};

class Auth3dTestWindow : public app::TaskWindow {
public:
    struct Category {
        std::string* name;
        int32_t index;
        prj::vector_pair<std::string*, int32_t> uid;

        Category();
        ~Category();
    };

    int32_t auth_3d_category_index;
    int32_t auth_3d_category_index_prev;
    int32_t auth_3d_index;

    int32_t auth_3d_uid;
    bool auth_3d_load;
    bool auth_3d_uid_load;

    bool enable;
    float_t frame;
    bool frame_changed;
    float_t last_frame;
    bool paused;
    bool stg_auth_display;
    bool stg_display;

    std::vector<const char*> stage;
    std::vector<Category> category;

    Auth3dTestWindow();
    virtual ~Auth3dTestWindow() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;
};

extern Auth3dTestTask* auth_3d_test_task;
extern Auth3dTestWindow* auth_3d_test_window;

extern void auth_3d_test_task_init();
extern void auth_3d_test_task_disp_chara();
extern void auth_3d_test_task_free();

extern void auth_3d_test_window_init();
extern void auth_3d_test_window_free();
