/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_test.h"
#include "../../../KKdLib/io/path.h"
#include "../../../KKdLib/sort.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../KKdLib/vector.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/effect_group.h"
#include "../../../CRE/Glitter/file_reader.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/Glitter/scene.h"
#include "../../../CRE/data.h"
#include "../../../CRE/render_context.h"
#include "../../../CRE/stage.h"
#include "../../input.h"
#include "../data_test.h"
#include "../imgui_helper.h"

class data_test_stage_test_stage_pv {
public:
    int32_t pv_id;
    std::vector<std::int32_t> stage;

    data_test_stage_test_stage_pv(int32_t pv_id);
    ~data_test_stage_test_stage_pv();
};

class DtwStg : public TaskWindow {
public:
    int32_t pv_id;
    int32_t pv_index;
    int32_t ns_index;
    int32_t other_index;
    int32_t stage_index;

    int32_t stage_index_load;
    bool stage_load;

    std::vector<data_test_stage_test_stage_pv> stage_pv;
    std::vector<std::int32_t> stage_ns;
    std::vector<std::int32_t> stage_other;

    DtwStg();
    virtual ~DtwStg() override;
    virtual void Window() override;
};

extern int32_t width;
extern int32_t height;
extern render_context* rctx_ptr;
extern vec3 clear_color;

static const char* data_test_stage_test_window_title = "Stage Test##Data Test";

static DtwStg* dtw_stg;

static int data_test_stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2);
static bool stage_test_load();
static bool stage_test_unload();

bool data_test_stage_test_init(class_data* data, render_context* rctx) {
    return true;
}

void data_test_stage_test_ctrl(class_data* data) {
    if (dtw_stg->stage_load) {
        task_stage_set_stage_index(dtw_stg->stage_index_load);
        dtw_stg->stage_load = false;
    }
}

bool data_test_stage_test_hide(class_data* data) {
    stage_test_unload();

    enum_and(data->flags, ~CLASS_HIDE);
    enum_or(data->flags, CLASS_HIDDEN);
    return true;
}

void data_test_stage_test_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 240.0f);
    float_t h = min((float_t)height, 240.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_test_stage_test_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    std::vector<stage_data>& stg_data = rctx_ptr->data->data_ft.stage_data.stage_data;

    std::vector<data_test_stage_test_stage_pv>& stage_pv = dtw_stg->stage_pv;
    std::vector<std::int32_t>& stage_ns = dtw_stg->stage_ns;
    std::vector<std::int32_t>& stage_other = dtw_stg->stage_other;

    int32_t pv_id = dtw_stg->pv_id;

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%03d", pv_id);

    ImGui::Text("PV:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(48.0f);
    if (ImGui::BeginCombo("##PV:", buf, 0)) {
        for (data_test_stage_test_stage_pv& i : stage_pv) {
            ImGui::PushID(&i);
            sprintf_s(buf, sizeof(buf), "%03d", i.pv_id);
            if (ImGui::Selectable(buf, pv_id == i.pv_id)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && pv_id != i.pv_id))
                pv_id = i.pv_id;
            ImGui::PopID();

            if (pv_id == i.pv_id)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (pv_id != dtw_stg->pv_id) {
        dtw_stg->pv_index = -1;
        dtw_stg->pv_id = pv_id;
    }

    int32_t pv_index = dtw_stg->pv_index;

    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    bool pv_id_found = false;
    for (data_test_stage_test_stage_pv& i : stage_pv) {
        if (pv_id != i.pv_id)
            continue;

        if (ImGui::BeginCombo("##PV Index", pv_index > -1
            ? stg_data[i.stage[pv_index]].name.c_str() : "", 0)) {
            for (int32_t& j : i.stage) {
                int32_t pv_idx = (int32_t)(&j - i.stage.data());

                ImGui::PushID(j);
                if (ImGui::Selectable(stg_data[j].name.c_str(), pv_index == pv_idx)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && pv_index != pv_idx)) {
                    dtw_stg->pv_index = -1;
                    pv_index = pv_idx;
                }
                ImGui::PopID();

                if (pv_index == pv_idx)
                    ImGui::SetItemDefaultFocus();
            }
            data->imgui_focus |= ImGui::IsWindowFocused();
            ImGui::EndCombo();
        }

        if (pv_index != dtw_stg->pv_index) {
            dtw_stg->stage_load = true;
            dtw_stg->stage_index_load = i.stage[pv_index];
            dtw_stg->pv_index = pv_index;
        }
        pv_id_found = true;
        break;
    }

    if (!pv_id_found && ImGui::BeginCombo("##PV Index", "", 0)) {
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    int32_t ns_index = dtw_stg->ns_index;

    ImGui::Text("NS:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    if (ImGui::BeginCombo("##NS Index", ns_index > -1
        ? stg_data[(stage_ns)[ns_index]].name.c_str() : "", 0)) {
        for (int32_t& i : stage_ns) {
            int32_t ns_idx = (int32_t)(&i - stage_ns.data());

            ImGui::PushID(i);
            if (ImGui::Selectable(stg_data[stage_ns[i]].name.c_str(), ns_index == ns_idx)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && ns_index != ns_idx)) {
                dtw_stg->ns_index = -1;
                ns_index = ns_idx;
            }
            ImGui::PopID();

            if (ns_index == ns_idx)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (ns_index != dtw_stg->ns_index) {
        dtw_stg->stage_load = true;
        dtw_stg->stage_index_load = stage_ns[ns_index];
        dtw_stg->ns_index = ns_index;
    }

    int32_t other_index = dtw_stg->other_index;

    ImGui::Text("Other:");
    ImGui::SameLine(0.0f, 1.0f);
    ImGui::SetNextItemWidth(160.0f);
    if (ImGui::BeginCombo("##Other Index", other_index > -1
        ? stg_data[stage_other[other_index]].name.c_str() : "", 0)) {
        for (int32_t& i : stage_other) {
            int32_t other_idx = (int32_t)(&i - stage_other.data());

            ImGui::PushID(i);
            if (ImGui::Selectable(stg_data[i].name.c_str(), other_index == other_idx)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && other_index != other_idx)) {
                dtw_stg->other_index = -1;
                other_index = other_idx;
            }
            ImGui::PopID();

            if (other_index == other_idx)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (other_index != dtw_stg->other_index) {
        dtw_stg->stage_load = true;
        dtw_stg->stage_index_load = stage_other[other_index];
        dtw_stg->other_index = other_index;
    }

    int32_t stage_index = dtw_stg->stage_index;

    imguiGetContentRegionAvailSetNextItemWidth();
    if (ImGui::BeginCombo("##Stage Index", stage_index > -1
        ? stg_data[stage_index].name.c_str() : "", 0)) {
        for (stage_data& i : stg_data) {
            int32_t stage_idx = (int32_t)(&i - stg_data.data());

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.name.c_str(), stage_index == stage_idx)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && stage_index != stage_idx)) {
                dtw_stg->stage_index = -1;
                stage_index = stage_idx;
            }
            ImGui::PopID();

            if (stage_index == stage_idx)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    if (stage_index != dtw_stg->stage_index) {
        dtw_stg->stage_load = true;
        dtw_stg->stage_index_load = stage_index;
        dtw_stg->stage_index = stage_index;
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

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_test_stage_test_show(class_data* data) {
    stage_test_load();
    return true;
}

bool data_test_stage_test_dispose(class_data* data) {
    delete dtw_stg;

    data->flags = (class_flags)(CLASS_HIDDEN | CLASS_DISPOSED);
    data->imgui_focus = false;
    return true;
}

static int data_test_stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2) {
    int32_t pv1 = ((data_test_stage_test_stage_pv*)src1)->pv_id;
    int32_t pv2 = ((data_test_stage_test_stage_pv*)src2)->pv_id;
    return pv1 > pv2 ? 1 : (pv1 < pv2 ? -1 : 0);
}

static bool stage_test_load() {
    clear_color = { (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0) };

    camera* cam = rctx_ptr->camera;
    camera_reset(cam);
    vec3 view_point = { 0.0f, 0.88f, 4.3f };
    camera_set_view_point(cam, &view_point);
    vec3 interest = { 0.0f, 1.0f, 0.0f };
    camera_set_interest(cam, &interest);
    TaskWork::AppendTask(&dtm_stg, "DATA_TEST_STAGE");

    if (!dtw_stg)
        dtw_stg = new DtwStg();
    return true;
}

static bool stage_test_unload() {
    dtm_stg.SetDest();

    classes_data* c = &data_test_classes[DATA_TEST_STAGE_TEST];
    enum_or(c->data.flags, CLASS_HIDE);
    return true;
}
data_test_stage_test_stage_pv::data_test_stage_test_stage_pv(int32_t pv_id) {
    this->pv_id = pv_id;
}

data_test_stage_test_stage_pv::~data_test_stage_test_stage_pv() {

}

DtwStg::DtwStg() : pv_id(), pv_index(), ns_index(),
other_index(), stage_index(), stage_index_load(), stage_load() {
    std::vector<stage_data>& stg_data = rctx_ptr->data->data_ft.stage_data.stage_data;

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

            std::vector<data_test_stage_test_stage_pv>::iterator stg_pv = stage_pv.begin();
            while (stg_pv != stage_pv.end())
                if (pv_id == (stg_pv++)->pv_id)
                    break;

            if (stg_pv == stage_pv.end()) {
                stage_pv.push_back(data_test_stage_test_stage_pv(pv_id));
                stg_pv = stage_pv.end() - 1;
            }

            stg_pv->stage.push_back(i.id);
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

            stage_ns.push_back(i.id);
        }
        else
            stage_other.push_back(i.id);
    }

    if (stage_pv.size())
        quicksort_custom(stage_pv.data(), stage_pv.size(),
            sizeof(data_test_stage_test_stage_pv),
            data_test_stage_test_stage_pv_quicksort_compare_func);

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

void DtwStg::Window() {

}
