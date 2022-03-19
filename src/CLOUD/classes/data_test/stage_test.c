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
#include "../imgui_helper.h"

class data_test_stage_test_stage_pv {
public:
    int32_t pv_id;
    std::vector<std::int32_t> stage;

    data_test_stage_test_stage_pv(int32_t pv_id);
    ~data_test_stage_test_stage_pv();
};

class data_test_stage_test {
public:
    int32_t pv_id;
    int32_t pv_index;
    int32_t ns_index;
    int32_t other_index;
    int32_t stage_index;

    int32_t stage_index_load;
    bool stage_load;

    render_context* rctx;

    std::vector<data_test_stage_test_stage_pv> stage_pv;
    std::vector<std::int32_t> stage_ns;
    std::vector<std::int32_t> stage_other;

    data_test_stage_test();
    ~data_test_stage_test();
};

extern int32_t width;
extern int32_t height;

static const char* data_test_stage_test_window_title = "Stage Test##Data Test";

static int data_test_stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2);

bool data_test_stage_test_init(class_data* data, render_context* rctx) {
    data->data = new data_test_stage_test;

    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;
    if (stage_test) {
        vector_old_stage_data* stg_data = &rctx->data->data_ft.stage_data.stage_data;

        stage_test->pv_id = -1;
        stage_test->pv_index = -1;
        stage_test->ns_index = -1;
        stage_test->other_index = -1;
        stage_test->stage_index = -1;

        stage_test->stage_index_load = -1;
        stage_test->stage_load = false;

        stage_test->rctx = rctx;

        size_t stg_pv_count = 0;
        size_t stg_ns_count = 0;
        size_t stg_other_count = 0;
        for (stage_data* i = stg_data->begin; i != stg_data->end; i++) {
            bool stgpv = false;
            bool stgd2pv = false;
            bool stgns = false;
            bool stgd2ns = false;
            if (i->name.length >= 8 && !memcmp(string_data(&i->name), "STGPV", 5))
                stg_pv_count++;
            else if (i->name.length >= 10 && !memcmp(string_data(&i->name), "STGD2PV", 7))
                stg_pv_count++;
            else if (i->name.length >= 8 && !memcmp(string_data(&i->name), "STGNS", 5))
                stg_ns_count++;
            else if (i->name.length >= 10 && !memcmp(string_data(&i->name), "STGD2NS", 7))
                stg_ns_count++;
            else
                stg_other_count++;
        }

        std::vector<data_test_stage_test_stage_pv> stage_pv;
        std::vector<std::int32_t> stage_ns;
        std::vector<std::int32_t> stage_other;

        stage_pv.reserve(stg_pv_count);
        stage_ns.reserve(stg_ns_count);
        stage_other.reserve(stg_other_count);

        for (stage_data* i = stg_data->begin; i != stg_data->end; i++) {
            bool stgpv = false;
            bool stgd2pv = false;
            bool stgns = false;
            bool stgd2ns = false;
            if (i->name.length >= 8 && !memcmp(string_data(&i->name), "STGPV", 5))
                stgpv = true;
            else if (i->name.length >= 10 && !memcmp(string_data(&i->name), "STGD2PV", 7))
                stgd2pv = true;
            else if (i->name.length >= 8 && !memcmp(string_data(&i->name), "STGNS", 5))
                stgns = true;
            else if (i->name.length >= 10 && !memcmp(string_data(&i->name), "STGD2NS", 7))
                stgd2ns = true;

            if (stgpv || stgd2pv) {
                int32_t pv_id = 0;
                int32_t ret;
                if (stgpv)
                    ret = sscanf_s(string_data(&i->name) + 5, "%03d", &pv_id);
                else
                    ret = sscanf_s(string_data(&i->name) + 7, "%03d", &pv_id);

                if (ret != 1)
                    continue;

                data_test_stage_test_stage_pv* stg_pv = 0;
                for (stg_pv = stage_pv.begin()._Ptr; stg_pv != stage_pv.end()._Ptr; stg_pv++)
                    if (pv_id == stg_pv->pv_id)
                        break;

                if (stg_pv == stage_pv.end()._Ptr) {
                    stage_pv.push_back(data_test_stage_test_stage_pv(pv_id));
                    stg_pv = &stage_pv.end()[-1];
                }

                stg_pv->stage.push_back(i->id);
            }
            else if (stgns || stgd2ns) {
                int32_t ns_id = 0;
                int32_t ret;
                if (stgns)
                    ret = sscanf_s(string_data(&i->name) + 5, "%03d", &ns_id);
                else
                    ret = sscanf_s(string_data(&i->name) + 7, "%03d", &ns_id);

                if (ret != 1)
                    continue;

                stage_ns.push_back(i->id);
            }
            else {
                std::string stg = std::string(string_data(&i->name), i->name.length);
                stage_other.push_back(i->id);
            }
        }

        if (stage_pv.size())
            quicksort_custom(stage_pv.data(), stage_pv.size(),
                sizeof(data_test_stage_test_stage_pv),
                data_test_stage_test_stage_pv_quicksort_compare_func);

        if (stage_pv.size())
            stage_test->pv_id = stage_pv[0].pv_id;

        stage_test->stage_pv = stage_pv;
        stage_test->stage_ns = stage_ns;
        stage_test->stage_other = stage_other;

        stage_test->stage_index = 0;
        stage_test->stage_load = false;
        stage_test->stage_index_load = 0;
    }
    return true;
}

void data_test_stage_test_ctrl(class_data* data) {
    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;

    if (stage_test->stage_load) {
        task_stage_set_stage_index(stage_test->stage_index_load);
        stage_test->stage_load = false;
    }
}

bool data_test_stage_test_hide(class_data* data) {
    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;
    if (!stage_test)
        return false;

    task_stage_set_stage_index(-1);
    task_stage_unload();

    enum_and(data->flags, ~CLASS_HIDE);
    enum_or(data->flags, CLASS_HIDDEN);
    return true;
}

void data_test_stage_test_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 240.0f);
    float_t h = min((float_t)height, 240.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_test_stage_test_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        igEnd();
        return;
    }
    else if (collapsed) {
        igEnd();
        return;
    }

    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;

    vector_old_stage_data* stg_data = &stage_test->rctx->data->data_ft.stage_data.stage_data;

    std::vector<data_test_stage_test_stage_pv>& stage_pv = stage_test->stage_pv;
    std::vector<std::int32_t>& stage_ns = stage_test->stage_ns;
    std::vector<std::int32_t>& stage_other = stage_test->stage_other;

    int32_t pv_id = stage_test->pv_id;

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%03d", pv_id);

    igText("PV:");
    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(48.0f);
    if (igBeginCombo("##PV:", buf, 0)) {
        for (data_test_stage_test_stage_pv& i : stage_pv) {
            igPushID_Ptr(&i);
            sprintf_s(buf, sizeof(buf), "%03d", i.pv_id);
            if (igSelectable_Bool(buf, pv_id == i.pv_id, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && pv_id != i.pv_id))
                pv_id = i.pv_id;
            igPopID();

            if (pv_id == i.pv_id)
                igSetItemDefaultFocus();
        }

        data->imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
    }

    if (pv_id != stage_test->pv_id) {
        stage_test->pv_index = -1;
        stage_test->pv_id = pv_id;
    }

    int32_t pv_index = stage_test->pv_index;

    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(160.0f);
    bool pv_id_found = false;
    for (data_test_stage_test_stage_pv& i : stage_pv) {
        if (pv_id != i.pv_id)
            continue;

        if (igBeginCombo("##PV Index", pv_index > -1
            ? string_data(&stg_data->begin[i.stage[pv_index]].name) : "", 0)) {
            for (int32_t& j : i.stage) {
                int32_t pv_idx = (int32_t)(&j - i.stage.data());

                igPushID_Int(j);
                if (igSelectable_Bool(string_data(&stg_data->begin[j].name),
                        pv_index == pv_idx, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && pv_index != pv_idx)) {
                    stage_test->pv_index = -1;
                    pv_index = pv_idx;
                }
                igPopID();

                if (pv_index == pv_idx)
                    igSetItemDefaultFocus();
            }
            data->imgui_focus |= igIsWindowFocused(0);
            igEndCombo();
        }

        if (pv_index != stage_test->pv_index) {
            stage_test->stage_load = true;
            stage_test->stage_index_load = i.stage[pv_index];
            stage_test->pv_index = pv_index;
        }
        pv_id_found = true;
        break;
    }
    
    if (!pv_id_found && igBeginCombo("##PV Index", "", 0)) {
        data->imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
    }

    int32_t ns_index = stage_test->ns_index;

    igText("NS:");
    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(160.0f);
    if (igBeginCombo("##NS Index", ns_index > -1
        ? string_data(&stg_data->begin[(stage_ns)[ns_index]].name) : "", 0)) {
        for (int32_t& i : stage_ns) {
            int32_t ns_idx = (int32_t)(&i - stage_ns.data());

            igPushID_Int(i);
            if (igSelectable_Bool(string_data(&stg_data->begin[stage_ns[i]].name),
                ns_index == ns_idx, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && ns_index != ns_idx)) {
                stage_test->ns_index = -1;
                ns_index = ns_idx;
            }
            igPopID();

            if (ns_index == ns_idx)
                igSetItemDefaultFocus();
        }

        data->imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
    }

    if (ns_index != stage_test->ns_index) {
        stage_test->stage_load = true;
        stage_test->stage_index_load = stage_ns[ns_index];
        stage_test->ns_index = ns_index;
    }

    int32_t other_index = stage_test->other_index;

    igText("Other:");
    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(160.0f);
    if (igBeginCombo("##Other Index", other_index > -1
        ? string_data(&stg_data->begin[stage_other[ns_index]].name) : "", 0)) {
        for (int32_t& i : stage_other) {
            int32_t other_idx = (int32_t)(&i - stage_other.data());

            igPushID_Int(i);
            if (igSelectable_Bool(string_data(&stg_data->begin[stage_other[i]].name),
                other_index == other_idx, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && other_index != other_idx)) {
                stage_test->other_index = -1;
                other_index = other_idx;
            }
            igPopID();

            if (other_index == other_idx)
                igSetItemDefaultFocus();
        }

        data->imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
    }

    if (other_index != stage_test->other_index) {
        stage_test->stage_load = true;
        stage_test->stage_index_load = stage_other[other_index];
        stage_test->other_index = other_index;
    }

    int32_t stage_index = stage_test->stage_index;

    ImVec2 t;
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    if (igBeginCombo("##Stage Index", stage_index > -1
        ? string_data(&stg_data->begin[stage_index].name) : "", 0)) {
        for (stage_data* i = stg_data->begin; i != stg_data->end; i++) {
            int32_t stage_idx = (int32_t)(i - stg_data->begin);

            igPushID_Ptr(i);
            if (igSelectable_Bool(string_data(&i->name), stage_index == stage_idx, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && stage_index != stage_idx)) {
                stage_test->stage_index = -1;
                stage_index = stage_idx;
            }
            igPopID();

            if (stage_index == stage_idx)
                igSetItemDefaultFocus();
        }

        data->imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
    }

    if (stage_index != stage_test->stage_index) {
        stage_test->stage_load = true;
        stage_test->stage_index_load = stage_index;
        stage_test->stage_index = stage_index;
    }

    stage* stg = task_stage_get_current_stage();
    if (stg) {
        igCheckbox("Stage display", &stg->stage_display);
        igCheckbox("[Ring]", &stg->ring);
        igCheckbox("[Ground]", &stg->ground);
        igCheckbox("[Sky]", &stg->sky);
        igCheckbox("Effects display", &stg->effects);

    }
    else {
        bool stage_display = true;
        igCheckbox("Stage display", &stage_display);
        bool ring = true;
        igCheckbox("[Ring]", &ring);
        bool ground = true;
        igCheckbox("[Ground]", &ground);
        bool sky = true;
        igCheckbox("[Sky]", &sky);
        bool effects_display = true;
        igCheckbox("Effects display", &effects_display);
    }

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

bool data_test_stage_test_show(class_data* data) {
    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;
    if (!stage_test)
        return false;

    task_stage_load("DATA_TEST_STG_STAGE");
    task_stage_set_stage_index(stage_test->stage_index);

    return true;
}

bool data_test_stage_test_dispose(class_data* data) {
    data_test_stage_test* stage_test = (data_test_stage_test*)data->data;
    delete stage_test;

    data->flags = (class_flags)(CLASS_HIDDEN | CLASS_DISPOSED);
    data->imgui_focus = false;
    return true;
}

static int data_test_stage_test_stage_pv_quicksort_compare_func(void const* src1, void const* src2) {
    int32_t pv1 = ((data_test_stage_test_stage_pv*)src1)->pv_id;
    int32_t pv2 = ((data_test_stage_test_stage_pv*)src2)->pv_id;
    return pv1 > pv2 ? 1 : (pv1 < pv2 ? -1 : 0);
}

data_test_stage_test_stage_pv::data_test_stage_test_stage_pv(int32_t pv_id) {
    this->pv_id = pv_id;
}

data_test_stage_test_stage_pv::~data_test_stage_test_stage_pv() {

}

data_test_stage_test::data_test_stage_test() : pv_id(), pv_index(), ns_index(),
other_index(), stage_index(), stage_index_load(), stage_load(), rctx() {

}

data_test_stage_test::~data_test_stage_test() {

}
