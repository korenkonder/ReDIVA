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

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
    };
}

extern int32_t system_startup_ready;

extern bool task_system_startup_add_task();
extern bool task_system_startup_del_task();
