/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <GLFW/glfw3.h>

namespace ImGui {
    extern const float_t AlphaDisabledScale;

    extern bool IsItemActiveAccum;
    extern bool IsItemFocusedAccum;
    extern bool IsItemVisibleAccum;
    extern bool IsItemEditedAccum;
    extern bool IsItemActivatedAccum;
    extern bool IsItemDeactivatedAccum;
    extern bool IsItemDeactivatedAfterEditAccum;
    extern bool IsItemToggledOpenAccum;

    extern float_t ColumnSpace;

    inline bool ItemKeyDown(ImGuiKey key) {
        return IsItemFocused() && IsKeyDown(key);
    }

    inline bool ItemKeyPressed(ImGuiKey key, bool repeat = true) {
        return IsItemFocused() && IsKeyPressed(key, repeat);
    }

    inline bool ItemKeyReleased(ImGuiKey key) {
        return IsItemFocused() && IsKeyReleased(key);
    }

    inline float_t GetColumnSpace() {
        return ColumnSpace;
    }

    inline void SetColumnSpace(float_t val) {
        ColumnSpace = val;
    }

    inline void SetDefaultColumnSpace() {
        ColumnSpace = (float_t)(1.0 / 3.0);
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

    extern ImVec2 CalcEmptyGlyphSize();

    inline float_t CalcEmptyGlyphWidth() {
        return CalcEmptyGlyphSize().x;
    }

    inline float_t CalcEmptyGlyphHeight() {
        return CalcEmptyGlyphSize().y;
    }

    extern void StartPropertyColumn(const char* label);
    extern void EndPropertyColumn();

    extern bool SliderFloatButton(const char* label, float_t* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format, ImGuiSliderFlags flags = 0);
    extern bool SliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags = 0);

    extern bool ButtonEnterKeyPressed(const char* label, const ImVec2& size = ImVec2(0, 0));
    extern bool ButtonExEnterKeyPressed(const char* label, const ImVec2& size = ImVec2(0, 0), ImGuiButtonFlags flags = 0);

    extern bool CheckboxEnterKeyPressed(const char* label, bool* v);
    extern bool CheckboxFlagsEnterKeyPressed(const char* label, int* flags, int flags_value);
    extern bool CheckboxFlagsEnterKeyPressed(const char* label, unsigned int* flags, unsigned int flags_value);

    extern bool ComboBox(const char* label, const char** items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ComboBox(const char* label, const char** items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ComboBoxString(const char* label, std::string* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ComboBoxString(const char* label, std::string* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ComboBoxConfigFile(const char* label, void* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ComboBoxConfigFile(const char* label, void* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);

    extern bool ColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags);
    extern bool ColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags);

    extern bool ColumnComboBox(const char* label, const char** items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ColumnComboBox(const char* label, const char** items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ColumnComboBoxString(const char* label, std::string* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ColumnComboBoxString(const char* label, std::string* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
        int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
    extern bool ColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
        size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);

    extern bool ColumnDragFloat(const char* label, float_t* val, float_t speed,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x01);
    extern bool ColumnDragVec2(const char* label, vec2* val, float_t speed,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x03);
    extern bool ColumnDragVec3(const char* label, vec3* val, float_t speed,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x07);
    extern bool ColumnDragVec4(const char* label, vec4* val, float_t speed,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x09);

    extern bool ColumnDragInt(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x01);
    extern bool ColumnDragVec2I(const char* label, vec2i* val, float_t speed,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x03);
    extern bool ColumnDragVec3I(const char* label, vec3i* val, float_t speed,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x07);
    extern bool ColumnDragVec4I(const char* label, vec4i* val, float_t speed,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x09);

    extern bool ColumnInputText(const char* label, char* buf, size_t buf_size,
        ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);

    extern bool ColumnSliderFloat(const char* label, float_t* val,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x01);
    extern bool ColumnSliderVec2(const char* label, vec2* val,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x03);
    extern bool ColumnSliderVec3(const char* label, vec3* val,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x07);
    extern bool ColumnSliderVec4(const char* label, vec4* val,
        float_t min, float_t max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x09);

    extern bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x01);
    extern bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x03);
    extern bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x07);
    extern bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0, int32_t bit_flag = 0x09);

    extern bool ColumnSliderFloatButton(const char* label, float_t* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec2Button(const char* label, vec2* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec3Button(const char* label, vec3* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec4Button(const char* label, vec4* val, float_t button_step,
        float_t min, float_t max, float_t step, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

    extern bool ColumnSliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec2IButton(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec3IButton(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderVec4IButton(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);

    extern bool ColumnSliderLogInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderLogVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderLogVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    extern bool ColumnSliderLogVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format = "%d", ImGuiSliderFlags flags = 0);

    extern bool ColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
        const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);

    extern void TextCentered(_In_z_ _Printf_format_string_ const char* const fmt, ...);
    extern void TextRight(_In_z_ _Printf_format_string_ const char* const fmt, ...);
}
