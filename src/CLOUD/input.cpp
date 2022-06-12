/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.hpp"
#include <map>
#include "../KKdLib/vec.hpp"
#include "../CRE/timer.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <timeapi.h>

double_t input_movement_speed = 0.1;
double_t input_rotation_sensitivity = 0.5;
vec2d input_move;
vec2d input_rotate;
double_t input_roll;
bool input_reset;
bool input_reset_mouse_position;
bool input_shaders_reload;
bool input_locked;

extern bool close;
extern lock render_lock;
extern timer render_timer;
extern HWND window_handle;
extern ImGuiContext* imgui_context;
extern lock imgui_context_lock;

namespace Input {
    struct Key {
        int32_t key;
        int32_t scancode;
        int32_t action;
        int32_t mods;
    };

    struct MouseButton {
        int32_t button;
        int32_t action;
        int32_t mods;
    };

    std::map<int32_t, Key> keys;
    std::map<int32_t, MouseButton> mouse_buttons;
    POINT pos;

    std::map<int32_t, Key> keys_prev;
    std::map<int32_t, MouseButton> mouse_buttons_prev;
    POINT pos_prev;

    static void KeyboardCallback(GLFWwindow* window,
        int32_t key, int32_t scancode, int32_t action, int32_t mods);
    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods);

    bool IsKeyDown(int32_t key, int32_t mods) {
        auto elem = keys.find(key);
        if (elem != keys.end())
            if (elem->second.mods == mods)
                return elem->second.action != GLFW_RELEASE;
        return false;
    }

    bool IsKeyUp(int32_t key, int32_t mods) {
        auto elem = keys.find(key);
        if (elem != keys.end())
            if (elem->second.mods == mods)
                return elem->second.action == GLFW_RELEASE;
        return false;
    }

    bool IsKeyTapped(int32_t key, int32_t mods) {
        auto elem = keys.find(key);
        auto elem_prev = keys_prev.find(key);
        if (elem != keys.end())
            if (elem_prev != keys_prev.end()) {
                if (elem->second.mods == mods && elem_prev->second.mods == mods)
                    return elem->second.action != GLFW_RELEASE && elem_prev->second.action == GLFW_RELEASE;
            }
            else {
                if (elem->second.mods == mods)
                    return elem->second.action != GLFW_RELEASE;
            }
        return false;
    }

    bool IsKeyReleased(int32_t key, int32_t mods) {
        auto elem = keys.find(key);
        auto elem_prev = keys_prev.find(key);
        if (elem != keys.end())
            if (elem_prev != keys_prev.end()) {
                if (elem->second.mods == mods && elem_prev->second.mods == mods)
                    return elem->second.action == GLFW_RELEASE && elem_prev->second.action != GLFW_RELEASE;
            }
            else {
                if (elem->second.mods == mods)
                    return elem->second.action == GLFW_RELEASE;
            }
        return false;
    }

    bool IsMouseButtonDown(int32_t button, int32_t mods) {
        auto elem = mouse_buttons.find(button);
        if (elem != mouse_buttons.end())
            if (elem->second.mods == mods)
                return elem->second.action != GLFW_RELEASE;
        return false;
    }

    bool IsMouseButtonUp(int32_t button, int32_t mods) {
        auto elem = mouse_buttons.find(button);
        if (elem != mouse_buttons.end())
            if (elem->second.mods == mods)
                return elem->second.action == GLFW_RELEASE;
        return false;
    }

    bool IsMouseButtonTapped(int32_t button, int32_t mods) {
        auto elem = mouse_buttons.find(button);
        auto elem_prev = mouse_buttons_prev.find(button);
        if (elem != mouse_buttons.end())
            if (elem_prev != mouse_buttons_prev.end()) {
                if (elem->second.mods == mods && elem_prev->second.mods == mods)
                    return elem->second.action != GLFW_RELEASE && elem_prev->second.action == GLFW_RELEASE;
            }
            else {
                if (elem->second.mods == mods)
                    return elem->second.action != GLFW_RELEASE;
            }
        return false;
    }

    bool IsMouseButtonReleased(int32_t button, int32_t mods) {
        auto elem = mouse_buttons.find(button);
        auto elem_prev = mouse_buttons_prev.find(button);
        if (elem != mouse_buttons.end())
            if (elem_prev != mouse_buttons_prev.end()) {
                if (elem->second.mods == mods && elem_prev->second.mods == mods)
                    return elem->second.action == GLFW_RELEASE && elem_prev->second.action != GLFW_RELEASE;
            }
            else {
                if (elem->second.mods == mods)
                    return elem->second.action == GLFW_RELEASE;
            }
        return true;
    }

    bool WasKeyDown(int32_t key, int32_t mods) {
        auto elem_prev = keys_prev.find(key);
        if (elem_prev != keys_prev.end())
            if (elem_prev->second.mods == mods)
                return elem_prev->second.action != GLFW_RELEASE;
        return false;
    }

    bool WasKeyUp(int32_t key, int32_t mods) {
        auto elem_prev = keys_prev.find(key);
        if (elem_prev != keys_prev.end())
            if (elem_prev->second.mods == mods)
                return elem_prev->second.action == GLFW_RELEASE;
        return true;
    }

    bool WasMouseButtonDown(int32_t button, int32_t mods) {
        auto elem_prev = mouse_buttons_prev.find(button);
        if (elem_prev != mouse_buttons_prev.end())
            if (elem_prev->second.mods == mods)
                return elem_prev->second.action != GLFW_RELEASE;
        return true;
    }

    bool WasMouseButtonUp(int32_t button, int32_t mods) {
        auto elem_prev = mouse_buttons_prev.find(button);
        if (elem_prev != mouse_buttons_prev.end())
            if (elem_prev->second.mods == mods)
                return elem_prev->second.action == GLFW_RELEASE;
        return true;
    }

    void EndFrame() {
        for (auto& i : keys)
            keys_prev.insert_or_assign(i.first, i.second);
        for (auto& i : mouse_buttons)
            mouse_buttons_prev.insert_or_assign(i.first, i.second);
    }

    void NewFrame() {
        pos_prev = pos;

        if (window_handle) {
            GetCursorPos(&pos);
            ScreenToClient(window_handle, &pos);
        }

        input_move.x = 0.0;
        input_move.y = 0.0;
        input_rotate.x = 0.0;
        input_rotate.y = 0.0;
        input_roll = 0.0;

        if (!window_handle || window_handle != GetForegroundWindow())
            return;

        double_t freq = timer_get_freq(&render_timer);
        double_t freq_hist = timer_get_freq_hist(&render_timer);
        double_t frame_speed = freq / freq_hist;

        input_locked = false;
        classes_process_input(classes, classes_count);

        if (!input_locked) {
            double_t speed = input_movement_speed;
            double_t speed_fast = input_movement_speed * 10.0;
            double_t speed_slow = input_movement_speed / 10.0;
            speed *= frame_speed;
            speed_fast *= frame_speed;
            speed_slow *= frame_speed;

            if (IsKeyDown(GLFW_KEY_W) || IsKeyDown(GLFW_KEY_W, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_move.x += speed;
            else if (IsKeyDown(GLFW_KEY_W, GLFW_MOD_SHIFT))
                input_move.x += speed_fast;
            else if (IsKeyDown(GLFW_KEY_W, GLFW_MOD_CONTROL))
                input_move.x += speed_slow;

            if (IsKeyDown(GLFW_KEY_S) || IsKeyDown(GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_move.x -= speed;
            else if (IsKeyDown(GLFW_KEY_S, GLFW_MOD_SHIFT))
                input_move.x -= speed_fast;
            else if (IsKeyDown(GLFW_KEY_S, GLFW_MOD_CONTROL))
                input_move.x -= speed_slow;

            if (IsKeyDown(GLFW_KEY_A) || IsKeyDown(GLFW_KEY_A, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_move.y -= speed;
            else if (IsKeyDown(GLFW_KEY_A, GLFW_MOD_SHIFT))
                input_move.y -= speed_fast;
            else if (IsKeyDown(GLFW_KEY_A, GLFW_MOD_CONTROL))
                input_move.y -= speed_slow;

            if (IsKeyDown(GLFW_KEY_D) || IsKeyDown(GLFW_KEY_D, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_move.y += speed;
            else if (IsKeyDown(GLFW_KEY_D, GLFW_MOD_SHIFT))
                input_move.y += speed_fast;
            else if (IsKeyDown(GLFW_KEY_D, GLFW_MOD_CONTROL))
                input_move.y += speed_slow;

            speed = input_movement_speed;
            speed_fast = input_movement_speed * 2.0;
            speed_slow = input_movement_speed / 2.0;
            speed *= frame_speed;
            speed_fast *= frame_speed;
            speed_slow *= frame_speed;
            speed *= freq / freq_hist;
            speed_fast *= freq / freq_hist;
            speed_slow *= freq / freq_hist;
            speed *= 10.0;
            speed_fast *= 10.0;
            speed_slow *= 10.0;

            if (IsKeyDown(GLFW_KEY_UP) || IsKeyDown(GLFW_KEY_UP, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_rotate.y += speed;
            else if (IsKeyDown(GLFW_KEY_UP, GLFW_MOD_SHIFT))
                input_rotate.y += speed_fast;
            else if (IsKeyDown(GLFW_KEY_UP, GLFW_MOD_CONTROL))
                input_rotate.y += speed_slow;

            if (IsKeyDown(GLFW_KEY_DOWN) || IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_rotate.y -= speed;
            else if (IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_SHIFT))
                input_rotate.y -= speed_fast;
            else if (IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_CONTROL))
                input_rotate.y -= speed_slow;

            if (IsKeyDown(GLFW_KEY_LEFT) || IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_rotate.x -= speed;
            else if (IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_SHIFT))
                input_rotate.x -= speed_fast;
            else if (IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_CONTROL))
                input_rotate.x -= speed_slow;

            if (IsKeyDown(GLFW_KEY_RIGHT) || IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_rotate.x += speed;
            else if (IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_SHIFT))
                input_rotate.x += speed_fast;
            else if (IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL))
                input_rotate.x += speed_slow;

            if (IsKeyDown(GLFW_KEY_Q) || IsKeyDown(GLFW_KEY_Q, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_roll += speed;
            else if (IsKeyDown(GLFW_KEY_Q, GLFW_MOD_SHIFT))
                input_roll += speed_fast;
            else if (IsKeyDown(GLFW_KEY_Q, GLFW_MOD_CONTROL))
                input_roll += speed_slow;

            if (IsKeyDown(GLFW_KEY_E) || IsKeyDown(GLFW_KEY_E, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
                input_roll += speed;
            else if (IsKeyDown(GLFW_KEY_E, GLFW_MOD_SHIFT))
                input_roll += speed_fast;
            else if (IsKeyDown(GLFW_KEY_E, GLFW_MOD_CONTROL))
                input_roll += speed_slow;

            if (IsKeyDown(GLFW_KEY_R, GLFW_MOD_CONTROL))
                input_shaders_reload = true;
            else if (IsKeyDown(GLFW_KEY_R))
                input_reset = true;
        }

        lock_lock(&imgui_context_lock);
        ImGui::SetCurrentContext(imgui_context);
        if (!ImGui::GetIO().WantCaptureMouse && Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE)) {
            if (!input_reset_mouse_position) {
                input_rotate.y += ((double_t)pos_prev.y - (double_t)pos.y) * input_rotation_sensitivity;
                input_rotate.x += ((double_t)pos.x - (double_t)pos_prev.x) * input_rotation_sensitivity;
            }
            input_reset_mouse_position = false;
        }
        else
            input_reset_mouse_position = true;
        lock_unlock(&imgui_context_lock);
    }

    void SetInputs(GLFWwindow* window) {
        glfwSetKeyCallback(window, (GLFWkeyfun)KeyboardCallback);
        glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseButtonCallback);
    }

    static void KeyboardCallback(GLFWwindow* window,
        int32_t key, int32_t scancode, int32_t action, int32_t mods) {
        auto elem = keys.find(key);
        if (elem != keys.end())
            elem->second = { key, scancode, action, mods };
        else
            keys.insert({ key, { key, scancode, action, mods } });
    }

    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods) {
        auto elem = mouse_buttons.find(button);
        if (elem != mouse_buttons.end())
            elem->second = { button, action, mods };
        else
            mouse_buttons.insert({ button, { button, action, mods } });
    }
};
