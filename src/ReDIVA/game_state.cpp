/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "game_state.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/customize_item_table.hpp"
#include "../CRE/module_table.hpp"
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
#include "am_data.hpp"
#include "data_initialize.hpp"
#include "input_state.hpp"
#include "mask_screen.hpp"
#include "system_startup.hpp"
#include "task_movie.hpp"
#include "wait_screen.hpp"
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
    int32_t inner_state;
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
        struct Data {
            bool load;
            int32_t pv_state;
            int32_t index;
            int32_t difficulty_select;
            int32_t state;
        };

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
int32_t startup_error_code;
bool test_mode;

#if PV_DEBUG
bool pv_x;
#endif
#if BAKE_X_PACK
bool pv_x_bake;
#endif

GameStateEnum data_edit_game_state_prev;
GameStateEnum data_test_game_state_prev;

SubGameState::Demo::Data sub_game_state_demo_data;

std::vector<std::pair<int32_t, pv_difficulty[2]>> stru_1411A12E0;
std::vector<uint32_t> stru_1411A12F8;

static bool game_state_call_sub(GameState* game_state);
static GameState* game_state_get();
static const char* game_state_get_name(GameStateEnum state);
static void game_state_set_state(GameStateEnum state, SubGameStateEnum sub_state);

static GameStateData* game_state_data_array_get(GameStateEnum state);
static SubGameStateData* sub_game_state_data_array_get(SubGameStateEnum state);

static bool adv_load();
static void adv_read();
static void adv_unload();

static bool check_photo_mode_demo_movie_exists();

static void game_state_set_inner_state(int32_t state);

static void sub_game_state_demo_data_difficulty_select_increment();

bool GameState::Startup::Init() {
    startup_error_code = false;
    return true;
}

bool GameState::Startup::Ctrl() {
    return false;
}

bool GameState::Startup::Dest() {
    /*if (game_state_get()->game_state_next != GAME_STATE_APP_ERROR && !test_mode_get())
        sub_14066C670();*/

    if (test_mode_get())
        game_state_set_game_state_next(GAME_STATE_TEST_MODE);
    else {
        //sub_14066D190();

        GameStateEnum state = game_state_get()->game_state_next == GAME_STATE_TEST_MODE
            ? GAME_STATE_TEST_MODE : GAME_STATE_ADVERTISE;
        game_state_set_game_state_next(state);
    }
    return true;
}

bool GameState::Advertise::Init() {
    switch (game_state_get()->inner_state) {
    case 0:
        //net_alw_detail::context_ptr_get()->field_168(0);
        //net_alw_detail::context_ptr_get()->field_C8(1);
        //sub_14066C680();
        //task_slider_control_get()->sub_140618840();
        //task_slider_control_get()->sub_140618860();

        adv_read();
        game_state_set_inner_state(1);

        //sub_1404A7370(player_data_array);
        //sub_14038AFC0(sub_14038BB30());

        if (game_state_get()->game_state != game_state_get()->game_state_prev) {
            //sub_1403F4670();
            sub_game_state_demo_data_difficulty_select_increment();
        }

        /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
        task_lamp_ctrl->field_74 = 1;
        task_lamp_ctrl->field_78 = color_white;*/

        //task_slider_control_get()->sub_140618980(2);

        /*if (task_printer_get()->check_task_ready() && !wrap_collection_get()->printer.get_value())
            task_printer_get()->del();*/
        break;
    case 1:
        if (!adv_load())
            game_state_set_inner_state(2);
        break;
    case 2:
        //app::TaskWork::add_task(task_information_ptr, "INFORMATION");

        task_wait_screen_add_task();
        //sub_1401F4440();
        //sub_1401E87E0();

        const float_t min_volume = get_min_speakers_volume();
        const float_t max_volume = get_max_speakers_volume();
        sound_work_set_speakers_volume((float_t)wrap_collection_get()->advertise_sound_volume.get_value()
            * (max_volume - min_volume) * 0.01f + min_volume);

        //sub_1402B7880()->sub_1402BAE80();
        module_data_handler_data_add_all_modules();
        customize_item_data_handler_data_add_all_customize_items();
        sound_work_reset_all_se();
        //adv_touch_get()->add(sub_1402103A0());
        //adv_festa_get()->add();
        //adv_noblesse_get()->add();
        //task_information_ptr->field_77 = false;
        //sub_1403F35A0();
        return true;
    }
    return false;
}

bool GameState::Advertise::Ctrl() {
    /*task_information_ptr->sub_1403BB780(2);

    if (sub_14066CC00()) {
        sub_14066FA90();
        game_state_set_error_code(91);
    }*/

    /*if (sub_1403BADE0())
        sub_1403BB7B0();*/

    //*(uint8_t*)((size_t)task_aime_get() + 0x15C) = game_state_is_advertise_not_sub_demo();

    /*if (!sup_err_task_supplies_error_check_task_ready() && task_information_ptr->field_77
        && wrap_collection_get()->printer.get_value() && sub_140662AD0() != 3)
        task_information_ptr->field_77 = false;*/

    const InputState* input_state = input_state_get(0);
    bool v2 = false;//sub_1403F49C0();
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_START) && v2) {
        game_state_set_game_state_next(GAME_STATE_GAME);
        sound_work_play_se(0, "se_sy_01", 1.0);
    }

    /*adv_touch_get()->sub_14014ED60(true);
    if (!v2)
        adv_touch_get()->sub_14014ED60(false);

    adv_touch_get()->sub_14014EC20(v2);
    if (game_state_get()->sub_game_state == SUB_GAME_STATE_TITLE)
        adv_touch_get()->sub_14014EC20(false);

    if (game_state_get()->sub_game_state == SUB_GAME_STATE_CM && !*(uint8_t*)((size_t)adv_cm_get() + 0x108))
        adv_touch_get()->sub_14014EC20(false);

    if (game_state_get()->sub_game_state == SUB_GAME_STATE_PHOTO_MODE_DEMO && !check_photo_mode_demo_movie_exists())
        adv_touch_get()->sub_14014EC20(false);

    if (input_state->CheckDown(INPUT_BUTTON_JVS_L) || input_state->CheckDown(INPUT_BUTTON_JVS_R))
        adv_touch_get()->sub_14014ED60(false);

    if (adv_touch_get()->field_70) {
        game_state_set_game_state_next(GAME_STATE_GAME);
        sound_work_play_se(0, "se_sy_01");
        adv_touch_get()->sub_14014EC00();
    }*/

    /*if (sub_14022EF20(0)->sub_14022ED30() && v2)
        game_state_set_game_state_next(GAME_STATE_GAME);*/
    return false;
}

bool GameState::Advertise::Dest() {
    //task_slider_control_get()->sub_140618840(v0);
    //adv_touch_get()->del();
    //adv_festa_get()->del();
    //adv_noblesse_get()->del();
    adv_unload();
    sound_work_set_speakers_volume(get_max_speakers_volume());
    //*(uint8_t*)((size_t)task_aime_get() + 0x15C) = 0;
    return true;
}

bool GameState::Game::Init() {
    //task_information_ptr->sub_1403BB780(4);
    //sub_14066C660();
    task_rob_manager_add_task();
    return true;
}

bool GameState::Game::Ctrl() {
    return false;
}

bool GameState::Game::Dest() {
    if (!task_pv_game_del_task() || !task_rob_manager_del_task())
        return false;

    //sub_1403935A0();
    //task_sel_ticket_del_task();
    //sel_vocal_change_get()->del();
    return true;
}

bool GameState::TestMode::Init() {
    test_mode_set(true);
    sound_work_reset_all_se();

    if (!test_mode_get()) {
        //sub_1401F4440();
        //sub_1401E87E0();
    }

    test_mode_set(1);

    //task_information_ptr->sub_1403BB780(0);

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 0;
    task_lamp_ctrl->field_78 = color_white;
    task_lamp_ctrl->field_7C = 0;*/

    //task_slider_control_get()->sub_140618980(0);

    task_wait_screen_set_load_loop_none();
    task_mask_screen_fade_in(0.0f, 0);

    sound_work_reset_all_se();
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    return true;
}

bool GameState::TestMode::Ctrl() {
    return false;
}

bool GameState::TestMode::Dest() {
    //sub_1401F4420();
    test_mode_set(false);
    return true;
}

bool GameState::AppError::Init() {
    //task_information_ptr->del();

    task_wait_screen_set_load_loop_none();
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    sound_work_reset_all_se();

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 0;
    task_lamp_ctrl->field_78 = color_white;*/

    //task_slider_control_get()->sub_140618980(0);
    //task_photo_service_del();
    return true;
}

bool GameState::AppError::Ctrl() {
    return false;
}

bool GameState::AppError::Dest() {
    return true;
}

bool GameState::DataTest::Init() {
    rctx_ptr->render_manager->set_multisample(false);
    rctx_ptr->render_manager->set_clear(true);
    data_test_game_state_prev = game_state_get()->game_state_prev;
    //task_information_ptr->sub_1403BB780(0);
    task_wait_screen_set_load_loop_none();
    task_rob_manager_add_task();
    //touch_util::touch_reaction_set_enable(false);
    //net_alw_detail::context_ptr_get()->field_C8(2);
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
    //touch_util::touch_reaction_set_enable(true);
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
    task_system_startup_add_task();
    task_pv_db_add_task();

    /*if (!sub_14066E820())
        game_state_set_error_code(949);*/

    resolution_struct* res_wind = res_window_get();

    //sub_14066D190();

    if (!test_mode_get()) {
        /*if (res_wind->resolution_mode != RESOLUTION_MODE_HD)
            game_state_set_error_code(910);*/

        /*if (sub_14066C9A0()) {
            sub_14066FA90();
            game_state_set_error_code(90);
        }*/
    }

    /*if (sub_1401E8A40())
        sub_1406A1DE0(task_touch_ptr[0], 0);*/

    /*if (sub_1401E89A0())
        task_aime_get()->sub_14017A890(9);*/

    //task_lamp_ctrl_get()->sub_1403C8B40();

    /*if (sub_1401E8A50())
        task_slider_control_get()->sub_140618A60();*/

    /*sub_1403D4760();*/

    /*const Wrap_collection* wrap_collection = wrap_collection_get();
    if (wrap_collection && wrap_collection->printer.get_value())
        task_printer_get()->sub_1404C0F90();*/
    return true;
}

bool SubGameState::SystemStartup::Ctrl() {
    return !!system_startup_ready;
}

bool SubGameState::SystemStartup::Dest() {
    task_system_startup_del_task();
    //app::TaskWork::add_task(task_closing_ptr, "CLOSING");
    //app::TaskWork::add_task(task_touch_area_ptr, "TOUCH AREA", 0);
    //sound_volume_get();
    //sub_140623810();
    //sub_1406A1F30();

    task_mask_screen_add_task();
    game_state_set_sub_game_state_next(SUB_GAME_STATE_WARNING);

    if (!test_mode_get()) {
        //sub_14066D190();

        /*uint8_t v2[4];
        net_alw_detail::context_ptr_get()->field_90(v2);
        if (!v2[1] && !v2[3]) {
            startup_error_code = 8005;
            game_state_set_sub_game_state_next(SUB_GAME_STATE_SYSTEM_STARTUP_ERROR);
            sub_140022740();
        }*/
    }
    return true;
}

bool SubGameState::SystemStartupError::Init() {
    //task_mode_app_error_add_task(startup_error_code);
    return true;
}

bool SubGameState::SystemStartupError::Ctrl() {
    GameState* game_state = game_state_get();
    if (game_state->game_state || game_state->sub_game_state != SUB_GAME_STATE_SYSTEM_STARTUP_ERROR)
        return false;

    const InputState* input_state = input_state_get(0);
    return input_state->CheckDown(INPUT_BUTTON_JVS_START) && input_state->CheckTapped(INPUT_BUTTON_JVS_TEST);
}

bool SubGameState::SystemStartupError::Dest() {
    //task_mode_app_error_del_task();
    game_state_set_sub_game_state_next(SUB_GAME_STATE_WARNING);
    return true;
}

bool SubGameState::Warning::Init() {
    /*if (sub_14066F660() && !test_mode_get() && !sub_14066D030())
        app::TaskWork::add_task(&task_warning, "WARNING");*/
    return true;
}

bool SubGameState::Warning::Ctrl() {
    return true;
    //return !app::TaskWork::check_task_ready(&task_warning);
}

bool SubGameState::Warning::Dest() {
    //task_warning.del();
    return true;
}

bool SubGameState::Logo::Init() {
    //task_information_ptr->sub_1403BB780(1);
    //task_adv_logo.add(0);
    //sub_1403F35A0();
    return true;
}

bool SubGameState::Logo::Ctrl() {
    return true;
    //*task_information_ptr->sub_1403BB780(1);
    //return !app::TaskWork::check_task_ready(&task_adv_logo);
}

bool SubGameState::Logo::Dest() {
    //task_information_ptr->sub_1403BB780(1);
    //sub_14066E8D0();
    //app::Task::del(&task_adv_logo);
    return true;
}

bool SubGameState::Rating::Init() {
    //sub_1403F35A0();
    return true;
}

bool SubGameState::Rating::Ctrl() {
    return true;
    //return !app::TaskWork::check_task_ready(&task_adv_rating);
}

bool SubGameState::Rating::Dest() {
    //app::Task::del(&task_adv_rating);
    return true;
}

bool SubGameState::Demo::Init() {
    rctx_ptr->render_manager->set_multisample(false);

    if (check_photo_mode_demo_movie_exists())
        sub_game_state_demo_data.state = 1;

    switch (sub_game_state_demo_data.state) {
    case 0: {
        resolution_struct* res_wind = res_window_get();
        resolution_struct* res_wind_int = res_window_internal_get();
        res_window_get();

        rectangle rect;
        rect.pos = { 0.0f, (float_t)(res_wind->height - res_wind_int->height) * 0.5f };
        rect.size = { (float_t)res_wind_int->width, (float_t)res_wind_int->height };

        TaskMovie::SprParams spr_params;
        spr_params.disp.rect = rect;
        spr_params.disp.resolution_mode = res_wind->resolution_mode;
        spr_params.disp.scale = -1.0f;
        spr_params.disp.field_18 = 0;
        spr_params.disp.index = -1;
        spr_params.prio = spr::SPR_PRIO_07;
        task_movie_get(0)->Reset(spr_params);
        //task_movie_get(0)->Load("rom/movie/diva_adv.wmv");
    } break;
    case 1:
        task_wait_screen_set_load_loop_demo_load();
        sub_game_state_demo_data.load = false;
        task_rob_manager_add_task();
        sub_game_state_demo_data.pv_state = 0;
        break;
    }

    //sub_1403F35A0();
    return true;
}

bool SubGameState::Demo::Ctrl() {
    bool ret = false;
    bool v1 = false;
    /*if (sup_err_task_supplies_error_check_task_ready()) {
        if (!task_pv_game_check_task_ready())
            return true;

        v1 = true;
    }*/

    switch (sub_game_state_demo_data.state) {
    case 0:
        ret = !task_movie_get(0)->CheckState();
        break;
    case 1: {
        if (sub_game_state_demo_data.load) {
            if (!task_pv_game_check_task_ready())
                ret = true;
        }
        else if (!task_wait_screen_get_started()) {
            int32_t index = sub_game_state_demo_data.index;
            if (stru_1411A12F8.size()) {
                size_t size = stru_1411A12E0.size();
                if (size) {
                    uint32_t pv_index = stru_1411A12F8.data()[sub_game_state_demo_data.index];
                    if (pv_index < size) {
                        const std::pair<int32_t, pv_difficulty[2]>* elem = &stru_1411A12E0.data()[pv_index];
                        if (elem) {
                            pv_difficulty diff = elem->second[sub_game_state_demo_data.difficulty_select];
                            if (diff != PV_DIFFICULTY_MAX) {
                                if (!task_pv_game_init_demo_pv(elem->first, diff, true))
                                    task_wait_screen_set_load_loop_none();
                                goto Next;
                            }
                        }
                    }
                }
            }

            v1 = true;

        Next:
            sub_game_state_demo_data.index = ++index;

            if (index >= stru_1411A12E0.size()) {
                sub_game_state_demo_data.index = 0;
                sub_game_state_demo_data_difficulty_select_increment();
            }
            sub_game_state_demo_data.load = true;
        }
    } break;
    default:
        ret = 1;
        break;
    }

    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v1 = true;

    if (v1 && task_wait_screen_check_index_none())
        ret = true;
    return ret;
}

bool SubGameState::Demo::Dest() {
    //net_alw_detail::context_ptr_get()->field_C8(1);

    switch (sub_game_state_demo_data.state) {
    case 0:
        task_movie_get(0)->Unload();
        task_movie_get(0)->del();
        sub_game_state_demo_data.state = 1;
    case 1:
        switch (sub_game_state_demo_data.pv_state) {
        case 0:
        default:
            if (pv_game_get())
                pv_game_get()->unload();
            sub_game_state_demo_data.pv_state = 1;
            return false;
        case 1:
            if (task_pv_game_del_task() && task_rob_manager_del_task())
                sub_game_state_demo_data.pv_state = 2;
            return false;
        case 2:
            task_wait_screen_set_load_loop_none();
            task_mask_screen_fade_in(0.0f, 0);
            sub_game_state_demo_data.state = 0;
            break;
        }
        break;
    }

    rctx_ptr->render_manager->set_multisample(true);
    return true;
}

bool SubGameState::Title::Init() {
    //sub_140141F80(&task_adv_title, 0, false);
    //sub_1403F35A0();
    return true;
}

bool SubGameState::Title::Ctrl() {
    return true;
    //return !app::TaskWork::check_task_ready(&task_adv_title);
}

bool SubGameState::Title::Dest() {
    //sub_14066E8D0();
    //app::Task::del(&task_adv_title);
    return true;
}

bool SubGameState::Ranking::Init() {
    //adv_rank_main_get();
    //sub_140143280();
    //sub_1403F35A0();
    return true;
}

bool SubGameState::Ranking::Ctrl() {
    return true;
    //return !app::TaskWork::check_task_ready(adv_rank_main_get());
}

bool SubGameState::Ranking::Dest() {
    //sub_14066E8D0();
    //adv_rank_main_get()->del();
    return true;
}

bool SubGameState::ScoreRanking::Init() {
    //adv_score_rank_main_get();
    //sub_140149010();
    //sub_1403F35A0();
    return true;
}

bool SubGameState::ScoreRanking::Ctrl() {
    return true;
    //return !app::TaskWork::check_task_ready(adv_score_rank_main_get());
}

bool SubGameState::ScoreRanking::Dest() {
    //sub_14066E8D0();
    //adv_score_rank_main_get()->del();
    return true;
}

bool SubGameState::CM::Init() {
    //adv_cm_get();
    //sub_14013F750();
    //sub_1403F35A0();
    return true;
}

bool SubGameState::CM::Ctrl() {
    return true;
    /*bool v0 = false;
    bool v1 = false;//adv_cm_get()->field_68 == 3;
    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v0 = true;
    return v0 ? v0 : v1;*/
}

bool SubGameState::CM::Dest() {
    //sub_14066E8D0();
    //adv_cm_get()->del();
    return true;
}

bool SubGameState::PhotoModeDemo::Init() {
    if (check_photo_mode_demo_movie_exists()) {
        resolution_struct* res_wind = res_window_get();
        resolution_struct* res_wind_int = res_window_internal_get();

        rectangle rect;
        rect.pos = { 0.0f, (float_t)(res_wind->height - res_wind_int->height) * 0.5f };
        rect.size = { (float_t)res_wind_int->width, (float_t)res_wind_int->height };

        TaskMovie::SprParams spr_params;
        spr_params.disp.rect = rect;
        spr_params.disp.resolution_mode = res_wind->resolution_mode;
        spr_params.disp.scale = -1.0f;
        spr_params.disp.field_18 = 0;
        spr_params.disp.index = -1;
        spr_params.prio = spr::SPR_PRIO_07;
        task_movie_get(0)->Reset(spr_params);
        //task_movie_get(0)->Load("rom/movie/diva_adv02.wmv");

        //adv_festa_get()->field_6C = true;
        //adv_noblesse_get()->sub_140142530(v6, true);
    }
    else {
        //adv_festa_get()->field_6C = false;
        //adv_noblesse_get()->sub_140142530(v6, false);
    }

    //sub_1403F35A0();
    return true;
}

bool SubGameState::PhotoModeDemo::Ctrl() {
    return true;
    /*if (!check_photo_mode_demo_movie_exists())
        return true;

    bool v2 = !task_movie_get(0)->CheckState();

    bool v4 = false;
    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v4 = true;
    if (v4 && task_wait_screen_check_index_none())
        v2 = true;
    return v2;*/
}

bool SubGameState::PhotoModeDemo::Dest() {
    if (check_photo_mode_demo_movie_exists()) {
        task_movie_get(0)->Unload();
        task_movie_get(0)->del();
    }
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

    /*task_information_ptr->sub_1403BB780(3);
    sub_1402B7880()->sub_1402BAE80();
    module_data_handler_data_add_all_modules();
    //sub_14022CE00()->sub_14022DC10();
    //sub_1404FF3D0();
    //sel_main_get()->add_task(false);
    //task_slider_control_get()->sub_140618980(1);
    return true;*/
}

bool SubGameState::Selector::Ctrl() {
#if PV_DEBUG
    if (!pv_x) {
        PVGameSelector* sel = pv_game_selector_get();
        if (sel->exit) {
            if (sel->start && pv_game_init()) {
                struc_717* v0 = sub_14038BB30();
                v0->field_0.stage_index = 0;

                struc_716* v2 = v0->get_stage();
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

    /*if (app::TaskWork::check_task_ready(sel_main_get())) {
        if (sel_main_get()->field_7A && !task_pv_game_check_task_ready())
            task_pv_game_init_pv();
        return false;
    }
    else {
        if (sel_main_get()->field_79)
            game_state_set_game_state_next(GAME_STATE_ADVERTISE);
        else
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
        return true;
    }*/
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

    /*if (app::TaskWork::check_task_ready(sel_main_get())) {
        sel_main_get()->del();
        return false;
    }
    return true;*/
}

bool SubGameState::GameMain::Init() {
    sound_work_release_stream(0);
    game_state_set_inner_state(0);
    //task_information_ptr->sub_1403BB780(4);
    rctx_ptr->render_manager->set_multisample(false);
    return true;
}

bool SubGameState::GameMain::Ctrl() {
    switch (game_state_get()->inner_state) {
    case 0:
        //if (task_wait_screen_get_started())
        //    break;

        game_state_set_inner_state(1);
        break;
    case 1:
        if (task_wait_screen_check_index_none()) {
            bool watch = sub_14038BB30()->field_0.watch;
            //sound_volume_get()->show_enable(true, watch);

            /*if (watch)
                task_photo_service_add();*/
            /*else {
                if (!app::TaskWork::check_task_ready(act_toggle_get()))
                    act_toggle_get()->sub_14013CE40();
                *(int32_t*)((size_t)act_toggle_get() + 0xA0) = 5;
            }*/

            //sound_volume_get()->field_6C = spr::SPR_PRIO_04;
            //sound_volume_get()->field_70 = spr::SPR_PRIO_05;
        }

#if PV_DEBUG
        if (!pv_x) {
            if (!task_pv_game_check_task_ready()) {
                if (!sub_14038BB30()->field_0.watch) {
                    //game_state_set_sub_game_state_next(SUB_GAME_STATE_STAGE_RESULT);
                    return true;
                }

                /*if (player_data_array_get(0)->field_0)
                    net_mgr::net_mgr_stage_result_add_task();*/

                game_state_set_inner_state(2);
            }
            break;
        }
#endif

        if (!app::TaskWork::check_task_ready(x_pv_game_get()))
            return true;
        break;
    case 2: {
        /*if (net_mgr::net_mgr_stage_result_check_task_ready())
            break;*/

        /*std::vector<texture*> textures;
        //sub_140490230(textures);
        if (textures.size() > 0)
            game_state_set_sub_game_state_next(SUB_GAME_STATE_SCREEN_SHOT_SEL);
        else {
            //task_photo_service_del();
            if (!sub_1403F6B00())
                game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_OVER);
            else
                game_state_set_sub_game_state_next(SUB_GAME_STATE_SCREEN_SHOT_RESULT);
        }*/
    } return true;
    }
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

    task_mask_screen_fade_in(0.0f, 0);

    //sound_volume_get()->field_6C = spr::SPR_PRIO_09;
    //sound_volume_get()->field_70 = spr::SPR_PRIO_10;
    //*(int32_t*)((size_t)act_toggle_get() + 0xA0) = 10;
    //sound_volume_get()->show_enable(false, false);
    //act_toggle_get()->del();

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 1;
    task_lamp_ctrl->field_78 = color_white;*/

    //task_slider_control_get()->sub_140618980(1);

    rctx_ptr->render_manager->set_multisample(true);
    return true;
}

bool SubGameState::GameSel::Init() {
    //task_information_ptr->sub_1403BB780(3);
    //sel_main_get()->add_task(true);
    return true;
}

bool SubGameState::GameSel::Ctrl() {
    return true;
    /*if (app::TaskWork::check_task_ready(sel_main_get())) {
        if (sel_main_get()->field_7A && !task_pv_game_check_task_ready())
            task_pv_game_init_pv();
        return false;
    }
    else {
        game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_MAIN);
        return true;
    }*/
}

bool SubGameState::GameSel::Dest() {
    /*if (app::TaskWork::check_task_ready(sel_main_get()))
        sel_main_get()->del();*/
    return true;
}

bool SubGameState::StageResult::Init() {
    //task_information_ptr->sub_1403BB780(3);
    //stage_result_main_get();
    //sub_140656320();
    return true;
}

bool SubGameState::StageResult::Ctrl() {
    return true;
    /*if (app::TaskWork::check_task_ready(stage_result_main_get()))
        return false;

    if (sub_14038BB30()->field_0.no_fail) {
        game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_OVER);
        return true;
    }

    int32_tv2 = sub_14038BB30()->field_0.stage_index;
    sub_14038BB30();
    int32_t v3 = 2;//sub_14038AEE0();
    //if (player_data_array_get(0)->contest.enable)
    //    sub_14038BB30()->sub_14038D080(false);

    if (v2 >= v3 - 1)
        game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_OVER);
    else {
        struc_716* v7 = sub_14038BB30()->get_stage();
        if (v7->field_0 == -1 && !v7->field_128.next_stage) {
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_OVER);
        }
        else {
            sub_14038BB30()->cycle_state_index();
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_SEL);
        }
    }
    return true;*/
}

bool SubGameState::StageResult::Dest() {
    //stage_result_main_get()->del();
    return true;
}

bool SubGameState::ScreenShotSel::Init() {
    //task_information_ptr->sub_1403BB780(3);
    //sel_screen_shot_get();
    //sub_1405D7C60();
    return true;
}

bool SubGameState::ScreenShotSel::Ctrl() {
    return true;
    /*if (app::TaskWork::check_task_ready(sel_screen_shot_get()))
        return false;

    game_state_set_sub_game_state_next(sub_1403F6B00()
        ? SUB_GAME_STATE_SCREEN_SHOT_RESULT : SUB_GAME_STATE_GAME_OVER);
    return true;*/
}

bool SubGameState::ScreenShotSel::Dest() {
    //sel_screen_shot_get()->del();
    return true;
}

bool SubGameState::ScreenShotResult::Init() {
    //task_information_ptr->sub_1403BB780(3);
    //screen_shot_result_main_get();
    //sub_140559010();
    return true;
}

bool SubGameState::ScreenShotResult::Ctrl() {
    return true;
    /*if (app::TaskWork::check_task_ready(screen_shot_result_main_get()))
        return false;

    game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_OVER);
    return true;*/
}

bool SubGameState::ScreenShotResult::Dest() {
    //screen_shot_result_main_get()->del();
    return true;
}

bool SubGameState::GameOver::Init() {
    //task_information_ptr->sub_1403BB780(5);

    /*bool v0 = true;
    int32_t v1 = sub_14038BB30()->field_0.stage_index;
    sub_14038BB30();

    int32_t v2 = sub_14038AEE0() - 1;
    if (!sub_1402103A0())
        v0 = false;
    if (sub_14038BB30()->field_0.no_fail)
        v0 = false;
    if (!sub_1403933C0())
        v0 = false;

    if (v1 == v2) {
        if (sub_14038BB30()->get_stage()->field_0 != -1)
            v0 = false;
    }
    else if (v1 > v2)
        v0 = false;
    game_over_main_add_task(game_over_main_get(), v0);*/
    return true;
}

bool SubGameState::GameOver::Ctrl() {
    return true;
    /*if (app::TaskWork::check_task_ready(game_over_main_get())) {
        struc_705* v2 = sub_14022EF20(0);
        if (v2 && sub_14022ED30(v2))
            *(uint8_t*)((size_t)game_over_main_get() + 0x8D) = 1;
    }
    else {
        if (*(uint8_t*)((size_t)game_over_main_get() + 0x8D))
            game_state_set_sub_game_state_next(SUB_GAME_STATE_GAME_SEL);
        else
            game_state_set_game_state_next(GAME_STATE_ADVERTISE);
    }
    return false;*/
}

bool SubGameState::GameOver::Dest() {
    //game_over_main_get()->del();
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

bool game_state_is_advertise_not_sub_demo() {
    GameState* game_state = game_state_get();
    if (game_state->game_state == GAME_STATE_ADVERTISE)
        return game_state->sub_game_state != SUB_GAME_STATE_DEMO;
    return false;
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

static const char* game_state_get_name(GameStateEnum state) {
    return game_state_names[state];
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
        game_state->inner_state = 0;

        dw_console_printf(DW_CONSOLE_MODE_CTRL, "[%s]->[%s]\n",
            game_state_get_name(game_state_prev), game_state_get_name(state));

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

static bool adv_load() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    if (sprite_manager_load_file(37, aft_spr_db) || aet_manager_load_file(30, aft_aet_db))
        return true;
    return sound_work_load_farc("rom/sound/se_adv.farc");
}

static void adv_read() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    sprite_manager_read_file(37, "", aft_data, aft_spr_db);
    aet_manager_read_file(30, "", aft_data, aft_aet_db);
    sound_work_read_farc("rom/sound/se_adv.farc");
}

static void adv_unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    sprite_manager_unload_set(37, aft_spr_db);
    aet_manager_unload_set(30, aft_aet_db);
    sound_work_unload_farc("rom/sound/se_adv.farc");
}

static bool check_photo_mode_demo_movie_exists() {
    if (wrap_collection_get()->printer.get_value())
        return data_list[DATA_AFT].check_file_exists("rom/movie/diva_adv02.wmv");
    return false;
}

static void game_state_set_inner_state(int32_t state) {
    game_state_get()->inner_state = state;
}

static void sub_game_state_demo_data_difficulty_select_increment() {
    int32_t difficulty_select = sub_game_state_demo_data.difficulty_select + 1;
    if (difficulty_select < 0 || difficulty_select >= 1)
        difficulty_select = 0;
    sub_game_state_demo_data.difficulty_select = difficulty_select;
}
