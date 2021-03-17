/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_curve* FASTCALL glitter_curve_init();
extern void FASTCALL glitter_curve_dispose(glitter_curve* c);

extern void FASTCALL Glitter__Curve__GetKeyIndexes(vector_glitter_curve_key* a1, float_t a2, int32_t* a3, int32_t* a4);
extern bool FASTCALL Glitter__Curve__GetValue(glitter_curve* curv, float_t frame, float_t* value, int32_t efct_val);
extern float_t FASTCALL Glitter__Curve__Interpolate(glitter_curve* a1, float_t frame,
    glitter_curve_key* a3, glitter_curve_key* a4, glitter_key_type type);
extern float_t FASTCALL Glitter__Curve__ApplyFlag(glitter_curve* curve, float_t value);
extern float_t FASTCALL Glitter__Curve__Randomize(glitter_curve* a1, glitter_curve_key* a2);
extern void FASTCALL Glitter__Curve__ParseFile(glitter_file_reader* fr,
    f2_header* header, vector_ptr_glitter_curve* vec);
extern void FASTCALL Glitter__Curve__UnpackFile(glitter_file_reader* fr, float_t* data,
    vector_ptr_glitter_curve* vec, glitter_curve* curve, uint32_t count, uint32_t keys_version);
