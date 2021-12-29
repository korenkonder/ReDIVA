/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "imgui_helper.h"
#include "../../KKdLib/str_utils.h"
#include "../../CRE/data.h"

const ImVec1 ImVec1_Empty = { 0.0f };
const ImVec2 ImVec2_Empty = { 0.0f, 0.0f };
const ImVec4 ImVec4_Empty = { 0.0f, 0.0f, 0.0f, 0.0f };
const ImVec1 ImVec1_Identity = { 1.0f };
const ImVec2 ImVec2_Identity = { 1.0f, 1.0f };
const ImVec4 ImVec4_Identity = { 1.0f, 1.0f, 1.0f, 1.0f };
const float_t imgui_alpha_disabled_scale = 0.5f;

static float_t column_space = (float_t)(1.0 / 3.0);
static float_t cell_padding;

#undef min
#undef max

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

inline float_t imguiGetContentRegionAvailWidth() {
    ImVec2 t;
    igGetContentRegionAvail(&t);
    return t.x;
}

inline float_t imguiGetContentRegionAvailHeight() {
    ImVec2 t;
    igGetContentRegionAvail(&t);
    return t.y;
}

inline void imguiGetContentRegionAvailSetNextItemWidth() {
    ImVec2 t;
    igGetContentRegionAvail(&t);
    igSetNextItemWidth(t.x);
}

void imguiStartPropertyColumn(const char* label) {
    igPushID_Str(label);

    float_t w = imguiGetContentRegionAvailWidth();
    igBeginTable("table", 2, 0, ImVec2_Empty, 0.0f);
    igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

    char* label_temp = str_utils_copy((char*)label);
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    igTableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    igText(label_temp);
    free(label_temp);

    igTableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
}

void imguiEndPropertyColumn() {
    igEndTable();
    igPopID();
}

bool igSliderFloatButton(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t button_size = igGetFont()->FontSize + igGetStyle()->FramePadding.y * 2.0f;
    ImGuiIO* io = igGetIO();

    float_t key_repeat_delay = io->KeyRepeatDelay;
    float_t key_repeat_rate = io->KeyRepeatRate;
    io->KeyRepeatDelay = (float_t)(30.0 / 60.0);
    io->KeyRepeatRate = (float_t)(10.0 / 60.0);

    ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat;
    if (flags & ImGuiSliderFlags_NoInput)
        igPushItemFlag(ImGuiItemFlags_Disabled, true);

    float_t v = *val;
    float_t w = igCalcItemWidth();
    igPushID_Str(label);
    igBeginGroup();
    igPushButtonRepeat(true);
    igButtonEx("<", (ImVec2) { button_size, button_size }, ImGuiButtonFlags_Repeat);
    bool l = igIsItemActive() && (igIsKeyPressed(VK_RETURN, true)
        || igIsMouseClicked(ImGuiMouseButton_Left, true));
    igSameLine(0.0f, 0.0f);
    igSetNextItemWidth(w - button_size * 2.0f);
    igSliderScalar(label, ImGuiDataType_Float, &v, &min, &max, format, flags);
    igSameLine(0.0f, 0.0f);
    igButtonEx(">", (ImVec2) { button_size, button_size }, ImGuiButtonFlags_Repeat);
    bool r = igIsItemActive() && (igIsKeyPressed(VK_RETURN, true)
        || igIsMouseClicked(ImGuiMouseButton_Left, true));
    igPopButtonRepeat();
    igEndGroup();
    igPopID();
    if (flags & ImGuiSliderFlags_NoInput)
        igPopItemFlag();

    io->KeyRepeatDelay = key_repeat_delay;
    io->KeyRepeatRate = key_repeat_rate;

    if (l) {
        v -= step;
        if (v < min)
            v = min;
    }
    else if (r) {
        v += step;
        if (v > max)
            v = max;
    }

    bool res = *val != v ? true : false;
    *val = v;
    return res;
}

bool igSliderIntButton(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    float_t button_size = igGetFont()->FontSize + igGetStyle()->FramePadding.y * 2.0f;
    ImGuiIO* io = igGetIO();

    float_t key_repeat_delay = io->KeyRepeatDelay;
    float_t key_repeat_rate = io->KeyRepeatRate;
    io->KeyRepeatDelay = (float_t)(30.0 / 60.0);
    io->KeyRepeatRate = (float_t)(10.0 / 60.0);

    int32_t v = *val;
    float_t w = igCalcItemWidth();
    igPushID_Str(label);
    igBeginGroup();
    igPushButtonRepeat(true);
    igButtonEx("<", (ImVec2) { button_size, button_size }, ImGuiButtonFlags_Repeat);
    bool l = igIsItemActive() && (igIsKeyPressed(VK_RETURN, true)
        || igIsMouseClicked(ImGuiMouseButton_Left, true));
    igSameLine(0.0f, 0.0f);
    igSetNextItemWidth(w - button_size * 2.0f);
    igSliderScalar(label, ImGuiDataType_S32, &v, &min, &max, format, flags);
    igSameLine(0.0f, 0.0f);
    igButtonEx(">", (ImVec2) { button_size, button_size }, ImGuiButtonFlags_Repeat);
    bool r = igIsItemActive() && (igIsKeyPressed(VK_RETURN, true)
        || igIsMouseClicked(ImGuiMouseButton_Left, true));
    igPopButtonRepeat();
    igEndGroup();
    igPopID();

    io->KeyRepeatDelay = key_repeat_delay;
    io->KeyRepeatRate = key_repeat_rate;

    if (l) {
        v--;
        if (v < min)
            v = min;
    }
    else if (r) {
        v++;
        if (v > max)
            v = max;
    }

    bool res = *val != v ? true : false;
    *val = v;
    return res;
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
        v[0] ^= true;
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
    bool res = igSliderFloat("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igSliderFloatButtonDisable(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igSliderFloatButton("", val, step, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igSliderIntDisable(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igSliderInt("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static bool igSliderIntButtonDisable(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    igPushID_Str(label);
    imguiDisableElementPush(enable);
    bool res = igSliderIntButton("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    igPopID();
    igPopItemWidth();
    return res;
}

static const char* imguiStartPropertyColumnDisable(const char* label, bool enable) {
    igPushID_Str(label);

    float_t w = imguiGetContentRegionAvailWidth();
    igBeginTable("table", 2, 0, ImVec2_Empty, 0.0f);
    igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

    char* label_temp = str_utils_copy((char*)label);
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    igTableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    imguiDisableElementPush(enable);
    igText(label_temp);
    imguiDisableElementPop(enable);
    free(label_temp);

    igTableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    return (const char*)str_utils_add("##", (char*)label);
}

bool imguiComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (igBeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            igEndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (igBeginCombo(label, items[*selected_idx], flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool(items[n], *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool(items[n], *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        igEndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxString(const char* label, string* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (igBeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            igEndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (igBeginCombo(label, string_data(&items[*selected_idx]), flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool((const char*)string_data(&items[n]),
                    *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool((const char*)string_data(&items[n]),
                    *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        igEndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxConfigFile(const char* label, void* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (igBeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            igEndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    data_struct_file* items_ds = items;
    if (igBeginCombo(label, string_data(&items_ds[*selected_idx].name), flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool((const char*)string_data(&items_ds[n].name),
                    *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                igPushID_Int((int32_t)n);
                if (igSelectable_Bool((const char*)string_data(&items_ds[n].name),
                    *selected_idx == n, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                igPopID();

                if (*selected_idx == n)
                    igSetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        igEndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags) {
    float_t v[3];
    *(vec3*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = igColorEdit3("", v, flags);
    imguiEndPropertyColumn();
    if (res)
        *val = *(vec3*)v;
    return res;
}

bool imguiColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags) {
    float_t v[4];
    *(vec4*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = igColorEdit4("", v, flags);
    imguiEndPropertyColumn();
    if (res)
        *val = *(vec4*)v;
    return res;
}

bool imguiColumnComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    imguiStartPropertyColumn(label);
    bool res = imguiComboBox("", items, size, selected_idx, flags, include_last, focus);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnComboBoxString(const char* label, string* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    imguiStartPropertyColumn(label);
    bool res = imguiComboBoxString("", items, size, selected_idx, flags, include_last, focus);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    imguiStartPropertyColumn(label);
    bool res = imguiComboBoxConfigFile("", items, size, selected_idx, flags, include_last, focus);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnDragFloat(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = igDragFloat("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnDragVec2(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t v[2];
    *(vec2*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = igDragFloat2("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumn(label);
    bool res = igDragFloat3("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumn(label);
    bool res = igDragFloat4("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = igDragInt("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnDragVec2I(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    int32_t v[2];
    *(vec2i*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = igDragInt2("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumn(label);
    bool res = igDragInt3("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumn(label);
    bool res = igDragInt4("", v, speed, min, max, format, flags);
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumn(label);
    bool res = igInputText("", buf, buf_size, flags, callback, user_data);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnSliderFloat(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button)
        res = igSliderFloatButton("", v, step, min, max, format, flags);
    else
        res = igSliderFloat("", v, min, max, format, flags);
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnSliderVec2(const char* label, vec2* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    float_t v[2];
    *(vec2*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(2, igCalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        igPopItemWidth();
        igEndGroup();
    }
    else
        res = igSliderFloat2("", v, min, max, format, flags);
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    v[1] = clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2*)v;
    return true;
}

bool imguiColumnSliderVec3(const char* label, vec3* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    float_t v[3];
    *(vec3*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(3, igCalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Z", &v[2], step, min, max, format, flags);
        igPopItemWidth();
        igEndGroup();
    }
    else
        res = igSliderFloat3("", v, min, max, format, flags);
    imguiEndPropertyColumn();
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

bool imguiColumnSliderVec4(const char* label, vec4* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    float_t v[4];
    *(vec4*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(4, igCalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Z", &v[2], step, min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderFloatButton("##W", &v[3], step, min, max, format, flags);
        igPopItemWidth();
        igEndGroup();
    }
    else
        res = igSliderFloat4("", v, min, max, format, flags);
    imguiEndPropertyColumn();
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
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    int32_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button)
        res = igSliderIntButton("", v, min, max, format, flags);
    else
        res = igSliderInt("", v, min, max, format, flags);
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnSliderVec2I(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    int32_t v[2];
    *(vec2i*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(4, igCalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        igPopItemWidth();

        igEndGroup();
    }
    else
        res = igSliderInt2("", v, min, max, format, flags);
    imguiEndPropertyColumn();
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
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    int32_t v[3];
    *(vec3i*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(4, igCalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##Z", &v[2], min, max, format, flags);
        igPopItemWidth();
        igEndGroup();
    }
    else
        res = igSliderInt3("", v, min, max, format, flags);
    imguiEndPropertyColumn();
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
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    int32_t v[4];
    *(vec4i*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    if (button) {
        ImGuiStyle* style = igGetStyle();
        igBeginGroup();
        igPushMultiItemsWidths(4, igCalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##Z", &v[2], min, max, format, flags);
        igPopItemWidth();
        igSameLine(0.0f, style->ItemInnerSpacing.x);
        res |= igSliderIntButton("##W", &v[3], min, max, format, flags);
        igPopItemWidth();
        igEndGroup();
    }
    else
        res = igSliderInt4("", v, min, max, format, flags);
    imguiEndPropertyColumn();
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

bool imguiColumnSliderLogInt(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    char buf[0x1000];
    int32_t v[1];
    if (min < 1)
        min = 1;
    if (min > max)
        max = min;
    flags |= ImGuiSliderFlags_NoInput;

    v[0] = (int32_t)roundf(log2f((float_t)*val));
    imguiStartPropertyColumn(label);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(1, igCalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    snprintf(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = 1 << clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnSliderLogVec2I(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    char buf[0x1000];
    int32_t v[2];
    if (min < 1)
        min = 1;
    if (min > max)
        max = min;
    flags |= ImGuiSliderFlags_NoInput;

    v[0] = (int32_t)roundf(log2f((float_t)val->x));
    v[1] = (int32_t)roundf(log2f((float_t)val->y));
    imguiStartPropertyColumn(label);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(2, igCalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    snprintf(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = 1 << clamp(v[0], min, max);
    v[1] = 1 << clamp(v[1], min, max);
    if (v[0] == val->x && v[1] == val->y)
        return false;

    *val = *(vec2i*)v;
    return true;
}

bool imguiColumnSliderLogVec3I(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    char buf[0x1000];
    int32_t v[3];
    if (min < 1)
        min = 1;
    if (min > max)
        max = min;
    flags |= ImGuiSliderFlags_NoInput;

    v[0] = (int32_t)roundf(log2f((float_t)val->x));
    v[1] = (int32_t)roundf(log2f((float_t)val->y));
    v[2] = (int32_t)roundf(log2f((float_t)val->z));
    imguiStartPropertyColumn(label);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(3, igCalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    snprintf(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[2]);
    res |= igSliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = 1 << clamp(v[0], min, max);
    v[1] = 1 << clamp(v[1], min, max);
    v[2] = 1 << clamp(v[2], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
        return false;

    *val = *(vec3i*)v;
    return true;
}

bool imguiColumnSliderLogVec4I(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
    char buf[0x1000];
    int32_t v[4];
    if (min < 1)
        min = 1;
    if (min > max)
        max = min;
    flags |= ImGuiSliderFlags_NoInput;

    v[0] = (int32_t)roundf(log2f((float_t)val->x));
    v[1] = (int32_t)roundf(log2f((float_t)val->y));
    v[2] = (int32_t)roundf(log2f((float_t)val->z));
    v[3] = (int32_t)roundf(log2f((float_t)val->w));
    imguiStartPropertyColumn(label);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(4, igCalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    snprintf(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[2]);
    res |= igSliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    snprintf(buf, sizeof(buf), format, 1 << v[3]);
    res |= igSliderIntDisable("##W", &v[3], min, max, buf, flags, true);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = 1 << clamp(v[0], min, max);
    v[1] = 1 << clamp(v[1], min, max);
    v[2] = 1 << clamp(v[2], min, max);
    v[3] = 1 << clamp(v[3], min, max);
    if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
        return false;

    *val = *(vec4i*)v;
    return true;
}

bool imguiColumnInputFloat(const char* label, float_t* val,
    float_t step, float_t step_fast, const char* format, ImGuiInputTextFlags flags) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = igInputFloat("", v, step, step_fast, format, flags);
    imguiEndPropertyColumn();
    if (res)
        *val = v[0];
    return res;
}

bool imguiColumnInputInt(const char* label, int32_t* val,
    int32_t step, int32_t step_fast, ImGuiInputTextFlags flags) {
    int32_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = igInputInt("", v, step, step_fast, flags);
    imguiEndPropertyColumn();
    if (res)
        *val = v[0];
    return res;
}

bool imguiColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
    const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
    imguiStartPropertyColumn(label);
    bool res = igInputScalar("", data_type, p_data, p_step, p_step_fast, format, flags);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnDragFloatFlag(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnDragVec2Flag(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[2];
    *(vec2*)v = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragFloatDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    v[0] = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnDragVec2IFlag(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[2];
    *(vec2i*)v = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igDragIntDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    v[0] = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnSliderVec2Flag(const char* label, vec2* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[2];
    *(vec2*)v = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    v[0] = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(1, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
    if (!res)
        return false;

    v[0] = clamp(v[0], min, max);
    if (v[0] == *val)
        return false;

    *val = v[0];
    return true;
}

bool imguiColumnSliderVec2IFlag(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    int32_t v[2];
    *(vec2i*)v = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(2, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(3, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
    imguiStartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
    bool res = false;
    ImGuiStyle* style = igGetStyle();
    igBeginGroup();
    igPushMultiItemsWidths(4, igCalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    igSameLine(0.0f, style->ItemInnerSpacing.x);
    res |= igSliderIntDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    igEndGroup();
    imguiEndPropertyColumn();
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
