/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wind.hpp"
#include "../app_system_detail.hpp"

TaskWind* task_wind;

extern bool pv_osage_manager_array_get_disp();

Wind::Wind() : scale(), cycle(), yrot(), zrot(),
bias(), spectrum(), val(), frame(), strength() {
    count = 16;
}

Wind::~Wind() {

}

float_t Wind::calc_spectrum(float_t time) const {
    size_t count = this->count;
    float_t value = 0.0f;
    const WindSpectrum* spectrum = this->spectrum;
    for (size_t i = 0; i < count; i++, spectrum++) {
        float_t power = (float_t)(1 << (int32_t)i) * frame;
        value += cosf(power) * spectrum->cos + sinf(power) * spectrum->sin;
    }
    return value + bias;
}

void Wind::ctrl() {
    float_t value = calc_spectrum(cycle * frame) * scale;
    mat4 mat = mat4_identity;
    mat4_mul_rotate_y(&mat, yrot * DEG_TO_RAD_FLOAT, &mat);
    mat4_mul_rotate_z(&mat, zrot * DEG_TO_RAD_FLOAT, &mat);

    vec3 val(value, 0.0f, 0.0f);
    mat4_transform_vector(&mat, &val, &val);
    this->val = val * strength;

    if (strength >= 1.0f)
        strength = 1.0f;
    else
        strength = strength + (float_t)(1.0 / 60.0);

    frame += get_delta_frame();
    if (frame > 65535.0f)
        frame = 0.0f;
}

float_t Wind::get_scale() const {
    return scale;
}

void Wind::set_scale(float_t value) {
    scale = value;
}

float_t Wind::get_cycle() const {
    return cycle;
}

void Wind::set_cycle(float_t value) {
    cycle = value;
}

void Wind::get_rot(vec2& value) const {
    value.x = yrot;
    value.y = zrot;
}

void Wind::set_rot(const vec2& value) {
    yrot = value.x;
    zrot = value.y;
}

float_t Wind::get_yrot() const {
    return yrot;
}

void Wind::set_yrot(float_t value) {
    yrot = value;
}

float_t Wind::get_zrot() const {
    return zrot;
}

void Wind::set_zrot(float_t value) {
    zrot = value;
}

float_t Wind::get_bias() const {
    return bias;
}

void Wind::set_bias(float_t value) {
    bias = value;
}

void Wind::reset() {
    strength = 0.0f;
    frame = 0.0f;
    val = 0.0f;
}

StageWind::StageWind() {
    wind = new Wind;
}

StageWind::~StageWind() {
    delete wind;
}

TaskWind::TaskWind() {

}

TaskWind::~TaskWind() {

}

bool TaskWind::init() {
    return true;
}

bool TaskWind::ctrl() {
    if (!pv_osage_manager_array_get_disp())
        stage_wind.wind->ctrl();
    return false;
}

bool TaskWind::dest() {
    stage_wind.wind->reset();
    return true;
}

void TaskWind::disp() {
    pv_osage_manager_array_get_disp();
}

void task_wind_init() {
    task_wind = new TaskWind;
}

bool task_wind_hide_task() {
    task_wind->stage_wind.wind->reset();
    return task_wind->hide();
}

bool task_wind_run_task() {
    return task_wind->run();
}

void task_wind_free() {
    if (task_wind) {
        delete task_wind;
        task_wind = 0;
    }
}
