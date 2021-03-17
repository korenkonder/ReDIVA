/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter.h"
#include "animation.h"
#include "curve.h"
#include "particle.h"

static bool FASTCALL glitter_emitter_pack_file(f2_struct* st, glitter_emitter* a2, bool use_big_endian);
static bool FASTCALL glitter_emitter_unpack_file(int32_t* data, glitter_emitter* a2, bool use_big_endian);

glitter_emitter* FASTCALL glitter_emitter_init() {
    glitter_emitter* e = force_malloc(sizeof(glitter_emitter));
    return e;
}

bool FASTCALL glitter_emitter_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_emitter* vec, glitter_effect* effect) {
    f2_struct* i;
    glitter_emitter* emitter;

    if (!st || !st->header.data_size)
        return false;

    emitter = glitter_emitter_init();
    emitter->version = st->header.version;
    if (!glitter_emitter_unpack_file(st->data, emitter, st->header.use_big_endian)) {
        glitter_emitter_dispose(emitter);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x4D494E41)
            glitter_animation_parse_file(i, &emitter->curve);
        else if (i->header.signature == 0x4C435450)
            glitter_particle_parse_file(a1, i, &emitter->particles, effect);
    }
    vector_ptr_glitter_emitter_push_back(vec, &emitter);
    return true;
}

bool FASTCALL glitter_emitter_unparse_file(glitter_effect_group* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect, bool use_big_endian) {
    if (!glitter_emitter_pack_file(st, a3, use_big_endian))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(&s, &a3->curve, use_big_endian))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    for (glitter_particle** i = a3->particles.begin; i != a3->particles.end; i++)
        if (*i && glitter_particle_unparse_file(a1, &s, *i, effect, use_big_endian))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    return true;
}

void FASTCALL glitter_emitter_dispose(glitter_emitter* e) {
    vector_ptr_glitter_curve_free(&e->curve, (void*)glitter_curve_dispose);
    vector_ptr_glitter_particle_free(&e->particles, (void*)glitter_particle_dispose);
    free(e);
}

static bool FASTCALL glitter_emitter_pack_file(f2_struct* st, glitter_emitter* a2, bool use_big_endian) {
    size_t l;
    int32_t* d;

    if (!a2->version)
        return false;

    memset(st, 0, sizeof(f2_struct));
    l = 96;

    switch (a2->data.type) {
    case GLITTER_EMITTER_BOX:
        l += 12;
        break;
    case GLITTER_EMITTER_CYLINDER:
        l += 20;
        break;
    case GLITTER_EMITTER_SPHERE:
        l += 16;
        break;
    case GLITTER_EMITTER_POLYGON:
        l += 8;
        break;
    default:
        return false;
    }

    l = align_val(l, 0x10);
    d = force_malloc(l);
    st->length = l;
    st->data = d;

    if (use_big_endian) {
        d[0] = reverse_endianess_int32_t((int32_t)roundf(a2->data.start_time));
        d[1] = reverse_endianess_int32_t((int32_t)roundf(a2->data.life_time));
        d[2] = reverse_endianess_int32_t((int32_t)roundf(a2->data.loop_start_time));
        d[3] = reverse_endianess_int32_t((int32_t)roundf(a2->data.loop_end_time));
        d[4] = reverse_endianess_int32_t(a2->data.flags);
        *((int16_t*)d + 10) = reverse_endianess_int16_t((int16_t)a2->data.type);
        *((int16_t*)d + 11) = reverse_endianess_int16_t((int16_t)a2->data.direction);
        *(float_t*)&d[6] = reverse_endianess_float_t(a2->data.emission_interval);
        *(float_t*)&d[7] = reverse_endianess_float_t(a2->data.particles_per_emission);
        //*((int16_t*)data + 16) = reverse_endianess_int16_t((int16_t)a3->data.dword2C);
        /*if (a2->version > 1)
            *((int16_t*)data + 17) = reverse_endianess_uint16_t((int16_t)a3->data.dword30);*/

        *(float_t*)&d[9] = reverse_endianess_float_t(a2->translation.x);
        *(float_t*)&d[10] = reverse_endianess_float_t(a2->translation.y);
        *(float_t*)&d[11] = reverse_endianess_float_t(a2->translation.z);
        *(float_t*)&d[12] = reverse_endianess_float_t(a2->rotation.x);
        *(float_t*)&d[13] = reverse_endianess_float_t(a2->rotation.y);
        *(float_t*)&d[14] = reverse_endianess_float_t(a2->rotation.z);
        *(float_t*)&d[15] = reverse_endianess_float_t(a2->scale.x);
        *(float_t*)&d[16] = reverse_endianess_float_t(a2->scale.y);
        *(float_t*)&d[17] = reverse_endianess_float_t(a2->scale.z);
        *(float_t*)&d[18] = reverse_endianess_float_t(a2->data.rotation_add.x);
        *(float_t*)&d[19] = reverse_endianess_float_t(a2->data.rotation_add.y);
        *(float_t*)&d[20] = reverse_endianess_float_t(a2->data.rotation_add.z);
        *(float_t*)&d[21] = reverse_endianess_float_t(a2->data.rotation_add_random.x);
        *(float_t*)&d[22] = reverse_endianess_float_t(a2->data.rotation_add_random.y);
        *(float_t*)&d[23] = reverse_endianess_float_t(a2->data.rotation_add_random.z);
    }
    else {
        d[0] = (int32_t)roundf(a2->data.start_time);
        d[1] = (int32_t)roundf(a2->data.life_time);
        d[2] = (int32_t)roundf(a2->data.loop_start_time);
        d[3] = (int32_t)roundf(a2->data.loop_end_time);
        d[4] = a2->data.flags;
        *((int16_t*)d + 10) = (int16_t)a2->data.type;
        *((int16_t*)d + 11) = (int16_t)a2->data.direction;
        *(float_t*)&d[6] = a2->data.emission_interval;
        *(float_t*)&d[7] = a2->data.particles_per_emission;
        //*((int16_t*)data + 16) = (int16_t)a3->data.dword2C;
        /*if (a2->version > 1)
            *((int16_t*)data + 17) = (int16_t)a3->data.dword30;*/

        *(vec3*)&d[9] = a2->translation;
        *(vec3*)&d[12] = a2->rotation;
        *(vec3*)&d[15] = a2->scale;
        *(vec3*)&d[18] = a2->data.rotation_add;
        *(vec3*)&d[21] = a2->data.rotation_add_random;
    }
    d += 24;

    if (use_big_endian)
        switch (a2->data.type) {
        case GLITTER_EMITTER_BOX:
            *(float_t*)&d[0] = reverse_endianess_float_t(a2->data.box.size.x);
            *(float_t*)&d[1] = reverse_endianess_float_t(a2->data.box.size.y);
            *(float_t*)&d[2] = reverse_endianess_float_t(a2->data.box.size.z);
            break;
        case GLITTER_EMITTER_CYLINDER:
            *(float_t*)&d[0] = reverse_endianess_float_t(a2->data.cylinder.radius);
            *(float_t*)&d[1] = reverse_endianess_float_t(a2->data.cylinder.height);
            *(float_t*)&d[2] = reverse_endianess_float_t(a2->data.cylinder.start_angle);
            *(float_t*)&d[3] = reverse_endianess_float_t(a2->data.cylinder.end_angle);
            d[4] = reverse_endianess_int32_t(((int32_t)a2->data.cylinder.direction << 1)
                | (a2->data.cylinder.plain ? 1 : 0));
            break;
        case GLITTER_EMITTER_SPHERE:
            *(float_t*)&d[0] = reverse_endianess_float_t(a2->data.sphere.radius);
            *(float_t*)&d[1] = reverse_endianess_float_t(a2->data.sphere.latitude);
            *(float_t*)&d[2] = reverse_endianess_float_t(a2->data.sphere.longitude);
            d[3] = reverse_endianess_int32_t(((int32_t)a2->data.cylinder.direction << 1)
                | (a2->data.cylinder.plain ? 1 : 0));
            break;
        case GLITTER_EMITTER_POLYGON:
            *(float_t*)&d[0] = reverse_endianess_float_t(a2->data.polygon.scale);
            d[1] = reverse_endianess_int32_t(a2->data.polygon.count);
            break;
        }
    else
        switch (a2->data.type) {
        case GLITTER_EMITTER_BOX:
            *(vec3*)&d[0] = a2->data.box.size;
            break;
        case GLITTER_EMITTER_CYLINDER:
            *(float_t*)&d[0] = a2->data.cylinder.radius;
            *(float_t*)&d[1] = a2->data.cylinder.height;
            *(float_t*)&d[2] = a2->data.cylinder.start_angle;
            *(float_t*)&d[3] = a2->data.cylinder.end_angle;
            d[4] = ((int32_t)a2->data.cylinder.direction << 1)
                | (a2->data.cylinder.plain ? 1 : 0);
            break;
        case GLITTER_EMITTER_SPHERE:
            *(float_t*)&d[0] = a2->data.sphere.radius;
            *(float_t*)&d[1] = a2->data.sphere.latitude;
            *(float_t*)&d[2] = a2->data.sphere.longitude;
            d[3] = ((int32_t)a2->data.cylinder.direction << 1)
                | (a2->data.cylinder.plain ? 1 : 0);
            break;
        case GLITTER_EMITTER_POLYGON:
            *(float_t*)&d[0] = a2->data.polygon.scale;
            d[1] = a2->data.polygon.count;
            break;
        }

    st->header.signature = 0x54494D45;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    st->header.version = a2->version;
    return true;
}

static bool FASTCALL glitter_emitter_unpack_file(int32_t* data, glitter_emitter* a2, bool use_big_endian) {
    if (use_big_endian) {
        a2->data.start_time = (float_t)reverse_endianess_int32_t(data[0]);
        a2->data.life_time = (float_t)reverse_endianess_int32_t(data[1]);
        a2->data.loop_start_time = (float_t)reverse_endianess_int32_t(data[2]);
        a2->data.loop_end_time = (float_t)reverse_endianess_int32_t(data[3]);
        a2->data.flags = reverse_endianess_int32_t(data[4]);
    }
    else {
        a2->data.start_time = (float_t)data[0];
        a2->data.life_time = (float_t)data[1];
        a2->data.loop_start_time = (float_t)data[2];
        a2->data.loop_end_time = (float_t)data[3];
        a2->data.flags = data[4];
    }

    //a3->data.dword30 = 0;
    if (!a2->version)
        return false;

    if (use_big_endian) {
        a2->data.type = reverse_endianess_int16_t(*((int16_t*)data + 10));
        a2->data.direction = reverse_endianess_int16_t(*((int16_t*)data + 11));
        a2->data.emission_interval = reverse_endianess_float_t(*(float_t*)&data[6]);
        a2->data.particles_per_emission = reverse_endianess_float_t(*(float_t*)&data[7]);
        //a3->data.dword2C = reverse_endianess_int16_t(*((int16_t*)data + 16));
        /*if (a2->version > 1)
            a3->data.dword30 = reverse_endianess_uint16_t(*((uint16_t*)data + 17));*/

        a2->translation.x = reverse_endianess_float_t(*(float_t*)&data[9]);
        a2->translation.y = reverse_endianess_float_t(*(float_t*)&data[10]);
        a2->translation.z = reverse_endianess_float_t(*(float_t*)&data[11]);
        a2->rotation.x = reverse_endianess_float_t(*(float_t*)&data[12]);
        a2->rotation.y = reverse_endianess_float_t(*(float_t*)&data[13]);
        a2->rotation.z = reverse_endianess_float_t(*(float_t*)&data[14]);
        a2->scale.x = reverse_endianess_float_t(*(float_t*)&data[15]);
        a2->scale.y = reverse_endianess_float_t(*(float_t*)&data[16]);
        a2->scale.z = reverse_endianess_float_t(*(float_t*)&data[17]);
        a2->data.rotation_add.x = reverse_endianess_float_t(*(float_t*)&data[18]);
        a2->data.rotation_add.y = reverse_endianess_float_t(*(float_t*)&data[19]);
        a2->data.rotation_add.z = reverse_endianess_float_t(*(float_t*)&data[20]);
        a2->data.rotation_add_random.x = reverse_endianess_float_t(*(float_t*)&data[21]);
        a2->data.rotation_add_random.y = reverse_endianess_float_t(*(float_t*)&data[22]);
        a2->data.rotation_add_random.z = reverse_endianess_float_t(*(float_t*)&data[23]);
    }
    else {
        a2->data.type = *((int16_t*)data + 10);
        a2->data.direction = *((int16_t*)data + 11);
        a2->data.emission_interval = *(float_t*)&data[6];
        a2->data.particles_per_emission = *(float_t*)&data[7];
        //a3->data.dword2C = *((int16_t*)data + 16);
        /*if (emit_version > 1)
            a3->data.dword30 = *((uint16_t*)data + 17);*/

        a2->translation = *(vec3*)&data[9];
        a2->rotation = *(vec3*)&data[12];
        a2->scale = *(vec3*)&data[15];
        a2->data.rotation_add = *(vec3*)&data[18];
        a2->data.rotation_add_random = *(vec3*)&data[21];
    }
    data += 24;

    if (use_big_endian)
        switch (a2->data.type) {
        case GLITTER_EMITTER_BOX:
            a2->data.box.size.x = reverse_endianess_float_t(*(float_t*)&data[0]);
            a2->data.box.size.y = reverse_endianess_float_t(*(float_t*)&data[1]);
            a2->data.box.size.z = reverse_endianess_float_t(*(float_t*)&data[2]);
            return true;
        case GLITTER_EMITTER_CYLINDER:
            a2->data.cylinder.radius = reverse_endianess_float_t(*(float_t*)&data[0]);
            a2->data.cylinder.height = reverse_endianess_float_t(*(float_t*)&data[1]);
            a2->data.cylinder.start_angle = reverse_endianess_float_t(*(float_t*)&data[2]);
            a2->data.cylinder.end_angle = reverse_endianess_float_t(*(float_t*)&data[3]);
            a2->data.cylinder.plain = reverse_endianess_int32_t(data[4]) & 1 ? true : false;
            a2->data.cylinder.direction = reverse_endianess_int32_t(data[4]) >> 1;
            return true;
        case GLITTER_EMITTER_SPHERE:
            a2->data.sphere.radius = reverse_endianess_float_t(*(float_t*)&data[0]);
            a2->data.sphere.latitude = reverse_endianess_float_t(*(float_t*)&data[1]);
            a2->data.sphere.longitude = reverse_endianess_float_t(*(float_t*)&data[2]);
            a2->data.sphere.plain = reverse_endianess_int32_t(data[3]) & 1 ? true : false;
            a2->data.sphere.direction = reverse_endianess_int32_t(data[3]) >> 1;
            return true;
        case GLITTER_EMITTER_POLYGON:
            a2->data.polygon.scale = reverse_endianess_float_t(*(float_t*)&data[0]);
            a2->data.polygon.count = reverse_endianess_int32_t(data[1]);
            return true;
        }
    else
        switch (a2->data.type) {
        case GLITTER_EMITTER_BOX:
            a2->data.box.size = *(vec3*)&data[0];
            return true;
        case GLITTER_EMITTER_CYLINDER:
            a2->data.cylinder.radius = *(float_t*)&data[0];
            a2->data.cylinder.height = *(float_t*)&data[1];
            a2->data.cylinder.start_angle = *(float_t*)&data[2];
            a2->data.cylinder.end_angle = *(float_t*)&data[3];
            a2->data.cylinder.plain = data[4] & 1 ? true : false;
            a2->data.cylinder.direction = data[4] >> 1;
            return true;
        case GLITTER_EMITTER_SPHERE:
            a2->data.sphere.radius = *(float_t*)&data[0];
            a2->data.sphere.latitude = *(float_t*)&data[1];
            a2->data.sphere.longitude = *(float_t*)&data[2];
            a2->data.sphere.plain = data[3] & 1 ? true : false;
            a2->data.sphere.direction = data[3] >> 1;
            return true;
        case GLITTER_EMITTER_POLYGON:
            a2->data.polygon.scale = *(float_t*)&data[0];
            a2->data.polygon.count = data[1];
            return true;
        }
    return false;
}
