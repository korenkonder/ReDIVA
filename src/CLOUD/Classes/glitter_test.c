/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.h"
#include "../../CRE/Glitter/glitter.h"
#include "../../CRE/Glitter/effect_group.h"
#include "../../CRE/Glitter/file_reader.h"
#include "../../CRE/Glitter/particle_manager.h"
#include "../../CRE/Glitter/scene.h"
#include "../../CRE/microui.h"
#include "../input.h"

extern mu_Context* muctx;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern float_t frame_speed;

extern glitter_particle_manager* gpm;

const char* glitter_test_window_title = "Glitter";

glitter_test_struct glitter_test;

void glitter_test_dispose() {
    vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)glitter_effect_group_dispose);

    vector_ptr_wchar_t_free(&glitter_test.files, 0);
    glitter_test.file = 0;
    glitter_test.input_play = false;
    glitter_test.input_stop = false;
    glitter_test.input_auto = false;
    glitter_test.input_pause = false;
    glitter_test.frame_counter = 0.0f;
    glitter_test.enabled = false;
    glitter_test.dispose = false;
    glitter_test.disposed = true;
}

void glitter_test_init() {
    vector_ptr_wchar_t_free(&glitter_test.files, 0);
    path_wget_files(&glitter_test.files, L"rom\\particle");
    for (wchar_t** i = glitter_test.files.begin; i != glitter_test.files.end; i++) {
        wchar_t* temp = 0;
        if (path_wcheck_ends_with(*i, L".farc"))
            temp = path_wget_without_extension(*i);
        free(*i);
        *i = temp;
    }

    size_t glitter_test_files_count = 0;
    for (wchar_t** i = glitter_test.files.begin; i != glitter_test.files.end; i++)
        if (*i)
            glitter_test.files.begin[glitter_test_files_count++] = *i;
    glitter_test.files.end = &glitter_test.files.begin[glitter_test_files_count];
    glitter_test.enabled = true;
    gpm->emission = 1.0f;
}

void glitter_test_control() {
    if (!glitter_test.enabled)
        return;
}

void glitter_test_input() {
    if (!glitter_test.enabled)
        return;

    if (input_is_tapped('F'))
        glitter_test.input_play = true;

    if (input_is_tapped('V'))
        glitter_test.input_stop = true;

    if (input_is_tapped('T'))
        glitter_test.input_auto ^= true;

    if (input_is_tapped('G'))
        glitter_test.input_pause ^= true;
}

void glitter_test_mui() {
    if (!glitter_test.enabled)
        return;
    else if (glitter_test.disposed) {
        mu_Container* cnt = mu_get_container(muctx, glitter_test_window_title);
        muctx->hover_root = muctx->next_hover_root = cnt;
        cnt->rect = mu_rect(0, 0, 0, 0);
        cnt->open = true;
        mu_bring_to_front(muctx, cnt);
        glitter_test.disposed = false;
    }

    int32_t w = min(width / 3, 360);
    int32_t h = 340;

    if (!mu_begin_window_ex(muctx, glitter_test_window_title,
        mu_rect(0, 0, w, h),
        0 /*MU_OPT_NOINTERACT | MU_OPT_NOCLOSE*/, false, true, false)) {
        glitter_test.dispose = true;
        return;
    }

    mu_layout_row(muctx, 2, (int[]) { 32, -1 }, 0);
    mu_label(muctx, "File:");
    char* glitter_file_temp = wchar_t_string_to_char_string(glitter_test.file);
    if (mu_button(muctx, glitter_file_temp ? glitter_file_temp : "..."))
        mu_open_popup(muctx, "glitter file");
    free(glitter_file_temp);

    if (mu_begin_window_ex(muctx, "glitter file",
        mu_rect(0, 0, 240, 360),
        MU_OPT_NOTITLE | MU_OPT_POPUP | MU_OPT_CLOSED, false, true, false)) {
        mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
        mu_layout_row(muctx, 1, (int[]) { body.w }, 0);
        for (wchar_t** i = glitter_test.files.begin; i != glitter_test.files.end; i++) {
            mu_push_id(muctx, &i, sizeof(i));
            char* temp = wchar_t_string_to_char_string(*i);
            if (glitter_test.file && !wcscmp(*i, glitter_test.file))
                mu_draw_control_text(muctx, temp, mu_layout_next(muctx), MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
            else if (mu_button(muctx, temp))
                glitter_test.file = *i;
            free(temp);
            mu_pop_id(muctx);
        }
        mu_end_window(muctx);
    }

    mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
    if (mu_button(muctx, "Reset Camera (R)"))
        input_reset = true;

    if (mu_button(muctx, "Play (F)"))
        glitter_test.input_play = true;

    if (mu_button(muctx, "Stop (V)"))
        glitter_test.input_stop = true;

    mu_checkbox(muctx, "Auto (T)", &glitter_test.input_auto);
    mu_checkbox(muctx, "Pause (G)", &glitter_test.input_pause);

    size_t ctrl;
    size_t disp;
    float_t frame;
    float_t life_time;
    char buf[0x40];

    mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
    glitter_particle_manager_get_frame(gpm, &frame, &life_time);
    sprintf_s(buf, sizeof(buf), "%.0f - %.0f/%.0f", glitter_test.frame_counter, frame, life_time);
    mu_label(muctx, buf);

    mu_layout_row(muctx, 2, (int[]) { 40, -1 }, 0);
    ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_QUAD);
    disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_QUAD);
    mu_label(muctx, "Quad: ");
    sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
    mu_label(muctx, buf);

    ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_LOCUS);
    disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_LOCUS);
    mu_label(muctx, "Locus: ");
    sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
    mu_label(muctx, buf);

    ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_LINE);
    disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_LINE);
    mu_label(muctx, "Line: ");
    sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
    mu_label(muctx, buf);

    mu_layout_row(muctx, 2, (int[]) { 80, -1 }, 0);
    mu_label(muctx, "Emission: ");
    mu_slider_step(muctx, &gpm->emission, 1.0f, 2.0f, 0.01f);

    mu_label(muctx, "Frame Speed: ");
    mu_slider_step(muctx, &frame_speed, 0.0f, 3.0f, 0.01f);
    mu_end_window(muctx);
}

void glitter_test_render() {
    if (glitter_test.dispose) {
        glitter_test_dispose();
        return;
    }
    else if (!glitter_test.enabled)
        return;

    if (!glitter_test.input_pause) {
        if (gpm->scenes.end != gpm->scenes.begin)
            glitter_test.frame_counter += get_frame_speed();

        glitter_particle_manager_update_scene(gpm);
    }

    if (glitter_test.file) {
        uint64_t hash = gpm->f2
            ? hash_wchar_t_murmurhash(glitter_test.file, 0, false)
            : hash_wchar_t_fnv1a64(glitter_test.file);
        if (glitter_test.input_play || glitter_test.input_stop) {
            glitter_particle_manager_free_scene(gpm, hash);
            glitter_test.frame_counter = 0.0f;
        }

        if (glitter_test.input_auto && !glitter_particle_manager_check_scene(gpm, hash)) {
            if (glitter_particle_manager_check_effect_group(gpm, hash))
                goto load;
            else
                goto reset;
        }
        else if (glitter_test.input_play && !glitter_particle_manager_test_load_scene(gpm, hash)) {
        reset:
            vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
            vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)glitter_effect_group_dispose);

            glitter_file_reader* fr = glitter_file_reader_init(0, glitter_test.file, gpm->f2, -1.0f);
            vector_ptr_glitter_file_reader_push_back(&gpm->file_readers, &fr);
            glitter_particle_manager_update_file_reader(gpm);
        load:
            glitter_test.frame_counter = 0.0f;
            glitter_particle_manager_test_load_scene(gpm, hash);
        }
        glitter_test.input_play = false;
        glitter_test.input_stop = false;
    }
}

void glitter_test_sound() {
    if (!glitter_test.enabled)
        return;
}
