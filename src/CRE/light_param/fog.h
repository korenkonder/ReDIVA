/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/fog.h"
#include "../../KKdLib/vec.h"
#include "../static_var.h"

typedef struct fog {
    fog_type type;
    float_t density;
    float_t start;
    float_t end;
    int32_t index;
    vec4 color;
} fog;

extern void fog_init(fog* fog);
extern fog_type fog_get_type(fog* fog);
extern void fog_set_type(fog* fog, fog_type value);
extern float_t fog_get_density(fog* fog);
extern void fog_set_density(fog* fog, float_t value);
extern float_t fog_get_start(fog* fog);
extern void fog_set_start(fog* fog, float_t value);
extern float_t fog_get_end(fog* fog);
extern void fog_set_end(fog* fog, float_t value);
extern int32_t fog_get_index(fog* fog);
extern void fog_set_index(fog* fog, int32_t value);
extern void fog_get_color(fog* fog, vec4* value);
extern void fog_set_color(fog* fog, vec4* value);
extern void fog_data_set(fog* fog, fog_id id);
