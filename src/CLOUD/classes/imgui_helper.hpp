/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glad/glad.h>
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

    extern bool ItemKeyDown(int32_t key);
    extern bool ItemKeyPressed(int32_t key, bool repeat);
    extern bool ItemKeyReleased(int32_t key);
    extern float_t GetColumnSpace();
    extern void SetColumnSpace(float_t val);
    extern void SetDefaultColumnSpace();
    extern void DisableElementPush(bool enable);
    extern void DisableElementPop(bool enable);
    extern float_t GetContentRegionAvailWidth();
    extern float_t GetContentRegionAvailHeight();
    extern void GetContentRegionAvailSetNextItemWidth();

    extern void StartPropertyColumn(const char* label);
    extern void EndPropertyColumn();

    bool SliderFloatButton(const char* label, float_t* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
    bool SliderIntButton(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);

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
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec2(const char* label, vec2* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec3(const char* label, vec3* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec4(const char* label, vec4* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragInt(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec2I(const char* label, vec2i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec3I(const char* label, vec3i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnDragVec4I(const char* label, vec4i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnInputText(const char* label, char* buf, size_t buf_size,
        ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
    extern bool ColumnSliderFloat(const char* label, float_t* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec2(const char* label, vec2* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec3(const char* label, vec3* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec4(const char* label, vec4* val, float_t step,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button = true);
    extern bool ColumnSliderLogInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnSliderLogVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnSliderLogVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnSliderLogVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
    extern bool ColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
        const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);
    extern bool ColumnDragFloat(const char* label, float_t* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec2(const char* label, vec2* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec3(const char* label, vec3* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec4(const char* label, vec4* val, float_t speed,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragInt(const char* label, int32_t* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec2I(const char* label, vec2i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec3I(const char* label, vec3i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnDragVec4I(const char* label, vec4i* val, float_t speed,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderFloat(const char* label, float_t* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec2(const char* label, vec2* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec3(const char* label, vec3* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec4(const char* label, vec4* val,
        float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderInt(const char* label, int32_t* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec2I(const char* label, vec2i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec3I(const char* label, vec3i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
    extern bool ColumnSliderVec4I(const char* label, vec4i* val,
        int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
}