/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hash.h"
#include "utf8.h"

const uint64_t hash_fnv1a64_empty = 0xCBF29CE44FD0BFC1;
const uint32_t hash_murmurhash_empty = 0x0CAD3078;

uint64_t hash_fnv1a64(uint8_t* data, size_t length) { // 0x1403B04D0 in SBZV_7.10; FNV 1a 64-bit
    uint64_t hash = 0xCBF29CE484222325;
    if (data)
        for (size_t i = 0; i < length; i++) {
            hash ^= data[i];
            hash *= 0x100000001B3;
        }
    return (hash >> 32) ^ hash;
}

inline uint64_t hash_char_fnv1a64(char* data) {
    uint64_t hash = hash_fnv1a64((uint8_t*)data, strlen(data));
    return hash;
}

inline uint64_t hash_wchar_t_fnv1a64(wchar_t* data) {
    char* temp = utf8_encode(data);
    uint64_t hash = hash_fnv1a64((uint8_t*)temp, strlen(temp));
    free(temp);
    return hash;
}

uint32_t hash_murmurhash(uint8_t* data, size_t length,
    uint32_t seed, bool already_upper, bool big_endian) { // 0x814D7A9C in PCSB00554; MurmurHash
                                                          // 0x8134C304 in PCSB01007
                                                          // 0x0069CEA4 in NPEB02013
    uint32_t a, b, hash;
    size_t i;

    const uint32_t m = 0x7FD652AD;
    const int32_t r = 16;

    hash = seed + 0xDEADBEEF;
    if (data)
        if (already_upper) {
            if (big_endian)
                for (i = 0; length > 3; length -= 4, i += 4) {
                    b = reverse_endianess_uint32_t(*(uint32_t*)&data[i]);
                    hash += b;
                    hash *= m;
                    hash ^= hash >> r;
                }
            else
                for (i = 0; length > 3; length -= 4, i += 4) {
                    b = *(uint32_t*)&data[i];
                    hash += b;
                    hash *= m;
                    hash ^= hash >> r;
                }

            if (length > 0) {
                if (length > 1) {
                    if (length > 2)
                        hash += (uint32_t)data[i + 2] << 16;
                    hash += (uint32_t)data[i + 1] << 8;
                }
                hash += data[i];
                hash *= m;
                hash ^= hash >> r;
            }
        }
        else {
            if (big_endian)
                for (i = 0; length > 3; length -= 4, i += 4) {
                    b = reverse_endianess_uint32_t(*(uint32_t*)&data[i]);

                    a = b & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFFFFFF00) | a;

                    a = (b >> 8) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFFFF00FF) | (a << 8);

                    a = (b >> 16) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFF00FFFF) | (a << 16);

                    a = (b >> 24) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0x00FFFFFF) | (a << 24);

                    hash += b;
                    hash *= m;
                    hash ^= hash >> r;
                }
            else
                for (i = 0; length > 3; length -= 4, i += 4) {
                    b = *(uint32_t*)&data[i];

                    a = b & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFFFFFF00) | a;

                    a = (b >> 8) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFFFF00FF) | (a << 8);

                    a = (b >> 16) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0xFF00FFFF) | (a << 16);

                    a = (b >> 24) & 0xFF;
                    if (a > 0x60 && a < 0x7B)
                        a -= 0x20;
                    b = (b & 0x00FFFFFF) | (a << 24);

                    hash += b;
                    hash *= m;
                    hash ^= hash >> r;
                }

            if (length > 0) {
                if (length > 1) {
                    if (length > 2) {
                        b = data[i + 2];
                        if (b > 0x60 && b < 0x7B)
                            b -= 0x20;
                        hash += b << 16;
                    }

                    b = data[i + 1];
                    if (b > 0x60 && b < 0x7B)
                        b -= 0x20;
                    hash += b << 8;
                }

                b = data[i];
                if (b > 0x60 && b < 0x7B)
                    b -= 0x20;
                hash += b;

                hash *= m;
                hash ^= hash >> r;
            }
        }

    hash *= m;
    hash ^= hash >> 10;
    hash *= m;
    hash ^= hash >> 17;
    return hash;
}

inline uint32_t hash_char_murmurhash(char* data, uint32_t seed, bool already_upper) {
    uint32_t hash = hash_murmurhash((uint8_t*)data, strlen(data), seed, already_upper, false);
    return hash;
}

inline uint32_t hash_wchar_t_murmurhash(wchar_t* data, uint32_t seed, bool already_upper) {
    char* temp = utf8_encode(data);
    uint32_t hash = hash_murmurhash((uint8_t*)temp, strlen(temp), seed, already_upper, false);
    free(temp);
    return hash;
}
