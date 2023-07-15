/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/task.hpp"

class DtmEqVs : public app::Task {
public:
    int32_t state;
    int32_t chara_id;
    ::chara_index chara_index;
    ::chara_index curr_chara_index;
    int32_t module_index;
    int32_t curr_module_index;
    int32_t disp_parts;

    DtmEqVs();
    virtual ~DtmEqVs() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;

    virtual bool AddTask(int32_t chara_id, ::chara_index chara_index);
    virtual bool DelTask();

    void SetCharaIndexModuleIndex(::chara_index chara_index, int32_t module_index);

    void CtrlChara();
    void CtrlDispParts();
};

extern DtmEqVs* dtm_eq_vs_array;

extern void equip_test_init();
extern void equip_test_free();

extern void data_test_equip_dw_init();
