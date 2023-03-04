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

    task_stage_current_set_stage_display(dtw_stg->stage_display->value, 1);
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

    dw::Widget::SetName("STAGE TEST");

    dw::Composite* pv_comp = new dw::Composite(this);
    pv_comp->SetLayout(new dw::RowLayout);

    (new dw::Label(pv_comp))->SetName("PV:");

    pv_id = new dw::ListBox(pv_comp);
    pv = new dw::ListBox(pv_comp);

    dw::Composite* ns_comp = new dw::Composite(this);
    ns_comp->SetLayout(new dw::RowLayout);

    (new dw::Label(ns_comp))->SetName("NS:");

    ns = new dw::ListBox(ns_comp);

    dw::Composite* other_comp = new dw::Composite(this);
    other_comp->SetLayout(new dw::RowLayout);

    (new dw::Label(other_comp))->SetName("Other:");

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

    //dw::List::sub_1402F9930(this->pv_id->list, 30);
    pv_id->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::PvIdCallback));

    //dw::List::sub_1402F9930(this->pv->list, 30);
    pv->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    //dw::List::sub_1402F9930(this->ns->list, 30);
    ns->list->hovered_item = 0;
    ns->list->ResetSetSelectedItem(0);
    ns->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    //dw::List::sub_1402F9930(this->stage->list, 30);
    stage->list->hovered_item = 0;
    stage->list->ResetSetSelectedItem(0);
    stage->AddSelectionListener(new dw::SelectionListenerOnHook(DtwStg::StageCallback));

    stage_display = new dw::Button(this, WIDGET_CHECKBOX);
    stage_display->SetName("Stage display");
    stage_display->SetValue(true);

    ring = new dw::Button(this, WIDGET_CHECKBOX);
    ring->SetName("[Ring]");
    ring->SetValue(true);

    ground = new dw::Button(this, WIDGET_CHECKBOX);
    ground->SetName("[Ground]");
    ground->SetValue(true);

    sky = new dw::Button(this, WIDGET_CHECKBOX);
    sky->SetName("[Sky]");
    sky->SetValue(true);

    effect_display = new dw::Button(this, WIDGET_CHECKBOX);
    effect_display->SetName("Effects display");
    effect_display->SetValue(true);

    //GetSetSize();
}

DtwStg::~DtwStg() {

}

void DtwStg::Hide() {
    SetDisp(false);
}

/*void DtwStg::Window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 240.0f);
    float_t h = min_def((float_t)height, 240.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    if (!ImGui::Begin("Stage Test##Data Test", 0, window_flags)) {
        ImGui::End();
        return;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    std::vector<stage_data>& stg_data = aft_stage_data->stage_data;

    int32_t _pv_id = this->_pv_id;

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%03d", _pv_id);

    ImGui::Text("PV:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(48.0f);
    if (ImGui::BeginCombo("##PV:", buf, 0)) {
        for (stage_test_stage_pv& i : stage_pv) {
            ImGui::PushID(&i);
            sprintf_s(buf, sizeof(buf), "%03d", i.pv_id);
            if (ImGui::Selectable(buf, _pv_id == i.pv_id)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _pv_id != i.pv_id))
                _pv_id = i.pv_id;
            ImGui::PopID();

            if (_pv_id == i.pv_id)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (_pv_id != this->_pv_id) {
        pv_index = -1;
        this->_pv_id = _pv_id;
    }

    int32_t _pv_index = pv_index;

    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    bool pv_id_found = false;
    for (stage_test_stage_pv& i : stage_pv) {
        if (_pv_id != i.pv_id)
            continue;

        if (ImGui::BeginCombo("##PV Index", pv_index > -1
            ? stg_data[i.stage[pv_index]].name.c_str() : "", 0)) {
            for (int32_t& j : i.stage) {
                int32_t pv_idx = (int32_t)(&j - i.stage.data());

                ImGui::PushID(j);
                if (ImGui::Selectable(stg_data[j].name.c_str(), _pv_index == pv_idx)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && _pv_index != pv_idx)) {
                    pv_index = -1;
                    _pv_index = pv_idx;
                }
                ImGui::PopID();

                if (_pv_index == pv_idx)
                    ImGui::SetItemDefaultFocus();
            }
            window_focus |= ImGui::IsWindowFocused();
            ImGui::EndCombo();
        }

        if (_pv_index != pv_index) {
            //stage_load = true;
            //stage_index_load = i.stage[_pv_index];
            pv_index = _pv_index;
        }
        pv_id_found = true;
        break;
    }

    if (!pv_id_found && ImGui::BeginCombo("##PV Index", "", 0)) {
        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    int32_t _ns_index = ns_index;

    ImGui::Text("NS:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    if (ImGui::BeginCombo("##NS Index", _ns_index > -1
        ? stg_data[(stage_ns)[_ns_index]].name.c_str() : "", 0)) {
        for (int32_t& i : stage_ns) {
            int32_t ns_idx = (int32_t)(&i - stage_ns.data());

            ImGui::PushID(i);
            if (ImGui::Selectable(stg_data[stage_ns[i]].name.c_str(), _ns_index == ns_idx)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _ns_index != ns_idx)) {
                ns_index = -1;
                _ns_index = ns_idx;
            }
            ImGui::PopID();

            if (_ns_index == ns_idx)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (_ns_index != ns_index) {
        //stage_load = true;
        //stage_index_load = stage_ns[_ns_index];
        ns_index = _ns_index;
    }

    int32_t _other_index = other_index;

    ImGui::Text("Other:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    if (ImGui::BeginCombo("##Other Index", _other_index > -1
        ? stg_data[stage_other[_other_index]].name.c_str() : "", 0)) {
        for (int32_t& i : stage_other) {
            int32_t other_idx = (int32_t)(&i - stage_other.data());

            ImGui::PushID(i);
            if (ImGui::Selectable(stg_data[i].name.c_str(), _other_index == other_idx)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _other_index != other_idx)) {
                other_index = -1;
                _other_index = other_idx;
            }
            ImGui::PopID();

            if (_other_index == other_idx)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (_other_index != other_index) {
        //stage_load = true;
        //stage_index_load = stage_other[_other_index];
        other_index = _other_index;
    }

    int32_t _stage_index = stage_index;

    ImGui::GetContentRegionAvailSetNextItemWidth();
    if (ImGui::BeginCombo("##Stage Index", _stage_index > -1
        ? stg_data[_stage_index].name.c_str() : "", 0)) {
        for (stage_data& i : stg_data) {
            int32_t stage_idx = (int32_t)(&i - stg_data.data());

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.name.c_str(), _stage_index == stage_idx)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _stage_index != stage_idx)) {
                stage_index = -1;
                _stage_index = stage_idx;
            }
            ImGui::PopID();

            if (_stage_index == stage_idx)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (_stage_index != stage_index) {
        //stage_load = true;
        //stage_index_load = _stage_index;
        stage_index = _stage_index;
    }

    ::stage* stg = task_stage_get_current_stage();
    if (stg) {
        ImGui::Checkbox("Stage display", &stg->stage_display);
        ImGui::Checkbox("[Ring]", &stg->ring);
        ImGui::Checkbox("[Ground]", &stg->ground);
        ImGui::Checkbox("[Sky]", &stg->sky);
        //ImGui::Checkbox("Effects display", &stg->effect_display);
        bool effects_display = true;
        ImGui::Checkbox("Effects display", &effects_display);
    }
    else {
        bool stage_display = true;
        ImGui::Checkbox("Stage display", &stage_display);
        bool ring = true;
        ImGui::Checkbox("[Ring]", &ring);
        bool ground = true;
        ImGui::Checkbox("[Ground]", &ground);
        bool sky = true;
        ImGui::Checkbox("[Sky]", &sky);
        bool effects_display = true;
        ImGui::Checkbox("Effects display", &effects_display);
    }

    window_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}*/

void DtwStg::PvIdCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        std::string name = list_box->GetItem(list_box->list->selected_item);

        dw::ListBox* pv_list_box = dtw_stg->pv;
        pv_list_box->ClearItems();
        auto elem = dtw_stg->pv_stage.find(name);
        if (elem != dtw_stg->pv_stage.end())
            for (std::string& i : elem->second)
                pv_list_box->AddItem(i);
    }
}

void DtwStg::StageCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        std::string name = list_box->GetItem(list_box->list->selected_item);
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
    if (!dtw_stg)
        dtw_stg = new DtwStg;
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
