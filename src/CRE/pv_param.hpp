/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/post_process_table/dof.hpp"
#include "../KKdLib/vec.hpp"
#include "rob/rob.hpp"
#include "light_param.hpp"
#include "task.hpp"

namespace pv_param {
    struct bloom {
        int32_t id;
        vec3 color;
        vec3 brightpass;
        float_t range;

        bloom();
    };

    struct dof {
        int32_t id;
        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        float_t quality;
        int32_t chara_id;

        dof();
    };

    struct chara_alpha {
        float_t frame;
        float_t duration;
        float_t prev_alpha;
        float_t alpha;
        int32_t type;

        chara_alpha();
    };

    struct color_correction {
        int32_t id;
        float_t hue;
        float_t saturation;
        float_t lightness;
        float_t exposure;
        vec3 gamma;
        float_t contrast;

        color_correction();
    };

    extern void light_data_clear_data();
    extern light_param_light_data& light_data_get_chara_light_data(int32_t id);
    extern light_param_light_data& light_data_get_stage_light_data(int32_t id);
    extern bool light_data_load_files(int32_t pv_id, std::string&& mdata_dir);

    extern void post_process_data_clear_data();
    extern pv_param::bloom& post_process_data_get_bloom_data(int32_t id);
    extern pv_param::color_correction& post_process_data_get_color_correction_data(int32_t id);
    extern pv_param::dof& post_process_data_get_dof_data(int32_t id);
    extern bool post_process_data_load_files(int32_t pv_id, std::string&& mdata_dir);
    extern void post_process_data_set_dof(::dof& d);
}

namespace pv_param_task {
    typedef void (*post_process_task_set_chara_item_alpha_callback)
        (void* data, int32_t chara_id, int32_t type, float_t alpha);
    typedef vec3(*post_process_task_set_dof_callback)
        (void* data, int32_t chara_id);

    extern bool post_process_task_add_task();
    extern void post_process_task_set_bloom_data(
        pv_param::bloom& data, float_t duration);
    extern void post_process_task_set_color_correction_data(
        pv_param::color_correction& data, float_t duration);
    extern void post_process_task_set_dof_data(
        pv_param::dof& data, float_t duration);
    extern void post_process_task_set_dof_data(
        pv_param::dof& data, float_t duration,
        post_process_task_set_dof_callback callback, void* callback_data);
    extern void post_process_task_set_chara_alpha(
        int32_t chara_id, int32_t type, float_t alpha, float_t duration);
    extern void post_process_task_set_chara_item_alpha(
        int32_t chara_id, int32_t type, float_t alpha, float_t duration,
        post_process_task_set_chara_item_alpha_callback callback, void* callback_data);
    extern bool post_process_task_del_task();
}
