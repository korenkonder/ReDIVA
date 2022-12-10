/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../../KKdLib/hash.hpp"
#include "../../../KKdLib/sort.hpp"
#include "../../imgui_helper.hpp"

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

static const char* draw_task_type_name[] = {
    "Object",
    "Primitive",
    "Preprocess",
    "Object Translucent",
};

typedef std::pair<draw_task*, uint32_t> draw_task_sort;

struct data_view_draw_task {
    render_context* rctx;
    std::vector<draw_task_sort> draw_tasks;

    data_view_draw_task();
    ~data_view_draw_task();
};

extern int32_t width;
extern int32_t height;

const char* data_view_draw_task_window_title = "Draw Task##Data Viewer";

static void data_view_draw_task_imgui_draw_object(draw_object* object);

bool data_view_draw_task_init(class_data* data, render_context* rctx) {
    data_view_draw_task* data_view = new data_view_draw_task;
    if (data_view)
        data_view->rctx = rctx;
    data->data = data_view;
    return true;
}

void data_view_draw_task_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 480.0f);
    float_t h = min_def((float_t)height, 542.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_view_draw_task_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    if (!data_view) {
        ImGui::End();
        return;
    }

    render_context* rctx = data_view->rctx;
    std::vector<draw_task*>* draw_task_array = rctx->object_data.draw_task_array;
    std::vector<draw_task_sort>& draw_tasks_sort = data_view->draw_tasks;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    for (int32_t i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++) {
        bool translucent_no_shadow = i == DRAW_OBJECT_TRANSLUCENT_NO_SHADOW;
        tree_node_flags = tree_node_base_flags;

        ImGui::PushID(i);
        std::vector<draw_task*>& draw_tasks = draw_task_array[i];
        size_t count = draw_tasks.size();
        bool enable = count > 0;
        ImGui::DisableElementPush(enable);
        if (!ImGui::TreeNodeEx("", tree_node_flags,
            "%s; Count: %llu", draw_object_type_name[i], count)) {
            ImGui::DisableElementPop(enable);
            ImGui::PopID();
            continue;
        }

        draw_tasks_sort.reserve(count);
        for (draw_task*& j : draw_tasks) {
            draw_task_sort draw_task;
            draw_task.first = j;
            draw_task.second = hash_murmurhash(&j, 8, 0, true);
            draw_tasks_sort.push_back(draw_task);
        }

        ImGui::Text("       Hash; Type");

        for (draw_task_sort& j : draw_tasks_sort) {
            draw_task* task = j.first;
            if (task->type < DRAW_TASK_OBJECT || task->type > DRAW_TASK_OBJECT_TRANSLUCENT)
                continue;

            ImGui::PushID(task);
            if (!ImGui::TreeNodeEx("", tree_node_flags, "%08x; %s",
                j.second, draw_task_type_name[task->type])) {
                ImGui::PopID();
                continue;
            }

            vec3 trans;
            mat4_get_translation(&task->mat, &trans);

            vec3 rot;
            mat4_get_rotation(&task->mat, &rot);
            rot *= RAD_TO_DEG_FLOAT;

            vec3 scale;
            mat4_get_scale(&task->mat, &scale);

            ImGui::Text("Trans: %9.4f %9.4f %9.4f", trans.x, trans.y, trans.z);
            ImGui::Text("  Rot: %9.4f %9.4f %9.4f", rot.x, rot.y, rot.z);
            ImGui::Text("Scale: %9.4f %9.4f %9.4f", scale.x, scale.y, scale.z);

            switch (task->type) {
            case DRAW_TASK_OBJECT: {
                if (ImGui::TreeNodeEx("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
                    data_view_draw_task_imgui_draw_object(&task->data.object);
                    ImGui::TreePop();
                }
            } break;
            case DRAW_TASK_OBJECT_PRIMITIVE: {

            } break;
            case DRAW_TASK_OBJECT_PREPROCESS: {

            } break;
            case DRAW_TASK_OBJECT_TRANSLUCENT: {
                draw_task_object_translucent* object_translucent = &task->data.object_translucent;
                for (int32_t l = 0; l < 40 && l < object_translucent->count; l++) {
                    ImGui::PushID(l);
                    if (ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_DefaultOpen, "Data %2d", l)) {
                        data_view_draw_task_imgui_draw_object(object_translucent->objects[l]);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
            } break;
            }

            ImGui::TreePop();
            ImGui::PopID();
        }
        draw_tasks_sort.clear();

        ImGui::TreePop();
        ImGui::DisableElementPop(enable);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_draw_task_dispose(class_data* data) {
    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    if (data_view) {
        delete data_view;
        data->data = 0;
    }
    return true;
}

data_view_draw_task::data_view_draw_task() : rctx() {

}

data_view_draw_task::~data_view_draw_task() {

}

static void data_view_draw_task_imgui_draw_object(draw_object* object) {
    ImGui::Text("Mesh Name: %s", object->mesh->name);
    ImGui::Text("Sub Mesh Index: %lld", object->sub_mesh - object->mesh->submesh_array);
}