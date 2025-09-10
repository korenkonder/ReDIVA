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

enum TaskWaitScreenIndex {
    TASK_WAIT_SCREEN_NONE = 0,
    TASK_WAIT_SCREEN_LOAD_SCREEN_PLAY,
    TASK_WAIT_SCREEN_DEMO_LOAD,
    TASK_WAIT_SCREEN_SELECT_INTRO,
};

class TaskWaitScreen : public app::Task {
public:
    int32_t state;
    TaskWaitScreenIndex index;
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
    bool end_load_loop();
    void free_aet_obj(int32_t index);
    bool get_aet_obj_disp(int32_t index);
    bool get_ended();
    int32_t get_index();
    bool get_started();
    bool init_load_start();
    void play_stream(const char* path);
    bool prepare_load();
    void reload_aet_obj(int32_t index, const AetArgs& args);
    void set_end();
    void set_load_loop(TaskWaitScreenIndex index);
    bool wait_load_loop();

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

bool task_wait_screen_check_index_none() {
    return task_wait_screen.get_index() == TASK_WAIT_SCREEN_NONE;
}

bool task_wait_screen_get_ended() {
    if (app::TaskWork::check_task_ready(&task_wait_screen))
        return task_wait_screen.get_ended();
    return true;
}

bool task_wait_screen_get_started() {
    if (app::TaskWork::check_task_ready(&task_wait_screen))
        return task_wait_screen.get_started();
    return false;
}

void task_wait_screen_set_end() {
    task_wait_screen.set_end();
}

void task_wait_screen_set_load_loop_demo_load() {
    sub_14066FEC0(1);
    task_wait_screen.set_load_loop(TASK_WAIT_SCREEN_DEMO_LOAD);
}

void task_wait_screen_set_load_loop_load_screen_play() {
    sub_14066FEC0(1);
    task_wait_screen.set_load_loop(TASK_WAIT_SCREEN_LOAD_SCREEN_PLAY);
}

void task_wait_screen_set_load_loop_none() {
    sub_14066FEC0(0);
    task_wait_screen.set_load_loop(TASK_WAIT_SCREEN_NONE);
}

void task_wait_screen_set_load_loop_select_intro() {
    sub_14066FEC0(1);
    task_wait_screen.set_load_loop(TASK_WAIT_SCREEN_SELECT_INTRO);
}

TaskWaitScreen::TaskWaitScreen() : state(), index(),
stream_init(), aet_id(), end(), ended(), aet_obj_id() {
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
        prepare_load();
        break;
    case 1:
        init_load_start();
        wait_load_loop();
        break;
    case 2:
        wait_load_loop();
        break;
    case 3:
        end_load_loop();
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
    print_work.set_font(&font);
    print_work.set_prio(spr::SPR_PRIO_22);
    print_work.set_color(color_white);
    print_work.set_position({ 1268.0f - font.glyph.x, 708.0f - font.glyph.y * 2.0f });
    print_work.set_resolution_mode(RESOLUTION_MODE_HD);
    print_work.printf(app::TEXT_FLAG_ALIGN_FLAG_RIGHT, "%s\n", "VERSION B");
    print_work.printf(app::TEXT_FLAG_ALIGN_FLAG_RIGHT, "%s\n", "REVISION 3");
}

bool TaskWaitScreen::end_load_loop() {
    switch (index) {
    case TASK_WAIT_SCREEN_LOAD_SCREEN_PLAY:
        if (get_aet_obj_disp(0)) {
            free_aet_obj(0);
            free_aet_obj(1);
            //task_sel_ticket_del_task();
            sound_work_cue_release(1, "load01_2");
            ended = true;
        }
        break;
    case TASK_WAIT_SCREEN_DEMO_LOAD:
    case TASK_WAIT_SCREEN_SELECT_INTRO:
        if (get_aet_obj_disp(0)) {
            free_aet_obj(0);
            sound_work_cue_release(1, "load01_2");
            ended = true;
        }
        break;
    }
    return 0;
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

int32_t TaskWaitScreen::get_index() {
    return index;
}

bool TaskWaitScreen::get_started() {
    return ended;
}

bool TaskWaitScreen::init_load_start() {
    switch (index) {
    case 1: {
        if (!get_aet_obj_disp(0))
            break;

        reload_aet_obj(0, "load_in", "load_loop");
        started = false;
        sound_work_play_se(1, "load01_2");
        state = 2;
    } break;
    case 2: {
        if (!get_aet_obj_disp(0))
            break;

        reload_aet_obj(0, "load_start", "end");
        started = false;
        sound_work_play_se(1, "load01_2");
        state = 2;
    } break;
    case 3: {
        reload_aet_obj(0, "load_start", "end");
        started = false;
        sound_work_play_se(1, "load01_2");
        state = 2;
    } break;
    default:
        started = false;
        state = 2;
        break;
    }
    return 0;
}

void TaskWaitScreen::play_stream(const char* path) {
    if (!path)
        return;

    sound_work_play_stream(0, path);
    stream_init = true;
}

bool TaskWaitScreen::prepare_load() {
    sound_work_cue_release(1, "load01_2");
    stream_init = false;
    state = 1;
    return false;
}

void TaskWaitScreen::reload_aet_obj(int32_t index, const AetArgs& args) {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    free_aet_obj(index);
    aet_obj_id[index] = aet_manager_init_aet_object(args, aft_aet_db);
}

void TaskWaitScreen::set_end() {
    end = true;
}

void TaskWaitScreen::set_load_loop(TaskWaitScreenIndex index) {
    if (this->index == index)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    switch (index) {
    case TASK_WAIT_SCREEN_NONE:
        for (int32_t i = 0; i < 2; i++)
            free_aet_obj(i);
        sound_work_cue_release(1, "load01_2");
        break;
    case TASK_WAIT_SCREEN_LOAD_SCREEN_PLAY: {
        aet_id = 24;
        layer_name.assign("loadscp");

        reload_aet_obj(0, 0, "load_in");
    } break;
    case TASK_WAIT_SCREEN_DEMO_LOAD: {
        aet_id = 33;
        layer_name.assign("demo_load");

        reload_aet_obj(0, 0, "load_start");
    } break;
    case TASK_WAIT_SCREEN_SELECT_INTRO:
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

bool TaskWaitScreen::wait_load_loop() {
    switch (index) {
    case TASK_WAIT_SCREEN_NONE:
    default:
        ended = true;
        break;
    case TASK_WAIT_SCREEN_LOAD_SCREEN_PLAY: {
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
        sound_work_cue_release(1, "load01_2", false);
        state = 3;
        break;
    } break;
    case TASK_WAIT_SCREEN_DEMO_LOAD: {
        if (!get_aet_obj_disp(0))
            break;

        if (end) {
            reload_aet_obj(0, "end", 0);

            sound_work_cue_release(1, "load01_2", false);
            state = 3;
            break;
        }
        else
            reload_aet_obj(0, "load_start", "end");
    } break;
    case TASK_WAIT_SCREEN_SELECT_INTRO: {
        if (!get_aet_obj_disp(0))
            break;

        if (end) {
            reload_aet_obj(0, "end", 0);

            aet_manager_set_obj_frame(aet_obj_id[0], 5.0f);
            sound_work_cue_release(1, "load01_2", false);
            state = 3;
        }
        else
            reload_aet_obj(0, "load_start", "end");
    } break;
    }

    return false;
}

static void sub_14066FEC0(int32_t) {

}
