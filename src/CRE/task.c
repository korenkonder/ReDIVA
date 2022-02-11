/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.h"
#include "../KKdLib/vector.h"
#include "time.h"

vector_ptr(Task)

typedef struct TaskWork {
    vector_ptr_Task tasks;
    Task* current;
    bool disp;
} TaskWork;

vector_ptr_func(Task)

extern float_t get_delta_frame();
extern uint32_t get_frame_counter();

static void Task_add_base_calc_time(Task* t, uint32_t calc_time);
static void Task_add_disp_time(Task* t, uint32_t disp_time);
static void Task_do_basic(Task* t);
static void Task_do_ctrl(Task* t);
static void Task_do_ctrl_frames(int32_t frames, bool a2);
static void Task_do_disp(Task* t);
static void Task_set_base_calc_time(Task* t, uint32_t base_calc_time);
static void Task_set_calc_time(Task* t);
static void Task_set_disp_time(Task* t, uint32_t disp_time);
static bool Task_sub_14019B810(Task* t, int32_t a2);
static void Task_sub_14019C5A0(Task* t);

static bool TaskWork_has_task_dest(Task* t);
static bool TaskWork_sub_14019B6B0(Task* t);

TaskWork task_work_data;

Task::Task() {
    priority = 0;
    parent = 0;
    field_18 = task_enum::NONE;
    field_1C = 0;
    field_20 = 0;
    field_24 = task_enum::NONE;
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

void Task::Basic() {

}

bool Task::Ctrl() {
    return false;
}

bool Task::Dest() {
    return true;
}

void Task::Disp() {

}

void Task::Dispose(bool free_data) {
    Free();
    if (free_data) {
        void* data = (void*)this;
        free(data);
    }
}

bool Task::Init() {
    return true;
}

void Task::Free() {

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

Task::~Task() {
    Free();
}

void TaskWork_init() {
    task_work_data.tasks = vector_ptr_empty(Task);
    task_work_data.current = 0;
    task_work_data.disp = false;
}

void TaskWork_basic() {
    vector_ptr_Task* tasks = &task_work_data.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task** j = tasks->begin; j != tasks->end; j++)
            if ((*j)->priority == i)
                Task_do_basic(*j);
}

void TaskWork_ctrl() {
    vector_ptr_Task* tasks = &task_work_data.tasks;
    for (Task** i = tasks->begin; i != tasks->end; i++) {
        Task_sub_14019C5A0(*i);
        Task_set_base_calc_time(*i, 0);
    }

    if (vector_length(task_work_data.tasks))
        for (int32_t i = 2; i >= 0; i--)
            for (Task** j = &tasks->end[-1]; j != tasks->begin; j--) {
                Task* tsk = *j;
                if (!tsk->priority != i || !TaskWork_has_task_dest(tsk))
                    continue;

                time_struct t;
                time_struct_init(&t);
                task_work_data.current = tsk;
                Task_do_ctrl(tsk);
                task_work_data.current = 0;
                Task_add_base_calc_time(tsk, (uint32_t)time_struct_calc_time(&t));
            }

    for (Task** i = tasks->begin; i != tasks->end; ) {
        if (!TaskWork_sub_14019B6B0(*i)) {
            i++;
            continue;
        }

        *i = 0;
        vector_ptr_Task_erase(tasks, i - tasks->begin, 0);
    }

    int32_t delta_frame = (int32_t)get_delta_frame();
    if (delta_frame > 1)
        for (int32_t i = delta_frame - 1; i; i--)
            Task_do_ctrl_frames(delta_frame, true);
    Task_do_ctrl_frames(delta_frame, false);

    for (Task** i = tasks->begin; i != tasks->end; i++)
        Task_set_calc_time(*i);
}

void TaskWork_disp() {
    task_work_data.disp = true;
    vector_ptr_Task* tasks = &task_work_data.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task** j = tasks->begin; j != tasks->end; j++) {
            Task* tsk = *j;
            if (tsk->priority != i)
                continue;

            time_struct t;
            time_struct_init(&t);
            Task_do_disp(tsk);
            Task_add_disp_time(tsk, (uint32_t)time_struct_calc_time(&t));
        }
    task_work_data.disp = false;
}

bool task_work_has_Task(Task* t) {
    vector_ptr_Task* tasks = &task_work_data.tasks;
    for (Task** i = tasks->begin; i != tasks->end; i++)
        if ((*i) == t)
            return true;
    return false;
}

void TaskWork_free() {
    task_work_data.tasks.end = task_work_data.tasks.begin;
    vector_ptr_Task_free(&task_work_data.tasks, 0);
    task_work_data.current = 0;
    task_work_data.disp = false;
}

static void Task_add_base_calc_time(Task* t, uint32_t calc_time) {
    t->base_calc_time += calc_time;
}

static void Task_add_disp_time(Task* t, uint32_t disp_time) {
    t->disp_time += disp_time;
}

static void Task_do_basic(Task* t) {
    if ((t->field_1C == 1 || t->field_1C == 2)
        && t->field_18 != task_enum::INIT && t->field_18 != task_enum::DEST)
        t->Basic();
}

static void Task_do_ctrl(Task* t) {
    if (t->field_1C != 1)
        return;

    if (t->field_18 == task_enum::INIT && t->Init()) {
        t->field_24 = task_enum::CTRL;
        t->field_18 = task_enum::CTRL;
    }

    if (t->field_18 == task_enum::CTRL && t->Ctrl()) {
        t->field_24 = task_enum::DEST;
        t->field_18 = task_enum::DEST;
    }

    if (t->field_18 == task_enum::DEST && t->Dest()) {
        t->field_28 = 0;
        t->field_24 = task_enum::MAX;
        t->field_20 = 0;
    }
}

static void Task_do_ctrl_frames(int32_t frames, bool a2) {
    vector_ptr_Task* tasks = &task_work_data.tasks;
    for (int32_t i = 0; i < 3; i++)
        for (Task** j = tasks->begin; j != tasks->end; j++) {
            Task* tsk = *j;
            if (tsk->priority != i || TaskWork_has_task_dest(tsk))
                continue;
            else if ((!tsk->field_2D || frames <= 0) && a2)
                continue;

            time_struct t;
            time_struct_init(&t);
            task_work_data.current = tsk;
            Task_do_ctrl(tsk);
            task_work_data.current = 0;
            Task_add_base_calc_time(tsk, (uint32_t)time_struct_calc_time(&t));
        }
}

static void Task_do_disp(Task* t) {
    if ((t->field_1C == 1 || t->field_1C == 2)
        && t->field_18 != task_enum::INIT && t->field_18 != task_enum::DEST)
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
    if (task_work_data.disp)
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

static void Task_sub_14019C5A0(Task* t) {
    if (t->field_18 != task_enum::INIT && t->field_18 != task_enum::DEST
        && Task_sub_14019B810(t, t->field_20)) {
        switch (t->field_20) {
        case 1:
            t->field_24 = task_enum::INIT;
            t->field_28 = 1;
            break;
        case 2:
            t->field_24 = task_enum::DEST;
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
    if (task_work_has_Task(t))
        return t->field_18 == task_enum::DEST;
    return false;
}

static bool TaskWork_sub_14019B6B0(Task* t) {
    return task_work_has_Task(t) && (t->field_18 == task_enum::NONE || t->field_1C);
}
