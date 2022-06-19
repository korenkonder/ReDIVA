/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../camera.hpp"
#include "../render_texture.hpp"
#include "../shared.hpp"

struct post_process_exposure_chara_data {
    vec4 field_0[8];
    float_t field_80;
    GLuint query[3];
    GLuint query_data[3];
};

struct post_process_exposure {
    render_texture exposure_history;
    render_texture exposure;
    int32_t exposure_history_counter;
    post_process_exposure_chara_data* chara_data;
    int32_t query_index;

    post_process_exposure();
    virtual ~post_process_exposure();

    void get_exposure(camera* cam, int32_t render_width, int32_t render_height,
        bool reset_exposure, GLuint in_tex_0, GLuint in_tex_1);
    void get_exposure_chara_data(void* pp, camera* cam);
    void init_fbo();
};
