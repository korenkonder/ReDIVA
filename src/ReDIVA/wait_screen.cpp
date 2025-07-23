/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wait_screen.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/data.hpp"
#include "../CRE/sound.hpp"
#include "../CRE/task.hpp"
#include "font_info.hpp"
#include "print_work.hpp"

class TaskWaitScreen : public app::Task {
public:
    int32_t state;
    int32_t index;
    bool stream_init;
    int32_t aet_id;
    std::string layer_name;
    bool start;
    bool started;
    bool end;
    bool ended;
    uint32_t aet_obj_id[2];

    TaskWaitScreen();
    virtual ~TaskWaitScreen() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void disp_version_text();
    void free_aet_obj(int32_t index);
    bool get_aet_obj_disp(int32_t index);
    bool get_ended();
    void reload_aet_obj(int32_t index, const AetArgs& args);
    void set_index(int32_t index);
    void set_end();

    bool sub_1406A2700();
    bool sub_1406A2A80();
    bool sub_1406A2B00();
    bool sub_1406A2B30();

    inline void reload_aet_obj(int32_t index, const char* start_marker, const char* end_marker) {
        data_struct* aft_data = &data_list[DATA_AFT];
        sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

        AetArgs args;
        args.id.id = aet_id;
        args.layer_name = layer_name.c_str();
        args.prio = spr::SPR_PRIO_21;
        args.flags = AET_PLAY_ONCE;
        if (start_marker)
            args.start_marker = start_marker;
        if (end_marker)
            args.end_marker = end_marker;
        args.spr_db = aft_spr_db;
        reload_aet_obj(index, args);
    }
};

TaskWaitScreen task_wait_screen;

static void sub_14066FEC0(int32_t);

bool task_wait_screen_add_task() {
    return app::TaskWork::add_task(&task_wait_screen, "WAIT_SCREEN");
}

bool task_wait_screen_del_task() {
    return task_wait_screen.del();
}

bool task_wait_screen_get_ended() {
    if (app::TaskWork::check_task_ready(&task_wait_screen))
        return task_wait_screen.get_ended();
    return true;
}

void task_wait_screen_set_end() {
    task_wait_screen.set_end();
}

void task_wait_screen_set_index_0() {
    sub_14066FEC0(0);
    task_wait_screen.set_index(0);
}

void task_wait_screen_set_index_1() {
    sub_14066FEC0(1);
    task_wait_screen.set_index(1);
}

void task_wait_screen_set_index_2() {
    sub_14066FEC0(1);
    task_wait_screen.set_index(2);
}

void task_wait_screen_set_index_3() {
    sub_14066FEC0(1);
    task_wait_screen.set_index(3);
}

bool task_wait_screen_sub_1406A2F80() {
    if (app::TaskWork::check_task_ready(&task_wait_screen))
        return task_wait_screen.started;
    return false;
}

TaskWaitScreen::TaskWaitScreen() : state(), index(), stream_init(), aet_id(), end(), ended(), aet_obj_id() {
    start = true;
    started = true;
}

TaskWaitScreen::~TaskWaitScreen() {

}

bool TaskWaitScreen::init() {
    return true;
}

bool TaskWaitScreen::ctrl() {
    if (start) {
        start = false;
        state = 0;
    }

    switch (state) {
    case 0:
        sub_1406A2B00();
        break;
    case 1:
        sub_1406A2B30();
        sub_1406A2700();
        break;
    case 2:
        sub_1406A2700();
        break;
    case 3:
        sub_1406A2A80();
        break;
    }
    return false;
}

bool TaskWaitScreen::dest() {
    for (int32_t i = 0; i < 2; i++)
        free_aet_obj(i);

    if (stream_init)
        sound_work_release_stream(0);
    return true;
}

void TaskWaitScreen::disp() {
    if (index == 1 || index == 3 && state == 2)
        disp_version_text();
}

void TaskWaitScreen::disp_version_text() {
    if (ended)
        return;

    font_info font(15);

    PrintWork print_work;
    print_work.SetFont(&font);
    print_work.prio = spr::SPR_PRIO_22;
    print_work.color = color_white;
    print_work.line_origin_loc = { 1268.0f - font.glyph.x, 708.0f - font.glyph.y * 2.0f };
    print_work.text_current_loc = print_work.line_origin_loc;
    print_work.SetResolutionMode(RESOLUTION_MODE_HD);
    print_work.printf(app::TEXT_FLAG_ALIGN_FLAG_RIGHT, "%s\n", "VERSION B");
    print_work.printf(app::TEXT_FLAG_ALIGN_FLAG_RIGHT, "%s\n", "REVISION 3");
}

void TaskWaitScreen::free_aet_obj(int32_t index) {
    if (aet_obj_id[index]) {
        aet_manager_free_aet_object(aet_obj_id[index]);
        aet_obj_id[index] = 0;
    }
}

bool TaskWaitScreen::get_aet_obj_disp(int32_t index) {
    if (aet_obj_id[index])
        return aet_manager_get_obj_end(aet_obj_id[index]);
    return true;
}

bool TaskWaitScreen::get_ended() {
    return ended;
}

void TaskWaitScreen::reload_aet_obj(int32_t index, const AetArgs& args) {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    free_aet_obj(index);
    aet_obj_id[index] = aet_manager_init_aet_object(args, aft_aet_db);
}

void TaskWaitScreen::set_index(int32_t index) {
    if (this->index == index)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    switch (index) {
    case 0:
        for (int32_t i = 0; i < 2; i++)
            free_aet_obj(i);
        sound_work_cue_release(1, "load01_2", 1);
        break;
    case 1: {
        aet_id = 24;
        layer_name.assign("loadscp");

        reload_aet_obj(0, 0, "load_in");
    } break;
    case 2: {
        aet_id = 33;
        layer_name.assign("demo_load");

        reload_aet_obj(0, 0, "load_start");
    } break;
    case 3:
        aet_id = 43;
        layer_name.assign("sel_intro");
        break;
    }

    this->index = index;
    start = true;
    started = true;
    end = false;
    ended = false;
}

void TaskWaitScreen::set_end() {
    end = true;
}

bool TaskWaitScreen::sub_1406A2700() {
    switch (index) {
    case 0:
    default:
        ended = true;
        break;
    case 1: {
        if (get_aet_obj_disp(0)) {
            if (!end)
                reload_aet_obj(0, "load_loop", "load_out");
        }

        if (!end)
            break;

        data_struct* aft_data = &data_list[DATA_AFT];
        sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

        AetArgs args;
        args.id.id = aet_id;
        args.layer_name = layer_name.c_str();
        args.prio = spr::SPR_PRIO_21;
        args.flags = AET_PLAY_ONCE;
        args.start_marker = "load_out";
        args.spr_db = aft_spr_db;
        reload_aet_obj(0, args);

        args.layer_name = "loadscp_white";
        args.prio = spr::SPR_PRIO_24;
        args.spr_db = aft_spr_db;
        reload_aet_obj(1, args);
        sound_work_cue_release(1, "load01_2", 0);
        state = 3;
        break;
    } break;
    case 2: {
        if (!get_aet_obj_disp(0))
            break;

        if (end) {
            reload_aet_obj(0, "end", 0);

            sound_work_cue_release(1, "load01_2", 0);
            state = 3;
            break;
        }
        else
            reload_aet_obj(0, "load_start", "end");
    } break;
    case 3: {
        if (!get_aet_obj_disp(0))
            break;

        if (end) {
            reload_aet_obj(0, "end", 0);

            aet_manager_set_obj_frame(aet_obj_id[0], 5.0f);
            sound_work_cue_release(1, "load01_2", 0);
            state = 3;
        }
        else
            reload_aet_obj(0, "load_start", "end");
    } break;
    }

    return false;
}

bool TaskWaitScreen::sub_1406A2A80() {
    switch (index) {
    case 1:
        if (get_aet_obj_disp(0)) {
            free_aet_obj(0);
            free_aet_obj(1);
            //task_sel_ticket_del_task();
            sound_work_cue_release(1, "load01_2", 1);
            ended = true;
        }
        break;
    case 2:
    case 3:
        if (get_aet_obj_disp(0)) {
            free_aet_obj(0);
            sound_work_cue_release(1, "load01_2", 1);
            ended = true;
        }
        break;
    }
    return 0;
}

bool TaskWaitScreen::sub_1406A2B00() {
    sound_work_cue_release(1, "load01_2");
    stream_init = false;
    state = 1;
    return false;
}

bool TaskWaitScreen::sub_1406A2B30() {
    switch (index) {
    case 1: {
        if (!get_aet_obj_disp(0))
            break;

        reload_aet_obj(0, "load_in", "load_loop");
        started = false;
        sound_work_play_se(1, "load01_2", 1.0f);
        state = 2;
    } break;
    case 2: {
        if (!get_aet_obj_disp(0))
            break;

        reload_aet_obj(0, "load_start", "end");
        started = false;
        sound_work_play_se(1, "load01_2", 1.0f);
        state = 2;
    } break;
    case 3: {
        reload_aet_obj(0, "load_start", "end");
        started = false;
        sound_work_play_se(1, "load01_2", 1.0f);
        state = 2;
    } break;
    default:
        started = false;
        state = 2;
        break;
    }
    return 0;
}

static void sub_14066FEC0(int32_t) {

}
