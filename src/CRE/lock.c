/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "lock.h"

inline void lock_init(lock* l) {
    l->init = InitializeCriticalSectionAndSpinCount(&l->cs, 0);
}

inline bool lock_check_init(lock* l) {
    return l->init;
}

inline void lock_free(lock* l) {
    if (l->init)
        DeleteCriticalSection(&l->cs);
    l->init = FALSE;
}

inline bool lock_data_init(lock_data* ld, lock* l,
    void* data, void(*free_func)(void* data)) {
    if (ld->lock_free && ld->lock_free != free_func) {
        lock* lock = ld->lock;
        lock_lock(lock);
        ld->lock_free(ld->lock_data);
        lock_unlock(lock);

        if (ld->lock_free)
            return false;
    }
    else if (ld->lock_free)
        ld->lock_free(ld->lock_data);

    ld->lock = l;
    ld->lock_data = data;
    ld->lock_free = free_func;
    return true;
}

inline bool lock_data_free(lock_data* ld, void(*free_func)(void* data)) {
    if (ld->lock_free != free_func)
        return false;

    ld->lock = 0;
    ld->lock_data = 0;
    ld->lock_free = 0;
    return true;
}