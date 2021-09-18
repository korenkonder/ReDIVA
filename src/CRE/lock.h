/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef struct lock {
    CRITICAL_SECTION cs;
    BOOL init;
} lock;

#define lock_lock(val) \
if ((val)->init) { \
EnterCriticalSection(&(val)->cs)

#define lock_unlock(val) \
LeaveCriticalSection(&(val)->cs); \
}

extern void lock_init(lock* l);
extern bool lock_check_init(lock* l);
extern void lock_free(lock* l);
