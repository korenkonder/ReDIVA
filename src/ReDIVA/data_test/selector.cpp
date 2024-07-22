/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "selector.hpp"
#include "../game_state.hpp"
#include "../input_state.hpp"
#include "../print_work.hpp"

struct DataTestSelData {
    SubGameStateEnum state;
    int32_t index;
    const char* name;
};

static const DataTestSelData data_test_sel_data[] = {
    { SUB_GAME_STATE_DATA_TEST_MISC     ,  0, "MISC   TEST" },
    { SUB_GAME_STATE_DATA_TEST_OBJ      ,  1, "OBJECT TEST" },
    { SUB_GAME_STATE_DATA_TEST_STG      ,  2, "STAGE  TEST" },
    { SUB_GAME_STATE_DATA_TEST_MOT      ,  3, "MOTION TEST" },
    { SUB_GAME_STATE_DATA_TEST_COLLISION,  4, "COLLISION TEST" },
    { SUB_GAME_STATE_DATA_TEST_SPR      ,  5, "SPRITE TEST" },
    { SUB_GAME_STATE_DATA_TEST_AET      ,  6, "2DAUTH TEST" },
    { SUB_GAME_STATE_DATA_TEST_AUTH_3D  ,  7, "3DAUTH TEST" },
    { SUB_GAME_STATE_DATA_TEST_CHR      ,  8, "CHARA  TEST" },
    { SUB_GAME_STATE_DATA_TEST_ITEM     ,  9, "ITEM   TEST" },
    { SUB_GAME_STATE_DATA_TEST_OPD      , 10, "OPD    TEST" },
    { SUB_GAME_STATE_DATA_TEST_PERF     , 11, "PERFORMANCE TEST" },
    { SUB_GAME_STATE_DATA_TEST_PRINT    , 12, "PRINT TEST" },
    { SUB_GAME_STATE_DATA_TEST_CARD     , 13, "CARD TEST" },
    { SUB_GAME_STATE_DATA_TEST_PVSCRIPT , 14, "PVSCRIPT TEST" },
    { SUB_GAME_STATE_DATA_TEST_GLITTER  , 15, "GLITTER TEST" },
};

static const int32_t data_test_sel_data_count = sizeof(data_test_sel_data) / sizeof(DataTestSelData);

DataTestSel* data_test_sel;

DataTestSel::DataTestSel() : selected_index(), item_index() {
    init();
}

DataTestSel::~DataTestSel() {

}

bool DataTestSel::init() {
    selected_index = -1;
    return true;
}

bool DataTestSel::ctrl() {
    InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(91))
        item_index--;
    if (input_state->CheckTapped(93))
        item_index++;

    if (item_index >= data_test_sel_data_count)
        item_index = 0;
    else if (item_index < 0)
        item_index = data_test_sel_data_count - 1;

    if (input_state->CheckTapped(80))
        selected_index = data_test_sel_data[item_index].index;

    return false;
}

bool DataTestSel::dest() {
    return true;
}

void DataTestSel::disp() {
    PrintWork print_work;
    font_info font(16);
    print_work.SetFont(&font);
    const vec2 pos = { 540.0f, 180.0f };
    print_work.line_origin_loc = pos;
    print_work.text_current_loc = pos;
    print_work.SetResolutionMode(RESOLUTION_MODE_HD);
    print_work.prio = spr::SPR_PRIO_02;

    int32_t index = 0;
    for (const DataTestSelData& i : data_test_sel_data) {
        print_work.color = index == item_index ? color_yellow : color_white;
        print_work.printf_align_left("%s\n", i.name);
        index++;
    }
}

void data_test_sel_init() {
    if (!data_test_sel)
        data_test_sel = new DataTestSel;
}

int32_t data_test_sel_get_sub_state() {
    if (data_test_sel_data[data_test_sel->item_index].index == data_test_sel->selected_index)
        return data_test_sel_data[data_test_sel->item_index].state;
    return -1;
}

void data_test_sel_free() {
    if (data_test_sel) {
        delete data_test_sel;
        data_test_sel = 0;
    }
}
