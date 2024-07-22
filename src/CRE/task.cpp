/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../KKdLib/time.hpp"

namespace app {
    static void Task_add_base_calc_time(Task* t, uint32_t value);
    static void Task_do_basic(Task* t);
    static void Task_do_ctrl(Task* t);
    static void Task_do_ctrl_frames(int32_t frames, bool frame_skip);
    static void Task_do_disp(Task* t);
    static void Task_set_base_calc_time(Task* t, uint32_t value);
    static void Task_set_calc_time(Task* t);
    static void Task_set_disp_time(Task* t, uint32_t value);
    static bool Task_check_request(Task* t, Task::Request request);
    static void Task_set_request(Task* t, Task::Request request);
    static void Task_update_op_state(Task* t);

    TaskWork* task_work;

    TaskInterface::TaskInterface() {

    }

    TaskInterface::~TaskInterface() {

    }

    bool TaskInterface::init() {
        return true;
    }

    bool TaskInterface::ctrl() {
        return false;
    }

    bool TaskInterface::dest() {
        return true;
    }

    void TaskInterface::disp() {

    }

    void TaskInterface::basic() {

    }

    Task::Task() {
        priority = 1;
        parent_task = 0;
        op = Task::Op::None;
        state = Task::State::None;
        request = Task::Request::None;
        next_op = Task::Op::None;
        next_state = Task::State::None;
        field_2C = false;
        frame_dependent = false;
        set_name("(unknown)");
        base_calc_time = 0;
        calc_time = 0;
        calc_time_max = 0;
        disp_time = 0;
        disp_time_max = 0;
    }

    Task::~Task() {

    }

    uint32_t Task::get_calc_time() {
        return calc_time;
    }

    uint32_t Task::get_calc_time_max() {
        return calc_time_max;
    }

    uint32_t Task::get_disp_time() {
        return disp_time;
    }

    uint32_t Task::get_disp_time_max() {
        return disp_time_max;
    }

    const char* Task::get_name() {
        return name;
    }

    bool Task::del() {
        if (!TaskWork::has_task(this) || !Task_check_request(this, Task::Request::Dest))
            return false;

        Task_set_request(this, Task::Request::Dest);
        return true;
    }

    bool Task::hide() {
        if (!TaskWork::has_task(this) || !Task_check_request(this, Task::Request::Hide))
            return false;

        Task_set_request(this, Task::Request::Hide);
        return true;
    }

    bool Task::run() {
        if (!TaskWork::has_task(this) || !Task_check_request(this, Task::Request::Run))
            return false;

        Task_set_request(this, Task::Request::Run);
        return true;
    }

    bool Task::suspend() {
        if (!TaskWork::has_task(this) || !Task_check_request(this, Task::Request::Suspend))
            return false;

        Task_set_request(this, Task::Request::Suspend);
        return true;
    }

    void Task::set_name(const char* name) {
        if (!name) {
            this->name[0] = 0;
            return;
        }

        size_t len = utf8_length(name);
        len = min_def(len, sizeof(this->name) - 1);
        memmove(&this->name, name, len);
        this->name[len] = 0;
    }

    void Task::set_priority(int32_t priority) {
        this->priority = priority;
    }

    TaskWork::TaskWork() : current_task(), disp_task() {

    }

    TaskWork::~TaskWork() {
        for (Task* i : tasks)
            i->del();

        while (tasks.size()) {
            ctrl();
            basic();
        }
    }

    bool TaskWork::add_task(Task* t, const char* name, int32_t priority) {
        return TaskWork::add_task(t, task_work->current_task, name, priority);
    }

    bool TaskWork::add_task(Task* t, Task* parent_task, const char* name, int32_t priority) {
        Task_set_request(t, Task::Request::None);
        if (TaskWork::has_task(t) || !Task_check_request(t, Task::Request::Init))
            return false;

        t->set_priority(priority);
        t->parent_task = parent_task;
        t->op = Task::Op::None;
        t->state = Task::State::None;
        t->next_op = Task::Op::None;
        t->next_state = Task::State::None;
        t->set_name(name);
        Task_set_request(t, Task::Request::Init);
        Task_update_op_state(t);
        task_work->tasks.push_back(t);
        return true;
    }

    void TaskWork::basic() {
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks)
                if (j->priority == i)
                    Task_do_basic(j);
    }

    bool TaskWork::check_task_ctrl(Task* t) {
        return t && app::TaskWork::has_task(t) && t->state == Task::State::Running && t->op == Task::Op::Ctrl;
    }

    bool TaskWork::check_task_ready(Task* t) {
        return t && TaskWork::has_task(t) && (t->op == Task::Op::None || t->state != Task::State::None);
    }

    void TaskWork::ctrl() {
        for (Task*& i : task_work->tasks) {
            Task_update_op_state(i);
            Task_set_base_calc_time(i, 0);
        }

        for (int32_t i = 2; i >= 0; i--)
            for (auto j = task_work->tasks.end(); j != task_work->tasks.begin(); ) {
                --j;
                Task* tsk = *j;
                if (tsk->priority != i || !has_task_dest(tsk))
                    continue;

                time_struct t;
                task_work->current_task = tsk;
                Task_do_ctrl(tsk);
                task_work->current_task = 0;
                Task_add_base_calc_time(tsk, (uint32_t)(t.calc_time() * 1000.0));
            }

        for (auto i = task_work->tasks.begin(); i != task_work->tasks.end(); ) {
            if (TaskWork::check_task_ready(*i)) {
                i++;
                continue;
            }

            i = task_work->tasks.erase(i);
        }

        int32_t frames = get_delta_frame_history_int();
        if (frames > 1)
            for (int32_t i = frames - 1; i; i--)
                Task_do_ctrl_frames(frames, true);
        Task_do_ctrl_frames(frames, false);

        for (Task*& i : task_work->tasks)
            Task_set_calc_time(i);
    }

    void TaskWork::dest() {
        for (Task*& i : task_work->tasks)
            i->del();
    }

    void TaskWork::disp() {
        task_work->disp_task = true;
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks) {
                Task* tsk = j;
                if (tsk->priority != i)
                    continue;

                time_struct t;
                Task_do_disp(tsk);
                Task_set_disp_time(tsk, (uint32_t)(t.calc_time() * 1000.0));
            }
        task_work->disp_task = false;
    }

    Task* TaskWork::get_task_by_index(int32_t index) {
        int32_t k = 0;
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks)
                if (j->priority == i)
                    if (k++ == index)
                        return j;
        return 0;
    }

    bool TaskWork::has_task(Task* t) {
        if (!task_work->tasks.size())
            return false;

        for (Task*& i : task_work->tasks)
            if (i == t)
                return true;
        return false;
    }

    bool TaskWork::has_task_init(Task* t) {
        if (t && TaskWork::has_task(t))
            return t->op == Task::Op::Init;
        else
            return false;
    }

    bool TaskWork::has_task_ctrl(Task* t) {
        if (t && TaskWork::has_task(t))
            return t->op == Task::Op::Ctrl;
        else
            return false;
    }

    bool TaskWork::has_task_dest(Task* t) {
        if (t && TaskWork::has_task(t))
            return t->op == Task::Op::Dest;
        else
            return false;
    }

    bool TaskWork::has_tasks_dest() {
        for (Task*& i : task_work->tasks)
            if (TaskWork::has_task_dest(i))
                return true;
        return false;
    }

    extern void task_work_init() {
        task_work = new TaskWork;
    }

    extern void task_work_free() {
        delete task_work;
    }

    static void Task_add_base_calc_time(Task* t, uint32_t value) {
        t->base_calc_time += value;
    }

    static void Task_do_basic(Task* t) {
        if ((t->state == Task::State::Running || t->state == Task::State::Suspended)
            && t->op != Task::Op::Init && t->op != Task::Op::Dest)
            t->basic();
    }

    static void Task_do_ctrl(Task* t) {
        if (t->state != Task::State::Running)
            return;

        if (t->op == Task::Op::Init && t->init()) {
            t->next_op = Task::Op::Ctrl;
            t->op = Task::Op::Ctrl;
        }

        if (t->op == Task::Op::Ctrl && t->ctrl()) {
            t->next_op = Task::Op::Dest;
            t->op = Task::Op::Dest;
        }

        if (t->op == Task::Op::Dest && t->dest()) {
            t->next_state = Task::State::None;
            t->next_op = Task::Op::Max;
            t->request = Task::Request::None;
        }
    }

    static void Task_do_ctrl_frames(int32_t frames, bool frame_skip) {
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks) {
                Task* tsk = j;
                if (tsk->priority != i || TaskWork::has_task_dest(tsk))
                    continue;
                else if (tsk->frame_dependent) {
                    if (frames <= 0)
                        continue;
                }
                else if (frame_skip)
                    continue;

                time_struct t;
                task_work->current_task = tsk;
                Task_do_ctrl(tsk);
                task_work->current_task = 0;
                Task_add_base_calc_time(tsk, (uint32_t)(t.calc_time() * 1000.0));
            }
    }

    static void Task_do_disp(Task* t) {
        if ((t->state == Task::State::Running || t->state == Task::State::Suspended)
            && t->op != Task::Op::Init && t->op != Task::Op::Dest)
            t->disp();
    }

    static void Task_set_base_calc_time(Task* t, uint32_t value) {
        t->base_calc_time = value;
    }

    static void Task_set_calc_time(Task* t) {
        t->calc_time = t->base_calc_time;
        uint32_t main_timer = get_main_timer();
        if (main_timer == (main_timer / 300) * 300)
            t->calc_time_max = 0;
        t->calc_time = max_def(t->calc_time, t->calc_time_max);
    }

    static void Task_set_disp_time(Task* t, uint32_t value) {
        t->disp_time = value;
        uint32_t main_timer = get_main_timer();
        if (main_timer == (main_timer / 300) * 300)
            t->disp_time_max = 0;
        t->disp_time = max_def(t->disp_time, t->disp_time_max);
    }

    static bool Task_check_request(Task* t, Task::Request request) {
        if (task_work->disp_task)
            return false;

        switch (request) {
        case Task::Request::Init:
            return true;
        case Task::Request::Dest:
            return t->state != Task::State::None;
        case Task::Request::Suspend:
            return t->state == Task::State::Running || t->state == Task::State::Hidden;
        case Task::Request::Hide:
            return t->state == Task::State::Running || t->state == Task::State::Suspended;
        case Task::Request::Run:
            return t->state == Task::State::Suspended || t->state == Task::State::Hidden;
        }
        return false;
    }

    static void Task_set_request(Task* t, Task::Request request) {
        if (request > Task::Request::Init)
            for (Task*& i : task_work->tasks)
                if (i->parent_task == t)
                    Task_set_request(i, request);
        t->request = request;
    }

    static void Task_update_op_state(Task* t) {
        if (t->op != Task::Op::Init && t->op != Task::Op::Dest
            && Task_check_request(t, t->request)) {
            switch (t->request) {
            case Task::Request::Init:
                t->next_op = Task::Op::Init;
                t->next_state = Task::State::Running;
                break;
            case Task::Request::Dest:
                t->next_op = Task::Op::Dest;
                t->next_state = Task::State::Running;
                break;
            case Task::Request::Suspend:
                t->next_state = Task::State::Suspended;
                break;
            case Task::Request::Hide:
                t->next_state = Task::State::Hidden;
                break;
            case Task::Request::Run:
                t->next_state = Task::State::Running;
                break;
            }
            t->request = Task::Request::None;
        }

        t->op = t->next_op;
        t->state = t->next_state;
    }
}
