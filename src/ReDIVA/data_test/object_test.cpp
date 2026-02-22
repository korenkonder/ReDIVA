/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object_test.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../dw.hpp"
#include "../input_state.hpp"

class DataTestObjectManager : public app::Task {
public:
    int32_t object_set_index;
    uint32_t object_set_id;
    int32_t obj_num;
    int32_t obj_index;
    vec3 rotation;
    mdl::ObjFlags obj_flags;
    int32_t state;

    DataTestObjectManager();
    virtual ~DataTestObjectManager() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void del_task();

    int32_t get_obj_num();
    const obj_bounding_sphere* get_object_bounding_sphere(int32_t obj_index);
    const char* get_object_name(int32_t obj_index);
    void set_object_index(int32_t value);
    void set_object_set_index(int32_t object_set_index);
    void set_rotation(const vec3& value);
    void set_shadow(bool value);
};

class DataTestObjDw : public dw::Shell {
public:
    dw::ListBox* objects;

    DataTestObjDw();
    virtual ~DataTestObjDw();

    virtual void Hide() override;

    virtual void ClearIDs();
    virtual void AddID(const char* value);

    static void CameraCallback(dw::Widget* data);
    static void ObjectCallback(dw::Widget* data);
    static void ObjectSetCallback(dw::Widget* data);
    static void ShadowCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
    static void StageDisplayCallback(dw::Widget* data);
};

extern render_context* rctx_ptr;

DataTestObjDw* data_test_obj_dw;
DataTestObjectManager* data_test_object_manager;
TaskDataTestObj* task_data_test_obj;

static void data_test_obj_dw_init();

TaskDataTestObj::Data::Data() : object_set_index(), curr_object_set_index(), obj_num(),
curr_obj_num(), cull_camera(), curr_cull_camera(), stage_index(), curr_stage_index(), stage_display() {
    object_index = -1;
    curr_object_index = -1;
    shadow = 1;
}

TaskDataTestObj::TaskDataTestObj() {

}

TaskDataTestObj::~TaskDataTestObj() {

}

bool TaskDataTestObj::init() {
    clear_color = 0xFF606060;

    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    data_test_obj_dw_init();
    task_stage_set_stage_index(data.stage_index);
    task_stage_current_set_stage_display(true, true);
    return true;
}

bool TaskDataTestObj::ctrl() {
     data_test_object_manager->set_object_set_index(data.object_set_index);

    if (data.object_set_index != data.curr_object_set_index) {
        data.curr_object_set_index = data.object_set_index;
        data.object_index = -1;

        data_test_object_manager->del_task();
    }

    data.obj_num = data_test_object_manager->get_obj_num();
    if (data.obj_num != data.curr_obj_num) {
        data.curr_obj_num = data.obj_num;

        data_test_obj_dw->ClearIDs();
        for (int32_t i = 0; i < data.obj_num; i++)
            data_test_obj_dw->AddID(data_test_object_manager->get_object_name(i));
    }

    if (data.object_index != data.curr_object_index) {
        data.curr_object_index = data.object_index;
        data_test_object_manager->set_object_index(data.object_index);
    }

    if (data.cull_camera != data.curr_cull_camera) {
        data.curr_cull_camera = data.cull_camera;

        if (data.cull_camera) {
            const obj_bounding_sphere* sphere = data_test_object_manager->get_object_bounding_sphere(data.object_index);
            if (sphere) {
                camera* cam = rctx_ptr->camera;

                float_t v13 = tanf(cam->get_fov() * DEG_TO_RAD_FLOAT * 0.5f);

                vec3 view_point;
                view_point.x = sphere->center.x;
                view_point.y = sphere->center.y;
                view_point.z = v13 * sphere->radius + sphere->center.z + 0.2f;

                vec3 interest;
                interest.x = sphere->center.x;
                interest.y = sphere->center.y;
                interest.z = 0.0f;

                cam->set_view_point(view_point);
                cam->set_interest(interest);
            }
        }
        else {
            camera* cam = rctx_ptr->camera;
            cam->set_view_point({ 0.0f, 1.0f, 3.45f });
            cam->set_interest({ 0.0f, 1.0f, 0.0f });
        }
    }

    task_stage_check_not_loaded();

    if (data.stage_index != data.curr_stage_index) {
        data.curr_stage_index = data.stage_index;
        task_stage_set_stage_index(data.stage_index);
    }

    data_test_object_manager->set_shadow(data.shadow != 0);

    switch (data.stage_display) {
    case 0:
        task_stage_current_set_stage_display(false, true);
        break;
    case 1:
        task_stage_current_set_stage_display(true, true);
        break;
    case 2:
        task_stage_current_set_stage_display(false, true);
        break;
    }

    const InputState* input_state = input_state_get(0);
    if (input_state->CheckDown(INPUT_BUTTON_A)) {
        if (input_state->CheckDown(INPUT_BUTTON_MOUSE_BUTTON_LEFT))
            data.rotation.y += (float_t)input_state->sub_14018CCC0(10);
        if (input_state->CheckDown(INPUT_BUTTON_MOUSE_BUTTON_MIDDLE))
            data.rotation.x += (float_t)input_state->sub_14018CCC0(10);
        if (input_state->CheckDown(INPUT_BUTTON_MOUSE_BUTTON_RIGHT))
            data.rotation.z += (float_t)input_state->sub_14018CCC0(10);

        data_test_object_manager->set_rotation(data.rotation);
    }

    if (input_state->CheckDoubleTapped(INPUT_BUTTON_A))
        data.rotation = 0.0f;
    return false;
}

bool TaskDataTestObj::dest() {
    data_test_object_manager->del_task();
    clear_color = color_black;
    data_test_obj_dw->Hide();
    return true;
}

void TaskDataTestObj::disp() {
    if (data.stage_display != 2)
        return;

    mdl::EtcObj etc(mdl::ETC_OBJ_GRID);
    etc.color = color_black;
    etc.data.grid.w = 40;
    etc.data.grid.h = 40;
    etc.data.grid.ws = 40;
    etc.data.grid.hs = 40;
    rctx_ptr->disp_manager->entry_obj_etc(mat4_identity, etc);
}

TaskDataTestObj::Data* TaskDataTestObj::get_data() {
    return &data;
}

void object_test_init() {
    if (!data_test_object_manager)
        data_test_object_manager = new DataTestObjectManager;

    if (!task_data_test_obj)
        task_data_test_obj = new TaskDataTestObj;
}

void object_test_free() {
    if (task_data_test_obj) {
        delete task_data_test_obj;
        task_data_test_obj = 0;
    }

    if (data_test_object_manager) {
        delete data_test_object_manager;
        data_test_object_manager = 0;
    }
}

DataTestObjectManager::DataTestObjectManager() : obj_num(), obj_flags(), state() {
    object_set_index = -1;
    object_set_id = -1;
    obj_index = -1;
}

DataTestObjectManager::~DataTestObjectManager() {

}

bool DataTestObjectManager::init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    obj_num = 0;
    obj_index = -1;
    state = 2;

    if (object_set_index < 0 || (int32_t)aft_obj_db->get_object_set_count() <= object_set_index) {
        state = 0;
        return true;
    }

    if (object_set_id != -1) {
        objset_info_storage_load_set(aft_data, aft_obj_db, object_set_id);
        state = 1;
    }

    objset_info_storage_load_set(aft_data, aft_obj_db, aft_obj_db->get_object_set_id(object_set_index));
    return true;
}

bool DataTestObjectManager::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    switch (state) {
    case 1:
        if (object_set_id == -1 || !objset_info_storage_load_obj_set_check_not_read(object_set_id))
            state = 2;
        break;
    case 2: {
        uint32_t obj_set_id = aft_obj_db->get_object_set_id(object_set_index);
        if (!objset_info_storage_load_obj_set_check_not_read(obj_set_id)) {
            obj_num = objset_info_storage_get_obj_set(obj_set_id)->obj_num;
            state = 3;
        }
    } break;
    }
    return false;
}

bool DataTestObjectManager::dest() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    objset_info_storage_unload_set(aft_obj_db->get_object_set_id(object_set_index));

    if (object_set_id != -1)
        objset_info_storage_unload_set(object_set_id);
    return true;
}

void DataTestObjectManager::disp() {
    if (state != 3 || obj_index < 0 || obj_index >= this->obj_num)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    uint32_t obj_set_id = aft_obj_db->get_object_set_id(object_set_index);
    uint32_t obj_id = objset_info_storage_get_obj_by_index(obj_set_id, obj_index)->id;
    rctx_ptr->disp_manager->set_obj_flags((mdl::ObjFlags)(mdl::OBJ_40 | mdl::OBJ_20 | obj_flags));

    vec3 rotation = this->rotation * DEG_TO_RAD_FLOAT;
    mat4 mat;
    mat4_rotate_xyz(&rotation, &mat);

    vec3 pos = 0.0f;
    shadow_ptr_get()->positions[SHADOW_CHARA].push_back(pos);

    rctx_ptr->disp_manager->set_shadow_type(SHADOW_CHARA);
    rctx_ptr->disp_manager->entry_obj_by_object_info(mat, { obj_id, obj_set_id });
    rctx_ptr->disp_manager->set_obj_flags((mdl::ObjFlags)0);
}

void DataTestObjectManager::del_task() {
    if (!app::TaskWork::check_task_ready(this))
        return;

    state = 0;
    del();
}

int32_t DataTestObjectManager::get_obj_num() {
    return obj_num;
}

const obj_bounding_sphere* DataTestObjectManager::get_object_bounding_sphere(int32_t obj_index) {
    if (state != 3)
        return 0;

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    uint32_t obj_set_id = aft_obj_db->get_object_set_id(object_set_index);
    const obj* obj = objset_info_storage_get_obj_by_index(obj_set_id, obj_index);
    if (obj)
        return &obj->bounding_sphere;
    return 0;
}

const char* DataTestObjectManager::get_object_name(int32_t obj_index) {
    if (state != 3)
        return "NULL";

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    uint32_t obj_set_id = aft_obj_db->get_object_set_id(object_set_index);
    const obj* obj = objset_info_storage_get_obj_by_index(obj_set_id, obj_index);
    if (obj)
        return obj->name;
    return"NULL";
}

void DataTestObjectManager::set_object_index(int32_t value) {
    obj_index = value;
}

void DataTestObjectManager::set_object_set_index(int32_t object_set_index) {
    if (app::TaskWork::check_task_ready(this))
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    this->object_set_index = object_set_index;
    object_set_id = -1;

    uint32_t obj_set_id = aft_obj_db->get_object_set_id(object_set_index);
    std::string obj_set_name(aft_obj_db->get_object_set_name(obj_set_id));
    if (!obj_set_name.find("STGPV"))
        obj_set_name.assign(obj_set_name.substr(0, 8));
    else if (!obj_set_name.find("STGD2PV"))
        obj_set_name.assign(obj_set_name.substr(0, 10));

    object_set_id = aft_obj_db->get_object_set_id(obj_set_name.c_str());
    app::TaskWork::add_task(this, "DATA_TEST_OBJECT_MANAGER");
}

void DataTestObjectManager::set_rotation(const vec3& value) {
    rotation = value;
}

void DataTestObjectManager::set_shadow(bool value) {
    obj_flags = value ? (mdl::ObjFlags)(mdl::OBJ_4 | mdl::OBJ_SHADOW) : (mdl::ObjFlags)0;
}

DataTestObjDw::DataTestObjDw() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    SetText("OBJECT TEST");

    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();

    (new dw::Label(this))->SetText("OBJECT SET");

    dw::ListBox* object_sets = new dw::ListBox(this);
    object_sets->SetMaxItems(20);

    uint32_t obj_set_count = aft_obj_db->get_object_set_count();
    for (uint32_t i = obj_set_count, j = 0; i; i--, j++)
        object_sets->AddItem(aft_obj_db->get_object_set_name(aft_obj_db->get_object_set_id(j)));
    object_sets->SetItemIndex((uint32_t)obj_data->object_set_index);
    object_sets->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::ObjectSetCallback));

    (new dw::Label(this))->SetText("          OBJECT          ");

    objects = new dw::ListBox(this);
    objects->SetMaxItems(20);
    objects->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::ObjectCallback));

    (new dw::Label(this))->SetText("CAMERA");

    dw::ListBox* camera = new dw::ListBox(this, dw::MULTISELECT);
    camera->AddItem("NORMAL");
    camera->AddItem("CULLING");
    camera->SetItemIndex(obj_data->cull_camera ? 1 : 0);
    camera->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::CameraCallback));

    (new dw::Label(this))->SetText("SHADOW");

    dw::ListBox* shadow = new dw::ListBox(this, dw::MULTISELECT);
    shadow->AddItem("OFF");
    shadow->AddItem("ON");
    shadow->SetItemIndex(obj_data->shadow);
    shadow->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::ShadowCallback));

    (new dw::Label(this))->SetText("STAGE HDRI");

    dw::ListBox* stage_hdri = new dw::ListBox(this);

    for (const stage_data& i : aft_stage_data->stage_data)
        stage_hdri->AddItem(i.name);

    stage_hdri->SetItemIndex(obj_data->stage_index);
    stage_hdri->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::StageCallback));

    (new dw::Label(this))->SetText("STAGE DISPLAY");

    dw::ListBox* stage_display = new dw::ListBox(this, dw::MULTISELECT);
    stage_display->AddItem("OFF");
    stage_display->AddItem("ON");
    stage_display->AddItem("GLID");
    stage_display->SetItemIndex(obj_data->stage_display);
    stage_display->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestObjDw::StageDisplayCallback));

    UpdateLayout();
}

DataTestObjDw::~DataTestObjDw() {

}

void DataTestObjDw::Hide() {
    SetDisp();
}

void DataTestObjDw::ClearIDs() {
    objects->ClearItems();
}

void DataTestObjDw::AddID(const char* value) {
    objects->AddItem(value);
}

void DataTestObjDw::CameraCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->cull_camera = !!list_box->list->selected_item;
}

void DataTestObjDw::ObjectCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->object_index = (int32_t)list_box->list->selected_item;
}

void DataTestObjDw::ObjectSetCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->object_set_index = (int32_t)list_box->list->selected_item;
}

void DataTestObjDw::ShadowCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->shadow = (int32_t)list_box->list->selected_item;
}

void DataTestObjDw::StageCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->stage_index = (int32_t)list_box->list->selected_item;
}

void DataTestObjDw::StageDisplayCallback(dw::Widget* data) {
    TaskDataTestObj::Data* obj_data = task_data_test_obj->get_data();
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        obj_data->stage_display = (int32_t)list_box->list->selected_item;
}

static void data_test_obj_dw_init() {
    if (!data_test_obj_dw) {
        data_test_obj_dw = new DataTestObjDw;
        data_test_obj_dw->LimitPosDisp();
    }
    else
        data_test_obj_dw->Disp();
}
