/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

extern int32_t prj_IsDebuggerPresent();

extern void prj_assert_post_dummy();
extern int32_t prj_tracef(const char* fmt, ...);
extern void prj_trap(const char* fmt, ...);
extern int32_t prj_vtracef(const char* fmt, va_list args);

#define prj_assert(expression) \
if (!(expression)) { \
    prj_tracef("** ASSERT:%s:%d: `%s' failed.\n", __FILE__, __LINE__, #expression); \
    if (prj_IsDebuggerPresent()) \
        __debugbreak(); \
    prj_assert_post_dummy(); \
} 
