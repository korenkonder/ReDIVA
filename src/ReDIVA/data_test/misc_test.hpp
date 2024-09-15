/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/task.hpp"
#include <vector>

class TaskDataTestMisc : public app::Task {
public:
    int32_t state;

    TaskDataTestMisc();
    virtual ~TaskDataTestMisc() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
};

extern TaskDataTestMisc* task_data_test_misc;

extern void task_data_test_misc_init();
extern void task_data_test_misc_free();
