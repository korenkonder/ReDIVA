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
    int32_t cos_id;
    int32_t curr_cos_id;
    int32_t disp_parts;

    DtmEqVs();
    virtual ~DtmEqVs() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool add_task(int32_t chara_id, ::chara_index chara_index);
    virtual bool del_task();

    void SetCharaIndexCosId(::chara_index chara_index, int32_t cos_id);

    void CtrlChara();
    void CtrlDispParts();
};

extern DtmEqVs* dtm_eq_vs_array;

extern void equip_test_init();
extern void equip_test_free();

extern void data_test_equip_dw_init();
