/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "input_state.hpp"
#include "../CRE/render_context.hpp"
#include "config.hpp"
#include "input.hpp"

#ifndef USE_OPENGL
#if BAKE_PNG
#undef BAKE_PNG
#define BAKE_PNG (0)
#endif
#if BAKE_VIDEO
#undef BAKE_VIDEO
#define BAKE_VIDEO (0)
#endif
#endif

/*
Bit / Name
  0   JVS_TEST
  1   JVS_SERVICE

  2   JVS_START

  3   JVS_UP
  4   JVS_DOWN
  5   JVS_LEFT
  6   JVS_RIGHT

  7   JVS_TRIANGLE
  8   JVS_SQUARE
  9   JVS_CROSS
 10   JVS_CIRCLE
 11   JVS_L        (also triggered by pressing L on Slider Panel)
 12   JVS_R        (also triggered by pressing R on Slider Panel)

 18   JVS_SW1
 19   JVS_SW2

 28   KEYBOARD_PRESS (any of keyboard keys)

 29   '0'
 30   '1'
...
 38   '9'

 39   'A'
 40   'B'
...
 64   'Z'

 65   VK_ESCAPE

 66   VK_F1
 67   VK_F2
 68   VK_F3
 69   VK_F4
 70   VK_F5
 71   VK_F6
 72   VK_F7
 73   VK_F8
 74   VK_F9
 75   VK_F10
 76   VK_F11
 77   VK_F12

 78   VK_BACK
 79   VK_TAB
 80   VK_RETURN

 81   VK_SHIFT
 82   VK_CONTROL
 83   VK_MENU

 84   VK_SPACE

 85   VK_INSERT
 86   VK_HOME
 87   VK_PRIOR
 88   VK_DELETE
 89   VK_END
 90   VK_NEXT

 91   VK_UP
 92   VK_LEFT
 93   VK_DOWN
 94   VK_RIGHT

 95   MOUSE_PRESS (any of mouse buttons or scroll)

 96   VK_LBUTTON
 97   VK_MBUTTON
 98   VK_RBUTTON

 99   MOUSE_SCROLL_UP
100   MOUSE_SCROLL_DOWN

110   ADV_SKIP*/

struct amInputState {
    ButtonState tapped;
    ButtonState released;
    ButtonState down;
    int32_t field_30[18];
    int32_t field_78;
    uint8_t* key_ptr;

    amInputState();

    void reset();
};

struct amOutputState {
    int32_t data;

    amOutputState();
};

struct amInputOutputState {
    amInputState input[2];
    amOutputState output[2];

    amInputOutputState();

    void ctrl();
};

static void am_input_output_ctrl();
static void am_input_output_ctrl_parent();
static bool am_input_output_get_input(amInputState* data, int32_t index);
static bool am_input_output_set_output(const amOutputState value, int32_t index);

// Own stuff
static void am_input_output_pc_ctrl(amInputState& input);

static void input_state_ctrl(InputState* input_state);
static int32_t input_state_get_delta_frame();

amInputOutputState am_input_output;

InputState* input_state;

uint8_t disable_input_state_update = 0;
bool disable_cursor = false;

extern render_context* rctx_ptr;

InputState::DoubleTap::DoubleTap() : hold_frame(), frame(), state() {

}

bool InputState::DoubleTap::Ctrl(int32_t delta_frame, bool down, bool up) {
    if (hold_frame <= 0)
        return 0;

    switch (state) {
    case 0:
    default:
        if (up) {
            state = 1;
            frame = 0;
        }
        return false;
    case 1:
        frame += delta_frame;
        if (frame > hold_frame) {
            state = 0;
            return false;
        }

        if (down) {
            state = 0;
            return true;
        }
        return false;
    }
}

void InputState::DoubleTap::Set(int32_t hold_frame) {
    this->hold_frame = hold_frame;
}

InputState::HoldTap::HoldTap() : hold_frame(), frame() {

}

bool InputState::HoldTap::Ctrl(int32_t delta_frame, bool down, bool up) {
    if (hold_frame <= 0)
        return 0;

    if (down)
        frame += delta_frame;
    else if (!up)
        frame = 0;

    return frame >= hold_frame;
}

void InputState::HoldTap::Set(int32_t hold_frame) {
    this->hold_frame = hold_frame;
}

InputState::IntervalTap::IntervalTap() : hold_frame(), cycle_frame(), frame(), state() {

}

bool InputState::IntervalTap::Ctrl(int32_t delta_frame, bool down) {
    if (hold_frame <= 0)
        return false;

    if (!down) {
        state = 0;
        return false;
    }

    switch (state) {
    case 0:
    default:
        state = 1;
        frame = 0;
        return true;
    case 1:
        frame += delta_frame;
        if (frame >= cycle_frame) {
            state = 2;
            frame = 0;
            return true;
        }
        return false;
    case 2:
        frame += delta_frame;
        if (frame >= hold_frame) {
            frame = 0;
            return true;
        }
        return false;
    }
}

void InputState::IntervalTap::Set(int32_t hold_frame, int32_t cycle_frame) {
    this->hold_frame = hold_frame;
    this->cycle_frame = cycle_frame;
}

InputState::InputState() : field_80(), field_C8(), updated(), field_D1(), field_D2(), key() {
    for (DoubleTap& i : DoubleTapData)
        i.Set(15);

    for (HoldTap& i : HoldTapData)
        i.Set(12);

    for (IntervalTap& i : IntervalTapData)
        i.Set(3, 40);
}

InputState::~InputState() {

}

bool InputState::CheckDown(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return Down[button];
    return false;
}

bool InputState::CheckDownPrev(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return DownPrev[button];
    return false;
}

bool InputState::CheckDoubleTapped(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return DoubleTapped[button];
    return false;
}

bool InputState::CheckHold(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return Hold[button];
    return false;
}

bool InputState::CheckIntervalTapped(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return IntervalTapped[button];
    return false;
}

bool InputState::CheckReleased(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return Released[button];
    return false;
}

bool InputState::CheckTapped(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return Tapped[button];
    return false;
}

bool InputState::CheckToggle(InputButton button) const {
    if (button >= 0 && button < INPUT_BUTTON_MAX)
        return Toggle[button];
    return false;
}

uint8_t InputState::GetKey() const {
    return key;
}

void InputState::Update(int32_t index, int32_t delta_frame) {
    DownPrev = Down;

    Tapped.reset();
    Released.reset();
    Down.reset();

    amInputState am_input;
    updated = am_input_output_get_input(&am_input, index);

    Down |= am_input.down;
    Tapped |= am_input.tapped;
    Released |= am_input.released;

    if (updated) {
        field_80[0] = am_input.field_30[0];
        field_80[1] = am_input.field_30[1];
        field_80[2] = am_input.field_30[2];
        field_80[3] = am_input.field_30[3];
        field_80[4] = am_input.field_30[4];
        field_80[5] = am_input.field_30[5];
        field_80[6] = am_input.field_30[6];
        field_80[7] = am_input.field_30[7];

        //Added
        field_80[8] = am_input.field_30[8];
        field_80[9] = am_input.field_30[9];
        field_80[10] = am_input.field_30[10];
        field_80[11] = am_input.field_30[11];
    }

    /*if (task_slider_control_get()->sub_140618C60(36))
        am_input.tapped[INPUT_BUTTON_JVS_L] = true;
    else
        am_input.tapped[INPUT_BUTTON_JVS_L] = false;

    if (task_slider_control_get()->sub_140618C40(36))
        am_input.released[INPUT_BUTTON_JVS_L] = true;
    else
        am_input.released[INPUT_BUTTON_JVS_L] = false;

    if (task_slider_control_get()->sub_140618C20(36))
        am_input.down[INPUT_BUTTON_JVS_L] = true;
    else
        am_input.down[INPUT_BUTTON_JVS_L] = false;

    if (task_slider_control_get()->sub_140618C60(37))
        am_input.tapped[INPUT_BUTTON_JVS_R] = true;
    else
        am_input.tapped[INPUT_BUTTON_JVS_R] = false;

    if (task_slider_control_get()->sub_140618C40(37))
        am_input.released[INPUT_BUTTON_JVS_R] = true;
    else
        am_input.released[INPUT_BUTTON_JVS_R] = false;

    if (task_slider_control_get()->sub_140618C20(37))
        am_input.down[INPUT_BUTTON_JVS_R] = true;
    else
        am_input.down[INPUT_BUTTON_JVS_R] = false;*/

    Tapped |= am_input.tapped;
    Released |= am_input.released;
    Down |= am_input.down;

    Released &= ~Down;

    if (!Down[INPUT_BUTTON_JVS_L] && !Down[INPUT_BUTTON_JVS_R]) {
        if (Tapped[INPUT_BUTTON_JVS_START])
            Tapped[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Tapped[INPUT_BUTTON_JVS_TRIANGLE])
            Tapped[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Tapped[INPUT_BUTTON_JVS_SQUARE])
            Tapped[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Tapped[INPUT_BUTTON_JVS_CROSS])
            Tapped[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Tapped[INPUT_BUTTON_JVS_CIRCLE])
            Tapped[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;

        if (Released[INPUT_BUTTON_JVS_START])
            Released[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Released[INPUT_BUTTON_JVS_TRIANGLE])
            Released[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Released[INPUT_BUTTON_JVS_SQUARE])
            Released[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Released[INPUT_BUTTON_JVS_CROSS])
            Released[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Released[INPUT_BUTTON_JVS_CIRCLE])
            Released[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;

        if (Down[INPUT_BUTTON_JVS_START])
            Down[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Down[INPUT_BUTTON_JVS_TRIANGLE])
            Down[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Down[INPUT_BUTTON_JVS_SQUARE])
            Down[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Down[INPUT_BUTTON_JVS_CROSS])
            Down[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
        if (Down[INPUT_BUTTON_JVS_CIRCLE])
            Down[INPUT_BUTTON_MOUSE_ADV_SKIP] = true;
    }

    Toggle ^= Down;

    ButtonState Down = this->Down;
    ButtonState Up = ~this->Down;

    ButtonState DoubleTapped;
    for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
        if (DoubleTapData[i].Ctrl(delta_frame, Down[i], Up[i]))
            DoubleTapped[i] = true;
        else
            DoubleTapped[i] = false;
    this->DoubleTapped = DoubleTapped;

    ButtonState Hold;
    for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
        if (HoldTapData[i].Ctrl(delta_frame, Down[i], Up[i]))
            Hold[i] = true;
        else
            Hold[i] = false;
    this->Hold = Hold;

    ButtonState IntervalTapped;
    for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
        if (IntervalTapData[i].Ctrl(delta_frame, Down[i]))
            IntervalTapped[i] = true;
        else
            IntervalTapped[i] = false;
    this->IntervalTapped = IntervalTapped;
}

int32_t InputState::sub_14018CCC0(int32_t index) const {
    if (index >= 0 && index < 18)
        return field_80[index];
    return 0;
}

void input_state_init() {
    if (!input_state)
        input_state = new InputState[2];
}

void input_state_am_ctrl() {
    if (input_state)
        am_input_output_ctrl();
}

void input_state_ctrl() {
    if (input_state)
        input_state_ctrl(input_state);
}

const InputState* input_state_get(int32_t index) {
    if (input_state && index >= 0 && index < 2)
        return &input_state[index];
    return 0;
}

void input_state_free() {
    if (input_state) {
        delete[] input_state;
        input_state = 0;
    }
}

amInputState::amInputState() : field_30(), field_78(), key_ptr() {
    reset();
}

void amInputState::reset() {
    tapped.reset();
    released.reset();
    down.reset();

    field_78 = 0;
    memset(field_30, 0, sizeof(field_30));
    field_30[2] = 0x80000000;
    field_30[3] = 0x80000000;
    key_ptr = 0;
}

amOutputState::amOutputState() : data() {

}

amInputOutputState::amInputOutputState() {

}

void amInputOutputState::ctrl() {
    /*struct struc_938 {
        uint8_t field_0[4];
        uint16_t field_4[2];
        uint16_t field_8;
        uint16_t field_A;
        uint16_t field_C;
        uint8_t field_E[2];
        uint64_t field_10;
        uint64_t field_18;
        uint64_t field_20;
    };*/

    /*struct struc_807 {
        uint8_t field_0[2];
    };*/

    ButtonState prev_down[2];
    for (size_t i = 0; i < 2; i++) {
        amInputState& input = this->input[i];
        prev_down[i] = input.down;

        //struc_938* v11 = sub_14066CA30(sub_14066CE80() == 1 ? 0 : i);

        input.down = 0;
        memset(input.field_30, 0, sizeof(input.field_30));

        /*if (v11) {
            if (v11->field_0[0] & 0x80)
                input.down[0] = true;
            if (v11->field_0[1] & 0x80)
                input.down[2] = true;
            if (v11->field_0[1] & 0x40)
                input.down[1] = true;
            if (v11->field_0[1] & 0x20)
                input.down[3] = true;
            if (v11->field_0[1] & 0x10)
                input.down[4] = true;
            if (v11->field_0[1] & 0x08)
                input.down[5] = true;
            if (v11->field_0[1] & 0x04)
                input.down[6] = true;
            if (v11->field_0[1] & 0x02)
                input.down[7] = true;
            if (v11->field_0[1] & 0x01)
                input.down[8] = true;
            if (v11->field_0[2] & 0x80)
                input.down[9] = true;
            if (v11->field_0[2] & 0x40)
                input.down[10] = true;
            if (v11->field_0[2] & 0x20)
                input.down[11] = true;
            if (v11->field_0[2] & 0x10)
                input.down[12] = true;
            if (v11->field_0[2] & 0x08)
                input.down[13] = true;
            if (v11->field_0[2] & 0x04)
                input.down[14] = true;
            if (v11->field_0[2] & 0x02)
                input.down[15] = true;
            if (v11->field_0[2] & 0x01)
                input.down[16] = true;

            uint16_t* v15 = v11->field_4;
            for (size_t j = 0; j < 18; j++, v15++)
                if (j < 2)
                    input.field_30[j] = *v15;
        }*/

        am_input_output_pc_ctrl(input);

        if (input.down[0])
            this->input[0].down[0] = true;
        if (input.down[1])
            this->input[0].down[1] = true;
    }

    /*struc_807* v16 = sub_14066CA20();
    if (v16->field_0[1] & 0x01) {
        input[0].down[0] = true;
        input[0].down[18] = true;
    }

    if (v16->field_0[1] & 0x02) {
        input[0].down[1] = true;
        input[0].down[19] = true;
    }

    if (v16->field_0[0] & 0x01)
        input[0].down[20] = true;
    if (v16->field_0[0] & 0x02)
        input[0].down[21] = true;
    if (v16->field_0[0] & 0x04)
        input[0].down[22] = true;
    if (v16->field_0[0] & 0x08)
        input[0].down[23] = true;
    if (v16->field_0[0] & 0x10)
        input[0].down[24] = true;
    if (v16->field_0[0] & 0x20)
        input[0].down[25] = true;
    if (v16->field_0[0] & 0x40)
        input[0].down[26] = true;
    if (v16->field_0[0] & 0x80)
        input[0].down[27] = true;*/

    for (size_t i = 0; i < 2; i++) {
        input[i].tapped = input[i].down & ~prev_down[i];
        input[i].released = prev_down[i] & ~input[i].down;
    }

    /*for (size_t i = 0; i < 2; i++) {
        int32_t v34 = output[i].data;
        int32_t v35 = 0x00;
        if (v34 & 0x04)
            v35 |= 0x01;
        if (v34 & 0x08)
            v35 |= 0x02;
        if (v34 & 0x10)
            v35 |= 0x04;
        if (v34 & 0x20)
            v35 |= 0x08;
        if (v34 & 0x40)
            v35 |= 0x10;
        if (v34 & 0x80)
            v35 |= 0x20;
        if (v34 & 0x100)
            v35 |= 0x40;
        if (v34 & 0x200)
            v35 |= 0x80;
        if (v34 & 0x400)
            v35 |= 0x100;
        if (v34 & 0x800)
            v35 |= 0x200;
        if (v34 & 0x1000)
            v35 |= 0x400;
        if (v34 & 0x2000)
            v35 |= 0x800;
        if (v34 & 0x4000)
            v35 |= 0x1000;
        if (v34 & 0x8000)
            v35 |= 0x2000;
        if (v34 & 0x10000)
            v35 |= 0x4000;
        if (v34 & 0x20000)
            v35 |= 0x8000;
        if (v34 & 0x40000)
            v35 |= 0x10000;
        if (v34 & 0x80000)
            v35 |= 0x20000;
        if (v34 & 0x100000)
            v35 |= 0x40000;
        if (v34 & 0x200000)
            v35 |= 0x80000;
        sub_14066FF20(i, v35);
    }*/

    /*amOutputState* v31 = output;
    uint8_t v4 = 0x00;
    if (v31->data & 0x01)
        v4 |= 0x01;
    if (v31->data & 0x02)
        v4 |= 0x02;
    sub_14066FEF0(v4);*/
}

static void am_input_output_ctrl() {
    am_input_output.ctrl();
}

static void am_input_output_ctrl_parent() {
    am_input_output_ctrl();
}

static bool am_input_output_get_input(amInputState* data, int32_t index) {
    if (!data || index < 0 || index >= 2)
        return false;

    *data = am_input_output.input[index];
    return true;
}

// Own stuff
static void am_input_output_pc_ctrl(amInputState& input) {
    if (disable_input_state_update)
        return;

    struct key_map {
        int32_t index;
        int32_t keys[3];

        inline key_map() : index(), keys() {
            for (int32_t& i : keys)
                i = -1;
        }

        inline key_map(int32_t index, int32_t key) : key_map() {
            this->index = index;
            this->keys[0] = key;
        }

        inline key_map(int32_t index, int32_t key1, int32_t key2) : key_map() {
            this->index = index;
            this->keys[0] = key1;
            this->keys[1] = key2;
        }

        inline key_map(int32_t index, int32_t key1, int32_t key2, int32_t key3) : key_map() {
            this->index = index;
            this->keys[0] = key1;
            this->keys[1] = key2;
            this->keys[2] = key3;
        }
    };

    static const key_map key_map_array[] = {
        { INPUT_BUTTON_0, GLFW_KEY_0, },
        { INPUT_BUTTON_1, GLFW_KEY_1, },
        { INPUT_BUTTON_2, GLFW_KEY_2, },
        { INPUT_BUTTON_3, GLFW_KEY_3, },
        { INPUT_BUTTON_4, GLFW_KEY_4, },
        { INPUT_BUTTON_5, GLFW_KEY_5, },
        { INPUT_BUTTON_6, GLFW_KEY_6, },
        { INPUT_BUTTON_7, GLFW_KEY_7, },
        { INPUT_BUTTON_8, GLFW_KEY_8, },
        { INPUT_BUTTON_9, GLFW_KEY_9, },

        { INPUT_BUTTON_A, GLFW_KEY_A, },
        { INPUT_BUTTON_B, GLFW_KEY_B, },
        { INPUT_BUTTON_C, GLFW_KEY_C, },
        { INPUT_BUTTON_D, GLFW_KEY_D, },
        { INPUT_BUTTON_E, GLFW_KEY_E, },
        { INPUT_BUTTON_F, GLFW_KEY_F, },
        { INPUT_BUTTON_G, GLFW_KEY_G, },
        { INPUT_BUTTON_H, GLFW_KEY_H, },
        { INPUT_BUTTON_I, GLFW_KEY_I, },
        { INPUT_BUTTON_J, GLFW_KEY_J, },
        { INPUT_BUTTON_K, GLFW_KEY_K, },
        { INPUT_BUTTON_L, GLFW_KEY_L, },
        { INPUT_BUTTON_M, GLFW_KEY_M, },
        { INPUT_BUTTON_N, GLFW_KEY_N, },
        { INPUT_BUTTON_O, GLFW_KEY_O, },
        { INPUT_BUTTON_P, GLFW_KEY_P, },
        { INPUT_BUTTON_Q, GLFW_KEY_Q, },
        { INPUT_BUTTON_R, GLFW_KEY_R, },
        { INPUT_BUTTON_S, GLFW_KEY_S, },
        { INPUT_BUTTON_T, GLFW_KEY_T, },
        { INPUT_BUTTON_U, GLFW_KEY_U, },
        { INPUT_BUTTON_V, GLFW_KEY_V, },
        { INPUT_BUTTON_W, GLFW_KEY_W, },
        { INPUT_BUTTON_X, GLFW_KEY_X, },
        { INPUT_BUTTON_Y, GLFW_KEY_Y, },
        { INPUT_BUTTON_Z, GLFW_KEY_Z, },

        { INPUT_BUTTON_ESCAPE, GLFW_KEY_ESCAPE, },

        { INPUT_BUTTON_F1 , GLFW_KEY_F1, },
        { INPUT_BUTTON_F2 , GLFW_KEY_F2, },
        { INPUT_BUTTON_F3 , GLFW_KEY_F3, },
        { INPUT_BUTTON_F4 , GLFW_KEY_F4, },
        { INPUT_BUTTON_F5 , GLFW_KEY_F5, },
        { INPUT_BUTTON_F6 , GLFW_KEY_F6, },
        { INPUT_BUTTON_F7 , GLFW_KEY_F7, },
        { INPUT_BUTTON_F8 , GLFW_KEY_F8, },
        { INPUT_BUTTON_F9 , GLFW_KEY_F9, },
        { INPUT_BUTTON_F10, GLFW_KEY_F10, },
        { INPUT_BUTTON_F11, GLFW_KEY_F11, },
        { INPUT_BUTTON_F12, GLFW_KEY_F12, },

        { INPUT_BUTTON_BACKSPACE, GLFW_KEY_BACKSPACE, },
        { INPUT_BUTTON_TAB      , GLFW_KEY_TAB, },
        { INPUT_BUTTON_ENTER    , GLFW_KEY_ENTER, },

        { INPUT_BUTTON_SHIFT  , GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, },
        { INPUT_BUTTON_CONTROL, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, },
        { INPUT_BUTTON_ALT    , GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT, },

        { INPUT_BUTTON_SPACE, GLFW_KEY_SPACE, },

        { INPUT_BUTTON_INSERT   , GLFW_KEY_INSERT, },
        { INPUT_BUTTON_HOME     , GLFW_KEY_HOME, },
        { INPUT_BUTTON_PAGE_UP  , GLFW_KEY_PAGE_UP, },
        { INPUT_BUTTON_DELETE   , GLFW_KEY_DELETE, },
        { INPUT_BUTTON_END      , GLFW_KEY_END, },
        { INPUT_BUTTON_PAGE_DOWN, GLFW_KEY_PAGE_DOWN, },

        { INPUT_BUTTON_UP   , GLFW_KEY_UP, },
        { INPUT_BUTTON_LEFT , GLFW_KEY_LEFT, },
        { INPUT_BUTTON_DOWN , GLFW_KEY_DOWN, },
        { INPUT_BUTTON_RIGHT, GLFW_KEY_RIGHT, },
    };

    static const key_map mouse_button_array[] = {
        { INPUT_BUTTON_MOUSE_BUTTON_LEFT  , GLFW_MOUSE_BUTTON_LEFT, },
        { INPUT_BUTTON_MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_MIDDLE, },
        { INPUT_BUTTON_MOUSE_BUTTON_RIGHT , GLFW_MOUSE_BUTTON_RIGHT, },
    };

    static const key_map copy_map_array[] = {
        { INPUT_BUTTON_JVS_TEST   , INPUT_BUTTON_F1, },
        { INPUT_BUTTON_JVS_SERVICE, INPUT_BUTTON_F2, },

        { INPUT_BUTTON_JVS_START  , INPUT_BUTTON_ENTER, },

        { INPUT_BUTTON_JVS_UP   , INPUT_BUTTON_UP, },
        { INPUT_BUTTON_JVS_DOWN , INPUT_BUTTON_DOWN, },
        { INPUT_BUTTON_JVS_LEFT , INPUT_BUTTON_LEFT, },
        { INPUT_BUTTON_JVS_RIGHT, INPUT_BUTTON_RIGHT, },

        { INPUT_BUTTON_JVS_TRIANGLE, INPUT_BUTTON_W, INPUT_BUTTON_I, },
        { INPUT_BUTTON_JVS_SQUARE  , INPUT_BUTTON_A, INPUT_BUTTON_J, },
        { INPUT_BUTTON_JVS_CROSS   , INPUT_BUTTON_S, INPUT_BUTTON_K, },
        { INPUT_BUTTON_JVS_CIRCLE  , INPUT_BUTTON_D, INPUT_BUTTON_L, },

        { INPUT_BUTTON_JVS_L, INPUT_BUTTON_Q, INPUT_BUTTON_U, },
        { INPUT_BUTTON_JVS_R, INPUT_BUTTON_E, INPUT_BUTTON_O, },

        { INPUT_BUTTON_JVS_SW1, INPUT_BUTTON_F11, },
        { INPUT_BUTTON_JVS_SW2, INPUT_BUTTON_F12, },
    };

    for (const key_map& i : key_map_array) {
        bool down = false;

        for (const int32_t& j : i.keys) {
            if (j == -1)
                break;

            if (!down)
                if (Input::IsKeyDown(j, -1))
                    down = true;
        }

        input.down[i.index] = down;
    }

    for (const key_map& i : mouse_button_array) {
        bool down = false;

        for (const int32_t& j : i.keys) {
            if (j == -1)
                break;

            if (Input::IsMouseButtonDown(j)) {
                down = true;
                break;
            }
        }

        input.down[i.index] = down;
    }

    {
        const int32_t index = INPUT_BUTTON_MOUSE_SCROLL_UP;

        input.down[index] = Input::scroll > 0.0;
    }

    {
        const int32_t index = INPUT_BUTTON_MOUSE_SCROLL_DOWN;

        input.down[index] = Input::scroll < 0.0;
    }

    for (const key_map& i : copy_map_array) {
        bool down = false;

        for (const int32_t& j : i.keys) {
            if (j == -1)
                break;

            down |= input.down[j];
        }

        input.down[i.index] = down;
    }

#if BAKE_PNG || BAKE_VIDEO
    int32_t x_offset = 0;
    int32_t y_offset = 0;
    float_t x_scale = 1280.0f / (float_t)BAKE_BASE_WIDTH;
    float_t y_scale = 720.0f / (float_t)BAKE_BASE_HEIGHT;
#else
    int32_t x_offset = rctx_ptr->screen_x_offset;
    int32_t y_offset = rctx_ptr->screen_y_offset;
    float_t x_scale = 1280.0f / (float_t)rctx_ptr->sprite_width;
    float_t y_scale = 720.0f / (float_t)rctx_ptr->sprite_height;
#endif

    input.field_30[8] = (int32_t)((float_t)(Input::pos.x - x_offset) * x_scale);
    input.field_30[9] = (int32_t)((float_t)(Input::pos.y - y_offset) * y_scale);
    input.field_30[10] = (int32_t)((float_t)(Input::pos_prev.x - Input::pos.x - x_offset) * x_scale);
    input.field_30[11] = (int32_t)((float_t)(Input::pos_prev.y - Input::pos.y - y_offset) * y_scale);
}

static bool am_input_output_set_output(const amOutputState data, int32_t index) {
    if (index < 0 || index >= 2)
        return false;

    am_input_output.output[index] = data;
    return true;
}

static void input_state_ctrl(InputState* input_state) {
    int32_t delta_frame = input_state_get_delta_frame();
    for (int32_t i = 0; i < 2; i++)
        input_state[i].Update(i, delta_frame);
}

static int32_t input_state_get_delta_frame() {
    return 1;
}
