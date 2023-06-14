/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "equip_test.hpp"
#include "../../CRE/rob/rob.hpp"
#include "auth_3d_test.hpp"
#include "motion_test.hpp"

extern render_context* rctx_ptr;

DtmEqVs* dtm_eq_vs_array;

DtmEqVs::DtmEqVs() : chara_id(), chara_index(), curr_chara_index(),
module_index(), curr_module_index(), field_80() {
    state = -1;
}

DtmEqVs::~DtmEqVs() {

}

bool DtmEqVs::Init() {
    state = 0;
    return true;
}

bool DtmEqVs::Ctrl() {
    rob_chara_item_cos_data* item_cos_data = rob_chara_array_get_item_cos_data(chara_id);
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    switch (state) {
    case 0:
        //data_test_equip_dw_sub_14025FCC0(chara_id);
        //data_test_equip_dw_sub_14025FA80(chara_id, item_cos_data->get_cos());
        state = 3;
    case 3:
        if (false /*dtm_mot_array_sub_140287740()*/) {
            sub_140262050();
            sub_140261FF0();

            int32_t item_no = ITEM_NONE;//data_test_equip_dw_sub_14025FB20(chara_id);
            ::item_cos_data temp_cos;
            if (rob_chr)
                temp_cos = *rob_chr->item_cos_data.get_cos();

            bool v2 = false;
            /*if (data_test_equip_dw_sub_14025FAF0(chara_id)) {
                item_cos_data->set_chara_index_item(chara_index, item_no);
                v2 = true;
            }*/

            /*if (data_test_equip_dw_sub_14025FB80(chara_id)) {
                item_cos_data->set_chara_index_item_zero(chara_index, item_no);
                v2 = true;
            }*/

            const ::item_cos_data* cos = 0;//data_test_equip_dw_get_cos(chara_id);
            if (cos) {
                item_cos_data->set_chara_index_item_nos(chara_index, cos->arr);
                v2 = true;
            }

            /*if (sub_1402CEEC0(chara_id)) {
                v2 = true;
                sub_1402CF8F0();
            }*/

            //data_test_equip_dw_sub_14025FBE0(chara_id);

            if (v2) {
                if (rob_chr)
                    task_rob_load_append_free_req_data_obj(rob_chr->chara_index, &temp_cos);
                rob_chara_array_reset_bone_data_item_equip(chara_id);
                task_rob_manager_hide_task();
                state = 1;
            }
        }
        break;
    case 1:
        if (rob_chr->chara_index) {
            task_rob_load_append_load_req_data_obj(rob_chr->chara_index, rob_chr->item_cos_data.get_cos());
            state = 2;
        }
        break;
    case 2:
        if (!task_rob_load_check_load_req_data()) {
            task_rob_manager_run_task();
            if (rob_chara_array_get(chara_id)) {
                data_struct* aft_data = &data_list[DATA_AFT];
                bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
                object_database* aft_obj_db = &aft_data->data_ft.obj_db;
                item_cos_data->reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);
            }
            auth_3d_test_task_disp_chara();
            dtm_mot_array_set_reset_mot();
            this->state = 3;
        }
        break;
    }
    return false;
}

bool DtmEqVs::Dest() {
    return true;
}

bool DtmEqVs::AddTask(int32_t chara_id, ::chara_index chara_index) {
    chara_id = chara_id;
    this->chara_index = chara_index;
    this->curr_chara_index = chara_index;
    module_index = 0;
    curr_module_index = 0;
    this->field_80 = 0;
    //data_test_equip_dw_set_chara_index(chara_id, chara_index);
    //data_test_equip_dw_set_module_index(chara_id, module_index);
    return app::TaskWork::AddTask(this, "DATA TEST EQUIP MANAGER FOR VS");
    return true;
}

bool DtmEqVs::DelTask() {
    return app::Task::DelTask();
}

void DtmEqVs::SetCharaIndexModuleIndex(::chara_index chara_index, int32_t module_index) {
    this->chara_index = chara_index;
    this->module_index = module_index;
    //data_test_equip_dw_set_chara_index(chara_id, chara_index);
    //data_test_equip_dw_set_module_index(chara_id, module_index);
}

void DtmEqVs::sub_140261FF0() {
    if (chara_index == curr_chara_index && module_index == curr_module_index)
        return;

    curr_chara_index = chara_index;
    curr_module_index = module_index;
    const item_cos_data* cos = item_table_handler_array_get_item_cos_data_by_module_index(chara_index, module_index);
    if (!check_module_index_is_501(curr_module_index) || !cos) {
        //data_test_equip_dw_sub_14025FCC0(chara_id);
        //data_test_equip_dw_sub_14025FA80(chara_id, cos);
    }
}

void DtmEqVs::sub_140262050() {
    return;

    int32_t v2 = 0;//data_test_equip_dw_sub_14025FAC0(chara_id);
    /*if (v2 == field_80)
        return;*/

    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    bool disp = false;
    item_id id;
    switch (v2) {
    default:
        id = ITEM_NONE;
        disp = true;
        break;
    case 1:
        id = ITEM_NONE;
        break;
    case 2:
        id = ITEM_OUTER;
        break;
    case 3:
        id = ITEM_PANTS;
        break;
    }

    for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++)
        if (rob_itm_equip->get_object_info((item_id)i).not_null())
            rob_itm_equip->set_disp((item_id)i, true);

    for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++)
        if (i != id && (!disp || i != rob_itm_equip->field_D4))
            rob_itm_equip->set_disp((item_id)i, disp);

    field_80 = v2;
}

void equip_test_init() {
    if (!dtm_eq_vs_array)
        dtm_eq_vs_array = new DtmEqVs[2];
}

void equip_test_free() {
    if (dtm_eq_vs_array) {
        delete[] dtm_eq_vs_array;
        dtm_eq_vs_array = 0;
    }
}
