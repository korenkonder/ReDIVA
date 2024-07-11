/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light_param.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "render_context.hpp"
#include "stage_param.hpp"

struct light_param_data_storage;

struct light_param_data {
    std::string name;
    std::string paths[6];
    bool pv;
    light_param_light light;
    light_param_fog fog;
    light_param_glow glow;
    light_param_ibl ibl;
    light_param_wind wind;
    light_param_face face;

    light_param_data();
    ~light_param_data();

    int32_t load_file(p_file_handler file_handlers[6]);
    void load_pv_cut_file_names(int32_t cut_id, const std::string& name);
    void load_stage_file_names(const int32_t& stage_index, const std::string& name);
    void set(light_param_data_storage* storage,
        light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);

    static void load_file_pv_cut(std::map<int32_t, light_param_data>& tree,
        farc* f, light_param_data* default_light_param, int32_t pv_id);

    static void set_face(const light_param_face* face);
    static void set_glow(const light_param_glow* glow);
    static void set_fog(const light_param_fog* f);
    static void set_ibl(const light_param_ibl* ibl, const light_param_data_storage* storage);
    static void set_ibl_diffuse(const light_param_ibl_diffuse* diffuse);
    static void set_ibl_specular(const light_param_ibl_specular* specular);
    static void set_light(const light_param_light* light);
    static void set_wind(const light_param_wind* w);
};

struct light_param_data_storage {
    GLuint textures[5];
    int32_t state;
    std::string name;
    int32_t stage_index;
    light_param_data default_light_param;
    std::map<int32_t, light_param_data> stage;
    std::map<int32_t, light_param_data>::iterator current_light_param;
    std::map<int32_t, light_param_data> pv_cut;
    p_file_handler file_handlers[6];
    p_file_handler farc_file_handler;
    int32_t pv_id;

    light_param_data_storage();
    ~light_param_data_storage();

    void free_file_handlers();
    std::string get_name();
    void load_stages(const std::vector<int32_t>& stage_indices);
    void load_stages(const std::vector<uint32_t>& stage_hashes, stage_database* stage_data);
    int32_t load_file(bool read_now);
    void reset();
    void set_default_light_param(int32_t stage_index);
    void set_pv_cut(std::map<int32_t, light_param_data>::iterator* elem, int32_t cut_id,
        light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);
    void set_stage(std::map<int32_t, light_param_data>::iterator* elem, int32_t stage_index,
        light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);
};

static void light_param_data_get_stage_name_string(std::string& str, int32_t stage_index);
static void light_param_data_get_stage_name_string(std::string& str,
    uint32_t stage_hash, stage_database* stage_data);

light_param_data_storage* light_param_data_storage_data;

extern render_context* rctx_ptr;

int32_t light_param_data_load_file() {
    return light_param_data_storage_data->load_file(false);
}

void light_param_data_storage_data_init() {
    light_param_data_storage_data = new light_param_data_storage;
}

#if DISPLAY_IBL
GLuint light_param_data_storage_data_get_ibl_texture(int32_t index) {
    return light_param_data_storage_data->textures[index];
}
#endif
 
std::string light_param_data_storage_data_get_name() {
    return light_param_data_storage_data->get_name();
}

int32_t light_param_data_storage_data_get_pv_id() {
    return light_param_data_storage_data->pv_id;
}

int32_t light_param_data_storage_data_get_stage_index() {
    return light_param_data_storage_data->stage_index;
}

void light_param_data_storage_data_free_file_handlers() {
    light_param_data_storage_data->free_file_handlers();
}

void light_param_data_storage_data_load() {
    glGenTextures(5, light_param_data_storage_data->textures);
    light_param_data_storage_data_load_stage(0);
    while (light_param_data_storage_data->load_file(true));
    light_param_data_storage_data->set_default_light_param(0);
}

int32_t light_param_data_storage_data_load_file() {
    return light_param_data_storage_data->load_file(false);
}

void light_param_data_storage_data_load_stage(int32_t stage_index) {
    std::vector<int32_t> stage_indices = { stage_index };
    light_param_data_storage_data->load_stages(stage_indices);
}

void light_param_data_storage_data_load_stage(
    uint32_t stage_hash, stage_database* stage_data) {
    std::vector<uint32_t> stage_hashes = { stage_hash };
    light_param_data_storage_data->load_stages(stage_hashes, stage_data);
}

void light_param_data_storage_data_load_stages(std::vector<int32_t>& stage_indices) {
    light_param_data_storage_data->load_stages(stage_indices);
}

void light_param_data_storage_data_load_stages(
    std::vector<uint32_t>& stage_hashes, stage_database* stage_data) {
    light_param_data_storage_data->load_stages(stage_hashes, stage_data);
}

void light_param_data_storage_data_reset() {
    light_param_data_storage_data->reset();
}

void light_param_data_storage_data_set_default_light_param(light_param_data_storage_flags flags) {
    light_param_data_storage_data->default_light_param.set(light_param_data_storage_data, flags);
}

void light_param_data_storage_data_set_ibl() {
    static const int32_t ibl_texture_index[] = {
        9, 10, 11, 12, 13
    };

    for (int32_t i = 0; i < 5; i++)
        gl_state_active_bind_texture_cube_map(ibl_texture_index[i],
            light_param_data_storage_data->textures[i]);
}

void light_param_data_storage_data_set_pv_id(int32_t pv_id) {
    light_param_data_storage_data->pv_id = pv_id;
}

void light_param_data_storage_data_set_pv_cut(int32_t cut_id,
    light_param_data_storage_flags flags) {
    std::map<int32_t, light_param_data>::iterator elem;
    light_param_data_storage_data->set_pv_cut(&elem, cut_id);
}

void light_param_data_storage_data_set_stage(int32_t stage_id,
    light_param_data_storage_flags flags) {
    std::map<int32_t, light_param_data>::iterator elem;
    light_param_data_storage_data->set_stage(&elem, stage_id);
}

void light_param_data_storage_data_unload() {
    glDeleteTextures(5, light_param_data_storage_data->textures);
}

void light_param_data_storage_data_free() {
    delete light_param_data_storage_data;
}

light_param_data::light_param_data() : pv() {

}

light_param_data::~light_param_data() {

}

int32_t light_param_data::load_file(p_file_handler file_handlers[6]) {
    if (file_handlers[0].ptr) {
        ibl = light_param_ibl();
        ibl.read(file_handlers[0].get_data(), file_handlers[0].get_size());
    }

    if (file_handlers[1].ptr) {
        light = light_param_light();
        light.read(file_handlers[1].get_data(), file_handlers[1].get_size());
    }

    if (file_handlers[2].ptr) {
        fog = light_param_fog();
        fog.read(file_handlers[2].get_data(), file_handlers[2].get_size());
    }

    if (file_handlers[3].ptr) {
        glow = light_param_glow();
        glow.read(file_handlers[3].get_data(), file_handlers[3].get_size());
    }

    if (file_handlers[4].ptr) {
        wind = light_param_wind();
        wind.read(file_handlers[4].get_data(), file_handlers[4].get_size());
    }

    if (file_handlers[5].ptr) {
        face = light_param_face();
        face.read(file_handlers[5].get_data(), file_handlers[5].get_size());
    }

    return !(ibl.ready | light.ready | fog.ready | glow.ready | wind.ready | face.ready) ? 1 : 0;
}

void light_param_data::load_pv_cut_file_names(int32_t cut_id, const std::string& name) {
    this->name.assign(sprintf_s_string("%s_c%03d", name.c_str(), cut_id));

    paths[0].assign("rom/ibl/");
    paths[0].append(sprintf_s_string("%s.ibl", this->name.c_str()));

    paths[1].assign("rom/light_param/");
    paths[1].append(sprintf_s_string("light_%s.txt", this->name.c_str()));

    paths[2].assign("rom/light_param/");
    paths[2].append(sprintf_s_string("fog_%s.txt", this->name.c_str()));

    paths[3].assign("rom/light_param/");
    paths[3].append(sprintf_s_string("glow_%s.txt", this->name.c_str()));

    paths[4].assign("rom/light_param/");
    paths[4].append(sprintf_s_string("wind_%s.txt", this->name.c_str()));

    paths[5].assign("rom/light_param/");
    paths[5].append(sprintf_s_string("face_%s.txt", this->name.c_str()));
}

void light_param_data::load_stage_file_names(const int32_t& stage_index, const std::string& name) {
    this->name.assign(name);

    paths[0].assign("rom/ibl/");
    paths[0].append(sprintf_s_string("%s.ibl", this->name.c_str()));

    paths[1].assign("rom/light_param/");
    paths[1].append(sprintf_s_string("light_%s.txt", this->name.c_str()));

    paths[2].assign("rom/light_param/");
    paths[2].append(sprintf_s_string("fog_%s.txt", this->name.c_str()));

    paths[3].assign("rom/light_param/");
    paths[3].append(sprintf_s_string("glow_%s.txt", this->name.c_str()));

    paths[4].assign("rom/light_param/");
    paths[4].append(sprintf_s_string("wind_%s.txt", this->name.c_str()));

    paths[5].assign("rom/light_param/");
    paths[5].append(sprintf_s_string("face_%s.txt", this->name.c_str()));

    stage_param_data_coli_data_get_stage_wind_file_path(paths[4], stage_index);
}

void light_param_data::set(light_param_data_storage* storage, light_param_data_storage_flags flags) {
    if ((!pv || light.ready) && (flags & LIGHT_PARAM_DATA_STORAGE_LIGHT))
        light_param_data::set_light(&light);
    if ((!pv || fog.ready) && (flags & LIGHT_PARAM_DATA_STORAGE_FOG))
        light_param_data::set_fog(&fog);
    if ((!pv || glow.ready) && (flags & LIGHT_PARAM_DATA_STORAGE_GLOW))
        light_param_data::set_glow(&glow);
    if ((!pv || ibl.ready) && (flags & LIGHT_PARAM_DATA_STORAGE_IBL))
        light_param_data::set_ibl(&ibl, storage);
    if (!pv) {
        if (flags & LIGHT_PARAM_DATA_STORAGE_WIND)
            light_param_data::set_wind(&wind);
        if (face.ready && (flags & LIGHT_PARAM_DATA_STORAGE_FACE))
            light_param_data::set_face(&face);
    }
}

void light_param_data::load_file_pv_cut(std::map<int32_t, light_param_data>& tree,
    farc* f, light_param_data* default_light_param, int32_t pv_id) {
    char buf[0x100];

    sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);

    std::string pv_str = buf;

    for (farc_file& i : f->files) {
        const char* name = i.name.c_str();
        size_t name_len = i.name.size();

        int32_t type = 0;
        if (!strncmp(name, "light", min_def(name_len, 5)) && name_len > 6 && name[5] == '_') {
            type = 1;
            name += 6;
            name_len -= 6;
        }
        else if (!strncmp(name, "fog", min_def(name_len, 3)) && name_len > 4 && name[3] == '_') {
            type = 2;
            name += 4;
            name_len -= 4;
        }
        else if (!strncmp(name, "glow", min_def(name_len, 4)) && name_len > 5 && name[4] == '_') {
            type = 3;
            name += 5;
            name_len -= 5;
        }
        else if (!strncmp(name, "wind", min_def(name_len, 4)) && name_len > 5 && name[4] == '_') {
            type = 4;
            name += 5;
            name_len -= 5;
        }
        else if (!strncmp(name, "face", min_def(name_len, 4)) && name_len > 5 && name[4] == '_') {
            type = 5;
            name += 5;
            name_len -= 5;
        }
        else
            continue;

        char* b = buf;
        const char* n = name;
        size_t s = 0;
        bool ret = false;
        do {
            char c = *n++;
            if (c == '_' || c == '.')
                break;
            else if (!c) {
                ret = true;
                break;
            }

            *b++ = c;
            s++;
        } while (*n);

        if (ret)
            continue;

        *b++ = 0;

        b = buf;
        if (s < 5 || b[0] != 'p' || b[1] != 'v' || b[2] < '0' && b[2] > '9'
            || b[3] < '0' && b[3] > '9' || b[4] < '0' && b[4] > '9')
            continue;

        int32_t pv_id = (b[2] - '0') * 100;
        pv_id += (b[3] - '0') * 10;
        pv_id += b[4] - '0';

        s = 0;
        ret = false;
        do {
            char c = *n++;
            if (c == '_' || c == '.')
                break;
            else if (!c) {
                ret = true;
                break;
            }

            *b++ = c;
            s++;
        } while (*n);

        if (ret)
            continue;

        *b++ = 0;

        b = buf;
        if (s != 4 || b[0] != 'c' || b[1] < '0' && b[1] > '9'
            || b[2] < '0' && b[2] > '9' || b[3] < '0' && b[3] > '9')
            continue;

        int32_t cut_id = (b[1] - '0') * 100;
        cut_id += (b[2] - '0') * 10;
        cut_id += b[3] - '0';

        std::map<int32_t, light_param_data>::iterator elem = tree.find(cut_id);
        light_param_data* light_param;
        if (elem != tree.end())
            light_param = &elem->second;
        else {
            light_param = &tree.insert({ cut_id, *default_light_param }).first->second;
            light_param->ibl.ready = false;
            light_param->light.ready = false;
            light_param->fog.ready = false;
            light_param->glow.ready = false;
            light_param->wind.ready = false;
            light_param->face.ready = false;
            light_param->pv = true;
            light_param->load_pv_cut_file_names(cut_id, pv_str);
        }

        if (type == 1) {
            light_param->light = light_param_light();
            light_param->light.read(i.data, i.size);
        }
        else if (type == 2) {
            light_param->fog = light_param_fog();
            light_param->fog.read(i.data, i.size);
        }
        else if (type == 3) {
            light_param->glow = light_param_glow();
            light_param->glow.read(i.data, i.size);
        }
        else if (type == 4) {
            light_param->wind = light_param_wind();
            light_param->wind.read(i.data, i.size);
        }
        else if (type == 5) {
            light_param->face = light_param_face();
            light_param->face.read(i.data, i.size);
        }
    }
}

void light_param_data::set_face(const light_param_face* face) {
    rctx_ptr->face.set_offset(face->offset);
    rctx_ptr->face.set_scale(face->scale);
    rctx_ptr->face.set_position(face->position);
    rctx_ptr->face.set_direction(face->direction);
}

void light_param_data::set_fog(const light_param_fog* f) {
    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++) {
        const light_param_fog_group* group = &f->group[i];
        ::fog* fog = &rctx_ptr->fog[i];

        if (group->has_type)
            fog->set_type(group->type);

        if (group->has_density)
            fog->set_density(group->density);

        if (group->has_linear) {
            fog->set_start(group->linear_start);
            fog->set_end(group->linear_end);
        }

        if (group->has_color)
            fog->set_color(group->color);
    }
}

void light_param_data::set_glow(const light_param_glow* glow) {
    rndr::Render* rend = &rctx_ptr->render;

    rend->set_auto_exposure(true);
    rend->set_tone_map(TONE_MAP_YCC_EXPONENT);
    rend->reset_saturate_coeff(0, false);
    rend->reset_scene_fade(0);
    rend->reset_tone_trans(0);

    if (glow->has_exposure)
        rend->set_exposure(glow->exposure);

    if (glow->has_gamma)
        rend->set_gamma(glow->gamma);

    if (glow->has_saturate_power)
        rend->set_saturate_power(glow->saturate_power);

    if (glow->has_saturate_coef)
        rend->set_saturate_coeff(glow->saturate_coef, 0, false);

    if (glow->has_flare)
        rend->set_lens(glow->flare);

    if (glow->has_sigma)
        rend->set_radius(glow->sigma);

    if (glow->has_intensity)
        rend->set_intensity(glow->intensity);

    if (glow->has_auto_exposure)
        rend->set_auto_exposure(glow->auto_exposure);

    if (glow->has_tone_map_method)
        rend->set_tone_map(glow->tone_map_method);

    if (glow->has_fade_color) {
        vec4 fade_color = glow->fade_color;
        rend->set_scene_fade(fade_color, 0);
        rend->set_scene_fade_blend_func(glow->fade_color_blend_func, 0);
    }

    if (glow->has_tone_transform)
        rend->set_tone_trans(glow->tone_transform_start, glow->tone_transform_end, 0);
}

void light_param_data::set_ibl(const light_param_ibl* ibl, const light_param_data_storage* storage) {
    if (!ibl->ready)
        return;

    for (int32_t i = 0, j = -1; i < 5; i++, j++) {
        gl_state_bind_texture_cube_map(storage->textures[i]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        if (!i) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            light_param_data::set_ibl_diffuse(&ibl->diffuse[0]);
            light_param_data::set_ibl_diffuse(&ibl->diffuse[1]);
        }
        else {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, ibl->specular[j].max_level);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, 0.0f);
            light_param_data::set_ibl_specular(&ibl->specular[j]);
        }
    }
    gl_state_bind_texture_cube_map(0);

    ::light_set* set = &rctx_ptr->light_set[LIGHT_SET_MAIN];
    set->set_irradiance(ibl->diff_coef[1][0], ibl->diff_coef[1][1], ibl->diff_coef[1][2]);

    float_t len;
    vec3 pos;

    light_data* l = &set->lights[LIGHT_CHARA];
    l->get_position(pos);
    len = vec3::length(pos);
    if (fabsf(len - 1.0f) < 0.02f)
        l->set_position(ibl->lit_dir[0]);

    l->set_ibl_color0(ibl->lit_col[0]);
    l->set_ibl_color1(ibl->lit_col[2]);
    l->set_ibl_direction(ibl->lit_dir[0]);

    l = &set->lights[LIGHT_STAGE];
    l->get_position(pos);
    len = vec3::length(pos);
    if (fabsf(len - 1.0f) < 0.02f)
        l->set_position(ibl->lit_dir[1]);

    l->set_ibl_color0(ibl->lit_col[1]);
    l->set_ibl_direction(ibl->lit_dir[1]);
}

void light_param_data::set_ibl_diffuse(const light_param_ibl_diffuse* diffuse) {
    if (!diffuse->data.size())
        return;

    int32_t size = diffuse->size;
    const int32_t level = diffuse->level;
    const half_t* data = diffuse->data.data();
    size_t data_size = diffuse->data.size() / 6;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 3]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 5]);
}

void light_param_data::set_ibl_specular(const light_param_ibl_specular* specular) {
    int32_t size = specular->size;
    int32_t max_level = specular->max_level;
    for (int32_t i = 0; i <= max_level; i++, size /= 2) {
        const half_t* data = specular->data[i].data();
        size_t data_size = specular->data[i].size() / 6;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 2]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 3]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 4]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 5]);
    }
}

void light_param_data::set_light(const light_param_light* light) {
    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        const light_param_light_group* group = &light->group[i];
        ::light_set* set = &rctx_ptr->light_set[i];

        for (int32_t j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            const light_param_light_data* data = &group->data[j];
            light_data* light = &set->lights[j];

            if (data->has_type)
                light->set_type(data->type);

            if (data->has_ambient)
                light->set_ambient(data->ambient);

            if (data->has_diffuse)
                light->set_diffuse(data->diffuse);

            if (data->has_specular)
                light->set_specular(data->specular);

            if (data->has_position)
                light->set_position(data->position);

            if (data->has_spot_direction)
                light->set_spot_direction(data->spot_direction);

            if (data->has_spot_exponent)
                light->set_spot_exponent(data->spot_exponent);

            if (data->has_spot_cutoff)
                light->set_spot_cutoff(data->spot_cutoff);

            if (data->has_attenuation)
                light->set_attenuation(data->attenuation);

            light->set_clip_plane(data->clip_plane);

            if (data->has_tone_curve)
                light->set_tone_curve(data->tone_curve);
        }
    }
}

void light_param_data::set_wind(const light_param_wind* w) {
    ::wind* wind = task_wind->ptr;
    if (w->has_scale)
        wind->scale = w->scale;

    if (w->has_cycle)
        wind->cycle = w->cycle;

    if (w->has_rot) {
        wind->rot_y = w->rot_y;
        wind->rot_z = w->rot_z;
    }

    if (w->has_bias)
        wind->bias = w->bias;

    for (int32_t i = 0; i < 16; i++)
        if (w->has_spc[i]) {
            wind->spc[i].cos = w->spc[i].cos;
            wind->spc[i].sin = w->spc[i].sin;
        }
}

light_param_data_storage::light_param_data_storage() : textures(), state(), stage_index(), pv_id() {

}

light_param_data_storage::~light_param_data_storage() {

}

void light_param_data_storage::free_file_handlers() {
    for (int32_t i = 0; i < 6; ++i)
        file_handlers[i].reset();
    farc_file_handler.reset();
}

std::string light_param_data_storage::get_name() {
    return name;
}

void light_param_data_storage::load_stages(const std::vector<int32_t>& stage_indices) {
    if (state)
        for (int32_t i = 0; i < 6; ++i)
            if (file_handlers[i].check_not_ready())
                file_handlers[i].call_free_callback();

    stage_index = -1;
    stage.clear();

    for (const int32_t& i : stage_indices) {
        std::string name;
        light_param_data_get_stage_name_string(name, i);
        if (!name.size())
            continue;

        std::map<int32_t, light_param_data>::iterator elem = stage.find(i);
        light_param_data* light_param;
        if (elem != stage.end())
            light_param = &elem->second;
        else
            light_param = &stage.insert({ i, default_light_param }).first->second;

        light_param->load_stage_file_names(i, name);
        light_param->pv = false;
    }
    current_light_param = stage.begin();
    state = stage.size() ? 1 : 0;
}

void light_param_data_storage::load_stages(const std::vector<uint32_t>& stage_hashes, stage_database* stage_data) {
    if (state)
        for (int32_t i = 0; i < 6; ++i)
            if (file_handlers[i].check_not_ready())
                file_handlers[i].call_free_callback();

    stage_index = -1;
    stage.clear();

    for (const uint32_t& i : stage_hashes) {
        std::string name;
        light_param_data_get_stage_name_string(name, i, stage_data);
        if (!name.size())
            continue;

        size_t name_len = name.size();
        if (!strncmp(name.c_str(), "pv0", min_def(name_len, 3)))
            name[2] = '8';

        std::map<int32_t, light_param_data>::iterator elem = stage.find(i);
        light_param_data* light_param;
        if (elem != stage.end())
            light_param = &elem->second;
        else
            light_param = &stage.insert({ i, default_light_param }).first->second;

        light_param->load_stage_file_names(i, name);
        light_param->pv = false;
    }
    current_light_param = stage.begin();
    state = stage.size() ? 1 : 0;
}

int32_t light_param_data_storage::load_file(bool read_now) {
    if (state == 1) {
        light_param_data* light_param = &current_light_param->second;
        std::string* paths = light_param->paths;
        for (int32_t i = 0; i < 6; i++)
            file_handlers[i].read_file(&data_list[DATA_AFT], paths[i].c_str());
        state = 2;
        return 1;
    }
    else if (state == 2) {
        for (int32_t i = 0; i < 6; ++i)
            if (read_now)
                file_handlers[i].read_now();
            else if (file_handlers[i].check_not_ready())
                return 1;

        int32_t load_error = current_light_param->second.load_file(file_handlers);

        for (int32_t i = 0; i < 6; i++)
            file_handlers[i].reset();

        current_light_param++;

        if (current_light_param == stage.end())
            state = 3;
        else
            state = 1;

        if (load_error) {
            for (int32_t i = 0; i < 6; i++)
                file_handlers[i].call_free_callback();
            state = 0;
            return 0;
        }
        return 1;
    }
    else if (state == 3) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "light_pv%03d.farc", pv_id);
        if (farc_file_handler.read_file(&data_list[DATA_AFT], "rom/light_param/", buf))
            state = 4;
        else {
            farc_file_handler.reset();
            state = 0;
            return 0;
        }
        return 1;
    }
    else if (state == 4) {
        if (read_now)
            farc_file_handler.read_now();
        else if (farc_file_handler.check_not_ready())
            return 1;

        farc a1a;
        a1a.read(farc_file_handler.get_data(), farc_file_handler.get_size(), true);
        if (a1a.files.size())
            light_param_data::load_file_pv_cut(pv_cut, &a1a, &default_light_param, pv_id);
        state = 0;
        return 0;
    }
    return 0;
}

void light_param_data_storage::reset() {
    stage.clear();
    pv_cut.clear();
    pv_id = 0;
    for (p_file_handler& i : file_handlers)
        i.reset();
    farc_file_handler.reset();
}

void light_param_data_storage::set_default_light_param(int32_t stage_index) {
    std::map<int32_t, light_param_data>::iterator elem;
    set_stage(&elem, stage_index);
    if (elem != stage.end())
        default_light_param = elem->second;
}

void light_param_data_storage::set_pv_cut(std::map<int32_t, light_param_data>::iterator* elem, int32_t cut_id,
    light_param_data_storage_flags flags) {
    *elem = pv_cut.find(cut_id);
    if (*elem != pv_cut.end())
        (*elem)->second.set(this);
}

void light_param_data_storage::set_stage(std::map<int32_t, light_param_data>::iterator* elem, int32_t stage_index,
    light_param_data_storage_flags flags) {
    *elem = stage.find(stage_index);
    if (*elem != stage.end()) {
        this->stage_index = stage_index;
        name = (*elem)->second.name;
        (*elem)->second.set(this);
    }
}

static void light_param_data_get_stage_name_string(std::string& str, int32_t stage_index) {
    str.clear();

    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    const char* stage_name = aft_stage_data->get_stage_name(stage_index);
    if (!stage_name)
        return;

    size_t length = utf8_length(stage_name);
    if (length <= 3)
        return;

    for (size_t i = 3; i < length; i++)
        if (stage_name[i] == '_') {
            length = i;
            break;
        }

    str.assign(stage_name + 3, length - 3);
    for (char& c : str)
        if (c >= 'A' && c <= 'Z')
            c += 0x20;
}

static void light_param_data_get_stage_name_string(std::string& str,
    uint32_t stage_hash, stage_database* stage_data) {
    str.clear();

    const char* stage_name = stage_data->get_stage_name_modern(stage_hash);
    if (!stage_name)
        return;

    size_t length = utf8_length(stage_name);
    if (length <= 3)
        return;

    for (size_t i = 3; i < length; i++)
        if (stage_name[i] == '_') {
            length = i;
            break;
        }

    str.assign(stage_name + 3, length - 3);
    for (char& c : str)
        if (c >= 'A' && c <= 'Z')
            c += 0x20;
}
