/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_param.hpp"

template<typename T>
static T& stage_param_data_storage_get_value(std::map<int32_t, T>& map, int32_t key);

std::map<int32_t, stage_param_fog_ring> stage_param_data_fog_ring_storage;
std::map<int32_t, stage_param_leaf> stage_param_data_leaf_storage;
std::map<int32_t, stage_param_litproj> stage_param_data_litproj_storage;
std::map<int32_t, stage_param_rain> stage_param_data_rain_storage;
std::map<int32_t, stage_param_ripple> stage_param_data_ripple_storage;
std::map<int32_t, stage_param_snow> stage_param_data_snow_storage;
std::map<int32_t, stage_param_splash> stage_param_data_splash_storage;
std::map<int32_t, stage_param_star> stage_param_data_star_storage;

void stage_param_data_storage_init() {
    stage_param_data_fog_ring_storage = {};
    stage_param_data_leaf_storage = {};
    stage_param_data_litproj_storage = {};
    stage_param_data_rain_storage = {};
    stage_param_data_ripple_storage = {};
    stage_param_data_snow_storage = {};
    stage_param_data_splash_storage = {};
    stage_param_data_star_storage = {};
}

void stage_param_data_storage_free() {
    stage_param_data_fog_ring_storage.clear();
    stage_param_data_leaf_storage.clear();
    stage_param_data_litproj_storage.clear();
    stage_param_data_rain_storage.clear();
    stage_param_data_ripple_storage.clear();
    stage_param_data_snow_storage.clear();
    stage_param_data_splash_storage.clear();
    stage_param_data_star_storage.clear();
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

template<typename T>
static T& stage_param_data_storage_get_value(std::map<int32_t, T>& map, int32_t key) {
    auto elem = map.find(key);
    if (elem != map.end())
        return elem->second;

    return map.insert({ key, {} }).first->second;
}
