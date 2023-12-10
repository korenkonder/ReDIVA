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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
};

extern bool task_data_init_add_task();
extern bool task_data_init_check_state();
extern bool task_data_init_del_task();
