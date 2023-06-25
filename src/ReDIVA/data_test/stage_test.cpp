/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_test.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/task_effect.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "../task_window.hpp"

extern int32_t width;
extern int32_t height;

DtmStg* dtm_stg;
DtwStg* dtw_stg;

static int stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2);

stage_test_stage_pv::stage_test_stage_pv(int32_t pv_id) {
    this->pv_id = pv_id;
}

stage_test_stage_pv::~stage_test_stage_pv() {

}

DtmStg::DtmStg() : stage_index(), load_stage_index() {

}

DtmStg::~DtmStg() {

}

bool DtmStg::Init() {
    task_stage_load_task("DATA_TEST_STG_STAGE");
    task_stage_set_stage_index(stage_index);
    return true;
}

bool DtmStg::Ctrl() {
    if (task_stage_check_not_loaded())
        return false;

    if (load_stage_index != stage_index) {
        task_stage_set_stage_index(load_stage_index);
        stage_index = load_stage_index;
        return false;
    }

    task_stage_current_set_stage_display(dtw_stg->stage_display->value, true);
    task_stage_current_set_ground(dtw_stg->ground->value);
    task_stage_current_set_ring(dtw_stg->ring->value);
    task_stage_current_set_sky(dtw_stg->sky->value);
    task_effect_parent_set_enable(dtw_stg->effect_display->value);
    return false;
}

bool DtmStg::Dest() {
    task_stage_unload_task();
    return true;
}

DtwStg::DtwStg() : Shell(0) {
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    std::vector<stage_data>& stg_data = aft_stage_data->stage_data;

    dw::Widget::SetText(L"STAGE TEST");

    dw::Composite* pv_comp = new dw::Composite(this);
    pv_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(pv_comp))->SetText(L"PV:");

    pv_id = new dw::ListBox(pv_comp);

    pv = new dw::ListBox(pv_comp);

    dw::Composite* ns_comp = new dw::Composite(this);
    ns_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(ns_comp))->SetText(L"NS:");

    ns = new dw::ListBox(ns_comp);

    dw::Composite* other_comp = new dw::Composite(this);
    other_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(other_comp))->SetText(L"Other:");

    other = new dw::ListBox(other_comp);

    stage = new dw::ListBox(this);

    for (const stage_data& i : aft_stage_data->stage_data) {
        const std::string& name = i.name;
        stage->AddItem(name);

        std::string pv_id;
        if (name.size() >= 8 && !name.find("STGPV"))
            pv_id.assign(name, 5, 3);
        else if (name.size() >= 10 && !name.find("STGD2PV"))
            pv_id.assign(name, 7, 3);

        if (pv_id.size()) {
            auto elem = pv_stage.find(pv_id);
            if (elem == pv_stage.end())
                elem = pv_stage.insert({ pv_id, {} }).first;
            elem->second.push_back(name);
        }
        else if (!name.find("STGNS") || !name.find("STGD2NS"))
            ns->AddItem(name);
        else
            other->AddItem(name);
    }

    for (auto& i : pv_stage)
        pv_id->AddItem(i.first);

    pv_id->SetMaxItems(30);
    pv_id->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::PvIdCallback));

    pv->SetMaxItems(30);
    pv->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    ns->SetMaxItems(30);
    ns->SetItemIndex(0);
    ns->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    stage->SetMaxItems(30);
    stage->SetItemIndex(0);
    stage->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    stage_display = new dw::Button(this, dw::CHECKBOX);
    stage_display->SetText(L"Stage display");
    stage_display->SetValue(true);

    ring = new dw::Button(this, dw::CHECKBOX);
    ring->SetText(L"[Ring]");
    ring->SetValue(true);

    ground = new dw::Button(this, dw::CHECKBOX);
    ground->SetText(L"[Ground]");
    ground->SetValue(true);

    sky = new dw::Button(this, dw::CHECKBOX);
    sky->SetText(L"[Sky]");
    sky->SetValue(true);

    effect_display = new dw::Button(this, dw::CHECKBOX);
    effect_display->SetText(L"Effects display");
    effect_display->SetValue(true);

    GetSetSize();
}

DtwStg::~DtwStg() {

}

void DtwStg::Hide() {
    SetDisp(false);
}

void DtwStg::PvIdCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        std::string name = list_box->GetSelectedItemStr();

        dw::ListBox* pv_list_box = dtw_stg->pv;
        pv_list_box->ClearItems();
        auto elem = dtw_stg->pv_stage.find(name);
        if (elem != dtw_stg->pv_stage.end()) {
            for (std::string& i : elem->second)
                pv_list_box->AddItem(i);
            dtw_stg->GetSetSize();
        }
    }
}

void DtwStg::StageCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        std::string name = list_box->GetSelectedItemStr();
        dtm_stg->load_stage_index = aft_stage_data->get_stage_index(name.c_str());
    }
}

void dtm_stg_init() {
    dtm_stg = new DtmStg;
}

void dtm_stg_load(int32_t stage_index) {
    if (app::TaskWork::CheckTaskReady(dtm_stg))
        return;

    if (app::TaskWork::CheckTaskReady(dtm_stg)) {
        dtm_stg->stage_index = stage_index;
        dtm_stg->load_stage_index = stage_index;
    }
    app::TaskWork::AddTask(dtm_stg, "DATA_TEST_STAGE");
}

bool dtm_stg_unload() {
    return dtm_stg->DelTask();
}

void dtm_stg_free() {
    if (dtw_stg) {
        delete dtw_stg;
        dtw_stg = 0;
    }

    if (dtm_stg) {
        delete dtm_stg;
        dtm_stg = 0;
    }
}

void dtw_stg_init() {
    if (!dtw_stg) {
        dtw_stg = new DtwStg;
        dtw_stg->sub_1402F38B0();
    }
    else
        dtw_stg->Disp();
}

void dtw_stg_load(bool hide) {
    dtw_stg_init();
    if (hide)
        dtw_stg->SetDisp(false);
}

void dtw_stg_unload() {
    dtw_stg->Hide();
}

static int stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2) {
    int32_t pv1 = ((stage_test_stage_pv*)src1)->pv_id;
    int32_t pv2 = ((stage_test_stage_pv*)src2)->pv_id;
    return pv1 > pv2 ? 1 : (pv1 < pv2 ? -1 : 0);
}
