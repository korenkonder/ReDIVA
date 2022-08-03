/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

enum GameStateEnum {
    GAME_STATE_STARTUP = 0,
    GAME_STATE_ADVERTISE,
    GAME_STATE_GAME,
    GAME_STATE_DATA_TEST,
    GAME_STATE_TEST_MODE,
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
    SUB_GAME_STATE_APP_ERROR,
    SUB_GAME_STATE_MAX,
};

enum GameStateMode {
    GAME_STATE_MODE_INIT = 0,
    GAME_STATE_MODE_CTRL,
    GAME_STATE_MODE_DEST,
    GAME_STATE_MODE_MAX,
};

struct GameState {
    struct Startup {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Advertise {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Game {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct TestMode {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct AppError {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTest {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    GameStateEnum game_state;
    GameStateEnum game_state_next;
    GameStateEnum game_state_prev;
    GameStateMode mode;
    bool set_game_state_next;
    bool next_sub_game_state;
    int32_t call_count;
    int32_t advertise_state;
    SubGameStateEnum sub_game_state;
    SubGameStateEnum sub_game_state_next;
    SubGameStateEnum sub_game_state_prev;
    GameStateMode sub_mode;
    bool set_sub_game_state_next;
    bool next_game_state;
    int32_t sub_call_count;
};

struct SubGameState {
    struct DataInitialize {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct SystemStartup {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct SystemStartupError {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Warning {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Logo {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Rating {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Demo {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Title {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Ranking {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct ScoreRanking {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct CM {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct PhotoModeDemo {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct Selector {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct GameMain {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct GameSel {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct StageResult {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct ScreenShotSel {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct ScreenShotResult {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct GameOver {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestMain {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestMisc {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestObj {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestStg {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestMot {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestCollision {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestSpr {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestAet {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestAuth3d {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestChr {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestItem {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestPerf {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestPvScript {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestPrint {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestCard {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestOpd {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestSlider {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestGlitter {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestGraphics {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestCollectionCard {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestModeMain {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataTestAppError {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };
};

extern void game_state_init();
extern void game_state_ctrl();
extern GameState* game_state_get();
extern size_t game_state_print(char* buf, size_t buf_size);
extern void game_state_set_game_state_next(GameStateEnum state);
extern void game_state_set_sub_game_state_next(SubGameStateEnum state);

extern bool test_mode_get();
extern void test_mode_set(bool value);
