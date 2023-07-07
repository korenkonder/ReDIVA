/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../../KKdLib/default.hpp"
#include "../../KKdLib/database/item_table.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../task_window.hpp"
#include "../dw.hpp"

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

class Auth3dTestRobWindow : public dw::Shell {
public:
    class SelectionListRob : public dw::SelectionAdapter {
    public:
        SelectionListRob();
        virtual ~SelectionListRob() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionListDispStyle : public dw::SelectionAdapter {
    public:
        SelectionListDispStyle();
        virtual ~SelectionListDispStyle() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    dw::ListBox* chara_list[2];
    dw::ListBox* costume_list[2];
    //int64_t field_1A0;
    SelectionListRob selection_list_rob;
    SelectionListDispStyle selection_list_disp_style;

    Auth3dTestRobWindow();
    virtual ~Auth3dTestRobWindow() override;

    void Hide() override;

    void GetCharaCos(::chara_index chara_index, int32_t chara_id, dw::ListBox* list_box);
};

class Auth3dTestSubWindow : public dw::Shell {
public:
    class SelectionListStage : public dw::SelectionAdapter {
    public:
        SelectionListStage();
        virtual ~SelectionListStage() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionListAet : public dw::SelectionAdapter {
    public:
        SelectionListAet();
        virtual ~SelectionListAet() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    dw::Button* plane_above_floor;
    dw::Button* stage;
    dw::Button* stg_auth;
    dw::ListBox* stg_list;
    dw::Button* stage_link_change;
    dw::Button* auth_2d;
    dw::ListBox* aet_list;
    SelectionListStage selection_list_stage;
    SelectionListAet selection_list_aet;

    Auth3dTestSubWindow();
    virtual ~Auth3dTestSubWindow() override;

    void Hide() override;

    void ResetAet();
};

class Auth3dTestWindow : public dw::Shell {
public:
    class SelectionCategoryMenuItem : public dw::SelectionAdapter {
    public:
        SelectionCategoryMenuItem();
        virtual ~SelectionCategoryMenuItem() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionCategoryList : public dw::SelectionAdapter {
    public:
        SelectionCategoryList();
        virtual ~SelectionCategoryList() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionIDMenuItem : public dw::SelectionAdapter {
    public:
        SelectionIDMenuItem();
        virtual ~SelectionIDMenuItem() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionList : public dw::SelectionAdapter {
    public:
        SelectionList();
        virtual ~SelectionList() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionSliderFrame : public dw::SelectionAdapter {
    public:
        SelectionSliderFrame();
        virtual ~SelectionSliderFrame() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionSliderTransElement : public dw::SelectionAdapter {
    public:
        int32_t axis;

        SelectionSliderTransElement();
        virtual ~SelectionSliderTransElement() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionSliderRotY : public dw::SelectionAdapter {
    public:
        SelectionSliderRotY();
        virtual ~SelectionSliderRotY() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonPlay : public dw::SelectionAdapter {
    public:
        SelectionButtonPlay();
        virtual ~SelectionButtonPlay() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonBegin : public dw::SelectionAdapter {
    public:
        SelectionButtonBegin();
        virtual ~SelectionButtonBegin() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonEnd : public dw::SelectionAdapter {
    public:
        SelectionButtonEnd();
        virtual ~SelectionButtonEnd() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonBindBoolFunc : public dw::SelectionAdapter {
    public:
        void(*callback)(bool value);

        SelectionButtonBindBoolFunc();
        virtual ~SelectionButtonBindBoolFunc() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonSnapShot : public dw::SelectionAdapter {
    public:
        SelectionButtonSnapShot();
        virtual ~SelectionButtonSnapShot() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonLog : public dw::SelectionAdapter {
    public:
        SelectionButtonLog();
        virtual ~SelectionButtonLog() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonShadowType : public dw::SelectionAdapter {
    public:
        SelectionButtonShadowType();
        virtual ~SelectionButtonShadowType() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonSave : public dw::SelectionAdapter {
    public:
        SelectionButtonSave();
        virtual ~SelectionButtonSave() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonCamReset : public dw::SelectionAdapter {
    public:
        SelectionButtonCamReset();
        virtual ~SelectionButtonCamReset() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SelectionButtonDebugCamera : public dw::SelectionAdapter {
    public:
        SelectionButtonDebugCamera();
        virtual ~SelectionButtonDebugCamera() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    std::vector<size_t> categories;
    dw::Button* chara;
    dw::Button* stg;
    dw::Button* eff;
    dw::Button* nage;
    dw::Menu* chara_menu;
    dw::Menu* stg_menu;
    dw::Menu* eff_menu;
    dw::Menu* nage_menu;
    dw::ListBox* category_list;
    dw::ListBox* id_list;
    dw::Button* obj_link;
    dw::Label* frame;
    dw::Slider* frame_slider;
    dw::Button* begin;
    dw::Button* play;
    dw::Button* end;
    dw::Button* repeat;
    dw::Button* left_right_reverse;
    dw::Button* pos;
    dw::Button* log;
    dw::Button* snap_shot;
    dw::Button* self_shadow_off;
    dw::Button* self_shadow_on;
    dw::Button* black_mask;
    dw::Slider* trans_x;
    dw::Slider* trans_z;
    dw::Slider* rot_y;
    dw::Button* save;
    Auth3dTestSubWindow* sub_window;
    Auth3dTestRobWindow* rob_window;
    SelectionCategoryMenuItem category_menu_item_listener;
    SelectionCategoryList category_list_listener;
    SelectionIDMenuItem id_menu_listener;
    SelectionList list_listener;
    SelectionSliderFrame slider_frame_listener;
    SelectionSliderTransElement trans_x_listener;
    SelectionSliderTransElement trans_z_listener;
    SelectionSliderRotY rot_y_listener;
    SelectionButtonPlay play_listener;
    SelectionButtonBegin begin_listener;
    SelectionButtonEnd end_listener;
    SelectionButtonBindBoolFunc pos_listener;
    SelectionButtonBindBoolFunc repeat_listener;
    SelectionButtonBindBoolFunc left_right_reverse_listener;
    SelectionButtonSnapShot snap_shot_listener;
    SelectionButtonLog log_listener;
    SelectionButtonShadowType shadow_type_listener;
    SelectionButtonSave save_listener;
    SelectionButtonCamReset cam_reset_listener;
    SelectionButtonDebugCamera debug_camera_listener;

    Auth3dTestWindow();
    virtual ~Auth3dTestWindow() override;

    void Hide() override;

    void SetAuth3dCategoryIndex(size_t index);
    void SetAuth3dIndex(size_t index);
    void SetMaxFrame(float_t max_frame);
    void UpdatePlayButton();

    static void CategoryCharaMenuInit(dw::Button* button, dw::Menu*& menu,
        std::vector<std::string>& items, dw::SelectionListener* selection_listener);
    static void LeftRightReverseCallback(bool value);
    static void PosCallback(bool value);
    static void RepeatCallback(bool value);
};

extern Auth3dTestTask* auth_3d_test_task;
extern Auth3dTestWindow* auth_3d_test_window;

extern void auth_3d_test_task_init();
extern void auth_3d_test_task_disp_chara();
extern void auth_3d_test_task_free();

extern void auth_3d_test_window_init();
