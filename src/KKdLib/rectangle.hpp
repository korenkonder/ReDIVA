/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"

struct rectangle {
    vec2 pos;
    vec2 size;

    inline rectangle() : pos(), size() {

    }

    inline rectangle(vec2 pos, vec2 size) {
        this->pos = pos;
        this->size = size;
    }

    inline rectangle(float_t pos_x, float_t pos_y, float_t size_x, float_t size_y) {
        this->pos = { pos_x, pos_y };
        this->size = { size_x, size_y };
    }
};
