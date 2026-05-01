/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "random.hpp"

#define n 624
#define m 397
#define w 32
#define r 31
#define LMASK (0xffffffffUL >> ((w) - (r)))
#define a 0x9908b0dfUL
#define u 11
#define s 7
#define t 15
#define l 18
#define b 0x9d2c5680UL
#define c 0xefc60000UL
#define f 1812433253UL

#define SHIFT_VAL(a, b, c) ((0xffffffffUL & ~((1U << ((c) - (b))) - 1U)) | ((a) >> (b)))

class MtRandom {
public:
    uint32_t mt[n];
    int32_t index;

    MtRandom();
    virtual ~MtRandom();

    void Init(uint32_t seed);
    void InitByArray(const uint32_t* key, int32_t key_length);
    uint32_t Rand();
};

struct RandState {
    MtRandom mtrand;
    uint32_t seed;
    bool seed_init;

    RandState();
    virtual ~RandState();
};

static RandState* rand_state;

static RandState* get_random_state(RandomType type);
static uint32_t sub_1405000B0();

// 0x1404FFB20
void DestRandom() {
    if (rand_state) {
        delete[] rand_state;
        rand_state = 0;
    }
}

// 0x1404FFB70
uint32_t GetRandomSeed(RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return rand_state->seed;
    return 0;
}

// 0x1404FFB90
void InitRandom() {
    if (!rand_state) {
        rand_state = new RandState[RANDOM_TYPE_MAX];

        InitRandomStateForBoot(RANDOM_TYPE_0);
        for (int32_t i = RANDOM_TYPE_1; i < RANDOM_TYPE_MAX; i++)
            InitRandomState(Random(RANDOM_TYPE_0), (RandomType)i);
    }
}

// 0x1404FFC30
void InitRandomState(uint32_t seed, RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state) {
        rand_state->seed = seed;
        rand_state->seed_init = true;
        rand_state->mtrand.Init(seed);
    }
}

// 0x1404FFC70
void InitRandomStateForBoot(RandomType type) {
    uint32_t key[3];
    key[0] = sub_1405000B0();
    key[1] = sub_1405000B0();
    key[2] = sub_1405000B0();

    RandState* rand_state = get_random_state(type);
    if (rand_state) {
        rand_state->seed = 0;
        rand_state->seed_init = 0;
        rand_state->mtrand.InitByArray(key, 3);
    }
}

// 0x1404FFCF0
bool Probability(uint64_t rate, RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return rate > rand_state->mtrand.Rand();
    return false;
}

// 0x1404FFD30
bool ProbabilityF(float_t rate, RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return rate > (float_t)(rand_state->mtrand.Rand() >> 8) * (float_t)(1.0 / (double_t)0x1000000);
    return false;
}

// 0x1404FFD80
uint32_t Random(uint32_t in_min, uint32_t in_max, RandomType type) {
    uint64_t _min = in_min;
    uint64_t _max = in_max;
    RandState* rand_state = get_random_state(type);
    if (rand_state && _min <= _max)
        return (uint32_t)(_min + (((_max - _min + 1) * (uint64_t)rand_state->mtrand.Rand()) >> 32));
    return 0;
}

// 0x1404FFDE0
uint32_t Random(RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return rand_state->mtrand.Rand();
    return 0;
}

// 0x1404FFE00
double_t RandomD(RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return (double_t)(int64_t)rand_state->mtrand.Rand() / (double_t)0; // WHAT?!
    return 0.0;
}

// 0x1404FFE60
float_t RandomF(RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state)
        return (float_t)(rand_state->mtrand.Rand() >> 8) * (float_t)(1.0 / (double_t)0x1000000);
    return 0.0f;
}

// 0x1404FFEA0
bool SafeProbability(uint64_t rate, RandomType type) {
    RandState* rand_state = get_random_state(type);
    if (rand_state && rate)
        return (rand_state->mtrand.Rand() + 1ULL) % rate == 0;
    return false;
}

// 0x1404FFEF0
uint32_t Share(const float_t* arr, int32_t num, RandomType type) {
    uint32_t v5 = 0;
    float_t v6 = RandomF(type);
    float_t v7 = 0.0f;
    if (arr && num) {
        for (uint32_t i = num - 1; i; i--) {
            v7 += *arr;
            if (v6 < v7)
                break;

            v5++;
            arr++;
        }
    }
    return v5;
}

// 0x1404FFFF0
uint32_t Share(const uint64_t* arr, uint32_t num, RandomType type) {
    uint32_t v5 = 0;
    uint32_t v6 = Random(type);
    uint64_t v7 = 0;
    if (arr && num) {
        for (uint32_t i = num - 1; i; i--) {
            v7 += *arr;
            if (v6 < v7)
                break;

            v5++;
            arr++;
        }
    }
    return v5;
}

MtRandom::MtRandom() {
    Init(0x12BD6AA);
}

MtRandom::~MtRandom() {

}

void MtRandom::Init(uint32_t seed) {
    mt[0] = seed;
    for (int32_t i = 1; i < n; i++)
        mt[i] = f * (mt[i - 1] ^ (mt[i - 1] >> (w - 2))) + i;
    index = 0;
}

void MtRandom::InitByArray(const uint32_t* key, int32_t key_length) {
    Init(0x12BD6AA);

    int32_t i = 1;
    int32_t j = 0;
    int32_t k = max_def(n, key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 0x19660D)) + key[j] + j;
        i++;
        j++;

        if (i >= n) {
            mt[0] = mt[n - 1];
            i = 1;
        }

        if (j >= key_length)
            j = 0;
    }

    for (k = n - 1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 0x5D588B65)) - i;
        i++;

        if (i >= n) {
            mt[0] = mt[n - 1];
            i = 1;
        }
    }

    mt[0] = 0x80000000;
}

uint32_t MtRandom::Rand() {
    int32_t index = this->index;
    int32_t shift_index = index + m;
    int32_t next_index = index + 1;
    if (shift_index >= n) {
        shift_index -= n;
        if (next_index >= n)
            next_index = 0;
    }

    uint32_t& state = mt[index];
    uint32_t mt_value = state ^ (state ^ mt[next_index]) & LMASK;
    mt_value = mt[shift_index] ^ ((uint32_t)-((int32_t)mt_value & 0x01) & a) ^ (mt_value >> 1);
    state = mt_value;
    this->index = next_index;

    mt_value ^= mt_value >> u;
    mt_value ^= (mt_value & SHIFT_VAL(b, s, w)) << s;
    mt_value ^= (mt_value & SHIFT_VAL(c, t, w)) << t;
    mt_value ^= mt_value >> l;
    return mt_value;
}

RandState::RandState() : seed(), seed_init() {

}

RandState::~RandState() {

}

// 0x140500090
static RandState* get_random_state(RandomType type) {
    if (type >= 0 && type < RANDOM_TYPE_MAX)
        return &rand_state[type];
    return 0;
}

// 0x1405000B0
static uint32_t sub_1405000B0() {
    LARGE_INTEGER large_int;
    QueryPerformanceCounter(&large_int);
    return large_int.LowPart ^ 0xA306103A;
}
