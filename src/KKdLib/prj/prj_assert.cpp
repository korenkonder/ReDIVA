/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "prj_assert.hpp"

static int32_t is_debugger_present;
static int32_t is_debugger_present_init;

int32_t(*prj_vtracef_hook)(const char* fmt, va_list args);

// Missing
int32_t prj_IsDebuggerPresent() {
    if (is_debugger_present_init)
        return is_debugger_present;

    is_debugger_present = 1;
    is_debugger_present_init = 1;
    return 1;
}

// Missing
void prj_assert_post_dummy() {

}

// 0x1400DE640
int32_t prj_tracef(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int32_t ret = prj_vtracef(fmt, args);
    va_end(args);
    return ret;
}

// 0x1400DE660
void prj_trap(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    prj_tracef("prj_trap:");
    prj_vtracef(fmt, args);
    prj_tracef("\n");
    va_end(args);

    prj_assert(0);
}

// Missing
int32_t prj_vtracef(const char* fmt, va_list args) {
    int32_t ret = vprintf(fmt, args);
    if (prj_vtracef_hook)
        prj_vtracef_hook(fmt, args);
    return ret;
}
