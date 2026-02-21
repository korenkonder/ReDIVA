/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/task.hpp"
#include "../dw.hpp"
#include <algorithm>

class DwTask : public dw::Shell {
public:
    dw::List* list;
    dw::Button* buttons[5];

    DwTask();
    virtual ~DwTask() override;

    virtual void Draw() override;

    virtual void Hide() override;

    void AddTaskTree(app::Task* t, int32_t depth);
    void UpdateTasks();
    void UpdateTasksExec();
    void UpdateTasksTree();

    static void ButtonCallback(dw::Widget* data);
};

DwTask* dw_task;
int32_t dw_task_select = 0;

static int32_t task_sort_by_calc(void const* src1, void const* src2);
static int32_t task_sort_by_disp(void const* src1, void const* src2);
static int32_t task_sort_by_name(void const* src1, void const* src2);

void dw_task_init() {
    if (!dw_task) {
        dw_task = new DwTask();
        dw_task->LimitPosDisp();
    }
    else
        dw_task->Disp();
}

DwTask::DwTask() {
    SetText("Task");

    rect.pos = 0.0f;
    SetSize({ 320.0f, 240.0f });

    list = new dw::List(this, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));

    UpdateTasks();

    dw::Composite* comp = new dw::Composite(this);
    comp->layout = new dw::RowLayout(dw::HORIZONTAL);

    static const char* dw_task_sort_labels[] = {
        "exec",
        "tree",
        "name",
        "calc",
        "disp",
    };

    for (int32_t i = 0; i < 5; i++) {
        buttons[i] = new dw::Button(comp, dw::RADIOBUTTON);
        buttons[i]->SetText(dw_task_sort_labels[i]);

        buttons[i]->callback_data.i64 = i;
        buttons[i]->callback = DwTask::ButtonCallback;
    }

    buttons[dw_task_select]->SetValue(true);

    UpdateLayout();
}

DwTask::~DwTask() {

}

void DwTask::Draw() {
    UpdateTasks();
    dw::Shell::Draw();
}

void DwTask::Hide() {
    SetDisp();
}

void DwTask::AddTaskTree(app::Task* parent_task, int32_t depth) {
    for (int32_t i = 0; ; i++) {
        app::Task* t = app::TaskWork::get_task_by_index(i);
        if (!t)
            break;
        else if (t->get_parent_task() != parent_task)
            continue;

        std::string str(depth, ' ');
        str.append(t->get_name());

        list->AddItem(sprintf_s_string("%-32s % 7d(% 7d) % 7d(% 7d)", t->get_name(),
            t->get_calc_time(), t->get_calc_time_max(),
            t->get_disp_time(), t->get_disp_time_max()));
        AddTaskTree(t, depth + 1);
    }
}

void DwTask::UpdateTasks() {
    size_t hovered_item = list->hovered_item;
    size_t selected_item = list->selected_item;
    switch (dw_task_select) {
    case 0:
        UpdateTasksExec();
        break;
    case 1:
        UpdateTasksTree();
        break;
    case 2:
    case 3:
    case 4:
        UpdateTasksExec();
        break;
    }

    list->hovered_item = hovered_item;
    list->ResetSetSelectedItem(selected_item);

    int32_t max_items = (int32_t)(rect.size.y / font.GetFontGlyphHeight()) - 3;
    list->SetMaxItems(max_def(max_items, 0));
}

void DwTask::UpdateTasksExec() {
    std::vector<app::Task*> vec;
    for (int32_t i = 0; ; i++) {
        app::Task* t = app::TaskWork::get_task_by_index(i);
        if (!t)
            break;

        vec.push_back(t);
    }

    switch (dw_task_select) {
    case 2:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_name);
        break;
    case 3:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_calc);
        break;
    case 4:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_disp);
        break;
    }

    list->ClearItems();

    for (app::Task*& t : vec)
        list->AddItem(sprintf_s_string("%-32s % 7d(% 7d) % 7d(% 7d)", t->get_name(),
            t->get_calc_time(), t->get_calc_time_max(),
            t->get_disp_time(), t->get_disp_time_max()));

    while (list->items.size() < 50)
        list->AddItem("");
}

void DwTask::UpdateTasksTree() {
    list->ClearItems();

    AddTaskTree(0, 0);

    while (list->items.size() < 50)
        list->AddItem("");
}

void DwTask::ButtonCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dw_task_select = button->callback_data.i32;
}

static int32_t task_sort_by_calc(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return t2->get_calc_time() - t1->get_calc_time();
}

static int32_t task_sort_by_disp(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return t2->get_disp_time() - t1->get_disp_time();
}

static int32_t task_sort_by_name(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return str_utils_compare(t1->get_name(), t2->get_name());
}
