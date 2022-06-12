/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct lock {
    CRITICAL_SECTION cs;
    bool init;
};

struct lock_data {
    lock* lock;
    void* lock_data;
    void(*lock_free)(void* data);
};

#define lock_lock(val) \
if ((val) && (val)->init) { \
EnterCriticalSection(&(val)->cs)

#define lock_trylock(val) \
if ((val) && (val)->init && TryEnterCriticalSection(&(val)->cs)) {

#define lock_unlock(val) \
LeaveCriticalSection(&(val)->cs); \
}

extern void lock_init(lock* l);
extern bool lock_check_init(lock* l);
extern void lock_free(lock* l);
