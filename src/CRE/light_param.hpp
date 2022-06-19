/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <map>
#include "../KKdLib/default.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/light_param/face.hpp"
#include "../KKdLib/light_param/fog.hpp"
#include "../KKdLib/light_param/glow.hpp"
#include "../KKdLib/light_param/ibl.hpp"
#include "../KKdLib/light_param/light.hpp"
#include "../KKdLib/light_param/wind.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include <glad/glad.h>

struct light_param_data {
    std::string name;
    std::string paths[6];
    std::string files[6];
    bool pv;
    light_param_light light;
    light_param_fog fog;
    light_param_glow glow;
    light_param_ibl ibl;
    light_param_wind wind;
    light_param_face face;

    light_param_data();
    ~light_param_data();
};

struct light_param_data_storage {
    GLuint textures[5];
    int32_t state;
    std::string name;
    int32_t stage_index;
    light_param_data default_light_param;
    std::map<int32_t, light_param_data> stage;
    std::map<int32_t, light_param_data>* default_stage_node;
    std::map<int32_t, light_param_data> pv_cut;
    std::map<int32_t, light_param_data>::iterator stage_light_param_iterator;
    p_file_handler file_handlers[6];
    p_file_handler farc_file_handler;
    uint32_t pv_id;

    light_param_data_storage();
    virtual ~light_param_data_storage();

    static void load(data_struct* data);
    static void unload();
};

extern void light_param_storage_data_init();
extern void light_param_storage_data_free_file_handlers();
extern int32_t light_param_storage_data_load_file();
extern void light_param_storage_data_load_stage(int32_t stage_index);
extern void light_param_storage_data_load_stage(uint32_t stage_hash, stage_database* stage_data);
extern void light_param_storage_data_load_stages(std::vector<int32_t>* stage_indices);
extern void light_param_storage_data_load_stages(
    std::vector<uint32_t>* stage_hashes, stage_database* stage_data);
extern void light_param_storage_data_reset();
extern void light_param_storage_data_set_default_light_param();
extern void light_param_storage_data_set_pv_id(int32_t pv_id);
extern void light_param_storage_data_set_pv_cut(int32_t cut_id);
extern void light_param_storage_data_set_stage(int32_t stage_id);
extern void light_param_storage_data_free();