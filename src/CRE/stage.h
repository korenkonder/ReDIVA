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

class stage {
public:
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
    mat4u mat;
    float_t rot_y;
    int32_t obj_set;

    // Temp
    std::vector<int32_t> auth_3d_uids;
    bool effects;

    stage();
    ~stage();
};

#define TASK_STAGE_STAGE_COUNT 37

namespace stage_detail {
    class TaskStage : public Task {
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
        mat4u mat;
        float_t field_FF8;

        TaskStage();
        virtual ~TaskStage() override;
        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
        virtual void Disp() override;
    };
}

typedef struct task_stage_info {
    int16_t load_index;
    uint16_t load_counter;
} task_stage_info;

extern bool task_stage_check_not_loaded();
extern void task_stage_disp_shadow();
extern stage* task_stage_get_current_stage();
extern void task_stage_get_current_stage_info(task_stage_info* stg_info);
extern void task_stage_get_loaded_stage_infos(std::vector<task_stage_info>* vec);
extern bool task_stage_load(char* name);
extern bool task_stage_load(const char* name);
extern void task_stage_set_mat(mat4* mat);
extern void task_stage_set_mat(mat4u* mat);
extern void task_stage_set_stage(task_stage_info* stg_info);
extern void task_stage_set_stage_index(int32_t stage_index);
extern void task_stage_set_stage_indices(std::vector<int32_t>* stage_indices);
extern bool task_stage_unload();
