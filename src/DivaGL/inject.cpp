/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "inject.hpp"
#include "bone_data.hpp"
#include "gl.hpp"
#include "print.hpp"
#include <Windows.h>

struct patch_struct {
    void* address;
    size_t func_ptr;
};

void inject_data(void* address, void* data, size_t count) {
    DWORD old_protect;
    VirtualProtect(address, count, PAGE_EXECUTE_READWRITE, &old_protect);
    memmove(address, data, count);
    VirtualProtect(address, count, old_protect, &old_protect);
}

void inject_uint8_t(void* address, uint8_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &old_protect);
    *(uint8_t*)address = data;
    VirtualProtect(address, sizeof(uint8_t), old_protect, &old_protect);
}

void inject_uint16_t(void* address, uint16_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &old_protect);
    *(uint16_t*)address = data;
    VirtualProtect(address, sizeof(uint16_t), old_protect, &old_protect);
}

void inject_uint32_t(void* address, uint32_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &old_protect);
    *(uint32_t*)address = data;
    VirtualProtect(address, sizeof(uint32_t), old_protect, &old_protect);
}

void inject_uint64_t(void* address, uint64_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint64_t), PAGE_EXECUTE_READWRITE, &old_protect);
    *(uint64_t*)address = data;
    VirtualProtect(address, sizeof(uint64_t), old_protect, &old_protect);
}

static patch_struct patch_data[] = {
    { (void*)0x00000001401D3860, (uint64_t)&printf_proxy, },
    { (void*)0x00000001400DE640, (uint64_t)&printf_proxy, },
    { (void*)0x00000001405F39E0, (uint64_t)&ExNodeBlock__Field_10, },
    { (void*)0x00000001405F3640, (uint64_t)&ExOsageBlock__Init, },
    { (void*)0x00000001405F49F0, (uint64_t)&ExOsageBlock__Field_18, },
    { (void*)0x00000001405F2140, (uint64_t)&ExOsageBlock__Field_20, },
    { (void*)0x00000001405F2470, (uint64_t)&ExOsageBlock__SetOsagePlayData, },
    { (void*)0x00000001405F26F0, (uint64_t)&ExOsageBlock__Disp, },
    { (void*)0x00000001405F2640, (uint64_t)&ExOsageBlock__Reset, },
    { (void*)0x00000001405F2860, (uint64_t)&ExOsageBlock__Field_40, },
    { (void*)0x00000001405F4640, (uint64_t)&ExOsageBlock__Field_48, },
    { (void*)0x00000001405F4730, (uint64_t)&ExOsageBlock__Field_50, },
    { (void*)0x00000001405F48F0, (uint64_t)&ExOsageBlock__Field_58, },
};

void inject_patches() {
    uint8_t buf[12];
    buf[0] = 0x48;
    buf[1] = 0xB8;
    buf[10] = 0xFF;
    buf[11] = 0xE0;

    for (int32_t i = 0; i < sizeof(patch_data) / sizeof(patch_struct); i++)
        if (patch_data[i].func_ptr) {
            memcpy(&buf[2], &patch_data[i].func_ptr, sizeof(uint64_t));
            inject_data(patch_data[i].address, buf, 12);
        }

    glutMainLoop = (void (FASTCALL*)())*(size_t*)0x0000000140966008;
    inject_uint64_t((void*)0x0000000140966008, (uint64_t)&gl_get_func_pointers);
}
