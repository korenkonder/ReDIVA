/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "system_startup.hpp"
#include "../CRE/mdata_manager.hpp"
#include "game_state.hpp"
#include "pv_game.hpp"

system_startup_detail::TaskSystemStartup task_system_startup;

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
            mdata_manager_append_task();
            system_startup.state = 3;
            break;
        case 3:
            if (mdata_manager_get()->state == 5 ||mdata_manager_get()->state == 15)
                system_startup.state = 4;
            break;
        case 4:
            rctx_ptr->draw_pass.set_enable(DRAW_PASS_3D, false);
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
            rctx_ptr->draw_pass.set_enable(DRAW_PASS_3D, true);
            system_startup_ready = 1;
            break;
        }
        return false;
    }

    bool TaskSystemStartup::Dest() {
        if (task_pv_game_check_task_ready() && !system_startup_check_ready())
            return false;

        opd_make_manager_task_free();
        mdata_manager_free_task();
        return true;
    }
}

bool task_system_startup_append_task() {
    return app::TaskWork::AppendTask(&task_system_startup, "SYSTEM_STARTUP");
}

bool task_system_startup_free_task() {
    return task_system_startup.SetDest();
}

static bool system_startup_check_ready() {
    if (test_mode_get() || system_startup.ready)
        return true;

    task_pv_game_free_task();
    if (!task_pv_game_check_task_ready() && task_rob_manager_free_task())
        system_startup.ready = true;
    return system_startup.ready;
}

static void task_pv_game_init_test_pv() {
    if (test_mode_get())
        return;

    task_rob_manager_append_task();
    TaskPvGame::InitData init_data;
    init_data.data.pv_id = 999;
    init_data.data.difficulty = 1;
    init_data.data.field_C = 0;
    init_data.data.score_percentage_clear = 50;
    init_data.data.life_gauge_safety_time = 40;
    init_data.data.life_gauge_border = 30;
    init_data.field_190 = false;
    init_data.field_191 = false;
    init_data.no_fail = false;
    init_data.field_193 = true;
    init_data.field_194 = true;
    init_data.field_195 = true;
    init_data.field_198 = true;
    task_pv_game_append_task(&init_data);
}
