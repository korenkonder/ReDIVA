/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/txp.h"
#include "static_var.h"

static void load_texture_data(GLenum target, txp_format format,
    uint32_t width, uint32_t height, uint32_t level, void* data);

static void texture_1d_load(texture* tex, texture_data* data) {
    uint32_t width = max(data->width, 0);

    bind_tex1d(tex->id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage1D(GL_TEXTURE_1D, 0, data->pixel_internal_format, width,
        0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_1D);
    bind_tex1d(0);
}

static void texture_1d_update(texture* tex) {
    bind_tex1d(tex->id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    bind_tex1d(0);
}

static void texture_2d_load(texture* tex, texture_data* data) {
    uint32_t width = max(data->width, 0);
    uint32_t height = max(data->height, 0);

    bind_tex2d(tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data->wrap_mode_t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage2D(GL_TEXTURE_2D, 0, data->pixel_internal_format, width, height,
        0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
    bind_tex2d(0);
}

static void texture_2d_update(texture* tex) {
    bind_tex2d(tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    bind_tex2d(0);
}

static void texture_3d_load(texture* tex, texture_data* data) {
    uint32_t width = max(data->width, 0);
    uint32_t height = max(data->height, 0);
    uint32_t depth = max(data->depth, 0);

    bind_tex3d(tex->id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, data->wrap_mode_t);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, data->wrap_mode_r);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage3D(GL_TEXTURE_3D, 0, data->pixel_internal_format, width, height,
        depth, 0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_3D);
    bind_tex3d(0);
}

static void texture_3d_update(texture* tex) {
    bind_tex3d(tex->id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    bind_tex3d(0);
}

static void texture_cube_load(texture* tex, texture_cube_data* data) {
    uint32_t px_width = max(data->px.width, 0);
    uint32_t px_height = max(data->px.height, 0);
    uint32_t nx_width = max(data->nx.width, 0);
    uint32_t nx_height = max(data->nx.height, 0);
    uint32_t py_width = max(data->py.width, 0);
    uint32_t py_height = max(data->py.height, 0);
    uint32_t ny_width = max(data->ny.width, 0);
    uint32_t ny_height = max(data->ny.height, 0);
    uint32_t pz_width = max(data->pz.width, 0);
    uint32_t pz_height = max(data->pz.height, 0);
    uint32_t nz_width = max(data->nz.width, 0);
    uint32_t nz_height = max(data->nz.height, 0);

    bind_texcube(tex->id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, data->wrap_mode_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, data->wrap_mode_r);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, data->pixel_internal_format, px_width,
        px_height, 0, data->px.pixel_format, data->px.pixel_type, data->px.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, data->pixel_internal_format, nx_width,
        nx_height, 0, data->nx.pixel_format, data->nx.pixel_type, data->nx.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, data->pixel_internal_format, py_width,
        py_height, 0, data->py.pixel_format, data->py.pixel_type, data->py.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, data->pixel_internal_format, ny_width,
        ny_height, 0, data->ny.pixel_format, data->ny.pixel_type, data->ny.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, data->pixel_internal_format, pz_width,
        pz_height, 0, data->pz.pixel_format, data->pz.pixel_type, data->pz.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, data->pixel_internal_format, nz_width,
        nz_height, 0, data->nz.pixel_format, data->nz.pixel_type, data->nz.data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    bind_texcube(0);
}

static void texture_cube_update(texture* tex) {
    bind_texcube(tex->id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    bind_texcube(0);
}

void texture_load(texture* tex, texture_data* data) {
    if (!tex)
        return;

    if (tex->id == 0)
        glGenTextures(1, &tex->id);

    if (!data)
        return;

    tex->mode = TEXTURE_MODE_DEFAULT;
    tex->type = data->type;
    switch (tex->type) {
    case TEXTURE_1D:
        texture_1d_load(tex, data);
        break;
    case TEXTURE_2D:
        texture_2d_load(tex, data);
        break;
    case TEXTURE_3D:
        texture_3d_load(tex, data);
        break;
    case TEXTURE_CUBE:
        texture_cube_load(tex, (texture_cube_data*)data);
        break;
    }
}

void texture_bind(texture* tex, int32_t index) {
    if (!tex)
        return;

    switch (tex->type) {
    case TEXTURE_1D:
        bind_index_tex1d(index, tex->id);
        break;
    case TEXTURE_2D:
        bind_index_tex2d(index, tex->id);
        break;
    case TEXTURE_3D:
        bind_index_tex3d(index, tex->id);
        break;
    case TEXTURE_CUBE:
        bind_index_texcube(index, tex->id);
        break;
    }
}

void texture_reset(texture* tex, int32_t index) {
    if (!tex)
        return;

    switch (tex->type) {
    case TEXTURE_1D:
        bind_index_tex1d(index, 0);
        break;
    case TEXTURE_2D:
        bind_index_tex2d(index, 0);
        break;
    case TEXTURE_3D:
        bind_index_tex3d(index, 0);
        break;
    case TEXTURE_CUBE:
        bind_index_texcube(index, 0);
        break;
    }
}

void texture_update(texture* tex) {
    if (!tex)
        return;

    switch (tex->type) {
    case TEXTURE_1D:
        texture_1d_update(tex);
        break;
    case TEXTURE_2D:
        texture_2d_update(tex);
        break;
    case TEXTURE_3D:
        texture_3d_update(tex);
        break;
    case TEXTURE_CUBE:
        texture_cube_update(tex);
        break;
    }
}

void texture_free(texture* tex) {
    if (!tex)
        return;

    if (tex->id)
        glDeleteTextures(1, &tex->id);
    tex->id = 0;
}

void texture_set_load(texture_set* tex, texture_set_data* data) {
    if (!tex)
        return;

    if (data)
        for (int32_t i = 0; i < 8; i++)
            texture_load(&tex->tex[i], data->tex[i]);
    else
        for (int32_t i = 0; i < 8; i++)
            texture_load(&tex->tex[i], 0);
}

void texture_set_bind(texture_set* tex) {
    if (!tex)
        return;

    for (int32_t i = 0; i < 8; i++)
        texture_bind(&tex->tex[i], i);
}

void texture_set_reset(texture_set* tex) {
    if (!tex)
        return;

    for (int32_t i = 0; i < 8; i++)
        texture_reset(&tex->tex[i], i);
}

void texture_set_update(texture_set* tex) {
    if (!tex)
        return;

    for (int32_t i = 0; i < 8; i++)
        texture_update(&tex->tex[i]);
}

void texture_set_free(texture_set* tex) {
    if (!tex)
        return;

    for (int32_t i = 0; i < 8; i++)
        texture_free(&tex->tex[i]);
}

void texture_bone_mat_load(texture_bone_mat* tex, texture_bone_mat_data* data) {
    if (!tex)
        return;

    if (tex->id == 0)
        glGenTextures(1, &tex->id);

    if (!data || !data->data || data->count < 1)
        return;

    mat4* mat = data->data;
    void* temp_data = force_malloc_s(mat4, data->count * 2LL);
    mat4* d = temp_data;
    for (int32_t i = 0; i < data->count; i++, mat++, d += 2) {
        d[0] = *mat;
        mat4_invtrans(mat, &d[1]);
    }

    bind_tex2d(tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeof(mat4) / sizeof(vec4) * 2,
        data->count, 0, GL_RGBA, GL_FLOAT, temp_data);
    bind_tex2d(0);
    free(temp_data);
}

void texture_bone_mat_bind(texture_bone_mat* tex, int32_t index) {
    if (!tex)
        return;

    bind_index_tex2d(index, tex->id);
}

void texture_bone_mat_reset(texture_bone_mat* tex, int32_t index) {
    if (!tex)
        return;

    bind_index_tex2d(index, tex->id);
}

void texture_bone_mat_free(texture_bone_mat* tex) {
    if (!tex)
        return;

    if (tex->id)
        glDeleteTextures(1, &tex->id);
    tex->id = 0;
}

bool texture_txp_load(vector_txp* tex, vector_int32_t* textures) {
    static const GLenum target_cubemap_array[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    };

    if (!tex || !textures)
        return false;

    size_t count = tex->end - tex->begin;
    if (count < 1)
        return false;

    if (textures->end - textures->begin != 0)
        texture_txp_unload(textures);

    vector_int32_t_append(textures, count);
    for (uint32_t i = 0; i < count; i++)
        vector_int32_t_push_back(textures, &i);
    glGenTextures((GLsizei)count, textures->begin);

    txp* tex_data = tex->begin;
    for (size_t i = 0; i < count; i++, tex_data++)
        if ((tex_data->has_cubemap && tex_data->array_size == 6) || tex_data->array_size == 1) {
            GLenum target = tex_data->has_cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
            uint32_t mipmaps_count = tex_data->mipmaps_count;
            if (tex_data->has_cubemap)
                bind_texcube(textures->begin[i]);
            else
                bind_tex2d(textures->begin[i]);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, (float_t*)&vec4_null);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                mipmaps_count > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, mipmaps_count - 1);
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, tex_data->has_cubemap ? 1.0f : 16.0f);
            txp_mipmap* tex_mipmap = tex_data->data.begin;
            if (tex_data->has_cubemap)
                for (uint32_t j = 0; j < 6; j++, tex_data++) {
                    uint32_t width = tex_mipmap->width;
                    uint32_t height = tex_mipmap->height;
                    txp_format format = tex_mipmap->format;
                    for (uint32_t k = 0; k < mipmaps_count; k++, tex_mipmap++)
                        load_texture_data(target_cubemap_array[j], format,
                            max(width >> k, 1), max(height >> k, 1), k, tex_mipmap->data);
                }
            else {
                uint32_t width = tex_mipmap->width;
                uint32_t height = tex_mipmap->height;
                txp_format format = tex_mipmap->format;
                for (uint32_t k = 0; k < mipmaps_count; k++, tex_mipmap++)
                    load_texture_data(target, format,
                        max(width >> k, 1), max(height >> k, 1), k, tex_mipmap->data);
            }
            glBindTexture(target, 0);
        }
    bind_tex2d(0);
    bind_texcube(0);
    return true;
}

void texture_txp_unload(vector_int32_t* textures) {
    if (!textures)
        return;

    if (textures->end - textures->begin != 0)
        glDeleteTextures((GLsizei)(textures->end - textures->begin), textures->begin);
    vector_int32_t_free(textures);
}

static void load_texture_data(GLenum target, txp_format format,
    uint32_t width, uint32_t height, uint32_t level, void* data) {
    static const GLint textureswizzles_a8  [] = { GL_ZERO, GL_ZERO , GL_ZERO, GL_RED   };
    static const GLint textureswizzles_ati1[] = { GL_RED , GL_RED  , GL_RED , GL_ONE   };
    static const GLint textureswizzles_ati2[] = { GL_RED , GL_GREEN, GL_ZERO, GL_RED   };
    static const GLint textureswizzles_l8  [] = { GL_RED , GL_RED  , GL_RED , GL_ONE   };
    static const GLint textureswizzles_l8a8[] = { GL_RED , GL_RED  , GL_RED , GL_GREEN };

    static const GLint* textureswizzles[] = {
        [TXP_A8]     = textureswizzles_a8,
        [TXP_RGB8]   = 0,
        [TXP_RGBA8]  = 0,
        [TXP_RGB5]   = 0,
        [TXP_RGB5A1] = 0,
        [TXP_RGBA4]  = 0,
        [TXP_DXT1]   = 0,
        [TXP_DXT1a]  = 0,
        [TXP_DXT3]   = 0,
        [TXP_DXT5]   = 0,
        [TXP_ATI1]   = textureswizzles_ati1,
        [TXP_ATI2]   = textureswizzles_ati2,
        [TXP_L8]     = textureswizzles_l8,
        [TXP_L8A8]   = textureswizzles_l8a8,
    };

    static const GLenum types[] = {
        [TXP_A8]     = GL_UNSIGNED_BYTE,
        [TXP_RGB8]   = GL_UNSIGNED_BYTE,
        [TXP_RGBA8]  = GL_UNSIGNED_BYTE,
        [TXP_RGB5]   = GL_UNSIGNED_SHORT_5_6_5_REV,
        [TXP_RGB5A1] = GL_UNSIGNED_SHORT_1_5_5_5_REV,
        [TXP_RGBA4]  = GL_UNSIGNED_SHORT_4_4_4_4_REV,
        [TXP_DXT1]   = 0,
        [TXP_DXT1a]  = 0,
        [TXP_DXT3]   = 0,
        [TXP_DXT5]   = 0,
        [TXP_ATI1]   = 0,
        [TXP_ATI2]   = 0,
        [TXP_L8]     = GL_UNSIGNED_BYTE,
        [TXP_L8A8]   = GL_UNSIGNED_BYTE,
    };

    static const GLenum formats[] = {
        [TXP_A8]     = GL_RED,
        [TXP_RGB8]   = GL_RGB,
        [TXP_RGBA8]  = GL_RGBA,
        [TXP_RGB5]   = GL_RGB,
        [TXP_RGB5A1] = GL_RGBA,
        [TXP_RGBA4]  = GL_RGBA,
        [TXP_DXT1]   = 0,
        [TXP_DXT1a]  = 0,
        [TXP_DXT3]   = 0,
        [TXP_DXT5]   = 0,
        [TXP_ATI1]   = 0,
        [TXP_ATI2]   = 0,
        [TXP_L8]     = GL_RED,
        [TXP_L8A8]   = GL_RG,
    };

    static const GLenum internalformats[] = {
        [TXP_A8]     = GL_R8,
        [TXP_RGB8]   = GL_RGB8,
        [TXP_RGBA8]  = GL_RGBA8,
        [TXP_RGB5]   = GL_RGB5,
        [TXP_RGB5A1] = GL_RGB5_A1,
        [TXP_RGBA4]  = GL_RGBA4,
        [TXP_DXT1]   = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
        [TXP_DXT1a]  = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        [TXP_DXT3]   = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        [TXP_DXT5]   = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
        [TXP_ATI1]   = GL_COMPRESSED_RED_RGTC1_EXT,
        [TXP_ATI2]   = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
        [TXP_L8]     = GL_R8,
        [TXP_L8A8]   = GL_RG8,
    };

    switch (format) {
    case TXP_A8:
    case TXP_L8:
    case TXP_L8A8:
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, textureswizzles[format]);
        glTexImage2D(target, level, internalformats[format],
            width, height, 0, formats[format], types[format], data);
        break;
    case TXP_RGB8:
    case TXP_RGBA8:
    case TXP_RGB5:
    case TXP_RGB5A1:
    case TXP_RGBA4:
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(target, level, internalformats[format],
            width, height, 0, formats[format], types[format], data);
        break;
    case TXP_DXT1:
    case TXP_DXT1a:
    case TXP_DXT3:
    case TXP_DXT5: {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        uint32_t size = txp_get_size(format, width, height);
        glCompressedTexImage2D(target, level, internalformats[format],
            width, height, 0, size, data);
    } break;
    case TXP_ATI1:
    case TXP_ATI2: {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, textureswizzles[format]);
        uint32_t size = txp_get_size(format, width, height);
        glCompressedTexImage2D(target, level, internalformats[format],
            width, height, 0, size, data);
    } break;
    }
}