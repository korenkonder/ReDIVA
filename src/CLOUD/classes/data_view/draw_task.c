/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.h"
#include "../../../CRE/render_context.h"
#include "../imgui_helper.h"

static const char* draw_object_type_name[] = {
    [DRAW_OBJECT_OPAQUE]                    = "Opaque",
    [DRAW_OBJECT_TRANSLUCENT]               = "Translucent",
    [DRAW_OBJECT_TRANSLUCENT_NO_SHADOW]     = "Translucent (No Shadow)",
    [DRAW_OBJECT_TRANSPARENT]               = "Transparent",
    [DRAW_OBJECT_SHADOW_CHARA]              = "Shadow Chara",
    [DRAW_OBJECT_SHADOW_STAGE]              = "Shadow Stage",
    [DRAW_OBJECT_TYPE_6]                    = "Type 6",
    [DRAW_OBJECT_TYPE_7]                    = "Type 7",
    [DRAW_OBJECT_SHADOW_OBJECT_CHARA]       = "Shadow Object Chara",
    [DRAW_OBJECT_SHADOW_OBJECT_STAGE]       = "Shadow Object Stage",
    [DRAW_OBJECT_REFLECT_CHARA_OPAQUE]      = "Reflect Chara Opaque",
    [DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT] = "Reflect Chara Translucent",
    [DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT] = "Reflect Chara Transparent",
    [DRAW_OBJECT_REFLECT_OPAQUE]            = "Reflect Opaque",
    [DRAW_OBJECT_REFLECT_TRANSLUCENT]       = "Reflect Translucent",
    [DRAW_OBJECT_REFLECT_TRANSPARENT]       = "Reflect Transparent",
    [DRAW_OBJECT_REFRACT_OPAQUE]            = "Refract Opaque",
    [DRAW_OBJECT_REFRACT_TRANSLUCENT]       = "Refract Translucent",
    [DRAW_OBJECT_REFRACT_TRANSPARENT]       = "Refract Transparent",
    [DRAW_OBJECT_SSS]                       = "SSS",
    [DRAW_OBJECT_OPAQUE_TYPE_20]            = "Opaque (Type 20)",
    [DRAW_OBJECT_TRANSPARENT_TYPE_21]       = "Transparent (Type 21)",
    [DRAW_OBJECT_TRANSLUCENT_TYPE_22]       = "Translucent (Type 22)",
    [DRAW_OBJECT_OPAQUE_TYPE_23]            = "Opaque (Type 23)",
    [DRAW_OBJECT_TRANSPARENT_TYPE_24]       = "Transparent (Type 24)",
    [DRAW_OBJECT_TRANSLUCENT_TYPE_25]       = "Translucent (Type 25)",
    [DRAW_OBJECT_OPAQUE_TYPE_26]            = "Opaque (Type 26)",
    [DRAW_OBJECT_TRANSPARENT_TYPE_27]       = "Transparent (Type 27)",
    [DRAW_OBJECT_TRANSLUCENT_TYPE_28]       = "Translucent (Type 28)",
    [DRAW_OBJECT_RIPPLE]                    = "Ripple",
};

static const char* draw_task_type_name[] = {
    [DRAW_TASK_TYPE_OBJECT]             = "Object",
    [DRAW_TASK_TYPE_PRIMITIVE]          = "Primitive",
    [DRAW_TASK_TYPE_PREPROCESS]         = "Preprocess",
    [DRAW_TASK_TYPE_OBJECT_TRANSLUCENT] = "Object Translucent",
};

typedef struct data_view_draw_task {
    render_context* rctx;
    vector_ptr_draw_task draw_tasks;
    vector_uint32_t draw_task_hashes;
    vector_uint32_t draw_task_indices;
} data_view_draw_task;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

const char* data_view_draw_task_window_title = "Draw Task##Data Viewer";

static void data_view_draw_task_imgui_draw_object(draw_object* object);

bool data_view_draw_task_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_view_draw_task));
    data_view_draw_task* data_view = data->data;
    if (data_view) {
        data_view->rctx = rctx;
        data_view->draw_tasks = vector_ptr_empty(draw_task);
        data_view->draw_task_hashes = vector_empty(uint32_t);
        data_view->draw_task_indices = vector_empty(uint32_t);
    }
    return true;
}

void data_view_draw_task_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 480.0f);
    float_t h = min((float_t)height, 542.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_view_draw_task_window_title, &open, 0);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    data_view_draw_task* data_view = data->data;
    if (!data_view)
        goto End;

    render_context* rctx = data_view->rctx;
    vector_ptr_draw_task* draw_task_array = rctx->object_data.draw_task_array;
    vector_ptr_draw_task* draw_tasks_sort = &data_view->draw_tasks;
    vector_uint32_t* draw_task_hashes = &data_view->draw_task_hashes;
    vector_uint32_t* draw_task_indices = &data_view->draw_task_indices;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    for (draw_object_type i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++) {
        bool translucent_no_shadow = i == DRAW_OBJECT_TRANSLUCENT_NO_SHADOW;
        tree_node_flags = tree_node_base_flags;

        igPushID_Int(i);
        vector_ptr_draw_task* draw_tasks = &draw_task_array[i];
        size_t count = vector_length(*draw_tasks);
        bool enable = count > 0;
        imguiDisableElementPush(enable);
        if (!igTreeNodeEx_StrStr("", tree_node_flags,
            "%s; Count: %llu", draw_object_type_name[i], count)) {
            imguiDisableElementPop(enable);
            igPopID();
            continue;
        }

        vector_ptr_draw_task_insert_range(draw_tasks_sort, 0, draw_tasks->begin, draw_tasks->end);
        vector_uint32_t_reserve(draw_task_hashes, count);
        vector_uint32_t_reserve(draw_task_indices, count);
        for (draw_task** j = draw_tasks_sort->begin; j != draw_tasks_sort->end; j++) {
            *vector_uint32_t_reserve_back(draw_task_hashes) = hash_murmurhash((void*)j, 8, 0, true, false);
            *vector_uint32_t_reserve_back(draw_task_indices) = (uint32_t)(j - draw_tasks_sort->begin);
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

        igText("       Hash;    Index; Type");

        for (draw_task** j = draw_tasks_sort->begin; j != draw_tasks_sort->end; j++) {
            size_t task_idx = j - draw_tasks_sort->begin;
            draw_task* task = *j;
            igPushID_Ptr(task);
            if (!igTreeNodeEx_StrStr("", tree_node_flags,
                "%08x; %8u; %s", draw_task_hashes->begin[task_idx],
                draw_task_indices->begin[task_idx], draw_task_type_name[task->type])) {
                igPopID();
                continue;
            }

            if (igTreeNodeEx_Str("Matrix", ImGuiTreeNodeFlags_DefaultOpen)) {
                mat4u* mat = &task->mat;
                igText("%9.4f %9.4f %9.4f %9.4f", mat->row0.x, mat->row1.x, mat->row2.x, mat->row3.x);
                igText("%9.4f %9.4f %9.4f %9.4f", mat->row0.y, mat->row1.y, mat->row2.y, mat->row3.y);
                igText("%9.4f %9.4f %9.4f %9.4f", mat->row0.z, mat->row1.z, mat->row2.z, mat->row3.z);
                igText("%9.4f %9.4f %9.4f %9.4f", mat->row0.w, mat->row1.w, mat->row2.w, mat->row3.w);
                igTreePop();
            }

            switch (task->type) {
            case DRAW_TASK_TYPE_OBJECT: {
                if (igTreeNodeEx_Str("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
                    data_view_draw_task_imgui_draw_object(&task->data.object);
                    igTreePop();
                }
            } break;
            case DRAW_TASK_TYPE_PRIMITIVE: {

            } break;
            case DRAW_TASK_TYPE_PREPROCESS: {

            } break;
            case DRAW_TASK_TYPE_OBJECT_TRANSLUCENT: {
                draw_task_object_translucent* object_translucent = &task->data.object_translucent;
                for (int32_t l = 0; l < 40 && l < object_translucent->count; l++) {
                    igPushID_Int(l);
                    if (igTreeNodeEx_StrStr("", ImGuiTreeNodeFlags_DefaultOpen, "Data %2d", l)) {
                        data_view_draw_task_imgui_draw_object(object_translucent->objects[l]);
                        igTreePop();
                    }
                    igPopID();
                }
            } break;
            }

            igTreePop();
            igPopID();
        }
        draw_tasks_sort->end = draw_tasks_sort->begin;
        draw_task_hashes->end = draw_task_hashes->begin;
        draw_task_indices->end = draw_task_indices->begin;

        igTreePop();
        imguiDisableElementPop(enable);
        igPopID();
    }

    data->imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
}

void data_view_draw_task_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

bool data_view_draw_task_dispose(class_data* data) {
    data_view_draw_task* data_view = data->data;
    if (data_view) {
        data_view->draw_tasks.end = data_view->draw_tasks.begin;
        vector_ptr_draw_task_free(&data_view->draw_tasks, 0);
        vector_uint32_t_free(&data_view->draw_task_hashes, 0);
        vector_uint32_t_free(&data_view->draw_task_indices, 0);
    }
    free(data->data);
    return true;
}

static void data_view_draw_task_imgui_draw_object(draw_object* object) {
    igText("Mesh Name: %s", object->mesh->name);
    igText("Sub Mesh Index: %lld", object->sub_mesh - object->mesh->sub_meshes);
}