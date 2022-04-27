/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "imgui_helper.h"
#include "../../KKdLib/str_utils.h"
#include "../../CRE/data.h"

const float_t imgui_alpha_disabled_scale = 0.5f;

bool imgui_is_item_active;
bool imgui_is_item_focused;
bool imgui_is_item_visible;
bool imgui_is_item_edited;
bool imgui_is_item_activated;
bool imgui_is_item_deactivated;
bool imgui_is_item_deactivated_after_edit;
bool imgui_is_item_toggled_open;

static float_t column_space = (float_t)(1.0 / 3.0);
static float_t cell_padding;

#undef min
#undef max

static void imgui_reset_is_item() {
    imgui_is_item_active = false;
    imgui_is_item_focused = false;
    imgui_is_item_visible = false;
    imgui_is_item_edited = false;
    imgui_is_item_activated = false;
    imgui_is_item_deactivated = false;
    imgui_is_item_deactivated_after_edit = false;
    imgui_is_item_toggled_open = false;
}

static void imgui_get_is_item() {
    imgui_is_item_active |= ImGui::IsItemActive();
    imgui_is_item_focused |= ImGui::IsItemFocused();
    imgui_is_item_visible |= ImGui::IsItemVisible();
    imgui_is_item_edited |= ImGui::IsItemEdited();
    imgui_is_item_activated |= ImGui::IsItemActivated();
    imgui_is_item_deactivated |= ImGui::IsItemDeactivated();
    imgui_is_item_deactivated_after_edit |= ImGui::IsItemDeactivatedAfterEdit();
    imgui_is_item_toggled_open |= ImGui::IsItemToggledOpen();
}

inline bool imguiItemKeyDown(int32_t key) {
    return ImGui::IsItemFocused() && ImGui::IsKeyDown(key);
}

inline bool imguiItemKeyPressed(int32_t key, bool repeat) {
    return ImGui::IsItemFocused() && ImGui::IsKeyPressed(key, repeat);
}

inline bool imguiItemKeyReleased(int32_t key) {
    return ImGui::IsItemFocused() && ImGui::IsKeyReleased(key);
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
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * imgui_alpha_disabled_scale);
    }
}

inline void imguiDisableElementPop(bool enable) {
    if (!enable) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar(1);
    }
}

inline float_t imguiGetContentRegionAvailWidth() {
    return ImGui::GetContentRegionAvail().x;
}

inline float_t imguiGetContentRegionAvailHeight() {
    return ImGui::GetContentRegionAvail().y;
}

inline void imguiGetContentRegionAvailSetNextItemWidth() {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
}

void imguiStartPropertyColumn(const char* label) {
    ImGui::PushID(label);

    float_t w = imguiGetContentRegionAvailWidth();
    ImGui::BeginTable("table", 2);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

    char* label_temp = str_utils_copy(label);
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    ImGui::TableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    ImGui::Text(label_temp);
    free(label_temp);

    ImGui::TableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
}

void imguiEndPropertyColumn() {
    ImGui::EndTable();
    ImGui::PopID();
}

bool igSliderFloatButton(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
    float_t button_size = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImGuiIO& io = ImGui::GetIO();

    float_t key_repeat_delay = io.KeyRepeatDelay;
    float_t key_repeat_rate = io.KeyRepeatRate;
    io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
    io.KeyRepeatRate = (float_t)(10.0 / 60.0);

    ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat;
    if (flags & ImGuiSliderFlags_NoInput)
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

    float_t v = *val;
    float_t w = ImGui::CalcItemWidth();
    ImGui::PushID(label);
    ImGui::BeginGroup();
    ImGui::PushButtonRepeat(true);
    imgui_reset_is_item();
    ImGui::ButtonEx("<", { button_size, button_size }, ImGuiButtonFlags_Repeat);
    imgui_get_is_item();
    bool l = ImGui::IsItemActive() && (ImGui::IsKeyPressed(VK_RETURN, true)
        || ImGui::IsMouseClicked(ImGuiMouseButton_Left, true));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetNextItemWidth(w - button_size * 2.0f);
    ImGui::SliderScalar(label, ImGuiDataType_Float, &v, &min, &max, format, flags);
    imgui_get_is_item();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
    imgui_get_is_item();
    bool r = ImGui::IsItemActive() && (ImGui::IsKeyPressed(VK_RETURN, true)
        || ImGui::IsMouseClicked(ImGuiMouseButton_Left, true));
    ImGui::PopButtonRepeat();
    ImGui::EndGroup();
    ImGui::PopID();
    if (flags & ImGuiSliderFlags_NoInput)
        ImGui::PopItemFlag();

    io.KeyRepeatDelay = key_repeat_delay;
    io.KeyRepeatRate = key_repeat_rate;

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
    float_t button_size = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImGuiIO& io = ImGui::GetIO();

    float_t key_repeat_delay = io.KeyRepeatDelay;
    float_t key_repeat_rate = io.KeyRepeatRate;
    io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
    io.KeyRepeatRate = (float_t)(10.0 / 60.0);

    int32_t v = *val;
    float_t w = ImGui::CalcItemWidth();
    ImGui::PushID(label);
    ImGui::BeginGroup();
    ImGui::PushButtonRepeat(true);
    imgui_reset_is_item();
    ImGui::ButtonEx("<", { button_size, button_size }, ImGuiButtonFlags_Repeat);
    imgui_get_is_item();
    bool l = ImGui::IsItemActive() && (ImGui::IsKeyPressed(VK_RETURN, true)
        || ImGui::IsMouseClicked(ImGuiMouseButton_Left, true));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetNextItemWidth(w - button_size * 2.0f);
    ImGui::SliderScalar(label, ImGuiDataType_S32, &v, &min, &max, format, flags);
    imgui_get_is_item();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
    imgui_get_is_item();
    bool r = ImGui::IsItemActive() && (ImGui::IsKeyPressed(VK_RETURN, true)
        || ImGui::IsMouseClicked(ImGuiMouseButton_Left, true));
    ImGui::PopButtonRepeat();
    ImGui::EndGroup();
    ImGui::PopID();

    io.KeyRepeatDelay = key_repeat_delay;
    io.KeyRepeatRate = key_repeat_rate;

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

bool imguiButton(const char* label, const ImVec2& size) {
    imgui_reset_is_item();
    bool res = ImGui::Button(label, size);
    imgui_get_is_item();
    return res || imguiItemKeyPressed(GLFW_KEY_ENTER, true);
}

bool imguiButtonEx(const char* label, const ImVec2& size, ImGuiButtonFlags flags) {
    imgui_reset_is_item();
    bool res = ImGui::ButtonEx(label, size, flags);
    imgui_get_is_item();
    return res || imguiItemKeyPressed(GLFW_KEY_ENTER, true);
}

bool imguiCheckbox(const char* label, bool* v) {
    if (ImGui::Checkbox(label, v))
        return true;
    else if (imguiItemKeyPressed(GLFW_KEY_ENTER, true)) {
        v[0] ^= true;
        return true;
    }
    else
        return false;
}

bool imguiCheckboxFlags(const char* label, int* flags, int flags_value) {
    if (ImGui::CheckboxFlags(label, flags, flags_value))
        return true;
    else if (imguiItemKeyPressed(GLFW_KEY_ENTER, true)) {
        *flags ^= flags_value;
        return true;
    }
    else
        return false;
}

bool imguiCheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value) {
    if (ImGui::CheckboxFlags(label, flags, flags_value))
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
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = ImGui::DragFloat("", val, speed, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static bool igDragIntDisable(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = ImGui::DragInt("", val, speed, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static bool igSliderFloatDisable(const char* label, float_t* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = ImGui::SliderFloat("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static bool igSliderFloatButtonDisable(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = igSliderFloatButton("", val, step, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static bool igSliderIntDisable(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = ImGui::SliderInt("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static bool igSliderIntButtonDisable(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
    ImGui::PushID(label);
    imguiDisableElementPush(enable);
    bool res = igSliderIntButton("", val, min, max, enable ? format : "...", flags);
    imguiDisableElementPop(enable);
    ImGui::PopID();
    ImGui::PopItemWidth();
    return res;
}

static const char* imguiStartPropertyColumnDisable(const char* label, bool enable) {
    ImGui::PushID(label);

    float_t w = imguiGetContentRegionAvailWidth();
    ImGui::BeginTable("table", 2);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

    char* label_temp = str_utils_copy(label);
    char* temp;
    if (temp = strstr(label_temp, "##"))
        *temp = 0;
    ImGui::TableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    imguiDisableElementPush(enable);
    ImGui::Text(label_temp);
    imguiDisableElementPop(enable);
    free(label_temp);

    ImGui::TableNextColumn();
    imguiGetContentRegionAvailSetNextItemWidth();
    return 0;
}

bool imguiComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (ImGui::BeginCombo(label, *selected_idx != -1 ? items[*selected_idx] : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(items[n], *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(items[n], *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBox(const char* label, const char** items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    size_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (ImGui::BeginCombo(label, *selected_idx != -1 ? items[*selected_idx] : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(items[n], *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(items[n], *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxString(const char* label, string* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (ImGui::BeginCombo(label, *selected_idx != -1 ? string_data(&items[*selected_idx]) : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(string_data(&items[n]), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(string_data(&items[n]), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxString(const char* label, string* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    size_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    if (ImGui::BeginCombo(label, *selected_idx != -1 ? string_data(&items[*selected_idx]) : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(string_data(&items[n]), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(string_data(&items[n]), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxConfigFile(const char* label, void* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    int32_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    data_struct_file* items_ds = (data_struct_file*)items;
    if (ImGui::BeginCombo(label, *selected_idx != -1 ? items_ds[*selected_idx].name.c_str() : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((int32_t)n);
                if (ImGui::Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = (int32_t)n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiComboBoxConfigFile(const char* label, void* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
    size_t prev_selected_idx = *selected_idx;

    if (!include_last && size < 1) {
        imguiDisableElementPush(false);
        if (ImGui::BeginCombo(label, "None", flags)) {
            if (focus)
                *focus |= true;
            ImGui::EndCombo();
        }
        imguiDisableElementPop(false);
        return false;
    }

    data_struct_file* items_ds = (data_struct_file*)items;
    if (ImGui::BeginCombo(label, *selected_idx != -1 ? items_ds[*selected_idx].name.c_str() : 0, flags)) {
        if (include_last)
            for (size_t n = 0; n <= size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
        else
            for (size_t n = 0; n < size; n++) {
                ImGui::PushID((void*)n);
                if (ImGui::Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && *selected_idx != n))
                    *selected_idx = n;
                ImGui::PopID();

                if (*selected_idx == n)
                    ImGui::SetItemDefaultFocus();
            }

        if (focus)
            *focus |= true;
        ImGui::EndCombo();
    }
    return prev_selected_idx != *selected_idx;
}

bool imguiColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags) {
    float_t v[3];
    *(vec3*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = ImGui::ColorEdit3("", v, flags);
    imguiEndPropertyColumn();
    if (res)
        *val = *(vec3*)v;
    return res;
}

bool imguiColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags) {
    float_t v[4];
    *(vec4*)v = *val;
    imguiStartPropertyColumn(label);
    bool res = ImGui::ColorEdit4("", v, flags);
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

bool imguiColumnComboBox(const char* label, const char** items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
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

bool imguiColumnComboBoxString(const char* label, string* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
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

bool imguiColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
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
    bool res = ImGui::DragFloat("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragFloat2("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragFloat3("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragFloat4("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragInt("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragInt2("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragInt3("", v, speed, min, max, format, flags);
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
    bool res = ImGui::DragInt4("", v, speed, min, max, format, flags);
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
    bool res = ImGui::InputText("", buf, buf_size, flags, callback, user_data);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnSliderFloat(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumn(label);
    bool res = false;
    imgui_reset_is_item();
    if (button)
        res = igSliderFloatButton("", v, step, min, max, format, flags);
    else {
        res = ImGui::SliderFloat("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderFloat2("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Z", &v[2], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderFloat3("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        res |= igSliderFloatButton("##X", &v[0], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Y", &v[1], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##Z", &v[2], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderFloatButton("##W", &v[3], step, min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderFloat4("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button)
        res = igSliderIntButton("", v, min, max, format, flags);
    else {
        res = ImGui::SliderInt("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        ImGui::PopItemWidth();

        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderInt2("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##Z", &v[2], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderInt3("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    imgui_reset_is_item();
    if (button) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        res |= igSliderIntButton("##X", &v[0], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##Y", &v[1], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##Z", &v[2], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= igSliderIntButton("##W", &v[3], min, max, format, flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
    else {
        res = ImGui::SliderInt4("", v, min, max, format, flags);
        imgui_get_is_item();
    }
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[2]);
    res |= igSliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    min = (int32_t)roundf(log2f((float_t)min));
    max = (int32_t)roundf(log2f((float_t)max));
    sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
    res |= igSliderIntDisable("##X", &v[0], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
    res |= igSliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[2]);
    res |= igSliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    sprintf_s(buf, sizeof(buf), format, 1 << v[3]);
    res |= igSliderIntDisable("##W", &v[3], min, max, buf, flags, true);
    ImGui::EndGroup();
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

bool imguiColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
    const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
    imguiStartPropertyColumn(label);
    bool res = ImGui::InputScalar("", data_type, p_data, p_step, p_step_fast, format, flags);
    imguiEndPropertyColumn();
    return res;
}

bool imguiColumnDragFloatFlag(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
    float_t v[1];
    v[0] = *val;
    imguiStartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
    bool res = false;
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    res |= igDragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragFloatDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    res |= igDragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igDragIntDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    res |= igSliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderFloatDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::EndGroup();
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
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    res |= igSliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
    res |= igSliderIntDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
    ImGui::EndGroup();
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
