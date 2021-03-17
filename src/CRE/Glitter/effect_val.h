/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern float_t FASTCALL Glitter__EffectVal__GetFloatClamp(float_t value);
extern int32_t FASTCALL Glitter__EffectVal__GetMax();
extern int32_t FASTCALL Glitter__EffectVal__GetInt(int32_t value);
extern int32_t FASTCALL Glitter__EffectVal__Get();
extern float_t FASTCALL Glitter__EffectVal__GetFloat(float_t min, float_t max);
extern int32_t FASTCALL Glitter__EffectVal__Clamp(int32_t min, int32_t max);
extern void FASTCALL Glitter__EffectVal__GetFloatVec3Clamp(vec3* src, vec3* dst);
extern void FASTCALL Glitter__EffectVal__Set(int32_t value);
