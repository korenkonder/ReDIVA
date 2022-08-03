/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/lock.hpp"
#include "../CRE/render_context.hpp"
#include "../CRE/task.hpp"

#define CLASS_DATA_NO_DATA (0)
#define CLASSES_DATA_NO_FUNC (0)

namespace app {
    class TaskWindow : public Task {
    public:
        bool show_window;
        bool window_focus;
        bool first_show;

        TaskWindow();
        ~TaskWindow();
        virtual void Window() = 0;

        void HideWindow();
        void ShowWindow();
    };

    extern void TaskWork_Window();
}
