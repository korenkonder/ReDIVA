/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "input_state.hpp"
#include "../CRE/render_context.hpp"
#include "../CRE/resolution_mode.hpp"
#include "config.hpp"

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

 13   JVS_BUTTON_7
 14   JVS_BUTTON_8
 15   JVS_BUTTON_9
 16   JVS_BUTTON_10

 18   JVS_SW1
 19   JVS_SW2

 20   DIP_SW1
 21   DIP_SW2
 22   DIP_SW3
 23   DIP_SW4
 24   DIP_SW5
 25   DIP_SW6
 26   DIP_SW7
 27   DIP_SW8

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
    void update_modifiers(bool get);
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

static int32_t get_modifiers();

static void input_state_ctrl(InputState* input_state);
static int32_t input_state_get_delta_frame();

amInputOutputState am_input_output;

InputState* input_state;

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
        for (int32_t i = 0; i < 8; i++)
            field_80[i] = am_input.field_30[i];

        //Added
        for (int32_t i = 8; i < 12; i++)
            field_80[i] = am_input.field_30[i];
    }

    /*am_input.tapped[INPUT_BUTTON_JVS_L] = task_slider_control_get()->sub_140618C60(36);
    am_input.released[INPUT_BUTTON_JVS_L] = task_slider_control_get()->sub_140618C40(36);
    am_input.down[INPUT_BUTTON_JVS_L] = task_slider_control_get()->sub_140618C20(36);

    am_input.tapped[INPUT_BUTTON_JVS_R] = task_slider_control_get()->sub_140618C60(37);
    am_input.released[INPUT_BUTTON_JVS_R] = task_slider_control_get()->sub_140618C40(37);
    am_input.down[INPUT_BUTTON_JVS_R] = task_slider_control_get()->sub_140618C20(37);*/

    Tapped   |= am_input.tapped;
    Released |= am_input.released;
    Down     |= am_input.down;

    // Added
    key = 0;
    if (am_input.key_ptr)
        key = *am_input.key_ptr;

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
    ButtonState DownNew = this->Down & ~DownPrev;
    ButtonState UpNew = ~this->Down & DownPrev;

    {
        ButtonState DoubleTapped;
        for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
            DoubleTapped[i] = DoubleTapData[i].Ctrl(delta_frame, DownNew[i], UpNew[i]);
        this->DoubleTapped = DoubleTapped;
    }

    {
        ButtonState Hold;
        for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
            Hold[i] = HoldTapData[i].Ctrl(delta_frame, Down[i], UpNew[i]);
        this->Hold = Hold;
    }

    {
        ButtonState IntervalTapped;
        for (int32_t i = 0; i < INPUT_BUTTON_MAX; i++)
            IntervalTapped[i] = IntervalTapData[i].Ctrl(delta_frame, Down[i]);
        this->IntervalTapped = IntervalTapped;
    }
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

// Own stuff
void input_state_pc_ctrl() {
    if (input_state)
        am_input_output_pc_ctrl(am_input_output.input[0]);
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

void amInputState::update_modifiers(bool get) {
    int32_t modifiers = 0x00;
    if (get)
        modifiers = get_modifiers();

    if (modifiers & 0x01) {
        if (!down[INPUT_BUTTON_SHIFT])
            tapped[INPUT_BUTTON_SHIFT] = true;
        down[INPUT_BUTTON_SHIFT] = true;
    }
    else {
        if (down[INPUT_BUTTON_SHIFT])
            released[INPUT_BUTTON_SHIFT] = true;
        down[INPUT_BUTTON_SHIFT] = false;
    }

    if (modifiers & 0x02) {
        if (!down[INPUT_BUTTON_CONTROL])
            tapped[INPUT_BUTTON_CONTROL] = true;
        down[INPUT_BUTTON_CONTROL] = true;
    }
    else {
        if (down[INPUT_BUTTON_CONTROL])
            released[INPUT_BUTTON_CONTROL] = true;
        down[INPUT_BUTTON_CONTROL] = false;
    }

    if (modifiers & 0x04) {
        if (!down[INPUT_BUTTON_ALT])
            tapped[INPUT_BUTTON_ALT] = true;
        down[INPUT_BUTTON_ALT] = true;
    }
    else {
        if (down[INPUT_BUTTON_ALT])
            released[INPUT_BUTTON_ALT] = true;
        down[INPUT_BUTTON_ALT] = false;
    }
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
                input.down[INPUT_BUTTON_JVS_TEST] = true;
            if (v11->field_0[1] & 0x80)
                input.down[JVS_SERVICE] = true;
            if (v11->field_0[1] & 0x40)
                input.down[JVS_START] = true;
            if (v11->field_0[1] & 0x20)
                input.down[JVS_UP] = true;
            if (v11->field_0[1] & 0x10)
                input.down[JVS_DOWN] = true;
            if (v11->field_0[1] & 0x08)
                input.down[JVS_LEFT] = true;
            if (v11->field_0[1] & 0x04)
                input.down[JVS_RIGHT] = true;
            if (v11->field_0[1] & 0x02)
                input.down[JVS_TRIANGLE] = true;
            if (v11->field_0[1] & 0x01)
                input.down[JVS_SQUARE] = true;
            if (v11->field_0[2] & 0x80)
                input.down[JVS_CROSS] = true;
            if (v11->field_0[2] & 0x40)
                input.down[JVS_CIRCLE] = true;
            if (v11->field_0[2] & 0x20)
                input.down[JVS_L] = true;
            if (v11->field_0[2] & 0x10)
                input.down[JVS_R] = true;
            if (v11->field_0[2] & 0x08)
                input.down[JVS_BUTTON_7] = true;
            if (v11->field_0[2] & 0x04)
                input.down[JVS_BUTTON_8] = true;
            if (v11->field_0[2] & 0x02)
                input.down[JVS_BUTTON_9] = true;
            if (v11->field_0[2] & 0x01)
                input.down[JVS_BUTTON_10] = true;

            for (size_t j = 0; j < 18; j++)
                if (j < 2)
                    input.field_30[j] = v11->field_4[j];
        }*/

        if (input.down[INPUT_BUTTON_JVS_TEST])
            this->input[0].down[INPUT_BUTTON_JVS_TEST] = true;
        if (input.down[INPUT_BUTTON_JVS_SERVICE])
            this->input[0].down[INPUT_BUTTON_JVS_SERVICE] = true;
    }

    /*struc_807* v16 = sub_14066CA20();
    if (v16->field_0[1] & 0x01) {
        input[0].down[INPUT_BUTTON_JVS_TEST] = true;
        input[0].down[INPUT_BUTTON_JVS_SW1] = true;
    }

    if (v16->field_0[1] & 0x02) {
        input[0].down[INPUT_BUTTON_JVS_SERVICE] = true;
        input[0].down[INPUT_BUTTON_JVS_SW2] = true;
    }

    if (v16->field_0[0] & 0x01)
        input[0].down[INPUT_BUTTON_DIP_SW1] = true;
    if (v16->field_0[0] & 0x02)
        input[0].down[INPUT_BUTTON_DIP_SW2] = true;
    if (v16->field_0[0] & 0x04)
        input[0].down[INPUT_BUTTON_DIP_SW3] = true;
    if (v16->field_0[0] & 0x08)
        input[0].down[INPUT_BUTTON_DIP_SW4] = true;
    if (v16->field_0[0] & 0x10)
        input[0].down[INPUT_BUTTON_DIP_SW5] = true;
    if (v16->field_0[0] & 0x20)
        input[0].down[INPUT_BUTTON_DIP_SW6] = true;
    if (v16->field_0[0] & 0x40)
        input[0].down[INPUT_BUTTON_DIP_SW7] = true;
    if (v16->field_0[0] & 0x80)
        input[0].down[INPUT_BUTTON_DIP_SW8] = true;*/

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

struct glut_input_struct {
    uint8_t key;
    uint8_t key_last;
    int32_t motion_x;
    int32_t motion_y;
    int32_t scroll_up;
    int32_t scroll_down;
};

int32_t glut_modifiers;
amInputState glut_pc_input[2];
amInputState glut_key_input;
amInputState glut_mouse_input;
glut_input_struct glut_input;

// Own stuff
static void am_input_output_pc_ctrl(amInputState& input) {
    void glut_input_ctrl();
    glut_input_ctrl();

    input.tapped   = glut_pc_input[0].tapped;
    input.released = glut_pc_input[0].released;
    input.down     = glut_pc_input[0].down;

    input.field_30[0] = glut_pc_input[0].field_30[0];
    input.field_30[1] = glut_pc_input[0].field_30[1];
    input.field_30[2] = glut_pc_input[0].field_30[2];
    input.field_30[3] = glut_pc_input[0].field_30[3];
    input.field_30[4] = glut_pc_input[0].field_30[4];
    input.field_30[5] = glut_pc_input[0].field_30[5];
    input.field_30[6] = glut_pc_input[0].field_30[6];
    input.field_30[7] = glut_pc_input[0].field_30[7];

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

    input.field_30[8] = (int32_t)(((float_t)glut_pc_input[0].field_30[8] - x_offset) * x_scale);
    input.field_30[9] = (int32_t)(((float_t)glut_pc_input[0].field_30[9] - y_offset) * y_scale);
    input.field_30[10] = (int32_t)(((float_t)glut_pc_input[0].field_30[10] - x_offset) * x_scale);
    input.field_30[11] = (int32_t)(((float_t)glut_pc_input[0].field_30[11] - y_offset) * y_scale);

    input.field_30[12] = glut_pc_input[0].field_30[12];
    input.field_30[13] = glut_pc_input[0].field_30[12];

    input.key_ptr = glut_pc_input[0].key_ptr;
}

static int32_t get_modifiers() {
    if (glut_modifiers == -1)
        return 0x00;

    return glut_modifiers;
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

static void glut_pc_input_copy_button(InputButton src, InputButton dst);

void glut_mouse_input_check_mouse_click() {
    bool prev_down = glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS];

    glut_mouse_input.released[INPUT_BUTTON_MOUSE_PRESS] = false;
    glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_PRESS] = false;
    glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS] = false;

    if (glut_mouse_input.down[INPUT_BUTTON_MOUSE_BUTTON_LEFT]
        || glut_mouse_input.down[INPUT_BUTTON_MOUSE_BUTTON_MIDDLE]
        || glut_mouse_input.down[INPUT_BUTTON_MOUSE_BUTTON_RIGHT]
        || glut_mouse_input.down[INPUT_BUTTON_MOUSE_SCROLL_UP]
        || glut_mouse_input.down[INPUT_BUTTON_MOUSE_SCROLL_DOWN])
        glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS] = true;

    if (prev_down) {
        if (!glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS])
            glut_mouse_input.released[INPUT_BUTTON_MOUSE_PRESS] = true;
    }
    else {
        if (glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS])
            glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_PRESS] = true;
    }
}

void glut_input_ctrl() {
    int32_t scroll_up = glut_input.scroll_up;
    glut_input.scroll_up = 0;

    int32_t motion_x_delta = glut_input.motion_x - glut_mouse_input.field_30[8];
    glut_mouse_input.field_30[8] = glut_input.motion_x;
    glut_mouse_input.field_30[10] = motion_x_delta;

    int32_t scroll_down = glut_input.scroll_down;
    glut_input.scroll_down = 0;

    int32_t motion_y_delta = glut_input.motion_y - glut_mouse_input.field_30[9];
    glut_mouse_input.field_30[9] = glut_input.motion_y;
    glut_mouse_input.field_30[11] = motion_y_delta;

    glut_mouse_input.field_30[12] = scroll_up + (scroll_up == 0 ? 0 : glut_mouse_input.field_30[12]);
    glut_mouse_input.field_30[13] = scroll_down + (scroll_down == 0 ? 0 : glut_mouse_input.field_30[13]);

    if ((int32_t)glut_mouse_input.field_30[12] > 0) {
        glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_SCROLL_UP] = true;
        glut_mouse_input.released[INPUT_BUTTON_MOUSE_SCROLL_UP] = true;
        glut_mouse_input.down[INPUT_BUTTON_MOUSE_SCROLL_UP] = false;
    }

    if ((int32_t)glut_mouse_input.field_30[13] > 0) {
        glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_SCROLL_DOWN] = true;
        glut_mouse_input.released[INPUT_BUTTON_MOUSE_SCROLL_DOWN] = true;
        glut_mouse_input.down[INPUT_BUTTON_MOUSE_SCROLL_DOWN] = false;
    }

    glut_mouse_input_check_mouse_click();

    if (!glut_key_input.down[INPUT_BUTTON_KEYBOARD_PRESS])
        glut_key_input.update_modifiers(false);

    if (!glut_mouse_input.down[INPUT_BUTTON_MOUSE_PRESS])
        glut_mouse_input.update_modifiers(false);

    glut_pc_input[0].down = glut_key_input.down | glut_mouse_input.down;
    glut_pc_input[0].tapped = glut_key_input.tapped | glut_mouse_input.tapped;
    glut_pc_input[0].released = glut_key_input.released | glut_mouse_input.released;

    glut_pc_input[0].tapped &= glut_pc_input[0].down;
    glut_pc_input[0].released &= ~glut_pc_input[0].down;

    if (glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_SCROLL_UP])
        glut_pc_input[0].tapped[INPUT_BUTTON_MOUSE_SCROLL_UP] = true;
    if (glut_mouse_input.tapped[INPUT_BUTTON_MOUSE_SCROLL_DOWN])
        glut_pc_input[0].tapped[INPUT_BUTTON_MOUSE_SCROLL_DOWN] = true;

    for (int32_t i = 0; i < 14; i++)
        glut_pc_input[0].field_30[i] = glut_mouse_input.field_30[i];

    glut_pc_input[0].key_ptr = &glut_input.key_last;

    glut_pc_input_copy_button(INPUT_BUTTON_F1, INPUT_BUTTON_JVS_TEST   );
    glut_pc_input_copy_button(INPUT_BUTTON_F2, INPUT_BUTTON_JVS_SERVICE);

    glut_pc_input_copy_button(INPUT_BUTTON_SPACE, INPUT_BUTTON_JVS_START);

    glut_pc_input_copy_button(INPUT_BUTTON_UP   , INPUT_BUTTON_JVS_UP   );
    glut_pc_input_copy_button(INPUT_BUTTON_DOWN , INPUT_BUTTON_JVS_DOWN );
    glut_pc_input_copy_button(INPUT_BUTTON_LEFT , INPUT_BUTTON_JVS_LEFT );
    glut_pc_input_copy_button(INPUT_BUTTON_RIGHT, INPUT_BUTTON_JVS_RIGHT);

    glut_pc_input_copy_button(INPUT_BUTTON_W, INPUT_BUTTON_JVS_TRIANGLE);
    glut_pc_input_copy_button(INPUT_BUTTON_A, INPUT_BUTTON_JVS_SQUARE  );
    glut_pc_input_copy_button(INPUT_BUTTON_S, INPUT_BUTTON_JVS_CROSS   );
    glut_pc_input_copy_button(INPUT_BUTTON_D, INPUT_BUTTON_JVS_CIRCLE  );

    glut_pc_input_copy_button(INPUT_BUTTON_I, INPUT_BUTTON_JVS_TRIANGLE);
    glut_pc_input_copy_button(INPUT_BUTTON_J, INPUT_BUTTON_JVS_SQUARE  );
    glut_pc_input_copy_button(INPUT_BUTTON_K, INPUT_BUTTON_JVS_CROSS   );
    glut_pc_input_copy_button(INPUT_BUTTON_L, INPUT_BUTTON_JVS_CIRCLE  );

    glut_pc_input_copy_button(INPUT_BUTTON_Q, INPUT_BUTTON_JVS_L);
    glut_pc_input_copy_button(INPUT_BUTTON_E, INPUT_BUTTON_JVS_R);

    glut_pc_input_copy_button(INPUT_BUTTON_U, INPUT_BUTTON_JVS_L);
    glut_pc_input_copy_button(INPUT_BUTTON_O, INPUT_BUTTON_JVS_R);

    glut_pc_input_copy_button(INPUT_BUTTON_F11, INPUT_BUTTON_JVS_SW1);
    glut_pc_input_copy_button(INPUT_BUTTON_F12, INPUT_BUTTON_JVS_SW2);

    glut_input.key_last = glut_input.key;
    glut_input.key = 0;

    glut_key_input.tapped = 0;
    glut_key_input.released = 0;
    glut_mouse_input.tapped = 0;
    glut_mouse_input.released = 0;
}

static void glut_pc_input_copy_button(InputButton src, InputButton dst) {
    glut_pc_input[0].down[dst] = glut_pc_input[0].down[dst] | glut_pc_input[0].down[src];
    glut_pc_input[0].tapped[dst] = glut_pc_input[0].tapped[dst] | glut_pc_input[0].tapped[src];
    glut_pc_input[0].released[dst] = glut_pc_input[0].released[dst] | glut_pc_input[0].released[src];

    glut_pc_input[0].tapped[dst] = glut_pc_input[0].tapped[dst] & glut_pc_input[0].down[dst];
    glut_pc_input[0].released[dst] = glut_pc_input[0].released[dst] & ~glut_pc_input[0].down[dst];
}

struct glut_table {
    int32_t src;
    InputButton dst;
};

const glut_table glut_key_table[] = {
    { 0x1B, INPUT_BUTTON_ESCAPE    },
    { '\b', INPUT_BUTTON_BACKSPACE },
    { '\t', INPUT_BUTTON_TAB       },
    { '\r', INPUT_BUTTON_ENTER     },
    {  ' ', INPUT_BUTTON_SPACE     },
    { 0x7F, INPUT_BUTTON_DELETE    },
    {   -1, INPUT_BUTTON_MAX       },
};

const glut_table glut_skey_table[] = {
    {   1, INPUT_BUTTON_F1        },
    {   2, INPUT_BUTTON_F2        },
    {   3, INPUT_BUTTON_F3        },
    {   4, INPUT_BUTTON_F4        },
    {   5, INPUT_BUTTON_F5        },
    {   6, INPUT_BUTTON_F6        },
    {   7, INPUT_BUTTON_F7        },
    {   8, INPUT_BUTTON_F8        },
    {   9, INPUT_BUTTON_F9        },
    {  10, INPUT_BUTTON_F10       },
    {  11, INPUT_BUTTON_F11       },
    {  12, INPUT_BUTTON_F12       },
    { 100, INPUT_BUTTON_LEFT      },
    { 101, INPUT_BUTTON_UP        },
    { 102, INPUT_BUTTON_RIGHT     },
    { 103, INPUT_BUTTON_DOWN      },
    { 104, INPUT_BUTTON_PAGE_UP   },
    { 105, INPUT_BUTTON_PAGE_DOWN },
    { 106, INPUT_BUTTON_HOME      },
    { 107, INPUT_BUTTON_END       },
    { 108, INPUT_BUTTON_INSERT    },
    {  -1, INPUT_BUTTON_MAX       },
};

void glut_motion_cb(int32_t x, int32_t y);

static void glut_key_down(int32_t key, const glut_table* table);
static void glut_key_up(int32_t key, const glut_table* table);

static void glut_mouse_input_down(InputButton button);
static void glut_mouse_input_up(InputButton button);

static InputButton glut_translate_key(const glut_table* table, int32_t key);

void glut_get_modifiers() {
    int32_t modifiers = 0x00;
    if (GetKeyState(VK_SHIFT) & 0xFF00)
        modifiers |= 0x01;
    if (GetKeyState(VK_CONTROL) & 0xFF00)
        modifiers |= 0x02;
    if (GetKeyState(VK_MENU) & 0xFF00)
        modifiers |= 0x04;
    glut_modifiers = modifiers;
}

void glut_reset_modifiers() {
    glut_modifiers = -1;
}

void glut_keydown_cb(uint8_t key, int32_t x, int32_t y) {
    glut_key_down(key, glut_key_table);
    glut_input.key = key;
}

void glut_keyup_cb(uint8_t key, int32_t x, int32_t y) {
    glut_key_up(key, glut_key_table);
}

void glut_skeydown_cb(int32_t key, int32_t x, int32_t y) {
    glut_key_down(key, glut_skey_table);
}

void glut_skeyup_cb(int32_t key, int32_t x, int32_t y) {
    glut_key_up(key, glut_skey_table);
}

void glut_mouse_cb(int32_t bn, int32_t state, int32_t x, int32_t y) {
    InputButton button = INPUT_BUTTON_MAX;

    switch (bn) {
    case 0:
        button = INPUT_BUTTON_MOUSE_BUTTON_LEFT;
        break;
    case 1:
        button = INPUT_BUTTON_MOUSE_BUTTON_MIDDLE;
        break;
    case 2:
        button = INPUT_BUTTON_MOUSE_BUTTON_RIGHT;
        break;
    case 3:
        glut_input.scroll_up++;
        break;
    case 4:
        glut_input.scroll_down++;
        break;
    }

    if (button != INPUT_BUTTON_MAX) {
        switch (state) {
        case 0:
            glut_mouse_input_down(button);
            break;
        case 1:
            glut_mouse_input_up(button);
            break;
        }
    }

    glut_mouse_input.update_modifiers(true);

    glut_motion_cb(x, y);
}

void glut_motion_cb(int32_t x, int32_t y) {
    resolution_struct res =  *res_window_get();

    glut_input.motion_x = x;
    glut_input.motion_y = y;

    glut_input.motion_x = clamp_def(glut_input.motion_x, 0, res.width - 1);
    glut_input.motion_y = clamp_def(glut_input.motion_y, 0, res.height - 1);
}

static void glut_key_down(int32_t key, const glut_table* table) {
    InputButton button = glut_translate_key(table, key);
    if (button != INPUT_BUTTON_MAX) {
        glut_key_input.tapped[button] = true;
        glut_key_input.released[button] = false;
        glut_key_input.down[button] = true;

        if (!glut_key_input.down[INPUT_BUTTON_KEYBOARD_PRESS]) {
            glut_key_input.tapped[INPUT_BUTTON_KEYBOARD_PRESS] = true;
            glut_key_input.released[INPUT_BUTTON_KEYBOARD_PRESS] = false;
        }

        glut_key_input.down[INPUT_BUTTON_KEYBOARD_PRESS] = true;
    }

    glut_key_input.update_modifiers(true);
}

static void glut_key_up(int32_t key, const glut_table* table) {
    InputButton button = glut_translate_key(table, key);
    if (button != INPUT_BUTTON_MAX) {
        glut_key_input.tapped[button] = false;
        glut_key_input.released[button] = true;
        glut_key_input.down[button] = false;

        if (glut_key_input.down[INPUT_BUTTON_KEYBOARD_PRESS]) {
            glut_key_input.tapped[INPUT_BUTTON_KEYBOARD_PRESS] = false;
            glut_key_input.released[INPUT_BUTTON_KEYBOARD_PRESS] = true;
        }

        glut_key_input.down[INPUT_BUTTON_KEYBOARD_PRESS] = false;
    }

    glut_key_input.update_modifiers(true);
}

static void glut_mouse_input_down(InputButton button) {
    glut_mouse_input.released[button] = false;
    if (!glut_mouse_input.down[button])
        glut_mouse_input.tapped[button] = true;
    glut_mouse_input.down[button] = true;
}

static void glut_mouse_input_up(InputButton button) {
    glut_mouse_input.tapped[button] = false;
    if (glut_mouse_input.down[button])
        glut_mouse_input.released[button] = true;
    glut_mouse_input.down[button] = false;
}

static InputButton glut_translate_key(const glut_table* table, int32_t key) {
    InputButton button = INPUT_BUTTON_MAX;
    if (table == glut_key_table) {
        if ((key - 'a') >= 0 && (key - 'a') <= ('z' - 'a'))
            return (InputButton)(INPUT_BUTTON_A + (key - 'a')); // A - Z
        else if ((key - 'A') >= 0 && (key - 'A') <= ('Z' - 'A'))
            return (InputButton)(INPUT_BUTTON_A + (key - 'A')); // A - Z
        else if ((key - '0') >= 0 && (key - '0') <= ('9' - '0'))
            return (InputButton)(INPUT_BUTTON_0 + (key - '0')); // 0 - 9
    }

    while (table->src >= 0) {
        if (table->src == key) {
            button = table->dst;
            break;
        }
        table++;
    }

    return button;
}
