/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct lock {
    CRITICAL_SECTION cs;
    bool init;

    inline lock() : cs() {
        init = InitializeCriticalSectionAndSpinCount(&cs, 0);
    }

    inline ~lock() {
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

struct lock_uint32_t {
private:
    volatile uint32_t value;

public:
    inline lock_uint32_t() {
        _InterlockedExchange(&value, 0);
    }

    inline uint32_t get() {
        _m_prefetchw(&value);
        int32_t v3 = value;
        while (true) {
            int32_t v4 = v3;
            v3 = _InterlockedCompareExchange(&value, v3, v3);
            if (v4 == v3)
                break;
        }
        return v3;
    }

    inline void set(uint32_t value) {
        _InterlockedExchange(&this->value, value);
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
