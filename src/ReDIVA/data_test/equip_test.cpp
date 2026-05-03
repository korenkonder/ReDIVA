/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "equip_test.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/data.hpp"
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
        uint32_t item_no;
        const RobItemEquip* item_set;
        CHARA_NUM cn;
        int32_t cs;
        bool update_item;
        int32_t dp;
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

    int32_t rc;
    Data data[ROB_ID_MAX];
    Item item;

    DataTestEquipDw();
    virtual ~DataTestEquipDw() override;

    virtual void Hide() override;

    void DataReset(int32_t rc);
    int32_t GetCharaID();
    CHARA_NUM GetCharaNum(int32_t rc);
    int32_t GetCosId(int32_t rc);
    int32_t GetCurrentCharaItem(int32_t rc, dw::Widget* data);
    int32_t GetDispParts(int32_t rc);
    const RobItemEquip* GetEquip(int32_t rc);
    int32_t GetItemNo(int32_t rc);
    bool GetRefresh(int32_t rc);
    bool GetResetItemNo(int32_t rc);
    bool GetSetItemNo(int32_t rc);
    int32_t GetSelectedCharaItemNo(int32_t rc, dw::Widget* data);
    bool IsEquipAllowed(int32_t equip);
    void ItemReset();
    void ResetCharaItemCos(int32_t rc);
    void SetCharaDisp(int32_t rc, bool value);
    void SetCharaID(int32_t value);
    void SetCharaItemEquip(int32_t rc, const RobItemEquip* item_set);
    void SetCharaNum(int32_t rc, CHARA_NUM cn);
    void SetCosId(int32_t rc, int32_t cs);
    void SetCurrentItemNo(int32_t rc, dw::Widget* data, int32_t value);
    void SetDispParts(int32_t rc, int32_t parts);
    void SetEquip(int32_t rc, const RobItemEquip* item_set);
    void SetItemNo(int32_t rc, int32_t value);
    void SetRefresh(int32_t rc, bool value);
    void SetResetItemNo(int32_t rc, bool value);
    void SetSetItemNo(int32_t rc, bool value);
    void SetUpdateItem(int32_t rc, bool value);

    static void Disp1pCallback(dw::Widget* data);
    static void Disp2pCallback(dw::Widget* data);
    static void DispCallback(int32_t rc, dw::Widget* widget);
    static void DispPartsCallback(dw::Widget* data);
    static void ExclusiveCheckCallback(dw::Widget* widget);
    static void ItemCallback(dw::Widget* data);
    static void PartsCallback(dw::Widget* data);
    static void RefreshCallback(dw::Widget* data);
    static void RobCallback(dw::Widget* data);

    void sub_14025FD80(int32_t rc);

    void sub_140261520(int32_t rc);
    void sub_140261580(int32_t rc);
    void sub_140261AF0(int32_t rc);
    void sub_140261C40(int32_t rc);
};

static const ROB_ITEM_EQUIP_SUB_ID data_test_equip_sub_ids[] = {
     ROB_ITEM_EQUIP_SUB_ID_ZUJO, ROB_ITEM_EQUIP_SUB_ID_KAMI, ROB_ITEM_EQUIP_SUB_ID_HITAI,
     ROB_ITEM_EQUIP_SUB_ID_ME, ROB_ITEM_EQUIP_SUB_ID_MEGANE, ROB_ITEM_EQUIP_SUB_ID_MIMI,
     ROB_ITEM_EQUIP_SUB_ID_KUCHI, ROB_ITEM_EQUIP_SUB_ID_MAKI, ROB_ITEM_EQUIP_SUB_ID_KUBI,
     ROB_ITEM_EQUIP_SUB_ID_INNER, ROB_ITEM_EQUIP_SUB_ID_OUTER, ROB_ITEM_EQUIP_SUB_ID_KATA,
     ROB_ITEM_EQUIP_SUB_ID_U_UDE, ROB_ITEM_EQUIP_SUB_ID_L_UDE, ROB_ITEM_EQUIP_SUB_ID_TE,
     ROB_ITEM_EQUIP_SUB_ID_JOHA_MAE, ROB_ITEM_EQUIP_SUB_ID_JOHA_USHIRO, ROB_ITEM_EQUIP_SUB_ID_BELT,
     ROB_ITEM_EQUIP_SUB_ID_KOSI, ROB_ITEM_EQUIP_SUB_ID_PANTS, ROB_ITEM_EQUIP_SUB_ID_ASI,
     ROB_ITEM_EQUIP_SUB_ID_SUNE, ROB_ITEM_EQUIP_SUB_ID_KUTSU, ROB_ITEM_EQUIP_SUB_ID_HADA,
     ROB_ITEM_EQUIP_SUB_ID_HEAD,
};

extern render_context* rctx_ptr;

DtmEqVs* dtm_eq_vs_array;

DataTestEquipDw* data_test_equip_dw;

CHARA_NUM data_test_equip_dw_chara_index_array[ROB_ID_MAX];

static int32_t data_test_equip_dw_get_disp_parts(int32_t rc);
static void data_test_equip_dw_set_chara_num(int32_t rc, CHARA_NUM cn);
static void data_test_equip_dw_set_chara_item_equip(int32_t rc, const RobItemEquip* item_set);
static const RobItemEquip* data_test_equip_dw_get_equip(int32_t rc);
static int32_t data_test_equip_dw_get_item_no(int32_t rc);
static bool data_test_equip_dw_get_reset_item_no(int32_t rc);
static bool data_test_equip_dw_get_set_item_no(int32_t rc);
static void data_test_equip_dw_reset_chara_item_cos(int32_t rc);
static void data_test_equip_dw_set_cos_id(int32_t rc, int32_t cos);
static void data_test_equip_dw_sub_140261520(int32_t rc);

DtmEqVs::DtmEqVs() : m_rc(), m_cn_cur(), m_cn_pre(), m_cs_cur(), m_cs_pre(), m_dp() {
    m_mode = -1;
}

DtmEqVs::~DtmEqVs() {

}

bool DtmEqVs::init() {
    m_mode = 0;
    return true;
}

bool DtmEqVs::ctrl() {
    RobManagement* rob_man = get_rob_management();
    RobItem* rob_item = rob_man->get_rob_robitem_work((ROB_ID)m_rc);
    rob_chara* rob_chr = rob_man->get_rob((ROB_ID)m_rc);
    switch (m_mode) {
    case 0:
        data_test_equip_dw_sub_140261520(m_rc);
        data_test_equip_dw_set_chara_item_equip(m_rc, rob_item->get_equip());
        m_mode = 3;
    case 3:
        if (dtm_mot_array_get_loaded()) {
            CtrlDispParts();
            CtrlChara();

            uint32_t item_no = data_test_equip_dw_get_item_no(m_rc);
            RobItemEquip temp_equip;
            if (rob_chr)
                temp_equip = *rob_chr->item.get_equip();

            bool reload = false;
            if (data_test_equip_dw_get_set_item_no(m_rc)) {
                RobItem::s_regist_item_one((CHARA_NUM)m_cn_cur, item_no, rob_item);
                reload = true;
            }

            if (data_test_equip_dw_get_reset_item_no(m_rc)) {
                RobItem::s_delete_item((CHARA_NUM)m_cn_cur, item_no, rob_item);
                reload = true;
            }

            const RobItemEquip* item_set = data_test_equip_dw_get_equip(m_rc);
            if (item_set) {
                RobItem::s_regist_item_all((CHARA_NUM)m_cn_cur, item_set, rob_item);
                reload = true;
            }

            /*if (sub_1402CEEC0(rc)) {
                reload = true;
                sub_1402CF8F0();
            }*/

            data_test_equip_dw_reset_chara_item_cos(m_rc);

            if (reload) {
                if (rob_chr)
                    task_rob_load_append_free_req_data_obj(rob_chr->chara_num, &temp_equip);
                get_rob_management()->rob_disp_init((ROB_ID)m_rc);
                task_rob_manager_suspend();
                m_mode = 1;
            }
        }
        break;
    case 1:
        if (rob_chr) {
            task_rob_load_append_load_req_data_obj(rob_chr->chara_num, rob_chr->item.get_equip());
            m_mode = 2;
        }
        break;
    case 2:
        if (!task_rob_load_check_load_req_data()) {
            task_rob_manager_restart();
            if (get_rob_management()->get_rob((ROB_ID)m_rc)) {
                data_struct* aft_data = &data_list[DATA_AFT];
                bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
                object_database* aft_obj_db = &aft_data->data_ft.obj_db;
                rob_item->equip(m_rc, aft_bone_data, aft_data, aft_obj_db);
            }
            auth_3d_test_task_disp_chara();
            dtm_mot_array_set_reset_mot();
            m_mode = 3;
        }
        break;
    }
    return false;
}

bool DtmEqVs::dest() {
    return true;
}

bool DtmEqVs::open(int32_t rc, int32_t cn) {
    m_rc = rc;
    m_cn_cur = cn;
    m_cn_pre = cn;
    m_cs_cur = 0;
    m_cs_pre = 0;
    m_dp = 0;
    data_test_equip_dw_set_chara_num(rc, (CHARA_NUM)cn);
    data_test_equip_dw_set_cos_id(rc, m_cs_cur);
    return app::Task::open("DATA TEST EQUIP MANAGER FOR VS");
}

bool DtmEqVs::close() {
    return app::Task::close();
}

void DtmEqVs::SetCharaNumCosId(int32_t cn, int32_t cs) {
    m_cn_cur = cn;
    m_cs_cur = cs;
    data_test_equip_dw_set_chara_num(m_rc, (CHARA_NUM)cn);
    data_test_equip_dw_set_cos_id(m_rc, m_cs_cur);
}

void DtmEqVs::CtrlChara() {
    if (m_cn_cur == m_cn_pre && m_cs_cur == m_cs_pre)
        return;

    m_cn_pre = m_cn_cur;
    m_cs_pre = m_cs_cur;
    const RobItemEquip* item_set = get_default_costume_data((CHARA_NUM)m_cn_cur, m_cs_cur);
    if (!RobDisp::check_one_skin(m_cs_pre) || !item_set) {
        data_test_equip_dw_sub_140261520(m_rc);
        data_test_equip_dw_set_chara_item_equip(m_rc, item_set);
    }
}

void DtmEqVs::CtrlDispParts() {
    int32_t dp = data_test_equip_dw_get_disp_parts(m_rc);
    if (m_dp == dp)
        return;

    RobDisp* rob_disp = get_rob_management()->get_rob_robdisp_work((ROB_ID)m_rc);
    bool disp = false;
    ROB_PARTS_KIND rpk;
    switch (dp) {
    default:
        rpk = RPK_NONE;
        disp = true;
        break;
    case 1:
        rpk = RPK_NONE;
        break;
    case 2:
        rpk = RPK_OUTER;
        break;
    case 3:
        rpk = RPK_PANTS;
        break;
    }

    for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++)
        if (rob_disp->get_objid((ROB_PARTS_KIND)i).not_null())
            rob_disp->set_disp_flag((ROB_PARTS_KIND)i, true);

    for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++)
        if (i != rpk && (!disp || i != rob_disp->hyoutan_rpk))
            rob_disp->set_disp_flag((ROB_PARTS_KIND)i, disp);

    m_dp = dp;
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

DataTestEquipDw::Data::Data() : disp(), set_item_no(), reset_item_no(), refresh(),
item_no(), item_set(), cn(), cs(), update_item(), dp(), current_items() {

}

DataTestEquipDw::Data::~Data() {

}

DataTestEquipDw::Item::Item() : comp(), label(), list_box(), parts() {

}

DataTestEquipDw::DataTestEquipDw() : rc() {
    ItemReset();

    for (int32_t i = 0; i < ROB_ID_MAX; i++) {
        DataReset(i);
        data[i].cn = data_test_equip_dw_chara_index_array[i];
    }

    RobItem::init_have_dbg();

    const char* exclusive_check_text;
    if (dw::translate)
        exclusive_check_text = u8"Switch to exclusive check window";
    else
        exclusive_check_text = u8"排他チェック窓に切替";

    dw::Button* exclusive_check = new dw::Button(this);
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

    for (int32_t i = 0; i <= ROB_ID_2P; i++) {
        ROB_ID rob_id = (ROB_ID)i;
        sub_140261520(rob_id);
        SetCharaItemEquip(rob_id, get_rob_management()->get_rob_robitem_work(rob_id)->get_equip());
    }

    rect.pos = { 953.0f, 0.0f };
}

DataTestEquipDw::~DataTestEquipDw() {

}

void DataTestEquipDw::Hide() {
    SetDisp();

    for (DataTestEquipDw::Data& i : data)
        i.update_item = false;

    RobItem::dest_have_dbg();
}

void DataTestEquipDw::DataReset(int32_t rc) {
    DataTestEquipDw::Data& data = this->data[rc];
    data.disp = true;
    data.set_item_no = false;
    data.reset_item_no = false;
    data.refresh = false;
    data.item_no = 0;
    data.cn = CN_MIKU;
    data.update_item = false;
    data.dp = 0;

    for (int32_t i = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        data.items[i].clear();

        if (IsEquipAllowed(i)) {
            data.field_278[i].clear();
            data.current_items[i] = 0;
        }
    }
}

int32_t DataTestEquipDw::GetCharaID() {
    return rc;
}

CHARA_NUM DataTestEquipDw::GetCharaNum(int32_t rc) {
    return data[rc].cn;
}

int32_t DataTestEquipDw::GetCosId(int32_t rc) {
    return data[rc].cs;
}

int32_t DataTestEquipDw::GetCurrentCharaItem(int32_t rc, dw::Widget* data) {
    return this->data[rc].current_items[data->callback_data.i32];
}

int32_t DataTestEquipDw::GetDispParts(int32_t rc) {
    return data[rc].dp;
}

const RobItemEquip* DataTestEquipDw::GetEquip(int32_t rc) {
    return data[rc].item_set;
}

int32_t DataTestEquipDw::GetItemNo(int32_t rc) {
    return data[rc].item_no;
}

bool DataTestEquipDw::GetRefresh(int32_t rc) {
    return data[rc].refresh;
}

bool DataTestEquipDw::GetResetItemNo(int32_t rc) {
    return data[rc].reset_item_no;
}

int32_t DataTestEquipDw::GetSelectedCharaItemNo(int32_t rc, dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        return this->data[rc].items[data->callback_data.i32][list_box->list->selected_item];
    return 0;
}

bool DataTestEquipDw::GetSetItemNo(int32_t rc) {
    return data[rc].set_item_no;
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

void DataTestEquipDw::ResetCharaItemCos(int32_t rc) {
    data[rc].set_item_no = false;
    data[rc].reset_item_no = false;
    data[rc].item_no = 0;
    data[rc].item_set = 0;
}

void DataTestEquipDw::SetCharaDisp(int32_t rc, bool value) {
    data[rc].disp = value;
    data[rc].dp = value ? 0 : 1;
}

void DataTestEquipDw::SetCharaID(int32_t value) {
    rc = value;
}

void DataTestEquipDw::SetCharaItemEquip(int32_t rc, const RobItemEquip* item_set) {
    if (!item_set)
        return;

    DataTestEquipDw::Data& data = this->data[rc];

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        if (this->rc == rc) {
            auto elem = data.field_278[j].find(item_set->item_no[data_test_equip_sub_ids[i]]);
            if (elem != data.field_278[j].end() && elem->second)
                item.list_box[j]->SetItemIndex(elem->second);
            else
                item.list_box[j]->SetItemIndex(-1);
        }

        data.current_items[j] = item_set->item_no[data_test_equip_sub_ids[i]];
        j++;
    }
}

void DataTestEquipDw::SetCharaNum(int32_t rc, CHARA_NUM cn) {
    data[rc].cn = cn;
}

void DataTestEquipDw::SetCosId(int32_t rc, int32_t cs) {
    data[rc].cs = cs;
}

void DataTestEquipDw::SetCurrentItemNo(int32_t rc, dw::Widget* data, int32_t value) {
    this->data[rc].current_items[data->callback_data.i32] = value;
}

void DataTestEquipDw::SetDispParts(int32_t rc, int32_t parts) {
    if (data[rc].disp)
        data[rc].dp = parts;
}

void DataTestEquipDw::SetEquip(int32_t rc, const RobItemEquip* item_set) {
    data[rc].item_set = item_set;
}

void DataTestEquipDw::SetItemNo(int32_t rc, int32_t value) {
    data[rc].item_no = value;
}

void DataTestEquipDw::SetRefresh(int32_t rc, bool value) {
    data[rc].refresh = value;
}

void DataTestEquipDw::SetResetItemNo(int32_t rc, bool value) {
    data[rc].reset_item_no = value;
}

void DataTestEquipDw::SetSetItemNo(int32_t rc, bool value) {
    data[rc].set_item_no = value;
}

void DataTestEquipDw::SetUpdateItem(int32_t rc, bool value) {
    data[rc].update_item = value;
}

void DataTestEquipDw::Disp1pCallback(dw::Widget* data) {
    DispCallback(0, data);
}

void DataTestEquipDw::Disp2pCallback(dw::Widget* data) {
    DispCallback(1, data);
}

void DataTestEquipDw::DispCallback(int32_t rc, dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        data_test_equip_dw->SetCharaDisp(rc, button->value);
}

void DataTestEquipDw::DispPartsCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        data_test_equip_dw->SetDispParts(data_test_equip_dw->GetCharaID(),
            (int32_t)list_box->list->selected_item);
}

void DataTestEquipDw::ExclusiveCheckCallback(dw::Widget* data) {
    data_test_equip_dw->Hide();
    //data_test_item_check_open(&data_test_item_check);
}

void DataTestEquipDw::ItemCallback(dw::Widget* data) {
    int32_t rc = data_test_equip_dw->GetCharaID();
    uint32_t item_no = data_test_equip_dw->GetSelectedCharaItemNo(rc, data);
    int32_t curr_item_no = data_test_equip_dw->GetCurrentCharaItem(rc, data);

    if (item_no) {
        data_test_equip_dw->SetSetItemNo(rc, true);
        data_test_equip_dw->SetResetItemNo(rc, false);
        data_test_equip_dw->SetUpdateItem(rc, true);
        data_test_equip_dw->SetItemNo(rc, item_no);
        data_test_equip_dw->SetCurrentItemNo(rc, data, item_no);
    }
    else if (curr_item_no) {
        data_test_equip_dw->SetSetItemNo(rc, false);
        data_test_equip_dw->SetResetItemNo(rc, true);
        data_test_equip_dw->SetUpdateItem(rc, true);
        data_test_equip_dw->SetItemNo(rc, curr_item_no);
        data_test_equip_dw->SetCurrentItemNo(rc, data, curr_item_no);
    }
    else{
        data_test_equip_dw->SetItemNo(rc, item_no);
        data_test_equip_dw->SetCurrentItemNo(rc, data, item_no);
    }

}

void DataTestEquipDw::PartsCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (!list_box)
        return;

    int32_t rc = data_test_equip_dw->GetCharaID();
    CHARA_NUM cn = data_test_equip_dw->GetCharaNum(rc);
    int32_t cos = data_test_equip_dw->GetCosId(rc);

    const RobItemEquip* item_set = get_default_costume_data(cn, cos);
    size_t selected_item = list_box->list->selected_item;
    switch (selected_item) {
    case 0:
        break;
    case 1:
        item_set = get_default_costume_data(cn, 499);
        break;
    case 2:
    default: {
        const RobItemDbgSet* dbgset = RobItem::get_dbgset(cn);
        auto elem = dbgset->find(list_box->GetItemStr(selected_item).c_str());
        if (elem != dbgset->end())
            item_set = &elem->second;
    } break;
    }

    RobItemEquip* rob_item_set = get_rob_management()->get_rob_robitem_work((ROB_ID)rc)->get_equip();
    if (selected_item >= 2 && data_test_equip_dw->GetRefresh(rc)) {
        *rob_item_set = *item_set;
        item_set = rob_item_set;
    }

    data_test_equip_dw->SetEquip(rc, item_set);
    data_test_equip_dw->sub_140261C40(rc);
    data_test_equip_dw->sub_140261580(rc);
    data_test_equip_dw->UpdateLayout();
    data_test_equip_dw->SetCharaItemEquip(rc, item_set);
}

void DataTestEquipDw::RefreshCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        int32_t rc = data_test_equip_dw->GetCharaID();
        if (button->value)
            data_test_equip_dw->SetRefresh(rc, false);
        else
            data_test_equip_dw->SetRefresh(rc, true);
    }
}

void DataTestEquipDw::RobCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        int32_t rc = (int32_t)list_box->list->selected_item;
        data_test_equip_dw->SetCharaID(rc);
        data_test_equip_dw->sub_140261C40(rc);
        data_test_equip_dw->sub_140261AF0(rc);
        data_test_equip_dw->sub_14025FD80(rc);
    }
}

void DataTestEquipDw::sub_14025FD80(int32_t rc) {
    if (this->rc != rc)
        return;

    DataTestEquipDw::Data& data = this->data[rc];

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

void DataTestEquipDw::sub_140261520(int32_t rc) {
    if (this->rc == rc) {
        sub_140261C40(rc);
        sub_140261AF0(rc);
        sub_140261580(rc);
        UpdateLayout();
    }
    else
        sub_140261580(rc);
}

void DataTestEquipDw::sub_140261580(int32_t rc) {
    DataTestEquipDw::Data& data = this->data[rc];

    CHARA_NUM cn = data.cn;

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        RobItemHaveSub* have_dbg = RobItem::get_have_sub_dbg(cn);
        data.items[j].clear();
        data.field_278[j].clear();
        data.current_items[j] = 0;
        data.items[j].push_back(0);

        data.field_278[j].insert({ 0, 0 });

        int32_t index = 1;
        for (uint32_t& k : have_dbg->part[data_test_equip_sub_ids[i]].item_no) {
            data.items[j].push_back(k);
            data.field_278[j].insert({ k, index++ });
        }
        j++;
    }
}

void DataTestEquipDw::sub_140261AF0(int32_t rc) {
    dw::ListBox* parts = item.parts;
    parts->ClearItems();
    parts->AddItem("DEFAULT");
    parts->AddItem("NUDE");

    for (auto& i : *RobItem::get_dbgset(data[rc].cn))
        parts->AddItem(i.first);
}

void DataTestEquipDw::sub_140261C40(int32_t rc) {
    DataTestEquipDw::Data& data = this->data[rc];

    CHARA_NUM cn = data.cn;

    char buf[0x20];
    sprintf_s(buf, sizeof(buf), "ITEM EQUIP (%s)", get_chara_name_full(cn));
    SetText(buf);

    for (int32_t i = 0, j = 0; i < DATA_TEST_ITEM_EQUIP_MAX; i++) {
        if (!IsEquipAllowed(i))
            continue;

        RobItemHaveSub* have_dbg = RobItem::get_have_sub_dbg(cn);
        item.list_box[j]->ClearItems();
        item.list_box[j]->AddItem("REMOVE ITEM");

        for (uint32_t& k : have_dbg->part[data_test_equip_sub_ids[i]].item_no)
            item.list_box[j]->AddItem(sprintf_s_string("%03d", k) + get_rob_item_table_name(cn, k));
        j++;
    }
}

void data_test_equip_dw_init() {
    if (!data_test_equip_dw) {
        data_test_equip_dw = new DataTestEquipDw;
        data_test_equip_dw->LimitPosDisp();
    }
    else {
        RobItem::init_have_dbg();
        //if (!data_test_item_check_dw_get_disp())
            data_test_equip_dw->Disp();
    }
}

static int32_t data_test_equip_dw_get_disp_parts(int32_t rc) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetDispParts(rc);
    return 0;
}

static void data_test_equip_dw_set_chara_num(int32_t rc, CHARA_NUM cn) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCharaNum(rc, cn);
    data_test_equip_dw_chara_index_array[rc] = cn;
}

static void data_test_equip_dw_set_chara_item_equip(int32_t rc, const RobItemEquip* item_set) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCharaItemEquip(rc, item_set);
}

static const RobItemEquip* data_test_equip_dw_get_equip(int32_t rc) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetEquip(rc);
    return 0;
}

static int32_t data_test_equip_dw_get_item_no(int32_t rc) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetItemNo(rc);
    return 0;
}

static bool data_test_equip_dw_get_reset_item_no(int32_t rc) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetResetItemNo(rc);
    return false;
}

static bool data_test_equip_dw_get_set_item_no(int32_t rc) {
    if (data_test_equip_dw)
        return data_test_equip_dw->GetSetItemNo(rc);
    return false;
}

static void data_test_equip_dw_reset_chara_item_cos(int32_t rc) {
    if (data_test_equip_dw)
        data_test_equip_dw->ResetCharaItemCos(rc);
}

static void data_test_equip_dw_set_cos_id(int32_t rc, int32_t cos) {
    if (data_test_equip_dw)
        data_test_equip_dw->SetCosId(rc, cos);
}

static void data_test_equip_dw_sub_140261520(int32_t rc) {
    if (data_test_equip_dw)
        data_test_equip_dw->sub_140261520(rc);
}
