/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/task.hpp"
#include <list>
#include <string>

class TaskDataInit : public app::Task {
public:
    int32_t state;
    bool field_6C;

    TaskDataInit();
    virtual ~TaskDataInit() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp();
};

extern bool task_data_init_append_task();
extern bool task_data_init_check_state();
extern bool task_data_init_free_task();
