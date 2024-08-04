/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct lock_cs {
    CRITICAL_SECTION cs;
    bool init;

    inline lock_cs() : cs() {
        init = InitializeCriticalSectionAndSpinCount(&cs, 0);
    }

    inline ~lock_cs() {
        if (init)
            DeleteCriticalSection(&cs);
        cs = {};
        init = false;
    }

    inline bool check_init() {
        if (!this)
            return false;
        return init;
    }
};

#define lock_lock(val) \
if ((val) && (val)->init) { \
EnterCriticalSection(&(val)->cs)

#define lock_trylock(val) \
if ((val) && (val)->init && TryEnterCriticalSection(&(val)->cs)) {

#define lock_unlock(val) \
LeaveCriticalSection(&(val)->cs); \
}
