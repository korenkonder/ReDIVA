/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

enum RandomType {
    RANDOM_TYPE_0 = 0,
    RANDOM_TYPE_1,
    RANDOM_TYPE_2,
    RANDOM_TYPE_3,
    RANDOM_TYPE_GLOBAL,
    RANDOM_TYPE_MAX,
};

extern void DestRandom();
extern uint32_t GetRandomSeed(RandomType type);
extern void InitRandom();
extern void InitRandomState(uint32_t seed, RandomType type);
extern void InitRandomStateForBoot(RandomType type);
extern bool Probability(uint64_t rate, RandomType type);
extern uint32_t Random(uint32_t min, uint32_t max, RandomType type);
extern uint32_t Random(RandomType type);
extern double_t RandomD(RandomType type);
extern float_t RandomF(RandomType type);
extern bool SafeProbability(uint64_t rate, RandomType type);
extern uint32_t Share(const float_t* arr, int32_t num, RandomType type);
extern uint32_t Share(const uint64_t* arr, uint32_t num, RandomType type);
