/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.h"
#include "../../../KKdLib/io/path.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../KKdLib/vector.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/effect_group.h"
#include "../../../CRE/Glitter/file_reader.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/Glitter/scene.h"
#include "../../../CRE/data.h"
#include "../../../CRE/render_context.h"
#include "../../input.h"
#include "../data_test.h"
#include "../imgui_helper.h"

typedef struct data_test_glitter_test_struct {
    bool input_play;
    bool input_stop;
    bool input_auto;
    bool input_pause;
    int32_t frame_counter;
    double_t delta_frame;
    char* file;
    vector_data_struct_file files;

    bool stage_test;
    render_context* rctx;
} data_test_glitter_test_struct;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern bool input_locked;
extern bool draw_grid_3d;

static const char* data_test_glitter_test_window_title = "Glitter Test##Data Test";

static data_test_glitter_test_struct data_test_glitter_test;

bool data_test_glitter_test_init(class_data* data, render_context* rctx) {
    if (!lock_data_init(&glitter_data_lock, &data->lock, data, data_test_glitter_test_dispose))
        return false;

    lock_trylock(&pv_lock);
    data->data = force_malloc(sizeof(data_test_glitter_test_struct));

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    GPM_VAL->counter = (uint32_t)(time.LowPart * 0x0CAD3078ULL);

    data_test_glitter_test_struct* glt_test = data->data;
    if (glt_test) {
        data_struct_get_directory_files(&data_list[DATA_AFT], "rom/particle/", &glt_test->files);
        for (data_struct_file* i = glt_test->files.begin; i != glt_test->files.end;)
            if (str_utils_check_ends_with(string_data(&i->name), ".farc")) {
                char* temp = str_utils_get_without_extension(string_data(&i->name));
                string_free(&i->name);
                string_init(&i->name, temp);
                free(temp);
                i++;
            }
            else
                vector_data_struct_file_erase(&glt_test->files,
                    i - glt_test->files.begin, data_struct_file_free);

        glt_test->file = vector_length(glt_test->files) > 0
            ? string_data(&glt_test->files.begin[0].name) : 0;
        glt_test->stage_test = false;
        glt_test->rctx = rctx;
        GPM_VAL->emission = 1.0f;
        GPM_VAL->draw_all = false;
        GPM_VAL->draw_all_mesh = false;
        draw_grid_3d = false;
    }
    lock_unlock(&pv_lock);
    return true;
}

void data_test_glitter_test_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 320.0f);
    float_t h = min((float_t)height, 256.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_test_glitter_test_window_title, &open, window_flags);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    data_test_glitter_test_struct* glt_test = data->data;
    if (!glt_test)
        goto End;

    int32_t file_index = -1;
    if (glt_test->file)
        for (data_struct_file* i = glt_test->files.begin; i != glt_test->files.end; i++)
            if (!str_utils_compare(string_data(&i->name), glt_test->file)) {
                file_index = (int32_t)(i - glt_test->files.begin);
                break;
            }

    int32_t file_index_old = file_index;
    imguiColumnComboBoxConfigFile("File", glt_test->files.begin,
        vector_length(glt_test->files), &file_index, 0, false, &data->imgui_focus);

    if (file_index != file_index_old) {
        glt_test->file = string_data(&glt_test->files.begin[file_index].name);
        glt_test->input_stop = true;
    }

    if (imguiButton("Reset Camera (R)", ImVec2_Empty))
        input_reset = true;

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Play (F)", (ImVec2) { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_F, true))
        glt_test->input_play = true;

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Stop (V)", (ImVec2) { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_V, true))
        glt_test->input_stop = true;

    imguiCheckbox("Auto (T)", &glt_test->input_auto);
    if (igIsKeyPressed(GLFW_KEY_T, true))
        glt_test->input_auto ^= true;

    imguiCheckbox("Pause (G)", &glt_test->input_pause);
    if (igIsKeyPressed(GLFW_KEY_G, true))
        glt_test->input_pause ^= true;

    igSeparator();

    imguiColumnSliderFloat("Emission", &GPM_VAL->emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    igSeparator();

    imguiCheckbox("Show Grid", &draw_grid_3d);

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Stage", (ImVec2) { w, 0.0f }))
        glt_test->stage_test = true;

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();

    float_t win_x = min((float_t)width, 240.0f);
    float_t win_y = min((float_t)height, 96.0f);

    float_t x = 0.0f;
    float_t y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMouseInputs;
    window_flags |= ImGuiWindowFlags_NoNavInputs;
    window_flags |= ImGuiWindowFlags_NoNavFocus;

    igPushStyleColor_U32(ImGuiCol_Border, 0);
    igPushStyleColor_U32(ImGuiCol_WindowBg, 0);
    if (igBegin("Glitter Test Sub##Data Test", 0, window_flags)) {
        size_t ctrl;
        size_t disp;
        float_t frame;
        int32_t life_time;

        frame = 0.0f;
        life_time = 0;
        glitter_particle_manager_get_frame(GPM_VAL, &frame, &life_time);
        igText("%d - %.0f/%d", glt_test->frame_counter, frame, life_time);

        ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
        disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
        igText(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
        disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
        igText("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LINE);
        disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LINE);
        igText(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_MESH);
        disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_MESH);
        igText(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
    }
    igPopStyleColor(2);

End:
    igEnd();
}

void data_test_glitter_test_input(class_data* data) {
    data_test_glitter_test_struct* glt_test = data->data;
    if (!glt_test)
        return;

    input_locked |= data->imgui_focus;
}

void data_test_glitter_test_render(class_data* data) {
    data_test_glitter_test_struct* glt_test = data->data;
    if (!glt_test)
        return;

    if (!glt_test->input_pause) {
    }
    if (!glt_test->input_pause) {
        if (GPM_VAL->scenes.end != GPM_VAL->scenes.begin)
            glt_test->delta_frame += get_delta_frame();

        int32_t delta_frame = (int32_t)glt_test->delta_frame;
        glt_test->frame_counter += delta_frame;
        glt_test->delta_frame -= (double_t)delta_frame;

        glitter_particle_manager_update_scene(GPM_VAL, (float_t)delta_frame);
    }

    if (glt_test->file) {
        if (glt_test->input_play || glt_test->input_stop) {
            glitter_particle_manager_free_scenes(GPM_VAL);
            glt_test->frame_counter = 0;
        }

        uint64_t hash = hash_utf8_fnv1a64m(glt_test->file, false);
        if (glt_test->input_auto && !glitter_particle_manager_check_scene(GPM_VAL, hash)) {
            if (glitter_particle_manager_check_effect_group(GPM_VAL, hash))
                goto load;
            else
                goto reset;
        }
        else if (glt_test->input_play && !glitter_particle_manager_load_scene(GPM_VAL, hash)) {
        reset:
            glitter_particle_manager_free_scenes(GPM_VAL);
            glitter_particle_manager_free_effect_groups(GPM_VAL);

            GPM_VAL->data = &data_list[DATA_AFT];
            glitter_file_reader* fr = glitter_file_reader_init(GLITTER_FT, 0, glt_test->file, -1.0f);
            vector_ptr_glitter_file_reader_push_back(&GPM_VAL->file_readers, &fr);
            glitter_particle_manager_update_file_reader(GPM_VAL);
        load:
            glt_test->frame_counter = 0;
            glitter_particle_manager_load_scene(GPM_VAL, hash);
        }
        glt_test->input_play = false;
        glt_test->input_stop = false;
    }

    if (glt_test->stage_test) {
        classes_struct* c = &data_test_classes[DATA_TEST_STAGE_TEST];
        if (~c->data.flags & CLASS_INIT) {
            lock_init(&c->data.lock);
            if (lock_check_init(&c->data.lock) && c->init) {
                lock_lock(&c->data.lock);
                c->init(&c->data, glt_test->rctx);
                lock_unlock(&c->data.lock);
            }
        }

        if (lock_check_init(&c->data.lock)) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show))
                c->data.flags &= ~(CLASS_HIDE | CLASS_HIDDEN);
            lock_unlock(&c->data.lock);
        }
        glt_test->stage_test = false;
    }
}

bool data_test_glitter_test_dispose(class_data* data) {
    vector_ptr_glitter_scene_free(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_effect_group_free(&GPM_VAL->effect_groups, glitter_effect_group_dispose);

    lock_data_free(&glitter_data_lock, data_test_glitter_test_dispose);

    data_test_glitter_test_struct* glt_test = data->data;

    draw_grid_3d = false;
    if (glt_test) {
        vector_data_struct_file_free(&glt_test->files, data_struct_file_free);
        glt_test->file = 0;
        glt_test->input_play = false;
        glt_test->input_stop = false;
        glt_test->input_auto = false;
        glt_test->input_pause = false;
        glt_test->frame_counter = 0;
    }
    free(data->data);

    data->flags = CLASS_HIDDEN | CLASS_DISPOSED;
    data->imgui_focus = false;
    return true;
}
