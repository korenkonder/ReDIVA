/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mask_screen.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/data.hpp"
#include "../CRE/task.hpp"

struct TaskMaskScreen : app::Task {
    int32_t type;
    bool field_6C;
    int32_t aet_obj_id;
    float_t opacity;
    int32_t duration;
    int32_t frame;
    int32_t index;

    TaskMaskScreen();
    virtual ~TaskMaskScreen() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool ctrl_inner();
    void set_type(int32_t value);
};

TaskMaskScreen task_mask_screen;

bool task_mask_screen_add_task() {
    return app::TaskWork::add_task(&task_mask_screen, "MASK_SCREEN");
}

bool task_mask_screen_del_task() {
    return task_mask_screen.del();
}

void task_mask_screen_fade_in(float_t duration, int32_t index) {
    task_mask_screen.set_type(0);
    task_mask_screen.opacity = task_mask_screen.type ? 0.0f : 1.0f;
    task_mask_screen.frame = 0;
    task_mask_screen.duration = (int32_t)(duration * 60.0f);
    if (index >= 0 && index <= 1)
        task_mask_screen.index = index;
}

void task_mask_screen_fade_out(float_t duration, int32_t index) {
    task_mask_screen.set_type(1);
    task_mask_screen.opacity = task_mask_screen.type ? 0.0f : 1.0f;
    task_mask_screen.frame = 0;
    task_mask_screen.duration = (int32_t)(duration * 60.0f);
    if (index >= 0 && index <= 1)
        task_mask_screen.index = index;
}

TaskMaskScreen::TaskMaskScreen(): type(), field_6C(),
aet_obj_id(), opacity(), duration(), frame(), index() {

}

TaskMaskScreen::~TaskMaskScreen() {

}

bool TaskMaskScreen::init() {
    return true;
}

bool TaskMaskScreen::ctrl() {
    return ctrl_inner();
}

bool TaskMaskScreen::dest() {
    return true;
}

void TaskMaskScreen::disp() {

}

bool TaskMaskScreen::ctrl_inner() {
    switch (type) {
    case 0: {
        float_t duration = (float_t)this->duration;
        if (duration > 0.0f)
            opacity = 1.0f - (float_t)frame / duration;
        else
            opacity = 0.0f;
    } break;
    case 1: {
        float_t duration = (float_t)this->duration;
        if (duration > 0.0f)
            opacity = (float_t)frame / duration;
        else
            opacity = 1.0f;
    } break;
    }

    opacity = clamp_def(opacity, 0.0f, 1.0f);
    frame++;

    if (opacity > 0.0f) {
        if (!aet_obj_id) {
            static const char* layer_names[2] = {
                "black",
                "white",
            };

            data_struct* aft_data = &data_list[DATA_AFT];
            aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
            sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

            aet_obj_id = aet_manager_init_aet_object(43, layer_names[index],
                spr::SPR_PRIO_21, AET_PLAY_ONCE, 0, 0, aft_aet_db, aft_spr_db);
        }
    }
    else {
        if (aet_obj_id) {
            aet_manager_free_aet_object(aet_obj_id);
            aet_obj_id = 0;
        }
    }

    if (aet_obj_id)
        aet_manager_set_obj_alpha(aet_obj_id, opacity);
    return false;
}

void TaskMaskScreen::set_type(int32_t value) {
    type = value;
}
