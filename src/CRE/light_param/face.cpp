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

float_t face::get_offset() const {
    return offset;
}

void face::set_offset(float_t value) {
    offset = value;
}

float_t face::get_scale() const {
    return scale;
}

void face::set_scale(float_t value) {
    scale = value;
}

void face::get_position(vec3& value) const {
    value = position;
}

void face::set_position(const vec3& value) {
    position = value;
}

void face::get_direction(vec3& value) const {
    value = direction;
}

void face::set_direction(const vec3& value) {
    direction = value;
}
