/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

extern void* inject_data(void* address, void* data, size_t count);
extern uint8_t inject_uint8_t(void* address, uint8_t data);
extern uint16_t inject_uint16_t(void* address, uint16_t data);
extern uint32_t inject_uint32_t(void* address, uint32_t data);
extern uint64_t inject_uint64_t(void* address, uint64_t data);
extern void inject_patches();
extern void inject_restore();
