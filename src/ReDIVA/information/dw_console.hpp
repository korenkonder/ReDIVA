/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"

enum dw_console_index {
    DW_CONSOLE_SYSTEM,
    DW_CONSOLE_MATCHING,
    DW_CONSOLE_PV_SCRIPT,
    DW_CONSOLE_GAME,
    DW_CONSOLE_GAME_SUB_COMMAND,
    DW_CONSOLE_5,
    DW_CONSOLE_6,
    DW_CONSOLE_7,
    DW_CONSOLE_8,
    DW_CONSOLE_9,
    DW_CONSOLE_MODE_CTRL,
    DW_CONSOLE_MODE_MEMORY,
    DW_CONSOLE_EVENT_MEMORY,
    DW_CONSOLE_TEMP_MEMORY,
    DW_CONSOLE_AUTH_BILLING,
    DW_CONSOLE_BOOKKEEP,
    DW_CONSOLE_AIME,
    DW_CONSOLE_17,
    DW_CONSOLE_18,
    DW_CONSOLE_19,
    DW_CONSOLE_MAX,
};

extern void dw_console_init();
extern void dw_console_printf(dw_console_index index, const char* fmt, ...);
extern void dw_console_vprintf(dw_console_index index, const char* fmt, va_list args);

extern void dw_console_c_buff_array_init();
extern void dw_console_c_buff_array_free();
