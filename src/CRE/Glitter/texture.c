/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../texture.h"

bool FASTCALL glitter_texture_hashes_pack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t l;
    size_t d;
    size_t count;

    if (a1->effects.end - a1->effects.begin < 1 || !a1->resources_count
        || a1->resource_hashes.end - a1->resource_hashes.begin < 1)
        return false;

    count = a1->resources_count;

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 2, (uint32_t)(8 + count * 8), 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, (uint32_t)count, ENRS_TYPE_QWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += 8 + count * 8;

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->data = (void*)d;
    st->length = l;
    st->enrs = e;

    *(int32_t*)d = (int32_t)count;
    *(int32_t*)(d + 4) = 0;
    d += 8;

    memcpy((void*)d, a1->resource_hashes.begin, sizeof(uint64_t) * count);

    st->header.signature = 0x53525644;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool FASTCALL glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    uint64_t data;
    uint32_t count;
    uint64_t* resource_hashes;
    size_t i;

    if (a1->resources_count && a1->resource_hashes.end - a1->resource_hashes.begin > 0)
        return true;

    if (!st || !st->header.data_size)
        return false;

    data = (uint64_t)st->data;
    if (!data)
        return false;

    if (st->header.use_big_endian)
        count = reverse_endianess_uint32_t(*(int32_t*)data);
    else
        count = *(int32_t*)data;
    data += 8;

    a1->resources_count = count;
    if (count) {
        resource_hashes = force_malloc_s(uint64_t, count);
        a1->resource_hashes.begin = resource_hashes;
        a1->resource_hashes.end = resource_hashes + count;
        a1->resource_hashes.capacity_end = resource_hashes + count;
        if (!resource_hashes)
            return false;

        if (st->header.use_big_endian)
            for (i = 0; i < count; i++, data += sizeof(uint64_t))
                resource_hashes[i] = reverse_endianess_uint64_t(*(uint64_t*)data);
        else
            memcpy(resource_hashes, (void*)data, sizeof(uint64_t) * count);
    }
    return true;
}

bool FASTCALL glitter_texture_resource_pack_file(glitter_effect_group* a1, f2_struct* st) {
    if (a1->resources_tex.end - a1->resources_tex.begin < 1)
        return false;

    memset(st, 0, sizeof(f2_struct));

    if (!tex_set_produce_enrs(&a1->resources_tex, &st->enrs))
        return false;

    tex_set_pack_file(&a1->resources_tex, &st->data, &st->length, false);

    st->header.signature = 0x43505854;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool FASTCALL glitter_texture_resource_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    if (!st || !st->header.data_size)
        return false;

    tex_set_unpack_file(&a1->resources_tex, st->data, st->header.use_big_endian);
    return glitter_texture_load(a1);
}

bool FASTCALL glitter_texture_load(glitter_effect_group* a1) {
    if (!a1->resources_count)
        return false;

    size_t count = a1->resources_tex.end - a1->resources_tex.begin;

    if (!count || !a1->resources_count || a1->resources_count != count)
        return false;

    if (!texture_txp_load(&a1->resources_tex, &a1->resources))
        return false;

    for (glitter_effect** l = a1->effects.begin; l != a1->effects.end; l++) {
        if (!*l)
            continue;

        glitter_effect* effect = *l;
        for (glitter_emitter** m = effect->emitters.begin; m != effect->emitters.end; m++) {
            if (!*m)
                continue;

            glitter_emitter* emitter = *m;
            for (glitter_particle** n = emitter->particles.begin; n != emitter->particles.end; n++) {
                if (!*n)
                    continue;

                glitter_particle* particle = *n;
                particle->data.texture0 = 0;
                particle->data.texture1 = 0;
            }
        }
    }

    for (size_t i = 0; i < a1->resources_count; i++)
        for (glitter_effect** l = a1->effects.begin; l != a1->effects.end; l++) {
            if (!*l)
                continue;

            glitter_effect* effect = *l;
            for (glitter_emitter** m = effect->emitters.begin; m != effect->emitters.end; m++) {
                if (!*m)
                    continue;

                glitter_emitter* emitter = *m;
                for (glitter_particle** n = emitter->particles.begin; n != emitter->particles.end; n++) {
                    if (!*n)
                        continue;

                    glitter_particle* particle = *n;
                    if (particle->data.type == GLITTER_PARTICLE_LINE
                        || particle->data.type == GLITTER_PARTICLE_MESH)
                        continue;

                    if (particle->data.tex_hash0 == a1->resource_hashes.begin[i])
                        particle->data.texture0 = a1->resources.begin[i];
                    if (particle->data.tex_hash1 == a1->resource_hashes.begin[i])
                        particle->data.texture1 = a1->resources.begin[i];
                }
            }
        }
    return true;
}

void FASTCALL glitter_texture_unload(glitter_effect_group* a1) {
    texture_txp_unload(&a1->resources);
}
