/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../../CRE/task.hpp"

class TaskDataTestObj : public app::Task {
public:
    struct Data {
        int32_t object_set_index;
        int32_t curr_object_set_index;
        int32_t obj_num;
        int32_t curr_obj_num;
        int32_t object_index;
        int32_t curr_object_index;
        bool cull_camera;
        bool curr_cull_camera;
        int32_t stage_index;
        int32_t curr_stage_index;
        int32_t shadow;
        int32_t stage_display;
        vec3 rotation;

        Data();
    };

    Data data;

    TaskDataTestObj();
    virtual ~TaskDataTestObj() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    virtual Data* get_data();
};

extern TaskDataTestObj* task_data_test_obj;

extern void object_test_init();
extern void object_test_free();
