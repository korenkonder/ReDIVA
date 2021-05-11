/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter.h"
#include "animation.h"
#include "curve.h"
#include "particle.h"

static bool FASTCALL glitter_emitter_pack_file(GPM, f2_struct* st, glitter_emitter* a2);
static bool FASTCALL glitter_emitter_unpack_file(GPM, int32_t* data, glitter_emitter* a2, bool use_big_endian);

glitter_emitter* FASTCALL glitter_emitter_init(GPM) {
    glitter_emitter* e = force_malloc(sizeof(glitter_emitter));
    e->version = glt_type == GLITTER_X ? 0x04 : 0x02;
    return e;
}

glitter_emitter* FASTCALL glitter_emitter_copy(GPM, glitter_emitter* e) {
    if (!e)
        return 0;

    glitter_emitter* ec = force_malloc(sizeof(glitter_emitter));
    *ec = *e;

    ec->particles = (vector_ptr_glitter_particle){ 0, 0, 0 };
    vector_ptr_glitter_particle_append(&ec->particles, e->particles.end - e->particles.begin);
    for (glitter_particle** i = e->particles.begin; i != e->particles.end; i++)
        if (*i) {
            glitter_particle* p = glitter_particle_copy(GPM_VAL, *i);
            if (p)
                vector_ptr_glitter_particle_push_back(&ec->particles, &p);
        }

    ec->curve = (vector_ptr_glitter_curve){ 0, 0, 0 };
    glitter_animation_copy(GPM_VAL, &e->curve, &ec->curve);
    return ec;
}

bool FASTCALL glitter_emitter_parse_file(GPM, glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_emitter* vec, glitter_effect* effect) {
    f2_struct* i;
    glitter_emitter* emitter;

    if (!st || !st->header.data_size)
        return false;

    emitter = glitter_emitter_init(GPM_VAL);
    emitter->version = st->header.version;
    if (!glitter_emitter_unpack_file(GPM_VAL, st->data, emitter, st->header.use_big_endian)) {
        glitter_emitter_dispose(emitter);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianess_uint32_t('ANIM'))
            glitter_animation_parse_file(GPM_VAL, i, &emitter->curve, glitter_emitter_curve_flags);
        else if (i->header.signature == reverse_endianess_uint32_t('PTCL'))
            glitter_particle_parse_file(GPM_VAL, a1, i, &emitter->particles, effect);
    }
    vector_ptr_glitter_emitter_push_back(vec, &emitter);
    return true;
}

bool FASTCALL glitter_emitter_unparse_file(GPM, glitter_effect_group* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect) {
    if (!glitter_emitter_pack_file(GPM_VAL, st, a3))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(GPM_VAL, &s, &a3->curve, glitter_emitter_curve_flags))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    for (glitter_particle** i = a3->particles.begin; i != a3->particles.end; i++) {
        if (!*i)
            continue;

        f2_struct s;
        if (glitter_particle_unparse_file(GPM_VAL, a1, &s, *i, effect))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    }
    return true;
}

void FASTCALL glitter_emitter_dispose(glitter_emitter* e) {
    vector_ptr_glitter_curve_free(&e->curve, glitter_curve_dispose);
    vector_ptr_glitter_particle_free(&e->particles, glitter_particle_dispose);
    free(e);
}

static bool FASTCALL glitter_emitter_pack_file(GPM, f2_struct* st, glitter_emitter* a2) {
    size_t l;
    size_t d;

    if (a2->version != 1 && a2->version != 2)
        return false;

    switch (a2->data.type) {
    case GLITTER_EMITTER_BOX:
    case GLITTER_EMITTER_CYLINDER:
    case GLITTER_EMITTER_SPHERE:
    case GLITTER_EMITTER_POLYGON:
        break;
    default:
        return false;
    }

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 5, 96, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 5, ENRS_TYPE_DWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 15, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 96;

    switch (a2->data.type) {
    case GLITTER_EMITTER_BOX:
        ee = (enrs_entry){ o, 1, 12, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 12;
        break;
    case GLITTER_EMITTER_CYLINDER:
        ee = (enrs_entry){ o, 1, 20, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 5, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 20;
        break;
    case GLITTER_EMITTER_SPHERE:
        ee = (enrs_entry){ o, 1, 16, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 16;
        break;
    case GLITTER_EMITTER_POLYGON:
        ee = (enrs_entry){ o, 1, 8, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 8;
        break;
    }

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->length = l;
    st->data = (void*)d;
    st->enrs = e;

    *(int32_t*)d = a2->data.start_time;
    *(int32_t*)(d + 4) = a2->data.life_time;
    *(int32_t*)(d + 8) = a2->data.loop_start_time;
    *(int32_t*)(d + 12) = a2->data.loop_end_time;
    *(int32_t*)(d + 16) = a2->data.flags;
    *(int16_t*)(d + 20) = (int16_t)a2->data.type;
    *(int16_t*)(d + 22) = (int16_t)a2->data.direction;
    *(float_t*)(d + 24) = a2->data.emission_interval;
    *(float_t*)(d + 28) = a2->data.particles_per_emission;
    *(int16_t*)(d + 32) = 0;
    *(int16_t*)(d + 34) = 0;

    *(vec3*)(d + 36) = a2->translation;
    *(vec3*)(d + 48) = a2->rotation;
    *(vec3*)(d + 60) = a2->scale;
    *(vec3*)(d + 72) = a2->data.rotation_add;
    *(vec3*)(d + 84) = a2->data.rotation_add_random;
    d += 96;

    switch (a2->data.type) {
    case GLITTER_EMITTER_BOX:
        *(vec3*)d = a2->data.box.size;
        break;
    case GLITTER_EMITTER_CYLINDER:
        *(float_t*)d = a2->data.cylinder.radius;
        *(float_t*)(d + 4) = a2->data.cylinder.height;
        *(float_t*)(d + 8) = a2->data.cylinder.start_angle;
        *(float_t*)(d + 12) = a2->data.cylinder.end_angle;
        *(int32_t*)(d + 16) = ((int32_t)a2->data.cylinder.direction << 1)
            | (a2->data.cylinder.on_edge ? 1 : 0);
        break;
    case GLITTER_EMITTER_SPHERE:
        *(float_t*)d = a2->data.sphere.radius;
        *(float_t*)(d + 4) = a2->data.sphere.latitude;
        *(float_t*)(d + 8) = a2->data.sphere.longitude;
        *(int32_t*)(d + 12) = ((int32_t)a2->data.sphere.direction << 1)
            | (a2->data.sphere.on_edge ? 1 : 0);
        break;
    case GLITTER_EMITTER_POLYGON:
        *(float_t*)d = a2->data.polygon.size;
        *(int32_t*)(d + 4) = a2->data.polygon.count;
        break;
    }

    st->header.signature = reverse_endianess_uint32_t('EMIT');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a2->version;
    return true;
}

static bool FASTCALL glitter_emitter_unpack_file(GPM,
    int32_t* data, glitter_emitter* a2, bool use_big_endian) {
    if (glt_type == GLITTER_X) {
        if (use_big_endian) {
            a2->data.start_time = reverse_endianess_int32_t(data[0]);
            a2->data.life_time = reverse_endianess_int32_t(data[1]);
            a2->data.loop_start_time = reverse_endianess_int32_t(data[2]);
            a2->data.loop_end_time = reverse_endianess_int32_t(data[3]);
            a2->data.flags = reverse_endianess_int32_t(data[4]);
        }
        else {
            a2->data.start_time = data[0];
            a2->data.life_time = data[1];
            a2->data.loop_start_time = data[2];
            a2->data.loop_end_time = data[3];
            a2->data.flags = data[4];
        }

        if (a2->version != 3 && a2->version != 4)
            return false;

        if (use_big_endian) {
            a2->data.type = reverse_endianess_int16_t(*((int16_t*)data + 10));
            a2->data.direction = reverse_endianess_int16_t(*((int16_t*)data + 11));
            a2->data.emission_interval = reverse_endianess_float_t(*(float_t*)&data[6]);
            a2->data.particles_per_emission = reverse_endianess_float_t(*(float_t*)&data[7]);
            a2->data.timer = reverse_endianess_int16_t(*((int16_t*)data + 16));
            a2->data.seed = reverse_endianess_int32_t(data[9]);
        }
        else {
            a2->data.type = *((int16_t*)data + 10);
            a2->data.direction = *((int16_t*)data + 11);
            a2->data.emission_interval = *(float_t*)&data[6];
            a2->data.particles_per_emission = *(float_t*)&data[7];
            a2->data.timer = *((int16_t*)data + 16);
            a2->data.seed = data[9];
        }
        data += a2->version == 3 ? 14 : 13;

        if (use_big_endian) {
            a2->translation.x = reverse_endianess_float_t(*(float_t*)&data[0]);
            a2->translation.y = reverse_endianess_float_t(*(float_t*)&data[1]);
            a2->translation.z = reverse_endianess_float_t(*(float_t*)&data[2]);
            a2->rotation.x = reverse_endianess_float_t(*(float_t*)&data[3]);
            a2->rotation.y = reverse_endianess_float_t(*(float_t*)&data[4]);
            a2->rotation.z = reverse_endianess_float_t(*(float_t*)&data[5]);
            a2->scale.x = reverse_endianess_float_t(*(float_t*)&data[6]);
            a2->scale.y = reverse_endianess_float_t(*(float_t*)&data[7]);
            a2->scale.z = reverse_endianess_float_t(*(float_t*)&data[8]);
            a2->data.rotation_add.x = reverse_endianess_float_t(*(float_t*)&data[9]);
            a2->data.rotation_add.y = reverse_endianess_float_t(*(float_t*)&data[10]);
            a2->data.rotation_add.z = reverse_endianess_float_t(*(float_t*)&data[11]);
            a2->data.rotation_add_random.x = reverse_endianess_float_t(*(float_t*)&data[12]);
            a2->data.rotation_add_random.y = reverse_endianess_float_t(*(float_t*)&data[13]);
            a2->data.rotation_add_random.z = reverse_endianess_float_t(*(float_t*)&data[14]);
        }
        else {
            a2->translation = *(vec3*)&data[0];
            a2->rotation = *(vec3*)&data[3];
            a2->scale = *(vec3*)&data[6];
            a2->data.rotation_add = *(vec3*)&data[9];
            a2->data.rotation_add_random = *(vec3*)&data[12];
        }
        data += 15;

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
                a2->data.cylinder.on_edge = reverse_endianess_int32_t(data[4]) & 1 ? true : false;
                a2->data.cylinder.direction = reverse_endianess_int32_t(data[4]) >> 1;
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = reverse_endianess_float_t(*(float_t*)&data[0]);
                a2->data.sphere.latitude = reverse_endianess_float_t(*(float_t*)&data[1]);
                a2->data.sphere.longitude = reverse_endianess_float_t(*(float_t*)&data[2]);
                a2->data.sphere.on_edge = reverse_endianess_int32_t(data[3]) & 1 ? true : false;
                a2->data.sphere.direction = reverse_endianess_int32_t(data[3]) >> 1;
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = reverse_endianess_float_t(*(float_t*)&data[0]);
                a2->data.polygon.count = reverse_endianess_int32_t(data[1]);
                a2->data.polygon.direction = reverse_endianess_int32_t(data[2]) >> 1;
                return true;
            default:
                return false;
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
                a2->data.cylinder.on_edge = data[4] & 1 ? true : false;
                a2->data.cylinder.direction = data[4] >> 1;
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = *(float_t*)&data[0];
                a2->data.sphere.latitude = *(float_t*)&data[1];
                a2->data.sphere.longitude = *(float_t*)&data[2];
                a2->data.sphere.on_edge = data[3] & 1 ? true : false;
                a2->data.sphere.direction = data[3] >> 1;
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = *(float_t*)&data[0];
                a2->data.polygon.count = data[1];
                a2->data.polygon.direction = data[2] >> 1;
                return true;
            default:
                return false;
            }
    }
    else {
        if (use_big_endian) {
            a2->data.start_time = reverse_endianess_int32_t(data[0]);
            a2->data.life_time = reverse_endianess_int32_t(data[1]);
            a2->data.loop_start_time = reverse_endianess_int32_t(data[2]);
            a2->data.loop_end_time = reverse_endianess_int32_t(data[3]);
            a2->data.flags = reverse_endianess_int32_t(data[4]);
        }
        else {
            a2->data.start_time = data[0];
            a2->data.life_time = data[1];
            a2->data.loop_start_time = data[2];
            a2->data.loop_end_time = data[3];
            a2->data.flags = data[4];
        }

        a2->data.timer = GLITTER_EMITTER_TIMER_BY_TIME;
        a2->data.seed = 0;
        if (a2->version != 1 && a2->version != 2)
            return false;

        if (use_big_endian) {
            a2->data.type = reverse_endianess_int16_t(*((int16_t*)data + 10));
            a2->data.direction = reverse_endianess_int16_t(*((int16_t*)data + 11));
            a2->data.emission_interval = reverse_endianess_float_t(*(float_t*)&data[6]);
            a2->data.particles_per_emission = reverse_endianess_float_t(*(float_t*)&data[7]);
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
                a2->data.cylinder.on_edge = reverse_endianess_int32_t(data[4]) & 1 ? true : false;
                a2->data.cylinder.direction = reverse_endianess_int32_t(data[4]) >> 1;
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = reverse_endianess_float_t(*(float_t*)&data[0]);
                a2->data.sphere.latitude = reverse_endianess_float_t(*(float_t*)&data[1]);
                a2->data.sphere.longitude = reverse_endianess_float_t(*(float_t*)&data[2]);
                a2->data.sphere.on_edge = reverse_endianess_int32_t(data[3]) & 1 ? true : false;
                a2->data.sphere.direction = reverse_endianess_int32_t(data[3]) >> 1;
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = reverse_endianess_float_t(*(float_t*)&data[0]);
                a2->data.polygon.count = reverse_endianess_int32_t(data[1]);
                return true;
            default:
                return false;
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
                a2->data.cylinder.on_edge = data[4] & 1 ? true : false;
                a2->data.cylinder.direction = data[4] >> 1;
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = *(float_t*)&data[0];
                a2->data.sphere.latitude = *(float_t*)&data[1];
                a2->data.sphere.longitude = *(float_t*)&data[2];
                a2->data.sphere.on_edge = data[3] & 1 ? true : false;
                a2->data.sphere.direction = data[3] >> 1;
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = *(float_t*)&data[0];
                a2->data.polygon.count = data[1];
                return true;
            default:
                return false;
            }
    }
}
