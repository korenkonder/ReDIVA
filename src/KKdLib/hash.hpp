/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "default.hpp"

extern const uint64_t hash_fnv1a64m_empty;
extern const uint32_t hash_murmurhash_empty;
extern const uint32_t hash_crc16_ccitt_empty;

extern uint64_t hash_fnv1a64m(const void* data, size_t size, bool make_upper = false);
extern uint64_t hash_utf8_fnv1a64m(const char* data, bool make_upper = false);
extern uint64_t hash_utf16_fnv1a64m(const wchar_t* data, bool make_upper = false);
extern uint64_t hash_string_fnv1a64m(const std::string* data, bool make_upper = false);
extern uint32_t hash_murmurhash(const void* data, size_t size, uint32_t seed = 0, bool upper = false, bool big_endian = false);
extern uint32_t hash_utf8_murmurhash(const char* data, uint32_t seed = 0, bool upper = false);
extern uint32_t hash_utf16_murmurhash(const wchar_t* data, uint32_t seed = 0, bool upper = false);
extern uint32_t hash_string_murmurhash(const std::string* data, uint32_t seed = 0, bool upper = false);
extern uint16_t hash_crc16_ccitt(const void* data, size_t size, bool make_upper = false);
extern uint16_t hash_utf8_crc16_ccitt(const char* data, bool make_upper = false);
extern uint16_t hash_utf16_crc16_ccitt(const wchar_t* data, bool make_upper = false);
extern uint16_t hash_string_crc16_ccitt(const std::string* data, bool make_upper = false);
