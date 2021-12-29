/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef struct lock {
    CRITICAL_SECTION cs;
    bool init;
} lock;

typedef struct lock_data {
    lock* lock;
    void* lock_data;
    void(*lock_free)(void* data);
} lock_data;

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

extern bool lock_data_init(lock_data* ld, lock* l,
    void* data, void(*free_func)(void* data));
extern bool lock_data_free(lock_data* ld, void(*free_func)(void* data));
