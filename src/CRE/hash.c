/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hash.h"
#include "../KKdLib/hash.h"

inline hash hash_char(char* data) {
    return hash_data(data, strlen(data));
}

inline hash hash_wchar_t(wchar_t* data) {
    return hash_data(data, wcslen(data));
}

inline hash hash_data(void* data, size_t length) {
    hash hash;
    hash.f = hash_fnv1a64(data, length);
    hash.m = hash_murmurhash(data, length, 0, false, false);
    return hash;
}
