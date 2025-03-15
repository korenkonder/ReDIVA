/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob_chara_adjust.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../data_test/motion_test.hpp"
#include "../dw.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"

static const char* chara_name[] = {
    "1P",
    "2P",
};

static const char* parts_name[] = {
    "",
    "Left",
    "Right",
    "Center",
    "Long C",
    "Short L",
    "Short R",
    "Append L",
    "Append R",
    "Muffler",
    "White One L",
    "Pony",
    "Angel L",
    "Angel R",
    "Global",
};

static const char* type_name[] = {
    "Disable",
    "No rotation",
    "Body rotation",
    "Chest rotation",
    "Neck rotation",
    "Face rotation",
    "Waist rotation",
};

static const char* cycle_type_name[] = {
    "No cycle",
    "Sine",
    "Cosine",
};

RobCharaAdjust* rob_chara_adjust;

extern bool input_locked;

extern render_context* rctx_ptr;

RobCharaAdjust::RobCharaAdjust() : apply(), apply_wait(),
apply_frame(), save(), track_frame(), visible(), chara_id(), data() {
    motion_id = -1;
    parts = ROB_OSAGE_PARTS_NONE;
}

RobCharaAdjust::~RobCharaAdjust() {

}

bool RobCharaAdjust::init() {
    return true;
}

bool RobCharaAdjust::ctrl() {
    if (motion_id != dtm_mot_array[chara_id].motion_id) {
        motion_id = dtm_mot_array[chara_id].motion_id;
        parts = ROB_OSAGE_PARTS_NONE;
        data = 0;
    }

    if (save) {
        save = false;

        path_create_directory("ram/rob/");

        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        uint32_t set_id = dtm_mot_array[chara_id].motion_set_id;

        MhdFile* mhd = mothead_storage_get_mhd_file(set_id);
        const motion_set_info* set_info = aft_mot_db->get_motion_set_by_id(set_id);
        if (mhd && set_info) {
            std::string file;
            file.assign("mothead_");
            file.append(set_info->name);
            file.append(".bin");

            std::string path("ram/rob/");
            path.append(file);

            file_stream s;
            s.open(path.c_str(), "wb");
            s.write(mhd->file_handler.get_data(), mhd->file_handler.get_size());
            s.close();
        }
    }

    if (apply && dtm_mot_array[chara_id].state == 13) {
        apply = false;

        apply_wait = true;
        apply_frame = dtm_mot_array[chara_id].GetFrame();
        dtm_mot_array[chara_id].SetResetMot();
    }
    else if (apply_wait && dtm_mot_array[chara_id].state == 13) {
        apply_wait = false;

        dtm_mot_array[chara_id].SetFrame(apply_frame);
    }
    return false;
}

bool RobCharaAdjust::dest() {
    return true;
}

void RobCharaAdjust::window() {
    if (Input::IsKeyTapped(GLFW_KEY_A, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
        visible ^= true;

    if (!visible)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 360.0f;
    float_t h = (float_t)height;
    h = min_def(h, 371.0f);

    ImGui::SetNextWindowPos({ 0.0f, 349.0f }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Rob Chara Adjust", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        visible = false;
        ImGui::End();
        return;
    }

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("head", 5)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.2f);

        ImGui::TableNextColumn();
        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::BeginCombo("##Chara", chara_name[chara_id], 0)) {
            input_locked |= true;

            for (int32_t i = 0; i < ROB_CHARA_COUNT && i < 2; i++)
                if (ImGui::Selectable(chara_name[i], chara_id == i)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter))
                    chara_id = i;
            ImGui::EndCombo();
        }

        ImGui::TableNextColumn();
        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::BeginCombo("##Parts", parts_name[(int32_t)parts + 1], 0)) {
            input_locked |= true;

            for (int32_t i = ROB_OSAGE_PARTS_LEFT; i <= ROB_OSAGE_PARTS_MAX; i++)
                if (ImGui::Selectable(parts_name[i + 1], parts == i)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter))
                    parts = (rob_osage_parts)i;

            ImGui::EndCombo();
        }

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::Button("Save", { w, 0.0f }))
            save = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
#if OPD_PLAY
        ImGui::BeginDisabled(!(dtm_mot_array[chara_id].use_opd && dtm_mot_array[chara_id].state == 13));
#else
        ImGui::BeginDisabled(true);
#endif
        if (ImGui::Button("Apply", { w, 0.0f }))
            apply = true;
        ImGui::EndDisabled();

        ImGui::TableNextColumn();
        ImGui::Checkbox("Track", &track_frame);

        input_locked |= ImGui::IsWindowFocused();
        ImGui::EndTable();
    }

    if (track_frame) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        float_t frame = dtm_mot_array[chara_id].GetFrame();
        const mothead_data* data = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data;
        if (data) {
            mothead_data_type type = data->type;
            while (type >= MOTHEAD_DATA_TYPE_0) {
                if ((type == MOTHEAD_DATA_ROB_PARTS_ADJUST
                    && ((RobCharaAdjust::PartsData*)data->data)->parts == parts
                    || type == MOTHEAD_DATA_ROB_ADJUST_GLOBAL)
                    && frame >= (float_t)data->frame) {
                    this->data = (void*)data->data;
                }

                data++;
                type = data->type;
            }
        }
    }

    ImVec2 cont_reg_avail = ImGui::GetContentRegionAvail();
    if (ImGui::BeginListBox("##Frames", { cont_reg_avail.x * 0.18f, cont_reg_avail.y })) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        const mothead_data* data = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data;
        if (data) {
            mothead_data_type type = data->type;
            while (type >= MOTHEAD_DATA_TYPE_0) {
                if (type == MOTHEAD_DATA_ROB_PARTS_ADJUST
                    && ((RobCharaAdjust::PartsData*)data->data)->parts == parts
                    || type == MOTHEAD_DATA_ROB_ADJUST_GLOBAL) {
                    char buf[0x40];
                    sprintf_s(buf, sizeof(buf), "%d", data->frame);
                    ImGui::PushID(data);
                    if (ImGui::Selectable(buf, this->data == data->data) && !track_frame)
                        this->data = (void*)data->data;
                    ImGui::PopID();
                }

                data++;
                type = data->type;
            }
        }
        ImGui::EndListBox();
    }

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::SetColumnSpace((float_t)(2.0 / 5.0));
    if (data && parts <= ROB_OSAGE_PARTS_MAX) {
        RobCharaAdjust::PartsData* data = (RobCharaAdjust::PartsData*)this->data;

        int32_t transition_duration = data->transition_duration;
        if (ImGui::ColumnDragInt("Transition", &transition_duration, 1, 0, INT32_MAX))
            data->transition_duration = transition_duration;

        int32_t type = data->type;

        ImGui::StartPropertyColumn("Ext. Force Type");
        if (ImGui::BeginCombo("", type_name[type + 1], 0)) {
            input_locked |= true;

            bool type_changed = false;
            for (int32_t i = -1; i < 6; i++)
                if (ImGui::Selectable(type_name[i + 1], type == i)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)) {
                    type_changed = true;
                    type = i;
                }

            if (type_changed)
                data->type = type;

            ImGui::EndCombo();
        }
        ImGui::EndPropertyColumn();

        bool ignore_gravity = !!data->ignore_gravity;
        if (ImGui::Checkbox("Ignore Gravity", &ignore_gravity))
            data->ignore_gravity = ignore_gravity ? 0x01 : 0x00;

        int32_t cycle_type = data->cycle_type;
        if (ImGui::ColumnComboBox("Cycle Type", cycle_type_name,
            2, &cycle_type, ImGuiComboFlags_None, true, &input_locked))
            data->cycle_type = (uint8_t)cycle_type;

        vec3 external_force = data->external_force;
        ImGui::GetContentRegionAvailSetNextItemWidth();
        ImGui::TextCentered("External Force");
        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::DragFloat3("##External Force",
            (float_t*)&external_force, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force = external_force;

        vec3 external_force_cycle_strength = data->external_force_cycle_strength;
        ImGui::GetContentRegionAvailSetNextItemWidth();
        ImGui::TextCentered("External Force Cycle Strength");
        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::DragFloat3("##External Force Cycle Strength",
            (float_t*)&external_force_cycle_strength, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force_cycle_strength = external_force_cycle_strength;

        vec3 external_force_cycle = data->external_force_cycle;
        ImGui::GetContentRegionAvailSetNextItemWidth();
        ImGui::TextCentered("External Force Cycle");
        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::DragFloat3("##External Force Cycle",
            (float_t*)&external_force_cycle, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force_cycle = external_force_cycle;

        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::BeginTable("cycle phase", 2)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

            ImGui::TableNextColumn();
            float_t cycle = data->cycle;
            if (ImGui::ColumnDragFloat("Cycle", &cycle, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
                data->cycle = cycle;

            ImGui::TableNextColumn();
            float_t phase = data->phase;
            if (ImGui::ColumnDragFloat("Phase", &phase, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
                data->phase = phase;

            input_locked |= ImGui::IsWindowFocused();
            ImGui::EndTable();
        }

        float_t force = data->force;
        if (ImGui::ColumnDragFloat("Force", &force, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->force = force;

        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::BeginTable("strength", 2)) {
            ImGui::TableNextColumn();
            float_t strength = data->strength;
            if (ImGui::ColumnDragFloat("Stren.", &strength, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
                data->strength = strength;

            ImGui::TableNextColumn();
            int32_t strength_transition = data->strength_transition;
            if (ImGui::ColumnDragInt("Trans.", &strength_transition, 1.0f, 0, INT32_MAX))
                data->strength_transition = strength_transition;

            input_locked |= ImGui::IsWindowFocused();
            ImGui::EndTable();
        }
    }
    else if (data && parts == ROB_OSAGE_PARTS_MAX) {
        RobCharaAdjust::GlobalData* data = (RobCharaAdjust::GlobalData*)this->data;

        int32_t transition_duration = data->transition_duration;
        if (ImGui::DragInt("Transition Duration", &transition_duration, 0, INT32_MAX))
            data->transition_duration = transition_duration;

        int32_t type = data->type;

        ImGui::StartPropertyColumn("Ext. Force Type");
        if (ImGui::BeginCombo("", type_name[type + 1], 0)) {
            input_locked |= true;

            bool type_changed = false;
            for (int32_t i = -1; i < 6; i++) {
                if (ImGui::Selectable(type_name[i + 1], type == i)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)) {
                    type_changed = true;
                    type = i;
                }
            }

            if (type_changed)
                data->type = type;

            ImGui::EndCombo();
        }
        ImGui::EndPropertyColumn();

        int32_t cycle_type = data->cycle_type;
        if (ImGui::ColumnComboBox("Cycle Type", cycle_type_name,
            2, &cycle_type, ImGuiComboFlags_None, true, &input_locked))
            data->cycle_type = (uint8_t)cycle_type;

        vec3 external_force = data->external_force;
        ImGui::TextCentered("External Force");
        if (ImGui::DragFloat3("##External Force",
            (float_t*)&external_force, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force = external_force;

        vec3 external_force_cycle_strength = data->external_force_cycle_strength;
        ImGui::TextCentered("External Force Cycle Strength");
        if (ImGui::DragFloat3("##External Force Cycle Strength",
            (float_t*)&external_force_cycle_strength, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force_cycle_strength = external_force_cycle_strength;

        vec3 external_force_cycle = data->external_force_cycle;
        ImGui::TextCentered("External Force Cycle");
        if (ImGui::DragFloat3("##External Force Cycle",
            (float_t*)&external_force_cycle, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
            data->external_force_cycle = external_force_cycle;

        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::BeginTable("cycle phase", 2)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

            ImGui::TableNextColumn();
            ImGui::GetContentRegionAvailSetNextItemWidth();

            float_t cycle = data->cycle;
            if (ImGui::ColumnDragFloat("Cycle", &cycle, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
                data->cycle = cycle;

            ImGui::TableNextColumn();
            ImGui::GetContentRegionAvailSetNextItemWidth();

            float_t phase = data->phase;
            if (ImGui::ColumnDragFloat("Phase", &phase, 0.1f, -FLT_MAX, FLT_MAX, "%g"))
                data->phase = phase;

            input_locked |= ImGui::IsWindowFocused();
            ImGui::EndTable();
        }
    }
    ImGui::SetDefaultColumnSpace();
    ImGui::EndGroup();

    input_locked |= ImGui::IsWindowFocused();

    ImGui::End();
}

void rob_chara_adjust_init() {
    if (!rob_chara_adjust)
        rob_chara_adjust = new RobCharaAdjust;
}

void rob_chara_adjust_free() {
    if (rob_chara_adjust) {
        delete rob_chara_adjust;
        rob_chara_adjust = 0;
    }
}
