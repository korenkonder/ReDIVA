/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.hpp"
#include "../KKdLib/timer.hpp"
#include "../KKdLib/vec.hpp"
#include "input_state.hpp"
#include <GLFW/glfw3.h>

double_t input_movement_speed = 0.1;
double_t input_rotation_sensitivity = 0.5;
double_t input_move_x;
double_t input_move_y;
double_t input_rotate_x;
double_t input_rotate_y;
double_t input_roll;
double_t input_scroll;
bool input_reset;
bool input_reset_mouse_position;
bool input_locked;

uint8_t disable_dw_input_update = false;
bool disable_cursor = false;

extern bool close;
extern timer* render_timer;
extern HWND window_handle;

extern void glut_get_modifiers();
extern void glut_reset_modifiers();

extern void glut_keydown_cb(uint8_t key, int32_t x, int32_t y);
extern void glut_keyup_cb(uint8_t key, int32_t x, int32_t y);
extern void glut_skeydown_cb(int32_t key, int32_t x, int32_t y);
extern void glut_skeyup_cb(int32_t key, int32_t x, int32_t y);
extern void glut_mouse_cb(int32_t bn, int32_t state, int32_t x, int32_t y);
extern void glut_motion_cb(int32_t x, int32_t y);

namespace Input {
    static vec2i pos;

    static bool is_key_mod(int32_t key);
    static bool is_key_numpad(int32_t key);

    inline static void process_button(const InputState* input_state, double_t& value, InputButton button,
        const double_t speed, const double_t speed_fast, const double_t speed_slow);

    static void CursorPosCallback(GLFWwindow* window,
        double_t xpos, double_t ypos);
    static void KeyboardCallback(GLFWwindow* window,
        int32_t key, int32_t scancode, int32_t action, int32_t mods);
    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods);
    static void ScrollCallback(GLFWwindow* window,
        double_t x_offset, double_t y_offset);

    void NewFrame() {
        input_move_x = 0.0;
        input_move_y = 0.0;
        input_rotate_x = 0.0;
        input_rotate_y = 0.0;
        input_roll = 0.0;

        const InputState* input_state = input_state_get(0);
        if (input_state->CheckTapped(INPUT_BUTTON_F3))
            disable_dw_input_update ^= 0x01;
        else if (input_state->CheckTapped(INPUT_BUTTON_F3) && input_state->CheckDown(INPUT_BUTTON_CONTROL))
            disable_cursor ^= true;

        if (input_locked)
            disable_dw_input_update |= 0x02;
        else
            disable_dw_input_update &= ~0x02;

        if (!window_handle || window_handle != GetForegroundWindow() || !disable_dw_input_update)
            return;

        double_t freq = render_timer->get_freq();
        double_t freq_hist = render_timer->get_freq_hist();
        double_t frame_speed = freq / freq_hist;

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

        process_button(input_state, input_move_x, INPUT_BUTTON_W,  speed,  speed_fast,  speed_slow);
        process_button(input_state, input_move_x, INPUT_BUTTON_S, -speed, -speed_fast, -speed_slow);
        process_button(input_state, input_move_y, INPUT_BUTTON_A, -speed, -speed_fast, -speed_slow);
        process_button(input_state, input_move_y, INPUT_BUTTON_D,  speed,  speed_fast,  speed_slow);

        speed = input_movement_speed;
        speed_fast = input_movement_speed * 2.0;
        speed_slow = input_movement_speed / 2.0;
        speed *= frame_speed;
        speed_fast *= frame_speed;
        speed_slow *= frame_speed;
        speed *= 10.0;
        speed_fast *= 10.0;
        speed_slow *= 10.0;

        process_button(input_state, input_rotate_y, INPUT_BUTTON_UP   ,  speed,  speed_fast,  speed_slow);
        process_button(input_state, input_rotate_y, INPUT_BUTTON_DOWN , -speed, -speed_fast, -speed_slow);
        process_button(input_state, input_rotate_x, INPUT_BUTTON_LEFT , -speed, -speed_fast, -speed_slow);
        process_button(input_state, input_rotate_x, INPUT_BUTTON_RIGHT,  speed,  speed_fast,  speed_slow);

        process_button(input_state, input_roll, INPUT_BUTTON_Q,  speed,  speed_fast,  speed_slow);
        process_button(input_state, input_roll, INPUT_BUTTON_E, -speed, -speed_fast, -speed_slow);

        if (input_state->CheckDown(INPUT_BUTTON_R))
            input_reset = true;

        if (input_state->CheckDown(INPUT_BUTTON_MOUSE_BUTTON_MIDDLE)) {
            if (!input_reset_mouse_position) {
                input_rotate_y += -input_state->sub_14018CCC0(11) * input_rotation_sensitivity;
                input_rotate_x += input_state->sub_14018CCC0(10) * input_rotation_sensitivity;
            }
            input_reset_mouse_position = false;
        }
        else
            input_reset_mouse_position = true;
        input_locked = false;
    }

    void SetCallbacks(GLFWwindow* window) {
        glfwSetCursorPosCallback(window, (GLFWcursorposfun)CursorPosCallback);
        glfwSetKeyCallback(window, (GLFWkeyfun)KeyboardCallback);
        glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseButtonCallback);
        glfwSetScrollCallback(window, (GLFWscrollfun)ScrollCallback);
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

    inline static void process_button(const InputState* input_state, double_t& value, InputButton button,
        const double_t speed, const double_t speed_fast, const double_t speed_slow) {
        if (input_state->CheckDown(button)
            && input_state->CheckDown(INPUT_BUTTON_CONTROL) && input_state->CheckDown(INPUT_BUTTON_SHIFT))
            value += speed;
        else if (input_state->CheckDown(button) && input_state->CheckDown(INPUT_BUTTON_SHIFT))
            value += speed_fast;
        else if (input_state->CheckDown(button) && input_state->CheckDown(INPUT_BUTTON_CONTROL))
            value += speed_slow;
        else if (input_state->CheckDown(button))
            value += speed;
    }

    static void CursorPosCallback(GLFWwindow* window,
        double_t xpos, double_t ypos) {
        pos.x = (int32_t)xpos;
        pos.y = (int32_t)ypos;

        glut_get_modifiers();
        glut_motion_cb(pos.x, pos.y);
        glut_reset_modifiers();
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

        if (is_mod)
            return;

        static const std::pair<int32_t, int32_t> glut_key_table[] = {
            { GLFW_KEY_ESCAPE   , 0x1B },
            { GLFW_KEY_BACKSPACE, '\b' },
            { GLFW_KEY_TAB      , '\t' },
            { GLFW_KEY_ENTER    , '\r' },
            { GLFW_KEY_SPACE    ,  ' ' },
            { GLFW_KEY_DELETE   , 0x7F },
        };

        static const std::pair<int32_t, int32_t> glut_skey_table[] = {
            { GLFW_KEY_F1       ,   1, },
            { GLFW_KEY_F2       ,   2, },
            { GLFW_KEY_F3       ,   3, },
            { GLFW_KEY_F4       ,   4, },
            { GLFW_KEY_F5       ,   5, },
            { GLFW_KEY_F6       ,   6, },
            { GLFW_KEY_F7       ,   7, },
            { GLFW_KEY_F8       ,   8, },
            { GLFW_KEY_F9       ,   9, },
            { GLFW_KEY_F10      ,  10, },
            { GLFW_KEY_F11      ,  11, },
            { GLFW_KEY_F12      ,  12, },
            { GLFW_KEY_LEFT     , 100, },
            { GLFW_KEY_UP       , 101, },
            { GLFW_KEY_RIGHT    , 102, },
            { GLFW_KEY_DOWN     , 103, },
            { GLFW_KEY_PAGE_UP  , 104, },
            { GLFW_KEY_PAGE_DOWN, 105, },
            { GLFW_KEY_HOME     , 106, },
            { GLFW_KEY_END      , 107, },
            { GLFW_KEY_INSERT   , 108, },
        };

        glut_get_modifiers();

        int32_t glut_key = -1;

        bool is_skey = false;
        for (const auto& i : glut_skey_table)
            if (i.first == key) {
                glut_key = i.second;
                is_skey = true;
                break;
            }

        if (is_skey) {
            switch (action) {
            case GLFW_RELEASE:
                glut_skeyup_cb(glut_key, pos.x, pos.y);
                break;
            case GLFW_PRESS:
                glut_skeydown_cb(glut_key, pos.x, pos.y);
                break;
            }
        }
        else {
            for (const auto& i : glut_key_table)
                if (i.first == key) {
                    glut_key = i.second;
                    break;
                }

            if (glut_key == -1
                && ((key >= '0' && key <= '9')
                    || (key >= 'A' && key <= 'Z')
                    || (key >= 'a' && key <= 'z')))
                glut_key = key;

            if (glut_key != -1)
                switch (action) {
                case GLFW_RELEASE:
                    glut_keyup_cb(glut_key, pos.x, pos.y);
                    break;
                case GLFW_PRESS:
                    glut_keydown_cb(glut_key, pos.x, pos.y);
                    break;
                }
        }

        glut_reset_modifiers();
    }

    static void MouseButtonCallback(GLFWwindow* window,
        int32_t button, int32_t action, int32_t mods) {
        int32_t bn = -1;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            bn = 0;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            bn = 1;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            bn = 2;

        int32_t state = action == GLFW_PRESS ? 0 : 1;

        glut_get_modifiers();
        glut_mouse_cb(bn, state, pos.x, pos.y);
        glut_reset_modifiers();
    }

    static void ScrollCallback(GLFWwindow* window,
        double_t x_offset, double_t y_offset) {
        glut_get_modifiers();
        glut_mouse_cb(y_offset < 0.0f ? 4 : 3, 0, pos.x, pos.y);
        glut_reset_modifiers();
    }
};
