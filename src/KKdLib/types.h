/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <stdint.h>

#define FORCE_INLINE __forceinline
#define FASTCALL __fastcall

#ifdef bool
#undef bool
#endif

typedef char bool;
#define true (1)
#define false (0)

#ifdef ssize_t
#undef ssize_t;
#endif
typedef int64_t ssize_t;

#ifdef float_t
#undef float_t;
#endif
typedef float float_t;

#ifdef double_t
#undef double_t;
#endif
typedef double double_t;
