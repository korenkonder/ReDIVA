/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_2d_test.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/auth_2d.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/sound.hpp"
#include "../../CRE/sprite.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/task_effect.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "../task_window.hpp"

extern int32_t width;
extern int32_t height;

DtmAet* dtm_aet;
DtwAet* dtw_aet;

DtmAet::DtmAet() : curr_set_index(), set_index(), id_index(), layer_index(), marker_index(),
type(), frame(), start_time(), end_time(), lock(), loop(), centering(), state() {

}

DtmAet::~DtmAet() {

}

bool DtmAet::Init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(this->curr_set_index);
    std::string mdata_dir = "";//GetMdataDir(set_id);
    sprite_manager_read_file(aft_aet_db->get_aet_set_by_id(set_id)->sprite_set_id,
        mdata_dir, aft_data, aft_spr_db);
    aet_manager_read_file(set_id, mdata_dir, aft_data, aft_aet_db);
    GetSoundFarcs();
    state = 1;
    return true;
}

bool DtmAet::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    switch (state) {
    case 1: {
        bool wait_load = false;

        for (std::string& i : sound_farcs)
            wait_load |= sound_work_load_farc(i.c_str());

        if (!wait_load)
            state = 2;
    } break;
    case 2: {
        uint32_t prev_set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        sprite_manager_unload_set(aft_aet_db->get_aet_set_by_id(prev_set_id)->sprite_set_id, aft_spr_db);
        aet_manager_unload_set(prev_set_id, aft_aet_db);

        curr_set_index = set_index;
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(set_index);

        std::string mdata_dir = "";//GetMdataDir(set_id);
        sprite_manager_read_file(aft_aet_db->get_aet_set_by_id(set_id)->sprite_set_id,
            mdata_dir, aft_data, aft_spr_db);
        aet_manager_read_file(set_id, mdata_dir, aft_data, aft_aet_db);
        state = 3;
    } break;
    case 3: {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        if (sprite_manager_load_file(aft_aet_db->get_aet_set_by_id(set_id)->sprite_set_id, aft_spr_db)
            || aet_manager_load_file(set_id, aft_aet_db))
            break;

        dw::List* id_list = dtw_aet->id->list;
        id_list->ClearItems();

        uint32_t scenes_count = aet_manager_get_scenes_count(set_id, aft_aet_db);
        for (uint32_t i = scenes_count, j = 0; i; i--, j++)
            id_list->AddItem(aet_manager_get_scene_name(
                aft_aet_db->get_aet_by_set_id_index(set_id, j)->id, aft_aet_db));

        id_list->hovered_item = id_index;
        id_list->ResetSetSelectedItem(id_index);
        state = 4;
    } break;
    case 4: {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        uint32_t aet_id = aft_aet_db->get_aet_by_set_id_index(set_id, id_index)->id;

        comp_layers.clear();
        aet_manager_get_scene_comp_layer_names(comp_layers, aet_id, aft_aet_db);
        prj::sort_unique(comp_layers);

        auto i = comp_layers.begin();
        auto i_end = comp_layers.end();
        while (i != i_end)
            if (i->find(".pic") != -1)
                i = comp_layers.erase(i);
            else
                i++;

        dw::List* layer_list = dtw_aet->layer->list;
        layer_list->ClearItems();
        layer_list->AddItem("ROOT");
        for (std::string& i : comp_layers)
            layer_list->AddItem(i);

        layer_list->hovered_item = layer_index;
        layer_list->ResetSetSelectedItem(layer_index);
        state = 5;
    } break;
    case 5: {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        uint32_t aet_id = aft_aet_db->get_aet_by_set_id_index(set_id, id_index)->id;

        markers.clear();

        if (layer_index)
            aet_manager_get_scene_layer_marker_names(markers, aet_id,
                comp_layers[layer_index - 1].c_str(), aft_aet_db);

        if (layer_index)
            start_time = aet_manager_get_scene_layer_start_time(aet_id,
                comp_layers[layer_index - 1].c_str(), aft_aet_db);
        else
            start_time = aet_manager_get_scene_start_time(aet_id, aft_aet_db);

        if (layer_index)
            end_time = aet_manager_get_scene_layer_end_time(aet_id,
                comp_layers[layer_index - 1].c_str(), aft_aet_db);
        else
            end_time = aet_manager_get_scene_end_time(aet_id, aft_aet_db);

        dw::ListBox* marker = dtw_aet->marker;
        if (marker->list)
            marker->ClearItems();

        marker->AddItem("NOMARKER");

        for (std::string& i : markers)
            marker->AddItem(i);

        if (marker->list) {
            marker->list->hovered_item = marker_index;
            marker->list->ResetSetSelectedItem(marker_index);
        }
        state = 6;
    } break;
    case 6: {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        uint32_t aet_id = aft_aet_db->get_aet_by_set_id_index(set_id, id_index)->id;

        float_t frame;
        if (layer_index && marker_index)
            frame = aet_manager_get_scene_layer_marker_time(aet_id,
                comp_layers[layer_index - 1].c_str(), markers[marker_index - 1].c_str(), aft_aet_db);
        else
            frame = start_time;

        char buf[0x20];
        sprintf_s(buf, sizeof(buf), "%f frame", frame);
        dtw_aet->marker_frame->SetName(buf);
        this->frame = frame;
        state = 7;
    } break;
    case 7: {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
        uint32_t aet_id = aft_aet_db->get_aet_by_set_id_index(set_id, id_index)->id;

        const char* layer_name = 0;
        if (layer_index)
            layer_name = comp_layers[layer_index - 1].c_str();

        if (type) {
            comp.data.clear();
            aet_manager_init_aet_layout(&comp, aet_id, layer_name,
                (AetFlags)0, RESOLUTION_MODE_HD, 0, frame, aft_aet_db);
        }
        else {
            float_t x = 0.0f;
            float_t y = 0.0f;

            if (centering) {
                x = res_window_get()->width * 0.5f;
                y = res_window_get()->height * 0.5f;
            }

            AetArgs args;
            args.id.id = aet_id;
            args.layer_name = layer_name;
            args.start_time = frame;
            args.end_time = frame;
            args.pos.x = x;
            args.pos.y = y;
            args.pos.z = 0.0f;
            args.sound_voice = true;
            args.spr_db = aft_spr_db;
            aet_manager_init_aet_object(args, aft_aet_db);
        }

        dtw_aet->frame->scroll_bar->SetParams(frame,
            start_time, end_time, (end_time - start_time) * 0.1f, 1.0f, 10.0f);
    } break;
    }
    return false;
}

bool DtmAet::Dest() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index(curr_set_index);
    sprite_manager_unload_set(aft_aet_db->get_aet_set_by_id(set_id)->sprite_set_id, aft_spr_db);
    aet_manager_unload_set(set_id, aft_aet_db);

    for (std::string& i : sound_farcs)
        sound_work_unload_farc(i.c_str());
    sound_farcs.clear();
    state = 0;
    return true;
}

void DtmAet::Disp() {
    if (state != 7)
        return;
}

void DtmAet::Basic() {
    if (state != 7)
        return;

    if (!lock)
        frame += 1.0f;

    if (frame > end_time)
        if (loop)
            frame = start_time;
        else
            frame = end_time;
}

void DtmAet::GetSoundFarcs() {
    static const char* dtm_aet_sound_farcs[] = {
        "rom/sound/se_sel.farc",
        "rom/sound/se_result.farc",
        "rom/sound/se_adv.farc",
        "rom/sound/se_over.farc",
    };

    sound_farcs.clear();
    for (const char* i : dtm_aet_sound_farcs)
        if (sound_work_read_farc(i))
            sound_farcs.push_back(i);
}

void DtmAet::SetSetIndex(int32_t value) {
    if (curr_set_index != value) {
        set_index = value;
        id_index = 0;
        layer_index = 0;
        marker_index = 0;
        state = 2;
    }
}

void DtmAet::SetIdIndex(int32_t value) {
    if (id_index != value) {
        id_index = value;
        layer_index = 0;
        marker_index = 0;
        state = 4;
    }
}

void DtmAet::SetLayerIndex(int32_t value) {
    if (layer_index != value) {
        layer_index = value;
        marker_index = 0;
        state = 5;
    }
}

void DtmAet::SetMarkerIndex(int32_t value) {
    if (marker_index != value) {
        marker_index = value;
        state = 6;
    }
}

void DtmAet::SetType(uint8_t value) {
    if (type != value) {
        type = value;
        state = 7;
    }
}

void DtmAet::SetFrame(float_t value) {
    if (frame != value)
        frame = value;
}

void DtmAet::SetLock(bool value) {
    if (lock != value)
        lock = value;
}

void DtmAet::SetLoop(bool value) {
    if (loop != value)
        loop = value;
}

void DtmAet::SetCentering(bool value) {
    if (centering != value)
        centering = value;
}

DtwAet::DtwAet() : Shell(0) {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    position = { 0.0f, 0.0f };
    size = { 220.0f, 380.0f };

    dw::Widget::SetName("2DAUTH TEST");

    (new dw::Label(this))->SetName("SET");

    set = new dw::ListBox(this);

    for (size_t i = aft_aet_db->aet_set_names.size(), j = 0; i; i--, j++) {
        uint32_t set_id = aft_aet_db->get_aet_set_id_by_name_index((uint32_t)j);
        const aet_db_aet_set* set = aft_aet_db->get_aet_set_by_id(set_id);
        this->set->list->AddItem(set->name);
    }

    //set->list->sub_1402F9930(20);
    set->list->hovered_item = dtm_aet->curr_set_index;
    set->list->ResetSetSelectedItem(dtm_aet->curr_set_index);

    set->list->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::SetCallback));

    (new dw::Label(this))->SetName("ID");

    id = new dw::ListBox(this);
    //id->list->sub_1402F9930(20);

    id->list->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::IdCallback));

    (new dw::Label(this))->SetName("LAYER");

    layer = new dw::ListBox(this);
    layer->AddItem("ROOT");
    //layer->list->sub_1402F9930(20);

    layer->list->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::LayerCallback));

    (new dw::Label(this))->SetName("MARKER");

    marker = new dw::ListBox(this);
    marker->AddItem("NOMARKER");
    //marker->list->sub_1402F9930(20);

    marker->list->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::MarkerCallback));

    marker_frame = new dw::Label(this);
    marker_frame->SetName("0");

    (new dw::Label(this))->SetName("TYPE");

    type = new dw::ListBox(this);
    type->AddItem("DRAW");
    type->AddItem("LAYOUT");

    type->list->hovered_item = dtm_aet->type;
    type->list->ResetSetSelectedItem(dtm_aet->type);

    type->list->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::TypeCallback));

    (new dw::Label(this))->SetName("FRAME");

    frame = dw::Slider::make(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 128.0f, 20.0f, "slider");
    frame->SetName("FRAME");
    frame->format = "%4.0f";

    frame->scroll_bar->SetParams(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 10.0f);

    frame->AddSelectionListener(new dw::SelectionListenerOnHook(DtwAet::FrameCallback));

    (new dw::Label(this))->SetName("SWITCH");

    dw::Button* lock = new dw::Button(this, dw::CHECKBOX);
    lock->SetName("LOCK");
    lock->callback = DtwAet::LockCallback;

    dw::Button* loop = new dw::Button(this, dw::CHECKBOX);
    loop->SetName("LOOP");
    loop->callback = DtwAet::LoopCallback;

    dw::Button* centering = new dw::Button(this, dw::CHECKBOX);
    centering->SetName("CENTERING");
    centering->callback = DtwAet::CenteringCallback;

    //GetSetSize();
}

DtwAet::~DtwAet() {

}

void DtwAet::Hide() {
    SetDisp(false);
}

void DtwAet::CenteringCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dtm_aet->SetCentering(button->value);
}

void DtwAet::FrameCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        dtm_aet->SetFrame(slider->scroll_bar->value);
}

void DtwAet::IdCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dtm_aet->SetIdIndex((int32_t)list_box->list->selected_item);
}

void DtwAet::LayerCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dtm_aet->SetLayerIndex((int32_t)list_box->list->selected_item);
}

void DtwAet::LockCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dtm_aet->SetLock(button->value);
}

void DtwAet::LoopCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dtm_aet->SetLoop(button->value);
}

void DtwAet::MarkerCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dtm_aet->SetMarkerIndex((int32_t)list_box->list->selected_item);
}

void DtwAet::SetCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dtm_aet->SetSetIndex((int32_t)list_box->list->selected_item);
}

void DtwAet::TypeCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dtm_aet->SetType(list_box->list->selected_item ? 1 : 0);
}

void dtm_aet_init() {
    dtm_aet = new DtmAet;
}

void dtm_aet_load() {
    if (app::TaskWork::CheckTaskReady(dtm_aet))
        return;

    app::TaskWork::AddTask(dtm_aet, "DATA_TEST_AET_MANAGER");
    if (!dtw_aet)
        dtw_aet = new DtwAet();
    dtw_aet->Disp();
}

void dtm_aet_unload() {
    dtm_aet->DelTask();
    dtw_aet->Hide();
}

void dtm_aet_free() {
    if (dtw_aet) {
        delete dtw_aet;
        dtw_aet = 0;
    }

    if (dtm_aet) {
        delete dtm_aet;
        dtm_aet = 0;
    }
}
