/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/customize_item_table.hpp"
#include "../../CRE/task.hpp"
#include <vector>

class TaskDataTestOpd : public app::Task {
public:
    int32_t state;
    std::vector<std::string> paths;

    TaskDataTestOpd();
    virtual ~TaskDataTestOpd() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void BeginDataCreation();
    void GetCustomizeItemObjectNames(const customize_item& cstm, std::vector<std::string>& objects);

    bool add_task();
};

extern TaskDataTestOpd* task_data_test_opd;

extern void opd_test_init();
extern void opd_test_free();
