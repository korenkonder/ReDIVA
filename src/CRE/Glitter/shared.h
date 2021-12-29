/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/database/object.h"
#include "../../KKdLib/f2/header.h"
#include "../../KKdLib/f2/struct.h"
#include "../../KKdLib/hash.h"
#include "../../KKdLib/mat.h"
#include "../../KKdLib/txp.h"
#include "../../KKdLib/vec.h"
#include "../../KKdLib/vector.h"
#include "../static_var.h"
#include "../texture.h"

extern void axis_angle_from_vectors(vec3* axis, float_t* angle, vec3* vec0, vec3* vec1);
extern void mat3_mult_axis_angle(mat3* src, mat3* dst, vec3* axis, float_t angle);
extern void mat4_mult_axis_angle(mat4* src, mat4* dst, vec3* axis, float_t angle);
