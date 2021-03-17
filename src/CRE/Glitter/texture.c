/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "parse_file.h"

typedef enum glitter_texture_format {
    GLITTER_TEXTURE_A8     = 0,
    GLITTER_TEXTURE_RGB8   = 1,
    GLITTER_TEXTURE_RGBA8  = 2,
    GLITTER_TEXTURE_RGB5   = 3,
    GLITTER_TEXTURE_RGB5A1 = 4,
    GLITTER_TEXTURE_RGBA4  = 5,
    GLITTER_TEXTURE_DXT1   = 6,
    GLITTER_TEXTURE_DXT1a  = 7,
    GLITTER_TEXTURE_DXT3   = 8,
    GLITTER_TEXTURE_DXT5   = 9,
    GLITTER_TEXTURE_ATI1   = 10,
    GLITTER_TEXTURE_ATI2   = 11,
    GLITTER_TEXTURE_L8     = 12,
    GLITTER_TEXTURE_L8A8   = 13,
} glitter_texture_format;

typedef struct glitter_texture_sub_data {
    uint32_t width;
    uint32_t height;
    glitter_texture_format format;
    uint32_t size;
    void* data;
} glitter_texture_sub_data;

typedef struct glitter_texture_data {
    uint32_t array_size;
    uint32_t mipmaps_count;
    glitter_texture_sub_data** data;
} glitter_texture_data;

typedef struct glitter_texture {
    uint32_t count;
    glitter_texture_data* data;
} glitter_texture;

static void load_texture_data(glitter_texture_sub_data* data, int32_t mipmap);

bool FASTCALL glitter_texture_hashes_unpack_file(glitter_effect_group* a1, f2_header* header) {
    uint64_t data;
    uint32_t count;
    uint64_t* resource_hashes;

    data = (uint64_t)glitter_parse_file_get_data_ptr(header);
    if (!data)
        return false;

    count = *(int32_t*)data;
    a1->resources_count = count;
    if (count && !a1->resource_hashes) {
        resource_hashes = force_malloc_s(sizeof(uint64_t), count);
        a1->resource_hashes = resource_hashes;
        if (!resource_hashes)
            return false;

        memcpy(resource_hashes, (void*)(data + 8), sizeof(uint64_t) * count);
    }
    return true;
}

bool FASTCALL glitter_texture_resource_unpack_file(glitter_effect_group* a1, uint64_t data) {
    int64_t i;
    int64_t j;
    int64_t k;
    glitter_texture tex;
    uint32_t tex_count;
    glitter_texture_data* tex_data;
    glitter_texture_sub_data** tex_sub_data;
    uint64_t sub_data;
    uint32_t sub_tex_count;
    uint32_t info;
    uint64_t sub_sub_data;
    glitter_texture_sub_data* tex_sub_sub_data;
    glitter_effect** l;
    glitter_effect* effect;
    glitter_emitter** m;
    glitter_emitter* emitter;
    glitter_particle** n;
    glitter_particle* particle;

    if (*(uint32_t*)data != 0x03505854 || a1->resources_count != (tex_count = *(uint32_t*)(data + 4)))
        return false;
    
    tex.count = tex_count;
    tex_data = force_malloc_s(sizeof(glitter_texture_data), tex_count);
    tex.data = tex_data;
    for (i = 0; i < tex_count; i++, tex_data++) {
        sub_data = data + (uint64_t)((uint32_t*)(data + 12))[i];
        if (*(uint32_t*)sub_data != 0x04505854 && *(uint32_t*)sub_data != 0x05505854)
            continue;

        sub_tex_count = *(uint32_t*)(sub_data + 4);
        info = *(uint32_t*)(sub_data + 8);

        tex_data->mipmaps_count = info & 0xFF;
        tex_data->array_size = (info >> 8) & 0xFF;

        if (tex_data->array_size == 1 && tex_data->mipmaps_count != sub_tex_count)
            tex_data->mipmaps_count = sub_tex_count & 0xFF;

        tex_sub_data = force_malloc_s(sizeof(glitter_texture_sub_data*), tex_data->array_size);
        tex_data->data = tex_sub_data;
        for (j = 0; j < tex_data->array_size; j++, tex_sub_data++) {
            tex_sub_sub_data = force_malloc_s(sizeof(glitter_texture_sub_data), tex_data->mipmaps_count);
            *tex_sub_data = tex_sub_sub_data;
            for (k = 0; k < tex_data->mipmaps_count; k++, tex_sub_sub_data++) {
                sub_sub_data = sub_data + (uint64_t)((uint32_t*)(sub_data + 12))[j * tex_data->mipmaps_count + k];
                if (*(uint32_t*)sub_sub_data != 0x02505854)
                    continue;

                tex_sub_sub_data->width = *(uint32_t*)(sub_sub_data + 4);
                tex_sub_sub_data->height = *(uint32_t*)(sub_sub_data + 8);
                tex_sub_sub_data->format = *(uint32_t*)(sub_sub_data + 12);
                tex_sub_sub_data->size = *(uint32_t*)(sub_sub_data + 20);
                tex_sub_sub_data->data = (void*)(sub_sub_data + 24);
            }
        }
    }

    if (!a1->resources_count)
        goto End;
    
    if (!a1->resources) {
        a1->resources = force_malloc_s(sizeof(int32_t), a1->resources_count);
        glGenTextures(a1->resources_count, a1->resources);

        tex_data = tex.data;
        for (i = 0; i < tex.count; i++, tex_data++) {
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
    tex_data = tex.data;
    for (i = 0; i < tex.count; i++, tex_data++) {
        tex_sub_data = tex_data->data;
        for (j = 0; j < tex_data->array_size; j++, tex_sub_data++)
            free(*tex_sub_data);
        free(tex_data->data);
    }
    free(tex.data);
    return true;
}

void load_texture_data(glitter_texture_sub_data* data, int32_t mipmap) {
    switch (data->format) {
    case GLITTER_TEXTURE_A8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_ALPHA8, data->width, data->height,
            0, GL_ALPHA, GL_UNSIGNED_BYTE, data->data);
        break;
    case GLITTER_TEXTURE_RGB8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB8, data->width, data->height,
            0, GL_RGB, GL_UNSIGNED_BYTE, data->data);
        break;
    case GLITTER_TEXTURE_RGBA8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA8, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, data->data);
        break;
    case GLITTER_TEXTURE_RGB5:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB5, data->width, data->height,
            0, GL_RGB, GL_UNSIGNED_SHORT_5_5_5_1, data->data);
        break;
    case GLITTER_TEXTURE_RGB5A1:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGB5_A1, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data->data);
        break;
    case GLITTER_TEXTURE_RGBA4:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA4, data->width, data->height,
            0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data->data);
        break;
    case GLITTER_TEXTURE_DXT1:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_DXT1a:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_DXT3:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_DXT5:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_ATI1:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RED_RGTC1_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_ATI2:
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
            data->width, data->height, 0, data->size, data->data);
        break;
    case GLITTER_TEXTURE_L8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_LUMINANCE8, data->width, data->height,
            0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data->data);
        break;
    case GLITTER_TEXTURE_L8A8:
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_LUMINANCE8_ALPHA8, data->width, data->height,
            0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data->data);
        break;
    }
}
