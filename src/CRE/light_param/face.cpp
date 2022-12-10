/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face.hpp"

face::face() {
    offset = 1.0f;
    scale = 0.35f;
    position = 0.0f;
    direction = 0.0f;
}

float_t face::get_offset() {
    return offset;
}

void face::set_offset(float_t value) {
    offset = value;
}

float_t face::get_scale() {
    return scale;
}

void face::set_scale(float_t value) {
    scale = value;
}

void face::get_position(vec3& value) {
    value = position;
}

void face::set_position(vec3& value) {
    position = value;
}

void face::set_position(vec3&& value) {
    position = value;
}

void face::get_direction(vec3& value) {
    value = direction;
}

void face::set_direction(vec3& value) {
    direction = value;
}

void face::set_direction(vec3&& value) {
    direction = value;
}
