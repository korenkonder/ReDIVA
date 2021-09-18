/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.h"
#include "../../../KKdLib/io_path.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/effect_group.h"
#include "../../../CRE/Glitter/file_reader.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/Glitter/scene.h"
#include "../../input.h"
#include "../imgui_helper.h"

typedef struct glitter_test_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
    bool input_play;
    bool input_stop;
    bool input_auto;
    bool input_pause;
    float_t frame_counter;
    char* file;
    vector_string files;
    lock lock;
} glitter_test_struct;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern bool input_locked;
extern float_t frame_speed;
extern bool grid_3d;

extern GPM;

const char* glitter_test_window_title = "Glitter";

#if defined(CLOUD_DEV)
bool glitter_test_enabled = false;
#else
bool glitter_test_enabled = true;
#endif
static glitter_test_struct glitter_test;

void glitter_test_dispose() {
    lock_lock(&glitter_test.lock);
    vector_ptr_glitter_scene_free(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_effect_group_free(&GPM_VAL->effect_groups, glitter_effect_group_dispose);

    vector_string_free(&glitter_test.files);
    glitter_test_enabled = false;
    glitter_test.file = 0;
    glitter_test.imgui_focus = false;
    glitter_test.input_play = false;
    glitter_test.input_stop = false;
    glitter_test.input_auto = false;
    glitter_test.input_pause = false;
    glitter_test.frame_counter = 0.0f;
    glitter_test.dispose = false;
    glitter_test.disposed = true;
    lock_unlock(&glitter_test.lock);
    lock_free(&glitter_test.lock);
}

void glitter_test_init() {
    bool dispose = false;
    lock_lock(&glitter_test.lock);
    dispose = glitter_test_enabled || glitter_test.dispose;
    lock_unlock(&glitter_test.lock);

    if (dispose)
        glitter_test_dispose();

    lock_init(&glitter_test.lock);
    lock_lock(&glitter_test.lock);
    vector_string_free(&glitter_test.files);
    path_get_files(&glitter_test.files, "rom\\particle");
    for (string* i = glitter_test.files.begin; i != glitter_test.files.end;)
        if (str_utils_check_ends_with(string_data(i), ".farc")) {
            string s;
            char* temp = str_utils_get_without_extension(string_data(i));
            string_init(&s, temp);
            string_free(i);
            vector_string_erase(&glitter_test.files, i - glitter_test.files.begin);
            vector_string_insert(&glitter_test.files, i - glitter_test.files.begin, &s);
            free(temp);
            i++;
        }
        else {
            string_free(i);
            vector_string_erase(&glitter_test.files, i - glitter_test.files.begin);
        }

    glitter_test.file = glitter_test.files.end - glitter_test.files.begin > 0
        ? string_data(&glitter_test.files.begin[0]) : 0;

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    GPM_VAL->counter = (uint32_t)(time.LowPart * 0x0CAD3078LL);
    glitter_test_enabled = true;
    GPM_VAL->emission = 1.0f;
    GPM_VAL->draw_all = false;
    GPM_VAL->draw_all_mesh = false;
    grid_3d = false;
    lock_unlock(&glitter_test.lock);
}

void glitter_test_imgui() {
    bool ret = false;
    lock_lock(&glitter_test.lock);
    if (!glitter_test_enabled) {
        if (!glitter_test.disposed)
            glitter_test.dispose = true;
        ret = true;
    }
    else if (glitter_test.disposed)
        glitter_test.disposed = false;
    lock_unlock(&glitter_test.lock);

    if (ret)
        return;

    lock_lock(&glitter_test.lock);
    float_t w = min((float_t)width / 4.0f, 360.0f);
    float_t h = min((float_t)height, 348.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    glitter_test.imgui_focus = false;
    if (!igBegin(glitter_test_window_title, &glitter_test_enabled, window_flags)) {
        glitter_test.dispose = true;
        goto End;
    }

    int32_t file_index = -1;
    if (glitter_test.file)
        for (string* i = glitter_test.files.begin; i != glitter_test.files.end; i++)
            if (!strcmp(string_data(i), glitter_test.file)) {
                file_index = (int32_t)(i - glitter_test.files.begin);
                break;
            }

    int32_t file_index_old = file_index;
    imguiColumnComboBoxString("File", glitter_test.files.begin,
        glitter_test.files.end - glitter_test.files.begin,
        &file_index, 0, false, &glitter_test.imgui_focus);

    if (file_index != file_index_old) {
        glitter_test.file = string_data(&glitter_test.files.begin[file_index]);
        glitter_test.input_stop = true;
    }

    if (imguiButton("Reset Camera (R)", ImVec2_Empty))
        input_reset = true;

    if (imguiButton("Play (F)", ImVec2_Empty) || igIsKeyPressed(GLFW_KEY_F, true))
        glitter_test.input_play = true;

    if (imguiButton("Stop (V)", ImVec2_Empty) || igIsKeyPressed(GLFW_KEY_V, true))
        glitter_test.input_stop = true;

    imguiCheckbox("Auto (T)", &glitter_test.input_auto);
    if (igIsKeyPressed(GLFW_KEY_T, true))
        glitter_test.input_auto ^= true;

    imguiCheckbox("Pause (G)", &glitter_test.input_pause);
    if (igIsKeyPressed(GLFW_KEY_G, true))
        glitter_test.input_pause ^= true;

    igSeparator();

    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    frame = 0.0f;
    life_time = 0;
    glitter_particle_manager_get_frame(GPM_VAL, &frame, &life_time);
    igText("%.0f - %.0f/%d", glitter_test.frame_counter, frame, life_time);

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

    igSeparator();

    imguiColumnSliderFloat("Emission", &GPM_VAL->emission, 1.0f, 2.0f, "%.2f", 0);
    imguiColumnSliderFloat("Frame Speed", &frame_speed, 0.0f, 3.0f, "%.2f", 0);

    igSeparator();

    imguiCheckbox("Grid", &grid_3d);

    glitter_test.imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
    lock_unlock(&glitter_test.lock);
}

void glitter_test_input() {
    bool ret = false;
    lock_lock(&glitter_test.lock);
    if (!glitter_test_enabled)
        ret = true;
    lock_unlock(&glitter_test.lock);

    if (ret)
        return;

    input_locked |= glitter_test.imgui_focus;
}

void glitter_test_render() {
    bool dispose = false;
    bool ret = false;
    lock_lock(&glitter_test.lock);
    if (glitter_test.dispose) {
        dispose = true;
        ret = true;
    }
    else if (!glitter_test_enabled)
        ret = true;
    lock_unlock(&glitter_test.lock);

    if (dispose)
        glitter_test_dispose();

    if (ret)
        return;

    lock_lock(&glitter_test.lock);
    if (!glitter_test.input_pause) {
        if (GPM_VAL->scenes.end != GPM_VAL->scenes.begin)
            glitter_test.frame_counter += get_frame_speed();

        glitter_particle_manager_update_scene(GPM_VAL);
    }

    if (glitter_test.file) {
        if (glitter_test.input_play || glitter_test.input_stop) {
            glitter_particle_manager_free_scenes(GPM_VAL);
            glitter_test.frame_counter = 0.0f;
        }

        uint64_t hash = hash_utf8_fnv1a64(glitter_test.file);
        if (glitter_test.input_auto && !glitter_particle_manager_check_scene(GPM_VAL, hash)) {
            if (glitter_particle_manager_check_effect_group(GPM_VAL, hash))
                goto load;
            else
                goto reset;
        }
        else if (glitter_test.input_play && !glitter_particle_manager_test_load_scene(GPM_VAL, hash, true)) {
        reset:
            glitter_particle_manager_free_scenes(GPM_VAL);
            glitter_particle_manager_free_effect_groups(GPM_VAL);

            glitter_file_reader* fr = glitter_file_reader_init(GLITTER_AFT, 0, glitter_test.file, -1.0f);
            vector_ptr_glitter_file_reader_push_back(&GPM_VAL->file_readers, &fr);
            glitter_particle_manager_update_file_reader(GPM_VAL);
        load:
            glitter_test.frame_counter = 0.0f;
            glitter_particle_manager_test_load_scene(GPM_VAL, hash, true);
        }
        glitter_test.input_play = false;
        glitter_test.input_stop = false;
    }
    lock_unlock(&glitter_test.lock);
}
