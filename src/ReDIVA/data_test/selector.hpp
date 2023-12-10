/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/task.hpp"

class DataTestSel : public app::Task {
public:
    int32_t selected_index;
    int32_t item_index;

    DataTestSel();
    ~DataTestSel();

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
};

extern DataTestSel* data_test_sel;

extern void data_test_sel_init();
extern int32_t data_test_sel_get_sub_state();
extern void data_test_sel_free();
