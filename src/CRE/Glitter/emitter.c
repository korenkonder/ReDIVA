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

void FASTCALL glitter_emitter_dispose(glitter_emitter* e) {
    vector_ptr_glitter_curve_clear(&e->curve, (void*)&glitter_curve_dispose);
    vector_ptr_glitter_curve_dispose(&e->curve);
    vector_ptr_glitter_particle_clear(&e->particles, (void*)&glitter_particle_dispose);
    vector_ptr_glitter_particle_dispose(&e->particles);
    free(e);
}

bool FASTCALL Glitter__Emitter__ParseFile(glitter_file_reader* a1, f2_header* a2, glitter_effect* a3) {
    glitter_emitter* emitter;
    f2_header* v10;

    if (!a2->data_size || Glitter__ParseFile__ReverseSignatureEndianess(a2) != 'EMIT')
        return false;

    emitter = glitter_emitter_init(sizeof(glitter_emitter));
    if (!Glitter__Emitter__UnpackFile(a1, Glitter__ParseFile__GetDataPointer(a2),
        emitter, Glitter__ParseFile__GetVersion(a2))) {
        glitter_emitter_dispose(emitter);
        return false;
    }

    v10 = Glitter__ParseFile__GetSubStructPointer(a2);
    if (v10) {
        while (Glitter__ParseFile__ReverseSignatureEndianess(v10) == 'ENRS') {
            v10 = Glitter__ParseFile__CheckForEOFC(v10);
            if (!v10) {
                vector_ptr_glitter_emitter_append_element(&a3->emitters, &emitter);
                return true;
            }
        }
        Glitter__Animation__ParseFile(a1, v10, &emitter->curve);
        while (v10) {
            Glitter__Particle__ParseFile(a1, v10, emitter, a3);
            v10 = Glitter__ParseFile__CheckForEOFC(v10);
        }
    }

    vector_ptr_glitter_emitter_append_element(&a3->emitters, &emitter);
    return true;
}

bool FASTCALL Glitter__Emitter__UnpackFile(glitter_file_reader* a1,
    int32_t* data, glitter_emitter* a3, uint32_t emit_version) {
    a3->data.start_time = (float_t)*data;
    a3->data.life_time = (float_t)data[1];
    a3->data.loop_start_time = (float_t)data[2];
    a3->data.loop_life_time = (float_t)data[3];
    a3->data.flags = data[4];
    a3->data.dword30 = 0;
    if (!emit_version)
        return false;

    a3->data.type = *((int16_t*)data + 10);
    a3->data.direction = *((int16_t*)data + 11);
    a3->data.emission_interval = *(float_t*)(data + 6);
    a3->data.particles_per_emission = *(float_t*)(data + 7);
    a3->data.dword2C = *((int16_t*)data + 16);
    if (emit_version >= 2)
        a3->data.dword30 = *((uint16_t*)data + 17);

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
