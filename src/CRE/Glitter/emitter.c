/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter.h"
#include "animation.h"
#include "curve.h"
#include "parse_file.h"
#include "particle.h"

glitter_emitter* FASTCALL glitter_emitter_init() {
    glitter_emitter* e = force_malloc(sizeof(glitter_emitter));
    return e;
}

bool FASTCALL glitter_emitter_parse_file(glitter_file_reader* a1, f2_header* a2, glitter_effect* a3) {
    glitter_emitter* emitter;
    f2_header* v10;

    if (!a2->data_size || glitter_parse_file_reverse_signature_endianess(a2) != 'EMIT')
        return false;

    emitter = glitter_emitter_init(sizeof(glitter_emitter));
    if (!glitter_emitter_unpack_file(a1, glitter_parse_file_get_data_ptr(a2), emitter, a2->version)) {
        glitter_emitter_dispose(emitter);
        return false;
    }

    v10 = glitter_parse_file_get_sub_struct_ptr(a2);
    if (v10) {
        while (glitter_parse_file_reverse_signature_endianess(v10) == 'ENRS') {
            v10 = glitter_parse_file_check_for_end_of_container(v10);
            if (!v10) {
                vector_ptr_glitter_emitter_append_element(&a3->emitters, &emitter);
                return true;
            }
        }
        glitter_animation_parse_file(a1, v10, &emitter->curve);
        while (v10) {
            glitter_particle_parse_file(a1, v10, emitter, a3);
            v10 = glitter_parse_file_check_for_end_of_container(v10);
        }
    }

    vector_ptr_glitter_emitter_append_element(&a3->emitters, &emitter);
    return true;
}

bool FASTCALL glitter_emitter_unpack_file(glitter_file_reader* a1,
    int32_t* data, glitter_emitter* a3, uint32_t emit_version) {
    a3->data.start_time = (float_t)*data;
    a3->data.life_time = (float_t)data[1];
    a3->data.loop_start_time = (float_t)data[2];
    a3->data.loop_life_time = (float_t)data[3];
    a3->data.flags = data[4];
    //a3->data.dword30 = 0;
    if (!emit_version)
        return false;

    a3->data.type = *((int16_t*)data + 10);
    a3->data.direction = *((int16_t*)data + 11);
    a3->data.emission_interval = *(float_t*)(data + 6);
    a3->data.particles_per_emission = *(float_t*)(data + 7);
    //a3->data.dword2C = *((int16_t*)data + 16);
    /*if (emit_version >= 2)
        a3->data.dword30 = *((uint16_t*)data + 17);*/

    a3->translation = *(vec3*)(data + 9);
    a3->rotation = *(vec3*)(data + 12);
    *(vec3*)&a3->scale = *(vec3*)(data + 15);
    a3->scale.w = 1.0;
    a3->data.rotation_add = *(vec3*)(data + 18);
    a3->data.rotation_add_random = *(vec3*)(data + 21);

    data += 24;
    switch (a3->data.type) {
    case GLITTER_EMITTER_BOX:
        a3->data.data.box.size = *(vec3*)data;
        return true;
    case GLITTER_EMITTER_CYLINDER:
        a3->data.data.cylinder.radius = *(float_t*)data;
        a3->data.data.cylinder.height = *(float_t*)(data + 1);
        a3->data.data.cylinder.start_angle = *(float_t*)(data + 2);
        a3->data.data.cylinder.end_angle = *(float_t*)(data + 3);
        a3->data.data.cylinder.plain = *(int32_t*)(data + 4) & 1 ? true : false;
        a3->data.data.cylinder.direction = *(int32_t*)(data + 4) >> 1;
        return true;
    case GLITTER_EMITTER_SPHERE:
        a3->data.data.sphere.radius = *(float_t*)data;
        a3->data.data.sphere.latitude = *(float_t*)(data + 1);
        a3->data.data.sphere.longitude = *(float_t*)(data + 2);
        a3->data.data.sphere.plain = *(int32_t*)(data + 3) & 1 ? true : false;
        a3->data.data.sphere.direction = *(int32_t*)(data + 3) >> 1;
        return true;
    case GLITTER_EMITTER_POLYGON:
        a3->data.data.polygon.scale = *(float_t*)data;
        a3->data.data.polygon.count = *(int32_t*)(data + 1);
        return true;
    default:
        return false;
    }
}

void FASTCALL glitter_emitter_dispose(glitter_emitter* e) {
    vector_ptr_glitter_curve_clear(&e->curve, (void*)&glitter_curve_dispose);
    vector_ptr_glitter_curve_dispose(&e->curve);
    vector_ptr_glitter_particle_clear(&e->particles, (void*)&glitter_particle_dispose);
    vector_ptr_glitter_particle_dispose(&e->particles);
    free(e);
}
