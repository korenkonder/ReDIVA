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

struct stage_modern {
    uint32_t hash;
    uint16_t counter;
    int32_t state;
    stage_data_modern* stage_data;
    bool stage_display;
    bool ground;
    bool sky;
    bool auth_3d_loaded;
    mat4 mat;
    float_t rot_y;
    uint32_t obj_set_hash;

    stage_modern();
};

#define TASK_STAGE_STAGE_COUNT 37

namespace stage_detail {
    class TaskStageModern : public app::Task {
    public:
        int32_t state;
        stage_modern stages[TASK_STAGE_STAGE_COUNT];
        int16_t current_stage;
        std::vector<uint32_t> stage_hashes;
        std::vector<uint32_t> load_stage_hashes;
        std::vector<stage_data_modern*> load_stages_data;
        bool stage_display;
        bool field_FB1;
        bool field_FB2;
        bool field_FB3;
        bool field_FB4;
        mat4 mat;
        float_t field_FF8;

        void* data;
        object_database* obj_db;
        texture_database* tex_db;
        stage_database* stage_data;

        TaskStageModern();
        virtual ~TaskStageModern() override;

        virtual bool init() override;
        virtual bool ctrl() override;
        virtual bool dest() override;
        virtual void disp() override;
    };
}

struct task_stage_modern_info {
    int16_t load_index;
    uint16_t load_counter;

    task_stage_modern_info();
    task_stage_modern_info(int16_t load_index, uint16_t load_counter);

    bool check() const;
    uint32_t get_stage_hash() const;
    void set_ground(bool value) const;
    void set_sky(bool value) const;
    void set_stage() const;
    void set_stage_display(bool value, bool effect_enable) const;
};

extern void task_stage_modern_init();
extern bool task_stage_modern_add_task(const char* name);
extern bool task_stage_modern_check_not_loaded();
extern bool task_stage_modern_check_task_ready();
extern void task_stage_modern_current_set_ground(bool value);
extern void task_stage_modern_current_set_sky(bool value);
extern void task_stage_modern_current_set_stage_display(bool value, bool effect_enable);
extern bool task_stage_modern_del_task();
extern void task_stage_modern_disp_shadow();
extern stage_modern* task_stage_modern_get_current_stage();
extern uint32_t task_stage_modern_get_current_stage_hash();
extern task_stage_modern_info task_stage_modern_get_current_stage_info();
extern void task_stage_modern_get_loaded_stage_infos(std::vector<task_stage_modern_info>& vec);
extern stage_modern* task_stage_modern_get_stage(const task_stage_modern_info stg_info);
extern void task_stage_modern_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data);
extern void task_stage_modern_set_mat(const mat4& mat);
extern void task_stage_modern_set_stage_hash(uint32_t stage_hash, stage_data_modern* stg_data);
extern void task_stage_modern_set_stage_hashes(std::vector<uint32_t>& stage_hashes,
    std::vector<stage_data_modern*>& load_stage_data);
extern void task_stage_modern_free();
