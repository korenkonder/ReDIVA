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

uint8_t inject_uint8_t(void* address, uint8_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint8_t ret = *(uint8_t*)address;
    *(uint8_t*)address = data;
    VirtualProtect(address, sizeof(uint8_t), old_protect, &old_protect);
    return ret;
}

uint16_t inject_uint16_t(void* address, uint16_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint16_t ret = *(uint16_t*)address;
    *(uint16_t*)address = data;
    VirtualProtect(address, sizeof(uint16_t), old_protect, &old_protect);
    return ret;
}

uint32_t inject_uint32_t(void* address, uint32_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint32_t ret = *(uint32_t*)address;
    *(uint32_t*)address = data;
    VirtualProtect(address, sizeof(uint32_t), old_protect, &old_protect);
    return ret;
}

uint64_t inject_uint64_t(void* address, uint64_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint64_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint64_t ret = *(uint64_t*)address;
    *(uint64_t*)address = data;
    VirtualProtect(address, sizeof(uint64_t), old_protect, &old_protect);
    return ret;
}

static patch_struct patch_data[] = {
    { (void*)0x00000001401D3860, (uint64_t)&printf_proxy, },
    { (void*)0x00000001400DE640, (uint64_t)&printf_proxy, },
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

    *(uint64_t*)&origExNodeBlock__CtrlBegin = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlBegin, (uint64_t)ExNodeBlock__CtrlBegin);
    *(uint64_t*)&origExOsageBlock__Init = inject_uint64_t(
        &ExOsageBlock_vftable->Init, (uint64_t)ExOsageBlock__Init);
    *(uint64_t*)&origExOsageBlock__CtrlStep = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlStep, (uint64_t)ExOsageBlock__CtrlStep);
    *(uint64_t*)&origExOsageBlock__CtrlMain = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlMain, (uint64_t)ExOsageBlock__CtrlMain);
    *(uint64_t*)&origExOsageBlock__CtrlOsagePlayData = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlOsagePlayData, (uint64_t)ExOsageBlock__CtrlOsagePlayData);
    *(uint64_t*)&origExOsageBlock__Disp = inject_uint64_t(
        &ExOsageBlock_vftable->Disp, (uint64_t)ExOsageBlock__Disp);
    *(uint64_t*)&origExOsageBlock__Reset = inject_uint64_t(
        &ExOsageBlock_vftable->Reset, (uint64_t)ExOsageBlock__Reset);
    *(uint64_t*)&origExOsageBlock__Field_40 = inject_uint64_t(
        &ExOsageBlock_vftable->Field40, (uint64_t)ExOsageBlock__Field_40);
    *(uint64_t*)&origExOsageBlock__CtrlInitBegin = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlInitBegin, (uint64_t)ExOsageBlock__CtrlInitBegin);
    *(uint64_t*)&origExOsageBlock__CtrlInitMain = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlInitMain, (uint64_t)ExOsageBlock__CtrlInitMain);
    *(uint64_t*)&origExOsageBlock__CtrlEnd = inject_uint64_t(
        &ExOsageBlock_vftable->CtrlEnd, (uint64_t)ExOsageBlock__CtrlEnd);

    *(uint64_t*)&glutMainLoop = inject_uint64_t(
        (void*)0x0000000140966008, (uint64_t)&gl_get_func_pointers);
}
