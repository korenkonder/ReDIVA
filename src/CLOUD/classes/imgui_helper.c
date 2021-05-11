/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "imgui_helper.h"
#include "../../KKdLib/str_utils.h"

const ImVec1 ImVec1_Empty = { 0.0f };
const ImVec2 ImVec2_Empty = { 0.0f, 0.0f };
const ImVec4 ImVec4_Empty = { 0.0f, 0.0f, 0.0f, 0.0f };
const ImVec1 ImVec1_Identity = { 1.0f };
const ImVec2 ImVec2_Identity = { 1.0f, 1.0f };
const ImVec4 ImVec4_Identity = { 1.0f, 1.0f, 1.0f, 1.0f };
const float_t imgui_alpha_disabled_scale = 0.5f;

static float_t column_space = (float_t)(1.0 / 3.0);
static float_t cell_padding;

inline bool imguiItemKeyDown(int32_t key) {
    return igIsItemFocused() && igIsKeyDown(key);
}

inline bool imguiItemKeyPressed(int32_t key, bool repeat) {
    return igIsItemFocused() && igIsKeyPressed(key, repeat);
}

inline bool imguiItemKeyReleased(int32_t key) {
    return igIsItemFocused() && igIsKeyReleased(key);
}

inline float_t imguiGetColumnSpace() {
    return column_space;
}

inline void imguiSetColumnSpace(float_t val) {
    column_space = val;
}

inline void imguiSetDefaultColumnSpace() {
    column_space = (float_t)(1.0 / 3.0);
}

inline void imguiDisableElementPush(bool enable) {
    if (!enable) {
        igPushItemFlag(ImGuiItemFlags_Disabled, true);
        igPushStyleVar_Float(ImGuiStyleVar_Alpha, igGetStyle()->Alpha * imgui_alpha_disabled_scale);
    }
}

inline void imguiDisableElementPop(bool enable) {
    if (!enable) {
        igPopItemFlag();
        igPopStyleVar(1);
    }
}

const char* imguiStartPropertyColumn(const char* label) {
    igPushID_Str(label);

    ImVec2 t;
    igGetContentRegionAvail(&t);
    igBeginTable("table", 2, 0, ImVec2_Empty, 0.0f);
    igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, t.x * column_space, 0);

    char* label_temp = str_utils_add((char*)label, "");
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    igTableNextColumn();
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    igText(label_temp);
    free(label_temp);

    igTableNextColumn();
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    return (const char*)str_utils_add("##", (char*)label);
}

void imguiEndPropertyColumn(const char* temp) {
    igEndTable();
    igPopID();
    free((void*)temp);
}

bool imguiButton(const char* label, const ImVec2 size) {
    return igButton(label, size) || imguiItemKeyPressed(GLFW_KEY_ENTER, true);
}

bool imguiButtonEx(const char* label, const ImVec2 size, ImGuiButtonFlags flags) {
    return igButtonEx(label, size, flags) || imguiItemKeyPressed(GLFW_KEY_ENTER, true);
}

bool imguiCheckbox(const char* label, bool* v) {
    if (igCheckbox(label, v))
        return true;
    else if (imguiItemKeyPressed(GLFW_KEY_ENTER, true)) {
        *v ^= true;
        return true;
    }
    else
        return false;
}

bool imguiCheckboxFlags_IntPtr(const char* label, int* flags, int flags_value) {
    if (igCheckboxFlags_IntPtr(label, flags, flags_value))
        return true;
    else if (imguiItemKeyPressed(GLFW_KEY_ENTER, true)) {
        *flags ^= flags_value;
        return true;
    }
    else
        return false;
}

bool imguiCheckboxFlags_UintPtr(const char* label, unsigned int* flags, unsigned int flags_value) {
    if (igCheckboxFlags_UintPtr(label, flags, flags_value))
        return true;
    else if (imguiItemKeyPressed(GLFW_KEY_ENTER, true)) {
        *flags ^= flags_value;
        return true;
    }
    else
        return false;
}

static bool igDragFloatDisable(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igDragFloat("", val, speed, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igDragIntDisable(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igDragInt("", val, speed, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igSliderFloatDisable(const char* label, float_t* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igSliderFloat(label, val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igSliderIntDisable(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igSliderInt(label, val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static const char* startPropertyColumnDisable(const char* label, bool enable) {
    igPushID_Str(label);

    ImVec2 t;
    igGetContentRegionAvail(&t);
    igBeginTable("table", 2, 0, ImVec2_Empty, 0.0f);
    igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, t.x * column_space, 0);

    char* label_temp = str_utils_add((char*)label, "");
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    igTableNextColumn();
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    imguiDisableElementPush(enable);
    igText(label_temp);
    imguiDisableElementPop(enable);
    free(label_temp);

    igTableNextColumn();
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
    return (const char*)str_utils_add("##", (char*)label);
}

bool imguiComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (igBeginCombo(label, "None", flags))
            igEndCombo();
        imguiDisableElementPop(false);
        return false;
    }

    if (igBeginCombo(label, items[*selected_idx], flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool(items[n], *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool(items[n], *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }

        if (focus)
            *focus |= igIsWindowFocused(0);
        igEndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags) {
    float_t v[3];
    *(vec3*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igColorEdit3(temp_label, v, flags);
    imguiEndPropertyColumn(temp_label);
    if (res)
        *val = *(vec3*)v;
    return res;
}

bool imguiColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags) {
    float_t v[4];
    *(vec4*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igColorEdit4(temp_label, v, flags);
    imguiEndPropertyColumn(temp_label);
    if (res)
        *val = *(vec4*)v;
    return res;
}

bool imguiColumnComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = imguiComboBox(temp_label, items, size, selected_idx, flags, include_last, focus);
    imguiEndPropertyColumn(temp_label);
    return res;
}

bool imguiColumnDragFloat(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragFloat(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnDragVec2(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[2];
    *(vec2*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragFloat2(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2*)v;
    return true;
}

bool imguiColumnDragVec3(const char* label, vec3* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[3];
    *(vec3*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragFloat3(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3*)v;
    return true;
}

bool imguiColumnDragVec4(const char* label, vec4* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[4];
    *(vec4*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragFloat4(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4*)v;
    return true;
}

bool imguiColumnDragInt(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragInt(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnDragVec2I(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[2];
    *(vec2i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragInt2(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2i*)v;
    return true;
}

bool imguiColumnDragVec3I(const char* label, vec3i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[3];
    *(vec3i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragInt3(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3i*)v;
    return true;
}

bool imguiColumnDragVec4I(const char* label, vec4i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[4];
    *(vec4i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igDragInt4(temp_label, v, speed, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4i*)v;
    return true;
}

bool imguiColumnInputText(const char* label, char* buf, size_t buf_size,
    ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igInputText(temp_label, buf, buf_size, flags, callback, user_data);
    imguiEndPropertyColumn(temp_label);
    return res;
}

bool imguiColumnSliderFloat(const char* label, float_t* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderFloat(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnSliderVec2(const char* label, vec2* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[2];
    *(vec2*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderFloat2(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2*)v;
    return true;
}

bool imguiColumnSliderVec3(const char* label, vec3* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[3];
    *(vec3*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderFloat3(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3*)v;
    return true;
}

bool imguiColumnSliderVec4(const char* label, vec4* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[4];
    *(vec4*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderFloat4(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4*)v;
    return true;
}

bool imguiColumnSliderInt(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderInt(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnSliderVec2I(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[2];
    *(vec2i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderInt2(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2i*)v;
    return true;
}

bool imguiColumnSliderVec3I(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[3];
    *(vec3i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderInt3(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3i*)v;
    return true;
}

bool imguiColumnSliderVec4I(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[4];
    *(vec4i*)v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igSliderInt4(temp_label, v, min, max, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4i*)v;
    return true;
}

bool imguiColumnInputFloat(const char* label, float_t* val,
    float_t step, float_t step_fast, const char* format, ImGuiInputTextFlags flags) {
    float_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igInputFloat(temp_label, v, step, step_fast, format, flags);
    imguiEndPropertyColumn(temp_label);
    if (res)
        *val = *v;
    return res;
}

bool imguiColumnInputInt(const char* label, int32_t* val,
    int32_t step, int32_t step_fast, ImGuiInputTextFlags flags) {
    int32_t v[1];
    *v = *val;
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igInputInt(temp_label, v, step, step_fast, flags);
    imguiEndPropertyColumn(temp_label);
    if (res)
        *val = *v;
    return res;
}

bool imguiColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
    const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
    const char* temp_label = imguiStartPropertyColumn(label);
    bool res = igInputScalar(temp_label, data_type, p_data, p_step, p_step_fast, format, flags);
    imguiEndPropertyColumn(temp_label);
    return res;
}

bool imguiColumnDragFloatFlag(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[1];
    *v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnDragVec2Flag(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[2];
    *(vec2*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2*)v;
    return true;
}

bool imguiColumnDragVec3Flag(const char* label, vec3* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[3];
    *(vec3*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3*)v;
    return true;
}

bool imguiColumnDragVec4Flag(const char* label, vec4* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[4];
    *(vec4*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4*)v;
    return true;
}

bool imguiColumnDragIntFlag(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[1];
    *v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnDragVec2IFlag(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[2];
    *(vec2i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2i*)v;
    return true;
}

bool imguiColumnDragVec3IFlag(const char* label, vec3i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[3];
    *(vec3i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3i*)v;
    return true;
}

bool imguiColumnDragVec4IFlag(const char* label, vec4i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[4];
    *(vec4i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4i*)v;
    return true;
}

bool imguiColumnSliderFloatFlag(const char* label, float_t* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[1];
    *v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnSliderVec2Flag(const char* label, vec2* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[2];
    *(vec2*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2*)v;
    return true;
}

bool imguiColumnSliderVec3Flag(const char* label, vec3* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[3];
    *(vec3*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3*)v;
    return true;
}

bool imguiColumnSliderVec4Flag(const char* label, vec4* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[4];
    *(vec4*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4*)v;
    return true;
}

bool imguiColumnSliderIntFlag(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[1];
    *v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    *v = clamp(*v, min, max);
    if (*v == *val)
        return false;

    *val = *v;
    return true;
}

bool imguiColumnSliderVec2IFlag(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[2];
    *(vec2i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2i*)v;
    return true;
}

bool imguiColumnSliderVec3IFlag(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[3];
    *(vec3i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3i*)v;
    return true;
}

bool imguiColumnSliderVec4IFlag(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[4];
    *(vec4i*)v = *val;
    const char* temp_label = startPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    igPopID();
    igEndGroup();
    imguiEndPropertyColumn(temp_label);
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    v[2] = clamp(v[2], min, max);
    v[3] = clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4i*)v;
    return true;
}
