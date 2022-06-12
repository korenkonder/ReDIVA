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

extern const float_t imgui_alpha_disabled_scale;

extern bool imgui_is_item_active;
extern bool imgui_is_item_focused;
extern bool imgui_is_item_visible;
extern bool imgui_is_item_edited;
extern bool imgui_is_item_activated;
extern bool imgui_is_item_deactivated;
extern bool imgui_is_item_deactivated_after_edit;
extern bool imgui_is_item_toggled_open;

extern bool imguiItemKeyDown(int32_t key);
extern bool imguiItemKeyPressed(int32_t key, bool repeat);
extern bool imguiItemKeyReleased(int32_t key);
extern float_t imguiGetColumnSpace();
extern void imguiSetColumnSpace(float_t val);
extern void imguiSetDefaultColumnSpace();
extern void imguiDisableElementPush(bool enable);
extern void imguiDisableElementPop(bool enable);
extern float_t imguiGetContentRegionAvailWidth();
extern float_t imguiGetContentRegionAvailHeight();
extern void imguiGetContentRegionAvailSetNextItemWidth();

extern void imguiStartPropertyColumn(const char* label);
extern void imguiEndPropertyColumn();

bool igSliderFloatButton(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
bool igSliderIntButton(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);

extern bool imguiButton(const char* label, const ImVec2& size = ImVec2(0, 0));
extern bool imguiButtonEx(const char* label, const ImVec2& size = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
extern bool imguiCheckbox(const char* label, bool* v);
extern bool imguiCheckboxFlags(const char* label, int* flags, int flags_value);
extern bool imguiCheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
extern bool imguiComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiComboBox(const char* label, const char** items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiComboBoxString(const char* label, std::string* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiComboBoxString(const char* label, std::string* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiComboBoxConfigFile(const char* label, void* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiComboBoxConfigFile(const char* label, void* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnColorEdit3(const char* label, vec3* val, ImGuiColorEditFlags flags);
extern bool imguiColumnColorEdit4(const char* label, vec4* val, ImGuiColorEditFlags flags);
extern bool imguiColumnComboBox(const char* label, const char** items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnComboBox(const char* label, const char** items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnComboBoxString(const char* label, std::string* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnComboBoxString(const char* label, std::string* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
    int32_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnComboBoxConfigFile(const char* label, void* items, const size_t size,
    size_t* selected_idx, ImGuiComboFlags flags, bool include_last, bool* focus);
extern bool imguiColumnDragFloat(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec2(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec3(const char* label, vec3* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec4(const char* label, vec4* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragInt(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec2I(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec3I(const char* label, vec3i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnDragVec4I(const char* label, vec4i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnInputText(const char* label, char* buf, size_t buf_size,
    ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
extern bool imguiColumnSliderFloat(const char* label, float_t* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec2(const char* label, vec2* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec3(const char* label, vec3* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec4(const char* label, vec4* val, float_t step,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderInt(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec2I(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec3I(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderVec4I(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, bool button);
extern bool imguiColumnSliderLogInt(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnSliderLogVec2I(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnSliderLogVec3I(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnSliderLogVec4I(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags);
extern bool imguiColumnInputScalar(const char* label, ImGuiDataType data_type, void* p_data,
    const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);
extern bool imguiColumnDragFloatFlag(const char* label, float_t* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec2Flag(const char* label, vec2* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec3Flag(const char* label, vec3* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec4Flag(const char* label, vec4* val, float_t speed,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragIntFlag(const char* label, int32_t* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec2IFlag(const char* label, vec2i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec3IFlag(const char* label, vec3i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnDragVec4IFlag(const char* label, vec4i* val, float_t speed,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderFloatFlag(const char* label, float_t* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec2Flag(const char* label, vec2* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec3Flag(const char* label, vec3* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec4Flag(const char* label, vec4* val,
    float_t min, float_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderIntFlag(const char* label, int32_t* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec2IFlag(const char* label, vec2i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec3IFlag(const char* label, vec3i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
extern bool imguiColumnSliderVec4IFlag(const char* label, vec4i* val,
    int32_t min, int32_t max, const char* format, ImGuiSliderFlags flags, int32_t bit_flag);
