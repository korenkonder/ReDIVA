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
#include "../CRE/sound.hpp"
#include "../CRE/task.hpp"
#include "data_edit/face_anim.hpp"
#include "data_edit/glitter_editor.hpp"
#include "data_edit/rob_chara_adjust.hpp"
#include "data_edit/selector.hpp"
#include "data_test/auth_2d_test.hpp"
#include "data_test/auth_3d_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/misc_test.hpp"
#include "data_test/motion_test.hpp"
#include "data_test/object_test.hpp"
#include "data_test/opd_test.hpp"
#include "data_test/rob_osage_test.hpp"
#include "data_test/selector.hpp"
#include "data_test/stage_test.hpp"
#include "information/dw_console.hpp"
#include "pv_game/player_data.hpp"
#include "pv_game/pv_game.hpp"
#include "data_initialize.hpp"
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

    struct AppError {
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataEdit { // Added
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

    struct DataEditMain { // Added
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

    struct DataEditGlitterEditor { // Added
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };

#if FACE_ANIM
    struct DataEditFaceAnim { // Added
        static bool Init();
        static bool Ctrl();
        static bool Dest();
    };
#endif
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
    bool pause;
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

    { // Added
        GAME_STATE_DATA_EDIT,
        GameState::DataEdit::Init,
        GameState::DataEdit::Ctrl,
        GameState::DataEdit::Dest,
        {
            SUB_GAME_STATE_DATA_EDIT_MAIN,
            SUB_GAME_STATE_DATA_EDIT_GLITTER_EDITOR,
#if FACE_ANIM
            SUB_GAME_STATE_DATA_EDIT_FACE_ANIM,
#endif
            SUB_GAME_STATE_MAX,
        },
        GAME_STATE_ADVERTISE,
        SUB_GAME_STATE_MAX,
    },
};

SubGameStateData sub_game_state_data_array[] = {
    {
        SUB_GAME_STATE_DATA_INITIALIZE,
        true,
        SubGameState::DataInitialize::Init,
        SubGameState::DataInitialize::Ctrl,
        SubGameState::DataInitialize::Dest,
    },
    {
        SUB_GAME_STATE_SYSTEM_STARTUP,
        true,
        SubGameState::SystemStartup::Init,
        SubGameState::SystemStartup::Ctrl,
        SubGameState::SystemStartup::Dest,
    },
    {
        SUB_GAME_STATE_SYSTEM_STARTUP_ERROR,
        true,
        SubGameState::SystemStartupError::Init,
        SubGameState::SystemStartupError::Ctrl,
        SubGameState::SystemStartupError::Dest,
    },
    {
        SUB_GAME_STATE_WARNING,
        true,
        SubGameState::Warning::Init,
        SubGameState::Warning::Ctrl,
        SubGameState::Warning::Dest,
    },
    {
        SUB_GAME_STATE_LOGO,
        true,
        SubGameState::Logo::Init,
        SubGameState::Logo::Ctrl,
        SubGameState::Logo::Dest,
    },
    {
        SUB_GAME_STATE_RATING,
        true,
        SubGameState::Rating::Init,
        SubGameState::Rating::Ctrl,
        SubGameState::Rating::Dest,
    },
    {
        SUB_GAME_STATE_DEMO,
        true,
        SubGameState::Demo::Init,
        SubGameState::Demo::Ctrl,
        SubGameState::Demo::Dest,
    },
    {
        SUB_GAME_STATE_TITLE,
        true,
        SubGameState::Title::Init,
        SubGameState::Title::Ctrl,
        SubGameState::Title::Dest,
    },
    {
        SUB_GAME_STATE_RANKING,
        true,
        SubGameState::Ranking::Init,
        SubGameState::Ranking::Ctrl,
        SubGameState::Ranking::Dest,
    },
    {
        SUB_GAME_STATE_SCORE_RANKING,
        true,
        SubGameState::ScoreRanking::Init,
        SubGameState::ScoreRanking::Ctrl,
        SubGameState::ScoreRanking::Dest,
    },
    {
        SUB_GAME_STATE_CM,
        true,
        SubGameState::CM::Init,
        SubGameState::CM::Ctrl,
        SubGameState::CM::Dest,
    },
    {
        SUB_GAME_STATE_PHOTO_MODE_DEMO,
        true,
        SubGameState::PhotoModeDemo::Init,
        SubGameState::PhotoModeDemo::Ctrl,
        SubGameState::PhotoModeDemo::Dest,
    },
    {
        SUB_GAME_STATE_SELECTOR,
        true,
        SubGameState::Selector::Init,
        SubGameState::Selector::Ctrl,
        SubGameState::Selector::Dest,
    },
    {
        SUB_GAME_STATE_GAME_MAIN,
        true,
        SubGameState::GameMain::Init,
        SubGameState::GameMain::Ctrl,
        SubGameState::GameMain::Dest,
    },
    {
        SUB_GAME_STATE_GAME_SEL,
        true,
        SubGameState::GameSel::Init,
        SubGameState::GameSel::Ctrl,
        SubGameState::GameSel::Dest,
    },
    {
        SUB_GAME_STATE_STAGE_RESULT,
        true,
        SubGameState::StageResult::Init,
        SubGameState::StageResult::Ctrl,
        SubGameState::StageResult::Dest,
    },
    {
        SUB_GAME_STATE_SCREEN_SHOT_SEL,
        true,
        SubGameState::ScreenShotSel::Init,
        SubGameState::ScreenShotSel::Ctrl,
        SubGameState::ScreenShotSel::Dest,
    },
    {
        SUB_GAME_STATE_SCREEN_SHOT_RESULT,
        true,
        SubGameState::ScreenShotResult::Init,
        SubGameState::ScreenShotResult::Ctrl,
        SubGameState::ScreenShotResult::Dest,
    },
    {
        SUB_GAME_STATE_GAME_OVER,
        true,
        SubGameState::GameOver::Init,
        SubGameState::GameOver::Ctrl,
        SubGameState::GameOver::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MAIN,
        true,
        SubGameState::DataTestMain::Init,
        SubGameState::DataTestMain::Ctrl,
        SubGameState::DataTestMain::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MISC,
        true,
        SubGameState::DataTestMisc::Init,
        SubGameState::DataTestMisc::Ctrl,
        SubGameState::DataTestMisc::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_OBJ,
        false,
        SubGameState::DataTestObj::Init,
        SubGameState::DataTestObj::Ctrl,
        SubGameState::DataTestObj::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_STG,
        false,
        SubGameState::DataTestStg::Init,
        SubGameState::DataTestStg::Ctrl,
        SubGameState::DataTestStg::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_MOT,
        false,
        SubGameState::DataTestMot::Init,
        SubGameState::DataTestMot::Ctrl,
        SubGameState::DataTestMot::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_COLLISION,
        true,
        SubGameState::DataTestCollision::Init,
        SubGameState::DataTestCollision::Ctrl,
        SubGameState::DataTestCollision::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_SPR,
        true,
        SubGameState::DataTestSpr::Init,
        SubGameState::DataTestSpr::Ctrl,
        SubGameState::DataTestSpr::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_AET,
        true,
        SubGameState::DataTestAet::Init,
        SubGameState::DataTestAet::Ctrl,
        SubGameState::DataTestAet::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_AUTH_3D,
        true,
        SubGameState::DataTestAuth3d::Init,
        SubGameState::DataTestAuth3d::Ctrl,
        SubGameState::DataTestAuth3d::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_CHR,
        true,
        SubGameState::DataTestChr::Init,
        SubGameState::DataTestChr::Ctrl,
        SubGameState::DataTestChr::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_ITEM,
        true,
        SubGameState::DataTestItem::Init,
        SubGameState::DataTestItem::Ctrl,
        SubGameState::DataTestItem::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PERF,
        true,
        SubGameState::DataTestPerf::Init,
        SubGameState::DataTestPerf::Ctrl,
        SubGameState::DataTestPerf::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PVSCRIPT,
        true,
        SubGameState::DataTestPvScript::Init,
        SubGameState::DataTestPvScript::Ctrl,
        SubGameState::DataTestPvScript::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_PRINT,
        true,
        SubGameState::DataTestPrint::Init,
        SubGameState::DataTestPrint::Ctrl,
        SubGameState::DataTestPrint::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_CARD,
        true,
        SubGameState::DataTestCard::Init,
        SubGameState::DataTestCard::Ctrl,
        SubGameState::DataTestCard::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_OPD,
        true,
        SubGameState::DataTestOpd::Init,
        SubGameState::DataTestOpd::Ctrl,
        SubGameState::DataTestOpd::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_SLIDER,
        true,
        SubGameState::DataTestSlider::Init,
        SubGameState::DataTestSlider::Ctrl,
        SubGameState::DataTestSlider::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_GLITTER,
        true,
        SubGameState::DataTestGlitter::Init,
        SubGameState::DataTestGlitter::Ctrl,
        SubGameState::DataTestGlitter::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_GRAPHICS,
        true,
        SubGameState::DataTestGraphics::Init,
        SubGameState::DataTestGraphics::Ctrl,
        SubGameState::DataTestGraphics::Dest,
    },
    {
        SUB_GAME_STATE_DATA_TEST_COLLECTION_CARD,
        true,
        SubGameState::DataTestCollectionCard::Init,
        SubGameState::DataTestCollectionCard::Ctrl,
        SubGameState::DataTestCollectionCard::Dest,
    },
    {
        SUB_GAME_STATE_TEST_MODE_MAIN,
        false,
        SubGameState::DataTestModeMain::Init,
        SubGameState::DataTestModeMain::Ctrl,
        SubGameState::DataTestModeMain::Dest,
    },
    {
        SUB_GAME_STATE_APP_ERROR,
        false,
        SubGameState::DataTestAppError::Init,
        SubGameState::DataTestAppError::Ctrl,
        SubGameState::DataTestAppError::Dest,
    },

    { // Added
        SUB_GAME_STATE_DATA_EDIT_MAIN,
        false,
        SubGameState::DataEditMain::Init,
        SubGameState::DataEditMain::Ctrl,
        SubGameState::DataEditMain::Dest,
    },
    { // Added
        SUB_GAME_STATE_DATA_EDIT_GLITTER_EDITOR,
        false,
        SubGameState::DataEditGlitterEditor::Init,
        SubGameState::DataEditGlitterEditor::Ctrl,
        SubGameState::DataEditGlitterEditor::Dest,
    },
#if FACE_ANIM
    { // Added
        SUB_GAME_STATE_DATA_EDIT_FACE_ANIM,
        false,
        SubGameState::DataEditFaceAnim::Init,
        SubGameState::DataEditFaceAnim::Ctrl,
        SubGameState::DataEditFaceAnim::Dest,
    },
#endif
};

const char* game_state_names[] = {
    "ADVERTISE",
    "STARTUP",
    "GAME",
    "DATA_TEST",
    "TEST_MODE",
    "APP_ERROR",
    "DATA_EDIT",
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
    "APP_ERROR",
    "DATA_EDIT_MAIN",
    "DATA_EDIT_GLITTER_EDITOR",
#if FACE_ANIM
    "DATA_EDIT_FACE_ANIM",
#endif
    "MAX",
};

extern render_context* rctx_ptr;

GameState game_state;

bool data_edit_reset = false;
bool data_test_reset = false;
bool network_error;
bool test_mode;

#if PV_DEBUG
bool pv_x;
#endif
#if BAKE_X_PACK
bool pv_x_bake;
#endif

GameStateEnum data_edit_game_state_prev;
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
    rctx_ptr->render_manager->set_multisample(false);
    rctx_ptr->render_manager->set_clear(true);
    data_test_game_state_prev = game_state_get()->game_state_prev;
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
    if (!task_rob_manager_del_task())
        return false;

    rctx_ptr->render_manager->set_multisample(true);
    rctx_ptr->render_manager->set_clear(false);
    return true;
}

bool GameState::TestMode::Init() {
    test_mode_set(true);
    sound_work_reset_all_se();
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    return true;
}

bool GameState::TestMode::Ctrl() {
    return false;
}

bool GameState::TestMode::Dest() {
    test_mode_set(false);
    return true;
}

bool GameState::AppError::Init() {
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    sound_work_reset_all_se();
    return true;
}

bool GameState::AppError::Ctrl() {
    return false;
}

bool GameState::AppError::Dest() {
    return true;
}

bool GameState::DataEdit::Init() { // Added
    rctx_ptr->render_manager->set_multisample(false);
    rctx_ptr->render_manager->set_clear(true);
    data_edit_game_state_prev = game_state_get()->game_state_prev;
    return true;
}

bool GameState::DataEdit::Ctrl() { // Added
    if (data_edit_reset) {
        if (game_state_get()->sub_game_state != SUB_GAME_STATE_DATA_EDIT_MAIN)
            game_state_set_sub_game_state_next(SUB_GAME_STATE_DATA_EDIT_MAIN);
        else
            game_state_set_game_state_next(data_edit_game_state_prev);
    }
    return false;
}

bool GameState::DataEdit::Dest() { // Added
    rctx_ptr->render_manager->set_multisample(true);
    rctx_ptr->render_manager->set_clear(false);
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
#if PV_DEBUG
    if (!pv_x) {
        pv_game_selector_init();
        app::TaskWork::add_task(pv_game_selector_get(), "PVGAME SELECTOR", 0);
        return true;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (!baker) {
            x_pv_game_baker_init();
            app::TaskWork::add_task(x_pv_game_baker_get(), "X PVGAME BAKER", 0);
        }
        else
            baker->next = true;
        return true;
    }
#endif

    x_pv_game_selector_init();
    app::TaskWork::add_task(x_pv_game_selector_get(), "X PVGAME SELECTOR", 0);
    return true;
}

bool SubGameState::Selector::Ctrl() {
#if PV_DEBUG
    if (!pv_x) {
        PVGameSelector* sel = pv_game_selector_get();
        if (sel->exit) {
            if (sel->start && pv_game_init()) {
                struc_717* v0 = sub_14038BB30();
                v0->field_0.stage_index = 0;

                struc_716* v2 = &v0->field_28[v0->field_0.stage_index];
                v2->field_2C.pv_id = sel->pv_id;
                v2->field_2C.difficulty = sel->difficulty;
                v2->field_2C.edition = sel->edition;
                v2->field_2C.score_percentage_clear = 5000;
                v2->field_2C.life_gauge_safety_time = 40;
                v2->field_2C.life_gauge_border = 30;

                for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                    v2->field_2C.field_4C[i] = sel->modules[i];
                    v2->field_2C.field_64[i] = sel->modules[i];
                    v2->field_2C.field_7C[i] = {};
                    v2->field_2C.field_DC[i] = {};
                }

                v0->field_0.no_fail = true;
                v0->field_0.watch = true;
                v0->field_0.success = sel->success;

                PlayerData* player_data = player_data_array_get(0);
                player_data->field_0 = true;

                sub_14038BB30()->field_0.option = 0;

                task_pv_game_init_pv();
                game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
            }
            else
                game_state_set_game_state_next(GAME_STATE_ADVERTISE);
            return true;
        }
        return false;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker->wait)
            return false;

        const int32_t pv_ids[] = {
            801,
            802,
            803,
            804,
            805,
            806,
            807,
            808,
            809,
            810,
            811,
            812,
            813,
            814,
            815,
            816,
            817,
            818,
            819,
            820,
            821,
            822,
            823,
            824,
            825,
            826,
            827,
            828,
            829,
            830,
            831,
            832,
        };

        if (baker->start && x_pv_game_init()) {
            app::TaskWork::add_task(x_pv_game_get(), "PVGAME", 0);
            x_pv_game_get()->load(pv_ids[baker->index], pv_ids[baker->index] % 100, baker->charas, baker->modules);
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
        }
        else
            game_state_set_game_state_next(GAME_STATE_ADVERTISE);
        return true;
    }
#endif

    XPVGameSelector* sel = x_pv_game_selector_get();
    if (sel->exit) {
        if (sel->start && x_pv_game_init()) {
            app::TaskWork::add_task(x_pv_game_get(), "PVGAME", 0);
            x_pv_game_get()->load(sel->pv_id, sel->stage_id, sel->charas, sel->modules);
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
        }
        else
            game_state_set_game_state_next(GAME_STATE_ADVERTISE);
        return true;
    }
    return false;
}

bool SubGameState::Selector::Dest() {
#if PV_DEBUG
    if (!pv_x) {
        PVGameSelector* sel = pv_game_selector_get();
        if (app::TaskWork::check_task_ready(sel)) {
            sel->del();
            return false;
        }

        pv_game_selector_free();
        return true;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker && baker->start) {
            baker->start = false;
            return true;
        }
        else if (baker && !baker->exit)
            return false;
        else if (app::TaskWork::check_task_ready(baker)) {
            baker->del();
            return false;
        }

        x_pv_game_baker_free();
        return true;
    }
#endif

    XPVGameSelector* sel = x_pv_game_selector_get();
    if (app::TaskWork::check_task_ready(sel)) {
        sel->del();
        return false;
    }

    x_pv_game_selector_free();
    return true;
}

bool SubGameState::GameMain::Init() {
    sound_work_release_stream(0);
    rctx_ptr->render_manager->set_multisample(false);
    return true;
}

bool SubGameState::GameMain::Ctrl() {
#if PV_DEBUG
    if (!pv_x) {
        if (!task_pv_game_check_task_ready())
            return true;
        return false;
    }
#endif
    if (!app::TaskWork::check_task_ready(x_pv_game_get()))
        return true;
    return false;
}

bool SubGameState::GameMain::Dest() {
    bool res;
#if PV_DEBUG
    if (!pv_x) {
        task_pv_game_del_task();
        res = pv_game_free();
    }
    else
        res = x_pv_game_free();
#else
    res = x_pv_game_free();
#endif
    if (!res)
        return false;

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker)
            game_state_set_sub_game_state_next(SUB_GAME_STATE_SELECTOR);
    }
#endif

    rctx_ptr->render_manager->set_multisample(true);
    return true;
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
    app::TaskWork::add_task(data_test_sel, "DATA_TEST_MAIN");
    return true;
}

bool SubGameState::DataTestMain::Ctrl() {
    int32_t state = data_test_sel_get_sub_state();
    if (state >= 0)
        game_state_set_sub_game_state_next((SubGameStateEnum)state);
    return false;
}

bool SubGameState::DataTestMain::Dest() {
    data_test_sel->del();
    return true;
}

bool SubGameState::DataTestMisc::Init() {
    app::TaskWork::add_task(task_data_test_misc, "DATA_TEST_MISC");
    return true;
}

bool SubGameState::DataTestMisc::Ctrl() {
    return false;
}

bool SubGameState::DataTestMisc::Dest() {
    task_data_test_misc->del();
    return true;
}

bool SubGameState::DataTestObj::Init() {
    app::TaskWork::add_task(task_data_test_obj, "DATA_TEST_OBJ");
    task_stage_add_task("DATA_TEST_OBJ_STAGE");
    return true;
}

bool SubGameState::DataTestObj::Ctrl() {
    return false;
}

bool SubGameState::DataTestObj::Dest() {
    task_data_test_obj->del();
    task_stage_del_task();
    return true;
}

bool SubGameState::DataTestStg::Init() {
    clear_color = 0xFF606060;

    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 0.88f, 4.3f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    app::TaskWork::add_task(dtm_stg, "DATA_TEST_STAGE");
    dtw_stg_init();
    return true;
}

bool SubGameState::DataTestStg::Ctrl() {
    return false;
}

bool SubGameState::DataTestStg::Dest() {
    clear_color = 0xFF000000;

    dtm_stg_unload();
    dtw_stg_unload();
    return true;
}

bool SubGameState::DataTestMot::Init() {
    app::TaskWork::add_task(data_test_mot, "TASK_DATA_TEST_MOT");
    app::TaskWork::add_task(rob_osage_test, "ROB_OSAGE_TEST");
    app::TaskWork::add_task(rob_chara_adjust, "ROB_CHARA_ADJUST");
    dtm_stg_load(0);
    dtw_stg_load(true);
    return true;
}

bool SubGameState::DataTestMot::Ctrl() {
    return false;
}

bool SubGameState::DataTestMot::Dest() {
    data_test_mot->del();
    rob_osage_test->del();
    rob_chara_adjust->del();
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
    clear_color = 0xFF606060;
    dtm_aet_load();
    return true;
}

bool SubGameState::DataTestAet::Ctrl() {
    return false;
}

bool SubGameState::DataTestAet::Dest() {
    clear_color = 0xFF000000;
    dtm_aet_unload();
    return true;
}

bool SubGameState::DataTestAuth3d::Init() {
    app::TaskWork::add_task(auth_3d_test_task, "AUTH3DTEST");
    return true;
}

bool SubGameState::DataTestAuth3d::Ctrl() {
    return false;
}

bool SubGameState::DataTestAuth3d::Dest() {
    auth_3d_test_task->del();
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
    task_rob_manager_del_task();
    return task_data_test_opd->add_task();
}

bool SubGameState::DataTestOpd::Ctrl() {
    return !app::TaskWork::check_task_ready(task_data_test_opd);
}

bool SubGameState::DataTestOpd::Dest() {
    task_rob_manager_add_task();
    task_data_test_opd->del();
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
    app::TaskWork::add_task(task_data_test_glitter_particle, "DATA_TEST_PARTICLE");
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
    //if (data_test_chr->check_task_ready())
    //    data_test_chr->del();
    task_data_test_glitter_particle->del();
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

bool SubGameState::DataEditMain::Init() { // Added
    app::TaskWork::add_task(data_edit_sel, "DATA_TEST_EDIT");
    return true;
}

bool SubGameState::DataEditMain::Ctrl() {
    int32_t state = data_edit_sel_get_sub_state();
    if (state >= 0)
        game_state_set_sub_game_state_next((SubGameStateEnum)state);
    return false;
}

bool SubGameState::DataEditMain::Dest() {
    data_edit_sel->del();
    return true;
}

bool SubGameState::DataEditGlitterEditor::Init() { // Added
    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f });

    app::TaskWork::add_task(&glitter_editor, "GLITTER EDITOR", 0);
    return true;
}

bool SubGameState::DataEditGlitterEditor::Ctrl() { // Added
    return false;
}

bool SubGameState::DataEditGlitterEditor::Dest() { // Added
    glitter_editor.del();
    return true;
}

#if FACE_ANIM
bool SubGameState::DataEditFaceAnim::Init() { // Added
    app::TaskWork::add_task(&face_anim, "FACE ANIM", 0);
    return true;
}

bool SubGameState::DataEditFaceAnim::Ctrl() { // Added
    return false;
}

bool SubGameState::DataEditFaceAnim::Dest() { // Added
    face_anim.del();
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
            if (app::TaskWork::has_tasks_dest()) {
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
            if (app::TaskWork::has_tasks_dest()) {
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

GameStateEnum game_state_get_game_state() {
    return game_state_get()->game_state;
}

bool game_state_get_pause() {
    GameState* game_state = game_state_get();
    SubGameStateData* sub_game_state_data = sub_game_state_data_array_get(game_state->sub_game_state);
    if (sub_game_state_data)
        return sub_game_state_data->pause;
    return true;
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
    SubGameStateData* sub_game_state_data = sub_game_state_data_array_get(game_state->sub_game_state);
    if (!sub_game_state_data) {
        game_state->sub_call_count++;
        return false;
    }

    bool(*func)() = 0;
    switch (game_state->sub_mode) {
    case 0:
        func = sub_game_state_data->Init;
        break;
    case 1:
        func = sub_game_state_data->Ctrl;
        break;
    case 2:
        func = sub_game_state_data->Dest;
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

        dw_console_printf(DW_CONSOLE_MODE_CTRL, "[%s]->[%s]\n",
            game_state_names[game_state_prev], game_state_names[state]);

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
