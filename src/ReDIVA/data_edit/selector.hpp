/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/task.hpp"

class DataEditSel : public app::Task {
public:
    int32_t selected_index;
    int32_t item_index;

    DataEditSel();
    ~DataEditSel();

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
};

extern DataEditSel* data_edit_sel;

extern void data_edit_sel_init();
extern int32_t data_edit_sel_get_sub_state();
extern void data_edit_sel_free();
