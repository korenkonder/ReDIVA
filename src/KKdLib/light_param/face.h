/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

typedef struct light_param_face {
    bool ready;

    float_t offset;
    float_t scale;
    vec3 position;
    vec3 direction;
} light_param_face;

extern void light_param_face_init(light_param_face* face);
extern void light_param_face_read(light_param_face* face, char* path);
extern void light_param_face_wread(light_param_face* face, wchar_t* path);
extern void light_param_face_mread(light_param_face* face, void* data, size_t length);
extern void light_param_face_write(light_param_face* face, char* path);
extern void light_param_face_wwrite(light_param_face* face, wchar_t* path);
extern void light_param_face_mwrite(light_param_face* face, void** data, size_t* length);
extern bool light_param_face_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_face_free(light_param_face* face);
