/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.h"
#include "time.h"

extern float_t get_delta_frame();
extern uint32_t get_frame_counter();

static void Task_add_base_calc_time(Task* t, uint32_t calc_time);
static void Task_do_basic(Task* t);
static void Task_do_ctrl(Task* t);
static void Task_do_ctrl_frames(int32_t frames, bool a2);
static void Task_do_disp(Task* t);
static void Task_set_base_calc_time(Task* t, uint32_t base_calc_time);
static void Task_set_calc_time(Task* t);
static void Task_set_disp_time(Task* t, uint32_t disp_time);
static bool Task_sub_14019B810(Task* t, int32_t a2);
static void Task_sub_14019C480(Task* t, uint32_t a2);
static void Task_sub_14019C5A0(Task* t);

static bool TaskWork_has_task_dest(Task* t);

TaskWork task_work;

Task::Task() {
    priority = 1;
    parent_task = 0;
    field_18 = TASK_NONE;
    field_1C = 0;
    field_20 = 0;
    field_24 = TASK_NONE;
    field_28 = 0;
    field_2C = false;
    field_2D = false;
    SetName("(unknown)");
    field_4E = false;
    field_4F = false;
    base_calc_time = 0;
    calc_time = 0;
    calc_time_max = 0;
    disp_time = 0;
    disp_time_max = 0;
}

Task::~Task() {

}

bool Task::Init() {
    return true;
}

bool Task::Ctrl() {
    return false;
}

bool Task::Dest() {
    return true;
}

void Task::Disp() {

}

void Task::Basic() {

}

uint32_t Task::GetCalcTime() {
    return calc_time;
}

uint32_t Task::GetCalcTimeMax() {
    return calc_time_max;
}

uint32_t Task::GetDispTime() {
    return disp_time;
}

uint32_t Task::GetDispTimeMax() {
    return disp_time_max;
}

char* Task::GetName() {
    return name;
}

bool Task::SetDest() {
    if (!TaskWork::HasTask(this) || !Task_sub_14019B810(this, 2))
        return 0;

    Task_sub_14019C480(this, 2);
    return 1;
}

void Task::SetName(char* name) {
    if (!name) {
        this->name[0] = 0;
        return;
    }

    size_t len = utf8_length(name);
    len = min(len, sizeof(this->name) - 1);
    memmove(&this->name, name, len);
    this->name[len] = 0;
}

inline void Task::SetName(const char* name) {
    SetName((char*)name);
}

void Task::SetPriority(int32_t priority) {
    this->priority = priority;
}

TaskWork::TaskWork() : current(), disp() {

}

TaskWork::~TaskWork() {

}

bool TaskWork::AppendTask(Task* t, const char* name, int32_t priority) {
    return TaskWork::AppendTask(t, task_work.current, name, priority);
}

bool TaskWork::AppendTask(Task* t, Task* parent_task, const char* name, int32_t priority) {
    Task_sub_14019C480(t, 0);
    if (TaskWork::HasTask(t) || !Task_sub_14019B810(t, 1))
        return false;

    t->SetPriority(priority);
    t->parent_task = parent_task;
    t->field_18 = TASK_NONE;
    t->field_1C = 0;
    t->field_24 = TASK_NONE;
    t->field_28 = 0;
    t->SetName(name);
    Task_sub_14019C480(t, 1);
    Task_sub_14019C5A0(t);
    task_work.tasks.push_back(t);
    return true;
}

void TaskWork::Basic() {
    std::vector<Task*>* tasks = &task_work.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task*& j : task_work.tasks)
            if (j->priority == i)
                Task_do_basic(j);
}

bool TaskWork::CheckTaskNotReady(Task* t) {
    return TaskWork::HasTask(t) && (t->field_18 == TASK_NONE || t->field_1C);
}

void TaskWork::Ctrl() {
    std::vector<Task*>* tasks = &task_work.tasks;
    for (Task*& i : task_work.tasks) {
        Task_sub_14019C5A0(i);
        Task_set_base_calc_time(i, 0);
    }

    for (int32_t i = 2; i >= 0; i--)
        for (Task** j = task_work.tasks.end()._Ptr; j != task_work.tasks.begin()._Ptr; ) {
            --j;
            Task* tsk = *j;
            if (tsk->priority != i || !TaskWork_has_task_dest(tsk))
                continue;

            time_struct t;
            time_struct_init(&t);
            task_work.current = tsk;
            Task_do_ctrl(tsk);
            task_work.current = 0;
            Task_add_base_calc_time(tsk, (uint32_t)(time_struct_calc_time(&t) * 1000.0));
        }

    for (Task** i = task_work.tasks.begin()._Ptr; i != task_work.tasks.end()._Ptr; ) {
        if (TaskWork::CheckTaskNotReady(*i)) {
            i++;
            continue;
        }

        task_work.tasks.erase(task_work.tasks.begin()
            + (i - task_work.tasks.data()));
    }

    int32_t delta_frame = (int32_t)get_delta_frame();
    if (delta_frame > 1)
        for (int32_t i = delta_frame - 1; i; i--)
            Task_do_ctrl_frames(delta_frame, true);
    Task_do_ctrl_frames(delta_frame, false);

    for (Task*& i : task_work.tasks)
        Task_set_calc_time(i);
}

void TaskWork::Disp() {
    task_work.disp = true;
    std::vector<Task*>* tasks = &task_work.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task*& j : task_work.tasks) {
            Task* tsk = j;
            if (tsk->priority != i)
                continue;

            time_struct t;
            time_struct_init(&t);
            Task_do_disp(tsk);
            Task_set_disp_time(tsk, (uint32_t)(time_struct_calc_time(&t) * 1000.0));
        }
    task_work.disp = false;
}

Task* TaskWork::GetTaskByIndex(int32_t index) {
    int32_t k = 0;
    for (int32_t i = 0; i < 3; i++)
        for (Task*& j : task_work.tasks)
            if (j->priority == i)
                if (k++ == index)
                    return j;
    return 0;
}

bool TaskWork::HasTask(Task* t) {
    std::vector<Task*>* tasks = &task_work.tasks;
    for (Task*& i : task_work.tasks)
        if (i == t)
            return true;
    return false;
}

bool TaskWork::HasTaskDest(Task* t) {
    if (TaskWork::HasTask(t))
        return t->field_18 == TASK_DEST;
    else
        return false;
}

static void Task_add_base_calc_time(Task* t, uint32_t calc_time) {
    t->base_calc_time += calc_time;
}

static void Task_do_basic(Task* t) {
    if ((t->field_1C == 1 || t->field_1C == 2)
        && t->field_18 != TASK_INIT && t->field_18 != TASK_DEST)
        t->Basic();
}

static void Task_do_ctrl(Task* t) {
    if (t->field_1C != 1)
        return;

    if (t->field_18 == TASK_INIT && t->Init()) {
        t->field_24 = TASK_CTRL;
        t->field_18 = TASK_CTRL;
    }

    if (t->field_18 == TASK_CTRL && t->Ctrl()) {
        t->field_24 = TASK_DEST;
        t->field_18 = TASK_DEST;
    }

    if (t->field_18 == TASK_DEST && t->Dest()) {
        t->field_28 = 0;
        t->field_24 = TASK_MAX;
        t->field_20 = 0;
    }
}

static void Task_do_ctrl_frames(int32_t frames, bool a2) {
    std::vector<Task*>* tasks = &task_work.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task*& j : task_work.tasks) {
            Task* tsk = j;
            if (tsk->priority != i || TaskWork_has_task_dest(tsk))
                continue;
            else if ((!tsk->field_2D || frames <= 0) && a2)
                continue;

            time_struct t;
            time_struct_init(&t);
            task_work.current = tsk;
            Task_do_ctrl(tsk);
            task_work.current = 0;
            Task_add_base_calc_time(tsk, (uint32_t)(time_struct_calc_time(&t) * 1000.0));
        }
}

static void Task_do_disp(Task* t) {
    if ((t->field_1C == 1 || t->field_1C == 2)
        && t->field_18 != TASK_INIT && t->field_18 != TASK_DEST)
        t->Disp();
}

static void Task_set_base_calc_time(Task* t, uint32_t base_calc_time) {
    t->base_calc_time = base_calc_time;
}

static void Task_set_calc_time(Task* t) {
    t->calc_time = t->base_calc_time;
    uint32_t frame_counter = get_frame_counter();
    if (frame_counter == (frame_counter / 300) * 300)
        t->calc_time_max = 0;
    t->calc_time = max(t->calc_time, t->calc_time_max);
}

static void Task_set_disp_time(Task* t, uint32_t disp_time) {
    t->disp_time = disp_time;
    uint32_t frame_counter = get_frame_counter();
    if (frame_counter == (frame_counter / 300) * 300)
        t->disp_time_max = 0;
    t->disp_time = max(t->disp_time, t->disp_time_max);
}

static bool Task_sub_14019B810(Task* t, int32_t a2) {
    if (task_work.disp)
        return false;

    if (a2 == 1)
        return true;
    else if (a2 == 2)
        return t->field_1C != 0;
    else if (a2 == 3)
        return t->field_1C == 1 || t->field_1C == 3;
    else if (a2 == 4)
        return t->field_1C == 1 || t->field_1C == 2;
    else if (a2 == 5)
        return t->field_1C == 2 || t->field_1C == 3;
    return false;
}

static void Task_sub_14019C480(Task* t, uint32_t a2) {
    if (a2 > 1)
        for (Task*& i : task_work.tasks)
            if (i->parent_task == t)
                Task_sub_14019C480(i, a2);
    t->field_20 = a2;
}

static void Task_sub_14019C5A0(Task* t) {
    if (t->field_18 != TASK_INIT && t->field_18 != TASK_DEST
        && Task_sub_14019B810(t, t->field_20)) {
        switch (t->field_20) {
        case 1:
            t->field_24 = TASK_INIT;
            t->field_28 = 1;
            break;
        case 2:
            t->field_24 = TASK_DEST;
            t->field_28 = 1;
            break;
        case 3:
            t->field_28 = 2;
            break;
        case 4:
            t->field_28 = 3;
            break;
        case 5:
            t->field_28 = 1;
            break;
        }
        t->field_20 = 0;
    }

    t->field_18 = t->field_24;
    t->field_1C = t->field_28;
}

static bool TaskWork_has_task_dest(Task* t) {
    if (TaskWork::HasTask(t))
        return t->field_18 == TASK_DEST;
    return false;
}
