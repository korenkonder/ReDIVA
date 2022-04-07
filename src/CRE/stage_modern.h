/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.h"
#include "../KKdLib/database/stage.h"
#include "render_context.h"
#include "auth_3d.h"

class stage_modern {
public:
    uint32_t hash;
    uint16_t counter;
    int32_t state;
    stage_data_modern* stage_data;
    bool stage_display;
    bool ground;
    bool sky;
    bool auth_3d_loaded;
    mat4u mat;
    float_t rot_y;
    uint32_t obj_set_hash;

    // Temp
    std::vector<int32_t> auth_3d_ids;
    bool effect_display;

    stage_modern();
    ~stage_modern();
};

#define TASK_STAGE_STAGE_COUNT 37

namespace stage_detail {
    class TaskStageModern : public Task {
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
        mat4u mat;
        float_t field_FF8;

        void* data;
        object_database* obj_db;
        texture_database* tex_db;
        stage_database* stage_data;

        TaskStageModern();
        virtual ~TaskStageModern() override;
        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
        virtual void Disp() override;
    };
}

typedef struct task_stage_modern_info {
    int16_t load_index;
    uint16_t load_counter;
} task_stage_modern_info;

extern bool task_stage_modern_check_not_loaded();
extern void task_stage_modern_current_set_effect_display(bool value);
extern void task_stage_modern_current_set_ground(bool value);
extern void task_stage_modern_current_set_sky(bool value);
extern void task_stage_modern_current_set_stage_display(bool value);
extern void task_stage_modern_disp_shadow();
extern stage_modern* task_stage_modern_get_current_stage();
extern void task_stage_modern_get_current_stage_info(task_stage_modern_info* stg_info);
extern void task_stage_modern_get_loaded_stage_infos(std::vector<task_stage_modern_info>* vec);
extern stage_modern* task_stage_modern_get_stage(task_stage_modern_info stg_info);
extern bool task_stage_modern_has_stage_info(task_stage_modern_info* stg_info);
extern bool task_stage_modern_load(const char* name);
extern void task_stage_modern_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data);
extern void task_stage_modern_set_mat(mat4* mat);
extern void task_stage_modern_set_mat(mat4u* mat);
extern void task_stage_modern_set_stage(task_stage_modern_info* stg_info);
extern void task_stage_modern_set_stage_hash(uint32_t stage_hash, stage_data_modern* stg_data);
extern void task_stage_modern_set_stage_hashes(std::vector<uint32_t>* stage_hashes,
    std::vector<stage_data_modern*>* load_stage_data);
extern bool task_stage_modern_unload();
