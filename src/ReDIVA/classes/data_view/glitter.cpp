/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../../CRE/Glitter/glitter.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;

const char* data_view_glitter_window_title = "Glitter##Data Viewer";

bool data_view_glitter_init(class_data* data, render_context* rctx) {
    data->data = rctx;
    return true;
}

void data_view_glitter_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 600.0f);
    float_t h = min_def((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_view_glitter_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    render_context* rctx = (render_context*)data->data;
    if (!rctx) {
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
#if defined(CRE_DEV)
        for (auto& i : Glitter::glt_particle_manager->effect_groups)
            ImGui::Text("0x%08X: %s", i.first, i.second->name.c_str());
#else
        for (auto& i : Glitter::glt_particle_manager->effect_groups)
            ImGui::Text("0x%08X", i.first);
#endif
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
#if defined(CRE_DEV)
            if (!ImGui::TreeNodeEx("##Scene", tree_node_flags,
                "Scene %d. 0x%08X: %s", sc_index, i->hash, i->name.c_str())) {
#else
            if (!ImGui::TreeNodeEx("##Scene", tree_node_flags,
                "Scene %d. 0x%08X", sc_index, i->hash)) {
#endif
                ImGui::PopID();
                sc_index++;
                continue;
            }

            int32_t eff_index = 0;
            if (i->type != Glitter::X)
                for (Glitter::SceneEffect& j : i->effects) {
                    if (!j.disp || !j.ptr)
                        continue;

                    Glitter::F2EffectInst* eff_f2 = (Glitter::F2EffectInst*)j.ptr;
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_f2->flags & Glitter::EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID(eff_index);
#if defined(CRE_DEV)
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_f2->data.name_hash, eff_f2->name.c_str(), eff_f2->frame0)) {
#else
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X; Frame: %g", eff_index,
                        eff_f2->data.name_hash, eff_f2->frame0)) {
#endif
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (Glitter::F2EmitterInst*& k : eff_f2->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & Glitter::EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (Glitter::F2ParticleInst*& l : k->particles) {
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

                    Glitter::XEffectInst* eff_x = (Glitter::XEffectInst*)j.ptr;
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_x->flags & Glitter::EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID(eff_index);
#if defined(CRE_DEV)
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_x->data.name_hash, eff_x->name.c_str(), eff_x->frame0)) {
#else
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X; Frame: %g", eff_index,
                        eff_x->data.name_hash, eff_x->frame0)) {
#endif
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (Glitter::XEmitterInst*& k : eff_x->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & Glitter::EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (Glitter::XParticleInst*& l : k->particles) {
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

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_glitter_dispose(class_data* data) {
    data->data = 0;
    return true;
}
