/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

template <typename T>
struct lock {
private:
    volatile uint32_t value;

public:
    inline lock() {
        _InterlockedExchange(&value, 0);
    }

    inline T get() {
        _m_prefetchw(&value);
        int32_t v3 = value;
        while (true) {
            int32_t v4 = v3;
            v3 = _InterlockedCompareExchange(&value, v3, v3);
            if (v4 == v3)
                break;
        }
        return (T)v3;
    }

    inline void set(T value) {
        _InterlockedExchange(&this->value, (uint32_t)value);
    }
};

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
