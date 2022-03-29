/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter.h"
#include "animation.h"
#include "curve.h"
#include "particle.h"

static bool glitter_emitter_pack_file(GLT, f2_struct* st, glitter_emitter* a2);
static bool glitter_emitter_unpack_file(GLT, void* data, glitter_emitter* a2, bool use_big_endian);

glitter_emitter::glitter_emitter(GLT) : translation(), rotation(), data() {
    version = GLT_VAL == GLITTER_X ? 0x04 : 0x02;
    scale = vec3_identity;
    data.loop_start_time = 0;
    data.loop_end_time = -1;
    data.particles_per_emission = 1;
    data.direction = GLITTER_DIRECTION_EFFECT_ROTATION;
}

glitter_emitter::~glitter_emitter() {
    for (glitter_particle*& i : particles)
        delete i;
}

bool glitter_emitter_parse_file(GlitterEffectGroup* a1,
    f2_struct* st, std::vector<glitter_emitter*>* vec, glitter_effect* effect) {
    f2_struct* i;
    glitter_emitter* emitter;

    if (!st || !st->header.data_size)
        return false;

    emitter = new glitter_emitter(a1->type);
    emitter->version = st->header.version;
    if (!glitter_emitter_unpack_file(a1->type, st->data, emitter, st->header.use_big_endian)) {
        delete emitter;
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('ANIM'))
            glitter_animation_parse_file(a1->type, i, &emitter->animation, glitter_emitter_curve_flags);
        else if (i->header.signature == reverse_endianness_uint32_t('PTCL'))
            glitter_particle_parse_file(a1, i, &emitter->particles, effect);
    }
    vec->push_back(emitter);
    return true;
}

bool glitter_emitter_unparse_file(GLT, GlitterEffectGroup* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect) {
    if (!glitter_emitter_pack_file(GLT_VAL, st, a3))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, glitter_emitter_curve_flags))
        vector_old_f2_struct_push_back(&st->sub_structs, &s);

    for (glitter_particle*& i : a3->particles) {
        if (!i)
            continue;

        f2_struct s;
        if (glitter_particle_unparse_file(GLT_VAL, a1, &s, i, effect))
            vector_old_f2_struct_push_back(&st->sub_structs, &s);
    }
    return true;
}

static bool glitter_emitter_pack_file(GLT, f2_struct* st, glitter_emitter* a2) {
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
    vector_old_enrs_entry e = vector_old_empty(enrs_entry);
    enrs_entry ee;

    ee = { 0, 5, 96, 1, vector_old_empty(enrs_sub_entry) };
    vector_old_enrs_sub_entry_append(&ee.sub, 0, 5, ENRS_DWORD);
    vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_WORD);
    vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_DWORD);
    vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_WORD);
    vector_old_enrs_sub_entry_append(&ee.sub, 0, 15, ENRS_DWORD);
    vector_old_enrs_entry_push_back(&e, &ee);
    l += o = 96;

    switch (a2->data.type) {
    case GLITTER_EMITTER_BOX:
        ee = { o, 1, 12, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 3, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        l += o = 12;
        break;
    case GLITTER_EMITTER_CYLINDER:
        ee = { o, 1, 20, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 5, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        l += o = 20;
        break;
    case GLITTER_EMITTER_SPHERE:
        ee = { o, 1, 16, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 4, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        l += o = 16;
        break;
    case GLITTER_EMITTER_POLYGON:
        ee = { o, 1, 8, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
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

    st->header.signature = reverse_endianness_uint32_t('EMIT');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a2->version;
    return true;
}

static bool glitter_emitter_unpack_file(GLT,
    void* data, glitter_emitter* a2, bool use_big_endian) {
    size_t d;

    d = (size_t)data;
    if (GLT_VAL == GLITTER_X) {
        if (use_big_endian) {
            a2->data.start_time = load_reverse_endianness_int32_t((void*)d);
            a2->data.life_time = load_reverse_endianness_int32_t((void*)(d + 4));
            a2->data.loop_start_time = load_reverse_endianness_int32_t((void*)(d + 8));
            a2->data.loop_end_time = load_reverse_endianness_int32_t((void*)(d + 12));
            a2->data.flags = (glitter_emitter_flag)
                load_reverse_endianness_int32_t((void*)(d + 16));
        }
        else {
            a2->data.start_time = *(int32_t*)d;
            a2->data.life_time = *(int32_t*)(d + 4);
            a2->data.loop_start_time = *(int32_t*)(d + 8);
            a2->data.loop_end_time = *(int32_t*)(d + 12);
            a2->data.flags = (glitter_emitter_flag) * (int32_t*)(d + 16);
        }
        d += 20;

        if (a2->version != 3 && a2->version != 4)
            return false;

        if (use_big_endian) {
            a2->data.type = (glitter_emitter_type)
                load_reverse_endianness_int16_t((void*)d);
            a2->data.direction = (glitter_direction)
                load_reverse_endianness_int16_t((void*)(d + 2));
            a2->data.emission_interval = load_reverse_endianness_float_t((void*)(d + 4));
            a2->data.particles_per_emission = load_reverse_endianness_float_t((void*)(d + 8));
            a2->data.timer = (glitter_emitter_timer_type)
                load_reverse_endianness_int16_t((void*)(d + 12));
            a2->data.seed = load_reverse_endianness_int32_t((void*)(d + 16));
        }
        else {
            a2->data.type = (glitter_emitter_type) * (int16_t*)d;
            a2->data.direction = (glitter_direction) * (int16_t*)(d + 2);
            a2->data.emission_interval = *(float_t*)(d + 4);
            a2->data.particles_per_emission = *(float_t*)(d + 8);
            a2->data.timer = (glitter_emitter_timer_type) * (int16_t*)(d + 12);
            a2->data.seed = *(int32_t*)(d + 16);
        }
        d += a2->version == 3 ? 36 : 32;

        if (use_big_endian) {
            a2->translation.x = load_reverse_endianness_float_t((void*)d);
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 4));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 8));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 12));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 16));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 20));
            a2->scale.x = load_reverse_endianness_float_t((void*)(d + 24));
            a2->scale.y = load_reverse_endianness_float_t((void*)(d + 28));
            a2->scale.z = load_reverse_endianness_float_t((void*)(d + 32));
            a2->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 36));
            a2->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 40));
            a2->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 44));
            a2->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 48));
            a2->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 52));
            a2->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 56));
        }
        else {
            a2->translation = *(vec3*)d;
            a2->rotation = *(vec3*)(d + 12);
            a2->scale = *(vec3*)(d + 24);
            a2->data.rotation_add = *(vec3*)(d + 36);
            a2->data.rotation_add_random = *(vec3*)(d + 48);
        }
        d += 60;

        if (use_big_endian)
            switch (a2->data.type) {
            case GLITTER_EMITTER_BOX:
                a2->data.box.size.x = load_reverse_endianness_float_t((void*)d);
                a2->data.box.size.y = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.box.size.z = load_reverse_endianness_float_t((void*)(d + 8));
                return true;
            case GLITTER_EMITTER_CYLINDER:
                a2->data.cylinder.radius = load_reverse_endianness_float_t((void*)d);
                a2->data.cylinder.height = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.cylinder.start_angle = load_reverse_endianness_float_t((void*)(d + 8));
                a2->data.cylinder.end_angle = load_reverse_endianness_float_t((void*)(d + 12));
                a2->data.cylinder.on_edge = load_reverse_endianness_int32_t((void*)(d + 16)) & 1 ? true : false;
                a2->data.cylinder.direction = (glitter_emitter_emission_direction)
                    (load_reverse_endianness_int32_t((void*)(d + 16)) >> 1);
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = load_reverse_endianness_float_t((void*)d);
                a2->data.sphere.latitude = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.sphere.longitude = load_reverse_endianness_float_t((void*)(d + 8));
                a2->data.sphere.on_edge = load_reverse_endianness_int32_t((void*)(d + 12)) & 1 ? true : false;
                a2->data.sphere.direction = (glitter_emitter_emission_direction)
                    (load_reverse_endianness_int32_t((void*)(d + 12)) >> 1);
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = load_reverse_endianness_float_t((void*)d);
                a2->data.polygon.count = load_reverse_endianness_int32_t((void*)(d + 4));
                a2->data.polygon.direction = (glitter_emitter_emission_direction)
                    (load_reverse_endianness_int32_t((void*)(d + 8)) >> 1);
                return true;
            default:
                return false;
            }
        else
            switch (a2->data.type) {
            case GLITTER_EMITTER_BOX:
                a2->data.box.size = *(vec3*)d;
                return true;
            case GLITTER_EMITTER_CYLINDER:
                a2->data.cylinder.radius = *(float_t*)d;
                a2->data.cylinder.height = *(float_t*)(d + 4);
                a2->data.cylinder.start_angle = *(float_t*)(d + 8);
                a2->data.cylinder.end_angle = *(float_t*)(d + 12);
                a2->data.cylinder.on_edge = *(int32_t*)(d + 16) & 1 ? true : false;
                a2->data.cylinder.direction = (glitter_emitter_emission_direction)
                    (*(int32_t*)(d + 16) >> 1);
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = *(float_t*)d;
                a2->data.sphere.latitude = *(float_t*)(d + 4);
                a2->data.sphere.longitude = *(float_t*)(d + 8);
                a2->data.sphere.on_edge = *(int32_t*)(d + 12) & 1 ? true : false;
                a2->data.sphere.direction = (glitter_emitter_emission_direction)
                    (*(int32_t*)(d + 12) >> 1);
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = *(float_t*)d;
                a2->data.polygon.count = *(int32_t*)(d + 4);
                a2->data.polygon.direction = (glitter_emitter_emission_direction)
                    (*(int32_t*)(d + 8) >> 1);
                return true;
            default:
                return false;
            }
    }
    else {
        if (use_big_endian) {
            a2->data.start_time = load_reverse_endianness_int32_t((void*)d);
            a2->data.life_time = load_reverse_endianness_int32_t((void*)(d + 4));
            a2->data.loop_start_time = load_reverse_endianness_int32_t((void*)(d + 8));
            a2->data.loop_end_time = load_reverse_endianness_int32_t((void*)(d + 12));
            a2->data.flags = (glitter_emitter_flag)
                load_reverse_endianness_int32_t((void*)(d + 16));
        }
        else {
            a2->data.start_time = *(int32_t*)d;
            a2->data.life_time = *(int32_t*)(d + 4);
            a2->data.loop_start_time = *(int32_t*)(d + 8);
            a2->data.loop_end_time = *(int32_t*)(d + 12);
            a2->data.flags = (glitter_emitter_flag) * (int32_t*)(d + 16);
        }
        d += 20;

        a2->data.timer = GLITTER_EMITTER_TIMER_BY_TIME;
        a2->data.seed = 0;
        if (a2->version != 1 && a2->version != 2)
            return false;

        if (use_big_endian) {
            a2->data.type = (glitter_emitter_type)
                load_reverse_endianness_int16_t((void*)d);
            a2->data.direction = (glitter_direction)
                load_reverse_endianness_int16_t((void*)(d + 2));
            a2->data.emission_interval = load_reverse_endianness_float_t((void*)(d + 4));
            a2->data.particles_per_emission = load_reverse_endianness_float_t((void*)(d + 8));
            a2->translation.x = load_reverse_endianness_float_t((void*)(d + 16));
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 20));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 24));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 28));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 32));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 36));
            a2->scale.x = load_reverse_endianness_float_t((void*)(d + 40));
            a2->scale.y = load_reverse_endianness_float_t((void*)(d + 44));
            a2->scale.z = load_reverse_endianness_float_t((void*)(d + 48));
            a2->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 52));
            a2->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 56));
            a2->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 60));
            a2->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 64));
            a2->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 68));
            a2->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 72));
        }
        else {
            a2->data.type = (glitter_emitter_type) * (int16_t*)d;
            a2->data.direction = (glitter_direction) * (int16_t*)(d + 2);
            a2->data.emission_interval = *(float_t*)(d + 4);
            a2->data.particles_per_emission = *(float_t*)(d + 8);
            a2->translation = *(vec3*)(d + 16);
            a2->rotation = *(vec3*)(d + 28);
            a2->scale = *(vec3*)(d + 40);
            a2->data.rotation_add = *(vec3*)(d + 52);
            a2->data.rotation_add_random = *(vec3*)(d + 64);
        }
        d += 76;

        if (use_big_endian)
            switch (a2->data.type) {
            case GLITTER_EMITTER_BOX:
                a2->data.box.size.x = load_reverse_endianness_float_t((void*)d);
                a2->data.box.size.y = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.box.size.z = load_reverse_endianness_float_t((void*)(d + 8));
                return true;
            case GLITTER_EMITTER_CYLINDER:
                a2->data.cylinder.radius = load_reverse_endianness_float_t((void*)d);
                a2->data.cylinder.height = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.cylinder.start_angle = load_reverse_endianness_float_t((void*)(d + 8));
                a2->data.cylinder.end_angle = load_reverse_endianness_float_t((void*)(d + 12));
                a2->data.cylinder.on_edge = load_reverse_endianness_int32_t((void*)(d + 16)) & 1 ? true : false;
                a2->data.cylinder.direction = (glitter_emitter_emission_direction)
                    (load_reverse_endianness_int32_t((void*)(d + 16)) >> 1);
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = load_reverse_endianness_float_t((void*)d);
                a2->data.sphere.latitude = load_reverse_endianness_float_t((void*)(d + 4));
                a2->data.sphere.longitude = load_reverse_endianness_float_t((void*)(d + 8));
                a2->data.sphere.on_edge = load_reverse_endianness_int32_t((void*)(d + 12)) & 1 ? true : false;
                a2->data.sphere.direction = (glitter_emitter_emission_direction)
                    (load_reverse_endianness_int32_t((void*)(d + 12)) >> 1);
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = load_reverse_endianness_float_t((void*)d);
                a2->data.polygon.count = load_reverse_endianness_int32_t((void*)(d + 4));
                return true;
            default:
                return false;
            }
        else
            switch (a2->data.type) {
            case GLITTER_EMITTER_BOX:
                a2->data.box.size = *(vec3*)d;
                return true;
            case GLITTER_EMITTER_CYLINDER:
                a2->data.cylinder.radius = *(float_t*)d;
                a2->data.cylinder.height = *(float_t*)(d + 4);
                a2->data.cylinder.start_angle = *(float_t*)(d + 8);
                a2->data.cylinder.end_angle = *(float_t*)(d + 12);
                a2->data.cylinder.on_edge = *(int32_t*)(d + 16) & 1 ? true : false;
                a2->data.cylinder.direction = (glitter_emitter_emission_direction)
                    (*(int32_t*)(d + 16) >> 1);
                return true;
            case GLITTER_EMITTER_SPHERE:
                a2->data.sphere.radius = *(float_t*)d;
                a2->data.sphere.latitude = *(float_t*)(d + 4);
                a2->data.sphere.longitude = *(float_t*)(d + 8);
                a2->data.sphere.on_edge = *(int32_t*)(d + 12) & 1 ? true : false;
                a2->data.sphere.direction = (glitter_emitter_emission_direction)
                    (*(int32_t*)(d + 12) >> 1);
                return true;
            case GLITTER_EMITTER_POLYGON:
                a2->data.polygon.size = *(float_t*)d;
                a2->data.polygon.count = *(int32_t*)(d + 4);
                return true;
            default:
                return false;
            }
    }
}
