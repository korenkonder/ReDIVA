/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Emitter::Emitter(GLT) : data() {
        version = GLT_VAL == Glitter::X ? 0x04 : 0x02;
        data.loop_start_time = 0;
        data.loop_end_time = -1;
        data.particles_per_emission = 1;
        data.direction = DIRECTION_EFFECT_ROTATION;
    }

    Emitter::~Emitter() {
        for (Particle*& i : particles)
            delete i;
    }
}

/*
#include "emitter.hpp"
#include "animation.hpp"
#include "curve.hpp"
#include "particle.hpp"

static bool glitter_emitter_pack_file(GLT, f2_struct* st, glitter_emitter* a2);

bool glitter_emitter_unparse_file(GLT, EffectGroup* a1,
    f2_struct* st, glitter_emitter* a3, glitter_effect* effect) {
    if (!glitter_emitter_pack_file(GLT_VAL, st, a3))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, glitter_emitter_curve_flags))
        st->sub_structs.push_back(s);

    for (glitter_particle*& i : a3->particles) {
        if (!i)
            continue;

        f2_struct s;
        if (glitter_particle_unparse_file(GLT_VAL, a1, &s, i, effect))
            st->sub_structs.push_back(s);
    }
    return true;
}

static bool glitter_emitter_pack_file(GLT, f2_struct* st, glitter_emitter* a2) {
    size_t l;
    size_t d;

    if (a2->version != 1 && a2->version != 2)
        return false;

    switch (a2->data.type) {
    case EMITTER_BOX:
    case EMITTER_CYLINDER:
    case EMITTER_SPHERE:
    case EMITTER_POLYGON:
        break;
    default:
        return false;
    }

    l = 0;

    uint32_t o;
    enrs e;
    enrs_entry ee;

    ee = { 0, 5, 96, 1 };
    ee.append(0, 5, ENRS_DWORD);
    ee.append(0, 2, ENRS_WORD);
    ee.append(0, 2, ENRS_DWORD);
    ee.append(0, 2, ENRS_WORD);
    ee.append(0, 15, ENRS_DWORD);
    e.vec.push_back(ee);
    l += o = 96;

    switch (a2->data.type) {
    case EMITTER_BOX:
        ee = { o, 1, 12, 1 };
        ee.append(0, 3, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 12;
        break;
    case EMITTER_CYLINDER:
        ee = { o, 1, 20, 1 };
        ee.append(0, 5, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 20;
        break;
    case EMITTER_SPHERE:
        ee = { o, 1, 16, 1 };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 16;
        break;
    case EMITTER_POLYGON:
        ee = { o, 1, 8, 1 };
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 8;
        break;
    }

    l = align_val(l, 0x10);
    st->data.resize(l);
    d = (size_t)st->data.data();
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
    case EMITTER_BOX:
        *(vec3*)d = a2->data.box.size;
        break;
    case EMITTER_CYLINDER:
        *(float_t*)d = a2->data.cylinder.radius;
        *(float_t*)(d + 4) = a2->data.cylinder.height;
        *(float_t*)(d + 8) = a2->data.cylinder.start_angle;
        *(float_t*)(d + 12) = a2->data.cylinder.end_angle;
        *(int32_t*)(d + 16) = ((int32_t)a2->data.cylinder.direction << 1)
            | (a2->data.cylinder.on_edge ? 1 : 0);
        break;
    case EMITTER_SPHERE:
        *(float_t*)d = a2->data.sphere.radius;
        *(float_t*)(d + 4) = a2->data.sphere.latitude;
        *(float_t*)(d + 8) = a2->data.sphere.longitude;
        *(int32_t*)(d + 12) = ((int32_t)a2->data.sphere.direction << 1)
            | (a2->data.sphere.on_edge ? 1 : 0);
        break;
    case EMITTER_POLYGON:
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
*/