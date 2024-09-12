/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/light_param/face.hpp"
#include "../../KKdLib/vec.hpp"
#include "../static_var.hpp"

struct face {
    float_t offset;
    float_t scale;
    vec3 position;
    vec3 direction;

    face();

    float_t get_offset() const;
    void set_offset(float_t value);
    float_t get_scale() const;
    void set_scale(float_t value);
    void get_position(vec3& value) const;
    void set_position(const vec3& value);
    void get_direction(vec3& value) const;
    void set_direction(const vec3& value);
};
