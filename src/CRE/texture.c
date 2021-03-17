/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "static_var.h"

void texture_1d_load(texture* tex, texture_data* data) {
    if (!data->data || data->width < 1)
        return;

    glBindTexture(GL_TEXTURE_1D, tex->id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage1D(GL_TEXTURE_1D, 0, data->pixel_internal_format, data->width,
        0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void texture_1d_update(texture* tex) {
    glBindTexture(GL_TEXTURE_1D, tex->id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void texture_2d_load(texture* tex, texture_data* data) {
    if (!data->data || data->width < 1 || data->height < 1)
        return;

    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data->wrap_mode_t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage2D(GL_TEXTURE_2D, 0, data->pixel_internal_format, data->width, data->height,
        0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_2d_update(texture* tex) {
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_3d_load(texture* tex, texture_data* data) {
    if (!data->data)
        return;

    glBindTexture(GL_TEXTURE_3D, tex->id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, data->wrap_mode_t);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, data->wrap_mode_r);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage3D(GL_TEXTURE_3D, 0, data->pixel_internal_format, data->width, data->height,
        data->depth, 0, data->pixel_format, data->pixel_type, data->data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void texture_3d_update(texture* tex) {
    glBindTexture(GL_TEXTURE_3D, tex->id);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void texture_cube_load(texture* tex, texture_cube_data* data) {
    if (!data->px.data || !data->nx.data || !data->py.data || !data->ny.data || !data->pz.data || !data->nz.data)
        return;

    glBindTexture(GL_TEXTURE_CUBE_MAP, tex->id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, data->wrap_mode_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, data->wrap_mode_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, data->wrap_mode_r);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, data->min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, data->mag_filter);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, data->pixel_internal_format, data->px.width,
        data->px.height, 0, data->px.pixel_format, data->px.pixel_type, data->px.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, data->pixel_internal_format, data->nx.width,
        data->nx.height, 0, data->nx.pixel_format, data->nx.pixel_type, data->nx.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, data->pixel_internal_format, data->py.width,
        data->py.height, 0, data->py.pixel_format, data->py.pixel_type, data->py.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, data->pixel_internal_format, data->ny.width,
        data->ny.height, 0, data->ny.pixel_format, data->ny.pixel_type, data->ny.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, data->pixel_internal_format, data->pz.width,
        data->pz.height, 0, data->pz.pixel_format, data->pz.pixel_type, data->pz.data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, data->pixel_internal_format, data->nz.width,
        data->nz.height, 0, data->nz.pixel_format, data->nz.pixel_type, data->nz.data);
    if (data->generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void texture_cube_update(texture* tex) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex->id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, sv_anisotropy);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void texture_load(texture* tex, texture_data* data) {
    if (tex->id == 0)
        glGenTextures(1, &tex->id);

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
    GLenum target = 0;
    switch (tex->type) {
    case TEXTURE_1D:
        target = GL_TEXTURE_1D;
        break;
    case TEXTURE_2D:
        target = GL_TEXTURE_2D;
        break;
    case TEXTURE_3D:
        target = GL_TEXTURE_3D;
        break;
    case TEXTURE_CUBE:
        target = GL_TEXTURE_CUBE_MAP;
        break;
    }

    if (target) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(target, tex->id);
    }
}

void texture_reset(texture* tex, int32_t index) {
    GLenum target = 0;
    switch (tex->type) {
    case TEXTURE_1D:
        target = GL_TEXTURE_1D;
        break;
    case TEXTURE_2D:
        target = GL_TEXTURE_2D;
        break;
    case TEXTURE_3D:
        target = GL_TEXTURE_3D;
        break;
    case TEXTURE_CUBE:
        target = GL_TEXTURE_CUBE_MAP;
        break;
    }

    if (target) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(target, 0);
    }
}

void texture_update(texture* tex) {
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
    if (tex->id)
        glDeleteTextures(1, &tex->id);
    tex->id = 0;
}

void texture_set_load(texture_set* tex, texture_set_data* data) {
    for (int32_t i = 0; i < 8; i++)
        texture_load(&tex->tex[i], data->tex[i]);
}

void texture_set_bind(texture_set* tex) {
    for (int32_t i = 0; i < 8; i++)
        texture_bind(&tex->tex[i], i);
}

void texture_set_reset(texture_set* tex) {
    for (int32_t i = 0; i < 8; i++)
        texture_reset(&tex->tex[i], i);
}

void texture_set_update(texture_set* tex) {
    for (int32_t i = 0; i < 8; i++)
        texture_update(&tex->tex[i]);
}

void texture_set_free(texture_set* tex) {
    for (int32_t i = 0; i < 8; i++)
        texture_free(&tex->tex[i]);
}
