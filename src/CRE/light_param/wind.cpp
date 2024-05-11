/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wind.hpp"
#include "../rob/rob.hpp"

static float_t wind_apply_spc(wind* w, float_t frame);
static void wind_ctrl(wind* w);

TaskWind* task_wind;

wind::wind() : scale(), cycle(), rot_y(), rot_z(),
bias(), spc(), wind_direction(), frame(), strength() {
    count = 16;
}

wind::~wind() {

}

float_t wind::get_scale() const {
    return scale;
}

void wind::set_scale(float_t value) {
    scale = value;
}

float_t wind::get_cycle() const {
    return cycle;
}

void wind::set_cycle(float_t value) {
    cycle = value;
}

void wind::get_rot(vec2* value) const {
    value->x = rot_y;
    value->y = rot_z;
}

void wind::set_rot(vec2* value) {
    rot_y = value->x;
    rot_z = value->y;
}

float_t wind::get_rot_y() const {
    return rot_y;
}

void wind::set_rot_y(float_t value) {
    rot_y = value;
}

float_t wind::get_rot_z() const {
    return rot_z;
}

void wind::set_rot_z(float_t value) {
    rot_z = value;
}

float_t wind::get_bias() const {
    return bias;
}

void wind::set_bias(float_t value) {
    bias = value;
}

void wind::reset() {
    strength = 0.0f;
    frame = 0.0f;
    wind_direction = 0.0f;
}

static float_t wind_apply_spc(wind* w, float_t frame) {
    size_t count = w->count;
    float_t value = 0.0f;
    wind_spc* spc = w->spc;
    for (size_t i = 0; i < count; i++, spc++) {
        float_t power = (float_t)(1 << (int32_t)i) * frame;
        value += cosf(power) * spc->cos + sinf(power) * spc->sin;
    }
    return value + w->bias;
}

static void wind_ctrl(wind* w) {
    float_t value = wind_apply_spc(w, w->cycle * w->frame) * w->scale;
    mat4 mat = mat4_identity;
    mat4_mul_rotate_y(&mat, w->rot_y * DEG_TO_RAD_FLOAT, &mat);
    mat4_mul_rotate_z(&mat, w->rot_z * DEG_TO_RAD_FLOAT, &mat);
    vec3 wind_direction = 0.0f;
    wind_direction.x = value;
    mat4_transform_vector(&mat, &wind_direction, &wind_direction);
    w->wind_direction = wind_direction * w->strength;
    if (w->strength >= 1.0f)
        w->strength = 1.0f;
    else
        w->strength = w->strength + (float_t)(1.0 / 60.0);

    w->frame += get_delta_frame();
    if (w->frame > 65535.0f)
        w->frame = 0.0f;
}

TaskWind::TaskWind() {
    ptr = new wind;
}

TaskWind:: ~TaskWind() {
    delete ptr;
}

bool TaskWind::init() {
    return true;
}

bool TaskWind::ctrl() {
    if (!pv_osage_manager_array_get_disp())
        wind_ctrl(ptr);
    return false;
}

bool TaskWind::dest() {
    ptr->reset();
    return true;
}

void TaskWind::disp() {
    pv_osage_manager_array_get_disp();
}

void task_wind_init() {
    task_wind = new TaskWind;
}

bool task_wind_hide_task() {
    task_wind->ptr->reset();
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
