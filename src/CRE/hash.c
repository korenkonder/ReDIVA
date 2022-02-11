/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hash.h"
#include "../KKdLib/hash.h"

inline hash hash_char(char* data) {
    return hash_data(data, utf8_length(data));
}

inline hash hash_utf16(wchar_t* data) {
    return hash_data(data, utf16_length(data));
}

inline hash hash_data(void* data, size_t length) {
    hash hash;
    hash.f = hash_fnv1a64m((uint8_t*)data, length, false);
    hash.m = hash_murmurhash((uint8_t*)data, length, 0, false, false);
    return hash;
}
