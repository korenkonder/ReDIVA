/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "equip_test.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../dw.hpp"
#include "auth_3d_test.hpp"
#include "motion_test.hpp"

#define DATA_TEST_ITEM_EQUIP_MAX 25

class DataTestEquipDw : public dw::Shell {
public:
    struct Data {
        bool disp;
        bool set_item_no;
        bool reset_item_no;
        bool refresh;
        int32_t item_no;
        const item_cos_data* cos;
        chara_index chara_index;
        int32_t cos_id;
        bool update_item;
        int32_t disp_parts;
        std::vector<int32_t> items[DATA_TEST_ITEM_EQUIP_MAX];
        std::map<int32_t, int32_t> field_278[DATA_TEST_ITEM_EQUIP_MAX];
        int32_t current_items[DATA_TEST_ITEM_EQUIP_MAX];

        Data();
        ~Data();
    };

    struct Item {
        dw::Composite* comp[DATA_TEST_ITEM_EQUIP_MAX];
        dw::Label* label[DATA_TEST_ITEM_EQUIP_MAX];
        dw::ListBox* list_box[DATA_TEST_ITEM_EQUIP_MAX];
        dw::ListBox* parts;

        Item();
    };

    int32_t chara_id;
    Data data[ROB_CHARA_COUNT];
    Item item;

    DataTestEquipDw();
    virtual ~DataTestEquipDw() override;

    virtual void Hide() override;

    void DataReset(int32_t chara_id);
    int32_t GetCharaID();
    chara_index GetCharaIndex(int32_t chara_id);
    const item_cos_data* GetCos(int32_t chara_id);
    int32_t GetCosId(int32_t chara_id);
    int32_t GetCurrentCharaItem(int32_t chara_id, dw::Widget* data);
    int32_t GetDispParts(int32_t chara_id);
    int32_t GetItemNo(int32_t chara_id);
    bool GetRefresh(int32_t chara_id);
    bool GetResetItemNo(int32_t chara_id);
    bool GetSetItemNo(int32_t chara_id);
    int32_t GetSelectedCharaItemNo(int32_t chara_id, dw::Widget* data);
    bool IsEquipAllowed(int32_t equip);
    void ItemReset();
    void ResetCharaItemCos(int32_t chara_id);
    void SetCharaDisp(int32_t chara_id, bool value);
    void SetCharaID(int32_t value);
    void SetCharaIndex(int32_t chara_id, ::chara_index chara_index);
    void SetCharaItemCos(int32_t chara_id, const item_cos_data* cos);
    void SetCos(int32_t chara_id, const item_cos_data* cos);
    void SetCosId(int32_t chara_id, int32_t cos_id);
    void SetCurrentItemNo(int32_t chara_id, dw::Widget* data, int32_t value);
    void SetDispParts(int32_t chara_id, int32_t parts);
    void SetItemNo(int32_t chara_id, int32_t value);
    void SetRefresh(int32_t chara_id, bool value);
    void SetResetItemNo(int32_t chara_id, bool value);
    void SetSetItemNo(int32_t chara_id, bool value);
    void SetUpdateItem(int32_t chara_id, bool value);

    static void Disp1pCallback(dw::Widget* data);
    static void Disp2pCallback(dw::Widget* data);
    static void DispCallback(int32_t chara_id, dw::Widget* widget);
    static void DispPartsCallback(dw::Widget* data);
    static void ExclusiveCheckCallback(dw::Widget* widget);
    static void ItemCallback(dw::Widget* data);
    static void PartsCallback(dw::Widget* data);
    static void RefreshCallback(dw::Widget* data);
    static void RobCallback(dw::Widget* data);

    void sub_14025FD80(int32_t chara_id);

    void sub_140261520(int32_t chara_id);
    void sub_140261580(int32_t chara_id);
    void sub_140261AF0(int32_t chara_id);
    void sub_140261C40(int32_t chara_id);
};

static const item_sub_id data_test_equip_sub_ids[] = {
     ITEM_SUB_ZUJO, ITEM_SUB_KAMI, ITEM_SUB_HITAI, ITEM_SUB_ME, ITEM_SUB_MEGANE,
     ITEM_SUB_MIMI, ITEM_SUB_KUCHI, ITEM_SUB_MAKI, ITEM_SUB_KUBI, ITEM_SUB_INNER,
     ITEM_SUB_OUTER, ITEM_SUB_KATA, ITEM_SUB_U_UDE, ITEM_SUB_L_UDE, ITEM_SUB_TE,
     ITEM_SUB_JOHA_MAE, ITEM_SUB_JOHA_USHIRO, ITEM_SUB_BELT, ITEM_SUB_KOSI, ITEM_SUB_PANTS,
     ITEM_SUB_ASI, ITEM_SUB_SUNE, ITEM_SUB_KUTSU, ITEM_SUB_HADA, ITEM_SUB_HEAD,
};

extern render_context* rctx_ptr;

DtmEqVs* dtm_eq_vs_array;

DataTestEquipDw* data_test_equip_dw;

int32_t data_test_item_array_load_count;
std::vector<int32_t> data_test_item_array[CHARA_MAX][ITEM_SUB_MAX];

int32_t data_test_item_equip_array_load_count;
std::vector<int32_t> data_test_item_equip_array[CHARA_MAX][5];

chara_index data_test_equip_dw_chara_index_array[ROB_CHARA_COUNT];

static int32_t data_test_equip_dw_get_disp_parts(int32_t chara_id);
static void data_test_equip_dw_set_chara_index(int32_t chara_id, chara_index chara_index);
static void data_test_equip_dw_set_chara_item_cos(int32_t chara_id, const item_cos_data* cos);
static const item_cos_data* data_test_equip_dw_get_cos(int32_t chara_id);
static int32_t data_test_equip_dw_get_item_no(int32_t chara_id);
static bool data_test_equip_dw_get_reset_item_no(int32_t chara_id);
static bool data_test_equip_dw_get_set_item_no(int32_t chara_id);
static void data_test_equip_dw_reset_chara_item_cos(int32_t chara_id);
static void data_test_equip_dw_set_cos_id(int32_t chara_id, int32_t cos);
static void data_test_equip_dw_sub_140261520(int32_t chara_id);

static std::vector<int32_t>* data_test_item_array_get(::chara_index chara_index);
static void data_test_item_array_load();
static void data_test_item_array_unload();

static std::vector<int32_t>* data_test_item_equip_array_get(::chara_index chara_index);
static void data_test_item_equip_array_load();
static void data_test_item_equip_array_unload();

static int32_t data_test_item_get_item_equip(chara_index chara_index, int32_t item_no);

DtmEqVs::DtmEqVs() : chara_id(), chara_index(),
curr_chara_index(), cos_id(), curr_cos_id(), disp_parts() {
    state = -1;
}

DtmEqVs::~DtmEqVs() {

}

bool DtmEqVs::init() {
    state = 0;
    return true;
}

bool DtmEqVs::ctrl() {
    rob_chara_item_cos_data* item_cos_data = rob_chara_array_get_item_cos_data(chara_id);
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    switch (state) {
    case 0:
        data_test_equip_dw_sub_140261520(chara_id);
        data_test_equip_dw_set_chara_item_cos(chara_id, item_cos_data->get_cos());
        state = 3;
    case 3:
        if (dtm_mot_array_get_loaded()) {
            CtrlDispParts();
            CtrlChara();

            int32_t item_no = data_test_equip_dw_get_item_no(chara_id);
            ::item_cos_data temp_cos;
            if (rob_chr)
                temp_cos = *rob_chr->item_cos_data.get_cos();

            bool reload = false;
            if (data_test_equip_dw_get_set_item_no(chara_id)) {
                item_cos_data->set_chara_index_item(chara_index, item_no);
                reload = true;
            }

            if (data_test_equip_dw_get_reset_item_no(chara_id)) {
                item_cos_data->set_chara_index_item_zero(chara_index, item_no);
                reload = true;
            }

            const ::item_cos_data* cos = data_test_equip_dw_get_cos(chara_id);
            if (cos) {
                item_cos_data->set_chara_index_item_nos(chara_index, cos->arr);
                reload = true;
            }

            /*if (sub_1402CEEC0(chara_id)) {
                reload = true;
                sub_1402CF8F0();
            }*/

            data_test_equip_dw_reset_chara_item_cos(chara_id);

            if (reload) {
                if (rob_chr)
                    task_rob_load_append_free_req_data_obj(rob_chr->chara_index, &temp_cos);
                rob_chara_array_reset_bone_data_item_equip(chara_id);
                task_rob_manager_hide_task();
                state = 1;
            }
        }
        break;
    case 1:
        if (rob_chr) {
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
            state = 3;
        }
        break;
    }
    return false;
}

bool DtmEqVs::dest() {
    return true;
}

bool DtmEqVs::add_task(int32_t chara_id, ::chara_index chara_index) {
    this->chara_id = chara_id;
    this->chara_index = chara_index;
    this->curr_chara_index = chara_index;
    cos_id = 0;
    curr_cos_id = 0;
    disp_parts = 0;
    data_test_equip_dw_set_chara_index(chara_id, chara_index);
    data_test_equip_dw_set_cos_id(chara_id, cos_id);
    return app::TaskWork::add_task(this, "DATA TEST EQUIP MANAGER FOR VS");
}

bool DtmEqVs::del_task() {
    return app::Task::del();
}

void DtmEqVs::SetCharaIndexCosId(::chara_index chara_index, int32_t cos_id) {
    this->chara_index = chara_index;
    this->cos_id = cos_id;
    data_test_equip_dw_set_chara_index(chara_id, chara_index);
    data_test_equip_dw_set_cos_id(chara_id, cos_id);
}

void DtmEqVs::CtrlChara() {
    if (chara_index == curr_chara_index && cos_id == curr_cos_id)
        return;

    curr_chara_index = chara_index;
    curr_cos_id = cos_id;
    const item_cos_data* cos = item_table_handler_array_get_item_cos_data(chara_index, cos_id);
    if (!check_cos_id_is_501(curr_cos_id) || !cos) {
        data_test_equip_dw_sub_140261520(chara_id);
        data_test_equip_dw_set_chara_item_cos(chara_id, cos);
    }
}

void DtmEqVs::CtrlDispParts() {
    int32_t disp_parts = data_test_equip_dw_get_disp_parts(chara_id);
    if (this->disp_parts == disp_parts)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    bool disp = false;
    item_id id;
    switch (disp_parts) {
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

    this->disp_parts = disp_parts;
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

DataTestEquipDw::Data::Data() : disp(), set_item_no(), reset_item_no(), refresh(), item_no(),
cos(), chara_index(), cos_id(), update_item(), disp_parts(), current_items() {

}

DataTestEquipDw::Data::~Data() {

}

DataTestEquipDw::Item::Item() : comp(), label(), list_box(), parts() {

}

DataTestEquipDw::DataTestEquipDw() : chara_id() {
    ItemReset();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        DataReset(i);
        data[i].chara_index = data_test_equip_dw_chara_index_array[i];
    }

    data_test_item_array_load();

#if DW_TRANSLATE
    const char* exclusive_check_text = u8"Switch to exclusive check window";
#else
    const char* exclusive_check_text = u8"排他チェック窓に切替";
#endif

    dw::Button* exclusive_check = new dw::Button(this, dw::FLAG_8);
    exclusive_check->SetText(exclusive_check_text);
    exclusive_check->callback = DataTestEquipDw::ExclusiveCheckCallback;

    dw::Composite* rob_comp = new dw::Composite(this);

    dw::Label* rob_label = new dw::Label(rob_comp);
    rob_label->SetText("ROB");
    rob_label->SetFont(dw::p_font_type_6x12);

    dw::ListBox* rob_list = new dw::ListBox(rob_comp, dw::MULTISELECT);
    rob_list->SetFont(dw::p_font_type_6x12);
    rob_list->AddItem("1P");
    rob_list->AddItem("2P");
    rob_list->SetItemIndex(0);
    rob_list->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::RobCallback));

    dw::Composite* disp_comp = new dw::Composite(this);
    disp_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* disp_1p = new dw::Button(disp_comp, dw::CHECKBOX);
    disp_1p->SetText("DISP 1P    ");
    disp_1p->SetValue(data[0].disp);
    disp_1p->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::Disp1pCallback));

    dw::Button* disp_2p = new dw::Button(disp_comp, dw::CHECKBOX);
    disp_2p->SetText("DISP 2P    ");
    disp_2p->SetValue(data[1].disp);
    disp_2p->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::Disp2pCallback));

    dw::Composite* disp_parts_comp = new dw::Composite(this);

    dw::Label* disp_parts_label = new dw::Label(disp_parts_comp);
    disp_parts_label->SetText("DISP PARTS");
    disp_parts_label->SetFont(dw::p_font_type_6x12);

    static const char* disp_parts_array[] = {
        "ALL",
        "ALL_OFF",
        "OUTER ONLY",
        "PANTS ONLY",
    };

    dw::ListBox* disp_parts_list = new dw::ListBox(disp_parts_comp, dw::MULTISELECT);
    disp_parts_list->SetFont(dw::p_font_type_6x12);
    for (const char*& i : disp_parts_array)
        disp_parts_list->AddItem(i);
    disp_parts_list->SetItemIndex(0);
    disp_parts_list->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::DispPartsCallback));

    dw::Composite* v35 = new dw::Composite(this);
    v35->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Label* color_item_set = new dw::Label(v35);
    color_item_set->SetText("COLOR ITEM SET     ");
    color_item_set->SetFont(dw::p_font_type_6x12);

    dw::Button* refresh = new dw::Button(v35, dw::CHECKBOX);
    refresh->SetText("refresh all equip");
    refresh->SetValue(true);
    refresh->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::RefreshCallback));

    dw::Composite* parts_comp = new dw::Composite(this);

    dw::ListBox* parts_list = new dw::ListBox(parts_comp);
    parts_list->SetFont(dw::p_font_type_6x12);
    parts_list->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::PartsCallback));
    item.parts = parts_list;

    dw::Composite* items_comp = new dw::Composite(this);

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        dw::Composite* comp = new dw::Composite(items_comp);
        comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));
        item.comp[j] = comp;

        char buf[0x10];
        sprintf_s(buf, sizeof(buf), "*%-11s", item_sub_id_get_name(data_test_equip_sub_ids[i]));

        dw::Label* label = new dw::Label(comp);
        label->SetText(buf);
        label->SetFont(dw::p_font_type_6x12);
        item.label[j] = label;

        dw::ListBox* list = new dw::ListBox(comp);
        list->SetFont(dw::p_font_type_6x12);
        list->SetMaxItems(10);
        list->AddSelectionListener(new dw::SelectionListenerOnHook(DataTestEquipDw::ItemCallback));
        list->callback_data.i64 = j;
        item.list_box[j] = list;
        j++;
    }

    sub_140261520(0);
    SetCharaItemCos(0, &rob_chara_array_get_item_cos_data(0)->cos);

    sub_140261520(1);
    SetCharaItemCos(1, &rob_chara_array_get_item_cos_data(1)->cos);

    rect.pos = { 953.0f, 0.0f };
}

DataTestEquipDw::~DataTestEquipDw() {

}

void DataTestEquipDw::Hide() {
    SetDisp();

    for (DataTestEquipDw::Data& i : data)
        i.update_item = false;

    data_test_item_equip_array_unload();
}

void DataTestEquipDw::DataReset(int32_t chara_id) {
    DataTestEquipDw::Data& data = this->data[chara_id];
    data.disp = true;
    data.set_item_no = false;
    data.reset_item_no = false;
    data.refresh = false;
    data.item_no = 0;
    data.chara_index = CHARA_MIKU;
    data.update_item = false;
    data.disp_parts = 0;

    for (int32_t i = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        data.items[i].clear();

        if (IsEquipAllowed(i)) {
            data.field_278[i].clear();
            data.current_items[i] = 0;
        }
    }
}

int32_t DataTestEquipDw::GetCharaID() {
    return chara_id;
}

chara_index DataTestEquipDw::GetCharaIndex(int32_t chara_id) {
    return data[chara_id].chara_index;
}

const item_cos_data* DataTestEquipDw::GetCos(int32_t chara_id) {
    return data[chara_id].cos;
}

int32_t DataTestEquipDw::GetCosId(int32_t chara_id) {
    return data[chara_id].cos_id;
}

int32_t DataTestEquipDw::GetCurrentCharaItem(int32_t chara_id, dw::Widget* data) {
    return this->data[chara_id].current_items[data->callback_data.i32];
}

int32_t DataTestEquipDw::GetDispParts(int32_t chara_id) {
    return data[chara_id].disp_parts;
}

int32_t DataTestEquipDw::GetItemNo(int32_t chara_id) {
    return data[chara_id].item_no;
}

bool DataTestEquipDw::GetRefresh(int32_t chara_id) {
    return data[chara_id].refresh;
}

bool DataTestEquipDw::GetResetItemNo(int32_t chara_id) {
    return data[chara_id].reset_item_no;
}

int32_t DataTestEquipDw::GetSelectedCharaItemNo(int32_t chara_id, dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        return this->data[chara_id].items[data->callback_data.i32][list_box->list->selected_item];
    return 0;
}

bool DataTestEquipDw::GetSetItemNo(int32_t chara_id) {
    return data[chara_id].set_item_no;
}

bool DataTestEquipDw::IsEquipAllowed(int32_t equip) {
    if (equip >= 0 && equip < DATA_TEST_ITEM_EQUIP_MAX
        && (equip == 0 || equip == 1 || equip == 4 || equip == 6 || equip == 8
            || equip == 10 || equip == 14 || equip == 16 || equip == 24))
        return true;
    return false;
}

void DataTestEquipDw::ItemReset() {
    for (int32_t i = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++)
        item.comp[i] = 0;

    for (int32_t i = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++)
        if (IsEquipAllowed(i)) {
            item.label[i] = 0;
            item.list_box[i] = 0;
        }

    item.parts = 0;
}

void DataTestEquipDw::ResetCharaItemCos(int32_t chara_id) {
    data[chara_id].set_item_no = false;
    data[chara_id].reset_item_no = false;
    data[chara_id].item_no = 0;
    data[chara_id].cos = 0;
}

void DataTestEquipDw::SetCharaDisp(int32_t chara_id, bool value) {
    data[chara_id].disp = value;
    data[chara_id].disp_parts = value ? 0 : 1;
}

void DataTestEquipDw::SetCharaID(int32_t value) {
    chara_id = value;
}

void DataTestEquipDw::SetCharaIndex(int32_t chara_id, ::chara_index chara_index) {
    data[chara_id].chara_index = chara_index;
}

void DataTestEquipDw::SetCharaItemCos(int32_t chara_id, const item_cos_data* cos) {
    if (!cos)
        return;

    DataTestEquipDw::Data& data = this->data[chara_id];

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        if (this->chara_id == chara_id) {
            auto elem = data.field_278[j].find(cos->arr[data_test_equip_sub_ids[i]]);
            if (elem != data.field_278[j].end() && elem->second)
                item.list_box[j]->SetItemIndex(elem->second);
            else
                item.list_box[j]->SetItemIndex(-1);
        }

        data.current_items[j] = cos->arr[data_test_equip_sub_ids[i]];
        j++;
    }
}

void DataTestEquipDw::SetCos(int32_t chara_id, const item_cos_data* cos) {
    data[chara_id].cos = cos;
}

void DataTestEquipDw::SetCosId(int32_t chara_id, int32_t cos_id) {
    data[chara_id].cos_id = cos_id;
}

void DataTestEquipDw::SetCurrentItemNo(int32_t chara_id, dw::Widget* data, int32_t value) {
    this->data[chara_id].current_items[data->callback_data.i32] = value;
}

void DataTestEquipDw::SetDispParts(int32_t chara_id, int32_t parts) {
    if (data[chara_id].disp)
        data[chara_id].disp_parts = parts;
}

void DataTestEquipDw::SetItemNo(int32_t chara_id, int32_t value) {
    data[chara_id].item_no = value;
}

void DataTestEquipDw::SetRefresh(int32_t chara_id, bool value) {
    data[chara_id].refresh = value;
}

void DataTestEquipDw::SetResetItemNo(int32_t chara_id, bool value) {
    data[chara_id].reset_item_no = value;
}

void DataTestEquipDw::SetSetItemNo(int32_t chara_id, bool value) {
    data[chara_id].set_item_no = value;
}

void DataTestEquipDw::SetUpdateItem(int32_t chara_id, bool value) {
    data[chara_id].update_item = value;
}

void DataTestEquipDw::Disp1pCallback(dw::Widget* data) {
    DispCallback(0, data);
}

void DataTestEquipDw::Disp2pCallback(dw::Widget* data) {
    DispCallback(1, data);
}

void DataTestEquipDw::DispCallback(int32_t chara_id, dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        data_test_equip_dw->SetCharaDisp(chara_id, button->value);
}

void DataTestEquipDw::DispPartsCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        data_test_equip_dw->SetDispParts(data_test_equip_dw->GetCharaID(),
            (int32_t)list_box->list->selected_item);
}

void DataTestEquipDw::ExclusiveCheckCallback(dw::Widget* data) {
    data_test_equip_dw->Hide();
    //data_test_item_check_add_task(&data_test_item_check);
}

void DataTestEquipDw::ItemCallback(dw::Widget* data) {
    int32_t chara_id = data_test_equip_dw->GetCharaID();
    int32_t item_no = data_test_equip_dw->GetSelectedCharaItemNo(chara_id, data);
    int32_t curr_item_no = data_test_equip_dw->GetCurrentCharaItem(chara_id, data);

    if (item_no) {
        data_test_equip_dw->SetSetItemNo(chara_id, true);
        data_test_equip_dw->SetResetItemNo(chara_id, false);
        data_test_equip_dw->SetUpdateItem(chara_id, true);
        data_test_equip_dw->SetItemNo(chara_id, item_no);
        data_test_equip_dw->SetCurrentItemNo(chara_id, data, item_no);
    }
    else if (curr_item_no) {
        data_test_equip_dw->SetSetItemNo(chara_id, false);
        data_test_equip_dw->SetResetItemNo(chara_id, true);
        data_test_equip_dw->SetUpdateItem(chara_id, true);
        data_test_equip_dw->SetItemNo(chara_id, curr_item_no);
        data_test_equip_dw->SetCurrentItemNo(chara_id, data, curr_item_no);
    }
    else{
        data_test_equip_dw->SetItemNo(chara_id, item_no);
        data_test_equip_dw->SetCurrentItemNo(chara_id, data, item_no);
    }

}

void DataTestEquipDw::PartsCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (!list_box)
        return;

    int32_t chara_id = data_test_equip_dw->GetCharaID();
    ::chara_index chara_index = data_test_equip_dw->GetCharaIndex(chara_id);
    int32_t cos = data_test_equip_dw->GetCosId(chara_id);

    const item_cos_data* v9 = item_table_handler_array_get_item_cos_data(chara_index, cos);
    size_t selected_item = list_box->list->selected_item;
    switch (selected_item) {
    case 0:
        break;
    case 1:
        v9 = item_table_handler_array_get_item_cos_data(chara_index, 499);
        break;
    case 2:
    default: {
        auto& dbgset = item_table_handler_array_get_table(chara_index)->dbgset;
        auto elem = dbgset.find(list_box->GetItemStr(selected_item).c_str());
        if (elem != dbgset.end())
            v9 = &elem->second;
    } break;
    }

    item_cos_data* v23 = &rob_chara_array_get_item_cos_data(chara_id)->cos;
    if (selected_item >= 2 && data_test_equip_dw->GetRefresh(chara_id)) {
        *v23 = *v9;
        v9 = v23;
    }

    data_test_equip_dw->SetCos(chara_id, v9);
    data_test_equip_dw->sub_140261C40(chara_id);
    data_test_equip_dw->sub_140261580(chara_id);
    data_test_equip_dw->UpdateLayout();
    data_test_equip_dw->SetCharaItemCos(chara_id, v9);
}

void DataTestEquipDw::RefreshCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        int32_t chara_id = data_test_equip_dw->GetCharaID();
        if (button->value)
            data_test_equip_dw->SetRefresh(chara_id, false);
        else
            data_test_equip_dw->SetRefresh(chara_id, true);
    }
}

void DataTestEquipDw::RobCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        int32_t chara_id = (int32_t)list_box->list->selected_item;
        data_test_equip_dw->SetCharaID(chara_id);
        data_test_equip_dw->sub_140261C40(chara_id);
        data_test_equip_dw->sub_140261AF0(chara_id);
        data_test_equip_dw->sub_14025FD80(chara_id);
    }
}

void DataTestEquipDw::sub_14025FD80(int32_t chara_id) {
    if (this->chara_id != chara_id)
        return;

    DataTestEquipDw::Data& data = this->data[chara_id];

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        auto elem = data.field_278[j].find(data.current_items[j]);
        if (elem != data.field_278[j].end())
            item.list_box[j]->SetItemIndex(elem->second);
        else
            item.list_box[j]->SetItemIndex(-1);
    }
}

void DataTestEquipDw::sub_140261520(int32_t chara_id) {
    if (this->chara_id == chara_id) {
        sub_140261C40(chara_id);
        sub_140261AF0(chara_id);
        sub_140261580(chara_id);
        UpdateLayout();
    }
    else
        sub_140261580(chara_id);
}

void DataTestEquipDw::sub_140261580(int32_t chara_id) {
    DataTestEquipDw::Data& data = this->data[chara_id];

    ::chara_index chara_index = data.chara_index;

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        std::vector<int32_t>* item_array = data_test_item_array_get(chara_index);
        data.items[j].clear();
        data.field_278[j].clear();
        data.current_items[j] = 0;
        data.items[j].push_back(0);

        data.field_278[j].insert({ 0, 0 });

        int32_t index = 1;
        for (int32_t& k : item_array[data_test_equip_sub_ids[i]]) {
            data.items[j].push_back(k);
            data.field_278[j].insert({ k, index++ });
        }
        j++;
    }
}

void DataTestEquipDw::sub_140261AF0(int32_t chara_id) {
    dw::ListBox* parts = item.parts;
    parts->ClearItems();
    parts->AddItem("DEFAULT");
    parts->AddItem("NUDE");

    for (auto& i : item_table_handler_array_get_table(data[chara_id].chara_index)->dbgset)
        parts->AddItem(i.first);
}

void DataTestEquipDw::sub_140261C40(int32_t chara_id) {
    DataTestEquipDw::Data& data = this->data[chara_id];

    ::chara_index chara_index = data.chara_index;

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "ITEM EQUIP (%s)", chara_index_get_name(chara_index));
    SetText(buf);

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        std::vector<int32_t>* item_array = data_test_item_array_get(chara_index);
        item.list_box[j]->ClearItems();
        item.list_box[j]->AddItem("REMOVE ITEM");

        for (int32_t& k : item_array[data_test_equip_sub_ids[i]]) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "%03d %s", k,
                item_table_handler_array_get_item_name(chara_index, k).c_str());
            item.list_box[j]->AddItem(buf);
        }
        j++;
    }
}

void data_test_equip_dw_init() {
    if (!data_test_equip_dw) {
        data_test_equip_dw = new DataTestEquipDw;
        data_test_equip_dw->sub_1402F38B0();
    }
    else {
        data_test_item_equip_array_load();
        //if (!data_test_item_check_dw_get_disp())
            data_test_equip_dw->Disp();
    }
}

static int32_t data_test_equip_dw_get_disp_parts(int32_t chara_id) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetDispParts(chara_id);
    return 0;
}

static void data_test_equip_dw_set_chara_index(int32_t chara_id, chara_index chara_index) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCharaIndex(chara_id, chara_index);
    data_test_equip_dw_chara_index_array[chara_id] = chara_index;
}

static void data_test_equip_dw_set_chara_item_cos(int32_t chara_id, const item_cos_data* cos) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCharaItemCos(chara_id, cos);
}

static const item_cos_data* data_test_equip_dw_get_cos(int32_t chara_id) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetCos(chara_id);
    return 0;
}

static int32_t data_test_equip_dw_get_item_no(int32_t chara_id) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetItemNo(chara_id);
    return 0;
}

static bool data_test_equip_dw_get_reset_item_no(int32_t chara_id) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetResetItemNo(chara_id);
    return false;
}

static bool data_test_equip_dw_get_set_item_no(int32_t chara_id) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetSetItemNo(chara_id);
    return false;
}

static void data_test_equip_dw_reset_chara_item_cos(int32_t chara_id) {
    if (data_test_equip_dw)
        data_test_equip_dw->ResetCharaItemCos(chara_id);
}

static void data_test_equip_dw_set_cos_id(int32_t chara_id, int32_t cos) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCosId(chara_id, cos);
}

static void data_test_equip_dw_sub_140261520(int32_t chara_id) {
    if (data_test_equip_dw)
        data_test_equip_dw->sub_140261520(chara_id);
}

static std::vector<int32_t>* data_test_item_array_get(::chara_index chara_index) {
    return data_test_item_array[chara_index];
}

static void data_test_item_array_load() {
    if (data_test_item_array_load_count)
        data_test_item_array_load_count++;
    else
        for (int32_t i = 0; i < CHARA_MAX; i++) {
            const item_table* table = item_table_handler_array_get_table((chara_index)i);
            if (!table)
                continue;

            for (auto& j : table->item) {
                const item_table_item* item = item_table_handler_array_get_item((chara_index)i, j.first);
                if (item && item->sub_id != -1)
                    data_test_item_array[i][item->sub_id].push_back(j.first);
            }
        }
}

static void data_test_item_array_unload() {
    if (--data_test_item_array_load_count < 0)
        data_test_item_array_load_count = 0;
    else if (!data_test_item_array_load_count)
        for (auto& i : data_test_item_array)
            for (auto& j : i) {
                j.clear();
                j.shrink_to_fit();
            }
}

static std::vector<int32_t>* data_test_item_equip_array_get(::chara_index chara_index) {
    return data_test_item_equip_array[chara_index];
}

static void data_test_item_equip_array_load() {
    if (data_test_item_equip_array_load_count)
        data_test_item_equip_array_load_count++;
    else
        for (int32_t i = 0; i < CHARA_MAX; i++) {
            const item_table* table = item_table_handler_array_get_table((chara_index)i);
            if (!table || !data_test_item_equip_array[i]->size())
                continue;

            for (auto& j : table->item) {
                int32_t equip = data_test_item_get_item_equip((chara_index)i, j.first);
                if (equip != -1)
                    data_test_item_equip_array[i][equip].push_back(j.first);
            }
        }
}

static void data_test_item_equip_array_unload() {
    if (--data_test_item_equip_array_load_count < 0)
        data_test_item_equip_array_load_count = 0;
    else if (!data_test_item_equip_array_load_count)
        for (auto& i : data_test_item_equip_array)
            for (auto& j : i) {
                j.clear();
                j.shrink_to_fit();
            }
}

static int32_t data_test_item_get_item_equip(chara_index chara_index, int32_t item_no) {
    static const int32_t data_test_item_item_equip_array[] = {
        0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 2, 2, 4, 4, 4, 4, 4, 4, 2, 1,
    };

    if (!item_table_handler_array_get_table(chara_index))
        return -1;

    item_sub_id sub_id = item_table_handler_array_get_item_sub_id(chara_index, item_no);
    if (sub_id != ITEM_SUB_NONE)
        return data_test_item_item_equip_array[sub_id];
    return -1;
}
