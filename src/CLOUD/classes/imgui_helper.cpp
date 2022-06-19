/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "imgui_helper.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/data.hpp"

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

    static float_t column_space = (float_t)(1.0 / 3.0);
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

    inline bool ItemKeyDown(int32_t key) {
        return IsItemFocused() && IsKeyDown(key);
    }

    inline bool ItemKeyPressed(int32_t key, bool repeat) {
        return IsItemFocused() && IsKeyPressed(key, repeat);
    }

    inline bool ItemKeyReleased(int32_t key) {
        return IsItemFocused() && IsKeyReleased(key);
    }

    inline float_t GetColumnSpace() {
        return column_space;
    }

    inline void SetColumnSpace(float_t val) {
        column_space = val;
    }

    inline void SetDefaultColumnSpace() {
        column_space = (float_t)(1.0 / 3.0);
    }

    inline void DisableElementPush(bool enable) {
        if (!enable) {
            PushItemFlag(ImGuiItemFlags_Disabled, true);
            PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * AlphaDisabledScale);
        }
    }

    inline void DisableElementPop(bool enable) {
        if (!enable) {
            PopItemFlag();
            PopStyleVar(1);
        }
    }

    inline float_t GetContentRegionAvailWidth() {
        return GetContentRegionAvail().x;
    }

    inline float_t GetContentRegionAvailHeight() {
        return GetContentRegionAvail().y;
    }

    inline void GetContentRegionAvailSetNextItemWidth() {
        SetNextItemWidth(GetContentRegionAvail().x);
    }

    void StartPropertyColumn(const char* label) {
        PushID(label);

        float_t w = GetContentRegionAvailWidth();
        BeginTable("table", 2);
        TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

        char* label_temp = str_utils_copy(label);
        char* temp;
        if (temp = strstr(label_temp, "##"))
            *temp = 0;
        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
        Text(label_temp);
        free(label_temp);

        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
    }

    void EndPropertyColumn() {
        EndTable();
        PopID();
    }

    bool SliderFloatButton(const char* label, float_t* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags) {
        float_t button_size = GetFont()->FontSize + GetStyle().FramePadding.y * 2.0f;
        ImGuiIO& io = GetIO();

        float_t key_repeat_delay = io.KeyRepeatDelay;
        float_t key_repeat_rate = io.KeyRepeatRate;
        io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
        io.KeyRepeatRate = (float_t)(10.0 / 60.0);

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
        bool l = IsItemActive() && (IsKeyPressed(VK_RETURN, true)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        SameLine(0.0f, 0.0f);
        SetNextItemWidth(w - button_size * 2.0f);
        SliderScalar(label, ImGuiDataType_Float, &v, &min, &max, format, flags);
        GetIsItemAccum();
        SameLine(0.0f, 0.0f);
        ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool r = IsItemActive() && (IsKeyPressed(VK_RETURN, true)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        PopButtonRepeat();
        EndGroup();
        PopID();
        if (flags & ImGuiSliderFlags_NoInput)
            PopItemFlag();

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

    bool SliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags) {
        float_t button_size = GetFont()->FontSize + GetStyle().FramePadding.y * 2.0f;
        ImGuiIO& io = GetIO();

        float_t key_repeat_delay = io.KeyRepeatDelay;
        float_t key_repeat_rate = io.KeyRepeatRate;
        io.KeyRepeatDelay = (float_t)(30.0 / 60.0);
        io.KeyRepeatRate = (float_t)(10.0 / 60.0);

        int32_t v = *val;
        float_t w = CalcItemWidth();
        PushID(label);
        BeginGroup();
        PushButtonRepeat(true);
        ResetIsItemAccum();
        ButtonEx("<", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool l = IsItemActive() && (IsKeyPressed(VK_RETURN, true)
            || IsMouseClicked(ImGuiMouseButton_Left, true));
        SameLine(0.0f, 0.0f);
        SetNextItemWidth(w - button_size * 2.0f);
        SliderScalar(label, ImGuiDataType_S32, &v, &min, &max, format, flags);
        GetIsItemAccum();
        SameLine(0.0f, 0.0f);
        ButtonEx(">", { button_size, button_size }, ImGuiButtonFlags_Repeat);
        GetIsItemAccum();
        bool r = IsItemActive() && (IsKeyPressed(VK_RETURN, true)
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
        return res || ItemKeyPressed(GLFW_KEY_ENTER, true);
    }

    bool ButtonExEnterKeyPressed(const char* label, const ImVec2& size, ImGuiButtonFlags flags) {
        ResetIsItemAccum();
        bool res = ButtonEx(label, size, flags);
        GetIsItemAccum();
        return res || ItemKeyPressed(GLFW_KEY_ENTER, true);
    }

    bool CheckboxEnterKeyPressed(const char* label, bool* v) {
        if (Checkbox(label, v))
            return true;
        else if (ItemKeyPressed(GLFW_KEY_ENTER, true)) {
            v[0] ^= true;
            return true;
        }
        else
            return false;
    }

    bool CheckboxFlagsEnterKeyPressed(const char* label, int* flags, int flags_value) {
        if (CheckboxFlags(label, flags, flags_value))
            return true;
        else if (ItemKeyPressed(GLFW_KEY_ENTER, true)) {
            *flags ^= flags_value;
            return true;
        }
        else
            return false;
    }

    bool CheckboxFlagsEnterKeyPressed(const char* label, unsigned int* flags, unsigned int flags_value) {
        if (CheckboxFlags(label, flags, flags_value))
            return true;
        else if (ItemKeyPressed(GLFW_KEY_ENTER, true)) {
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
        bool res = SliderFloat("", val, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderFloatButtonDisable(const char* label, float_t* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderFloatButton("", val, step, min, max, enable ? format : "...", flags);
        DisableElementPop(enable);
        PopID();
        PopItemWidth();
        return res;
    }

    static bool SliderIntDisable(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool enable) {
        PushID(label);
        DisableElementPush(enable);
        bool res = SliderInt("", val, min, max, enable ? format : "...", flags);
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
        TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * column_space, 0);

        char* label_temp = str_utils_copy(label);
        char* temp;
        if (temp = strstr(label_temp, "##"))
            *temp = 0;
        TableNextColumn();
        GetContentRegionAvailSetNextItemWidth();
        DisableElementPush(enable);
        Text(label_temp);
        DisableElementPop(enable);
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                        || ItemKeyPressed(GLFW_KEY_ENTER, true)
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
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
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        float_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button)
            res = SliderFloatButton("", v, step, min, max, format, flags);
        else {
            res = SliderFloat("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2(const char* label, vec2* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        float_t v[2];
        *(vec2*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
            ImGuiStyle& style = GetStyle();
            BeginGroup();
            PushMultiItemsWidths(2, CalcItemWidth());
            res |= SliderFloatButton("##X", &v[0], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##Y", &v[1], step, min, max, format, flags);
            PopItemWidth();
            EndGroup();
        }
        else {
            res = SliderFloat2("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2*)v;
        return true;
    }

    bool ColumnSliderVec3(const char* label, vec3* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        float_t v[3];
        *(vec3*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
            ImGuiStyle& style = GetStyle();
            BeginGroup();
            PushMultiItemsWidths(3, CalcItemWidth());
            res |= SliderFloatButton("##X", &v[0], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##Y", &v[1], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##Z", &v[2], step, min, max, format, flags);
            PopItemWidth();
            EndGroup();
        }
        else {
            res = SliderFloat3("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
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

    bool ColumnSliderVec4(const char* label, vec4* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        float_t v[4];
        *(vec4*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
            ImGuiStyle& style = GetStyle();
            BeginGroup();
            PushMultiItemsWidths(4, CalcItemWidth());
            res |= SliderFloatButton("##X", &v[0], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##Y", &v[1], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##Z", &v[2], step, min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderFloatButton("##W", &v[3], step, min, max, format, flags);
            PopItemWidth();
            EndGroup();
        }
        else {
            res = SliderFloat4("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
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

    bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        int32_t v[1];
        v[0] = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button)
            res = SliderIntButton("", v, min, max, format, flags);
        else {
            res = SliderInt("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp(v[0], min, max);
        if (v[0] == *val)
            return false;

        *val = v[0];
        return true;
    }

    bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        int32_t v[2];
        *(vec2i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
            ImGuiStyle& style = GetStyle();
            BeginGroup();
            PushMultiItemsWidths(4, CalcItemWidth());
            res |= SliderIntButton("##X", &v[0], min, max, format, flags);
            PopItemWidth();
            SameLine(0.0f, style.ItemInnerSpacing.x);
            res |= SliderIntButton("##Y", &v[1], min, max, format, flags);
            PopItemWidth();

            EndGroup();
        }
        else {
            res = SliderInt2("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
        if (!res)
            return false;

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        if (v[0] == val->x && v[1] == val->y)
            return false;

        *val = *(vec2i*)v;
        return true;
    }

    bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        int32_t v[3];
        *(vec3i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
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
        }
        else {
            res = SliderInt3("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
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

    bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button) {
        int32_t v[4];
        *(vec4i*)v = *val;
        StartPropertyColumn(label);
        bool res = false;
        ResetIsItemAccum();
        if (button) {
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
        }
        else {
            res = SliderInt4("", v, min, max, format, flags);
            GetIsItemAccum();
        }
        EndPropertyColumn();
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

        v[0] = 1 << clamp(v[0], min, max);
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

        v[0] = 1 << clamp(v[0], min, max);
        v[1] = 1 << clamp(v[1], min, max);
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

        v[0] = 1 << clamp(v[0], min, max);
        v[1] = 1 << clamp(v[1], min, max);
        v[2] = 1 << clamp(v[2], min, max);
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

        v[0] = 1 << clamp(v[0], min, max);
        v[1] = 1 << clamp(v[1], min, max);
        v[2] = 1 << clamp(v[2], min, max);
        v[3] = 1 << clamp(v[3], min, max);
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
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

        v[0] = clamp(v[0], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
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

        v[0] = clamp(v[0], min, max);
        v[1] = clamp(v[1], min, max);
        v[2] = clamp(v[2], min, max);
        v[3] = clamp(v[3], min, max);
        if (v[0] == val->x && v[1] == val->y && v[2] == val->z && v[3] == val->w)
            return false;

        *val = *(vec4i*)v;
        return true;
    }
}