/*
    Original: https://github.com/kokke/tiny-AES-c
*/

#ifndef _AES_H_
#define _AES_H_

#include "default.hpp"
#include <immintrin.h>

// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES encryption in CBC-mode of operation.
// CTR enables encryption in counter-mode.
// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
#ifndef CBC
  #define CBC 1
#endif

#ifndef ECB
  #define ECB 1
#endif

#ifndef CTR
  #define CTR 1
#endif


//#define AES128 1
//#define AES192 1
//#define AES256 1

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only

#define AES128_KEYLEN 16   // Key length in bytes
#define AES128_keyExpSize 176
#define AES192_KEYLEN 24
#define AES192_keyExpSize 208
#define AES256_KEYLEN 32
#define AES256_keyExpSize 240

struct aes128_ctx {
    uint8_t RoundKey[AES128_keyExpSize];
    __m128i RoundKeyNI[AES128_keyExpSize / sizeof(__m128i) + 9];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
    uint8_t Iv[AES_BLOCKLEN];
#endif
};

struct aes192_ctx {
    uint8_t RoundKey[AES192_keyExpSize];
    __m128i RoundKeyNI[AES192_keyExpSize / sizeof(__m128i) + 11];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
    uint8_t Iv[AES_BLOCKLEN];
#endif
};

struct aes256_ctx {
    uint8_t RoundKey[AES256_keyExpSize];
    __m128i RoundKeyNI[AES256_keyExpSize / sizeof(__m128i) + 13];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
    uint8_t Iv[AES_BLOCKLEN];
#endif
};

void aes128_init_ctx(aes128_ctx* ctx, const uint8_t* key);
void aes192_init_ctx(aes192_ctx* ctx, const uint8_t* key);
void aes256_init_ctx(aes256_ctx* ctx, const uint8_t* key);
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
void aes128_init_ctx_iv(aes128_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void aes192_init_ctx_iv(aes192_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void aes256_init_ctx_iv(aes256_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void aes128_ctx_set_iv(aes128_ctx* ctx, const uint8_t* iv);
void aes192_ctx_set_iv(aes192_ctx* ctx, const uint8_t* iv);
void aes256_ctx_set_iv(aes256_ctx* ctx, const uint8_t* iv);
#endif

#if defined(ECB) && (ECB == 1)
// buffer size is exactly AES_BLOCKLEN bytes;
// you need only AES_init_ctx as IV is not used in ECB
// NB: ECB is considered insecure for most uses
void aes128_ecb_encrypt(aes128_ctx* ctx, uint8_t* buf);
void aes192_ecb_encrypt(aes192_ctx* ctx, uint8_t* buf);
void aes256_ecb_encrypt(aes256_ctx* ctx, uint8_t* buf);
void aes128_ecb_decrypt(aes128_ctx* ctx, uint8_t* buf);
void aes192_ecb_decrypt(aes192_ctx* ctx, uint8_t* buf);
void aes256_ecb_decrypt(aes256_ctx* ctx, uint8_t* buf);
void aes128_ecb_encrypt_buffer(aes128_ctx* ctx, uint8_t* buf, size_t length);
void aes192_ecb_encrypt_buffer(aes192_ctx* ctx, uint8_t* buf, size_t length);
void aes256_ecb_encrypt_buffer(aes256_ctx* ctx, uint8_t* buf, size_t length);
void aes128_ecb_decrypt_buffer(aes128_ctx* ctx, uint8_t* buf, size_t length);
void aes192_ecb_decrypt_buffer(aes192_ctx* ctx, uint8_t* buf, size_t length);
void aes256_ecb_decrypt_buffer(aes256_ctx* ctx, uint8_t* buf, size_t length);

#endif // #if defined(ECB) && (ECB == !)


#if defined(CBC) && (CBC == 1)
// buffer size MUST be mutile of AES_BLOCKLEN;
// Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key
void aes128_cbc_encrypt(aes128_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes192_cbc_encrypt(aes192_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes256_cbc_encrypt(aes256_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes128_cbc_decrypt(aes128_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes192_cbc_decrypt(aes192_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes256_cbc_decrypt(aes256_ctx* ctx, uint8_t buf[AES_BLOCKLEN]);
void aes128_cbc_encrypt_buffer(aes128_ctx* ctx, uint8_t* buf, size_t length);
void aes192_cbc_encrypt_buffer(aes192_ctx* ctx, uint8_t* buf, size_t length);
void aes256_cbc_encrypt_buffer(aes256_ctx* ctx, uint8_t* buf, size_t length);
void aes128_cbc_decrypt_buffer(aes128_ctx* ctx, uint8_t* buf, size_t length);
void aes192_cbc_decrypt_buffer(aes192_ctx* ctx, uint8_t* buf, size_t length);
void aes256_cbc_decrypt_buffer(aes256_ctx* ctx, uint8_t* buf, size_t length);

#endif // #if defined(CBC) && (CBC == 1)


#if defined(CTR) && (CTR == 1)

// Same function for encrypting as for decrypting.
// IV is incremented for every block, and used after encryption as XOR-compliment for output
// Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx with AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key
void aes128_ctr_xcrypt_buffer(aes128_ctx* ctx, uint8_t* buf, uint32_t length);
void aes192_ctr_xcrypt_buffer(aes192_ctx* ctx, uint8_t* buf, uint32_t length);
void aes256_ctr_xcrypt_buffer(aes256_ctx* ctx, uint8_t* buf, uint32_t length);

#endif // #if defined(CTR) && (CTR == 1)

#endif // _AES_H_
