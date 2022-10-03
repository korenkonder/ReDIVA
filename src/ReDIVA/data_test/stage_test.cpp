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
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "../task_window.hpp"

extern int32_t width;
extern int32_t height;

DtwStg* dtw_stg;

static int stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2);

stage_test_stage_pv::stage_test_stage_pv(int32_t pv_id) {
    this->pv_id = pv_id;
}

stage_test_stage_pv::~stage_test_stage_pv() {

}

DtwStg::DtwStg() : pv_id(), pv_index(), ns_index(),
other_index(), stage_index(), stage_index_load(), stage_load() {
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    std::vector<stage_data>& stg_data = aft_stage_data->stage_data;

    pv_id = -1;
    pv_index = -1;
    ns_index = -1;
    other_index = -1;
    stage_index = -1;

    stage_index_load = -1;
    stage_load = false;

    size_t stg_pv_count = 0;
    size_t stg_ns_count = 0;
    size_t stg_other_count = 0;
    for (stage_data& i : stg_data) {
        bool stgpv = false;
        bool stgd2pv = false;
        bool stgns = false;
        bool stgd2ns = false;
        if (i.name.size() >= 8 && !memcmp(i.name.c_str(), "STGPV", 5))
            stg_pv_count++;
        else if (i.name.size() >= 10 && !memcmp(i.name.c_str(), "STGD2PV", 7))
            stg_pv_count++;
        else if (i.name.size() >= 8 && !memcmp(i.name.c_str(), "STGNS", 5))
            stg_ns_count++;
        else if (i.name.size() >= 10 && !memcmp(i.name.c_str(), "STGD2NS", 7))
            stg_ns_count++;
        else
            stg_other_count++;
    }

    stage_pv.reserve(stg_pv_count);
    stage_ns.reserve(stg_ns_count);
    stage_other.reserve(stg_other_count);

    for (stage_data& i : stg_data) {
        bool stgpv = false;
        bool stgd2pv = false;
        bool stgns = false;
        bool stgd2ns = false;
        if (i.name.size() >= 8 && !memcmp(i.name.c_str(), "STGPV", 5))
            stgpv = true;
        else if (i.name.size() >= 10 && !memcmp(i.name.c_str(), "STGD2PV", 7))
            stgd2pv = true;
        else if (i.name.size() >= 8 && !memcmp(i.name.c_str(), "STGNS", 5))
            stgns = true;
        else if (i.name.size() >= 10 && !memcmp(i.name.c_str(), "STGD2NS", 7))
            stgd2ns = true;

        if (stgpv || stgd2pv) {
            int32_t pv_id = 0;
            int32_t ret;
            if (stgpv)
                ret = sscanf_s(i.name.c_str() + 5, "%03d", &pv_id);
            else
                ret = sscanf_s(i.name.c_str() + 7, "%03d", &pv_id);

            if (ret != 1)
                continue;

            std::vector<stage_test_stage_pv>::iterator stg_pv = stage_pv.begin();
            while (stg_pv != stage_pv.end())
                if (pv_id == (stg_pv++)->pv_id)
                    break;

            if (stg_pv == stage_pv.end()) {
                stage_pv.push_back(stage_test_stage_pv(pv_id));
                stg_pv = stage_pv.end() - 1;
            }

            stg_pv->stage.push_back((int32_t)(&i - stg_data.data()));
        }
        else if (stgns || stgd2ns) {
            int32_t ns_id = 0;
            int32_t ret;
            if (stgns)
                ret = sscanf_s(i.name.c_str() + 5, "%03d", &ns_id);
            else
                ret = sscanf_s(i.name.c_str() + 7, "%03d", &ns_id);

            if (ret != 1)
                continue;

            stage_ns.push_back((int32_t)(&i - stg_data.data()));
        }
        else
            stage_other.push_back((int32_t)(&i - stg_data.data()));
    }

    if (stage_pv.size())
        quicksort_custom(stage_pv.data(), stage_pv.size(),
            sizeof(stage_test_stage_pv),
            stage_test_stage_pv_quicksort_compare_func);

    if (stage_pv.size())
        pv_id = stage_pv[0].pv_id;

    stage_pv = stage_pv;
    stage_ns = stage_ns;
    stage_other = stage_other;

    stage_index = 0;
    stage_load = false;
    stage_index_load = 0;
}

DtwStg::~DtwStg() {

}

bool DtwStg::Init() {
    return true;
}

bool DtwStg::Ctrl() {
    if (stage_load) {
        task_stage_set_stage_index(stage_index_load);
        stage_load = false;
    }
    return false;
}

bool DtwStg::Dest() {
    return true;
}

void DtwStg::Window() {

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

    int32_t _pv_id = pv_id;

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
                || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && pv_id != i.pv_id))
                _pv_id = i.pv_id;
            ImGui::PopID();

            if (_pv_id == i.pv_id)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (_pv_id != pv_id) {
        pv_index = -1;
        pv_id = _pv_id;
    }

    int32_t _pv_index = pv_index;

    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    bool pv_id_found = false;
    for (stage_test_stage_pv& i : stage_pv) {
        if (pv_id != i.pv_id)
            continue;

        if (ImGui::BeginCombo("##PV Index", pv_index > -1
            ? stg_data[i.stage[pv_index]].name.c_str() : "", 0)) {
            for (int32_t& j : i.stage) {
                int32_t pv_idx = (int32_t)(&j - i.stage.data());

                ImGui::PushID(j);
                if (ImGui::Selectable(stg_data[j].name.c_str(), _pv_index == pv_idx)
                    || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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
            stage_load = true;
            stage_index_load = i.stage[_pv_index];
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
                || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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
        stage_load = true;
        stage_index_load = stage_ns[_ns_index];
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
                || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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
        stage_load = true;
        stage_index_load = stage_other[_other_index];
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
                || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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
        stage_load = true;
        stage_index_load = _stage_index;
        stage_index = _stage_index;
    }

    stage* stg = task_stage_get_current_stage();
    if (stg) {
        ImGui::Checkbox("Stage display", &stg->stage_display);
        ImGui::Checkbox("[Ring]", &stg->ring);
        ImGui::Checkbox("[Ground]", &stg->ground);
        ImGui::Checkbox("[Sky]", &stg->sky);
        ImGui::Checkbox("Effects display", &stg->effect_display);

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
}

void dtw_stg_init() {
    dtw_stg = new DtwStg;
}

void dtw_stg_load(bool hide) {
    if (hide)
        dtw_stg->HideWindow();
}

void dtw_stg_unload() {

}

void dtw_stg_free() {
    if (dtw_stg) {
        delete dtw_stg;
        dtw_stg = 0;
    }
}

static int stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2) {
    int32_t pv1 = ((stage_test_stage_pv*)src1)->pv_id;
    int32_t pv2 = ((stage_test_stage_pv*)src2)->pv_id;
    return pv1 > pv2 ? 1 : (pv1 < pv2 ? -1 : 0);
}
