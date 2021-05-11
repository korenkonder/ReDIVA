/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

#define lock_val(name) \
CRITICAL_SECTION name; \
BOOL name##_init

#define lock_extern_val(name) \
extern CRITICAL_SECTION name; \
extern BOOL name##_init

#define lock_init(val) \
(val##_init) = InitializeCriticalSectionAndSpinCount(&(val), 0);

#define lock_check_init(val) \
(val##_init)

#define lock_lock(val) \
if (val##_init) { \
EnterCriticalSection(&(val));

#define lock_unlock(val) \
LeaveCriticalSection(&(val)); \
}

#define lock_dispose(val) \
if (val##_init) \
    DeleteCriticalSection(&(val)); \
(val##_init) = FALSE
