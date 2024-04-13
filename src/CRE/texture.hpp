/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/image.hpp"
#include "../KKdLib/txp.hpp"
#include <glad/glad.h>
#include <map>

enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x01,
};

struct texture_id {
    uint32_t index;
    uint8_t id;

    inline texture_id() {
        id = 0;
        index = 0;
    }

    inline texture_id(uint8_t id, uint32_t index) {
        this->id = id;
        this->index = index;
    }

};

inline bool operator >(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) > (((uint64_t)right.id << 32) | right.index);
}

inline bool operator <(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) < (((uint64_t)right.id << 32) | right.index);
}

inline bool operator >=(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) >= (((uint64_t)right.id << 32) | right.index);
}

inline bool operator <=(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) <= (((uint64_t)right.id << 32) | right.index);
}

inline bool operator ==(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) == (((uint64_t)right.id << 32) | right.index);
}

inline bool operator !=(const texture_id& left, const texture_id& right) {
    return (((uint64_t)left.id << 32) | left.index) != (((uint64_t)right.id << 32) | right.index);
}

struct texture {
    int32_t ref_count;
    texture_id id;
    texture_flags flags;
    int16_t width;
    int16_t height;
    GLuint glid;
    GLenum target;
    GLenum internal_format;
    int32_t max_mipmap_level;
    int32_t size_texmem;

    texture();

    uint32_t get_height_align_mip_level(uint8_t mip_level = 0);
    uint32_t get_width_align_mip_level(uint8_t mip_level = 0);
};

struct texture_manager {
    std::map<texture_id, texture> textures;
    int16_t entry_count;
    int16_t alloc_count;
    int32_t texmem_now_size;
    int32_t texmem_peak_size;
    int32_t texmem_now_size_by_type[4];
    int32_t texmem_peak_size_by_type[4];
    int32_t copy_count;

    texture_manager();
    ~texture_manager();
};

struct texture_param {
    GLint width;
    GLint height;
};

extern texture* texture_alloc(texture_id id);
extern void texture_apply_color_tone(texture* chg_tex,
    texture* org_tex, const color_tone* col_tone);
extern texture* texture_copy(texture_id id, texture* org_tex);
extern texture* texture_load_tex_2d(texture_id id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr, bool use_high_anisotropy);
extern texture* texture_load_tex_cube_map(texture_id id, GLenum internal_format, int32_t width, int32_t height,
    int32_t max_mipmap_level, void** data_ptr);
extern texture* texture_txp_load(txp* t, texture_id id);
extern void texture_txp_store(texture* tex, txp* t);
extern void texture_release(texture* tex);

extern void texture_array_free(texture** arr);

extern void texture_set_params(GLenum target, int32_t max_mipmap_level, bool use_high_anisotropy);

extern void texture_params_get(GLuint tex_0 = 0, texture_param* tex_0_param = 0,
    GLuint tex_1 = 0, texture_param* tex_1_param = 0, GLuint tex_2 = 0, texture_param* tex_2_param = 0);
extern void texture_params_restore(texture_param* tex_0_param = 0,
    texture_param* tex_1_param = 0, texture_param* tex_2_param = 0);

extern bool texture_txp_set_load(txp_set* t, texture*** texs, uint32_t* ids);
extern bool texture_txp_set_load(txp_set* t, texture*** texs, texture_id* ids);

extern void texture_manager_init();
extern texture_id texture_manager_get_copy_id(uint32_t id);
extern texture* texture_manager_get_texture(uint32_t id);
extern texture* texture_manager_get_texture(texture_id id);
extern void texture_manager_free();
