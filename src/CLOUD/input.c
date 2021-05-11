/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.h"
#include "../KKdLib/vec.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#define KEYBOARD_KEYS 0xFF

typedef struct key_state {
    bool key_states[KEYBOARD_KEYS];
} key_state;

typedef struct mouse_state {
    POINT position;
} mouse_state;

key_state input_key_current_state;
key_state input_key_last_state;

mouse_state input_mouse_current_state;
mouse_state input_mouse_last_state;

double_t input_movement_speed = 0.1;
double_t input_rotation_sensitivity = 0.5;
vec2d input_move;
vec2d input_rotate;
double_t input_roll;
bool input_reset;
bool input_reset_mouse_position;
bool input_locked;

extern bool close;
lock_val(input_lock);
extern HANDLE render_lock;
extern HANDLE window_handle;

void input_poll();

#define FREQ 60
#include "../CRE/timer.h"
timer_val(input);

int32_t input_main(void* arg) {
    timer_init(input, "Input");
    lock_init(input_lock);
    if (!input_lock_init)
        goto End;

    while (state != RENDER_INITIALIZED)
        msleep(input_timer, 0.0625);

    while (!close) {
        timer_calc_pre(input);
        lock_lock(input_lock);
        input_poll();
        lock_unlock(input_lock);
        double_t cycle_time = timer_calc_post(input);
        msleep(input_timer, 1000.0 / FREQ - cycle_time);
    }
    lock_dispose(input_lock);

End:
    timer_dispose(input);
    return 0;
}

void input_poll() {
    input_mouse_last_state = input_mouse_current_state;
    input_key_last_state = input_key_current_state;

    ImGuiIO* io = igGetIO();
    for (uint8_t i = 0; i < KEYBOARD_KEYS; i++)
        input_key_current_state.key_states[i] = GetAsyncKeyState(i) < 0;

    if (window_handle) {
        GetCursorPos(&input_mouse_current_state.position);
        ScreenToClient(window_handle, &input_mouse_current_state.position);
    }

    input_move.x = 0.0;
    input_move.y = 0.0;
    input_rotate.x = 0.0;
    input_rotate.y = 0.0;
    input_roll = 0.0;

    if (!window_handle || window_handle != GetForegroundWindow())
        return;

    input_locked = false;
    classes_process_input(classes, classes_count);

    if (!input_locked) {
        double_t speed;
        if (input_is_down(VK_SHIFT))
            speed = input_movement_speed * 10.0;
        else if (input_is_down(VK_CONTROL))
            speed = input_movement_speed / 10.0;
        else
            speed = input_movement_speed;

        if (input_is_down('W'))
            input_move.x += speed;
        if (input_is_down('S'))
            input_move.x -= speed;

        if (input_is_down('A'))
            input_move.y -= speed;
        if (input_is_down('D'))
            input_move.y += speed;

        if (input_is_down(VK_SHIFT))
            speed = input_movement_speed * 100.0;
        else if (input_is_down(VK_CONTROL))
            speed = input_movement_speed;
        else
            speed = input_movement_speed * 10.0;

        if (input_is_down(VK_UP))
            input_rotate.y += speed;
        if (input_is_down(VK_DOWN))
            input_rotate.y -= speed;

        if (input_is_down(VK_LEFT))
            input_rotate.x -= speed;
        if (input_is_down(VK_RIGHT))
            input_rotate.x += speed;

        if (input_is_down('Q'))
            input_roll -= speed;
        if (input_is_down('E'))
            input_roll += speed;

        if (input_is_tapped('R'))
            input_reset = true;
    }

    if (!io->WantCaptureMouse && input_is_down(VK_MBUTTON)) {
        if (!input_reset_mouse_position) {
            POINT last = input_mouse_last_state.position;
            POINT curr = input_mouse_current_state.position;
            input_rotate.y += ((double_t)last.y - (double_t)curr.y) * input_rotation_sensitivity;
            input_rotate.x += ((double_t)curr.x - (double_t)last.x) * input_rotation_sensitivity;
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
