/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../../CRE/object.hpp"
#include "../../CRE/render_context.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"
#include <map>

class DataViewObject : public app::TaskWindow {
public:
    bool exit;

    DataViewObject();
    virtual ~DataViewObject() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

DataViewObject data_view_object;

void data_view_object_init() {
    app::TaskWork::add_task(&data_view_object, "DATA_VIEW_AUTH_3D", 2);
}

DataViewObject::DataViewObject() : exit() {

}

DataViewObject::~DataViewObject() {

}

bool DataViewObject::init() {
    exit = false;
    return true;
}

bool DataViewObject::ctrl() {
    return exit;
}

bool DataViewObject::dest() {
    return true;
}

void DataViewObject::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t width;
    extern int32_t height;

    float_t w = min_def((float_t)width, 420.0f);
    float_t h = min_def((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Object##Data Viewer", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    extern std::map<uint32_t, ObjsetInfo> objset_info_storage_data;
    for (auto& i : objset_info_storage_data) {
        ObjsetInfo* info = &i.second;
        if (!info->obj_set)
            continue;

        ImGui::PushID(info->set_id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            info->set_id, info->load_count, info->name.c_str());
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    extern std::map<uint32_t, ObjsetInfo> objset_info_storage_data_modern;
    if (objset_info_storage_data_modern.size())
        ImGui::Selectable("Modern:", false, ImGuiSelectableFlags_Disabled);

    for (auto& i : objset_info_storage_data_modern) {
        ObjsetInfo* info = &i.second;
        if (!info->obj_set)
            continue;

        ImGui::PushID(info->set_id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            info->set_id, info->load_count, info->name.c_str());
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    focus |= ImGui::IsWindowFocused();
    ImGui::End();
}
