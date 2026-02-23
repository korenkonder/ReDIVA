/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include <bitset>

#define INPUT_BUTTON_COUNT 111

enum InputButton : int32_t {
    INPUT_BUTTON_JVS_TEST = 0,

    INPUT_BUTTON_JVS_SERVICE,
    INPUT_BUTTON_JVS_START,

    INPUT_BUTTON_JVS_UP,
    INPUT_BUTTON_JVS_DOWN,
    INPUT_BUTTON_JVS_LEFT,
    INPUT_BUTTON_JVS_RIGHT,

    INPUT_BUTTON_JVS_TRIANGLE,
    INPUT_BUTTON_JVS_SQUARE,
    INPUT_BUTTON_JVS_CROSS,
    INPUT_BUTTON_JVS_CIRCLE,

    INPUT_BUTTON_JVS_L,
    INPUT_BUTTON_JVS_R,

    INPUT_BUTTON_JVS_BUTTON_7,
    INPUT_BUTTON_JVS_BUTTON_8,
    INPUT_BUTTON_JVS_BUTTON_9,
    INPUT_BUTTON_JVS_BUTTON_10,

    INPUT_BUTTON_JVS_SW1 = 18,
    INPUT_BUTTON_JVS_SW2,

    INPUT_BUTTON_DIP_SW1,
    INPUT_BUTTON_DIP_SW2,
    INPUT_BUTTON_DIP_SW3,
    INPUT_BUTTON_DIP_SW4,
    INPUT_BUTTON_DIP_SW5,
    INPUT_BUTTON_DIP_SW6,
    INPUT_BUTTON_DIP_SW7,
    INPUT_BUTTON_DIP_SW8,

    INPUT_BUTTON_KEYBOARD_PRESS,

    INPUT_BUTTON_0,
    INPUT_BUTTON_1,
    INPUT_BUTTON_2,
    INPUT_BUTTON_3,
    INPUT_BUTTON_4,
    INPUT_BUTTON_5,
    INPUT_BUTTON_6,
    INPUT_BUTTON_7,
    INPUT_BUTTON_8,
    INPUT_BUTTON_9,

    INPUT_BUTTON_A,
    INPUT_BUTTON_B,
    INPUT_BUTTON_C,
    INPUT_BUTTON_D,
    INPUT_BUTTON_E,
    INPUT_BUTTON_F,
    INPUT_BUTTON_G,
    INPUT_BUTTON_H,
    INPUT_BUTTON_I,
    INPUT_BUTTON_J,
    INPUT_BUTTON_K,
    INPUT_BUTTON_L,
    INPUT_BUTTON_M,
    INPUT_BUTTON_N,
    INPUT_BUTTON_O,
    INPUT_BUTTON_P,
    INPUT_BUTTON_Q,
    INPUT_BUTTON_R,
    INPUT_BUTTON_S,
    INPUT_BUTTON_T,
    INPUT_BUTTON_U,
    INPUT_BUTTON_V,
    INPUT_BUTTON_W,
    INPUT_BUTTON_X,
    INPUT_BUTTON_Y,
    INPUT_BUTTON_Z,

    INPUT_BUTTON_ESCAPE,

    INPUT_BUTTON_F1,
    INPUT_BUTTON_F2,
    INPUT_BUTTON_F3,
    INPUT_BUTTON_F4,
    INPUT_BUTTON_F5,
    INPUT_BUTTON_F6,
    INPUT_BUTTON_F7,
    INPUT_BUTTON_F8,
    INPUT_BUTTON_F9,
    INPUT_BUTTON_F10,
    INPUT_BUTTON_F11,
    INPUT_BUTTON_F12,

    INPUT_BUTTON_BACKSPACE,
    INPUT_BUTTON_TAB,
    INPUT_BUTTON_ENTER,

    INPUT_BUTTON_SHIFT,
    INPUT_BUTTON_CONTROL,
    INPUT_BUTTON_ALT,

    INPUT_BUTTON_SPACE,

    INPUT_BUTTON_INSERT,
    INPUT_BUTTON_HOME,
    INPUT_BUTTON_PAGE_UP,
    INPUT_BUTTON_DELETE,
    INPUT_BUTTON_END,
    INPUT_BUTTON_PAGE_DOWN,

    INPUT_BUTTON_UP,
    INPUT_BUTTON_LEFT,
    INPUT_BUTTON_DOWN,
    INPUT_BUTTON_RIGHT,

    INPUT_BUTTON_MOUSE_PRESS,

    INPUT_BUTTON_MOUSE_BUTTON_LEFT,
    INPUT_BUTTON_MOUSE_BUTTON_MIDDLE,
    INPUT_BUTTON_MOUSE_BUTTON_RIGHT,

    INPUT_BUTTON_MOUSE_SCROLL_UP,
    INPUT_BUTTON_MOUSE_SCROLL_DOWN,

    INPUT_BUTTON_MOUSE_ADV_SKIP = 110,

    INPUT_BUTTON_MAX = INPUT_BUTTON_COUNT,
};

typedef std::bitset<INPUT_BUTTON_COUNT> ButtonState;

struct InputState {
    struct DoubleTap {
        int32_t hold_frame;
        int32_t frame;
        int32_t state;

        DoubleTap();

        bool Ctrl(int32_t delta_frame, bool down, bool up);
        void Set(int32_t hold_frame);
    };

    struct HoldTap {
        int32_t hold_frame;
        int32_t frame;

        HoldTap();

        bool Ctrl(int32_t delta_frame, bool down, bool up);
        void Set(int32_t hold_frame);
    };

    struct IntervalTap {
        int32_t hold_frame;
        int32_t cycle_frame;
        int32_t frame;
        int32_t state;

        IntervalTap();

        bool Ctrl(int32_t delta_frame, bool down);
        void Set(int32_t hold_frame, int32_t cycle_frame);
    };

    ButtonState Tapped;
    ButtonState Released;
    ButtonState Down;
    ButtonState DownPrev;
    ButtonState DoubleTapped;
    ButtonState Hold;
    ButtonState IntervalTapped;
    ButtonState Toggle;
    int32_t field_80[18];
    std::bitset<22> field_C8;
    bool updated;
    bool field_D1;
    bool field_D2;
    uint8_t key;
    DoubleTap DoubleTapData[INPUT_BUTTON_COUNT];
    HoldTap HoldTapData[INPUT_BUTTON_COUNT];
    IntervalTap IntervalTapData[INPUT_BUTTON_COUNT];

    InputState();
    ~InputState();

    bool CheckDown(InputButton button) const;
    bool CheckDownPrev(InputButton button) const;
    bool CheckDoubleTapped(InputButton button) const;
    bool CheckHold(InputButton button) const;
    bool CheckIntervalTapped(InputButton button) const;
    bool CheckReleased(InputButton button) const;
    bool CheckTapped(InputButton button) const;
    bool CheckToggle(InputButton button) const;
    uint8_t GetKey() const;
    void Update(int32_t index, int32_t delta_frame);

    int32_t sub_14018CCC0(int32_t index) const;
};

extern void input_state_init();
extern void input_state_am_ctrl();
extern void input_state_pc_ctrl();
extern void input_state_ctrl();
extern const InputState* input_state_get(int32_t index);
extern void input_state_free();
