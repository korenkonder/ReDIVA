/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

#define FLOAT16_NAN           ((half_t)0x7FFF)
#define FLOAT16_POSITIVE_NAN  ((half_t)0x7FFF)
#define FLOAT16_NEGATIVE_NAN  ((half_t)0xFFFF)
#define FLOAT16_POSITIVE_ZERO ((half_t)0x0000)
#define FLOAT16_NEGATIVE_ZERO ((half_t)0x8000)
#define FLOAT16_POSITIVE_INF  ((half_t)0x7C00)
#define FLOAT16_NEGATIVE_INF  ((half_t)0xFC00)

#define HALF_MAX 65504
#define HALF_MIN 0.00006103515625

typedef unsigned short half_t;

extern half_t reverse_endianess_half_t(half_t value);
extern float_t half_to_float(half_t h);
extern half_t float_to_half(float_t val);
extern double_t half_to_double(half_t h);
extern half_t double_to_half(double_t val);
