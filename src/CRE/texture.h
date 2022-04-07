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

typedef struct texture_id {
    uint32_t index;
    uint8_t id;

    texture_id();
    texture_id(uint8_t id, uint32_t index);
} texture_id;

inline bool operator >(const texture_id& left, const texture_id& right) {
    return left.id > right.id && left.index > right.index;
}

inline bool operator <(const texture_id& left, const texture_id& right) {
    return left.id < right.id && left.index < right.index;
}

inline bool operator >=(const texture_id& left, const texture_id& right) {
    return left.id >= right.id && left.index >= right.index;
}

inline bool operator <=(const texture_id& left, const texture_id& right) {
    return left.id <= right.id && left.index <= right.index;
}

inline bool operator ==(const texture_id& left, const texture_id& right) {
    return left.id == right.id && left.index == right.index;
}

inline bool operator !=(const texture_id& left, const texture_id& right) {
    return left.id != right.id || left.index != right.index;
}

typedef struct texture {
    int32_t init_count;
    texture_id id;
    texture_flags flags;
    int16_t width;
    int16_t height;
    GLuint tex;
    GLenum target;
    GLenum internal_format;
    int32_t max_mipmap_level;
    int32_t size;

    texture();
} texture;

extern texture* texture_init(texture_id id);
extern void texture_apply_color_tone(texture* chg_tex,
    texture* org_tex, color_tone* col_tone);
extern texture* texture_copy(texture_id id, texture* org_tex);
extern texture* texture_load_tex_2d(texture_id id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy);
extern texture* texture_load_tex_cube_map(texture_id id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr);
extern texture* texture_txp_load(txp* t, texture_id id);
extern void texture_free(texture* tex);

extern bool texture_txp_set_load(txp_set* t, texture*** texs, uint32_t* ids);
extern bool texture_txp_set_load(txp_set* t, texture*** texs, texture_id* ids);

extern void texture_storage_init();
extern texture* texture_storage_create_texture(texture_id id);
extern texture* texture_storage_get_texture(uint32_t id);
extern texture* texture_storage_get_texture(texture_id id);
extern size_t texture_storage_get_texture_count();
extern texture* texture_storage_get_texture_by_index(size_t index);
extern void texture_storage_delete_texture(texture_id id);
extern void texture_storage_free();
