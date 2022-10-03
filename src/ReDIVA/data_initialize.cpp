/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_initialize.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/sound.hpp"
#include "game_state.hpp"

extern uint32_t dbg_set_id;

TaskDataInit task_data_init;

TaskDataInit::TaskDataInit() : state(), field_6C() {

}

TaskDataInit::~TaskDataInit() {

}

bool TaskDataInit::Init() {
    state = 0;
    field_6C = !test_mode_get();
    return true;
}

bool TaskDataInit::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    switch (state) {
    case 0: {
        //sub_14063FB20();

        std::string v10 = {};
        //sub_140640E10(4, &v10);

        std::string v14 = {};
        //sub_140640E10(472, &v14);

        std::string v12 = {};
        //sub_140640E10(43, &v12);
        //fontmap_data_read_file();
        state = 1;
    } break;
    case 1:
        //if (!SprLoadSetFile(4) && !SprLoadSetFile(472) && !SprLoadSetFile(43) && !fontmap_data_parse_file())
            state = field_6C ? 2 : 4;
        break;
    case 2:
        //sub_140194800(10);
        state = 3;
        break;
    case 3:
        //if (sub_1401935C0()) // Measure FPS
            state = 4;
        break;
    case 4: {
        std::string v9 = {};
        //sub_140640E10(32, &v9);

        std::string v11 = {};
        //sub_14019D650(26, &v11);

        std::string v13 = {};
        //sub_140640E10(34, &v13);

        std::string v15 = {};
        //sub_14019D650(35, &v15);
        state = 5;
    } break;
    case 5:
        //if (SprLoadSetFile(32) || AetLoadSetFile(26) || SprLoadSetFile(34) || AetLoadSetFile(35))
        //    break;

        state = 6;
    case 6:
        //auth_3d_database_read_file();
        //stage_database_read_file();
        state = 7;
        break;
    case 7:
        //if (auth_3d_database_load_file() || stage_database_load_file())
        //    break;
        state = 8;
    case 8:
        object_storage_load_set(aft_data, aft_obj_db, dbg_set_id);
        //bone_database_read();
        //item_table_handler_array_read();
        sound_work_read_farc("rom/sound/se.farc");
        sound_work_read_farc("rom/sound/button.farc");
        sound_work_read_farc("rom/sound/se_cmn.farc");
        sound_work_read_farc("rom/sound/se_sel.farc");
        sound_work_read_farc("rom/sound/se_aime.farc");
        sound_work_read_farc("rom/sound/pvchange.farc");
        sound_work_read_farc("rom/sound/slide_se.farc");
        sound_work_read_farc("rom/sound/slide_long.farc");
        //sub_14037EF30();
        //sub_14037F180();
        //sub_140377A10();
        //sub_140377C30();
        //sub_1403926C0();
        //sub_1403928E0();
        //sub_1403740D0();
        //sub_140374360();
        //sub_140390440();
        //sub_1403906D0();
        //sub_140375A90();
        //sub_140375D20();
        //sub_14038EF90();
        //sub_14038F220();
        //sub_140384580();
        //sub_1403847B0();
        //sub_1403882D0();
        //sub_140388560();
        //hand_item_data_read_file();
        //rob_sleeve_data_read_file();
        //sub_140542F80();
        state = 9;
        break;
    case 9:
        if (!object_storage_load_obj_set_check_not_read(dbg_set_id)
            //&& !bone_database_load()
            //&& !item_table_handler_array_load()
            && !sound_work_load_farc("rom/sound/se.farc")
            && !sound_work_load_farc("rom/sound/button.farc")
            && !sound_work_load_farc("rom/sound/se_cmn.farc")
            && !sound_work_load_farc("rom/sound/se_sel.farc")
            && !sound_work_load_farc("rom/sound/se_aime.farc")
            && !sound_work_load_farc("rom/sound/pvchange.farc")
            && !sound_work_load_farc("rom/sound/slide_se.farc")
            && !sound_work_load_farc("rom/sound/slide_long.farc")
            //&& !sub_14037FE80()
            //&& !sub_1403785C0()
            //&& !sub_1403933B0()
            //&& !sub_1403747D0()
            //&& !sub_140390AE0()
            //&& !sub_140376130()
            //&& !sub_14038F630()
            //&& !sub_140385220()
            //&& !sub_140388A50()
            //&& !hand_item_data_parse_file()
            //&& !rob_sleeve_data_parse_file()
            ) {
            //sub_14037F680();
            //sub_1403F98D0(sub_1403F8C30());
            //sub_140378060();
            //sub_140234DE0(sub_140234860());
            //sub_140392D10();
            //sub_140226A80(sub_140226760());
            //sub_140374650();
            //sub_140390960();
            //sub_140375FB0();
            //sub_14038F4B0();
            //sub_140384C70();
            //sub_140388830();
            state = 10;
        }
        break;
    case 10:
        state = 11;
        break;
    case 11:
        state = 12;
        break;
    }
    return false;
}

bool TaskDataInit::Dest() {
    return true;
}

void TaskDataInit::Disp() {

}

bool task_data_init_append_task() {
    return app::TaskWork::AppendTask(&task_data_init, "DATA_INITIALIZE");
}

bool task_data_init_check_state() {
    return task_data_init.state == 12;
}

bool task_data_init_free_task() {
    return task_data_init.SetDest();
}
