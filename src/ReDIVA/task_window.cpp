/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_window.hpp"

extern bool input_locked;

namespace app {
    static void TaskWindow_do_disp(TaskWindow* t);

    TaskWindow::TaskWindow() : focus() {
        show = true;
        first_show = true;
    }

    TaskWindow::~TaskWindow() {

    }

    void TaskWindow::window() {

    }

    void TaskWindow::hide_window() {
        show = false;
    }

    void TaskWindow::show_window() {
        show = true;
    }

    void TaskWork_window() {
        task_work->disp_task = true;
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks) {
                Task* tsk = j;
                TaskWindow* tsk_w = dynamic_cast<TaskWindow*>(tsk);
                if (tsk_w && tsk_w->priority == i && tsk_w->show) {
                    TaskWindow_do_disp(tsk_w);
                    input_locked |= tsk_w->focus;
                }
            }
        task_work->disp_task = false;
    }

    static void TaskWindow_do_disp(TaskWindow* t) {
        if ((t->state == Task::State::Running || t->state == Task::State::Suspended)
            && t->op != Task::Op::Init && t->op != Task::Op::Dest) {
            t->window();
            t->first_show = false;
        }
    }
}
