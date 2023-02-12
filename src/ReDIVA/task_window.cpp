/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_window.hpp"

extern bool input_locked;

namespace app {
    static void TaskWindow_do_disp(TaskWindow* t);

    TaskWindow::TaskWindow() : window_focus() {
        show_window = true;
        first_show = true;
    }

    TaskWindow::~TaskWindow() {

    }

    void TaskWindow::Window() {

    }

    void TaskWindow::HideWindow() {
        show_window = false;
    }

    void TaskWindow::ShowWindow() {
        show_window = true;
    }

    void TaskWork_Window() {
        task_work->disp = true;
        for (int32_t i = 0; i < 3; i++)
            for (Task*& j : task_work->tasks) {
                Task* tsk = j;
                TaskWindow* tsk_w = dynamic_cast<TaskWindow*>(tsk);
                if (tsk_w && tsk_w->priority == i && tsk_w->show_window) {
                    TaskWindow_do_disp(tsk_w);
                    input_locked |= tsk_w->window_focus;
                }
            }
        task_work->disp = false;
    }

    static void TaskWindow_do_disp(TaskWindow* t) {
        if ((t->state == Task::State::Running || t->state == Task::State::Suspended)
            && t->op != Task::Op::Init && t->op != Task::Op::Dest) {
            t->Window();
            t->first_show = false;
        }
    }
}
