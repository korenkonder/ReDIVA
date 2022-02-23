/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/txp.h"
#include "../KKdLib/vector.h"
#include <glad/glad.h>

typedef enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x1,
} texture_flags;

typedef struct texture {
    int32_t init_count;
    int32_t id;
    texture_flags flags;
    int16_t width;
    int16_t height;
    GLuint texture;
    GLenum target;
    GLenum internal_format;
    int32_t max_mipmap_level;
    int32_t size;
} texture;

vector_old(texture)
vector_old_ptr(texture)

#define texture_make_id(id, index) ((uint32_t)(((id) << 24) | ((index) & 0xFFFFFF)))
#define texture_get_id(id) (((id) >> 24) & 0xFF)
#define texture_get_index(id) ((id) & 0xFFFFFF)

extern texture* texture_init(uint32_t id);
extern texture* texture_load_tex_2d(uint32_t id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy);
extern texture* texture_load_tex_cube_map(uint32_t id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr);
extern texture* texture_txp_load(txp* t, uint32_t id);
extern void texture_free(texture* tex);

extern bool texture_txp_set_load(txp_set* t, texture*** texs, uint32_t* ids);

extern void texture_storage_init();
extern texture* texture_storage_create_texture(uint32_t id);
extern texture* texture_storage_get_texture(uint32_t id);
extern size_t texture_storage_get_texture_count();
extern texture* texture_storage_get_texture_by_index(ssize_t index);
extern void texture_storage_delete_texture(uint32_t id);
extern void texture_storage_free();
