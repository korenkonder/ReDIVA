/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_test.h"
#include "../../../KKdLib/io/path.h"
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

typedef struct data_test_stage_test_stage_pv {
    int32_t pv_id;
    vector_string stage;
} data_test_stage_test_stage_pv;

vector(data_test_stage_test_stage_pv)

typedef struct data_test_stage_test {
    int32_t pv_id;
    int32_t pv_index;
    int32_t ns_index;
    int32_t other_index;
    int32_t stage_index;

    char* stage_name;
    bool stage_load;

    render_context* rctx;

    vector_data_test_stage_test_stage_pv stage_pv;
    vector_string stage_ns;
    vector_string stage_other;
} data_test_stage_test;

vector_func(data_test_stage_test_stage_pv)

extern int32_t width;
extern int32_t height;
extern bool input_locked;
extern stage stage_stgtst;
extern stage stage_test_data;

static const char* data_test_stage_test_window_title = "Stage Test##Data Test";

static void data_test_stage_test_stage_pv_free(data_test_stage_test_stage_pv* pv);

bool data_test_stage_test_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_test_stage_test));

    data_test_stage_test* stage_test = data->data;
    if (stage_test) {
        vector_stage_info* stg_info = &rctx->data->data_ft.stage_data.stage;

        stage_test->pv_id = -1;
        stage_test->pv_index = -1;
        stage_test->ns_index = -1;
        stage_test->other_index = -1;
        stage_test->stage_index = -1;

        stage_test->stage_name = 0;
        stage_test->stage_load = false;

        stage_test->rctx = rctx;

        vector_data_test_stage_test_stage_pv* stage_pv = &stage_test->stage_pv;
        vector_string* stage_ns = &stage_test->stage_ns;
        vector_string* stage_other = &stage_test->stage_other;

        *stage_pv = vector_empty(data_test_stage_test_stage_pv);
        *stage_ns = vector_empty(string);
        *stage_other = vector_empty(string);

        for (stage_info* i = stg_info->begin; i != stg_info->end; i++) {
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
                    ret = sscanf_s(string_data(&i->name) + 8, "%03d", &pv_id);

                if (ret != 1)
                    continue;

                data_test_stage_test_stage_pv* stg_pv = 0;
                for (stg_pv = stage_pv->begin; stg_pv != stage_pv->end; stg_pv++)
                    if (pv_id == stg_pv->pv_id)
                        break;

                if (stg_pv == stage_pv->end) {
                    stg_pv = vector_data_test_stage_test_stage_pv_reserve_back(stage_pv);
                    stg_pv->pv_id = pv_id;
                    stg_pv->stage = vector_empty(string);
                }

                string stg;
                string_copy(&i->name, &stg);
                vector_string_push_back(&stg_pv->stage, &stg);
            }
            else if (stgns || stgd2ns) {
                int32_t ns_id = 0;
                int32_t ret;
                if (stgns)
                    ret = sscanf_s(string_data(&i->name) + 5, "%03d", &ns_id);
                else
                    ret = sscanf_s(string_data(&i->name) + 8, "%03d", &ns_id);

                if (ret != 1)
                    continue;

                string stg;
                string_copy(&i->name, &stg);
                vector_string_push_back(stage_ns, &stg);
            }
            else {
                string stg;
                string_copy(&i->name, &stg);
                vector_string_push_back(stage_other, &stg);
            }
        }

        for (data_test_stage_test_stage_pv* i = stage_pv->begin; i != &stage_pv->end[-1]; i++)
            for (data_test_stage_test_stage_pv* j = i + 1; j != stage_pv->end; j++)
                if (i->pv_id > j->pv_id) {
                    data_test_stage_test_stage_pv temp = *i;
                    *i = *j;
                    *j = temp;
                }

        if (stage_pv->end - stage_pv->begin > 0)
            stage_test->pv_id = stage_pv->begin[0].pv_id;

        stage* stg = rctx->stage;
        if ((stg && !stg->modern && stg->stage) || !stg) {
            if (!stg) {
                stg = &stage_stgtst;
                stage_set(&stage_stgtst, rctx);
            }

            stage_info* info = stg->stage;
            if (info - stg_info->begin >= 0 && info - stg_info->begin < stg_info->end - stg_info->begin) {
                stage_test->stage_index = (int32_t)(info - stg_info->begin);
                stage_test->stage_name = string_data(&info->name);
            }
        }
    }
    return true;
}

bool data_test_stage_test_hide(class_data* data) {
    data_test_stage_test* stage_test = data->data;
    if (!stage_test)
        return false;

    lock_data_free(&stage_data_lock, data_test_stage_test_hide);

    data->flags &= ~CLASS_HIDE;
    data->flags |= CLASS_HIDDEN;
    return true;
}

void data_test_stage_test_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 240.0f);
    float_t h = min((float_t)height, 240.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_test_stage_test_window_title, &open, window_flags);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    data_test_stage_test* stage_test = data->data;

    vector_stage_info* stg_info = &stage_test->rctx->data->data_ft.stage_data.stage;

    vector_data_test_stage_test_stage_pv* stage_pv = &stage_test->stage_pv;
    vector_string* stage_ns = &stage_test->stage_ns;
    vector_string* stage_other = &stage_test->stage_other;

    int32_t pv_id = stage_test->pv_id;

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%03d", pv_id);

    igText("PV:");
    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(48.0f);
    if (igBeginCombo("##PV:", buf, 0)) {
        for (data_test_stage_test_stage_pv* i = stage_pv->begin; i != stage_pv->end; i++) {
            igPushID_Ptr(i);
            sprintf_s(buf, sizeof(buf), "%03d", i->pv_id);
            if (igSelectable_Bool(buf, pv_id == i->pv_id, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && pv_id != i->pv_id))
                pv_id = i->pv_id;
            igPopID();

            if (pv_id == i->pv_id)
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
    for (data_test_stage_test_stage_pv* i = stage_pv->begin; i != stage_pv->end; i++) {
        if (pv_id != i->pv_id)
            continue;

        if (igBeginCombo("##PV Index", pv_index > -1
            ? string_data(&i->stage.begin[pv_index]) : "", 0)) {
            for (string* j = i->stage.begin; j != i->stage.end; j++) {
                int32_t pv_idx = (int32_t)(j - i->stage.begin);

                igPushID_Ptr(j);
                if (igSelectable_Bool(string_data(j), pv_index == pv_idx, 0, ImVec2_Empty)
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
            stage_test->stage_name = string_data(&i->stage.begin[pv_index]);
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
        ? string_data(&stage_ns->begin[ns_index]) : "", 0)) {
        for (string* i = stage_ns->begin; i != stage_ns->end; i++) {
            int32_t ns_idx = (int32_t)(i - stage_ns->begin);

            igPushID_Ptr(i);
            if (igSelectable_Bool(string_data(i), ns_index == ns_idx, 0, ImVec2_Empty)
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
        stage_test->stage_name = string_data(&stage_ns->begin[ns_index]);
        stage_test->ns_index = ns_index;
    }

    int32_t other_index = stage_test->other_index;

    igText("Other:");
    igSameLine(0.0f, 1.0f);
    igSetNextItemWidth(160.0f);
    if (igBeginCombo("##Other Index", other_index > -1
        ? string_data(&stage_other->begin[other_index]) : "", 0)) {
        for (string* i = stage_other->begin; i != stage_other->end; i++) {
            int32_t other_idx = (int32_t)(i - stage_other->begin);

            igPushID_Ptr(i);
            if (igSelectable_Bool(string_data(i), other_index == other_idx, 0, ImVec2_Empty)
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
        stage_test->stage_name = string_data(&stage_other->begin[other_index]);
        stage_test->other_index = other_index;
    }

    int32_t stage_index = stage_test->stage_index;

    ImVec2 t;
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    if (igBeginCombo("##Stage Index", stage_index > -1
        ? string_data(&stg_info->begin[stage_index].name) : "", 0)) {
        for (stage_info* i = stg_info->begin; i != stg_info->end; i++) {
            int32_t stage_idx = (int32_t)(i - stg_info->begin);

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
        stage_test->stage_name = string_data(&stg_info->begin[stage_index].name);
        stage_test->stage_index = stage_index;
    }

    stage* stg = stage_test->rctx->stage;
    if (!stg)
        if (stage_test_data.stage)
            stg = &stage_test_data;
        else
            stg = &stage_stgtst;

    igCheckbox("Stage display", &stg->display);
    igCheckbox("[Ring]", &stg->ring);
    igCheckbox("[Ground]", &stg->ground);
    igCheckbox("[Sky]", &stg->sky);
    igCheckbox("Effects display", &stg->effects);

    data->imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
}

void data_test_stage_test_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

void data_test_stage_test_render(class_data* data) {
    data_test_stage_test* stage_test = data->data;

    if (stage_test->stage_load) {
        render_context* rctx = stage_test->rctx;
        stage_free(&stage_test_data, rctx);

        data_struct* aft_data = rctx->data;
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        stage_init(&stage_test_data);
        stage_load(&stage_test_data, aft_data, aft_auth_3d_db,
            aft_obj_db, aft_tex_db, aft_stage_data, stage_test->stage_name, rctx);
        stage_set(&stage_test_data, rctx);
        stage_test->stage_load = false;
    }
}

bool data_test_stage_test_show(class_data* data) {
    data_test_stage_test* stage_test = data->data;
    if (!stage_test)
        return false;

    bool ret = false;
    lock_trylock(&pv_lock);
    if (!lock_data_init(&stage_data_lock, &data->lock, data, data_test_stage_test_hide))
        goto End;

    ret = true;

End:
    lock_unlock(&pv_lock);
    return ret;
}

bool data_test_stage_test_dispose(class_data* data) {
    lock_data_free(&stage_data_lock, data_test_stage_test_dispose);

    data_test_stage_test* stage_test = data->data;
    if (stage_test) {
        vector_data_test_stage_test_stage_pv_free(&stage_test->stage_pv,
            data_test_stage_test_stage_pv_free);
        vector_string_free(&stage_test->stage_ns, string_free);
        vector_string_free(&stage_test->stage_other, string_free);
    }
    free(data->data);

    data->flags = CLASS_HIDDEN | CLASS_DISPOSED;
    data->imgui_focus = false;
    return true;
}

static void data_test_stage_test_stage_pv_free(data_test_stage_test_stage_pv* pv) {
    vector_string_free(&pv->stage, string_free);
}
