/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.h"
#include "../../../CRE/render_context.hpp"
#include "../../../KKdLib/hash.hpp"
#include "../imgui_helper.h"

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
    "Opaque (Type 20)",
    "Transparent (Type 21)",
    "Translucent (Type 22)",
    "Opaque (Type 23)",
    "Transparent (Type 24)",
    "Translucent (Type 25)",
    "Opaque (Type 26)",
    "Transparent (Type 27)",
    "Translucent (Type 28)",
    "Ripple",
};

static const char* draw_task_type_name[] = {
    "Object",
    "Primitive",
    "Preprocess",
    "Object Translucent",
};

typedef std::pair<draw_task*, std::pair<uint32_t, uint32_t>> draw_task_sort;

struct data_view_draw_task {
    render_context* rctx;
    std::vector<std::pair<draw_task*, std::pair<uint32_t, uint32_t>>> draw_tasks;

    data_view_draw_task();
    virtual ~data_view_draw_task();
};

extern int32_t width;
extern int32_t height;

const char* data_view_draw_task_window_title = "Draw Task##Data Viewer";

static void data_view_draw_task_imgui_draw_object(draw_object* object);

bool data_view_draw_task_init(class_data* data, render_context* rctx) {
    data->data = new data_view_draw_task;
    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    if (data_view)
        data_view->rctx = rctx;
    return true;
}

void data_view_draw_task_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 480.0f);
    float_t h = min((float_t)height, 542.0f);

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
    std::vector<std::pair<draw_task*, std::pair<uint32_t, uint32_t>>>& draw_tasks_sort = data_view->draw_tasks;

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
        imguiDisableElementPush(enable);
        if (!ImGui::TreeNodeEx("", tree_node_flags,
            "%s; Count: %llu", draw_object_type_name[i], count)) {
            imguiDisableElementPop(enable);
            ImGui::PopID();
            continue;
        }

        draw_tasks_sort.reserve(count);
        for (draw_task*& j : draw_tasks) {
            draw_task_sort draw_task;
            draw_task.first = j;
            draw_task.second.first = hash_murmurhash((void*)j, 8, 0, true);
            draw_task.second.second = (uint32_t)(&j - draw_tasks.data());
            draw_tasks_sort.push_back(draw_task);
        }

        for (auto i = draw_tasks_sort.begin(); i != draw_tasks_sort.end() - 1; i++)
            for (auto j = i + 1; j != draw_tasks_sort.end(); j++)
                if (*i > *j) {
                    draw_task_sort temp = *i;
                    *i = *j;
                    *j = temp;
                }

        ImGui::Text("       Hash;    Index; Type");

        for (draw_task_sort& j : draw_tasks_sort) {
            draw_task* task = j.first;
            ImGui::PushID(task);
            if (!ImGui::TreeNodeEx("", tree_node_flags, "%08x; %8u; %s",
                j.second.first, j.second.second, draw_task_type_name[task->type])) {
                ImGui::PopID();
                continue;
            }

            mat4 mat = task->mat;

            vec3 trans;
            mat4_get_translation(&mat, &trans);

            vec3 rot;
            mat4_get_rotation(&mat, &rot);
            vec3_mult_scalar(rot, RAD_TO_DEG_FLOAT, rot);

            vec3 scale;
            mat4_get_scale(&mat, &scale);

            ImGui::Text("Trans: %9.4f %9.4f %9.4f", trans.x, trans.y, trans.z);
            ImGui::Text("  Rot: %9.4f %9.4f %9.4f", rot.x, rot.y, rot.z);
            ImGui::Text("Scale: %9.4f %9.4f %9.4f", scale.x, scale.y, scale.z);

            switch (task->type) {
            case DRAW_TASK_TYPE_OBJECT: {
                if (ImGui::TreeNodeEx("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
                    data_view_draw_task_imgui_draw_object(&task->data.object);
                    ImGui::TreePop();
                }
            } break;
            case DRAW_TASK_TYPE_PRIMITIVE: {

            } break;
            case DRAW_TASK_TYPE_PREPROCESS: {

            } break;
            case DRAW_TASK_TYPE_OBJECT_TRANSLUCENT: {
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
        imguiDisableElementPop(enable);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_draw_task_dispose(class_data* data) {
    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    delete data_view;
    data->data = 0;
    return true;
}

data_view_draw_task::data_view_draw_task() : rctx() {

}

data_view_draw_task::~data_view_draw_task() {

}

static void data_view_draw_task_imgui_draw_object(draw_object* object) {
    ImGui::Text("Mesh Name: %s", object->mesh->name);
    ImGui::Text("Sub Mesh Index: %lld", object->sub_mesh - object->mesh->sub_meshes);
}