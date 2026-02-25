/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/render_context.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"

class DataViewGlitter : public app::TaskWindow {
public:
    bool exit;

    DataViewGlitter();
    virtual ~DataViewGlitter() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

DataViewGlitter data_view_glitter;

void data_view_glitter_init() {
    app::TaskWork::add_task(&data_view_glitter, "DATA_VIEW_GLITTER", 2);
}

DataViewGlitter::DataViewGlitter() : exit() {

}

DataViewGlitter::~DataViewGlitter() {

}

bool DataViewGlitter::init() {
    exit = false;
    return true;
}

bool DataViewGlitter::ctrl() {
    return exit;
}

bool DataViewGlitter::dest() {
    return true;
}

void DataViewGlitter::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t width;
    extern int32_t height;

    float_t w = min_def((float_t)width, 600.0f);
    float_t h = min_def((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Glitter##Data Viewer", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx("Effect Groups", tree_node_flags)) {
        for (auto& i : Glitter::glt_particle_manager->effect_groups)
            ImGui::Text("0x%08X: %s", i.first, i.second->name.c_str());
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx("Scenes", tree_node_flags)) {
        int32_t sc_index = 0;
        for (Glitter::Scene*& i : Glitter::glt_particle_manager->scenes) {
            if (!i)
                continue;

            tree_node_flags = tree_node_base_flags;
            tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

            ImGui::PushID(sc_index);
            if (!ImGui::TreeNodeEx("##Scene", tree_node_flags,
                "Scene %d. 0x%08X: %s", sc_index, i->hash, i->name.c_str())) {
                ImGui::PopID();
                sc_index++;
                continue;
            }

            int32_t eff_index = 0;
            if (i->type != Glitter::X)
                for (Glitter::SceneEffect& j : i->effects) {
                    if (!j.disp || !j.ptr)
                        continue;

                    Glitter::EffectInstF2* eff_f2 = (Glitter::EffectInstF2*)j.ptr;
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_f2->flags & Glitter::EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID((void*)eff_f2->data.name_hash);
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_f2->data.name_hash, eff_f2->name.c_str(), eff_f2->frame0)) {
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (Glitter::EmitterInstF2*& k : eff_f2->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & Glitter::EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (Glitter::ParticleInstF2*& l : k->particles) {
                                if (!l)
                                    continue;

                                ImGui::PushStyleColor(ImGuiCol_Text,
                                    l->data.flags & Glitter::PARTICLE_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                                ImGui::PushID(ptcl_index);
                                ImGui::Text("Particle %d", ptcl_index);
                                ImGui::PopID();
                                ImGui::PopStyleColor();
                                ptcl_index++;
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        emit_index++;
                    }
                    ImGui::TreePop();
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                    eff_index++;

                }
            else
                for (Glitter::SceneEffect& j : i->effects) {
                    if (!j.disp || !j.ptr)
                        continue;

                    Glitter::EffectInstX* eff_x = (Glitter::EffectInstX*)j.ptr;
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_x->flags & Glitter::EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID((void*)eff_x->data.name_hash);
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_x->data.name_hash, eff_x->name.c_str(), eff_x->frame0)) {
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (Glitter::EmitterInstX*& k : eff_x->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & Glitter::EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (Glitter::ParticleInstX*& l : k->particles) {
                                if (!l)
                                    continue;

                                ImGui::PushStyleColor(ImGuiCol_Text,
                                    l->data.flags & Glitter::PARTICLE_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                                ImGui::PushID(ptcl_index);
                                ImGui::Text("Particle %d", ptcl_index);
                                ImGui::PopID();
                                ImGui::PopStyleColor();
                                ptcl_index++;
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        emit_index++;
                    }
                    ImGui::TreePop();
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                    eff_index++;
                }
            ImGui::TreePop();
            ImGui::PopID();
            sc_index++;
        }
        ImGui::TreePop();
    }

    focus |= ImGui::IsWindowFocused();
    ImGui::End();
}
