/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vector.h"

typedef struct pvsr_aet_sub1 {
    string name;
    uint32_t hash;
    uint16_t unk2;
} pvsr_aet_sub1;

typedef struct pvsr_aet_sub2 {
    uint16_t u00;
    uint16_t u02;
    uint16_t u04;
    uint16_t u06;
    uint16_t u08;
    uint16_t u0a;
    uint16_t u0c;
} pvsr_aet_sub2;

typedef struct pvsr_aet_entry {
    string name;
    uint32_t hash;
    float_t bright_scale;
} pvsr_aet_entry;

vector_old(pvsr_aet_entry)

typedef struct pvsr_aet {
    uint16_t u48;
    uint16_t u4a;
    uint16_t u4c;
    uint16_t u4e;
    uint16_t u50;
    uint16_t u52;
    uint16_t u54;
    uint16_t u56;
    uint16_t u58;
    uint16_t u5a;
    uint16_t u5c;
    uint16_t u5e;
    uint16_t u60;

    string set_name;
    vector_old_pvsr_aet_entry front;
    vector_old_pvsr_aet_entry front_low;
    vector_old_pvsr_aet_entry back;
    pvsr_aet_sub1 sub1_data;
    bool sub1_data_init;
    pvsr_aet_sub2 sub2a_data;
    bool sub2a_data_init;
    pvsr_aet_sub2 sub2b_data;
    bool sub2b_data_init;
    pvsr_aet_sub2 sub2c_data;
    bool sub2c_data_init;
    pvsr_aet_sub2 sub2d_data;
    bool sub2d_data_init;
    vector_old_pvsr_aet_entry unk03;
    vector_old_pvsr_aet_entry unk04;
} pvsr_aet;

typedef struct pvsr_a3da {
    string name;
    uint32_t hash;
} pvsr_a3da;

typedef struct pvsr_effect {
    string name;
    float_t emission;
} pvsr_effect;

typedef struct pvsr_glitter {
    string name;
    uint8_t unk1;
} pvsr_glitter;

vector_old(pvsr_a3da)
vector_old(pvsr_glitter)

typedef struct pvsr_stage_effect {
    vector_old_pvsr_a3da a3da;
    vector_old_pvsr_glitter glitter;
} pvsr_stage_effect;

vector_old(pvsr_aet)
vector_old(pvsr_effect)
vector_old(pvsr_stage_effect)

#define PVSR_STAGE_CHANGE_EFFECT_COUNT 0x10

typedef struct pvsr {
    bool ready;

    vector_old_pvsr_aet aet;
    vector_old_pvsr_effect effect;
    vector_old_string emcs;
    pvsr_stage_effect stage_change_effect[PVSR_STAGE_CHANGE_EFFECT_COUNT][PVSR_STAGE_CHANGE_EFFECT_COUNT];
    bool stage_change_effect_init[PVSR_STAGE_CHANGE_EFFECT_COUNT][PVSR_STAGE_CHANGE_EFFECT_COUNT];
    vector_old_pvsr_stage_effect stage_effect;
} pvsr;

extern void pvsr_init(pvsr* sr);
extern void pvsr_read(pvsr* sr, char* path);
extern void pvsr_wread(pvsr* sr, wchar_t* path);
extern bool pvsr_load_file(void* data, char* path, char* file, uint32_t hash);
extern void pvsr_free(pvsr* sr);

extern void pvsr_a3da_init(pvsr_a3da* a3d);
extern void pvsr_a3da_free(pvsr_a3da* a3d);
extern void pvsr_aet_init(pvsr_aet* aet);
extern void pvsr_aet_free(pvsr_aet* aet);
extern void pvsr_aet_entry_init(pvsr_aet_entry* aet_entry);
extern void pvsr_aet_entry_free(pvsr_aet_entry* aet_entry);
extern void pvsr_aet_sub1_init(pvsr_aet_sub1* aet_sub1);
extern void pvsr_aet_sub1_free(pvsr_aet_sub1* aet_sub1);
extern void pvsr_aet_sub2_init(pvsr_aet_sub2* aet_sub2);
extern void pvsr_aet_sub2_free(pvsr_aet_sub2* aet_sub2);
extern void pvsr_effect_init(pvsr_effect* eff);
extern void pvsr_effect_free(pvsr_effect* eff);
extern void pvsr_glitter_init(pvsr_glitter* glt);
extern void pvsr_glitter_free(pvsr_glitter* glt);
extern void pvsr_stage_effect_init(pvsr_stage_effect* stg_eff);
extern void pvsr_stage_effect_free(pvsr_stage_effect* stg_eff);
