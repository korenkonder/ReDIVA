/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "random.h"

const int32_t MSEED = 0x09A4EC86;

random* random_init(int32_t seed) {
    random* r = force_malloc(sizeof(random));

    int32_t ii;
    int32_t mj, mk;

    int32_t subtraction = (seed == INT32_MIN) ? INT32_MAX : (int32_t)abs(seed);
    mj = MSEED - subtraction;
    r->seed_array[55] = mj;
    mk = 1;
    for (int32_t i = 1; i < 55; i++) {
        ii = (21 * i) % 55;
        r->seed_array[ii] = mk;
        mk = mj - mk;
        if (mk < 0)
            mk += INT32_MAX;
        mj = r->seed_array[ii];
    }

    for (int32_t k = 1; k < 5; k++)
        for (int32_t i = 1; i < 56; i++) {
            r->seed_array[i] -= r->seed_array[1 + (i + 30) % 55];
            if (r->seed_array[i] < 0)
                r->seed_array[i] += INT32_MAX;
        }

    r->inext = 0;
    r->inextp = 21;
    seed = 1;
    return r;
}

int32_t random_next(random* r) {
    int32_t ret_val;
    int32_t temp_inext = r->inext;
    int32_t temp_inextp = r->inextp;

    if (++temp_inext >= 56)
        temp_inext = 1;
    if (++temp_inextp >= 56)
        temp_inextp = 1;

    ret_val = r->seed_array[temp_inext] - r->seed_array[temp_inextp];

    if (ret_val == INT32_MAX)
        ret_val--;
    if (ret_val < 0)
        ret_val += INT32_MAX;

    r->seed_array[temp_inext] = ret_val;

    r->inext = temp_inext;
    r->inextp = temp_inextp;

    return ret_val;
}

double_t random_next_double(random* r) {
    return (double_t)random_next(r) / (double_t)INT32_MAX;
}

void random_dispose(random* r) {
    free(r);
}
