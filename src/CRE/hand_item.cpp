/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hand_item.hpp"
#include "../KKdLib/database/hand_item.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "mdata_manager.hpp"
#include <map>

struct hand_item_handler {
    std::list<p_file_handler*> file_handlers;
    bool ready;
    std::map<std::pair<int32_t, chara_index>, hand_item> hand_items;

    hand_item_handler();
    ~hand_item_handler();

    void clear();
    const hand_item* get_hand_item(int32_t uid, chara_index chara_index);
    int32_t get_hand_item_uid(const char* str);
    bool load();
    void parse(p_file_handler* pfhndl);
    void read();
};

static void hand_item_load(data_struct* data,
    std::map<std::pair<int32_t, chara_index>, hand_item>& hand_items, hnd_itm& hnd_itm_file);

static int32_t mtp_hand_array_get_mottbl_index(const char* str);

hand_item_handler* hand_item_handler_data;

hand_item::hand_item() : file_size(), hand_mottbl_index(), hand_scale(), uid() {

}

hand_item::~hand_item() {

}

void hand_item_handler_data_init() {
    if (!hand_item_handler_data)
        hand_item_handler_data = new hand_item_handler;
}

const hand_item* hand_item_handler_data_get_hand_item(int32_t uid, chara_index chara_index) {
    if (uid >= 0)
        return hand_item_handler_data->get_hand_item(uid, chara_index);
    return 0;
}

const std::map<std::pair<int32_t, chara_index>, hand_item>& hand_item_handler_data_get_hand_items() {
    return hand_item_handler_data->hand_items;
}

int32_t hand_item_handler_data_get_hand_item_uid(const char* str) {
    if (str)
        return hand_item_handler_data->get_hand_item_uid(str);
    return -1;
}

bool hand_item_handler_data_load() {
    return hand_item_handler_data->load();
}

void hand_item_handler_data_read() {
    hand_item_handler_data->read();
}

void hand_item_handler_data_free() {
    if (hand_item_handler_data) {
        delete hand_item_handler_data;
        hand_item_handler_data = 0;
    }
}

hand_item_handler::hand_item_handler() : ready() {

}

hand_item_handler::~hand_item_handler() {
    clear();
}

void hand_item_handler::clear() {
    ready = false;
    hand_items.clear();

    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();
}

const hand_item* hand_item_handler::get_hand_item(int32_t uid, chara_index chara_index) {
    auto elem0 = hand_items.find({ uid, chara_index });
    if (elem0 != hand_items.end())
        return &elem0->second;

    auto elem1 = hand_items.find({ uid, CHARA_NONE });
    if (elem1 != hand_items.end())
        return &elem1->second;
    return 0;
}

int32_t hand_item_handler::get_hand_item_uid(const char* str) {
    if (str && hand_items.size())
        for (auto& i : hand_items)
            if (!i.second.item_str.compare(str))
                return i.first.first;
    return -1;
}

bool hand_item_handler::load() {
    if (ready)
        return false;

    for (p_file_handler*& i : file_handlers)
        if (i->check_not_ready())
            return true;

    for (p_file_handler*& i : file_handlers) {
        if (!i)
            continue;

        parse(i);

        delete i;
        i = 0;
    }
    file_handlers.clear();

    ready = true;
    return false;
}

void hand_item_handler::parse(p_file_handler* pfhndl) {
    hnd_itm hnd_itm;
    hnd_itm.read(pfhndl->get_data(), pfhndl->get_size());
    if (hnd_itm.ready)
        hand_item_load(&data_list[DATA_AFT], hand_items, hnd_itm);
}

void hand_item_handler::read() {
    ready = false;

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string dir;
        dir.assign("rom/");
        dir.append(i);

        const char* file = "hand_item_data.txt";

        if (!aft_data->check_file_exists(dir.c_str(), file))
            continue;

        p_file_handler* pfhndl = new p_file_handler;
        pfhndl->read_file(aft_data, dir.c_str(), file);
        file_handlers.push_back(pfhndl);
    }
}

static void hand_item_load(data_struct* data,
    std::map<std::pair<int32_t, chara_index>, hand_item>& hand_items, hnd_itm& hnd_itm_file) {
    object_database* aft_obj_db = &data->data_ft.obj_db;

    for (hnd_itm_data& i : hnd_itm_file.data) {
        hand_item itm;
        itm.obj_left = aft_obj_db->get_object_info(i.objname_left.c_str());
        itm.obj_right = aft_obj_db->get_object_info(i.objname_right.c_str());
        itm.item_str.assign(i.item_str);
        itm.item_name.assign(i.item_name);
        itm.file_size = i.file_size;
        itm.hand_scale = i.hand_scale;
        itm.uid = i.uid;

        uint32_t hand_motion_id = mtp_hand_array_get_mottbl_index(i.hand_motion.c_str());
        if (hand_motion_id == -1)
            continue;

        itm.hand_mottbl_index = hand_motion_id;
        hand_items.insert({ { itm.uid, CHARA_NONE }, itm });
    }
}

static int32_t mtp_hand_array_get_mottbl_index(const char* str) {
    static const std::pair<const char*, int32_t> mtp_hand_array[] = {
        { "MTP_HAND_NULL"      , 0xC0 },
        { "MTP_HAND_RESET"     , 0xC1 },
        { "MTP_HAND_NORMAL"    , 0xC2 },
        { "MTP_HAND_OPEN"      , 0xC3 },
        { "MTP_HAND_CLOSE"     , 0xC4 },
        { "MTP_HAND_PEACE"     , 0xC5 },
        { "MTP_HAND_GOOD"      , 0xC6 },
        { "MTP_HAND_ONE"       , 0xC7 },
        { "MTP_HAND_THREE"     , 0xC8 },
        { "MTP_HAND_MEGI"      , 0xC9 },
        { "MTP_HAND_SIZEN"     , 0xCA },
        { "MTP_HAND_PICK"      , 0xCB },
        { "MTP_HAND_MIC"       , 0xCC },
        { "MTP_HAND_FAN"       , 0xCD },
        { "MTP_HAND_BOTTLE"    , 0xCE },
        { "MTP_HAND_PHONE"     , 0xCF },
        { "MTP_HAND_HOLD"      , 0xD0 },
        { "MTP_HAND_FLASHLIGHT", 0xD1 },
        { "MTP_HAND_MIC_BLK"   , 0xD2 },
        { "MTP_HAND_MIC_SLV"   , 0xD3 },
        { "MTP_HAND_CUPICE"    , 0xD4 },
        { "MTP_HAND_ICEBAR"    , 0xD5 },
    };

    for (auto& i : mtp_hand_array)
        if (!str_utils_compare(i.first, str))
            return i.second;
    return -1;
}
