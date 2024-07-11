/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_param.hpp"
#include "rob/rob.hpp"
#include "data.hpp"
#include "pv_db.hpp"

struct stage_param_data_coli {
    int32_t pv_id;
    std::map<int32_t, osage_ring_data> stage_ring_data;
    std::map<int32_t, osage_ring_data> mhd_id_ring_data;
    std::map<int32_t, std::string> stage_wind_file;

    stage_param_data_coli();
    ~stage_param_data_coli();

    void get_mhd_id_data(const int32_t& mhd_id, osage_ring_data& ring);
    void get_stage_index_data(const int32_t& stage_index, osage_ring_data& ring);
    void get_stage_wind_file_path(std::string& result, const int32_t& stage_index);
    void load(const int32_t& pv_id);
    void reset();
};

stage_param_data_coli stage_param_data_coli_data;

std::map<int32_t, stage_param_fog_ring> stage_param_data_fog_ring_storage;
std::map<int32_t, stage_param_leaf> stage_param_data_leaf_storage;
std::map<int32_t, stage_param_litproj> stage_param_data_litproj_storage;
std::map<int32_t, stage_param_rain> stage_param_data_rain_storage;
std::map<int32_t, stage_param_ripple> stage_param_data_ripple_storage;
std::map<int32_t, stage_param_snow> stage_param_data_snow_storage;
std::map<int32_t, stage_param_splash> stage_param_data_splash_storage;
std::map<int32_t, stage_param_star> stage_param_data_star_storage;

template<typename T>
static T& stage_param_data_storage_get_value(std::map<int32_t, T>& map, int32_t key);

void stage_param_data_init() {
    stage_param_data_coli_data = {};

    stage_param_data_fog_ring_storage = {};
    stage_param_data_leaf_storage = {};
    stage_param_data_litproj_storage = {};
    stage_param_data_rain_storage = {};
    stage_param_data_ripple_storage = {};
    stage_param_data_snow_storage = {};
    stage_param_data_splash_storage = {};
    stage_param_data_star_storage = {};
}

void stage_param_data_free() {
    stage_param_data_coli_data.reset();

    stage_param_data_fog_ring_storage.clear();
    stage_param_data_leaf_storage.clear();
    stage_param_data_litproj_storage.clear();
    stage_param_data_rain_storage.clear();
    stage_param_data_ripple_storage.clear();
    stage_param_data_snow_storage.clear();
    stage_param_data_splash_storage.clear();
    stage_param_data_star_storage.clear();
}

void stage_param_data_coli_data_get_mhd_id_data(
    const int32_t& mhd_id, void* ring) { // osage_ring_data
    stage_param_data_coli_data.get_mhd_id_data(mhd_id, *(osage_ring_data*)ring);
}

int32_t stage_param_data_coli_data_get_pv_id() {
    return stage_param_data_coli_data.pv_id;
}

void stage_param_data_coli_data_get_stage_index_data(
    const int32_t& stage_index, void* ring) { // osage_ring_data
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    const stage_data* stage_data = aft_stage_data->get_stage_data(stage_index);
    if (!stage_data)
        return;

    osage_ring_data& _ring = *(osage_ring_data*)ring;
    stage_param_data_coli_data.get_stage_index_data(stage_index, _ring);
    if (!_ring.init) {
        _ring.rect_x = stage_data->rect_x;
        _ring.rect_y = stage_data->rect_y;
        _ring.rect_width = stage_data->rect_width;
        _ring.rect_height = stage_data->rect_height;
        _ring.ring_height = stage_data->ring_height;
        _ring.out_height = stage_data->out_height;
        _ring.init = true;
    }
}

void stage_param_data_coli_data_get_stage_wind_file_path(std::string& path, const int32_t& stage_index) {
    stage_param_data_coli_data.get_stage_wind_file_path(path, stage_index);
}

void stage_param_data_coli_data_load(const int32_t& pv_id) {
    stage_param_data_coli_data.load(pv_id);
}

void stage_param_data_coli_data_reset() {
    stage_param_data_coli_data.reset();
}

void stage_param_data_fog_ring_storage_clear() {
    stage_param_data_fog_ring_storage.clear();
}

stage_param_fog_ring* stage_param_data_fog_ring_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_fog_ring_storage.find(stage_index);
    if (elem != stage_param_data_fog_ring_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_fog_ring_storage_set_stage_data(int32_t stage_index, stage_param_fog_ring* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_fog_ring>(
            stage_param_data_fog_ring_storage, stage_index) = *value;
}

void stage_param_data_leaf_storage_clear() {
    stage_param_data_leaf_storage.clear();
}

stage_param_leaf* stage_param_data_leaf_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_leaf_storage.find(stage_index);
    if (elem != stage_param_data_leaf_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_leaf_storage_set_stage_data(int32_t stage_index, stage_param_leaf* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_leaf>(
            stage_param_data_leaf_storage, stage_index) = *value;
}

void stage_param_data_litproj_storage_clear() {
    stage_param_data_litproj_storage.clear();
}

stage_param_litproj* stage_param_data_litproj_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_litproj_storage.find(stage_index);
    if (elem != stage_param_data_litproj_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_litproj_storage_set_stage_data(int32_t stage_index, stage_param_litproj* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_litproj>(
            stage_param_data_litproj_storage, stage_index) = *value;
}

void stage_param_data_rain_storage_clear() {
    stage_param_data_rain_storage.clear();
}

stage_param_rain* stage_param_data_rain_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_rain_storage.find(stage_index);
    if (elem != stage_param_data_rain_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_rain_storage_set_stage_data(int32_t stage_index, stage_param_rain* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_rain>(
            stage_param_data_rain_storage, stage_index) = *value;
}

void stage_param_data_ripple_storage_clear() {
    stage_param_data_ripple_storage.clear();
}

stage_param_ripple* stage_param_data_ripple_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_ripple_storage.find(stage_index);
    if (elem != stage_param_data_ripple_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_ripple_storage_set_stage_data(int32_t stage_index, stage_param_ripple* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_ripple>(
            stage_param_data_ripple_storage, stage_index) = *value;
}

void stage_param_data_snow_storage_clear() {
    stage_param_data_snow_storage.clear();
}

stage_param_snow* stage_param_data_snow_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_snow_storage.find(stage_index);
    if (elem != stage_param_data_snow_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_snow_storage_set_stage_data(int32_t stage_index, stage_param_snow* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_snow>(
            stage_param_data_snow_storage, stage_index) = *value;
}

void stage_param_data_splash_storage_clear() {
    stage_param_data_splash_storage.clear();
}

stage_param_splash* stage_param_data_splash_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_splash_storage.find(stage_index);
    if (elem != stage_param_data_splash_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_splash_storage_set_stage_data(int32_t stage_index, stage_param_splash* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_splash>(
            stage_param_data_splash_storage, stage_index) = *value;
}

void stage_param_data_star_storage_clear() {
    stage_param_data_star_storage.clear();
}

stage_param_star* stage_param_data_star_storage_get_value(int32_t stage_index) {
    auto elem = stage_param_data_star_storage.find(stage_index);
    if (elem != stage_param_data_star_storage.end())
        return &elem->second;
    return 0;
}

void stage_param_data_star_storage_set_stage_data(int32_t stage_index, stage_param_star* value) {
    if (value)
        stage_param_data_storage_get_value<stage_param_star>(
            stage_param_data_star_storage, stage_index) = *value;
}

stage_param_data_coli::stage_param_data_coli() : pv_id() {
    reset();
}

stage_param_data_coli::~stage_param_data_coli() {

}

void stage_param_data_coli::get_mhd_id_data(const int32_t& mhd_id, osage_ring_data& ring) {
    auto elem = mhd_id_ring_data.find(mhd_id);
    if (elem != mhd_id_ring_data.end())
        ring = elem->second;
}

void stage_param_data_coli::get_stage_index_data(const int32_t& stage_index, osage_ring_data& ring) {
    auto elem = stage_ring_data.find(stage_index);
    if (elem != stage_ring_data.end())
        ring = elem->second;
}

void stage_param_data_coli::get_stage_wind_file_path(std::string& path, const int32_t& stage_index) {
    auto elem = stage_wind_file.find(stage_index);
    if (elem != stage_wind_file.end())
        path.assign(elem->second);
}

void stage_param_data_coli::load(const int32_t& pv_id) {
    data_struct* aft_data = &data_list[DATA_AFT];

    const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
    if (!pv)
        return;

    this->pv_id = pv->id;
    for (const pv_db_pv_stage_param& i : pv->stage_param) {
        if (!i.collision_file.size())
            continue;

        std::string path(pv->mdata.dir);
        path.append(i.collision_file);

        if (!aft_data->check_file_exists(path.c_str())) {
            path.assign(sprintf_s_string("%s/%s", "rom/stage_param", i.collision_file.c_str()));

            if (!aft_data->check_file_exists(path.c_str()))
                continue;
        }

        osage_ring_data* ring = 0;
        if (i.stage == -1) {
            if (i.mhd_id < 0)
                return;

            auto elem = mhd_id_ring_data.insert({ i.mhd_id, {} });
            if (elem.second)
                ring = &elem.first->second;
        }
        else {
            auto elem = stage_ring_data.insert({ i.stage, {} });
            if (elem.second)
                ring = &elem.first->second;
        }

        if (ring)
            osage_ring_data::parse(path, *ring);

        if (!i.wind_file.size())
            continue;

        stage_wind_file.insert({ i.stage, i.wind_file });
    }
}

void stage_param_data_coli::reset() {
    stage_ring_data.clear();;
    mhd_id_ring_data.clear();;
    stage_wind_file.clear();;
    pv_id = -1;
}

template<typename T>
static T& stage_param_data_storage_get_value(std::map<int32_t, T>& map, int32_t key) {
    auto elem = map.find(key);
    if (elem != map.end())
        return elem->second;

    return map.insert({ key, {} }).first->second;
}
