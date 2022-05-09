/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "random.hpp"

static RandState* rand_state_array;

RandState::RandState() {

}

RandState::~RandState() {

}

inline void rand_state_array_init() {
    rand_state_array = new RandState[5];
}

inline RandState* rand_state_array_get(int32_t id) {
    if (id > -1 && id < 5)
        return &rand_state_array[id];
    return 0;
}

inline float_t rand_state_array_get_float(int32_t id) {
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state) {
        std::uniform_int_distribution<uint32_t> dis;
        return (float_t)(dis(rand_state->mt) >> 8) * (float_t)(1.0 / (double_t)0x1000000);
    }
    return 0.0f;
}

inline uint32_t rand_state_array_get_int(int32_t id) {
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state) {
        std::uniform_int_distribution<uint32_t> dis;
        return dis(rand_state->mt);
    }
    return 0;
}

inline uint32_t rand_state_array_get_int(uint32_t min, uint32_t max, int32_t id) {
    uint64_t _min = min;
    uint64_t _max = max;
    RandState* rand_state = rand_state_array_get(id);
    if (rand_state && min <= max) {
        std::uniform_int_distribution<uint32_t> dis;
        return (uint32_t)(_min + (((_max - _min + 1) * (uint64_t)dis(rand_state->mt)) >> 32));
    }
    return 0;
}

inline void rand_state_array_free() {
    delete[] rand_state_array;
}
