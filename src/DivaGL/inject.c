/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "inject.h"
#include "bone_data.h"
#include "gl.h"
#include "print.h"
#include "shader_table.h"
#include <Windows.h>

typedef struct patch_struct {
    void* address;
    size_t func_ptr;
    void* orig_data;
} patch_struct;

void* inject_data(void* address, void* data, size_t count) {
    DWORD old_protect;
    VirtualProtect(address, count, PAGE_EXECUTE_READWRITE, &old_protect);
    void* orig_data = force_malloc(count);
    memcpy(orig_data, (const void*)address, count);
    memcpy(address, (const void*)data, count);
    VirtualProtect(address, count, old_protect, &old_protect);
    return orig_data;
}

uint8_t inject_uint8_t(void* address, uint8_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint8_t value = *(uint8_t*)address;
    *(uint8_t*)address = data;
    VirtualProtect(address, sizeof(uint8_t), old_protect, &old_protect);
    return value;
}

uint16_t inject_uint16_t(void* address, uint16_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint16_t value = *(uint16_t*)address;
    *(uint16_t*)address = data;
    VirtualProtect(address, sizeof(uint16_t), old_protect, &old_protect);
    return value;
}

uint32_t inject_uint32_t(void* address, uint32_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint32_t value = *(uint32_t*)address;
    *(uint32_t*)address = data;
    VirtualProtect(address, sizeof(uint32_t), old_protect, &old_protect);
    return value;
}

uint64_t inject_uint64_t(void* address, uint64_t data) {
    DWORD old_protect;
    VirtualProtect(address, sizeof(uint64_t), PAGE_EXECUTE_READWRITE, &old_protect);
    uint64_t value = *(uint64_t*)address;
    *(uint64_t*)address = data;
    VirtualProtect(address, sizeof(uint64_t), old_protect, &old_protect);
    return value;
}

static patch_struct patch_data[] = {
    { (void*)0x00000001405E4CE0, (uint64_t)&aft_shader_set, },
    { (void*)0x00000001405E4FC0, (uint64_t)&aft_shader_load_all_shaders, },
    { (void*)0x00000001405E4B50, (uint64_t)&aft_shader_bind, },
    { (void*)0x00000001401D3860, (uint64_t)&printf_proxy, },
    //{ (void*)0x0000000140507210, (uint64_t)&rob_chara_data_reset, },
    //{ (void*)0x00000001401EBD30, (uint64_t)&bone_data_mult_ik, },
    //{ (void*)0x00000001401EB6D0, (uint64_t)&bone_data_mult_1, },
    //{ (void*)0x000000014041DA50, (uint64_t)&bone_data_mult_2, },
    //{ (void*)0x000000014041CF50, (uint64_t)&rob_chara_data_bone_data_motion_load, },
    //{ (void*)0x0000000140418870, (uint64_t)&rob_chara_data_bone_data_interpolate, },
    //{ (void*)0x0000000140418A60, (uint64_t)&rob_chara_data_bone_data_update, },
};

static uint64_t shader_bind_func_orig_data[6];

void inject_patches() {
    char buf[12];
    buf[0] = 0x48;
    buf[1] = 0xB8;
    buf[10] = 0xFF;
    buf[11] = 0xE0;

    for (int32_t i = 0; i < sizeof(patch_data) / sizeof(patch_struct); i++)
        if (patch_data[i].func_ptr) {
            free(patch_data[i].orig_data);
            *(uint64_t*)&buf[2] = patch_data[i].func_ptr;
            patch_data[i].orig_data = inject_data(patch_data[i].address, buf, 12);
        }

    glutMainLoop = (void*)inject_uint64_t((void*)0x0000000140966008, (uint64_t)&gl_get_func_pointers);

    shader_bind_func_orig_data[0] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[0].bind_func, (uint64_t)&aft_shader_bind_blinn);
    shader_bind_func_orig_data[1] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[1].bind_func, (uint64_t)&aft_shader_bind_cloth);
    shader_bind_func_orig_data[2] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[2].bind_func, (uint64_t)&aft_shader_bind_hair);
    shader_bind_func_orig_data[3] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[3].bind_func, (uint64_t)&aft_shader_bind_membrane);
    shader_bind_func_orig_data[4] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[4].bind_func, (uint64_t)&aft_shader_bind_eye_ball);
    shader_bind_func_orig_data[5] = inject_uint64_t(
        (void*)&shader_name_bind_func_table[5].bind_func, (uint64_t)&aft_shader_bind_tone_map);
}

void inject_restore() {
    for (int32_t i = 0; i < sizeof(patch_data) / sizeof(patch_struct); i++)
        if (patch_data[i].func_ptr) {
            void* data = inject_data(patch_data[i].address, patch_data[i].orig_data, 12);
            free(data);
        }

     inject_uint64_t((void*)0x0000000140966008, (uint64_t)glutMainLoop);

     for (int32_t i = 0; i < 6; i++)
         inject_uint64_t(shader_name_bind_func_table[i].bind_func, shader_bind_func_orig_data[i]);
}
