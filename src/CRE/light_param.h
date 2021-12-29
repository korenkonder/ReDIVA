/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vector.h"
#include "../KKdLib/light_param/face.h"
#include "../KKdLib/light_param/fog.h"
#include "../KKdLib/light_param/glow.h"
#include "../KKdLib/light_param/ibl.h"
#include "../KKdLib/light_param/light.h"
#include "../KKdLib/light_param/wind.h"
#include "data.h"

typedef struct light_param_data {
    int32_t id;
    light_param_light light;
    light_param_fog fog;
    light_param_glow glow;
    light_param_ibl ibl;
    light_param_wind wind;
    light_param_face face;
} light_param_data;

vector(light_param_data)

typedef struct vector_light_param_data light_param_set;

extern void light_param_data_init(light_param_data* light_param);
extern void light_param_data_load(light_param_data* light_param, data_struct* data, char* name);
extern void light_param_data_free(light_param_data* light_param);

extern void light_param_set_init(light_param_set* set);
extern void light_param_set_load(light_param_set* set, data_struct* data, int32_t pv_id);
extern void light_param_set_free(light_param_set* set);

extern char* light_param_get_string(char* name);

extern void light_param_storage_init();
extern void light_param_storage_append_light_param_data(char* name);
extern void light_param_storage_append_light_param_set(int32_t pv_id);
extern void light_param_storage_load_light_param_data(data_struct* data, char* name);
extern void light_param_storage_load_light_param_set(data_struct* data, int32_t pv_id);
extern void light_param_storage_insert_light_param_data(light_param_data* data, char* name);
extern void light_param_storage_insert_light_param_set(light_param_set* set, int32_t pv_id);
extern light_param_data* light_param_storage_get_light_param_data(char* name);
extern light_param_set* light_param_storage_get_light_param_set(int32_t pv_id);
extern void light_param_storage_delete_light_param_data(char* name);
extern void light_param_storage_delete_light_param_set(int32_t pv_id);
extern void light_param_storage_free();
