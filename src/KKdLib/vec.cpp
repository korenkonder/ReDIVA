/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.hpp"

const __m128 vec2_negate = { -0.0f, -0.0f,  0.0f,  0.0f };
const __m128 vec3_negate = { -0.0f, -0.0f, -0.0f,  0.0f };
const __m128 vec4_negate = { -0.0f, -0.0f, -0.0f, -0.0f };

const vec4i vec4_mask_vec2 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000, (int32_t)0x00000000 };
const vec4i vec4_mask_vec3 = { (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, (int32_t)0x00000000 };

const __m128d vec2d_negate = { -0.0, -0.0 };

const vec2 vec2_identity = { 1.0f, 1.0f };
const vec3 vec3_identity = { 1.0f, 1.0f, 1.0f };
const vec4 vec4_identity = { 1.0f, 1.0f, 1.0f, 1.0f };

const vec2 vec2_null = { 0.0f, 0.0f };
const vec3 vec3_null = { 0.0f, 0.0f, 0.0f };
const vec4 vec4_null = { 0.0f, 0.0f, 0.0f, 0.0f };

const vec2i vec2i_null = { 0, 0 };
const vec3i vec3i_null = { 0, 0, 0 };
const vec4i vec4i_null = { 0, 0, 0, 0 };

vec2i8::vec2i8() : x(), y() {

}

vec2i8::vec2i8(int8_t value) : x(value), y(value) {

}

vec2i8::vec2i8(int8_t x, int8_t y) : x(x), y(y) {

}

vec3i8::vec3i8() : x(), y(), z() {

}

vec3i8::vec3i8(int8_t value) : x(value), y(value), z(value) {

}

vec3i8::vec3i8(int8_t x, int8_t y, int8_t z) : x(x), y(y), z(z) {

}

vec4i8::vec4i8() : x(), y(), z(), w() {

}

vec4i8::vec4i8(int8_t value) : x(value), y(value), z(value), w(value) {

}

vec4i8::vec4i8(int8_t x, int8_t y, int8_t z, int8_t w) : x(x), y(y), z(z), w(w) {

}

vec2u8::vec2u8() : x(), y() {

}

vec2u8::vec2u8(uint8_t value) : x(value), y(value) {

}

vec2u8::vec2u8(uint8_t x, uint8_t y) : x(x), y(y) {

}

vec3u8::vec3u8() : x(), y(), z() {

}

vec3u8::vec3u8(uint8_t value) : x(value), y(value), z(value) {

}

vec3u8::vec3u8(uint8_t x, uint8_t y, uint8_t z) : x(x), y(y), z(z) {

}

vec4u8::vec4u8() : x(), y(), z(), w() {

}

vec4u8::vec4u8(uint8_t value) : x(value), y(value), z(value), w(value) {

}

vec4u8::vec4u8(uint8_t x, uint8_t y, uint8_t z, uint8_t w) : x(x), y(y), z(z), w(w) {

}

vec2i16::vec2i16() : x(), y() {

}

vec2i16::vec2i16(int16_t value) : x(value), y(value) {

}

vec2i16::vec2i16(int16_t x, int16_t y) : x(x), y(y) {

}

vec3i16::vec3i16() : x(), y(), z() {

}

vec3i16::vec3i16(int16_t value) : x(value), y(value), z(value) {

}

vec3i16::vec3i16(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {

}

vec4i16::vec4i16() : x(), y(), z(), w() {

}

vec4i16::vec4i16(int16_t value) : x(value), y(value), z(value), w(value) {

}

vec4i16::vec4i16(int16_t x, int16_t y, int16_t z, int16_t w) : x(x), y(y), z(z), w(w) {

}

vec2u16::vec2u16() : x(), y() {

}

vec2u16::vec2u16(uint16_t value) : x(value), y(value) {

}

vec2u16::vec2u16(uint16_t x, uint16_t y) : x(x), y(y) {

}

vec3u16::vec3u16() : x(), y(), z() {

}
;
vec3u16::vec3u16(uint16_t value) : x(value), y(value), z(value) {

}

vec3u16::vec3u16(uint16_t x, uint16_t y, uint16_t z) : x(x), y(y), z(z) {

}

vec4u16::vec4u16() : x(), y(), z(), w() {

}

vec4u16::vec4u16(uint16_t value) : x(value), y(value), z(value), w(value) {

}

vec4u16::vec4u16(uint16_t x, uint16_t y, uint16_t z, uint16_t w) : x(x), y(y), z(z), w(w) {

}

vec2h::vec2h() : x(), y() {

}

vec2h::vec2h(half_t value) : x(value), y(value) {

}

vec2h::vec2h(half_t x, half_t y) : x(x), y(y) {

}

vec3h::vec3h() : x(), y(), z() {

}

vec3h::vec3h(half_t value) : x(value), y(value), z(value) {

}

vec3h::vec3h(half_t x, half_t y, half_t z) : x(x), y(y), z(z) {

}

vec4h::vec4h() : x(), y(), z(), w() {

}

vec4h::vec4h(half_t value) : x(value), y(value), z(value), w(value) {

}

vec4h::vec4h(half_t x, half_t y, half_t z, half_t w) : x(x), y(y), z(z), w(w) {

}

vec2::vec2() : x(), y() {

}

vec2::vec2(float_t value) : x(value), y(value) {

}

vec2::vec2(float_t x, float_t y) : x(x), y(y) {

}

vec3::vec3() : x(), y(), z() {

}
;
vec3::vec3(float_t value) : x(value), y(value), z(value) {

}

vec3::vec3(float_t x, float_t y, float_t z) : x(x), y(y), z(z) {

}

vec4::vec4() : x(), y(), z(), w() {

}
;
vec4::vec4(float_t value) : x(value), y(value), z(value), w(value) {

}

vec4::vec4(float_t x, float_t y, float_t z, float_t w) : x(x), y(y), z(z), w(w) {

}

vec2i::vec2i() : x(), y() {

}

vec2i::vec2i(int32_t value) : x(value), y(value) {

}

vec2i::vec2i(int32_t x, int32_t y) : x(x), y(y) {

}

vec3i::vec3i() : x(), y(), z() {

}

vec3i::vec3i(int32_t value) : x(value), y(value), z(value) {

}

vec3i::vec3i(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {

}

vec4i::vec4i() : x(), y(), z(), w() {

}

vec4i::vec4i(int32_t value) : x(value), y(value), z(value), w(value) {

}

vec4i::vec4i(int32_t x, int32_t y, int32_t z, int32_t w) : x(x), y(y), z(z), w(w) {

}
