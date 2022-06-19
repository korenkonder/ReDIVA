/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "lock.hpp"

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
