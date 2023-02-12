/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "config.hpp"
#include "../KKdLib/default.hpp"

enum GameStateEnum {
    GAME_STATE_STARTUP = 0,
    GAME_STATE_ADVERTISE,
    GAME_STATE_GAME,
    GAME_STATE_DATA_TEST,
    GAME_STATE_TEST_MODE,
#if DATA_EDIT
    GAME_STATE_DATA_EDIT, // Added
#endif
    GAME_STATE_APP_ERROR,
    GAME_STATE_MAX,
};

enum SubGameStateEnum {
    SUB_GAME_STATE_DATA_INITIALIZE = 0,
    SUB_GAME_STATE_SYSTEM_STARTUP,
    SUB_GAME_STATE_SYSTEM_STARTUP_ERROR,
    SUB_GAME_STATE_WARNING,
    SUB_GAME_STATE_LOGO,
    SUB_GAME_STATE_RATING,
    SUB_GAME_STATE_DEMO,
    SUB_GAME_STATE_TITLE,
    SUB_GAME_STATE_RANKING,
    SUB_GAME_STATE_SCORE_RANKING,
    SUB_GAME_STATE_CM,
    SUB_GAME_STATE_PHOTO_MODE_DEMO,
    SUB_GAME_STATE_SELECTOR,
    SUB_GAME_STATE_GAME_MAIN,
    SUB_GAME_STATE_GAME_SEL,
    SUB_GAME_STATE_STAGE_RESULT,
    SUB_GAME_STATE_SCREEN_SHOT_SEL,
    SUB_GAME_STATE_SCREEN_SHOT_RESULT,
    SUB_GAME_STATE_GAME_OVER,
    SUB_GAME_STATE_DATA_TEST_MAIN,
    SUB_GAME_STATE_DATA_TEST_MISC,
    SUB_GAME_STATE_DATA_TEST_OBJ,
    SUB_GAME_STATE_DATA_TEST_STG,
    SUB_GAME_STATE_DATA_TEST_MOT,
    SUB_GAME_STATE_DATA_TEST_COLLISION,
    SUB_GAME_STATE_DATA_TEST_SPR,
    SUB_GAME_STATE_DATA_TEST_AET,
    SUB_GAME_STATE_DATA_TEST_AUTH_3D,
    SUB_GAME_STATE_DATA_TEST_CHR,
    SUB_GAME_STATE_DATA_TEST_ITEM,
    SUB_GAME_STATE_DATA_TEST_PERF,
    SUB_GAME_STATE_DATA_TEST_PVSCRIPT,
    SUB_GAME_STATE_DATA_TEST_PRINT,
    SUB_GAME_STATE_DATA_TEST_CARD,
    SUB_GAME_STATE_DATA_TEST_OPD,
    SUB_GAME_STATE_DATA_TEST_SLIDER,
    SUB_GAME_STATE_DATA_TEST_GLITTER,
    SUB_GAME_STATE_DATA_TEST_GRAPHICS,
    SUB_GAME_STATE_DATA_TEST_COLLECTION_CARD,
    SUB_GAME_STATE_TEST_MODE_MAIN,
#if DATA_EDIT
    SUB_GAME_STATE_DATA_EDIT, // Added
#endif
    SUB_GAME_STATE_APP_ERROR,
    SUB_GAME_STATE_MAX,
};

extern void game_state_init();
extern void game_state_ctrl();
extern size_t game_state_print(char* buf, size_t buf_size);
extern void game_state_set_game_state_next(GameStateEnum state);
extern void game_state_set_sub_game_state_next(SubGameStateEnum state);

extern bool test_mode_get();
extern void test_mode_set(bool value);
