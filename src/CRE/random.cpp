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
    uint32_t state_array[n];
    uint32_t state_index;
    int32_t field_9CC;
    int32_t field_9D0;
    int8_t field_9D4;

    MtRandom();
    virtual ~MtRandom();

    uint32_t rand();
    void seed(uint32_t seed);
};

struct RandState {
    MtRandom mt_rand;

    RandState();
    virtual ~RandState();
};

static RandState* rand_state_array;

static RandState* rand_state_array_get(int32_t id);

void rand_state_array_init() {
    rand_state_array = new RandState[5];
}

float_t rand_state_array_get_float(int32_t id) {
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state)
        return (float_t)(rand_state->mt_rand.rand() >> 8) * (float_t)(1.0 / (double_t)0x1000000);
    return 0.0f;
}

uint32_t rand_state_array_get_int(int32_t id) {
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state)
        return rand_state->mt_rand.rand();
    return 0;
}

uint32_t rand_state_array_get_int(uint32_t min, uint32_t max, int32_t id) {
    uint64_t _min = min;
    uint64_t _max = max;
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state && min <= max)
        return (uint32_t)(_min + (((_max - _min + 1) * (uint64_t)rand_state->mt_rand.rand()) >> 32));
    return 0;
}

void rand_state_array_set_seed(uint32_t val, int32_t id) {
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state)
        rand_state->mt_rand.seed(val);
}

void rand_state_array_4_set_seed_1393939() {
    rand_state_array_set_seed(1393939, 4);
}

void rand_state_array_free() {
    delete[] rand_state_array;
}

MtRandom::MtRandom() : field_9CC(), field_9D0(), field_9D4() {
    seed(0x12BD6AA);
}

MtRandom::~MtRandom() {

}

uint32_t MtRandom::rand() {
    int32_t index = state_index;
    int32_t shift_index = index + m;
    int32_t next_index = index + 1;
    if (shift_index >= n) {
        shift_index -= n;
        if (next_index >= n)
            next_index = 0;
    }

    uint32_t& state = state_array[index];
    uint32_t mt_value = state ^ (state ^ state_array[next_index]) & LMASK;
    mt_value = state_array[shift_index] ^ ((uint32_t)-((int32_t)mt_value & 0x01) & a) ^ (mt_value >> 1);
    state = mt_value;
    state_index = next_index;

    mt_value ^= mt_value >> u;
    mt_value ^= (mt_value & SHIFT_VAL(b, s, w)) << s;
    mt_value ^= (mt_value & SHIFT_VAL(c, t, w)) << t;
    mt_value ^= mt_value >> l;
    return mt_value;
}

void MtRandom::seed(uint32_t val) {
    state_array[0] = val;
    for (int32_t i = 1; i < n; i++)
        state_array[i] = f * (state_array[i - 1] ^ (state_array[i - 1] >> (w - 2))) + i;
    state_index = 0;
}

RandState::RandState() {

}

RandState::~RandState() {

}

static RandState* rand_state_array_get(int32_t id) {
    if (id > -1 && id < 5)
        return &rand_state_array[id];
    return 0;
}
