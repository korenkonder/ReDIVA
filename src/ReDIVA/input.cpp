/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.hpp"
#include <map>
#include "../KKdLib/vec.hpp"
#include "../CRE/timer.hpp"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <timeapi.h>

double_t input_movement_speed = 0.1;
double_t input_rotation_sensitivity = 0.5;
double_t input_move_x;
double_t input_move_y;
double_t input_rotate_x;
double_t input_rotate_y;
double_t input_roll;
bool input_reset;
bool input_reset_mouse_position;
bool input_locked;

extern bool close;
extern lock render_lock;
extern timer* render_timer;
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

    std::vector<Key> keys;
    std::vector<MouseButton> mouse_buttons;
    POINT pos;

    std::vector<Key> keys_prev;
    std::vector<MouseButton> mouse_buttons_prev;
    POINT pos_prev;

    static bool is_key_mod(int32_t key);
    static bool is_key_numpad(int32_t key);
    static void KeyboardCallback(GLFWwindow* window,
        int32_t key, int32_t scancode, int32_t action, int32_t mods);
    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods);
    static Key* vector_Key_find(std::vector<Key>& keys, int32_t key);
    static MouseButton* vector_MouseButton_find(
        std::vector<MouseButton>& mouse_buttons, int32_t button);

    bool IsKeyDown(int32_t key, int32_t mods) {
        Key* k = vector_Key_find(keys, key);
        if (k && (mods == -1 || k->mods == mods))
            return k->action != GLFW_RELEASE;
        return false;
    }

    bool IsKeyUp(int32_t key, int32_t mods) {
        Key* k = vector_Key_find(keys, key);
        if (k && (mods == -1 || k->mods == mods))
            return k->action == GLFW_RELEASE;
        return false;
    }

    bool IsKeyTapped(int32_t key, int32_t mods) {
        Key* k = vector_Key_find(keys, key);
        Key* k_prev = vector_Key_find(keys_prev, key);
        if (k)
            if (k_prev) {
                if (mods == -1 || (k->mods == mods && k_prev->mods == mods))
                    return k->action != GLFW_RELEASE && k_prev->action == GLFW_RELEASE;
            }
            else {
                if (mods == -1 || k->mods == mods)
                    return k->action != GLFW_RELEASE;
            }
        return false;
    }

    bool IsKeyReleased(int32_t key, int32_t mods) {
        Key* k = vector_Key_find(keys, key);
        Key* k_prev = vector_Key_find(keys_prev, key);
        if (k)
            if (k_prev) {
                if ((k->mods == mods && k_prev->mods == mods))
                    return k->action == GLFW_RELEASE && k_prev->action != GLFW_RELEASE;
            }
            else {
                if (mods == -1 || k->mods == mods)
                    return k->action == GLFW_RELEASE;
            }
        return false;
    }

    bool IsMouseButtonDown(int32_t button, int32_t mods) {
        MouseButton* mb = vector_MouseButton_find(mouse_buttons, button);
        if (mb && (mods == -1 || mb->mods == mods))
            return mb->action != GLFW_RELEASE;
        return false;
    }

    bool IsMouseButtonUp(int32_t button, int32_t mods) {
        MouseButton* mb = vector_MouseButton_find(mouse_buttons, button);
        if (mb && (mods == -1 || mb->mods == mods))
            return mb->action == GLFW_RELEASE;
        return false;
    }

    bool IsMouseButtonTapped(int32_t button, int32_t mods) {
        MouseButton* mb = vector_MouseButton_find(mouse_buttons, button);
        MouseButton* mb_prev = vector_MouseButton_find(mouse_buttons_prev, button);
        if (mb)
            if (mb_prev) {
                if ((mods == -1 || mb->mods == mods && mb_prev->mods == mods))
                    return mb->action != GLFW_RELEASE && mb_prev->action == GLFW_RELEASE;
            }
            else {
                if (mods == -1 || mb->mods == mods)
                    return mb->action != GLFW_RELEASE;
            }
        return false;
    }

    bool IsMouseButtonReleased(int32_t button, int32_t mods) {
        MouseButton* mb = vector_MouseButton_find(mouse_buttons, button);
        MouseButton* mb_prev = vector_MouseButton_find(mouse_buttons_prev, button);
        if (mb)
            if (mb_prev) {
                if ((mods == -1 || mb->mods == mods && mb_prev->mods == mods))
                    return mb->action == GLFW_RELEASE && mb_prev->action != GLFW_RELEASE;
            }
            else {
                if (mods == -1 || mb->mods == mods)
                    return mb->action == GLFW_RELEASE;
            }
        return true;
    }

    bool WasKeyDown(int32_t key, int32_t mods) {
        Key* k_prev = vector_Key_find(keys_prev, key);
        if (k_prev && (mods == -1 || k_prev->mods == mods))
            return k_prev->action != GLFW_RELEASE;
        return false;
    }

    bool WasKeyUp(int32_t key, int32_t mods) {
        Key* k_prev = vector_Key_find(keys_prev, key);
        if (k_prev && (mods == -1 || k_prev->mods == mods))
                return k_prev->action == GLFW_RELEASE;
        return true;
    }

    bool WasMouseButtonDown(int32_t button, int32_t mods) {
        MouseButton* mb_prev = vector_MouseButton_find(mouse_buttons_prev, button);
        if (mb_prev && (mods == -1 || mb_prev->mods == mods))
            return mb_prev->action != GLFW_RELEASE;
        return true;
    }

    bool WasMouseButtonUp(int32_t button, int32_t mods) {
        MouseButton* mb_prev = vector_MouseButton_find(mouse_buttons_prev, button);
        if (mb_prev && (mods == -1 || mb_prev->mods == mods))
            return mb_prev->action == GLFW_RELEASE;
        return true;
    }

    void EndFrame() {
        keys_prev = keys;
        mouse_buttons_prev = mouse_buttons;
    }

    void NewFrame() {
        pos_prev = pos;

        if (window_handle) {
            GetCursorPos(&pos);
            ScreenToClient(window_handle, &pos);
        }

        input_move_x = 0.0;
        input_move_y = 0.0;
        input_rotate_x = 0.0;
        input_rotate_y = 0.0;
        input_roll = 0.0;

        if (!window_handle || window_handle != GetForegroundWindow())
            return;

        double_t freq = render_timer->get_freq();
        double_t freq_hist = render_timer->get_freq_hist();
        double_t frame_speed = freq / freq_hist;

        classes_process_input(classes, classes_count);

        if (input_locked) {
            input_reset_mouse_position = true;
            input_locked = false;
            return;
        }

        double_t speed = input_movement_speed;
        double_t speed_fast = input_movement_speed * 10.0;
        double_t speed_slow = input_movement_speed / 10.0;
        speed *= frame_speed;
        speed_fast *= frame_speed;
        speed_slow *= frame_speed;

        if (IsKeyDown(GLFW_KEY_W) || IsKeyDown(GLFW_KEY_W, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_move_x += speed;
        else if (IsKeyDown(GLFW_KEY_W, GLFW_MOD_SHIFT))
            input_move_x += speed_fast;
        else if (IsKeyDown(GLFW_KEY_W, GLFW_MOD_CONTROL))
            input_move_x += speed_slow;
        else if (IsKeyDown(GLFW_KEY_W, -1))
            input_move_x += speed;

        if (IsKeyDown(GLFW_KEY_S) || IsKeyDown(GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_move_x -= speed;
        else if (IsKeyDown(GLFW_KEY_S, GLFW_MOD_SHIFT))
            input_move_x -= speed_fast;
        else if (IsKeyDown(GLFW_KEY_S, GLFW_MOD_CONTROL))
            input_move_x -= speed_slow;
        else if (IsKeyDown(GLFW_KEY_S, -1))
            input_move_x -= speed;

        if (IsKeyDown(GLFW_KEY_A) || IsKeyDown(GLFW_KEY_A, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_move_y -= speed;
        else if (IsKeyDown(GLFW_KEY_A, GLFW_MOD_SHIFT))
            input_move_y -= speed_fast;
        else if (IsKeyDown(GLFW_KEY_A, GLFW_MOD_CONTROL))
            input_move_y -= speed_slow;
        else if (IsKeyDown(GLFW_KEY_A, -1))
            input_move_y -= speed;

        if (IsKeyDown(GLFW_KEY_D) || IsKeyDown(GLFW_KEY_D, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_move_y += speed;
        else if (IsKeyDown(GLFW_KEY_D, GLFW_MOD_SHIFT))
            input_move_y += speed_fast;
        else if (IsKeyDown(GLFW_KEY_D, GLFW_MOD_CONTROL))
            input_move_y += speed_slow;
        else if (IsKeyDown(GLFW_KEY_D, -1))
            input_move_y += speed;

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
            input_rotate_y += speed;
        else if (IsKeyDown(GLFW_KEY_UP, GLFW_MOD_SHIFT))
            input_rotate_y += speed_fast;
        else if (IsKeyDown(GLFW_KEY_UP, GLFW_MOD_CONTROL))
            input_rotate_y += speed_slow;
        else if (IsKeyDown(GLFW_KEY_UP, -1))
            input_rotate_y += speed;

        if (IsKeyDown(GLFW_KEY_DOWN) || IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_rotate_y -= speed;
        else if (IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_SHIFT))
            input_rotate_y -= speed_fast;
        else if (IsKeyDown(GLFW_KEY_DOWN, GLFW_MOD_CONTROL))
            input_rotate_y -= speed_slow;
        else if (IsKeyDown(GLFW_KEY_DOWN, -1))
            input_rotate_y -= speed;

        if (IsKeyDown(GLFW_KEY_LEFT) || IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_rotate_x -= speed;
        else if (IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_SHIFT))
            input_rotate_x -= speed_fast;
        else if (IsKeyDown(GLFW_KEY_LEFT, GLFW_MOD_CONTROL))
            input_rotate_x -= speed_slow;
        else if (IsKeyDown(GLFW_KEY_LEFT, -1))
            input_rotate_x -= speed;

        if (IsKeyDown(GLFW_KEY_RIGHT) || IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_rotate_x += speed;
        else if (IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_SHIFT))
            input_rotate_x += speed_fast;
        else if (IsKeyDown(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL))
            input_rotate_x += speed_slow;
        else if (IsKeyDown(GLFW_KEY_RIGHT, -1))
            input_rotate_x += speed;

        if (IsKeyDown(GLFW_KEY_Q) || IsKeyDown(GLFW_KEY_Q, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_roll += speed;
        else if (IsKeyDown(GLFW_KEY_Q, GLFW_MOD_SHIFT))
            input_roll += speed_fast;
        else if (IsKeyDown(GLFW_KEY_Q, GLFW_MOD_CONTROL))
            input_roll += speed_slow;
        else if (IsKeyDown(GLFW_KEY_Q, -1))
            input_roll += speed;

        if (IsKeyDown(GLFW_KEY_E) || IsKeyDown(GLFW_KEY_E, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            input_roll -= speed;
        else if (IsKeyDown(GLFW_KEY_E, GLFW_MOD_SHIFT))
            input_roll -= speed_fast;
        else if (IsKeyDown(GLFW_KEY_E, GLFW_MOD_CONTROL))
            input_roll -= speed_slow;
        else if (IsKeyDown(GLFW_KEY_E, -1))
            input_roll -= speed;

        if (IsKeyDown(GLFW_KEY_R))
            input_reset = true;

        if (IsMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE, -1)) {
            if (!input_reset_mouse_position) {
                input_rotate_y += ((double_t)pos_prev.y - (double_t)pos.y) * input_rotation_sensitivity;
                input_rotate_x += ((double_t)pos.x - (double_t)pos_prev.x) * input_rotation_sensitivity;
            }
            input_reset_mouse_position = false;
        }
        else
            input_reset_mouse_position = true;
        input_locked = false;
    }

    void SetInputs(GLFWwindow* window) {
        glfwSetKeyCallback(window, (GLFWkeyfun)KeyboardCallback);
        glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseButtonCallback);
    }

    static bool is_key_mod(int32_t key) {
        switch (key) {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_RIGHT_SHIFT:
        case GLFW_KEY_RIGHT_CONTROL:
        case GLFW_KEY_RIGHT_ALT:
        case GLFW_KEY_RIGHT_SUPER:
            return true;
        default:
            return false;
        }
    }

    static bool is_key_numpad(int32_t key) {
        switch (key) {
        case GLFW_KEY_KP_0:
        case GLFW_KEY_KP_1:
        case GLFW_KEY_KP_2:
        case GLFW_KEY_KP_3:
        case GLFW_KEY_KP_4:
        case GLFW_KEY_KP_5:
        case GLFW_KEY_KP_6:
        case GLFW_KEY_KP_7:
        case GLFW_KEY_KP_8:
        case GLFW_KEY_KP_9:
            return true;
        default:
            return false;
        }
    }

    static void KeyboardCallback(GLFWwindow* window,
        int32_t key, int32_t scancode, int32_t action, int32_t mods) {
        if (mods & GLFW_MOD_CAPS_LOCK)
            mods &= ~GLFW_MOD_CAPS_LOCK;

        if (mods & GLFW_MOD_NUM_LOCK)
            mods &= ~GLFW_MOD_NUM_LOCK;

        bool is_mod = false;
        switch (key) {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            mods = GLFW_MOD_SHIFT;
            is_mod = true;
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            mods = GLFW_MOD_CONTROL;
            is_mod = true;
            break;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            mods = GLFW_MOD_ALT;
            is_mod = true;
            break;
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_RIGHT_SUPER:
            mods = GLFW_MOD_SUPER;
            is_mod = true;
            break;
        case GLFW_MOD_CAPS_LOCK:
            mods = 0;
            break;
        case GLFW_KEY_NUM_LOCK:
            mods = 0;
            break;
        }

        if (is_mod) {
            if (action != GLFW_RELEASE) {
                for (Key& i : keys)
                    if (!is_key_mod(i.key))
                        i.mods |= mods;

                for (MouseButton& i : mouse_buttons)
                    i.mods |= mods;
            }
            else {
                mods = ~mods;
                for (Key& i : keys)
                    if (!is_key_mod(i.key))
                        i.mods &= mods;

                for (MouseButton& i : mouse_buttons)
                    i.mods &= mods;
            }
            mods = 0;
        }

        Key* k = vector_Key_find(keys, key);
        if (k)
            *k = { key, scancode, action, mods };
        else
            keys.push_back({ key, scancode, action, mods });
    }

    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods) {
        MouseButton* mb = vector_MouseButton_find(mouse_buttons, button);
        if (mb)
            *mb = { button, action, mods };
        else
            mouse_buttons.push_back({ button, action, mods });
    }

    static Key* vector_Key_find(std::vector<Key>& keys, int32_t key) {
        for (Key& i : keys)
            if (i.key == key)
                return &i;
        return 0;
    }

    static MouseButton* vector_MouseButton_find(
        std::vector<MouseButton>& mouse_buttons, int32_t button) {
        for (MouseButton& i : mouse_buttons)
            if (i.button == button)
                return &i;
        return 0;
    }
};
