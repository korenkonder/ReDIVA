/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "game_state.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/task.hpp"
#include "data_test/auth_2d_test.hpp"
#include "data_test/auth_3d_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/motion_test.hpp"
#include "data_test/stage_test.hpp"
#include "data_edit.hpp"
#include "data_initialize.hpp"
#include "pv_game.hpp"
#include "system_startup.hpp"
#include "x_pv_game.hpp"

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

    struct DataTest {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct TestMode {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

#if DATA_EDIT
    struct DataEdit { // Added
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };
#endif

    struct AppError {
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

#if DATA_EDIT
    struct DataEdit { // Added
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };
#endif

    struct DataTestAppError {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };
};

struct GameStateData {
    GameStateEnum game_state;
    bool(*Init)();
    bool(*Ctrl)();
    bool(*Dest)();
    SubGameStateEnum sub_game_state[24];
    GameStateEnum next_game_state;
    SubGameStateEnum next_sub_game_state;
};

struct SubGameStateData {
    SubGameStateEnum sub_game_state;
    int32_t field_4;
    bool(*Init)();
    bool(*Ctrl)();
    bool(*Dest)();
};

GameStateData game_state_data_array[] = {
    {
        GAME_STATE_STARTUP,
        GameState::Startup::Init,
        GameState::Startup::Ctrl,
        GameState::Startup::Dest,
        {
            SUB_GAME_STATE_DATA_INITIALIZE,
            SUB_GAME_STATE_SYSTEM_STARTUP,
            SUB_GAME_STATE_SYSTEM_STARTUP_ERROR,
            SUB_GAME_STATE_WARNING,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_LOGO,
    },
    {
        GAME_STATE_ADVERTISE,
        GameState::Advertise::Init,
        GameState::Advertise::Ctrl,
        GameState::Advertise::Dest,
        {
            SUB_GAME_STATE_LOGO,
            SUB_GAME_STATE_RATING,
            SUB_GAME_STATE_DEMO,
            SUB_GAME_STATE_TITLE,
            SUB_GAME_STATE_SCORE_RANKING,
            SUB_GAME_STATE_RANKING,
            SUB_GAME_STATE_CM,
            SUB_GAME_STATE_PHOTO_MODE_DEMO,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
    {
        GAME_STATE_GAME,
        GameState::Game::Init,
        GameState::Game::Ctrl,
        GameState::Game::Dest,
        {
            SUB_GAME_STATE_SELECTOR,
            SUB_GAME_STATE_GAME_MAIN,
            //SUB_GAME_STATE_GAME_SEL,
            //SUB_GAME_STATE_STAGE_RESULT,
            //SUB_GAME_STATE_SCREEN_SHOT_SEL,
            //SUB_GAME_STATE_SCREEN_SHOT_RESULT,
            //SUB_GAME_STATE_GAME_OVER,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
    {
        GAME_STATE_DATA_TEST,
        GameState::DataTest::Init,
        GameState::DataTest::Ctrl,
        GameState::DataTest::Dest,
        {
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
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
    {
        GAME_STATE_TEST_MODE,
        GameState::TestMode::Init,
        GameState::TestMode::Ctrl,
        GameState::TestMode::Dest,
        {
            SUB_GAME_STATE_TEST_MODE_MAIN,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
#if DATA_EDIT
    { // Added
        GAME_STATE_DATA_EDIT,
        GameState::DataEdit::Init,
        GameState::DataEdit::Ctrl,
        GameState::DataEdit::Dest,
        {
            SUB_GAME_STATE_DATA_EDIT,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
#endif
    {
        GAME_STATE_APP_ERROR,
        GameState::AppError::Init,
        GameState::AppError::Ctrl,
        GameState::AppError::Dest,
        {
            SUB_GAME_STATE_APP_ERROR,
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
};

SubGameStateData sub_game_state_data_array[] = {
    {
        SUB_GAME_STATE_DATA_INITIALIZE,
        1,
        SubGameState::DataInitialize::Init,
        SubGameState::DataInitialize::Ctrl,
        SubGameState::DataInitialize::Dest,
    },
    {
        SUB_GAME_STATE_SYSTEM_STARTUP,
        1,
        SubGameState::SystemStartup::Init,
        SubGameState::SystemStartup::Ctrl,
        SubGameState::SystemStartup::Dest,
    },
    {
        SUB_GAME_STATE_SYSTEM_STARTUP_ERROR,
        1,
        SubGameState::SystemStartupError::Init,
        SubGameState::SystemStartupError::Ctrl,
        SubGameState::SystemStartupError::Dest,
    },
    {
        SUB_GAME_STATE_WARNING,
        1,
        SubGameState::Warning::Init,
        SubGameState::Warning::Ctrl,
        SubGameState::Warning::Dest,
    },
    {
        SUB_GAME_STATE_LOGO,
        1,
        SubGameState::Logo::Init,
        SubGameState::Logo::Ctrl,
        SubGameState::Logo::Dest,
    },
    {
        SUB_GAME_STATE_RATING,
        1,
        SubGameState::Rating::Init,
        SubGameState::Rating::Ctrl,
        SubGameState::Rating::Dest,
    },
    {
        SUB_GAME_STATE_DEMO,
        1,
        SubGameState::Demo::Init,
        SubGameState::Demo::Ctrl,
        SubGameState::Demo::Dest,
    },
    {
        SUB_GAME_STATE_TITLE,
        1,
        SubGameState::Title::Init,
        SubGameState::Title::Ctrl,
        SubGameState::Title::Dest,
    },
    {
        SUB_GAME_STATE_RANKING,
        1,
        SubGameState::Ranking::Init,
        SubGameState::Ranking::Ctrl,
        SubGameState::Ranking::Dest,
    },
    {
        SUB_GAME_STATE_SCORE_RANKING,
        1,
        SubGameState::ScoreRanking::Init,
        SubGameState::ScoreRanking::Ctrl,
        SubGameState::ScoreRanking::Dest,
    },
    {
        SUB_GAME_STATE_CM,
        1,
        SubGameState::CM::Init,
        SubGameState::CM::Ctrl,
        SubGameState::CM::Dest,
    },
    {
        SUB_GAME_STATE_PHOTO_MODE_DEMO,
        1,
        SubGameState::PhotoModeDemo::Init,
        SubGameState::PhotoModeDemo::Ctrl,
        SubGameState::PhotoModeDemo::Dest,
    },
    {
        SUB_GAME_STATE_SELECTOR,
        1,
        SubGameState::Selector::Init,
        SubGameState::Selector::Ctrl,
        SubGameState::Selector::Dest,
    },
    {
        SUB_GAME_STATE_GAME_MAIN,
        1,
        SubGameState::GameMain::Init,
        SubGameState::GameMain::Ctrl,
        SubGameState::GameMain::Dest,
    },
    {
        SUB_GAME_STATE_GAME_SEL,
        1,
        SubGameState::GameSel::Init,
        SubGameState::GameSel::Ctrl,
        SubGameState::GameSel::Dest,
    },
    {
        SUB_GAME_STATE_STAGE_RESULT,
        1,
        SubGameState::StageResult::Init,
        SubGameState::StageResult::Ctrl,
        SubGameState::StageResult::Dest,
    },
    {
        SUB_GAME_STATE_SCREEN_SHOT_SEL,
        1,
        SubGameState::ScreenShotSel::Init,
        SubGameState::ScreenShotSel::Ctrl,
        SubGameState::ScreenShotSel::Dest,
    },
    {
        SUB_GAME_STATE_SCREEN_SHOT_RESULT,
        1,
        SubGameState::ScreenShotResult::Init,
        SubGameState::ScreenShotResult::Ctrl,
        SubGameState::ScreenShotResult::Dest,
    },
    {
        SUB_GAME_STATE_GAME_OVER,
        1,
        SubGameState::GameOver::Init,
        SubGameState::GameOver::Ctrl,
        SubGameState::GameOver::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MAIN,
        1,
        SubGameState::DataTestMain::Init,
        SubGameState::DataTestMain::Ctrl,
        SubGameState::DataTestMain::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MISC,
        1,
        SubGameState::DataTestMisc::Init,
        SubGameState::DataTestMisc::Ctrl,
        SubGameState::DataTestMisc::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_OBJ,
        0,
        SubGameState::DataTestObj::Init,
        SubGameState::DataTestObj::Ctrl,
        SubGameState::DataTestObj::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_STG,
        0,
        SubGameState::DataTestStg::Init,
        SubGameState::DataTestStg::Ctrl,
        SubGameState::DataTestStg::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MOT,
        0,
        SubGameState::DataTestMot::Init,
        SubGameState::DataTestMot::Ctrl,
        SubGameState::DataTestMot::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_COLLISION,
        1,
        SubGameState::DataTestCollision::Init,
        SubGameState::DataTestCollision::Ctrl,
        SubGameState::DataTestCollision::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_SPR,
        1,
        SubGameState::DataTestSpr::Init,
        SubGameState::DataTestSpr::Ctrl,
        SubGameState::DataTestSpr::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_AET,
        1,
        SubGameState::DataTestAet::Init,
        SubGameState::DataTestAet::Ctrl,
        SubGameState::DataTestAet::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_AUTH_3D,
        1,
        SubGameState::DataTestAuth3d::Init,
        SubGameState::DataTestAuth3d::Ctrl,
        SubGameState::DataTestAuth3d::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_CHR,
        1,
        SubGameState::DataTestChr::Init,
        SubGameState::DataTestChr::Ctrl,
        SubGameState::DataTestChr::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_ITEM,
        1,
        SubGameState::DataTestItem::Init,
        SubGameState::DataTestItem::Ctrl,
        SubGameState::DataTestItem::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PERF,
        1,
        SubGameState::DataTestPerf::Init,
        SubGameState::DataTestPerf::Ctrl,
        SubGameState::DataTestPerf::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PVSCRIPT,
        1,
        SubGameState::DataTestPvScript::Init,
        SubGameState::DataTestPvScript::Ctrl,
        SubGameState::DataTestPvScript::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PRINT,
        1,
        SubGameState::DataTestPrint::Init,
        SubGameState::DataTestPrint::Ctrl,
        SubGameState::DataTestPrint::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_CARD,
        1,
        SubGameState::DataTestCard::Init,
        SubGameState::DataTestCard::Ctrl,
        SubGameState::DataTestCard::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_OPD,
        1,
        SubGameState::DataTestOpd::Init,
        SubGameState::DataTestOpd::Ctrl,
        SubGameState::DataTestOpd::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_SLIDER,
        1,
        SubGameState::DataTestSlider::Init,
        SubGameState::DataTestSlider::Ctrl,
        SubGameState::DataTestSlider::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_GLITTER,
        1,
        SubGameState::DataTestGlitter::Init,
        SubGameState::DataTestGlitter::Ctrl,
        SubGameState::DataTestGlitter::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_GRAPHICS,
        1,
        SubGameState::DataTestGraphics::Init,
        SubGameState::DataTestGraphics::Ctrl,
        SubGameState::DataTestGraphics::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_COLLECTION_CARD,
        1,
        SubGameState::DataTestCollectionCard::Init,
        SubGameState::DataTestCollectionCard::Ctrl,
        SubGameState::DataTestCollectionCard::Dest,
    },
    {
        SUB_GAME_STATE_TEST_MODE_MAIN,
        0,
        SubGameState::DataTestModeMain::Init,
        SubGameState::DataTestModeMain::Ctrl,
        SubGameState::DataTestModeMain::Dest,
    },
#if DATA_EDIT
    { // Added
        SUB_GAME_STATE_DATA_EDIT,
        0,
        SubGameState::DataEdit::Init,
        SubGameState::DataEdit::Ctrl,
        SubGameState::DataEdit::Dest,
    },
#endif
    {
        SUB_GAME_STATE_APP_ERROR,
        0,
        SubGameState::DataTestAppError::Init,
        SubGameState::DataTestAppError::Ctrl,
        SubGameState::DataTestAppError::Dest,
    },
};

const char* game_state_names[] = {
    "ADVERTISE",
    "STARTUP",
    "GAME",
    "DATA_TEST",
    "TEST_MODE",
#if DATA_EDIT
    "DATA_EDIT",
#endif
    "APP_ERROR",
    "MAX",
};

const char* sub_game_state_names[] = {
    "DATA_INITIALIZE",
    "SYSTEM_STARTUP",
    "SYSTEM_STARTUP_ERROR",
    "WARNING",
    "LOGO",
    "RATING",
    "DEMO",
    "TITLE",
    "RANKING",
    "SCORE_RANKING",
    "CM",
    "PHOTO_MODE_DEMO",
    "SELECTOR",
    "GAME_MAIN",
    "GAME_SEL",
    "STAGE_RESULT",
    "SCREEN_SHOT_SEL",
    "SCREEN_SHOT_RESULT",
    "GAME_OVER",
    "DATA_TEST_MAIN",
    "DATA_TEST_MISC",
    "DATA_TEST_OBJ",
    "DATA_TEST_STG",
    "DATA_TEST_MOT",
    "DATA_TEST_COLLISION",
    "DATA_TEST_SPR",
    "DATA_TEST_AET",
    "DATA_TEST_AUTH_3D",
    "DATA_TEST_CHR",
    "DATA_TEST_ITEM",
    "DATA_TEST_PERF",
    "DATA_TEST_PVSCRIPT",
    "DATA_TEST_PRINT",
    "DATA_TEST_CARD",
    "DATA_TEST_OPD",
    "DATA_TEST_SLIDER",
    "DATA_TEST_GLITTER",
    "DATA_TEST_GRAPHICS",
    "DATA_TEST_COLLECTION_CARD",
    "TEST_MODE_MAIN",
#if DATA_EDIT
    "DATA_EDIT",
#endif
    "APP_ERROR",
    "MAX",
};

extern render_context* rctx_ptr;

GameState game_state;

bool data_test_reset = false;
bool network_error;
bool test_mode;

GameStateEnum data_test_game_state_prev;

static bool game_state_call_sub(GameState* game_state);
static GameState* game_state_get();
static void game_state_set_state(GameStateEnum state, SubGameStateEnum sub_state);

static GameStateData* game_state_data_array_get(GameStateEnum state);
static SubGameStateData* sub_game_state_data_array_get(SubGameStateEnum state);

bool GameState::Startup::Init() {
    network_error = false;
    return true;
}

bool GameState::Startup::Ctrl() {
    return false;
}

bool GameState::Startup::Dest() {
    if (test_mode_get())
        game_state_set_game_state_next(GAME_STATE_TEST_MODE);
    else {
        GameStateEnum state = GAME_STATE_ADVERTISE;
        if (game_state_get()->game_state_next == GAME_STATE_TEST_MODE)
            state = GAME_STATE_TEST_MODE;
        game_state_set_game_state_next(state);
    }
    return true;
}

bool GameState::Advertise::Init() {
    return true;
}

bool GameState::Advertise::Ctrl() {
    return false;
}

bool GameState::Advertise::Dest() {
    return true;
}

bool GameState::Game::Init() {
    task_rob_manager_add_task();
    return true;
}

bool GameState::Game::Ctrl() {
    return false;
}

bool GameState::Game::Dest() {
    if (!task_pv_game_del_task() || !task_rob_manager_del_task())
        return false;

    return true;
}

bool GameState::DataTest::Init() {
    task_rob_manager_add_task();
    return true;
}

bool GameState::DataTest::Ctrl() {
    if (data_test_reset) {
        if (game_state_get()->sub_game_state != SUB_GAME_STATE_DATA_TEST_MAIN)
            game_state_set_sub_game_state_next(SUB_GAME_STATE_DATA_TEST_MAIN);
        else
            game_state_set_game_state_next(data_test_game_state_prev);
    }
    return false;
}

bool GameState::DataTest::Dest() {
    task_rob_manager_del_task();
    return true;
}

bool GameState::TestMode::Init() {
    test_mode_set(true);
    return true;
}

bool GameState::TestMode::Ctrl() {
    return false;
}

bool GameState::TestMode::Dest() {
    test_mode_set(false);
    return true;
}

#if DATA_EDIT
bool GameState::DataEdit::Init() { // Added
    return true;
}

bool GameState::DataEdit::Ctrl() { // Added
    return false;
}

bool GameState::DataEdit::Dest() { // Added
    return true;
}
#endif

bool GameState::AppError::Init() {
    return true;
}

bool GameState::AppError::Ctrl() {
    return false;
}

bool GameState::AppError::Dest() {
    return true;
}

bool SubGameState::DataInitialize::Init() {
    task_data_init_add_task();
    task_auth_3d_add_task();
    aet_manager_add_task();
    //no_good_add_task();
    return true;
}

bool SubGameState::DataInitialize::Ctrl() {
    return task_data_init_check_state();
}

bool SubGameState::DataInitialize::Dest() {
    task_data_init_del_task();
    return true;
}

bool SubGameState::SystemStartup::Init() {
    if (task_system_startup_add_task()) {
        task_pv_db_add_task();
        return true;
    }
    return false;
}

bool SubGameState::SystemStartup::Ctrl() {
    return !!system_startup_ready;
}

bool SubGameState::SystemStartup::Dest() {
    if (task_system_startup_del_task()) {
        game_state_set_sub_game_state_next(SUB_GAME_STATE_WARNING);
        return true;
    }
    return false;
}

bool SubGameState::SystemStartupError::Init() {
    return true;
}

bool SubGameState::SystemStartupError::Ctrl() {
    return false;
}

bool SubGameState::SystemStartupError::Dest() {
    return true;
}

bool SubGameState::Warning::Init() {
    return true;
}

bool SubGameState::Warning::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Warning::Dest() {
    return true;
}

bool SubGameState::Logo::Init() {
    return true;
}

bool SubGameState::Logo::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Logo::Dest() {
    return true;
}

bool SubGameState::Rating::Init() {
    return true;
}

bool SubGameState::Rating::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Rating::Dest() {
    return true;
}

bool SubGameState::Demo::Init() {
    return true;
}

bool SubGameState::Demo::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Demo::Dest() {
    return true;
}

bool SubGameState::Title::Init() {
    return true;
}

bool SubGameState::Title::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Title::Dest() {
    return true;
}

bool SubGameState::Ranking::Init() {
    return true;
}

bool SubGameState::Ranking::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::Ranking::Dest() {
    return true;
}

bool SubGameState::ScoreRanking::Init() {
    return true;
}

bool SubGameState::ScoreRanking::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::ScoreRanking::Dest() {
    return true;
}

bool SubGameState::CM::Init() {
    return true;
}

bool SubGameState::CM::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::CM::Dest() {
    return true;
}

bool SubGameState::PhotoModeDemo::Init() {
    return true;
}

bool SubGameState::PhotoModeDemo::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::PhotoModeDemo::Dest() {
    return true;
}

bool SubGameState::Selector::Init() {
    app::TaskWork::AddTask(&x_pv_game_selector, "X PVGAME SELECTOR", 0);
    return true;
}

bool SubGameState::Selector::Ctrl() {
    if (x_pv_game_selector.exit) {
        if (x_pv_game_selector.start && x_pv_game_init()) {
            XPVGameSelector& sel = x_pv_game_selector;
            app::TaskWork::AddTask(x_pv_game_ptr, "X PVGAME", 0);
            x_pv_game_ptr->Load(sel.pv_id, sel.stage_id, sel.charas, sel.modules);
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
        }
        else
            game_state_set_game_state_next(GAME_STATE_ADVERTISE);
        return true;
    }
    return false;
}

bool SubGameState::Selector::Dest() {
    x_pv_game_selector.DelTask();
    return true;
}

bool SubGameState::GameMain::Init() {
    return true;
}

bool SubGameState::GameMain::Ctrl() {
    if (!app::TaskWork::CheckTaskReady(x_pv_game_ptr))
        return true;
    return false;
}

bool SubGameState::GameMain::Dest() {
    return x_pv_game_free();
}

bool SubGameState::GameSel::Init() {
    return true;
}

bool SubGameState::GameSel::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::GameSel::Dest() {
    return true;
}

bool SubGameState::StageResult::Init() {
    return true;
}

bool SubGameState::StageResult::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::StageResult::Dest() {
    return true;
}

bool SubGameState::ScreenShotSel::Init() {
    return true;
}

bool SubGameState::ScreenShotSel::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::ScreenShotSel::Dest() {
    return true;
}

bool SubGameState::ScreenShotResult::Init() {
    return true;
}

bool SubGameState::ScreenShotResult::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::ScreenShotResult::Dest() {
    return true;
}

bool SubGameState::GameOver::Init() {
    return true;
}

bool SubGameState::GameOver::Ctrl() {
    return true;
    //return false;
}

bool SubGameState::GameOver::Dest() {
    return true;
}

bool SubGameState::DataTestMain::Init() {
    return true;
}

bool SubGameState::DataTestMain::Ctrl() {
    return false;
}

bool SubGameState::DataTestMain::Dest() {
    return true;
}

bool SubGameState::DataTestMisc::Init() {
    return true;
}

bool SubGameState::DataTestMisc::Ctrl() {
    return false;
}

bool SubGameState::DataTestMisc::Dest() {
    return true;
}

bool SubGameState::DataTestObj::Init() {
    return true;
}

bool SubGameState::DataTestObj::Ctrl() {
    return false;
}

bool SubGameState::DataTestObj::Dest() {
    return true;
}

bool SubGameState::DataTestStg::Init() {
    clear_color = { 0x60, 0x60, 0x60, 0xFF };

    camera* cam = rctx_ptr->camera;
    cam->reset();
    cam->set_view_point({ 0.0f, 0.88f, 4.3f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    app::TaskWork::AddTask(dtm_stg, "DATA_TEST_STAGE");
    dtw_stg_init();
    return true;
}

bool SubGameState::DataTestStg::Ctrl() {
    return false;
}

bool SubGameState::DataTestStg::Dest() {
    clear_color = { 0x00, 0x00, 0x00, 0xFF };

    dtw_stg->Hide();
    dtm_stg->DelTask();
    return true;
}

bool SubGameState::DataTestMot::Init() {
    app::TaskWork::AddTask(data_test_mot, "TASK_DATA_TEST_MOT");
    dtm_stg_load(0);
    dtw_stg_load(true);
    return true;
}

bool SubGameState::DataTestMot::Ctrl() {
    return task_data_init_check_state();
}

bool SubGameState::DataTestMot::Dest() {
    data_test_mot->DelTask();
    dtm_stg_unload();
    dtw_stg_unload();
    return true;
}

bool SubGameState::DataTestCollision::Init() {
    return true;
}

bool SubGameState::DataTestCollision::Ctrl() {
    return false;
}

bool SubGameState::DataTestCollision::Dest() {
    return true;
}

bool SubGameState::DataTestSpr::Init() {
    return true;
}

bool SubGameState::DataTestSpr::Ctrl() {
    return false;
}

bool SubGameState::DataTestSpr::Dest() {
    return true;
}

bool SubGameState::DataTestAet::Init() {
    clear_color = { 0x60, 0x60, 0x60, 0xFF };
    dtm_aet_load();
    return true;
}

bool SubGameState::DataTestAet::Ctrl() {
    return false;
}

bool SubGameState::DataTestAet::Dest() {
    clear_color = { 0x00, 0x00, 0x00, 0xFF };
    dtm_aet_unload();
    return true;
}

bool SubGameState::DataTestAuth3d::Init() {
    app::TaskWork::AddTask(auth_3d_test_task, "AUTH3DTEST");
    app::TaskWork::AddTask(auth_3d_test_window, "AUTH3DTEST WINDOW");
    return true;
}

bool SubGameState::DataTestAuth3d::Ctrl() {
    return false;
}

bool SubGameState::DataTestAuth3d::Dest() {
    auth_3d_test_task->DelTask();
    auth_3d_test_window->DelTask();
    return true;
}

bool SubGameState::DataTestChr::Init() {
    return true;
}

bool SubGameState::DataTestChr::Ctrl() {
    return false;
}

bool SubGameState::DataTestChr::Dest() {
    return true;
}

bool SubGameState::DataTestItem::Init() {
    return true;
}

bool SubGameState::DataTestItem::Ctrl() {
    return false;
}

bool SubGameState::DataTestItem::Dest() {
    return true;
}

bool SubGameState::DataTestPerf::Init() {
    return true;
}

bool SubGameState::DataTestPerf::Ctrl() {
    return false;
}

bool SubGameState::DataTestPerf::Dest() {
    return true;
}

bool SubGameState::DataTestPvScript::Init() {
    return true;
}

bool SubGameState::DataTestPvScript::Ctrl() {
    return false;
}

bool SubGameState::DataTestPvScript::Dest() {
    return true;
}

bool SubGameState::DataTestPrint::Init() {
    return true;
}

bool SubGameState::DataTestPrint::Ctrl() {
    return false;
}

bool SubGameState::DataTestPrint::Dest() {
    return true;
}

bool SubGameState::DataTestCard::Init() {
    return true;
}

bool SubGameState::DataTestCard::Ctrl() {
    return false;
}

bool SubGameState::DataTestCard::Dest() {
    return true;
}

bool SubGameState::DataTestOpd::Init() {
    return true;
}

bool SubGameState::DataTestOpd::Ctrl() {
    return false;
}

bool SubGameState::DataTestOpd::Dest() {
    return true;
}

bool SubGameState::DataTestSlider::Init() {
    return true;
}

bool SubGameState::DataTestSlider::Ctrl() {
    return false;
}

bool SubGameState::DataTestSlider::Dest() {
    return true;
}

bool SubGameState::DataTestGlitter::Init() {
    app::TaskWork::AddTask(task_data_test_glitter_particle, "DATA_TEST_PARTICLE");
    dtm_stg_load(0);
    dtw_stg_load(true);
    return true;
}

bool SubGameState::DataTestGlitter::Ctrl() {
    return false;
}

bool SubGameState::DataTestGlitter::Dest() {
    dtw_stg_unload();
    dtm_stg_unload();
    //if (data_test_chr->CheckTaskReady())
    //    data_test_chr->DelTask();
    task_data_test_glitter_particle->DelTask();
    return true;
}

bool SubGameState::DataTestGraphics::Init() {
    return true;
}

bool SubGameState::DataTestGraphics::Ctrl() {
    return false;
}

bool SubGameState::DataTestGraphics::Dest() {
    return true;
}

bool SubGameState::DataTestCollectionCard::Init() {
    return true;
}

bool SubGameState::DataTestCollectionCard::Ctrl() {
    return false;
}

bool SubGameState::DataTestCollectionCard::Dest() {
    return true;
}

bool SubGameState::DataTestModeMain::Init() {
    return true;
}

bool SubGameState::DataTestModeMain::Ctrl() {
    return false;
}

bool SubGameState::DataTestModeMain::Dest() {
    return true;
}

#if DATA_EDIT
bool SubGameState::DataEdit::Init() { // Added
    app::TaskWork::AddTask(&data_edit, "DATA EDIT", 0);
    return true;
}

bool SubGameState::DataEdit::Ctrl() { // Added
    return false;
}

bool SubGameState::DataEdit::Dest() { // Added
    data_edit.DelTask();
    return true;
}
#endif

bool SubGameState::DataTestAppError::Init() {
    return true;
}

bool SubGameState::DataTestAppError::Ctrl() {
    return false;
}

bool SubGameState::DataTestAppError::Dest() {
    return true;
}

void game_state_init() {
    game_state_set_state(GAME_STATE_STARTUP, SUB_GAME_STATE_MAX);
}

void game_state_ctrl() {
    GameState* game_state = game_state_get();
    if (!game_state->set_game_state_next) {
        if (game_state->set_sub_game_state_next && game_state->next_game_state) {
            if (app::TaskWork::HasTasksDest()) {
                game_state->call_count++;
                return;
            }

            game_state_set_state(GAME_STATE_MAX, game_state->sub_game_state_next);
            game_state->next_sub_game_state = false;
            game_state->next_game_state = false;
        }
    }
    else {
        if (game_state->next_sub_game_state && game_state->next_game_state) {
            if (app::TaskWork::HasTasksDest()) {
                game_state->call_count++;
                return;
            }

            game_state_set_state(game_state->game_state_next, game_state->set_sub_game_state_next
                ? game_state->sub_game_state_next : SUB_GAME_STATE_MAX);
            game_state->next_sub_game_state = false;
            game_state->next_game_state = false;
        }
    }

    if (game_state->set_game_state_next && game_state->next_game_state)
        game_state->mode = GAME_STATE_MODE_DEST;

    GameStateData* game_state_data = game_state_data_array_get(game_state->game_state);
    if (game_state_data) {
        bool(*func)() = 0;
        switch (game_state->mode) {
        case GAME_STATE_MODE_INIT:
            func = game_state_data->Init;
            break;
        case GAME_STATE_MODE_CTRL:
            func = game_state_data->Ctrl;
            break;
        case GAME_STATE_MODE_DEST:
            func = game_state_data->Dest;
            break;
        }

        bool ret = game_state->mode != GAME_STATE_MODE_CTRL;
        if (func)
            ret = func();

        if (ret) {
            if (game_state->mode == GAME_STATE_MODE_INIT)
                game_state->mode = GAME_STATE_MODE_CTRL;
            else if (game_state->mode == GAME_STATE_MODE_CTRL)
                game_state->mode = GAME_STATE_MODE_DEST;
            else if (game_state->mode == GAME_STATE_MODE_DEST)
                game_state->next_sub_game_state = true;
            else if (!game_state->set_game_state_next) {
                game_state_set_game_state_next(game_state->game_state_next);
                game_state_set_sub_game_state_next(game_state->sub_game_state_next);
            }
        }

        if (game_state->mode && !game_state->next_game_state) {
            if (game_state->set_game_state_next || game_state->set_sub_game_state_next)
                game_state->sub_mode = GAME_STATE_MODE_DEST;

            if (game_state_call_sub(game_state)) {
                if (!game_state->set_game_state_next && !game_state->set_sub_game_state_next) {
                    SubGameStateEnum sub_game_state_next = game_state->sub_game_state_next;
                    if (sub_game_state_next == SUB_GAME_STATE_MAX) {
                        game_state_set_game_state_next(game_state->game_state_next);
                        sub_game_state_next = game_state->sub_game_state_next;
                    }
                    game_state_set_sub_game_state_next(sub_game_state_next);
                }
                game_state->next_game_state = true;
            }
        }
    }
    game_state->call_count++;
}

size_t game_state_print(char* buf, size_t buf_size) {
    GameState* game_state = game_state_get();
    const char* game_state_name = game_state_names[game_state->game_state];
    const char* sub_game_state_name = sub_game_state_names[game_state->sub_game_state];
    return sprintf_s(buf, buf_size, "%s/%s", game_state_name, sub_game_state_name);
}

void game_state_set_game_state_next(GameStateEnum state) {
    GameState* game_state = game_state_get();
    if (game_state->game_state_next != GAME_STATE_APP_ERROR || !game_state->set_game_state_next) {
        game_state->game_state_next = state;
        game_state->set_game_state_next = true;
        game_state->set_sub_game_state_next = false;
    }
}

void game_state_set_sub_game_state_next(SubGameStateEnum state) {
    GameState* game_state = game_state_get();
    if (game_state->game_state_next != GAME_STATE_APP_ERROR || !game_state->set_game_state_next) {
        game_state->sub_game_state_next = state;
        game_state->set_sub_game_state_next = true;
    }
}

bool test_mode_get() {
    return test_mode;
}

void test_mode_set(bool value) {
    test_mode = value;
}

static bool game_state_call_sub(GameState* game_state) {
    SubGameStateData* v2 = sub_game_state_data_array_get(game_state->sub_game_state);
    if (!v2) {
        game_state->sub_call_count++;
        return false;
    }

    bool(*func)() = 0;
    switch (game_state->sub_mode) {
    case 0:
        func = v2->Init;
        break;
    case 1:
        func = v2->Ctrl;
        break;
    case 2:
        func = v2->Dest;
        break;
    }

    if (!func || func()) {
        if (game_state->sub_mode == GAME_STATE_MODE_INIT)
            game_state->sub_mode = GAME_STATE_MODE_CTRL;
        else if (game_state->sub_mode == GAME_STATE_MODE_CTRL)
            game_state->sub_mode = GAME_STATE_MODE_DEST;
        else if (game_state->sub_mode == GAME_STATE_MODE_DEST) {
            game_state->sub_call_count++;
            return true;
        }
    }

    game_state->sub_call_count++;
    return false;
}

static GameState* game_state_get() {
    return &game_state;
}

static void game_state_set_state(GameStateEnum state, SubGameStateEnum sub_state) {
    GameState* game_state = game_state_get();
    if (state != GAME_STATE_MAX) {
        GameStateEnum game_state_prev = game_state->game_state;
        game_state->game_state = state;
        game_state->game_state_next = GAME_STATE_MAX;
        game_state->game_state_prev = game_state_prev;
        game_state->mode = GAME_STATE_MODE_INIT;
        game_state->set_game_state_next = false;
        game_state->call_count = 0;
        game_state->advertise_state = 0;

        GameStateData* game_state_data = game_state_data_array_get(game_state->game_state);
        if (game_state_data) {
            game_state->game_state_next = game_state_data->next_game_state;
            if (sub_state == SUB_GAME_STATE_MAX)
                game_state->sub_game_state_next = game_state_data->next_sub_game_state;
        }
    }

    SubGameStateEnum sub_game_state_prev = game_state->sub_game_state;
    game_state->sub_game_state = SUB_GAME_STATE_MAX;
    game_state->sub_game_state_next = SUB_GAME_STATE_MAX;
    game_state->sub_game_state_prev = sub_game_state_prev;
    game_state->sub_mode = GAME_STATE_MODE_INIT;
    game_state->set_sub_game_state_next = false;
    game_state->sub_call_count = 0;

    GameStateData* game_state_data = game_state_data_array_get(game_state->game_state);
    if (game_state_data) {
        if (sub_state == SUB_GAME_STATE_MAX) {
            game_state->sub_game_state = game_state_data->sub_game_state[0];
            game_state->sub_game_state_next = game_state_data->sub_game_state[1];
        }
        else {
            size_t i = 0;
            SubGameStateEnum* sub_game_state = game_state_data->sub_game_state;
            while (*sub_game_state != SUB_GAME_STATE_MAX && i < 24) {
                if (*sub_game_state == sub_state) {
                    game_state->sub_game_state = sub_state;
                    game_state->sub_game_state_next = game_state_data->sub_game_state[i + 1];
                    break;
                }

                i++;
                sub_game_state++;
            }
        }
    }
}


static GameStateData* game_state_data_array_get(GameStateEnum state) {
    for (GameStateData& i : game_state_data_array)
        if (i.game_state == state)
            return &i;
    return 0;
}

static SubGameStateData* sub_game_state_data_array_get(SubGameStateEnum state) {
    for (SubGameStateData& i : sub_game_state_data_array)
        if (i.sub_game_state == state)
            return &i;
    return 0;
}