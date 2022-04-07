/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/render_context.h"
#include "../imgui_helper.h"

typedef struct data_view_glitter {
    render_context* rctx;
} data_view_glitter;

extern int32_t width;
extern int32_t height;

const char* data_view_glitter_window_title = "Glitter##Data Viewer";

bool data_view_glitter_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_view_glitter));
    data_view_glitter* data_view = (data_view_glitter*)data->data;
    if (data_view) {
        data_view->rctx = rctx;
    }
    return true;
}

void data_view_glitter_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 600.0f);
    float_t h = min((float_t)height, 480.0f);

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

    data_view_glitter* data_view = (data_view_glitter*)data->data;
    if (!data_view) {
        ImGui::End();
        return;
    }

    render_context* rctx = data_view->rctx;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx("Effect Groups", tree_node_flags)) {
        for (auto& i : GPM_VAL.effect_groups)
            ImGui::Text("0x%08X: %s", i.first, i.second->name.c_str());
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx("Scenes", tree_node_flags)) {
        int32_t sc_index = 0;
        for (GlitterScene*& i : GPM_VAL.scenes) {
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
            for (GlitterSceneEffect& j : i->effects) {
                if (!j.disp || !j.ptr)
                    continue;

                GlitterF2EffectInst* eff_f2 = dynamic_cast<GlitterF2EffectInst*>(j.ptr);
                if (eff_f2) {
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_f2->flags & GLITTER_EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID(eff_index);
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_f2->data.name_hash, eff_f2->name.c_str(), eff_f2->frame0)) {
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (GlitterF2EmitterInst*& k : eff_f2->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & GLITTER_EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (GlitterF2ParticleInst*& l : k->particles) {
                                if (!l)
                                    continue;

                                ImGui::PushStyleColor(ImGuiCol_Text,
                                    l->data.flags & GLITTER_PARTICLE_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
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
                    continue;
                }

                GlitterXEffectInst* eff_x = dynamic_cast<GlitterXEffectInst*>(j.ptr);
                if (eff_x) {
                    tree_node_flags = tree_node_base_flags;

                    ImGui::PushStyleColor(ImGuiCol_Text,
                        eff_x->flags & GLITTER_EFFECT_INST_FREE ? 0xFF888888 : 0xFFFFFFFF);
                    ImGui::PushID(eff_index);
                    if (!ImGui::TreeNodeEx("##Effect", tree_node_flags,
                        "Effect %d. 0x%08X: %s; Frame: %g", eff_index,
                        eff_x->data.name_hash, eff_x->name.c_str(), eff_x->frame0)) {
                        ImGui::PopStyleColor();
                        ImGui::PopID();
                        eff_index++;
                        continue;
                    }

                    int32_t emit_index = 0;
                    for (GlitterXEmitterInst*& k : eff_x->emitters) {
                        if (!k)
                            continue;

                        tree_node_flags = tree_node_base_flags;

                        ImGui::PushStyleColor(ImGuiCol_Text,
                            k->flags & GLITTER_EMITTER_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
                        ImGui::PushID(emit_index);
                        if (ImGui::TreeNodeEx("##Effect", tree_node_flags,
                            "Emitter %d; Frame: %g", emit_index, k->frame)) {
                            int32_t ptcl_index = 0;
                            for (GlitterXParticleInst*& l : k->particles) {
                                if (!l)
                                    continue;

                                ImGui::PushStyleColor(ImGuiCol_Text,
                                    l->data.flags & GLITTER_PARTICLE_INST_ENDED ? 0xFF888888 : 0xFFFFFFFF);
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
                    continue;
                }

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
    data_view_glitter* data_view = (data_view_glitter*)data->data;
    if (data_view) {
    }
    free(data->data);
    return true;
}
