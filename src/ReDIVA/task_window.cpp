/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_window.hpp"

extern bool input_locked;

namespace app {
    TaskWindow::TaskWindow() {
        M_show = true;
        M_focus = false;
        M_first_show = true;
    }

    TaskWindow::~TaskWindow() {

    }

    void TaskWindow::window() {

    }

    void TaskWindow::exec_window() {
        if ((M_stat == TASK_STAT_ACTIVE || M_stat == TASK_STAT_SUSPEND)
            && M_proc != TASK_PROC_INIT && M_proc != TASK_PROC_DEST) {
            window();
            M_first_show = false;
        }
    }

    void TaskWindow::set_show(bool show) {
        M_show = show;
    }

    bool TaskWindow::check_show() {
        return M_show;
    }

    void TaskWindow::reset_focus() {
        M_focus = false;
    }

    void TaskWindow::set_focus(bool focus) {
        M_focus |= focus;
    }

    bool TaskWindow::check_focus() {
        return M_focus;
    }

    void window_task() {
        task_info->now_exec_disp = true;
        for (int32_t i = 0; i < TASK_PRIO_MAX; i++)
            for (Task*& j : task_info->list) {
                Task* tsk = j;
                TaskWindow* tsk_w = dynamic_cast<TaskWindow*>(tsk);
                if (tsk_w && tsk_w->check_priority((TASK_PRIO)i) && tsk_w->check_show()) {
                    tsk_w->exec_window();
                    input_locked |= tsk_w->check_focus();
                }
            }
        task_info->now_exec_disp = false;
    }
}
