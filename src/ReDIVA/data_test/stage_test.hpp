/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/task.hpp"
#include <vector>

struct stage_test_stage_pv {
    int32_t pv_id;
    std::vector<int32_t> stage;

    stage_test_stage_pv(int32_t pv_id);
    ~stage_test_stage_pv();
};

class DtmStg : public app::Task {
public:
    int32_t stage_index;
    int32_t load_stage_index;

    DtmStg();
    virtual ~DtmStg() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
};

extern DtmStg* dtm_stg;

extern void dtm_stg_init();
extern void dtm_stg_load(int32_t stage_index);
extern bool dtm_stg_unload();
extern void dtm_stg_free();

extern void dtw_stg_init();
extern void dtw_stg_load(bool hide);
extern void dtw_stg_unload();
