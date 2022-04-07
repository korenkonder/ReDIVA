/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../texture.h"

bool glitter_texture_hashes_pack_file(GlitterEffectGroup* a1, f2_struct* st) {
    size_t l;
    size_t d;
    size_t count;

    if (a1->effects.size() < 1 || !a1->resources_count
        || a1->resource_hashes.size() < 1)
        return false;

    count = a1->resources_count;

    l = 0;

    enrs e;
    enrs_entry ee;

    ee = { 0, 2, (uint32_t)(8 + count * 8), 1 };
    ee.sub.push_back({ 0, 1, ENRS_DWORD });
    ee.sub.push_back({ 4, (uint32_t)count, ENRS_QWORD });
    e.vec.push_back(ee);
    l += 8 + count * 8;

    l = align_val(l, 0x10);
    st->data.resize(l);
    d = (size_t)st->data.data();
    st->enrs = e;

    *(int32_t*)d = (int32_t)count;
    *(int32_t*)(d + 4) = 0;
    d += 8;

    memcpy((void*)d, a1->resource_hashes.data(), sizeof(uint64_t) * count);

    st->header.signature = reverse_endianness_uint32_t('DVRS');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_texture_hashes_unpack_file(GlitterEffectGroup* a1, f2_struct* st) {
    size_t d;
    uint32_t count;
    uint64_t* resource_hashes;
    size_t i;

    if (a1->resources_count && a1->resource_hashes.size() > 0)
        return true;

    if (!st || !st->header.data_size)
        return false;

    d = (size_t)st->data.data();
    if (!d)
        return false;

    if (st->header.use_big_endian)
        count = load_reverse_endianness_uint32_t((void*)d);
    else
        count = *(int32_t*)d;
    d += 8;

    a1->resources_count = count;
    if (count) {
        a1->resource_hashes = std::vector<uint64_t>(count);
        resource_hashes = a1->resource_hashes.data();
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

bool glitter_texture_resource_pack_file(GlitterEffectGroup* a1, f2_struct* st) {
    if (a1->resources_tex.textures.size() < 1)
        return false;

    if (!a1->resources_tex.produce_enrs(&st->enrs))
        return false;

    a1->resources_tex.pack_file(&st->data, false);

    st->header.signature = reverse_endianness_uint32_t('TXPC');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_texture_resource_unpack_file(GPM, GlitterEffectGroup* a1, f2_struct* st) {
    if (!st || !st->header.data_size)
        return false;

    a1->resources_tex.unpack_file(st->data.data(), st->header.use_big_endian);
    return glitter_texture_load(GPM_VAL, a1);
}

bool glitter_texture_load(GPM, GlitterEffectGroup* a1) {
    if (!a1->resources_count)
        return false;

    size_t count = a1->resources_tex.textures.size();

    if (count < 1 || a1->resources_count < 1 || a1->resources_count != count)
        return false;

    texture_id* ids = force_malloc_s(texture_id, count);
    for (size_t i = 0; i < count; i++) {
        ids[i] = texture_id(0x2A, GPM_VAL->texture_counter);
        GPM_VAL->texture_counter++;
    }

    if (!texture_txp_set_load(&a1->resources_tex, &a1->resources, ids)) {
        free(ids);
        return false;
    }
    free(ids);

    for (glitter_effect*& i : a1->effects) {
        if (!i)
            continue;

        glitter_effect* effect = i;
        for (glitter_emitter*& j : effect->emitters) {
            if (!j)
                continue;

            glitter_emitter* emitter = j;
            for (glitter_particle*& k : emitter->particles) {
                if (!k)
                    continue;

                glitter_particle* particle = k;
                particle->data.texture = 0;
                particle->data.mask_texture = 0;
            }
        }
    }

    for (size_t i = 0; i < a1->resources_count; i++)
        for (glitter_effect*& j : a1->effects) {
            if (!j)
                continue;

            glitter_effect* effect = j;
            for (glitter_emitter*& k : effect->emitters) {
                if (!k)
                    continue;

                glitter_emitter* emitter = k;
                for (glitter_particle*& l : emitter->particles) {
                    if (!l)
                        continue;

                    glitter_particle* particle = l;
                    if (particle->data.type == GLITTER_PARTICLE_LINE
                        || particle->data.type == GLITTER_PARTICLE_MESH)
                        continue;

                    if (particle->data.tex_hash == a1->resource_hashes[i])
                        particle->data.texture = a1->resources[i]->tex;
                    if (particle->data.mask_tex_hash == a1->resource_hashes[i])
                        particle->data.mask_texture = a1->resources[i]->tex;
                }
            }
        }
    return true;
}

void glitter_texture_unload(GlitterEffectGroup* a1) {
    if (a1->resources) {
        for (texture** i = a1->resources; *i; i++)
            texture_free(*i);
        free(a1->resources);
    }
}
