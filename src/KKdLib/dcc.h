/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Original research by Samyuu
*/

#pragma once

#include "default.h"

extern uint16_t calculate_checksum_uint16_t(uint8_t* data, size_t length);
extern uint16_t calculate_checksum_uint16_t_with_seed(uint8_t* data, size_t length, uint16_t seed);
extern uint32_t calculate_checksum_uint32_t(uint8_t* data, size_t length);
extern uint32_t calculate_checksum_uint32_t_with_seed(uint8_t* data, size_t length, uint32_t seed);
extern uint64_t calculate_checksum_uint64_t(uint8_t* data, size_t length);
extern uint64_t calculate_checksum_uint64_t_with_seed(uint8_t* data, size_t length, uint64_t seed);
