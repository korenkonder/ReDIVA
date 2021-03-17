/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "len_array.h"

typedef enum kkc_key_mode {
    KKC_KEY_MODE_SIMPLE = 1,
    KKC_KEY_MODE_PAST   = 2,
} kkc_key_mode;

typedef enum kkc_key_type {
    KKC_KEY_TYPE_NONE = 0,
    KKC_KEY_TYPE_128  = 2,
    KKC_KEY_TYPE_192  = 3,
    KKC_KEY_TYPE_256  = 4,
    KKC_KEY_TYPE_384  = 6,
    KKC_KEY_TYPE_512  = 8,
    KKC_KEY_TYPE_768  = 12,
    KKC_KEY_TYPE_1024 = 16,
    KKC_KEY_TYPE_1536 = 24,
    KKC_KEY_TYPE_2048 = 32,
} kkc_key_type;

typedef enum kkc_error {
    KKC_ERROR_NONE                = 0,
    KKC_ERROR_INVALID_DATA        = 1,
    KKC_ERROR_INVALID_DATA_LENGTH = 2,
    KKC_ERROR_INVALID_KEY         = 3,
    KKC_ERROR_INVALID_KEY_LENGTH  = 4,
    KKC_ERROR_UNINITIALIZED_TABLE = 5,
} kkc_error;

typedef struct kkc_key {
    uint8_t* data;
    kkc_key_mode mode;
    kkc_key_type type;
} kkc_key;

typedef struct kkc {
    kkc_key key;
    kkc_error error;

    uint32_t e, f, p;
    uint32_t a0, e0, f0, o0, o1, o2, o3;

    uint32_t state;

    uint32_t* k;
} kkc;

extern kkc* kkc_init();
extern void kkc_dispose(kkc* c);
extern void FASTCALL kkc_get_key(kkc_key* key, uint32_t seed, kkc_key_mode mode, kkc_key_type type);
extern void FASTCALL kkc_init_from_key_mode(kkc* c, len_array_char key, kkc_key_mode mode);
extern void FASTCALL kkc_init_from_type_mode(kkc* c, kkc_key_type type, kkc_key_mode mode);
extern void FASTCALL kkc_init_from_seed_type_mode(kkc* c, uint32_t seed, kkc_key_type type, kkc_key_mode mode);
extern void FASTCALL kkc_init_from_key(kkc* c, kkc_key* key);
extern void FASTCALL kkc_prepare_cursing_table(kkc* c);
extern void FASTCALL kkc_reset(kkc* c);
extern void FASTCALL kkc_reset_iv(kkc* c, uint32_t iv);
extern void FASTCALL kkc_reset_iv4(kkc* c, uint32_t iv0, uint32_t iv1, uint32_t iv2, uint32_t iv3);
extern void FASTCALL kkc_curse(kkc* c, uint8_t* src, uint8_t* dst, size_t length);
extern void FASTCALL kkc_decurse(kkc* c, uint8_t* src, uint8_t* dst, size_t length);