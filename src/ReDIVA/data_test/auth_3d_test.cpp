/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d_test.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/effect.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../dw.hpp"
#include "../input_state.hpp"

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

    void ResetData();
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

class SelectionButtonBool : public dw::SelectionAdapter {
public:
    SelectionButtonBool();
    virtual ~SelectionButtonBool() override;

    virtual void Callback(CallbackData* data) override;
};

extern render_context* rctx_ptr;

Auth3dTestTask* auth_3d_test_task;
Auth3dTestWindow* auth_3d_test_window;

static SelectionButtonBool selection_button_bool;

static bool snap_shot;

static void auth_3d_test_window_init();

Auth3dTestTask::Auth3dTestTask::Window::Window() {
    stage_link_change = true;
    obj_link = true;
}

Auth3dTestTask::Auth3dTestTask() {
    field_1C0 = 10;
    field_1C4 = 10;
    field_1C8 = 10;
    field_1CC = 10;
    field_1D0 = false;
    state = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    pos = false;
    snap_shot = false;
    snap_shot_state = 0;
    aet_index = false;
    auth_2d = false;
    black_mask = false;
    stage_link_change = true;
    effcmn_obj_set_state = 1;
    effcmn_obj_set = -1;
    aet_state = 1;
    plane_above_floor = false;
    stg_auth_display = true;
    stg_display = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    field_388 = false;
    field_38C = 0;
    save = false;
    load_state = 0;
}

Auth3dTestTask:: ~Auth3dTestTask() {

}

bool Auth3dTestTask::init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    field_1D0 = false;
    state = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    pos = false;
    snap_shot = false;
    snap_shot_state = 0;
    aet_index = false;
    auth_2d = false;
    black_mask = false;
    effcmn_obj_set_state = 1;
    effcmn_obj_set = aft_obj_db->get_object_set_id("EFFCMN");
    aet_state = 1;
    plane_above_floor = false;
    stg_auth_display = true;
    stg_display = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    save = false;
    load_state = 0;
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    obj_sets.clear();
    obj_sets.shrink_to_fit();

    shadow_ptr_get()->self_shadow = true;
    clear_color = 0x00999999;
    auth_3d_test_window_init();
    task_stage_add_task("A3D_STAGE");
    objset_info_storage_load_set(aft_data, aft_obj_db, effcmn_obj_set);
    return true;
}

bool Auth3dTestTask::ctrl() {
    InputState* input_state = input_state_get(0);
    if (auth_3d_test_task->auth_3d_id.check_not_empty() && input_state->CheckTapped(29))
        auth_3d_test_task->auth_3d_id.set_paused(
            !auth_3d_test_task->auth_3d_id.get_paused());

    SetStage();
    SetAuth3dId();

    //event_listener.data.sub_140247B60();

    if (effcmn_obj_set_state == 1 && !objset_info_storage_load_obj_set_check_not_read(effcmn_obj_set))
        effcmn_obj_set_state = 4;

    if (state == 1 && auth_3d_id.check_loaded())
        state = 2;

    if (state == 2/* && event_listener.data.field_8 == 4 && !event_listener.data.field_C*/) {
        auth_3d_id.set_enable(true);
        auth_3d_id.set_paused(false);
        auth_3d_id.set_repeat(auth_3d_test_task->repeat);
        auth_3d_id.set_left_right_reverse(auth_3d_test_task->left_right_reverse);
        auth_3d_id.set_pos(auth_3d_test_task->pos);
        state = 4;
    }

    if (auth_3d_id.check_not_empty()) {
        float_t frame = auth_3d_id.get_frame();
        if (auth_3d_test_window) {
            auth_3d_test_window->frame_slider->SetValue(frame);
            auth_3d_test_window->UpdatePlayButton();
            auth_3d_test_window->trans_x->SetValue(trans_value.x);
            auth_3d_test_window->trans_z->SetValue(trans_value.z);
            auth_3d_test_window->rot_y->SetValue(rot_y_value);
            auth_3d_test_window->save->SetEnabled(save);
        }

        mat4 mat;
        mat4_translate(&trans_value, &mat);
        mat4_mul_rotate_y(&mat, rot_y_value * DEG_TO_RAD_FLOAT, &mat);
        auth_3d_id.set_mat(mat);
    }

    sub_140244610();
    return false;
}

bool Auth3dTestTask::dest() {
    auth_3d_id.unload(rctx_ptr);
    objset_info_storage_unload_set(effcmn_obj_set);
    task_stage_del_task();
    auth_3d_test_window->Hide();
    clear_color = color_black;
    shadow_ptr_get()->self_shadow = true;
    if (category.size())
        auth_3d_data_unload_category(category.c_str());
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    for (uint32_t& i : obj_sets)
        objset_info_storage_unload_set(i);
    obj_sets.clear();
    obj_sets.shrink_to_fit();
    return true;
}

void Auth3dTestTask::disp() {
    if (plane_above_floor) {
        mat4 mat;
        mat4_translate_y(0.25f, &mat);
        mdl::EtcObj etc(mdl::ETC_OBJ_PLANE);
        etc.color = 0xFFFFFFFF;
        etc.data.plane.w = 20;
        etc.data.plane.h = 20;
        rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
    }
}

void Auth3dTestTask::DispAuth3dChara(::auth_3d_id& id) {
    id.get_uid(); // ???
    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(0);
    if (!rob_itm_equip)
        return;

    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++)
        rob_itm_equip->set_disp((item_id)i, true);
}

void Auth3dTestTask::DispChara() {
    if (auth_3d_id.check_not_empty())
        Auth3dTestTask::DispAuth3dChara(auth_3d_id);
}

void Auth3dTestTask::SetAuth3dId() {
    if (task_stage_check_not_loaded() || auth_3d_uid == -1
        || !load_category.size() || auth_3d_uid == auth_3d_id.get_uid())
        return;

    if (load_category.compare(category)) {
        if (category.size()) {
            auth_3d_data_unload_category(category.c_str());
            for (uint32_t& i : obj_sets)
                objset_info_storage_unload_set(i);
            obj_sets.clear();
        }
        auth_3d_data_load_category(load_category.c_str());
        category.assign(load_category);
        if (window.obj_link)
            load_state = 1;
    }

    if (!auth_3d_data_check_category_loaded(category.c_str()))
        return;

    if (load_state == 1) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;

        auth_3d_data_get_obj_sets_from_category(category, obj_sets, aft_auth_3d_db, aft_obj_db);

        for (uint32_t& i : obj_sets)
            objset_info_storage_load_set(aft_data, aft_obj_db, i);
        load_state = 2;
    }
    else if (load_state == 2) {
        bool wait_load = false;
        for (uint32_t& i : obj_sets)
            if (objset_info_storage_load_obj_set_check_not_read(i))
                wait_load = true;

        if (!wait_load)
            load_state = 0;
    }

    if (!load_state) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

        auth_3d_id.unload(rctx_ptr);
        auth_3d_id = ::auth_3d_id(auth_3d_uid, aft_auth_3d_db);
        if (auth_3d_id.check_not_empty()) {
            auth_3d_id.set_enable(false);
            auth_3d_id.read_file(aft_auth_3d_db);
            state = 1;
        }
        auth_3d_uid = -1;
        return;
    }
}

void Auth3dTestTask::SetAuth3dUid(int32_t value) {
    auth_3d_uid = value;
}

void Auth3dTestTask::SetAuth3dUidByName(const char* str) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    SetAuth3dUid(aft_auth_3d_db->get_uid(str));
}

void Auth3dTestTask::SetLoadCategory(const char* str) {
    if (str)
        load_category.assign(str);
}

void Auth3dTestTask::SetRotY(float_t value) {
    this->rot_y_value = value;
}

void Auth3dTestTask::SetStage() {
    if (window.stage_link_change) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        const char* name = auth_3d_data_get_uid_name(auth_3d_uid, aft_auth_3d_db);
        size_t name_length = utf8_length(name);
        if (name && name_length >= 3) {
            size_t v5 = name_length - 2;
            const char* v6 = name;
            while (v5) {
                const char* v7 = (const char*)memchr(v6, 'S', v5);
                if (!v7)
                    break;

                if (memcmp(v7, "STG", min_def(v5, 3))) {
                    v5 += v6 - v7 - 1;
                    v6 = v7 + 1;
                    continue;
                }

                size_t v16 = v7 - name;
                if (v16 == -1 || v16 >= name_length || name_length == v16)
                    break;

                size_t v17 = name_length - v16;
                for (const char* i = name + v16; v17; ) {
                    const char* v20 = (const char*)memchr(i, '_', v17);
                    if (!v20)
                        break;

                    if (*v20 != '_') {
                        v17 += i - v20 - 1;
                        i = v20 + 1;
                        continue;
                    }

                    size_t v24 = v20 - name;
                    if (v24 != -1) {
                        std::string v30(name + v16, v24 - v16);
                        int32_t stage_index = aft_stage_data->get_stage_index(v30.c_str());
                        if (this->stage_index != stage_index)
                            load_stage_index = stage_index;
                    }
                    break;
                }
                break;
            }
        }
    }

    if (load_stage_index != -1 && load_stage_index != stage_index) {
        task_stage_set_stage_index(load_stage_index);
        stage_index = load_stage_index;
        load_stage_index = -1;
    }
}

void Auth3dTestTask::SetTransAxis(float_t value, int32_t axis) {
    switch (axis) {
    case 0:
        trans_value.x = value;
        break;
    case 1:
        trans_value.y = value;
        break;
    case 2:
        trans_value.z = value;
        break;
    }
}

void Auth3dTestTask::sub_140244610() {
    if (auth_2d && aet_index) {
        task_stage_current_set_stage_display(false, true);
        effect_manager_set_enable(false);
    }
    else {
        task_stage_current_set_stage_display(stg_auth_display, true);
        effect_manager_set_enable(stg_display);
    }
}

void auth_3d_test_task_init() {
    auth_3d_test_task = new Auth3dTestTask;
}

void auth_3d_test_task_disp_chara() {
    auth_3d_test_task->DispChara();
}

void auth_3d_test_task_free() {
    if (auth_3d_test_task) {
        delete auth_3d_test_task;
        auth_3d_test_task = 0;
    }
}

Auth3dTestRobWindow::SelectionListRob::SelectionListRob() {

}

Auth3dTestRobWindow::SelectionListRob::~SelectionListRob() {

}

void Auth3dTestRobWindow::SelectionListRob::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        ::chara_index chara_index = (::chara_index)list_box->list->selected_item;
        int32_t chara_id = list_box->callback_data.i64 ? 1 : 0;

        //auth_3d_test_task->event_listener.data
        //    .chara_index[list_box->callback_data.i64 != 0] = chara_index;
        //auth_3d_test_task->event_listener.data.field_C = true;

        Auth3dTestRobWindow* rob_window = dynamic_cast<Auth3dTestRobWindow*>(list_box->parent_comp);
        if (rob_window)
            rob_window->GetCharaCos(chara_index, chara_id, rob_window->costume_list[chara_id]);
    }
}

Auth3dTestRobWindow::SelectionListDispStyle::SelectionListDispStyle() {

}

Auth3dTestRobWindow::SelectionListDispStyle::~SelectionListDispStyle() {

}

void Auth3dTestRobWindow::SelectionListDispStyle::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        size_t selected_item = list_box->list->selected_item;
        int32_t cos = atoi(list_box->GetSelectedItemStr().substr(0, 3).c_str()) - 1;
        int32_t chara_id = list_box->callback_data.i64 ? 1 : 0;

        Auth3dTestRobWindow* rob_window = dynamic_cast<Auth3dTestRobWindow*>(list_box->parent_comp);
        if (rob_window) {
            ::chara_index chara_index = (::chara_index)rob_window->chara_list[chara_id]->list->selected_item;
            //auth_3d_test_task->event_listener.data.chara_index[v6] = chara_index;
            //auth_3d_test_task->event_listener.data.field_C = true;
            rob_window->GetCharaCos(chara_index, chara_id, rob_window->costume_list[chara_id]);
            list_box->SetItemIndex(selected_item);
        }
        //auth_3d_test_task->event_listener.data.cos[chara_id] = cos;

        int32_t uid = auth_3d_test_task->auth_3d_id.get_uid();
        auth_3d_test_task->auth_3d_id.unload(rctx_ptr);
        auth_3d_test_task->SetAuth3dUid(uid);
    }
}

Auth3dTestRobWindow::Auth3dTestRobWindow() : chara_list(), costume_list()/*, field_1A0()*/ {
    SetText("A3D CHARA");

    dw::ListBox* chara_list = new dw::ListBox(this);
    chara_list->SetText("CHARAlist");

    for (int32_t i = 0; i < CHARA_MAX; i++)
        chara_list->AddItem(chara_index_get_name((chara_index)i));
    chara_list->AddSelectionListener(&selection_list_rob);
    chara_list->callback_data.i64 = 0;
    this->chara_list[0] = chara_list;

    dw::ListBox* costume_list = new dw::ListBox(this);
    costume_list->SetText("COSTUMElist");
    GetCharaCos(CHARA_MIKU, 0, costume_list);
    costume_list->AddSelectionListener(&selection_list_disp_style);
    costume_list->callback_data.i64 = 0;
    costume_list->list->SetItemIndex(0);
    this->costume_list[0] = costume_list;

    UpdateLayout();

    rect.pos = { 400.0f, 0.0f };
}

Auth3dTestRobWindow::~Auth3dTestRobWindow() {

}

void Auth3dTestRobWindow::Hide() {
    SetDisp();
}

void Auth3dTestRobWindow::GetCharaCos(::chara_index chara_index, int32_t chara_id, dw::ListBox* list_box) {
    list_box->ClearItems();

    for (const auto& i : item_table_handler_array_get_table(chara_index)->cos) {
        if (!i.second.data.outer)
            continue;

        const item_table_item* item = item_table_handler_array_get_item(chara_index, i.second.data.outer);
        if (item) {
            char buf[0x100];
            sprintf_s(buf, sizeof(buf), "%03d %s", i.first + 1, item->name.c_str());
            list_box->AddItem(buf);
        }
    }
}

Auth3dTestSubWindow::SelectionListStage::SelectionListStage() {

}

Auth3dTestSubWindow::SelectionListStage::~SelectionListStage() {

}

void Auth3dTestSubWindow::SelectionListStage::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;
        auth_3d_test_task->load_stage_index = aft_stage_data->get_stage_index(
            list_box->GetSelectedItemStr().c_str());
    }
}

Auth3dTestSubWindow::SelectionListAet::SelectionListAet() {

}

Auth3dTestSubWindow::SelectionListAet::~SelectionListAet() {

}

void Auth3dTestSubWindow::SelectionListAet::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box)
        if (!list_box->list->selected_item) {
            //auth_3d_test_task->aet_id = -1;
            auth_3d_test_task->aet_index = 1;
        }
}

Auth3dTestSubWindow::Auth3dTestSubWindow() {
    SetText("A3D STAGE");

    const char* plane_above_floor_text;
    if (dw::translate)
        plane_above_floor_text = u8"Plane 25 cm above floor";
    else
        plane_above_floor_text = u8"床上25cm";

    plane_above_floor = new dw::Button(this, dw::CHECKBOX);
    plane_above_floor->SetText(plane_above_floor_text);
    plane_above_floor->SetValue(auth_3d_test_task->plane_above_floor);
    plane_above_floor->callback_data.v64 = &auth_3d_test_task->plane_above_floor;
    plane_above_floor->AddSelectionListener(&selection_button_bool);

    stage = new dw::Button(this, dw::CHECKBOX);
    stage->SetText("stage");
    stage->SetValue(auth_3d_test_task->stg_auth_display);
    stage->callback_data.v64 = &auth_3d_test_task->stg_auth_display;
    stage->AddSelectionListener(&selection_button_bool);

    stg_auth = new dw::Button(this, dw::CHECKBOX);
    stg_auth->SetText("stg auth");
    stg_auth->SetValue(auth_3d_test_task->stg_display);
    stg_auth->callback_data.v64 = &auth_3d_test_task->stg_display;
    stg_auth->AddSelectionListener(&selection_button_bool);

    stg_list = new dw::ListBox(this);
    stg_list->SetText("STAGElist");

    {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        size_t stage_count = aft_stage_data->stage_data.size();
        std::vector<std::string> vec;
        for (size_t i = 0; i < stage_count; i++)
            vec.push_back(aft_stage_data->stage_data[i].name);

        std::sort(vec.begin(), vec.end());

        size_t stage_index = 0;
        std::string& stg_name = aft_stage_data->stage_data[0].name;
        for (size_t i = 0; i < stage_count; i++) {
            stg_list->AddItem(vec[i]);
            if (!vec[i].compare(stg_name))
                stage_index = i;
        }

        stg_list->SetItemIndex(stage_index);
    }

    stg_list->list->AddSelectionListener(&selection_list_stage);

    const char* stage_link_change_text;
    const char* auth_2d_text;
    if (dw::translate) {
        stage_link_change_text = u8"Stage Link Change";
        auth_2d_text = u8"2D Auth";
    }
    else {
        stage_link_change_text = u8"STG連動切り替え";
        auth_2d_text = u8"2Dオーサ";
    }

    stage_link_change = new dw::Button(this, dw::CHECKBOX);
    stage_link_change->SetText(stage_link_change_text);
    stage_link_change->SetValue(auth_3d_test_task->stage_link_change);
    stage_link_change->callback_data.v64 = &auth_3d_test_task->stage_link_change;
    stage_link_change->AddSelectionListener(&selection_button_bool);

    auth_2d = new dw::Button(this, dw::CHECKBOX);
    auth_2d->SetText(auth_2d_text);
    auth_2d->SetValue(auth_3d_test_task->auth_2d);
    auth_2d->callback_data.v64 = &auth_3d_test_task->auth_2d;
    auth_2d->AddSelectionListener(&selection_button_bool);

    aet_list = new dw::ListBox(this);
    aet_list->SetText("AETlist");
    aet_list->AddItem("INVALID");
    aet_list->SetItemIndex(0);
    aet_list->AddSelectionListener(&selection_list_aet);

    UpdateLayout();

    rect.pos = { 250.0f, 0.0f };
}

Auth3dTestSubWindow::~Auth3dTestSubWindow() {

}

void Auth3dTestSubWindow::Hide() {
    SetDisp();
}

void Auth3dTestSubWindow::ResetAet() {
    auth_2d->SetValue(false);
    aet_list->SetItemIndex(0);
}

Auth3dTestWindow::SelectionCategoryMenuItem::SelectionCategoryMenuItem() {

}

Auth3dTestWindow::SelectionCategoryMenuItem::~SelectionCategoryMenuItem() {

}

void Auth3dTestWindow::SelectionCategoryMenuItem::Callback(dw::SelectionListener::CallbackData* data) {
    /*dw::MenuItem* menu_item = dynamic_cast<dw::MenuItem*>(data->widget);
    if (menu_item) {
        dw::Menu* menu = menu_item->GetMenu();
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(menu->parent_shell);
        if (test_window)
            test_window->SetAuth3dCategoryName(menu_item->GetTextStr());
    }*/
}

Auth3dTestWindow::SelectionCategoryList::SelectionCategoryList() {

}

Auth3dTestWindow::SelectionCategoryList::~SelectionCategoryList() {

}

void Auth3dTestWindow::SelectionCategoryList::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(list_box->parent_shell);
        if (test_window)
            test_window->SetAuth3dCategoryIndex(list_box->list->selected_item);
    }
}

Auth3dTestWindow::SelectionIDMenuItem::SelectionIDMenuItem() {

}

Auth3dTestWindow::SelectionIDMenuItem::~SelectionIDMenuItem() {

}

void Auth3dTestWindow::SelectionIDMenuItem::Callback(dw::SelectionListener::CallbackData* data) {
    /*dw::MenuItem* menu_item = dynamic_cast<dw::MenuItem*>(data->widget);
    if (menu_item) {
        dw::Menu* menu = menu_item->GetMenu();
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(menu->parent_shell);
        if (test_window)
            test_window->SetAuth3dName(menu_item->GetTextStr());
    }*/
}

Auth3dTestWindow::SelectionList::SelectionList() {

}

Auth3dTestWindow::SelectionList::~SelectionList() {

}

void Auth3dTestWindow::SelectionList::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(list_box->parent_shell);
        if (test_window)
            test_window->SetAuth3dIndex(list_box->list->selected_item);
    }
}

Auth3dTestWindow::SelectionSliderFrame::SelectionSliderFrame() {

}

Auth3dTestWindow::SelectionSliderFrame::~SelectionSliderFrame() {

}

void Auth3dTestWindow::SelectionSliderFrame::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(slider->parent_shell);
        if (test_window && auth_3d_test_task->auth_3d_id.check_not_empty()) {
            auth_3d_test_task->auth_3d_id.set_paused(true);
            test_window->UpdatePlayButton();
            auth_3d_test_task->auth_3d_id.set_req_frame(slider->GetValue());
        }
    }
}

Auth3dTestWindow::SelectionSliderTransElement::SelectionSliderTransElement() : axis() {

}

Auth3dTestWindow::SelectionSliderTransElement::~SelectionSliderTransElement() {

}

void Auth3dTestWindow::SelectionSliderTransElement::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        auth_3d_test_task->SetTransAxis(slider->GetValue(), axis);
}

Auth3dTestWindow::SelectionSliderRotY::SelectionSliderRotY() {

}

Auth3dTestWindow::SelectionSliderRotY::~SelectionSliderRotY() {

}

void Auth3dTestWindow::SelectionSliderRotY::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        auth_3d_test_task->SetRotY(slider->GetValue());
}

Auth3dTestWindow::SelectionButtonPlay::SelectionButtonPlay() {

}

Auth3dTestWindow::SelectionButtonPlay::~SelectionButtonPlay() {

}

void Auth3dTestWindow::SelectionButtonPlay::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(button->parent_shell);
        if (test_window && auth_3d_test_task->auth_3d_id.check_not_empty()) {
            auth_3d_test_task->auth_3d_id.set_paused(!auth_3d_test_task->auth_3d_id.get_paused());
            test_window->UpdatePlayButton();
        }
    }
}

Auth3dTestWindow::SelectionButtonBegin::SelectionButtonBegin() {

}

Auth3dTestWindow::SelectionButtonBegin::~SelectionButtonBegin() {

}

void Auth3dTestWindow::SelectionButtonBegin::Callback(dw::SelectionListener::CallbackData* data) {
    if (!auth_3d_test_task->auth_3d_id.check_not_empty())
        return;

    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(button->parent_shell);
        if (test_window && auth_3d_test_task->auth_3d_id.check_not_empty()) {
            auth_3d_test_task->auth_3d_id.set_paused(false);
            test_window->UpdatePlayButton();
            auth_3d_test_task->auth_3d_id.set_req_frame(0.0f);
            if (::snap_shot) {
                auth_3d_test_task->snap_shot = true;
                sub_140194880(0);
            }
        }
    }
}

Auth3dTestWindow::SelectionButtonEnd::SelectionButtonEnd() {

}

Auth3dTestWindow::SelectionButtonEnd::~SelectionButtonEnd() {

}

void Auth3dTestWindow::SelectionButtonEnd::Callback(dw::SelectionListener::CallbackData* data) {
    if (!auth_3d_test_task->auth_3d_id.check_not_empty())
        return;

    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(button->parent_shell);
        if (test_window && auth_3d_test_task->auth_3d_id.check_not_empty()) {
            auth_3d_test_task->auth_3d_id.set_paused(false);
            test_window->UpdatePlayButton();
            auth_3d_test_task->auth_3d_id.set_req_frame(
                auth_3d_test_task->auth_3d_id.get_play_control_size());
        }
    }
}

Auth3dTestWindow::SelectionButtonBindBoolFunc::SelectionButtonBindBoolFunc() : callback() {

}

Auth3dTestWindow::SelectionButtonBindBoolFunc::~SelectionButtonBindBoolFunc() {

}

void Auth3dTestWindow::SelectionButtonBindBoolFunc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button)
        callback(button->value);
}

Auth3dTestWindow::SelectionButtonSnapShot::SelectionButtonSnapShot() {

}

Auth3dTestWindow::SelectionButtonSnapShot::~SelectionButtonSnapShot() {

}

void Auth3dTestWindow::SelectionButtonSnapShot::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Auth3dTestWindow* test_window = dynamic_cast<Auth3dTestWindow*>(button->parent_shell);
        if (test_window) {
            ::snap_shot = button->value;
            if (::snap_shot) {
                Auth3dTestWindow::RepeatCallback(false);
                test_window->repeat->SetValue(false);
            }
        }
    }
}

Auth3dTestWindow::SelectionButtonLog::SelectionButtonLog() {

}

Auth3dTestWindow::SelectionButtonLog::~SelectionButtonLog() {

}

void Auth3dTestWindow::SelectionButtonLog::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
}

Auth3dTestWindow::SelectionButtonShadowType::SelectionButtonShadowType() {

}

Auth3dTestWindow::SelectionButtonShadowType::~SelectionButtonShadowType() {

}

void Auth3dTestWindow::SelectionButtonShadowType::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button)
        shadow_ptr_get()->self_shadow = !!button->callback_data.i64;
}

Auth3dTestWindow::SelectionButtonSave::SelectionButtonSave() {

}

Auth3dTestWindow::SelectionButtonSave::~SelectionButtonSave() {

}

void Auth3dTestWindow::SelectionButtonSave::Callback(dw::SelectionListener::CallbackData* data) {

}

Auth3dTestWindow::SelectionButtonCamReset::SelectionButtonCamReset() {

}

Auth3dTestWindow::SelectionButtonCamReset::~SelectionButtonCamReset() {

}

void Auth3dTestWindow::SelectionButtonCamReset::Callback(dw::SelectionListener::CallbackData* data) {
    cam_struct cam;
    cam.set(rctx_ptr->camera);
}

Auth3dTestWindow::SelectionButtonDebugCamera::SelectionButtonDebugCamera() {

}

Auth3dTestWindow::SelectionButtonDebugCamera::~SelectionButtonDebugCamera() {

}

void Auth3dTestWindow::SelectionButtonDebugCamera::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
}

Auth3dTestWindow::Auth3dTestWindow() {
    SetText("AUTH3D");

    const char* nage_text;
    const char* obj_link_text;
    const char* left_right_reverse_text;
    const char* snap_shot_text;
    const char* self_shadow_off_text;
    const char* black_mask_text;
    const char* offset_save_text;
    if (dw::translate) {
        nage_text = u8"NAGE▽";
        obj_link_text = u8"OBJ Link";
        left_right_reverse_text = u8"Left Right Reverse";
        snap_shot_text = u8"ss capture";
        self_shadow_off_text = u8"Self Shadow Off";
        black_mask_text = u8"Black Mask";
        offset_save_text = u8"Offset Save";
    }
    else {
        nage_text = u8"投▽";
        obj_link_text = u8"OBJ連動";
        left_right_reverse_text = u8"左右逆";
        snap_shot_text = u8"ss画撮";
        self_shadow_off_text = u8"セルフ影OFF";
        black_mask_text = u8"黒mask";
        offset_save_text = u8"オフセットSave";
    }

    dw::Composite* v4 = new dw::Composite(this);
    v4->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(v4))->SetText(" ");

    chara = new dw::Button(v4, dw::FLAG_8);
    chara->SetText(u8"CHARA▽");
    chara_menu = 0;

    stg = new dw::Button(v4, dw::FLAG_8);
    stg->SetText(u8"STG▽");
    stg_menu = 0;

    eff = new dw::Button(v4, dw::FLAG_8);
    eff->SetText(u8"EFF▽");
    eff_menu = 0;

    nage = new dw::Button(v4, dw::FLAG_8);
    nage->SetText(nage_text);
    nage_menu = 0;

    category_list = new dw::ListBox(this);
    category_list->SetText("CATEGORYlist");
    category_list->SetMaxItems(20);

    {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

        std::vector<std::string> chara_categories;
        std::vector<std::string> stg_categories;
        std::vector<std::string> eff_categories;
        std::vector<std::string> nage_categories;

        categories.clear();
        categories.resize(aft_auth_3d_db->category.size());

        for (auth_3d_database_category& i : aft_auth_3d_db->category) {
            category_list->AddItem(i.name);

            if (chara_index_get_from_chara_name(i.name.c_str()) != CHARA_MAX)
                chara_categories.push_back(i.name);
            else if (!i.name.find("STG"))
                stg_categories.push_back(i.name);
            else if (!i.name.find("EFF"))
                eff_categories.push_back(i.name);
            else if (!i.name.find("NAGE"))
                nage_categories.push_back(i.name);
        }

        Auth3dTestWindow::CategoryCharaMenuInit(chara, chara_menu,
            chara_categories, &category_menu_item_listener);
        Auth3dTestWindow::CategoryCharaMenuInit(stg, stg_menu,
            stg_categories, &category_menu_item_listener);
        Auth3dTestWindow::CategoryCharaMenuInit(eff, eff_menu,
            eff_categories, &category_menu_item_listener);
        Auth3dTestWindow::CategoryCharaMenuInit(nage, nage_menu,
            nage_categories, &category_menu_item_listener);
    }

    category_list->AddSelectionListener(&category_list_listener);

    dw::Composite* v44 = new dw::Composite(this);
    v44->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(v44))->SetText(" ID  ");

    obj_link = new dw::Button(v44, dw::CHECKBOX);
    obj_link->SetText(obj_link_text);
    obj_link->SetValue(true);

    id_list = new dw::ListBox(this);
    id_list->SetText("IDlist");
    id_list->SetMaxItems(20);
    id_list->AddSelectionListener(&list_listener);

    frame = new dw::Label(this, dw::FLAG_4000);
    frame->SetText("");

    frame_slider = dw::Slider::Create(this, dw::HORIZONTAL, "", "% 5.0f", 160.0f);
    frame_slider->AddSelectionListener(&slider_frame_listener);
    frame_slider->SetRound(true);

    SetMaxFrame(100.0f);

    dw::Composite* v59 = new dw::Composite(this);
    v59->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    begin = new dw::Button(v59, dw::FLAG_8);
    begin->SetText("|<<");
    begin->AddSelectionListener(&begin_listener);

    play = new dw::Button(v59, (dw::Flags)(dw::FLAG_1000000 | dw::FLAG_8));
    play->SetText(" > ");
    play->AddSelectionListener(&play_listener);

    end = new dw::Button(v59, dw::FLAG_8);
    end->SetText(">>|");
    end->AddSelectionListener(&end_listener);

    repeat = new dw::Button(v59, dw::CHECKBOX);
    repeat->SetText("repeat");
    repeat->SetValue(auth_3d_test_task->repeat);

    repeat_listener.callback = Auth3dTestWindow::RepeatCallback;
    repeat->AddSelectionListener(&repeat_listener);

    left_right_reverse = new dw::Button(this, dw::CHECKBOX);
    left_right_reverse->SetText(left_right_reverse_text);
    left_right_reverse->SetValue(auth_3d_test_task->left_right_reverse);
    left_right_reverse_listener.callback = Auth3dTestWindow::LeftRightReverseCallback;
    left_right_reverse->AddSelectionListener(&left_right_reverse_listener);

    dw::Composite* v79 = new dw::Composite(this);
    v79->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    pos = new dw::Button(v79, dw::CHECKBOX);
    pos->SetText("pos");
    pos->SetValue(auth_3d_test_task->pos);

    pos_listener.callback = Auth3dTestWindow::PosCallback;
    pos->AddSelectionListener(&pos_listener);

    log = new dw::Button(v79, dw::CHECKBOX);
    log->SetText("log");
    log->SetValue(false /*!!sub_140248B90()*/);
    log->AddSelectionListener(&log_listener);

    (new dw::Label(v79))->SetText("   ");

    snap_shot = new dw::Button(v79, dw::CHECKBOX);
    snap_shot->SetText(snap_shot_text);
    snap_shot->SetValue(snap_shot);
    snap_shot->AddSelectionListener(&snap_shot_listener);

    dw::Composite* v97 = new dw::Composite(this);
    v97->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    self_shadow_off = new dw::Button(v97, dw::RADIOBUTTON);
    self_shadow_off->SetText(self_shadow_off_text);
    self_shadow_off->callback_data.i64 = 0;
    self_shadow_off->AddSelectionListener(&shadow_type_listener);

    self_shadow_on = new dw::Button(v97, dw::RADIOBUTTON);
    self_shadow_on->SetText("ON");
    self_shadow_on->callback_data.i64 = 1;
    self_shadow_on->SetValue(1);
    self_shadow_on->AddSelectionListener(&shadow_type_listener);

    black_mask = new dw::Button(this, dw::CHECKBOX);
    black_mask->SetText(black_mask_text);
    black_mask->SetValue(auth_3d_test_task->black_mask);
    black_mask->callback_data.v64 = &auth_3d_test_task->black_mask;
    black_mask->AddSelectionListener(&selection_button_bool);

    trans_x_listener.axis = 0;
    trans_x = dw::Slider::Create(this, dw::HORIZONTAL, "transX", "%4.2f", 120.0f);
    trans_x->SetParams(0.0f, -5.0f, 5.0f, 2.5f, 0.1f, 1.0f);
    trans_x->AddSelectionListener(&trans_x_listener);
    trans_x->SetRound(true);

    trans_z_listener.axis = 2;
    trans_z = dw::Slider::Create(this, dw::HORIZONTAL, "transZ", "%4.2f", 120.0f);
    trans_z->SetParams(0.0f, -5.0f, 5.0f, 2.5f, 0.1f, 1.0f);
    trans_z->AddSelectionListener(&trans_z_listener);
    trans_z->SetRound(true);

    rot_y = dw::Slider::Create(this, dw::HORIZONTAL, "rotY", "%4.0f", 120.0f);
    rot_y->SetParams(0.0f, -360.0f, 360.0f, 180.0f, 1.0f, 10.0);
    rot_y->AddSelectionListener(&rot_y_listener);
    rot_y->SetRound(true);

    save = new dw::Button(this, dw::FLAG_8);
    save->SetText(offset_save_text);
    save->AddSelectionListener(&save_listener);

    dw::Composite* v117 = new dw::Composite(this);
    v117->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* cam_reset = new dw::Button(v117, dw::FLAG_8);
    cam_reset->SetText("cam reset");
    cam_reset->AddSelectionListener(&cam_reset_listener);

    dw::Button* debug_camera = new dw::Button(v117, dw::CHECKBOX);
    debug_camera->SetText("DEBUG CAMERA");
    debug_camera->AddSelectionListener(&debug_camera_listener);

    UpdateLayout();

    if (category_list->GetItemCount())
        SetAuth3dCategoryIndex(0);
}

Auth3dTestWindow::~Auth3dTestWindow() {

}

void Auth3dTestWindow::Hide() {
    SetDisp();
    sub_window->ResetAet();
    sub_window->Hide();
    rob_window->Hide();
}

void Auth3dTestWindow::ResetData() {
    frame_slider->SetParams(0.0f, 0.0f, 100.0f, 10.0f, 1.0f, 10.0f);
    category_list->SetItemIndex(-1);
    id_list->SetItemIndex(-1);
}

void Auth3dTestWindow::SetAuth3dCategoryIndex(size_t index) {
    id_list->ClearItems();
    category_list->SetItemIndex(index);

    auth_3d_test_task->SetLoadCategory(category_list->GetItemStr(index).c_str());

    if (index >= category_list->GetItemCount())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

    const auth_3d_database_category& category = aft_auth_3d_db->category[index];

    size_t uids_count = category.uid.size();
    if (uids_count) {
        std::vector<std::string> vec;
        for (size_t i = 0; i < uids_count; i++)
            vec.push_back(aft_auth_3d_db->uid[category.uid[i]].name);

        std::sort(vec.begin(), vec.end());
        for (std::string& i : vec)
            id_list->AddItem(i);
    }

    if (index < categories.size())
        SetAuth3dIndex(categories[index]);
}

void Auth3dTestWindow::SetAuth3dIndex(size_t index) {
    id_list->SetItemIndex(index);

    if (auth_3d_test_task->auth_3d_id.check_not_empty()) {
        auth_3d_test_task->auth_3d_id.set_paused(true);
        UpdatePlayButton();
    }

    if (index < id_list->GetItemCount())
        auth_3d_test_task->SetAuth3dUidByName(id_list->GetItemStr(index).c_str());
    else
        index = -1;

    size_t selected_item = category_list->list->selected_item;
    if (selected_item < categories.size())
        categories[selected_item] = index;
}

void Auth3dTestWindow::SetMaxFrame(float_t max_frame) {
    frame_slider->SetMin(0.0f);
    frame_slider->SetMax(max_frame);
    frame_slider->SetGrab(max_frame * 0.25f);

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "frame[ 0,%4.0f)", max_frame);
    frame->SetText(buf);
}

void Auth3dTestWindow::UpdatePlayButton() {
    if (auth_3d_test_task->auth_3d_id.check_not_empty())
        play->SetText(auth_3d_test_task->auth_3d_id.get_paused() ? " > " : "||");
}

void Auth3dTestWindow::CategoryCharaMenuInit(dw::Button* button, dw::Menu*& menu,
    std::vector<std::string>& items, dw::SelectionListener* selection_listener) {
    if (!items.size()) {
        button->SetEnabled(false);
        return;
    }

    /*menu = new dw::Menu(button);
    menu->SetText("CategoryCharaMenu");

    for (const std::string& i : items) {
        dw::MenuItem* menu_item = new dw::MenuItem(menu, dw::FLAG_8);
        menu_item->SetText(i);
        menu_item->AddSelectionListener(selection_listener);
    }

    button->SetEnabled(true);
    button->SetParentMenu(menu);*/
}

void Auth3dTestWindow::LeftRightReverseCallback(bool value) {
    auth_3d_test_task->left_right_reverse = value;
    if (auth_3d_test_task->auth_3d_id.check_not_empty()) {
        auth_3d_test_task->auth_3d_id.set_req_frame(0.0f);
        auth_3d_test_task->auth_3d_id.set_left_right_reverse(value);
    }
}

void Auth3dTestWindow::PosCallback(bool value) {
    auth_3d_test_task->pos = value;
    if (auth_3d_test_task->auth_3d_id.check_not_empty())
        auth_3d_test_task->auth_3d_id.set_pos(value);
}

void Auth3dTestWindow::RepeatCallback(bool value) {
    auth_3d_test_task->repeat = value;
    if (auth_3d_test_task->auth_3d_id.check_not_empty())
        auth_3d_test_task->auth_3d_id.set_repeat(value);
}

SelectionButtonBool::SelectionButtonBool() {

}

SelectionButtonBool::~SelectionButtonBool() {

}

void SelectionButtonBool::Callback(CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        bool* p_bool = (bool*)button->callback_data.v64;
        if (p_bool)
            *p_bool = button->value;
    }
}

static void auth_3d_test_window_init() {
    if (!auth_3d_test_window) {
        Auth3dTestWindow* test_window = new Auth3dTestWindow;
        test_window->sub_1402F38B0();

        Auth3dTestSubWindow* sub_window = new Auth3dTestSubWindow;
        sub_window->sub_1402F38B0();
        test_window->sub_window = sub_window;

        Auth3dTestRobWindow* rob_window = new Auth3dTestRobWindow;
        rob_window->sub_1402F38B0();
        test_window->rob_window = rob_window;

        auth_3d_test_window = test_window;
    }
    else {
        auth_3d_test_window->ResetData();
        auth_3d_test_window->Disp();
        auth_3d_test_window->sub_window->Disp();
        auth_3d_test_window->rob_window->Disp();
    }
}
