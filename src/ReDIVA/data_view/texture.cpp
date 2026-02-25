/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.hpp"
#include "../../CRE/render_context.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"

class DataViewTexture : public app::TaskWindow {
public:
    bool exit;

    DataViewTexture();
    virtual ~DataViewTexture() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

DataViewTexture data_view_texture;

void data_view_texture_init() {
    app::TaskWork::add_task(&data_view_texture, "DATA_VIEW_TEXTURE", 2);
}

DataViewTexture::DataViewTexture() : exit() {

}

DataViewTexture::~DataViewTexture() {

}

bool DataViewTexture::init() {
    exit = false;
    return true;
}

bool DataViewTexture::ctrl() {
    return exit;
}

bool DataViewTexture::dest() {
    return true;
}

void DataViewTexture::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t width;
    extern int32_t height;

    float_t w = min_def((float_t)width, 580.0f);
    float_t h = min_def((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Texture##Data Viewer", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    extern texture_manager* texture_manager_work_ptr;
    for (auto& i : texture_manager_work_ptr->textures) {
        texture* tex = &i.second;
        if (tex->id.id)
            continue;

        ImGui::PushID(tex->id.index);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Ref Count: %3d; Width: %5d; Height: %5d; Mipmap Count: %2d",
            tex->id.index, tex->ref_count, tex->width, tex->height, tex->max_mipmap_level + 1);
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    focus |= ImGui::IsWindowFocused();
    ImGui::End();
}
