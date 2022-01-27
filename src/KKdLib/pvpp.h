/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vec.h"
#include "vector.h"

typedef enum pvpp_chara_type {
    PVPP_CHARA_MIKU   = 0,
    PVPP_CHARA_RIN    = 1,
    PVPP_CHARA_LEN    = 2,
    PVPP_CHARA_LUKA   = 3,
    PVPP_CHARA_NERU   = 4,
    PVPP_CHARA_HAKU   = 5,
    PVPP_CHARA_KAITO  = 6,
    PVPP_CHARA_MEIKO  = 7,
    PVPP_CHARA_SAKINE = 8,
    PVPP_CHARA_TETO   = 9,
    PVPP_CHARA_EXTRA  = 10,
    PVPP_CHARA_STAGE  = 255,
} pvpp_chara_type;

typedef struct pvpp_a3da {
    uint32_t hash;
    string name;
} pvpp_a3da;

typedef struct pvpp_chara_effect_a3da {
    pvpp_a3da a3da;
    uint8_t u00;
    uint8_t u01;
    uint8_t u02;
    uint8_t u03;
    uint8_t u04;
    uint8_t u05;
    uint8_t u06;
    uint8_t u07;
} pvpp_chara_effect_a3da;

vector(pvpp_chara_effect_a3da)

typedef struct pvpp_chara_effect {
    pvpp_chara_type base_chara;
    vector_pvpp_chara_effect_a3da effect_a3da;
} pvpp_chara_effect;

vector(pvpp_a3da)

typedef struct pvpp_chara_item {
    vector_pvpp_a3da a3da;
    string bone;
    vec4u u18;
} pvpp_chara_item;

typedef struct pvpp_glitter {
    string name;
    string unk1;
    bool unk2;
} pvpp_glitter;

typedef struct pvpp_motion {
    uint32_t hash;
    string name;
} pvpp_motion;

vector(pvpp_glitter)

typedef struct pvpp_effect {
    vector_pvpp_a3da a3da;
    pvpp_chara_type chara_index;
    vector_pvpp_glitter glitter;
} pvpp_effect;

vector(pvpp_chara_effect)
vector(pvpp_motion)
vector(pvpp_chara_item)

typedef struct pvpp_chara {
    vector_pvpp_a3da a3da;
    vector_pvpp_chara_item item;
    vector_pvpp_chara_effect chara_effect;
    vector_pvpp_glitter glitter;
    vector_pvpp_motion motion;
} pvpp_chara;

vector(pvpp_chara)
vector(pvpp_effect)

typedef struct pvpp {
    bool ready;

    vector_pvpp_chara chara;
    vector_pvpp_effect effect;
} pvpp;

extern void pvpp_init(pvpp* pp);
extern void pvpp_read(pvpp* pp, char* path);
extern void pvpp_wread(pvpp* pp, wchar_t* path);
extern bool pvpp_load_file(void* data, char* path, char* file, uint32_t hash);
extern void pvpp_free(pvpp* pp);
