/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/stage.hpp"
#include "auth_3d.hpp"
#include "render_context.hpp"
#include "task.hpp"

struct stage {
    int32_t index;
    uint16_t counter;
    int32_t state;
    stage_data* stage_data;
    bool stage_display;
    bool lens_flare;
    bool ground;
    bool ring;
    bool sky;
    bool auth_3d_loaded;
    mat4 mat;
    float_t rot_y;
    uint32_t obj_set;

    stage();

    float_t get_floor_height(const vec3& pos, const float_t& coli_r) const;
    void set_ground(bool value);
    void set_lens_flare(bool value);
    void set_ring(bool value);
    void set_sky(bool value);
    void set_stage_display(bool value, bool effect_enable);
};

#define TASK_STAGE_STAGE_COUNT 37

namespace stage_detail {
    class TaskStage : public app::Task {
    public:
        int32_t state;
        stage stages[TASK_STAGE_STAGE_COUNT];
        int16_t current_stage;
        std::vector<int32_t> stage_indices;
        std::vector<int32_t> load_stage_indices;
        bool stage_display;
        bool field_FB1;
        bool field_FB2;
        bool field_FB3;
        bool field_FB4;
        mat4 mat;
        float_t field_FF8;

        TaskStage();
        virtual ~TaskStage() override;

        virtual bool init() override;
        virtual bool ctrl() override;
        virtual bool dest() override;
        virtual void disp() override;
    };
}

struct task_stage_info {
    int16_t load_index;
    uint16_t load_counter;

    task_stage_info();
    task_stage_info(int16_t load_index, uint16_t load_counter);

    bool check() const;
    float_t get_floor_height(const vec3& pos, const float_t& coli_r) const;
    int32_t get_stage_index() const;
    void set_ground(bool value) const;
    void set_lens_flare(bool value) const;
    void set_ring(bool value) const;
    void set_sky(bool value) const;
    void set_stage() const;
    void set_stage_display(bool value, bool effect_enable) const;
};

extern void task_stage_init();
extern bool task_stage_add_task(const char* name);
extern bool task_stage_check_not_loaded();
extern float_t task_stage_current_get_floor_height(const vec3& pos, const float_t coli_r);
extern void task_stage_current_set_ground(bool value);
extern void task_stage_current_set_lens_flare(bool value);
extern void task_stage_current_set_ring(bool value);
extern void task_stage_current_set_sky(bool value);
extern void task_stage_current_set_stage_display(bool value, bool effect_enable);
extern bool task_stage_del_task();
extern void task_stage_disp_shadow();
extern stage* task_stage_get_current_stage();
int32_t task_stage_get_current_stage_index();
extern task_stage_info task_stage_get_current_stage_info();
extern void task_stage_get_loaded_stage_infos(std::vector<task_stage_info>& vec);
extern stage* task_stage_get_stage(const task_stage_info stg_info);
extern void task_stage_set_mat(const mat4& mat);
extern void task_stage_set_stage_index(int32_t stage_index);
extern void task_stage_set_stage_indices(const std::vector<int32_t>& stage_indices);
extern void task_stage_free();
