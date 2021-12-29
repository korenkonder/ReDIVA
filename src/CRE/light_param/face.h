/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/face.h"
#include "../../KKdLib/vec.h"
#include "../static_var.h"

typedef struct face {
    float_t offset;
    float_t scale;
    vec3 position;
    vec3 direction;
} face;

extern void face_init(face* face);
extern float_t face_get_offset(face* face);
extern void face_set_offset(face* face, float_t value);
extern float_t face_get_scale(face* face);
extern void face_set_scale(face* face, float_t value);
extern void face_get_position(face* face, vec3* value);
extern void face_set_position(face* face, vec3* value);
extern void face_get_direction(face* face, vec3* value);
extern void face_set_direction(face* face, vec3* value);
