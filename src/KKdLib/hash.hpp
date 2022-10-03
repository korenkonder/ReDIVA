/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "default.hpp"

// Empty string
extern const uint64_t hash_fnv1a64m_empty;
// Empty string
extern const uint32_t hash_murmurhash_empty;
// "NULL" string
extern const uint32_t hash_murmurhash_null;
// Empty string
extern const uint32_t hash_crc16_ccitt_empty;

struct string_hash {
    std::string str;
    uint64_t hash_fnv1a64m;
    uint32_t hash_murmurhash;

    string_hash();
    string_hash(const char* str);
    string_hash(std::string& str);
    string_hash(std::string&& str);
    ~string_hash();

    const char* c_str();
};

extern uint64_t hash_fnv1a64m(const void* data, size_t size, bool make_upper = false);
extern uint32_t hash_murmurhash(const void* data, size_t size,
    uint32_t seed = 0, bool upper = false, bool big_endian = false);
extern uint16_t hash_crc16_ccitt(const void* data, size_t size, bool make_upper = false);

inline uint64_t hash_utf8_fnv1a64m(const char* data, bool make_upper = false) {
    return hash_fnv1a64m(data, utf8_length(data), make_upper);
}

inline uint64_t hash_utf16_fnv1a64m(const wchar_t* data, bool make_upper = false) {
    char* temp = utf16_to_utf8(data);
    uint64_t hash = hash_fnv1a64m(temp, utf8_length(temp), make_upper);
    free_def(temp);
    return hash;
}

inline uint64_t hash_string_fnv1a64m(const std::string& data, bool make_upper = false) {
    return hash_fnv1a64m(data.c_str(), data.size(), make_upper);
}

inline uint32_t hash_utf8_murmurhash(const char* data, uint32_t seed = 0, bool upper = false) {
    return hash_murmurhash(data, utf8_length(data), seed, upper);
}

inline uint32_t hash_utf16_murmurhash(const wchar_t* data, uint32_t seed = 0, bool upper = false) {
    char* temp = utf16_to_utf8(data);
    uint32_t hash = hash_murmurhash(temp, utf8_length(temp), seed, upper);
    free_def(temp);
    return hash;
}

inline uint32_t hash_string_murmurhash(const std::string& data, uint32_t seed = 0, bool upper = false) {
    return hash_murmurhash(data.c_str(), data.size(), seed, upper);
}

inline uint16_t hash_utf8_crc16_ccitt(const char* data, bool make_upper = false) {
    return hash_crc16_ccitt(data, utf8_length(data), make_upper);
}

inline uint16_t hash_utf16_crc16_ccitt(const wchar_t* data, bool make_upper = false) {
    char* temp = utf16_to_utf8(data);
    uint32_t hash = hash_crc16_ccitt(temp, utf8_length(temp), make_upper);
    free_def(temp);
    return hash;
}

inline uint16_t hash_string_crc16_ccitt(const std::string& data, bool make_upper = false) {
    return hash_crc16_ccitt(data.c_str(), data.size(), make_upper);
}
