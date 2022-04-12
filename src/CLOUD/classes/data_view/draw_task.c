/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.h"
#include "../../../CRE/render_context.h"
#include "../../../KKdLib/hash.h"
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

typedef struct data_view_draw_task {
    render_context* rctx;
    vector_old_ptr_draw_task draw_tasks;
    vector_old_uint32_t draw_task_hashes;
    vector_old_uint32_t draw_task_indices;
} data_view_draw_task;

extern int32_t width;
extern int32_t height;

const char* data_view_draw_task_window_title = "Draw Task##Data Viewer";

static void data_view_draw_task_imgui_draw_object(draw_object* object);

bool data_view_draw_task_init(class_data* data, render_context* rctx) {
    data->data = force_malloc_s(data_view_draw_task, 1);
    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    if (data_view) {
        data_view->rctx = rctx;
        data_view->draw_tasks = vector_old_ptr_empty(draw_task);
        data_view->draw_task_hashes = vector_old_empty(uint32_t);
        data_view->draw_task_indices = vector_old_empty(uint32_t);
    }
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
    vector_old_ptr_draw_task* draw_task_array = rctx->object_data.draw_task_array;
    vector_old_ptr_draw_task* draw_tasks_sort = &data_view->draw_tasks;
    vector_old_uint32_t* draw_task_hashes = &data_view->draw_task_hashes;
    vector_old_uint32_t* draw_task_indices = &data_view->draw_task_indices;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    for (int32_t i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++) {
        bool translucent_no_shadow = i == DRAW_OBJECT_TRANSLUCENT_NO_SHADOW;
        tree_node_flags = tree_node_base_flags;

        ImGui::PushID(i);
        vector_old_ptr_draw_task* draw_tasks = &draw_task_array[i];
        size_t count = vector_old_length(*draw_tasks);
        bool enable = count > 0;
        imguiDisableElementPush(enable);
        if (!ImGui::TreeNodeEx("", tree_node_flags,
            "%s; Count: %llu", draw_object_type_name[i], count)) {
            imguiDisableElementPop(enable);
            ImGui::PopID();
            continue;
        }

        vector_old_ptr_draw_task_insert_range(draw_tasks_sort, 0, draw_tasks->begin, draw_tasks->end);
        vector_old_uint32_t_reserve(draw_task_hashes, count);
        vector_old_uint32_t_reserve(draw_task_indices, count);
        for (draw_task** j = draw_tasks_sort->begin; j != draw_tasks_sort->end; j++) {
            *vector_old_uint32_t_reserve_back(draw_task_hashes) = hash_murmurhash((void*)j, 8, 0, true, false);
            *vector_old_uint32_t_reserve_back(draw_task_indices) = (uint32_t)(j - draw_tasks_sort->begin);
        }

        for (uint32_t* i = draw_task_hashes->begin; i != &draw_task_hashes->end[-1]; i++)
            for (uint32_t* j = i + 1; j != draw_task_hashes->end; j++)
                if (*i > *j) {
                    size_t i_idx = i - draw_task_hashes->begin;
                    size_t j_idx = j - draw_task_hashes->begin;

                    draw_task* temp = draw_tasks_sort->begin[i_idx];
                    draw_tasks_sort->begin[i_idx] = draw_tasks_sort->begin[j_idx];
                    draw_tasks_sort->begin[j_idx] = temp;

                    uint32_t temp_hash = *i;
                    *i = *j;
                    *j = temp_hash;

                    uint32_t temp_index = draw_task_indices->begin[i_idx];
                    draw_task_indices->begin[i_idx] = draw_task_indices->begin[j_idx];
                    draw_task_indices->begin[j_idx] = temp_index;
                }

        ImGui::Text("       Hash;    Index; Type");

        for (draw_task** j = draw_tasks_sort->begin; j != draw_tasks_sort->end; j++) {
            size_t task_idx = j - draw_tasks_sort->begin;
            draw_task* task = *j;
            ImGui::PushID(task);
            if (!ImGui::TreeNodeEx("", tree_node_flags,
                "%08x; %8u; %s", draw_task_hashes->begin[task_idx],
                draw_task_indices->begin[task_idx], draw_task_type_name[task->type])) {
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
        draw_tasks_sort->end = draw_tasks_sort->begin;
        draw_task_hashes->end = draw_task_hashes->begin;
        draw_task_indices->end = draw_task_indices->begin;

        ImGui::TreePop();
        imguiDisableElementPop(enable);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_draw_task_dispose(class_data* data) {
    data_view_draw_task* data_view = (data_view_draw_task*)data->data;
    if (data_view) {
        data_view->draw_tasks.end = data_view->draw_tasks.begin;
        vector_old_ptr_draw_task_free(&data_view->draw_tasks, 0);
        vector_old_uint32_t_free(&data_view->draw_task_hashes, 0);
        vector_old_uint32_t_free(&data_view->draw_task_indices, 0);
    }
    free(data->data);
    return true;
}

static void data_view_draw_task_imgui_draw_object(draw_object* object) {
    ImGui::Text("Mesh Name: %s", object->mesh->name);
    ImGui::Text("Sub Mesh Index: %lld", object->sub_mesh - object->mesh->sub_meshes);
}