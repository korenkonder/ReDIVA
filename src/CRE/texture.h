/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.h"
#include "../KKdLib/image.h"
#include "../KKdLib/txp.h"
#include <glad/glad.h>

typedef enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x01,
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

#define texture_make_id(id, index) ((uint32_t)(((id) << 24) | ((index) & 0xFFFFFF)))
#define texture_get_id(id) (((id) >> 24) & 0xFF)
#define texture_get_index(id) ((id) & 0xFFFFFF)

extern texture* texture_init(uint32_t id);
extern void texture_apply_color_tone(texture* chg_tex,
    texture* org_tex, color_tone* col_tone);
extern texture* texture_copy(uint32_t id, texture* org_tex);
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
extern texture* texture_storage_get_texture_by_index(size_t index);
extern void texture_storage_delete_texture(uint32_t id);
extern void texture_storage_free();
