/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/f2_header.h"
#include "../../KKdLib/f2_struct.h"
#include "../../KKdLib/farc.h"
#include "../../KKdLib/hash.h"
#include "../../KKdLib/mat.h"
#include "../../KKdLib/quat.h"
#include "../../KKdLib/str_utils.h"
#include "../../KKdLib/txp.h"
#include "../../KKdLib/utf8.h"
#include "../../KKdLib/vec.h"
#include "../../KKdLib/vector.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

extern float_t frame_speed;
extern float_t target_fps;
extern float_t current_fps;

extern float_t get_frame_speed();
extern void FASTCALL axis_angle_from_vectors(vec3* axis, float_t* angle, vec3* vec0, vec3* vec1);
extern void FASTCALL mat3_mult_axis_angle(mat3* src, mat3* dst, vec3* axis, float_t angle);
extern void FASTCALL mat4_mult_axis_angle(mat4* src, mat4* dst, vec3* axis, float_t angle);
