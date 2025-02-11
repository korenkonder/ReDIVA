/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/render_context.hpp"
#include "../CRE/task.hpp"

#define CLASS_DATA_NO_DATA (0)
#define CLASSES_DATA_NO_FUNC (0)

namespace app {
    class TaskWindow : public Task {
    public:
        bool show;
        bool focus;
        bool first_show;

        TaskWindow();
        ~TaskWindow();

        virtual void window() = 0;

        void hide_window();
        void show_window();
    };

    extern void TaskWork_window();
}
