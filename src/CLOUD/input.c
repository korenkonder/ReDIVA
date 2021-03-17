/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.h"
#include "../KKdLib/vec.h"
#include "../CRE/microui.h"

#define KEYBOARD_KEYS 0xFF

typedef struct key_state {
    bool key_states[KEYBOARD_KEYS];
} key_state;

typedef struct mouse_state {
    POINT position;
    double_t scroll_x;
    double_t scroll_y;
} mouse_state;

key_state input_key_current_state;
key_state input_key_last_state;

mouse_state input_mouse_current_state;
mouse_state input_mouse_last_state;

float_t input_movement_speed = 1.0f;
float_t input_rotation_sensitivity = 0.5f;
vec2 input_move;
vec2 input_rotate;
bool input_reset;
bool input_reset_mouse_position;

extern bool close;
HANDLE input_lock = 0;
extern HANDLE window_handle;
extern HANDLE mu_input_lock;

void input_poll();

#define FREQ 60
#include "../CRE/timer.h"
timer_val(input);

int32_t input_main(void* arg) {
    timer_init(input, "Input");
    input_lock = CreateMutexW(0, 0, L"Input");
    if (!input_lock)
        goto End;

    while (!close) {
        timer_calc_pre(input);
        WaitForSingleObject(input_lock, INFINITE);
        input_poll();
        ReleaseMutex(input_lock);
        double_t cycle_time = timer_calc_post(input);
        msleep(input_timer, 1000.0 / FREQ - cycle_time);
    }
    CloseHandle(input_lock);

End:
    timer_dispose(input);
    return 0;
}

void input_poll() {
    input_mouse_last_state = input_mouse_current_state;
    input_key_last_state = input_key_current_state;

    for (uint8_t i = 0; i < KEYBOARD_KEYS; i++)
        input_key_current_state.key_states[i] = GetAsyncKeyState(i) < 0;

    if (window_handle) {
        GetCursorPos(&input_mouse_current_state.position);
        ScreenToClient(window_handle, &input_mouse_current_state.position);
    }

    input_rotate.x = 0.0f;
    input_rotate.y = 0.0f;
    input_move.x = 0.0f;
    input_move.y = 0.0f;

    if (!window_handle || window_handle != GetForegroundWindow() || (muctx && muctx->text_input))
        return;

    float_t temp_input_movement_speed = input_movement_speed / 10.0f;
    if (input_is_down(VK_SHIFT))
        temp_input_movement_speed = input_movement_speed;
    else if (input_is_down(VK_CONTROL))
        temp_input_movement_speed = input_movement_speed / 100.0f;

    if (input_is_down('W'))
        input_move.x += temp_input_movement_speed;
    if (input_is_down('A'))
        input_move.y -= temp_input_movement_speed;
    if (input_is_down('S'))
        input_move.x -= temp_input_movement_speed;
    if (input_is_down('D'))
        input_move.y += temp_input_movement_speed;

    temp_input_movement_speed = input_movement_speed;
    if (input_is_down(VK_SHIFT))
        temp_input_movement_speed = input_movement_speed * 10.0f;
    else if (input_is_down(VK_CONTROL))
        temp_input_movement_speed = input_movement_speed / 10.0f;

    if (input_is_down(VK_UP))
        input_rotate.y += temp_input_movement_speed;
    if (input_is_down(VK_LEFT))
        input_rotate.x -= temp_input_movement_speed;
    if (input_is_down(VK_DOWN))
        input_rotate.y -= temp_input_movement_speed;
    if (input_is_down(VK_RIGHT))
        input_rotate.x += temp_input_movement_speed;

    if (input_is_tapped('R'))
        input_reset = true;

    for (size_t i = 0; i < classes_count; i++)
        if (classes[i].input)
            classes[i].input();

    if (input_is_down(VK_MBUTTON)) {
        if (!input_reset_mouse_position) {
            POINT last = input_mouse_last_state.position;
            POINT curr = input_mouse_current_state.position;
            input_rotate.y += (float_t)(((double_t)last.y - (double_t)curr.y) * input_rotation_sensitivity);
            input_rotate.x += (float_t)(((double_t)curr.x - (double_t)last.x) * input_rotation_sensitivity);
        }
        input_reset_mouse_position = false;
    }
    else
        input_reset_mouse_position = true;
}

inline bool input_is_down(uint8_t keycode) {
    return input_key_current_state.key_states[keycode];
}

inline bool input_is_up(uint8_t keycode) {
    return !input_key_current_state.key_states[keycode];
}

inline bool input_is_tapped(uint8_t keycode) {
    return input_key_current_state.key_states[keycode] && !input_key_last_state.key_states[keycode];
}

inline bool input_is_released(uint8_t keycode) {
    return !input_key_current_state.key_states[keycode] && input_key_last_state.key_states[keycode];
}

inline bool input_was_down(uint8_t keycode) {
    return input_key_last_state.key_states[keycode];
}

inline bool input_was_up(uint8_t keycode) {
    return !input_key_last_state.key_states[keycode];
}

inline POINT input_mouse_position() {
    return input_mouse_current_state.position;
}

inline void input_mouse_reset_scroll() {
    input_mouse_current_state.scroll_x = 0.0f;
    input_mouse_current_state.scroll_y = 0.0f;
}

inline double_t input_mouse_scroll_x() {
    return input_mouse_current_state.scroll_x;
}

inline double_t input_mouse_scroll_y() {
    return input_mouse_current_state.scroll_y;
}

inline void input_mouse_add_scroll_x(double_t value) {
    input_mouse_current_state.scroll_x += value;
}

inline void input_mouse_add_scroll_y(double_t value) {
    input_mouse_current_state.scroll_y += value;
}
