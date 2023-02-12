/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../dw.hpp"
#include "../task_window.hpp"

struct stage_test_stage_pv {
    int32_t pv_id;
    std::vector<std::int32_t> stage;

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

class DtwStg : public dw::Shell {
public:
    std::map<std::string, std::vector<std::string>> pv_stage;
    dw::ListBox* stage;
    dw::ListBox* ns;
    dw::ListBox* pv;
    dw::ListBox* pv_id;
    dw::ListBox* other;
    dw::Button* stage_display;
    dw::Button* ground;
    dw::Button* ring;
    dw::Button* sky;
    dw::Button* effect_display;

    DtwStg();
    virtual ~DtwStg() override;

    virtual void Hide() override;

    static void PvIdCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
};

extern DtmStg* dtm_stg;

extern DtwStg* dtw_stg;

extern void dtm_stg_init();
extern void dtm_stg_load(int32_t stage_index);
extern bool dtm_stg_unload();
extern void dtm_stg_free();

extern void dtw_stg_init();
extern void dtw_stg_load(bool hide);
extern void dtw_stg_unload();
extern void dtw_stg_free();
