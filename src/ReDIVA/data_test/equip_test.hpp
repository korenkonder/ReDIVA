/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/task.hpp"

class DtmEqVs : public app::Task {
private:
    int32_t m_mode;
    int32_t m_rc;
    int32_t m_cn_cur;
    int32_t m_cn_pre;
    int32_t m_cs_cur;
    int32_t m_cs_pre;
    int32_t m_dp;

public:
    DtmEqVs();
    virtual ~DtmEqVs() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool add_task(int32_t rob_id, int32_t cn);
    virtual bool del_task();

    void SetCharaNumCosId(int32_t cn, int32_t cs);

    void CtrlChara();
    void CtrlDispParts();
};

extern DtmEqVs* dtm_eq_vs_array;

extern void equip_test_init();
extern void equip_test_free();

extern void data_test_equip_dw_init();
