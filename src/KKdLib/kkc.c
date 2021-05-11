/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "kkc.h"
#include <time.h>

#define XOR32(val) ((val & 0xFF) ^ (((val) >> 8) & 0xFF) ^ (((val) >> 16) & 0xFF) ^ (((val) >> 24) & 0xFF))

static inline void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state);

kkc* kkc_init() {
    kkc* k = force_malloc(sizeof(kkc));
    return k;
}

void FASTCALL kkc_get_key(kkc_key* key, uint32_t seed, kkc_key_mode mode, kkc_key_type type) {
    key->data = 0;
    key->mode = 0;
    key->type = KKC_KEY_TYPE_NONE;

    switch (mode) {
    case KKC_KEY_MODE_SIMPLE:
    case KKC_KEY_MODE_PAST:
        break;
    default:
        return;
    }

    switch (type) {
    case KKC_KEY_TYPE_128:
    case KKC_KEY_TYPE_192:
    case KKC_KEY_TYPE_256:
    case KKC_KEY_TYPE_384:
    case KKC_KEY_TYPE_512:
    case KKC_KEY_TYPE_768:
    case KKC_KEY_TYPE_1024:
    case KKC_KEY_TYPE_1536:
    case KKC_KEY_TYPE_2048:
        break;
    default:
        return;
    }

    size_t l = (size_t)type * 0x8;
    uint8_t* k = force_malloc(l);

    if (!k)
        return;

    uint32_t state = 1;
    if (seed)
        state = seed;

    next_rand_uint8_t_pointer(k, l, &state);

    key->data = k;
    key->mode = mode;
    key->type = type;
}

void FASTCALL kkc_init_from_key_mode(kkc* c, vector_uint8_t* key, kkc_key_mode mode) {
    c->state = 1;
    c->error = KKC_ERROR_NONE;

    if (!key || !key->begin || key->end - key->begin < 1) {
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    kkc_key_type type = 0;
    size_t kl = key->end - key->begin;

    switch (kl) {
    case 16:
        type = KKC_KEY_TYPE_128;
        break;
    case 24:
        type = KKC_KEY_TYPE_192;
        break;
    case 32:
        type = KKC_KEY_TYPE_256;
        break;
    case 48:
        type = KKC_KEY_TYPE_384;
        break;
    case 64:
        type = KKC_KEY_TYPE_512;
        break;
    case 96:
        type = KKC_KEY_TYPE_768;
        break;
    case 128:
        type = KKC_KEY_TYPE_1024;
        break;
    case 196:
        type = KKC_KEY_TYPE_1536;
        break;
    case 256:
        type = KKC_KEY_TYPE_2048;
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY_LENGTH;
        return;
    }

    c->key.data = (uint8_t*)key->begin;
    c->key.mode = mode;
    c->key.type = type;
}

void FASTCALL kkc_init_from_type_mode(kkc* c, kkc_key_type type, kkc_key_mode mode) {
    c->state = 1;
    c->error = KKC_ERROR_NONE;

    switch (type) {
    case KKC_KEY_TYPE_128:
    case KKC_KEY_TYPE_192:
    case KKC_KEY_TYPE_256:
    case KKC_KEY_TYPE_384:
    case KKC_KEY_TYPE_512:
    case KKC_KEY_TYPE_768:
    case KKC_KEY_TYPE_1024:
    case KKC_KEY_TYPE_1536:
    case KKC_KEY_TYPE_2048:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY_LENGTH;
        return;
    }

    switch (mode) {
    case KKC_KEY_MODE_SIMPLE:
    case KKC_KEY_MODE_PAST:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    struct tm t;
    time_t time = 0;
    localtime_s(&t, &time);
    kkc_get_key(&c->key, (uint32_t)time, mode, type);
}

void FASTCALL kkc_init_from_seed_type_mode(kkc* c, uint32_t seed, kkc_key_type type, kkc_key_mode mode) {
    c->state = 1;
    c->error = KKC_ERROR_NONE;

    switch (type) {
    case KKC_KEY_TYPE_128:
    case KKC_KEY_TYPE_192:
    case KKC_KEY_TYPE_256:
    case KKC_KEY_TYPE_384:
    case KKC_KEY_TYPE_512:
    case KKC_KEY_TYPE_768:
    case KKC_KEY_TYPE_1024:
    case KKC_KEY_TYPE_1536:
    case KKC_KEY_TYPE_2048:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY_LENGTH;
        return;
    }

    switch (mode) {
    case KKC_KEY_MODE_SIMPLE:
    case KKC_KEY_MODE_PAST:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    kkc_get_key(&c->key, seed, mode, type);
}

void FASTCALL kkc_init_from_key(kkc* c, kkc_key* key) {
    c->state = 1;
    c->error = KKC_ERROR_NONE;

    if (!key->data) {
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    switch (key->type) {
    case KKC_KEY_TYPE_128:
    case KKC_KEY_TYPE_192:
    case KKC_KEY_TYPE_256:
    case KKC_KEY_TYPE_384:
    case KKC_KEY_TYPE_512:
    case KKC_KEY_TYPE_768:
    case KKC_KEY_TYPE_1024:
    case KKC_KEY_TYPE_1536:
    case KKC_KEY_TYPE_2048:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY_LENGTH;
        return;
    }

    switch (key->mode) {
    case KKC_KEY_MODE_SIMPLE:
    case KKC_KEY_MODE_PAST:
        break;
    default:
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    c->key = *key;
}

void FASTCALL kkc_prepare_cursing_table(kkc* c) {
    if (!c)
        return;

    if (!c->key.data) {
        c->error = KKC_ERROR_INVALID_KEY;
        return;
    }

    size_t kl = (size_t)c->key.type * 8;
    uint8_t* arr = force_malloc(kl + 3);

    if (!arr)
        return;
    memcpy(arr, c->key.data, kl);

    if (c->k)
        free(c->k);

    c->k = force_malloc_s(uint32_t, kl);
    if (!c->k)
        return;

    size_t n = kl / 2;
    uint32_t* k = c->k;
    size_t i;
    uint32_t a, b, q, seed;
    while (true) {
        a = q = 0;
        for (i = 0; i < n; i++) {
            b = *(uint32_t*)(arr + i * 2);
            k[i] = b;
            a ^= b;
            q |= b;
        }

        b = *(uint32_t*)arr << 16;
        k[kl / 2 - 1] |= b;
        a ^= b;
        q |= b;

        if (q != 0) {
            arr++;
            k += n;
            for (i = 0; i < n; i++) {
                b = *(uint32_t*)(arr + i * 2);
                k[i] = b;
                a ^= b;
                q |= b;
            }
            k -= n;
            arr--;

            k[kl - 2] |= b = *(uint32_t*)arr << 24;
            a ^= b;
            q |= b;

            k[kl - 1] |= b = *(uint32_t*)arr << 8;
            a ^= b;
            q |= b;

            if (a != 0) break;
        }

        seed = a ^ q;
        next_rand_uint8_t_pointer(arr, kl, &c->state);
        seed = *(uint32_t*)arr;
        next_rand_uint8_t_pointer(arr, kl, &c->state);
    }
    free(arr);

    c->a0 = a;
    uint32_t t0 = ~c->a0 + ((c->a0 >> 1) | (c->a0 << 31));
    uint32_t t1 = ~c->a0 - ((c->a0 >> 5) | (c->a0 << 27));
    c->e0 = XOR32(k[XOR32(t0) % kl]) % kl;
    c->f0 = XOR32(k[XOR32(t1) % kl]) % kl;

    kkc_reset(c);
}

void FASTCALL kkc_reset(kkc* c) {
    if (!c)
        return;

    c->o0 = c->o1 = c->o2 = c->o3 = c->a0;
    c->e = c->e0;
    c->f = c->f0;
}

void FASTCALL kkc_reset_iv(kkc* c, uint32_t iv) {
    if (!c)
        return;

    c->o0 = c->o1 = c->o2 = c->o3 = c->a0 ^ iv;
    c->e = c->e0;
    c->f = c->f0;
}

void FASTCALL kkc_reset_iv4(kkc* c, uint32_t iv0, uint32_t iv1, uint32_t iv2, uint32_t iv3) {
    if (!c)
        return;

    size_t kl = (size_t)c->key.type * 8;
    c->o0 = c->a0 ^ iv0;
    c->o1 = c->a0 ^ iv1;
    c->o2 = c->a0 ^ iv2;
    c->o3 = c->a0 ^ iv3;
    c->e = (c->e0 ^ XOR32(iv0 ^ iv2)) % kl;
    c->f = (c->f0 ^ XOR32(iv1 ^ iv3)) % kl;
}

void FASTCALL kkc_curse(kkc* c, uint8_t* src, uint8_t* dst, size_t length) {
    if (!c)
        return;

    bool ret = true;
    if (c->error)
        return;
    else if (!c->k)
        c->error = KKC_ERROR_UNINITIALIZED_TABLE;
    else if (!c->key.data)
        c->error = KKC_ERROR_INVALID_KEY;
    else if (!src || !dst)
        c->error = KKC_ERROR_INVALID_DATA;
    else if (length & 0xF)
        c->error = KKC_ERROR_INVALID_DATA_LENGTH;
    else
        ret = false;
    if (ret)
        return;
    c->error = KKC_ERROR_NONE;

    kkc e = *c;
    uint32_t* s = (uint32_t*)src;
    uint32_t* d = (uint32_t*)dst;
    uint32_t* k = e.k;
    size_t kl = (size_t)e.key.type * 8;
    size_t n = length / 0x10;
    size_t i;
    uint32_t b, c0, c1, c2, c3, t;
    if (e.key.mode == KKC_KEY_MODE_SIMPLE)
        for (i = 0; i < n; i++) {
            e.e = e.e0;
            e.f = e.f0;

            e.p = e.f;
            e.f = e.e;
            t = c0 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c1 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c2 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c3 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            b = *s++;
            *d++ = b ^ c0;

            b = *s++;
            *d++ = b ^ c1;

            b = *s++;
            *d++ = b ^ c2;

            b = *s++;
            *d++ = b ^ c3;
        }
    else if (e.key.mode == KKC_KEY_MODE_PAST) {
        for (i = 0; i < n; i++) {
            e.p = e.f;
            e.f = e.e;
            t = c0 = e.o2 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c1 = e.o0 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c2 = e.o3 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c3 = e.o1 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            b = *s++;
            *d++ = e.o0 = b ^ c0 ^ e.o0;

            b = *s++;
            *d++ = e.o1 = b ^ c1 ^ e.o1;

            b = *s++;
            *d++ = e.o2 = b ^ c2 ^ e.o2;

            b = *s++;
            *d++ = e.o3 = b ^ c3 ^ e.o3;
        }
    }
    *c = e;
}

void FASTCALL kkc_decurse(kkc* c, uint8_t* src, uint8_t* dst, size_t length) {
    if (!c)
        return;

    bool ret = true;
    if (c->error)
        return;
    else if (!c->k)
        c->error = KKC_ERROR_UNINITIALIZED_TABLE;
    else if (!c->key.data)
        c->error = KKC_ERROR_INVALID_KEY;
    else if (!src || !dst)
        c->error = KKC_ERROR_INVALID_DATA;
    else if (length & 0xF)
        c->error = KKC_ERROR_INVALID_DATA_LENGTH;
    else
        ret = false;
    if (ret)
        return;
    c->error = KKC_ERROR_NONE;

    kkc e = *c;
    uint32_t* s = (uint32_t*)src;
    uint32_t* d = (uint32_t*)dst;
    uint32_t* k = e.k;
    size_t kl = (size_t)e.key.type * 8;
    size_t n = length / 0x10;
    size_t i;
    uint32_t b, c0, c1, c2, c3, t;
    if (e.key.mode == KKC_KEY_MODE_SIMPLE)
        for (i = 0; i < n; i++) {
            e.e = e.e0;
            e.f = e.f0;

            e.p = e.f;
            e.f = e.e;
            t = c0 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c1 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c2 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c3 = e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            b = *s++;
            *d++ = b ^ c0;

            b = *s++;
            *d++ = b ^ c1;

            b = *s++;
            *d++ = b ^ c2;

            b = *s++;
            *d++ = b ^ c3;
        }
    else if (e.key.mode == KKC_KEY_MODE_PAST) {
        for (i = 0; i < n; i++) {
            e.p = e.f;
            e.f = e.e;
            t = c0 = e.o2 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c1 = e.o0 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c2 = e.o3 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            e.p = e.f;
            e.f = e.e;
            t = c3 = e.o1 ^ e.k[e.p];
            e.e ^= e.f ^= e.p = XOR32(t) % kl;

            b = *s++;
            *d++ = b ^ c0 ^ e.o0;
            e.o0 = b;

            b = *s++;
            *d++ = b ^ c1 ^ e.o1;
            e.o1 = b;

            b = *s++;
            *d++ = b ^ c2 ^ e.o2;
            e.o2 = b;

            b = *s++;
            *d++ = b ^ c3 ^ e.o3;
            e.o3 = b;
        }
    }
    *c = e;
}

void kkc_dispose(kkc* c) {
    if (!c)
        return;

    free(c->key.data);
    free(c->k);
    free(c);
}

static inline void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state) {
    if (!arr || length < 1)
        return;

    for (size_t i = 0; i < length; i++) {
        uint32_t x = *state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state = x;
        arr[i] = (uint8_t)x;
    }
}
