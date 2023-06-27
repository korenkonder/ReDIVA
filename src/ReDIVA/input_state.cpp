/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "input_state.hpp"
#include "../CRE/render_context.hpp"
#include "input.hpp"

/*
Bit / Name
  0   JVS_TEST
  1   JVS_SERVICE

  2   JVS_START
  7   JVS_TRIANGLE
  8   JVS_SQUARE
  9   JVS_CROSS
 10   JVS_CIRCLE
 11   JVS_L       (also triggered by pressing L on Slider Panel)
 12   JVS_R       (also triggered by pressing R on Slider Panel)

 18   JVS_SW1
 19   JVS_SW2

 29   VK_SPACE

 39   'A'
 40   'B'
...
 64   'Z'

 80   VK_RETURN
 81   VK_SHIFT
 82   VK_CONTROL
 83   VK_MENU

 91   VK_UP
 92   VK_LEFT  (guessed, unsure)
 93   VK_DOWN
 94   VK_RIGHT (guessed, unsure)

 96   VK_LBUTTON
 97   VK_MBUTTON
 98   VK_RBUTTON
 
 99   MOUSE_SCROLL_UP
100   MOUSE_SCROLL_DOWN*/

struct struc_794 {
    ButtonState tapped;
    ButtonState released;
    ButtonState down;
    int32_t field_30[18];
    int32_t field_78;
    int64_t field_80;

    struc_794();
    ~struc_794();

    void Reset();
};

static void input_state_ctrl(InputState* input_state);
static int32_t input_state_get_delta_frame();

static bool sub_140190A30(struc_794* a1, int32_t index);

InputState* input_state;

bool disable_input_state_update = false;

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

bool InputState::CheckDown(int32_t index) {
    if (index >= 0 && index < 111)
        return !!Down[index];
    return false;
}

bool InputState::CheckDoubleTapped(int32_t index) {
    if (index >= 0 && index < 111)
        return !!DoubleTapped[index];
    return false;
}

bool InputState::CheckHold(int32_t index) {
    if (index >= 0 && index < 111)
        return !!Hold[index];
    return false;
}

bool InputState::CheckIntervalTapped(int32_t index) {
    if (index >= 0 && index < 111)
        return !!IntervalTapped[index];
    return false;
}

bool InputState::CheckReleased(int32_t index) {
    if (index >= 0 && index < 111)
        return !!Released[index];
    return false;
}

bool InputState::CheckTapped(int32_t index) {
    if (index >= 0 && index < 111)
        return !!Tapped[index];
    return false;
}

bool InputState::GetKey() {
    return key;
}

void InputState::Update(int32_t index, int32_t delta_frame) {
    DownPrev = Down;

    Tapped.reset();
    Released.reset();
    Down.reset();

    struc_794 v78;
    updated = sub_140190A30(&v78, index);

    Tapped |= v78.tapped;
    Released |= v78.released;
    Down |= v78.down;

    if (updated) {
        field_80[0] = v78.field_30[0];
        field_80[1] = v78.field_30[1];
        field_80[2] = v78.field_30[2];
        field_80[3] = v78.field_30[3];
        field_80[4] = v78.field_30[4];
        field_80[5] = v78.field_30[5];
        field_80[6] = v78.field_30[6];
        field_80[7] = v78.field_30[7];

        //Added
        field_80[8] = v78.field_30[8];
        field_80[9] = v78.field_30[9];
        field_80[10] = v78.field_30[10];
        field_80[11] = v78.field_30[11];
    }

    /*if (task_slider_control_get()->sub_140618C60(36))
        v78.tapped[11] = true;
    else
        v78.tapped[11] = false;

    if (task_slider_control_get()->sub_140618C40(36))
        v78.released[11] = true;
    else
        v78.released[11] = false;

    if (task_slider_control_get()->sub_140618C20(36))
        v78.down[11] = true;
    else
        v78.down[11] = false;

    if (task_slider_control_get()->sub_140618C60(37))
        v78.tapped[12] = true;
    else
        v78.tapped[12] = false;

    if (task_slider_control_get()->sub_140618C40(37))
        v78.released[12] = true;
    else
        v78.released[12] = false;

    if (task_slider_control_get()->sub_140618C20(37))
        v78.down[12] = true;
    else
        v78.down[12] = false;*/

    Tapped |= v78.tapped;
    Released |= v78.released;
    Down |= v78.down;

    Released &= ~Down;

    if (!Down[11] && !Down[12]) {
        if (Tapped[2])
            Tapped[110] = true;
        if (Tapped[7])
            Tapped[110] = true;
        if (Tapped[8])
            Tapped[110] = true;
        if (Tapped[9])
            Tapped[110] = true;
        if (Tapped[10])
            Tapped[110] = true;

        if (Released[2])
            Released[110] = true;
        if (Released[7])
            Released[110] = true;
        if (Released[8])
            Released[110] = true;
        if (Released[9])
            Released[110] = true;
        if (Released[10])
            Released[110] = true;

        if (Down[2])
            Down[110] = true;
        if (Down[7])
            Down[110] = true;
        if (Down[8])
            Down[110] = true;
        if (Down[9])
            Down[110] = true;
        if (Down[10])
            Down[110] = true;
    }

    field_70 ^= Down;

    ButtonState Down = this->Down;
    ButtonState Up = ~this->Down;

    ButtonState DoubleTapped;
    for (int32_t i = 0; i < INPUT_BUTTON_COUNT; i++)
        if (DoubleTapData[i].Ctrl(delta_frame, Down[i], Up[i]))
            DoubleTapped[i] = true;
        else
            DoubleTapped[i] = false;
    this->DoubleTapped = DoubleTapped;

    ButtonState Hold;
    for (int32_t i = 0; i < INPUT_BUTTON_COUNT; i++)
        if (HoldTapData[i].Ctrl(delta_frame, Down[i], Up[i]))
            Hold[i] = true;
        else
            Hold[i] = false;
    this->Hold = Hold;

    ButtonState IntervalTapped;
    for (int32_t i = 0; i < INPUT_BUTTON_COUNT; i++)
        if (IntervalTapData[i].Ctrl(delta_frame, Down[i]))
            IntervalTapped[i] = true;
        else
            IntervalTapped[i] = false;
    this->IntervalTapped = IntervalTapped;
}

int32_t InputState::sub_14018CCC0(int32_t index) {
    if (index >= 0 && index < 18)
        return field_80[index];
    return 0;
}

void input_state_init() {
    if (!input_state)
        input_state = new InputState[2];
}

void input_state_ctrl() {
    if (input_state)
        input_state_ctrl(input_state);
}

InputState* input_state_get(int32_t index) {
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

struc_794::struc_794() : field_30(), field_78(), field_80() {
    Reset();
}

struc_794::~struc_794() {

}

void struc_794::Reset() {
    tapped.reset();
    released.reset();
    down.reset();

    field_78 = 0;
    memset(field_30, 0, sizeof(field_30));
    field_30[2] = 0x80000000;
    field_30[3] = 0x80000000;
    field_80 = 0;
}

static void input_state_ctrl(InputState* input_state) {
    int32_t delta_frame = input_state_get_delta_frame();
    for (int32_t i = 0; i < 2; i++)
        input_state[i].Update(i, delta_frame);
}

static int32_t input_state_get_delta_frame() {
    return 1;
}

// Own stuff
static bool sub_140190A30(struc_794* a1, int32_t index) {
    if (!a1 || index < 0 || index >= 2)
        return false;

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
        {  0, GLFW_KEY_F1, /*JVS_TEST,*/ },
        {  1, GLFW_KEY_F2, /*JVS_SERVICE,*/ },

        {  2, GLFW_KEY_ENTER, /*JVS_START,*/ },
        {  7, GLFW_KEY_W, GLFW_KEY_I, /*JVS_TRIANGLE,*/ },
        {  8, GLFW_KEY_A, GLFW_KEY_J, /*JVS_SQUARE,*/ },
        {  9, GLFW_KEY_S, GLFW_KEY_K, /*JVS_CROSS,*/ },
        { 10, GLFW_KEY_D, GLFW_KEY_L, /*JVS_CIRCLE,*/ },
        { 11, GLFW_KEY_Q, GLFW_KEY_U, /*JVS_L,*/ },
        { 12, GLFW_KEY_E, GLFW_KEY_O, /*JVS_R,*/ },

        { 18, GLFW_KEY_F11, /*JVS_SW1,*/ },
        { 19, GLFW_KEY_F12, /*JVS_SW2,*/ },
        { 20, GLFW_KEY_C, /*JVS_COIN,*/ },

        { 29, GLFW_KEY_SPACE, },

        { 39, GLFW_KEY_A, },
        { 40, GLFW_KEY_B, },
        { 41, GLFW_KEY_C, },
        { 42, GLFW_KEY_D, },
        { 43, GLFW_KEY_E, },
        { 44, GLFW_KEY_F, },
        { 45, GLFW_KEY_G, },
        { 46, GLFW_KEY_H, },
        { 47, GLFW_KEY_I, },
        { 48, GLFW_KEY_J, },
        { 49, GLFW_KEY_K, },
        { 50, GLFW_KEY_L, },
        { 51, GLFW_KEY_M, },
        { 52, GLFW_KEY_N, },
        { 53, GLFW_KEY_O, },
        { 54, GLFW_KEY_P, },
        { 55, GLFW_KEY_Q, },
        { 56, GLFW_KEY_R, },
        { 57, GLFW_KEY_S, },
        { 58, GLFW_KEY_T, },
        { 59, GLFW_KEY_U, },
        { 60, GLFW_KEY_V, },
        { 61, GLFW_KEY_W, },
        { 62, GLFW_KEY_X, },
        { 63, GLFW_KEY_Y, },
        { 64, GLFW_KEY_Z, },

        { 80, GLFW_KEY_ENTER, },
        { 81, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, },
        { 82, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_CONTROL, },
        { 83, GLFW_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_ALT, },

        { 91, GLFW_KEY_UP, },
        { 92, GLFW_KEY_LEFT, },
        { 93, GLFW_KEY_DOWN, },
        { 94, GLFW_KEY_RIGHT, },
    };

    static const key_map mouse_button_array[] = {
        { 96, GLFW_MOUSE_BUTTON_LEFT, },
        { 97, GLFW_MOUSE_BUTTON_MIDDLE, },
        { 98, GLFW_MOUSE_BUTTON_RIGHT, },
    };

    if (!disable_input_state_update) {
        for (const key_map& i : key_map_array) {
            bool tapped = false;
            bool released = false;
            bool down = false;

            for (const int32_t& j : i.keys) {
                if (j == -1)
                    break;

                if (!tapped)
                    if (Input::IsKeyTapped(j, -1))
                        tapped = true;

                if (!released)
                    if (Input::IsKeyReleased(j, -1))
                        released = true;

                if (!down)
                    if (Input::IsKeyDown(j, -1))
                        down = true;
            }

            a1->tapped[i.index] = tapped;
            a1->released[i.index] = released;
            a1->down[i.index] = down;
        }

        for (const key_map& i : mouse_button_array) {
            bool tapped = false;
            bool released = false;
            bool down = false;

            for (const int32_t& j : i.keys) {
                if (j == -1)
                    break;

                if (!tapped)
                    if (Input::IsMouseButtonTapped(j))
                        tapped = true;

                if (!released)
                    if (Input::IsMouseButtonReleased(j))
                        released = true;

                if (!down)
                    if (Input::IsMouseButtonDown(j))
                        down = true;
            }

            a1->tapped[i.index] = tapped;
            a1->released[i.index] = released;
            a1->down[i.index] = down;
        }
    }

    int32_t x_offset = rctx_ptr->post_process.screen_x_offset;
    int32_t y_offset = rctx_ptr->post_process.screen_y_offset;
    float_t x_scale = 1280.0f / (float_t)rctx_ptr->post_process.sprite_width;
    float_t y_scale = 720.0f / (float_t)rctx_ptr->post_process.sprite_height;

    a1->field_30[8] = (int32_t)((float_t)(Input::pos.x - x_offset) * x_scale);
    a1->field_30[9] = (int32_t)((float_t)(Input::pos.y - y_offset) * y_scale);
    a1->field_30[10] = (int32_t)((float_t)(Input::pos_prev.x - Input::pos.x - x_offset) * x_scale);
    a1->field_30[11] = (int32_t)((float_t)(Input::pos_prev.y - Input::pos.y - y_offset) * y_scale);
    return true;
}
