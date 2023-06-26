/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include <bitset>

#define INPUT_BUTTON_COUNT 111

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
    ButtonState field_70;
    int32_t field_80[18];
    std::bitset<22> field_C8;
    bool updated;
    bool field_D1;
    bool field_D2;
    bool key;
    DoubleTap DoubleTapData[INPUT_BUTTON_COUNT];
    HoldTap HoldTapData[INPUT_BUTTON_COUNT];
    IntervalTap IntervalTapData[INPUT_BUTTON_COUNT];

    InputState();
    ~InputState();

    bool CheckDown(int32_t index);
    bool CheckDoubleTapped(int32_t index);
    bool CheckHold(int32_t index);
    bool CheckIntervalTapped(int32_t index);
    bool CheckReleased(int32_t index);
    bool CheckTapped(int32_t index);
    void Ctrl();
    bool GetKey();
    void Update(int32_t index, int32_t delta_frame);

    int32_t sub_14018CCC0(int32_t index);
};

extern void input_state_init();
extern void input_state_ctrl();
extern InputState* input_state_get(int32_t index);
extern void input_state_free();
