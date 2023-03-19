/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "system_startup.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/mdata_manager.hpp"
#include "../CRE/sprite.hpp"
#include "game_state.hpp"
#include "pv_game.hpp"

system_startup_detail::TaskSystemStartup* task_system_startup;

struct system_startup_struct {
    int32_t state;
    int32_t wait;
    bool ready;
};

extern render_context* rctx_ptr;

system_startup_struct system_startup;

int32_t system_startup_ready;

static bool system_startup_check_ready();

static void task_pv_game_init_test_pv();

namespace system_startup_detail {
    TaskSystemStartup::TaskSystemStartup() {

    }

    TaskSystemStartup::~TaskSystemStartup() {

    }

    bool TaskSystemStartup::Init() {
        system_startup.state = 0;
        system_startup.wait = 0;
        system_startup.ready = false;
        return true;
    }

    bool TaskSystemStartup::Ctrl() {
        system_startup_ready = 0;
        switch (system_startup.state) {
        case 0:
            system_startup.state = 1;
        case 1:
            system_startup.state = 2;
            break;
        case 2:
            mdata_manager_add_task();
            system_startup.state = 3;
            break;
        case 3:
            if (mdata_manager_get()->state == 5 ||mdata_manager_get()->state == 15)
                system_startup.state = 4;
            break;
        case 4:
            rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_ALL_3D, false);
            task_pv_game_init_test_pv();
            system_startup.state = 5;
            break;
        case 5:
            if (!task_pv_game_check_task_ready() && system_startup_check_ready())
                system_startup.state = 6;
            break;
        case 6:
            system_startup.wait = 0;//120;
            system_startup.state = 7;
            break;
        case 7:
            if (system_startup.wait-- < 0)
                system_startup.state = 8;
            break;
        case 8:
            rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_ALL_3D, true);
            system_startup_ready = 1;
            break;
        }
        return false;
    }

    bool TaskSystemStartup::Dest() {
        if (task_pv_game_check_task_ready() && !system_startup_check_ready())
            return false;

        opd_make_manager_del_task();
        mdata_manager_del_task();
        return true;
    }

    void TaskSystemStartup::Disp() {
        resolution_struct* res_wind = res_window_get();
        spr::put_sprite_rect({ 0.0f, 0.0f, (float_t)res_wind->width, (float_t)res_wind->height },
            res_wind->resolution_mode, spr::SPR_PRIO_25, color_black, 0);
    }
}

bool task_system_startup_add_task() {
    if (!task_system_startup)
        task_system_startup = new system_startup_detail::TaskSystemStartup;

    return app::TaskWork::AddTask(task_system_startup, "SYSTEM_STARTUP");
}

bool task_system_startup_del_task() {
    if (!app::TaskWork::CheckTaskReady(task_system_startup)) {
        delete task_system_startup;
        task_system_startup = 0;
        return true;
    }

    task_system_startup->DelTask();
    return false;
}

void task_system_startup_free() {
    if (task_system_startup) {
        delete task_system_startup;
        task_system_startup = 0;
    }
}

static bool system_startup_check_ready() {
    if (test_mode_get() || system_startup.ready)
        return true;

    task_pv_game_del_task();
    if (!task_pv_game_check_task_ready() && task_rob_manager_del_task())
        system_startup.ready = true;
    return system_startup.ready;
}

static void task_pv_game_init_test_pv() {
    if (test_mode_get())
        return;

    task_rob_manager_add_task();
    TaskPvGame::Args args;
    args.init_data.pv_id = 999;
    args.init_data.difficulty = 1;
    args.init_data.field_C = 0;
    args.init_data.score_percentage_clear = 50;
    args.init_data.life_gauge_safety_time = 40;
    args.init_data.life_gauge_border = 30;
    args.field_190 = false;
    args.field_191 = false;
    args.no_fail = false;
    args.field_193 = true;
    args.field_194 = true;
    args.field_195 = true;
    args.test_pv = true;
    task_pv_game_add_task(args);
}
