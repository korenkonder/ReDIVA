/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "opd_test.hpp"
#include "../../CRE/app_system_detail.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/module_table.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../dw.hpp"
#include "../print_work.hpp"

class DataTestOpdDw : public dw::Shell {
public:
    bool start;
    dw::ListBox* list;

    DataTestOpdDw();
    virtual ~DataTestOpdDw() override;

    void InitCustomizeItem();
    void InitModule();

    static void CopyToLocalCallback(dw::Widget* data);
    static void SelectorCallback(dw::Widget* data);
    static void SpecifiedMotionOnlyCallback(dw::Widget* data);
    static void StartDataCreationCallback(dw::Widget* data);
    static void UseOpdiCallback(dw::Widget* data);
};

class TaskDataTestOpdDw : public app::Task {
public:
    struct Data {
        int32_t selector;
        std::vector<uint32_t> modules;
        size_t selected_module;
        bool all_motions;
        bool copy_to_local;
        bool use_opdi;

        Data();
        ~Data();

        void Reset();
    };

    bool data_creation;
    Data data;

    TaskDataTestOpdDw();
    virtual ~TaskDataTestOpdDw() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    Data& GetData();
    bool GetDataCreation();
    void SetDataCreation();

    bool add_task();
};

DataTestOpdDw* data_test_opd_dw;
TaskDataTestOpd* task_data_test_opd;
TaskDataTestOpdDw* task_data_test_opd_dw;

bool byte_140C97958;

static bool data_test_opd_dw_get_start();
static void data_test_opd_dw_hide();
static void data_test_opd_dw_init();

static void task_data_test_opd_dw_add_module();
static void task_data_test_opd_dw_add_modules();

TaskDataTestOpd::TaskDataTestOpd() : state() {

}

TaskDataTestOpd::~TaskDataTestOpd() {

}

bool TaskDataTestOpd::init() {
    task_data_test_opd_dw->add_task();
    state = 1;
    return true;
}

bool TaskDataTestOpd::ctrl() {
    switch (state) {
    case 1:
        if (!task_data_test_opd_dw->GetDataCreation())
            break;

        BeginDataCreation();
        task_data_test_opd_dw->del();
        state = 2;
        break;
    case 2:
        if (!opd_make_manager_check_task_ready())
            state = task_data_test_opd_dw->GetData().copy_to_local ? 3 : 5;
        break;
    case 3: {
        paths.clear();

        std::vector<std::string> paths = path_get_directories_recursive(get_ram_osage_play_data_dir());
        this->paths.swap(paths);
        state = 4;
    } break;
    case 4: {
        if (!paths.size()) {
            state = 5;
            break;
        }

        std::string local_dir = sprintf_s_string("_LOCAL/%s", get_rom_osage_play_data_dir());
        path_create_directory(local_dir.c_str());

        std::string path = paths.back();
        paths.pop_back();

        size_t pos = path.find('/');
        if (pos != -1) {
            std::string local_path = sprintf_s_string("%s/%s",
                local_dir.c_str(), path.substr(pos + 1).c_str());
            if (path_compare_files(path.c_str(), local_path.c_str()))
                path_fs_copy_file(path.c_str(), local_path.c_str());
        }
    } break;
    }
    return false;
}

bool TaskDataTestOpd::dest() {
    task_data_test_opd_dw->del();
    return true;
}

void TaskDataTestOpd::disp() {
    static const char* cursor_array[] = {
        ".  ",
        " . ",
        "  .",
    };

    PrintWork print_work;
    font_info font(16);

    print_work.set_font(&font);
    print_work.set_position(10.0f);
    print_work.set_resolution_mode(RESOLUTION_MODE_MAX);
    print_work.printf_align_left("OSAGE PLAY DATA MAKE MODE \n");
    print_work.printf_align_left("%s\n", cursor_array[get_main_timer() % 3]);
    print_work.printf_align_left("STATUS: ");

    switch (state) {
    case 1:
        print_work.printf_align_left("WAIT REQUEST\n");
        break;
    case 2: {
        print_work.printf_align_left("WORKING\n");

        font.set_glyph_size(12.0f, 12.0f);

        print_work.set_font(&font);
        OpdMakeManagerData* opd_make_manager_data = opd_make_manager_get_data();
        print_work.printf_align_left("mode: %d\n", opd_make_manager_data->mode);
        if (opd_make_manager_data->count)
            print_work.printf_align_left("progress: %d/%d \n\n",
                opd_make_manager_data->count - opd_make_manager_data->left, opd_make_manager_data->count);

        size_t worker = 0;
        for (OpdMakeManagerData::Worker& i : opd_make_manager_data->workers) {
            if (i.chara >= CHARA_MAX) {
                worker++;
                continue;
            }

            print_work.printf_align_left("------------------\n");
            print_work.printf_align_left("worker:%2zu, ", worker);
            print_work.printf_align_left("chara:%2d, ", i.chara);
            print_work.printf_align_left("mode:%2d, ", i.mode);
            print_work.printf_align_left("progress:%3d%%\n", i.progress);
            print_work.printf_align_left("item:\n");

            for (uint32_t& j : i.items) {
                const item_table_item* item = item_table_handler_array_get_item(i.chara, j);
                if (item)
                    print_work.printf_align_left("%03u %s\n", j, item->name.c_str());
            }
            worker++;
        }
    } break;
    case 3:
    case 4:
        print_work.printf_align_left("COPY TO _LOCAL\n");
        break;
    case 5:
        print_work.printf_align_left("FINISHED\n");
        break;
    }
}

void TaskDataTestOpd::BeginDataCreation() {
    std::vector<int32_t> motion_ids;
    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();
    if (!data.all_motions) {
        const uint32_t* opd_motion_ids = get_opd_motion_ids();
        while (*opd_motion_ids != -1)
            motion_ids.push_back(*opd_motion_ids++);
    }

    opd_make_start_get_motion_ids(motion_ids);
    motion_ids.push_back(195); // CMN_MRA00_13_01
    prj::sort_unique(motion_ids);

    OpdMakeManagerArgs args;
    args.motion_ids = &motion_ids;
    args.modules = 0;
    args.objects = 0;
    args.use_current_skp = false;
    args.use_opdi = data.use_opdi;

    switch (data.selector) {
    case 0: {
        std::vector<uint32_t> modules;
        for (size_t i = module_table_handler_data_get_modules().size(), j = 0; i; i--, j++)
            modules.push_back((uint32_t)j);
        args.modules = &modules;

        std::vector<std::string> objects;
        for (auto& i : customize_item_table_handler_data_get_customize_items())
            GetCustomizeItemObjectNames(i.second, objects);
        args.objects = &objects;
        opd_make_manager_add_task(args);
    } break;
    case 1: {
        args.modules = &data.modules;
        opd_make_manager_add_task(args);
    } break;
    case 2: {
        const prj::vector_pair_combine<int32_t, customize_item>& customize_items
            = customize_item_table_handler_data_get_customize_items();

        std::vector<std::string> objects;
        for (const uint32_t& i : data.modules)
            GetCustomizeItemObjectNames(customize_items.find(i)->second, objects);
        args.objects = &objects;
        opd_make_manager_add_task(args);
    } break;
    }
}

void TaskDataTestOpd::GetCustomizeItemObjectNames(
    const customize_item& cstm, std::vector<std::string>& objects) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    const item_table_item* item = item_table_handler_array_get_item(
        cstm.chara != 10 ? (chara_index)cstm.chara : CHARA_MIKU, cstm.obj_id);
    if (item)
        for (const item_table_item_data_obj& i : item->data.obj)
            objects.push_back(aft_obj_db->get_object_name(i.obj_info));
}

bool TaskDataTestOpd::add_task() {
    return app::TaskWork::add_task(this, "DATA_TEST_OPD");
}

void opd_test_init() {
    if (!task_data_test_opd)
        task_data_test_opd = new TaskDataTestOpd;

    if (!task_data_test_opd_dw)
        task_data_test_opd_dw = new TaskDataTestOpdDw;
}

void opd_test_free() {
    if (task_data_test_opd_dw) {
        delete task_data_test_opd_dw;
        task_data_test_opd_dw = 0;
    }

    if (task_data_test_opd_dw) {
        delete task_data_test_opd_dw;
        task_data_test_opd_dw = 0;
    }
}

DataTestOpdDw::DataTestOpdDw() {
    start = false;
    list = 0;

    const char* osage_play_data_creation;
    const char* selector_items[3];
    const char* specified_motion_only_text;
    const char* copy_to_local_text;
    const char* use_opdi_text;
    const char* start_data_creation_text;
    if (dw::translate) {
        osage_play_data_creation = u8"Osage Play Data Creation";

        selector_items[0] = u8"All";
        selector_items[1] = u8"Module specific";
        selector_items[2] = u8"Customize Item specific";

        specified_motion_only_text = u8"Specified Motion Only";
        copy_to_local_text = u8"Copy to LOCAL";
        use_opdi_text = u8"Use opdi";
        start_data_creation_text = u8"Start Data Creation";
    }
    else {
        osage_play_data_creation = u8"おさげ再生データ作成";

        selector_items[0] = u8"全部";
        selector_items[1] = u8"モジュール指定";
        selector_items[2] = u8"カスタマイズアイテム指定";

        specified_motion_only_text = u8"指定モーションのみ";
        copy_to_local_text = u8"作業後_LOCALにコピー";
        use_opdi_text = u8"opdiを使う";
        start_data_creation_text = u8"データ作成開始";
    }

    SetText(osage_play_data_creation);

    rect.pos = { 400.0f, 60.0f };

    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();

    dw::Composite* comp = new dw::Composite(this);
    comp->SetText("Mode");
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* mode = new dw::Label(comp);
    mode->SetText("Mode:");
    mode->SetFont(dw::p_font_type_6x12);

    dw::ListBox* selector = new dw::ListBox(comp);
    selector->SetText("Selector");
    selector->SetFont(dw::p_font_type_6x12);
    for (const char*& i : selector_items)
        selector->AddItem(i);
    selector->SetItemIndex(data.selector);

    selector->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestOpdDw::SelectorCallback));
    switch (data.selector) {
    case 0:
        break;
    case 1:
        InitModule();
        break;
    case 2:
        InitCustomizeItem();
        break;
    }

    dw::Button* specified_motion_only = new dw::Button(this, dw::CHECKBOX);
    specified_motion_only->SetText(specified_motion_only_text);
    specified_motion_only->callback = DataTestOpdDw::SpecifiedMotionOnlyCallback;
    specified_motion_only->SetValue(!data.all_motions);

    dw::Button* copy_to_local = new dw::Button(this, dw::CHECKBOX);
    copy_to_local->SetText(copy_to_local_text);
    copy_to_local->callback = DataTestOpdDw::CopyToLocalCallback;
    copy_to_local->SetValue(data.copy_to_local);

    dw::Button* use_opdi = new dw::Button(this, dw::CHECKBOX);
    use_opdi->SetText(use_opdi_text);
    use_opdi->callback = DataTestOpdDw::UseOpdiCallback;
    use_opdi->SetValue(data.use_opdi);

    dw::Button* start_data_creation = new dw::Button(this, dw::FLAG_8);
    start_data_creation->SetText(start_data_creation_text);
    start_data_creation->callback = DataTestOpdDw::StartDataCreationCallback;

    UpdateLayout();
    SetDisp(true);
}

DataTestOpdDw::~DataTestOpdDw() {

}

void DataTestOpdDw::InitCustomizeItem() {
    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();

    const char* group_text;
    const char* add_all_items_text;
    const char* add_text;
    const char* working_target_text;
    if (dw::translate) {
        group_text = u8"Customize Item";
        add_all_items_text = u8"Add All Items";
        add_text = u8"Add";
        working_target_text = u8"Working Target";
    }
    else {
        group_text = u8"カスタマイズアイテム";
        add_all_items_text = u8"全アイテム追加";
        add_text = u8"追加";
        working_target_text = u8"作業対象";
    }

    dw::Group* group = new dw::Group(this);
    group->SetText(group_text);
    group->SetFont(dw::p_font_type_6x12);

    dw::Button* add_all_items = new dw::Button(group, dw::FLAG_8);
    add_all_items->SetText(add_all_items_text);
    add_all_items->callback = (dw::Widget::Callback)task_data_test_opd_dw_add_modules;
    add_all_items->SetFont(dw::p_font_type_6x12);

    dw::Composite* comp = new dw::Composite(group);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    list = new dw::ListBox(comp);
    for (auto& i : customize_item_table_handler_data_get_customize_items())
        list->AddItem(i.second.name);
    list->SetItemIndex(data.selected_module);
    list->SetMaxItems(40);
    list->SetFont(dw::p_font_type_6x12);

    dw::Button* add = new dw::Button(comp, dw::FLAG_8);
    add->SetText(add_text);
    add->callback = (dw::Widget::Callback)task_data_test_opd_dw_add_module;
    add->SetFont(dw::p_font_type_6x12);

    dw::Group* working_target_group = new dw::Group(group);
    working_target_group->SetText(working_target_text);
    working_target_group->SetFont(dw::p_font_type_6x12);

    dw::List* working_target = new dw::List(working_target_group);
    working_target->SetMaxItems(20);
    working_target->SetFont(dw::p_font_type_6x12);
    const prj::vector_pair_combine<int32_t, customize_item>& customize_items
        = customize_item_table_handler_data_get_customize_items();
    for (const uint32_t& i : data.modules)
        working_target->AddItem(customize_items.find(i)->second.name);
    working_target->UpdateLayout();
}

void DataTestOpdDw::InitModule() {
    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();

    const char* group_text;
    const char* add_all_modules_text;
    const char* add_text;
    const char* working_target_modules_text;
    if (dw::translate) {
        group_text = u8"Module";
        add_all_modules_text = u8"Add All Modules";
        add_text = u8"Add";
        working_target_modules_text = u8"Working Target Modules";
    }
    else {
        group_text = u8"モジュール";
        add_all_modules_text = u8"全アイテム追加";
        add_text = u8"追加";
        working_target_modules_text = u8"作業対象モジュール";
    }

    dw::Group* group = new dw::Group(this);
    group->SetText(group_text);
    group->SetFont(dw::p_font_type_6x12);

    dw::Button* add_all_modules = new dw::Button(group, dw::FLAG_8);
    add_all_modules->SetText(add_all_modules_text);
    add_all_modules->callback = (dw::Widget::Callback)task_data_test_opd_dw_add_modules;
    add_all_modules->SetFont(dw::p_font_type_6x12);

    dw::Composite* comp = new dw::Composite(group);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    list = new dw::ListBox(comp);
    for (auto& i : module_table_handler_data_get_modules())
        list->AddItem(i.second.name);
    list->SetItemIndex(data.selected_module);
    list->SetMaxItems(40);
    list->SetFont(dw::p_font_type_6x12);

    dw::Button* add = new dw::Button(comp, dw::FLAG_8);
    add->SetText(add_text);
    add->SetFont(dw::p_font_type_6x12);
    add->callback = (dw::Widget::Callback)task_data_test_opd_dw_add_module;

    dw::Group* working_target_modules_group = new dw::Group(group);
    working_target_modules_group->SetText(working_target_modules_text);
    working_target_modules_group->SetFont(dw::p_font_type_6x12);

    dw::List* working_target_modules = new dw::List(working_target_modules_group);
    working_target_modules->SetMaxItems(20);
    working_target_modules->SetFont(dw::p_font_type_6x12);

    const prj::vector_pair_combine<int32_t, module>& modules = module_table_handler_data_get_modules();
    for (const int32_t& i : data.modules)
        working_target_modules->AddItem(modules.data.data()[i].second.name);

    working_target_modules->UpdateLayout();
}

void DataTestOpdDw::CopyToLocalCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        task_data_test_opd_dw->GetData().copy_to_local = button->value;
}

void DataTestOpdDw::SelectorCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();
        int32_t selector = (int32_t)list_box->list->selected_item;
        if (data.selector != selector) {
            data.selector = selector;

            data.selected_module = 0;
            data.modules.clear();

            data_test_opd_dw->start = true;
        }
    }
}

void DataTestOpdDw::SpecifiedMotionOnlyCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        task_data_test_opd_dw->GetData().all_motions = !button->value;
}

void DataTestOpdDw::StartDataCreationCallback(dw::Widget* data) {
    task_data_test_opd_dw->SetDataCreation();
}

void DataTestOpdDw::UseOpdiCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        task_data_test_opd_dw->GetData().use_opdi = button->value;
}

TaskDataTestOpdDw::Data::Data() : selector(), selected_module(), all_motions(), copy_to_local(), use_opdi() {
    Reset();
}

TaskDataTestOpdDw::Data::~Data() {

}

inline void TaskDataTestOpdDw::Data::Reset() {
    selector = 0;
    modules.clear();
    selected_module = 0;
    all_motions = true;
    copy_to_local = false;
    use_opdi = false;
}

TaskDataTestOpdDw::TaskDataTestOpdDw() : data_creation() {
    init();
}

TaskDataTestOpdDw::~TaskDataTestOpdDw() {

}

bool TaskDataTestOpdDw::init() {
    data_creation = false;
    data_test_opd_dw_hide();
    return true;
}

bool TaskDataTestOpdDw::ctrl() {
    if (data_test_opd_dw_get_start())
        data_test_opd_dw_init();
    return false;
}

bool TaskDataTestOpdDw::dest() {
    data_test_opd_dw_hide();
    return true;
}

void TaskDataTestOpdDw::disp() {

}

bool TaskDataTestOpdDw::add_task() {
    data_creation = false;
    data.Reset();
    return app::TaskWork::add_task(this, "DATA_TEST_OPD_DW");
}

TaskDataTestOpdDw::Data& TaskDataTestOpdDw::GetData() {
    return data;
}

bool TaskDataTestOpdDw::GetDataCreation() {
    return data_creation;
}

void TaskDataTestOpdDw::SetDataCreation() {
    data_creation = true;
}

static bool data_test_opd_dw_get_start() {
    if (data_test_opd_dw)
        return data_test_opd_dw->start;
    return true;
}

static void data_test_opd_dw_hide() {
    if (byte_140C97958) {
        byte_140C97958 = 0;
        data_test_opd_dw = 0;
    }
    else if (data_test_opd_dw) {
        data_test_opd_dw->Hide();
        data_test_opd_dw = 0;
    }
}

static void data_test_opd_dw_init() {
    data_test_opd_dw_hide();
    data_test_opd_dw = new DataTestOpdDw;
}

static void task_data_test_opd_dw_add_module() {
    data_test_opd_dw->start = true;

    size_t index = data_test_opd_dw->list->list->selected_item;
    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();

    switch (data.selector) {
    case 0:
        break;
    case 1:
        data.modules.push_back((int32_t)index);
        data.selected_module = index;
        break;
    case 2: {
        std::string name = data_test_opd_dw->list->GetItemStr(index);
        for (auto& i : customize_item_table_handler_data_get_customize_items())
            if (!i.second.name.compare(name))
                data.modules.push_back(i.first);
    } break;
    }
    prj::sort_unique(data.modules);
}

static void task_data_test_opd_dw_add_modules() {
    data_test_opd_dw->start = true;

    TaskDataTestOpdDw::Data& data = task_data_test_opd_dw->GetData();

    switch (data.selector) {
    case 0:
        break;
    case 1:
        for (size_t i = module_table_handler_data_get_modules().size(), j = 0; i; i--, j++)
            data.modules.push_back((uint32_t)i);
        break;
    case 2:
        for (auto& i : customize_item_table_handler_data_get_customize_items())
            data.modules.push_back(i.first);
        break;
    }
}
