/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/task.hpp"
#include <list>
#include <string>

namespace system_startup_detail {
    class TaskSystemStartup : public app::Task {
    public:
        TaskSystemStartup();
        virtual ~TaskSystemStartup() override;

        virtual bool init() override;
        virtual bool ctrl() override;
        virtual bool dest() override;
        virtual void disp() override;
    };
}

extern int32_t system_startup_ready;

extern bool task_system_startup_add_task();
extern bool task_system_startup_del_task();
