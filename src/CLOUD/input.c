/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some input code is from samyuu's TLAC
*/

#include "input.h"
#include "../KKdLib/vec.h"
#include "../CRE/timer.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#define KEYBOARD_KEYS 0xFF

typedef struct key_state {
    bool key_states[KEYBOARD_KEYS];
} key_state;

typedef struct mouse_state {
    POINT position;
} mouse_state;

timer input_timer;

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
lock input_lock;
extern lock render_lock;
extern HANDLE window_handle;
extern ImGuiContext* imgui_context;
lock imgui_context_lock;

static void input_poll();

int32_t input_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(&input_timer, 60.0);
    lock_init(&input_lock);
    if (!lock_check_init(&input_lock))
        goto End;

    bool state_wait = false;
    bool state_disposed = false;
    do {
        lock_lock(&state_lock);
        state_wait = state != RENDER_INITIALIZED;
        state_disposed =  state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(&state_lock);
        if (state_disposed) {
            lock_free(&input_lock);
            goto End;
        }
        msleep(input_timer.timer, 0.0625);
    } while (state_wait);

    bool local_close = false;
    while (!close && !local_close) {
        timer_start_of_cycle(&input_timer);
        lock_lock(&state_lock);
        local_close = state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(&state_lock);

        lock_lock(&input_lock);
        input_poll();
        lock_unlock(&input_lock);
        timer_end_of_cycle(&input_timer);
    }
    lock_free(&input_lock);

End:
    timer_dispose(&input_timer);
    return 0;
}

static void input_poll() {
    input_mouse_last_state = input_mouse_current_state;
    input_key_last_state = input_key_current_state;

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

    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    if (!igGetIO()->WantCaptureMouse && input_is_down(VK_MBUTTON)) {
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
    lock_unlock(&imgui_context_lock);
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
