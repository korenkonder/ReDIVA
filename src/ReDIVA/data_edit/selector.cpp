/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "selector.hpp"
#include "../game_state.hpp"
#include "../input_state.hpp"
#include "../print_work.hpp"

struct DataEditSelData {
    SubGameStateEnum state;
    int32_t index;
    const char* name;
};

static const DataEditSelData data_edit_sel_data[] = {
    { SUB_GAME_STATE_DATA_EDIT_GLITTER_EDITOR,  0, "GLITTER EDITOR" },
#if FACE_ANIM
    { SUB_GAME_STATE_DATA_EDIT_FACE_ANIM     ,  1, "FACE ANIM" },
#endif
};

static const int32_t data_edit_sel_data_count = sizeof(data_edit_sel_data) / sizeof(DataEditSelData);

DataEditSel* data_edit_sel;

DataEditSel::DataEditSel() : selected_index(), item_index() {
    init();
}

DataEditSel::~DataEditSel() {

}

bool DataEditSel::init() {
    selected_index = -1;
    item_index = 0;
    return true;
}

bool DataEditSel::ctrl() {
    InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(91))
        item_index--;
    if (input_state->CheckTapped(93))
        item_index++;

    if (item_index >= data_edit_sel_data_count)
        item_index = 0;
    else if (item_index < 0)
        item_index = data_edit_sel_data_count - 1;

    if (input_state->CheckTapped(80))
        selected_index = data_edit_sel_data[item_index].index;

    return false;
}

bool DataEditSel::dest() {
    return true;
}

void DataEditSel::disp() {
    PrintWork print_work;
    font_info font(16);
    print_work.set_font(&font);
    print_work.set_position({ 540.0f, 180.0f });
    print_work.set_resolution_mode(RESOLUTION_MODE_HD);
    print_work.set_prio(spr::SPR_PRIO_02);

    int32_t index = 0;
    for (const DataEditSelData& i : data_edit_sel_data) {
        print_work.set_color(index == item_index ? color_yellow : color_white);
        print_work.printf_align_left("%s\n", i.name);
        index++;
    }
}

void data_edit_sel_init() {
    if (!data_edit_sel)
        data_edit_sel = new DataEditSel;
}

int32_t data_edit_sel_get_sub_state() {
    if (data_edit_sel_data[data_edit_sel->item_index].index == data_edit_sel->selected_index)
        return data_edit_sel_data[data_edit_sel->item_index].state;
    return -1;
}

void data_edit_sel_free() {
    if (data_edit_sel) {
        delete data_edit_sel;
        data_edit_sel = 0;
    }
}
