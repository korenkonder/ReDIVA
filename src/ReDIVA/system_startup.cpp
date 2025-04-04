/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "system_startup.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/mdata_manager.hpp"
#include "../CRE/sprite.hpp"
#include "../KKdLib/io/path.hpp"
#include "pv_game/pv_game.hpp"
#include "input_state.hpp"
#include "game_state.hpp"
#include "print_work.hpp"

struct system_startup_struct {
    int32_t state;
    int32_t wait;
    int32_t time;
    int32_t dhcp_wait_time;
    int32_t field_10;
    int32_t touch_panel_state;
    int32_t field_18;
    bool field_1C;
    bool ready;
    int64_t field_20;
    int32_t location_server_state;
    int32_t net_param_state;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t frame;
    int32_t field_44;
    int32_t field_48;
    int32_t field_4C;
    color4u8 field_50;
    int32_t card_system_state;
    int32_t touch_slider_state;
    int32_t all_net_state;
    int32_t pv_information_state;
    int32_t field_64;
    int32_t extended_data_state;
    int32_t printer_state;
    bool field_70;
    font_info font;
    PrintWork print_work;

    system_startup_struct();
};

class SSOpdMakeTask : public app::Task {
public:
    int32_t frame;
    PrintWork print_work;
    font_info font_info;
    int32_t state;
    bool skip_check;
    bool skip_make;

    SSOpdMakeTask();
    virtual ~SSOpdMakeTask() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool add();
    std::string GetStateString();
    void SetSkipCheck(bool value);
    void SetSkipMake(bool value);
};

extern render_context* rctx_ptr;

SSOpdMakeTask* ss_opd_make_task;
system_startup_struct* system_startup;
system_startup_detail::TaskSystemStartup* task_system_startup;

int32_t system_startup_ready = 0;

int32_t ss_opd_make_task_percentage_line_width = 85;

static void ss_opd_make_start();
static SSOpdMakeTask* ss_opd_make_task_get();

static bool system_startup_check_ready();

std::string system_startup_get_wait(size_t size, size_t frame);

static void system_startup_extended_data_ctrl();
static void system_startup_extended_data_disp();

static void system_startup_pv_information_ctrl();
static void system_startup_pv_information_dest();
static void system_startup_pv_information_disp();

static void task_pv_db_reset();

namespace system_startup_detail {
    TaskSystemStartup::TaskSystemStartup() {

    }

    TaskSystemStartup::~TaskSystemStartup() {

    }

    bool TaskSystemStartup::init() {
        system_startup->state = 0;
        system_startup->wait = 0;
        if (test_mode_get())
            system_startup->time = 3600;
        else
            system_startup->time = 7200;
        system_startup->dhcp_wait_time = 36000;
        system_startup->field_10 = 0;
        system_startup->touch_panel_state = 0;
        system_startup->field_18 = 0;
        system_startup->field_1C = false;
        system_startup->ready = false;
        system_startup->field_20 = 0;
        system_startup->location_server_state = 0;
        system_startup->net_param_state = 0;
        system_startup->field_30 = 0;
        system_startup->field_34 = 0;
        system_startup->field_38 = 36000;
        system_startup->frame = 0;
        system_startup->field_44 = 1;
        system_startup->field_48 = 0;
        system_startup->field_4C = 0;
        system_startup->field_50 = 0xFF4EA8C0;
        system_startup->card_system_state = 0;
        system_startup->touch_slider_state = 0;
        system_startup->all_net_state = 0;
        system_startup->pv_information_state = 0;
        system_startup->field_64 = 0;
        system_startup->extended_data_state = 0;
        system_startup->printer_state = 0;
        system_startup->field_70 = false;

        system_startup->field_10++;

        system_startup->font.init_font_data(16);
        system_startup->print_work.SetFont(&system_startup->font);
        system_startup->print_work.color = color_white;
        return true;
    }

    bool TaskSystemStartup::ctrl() {
        system_startup->frame++;
        system_startup_ready = 0;
        switch (system_startup->state) {
        case 0:
            system_startup->state = 1;
        case 1:
            system_startup->state = 2;
            break;
        case 2:
            system_startup->state = 3;
            break;
        case 3: {
            system_startup_pv_information_ctrl();
            system_startup_extended_data_ctrl();

            //if (system_startup->location_server_state < 0 && system_startup->net_param_state < 0
            //    && system_startup->field_1C && system_startup->pv_information_state < 0
            //    && system_startup->extended_data_state < 0 && (test_mode_get() || system_startup->all_net_state < 0))
            if (system_startup->pv_information_state < 0 && system_startup->extended_data_state < 0)
                if (test_mode_get())
                    system_startup->state = 6;
                else {
                    system_startup->state = 4;
                    system_startup->field_10++;
                }

            if (!system_startup->field_10)
                system_startup->time--;

            bool v1 = false;
            if (test_mode_get() && input_state_get(0)->CheckTapped(1))
                v1 = true;

            if (system_startup->time >= 0 && !v1)
                break;

            system_startup_pv_information_dest();

            if (test_mode_get())
                system_startup->state = 6;
            else {
                system_startup->state = 4;
                system_startup->field_10++;
            }
            if (!app::TaskWork::check_task_ready(mdata_manager_get()))
                system_startup->state = 4;
        } break;
        case 4:
            rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, false);
            task_pv_game_init_test_pv();
            system_startup->state = 5;
            break;
        case 5:
            if (!task_pv_game_check_task_ready() && system_startup_check_ready())
                system_startup->state = 6;
            break;
        case 6:
            system_startup->wait = 0;//120;
            system_startup->state = 7;
            break;
        case 7:
            if (system_startup->wait-- < 0)
                system_startup->state = 8;
            break;
        case 8:
            rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, true);
            system_startup_ready = 1;
            break;
        }
        return false;
    }

    bool TaskSystemStartup::dest() {
        if (task_pv_game_check_task_ready() && !system_startup_check_ready())
            return false;

        opd_make_stop();
        mdata_manager_del_task();
        return true;
    }

    void TaskSystemStartup::disp() {
        resolution_struct* res_wind = res_window_get();
        spr::put_sprite_rect({ 0.0f, 0.0f, (float_t)res_wind->width, (float_t)res_wind->height },
            res_wind->resolution_mode, spr::SPR_PRIO_25, color_black, 0);

        system_startup->print_work.prio = spr::SPR_PRIO_25;
        system_startup->print_work.color = color_white;
        system_startup->print_work.SetResolutionMode(RESOLUTION_MODE_HD);
        system_startup->print_work.set_text_position(7.0f, 3.0f);

        switch (system_startup->state) {
        case 0:
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                "SYSTEM INITIALIZE...");
            break;
        case 1: {
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "DHCP WAIT ");
            std::string buf = system_startup_get_wait(8, get_main_timer() / 15);
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");

            if (36000 - system_startup->dhcp_wait_time >= 900) {
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                    u8"ALL.Net用ルーターなどの\n");
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                    u8"ネットワーク機器の接続や\n");
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                    u8"起動順序を確認して下さい。\n");

                std::string buf = sprintf_s_string("%d\n", system_startup->dhcp_wait_time);
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
            }

            if (test_mode_get()) {
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                    "PRESS SERVICE BUTTON TO CANCEL\n");
            }
        } break;
        default:
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "SYSTEM STARTUP   ");

            switch (0 /*sub_14066D190()*/) {
            case 0: {
                std::string buf = sprintf_s_string("(SATELLITE:%s)", false /*sub_14066E890()*/ ? "MAIN" : "SUB");
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
            } break;
            case 1:
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "(SERVER)");
                break;
            default:
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "(UNKNOWN)");
                break;
            }

            system_startup->print_work.set_text_position(7.0, 4.0);
            system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n\n");

            //system_startup_touch_panel_disp();
            //system_startup_card_system_disp();
            //system_startup_touch_slider_bd_disp();
            //system_startup_all_net_disp();
            //system_startup_location_server_disp();
            system_startup_pv_information_disp();
            //system_startup_net_param_disp();
            system_startup_extended_data_disp();
            //system_startup_printer_disp();

            switch (system_startup->state) {
            case 2:
                if (36000 - system_startup->dhcp_wait_time >= 900) {
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                        u8"メイン機までの接続は正常です。ALL.Net用ルーターなどの\n");
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                        u8"ネットワーク機器の接続や起動順序を確認して下さい。\n");

                    std::string buf = sprintf_s_string("%d\n", system_startup->dhcp_wait_time);
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
                }
                break;
            }

            if ((unsigned int)(system_startup->state - 3) <= 2) {
                system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
                if (system_startup->field_10) {
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "TIME STOP ");
                    std::string buf = system_startup_get_wait(8, system_startup->frame / 15);
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
                }
                else {
                    std::string buf = sprintf_s_string("TIME %d\n", system_startup->time);
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
                }

                if (test_mode_get())
                    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
                        "PRESS SERVICE BUTTON TO CANCEL\n");
            }
            break;
        }
    }
}

bool task_system_startup_add_task() {
    if (!ss_opd_make_task)
        ss_opd_make_task = new SSOpdMakeTask;

    if (!system_startup)
        system_startup = new system_startup_struct;

    if (!task_system_startup)
        task_system_startup = new system_startup_detail::TaskSystemStartup;

    return app::TaskWork::add_task(task_system_startup, "SYSTEM_STARTUP");
}

bool task_system_startup_del_task() {
    if (!app::TaskWork::check_task_ready(task_system_startup)) {
        delete task_system_startup;
        task_system_startup = 0;

        delete system_startup;
        system_startup = 0;

        delete ss_opd_make_task;
        ss_opd_make_task = 0;
        return true;
    }

    task_system_startup->del();
    return false;
}

void task_system_startup_free() {
    if (task_system_startup) {
        delete task_system_startup;
        task_system_startup = 0;
    }
}

system_startup_struct::system_startup_struct() : state(), wait(), time(), dhcp_wait_time(),
field_10(), touch_panel_state(), field_18(), field_1C(), ready(), field_20(),
location_server_state(), net_param_state(), field_30(), field_34(), field_38(), frame(),
field_44(), field_48(), field_4C(), card_system_state(), touch_slider_state(), all_net_state(),
pv_information_state(), field_64(), extended_data_state(), printer_state(), field_70() {

}

SSOpdMakeTask::SSOpdMakeTask() : frame() {
    state = 1;
    skip_check = true;
    skip_make = true;
}

SSOpdMakeTask:: ~SSOpdMakeTask() {

}

bool SSOpdMakeTask::init() {
    frame = 0;
    font_info.init_font_data(16);
    print_work.SetFont(&font_info);
    print_work.color = color_white;
    print_work.prio = spr::SPR_PRIO_25;

    skip_check = true;
    skip_make = true;

    if (path_check_directory_exists(get_ram_osage_play_data_tmp_dir()))
        path_delete_directory(get_ram_osage_play_data_tmp_dir());
    return true;
}

bool SSOpdMakeTask::ctrl() {
    switch (state) {
    case 1:
        frame++;
        state = 3;
        break;
    case 2:
        if (++frame < 300) {
            frame++;
            break;
        }

        frame++;
        state = 3;
        break;
    case 3:
        if (skip_check) {
            frame++;
            break;
        }

        opd_checker_launch_thread();
        frame++;
        state = 4;
        break;
    case 4:
        if (opd_checker_check_state_not_3()) {
            frame++;
            break;
        }

        frame++;
        state = opd_checker_has_objects() ? 5 : 8;
        break;
    case 5:
        if (skip_make) {
            frame++;
            break;
        }

        ss_opd_make_start();
        frame++;
        state = 6;
        break;
    case 6:
        if (opd_make_manager_check_task_ready()) {
            frame++;
            break;
        }

        frame = 0;
        frame++;
        state = 7;
        break;
    case 7:
        if (frame < 60) {
            frame++;
            break;
        }

        frame++;
        state = 8;
        break;
    case 8:
        frame++;
        return true;
    }
    return false;
}

bool SSOpdMakeTask::dest() {
    return true;
}

void SSOpdMakeTask::disp() {
    OpdMakeManagerData* opd_make_manager_data = opd_make_manager_get_data();
    print_work.set_text_position(7.0f, 19.0f);
    print_work.SetResolutionMode(RESOLUTION_MODE_HD);

    if (state != 6 && state != 7)
        return;

    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
        u8"拡張データをインストール中です。電源を切らないで下さい。\n");
    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT,
        "Installing the extended data. Please do not turn off the power.\n");
    print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
    uint32_t count = opd_make_manager_data->count;
    if (!count)
        return;

    static const char rotating_cursor_array[] = {
        '-', '\\', '|', '/', '-', '\\', '|', '/'
    };

    int32_t percentage = 100 * (count - opd_make_manager_data->left) / count;
    if (state == 6 && percentage >= 100)
        percentage = 99;

    int32_t disp = max_def(percentage * ss_opd_make_task_percentage_line_width / 100, 0);

    std::string buf("|");
    for (int32_t i = 0; i < disp; i++)
        buf.append("*");

    if (state == 6)
        buf.append(1, rotating_cursor_array[(frame / 4) % 8]);

    for (int32_t i = 0; i < ss_opd_make_task_percentage_line_width - disp - 1; i++)
        buf.append(".");
    buf.append("|");
    buf.append(sprintf_s_string(" (%3d%%)", percentage));
    print_work.printf_align_left("%s\n", buf.c_str());
}

bool SSOpdMakeTask::add() {
    return app::TaskWork::add_task(this, "SSOpdMakeTask");
}

std::string SSOpdMakeTask::GetStateString() {
    std::string str = sprintf_s_string("WAIT %d ", state);
    switch (state) {
    case 4: {
        int32_t index = 0;
        int32_t size = 0;
        opd_checker_get()->GetIndexSize(index, size);
        if (size)
            str.append(sprintf_s_string("(%d/%d) ", index, size));
    } break;
    case 6: {
        OpdMakeManagerData* opd_make_manager_data = opd_make_manager_get_data();
        uint32_t count = opd_make_manager_data->count;
        if (count)
            str.append(sprintf_s_string("(%d/%d) ", count - opd_make_manager_data->left, count));
    } break;
    }
    return str;
}

void SSOpdMakeTask::SetSkipCheck(bool value) {
    skip_check = value;
}

void SSOpdMakeTask::SetSkipMake(bool value) {
    skip_make = value;
}

static void ss_opd_make_start() {
    opd_make_start();
    ss_opd_make_task_get()->add();
}

static SSOpdMakeTask* ss_opd_make_task_get() {
    return ss_opd_make_task;
}

static bool system_startup_check_ready() {
    if (test_mode_get() || system_startup->ready)
        return true;

    task_pv_game_del_task();
    if (!task_pv_game_check_task_ready()) {
        //task_mask_screen_fade_in(0.0f, 0);
        rctx_ptr->render_manager->set_multisample(true);
        if (task_rob_manager_del_task()) {
            system_startup->ready = true;
            return true;
        }
    }
    return system_startup->ready;
}

static void system_startup_extended_data_ctrl() {
    if (test_mode_get()) {
        system_startup->extended_data_state = -1;
        return;
    }

    switch (system_startup->extended_data_state) {
    case 0:
        system_startup->field_10++;
        ss_opd_make_task_get()->add();
        system_startup->extended_data_state = 1;
        break;
    case 1:
        //if (system_startup->aime_state < 0)
            ss_opd_make_task_get()->SetSkipCheck(false);

        //if (system_startup->location_server_state < 0 && system_startup->net_param_state < 0
        //    && system_startup->field_1C && system_startup->net_param_state < 0
        //    && system_startup->pv_information_state < 0 && system_startup->all_net_state < 0)
        if (system_startup->pv_information_state < 0)
            ss_opd_make_task_get()->SetSkipMake(false);

        if (!app::TaskWork::check_task_ready(ss_opd_make_task_get())) {
            system_startup->extended_data_state = -1;
            system_startup->field_10--;
        }
        break;
    }
}

static void system_startup_extended_data_disp() {
    if (test_mode_get())
        return;

    std::string buf("EXTENDED DATA     : ");

    switch (system_startup->extended_data_state) {
    case -2:
        buf.append("NG");
        break;
    case -1:
        buf.append("OK");
        break;
    default:
        buf.append(ss_opd_make_task_get()->GetStateString());
        break;
    }

    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
}

std::string system_startup_get_wait(size_t size, size_t frame) {
    std::string str;
    str.resize(size / 2 + 1, '.');
    size_t pos = frame % ((size / 2) * 2);
    if (pos >= size / 2)
        str[size / 2 * 2 - pos] = 'o';
    else
        str[pos] = 'o';
    return str;
}

static void system_startup_pv_information_ctrl() {
    switch (system_startup->pv_information_state) {
    case -1:
        break;
    case 0:
        /*if (system_startup->aime_state < 0)*/ {
            mdata_manager_get();
            mdata_manager_add_task();
            system_startup->pv_information_state = 1;
        }
        break;
    case 1:
        /*if (system_startup->field_28 < 0)
            mdata_manager_get()->field_179 = true;*/

        if (!app::TaskWork::check_task_ready(mdata_manager_get())) {
            system_startup->field_10--;
            system_startup->pv_information_state = -1;
        }
        break;
    default:
        system_startup->pv_information_state = -2;
        break;
    }
}

static void system_startup_pv_information_dest() {
    if ((unsigned int)(system_startup->pv_information_state + 2) > 1) {
        //mdata_manager_get()->field_179 = true;
        task_pv_db_reset();
        system_startup->field_10--;
        system_startup->pv_information_state = -2;
    }
}

static void system_startup_pv_information_disp() {
    std::string buf("PV INFORMATION    : ");
    switch (system_startup->pv_information_state) {
    case -2:
        buf.append("NG");
        break;
    case -1:
        buf.append("OK");
        break;
    default:
        if (!app::TaskWork::check_task_ready(mdata_manager_get()))
            buf.append("WAIT");
        else
            buf.append(mdata_manager_get()->GetStateString());
        break;
    }

    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, buf.data(), buf.size());
    system_startup->print_work.PrintText(app::TEXT_FLAG_ALIGN_FLAG_LEFT, "\n");
}

static void task_pv_db_reset() {
    pv_db::TaskPvDB* task_pv_db = task_pv_db_get();
    task_pv_db->reset = true;
    task_pv_db->field_99 = true;
}
