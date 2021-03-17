/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_curve* FASTCALL glitter_curve_init();
extern float_t FASTCALL glitter_curve_apply_flag(glitter_curve* curve, float_t value);
extern void FASTCALL glitter_curve_get_key_indexes(vector_glitter_curve_key* a1, float_t a2, size_t* a3, size_t* a4);
extern bool FASTCALL glitter_curve_get_value(glitter_curve* curv, float_t frame, float_t* value, int32_t random);
extern float_t FASTCALL glitter_curve_interpolate(glitter_curve* a1, float_t frame,
    glitter_curve_key* a3, glitter_curve_key* a4, glitter_key_type type);
extern void FASTCALL glitter_curve_parse_file(glitter_file_reader* fr,
    f2_header* header, vector_ptr_glitter_curve* vec);
extern float_t FASTCALL glitter_curve_randomize(glitter_curve* a1, glitter_curve_key* a2);
extern void FASTCALL glitter_curve_unpack_file(glitter_file_reader* fr, float_t* data,
    vector_ptr_glitter_curve* vec, glitter_curve* curve, uint32_t count, uint32_t keys_version);
extern void FASTCALL glitter_curve_dispose(glitter_curve* c);
