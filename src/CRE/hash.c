/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hash.h"
#include "../KKdLib/hash.h"

uint64_t hash_char(char* data) {
    return hash_data(data, strlen(data));
}

uint64_t hash_wchar_t(wchar_t* data) {
    return hash_data(data, wcslen(data));
}

uint64_t hash_data(void* data, size_t length) {
    uint64_t hash, hash1, hash2;
    hash1 = hash_fnv1a64(data, length);
    hash2 = hash_murmurhash(data, length, 0, false, false);
    hash = hash1 | (hash2 << 32);
    return hash;
}
