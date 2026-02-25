/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.hpp"
#include "../../CRE/render_context.hpp"
#include "../../KKdLib/hash.hpp"
#include "../../KKdLib/sort.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"

class DataViewDrawTask : public app::TaskWindow {
public:
    bool exit;

    DataViewDrawTask();
    virtual ~DataViewDrawTask() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

static const char* draw_object_type_name[] = {
    "Opaque",
    "Translucent",
    "Translucent (No Shadow)",
    "Transparent",
    "Shadow Chara",
    "Shadow Stage",
    "Type 6",
    "Type 7",
    "Shadow Object Chara",
    "Shadow Object Stage",
    "Reflect Chara Opaque",
    "Reflect Chara Translucent",
    "Reflect Chara Transparent",
    "Reflect Opaque",
    "Reflect Translucent",
    "Reflect Transparent",
    "Refract Opaque",
    "Refract Translucent",
    "Refract Transparent",
    "SSS",
    "Opaque (Alpha Order 1)",
    "Transparent (Alpha Order 1)",
    "Translucent (Alpha Order 1)",
    "Opaque (Alpha Order 2)",
    "Transparent (Alpha Order 2)",
    "Translucent (Alpha Order 2)",
    "Opaque (Alpha Order 3)",
    "Transparent (Alpha Order 3)",
    "Translucent (Alpha Order 3)",
    "Preprocess",
    "[Local] Opaque",
    "[Local] Translucent",
    "[Local] Transparent",
    "[Local] Opaque (Alpha Order 2)",
    "[Local] Transparent (Alpha Order 2)",
    "[Local] Translucent (Alpha Order 2)",
};

static const char* mdl_obj_kind_name[] = {
    "Normal",
    "Etc",
    "Preprocess",
    "User",
};

DataViewDrawTask data_view_draw_task;

static void data_view_draw_task_window_sub_mesh_args(mdl::ObjSubMeshArgs* args);

void data_view_draw_task_init() {
    app::TaskWork::add_task(&data_view_draw_task, "DATA_VIEW_DRAW_TASK", 2);
}

DataViewDrawTask::DataViewDrawTask() : exit() {

}

DataViewDrawTask::~DataViewDrawTask() {

}

bool DataViewDrawTask::init() {
    exit = false;
    return true;
}

bool DataViewDrawTask::ctrl() {
    return exit;
}

bool DataViewDrawTask::dest() {
    return true;
}

void DataViewDrawTask::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = min_def((float_t)width, 480.0f);
    float_t h = min_def((float_t)height, 542.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    focus = false;
    bool open = false;
    if (!ImGui::Begin("Draw Task##Data Viewer", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    extern render_context* rctx_ptr;
    mdl::ObjList* obj = rctx_ptr->disp_manager->obj;
    prj::vector_pair<mdl::ObjData*, uint32_t> obj_data;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    for (int32_t i = mdl::OBJ_TYPE_OPAQUE; i < mdl::OBJ_TYPE_MAX; i++) {
        tree_node_flags = tree_node_base_flags;

        ImGui::PushID(i);
        mdl::ObjList& draw_tasks = obj[i];
        size_t count = draw_tasks.size();
        bool enable = count > 0;
        ImGui::DisableElementPush(enable);
        if (!ImGui::TreeNodeEx("", tree_node_flags,
            "%s; Count: %llu", draw_object_type_name[i], count)) {
            ImGui::DisableElementPop(enable);
            ImGui::PopID();
            continue;
        }

        obj_data.reserve(count);
        for (mdl::ObjData*& j : draw_tasks)
            obj_data.push_back(j, hash_murmurhash(&j, 8, 0, true));

        ImGui::Text("       Hash; Type");

        for (auto& j : obj_data) {
            mdl::ObjData* data = j.first;
            if (data->kind < mdl::OBJ_KIND_NORMAL || data->kind > mdl::OBJ_KIND_TRANSLUCENT)
                continue;

            ImGui::PushID(data);
            if (!ImGui::TreeNodeEx("", tree_node_flags, "%08x; %s",
                j.second, mdl_obj_kind_name[data->kind])) {
                ImGui::PopID();
                continue;
            }

            mat4& mat = data->mat;

            vec3 trans;
            vec3 rot;
            vec3 scale;
            mat4_get_translation(&mat, &trans);
            mat4_get_rotation(&mat, &rot);
            mat4_get_scale(&mat, &scale);
            rot *= RAD_TO_DEG_FLOAT;

            ImGui::Text("Trans: %9.4f %9.4f %9.4f", trans.x, trans.y, trans.z);
            ImGui::Text("  Rot: %9.4f %9.4f %9.4f", rot.x, rot.y, rot.z);
            ImGui::Text("Scale: %9.4f %9.4f %9.4f", scale.x, scale.y, scale.z);

            switch (data->kind) {
            case mdl::OBJ_KIND_NORMAL: {
                if (ImGui::TreeNodeEx("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
                    data_view_draw_task_window_sub_mesh_args(&data->args.sub_mesh);
                    ImGui::TreePop();
                }
            } break;
            case mdl::OBJ_KIND_ETC: {

            } break;
            case mdl::OBJ_KIND_USER: {

            } break;
            case mdl::OBJ_KIND_TRANSLUCENT: {
                mdl::ObjTranslucentArgs* translucent = &data->args.translucent;
                for (int32_t l = 0; l < 40 && l < translucent->count; l++) {
                    ImGui::PushID(l);
                    if (ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_DefaultOpen, "Data %2d", l)) {
                        data_view_draw_task_window_sub_mesh_args(translucent->sub_mesh[l]);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
            } break;
            }

            ImGui::TreePop();
            ImGui::PopID();
        }
        obj_data.clear();

        ImGui::TreePop();
        ImGui::DisableElementPop(enable);
        ImGui::PopID();
    }

    focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

static void data_view_draw_task_window_sub_mesh_args(mdl::ObjSubMeshArgs* args) {
    ImGui::Text("Mesh Name: %s", args->mesh->name);
    ImGui::Text("Sub Mesh Index: %lld", args->sub_mesh - args->mesh->submesh_array);
}