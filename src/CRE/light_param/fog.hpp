/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/light_param/fog.hpp"
#include "../../KKdLib/vec.hpp"
#include "../static_var.hpp"

struct fog {
    fog_type type;
    float_t density;
    float_t start;
    float_t end;
    int32_t index;
    vec4 color;

    fog();

    fog_type get_type();
    void set_type(fog_type value);
    float_t get_density();
    void set_density(float_t value);
    float_t get_start();
    void set_start(float_t value);
    float_t get_end();
    void set_end(float_t value);
    int32_t get_index();
    void set_index(int32_t value);
    void get_color(vec4& value);
    void set_color(vec4& value);
    void set_color(vec4&& value);
    void data_set(fog_id id);
};
