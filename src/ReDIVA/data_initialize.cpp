/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data_initialize.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../CRE/customize_item_table.hpp"
#include "../CRE/data.hpp"
#include "../CRE/font.hpp"
#include "../CRE/hand_item.hpp"
#include "../CRE/item_table.hpp"
#include "../CRE/module_table.hpp"
#include "../CRE/object.hpp"
#include "../CRE/sound.hpp"
#include "../CRE/sprite.hpp"
#include "game_state.hpp"

extern uint32_t dbg_set_id;

extern uint32_t aet_gam_loadsc_set_id;
extern uint32_t spr_gam_loadsc_set_id;

extern uint32_t aet_cmn_all_set_id;
extern uint32_t spr_cmn_all_set_id;

extern uint32_t spr_fnt_24_set_id;
extern uint32_t spr_fnt_bold24_set_id;
extern uint32_t spr_fnt_cmn_set_id;

TaskDataInit task_data_init;

TaskDataInit::TaskDataInit() : state(), field_6C() {

}

TaskDataInit::~TaskDataInit() {

}

bool TaskDataInit::init() {
    state = 0;
    field_6C = !test_mode_get();
    return true;
}

bool TaskDataInit::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    switch (state) {
    case 0: {
        // Fucks up scaling
        //sprite_manager_reset_res_data();
        sprite_manager_read_file(spr_fnt_24_set_id, "", aft_data, aft_spr_db);
        sprite_manager_read_file(spr_fnt_bold24_set_id, "", aft_data, aft_spr_db);
        sprite_manager_read_file(spr_fnt_cmn_set_id, "", aft_data, aft_spr_db);
        fontmap_data_read_file();
        state = 1;
    } break;
    case 1:
        if (!sprite_manager_load_file(spr_fnt_24_set_id, aft_spr_db)
            && !sprite_manager_load_file(spr_fnt_bold24_set_id, aft_spr_db)
            && !sprite_manager_load_file(spr_fnt_cmn_set_id, aft_spr_db)
            && !fontmap_data_load_file())
            state = field_6C ? 2 : 4;
        break;
    case 2:
        measure_fps_init(10);
        state = 3;
        break;
    case 3:
        if (measure_fps_check_state())
            state = 4;
        break;
    case 4: {
        sprite_manager_read_file(spr_gam_loadsc_set_id, "", aft_data, aft_spr_db);
        aet_manager_read_file(aet_gam_loadsc_set_id, "", aft_data, aft_aet_db);
        sprite_manager_read_file(spr_cmn_all_set_id, "", aft_data, aft_spr_db);
        aet_manager_read_file(aet_cmn_all_set_id, "", aft_data, aft_aet_db);
        state = 5;
    } break;
    case 5:
        if (sprite_manager_load_file(spr_gam_loadsc_set_id, aft_spr_db)
            || aet_manager_load_file(aet_gam_loadsc_set_id, aft_aet_db)
            || sprite_manager_load_file(spr_cmn_all_set_id, aft_spr_db)
            || aet_manager_load_file(aet_cmn_all_set_id, aft_aet_db))
            break;

        state = 6;
    case 6:
        //auth_3d_database_read_file();
        //stage_database_read_file();
        state = 7;
        //break;
    case 7:
        //if (auth_3d_database_load_file() || stage_database_load_file())
        //    break;
        state = 8;
    case 8:
        objset_info_storage_load_set(aft_data, aft_obj_db, dbg_set_id);
        //bone_database_read();
        item_table_handler_array_read();
        sound_work_read_farc("rom/sound/se.farc");
        sound_work_read_farc("rom/sound/button.farc");
        sound_work_read_farc("rom/sound/se_cmn.farc");
        sound_work_read_farc("rom/sound/se_sel.farc");
        sound_work_read_farc("rom/sound/se_aime.farc");
        sound_work_read_farc("rom/sound/pvchange.farc");
        sound_work_read_farc("rom/sound/slide_se.farc");
        sound_work_read_farc("rom/sound/slide_long.farc");
        module_table_handler_data_read();
        customize_item_table_handler_data_read();
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
        hand_item_handler_data_read();
        rob_sleeve_handler_data_read();
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            rob_chara_age_age_array_reset(i);
        state = 9;
        break;
    case 9:
        if (!objset_info_storage_load_obj_set_check_not_read(dbg_set_id)
            //&& !bone_database_load()
            && !item_table_handler_array_load()
            && !sound_work_load_farc("rom/sound/se.farc")
            && !sound_work_load_farc("rom/sound/button.farc")
            && !sound_work_load_farc("rom/sound/se_cmn.farc")
            && !sound_work_load_farc("rom/sound/se_sel.farc")
            && !sound_work_load_farc("rom/sound/se_aime.farc")
            && !sound_work_load_farc("rom/sound/pvchange.farc")
            && !sound_work_load_farc("rom/sound/slide_se.farc")
            && !sound_work_load_farc("rom/sound/slide_long.farc")
            && !module_table_handler_data_load()
            && !customize_item_table_handler_data_load()
            //&& !sub_1403933B0()
            //&& !sub_1403747D0()
            //&& !sub_140390AE0()
            //&& !sub_140376130()
            //&& !sub_14038F630()
            //&& !sub_140385220()
            //&& !sub_140388A50()
            && !hand_item_handler_data_load()
            && !rob_sleeve_handler_data_load()) {
            module_data_handler_data_add_all_modules();
            customize_item_data_handler_data_add_all_customize_items();
            //sub_140392D10();
            //sub_140226760()->sub_140226A80();
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

bool TaskDataInit::dest() {
    return true;
}

void TaskDataInit::disp() {

}

bool task_data_init_add_task() {
    return app::TaskWork::add_task(&task_data_init, "DATA_INITIALIZE");
}

bool task_data_init_check_state() {
    return task_data_init.state == 12;
}

bool task_data_init_del_task() {
    return task_data_init.del();
}
