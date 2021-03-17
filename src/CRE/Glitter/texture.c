/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"

static void load_texture_data(txp_sub_data* data, int32_t mipmap);

bool FASTCALL glitter_texture_hashes_pack_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian) {
    char* data;
    size_t count;
    uint64_t* resource_hashes;
    size_t i;

    if (!a1->effects.begin)
        return false;

    memset(st, 0, sizeof(f2_struct));
    count = a1->resources_count;
    data = force_malloc(align_val(0x08 + 0x08 * count, 0x10));
    st->data = data;
    st->length = align_val(0x08 + 0x08 * count, 0x10);

    if (use_big_endian)
        *(int32_t*)data = reverse_endianess_int32_t((int32_t)count);
    else
        *(int32_t*)data = (int32_t)count;
    data += 0x04;
    *(int32_t*)data = 0;
    data += 0x04;

    if (count) {
        resource_hashes = a1->resource_hashes;
        if (!resource_hashes)
            return false;

        if (use_big_endian)
            for (i = 0; i < count; i++, data += sizeof(uint64_t))
                *(uint64_t*)data = reverse_endianess_uint64_t(resource_hashes[i]);
        else
            memcpy(data, resource_hashes, sizeof(uint64_t) * count);
    }

    st->header.signature = 0x53525644;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    return true;
}

bool FASTCALL glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    uint64_t data;
    uint32_t count;
    uint64_t* resource_hashes;
    size_t i;

    if (a1->resource_hashes)
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
        resource_hashes = force_malloc_s(sizeof(uint64_t), count);
        a1->resource_hashes = resource_hashes;
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

bool FASTCALL glitter_texture_resource_pack_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian) {
    if (!a1->resources_tex)
        return false;

    memset(st, 0, sizeof(f2_struct));

    txp_pack_file(a1->resources_tex, &st->data, &st->length, use_big_endian);

    st->header.signature = 0x43505854;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    return true;
}

bool FASTCALL glitter_texture_resource_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t i;
    size_t j;
    size_t k;
    txp_data* tex_data;
    txp_sub_data** tex_sub_data;
    txp_sub_data* tex_sub_sub_data;
    glitter_effect** l;
    glitter_effect* effect;
    glitter_emitter** m;
    glitter_emitter* emitter;
    glitter_particle** n;
    glitter_particle* particle;

    if (!st || !st->header.data_size)
        return false;

    txp* t = txp_init();
    if (!txp_unpack_file(t, st->data, st->header.use_big_endian) || !a1->resources_count || a1->resources_count != t->count)
        goto End;
    
    if (!a1->resources) {
        a1->resources = force_malloc_s(sizeof(int32_t), a1->resources_count);
        glGenTextures(a1->resources_count, a1->resources);

        tex_data = t->data;
        for (i = 0; i < t->count; i++, tex_data++) {
            glBindTexture(GL_TEXTURE_2D, a1->resources[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                tex_data->mipmaps_count > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex_data->mipmaps_count - 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
            tex_sub_data = tex_data->data;
            for (j = 0; j < tex_data->array_size; j++, tex_sub_data++) {
                tex_sub_sub_data = *tex_sub_data;
                for (k = 0; k < tex_data->mipmaps_count; k++, tex_sub_sub_data++)
                    load_texture_data(tex_sub_sub_data, (int32_t)k);
                break;
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    for (i = 0; i < a1->resources_count; i++)
        for (l = a1->effects.begin; l != a1->effects.end; l++) {
            effect = *l;
            if (!effect)
                continue;

            for (m = effect->emitters.begin; m != effect->emitters.end; m++) {
                emitter = *m;
                if (!emitter)
                    continue;

                for (n = emitter->particles.begin; n != emitter->particles.end; n++) {
                    particle = *n;
                    if (!particle)
                        continue;

                    if (particle->data.data.tex_hash0 == a1->resource_hashes[i])
                        particle->data.data.texture0 = a1->resources[i];
                    if (particle->data.data.tex_hash1 == a1->resource_hashes[i])
                        particle->data.data.texture1 = a1->resources[i];
                }
            }
        }
End:
    a1->resources_tex = t;
    return true;
}

static void load_texture_data(txp_sub_data* data, int32_t mipmap) {
    switch (data->format) {
    case TXP_A8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_ALPHA8, data->width, data->height,
            0, GL_ALPHA, GL_UNSIGNED_BYTE, data->data);
        break;
    case TXP_RGB8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB8, data->width, data->height,
            0, GL_RGB, GL_UNSIGNED_BYTE, data->data);
        break;
    case TXP_RGBA8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA8, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, data->data);
        break;
    case TXP_RGB5:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB5, data->width, data->height,
            0, GL_RGB, GL_UNSIGNED_SHORT_5_5_5_1, data->data);
        break;
    case TXP_RGB5A1:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB5_A1, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data->data);
        break;
    case TXP_RGBA4:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA4, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data->data);
        break;
    case TXP_DXT1:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_DXT1a:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_DXT3:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_DXT5:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_ATI1:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RED_RGTC1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_ATI2:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case TXP_L8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_LUMINANCE8, data->width, data->height,
            0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data->data);
        break;
    case TXP_L8A8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_LUMINANCE8_ALPHA8, data->width, data->height,
            0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data->data);
        break;
    }
}
