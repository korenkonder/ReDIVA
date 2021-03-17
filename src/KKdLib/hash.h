/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

extern uint64_t hash_fnv1a64(uint8_t* data, size_t length);
extern uint64_t hash_char_fnv1a64(char* data);
extern uint64_t hash_wchar_t_fnv1a64(wchar_t* data);
extern uint32_t hash_murmurhash(uint8_t* data, size_t length, uint32_t seed, bool already_upper, bool big_endian);
extern uint32_t hash_char_murmurhash(char* data, uint32_t seed, bool already_upper);
extern uint32_t hash_wchar_t_murmurhash(wchar_t* data, uint32_t seed, bool already_upper);
