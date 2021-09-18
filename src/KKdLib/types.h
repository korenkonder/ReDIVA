/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <float.h>
#include <stdbool.h>
#include <stdint.h>

#define FASTCALL __fastcall
#define ALIGN(n) __declspec(align(n))

#ifdef ssize_t
#undef ssize_t
#endif
typedef __int64 ssize_t;

#ifdef float_t
#undef float_t
#endif
typedef float float_t;

#ifdef double_t
#undef double_t
#endif
typedef double double_t;
