/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"

extern const uint64_t hash_fnv1a64m_empty;
extern const uint32_t hash_murmurhash_empty;
extern const uint32_t hash_crc16_ccitt_empty;

extern uint64_t hash_fnv1a64m(void* data, size_t length, bool make_upper);
inline uint64_t hash_fnv1a64m(const void* data, size_t length, bool make_upper);
extern uint64_t hash_utf8_fnv1a64m(char* data, bool make_upper);
extern uint64_t hash_utf8_fnv1a64m(const char* data, bool make_upper);
extern uint64_t hash_utf16_fnv1a64m(wchar_t* data, bool make_upper);
extern uint64_t hash_utf16_fnv1a64m(const wchar_t* data, bool make_upper);
extern uint64_t hash_string_fnv1a64m(string* data, bool make_upper);
extern uint32_t hash_murmurhash(void* data, size_t length, uint32_t seed, bool upper, bool big_endian);
extern uint32_t hash_murmurhash(const void* data, size_t length, uint32_t seed, bool upper, bool big_endian);
extern uint32_t hash_utf8_murmurhash(char* data, uint32_t seed, bool upper);
extern uint32_t hash_utf8_murmurhash(const char* data, uint32_t seed, bool upper);
extern uint32_t hash_utf16_murmurhash(wchar_t* data, uint32_t seed, bool upper);
extern uint32_t hash_utf16_murmurhash(const wchar_t* data, uint32_t seed, bool upper);
extern uint32_t hash_string_murmurhash(string* data, uint32_t seed, bool upper);
extern uint16_t hash_crc16_ccitt(void* data, size_t length, bool make_upper);
extern uint16_t hash_crc16_ccitt(const void* data, size_t length, bool make_upper);
extern uint16_t hash_utf8_crc16_ccitt(char* data, bool make_upper);
extern uint16_t hash_utf8_crc16_ccitt(const char* data, bool make_upper);
extern uint16_t hash_utf16_crc16_ccitt(wchar_t* data, bool make_upper);
extern uint16_t hash_utf16_crc16_ccitt(const wchar_t* data, bool make_upper);
extern uint16_t hash_string_crc16_ccitt(string* data, bool make_upper);
