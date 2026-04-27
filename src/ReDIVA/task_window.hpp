/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/task.hpp"

namespace app {
    class TaskWindow : public Task {
    private:
        bool M_show;
        bool M_focus;
        bool M_first_show;

    public:
        TaskWindow();
        ~TaskWindow();

        virtual void window() = 0;

        void exec_window();

        void set_show(bool show);
        bool check_show();
        void reset_focus();
        void set_focus(bool focus);
        bool check_focus();
    };

    extern void window_task();
}
