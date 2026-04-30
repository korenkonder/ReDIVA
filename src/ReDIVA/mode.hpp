/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "config.hpp"
#include "../KKdLib/default.hpp"

enum Mode {
    MODE_STARTUP = 0,
    MODE_ADVERTISE,
    MODE_GAME,
    MODE_DATA_TEST,
    MODE_TEST_MODE,
    MODE_APP_ERROR,
    MODE_DATA_EDIT, // Added
    MODE_MAX,
};

enum ModeProc {
    MODE_PROC_INIT = 0,
    MODE_PROC_CTRL,
    MODE_PROC_DEST,
    MODE_PROC_MAX,
};

enum ModeSub {
    MODE_SUB_DATA_INITIALIZE = 0,
    MODE_SUB_SYSTEM_STARTUP,
    MODE_SUB_SYSTEM_STARTUP_ERROR,
    MODE_SUB_WARNING,
    MODE_SUB_LOGO,
    MODE_SUB_RATING,
    MODE_SUB_DEMO,
    MODE_SUB_TITLE,
    MODE_SUB_RANKING,
    MODE_SUB_SCORE_RANKING,
    MODE_SUB_CM,
    MODE_SUB_PHOTO_MODE_DEMO,
    MODE_SUB_SELECTOR,
    MODE_SUB_GAME_MAIN,
    MODE_SUB_GAME_SEL,
    MODE_SUB_STAGE_RESULT,
    MODE_SUB_SCREEN_SHOT_SEL,
    MODE_SUB_SCREEN_SHOT_RESULT,
    MODE_SUB_GAME_OVER,
    MODE_SUB_DATA_TEST_MAIN,
    MODE_SUB_DATA_TEST_MISC,
    MODE_SUB_DATA_TEST_OBJ,
    MODE_SUB_DATA_TEST_STG,
    MODE_SUB_DATA_TEST_MOT,
    MODE_SUB_DATA_TEST_COLLISION,
    MODE_SUB_DATA_TEST_SPR,
    MODE_SUB_DATA_TEST_AET,
    MODE_SUB_DATA_TEST_AUTH_3D,
    MODE_SUB_DATA_TEST_CHR,
    MODE_SUB_DATA_TEST_ITEM,
    MODE_SUB_DATA_TEST_PERF,
    MODE_SUB_DATA_TEST_PVSCRIPT,
    MODE_SUB_DATA_TEST_PRINT,
    MODE_SUB_DATA_TEST_CARD,
    MODE_SUB_DATA_TEST_OPD,
    MODE_SUB_DATA_TEST_SLIDER,
    MODE_SUB_DATA_TEST_GLITTER,
    MODE_SUB_DATA_TEST_GRAPHICS,
    MODE_SUB_DATA_TEST_COLLECTION_CARD,
    MODE_SUB_TEST_MODE_MAIN,
    MODE_SUB_APP_ERROR,
    MODE_SUB_DATA_EDIT_MAIN, // Added
    MODE_SUB_DATA_EDIT_GLITTER_EDITOR, // Added
#if FACE_ANIM
    MODE_SUB_DATA_EDIT_FACE_ANIM, // Added
#endif
    MODE_SUB_MAX,
};

enum ModeSubProc {
    MODE_SUB_PROC_INIT = 0,
    MODE_SUB_PROC_CTRL,
    MODE_SUB_PROC_DEST,
    MODE_SUB_PROC_MAX,
};

struct ModeInfo {
    Mode current;
    Mode next;
    Mode old;
    ModeProc proc;
    bool shift;
    bool init;
    uint32_t time;
    uint32_t etc_work;
    ModeSub sub_current;
    ModeSub sub_next;
    ModeSub sub_old;
    ModeSubProc sub_proc;
    bool sub_shift;
    bool sub_init;
    uint32_t sub_call_count;
};

extern bool check_pause_enable();
extern void ctrl_mode();
extern Mode get_mode();
extern const ModeInfo* get_mode_info_const();
extern void init_mode(Mode mode, ModeSub mode_sub);
extern bool is_mode_advertise();
extern bool is_mode_advertise_not_sub_demo();
extern size_t print_mode(char* buf, size_t buf_size);
extern void set_etc_work(uint32_t work);
extern void shift_next_mode(Mode state);
extern void shift_next_mode_sub(ModeSub state);
