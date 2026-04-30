/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode.hpp"
#include "../CRE/task.hpp"
#include "information/dw_console.hpp"
#include "mode_advertise.hpp"
#include "mode_app_error.hpp"
#include "mode_data_edit.hpp"
#include "mode_data_test.hpp"
#include "mode_game.hpp"
#include "mode_startup.hpp"
#include "mode_test_mode.hpp"

struct ModeTable {
    Mode name;
    bool(*func[MODE_PROC_MAX])();
    ModeSub sub_list[24];
    Mode next;
    ModeSub sub_next;
};

struct ModeSubTable {
    ModeSub name;
    bool pause_enable;
    bool(*func[MODE_SUB_PROC_MAX])();
};

static ModeTable mode_table[] = {
    {
        MODE_STARTUP,
        init_startup,
        ctrl_startup,
        dest_startup,
        {
            MODE_SUB_DATA_INITIALIZE,
            MODE_SUB_SYSTEM_STARTUP,
            MODE_SUB_SYSTEM_STARTUP_ERROR,
            MODE_SUB_WARNING,
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_LOGO,
    },
    {
        MODE_ADVERTISE,
        init_advertise,
        ctrl_advertise,
        dest_advertise,
        {
            MODE_SUB_LOGO,
            MODE_SUB_RATING,
            MODE_SUB_DEMO,
            MODE_SUB_TITLE,
            MODE_SUB_SCORE_RANKING,
            MODE_SUB_RANKING,
            MODE_SUB_CM,
            MODE_SUB_PHOTO_MODE_DEMO,
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },
    {
        MODE_GAME,
        init_game,
        ctrl_game,
        dest_game,
        {
            MODE_SUB_SELECTOR,
            MODE_SUB_GAME_MAIN,
            //MODE_SUB_GAME_SEL,
            //MODE_SUB_STAGE_RESULT,
            //MODE_SUB_SCREEN_SHOT_SEL,
            //MODE_SUB_SCREEN_SHOT_RESULT,
            //MODE_SUB_GAME_OVER,
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },
    {
        MODE_TEST_MODE,
        init_test_mode,
        ctrl_test_mode,
        dest_test_mode,
        {
            MODE_SUB_TEST_MODE_MAIN,
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },
    {
        MODE_APP_ERROR,
        init_app_error,
        ctrl_app_error,
        dest_app_error,
        {
            MODE_SUB_APP_ERROR,
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },
    {
        MODE_DATA_TEST,
        init_data_test,
        ctrl_data_test,
        dest_data_test,
        {
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
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },

    { // Added
        MODE_DATA_EDIT,
        init_data_edit,
        ctrl_data_edit,
        dest_data_edit,
        {
            MODE_SUB_DATA_EDIT_MAIN,
            MODE_SUB_DATA_EDIT_GLITTER_EDITOR,
#if FACE_ANIM
            MODE_SUB_DATA_EDIT_FACE_ANIM,
#endif
            MODE_SUB_MAX,
        },
        MODE_ADVERTISE,
        MODE_SUB_MAX,
    },
};

static ModeSubTable mode_sub_table[] = {
    {
        MODE_SUB_DATA_INITIALIZE,
        true,
        init_data_initialize,
        ctrl_data_initialize,
        dest_data_initialize,
    },
    {
        MODE_SUB_SYSTEM_STARTUP,
        true,
        init_system_startup,
        ctrl_system_startup,
        dest_system_startup,
    },
    {
        MODE_SUB_SYSTEM_STARTUP_ERROR,
        true,
        init_system_startup_error,
        ctrl_system_startup_error,
        dest_system_startup_error,
    },
    {
        MODE_SUB_WARNING,
        true,
        init_warning,
        ctrl_warning,
        dest_warning,
    },
    {
        MODE_SUB_LOGO,
        true,
        init_logo,
        ctrl_logo,
        dest_logo,
    },
    {
        MODE_SUB_RATING,
        true,
        init_rating,
        ctrl_rating,
        dest_rating,
    },
    {
        MODE_SUB_DEMO,
        true,
        init_demo,
        ctrl_demo,
        dest_demo,
    },
    {
        MODE_SUB_TITLE,
        true,
        init_title,
        ctrl_title,
        dest_title,
    },
    {
        MODE_SUB_RANKING,
        true,
        init_ranking,
        ctrl_ranking,
        dest_ranking,
    },
    {
        MODE_SUB_SCORE_RANKING,
        true,
        init_score_ranking,
        ctrl_score_ranking,
        dest_score_ranking,
    },
    {
        MODE_SUB_CM,
        true,
        init_cm,
        ctrl_cm,
        dest_cm,
    },
    {
        MODE_SUB_PHOTO_MODE_DEMO,
        true,
        init_photo_mode_demo,
        ctrl_photo_mode_demo,
        dest_photo_mode_demo,
    },
    {
        MODE_SUB_SELECTOR,
        true,
        init_selector,
        ctrl_selector,
        dest_selector,
    },
    {
        MODE_SUB_GAME_MAIN,
        true,
        init_game_main,
        ctrl_game_main,
        dest_game_main,
    },
    {
        MODE_SUB_GAME_SEL,
        true,
        init_game_sel,
        ctrl_game_sel,
        dest_game_sel,
    },
    {
        MODE_SUB_STAGE_RESULT,
        true,
        init_stage_result,
        ctrl_stage_result,
        dest_stage_result,
    },
    {
        MODE_SUB_SCREEN_SHOT_SEL,
        true,
        init_screen_shot_sel,
        ctrl_screen_shot_sel,
        dest_screen_shot_sel,
    },
    {
        MODE_SUB_SCREEN_SHOT_RESULT,
        true,
        init_screen_shot_result,
        ctrl_screen_shot_result,
        dest_screen_shot_result,
    },
    {
        MODE_SUB_GAME_OVER,
        true,
        init_game_over,
        ctrl_game_over,
        dest_game_over,
    },
    {
        MODE_SUB_DATA_TEST_MAIN,
        true,
        init_data_test_main_md,
        ctrl_data_test_main_md,
        dest_data_test_main_md,
    },
    {
        MODE_SUB_DATA_TEST_MISC,
        true,
        init_data_test_misc_md,
        ctrl_data_test_misc_md,
        dest_data_test_misc_md,
    },
    {
        MODE_SUB_DATA_TEST_OBJ,
        false,
        init_data_test_obj_md,
        ctrl_data_test_obj_md,
        dest_data_test_obj_md,
    },
    {
        MODE_SUB_DATA_TEST_STG,
        false,
        init_data_test_stg_md,
        ctrl_data_test_stg_md,
        dest_data_test_stg_md,
    },
    {
        MODE_SUB_DATA_TEST_MOT,
        false,
        init_data_test_mot_md,
        ctrl_data_test_mot_md,
        dest_data_test_mot_md,
    },
    {
        MODE_SUB_DATA_TEST_COLLISION,
        true,
        init_data_test_collision_md,
        ctrl_data_test_collision_md,
        dest_data_test_collision_md,
    },
    {
        MODE_SUB_DATA_TEST_SPR,
        true,
        init_data_test_spr_md,
        ctrl_data_test_spr_md,
        dest_data_test_spr_md,
    },
    {
        MODE_SUB_DATA_TEST_AET,
        true,
        init_data_test_aet_md,
        ctrl_data_test_aet_md,
        dest_data_test_aet_md,
    },
    {
        MODE_SUB_DATA_TEST_AUTH_3D,
        true,
        init_data_test_auth_3d_md,
        ctrl_data_test_auth_3d_md,
        dest_data_test_auth_3d_md,
    },
    {
        MODE_SUB_DATA_TEST_CHR,
        true,
        init_data_test_chr_md,
        ctrl_data_test_chr_md,
        dest_data_test_chr_md,
    },
    {
        MODE_SUB_DATA_TEST_ITEM,
        true,
        init_data_test_item_md,
        ctrl_data_test_item_md,
        dest_data_test_item_md,
    },
    {
        MODE_SUB_DATA_TEST_PERF,
        true,
        init_data_test_perf_md,
        ctrl_data_test_perf_md,
        dest_data_test_perf_md,
    },
    {
        MODE_SUB_DATA_TEST_PVSCRIPT,
        true,
        init_data_test_pv_script_md,
        ctrl_data_test_pv_script_md,
        dest_data_test_pv_script_md,
    },
    {
        MODE_SUB_DATA_TEST_PRINT,
        true,
        init_data_test_print_md,
        ctrl_data_test_print_md,
        dest_data_test_print_md,
    },
    {
        MODE_SUB_DATA_TEST_CARD,
        true,
        init_data_test_card_md,
        ctrl_data_test_card_md,
        dest_data_test_card_md,
    },
    {
        MODE_SUB_DATA_TEST_OPD,
        true,
        init_data_test_opd_md,
        ctrl_data_test_opd_md,
        dest_data_test_opd_md,
    },
    {
        MODE_SUB_DATA_TEST_SLIDER,
        true,
        init_data_test_slider_md,
        ctrl_data_test_slider_md,
        dest_data_test_slider_md,
    },
    {
        MODE_SUB_DATA_TEST_GLITTER,
        true,
        init_data_test_glitter_md,
        ctrl_data_test_glitter_md,
        dest_data_test_glitter_md,
    },
    {
        MODE_SUB_DATA_TEST_GRAPHICS,
        true,
        init_data_test_graphics_md,
        ctrl_data_test_graphics_md,
        dest_data_test_graphics_md,
    },
    {
        MODE_SUB_DATA_TEST_COLLECTION_CARD,
        true,
        init_data_test_collection_card_md,
        ctrl_data_test_collection_card_md,
        dest_data_test_collection_card_md,
    },
    {
        MODE_SUB_TEST_MODE_MAIN,
        false,
        init_data_test_mode_main_md,
        ctrl_data_test_mode_main_md,
        dest_data_test_mode_main_md,
    },
    {
        MODE_SUB_APP_ERROR,
        false,
        init_app_error_md,
        ctrl_app_error_md,
        dest_app_error_md,
    },

    { // Added
        MODE_SUB_DATA_EDIT_MAIN,
        false,
        init_data_edit_main_md,
        ctrl_data_edit_main_md,
        dest_data_edit_main_md,
    },
    { // Added
        MODE_SUB_DATA_EDIT_GLITTER_EDITOR,
        false,
        init_data_edit_glitter_editor_md,
        ctrl_data_edit_glitter_editor_md,
        dest_data_edit_glitter_editor_md,
    },
#if FACE_ANIM
    { // Added
        MODE_SUB_DATA_EDIT_FACE_ANIM,
        false,
        init_data_edit_face_anim_md,
        ctrl_data_edit_face_anim_md,
        dest_data_edit_face_anim_md,
    },
#endif
};

const char* mode_name[] = {
    "ADVERTISE",
    "STARTUP",
    "GAME",
    "DATA_TEST",
    "TEST_MODE",
    "APP_ERROR",
    "DATA_EDIT",
    "MAX",
};

const char* mode_sub_name[] = {
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

ModeInfo mode_info;

static bool ctrl_sub_mode(ModeInfo* mode_info);
static ModeInfo* get_mode_info();
static const char* get_mode_name(Mode mode);
static ModeTable* get_mode_table(Mode mode);
static ModeSubTable* get_mode_sub_table(ModeSub mode);
static const char* get_mode_sub_name(ModeSub mode);
static void init_mode_info(Mode mode, ModeSub mode_sub);

bool check_pause_enable() {
    ModeInfo* mode_info = get_mode_info();
    ModeSubTable* sub_game_state_data = get_mode_sub_table(mode_info->sub_current);
    if (sub_game_state_data)
        return sub_game_state_data->pause_enable;
    return true;
}

void ctrl_mode() {
    ModeInfo* mode_info = get_mode_info();
    if (!mode_info->shift) {
        if (mode_info->sub_shift && mode_info->sub_init) {
            if (app::check_closing_task()) {
                mode_info->time++;
                return;
            }

            init_mode_info(MODE_MAX, mode_info->sub_next);
            mode_info->init = false;
            mode_info->sub_init = false;
        }
    }
    else {
        if (mode_info->init && mode_info->sub_init) {
            if (app::check_closing_task()) {
                mode_info->time++;
                return;
            }

            init_mode_info(mode_info->next, mode_info->sub_shift
                ? mode_info->sub_next : MODE_SUB_MAX);
            mode_info->init = false;
            mode_info->sub_init = false;
        }
    }

    if (mode_info->shift && mode_info->sub_init)
        mode_info->proc = MODE_PROC_DEST;

    ModeTable* tbl = get_mode_table(mode_info->current);
    if (tbl) {
        bool(*func)() = tbl->func[mode_info->proc];
        bool ret = mode_info->proc != MODE_PROC_CTRL;
        if (func)
            ret = func();

        if (ret) {
            if (mode_info->proc == MODE_PROC_INIT)
                mode_info->proc = MODE_PROC_CTRL;
            else if (mode_info->proc == MODE_PROC_CTRL)
                mode_info->proc = MODE_PROC_DEST;
            else if (mode_info->proc == MODE_PROC_DEST)
                mode_info->init = true;
            else if (!mode_info->shift) {
                shift_next_mode(mode_info->next);
                shift_next_mode_sub(mode_info->sub_next);
            }
        }

        if (mode_info->proc && !mode_info->sub_init) {
            if (mode_info->shift || mode_info->sub_shift)
                mode_info->sub_proc = MODE_SUB_PROC_DEST;

            if (ctrl_sub_mode(mode_info)) {
                if (!mode_info->shift && !mode_info->sub_shift) {
                    ModeSub sub_next = mode_info->sub_next;
                    if (sub_next == MODE_SUB_MAX) {
                        shift_next_mode(mode_info->next);
                        sub_next = mode_info->sub_next;
                    }
                    shift_next_mode_sub(sub_next);
                }
                mode_info->sub_init = true;
            }
        }
    }
    mode_info->time++;
}

Mode get_mode() {
    return get_mode_info()->current;
}

const ModeInfo* get_mode_info_const() {
    return &mode_info;
}

void init_mode(Mode mode, ModeSub mode_sub) {
    init_mode_info(mode, mode_sub);
}

static bool is_mode_advertise() {
    return get_mode_info_const()->current == MODE_ADVERTISE;
}

static bool is_mode_advertise_not_sub_demo() {
    ModeInfo* mode_info = get_mode_info();
    if (mode_info->current == MODE_ADVERTISE)
        return mode_info->sub_current != MODE_SUB_DEMO;
    return false;
}

size_t print_mode(char* buf, size_t buf_size) {
    ModeInfo* mode_info = get_mode_info();
    const char* mode_name = get_mode_name(mode_info->current);
    const char* mode_sub_name = get_mode_sub_name(mode_info->sub_current);
    return sprintf_s(buf, buf_size, "%s/%s", mode_name, mode_sub_name);
}

void set_etc_work(uint32_t work) {
    get_mode_info()->etc_work = work;
}

void shift_next_mode(Mode state) {
    ModeInfo* mode_info = get_mode_info();
    if (mode_info->next != MODE_APP_ERROR || !mode_info->shift) {
        mode_info->next = state;
        mode_info->shift = true;
        mode_info->sub_shift = false;
    }
}

void shift_next_mode_sub(ModeSub state) {
    ModeInfo* mode_info = get_mode_info();
    if (mode_info->next != MODE_APP_ERROR || !mode_info->shift) {
        mode_info->sub_next = state;
        mode_info->sub_shift = true;
    }
}

static bool ctrl_sub_mode(ModeInfo* mode_info) {
    ModeSubTable* sub_game_state_data = get_mode_sub_table(mode_info->sub_current);
    if (!sub_game_state_data) {
        mode_info->sub_call_count++;
        return false;
    }

    bool(*func)() = sub_game_state_data->func[mode_info->sub_proc];
    if (!func || func()) {
        if (mode_info->sub_proc == MODE_SUB_PROC_INIT)
            mode_info->sub_proc = MODE_SUB_PROC_CTRL;
        else if (mode_info->sub_proc == MODE_SUB_PROC_CTRL)
            mode_info->sub_proc = MODE_SUB_PROC_DEST;
        else if (mode_info->sub_proc == MODE_SUB_PROC_DEST) {
            mode_info->sub_call_count++;
            return true;
        }
    }

    mode_info->sub_call_count++;
    return false;
}

static ModeInfo* get_mode_info() {
    return &mode_info;
}

static const char* get_mode_name(Mode mode) {
    return mode_name[mode];
}

static ModeTable* get_mode_table(Mode name) {
    for (ModeTable& i : mode_table)
        if (i.name == name)
            return &i;
    return 0;
}

static const char* get_mode_sub_name(ModeSub mode) {
    return mode_sub_name[mode];
}

static ModeSubTable* get_mode_sub_table(ModeSub name) {
    for (ModeSubTable& i : mode_sub_table)
        if (i.name == name)
            return &i;
    return 0;
}

static void init_mode_info(Mode mode, ModeSub mode_sub) {
    ModeInfo* mode_info = get_mode_info();
    if (mode != MODE_MAX) {
        Mode old = mode_info->current;
        mode_info->current = mode;
        mode_info->next = MODE_MAX;
        mode_info->old = old;
        mode_info->proc = MODE_PROC_INIT;
        mode_info->shift = false;
        mode_info->time = 0;
        mode_info->etc_work = 0;

        dw_console_printf(DW_CONSOLE_MODE_CTRL, "[%s]->[%s]\n",
            get_mode_name(old), get_mode_name(mode));

        ModeTable* tbl = get_mode_table(mode_info->current);
        if (tbl) {
            mode_info->next = tbl->next;
            if (mode_sub == MODE_SUB_MAX)
                mode_info->sub_next = tbl->sub_next;
        }
    }

    ModeSub sub_old = mode_info->sub_current;
    mode_info->sub_current = MODE_SUB_MAX;
    mode_info->sub_next = MODE_SUB_MAX;
    mode_info->sub_old = sub_old;
    mode_info->sub_proc = MODE_SUB_PROC_INIT;
    mode_info->sub_shift = false;
    mode_info->sub_call_count = 0;

    ModeTable* tbl = get_mode_table(mode_info->current);
    if (tbl) {
        if (mode_sub == MODE_SUB_MAX) {
            mode_info->sub_current = tbl->sub_list[0];
            mode_info->sub_next = tbl->sub_list[1];
        }
        else {
            size_t i = 0;
            ModeSub* sub_list = tbl->sub_list;
            while (*sub_list != MODE_SUB_MAX && i < 24) {
                if (*sub_list == mode_sub) {
                    mode_info->sub_current = mode_sub;
                    mode_info->sub_next = tbl->sub_list[i + 1];
                    break;
                }

                i++;
                sub_list++;
            }
        }
    }
}
