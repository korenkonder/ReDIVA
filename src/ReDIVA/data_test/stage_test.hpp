/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../task_window.hpp"

struct stage_test_stage_pv {
    int32_t pv_id;
    std::vector<std::int32_t> stage;

    stage_test_stage_pv(int32_t pv_id);
    ~stage_test_stage_pv();
};

class DtwStg : public app::TaskWindow {
public:
    int32_t pv_id;
    int32_t pv_index;
    int32_t ns_index;
    int32_t other_index;
    int32_t stage_index;

    int32_t stage_index_load;
    bool stage_load;

    std::vector<stage_test_stage_pv> stage_pv;
    std::vector<std::int32_t> stage_ns;
    std::vector<std::int32_t> stage_other;

    DtwStg();
    virtual ~DtwStg() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;
};

extern DtwStg* dtw_stg;

extern void dtw_stg_init();
extern void dtw_stg_load(bool hide);
extern void dtw_stg_unload();
extern void dtw_stg_free();
