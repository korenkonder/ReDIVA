/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../texture.h"

bool glitter_texture_hashes_pack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t l;
    size_t d;
    size_t count;

    if (vector_length(a1->effects) < 1 || !a1->resources_count
        || vector_length(a1->resource_hashes) < 1)
        return false;

    count = a1->resources_count;

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;

    ee = { 0, 2, (uint32_t)(8 + count * 8), 1, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
    vector_enrs_sub_entry_append(&ee.sub, 4, (uint32_t)count, ENRS_QWORD);
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

    st->header.signature = reverse_endianness_uint32_t('DVRS');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t d;
    uint32_t count;
    uint64_t* resource_hashes;
    size_t i;

    if (a1->resources_count && vector_length(a1->resource_hashes) > 0)
        return true;

    if (!st || !st->header.data_size)
        return false;

    d = (uint64_t)st->data;
    if (!d)
        return false;

    if (st->header.use_big_endian)
        count = load_reverse_endianness_uint32_t((void*)d);
    else
        count = *(int32_t*)d;
    d += 8;

    a1->resources_count = count;
    if (count) {
        resource_hashes = force_malloc_s(uint64_t, count);
        a1->resource_hashes.begin = resource_hashes;
        a1->resource_hashes.end = resource_hashes + count;
        a1->resource_hashes.capacity_end = resource_hashes + count;
        if (!resource_hashes)
            return false;

        if (st->header.use_big_endian)
            for (i = 0; i < count; i++, d += sizeof(uint64_t))
                resource_hashes[i] = load_reverse_endianness_uint64_t((void*)d);
        else
            memcpy(resource_hashes, (void*)d, sizeof(uint64_t) * count);
    }
    return true;
}

bool glitter_texture_resource_pack_file(glitter_effect_group* a1, f2_struct* st) {
    if (vector_length(a1->resources_tex) < 1)
        return false;

    memset(st, 0, sizeof(f2_struct));

    if (!txp_set_produce_enrs(&a1->resources_tex, &st->enrs))
        return false;

    txp_set_pack_file(&a1->resources_tex, &st->data, &st->length, false);

    st->header.signature = reverse_endianness_uint32_t('TXPC');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_texture_resource_unpack_file(GPM, glitter_effect_group* a1, f2_struct* st) {
    if (!st || !st->header.data_size)
        return false;

    txp_set_unpack_file(&a1->resources_tex, st->data, st->header.use_big_endian);
    return glitter_texture_load(GPM_VAL, a1);
}

bool glitter_texture_load(GPM, glitter_effect_group* a1) {
    if (!a1->resources_count)
        return false;

    size_t count = vector_length(a1->resources_tex);

    if (count < 1 || a1->resources_count < 1 || a1->resources_count != count)
        return false;

    uint32_t* ids = force_malloc_s(uint32_t, count);
    for (size_t i = 0; i < count; i++) {
        ids[i] = texture_make_id(0x2A, GPM_VAL->texture_counter);
        GPM_VAL->texture_counter++;
    }

    if (!texture_txp_set_load(&a1->resources_tex, &a1->resources, ids)) {
        free(ids);
        return false;
    }
    free(ids);

    for (glitter_effect** i = a1->effects.begin; i != a1->effects.end; i++) {
        if (!*i)
            continue;

        glitter_effect* effect = *i;
        for (glitter_emitter** j = effect->emitters.begin; j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            glitter_emitter* emitter = *j;
            for (glitter_particle** k = emitter->particles.begin; k != emitter->particles.end; k++) {
                if (!*k)
                    continue;

                glitter_particle* particle = *k;
                particle->data.texture = 0;
                particle->data.mask_texture = 0;
            }
        }
    }

    for (size_t i = 0; i < a1->resources_count; i++)
        for (glitter_effect** j = a1->effects.begin; j != a1->effects.end; j++) {
            if (!*j)
                continue;

            glitter_effect* effect = *j;
            for (glitter_emitter** k = effect->emitters.begin; k != effect->emitters.end; k++) {
                if (!*k)
                    continue;

                glitter_emitter* emitter = *k;
                for (glitter_particle** l = emitter->particles.begin; l != emitter->particles.end; l++) {
                    if (!*l)
                        continue;

                    glitter_particle* particle = *l;
                    if (particle->data.type == GLITTER_PARTICLE_LINE
                        || particle->data.type == GLITTER_PARTICLE_MESH)
                        continue;

                    if (particle->data.tex_hash == a1->resource_hashes.begin[i])
                        particle->data.texture = a1->resources[i]->texture;
                    if (particle->data.mask_tex_hash == a1->resource_hashes.begin[i])
                        particle->data.mask_texture = a1->resources[i]->texture;
                }
            }
        }
    return true;
}

void glitter_texture_unload(glitter_effect_group* a1) {
    if (a1->resources) {
        for (texture** i = a1->resources; *i; i++)
            texture_free(*i);
        free(a1->resources);
    }
}
