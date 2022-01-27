/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

extern const uint64_t hash_fnv1a64m_empty;
extern const uint32_t hash_murmurhash_empty;
extern const uint32_t hash_crc16_ccitt_empty;

extern uint64_t hash_fnv1a64m(uint8_t* data, size_t length, bool make_upper);
extern uint64_t hash_utf8_fnv1a64m(char* data, bool make_upper);
extern uint64_t hash_utf16_fnv1a64m(wchar_t* data, bool make_upper);
extern uint32_t hash_murmurhash(uint8_t* data, size_t length, uint32_t seed, bool upper, bool big_endian);
extern uint32_t hash_utf8_murmurhash(char* data, uint32_t seed, bool upper);
extern uint32_t hash_utf16_murmurhash(wchar_t* data, uint32_t seed, bool upper);
extern uint16_t hash_crc16_ccitt(uint8_t* data, size_t length, bool make_upper);
extern uint16_t hash_utf8_crc16_ccitt(uint8_t* data, bool make_upper);
extern uint16_t hash_utf16_crc16_ccitt(wchar_t* data, bool make_upper);
