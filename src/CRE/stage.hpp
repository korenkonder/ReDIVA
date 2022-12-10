/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/stage.hpp"
#include "render_context.hpp"
#include "auth_3d.hpp"

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

    // Temp
    std::vector<int32_t> auth_3d_ids;
    bool effect_display;

    stage();
    ~stage();
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

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
        virtual void Disp() override;
    };
}

struct task_stage_info {
    int16_t load_index;
    uint16_t load_counter;

    task_stage_info();
};

class DtmStg : public app::Task {
public:
    int32_t stage_index;
    int32_t load_stage_index;

    DtmStg();
    virtual ~DtmStg() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
};

extern DtmStg* dtm_stg;

extern void dtm_stg_init();
extern void dtm_stg_load(int32_t stage_index);
extern bool dtm_stg_unload();
extern void dtm_stg_free();

extern void task_stage_init();
extern bool task_stage_check_not_loaded();
extern void task_stage_current_set_effect_display(bool value);
extern void task_stage_current_set_ground(bool value);
extern void task_stage_current_set_ring(bool value);
extern void task_stage_current_set_sky(bool value);
extern void task_stage_current_set_stage_display(bool value, bool effect_enable);
extern void task_stage_disp_shadow();
extern stage* task_stage_get_current_stage();
int32_t task_stage_get_current_stage_index();
extern void task_stage_get_current_stage_info(task_stage_info* stg_info);
extern void task_stage_get_loaded_stage_infos(std::vector<task_stage_info>* vec);
extern stage* task_stage_get_stage(task_stage_info stg_info);
int32_t task_stage_get_stage_index(task_stage_info* stg_info);
extern bool task_stage_has_stage_info(task_stage_info* stg_info);
extern bool task_stage_load_task(const char* name);
extern void task_stage_set_effect_display(task_stage_info* stg_info, bool value);
extern void task_stage_set_ground(task_stage_info* stg_info, bool value);
extern void task_stage_set_mat(mat4* mat);
extern void task_stage_set_ring(task_stage_info* stg_info, bool value);
extern void task_stage_set_sky(task_stage_info* stg_info, bool value);
extern void task_stage_set_stage(task_stage_info* stg_info);
extern void task_stage_set_stage_display(task_stage_info* stg_info, bool value, bool effect_enable);
extern void task_stage_set_stage_index(int32_t stage_index);
extern void task_stage_set_stage_indices(std::vector<int32_t>& stage_indices);
extern bool task_stage_unload_task();
extern void task_stage_free();
