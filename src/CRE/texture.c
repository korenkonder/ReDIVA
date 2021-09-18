/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/txp.h"
#include "gl_state.h"
#include "static_var.h"

static const GLenum target_cube_map_array[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
};

static void texture_get_format_type_by_internal_format(GLenum internal_format, GLenum* format, GLenum* type);
static int32_t texture_get_size(GLenum internal_format, int32_t width, int32_t height);
static int32_t texture_load(GLenum target, GLenum internal_format,
    int32_t width, int32_t height, int32_t level, void* data);
static texture* texture_load_tex(GLenum target,
    GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy);
static void texture_set_params(GLenum target, int32_t max_mipmap_level, bool use_high_anisotropy);
static GLenum texture_txp_get_internal_format(txp* t);

vector_ptr_func(texture)

texture* texture_load_tex_2d(GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy) {
    return texture_load_tex(GL_TEXTURE_2D, internal_format,
        width, height, max_mipmap_level, data_ptr, use_high_anisotropy);
}

texture* texture_load_tex_cube_map(GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr) {
    return texture_load_tex(GL_TEXTURE_CUBE_MAP, internal_format,
        width, height, max_mipmap_level, data_ptr, false);
}

texture* texture_txp_load(txp* t) {
    if (!t || !t->data.begin)
        return 0;

    int32_t count = t->array_size * t->mipmaps_count;
    void** data_ptr = force_malloc_s(void*, count);
    for (uint32_t i = 0, k = 0; i < t->array_size; i++)
        for (uint32_t j = 0; j < t->mipmaps_count; j++, k++)
            data_ptr[k] = t->data.begin[k].data;

    GLenum internal_format = texture_txp_get_internal_format(t);
    int32_t width = t->data.begin->width;
    int32_t height = t->data.begin->height;
    int32_t max_mipmap_level = t->mipmaps_count - 1;
    texture* tex;
    if (t->has_cube_map)
        tex = texture_load_tex_cube_map(internal_format, width, height, max_mipmap_level, data_ptr);
    else
        tex = texture_load_tex_2d(internal_format, width, height, max_mipmap_level, data_ptr, true);
    free(data_ptr);
    return tex;
}

void texture_dispose(texture* tex) {
    glDeleteTextures(1, &tex->texture);
    free(tex);
}

bool texture_txp_set_load(vector_txp* t, vector_ptr_texture* tex) {
    if (!t || !tex)
        return false;

    vector_ptr_texture_reserve(tex, t->end - t->begin);
    for (txp* i = t->begin; i != t->end; i++) {
        texture* temp = texture_txp_load(i);
        vector_ptr_texture_push_back(tex, &temp);
    }
    return tex->end - tex->begin > 0 ? true : false;
}

static void texture_get_format_type_by_internal_format(GLenum internal_format, GLenum* format, GLenum* type) {
    GLenum _format;
    GLenum _type;
    switch (internal_format) {
    case GL_ALPHA8:
        _format = GL_ALPHA;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_LUMINANCE8:
        _format = GL_LUMINANCE;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_LUMINANCE8_ALPHA8:
        _format = GL_LUMINANCE_ALPHA;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_INTENSITY8:
        _format = GL_INTENSITY;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_RGB5:
        _format = GL_RGB;
        _type = GL_UNSIGNED_SHORT_5_6_5_REV;
        break;
    case GL_RGB8:
        _format = GL_RGB;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_RGBA4:
        _format = GL_RGBA;
        _type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
        break;
    case GL_RGB5_A1:
        _format = GL_RGBA;
        _type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
        break;
    case GL_RGBA8:
        _format = GL_RGBA;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
        _format = GL_DEPTH_COMPONENT;
        _type = GL_FLOAT;
        break;
    case GL_RG8:
        _format = GL_RG;
        _type = GL_UNSIGNED_BYTE;
        break;
    case GL_R32F:
        _format = GL_RED;
        _type = GL_FLOAT;
        break;
    case GL_RG32F:
        _format = GL_RG;
        _type = GL_FLOAT;
        break;
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        _format = GL_RGB;
        _type = GL_ZERO;
        break;
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        _format = GL_RGBA;
        _type = GL_ZERO;
        break;
    case GL_RGBA32F:
        _format = GL_RGBA;
        _type = GL_FLOAT;
        break;
    case GL_RGBA16F:
        _format = GL_RGBA;
        _type = GL_HALF_FLOAT;
        break;
    case GL_DEPTH24_STENCIL8:
        _format = GL_DEPTH_STENCIL;
        _type = GL_UNSIGNED_INT_24_8;
        break;
    case GL_R11F_G11F_B10F:
        _format = GL_RGB;
        _type = GL_UNSIGNED_INT_10F_11F_11F_REV;
        break;
    case GL_RGB9_E5:
        _format = GL_RGB;
        _type = GL_UNSIGNED_INT_5_9_9_9_REV;
        break;
    case GL_DEPTH_COMPONENT32F:
        _format = GL_DEPTH_COMPONENT;
        _type = GL_FLOAT;
        break;
    case GL_COMPRESSED_RED_RGTC1:
        _format = GL_RED;
        _type = GL_ZERO;
        break;
    case GL_COMPRESSED_RG_RGTC2:
        _format = GL_RG;
        _type = GL_ZERO;
        break;
    default:
        _format = GL_ZERO;
        _type = GL_ZERO;
        break;
    }

    if (format)
        *format = _format;
    if (type)
        *type = _type;
}

static int32_t texture_get_size(GLenum internal_format, int32_t width, int32_t height) {
    int32_t size = width * height;
    switch (internal_format) {
    case GL_ALPHA8:
    case GL_LUMINANCE8:
    case GL_INTENSITY8:
    case GL_R8:
        return size;
    case GL_LUMINANCE8_ALPHA8:
    case GL_RGB5:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_DEPTH_COMPONENT16:
    case GL_RG8:
        return size * 2;
    case GL_RGB8:
    case GL_RGBA8:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
    case GL_R32F:
    case GL_RGBA16F:
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH_COMPONENT32F:
    case GL_R11F_G11F_B10F:
    case GL_RGB9_E5:
        return size * 4;
    case GL_RG32F:
        return size * 8;
    case GL_RGBA32F:
        return size * 16;
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RED_RGTC1:
        width = align_val(width, 4);
        height = align_val(height, 4);
        return width * height / 2;
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    case GL_COMPRESSED_RG_RGTC2:
        width = align_val(width, 4);
        height = align_val(height, 4);
        return width * height;
    default:
        return 0;
    }
}

static int32_t texture_load(GLenum target, GLenum internal_format,
    int32_t width, int32_t height, int32_t level, void* data) {
    gl_state_get_all_gl_errors();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    switch (internal_format) {
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    case GL_COMPRESSED_RED_RGTC1_EXT:
    case GL_COMPRESSED_RED_GREEN_RGTC2_EXT: {
        int32_t size = texture_get_size(internal_format, width, height);
        glCompressedTexImage2D(target, level, internal_format, width, height, 0, size, data);
    } break;
    default: {
        GLenum format;
        GLenum type;
        texture_get_format_type_by_internal_format(internal_format, &format, &type);
        glTexImage2D(target, level, internal_format, width, height, 0, format, type, data);
    } break;
    }
    return -(glGetError() != GL_ZERO);
}

static texture* texture_load_tex(GLenum target,
    GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy) {
    texture* tex = force_malloc(sizeof(texture));
    glGenTextures(1, &tex->texture);
    switch (target) {
    case GL_TEXTURE_2D:
        gl_state_bind_texture_2d(tex->texture);
        break;
    case GL_TEXTURE_CUBE_MAP:
        gl_state_bind_texture_cube_map(tex->texture);
        break;
    }
    texture_set_params(target, max_mipmap_level, use_high_anisotropy);

    switch (internal_format) {
    case GL_ALPHA8:
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]){ GL_ZERO , GL_ZERO , GL_ZERO, GL_RED   });
        internal_format = GL_R8;
        break;
    case GL_COMPRESSED_RED_RGTC1_EXT:
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED , GL_RED  , GL_RED , GL_ONE   });
        break;
    case GL_COMPRESSED_RED_GREEN_RGTC2_EXT:
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED , GL_GREEN, GL_ZERO, GL_RED   });
        break;
    case GL_LUMINANCE8:
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED , GL_RED  , GL_RED , GL_ONE   });
        internal_format = GL_R8;
        break;
    case GL_LUMINANCE8_ALPHA8:
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED , GL_RED  , GL_RED , GL_GREEN });
        internal_format = GL_RG8;
        break;
    }

    int32_t size = 0;
    if (target == GL_TEXTURE_CUBE_MAP)
        for (int32_t i = 0; i < 6; i++) {
            for (int32_t j = 0; j <= max_mipmap_level; j++) {
                int32_t mip_width = max(width >> i, 1);
                int32_t mip_height = max(height >> i, 1);
                void* data;
                if (data_ptr)
                    data = data_ptr[i * (max_mipmap_level + 1) + j];
                else
                    data = 0;

                if (texture_load(target_cube_map_array[i],
                    internal_format, mip_width, mip_height, i, data) < 0)
                    goto fail;
                size += texture_get_size(internal_format, mip_width, mip_height);
            }
        }
    else
        for (int32_t i = 0; i <= max_mipmap_level; i++) {
            int32_t mip_width = max(width >> i, 1);
            int32_t mip_height = max(height >> i, 1);
            void* data;
            if (data_ptr)
                data = data_ptr[i];
            else
                data = 0;

            if (texture_load(target, internal_format,
                mip_width, mip_height, i, data) < 0)
                goto fail;
            size += texture_get_size(internal_format, mip_width, mip_height);
        }

    switch (target) {
    case GL_TEXTURE_2D:
        gl_state_bind_texture_2d(0);
        break;
    case GL_TEXTURE_CUBE_MAP:
        gl_state_bind_texture_cube_map(0);
        break;
    }

    tex->target = target;
    tex->width = (int16_t)width;
    tex->height = (int16_t)height;
    tex->size = size;
    tex->internal_format = internal_format;
    tex->max_mipmap_level = max_mipmap_level;

    switch (internal_format) {
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    case GL_COMPRESSED_RED_RGTC1_EXT:
    case GL_COMPRESSED_RED_GREEN_RGTC2_EXT:
        tex->flags |= TEXTURE_BLOCK_COMPRESSION;
        break;
    default:
        tex->flags &= ~TEXTURE_BLOCK_COMPRESSION;
        break;
    }
    return tex;

fail:
    switch (target) {
    case GL_TEXTURE_2D:
        gl_state_bind_texture_2d(0);
        break;

    case GL_TEXTURE_CUBE_MAP:
        gl_state_bind_texture_cube_map(0);
        break;
    }
    glDeleteTextures(1, &tex->texture);
    free(tex);
    return 0;
}

static void texture_set_params(GLenum target, int32_t max_mipmap_level, bool use_high_anisotropy) {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, (float_t*)&vec4_null);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum min_filter = GL_LINEAR;
    if (max_mipmap_level > 0)
        min_filter = GL_LINEAR_MIPMAP_LINEAR;
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, max_mipmap_level);

    float_t max_anisotropy;
    if (use_high_anisotropy)
        max_anisotropy = 16.0;
    else
        max_anisotropy = 1.0;
    glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
}

static GLenum texture_txp_get_internal_format(txp* t) {
    if (!t || !t->data.begin)
        return GL_ZERO;

    switch (t->data.begin->format) {
    case TXP_A8:
        return GL_ALPHA8;
    case TXP_RGB8:
        return GL_RGB8;
    case TXP_RGBA8:
        return GL_RGBA8;
    case TXP_RGB5:
        return GL_RGB5;
    case TXP_RGB5A1:
        return GL_RGB5_A1;
    case TXP_RGBA4:
        return GL_RGBA4;
    case TXP_DXT1:
        return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case TXP_DXT1a:
        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case TXP_DXT3:
        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case TXP_DXT5:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case TXP_ATI1:
        return GL_COMPRESSED_RED_RGTC1_EXT;
    case TXP_ATI2:
        return GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
    case TXP_L8:
        return GL_LUMINANCE8;
    case TXP_L8A8:
        return GL_LUMINANCE8_ALPHA8;
    default:
        return GL_ZERO;
    }
}
