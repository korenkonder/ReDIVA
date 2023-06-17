/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "imgui_helper.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../CRE/data.hpp"

namespace ImGui {
    const float_t AlphaDisabledScale = 0.5f;

    bool IsItemActiveAccum;
    bool IsItemFocusedAccum;
    bool IsItemVisibleAccum;
    bool IsItemEditedAccum;
    bool IsItemActivatedAccum;
    bool IsItemDeactivatedAccum;
    bool IsItemDeactivatedAfterEditAccum;
    bool IsItemToggledOpenAccum;

    float_t ColumnSpace = (float_t)(1.0 / 3.0);

    static float_t cell_padding;

    #undef min
    #undef max

    static void ResetIsItemAccum() {
        IsItemActiveAccum = false;
        IsItemFocusedAccum = false;
        IsItemVisibleAccum = false;
        IsItemEditedAccum = false;
        IsItemActivatedAccum = false;
        IsItemDeactivatedAccum = false;
        IsItemDeactivatedAfterEditAccum = false;
        IsItemToggledOpenAccum = false;
    }

    static void GetIsItemAccum() {
        IsItemActiveAccum |= IsItemActive();
        IsItemFocusedAccum |= IsItemFocused();
        IsItemVisibleAccum |= IsItemVisible();
        IsItemEditedAccum |= IsItemEdited();
        IsItemActivatedAccum |= IsItemActivated();
        IsItemDeactivatedAccum |= IsItemDeactivated();
        IsItemDeactivatedAfterEditAccum |= IsItemDeactivatedAfterEdit();
        IsItemToggledOpenAccum |= IsItemToggledOpen();
    }

    static bool SliderScalarStep(const char* label, ImGuiDataType data_type, void* p_data,
        const void* p_min, const void* p_max, const void* p_step, const char* format, ImGuiSliderFlags flags);

    ImVec2 CalcEmptyGlyphSize() {
        return ImGui::CalcTextSize(" ");
    }

    void StartPropertyColumn(const char* label) {
        PushID(label);

        float_t w = GetContentRegionAvailWidth();
        BeginTable("table", 2);
        TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * ColumnSpace, 0);

        char* label_temp = str_utils_copy(label);
        char* temp;
        if (temp = strstr(label_temp, "##"))
            *temp = 0;
        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
        Text(label_temp);
        if (label_temp)
            free(label_temp);

        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
    }

    void EndPropertyColumn() {
        EndTable();
        PopID();
    }

    bool SliderFloatButton(const char* label, float_t* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags) {
        float_t button_size = GetFont()->FontSize + GetStyle().FramePadding.y * 2.0f;
        ImGuiIO& io = GetIO();

        float_t key_repeat_delay = io.KeyRepeatDelay;
        float_t key_repeat_rate = io.KeyRepeatRate;
        io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
        io.KeyRepeatRate = (float_t)(5.0 / 60.0);

        ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat;
        if (flags & ImGuiSliderFlags_NoInput)
            PushItemFlag(ImGuiItemFlags_Disabled, true);

        float_t v = *val;
        float_t w = CalcItemWidth();
        PushID(label);
        BeginGroup();
        PushButtonRepeat(true);
        ResetIsItemAccum();
        ButtonEx("<", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool l = IsItemActive() && (IsKeyPressed(ImGuiKey_Enter)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        SameLine(0.0f, 0.0f);
        SetNextItemWidth(w - button_size * 2.0f);
        SliderScalarStep(label, ImGuiDataType_Float, &v, &min, &max, &step, format, flags);
        GetIsItemAccum();
        SameLine(0.0f, 0.0f);
        ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool r = IsItemActive() && (IsKeyPressed(ImGuiKey_Enter)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        PopButtonRepeat();
        EndGroup();
        PopID();
        if (flags & ImGuiSliderFlags_NoInput)
            PopItemFlag();

        io.KeyRepeatDelay = key_repeat_delay;
        io.KeyRepeatRate = key_repeat_rate;

        if (l) {
            v -= button_step;
            if (v < min)
                v = min;
        }
        else if (r) {
            v += button_step;
            if (v > max)
                v = max;
        }

        bool res = *val != v ? true : false;
        *val = v;
        return res;
    }

    bool SliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        float_t button_size = GetFont()->FontSize + GetStyle().FramePadding.y * 2.0f;
        ImGuiIO& io = GetIO();

        float_t key_repeat_delay = io.KeyRepeatDelay;
        float_t key_repeat_rate = io.KeyRepeatRate;
        io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
        io.KeyRepeatRate = (float_t)(5.0 / 60.0);

        int32_t v = *val;
        float_t w = CalcItemWidth();
        PushID(label);
        BeginGroup();
        PushButtonRepeat(true);
        ResetIsItemAccum();
        ButtonEx("<", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool l = IsItemActive() && (IsKeyPressed(ImGuiKey_Enter)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        SameLine(0.0f, 0.0f);
        SetNextItemWidth(w - button_size * 2.0f);
        const int32_t step = 1;
        SliderScalarStep(label, ImGuiDataType_S32, &v, &min, &max, &step, format, flags);
        GetIsItemAccum();
        SameLine(0.0f, 0.0f);
        ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool r = IsItemActive() && (IsKeyPressed(ImGuiKey_Enter)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        PopButtonRepeat();
        EndGroup();
        PopID();

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

    bool ButtonEnterKeyPressed(const char* label, const ImVec2& size) {
        ResetIsItemAccum();
        bool res = Button(label, size);
        GetIsItemAccum();
        return res || ItemKeyPressed(ImGuiKey_Enter);
    }

    bool ButtonExEnterKeyPressed(const char* label, const ImVec2& size, ImGuiButtonFlags flags) {
        ResetIsItemAccum();
        bool res = ButtonEx(label, size, flags);
        GetIsItemAccum();
        return res || ItemKeyPressed(ImGuiKey_Enter);
    }

    bool CheckboxEnterKeyPressed(const char* label, bool* v) {
        if (Checkbox(label, v))
            return true;
        else if (ItemKeyPressed(ImGuiKey_Enter)) {
            v[0] ^= true;
            return true;
        }
        else
            return false;
    }

    bool CheckboxFlagsEnterKeyPressed(const char* label, int* flags, int flags_value) {
        if (CheckboxFlags(label, flags, flags_value))
            return true;
        else if (ItemKeyPressed(ImGuiKey_Enter)) {
            *flags ^= flags_value;
            return true;
        }
        else
            return false;
    }

    bool CheckboxFlagsEnterKeyPressed(const char* label, unsigned int* flags, unsigned int flags_value) {
        if (CheckboxFlags(label, flags, flags_value))
            return true;
        else if (ItemKeyPressed(ImGuiKey_Enter)) {
            *flags ^= flags_value;
            return true;
        }
        else
            return false;
    }

    static bool DragFloatDisable(const char* label, float_t* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = DragFloat("", val, speed, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool DragIntDisable(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = DragInt("", val, speed, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderFloatDisable(const char* label, float_t* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderScalar("", ImGuiDataType_Float, val, &min, &max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderFloatButtonDisable(const char* label, float_t* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderFloatButton("", val, step, button_step, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderIntDisable(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderScalar("", ImGuiDataType_S32, val, &min, &max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderIntButtonDisable(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderIntButton("", val, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static const char* StartPropertyColumnDisable(const char* label, bool enable) {
        PushID(label);

        float_t w = GetContentRegionAvailWidth();
        BeginTable("table", 2);
        TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * ColumnSpace, 0);

        char* label_temp = str_utils_copy(label);
        char* temp;
        if (temp = strstr(label_temp, "##"))
            *temp = 0;
        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
        DisableElementPush(enable);
        Text(label_temp);
        DisableElementPop(enable);
        if (label_temp)
            free(label_temp);

        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
        return 0;
    }

    bool ComboBox(const char* label, const char** items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        int32_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        if (BeginCombo(label, *selected_idx != -1 ? items[*selected_idx] : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items[n], *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items[n], *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ComboBox(const char* label, const char** items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        size_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        if (BeginCombo(label, *selected_idx != -1 ? items[*selected_idx] : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((void*)n);
                    if (Selectable(items[n], *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((void*)n);
                    if (Selectable(items[n], *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ComboBoxString(const char* label, std::string* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        int32_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        if (BeginCombo(label, *selected_idx != -1 ? items[*selected_idx].c_str() : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items[n].c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items[n].c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ComboBoxString(const char* label, std::string* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        size_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        if (BeginCombo(label, *selected_idx != -1 ? items[*selected_idx].c_str() : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((void*)n);
                    if (Selectable(items[n].c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((void*)n);
                    if (Selectable(items[n].c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ComboBoxConfigFile(const char* label, void* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        int32_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        data_struct_file* items_ds = (data_struct_file*)items;
        if (BeginCombo(label, *selected_idx != -1 ? items_ds[*selected_idx].name.c_str() : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((int32_t)n);
                    if (Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = (int32_t)n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ComboBoxConfigFile(const char* label, void* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        size_t prev_selected_idx = *selected_idx;

        if (!include_last && size < 1) {
            DisableElementPush(false);
            if (BeginCombo(label, "None", flags)) {
                if (focus)
                    *focus |= true;
                EndCombo();
            }
            DisableElementPop(false);
            return false;
        }

        data_struct_file* items_ds = (data_struct_file*)items;
        if (BeginCombo(label, *selected_idx != -1 ? items_ds[*selected_idx].name.c_str() : 0, flags)) {
            if (include_last)
                for (size_t n = 0; n <= size; n++) {
                    PushID((void*)n);
                    if (Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }
            else
                for (size_t n = 0; n < size; n++) {
                    PushID((void*)n);
                    if (Selectable(items_ds[n].name.c_str(), *selected_idx == n)
                        || ItemKeyPressed(ImGuiKey_Enter)
                        || (IsItemFocused() && *selected_idx != n))
                        *selected_idx = n;
                    PopID();

                    if (*selected_idx == n)
                        SetItemDefaultFocus();
                }

            if (focus)
                *focus |= true;
            EndCombo();
        }
        return prev_selected_idx != *selected_idx;
    }

    bool ColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumn(label);
        bool res = ColorEdit3("", v, flags);
        EndPropertyColumn();
        if (res)
            *val = *(vec3*)v;
        return res;
    }

    bool ColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumn(label);
        bool res = ColorEdit4("", v, flags);
        EndPropertyColumn();
        if (res)
            *val = *(vec4*)v;
        return res;
    }

    bool ColumnComboBox(const char* label, const char** items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBox("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnComboBox(const char* label, const char** items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBox("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnComboBoxString(const char* label, std::string* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBoxString("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnComboBoxString(const char* label, std::string* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBoxString("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBoxConfigFile("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus) {
        StartPropertyColumn(label);
        bool res = ComboBoxConfigFile("", items, size, selected_idx, flags, include_last, focus);
        EndPropertyColumn();
        return res;
    }

    bool ColumnDragFloat(const char* label, float_t* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = DragFloat("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnDragVec2(const char* label, vec2* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumn(label);
        bool res = DragFloat2("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnDragVec3(const char* label, vec3* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumn(label);
        bool res = DragFloat3("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3*)v;
        return true;
    }

    bool ColumnDragVec4(const char* label, vec4* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumn(label);
        bool res = DragFloat4("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4*)v;
        return true;
    }

    bool ColumnDragInt(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = DragInt("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnDragVec2I(const char* label, vec2i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumn(label);
        bool res = DragInt2("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnDragVec3I(const char* label, vec3i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumn(label);
        bool res = DragInt3("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnDragVec4I(const char* label, vec4i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumn(label);
        bool res = DragInt4("", v, speed, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    bool ColumnInputText(const char* label, char* buf, size_t buf_size,
        ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
        StartPropertyColumn(label);
        bool res = InputText("", buf, buf_size, flags, callback, user_data);
        EndPropertyColumn();
        return res;
    }

    bool ColumnSliderFloat(const char* label, float_t* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderScalar("", ImGuiDataType_Float, v, &min, &max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / step) * step;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2(const char* label, vec2* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderFloat2("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / step) * step;
        v[1] = roundf(v[1] / step) * step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnSliderVec3(const char* label, vec3* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderFloat3("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / step) * step;
        v[1] = roundf(v[1] / step) * step;
        v[2] = roundf(v[2] / step) * step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3*)v;
        return true;
    }

    bool ColumnSliderVec4(const char* label, vec4* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderFloat4("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / step) * step;
        v[1] = roundf(v[1] / step) * step;
        v[2] = roundf(v[2] / step) * step;
        v[3] = roundf(v[3] / step) * step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4*)v;
        return true;
    }

    bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderScalar("", ImGuiDataType_S32, v, &min, &max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderInt2("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderInt3("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumn(label);
        ResetIsItemAccum();
        bool res = SliderInt4("", v, min, max, format, flags);
        GetIsItemAccum();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    bool ColumnSliderFloatButton(const char* label, float_t* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        res = SliderFloatButton("", v, button_step, min, max, step, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / button_step) * button_step;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2Button(const char* label, vec2* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        res |= SliderFloatButton("##X", &v[0], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##Y", &v[1], button_step, min, max, step, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / button_step) * button_step;
        v[1] = roundf(v[1] / button_step) * button_step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnSliderVec3Button(const char* label, vec3* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        res |= SliderFloatButton("##X", &v[0], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##Y", &v[1], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##Z", &v[2], button_step, min, max, step, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / button_step) * button_step;
        v[1] = roundf(v[1] / button_step) * button_step;
        v[2] = roundf(v[2] / button_step) * button_step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3*)v;
        return true;
    }

    bool ColumnSliderVec4Button(const char* label, vec4* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderFloatButton("##X", &v[0], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##Y", &v[1], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##Z", &v[2], button_step, min, max, step, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatButton("##W", &v[3], button_step, min, max, step, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = roundf(v[0] / button_step) * button_step;
        v[1] = roundf(v[1] / button_step) * button_step;
        v[2] = roundf(v[2] / button_step) * button_step;
        v[3] = roundf(v[3] / button_step) * button_step;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4*)v;
        return true;
    }

    bool ColumnSliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        res = SliderIntButton("", v, min, max, format, flags);
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2IButton(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderIntButton("##X", &v[0], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##Y", &v[1], min, max, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnSliderVec3IButton(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderIntButton("##X", &v[0], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##Y", &v[1], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##Z", &v[2], min, max, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnSliderVec4IButton(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderIntButton("##X", &v[0], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##Y", &v[1], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##Z", &v[2], min, max, format, flags);
        PopItemWidth();
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntButton("##W", &v[3], min, max, format, flags);
        PopItemWidth();
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    bool ColumnSliderLogInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        char buf[0x1000];
        int32_t v[1];
        if (min < 1)
            min = 1;
        if (min > max)
            max = min;
        flags |= ImGuiSliderFlags_NoInput;

        v[0] = (int32_t)roundf(log2f((float_t)*val));
        StartPropertyColumn(label);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(1, CalcItemWidth());
        min = (int32_t)roundf(log2f((float_t)min));
        max = (int32_t)roundf(log2f((float_t)max));
        sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
        res |= SliderIntDisable("##X", &v[0], min, max, buf, flags, true);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = 1 << clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderLogVec2I(const char* label, vec2i* val,
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
        StartPropertyColumn(label);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        min = (int32_t)roundf(log2f((float_t)min));
        max = (int32_t)roundf(log2f((float_t)max));
        sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
        res |= SliderIntDisable("##X", &v[0], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
        res |= SliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = 1 << clamp_def(v[0], min, max);
        v[1] = 1 << clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnSliderLogVec3I(const char* label, vec3i* val,
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
        StartPropertyColumn(label);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        min = (int32_t)roundf(log2f((float_t)min));
        max = (int32_t)roundf(log2f((float_t)max));
        sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
        res |= SliderIntDisable("##X", &v[0], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
        res |= SliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[2]);
        res |= SliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = 1 << clamp_def(v[0], min, max);
        v[1] = 1 << clamp_def(v[1], min, max);
        v[2] = 1 << clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnSliderLogVec4I(const char* label, vec4i* val,
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
        StartPropertyColumn(label);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        min = (int32_t)roundf(log2f((float_t)min));
        max = (int32_t)roundf(log2f((float_t)max));
        sprintf_s(buf, sizeof(buf), format, 1 << v[0]);
        res |= SliderIntDisable("##X", &v[0], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[1]);
        res |= SliderIntDisable("##Y", &v[1], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[2]);
        res |= SliderIntDisable("##Z", &v[2], min, max, buf, flags, true);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        sprintf_s(buf, sizeof(buf), format, 1 << v[3]);
        res |= SliderIntDisable("##W", &v[3], min, max, buf, flags, true);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = 1 << clamp_def(v[0], min, max);
        v[1] = 1 << clamp_def(v[1], min, max);
        v[2] = 1 << clamp_def(v[2], min, max);
        v[3] = 1 << clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    bool ColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
        const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
        StartPropertyColumn(label);
        bool res = InputScalar("", data_type, p_data, p_step, p_step_fast, format, flags);
        EndPropertyColumn();
        return res;
    }

    bool ColumnDragFloat(const char* label, float_t* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(1, CalcItemWidth());
        res |= DragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnDragVec2(const char* label, vec2* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        res |= DragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnDragVec3(const char* label, vec3* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        res |= DragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3*)v;
        return true;
    }

    bool ColumnDragVec4(const char* label, vec4* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= DragFloatDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragFloatDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4*)v;
        return true;
    }

    bool ColumnDragInt(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(1, CalcItemWidth());
        res |= DragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnDragVec2I(const char* label, vec2i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        res |= DragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnDragVec3I(const char* label, vec3i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        res |= DragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnDragVec4I(const char* label, vec4i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= DragIntDisable("##X", &v[0], speed, min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##Y", &v[1], speed, min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##Z", &v[2], speed, min, max, format, flags, bit_flag & 0x04 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= DragIntDisable("##W", &v[3], speed, min, max, format, flags, bit_flag & 0x08 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    bool ColumnSliderFloat(const char* label, float_t* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(1, CalcItemWidth());
        res |= SliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2(const char* label, vec2* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        res |= SliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnSliderVec3(const char* label, vec3* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        res |= SliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3*)v;
        return true;
    }

    bool ColumnSliderVec4(const char* label, vec4* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderFloatDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderFloatDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4*)v;
        return true;
    }

    bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x01 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(1, CalcItemWidth());
        res |= SliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x03 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(2, CalcItemWidth());
        res |= SliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x07 ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(3, CalcItemWidth());
        res |= SliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z)
            return false;

        *val = *(vec3i*)v;
        return true;
    }

    bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumnDisable(label, bit_flag & 0x0F ? true : false);
        bool res = false;
        ImGuiStyle& style = GetStyle();
        BeginGroup();
        PushMultiItemsWidths(4, CalcItemWidth());
        res |= SliderIntDisable("##X", &v[0], min, max, format, flags, bit_flag & 0x01 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##Y", &v[1], min, max, format, flags, bit_flag & 0x02 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##Z", &v[2], min, max, format, flags, bit_flag & 0x04 ? true : false);
        SameLine(0.0f, style.ItemInnerSpacing.x);
        res |= SliderIntDisable("##W", &v[3], min, max, format, flags, bit_flag & 0x08 ? true : false);
        EndGroup();
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp_def(v[0], min, max);
        v[1] = clamp_def(v[1], min, max);
        v[2] = clamp_def(v[2], min, max);
        v[3] = clamp_def(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }

    void TextCentered(const char* fmt, ...) {
        ImGuiContext& g = *GImGui;

        va_list args;
        va_start(args, fmt);
        vsprintf_s(g.TempBuffer.Data, g.TempBuffer.Size, fmt, args);
        va_end(args);

        std::string tex_buf(g.TempBuffer.Data, g.TempBuffer.Size);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(tex_buf.c_str()).x) * 0.5f);
        ImGui::Text("%s", tex_buf.c_str());
    }

    void TextRight(const char* fmt, ...) {
        ImGuiContext& g = *GImGui;

        va_list args;
        va_start(args, fmt);
        vsprintf_s(g.TempBuffer.Data, g.TempBuffer.Size, fmt, args);
        va_end(args);

        std::string tex_buf(g.TempBuffer.Data, g.TempBuffer.Size);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(tex_buf.c_str()).x);
        ImGui::Text("%s", tex_buf.c_str());
    }

    // Taken from ImGui src
    // FIXME: Try to move more of the code into shared SliderBehavior()
    template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
    bool SliderBehaviorStepT(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v,
        const TYPE v_min, const TYPE v_max, const TYPE v_step, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb) {
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
        const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
        const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
        const SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);

        // Calculate bounds
        const float_t grab_padding = 2.0f; // FIXME: Should be part of style.
        const float_t slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
        float_t grab_sz = style.GrabMinSize * 1.5f;
        // v_range < 0 may happen on integer overflows
        if (!is_floating_point && v_range >= 0)
            // For integer sliders: if possible have the grab size represent 1 unit
            grab_sz = ImMax((float_t)(slider_sz / (v_range + 1)), style.GrabMinSize * 1.5f);
        grab_sz = ImMin(grab_sz, slider_sz);
        const float_t slider_usable_sz = slider_sz - grab_sz;
        const float_t slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
        const float_t slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

        float_t logarithmic_zero_epsilon = 0.0f; // Only valid when is_logarithmic is true
        float_t zero_deadzone_halfsize = 0.0f; // Only valid when is_logarithmic is true
        if (is_logarithmic) {
            // When using logarithmic sliders, we need to clamp to avoid hitting zero,
            // but our choice of clamp value greatly affects slider precision. We attempt
            // to use the specified precision to estimate a good lower bound.
            const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 1;
            logarithmic_zero_epsilon = ImPow(0.1f, (float_t)decimal_precision);
            zero_deadzone_halfsize = (style.LogSliderDeadzone * 0.5f) / ImMax(slider_usable_sz, 1.0f);
        }

        // Process interacting with the slider
        bool value_changed = false;
        bool continuos_value_change = false;
        if (g.ActiveId == id) {
            bool set_new_value = false;
            float_t clicked_t = 0.0f;
            if (g.ActiveIdSource == ImGuiInputSource_Mouse) {
                if (!g.IO.MouseDown[0])
                    ClearActiveID();
                else {
                    const float_t mouse_abs_pos = g.IO.MousePos[axis];
                    if (g.ActiveIdIsJustActivated) {
                        float_t grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                            *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                        if (axis == ImGuiAxis_Y)
                            grab_t = 1.0f - grab_t;
                        const float_t grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
                        // No harm being extra generous here.
                        const bool clicked_around_grab = (mouse_abs_pos >= grab_pos - grab_sz * 0.5f - 1.0f)
                            && (mouse_abs_pos <= grab_pos + grab_sz * 0.5f + 1.0f);
                        g.SliderGrabClickOffset = (clicked_around_grab
                            && is_floating_point) ? mouse_abs_pos - grab_pos : 0.0f;
                    }

                    ImRect out_grab_bb;
                    if (slider_sz < 1.0f) {
                        out_grab_bb = ImRect(bb.Min, bb.Min);
                    }
                    else {
                        float_t grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                            *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                        if (axis == ImGuiAxis_Y)
                            grab_t = 1.0f - grab_t;
                        const float_t grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
                        if (axis == ImGuiAxis_X)
                            out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
                        else
                            out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
                    }

                    if (slider_usable_sz > 0.0f)
                        clicked_t = ImSaturate((mouse_abs_pos - g.SliderGrabClickOffset
                            - slider_usable_pos_min) / slider_usable_sz);
                    if (axis == ImGuiAxis_Y)
                        clicked_t = 1.0f - clicked_t;
                    set_new_value = true;
                    if (out_grab_bb.Min[axis] <= g.IO.MousePosPrev[axis]
                        && g.IO.MousePosPrev[axis] <= out_grab_bb.Max[axis])
                        continuos_value_change = true;
                }
            }
            else if (g.ActiveIdSource == ImGuiInputSource_Keyboard || g.ActiveIdSource == ImGuiInputSource_Gamepad) {
                if (g.ActiveIdIsJustActivated) {
                    g.SliderCurrentAccum = 0.0f; // Reset any stored nav delta upon activation
                    g.SliderCurrentAccumDirty = false;
                }

                float_t input_delta = (axis == ImGuiAxis_X) ? GetNavTweakPressedAmount(axis)
                    : -GetNavTweakPressedAmount(axis);
                if (input_delta != 0.0f) {
                    const bool tweak_slow = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad)
                        ? ImGuiKey_NavGamepadTweakSlow : ImGuiKey_NavKeyboardTweakSlow);
                    const bool tweak_fast = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad)
                        ? ImGuiKey_NavGamepadTweakFast : ImGuiKey_NavKeyboardTweakFast);
                    const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 0;
                    if (decimal_precision > 0) {
                        // Gamepad/keyboard tweak speeds in % of slider bounds
                        input_delta /= 100.0f;
                        if (tweak_slow)
                            input_delta /= 10.0f;
                    }
                    else if ((v_range >= -100.0f && v_range <= 100.0f) || tweak_slow)
                        // Gamepad/keyboard tweak speeds in integer steps
                        input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / (float_t)v_range;
                    else
                        input_delta /= 100.0f;

                    if (tweak_fast)
                        input_delta *= 10.0f;

                    g.SliderCurrentAccum += input_delta;
                    g.SliderCurrentAccumDirty = true;
                }

                float_t delta = g.SliderCurrentAccum;
                if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
                    ClearActiveID();
                else if (g.SliderCurrentAccumDirty) {
                    clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                        *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                    // This is to avoid applying the saturation when already past the limits
                    if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) {
                        set_new_value = false;
                        g.SliderCurrentAccum = 0.0f; // If pushing up against the limits, don't continue to accumulate
                    }
                    else {
                        set_new_value = true;
                        float_t old_clicked_t = clicked_t;
                        clicked_t = ImSaturate(clicked_t + delta);

                        // Calculate what our "new" clicked_t will be, and thus how far
                        // we actually moved the slider, and subtract this from the accumulator
                        TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                            clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                        if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                            v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);
                        float_t new_clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                            v_new, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                        if (delta > 0)
                            g.SliderCurrentAccum -= ImMin(new_clicked_t - old_clicked_t, delta);
                        else
                            g.SliderCurrentAccum -= ImMax(new_clicked_t - old_clicked_t, delta);
                    }

                    g.SliderCurrentAccumDirty = false;
                }
            }

            if (set_new_value) {
                TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type,
                    clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                // Round to user desired precision based on format string
                if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                    v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);

                // Apply result
                if (continuos_value_change) {
                    *v = v_new;
                    value_changed = true;
                }
                else if (v_new - *v >= v_step) {
                    *v += v_step;
                    value_changed = true;
                }
                else if (*v - v_new >= v_step) {
                    *v -= v_step;
                    value_changed = true;
                }
                else if (*v - v_step <= v_min || *v + v_step >= v_max) {
                    *v = v_new;
                    value_changed = true;
                }
            }
        }

        if (slider_sz < 1.0f) {
            *out_grab_bb = ImRect(bb.Min, bb.Min);
        }
        else {
            // Output grab position so it can be displayed by the caller
            float_t grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
            if (axis == ImGuiAxis_Y)
                grab_t = 1.0f - grab_t;
            const float_t grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
            if (axis == ImGuiAxis_X)
                *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
            else
                *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
        }

        return value_changed;
    }

    // For 32-bit and larger types, slider bounds are limited to half the natural type range.
    // So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
    // It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
    bool SliderBehaviorStep(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v,
        const void* p_min, const void* p_max, const void* p_step, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb) {
        // Read imgui.cpp "API BREAKING CHANGES" section for 1.78 if you hit this assert.
        IM_ASSERT((flags == 1 || (flags & ImGuiSliderFlags_InvalidMask_) == 0)
            && "Invalid ImGuiSliderFlags flag!  Has the 'float_t power' argument been"
            " mistakenly cast to flags? Call function with ImGuiSliderFlags_Logarithmic flags instead.");

        ImGuiContext& g = *GImGui;
        if ((g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly) || (flags & ImGuiSliderFlags_ReadOnly))
            return false;

        switch (data_type) {
        case ImGuiDataType_S8: {
            int32_t v32 = (int32_t) * (int8_t*)p_v;
            const int8_t min = *(const int8_t*)p_min;
            const int8_t max = *(const int8_t*)p_max;
            const int8_t step = *(const int8_t*)p_step;
            bool r = SliderBehaviorStepT<int32_t, int32_t, float_t>(bb, id, ImGuiDataType_S32,
                &v32, min, max, step, format, flags, out_grab_bb);
            if (r)
                *(int8_t*)p_v = (int8_t)v32;
            return r;
        }
        case ImGuiDataType_U8: {
            uint32_t v32 = (uint32_t) * (uint8_t*)p_v;
            const uint8_t min = *(const uint8_t*)p_min;
            const uint8_t max = *(const uint8_t*)p_max;
            const uint8_t step = *(const uint8_t*)p_step;
            bool r = SliderBehaviorStepT<uint32_t, int32_t, float_t>(bb, id, ImGuiDataType_U32,
                &v32, min, max, step, format, flags, out_grab_bb);
            if (r)
                *(uint8_t*)p_v = (uint8_t)v32;
            return r;
        }
        case ImGuiDataType_S16: {
            int32_t v32 = (int32_t) * (int16_t*)p_v;
            const int16_t min = *(const int16_t*)p_min;
            const int16_t max = *(const int16_t*)p_max;
            const int16_t step = *(const int16_t*)p_step;
            bool r = SliderBehaviorStepT<int32_t, int32_t, float_t>(bb, id, ImGuiDataType_S32,
                &v32, min, max, step, format, flags, out_grab_bb);
            if (r)
                *(int16_t*)p_v = (int16_t)v32;
            return r;
        }
        case ImGuiDataType_U16: {
            uint32_t v32 = (uint32_t) * (uint16_t*)p_v;
            const uint16_t min = *(const uint16_t*)p_min;
            const uint16_t max = *(const uint16_t*)p_max;
            const uint16_t step = *(const uint16_t*)p_step;
            bool r = SliderBehaviorStepT<uint32_t, int32_t, float_t>(bb, id, ImGuiDataType_U32,
                &v32, min, max, step, format, flags, out_grab_bb);
            if (r)
                *(uint16_t*)p_v = (uint16_t)v32;
            return r;
        }
        case ImGuiDataType_S32: {
            const int32_t min = *(const int32_t*)p_min;
            const int32_t max = *(const int32_t*)p_max;
            const int32_t step = *(const int32_t*)p_step;
            IM_ASSERT(min >= INT32_MIN / 2 && max <= INT32_MAX / 2);
            return SliderBehaviorStepT<int32_t, int32_t, float_t >(bb, id, data_type,
                (int32_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_U32: {
            const uint32_t min = *(const uint32_t*)p_min;
            const uint32_t max = *(const uint32_t*)p_max;
            const uint32_t step = *(const uint32_t*)p_step;
            IM_ASSERT(max <= UINT32_MAX / 2);
            return SliderBehaviorStepT<uint32_t, int32_t, float_t >(bb, id, data_type,
                (uint32_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_S64: {
            const int64_t min = *(const int64_t*)p_min;
            const int64_t max = *(const int64_t*)p_max;
            const int64_t step = *(const int64_t*)p_step;
            IM_ASSERT(min >= INT64_MIN / 2 && max <= INT64_MAX / 2);
            return SliderBehaviorStepT<int64_t, int64_t, double_t>(bb, id, data_type,
                (int64_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_U64: {
            const uint64_t min = *(const uint64_t*)p_min;
            const uint64_t max = *(const uint64_t*)p_max;
            const uint64_t step = *(const uint64_t*)p_step;
            IM_ASSERT(max <= UINT64_MAX / 2);
            return SliderBehaviorStepT<uint64_t, int64_t, double_t>(bb, id, data_type,
                (uint64_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_Float: {
            const float_t min = *(const float_t*)p_min;
            const float_t max = *(const float_t*)p_max;
            const float_t step = *(const float_t*)p_step;
            IM_ASSERT(min >= -FLT_MAX / 2.0f && max <= FLT_MAX / 2.0f);
            return SliderBehaviorStepT<float_t, float_t, float_t >(bb, id, data_type,
                (float_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_Double: {
            const double_t min = *(const double_t*)p_min;
            const double_t max = *(const double_t*)p_max;
            const double_t step = *(const double_t*)p_step;
            IM_ASSERT(min >= -DBL_MAX / 2.0f && max <= DBL_MAX / 2.0f);
            return SliderBehaviorStepT<double_t, double_t, double_t>(bb, id, data_type,
                (double_t*)p_v, min, max, step, format, flags, out_grab_bb);
        }
        case ImGuiDataType_COUNT: break;
        }
        IM_ASSERT(0);
        return false;
    }

    static bool SliderScalarStep(const char* label, ImGuiDataType data_type, void* p_data,
        const void* p_min, const void* p_max, const void* p_step, const char* format, ImGuiSliderFlags flags) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float_t w = CalcItemWidth();
        ImGuiIO& io = g.IO;

        float_t key_repeat_delay = io.KeyRepeatDelay;
        float_t key_repeat_rate = io.KeyRepeatRate;
        io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
        io.KeyRepeatRate = (float_t)(5.0 / 60.0);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w,
            window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, ImVec2(frame_bb.Max.x
            + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), frame_bb.Max.y));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
            return false;

        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
        if (!temp_input_is_active) {
            // Tabbing or CTRL-clicking on Slider turns it into an input box
            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && IsMouseClicked(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id);
            if (make_active && clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active) {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        if (temp_input_is_active) {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
            return TempInputScalar(frame_bb, id, label, data_type, p_data,
                format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
        }

        // Draw frame
        const uint32_t frame_col = GetColorU32(g.ActiveId == id
            ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Slider behavior
        ImRect grab_bb;
        bool value_changed = SliderBehaviorStep(frame_bb, id, data_type,
            p_data, p_min, p_max, p_step, format, flags, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf,
            IM_ARRAYSIZE(value_buf), data_type, p_data, format);
        if (g.LogEnabled)
            LogSetNextTextDecoration("{", "}");
        RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x,
                frame_bb.Min.y + style.FramePadding.y), label);

        io.KeyRepeatDelay = key_repeat_delay;
        io.KeyRepeatRate = key_repeat_rate;

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return value_changed;
    }
}