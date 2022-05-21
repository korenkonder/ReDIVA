/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Curve Editor code based on Animation Timeline code from https://github.com/crash5band/Glitter
*/

#if defined(CLOUD_DEV)
#include "glitter_editor.h"
#include "../../KKdLib/io/path.h"
#include "../../KKdLib/io/stream.h"
#include "../../KKdLib/dds.h"
#include "../../KKdLib/interpolation.h"
#include "../../KKdLib/str_utils.h"
#include "../../KKdLib/txp.hpp"
#include "../../KKdLib/vec.h"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/camera.h"
#include "../../CRE/data.hpp"
#include "../../CRE/draw_task.h"
#include "../../CRE/gl_state.h"
#include "../../CRE/render_context.hpp"
#include "../../CRE/shader_glsl.h"
#include "../../CRE/stage.hpp"
#include "../../CRE/static_var.h"
#include "../input.hpp"
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include "imgui_helper.h"
#include <glad/glad.h>

enum glitter_editor_selected_enum {
    GLITTER_EDITOR_SELECTED_NONE = 0,
    GLITTER_EDITOR_SELECTED_EFFECT,
    GLITTER_EDITOR_SELECTED_EMITTER,
    GLITTER_EDITOR_SELECTED_PARTICLE,
};

enum glitter_editor_flags {
    GLITTER_EDITOR_ADD       = 0x01,
    GLITTER_EDITOR_DUPLICATE = 0x02,
    GLITTER_EDITOR_DELETE    = 0x04,
    GLITTER_EDITOR_MOVE_UP   = 0x08,
    GLITTER_EDITOR_MOVE_DOWN = 0x10,
    GLITTER_EDITOR_IMPORT    = 0x20,
    GLITTER_EDITOR_EXPORT    = 0x40,
};

enum glitter_editor_draw_flags {
    GLITTER_EDITOR_DRAW_WIREFRAME = 0x01,
    GLITTER_EDITOR_DRAW_SELECTED  = 0x02,
    GLITTER_EDITOR_DRAW_NO_DRAW   = 0x04,
};

struct glitter_curve_editor {
    Glitter::CurveType type;
    Glitter::Animation* animation;
    Glitter::Curve* list[Glitter::CURVE_V_SCROLL_ALPHA_2ND - Glitter::CURVE_TRANSLATION_X + 1];
    Glitter::Curve::Key* key;

    int32_t frame_width;
    float_t zoom_time;
    float_t prev_zoom_time;
    float_t zoom_value;
    float_t key_radius_in;
    float_t key_radius_out;
    float_t height_offset;
    int32_t frame;

    ImDrawList* draw_list;
    ImGuiIO* io;
    float_t timeline_pos;

    float_t range;

    bool add_key;
    bool del_key;
    bool add_curve;
    bool del_curve;
    bool key_edit;
};

typedef struct glitter_editor_gl_wireframe {
    shader_glsl shader;
} glitter_editor_gl_wireframe;

typedef struct glitter_editor_gl {
    glitter_editor_gl_wireframe wireframe;
} glitter_editor_gl;

struct glitter_editor_struct {
    bool test;
    bool create_popup;
    bool load;
    bool load_wait;
    bool load_data;
    bool load_data_wait;
    bool load_popup;
    bool load_data_popup;
    bool load_error_list_popup;
    bool save;
    bool save_popup;
    bool save_compress;
    bool close;
    bool input_play;
    bool input_reload;
    bool input_pause;
    bool input_pause_temp;
    bool input_reset;
    bool effect_group_add;
    glitter_editor_draw_flags draw_flags;
    glitter_editor_flags resource_flags;
    glitter_editor_flags effect_flags;
    glitter_editor_flags emitter_flags;
    glitter_editor_flags particle_flags;
    Glitter::Type load_glt_type;
    Glitter::Type save_glt_type;
    data_type load_data_type;
    float_t frame_counter;
    float_t old_frame_counter;
    int32_t start_frame;
    int32_t end_frame;
    Glitter::Random random;
    int32_t counter;
    Glitter::EffectGroup* effect_group;
    Glitter::Scene* scene;
    uint64_t hash;
    Glitter::SceneCounter scene_counter;
    char file[MAX_PATH * 2];
    GLuint vao;

    glitter_editor_selected_enum selected_type;
    int32_t selected_resource;
    Glitter::Effect* selected_effect;
    Glitter::Emitter* selected_emitter;
    Glitter::Particle* selected_particle;
    glitter_curve_editor curve_editor;

    glitter_editor_gl gl_data;
};

static const float_t curve_base_values[] = {
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
};

static const float_t curve_editor_timeline_base_pos = 50.0f;
static ImVec4 tint_col = { 1.0f, 1.0f, 1.0f, 1.0f };
static ImVec4 border_col = { 1.0f, 1.0f, 1.0f, 0.5f };

extern int32_t width;
extern int32_t height;
extern vec2i internal_2d_res;
extern vec2i internal_3d_res;
extern bool input_reset;
extern bool input_locked;
extern bool draw_grid_3d;

extern bool close;
extern bool global_context_menu;

extern float_t rob_frame;

static const char* glitter_editor_window_title = "Glitter Editor";

static void glitter_editor_windows(glitter_editor_struct* glt_edt, class_data* data);

static void glitter_editor_reload(glitter_editor_struct* glt_edt);
static void glitter_editor_reset(glitter_editor_struct* glt_edt);
static void glitter_editor_reset_draw();
static void glitter_editor_save(glitter_editor_struct* glt_edt);
static void glitter_editor_open_window(glitter_editor_struct* glt_edt);
static void glitter_editor_save_window(glitter_editor_struct* glt_edt);
static void glitter_editor_save_as_window(glitter_editor_struct* glt_edt);
static void glitter_editor_load_file(glitter_editor_struct* glt_edt, const char* path, const char* file);
static void glitter_editor_save_file(glitter_editor_struct* glt_edt, const char* path, const char* file);
static bool glitter_editor_list_open_window(Glitter::EffectGroup* eg);
static bool glitter_editor_resource_import(glitter_editor_struct* glt_edt);
static bool glitter_editor_resource_export(glitter_editor_struct* glt_edt);

static void glitter_editor_test_window(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_effects(glitter_editor_struct* glt_edt);
static void glitter_editor_effects_context_menu(glitter_editor_struct* glt_edt,
    Glitter::Effect* effect, Glitter::Emitter* emitter, Glitter::Particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type);
static void glitter_editor_resources(glitter_editor_struct* glt_edt);
static void glitter_editor_resources_context_menu(glitter_editor_struct* glt_edt,
    int32_t resource, ssize_t i_idx, bool selected_none);
static void glitter_editor_play_manager(glitter_editor_struct* glt_edt);
static void glitter_editor_property(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_property_effect(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_property_emitter(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_property_particle(glitter_editor_struct* glt_edt, class_data* data);
static bool glitter_editor_property_particle_texture(glitter_editor_struct* glt_edt,
    class_data* data, const char* label, char** items, Glitter::Particle* particle,
    int32_t* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
    int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex);

static void glitter_editor_popups(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_file_create_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_model_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_error_list_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_save_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);

static void glitter_editor_curve_editor(glitter_editor_struct* glt_edt);
static void glitter_editor_curve_editor_curve_set(glitter_editor_struct* glt_edt,
    Glitter::Curve* curve, Glitter::CurveType type);
static void glitter_editor_curve_editor_curves_reset(glitter_editor_struct* glt_edt);
static Glitter::Curve::Key* glitter_editor_curve_editor_get_closest_key(
    glitter_editor_struct* glt_edt, Glitter::Curve* curve);
static Glitter::Curve::Key* glitter_editor_curve_editor_get_selected_key(
    glitter_editor_struct* glt_edt, Glitter::Curve* curve);
static float_t glitter_editor_curve_editor_get_value(glitter_editor_struct* glt_edt,
    Glitter::CurveType type);
static void glitter_editor_curve_editor_key_manager(glitter_editor_struct* glt_edt,
    std::vector<Glitter::Curve::Key>* keys, bool* add_key, bool* del_key);
static void glitter_editor_curve_editor_property_window(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_curve_editor_reset_state(glitter_editor_struct* glt_edt,
    Glitter::CurveType type);
static void glitter_editor_curve_editor_selector(glitter_editor_struct* glt_edt, class_data* data);

static void glitter_editor_gl_load(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_draw(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_process(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_free(glitter_editor_struct* glt_edt);

static void glitter_editor_gl_draw_wireframe(glitter_editor_struct* glt_edt);
static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::EffectInst* eff, Glitter::ParticleInst* ptcl);
static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::F2EffectInst* eff, Glitter::F2ParticleInst* ptcl);
static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::XEffectInst* eff, Glitter::XParticleInst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::EffectInst* eff, Glitter::ParticleInst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::F2EffectInst* eff, Glitter::F2ParticleInst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::XEffectInst* eff, Glitter::XParticleInst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw_mesh(glitter_editor_struct* glt_edt,
    Glitter::XRenderGroup* rg);

static void glitter_editor_gl_select_particle(glitter_editor_struct* glt_edt);

static bool glitter_editor_hash_input(glitter_editor_struct* glt_edt,
    const char* label, uint64_t* hash);

bool glitter_editor_init(class_data* data, render_context* rctx) {
    data->data = force_malloc_s(glitter_editor_struct, 1);

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    Glitter::glt_particle_manager.random.value = (uint32_t)(time.LowPart * hash_fnv1a64m_empty);
    Glitter::glt_particle_manager.random.step = 1;

    QueryPerformanceCounter(&time);
    Glitter::glt_particle_manager.counter = (uint32_t)(time.LowPart * hash_murmurhash_empty);

    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (glt_edt) {
        glt_edt->test = false;
        glt_edt->draw_flags = (glitter_editor_draw_flags)0;
        Glitter::glt_particle_manager.emission = 1.0f;
        Glitter::glt_particle_manager.draw_all = true;
        Glitter::glt_particle_manager.draw_all_mesh = true;
        glGenVertexArrays(1, &glt_edt->vao);
        draw_grid_3d = true;

        glitter_editor_reset(glt_edt);
        glitter_editor_gl_load(glt_edt);
        dtm_stg_load(0);
    }

    /*char* path_x = "VRFL\\";
    vector_old_string files_x = vector_old_empty(string);
    path_get_files(&files_x, path_x);
    for (string* i = files_x.begin; i != files_x.end;)
        if (str_utils_check_ends_with(string_data(i), ".farc"))
            i++;
        else {
            string_free(i);
            vector_old_string_erase(&files_x, i - files_x.begin);
        }

    ssize_t c = vector_old_length(files_x);
    GlitterFileReader* fr = 0;
    if (files_x.begin) {
        stream s;
        io_open(&s, L"name_VRFL.glitter.txt", L"rb");
        size_t length = s.length;
        uint8_t* data = force_malloc(length);
        io_read(&s, data, length);
        io_free(&s);

        char* buf;
        char** lines;
        size_t count;
        if (str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
            for (size_t i = 0; i < count; i++) {
                char* t = strstr(lines[i], "#(?)");
                if (t)
                    *t = 0;
            }

            uint64_t* hashes = force_malloc_s(uint64_t, count);
            for (size_t i = 0; i < count; i++) {
                size_t len = utf8_length(lines[i]);
                hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F));
            }

            for (ssize_t i = 0; i < c; i++) {
                char* file_x = str_utils_get_without_extension(string_data(&files_x.begin[i]));

                char buf[0x100];
                sprintf_s(buf, 0x100, "%hs\n", file_x);
                OutputDebugStringA(buf);

                glt_edt->load_glt_type = Glitter::X;
                fr = glitter_file_reader_init(Glitter::X, path_x, file_x, 1.0f);
                glitter_file_reader_read(fr, Glitter::glt_particle_manager.emission);

                if (fr && fr->effect_group) {
                    Glitter::EffectGroup* eg = fr->effect_group;
                    for (Glitter::Effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                        if (!*i)
                            continue;

                        Glitter::Effect* e = *i;
                        if (e->data.name_hash == hash_murmurhash_empty)
                            continue;

                        size_t j;
                        for (j = 0; j < count; j++)
                            if (e->data.name_hash == hashes[j])
                                break;

                        if (j == count) {
                            char buf[0x100];
                            sprintf_s(buf, 0x100, "%08llX\n", e->data.name_hash);
                            OutputDebugStringA(buf);
                        }
                    }

                    Glitter::EffectGroup_dispose(fr->effect_group);
                }
                free(file_x);
            }
            free(lines);
            free(hashes);
        }
        free(data);
    }
    for (string* i = files_x.begin; i != files_x.end; i++)
        string_free(i);
    vector_old_string_free(&files_x);
    glitter_file_reader_dispose(fr);
    glt_edt->effect_group = 0;
    return;*/

    /*char* path_x = "X\\";
    vector_old_string files_x = vector_old_empty(string);
    path_get_files(&files_x, path_x);
    for (string* i = files_x.begin; i != files_x.end;)
        if (str_utils_check_ends_with(string_data(i), ".farc"))
            i++;
        else {
            string_free(i);
            vector_old_string_erase(&files_x, i - files_x.begin);
        }

    ssize_t c = vector_old_length(files_x);
    GlitterFileReader* fr = 0;
    if (files_x.begin)
        for (ssize_t i = 0; i < c; i++) {
            char* file_x = str_utils_get_without_extension(string_data(&files_x.begin[i]));

            char buf[0x100];
            sprintf_s(buf, 0x100, "%hs\n", file_x);
            OutputDebugStringA(buf);

            glt_edt->load_glt_type = Glitter::X;
            fr = glitter_file_reader_init(Glitter::X, path_x, file_x, 1.0f);
            glitter_file_reader_read(fr, Glitter::glt_particle_manager.emission);

            if (fr && fr->effect_group) {
                Glitter::EffectGroup* eg = fr->effect_group;
                for (Glitter::Effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    Glitter::Effect* e = *i;
                    if (!e->data.ext_anim)
                        continue;

                    Glitter::Effect_ext_anim* ea = e->data.ext_anim;
                    if (ea->instance_id != 0) {
                        char buf[0x100];
                        sprintf_s(buf, 0x100, "%08llX %d\n", e->data.name_hash, ea->instance_id);
                        OutputDebugStringA(buf);
                    }
                }

                Glitter::EffectGroup_dispose(fr->effect_group);
            }
            free(file_x);
        }
    for (string* i = files_x.begin; i != files_x.end; i++)
        string_free(i);
    vector_old_string_free(&files_x);
    glitter_file_reader_dispose(fr);
    glt_edt->effect_group = 0;
    return;*/

    /*char* path_f2 = "F2\\";
    char* path_ft = "AFT\\";
    vector_old_string files_f2 = vector_old_empty(string);
    vector_old_string files_ft = vector_old_empty(string);
    path_get_files(&files_f2, path_f2);
    path_get_files(&files_ft, path_ft);
    for (string* i = files_f2.begin; i != files_f2.end;)
        if (str_utils_check_ends_with(string_data(i), ".farc"))
            i++;
        else {
            string_free(i);
            vector_old_string_erase(&files_f2, i - files_f2.begin);
        }

    for (string* i = files_ft.begin; i != files_ft.end;)
        if (str_utils_check_ends_with(string_data(i), ".farc"))
            i++;
        else {
            string_free(i);
            vector_old_string_erase(&files_ft, i - files_ft.begin);
        }

    stream s;
    io_open(&s, L"name_F2.glitter.txt", L"rb");
    size_t length = s.length;
    uint8_t* data = force_malloc(length);
    io_read(&s, data, length);
    io_free(&s);

    char* buf;
    char** lines;
    size_t count;
    if (str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
        for (size_t i = 0; i < count; i++) {
            char* t = strstr(lines[i], "#(?)");
            if (t)
                *t = 0;
        }

        uint64_t* hashes = force_malloc_s(uint64_t, count);
        for (size_t i = 0; i < count; i++) {
            size_t len = utf8_length(lines[i]);
            hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F));
        }

        ssize_t c = vector_old_length(files_f2);
        GlitterFileReader* fr = 0;
        if (files_f2.begin && files_ft.begin)
            for (ssize_t i = 0; i < c; i++) {
                char* file_f2 = str_utils_get_without_extension(string_data(&files_f2.begin[i]));
                char* file_ft = str_utils_get_without_extension(string_data(&files_ft.begin[i]));
                glt_edt->load_glt_type = Glitter::F2;
                fr = glitter_file_reader_init(Glitter::F2, path_f2, file_f2, 1.0f);
                glitter_file_reader_read(fr, Glitter::glt_particle_manager.emission);

                if (fr && fr->effect_group) {
                    Glitter::EffectGroup* eg = fr->effect_group;
                    for (Glitter::Effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                        if (!*i)
                            continue;

                        Glitter::Effect* e = *i;
                        memset(e->name, 0, 0x80);
                        if (e->data.name_hash == hash_murmurhash_empty)
                            continue;

                        size_t j;
                        for (j = 0; j < count; j++)
                            if (e->data.name_hash == hashes[j])
                                break;

                        if (j == count)
                            break;

                        size_t len = utf8_length(lines[j]);
                        memcpy(e->name, lines[j], min(len, 0x7F));
                    }

                    glt_edt->effect_group = fr->effect_group;
                    //glt_edt->save_glt_type = Glitter::FT;
                    //glitter_editor_save_file(path_ft, file_ft);
                    Glitter::EffectGroup_dispose(fr->effect_group);
                }
                free(file_f2);
                free(file_ft);
            }
        glitter_file_reader_dispose(fr);

        free(buf);
        free(lines);
        free(hashes);
    }
    for (string* i = files_f2.begin; i != files_f2.end; i++)
        string_free(i);
    vector_old_string_free(&files_f2);

    for (string* i = files_ft.begin; i != files_ft.end; i++)
        string_free(i);
    vector_old_string_free(&files_ft);
    free(data);
    glt_edt->effect_group = 0;*/
    return true;
}

void glitter_editor_ctrl(class_data* data) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    int32_t sel_rsrc = glt_edt->selected_resource;
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    if (glt_edt->effect_group_add) {
        Glitter::glt_particle_manager.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        Glitter::EffectGroup* eg = new Glitter::EffectGroup(glt_edt->load_glt_type);
        char buf[0x100];
        snprintf(buf, sizeof(buf), "eff_%016llx_main", time.LowPart * hash_fnv1a64m_empty);
        uint64_t hash = glt_edt->load_glt_type == Glitter::FT
            ? hash_utf8_fnv1a64m(buf) : hash_utf8_murmurhash(buf);
        eg->hash = hash;
        eg->load_count = 1;
        Glitter::glt_particle_manager.effect_groups.insert({ hash, eg });

        glt_edt->scene_counter = Glitter::glt_particle_manager.LoadScene(hash, eg->type != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty);
        glt_edt->effect_group = eg;
        glt_edt->hash = hash;
    }

    bool has_resource = sel_rsrc != -1;
    bool has_effect = glt_edt->selected_effect != 0;
    bool has_emitter = glt_edt->selected_emitter != 0 && has_effect;
    bool has_particle = glt_edt->selected_particle != 0 && has_emitter;
    Glitter::EffectGroup* eg = glt_edt->effect_group;

    bool tex_reload = false;
    if (glt_edt->resource_flags & GLITTER_EDITOR_IMPORT)
        if (glitter_editor_resource_import(glt_edt))
            tex_reload = true;

    if (eg && has_resource) {
        if (glt_edt->resource_flags & GLITTER_EDITOR_EXPORT)
            glitter_editor_resource_export(glt_edt);
        else if (glt_edt->resource_flags & GLITTER_EDITOR_DELETE) {
            size_t rc = eg->resources_count;

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == sel_rsrc) {
                    j = i;
                    break;
                }

            if (j > -1) {
                uint64_t hash = eg->resource_hashes[j];
                eg->resource_hashes.erase(eg->resource_hashes.begin() + j);
                eg->resources_tex.textures.erase(eg->resources_tex.textures.begin() + j);
                eg->resources_count--;

                uint64_t empty_hash = eg->type != Glitter::FT
                    ? hash_murmurhash_empty : hash_fnv1a64m_empty;
                for (Glitter::Effect*& i : eg->effects) {
                    if (!i)
                        continue;

                    Glitter::Effect* effect = i;
                    for (Glitter::Emitter*& j : effect->emitters) {
                        if (!j)
                            continue;

                        Glitter::Emitter* emitter = j;
                        for (Glitter::Particle*& k : emitter->particles) {
                            if (!k)
                                continue;

                            Glitter::Particle* particle = k;
                            if (particle->data.tex_hash == hash) {
                                particle->data.tex_hash = empty_hash;
                                particle->data.texture = 0;
                            }

                            if (particle->data.mask_tex_hash == hash) {
                                particle->data.mask_tex_hash = empty_hash;
                                particle->data.mask_texture = 0;
                            }
                        }
                    }
                }
            }
            glt_edt->selected_resource = -1;
            tex_reload = true;
        }
        else if (glt_edt->resource_flags & GLITTER_EDITOR_MOVE_UP) {
            size_t rc = eg->resources_count;
            uint64_t* rh = eg->resource_hashes.data();
            txp* rt = eg->resources_tex.textures.data();

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == sel_rsrc) {
                    j = i;
                    break;
                }

            if (j > 0) {
                uint64_t temp_hash = rh[j - 1];
                txp temp_tex = rt[j - 1];
                rh[j - 1] = rh[j];
                rt[j - 1] = rt[j];
                rh[j] = temp_hash;
                rt[j] = temp_tex;
                glt_edt->input_reload = true;
            }
            tex_reload = true;
        }
        else if (glt_edt->resource_flags & GLITTER_EDITOR_MOVE_DOWN) {
            size_t rc = eg->resources_count;
            uint64_t* rh = eg->resource_hashes.data();
            txp* rt = eg->resources_tex.textures.data();

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == sel_rsrc) {
                    j = i;
                    break;
                }

            if (j > -1 && j < (ssize_t)rc - 1) {
                uint64_t temp_hash = rh[j + 1];
                txp temp_tex = rt[j + 1];
                rh[j + 1] = rh[j];
                rt[j + 1] = rt[j];
                rh[j] = temp_hash;
                rt[j] = temp_tex;
                glt_edt->input_reload = true;
            }
            tex_reload = true;
        }
    }

    if (tex_reload && glt_edt->effect_group) {
        //glitter_texture_load(&Glitter::glt_particle_manager, glt_edt->effect_group);
        glt_edt->input_reload = true;
    }

    if (eg && glt_edt->effect_flags & GLITTER_EDITOR_ADD) {
        ssize_t eff_count = eg->effects.size();

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        Glitter::Effect* e = new Glitter::Effect(eg->type);
        char name[0x80];
        sprintf_s(name, sizeof(name), "eff_%08x",
            (uint32_t)((eff_count + 1) * time.LowPart * hash_murmurhash_empty));
        e->name = name;
        eg->effects.push_back(e);
        glt_edt->input_reload = true;
    }

    if (eg && has_effect)
        if (glt_edt->effect_flags & GLITTER_EDITOR_DUPLICATE) {
            ssize_t eff_count = eg->effects.size();

            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            Glitter::Effect* e = new Glitter::Effect(eg->type);
            *e = *glt_edt->selected_effect;
            if (eg->type != Glitter::X) {
                if (glt_edt->selected_effect->data.ext_anim) {
                    e->data.ext_anim = force_malloc_s(Glitter::Effect::ExtAnim, 1);
                    *e->data.ext_anim = *glt_edt->selected_effect->data.ext_anim;
                }
            }
            else {
                if (glt_edt->selected_effect->data.ext_anim_x) {
                    e->data.ext_anim_x = force_malloc_s(Glitter::Effect::ExtAnimX, 1);
                    *e->data.ext_anim_x = *glt_edt->selected_effect->data.ext_anim_x;
                }
            }

            char name[0x80];
            sprintf_s(name, sizeof(name), "eff_%08x",
                (uint32_t)((eff_count + 1)* time.LowPart* hash_murmurhash_empty));
            e->name = name;
            eg->effects.push_back(e);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->effect_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = -1;
            for (Glitter::Effect*& i : eg->effects)
                if (i && i == glt_edt->selected_effect) {
                    j = &i - eg->effects.data();
                    break;
                }

            if (j > -1)
                eg->effects.erase(eg->effects.begin() + j);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_NONE;
            glt_edt->selected_resource = -1;
            glt_edt->selected_effect = 0;
            glt_edt->selected_emitter = 0;
            glt_edt->selected_particle = 0;

            if (eg->effects.size() < 1)
                glt_edt->close = true;
            else
                glt_edt->input_reload = true;
        }
        else if (glt_edt->effect_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (Glitter::Effect*& i : eg->effects)
                if (i == glt_edt->selected_effect) {
                    j = &i - eg->effects.data();
                    break;
                }

            if (j > 0) {
                Glitter::Effect* temp = eg->effects.data()[j - 1];
                eg->effects.data()[j - 1] = eg->effects.data()[j];
                eg->effects.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->effect_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (Glitter::Effect*& i : eg->effects)
                if (i == glt_edt->selected_effect) {
                    j = &i - eg->effects.data();
                    break;
                }

            if (j > -1 && j < (ssize_t)eg->effects.size() - 1) {
                Glitter::Effect* temp = eg->effects.data()[j + 1];
                eg->effects.data()[j + 1] = eg->effects.data()[j];
                eg->effects.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_ADD) {
            Glitter::Emitter* e = new Glitter::Emitter(eg->type);
            sel_efct->emitters.push_back(e);
            glt_edt->input_reload = true;
        }

    if (eg && has_emitter)
        if (glt_edt->emitter_flags & GLITTER_EDITOR_DUPLICATE) {
            Glitter::Emitter* e = new Glitter::Emitter(eg->type);
            *e = *glt_edt->selected_emitter;
            sel_efct->emitters.push_back(e);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = -1;
            for (Glitter::Emitter*& i : sel_efct->emitters)
                if (i && i == glt_edt->selected_emitter) {
                    j = &i - sel_efct->emitters.data();
                    break;
                }

            if (j > -1)
                sel_efct->emitters.erase(sel_efct->emitters.begin() + j);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_EFFECT;
            glt_edt->selected_emitter = 0;
            glt_edt->selected_particle = 0;
            glt_edt->input_reload = true;
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (Glitter::Emitter*& i : sel_efct->emitters)
                if (i == glt_edt->selected_emitter) {
                    j = &i - sel_efct->emitters.data();
                    break;
                }

            if (j > 0) {
                Glitter::Emitter* temp = sel_efct->emitters.data()[j - 1];
                sel_efct->emitters.data()[j - 1] = sel_efct->emitters.data()[j];
                sel_efct->emitters.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (Glitter::Emitter*& i : sel_efct->emitters)
                if (i == glt_edt->selected_emitter) {
                    j = &i - sel_efct->emitters.data();
                    break;
                }

            if (j > -1 && j < (ssize_t)sel_efct->emitters.size() - 1) {
                Glitter::Emitter* temp = sel_efct->emitters.data()[j + 1];
                sel_efct->emitters.data()[j + 1] = sel_efct->emitters.data()[j];
                sel_efct->emitters.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_ADD) {
            Glitter::Particle* p = new Glitter::Particle(eg->type);
            sel_emit->particles.push_back(p);
            glt_edt->input_reload = true;
        }

    if (eg && has_particle)
        if (glt_edt->particle_flags & GLITTER_EDITOR_DUPLICATE) {
            Glitter::Particle* p = new Glitter::Particle(eg->type);
            *p = *glt_edt->selected_particle;
            sel_emit->particles.push_back(p);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = (ssize_t)-1;
            for (Glitter::Particle*& i : sel_emit->particles)
                if (i && i == glt_edt->selected_particle) {
                    j = &i - sel_emit->particles.data();
                    break;
                }

            if (j > -1)
                sel_emit->particles.erase(sel_emit->particles.begin() + j);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_EMITTER;
            glt_edt->selected_particle = 0;
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (Glitter::Particle*& i : sel_emit->particles)
                if (i == glt_edt->selected_particle) {
                    j = &i - sel_emit->particles.data();
                    break;
                }

            if (j > 0) {
                Glitter::Particle* temp = sel_emit->particles.data()[j - 1];
                sel_emit->particles.data()[j - 1] = sel_emit->particles.data()[j];
                sel_emit->particles.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (Glitter::Particle*& i : sel_emit->particles)
                if (i == glt_edt->selected_particle) {
                    j = &i - sel_emit->particles.data();
                    break;
                }

            if (j > -1 && j < (ssize_t)sel_emit->particles.size() - 1) {
                Glitter::Particle* temp = sel_emit->particles.data()[j + 1];
                sel_emit->particles.data()[j + 1] = sel_emit->particles.data()[j];
                sel_emit->particles.data()[j] = temp;
                glt_edt->input_reload = true;
            }
        }

    glt_edt->effect_group_add = false;
    glt_edt->resource_flags = (glitter_editor_flags)0;
    glt_edt->effect_flags = (glitter_editor_flags)0;
    glt_edt->emitter_flags = (glitter_editor_flags)0;
    glt_edt->particle_flags = (glitter_editor_flags)0;

    if (glt_edt->load || glt_edt->save) {
        char* file = str_utils_get_without_extension(glt_edt->file);
        char* path = str_utils_split_right_get_left_include(glt_edt->file, '\\');

        if (glt_edt->load)
            glitter_editor_load_file(glt_edt, path, file);
        else if (glt_edt->save && glt_edt->effect_group)
            glitter_editor_save_file(glt_edt, path, file);

        glt_edt->save = false;
        glt_edt->save_compress = false;
        free(path);
        free(file);
        return;
    }
    else if (eg && glt_edt->load_data) {
        bool load_success = true;
        data_struct* ds = &data_list[glt_edt->load_data_type];
        switch (ds->type) {
        case DATA_F2LE:
        case DATA_F2BE:
        case DATA_VRFL:
        case DATA_X:
        case DATA_XHD: {
            std::vector<uint32_t>& hashes = ds->glitter_list_murmurhash;
            for (Glitter::Effect*& i : eg->effects) {
                if (!i)
                    continue;

                Glitter::Effect* e = i;
                e->name.clear();
                if (e->data.name_hash == hash_murmurhash_empty)
                    continue;

                std::vector<uint32_t>::iterator hash = hashes.end();
                for (std::vector<uint32_t>::iterator j = hashes.begin(); j != hashes.end(); j++)
                    if (e->data.name_hash == *j) {
                        hash = j;
                        break;
                    }

                if (hash == hashes.end()) {
                    load_success = false;
                    continue;
                }

                e->name = ds->glitter_list_names[hash - hashes.begin()];
            }

            switch (ds->type) {
            case DATA_VRFL:
            case DATA_X:
            case DATA_XHD:
                if (!glt_edt->load_data_wait && eg->CheckModel()) {
                    eg->LoadModel(ds);
                    glt_edt->load_data_wait = true;
                }
                break;
            }
        } break;
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39: {
            std::vector<uint64_t>& hashes = ds->glitter_list_fnv1a64m;
            for (Glitter::Effect* i : eg->effects) {
                if (!i)
                    continue;

                Glitter::Effect* e = i;
                e->name.clear();
                if (e->data.name_hash == hash_fnv1a64m_empty)
                    continue;

                std::vector<uint64_t>::iterator hash = hashes.end();
                for (std::vector<uint64_t>::iterator j = hashes.begin(); j != hashes.end(); j++)
                    if (e->data.name_hash == *j) {
                        hash = j;
                        break;
                    }

                if (hash == hashes.end()) {
                    load_success = false;
                    continue;
                }

                e->name = ds->glitter_list_names[hash - hashes.begin()];
            }
        } break;
        }

        if (glt_edt->load_data_wait)
            switch (ds->type) {
            case DATA_VRFL:
            case DATA_X:
            case DATA_XHD:
                if (!eg->CheckLoadModel()) {
                    glt_edt->load_data_wait = false;
                    glt_edt->load_data = false;
                }
                break;
            }
        else
            glt_edt->load_data = false;

        if (!load_success && !glitter_editor_list_open_window(glt_edt->effect_group)) {
            glt_edt->load_error_list_popup = true;
            goto effect_unload;
        }
        return;
    }
    else if (glt_edt->close) {
    effect_unload:
        data_struct* ds = &data_list[glt_edt->load_data_type];
        switch (ds->type) {
        case DATA_VRFL:
        case DATA_X:
        case DATA_XHD:
            if (eg)
                eg->FreeModel();
            break;
        }

        Glitter::glt_particle_manager.FreeSceneEffect(glt_edt->scene_counter, hash_fnv1a64m_empty, true);
        Glitter::glt_particle_manager.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();
        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->hash = 0;
        glt_edt->scene_counter = 0;
        memset(glt_edt->file, 0, sizeof(glt_edt->file));
        glt_edt->close = false;
        return;
    }

    Glitter::glt_particle_manager.GetStartEndFrame(&glt_edt->start_frame, &glt_edt->end_frame, glt_edt->hash);

    if (!glt_edt->effect_group) {
        glt_edt->input_play = false;
        glt_edt->input_reset = false;
        return;
    }

    if (!(glt_edt->input_pause || glt_edt->input_pause_temp) && Glitter::glt_particle_manager.scenes.size())
        glt_edt->frame_counter += get_delta_frame();

    if (glt_edt->input_reload) {
        Glitter::glt_particle_manager.FreeSceneEffect(glt_edt->scene_counter, hash_fnv1a64m_empty, true);
        glt_edt->effect_group->emission = Glitter::glt_particle_manager.emission;
        Glitter::glt_particle_manager.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
        Glitter::glt_particle_manager.scene = glt_edt->scene;
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
        glt_edt->input_reload = false;
    }

    if (glt_edt->old_frame_counter != glt_edt->frame_counter) {
        if (glt_edt->end_frame > 0)
            while (glt_edt->frame_counter >= (float_t)glt_edt->end_frame)
                glt_edt->frame_counter -= (float_t)(glt_edt->end_frame - glt_edt->start_frame);

        if (glt_edt->frame_counter < (float_t)glt_edt->start_frame)
            glt_edt->frame_counter = (float_t)glt_edt->start_frame;

        Glitter::glt_particle_manager.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, false);
        Glitter::glt_particle_manager.scene = glt_edt->scene;
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
    }

    if (glt_edt->input_play)
        glt_edt->input_pause = false;
    else if (glt_edt->input_reset) {
        Glitter::glt_particle_manager.counter = glt_edt->counter;
        glt_edt->frame_counter = (float_t)glt_edt->start_frame;
        Glitter::glt_particle_manager.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
        Glitter::glt_particle_manager.scene = glt_edt->scene;
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
        glt_edt->input_pause = true;
    }
    glt_edt->input_play = false;
    glt_edt->input_reset = false;

    rob_frame = glt_edt->frame_counter;

    glitter_editor_gl_process(glt_edt);
}

void glitter_editor_draw(class_data* data) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    glitter_editor_gl_draw(glt_edt);
}

void glitter_editor_drop(class_data* data, size_t count, char** paths) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    size_t c = min(utf8_length(paths[0]), sizeof(glt_edt->file) - 1);
    memcpy(glt_edt->file, paths[0], c);
    glt_edt->file[c] = 0;
    glt_edt->load_popup = true;
}

void glitter_editor_imgui(class_data* data) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    data->imgui_focus = false;
    if (glt_edt->test)
        glitter_editor_test_window(glt_edt, data);
    else
        glitter_editor_windows(glt_edt, data);
    glitter_editor_popups(glt_edt, data);
}

void glitter_editor_input(class_data* data) {
    if (ImGui::GetIO().WantTextInput)
        return;

    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    if (Input::IsKeyTapped(GLFW_KEY_O, GLFW_MOD_CONTROL))
        glitter_editor_open_window(glt_edt);
    else if (Input::IsKeyTapped(GLFW_KEY_F4, GLFW_MOD_CONTROL))
        glt_edt->close = true;
    else if (Input::IsKeyTapped(GLFW_KEY_F3, GLFW_MOD_CONTROL))
        enum_or(data->flags, CLASS_DISPOSE);
    else if (Input::IsKeyTapped(GLFW_KEY_Q, GLFW_MOD_CONTROL))
        close = true;
    else if (Input::IsKeyTapped(GLFW_KEY_P))
        glt_edt->test ^= true;
    else if (Input::IsKeyTapped(GLFW_KEY_T))
        glt_edt->input_play = true;
    else if (Input::IsKeyTapped(GLFW_KEY_Y))
        glt_edt->input_reload = true;
    else if (Input::IsKeyTapped(GLFW_KEY_F))
        glt_edt->input_pause = true;
    else if (Input::IsKeyTapped(GLFW_KEY_G))
        glt_edt->input_reset = true;

    if (data->imgui_focus) {
        if (Input::IsKeyTapped(GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
            glitter_editor_save_as_window(glt_edt);
        else if (Input::IsKeyTapped(GLFW_KEY_S, GLFW_MOD_CONTROL))
            glitter_editor_save_window(glt_edt);
    }
}

bool glitter_editor_dispose(class_data* data) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (glt_edt) {
        dtm_stg_unload();
        glitter_editor_gl_free(glt_edt);
        Glitter::glt_particle_manager.FreeSceneEffect(glt_edt->scene_counter, hash_fnv1a64m_empty, true);
        Glitter::glt_particle_manager.UnloadEffectGroup(glt_edt->hash);

        glt_edt->test = false;
        glt_edt->create_popup = false;
        glt_edt->load = false;
        glt_edt->load_data = false;
        glt_edt->load_data_wait = false;
        glt_edt->load_popup = false;
        glt_edt->load_data_popup = false;
        glt_edt->load_error_list_popup = false;
        glt_edt->save = false;
        glt_edt->save_popup = false;
        glt_edt->save_compress = false;
        glt_edt->close = false;
        glt_edt->input_play = false;
        glt_edt->input_reload = false;
        glt_edt->input_pause = true;
        glt_edt->input_reset = false;
        glt_edt->effect_group_add = false;
        glt_edt->draw_flags = (glitter_editor_draw_flags)0;
        glt_edt->resource_flags = (glitter_editor_flags)0;
        glt_edt->effect_flags = (glitter_editor_flags)0;
        glt_edt->emitter_flags = (glitter_editor_flags)0;
        glt_edt->particle_flags = (glitter_editor_flags)0;
        glt_edt->frame_counter = 0.0f;
        glt_edt->old_frame_counter = 0.0f;
        glt_edt->start_frame = 0;
        glt_edt->end_frame = 0;
        glt_edt->random.value = 0;
        glt_edt->random.step = 1;
        glt_edt->counter = 0;
        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->hash = 0;
        glt_edt->scene_counter = 0;
        memset(glt_edt->file, 0, sizeof(glt_edt->file));
        glDeleteVertexArrays(1, &glt_edt->vao);
    }
    draw_grid_3d = false;
    free(data->data);

    data->flags = (class_flags)(CLASS_HIDDEN | CLASS_DISPOSED);
    data->imgui_focus = false;
    return true;
}

static void glitter_editor_windows(glitter_editor_struct* glt_edt, class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    const float_t title_bar_size = font->FontSize
        + style.ItemSpacing.y + style.FramePadding.y * 2.0f;

    ImVec2 main_menu_bar_size = { 0, 0 };

    if (ImGui::BeginMainMenuBar()) {
        bool is_x = glt_edt->effect_group
            && glt_edt->effect_group->type == Glitter::X ? true : false;
        if (ImGui::BeginMenu("File", true)) {
            if (ImGui::MenuItem("Open", "Ctrl+O"))
                glitter_editor_open_window(glt_edt);
            if (ImGui::MenuItem("Save", "Ctrl+S", false, !is_x))
                glitter_editor_save_window(glt_edt);
            if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false, !is_x))
                glitter_editor_save_as_window(glt_edt);
            if (ImGui::MenuItem("Close", "Ctrl+F4"))
                glt_edt->close = true;
            if (ImGui::MenuItem("Close Editor", "Ctrl+F3"))
                enum_or(data->flags, CLASS_DISPOSE);
            if (ImGui::MenuItem("Quit", "Ctrl+Q"))
                close = true;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit", false)) {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Mode", glt_edt->effect_group ? true : false)) {
            Glitter::EffectGroup* eg = glt_edt->effect_group;
            if (eg->type != Glitter::X) {
                if (ImGui::MenuItem("F2", 0, false, eg->type != Glitter::F2)) {
                    eg->type = Glitter::F2;
                    glitter_editor_reload(glt_edt);
                }

                if (ImGui::MenuItem("FT", 0, false, eg->type != Glitter::FT)) {
                    eg->type = Glitter::FT;
                    glitter_editor_reload(glt_edt);
                }
            }
            else {
                if (ImGui::MenuItem("X", 0, false, eg->type != Glitter::X)) {
                    eg->type = Glitter::X;
                    glitter_editor_reload(glt_edt);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View", true)) {
            imguiCheckboxFlags("Draw Wireframe",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
            imguiCheckboxFlags("Draw Selected",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
            imguiCheckboxFlags("No Draw",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_NO_DRAW);
            ImGui::EndMenu();
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        main_menu_bar_size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;
    bool selected = glt_edt->selected_effect || glt_edt->selected_emitter
        || glt_edt->selected_particle ? true : false;

    win_x = min((float_t)width / 4.0f, 400.0f);

    x = 0;
    y = main_menu_bar_size.y;
    w = min((float_t)width / 4.0f, 400.0f);
    h = (float_t)height - y - min((float_t)height / 3.0f, 320.0f);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Effects Window");
    if (ImGui::Begin("Effects", 0, window_flags)) {
        if (ImGui::BeginTabBar("Effects Tab", 0)) {
            if (ImGui::BeginTabItem("Effects", 0, 0)) {
                if (glt_edt->effect_group)
                    glitter_editor_effects(glt_edt);
                ImGui::EndTabItem();
            }

            imguiDisableElementPush(glt_edt->effect_group != 0);
            if (ImGui::BeginTabItem("Resources", 0, 0)) {
                glitter_editor_resources_context_menu(glt_edt, -1, 0, true);
                if (glt_edt->effect_group)
                    glitter_editor_resources(glt_edt);
                ImGui::EndTabItem();
            }
            imguiDisableElementPop(glt_edt->effect_group != 0);

            if (ImGui::BeginTabItem("Play Manager", 0, 0)) {
                glitter_editor_play_manager(glt_edt);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();

    win_x = min((float_t)width / 4.0f, 400.0f);

    x = (float_t)width - win_x;
    y = main_menu_bar_size.y;
    w = win_x;
    h = (float_t)height - y - min((float_t)height / 3.0f, 320.0f);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Property Window");
    if (ImGui::Begin("Property", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_property(glt_edt, data);
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();

    win_x = min((float_t)width / 4.0f, 400.0f);
    win_y = min((float_t)height / 3.0f, 320.0f);

    x = 0;
    y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Curve Editor Selector Window");
    if (ImGui::Begin("Curve Editor Selector", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor_selector(glt_edt, data);
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();

    win_x = min((float_t)width / 4.0f, 400.0f);
    win_y = min((float_t)height / 3.0f, 320.0f);

    x = (float_t)width - win_x;
    y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Curve Editor Property Window");
    if (ImGui::Begin("Curve Editor Property", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor_property_window(glt_edt, data);
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();

    win_x = min((float_t)width / 4.0f, 400.0f);
    win_y = min((float_t)height / 3.0f, 320.0f);

    x = win_x;
    y = (float_t)height - win_y;
    w = (float_t)width - win_x * 2.0f;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Curve Editor Window");
    if (ImGui::Begin("Curve Editor", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor(glt_edt);
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();
}

static void glitter_editor_reload(glitter_editor_struct* glt_edt) {
    if (!glt_edt->effect_group)
        return;

    glt_edt->input_reload = true;
    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Scene* sc = glt_edt->scene;
    sc->type = eg->type;
    for (Glitter::Effect*& i : eg->effects) {
        if (!i)
            continue;

        Glitter::Effect* effect = i;
        if (eg->type != Glitter::FT)
            effect->data.name_hash = hash_string_murmurhash(&effect->name);
        else
            effect->data.name_hash = hash_string_fnv1a64m(&effect->name);

        for (Glitter::Curve*& c : effect->animation.curves)
            if (c)
                c->Recalculate(eg->type);

        for (Glitter::Emitter*& j : effect->emitters) {
            if (!j)
                continue;

            Glitter::Emitter* emitter = j;
            for (Glitter::Curve*& c : emitter->animation.curves)
                if (c)
                    c->Recalculate(eg->type);

            for (Glitter::Particle*& k : emitter->particles) {
                if (!k)
                    continue;

                Glitter::Particle* particle = k;
                for (Glitter::Curve*& c : particle->animation.curves)
                    if (c)
                        c->Recalculate(eg->type);
            }
        }
    }
}

static void glitter_editor_reset(glitter_editor_struct* glt_edt) {
    glt_edt->selected_type = GLITTER_EDITOR_SELECTED_NONE;
    glt_edt->selected_resource = -1;
    glt_edt->selected_effect = 0;
    glt_edt->selected_emitter = 0;
    glt_edt->selected_particle = 0;
    glitter_editor_curve_editor_reset_state(glt_edt, (Glitter::CurveType)-1);
    glitter_editor_curve_editor_curves_reset(glt_edt);
    glitter_editor_reset_draw();
}

static void glitter_editor_reset_draw() {
    Glitter::glt_particle_manager.scene = 0;
    Glitter::glt_particle_manager.effect = 0;
    Glitter::glt_particle_manager.emitter = 0;
    Glitter::glt_particle_manager.particle = 0;
}

static void glitter_editor_save(glitter_editor_struct* glt_edt) {
    glt_edt->save_popup = glt_edt->effect_group
        && glt_edt->effect_group->type == Glitter::X ? false : true;
}

static void glitter_editor_open_window(glitter_editor_struct* glt_edt) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return;

    wchar_t file[MAX_PATH];
    memset(&file, 0, sizeof(file));

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        char* file_temp = utf16_to_utf8(file);
        memcpy(glt_edt->file, file_temp, utf8_length(file_temp) + 1);
        free(file_temp);
        glt_edt->load_popup = true;
    }
    CoUninitialize();
}

static void glitter_editor_save_window(glitter_editor_struct* glt_edt) {
    if (!glt_edt->effect_group || !glt_edt->scene)
        return;

    if (glt_edt->file[0])
        glitter_editor_save(glt_edt);
    else
        glitter_editor_save_as_window(glt_edt);
}

static void glitter_editor_save_as_window(glitter_editor_struct* glt_edt) {
    if (glt_edt->effect_group
        && glt_edt->effect_group->type == Glitter::X) {
        glt_edt->save_popup = false;
        return;
    }

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        glt_edt->save_popup = false;
        return;
    }

    wchar_t file[MAX_PATH];
    memset(&file, 0, sizeof(file));

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Save";
    ofn.Flags = OFN_NONETWORKBUTTON;
    if (GetSaveFileNameW(&ofn)) {
        char* file_temp = utf16_to_utf8(file);
        memcpy(glt_edt->file, file_temp, utf8_length(file_temp) + 1);
        free(file_temp);
        glitter_editor_save(glt_edt);
    }
    else
        glt_edt->save_popup = false;
    CoUninitialize();
}

static void glitter_editor_load_file(glitter_editor_struct* glt_edt, const char* path, const char* file) {
    if (!glt_edt->load_wait) {
        Glitter::glt_particle_manager.FreeSceneEffect(glt_edt->scene_counter, hash_fnv1a64m_empty, true);
        Glitter::glt_particle_manager.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();

        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->load_wait = true;
    }

    if (!glt_edt->load_data_wait && Glitter::glt_particle_manager.GetEffectGroup(glt_edt->hash))
        return;

    if (!glt_edt->load_data_wait) {
        object_database* obj_db = 0;
        data_struct* ds = &data_list[glt_edt->load_data_type];
        switch (ds->type) {
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39:
            obj_db = &ds->data_ft.obj_db;
            break;
        }
        glt_edt->hash = Glitter::glt_particle_manager.LoadFile(glt_edt->load_glt_type,
            &data_list[glt_edt->load_data_type], file, path, -1.0f, false, obj_db);
        glt_edt->load_data_wait = true;
    }
    else if (Glitter::glt_particle_manager.CheckNoFileReaders(glt_edt->hash)) {
        Glitter::EffectGroup* eg = Glitter::glt_particle_manager.GetEffectGroup(glt_edt->hash);
        if (eg) {
            Glitter::glt_particle_manager.LoadScene(glt_edt->hash, eg->type != Glitter::FT
                ? hash_murmurhash_empty : hash_fnv1a64m_empty, false);
            Glitter::Scene* sc = Glitter::glt_particle_manager.GetScene(glt_edt->hash);
            if (sc) {
                bool lst_not_valid = true;
                enum_or(sc->flags, Glitter::SCENE_EDITOR);
                if (glt_edt->load_glt_type == Glitter::FT) {
                    lst_not_valid = false;
                    for (Glitter::Effect*& i : eg->effects) {
                        if (!i)
                            continue;

                        Glitter::Effect* e = i;
                        if (e->data.name_hash != hash_string_fnv1a64m(&e->name)) {
                            lst_not_valid = true;
                            break;
                        }
                    }
                }
                glt_edt->counter = Glitter::glt_particle_manager.counter;
                glt_edt->effect_group = eg;
                glt_edt->scene = sc;
                glt_edt->scene_counter = sc->counter;
                glt_edt->load_data_popup = lst_not_valid;
            }
            else {
                Glitter::glt_particle_manager.UnloadEffectGroup(eg->hash);
                glt_edt->counter = 0;
                glt_edt->effect_group = 0;
                glt_edt->scene = 0;
                glt_edt->scene_counter = 0;
            }
        }

        glitter_editor_reset_draw();

        Glitter::glt_particle_manager.GetStartEndFrame(&glt_edt->start_frame, &glt_edt->end_frame, glt_edt->hash);
        glt_edt->frame_counter = 0;
        glt_edt->old_frame_counter = 0;
        glt_edt->input_pause = true;
        glt_edt->load = false;
        glt_edt->load_wait = false;
        glt_edt->load_data_wait = false;
    }

    glitter_editor_reset(glt_edt);
}

static void glitter_editor_save_file(glitter_editor_struct* glt_edt, const char* path, const char* file) {
    Glitter::Type glt_type = glt_edt->save_glt_type;

    farc f;
    /*{
        f2_struct st;
        if (glitter_diva_resource_unparse_file(glt_edt->effect_group, &st)) {
            f.add_file();
            farc_file& ff_drs = f.files.back();
            st.write(&ff_drs.data, &ff_drs.size);
            ff_drs.name = std::string(file) + ".drs";
        }
    }*/

    /*{
        f2_struct st;
        if (glitter_diva_effect_unparse_file(glt_type, glt_edt->effect_group, &st)) {
            f.add_file();
            farc_file& ff_dve = f.files.back();
            st.write(&ff_dve.data, &ff_dve.size);
            ff_dve.name = std::string(file) + ".dve";
        }
        else
            return;
    }*/

    /*if (glt_type == Glitter::FT) {
        f2_struct st;
        if (glitter_diva_list_unparse_file(glt_edt->effect_group, &st)) {
            f.add_file();
            farc_file& ff_lst = f.files.back();
            st.write(&ff_lst.data, &ff_lst.size);
            ff_lst.name = std::string(file) + ".lst";
        }
        else
            return;
    }*/

    farc_compress_mode mode;
    if (glt_edt->save_compress)
        mode = glt_type != Glitter::FT ? FARC_COMPRESS_FARC_GZIP_AES : FARC_COMPRESS_FArC;
    else
        mode = FARC_COMPRESS_FArc;

    char* temp = str_utils_add(path, file);
    if (glt_type != Glitter::FT) {
        char* list_temp = str_utils_add(temp, ".glitter.txt");
        stream s;
        io_open(&s, list_temp, "wb");
        if (s.io.stream) {
            Glitter::EffectGroup* eg = glt_edt->effect_group;
            for (Glitter::Effect*& i : eg->effects)
                if (i) {
                    io_write_string(&s, &i->name);
                    io_write_char(&s, '\n');
                }
        }
        io_free(&s);
        free(list_temp);
    }
    f.write(temp, mode, false);
    free(temp);
}

static bool glitter_editor_list_open_window(Glitter::EffectGroup* eg) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH];
    memset(&file, 0, sizeof(file));

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter UTF-8 List Files (*.glitter.txt)\0*.glitter.txt\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        stream s;
        io_open(&s, file, L"rb");
        size_t length = s.length;
        uint8_t* data = force_malloc_s(uint8_t, length);
        io_read(&s, data, length);
        io_free(&s);

        char* buf;
        char** lines;
        size_t count;
        if (str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
            for (size_t i = 0; i < count; i++) {
                char* t = strstr(lines[i], "#(?)");
                if (t)
                    *t = 0;
            }

            uint64_t empty_hash = eg->type != Glitter::FT
                ? hash_murmurhash_empty : hash_fnv1a64m_empty;
            uint64_t* hashes = force_malloc_s(uint64_t, count);
            if (eg->type != Glitter::FT)
                for (size_t i = 0; i < count; i++) {
                    size_t len = utf8_length(lines[i]);
                    hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F));
                }
            else
                for (size_t i = 0; i < count; i++) {
                    size_t len = utf8_length(lines[i]);
                    hashes[i] = hash_fnv1a64m(lines[i], min(len, 0x7F));
                }

            ret = true;
            for (Glitter::Effect*& i : eg->effects) {
                if (!i)
                    continue;

                Glitter::Effect* e = i;
                e->name.clear();
                if (e->data.name_hash == empty_hash)
                    continue;

                size_t j;
                for (j = 0; j < count; j++)
                    if (e->data.name_hash == hashes[j])
                        break;

                if (j == count) {
                    ret = false;
                    continue;
                }

                e->name = lines[j];
            }

            free(buf);
            free(lines);
            free(hashes);
        }
        free(data);
    }
    CoUninitialize();
    return ret;
}

static bool glitter_editor_resource_import(glitter_editor_struct* glt_edt) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH];
    memset(&file, 0, sizeof(file));

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"DirectDraw Sufrace Files (*.dds)\0*.dds\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        wchar_t* f = str_utils_get_without_extension(file);
        wchar_t* p = str_utils_split_right_get_left(file, L'.');
        dds* d = dds_init();

        uint64_t hash_ft = hash_utf16_fnv1a64m(f);
        uint64_t hash_f2 = hash_utf16_murmurhash(f);

        txp* tex;
        uint32_t index;

        Glitter::EffectGroup* eg = glt_edt->effect_group;
        int32_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        for (int32_t i = 0; i < rc; i++)
            if (rh[i] == hash_ft || rh[i] == hash_f2)
                goto DDSEnd;

        dds_read(d, p);
        if (d->width == 0 || d->height == 0 || d->mipmaps_count == 0 || vector_old_length(d->data) < 1)
            goto DDSEnd;

        eg->resources_tex.textures.push_back({});
        tex = &eg->resources_tex.textures.back();
        tex->array_size = d->has_cube_map ? 6 : 1;
        tex->has_cube_map = d->has_cube_map;
        tex->mipmaps_count = d->mipmaps_count;

        tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
        index = 0;
        do
            for (uint32_t i = 0; i < tex->mipmaps_count; i++) {
                txp_mipmap tex_mip;
                memset(&tex_mip, 0, sizeof(txp_mipmap));
                tex_mip.width = max(d->width >> i, 1);
                tex_mip.height = max(d->height >> i, 1);
                tex_mip.format = d->format;

                uint32_t size = txp::get_size(tex_mip.format, tex_mip.width, tex_mip.height);
                tex_mip.size = size;
                tex_mip.data.resize(size);
                memcpy(tex_mip.data.data(), d->data.begin[index], size);
                tex->mipmaps.push_back(tex_mip);
                index++;
            }
        while (index / tex->mipmaps_count < tex->array_size);

        eg->resources_count++;
        if (eg->type == Glitter::FT)
            eg->resource_hashes.push_back(hash_ft);
        else
            eg->resource_hashes.push_back(hash_f2);
        ret = true;
    DDSEnd:
        dds_dispose(d);
        free(f);
        free(p);
    }
    CoUninitialize();
    return ret;
}

static bool glitter_editor_resource_export(glitter_editor_struct* glt_edt) {
    int32_t sel_rsrc = glt_edt->selected_resource;

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH];
    memset(&file, 0, sizeof(file));

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"DirectDraw Sufrace Files (*.dds)\0*.dds\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Save";
    ofn.Flags = OFN_NONETWORKBUTTON;
    if (GetSaveFileNameW(&ofn)) {
        wchar_t* p = str_utils_split_right_get_left(file, L'.');
        dds* d = dds_init();

        Glitter::EffectGroup* eg = glt_edt->effect_group;
        txp& tex = eg->resources_tex.textures[sel_rsrc];

        txp_format format = tex.mipmaps[0].format;
        uint32_t width = tex.mipmaps[0].width;
        uint32_t height = tex.mipmaps[0].height;

        d->format = format;
        d->width = width;
        d->height = height;
        d->mipmaps_count = tex.mipmaps_count;
        d->has_cube_map = tex.has_cube_map;
        d->data = vector_old_ptr_empty(void);

        vector_old_ptr_void_reserve(&d->data, (tex.has_cube_map ? 6LL : 1LL) * tex.mipmaps_count);
        uint32_t index = 0;
        do
            for (uint32_t i = 0; i < tex.mipmaps_count; i++) {
                uint32_t size = txp::get_size(format, max(width >> i, 1), max(height >> i, 1));
                void* data = force_malloc(size);
                memcpy(data, tex.mipmaps[index].data.data(), size);
                vector_old_ptr_void_push_back(&d->data, &data);
                index++;
            }
        while (index / tex.mipmaps_count < tex.array_size);
        dds_write(d, p);
        ret = true;
        dds_dispose(d);
        free(p);
    }
    CoUninitialize();
    return ret;
}

static void glitter_editor_test_window(glitter_editor_struct* glt_edt, class_data* data) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;

    float_t win_x;
    float_t win_y;

    float_t x;
    float_t y;

    float_t w = 280.0f;
    float_t h = 326.0f;

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Glitter Editor Test Window", &glt_edt->test, window_flags)) {
        data->imgui_focus |= ImGui::IsWindowFocused();
        return;
    }

    if (imguiButton("Reset Camera (R)"))
        input_reset = true;

    w = imguiGetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Play (T)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_T))
            glt_edt->input_play = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reload (Y)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_Y))
            glt_edt->input_reload = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Pause (F)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_F))
            glt_edt->input_pause = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reset (G)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_G))
            glt_edt->input_reset = true;
        ImGui::EndTable();
    }

    ImGui::Separator();

    glt_edt->old_frame_counter = glt_edt->frame_counter;
    imguiColumnSliderFloat("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, "%.0f", 0, true);
    glt_edt->input_pause_temp = imgui_is_item_activated;

    ImGui::Text("Start/End Frame: %d/%d", glt_edt->start_frame, glt_edt->end_frame);

    imguiColumnSliderFloat("Emission", &Glitter::glt_particle_manager.emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    ImGui::Separator();

    imguiCheckbox("Show Grid", &draw_grid_3d);

    imguiCheckbox("Draw All", &Glitter::glt_particle_manager.draw_all);

    imguiDisableElementPush(Glitter::glt_particle_manager.draw_all && eg && eg->type == Glitter::X);
    imguiCheckbox("Draw All Mesh", &Glitter::glt_particle_manager.draw_all_mesh);
    imguiDisableElementPop(Glitter::glt_particle_manager.draw_all && eg && eg->type == Glitter::X);

    ImGui::Separator();

    imguiCheckboxFlags("Draw Wireframe",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
    imguiCheckboxFlags("Draw Selected",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
    imguiCheckboxFlags("No Draw",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_NO_DRAW);

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();

    win_x = min((float_t)width, 240.0f);
    win_y = min((float_t)height, 96.0f);

    x = 0.0f;
    y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMouseInputs;
    window_flags |= ImGuiWindowFlags_NoNavInputs;
    window_flags |= ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleColor(ImGuiCol_Border, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
    if (ImGui::Begin("Glitter Test Sub##Data Test", 0, window_flags)) {
        size_t ctrl;
        size_t disp;
        float_t frame;
        int32_t life_time;

        life_time = 0;
        frame = Glitter::glt_particle_manager.GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
        ImGui::Text("%.0f - %.0f/%d", max(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

        ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_QUAD);
        disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_QUAD);
        ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_LOCUS);
        disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_LOCUS);
        ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_LINE);
        disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_LINE);
        ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_MESH);
        disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_MESH);
        ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
        ImGui::End();
    }
    ImGui::PopStyleColor(2);
}

static void glitter_editor_effects(glitter_editor_struct* glt_edt) {
    char buf[0x100];

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    ImGui::PushID(eg);
    ssize_t i_idx = 1;
    for (Glitter::Effect*& i : eg->effects) {
        if (!i) {
            i_idx++;
            continue;
        }

        Glitter::Effect* effect = i;
        tree_node_flags = tree_node_base_flags;
        if (effect == glt_edt->selected_effect && !glt_edt->selected_emitter
            && !glt_edt->selected_particle)
            tree_node_flags |= ImGuiTreeNodeFlags_Selected;

        const char* eff_str;
        if (effect->data.ext_anim) {
            if (effect->data.ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM)
                eff_str = "%s (%08X) [C]";
            else
                eff_str = "%s (%08X) [O]";
        }
        else if (effect->data.flags & Glitter::EFFECT_LOCAL)
            eff_str = "%s (%08X) [L]";
        else
            eff_str = "%s (%08X)";

        ImGui::PushID(effect);
        if (ImGui::TreeNodeEx("effect", tree_node_flags, eff_str,
            effect->name.c_str(), (uint32_t)((size_t)effect * hash_fnv1a64m_empty))) {
            glitter_editor_effects_context_menu(glt_edt, effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);

            ssize_t j_idx = 1;
            for (Glitter::Emitter*& j : effect->emitters) {
                if (!j) {
                    j_idx++;
                    continue;
                }

                Glitter::Emitter* emitter = j;
                tree_node_flags = tree_node_base_flags;
                if (emitter == glt_edt->selected_emitter && !glt_edt->selected_particle)
                    tree_node_flags |= ImGuiTreeNodeFlags_Selected;

                const char* emit_str;
                if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_TIME) {
                    if (emitter->data.emission_interval <= -0.000001f)
                        emit_str = "Emitter %lld (%08X) [E]";
                    else if (emitter->data.emission_interval >= 0.000001f)
                        emit_str = "Emitter %lld (%08X) [T]";
                    else
                        emit_str = "Emitter %lld (%08X) [S]";
                }
                else if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_DISTANCE)
                    emit_str = "Emitter %lld (%08X) [D]";
                else
                    emit_str = "Emitter %lld (%08X)";

                ImGui::PushID(emitter);
                if (ImGui::TreeNodeEx("emitter", tree_node_flags, emit_str,
                    j_idx, (uint32_t)((size_t)emitter * hash_fnv1a64m_empty))) {
                    glitter_editor_effects_context_menu(glt_edt, effect, emitter, 0,
                        i_idx, j_idx, 0, GLITTER_EDITOR_SELECTED_EMITTER);

                    ssize_t k_idx = 1;
                    for (Glitter::Particle*& k : emitter->particles) {
                        if (!k) {
                            k_idx++;
                            continue;
                        }

                        Glitter::Particle* particle = k;
                        tree_node_flags = tree_node_base_flags;

                        const char* ptcl_str;
                        switch (particle->data.type) {
                        case Glitter::PARTICLE_QUAD:
                            ptcl_str = "Particle %lld (%08X) [Quad]";
                            break;
                        case Glitter::PARTICLE_LINE:
                            ptcl_str = "Particle %lld (%08X) [Line]";
                            break;
                        case Glitter::PARTICLE_LOCUS:
                            ptcl_str = "Particle %lld (%08X) [Locus]";
                            break;
                        case Glitter::PARTICLE_MESH:
                            ptcl_str = "Particle %lld (%08X) [Mesh]";
                            break;
                        default:
                            ptcl_str = "Particle %lld (%08X)";
                            break;
                        }

                        ImGui::PushID(particle);
                        snprintf(buf, sizeof(buf), ptcl_str,
                            k_idx, (uint32_t)((size_t)particle * hash_fnv1a64m_empty));
                        ImGui::Selectable(buf, particle == glt_edt->selected_particle,
                            0, { 0.0f, 0.0f });
                        glitter_editor_effects_context_menu(glt_edt, effect, emitter, particle,
                            i_idx, j_idx, k_idx, GLITTER_EDITOR_SELECTED_PARTICLE);
                        ImGui::PopID();
                        k_idx++;
                    }
                    ImGui::TreePop();
                }
                else
                    glitter_editor_effects_context_menu(glt_edt, effect, emitter, 0,
                        i_idx, j_idx, 0, GLITTER_EDITOR_SELECTED_EMITTER);
                ImGui::PopID();
                j_idx++;
            }
            ImGui::TreePop();
        }
        else
            glitter_editor_effects_context_menu(glt_edt, effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);
        ImGui::PopID();
        i_idx++;
    }
    ImGui::PopID();
}

static void glitter_editor_effects_context_menu(glitter_editor_struct* glt_edt,
    Glitter::Effect* effect, Glitter::Emitter* emitter, Glitter::Particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type) {
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;

    bool focused = ImGui::IsItemFocused();

    ImGuiWindowFlags window_flags;
    bool close;

    static void* selected;
    if (type == GLITTER_EDITOR_SELECTED_NONE) {
        if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("effects popup", ImGuiPopupFlags_MouseButtonRight);
            selected = 0;
        }
        else if (selected != 0)
            goto End;
    }
    else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        switch (type) {
        case GLITTER_EDITOR_SELECTED_EFFECT:
            selected = effect;
            break;
        case GLITTER_EDITOR_SELECTED_EMITTER:
            selected = emitter;
            break;
        case GLITTER_EDITOR_SELECTED_PARTICLE:
            selected = particle;
            break;
        default:
            goto End;
        }
        ImGui::OpenPopup("effects popup", ImGuiPopupFlags_MouseButtonRight);
    }
    else
        switch (type) {
        case GLITTER_EDITOR_SELECTED_EFFECT:
            if (selected != effect)
                goto End;
            break;
        case GLITTER_EDITOR_SELECTED_EMITTER:
            if (selected != emitter)
                goto End;
            break;
        case GLITTER_EDITOR_SELECTED_PARTICLE:
            if (selected != particle)
                goto End;
            break;
        default:
            goto End;
        }

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::BeginPopup("effects popup", window_flags))
        goto End;

    global_context_menu = false;

    close = false;
    switch (type) {
    case GLITTER_EDITOR_SELECTED_NONE: {
        if (ImGui::MenuItem("Add Effect", 0)) {
            if (!glt_edt->effect_group)
                glt_edt->create_popup = true;
            else
                enum_or(glt_edt->effect_flags, GLITTER_EDITOR_ADD);
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_EFFECT: {
        if (!glt_edt->effect_group)
            break;

        const char* eff_str;
        if (effect->data.ext_anim) {
            if (effect->data.ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM)
                eff_str = "%s (%08X) [C]";
            else
                eff_str = "%s (%08X) [O]";
        }
        else if (effect->data.flags & Glitter::EFFECT_LOCAL)
            eff_str = "%s (%08X) [L]";
        else
            eff_str = "%s (%08X)";

        Glitter::EffectGroup* eg = glt_edt->effect_group;
        ImGui::Text(eff_str, effect->name.c_str(), (uint32_t)((size_t)effect * hash_fnv1a64m_empty));
        ImGui::Separator();
        if (ImGui::MenuItem("Duplicate Effect")) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Delete Effect")) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Move Effect Up", 0, false, i_idx > 1)) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Move Effect Down", 0, false, i_idx < (ssize_t)eg->effects.size())) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_effect = effect;
            close = true;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Add Emitter")) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_ADD);
            glt_edt->selected_effect = effect;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_EMITTER: {
        if (!glt_edt->selected_effect)
            break;

        const char* emit_str;
        if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_TIME) {
            if (emitter->data.emission_interval <= -0.000001f)
                emit_str = "Emitter %lld (%08X) [E]";
            else if (emitter->data.emission_interval >= 0.000001f)
                emit_str = "Emitter %lld (%08X) [T]";
            else
                emit_str = "Emitter %lld (%08X) [S]";
        }
        else if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_DISTANCE)
            emit_str = "Emitter %lld (%08X) [D]";
        else
            emit_str = "Emitter %lld (%08X)";

        ImGui::Text(emit_str, j_idx, (size_t)emitter * hash_fnv1a64m_empty);
        ImGui::Separator();
        if (ImGui::MenuItem("Duplicate Emitter")) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Delete Emitter")) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Move Emitter Up", 0, false, j_idx > 1)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Move Emitter Down", 0, false,
            j_idx < (ssize_t)sel_efct->emitters.size())) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Add Particle")) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_ADD);
            glt_edt->selected_emitter = emitter;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_PARTICLE: {
        if (!glt_edt->selected_emitter)
            break;

        const char* ptcl_str;
        switch (particle->data.type) {
        case Glitter::PARTICLE_QUAD:
            ptcl_str = "Particle %lld (%08X) [Quad]";
            break;
        case Glitter::PARTICLE_LINE:
            ptcl_str = "Particle %lld (%08X) [Line]";
            break;
        case Glitter::PARTICLE_LOCUS:
            ptcl_str = "Particle %lld (%08X) [Locus]";
            break;
        case Glitter::PARTICLE_MESH:
            ptcl_str = "Particle %lld (%08X) [Mesh]";
            break;
        default:
            ptcl_str = "Particle %lld (%08X)";
            break;
        }

        ImGui::Text(ptcl_str, k_idx, (uint32_t)((size_t)particle * hash_fnv1a64m_empty));
        ImGui::Separator();
        if (ImGui::MenuItem("Duplicate Particle")) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Delete Particle")) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Move Particle Up", 0, false, k_idx > 1)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Move Particle Down", 0, false,
            k_idx < (ssize_t)sel_emit->particles.size())) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_particle = particle;
            close = true;
        }
    } break;
    }

    if (close) {
        ImGui::CloseCurrentPopup();
        selected = 0;
    }
    ImGui::EndPopup();

End:
    if (!focused)
        return;

    switch (type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        if (glt_edt->selected_type != type
            || glt_edt->selected_effect != effect)
            glitter_editor_reset(glt_edt);
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = 0;
        glt_edt->selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        if (glt_edt->selected_type != type
            || glt_edt->selected_emitter != emitter)
            glitter_editor_reset(glt_edt);
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = emitter;
        glt_edt->selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (glt_edt->selected_type != type
            || glt_edt->selected_particle != particle)
            glitter_editor_reset(glt_edt);
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = emitter;
        glt_edt->selected_particle = particle;
        break;
    }
    glt_edt->selected_type = type;
}

static void glitter_editor_resources(glitter_editor_struct* glt_edt) {
    int32_t sel_rsrc = glt_edt->selected_resource;

    char buf[0x100];

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    texture** r = eg->resources;
    int32_t rc = eg->resources_count;
    uint64_t* rh = eg->resource_hashes.data();
    txp* rt = eg->resources_tex.textures.data();
    ImGui::PushID(eg);
    ssize_t i_idx = 1;
    for (int32_t i = 0; i < rc; i++, i_idx++) {
        snprintf(buf, sizeof(buf), "Texture %d (%016llX)", i + 1, rh[i]);
        ImGui::PushID(i);
        ImGui::Selectable(buf, i == sel_rsrc, 0, { 0.0f, 0.0f });
        if (ImGui::IsItemHovered()) {
            txp_mipmap* rtm = rt[i].mipmaps.data();
            float_t aspect = (float_t)rtm->width / (float_t)rtm->height;

            ImVec2 size = { 192.0f, 192.0f };
            if (aspect > 1.0f)
                size.y /= aspect;
            else if (aspect < 1.0f)
                size.x *= aspect;

            ImGui::BeginTooltip();
            ImGui::Text("Tex Size: %dx%d", rtm->width, rtm->height);
            ImGui::Image((void*)(size_t)r[i]->tex, size,
                { 0, 0 }, { 1, 1 }, tint_col, border_col);
            ImGui::EndTooltip();
        }
        glitter_editor_resources_context_menu(glt_edt, i, i_idx, false);
        ImGui::PopID();
    }
    ImGui::PopID();
}

static void glitter_editor_resources_context_menu(glitter_editor_struct* glt_edt,
    int32_t resource, ssize_t i_idx, bool selected_none) {
    bool focused = ImGui::IsItemFocused();

    ImGuiWindowFlags window_flags;
    bool close;

    static size_t selected;
    if (selected_none) {
        if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("resources popup", ImGuiPopupFlags_MouseButtonRight);
            selected = -1;
        }
        else if (selected != -1)
            goto End;
    }
    else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        selected = resource;
        ImGui::OpenPopup("resources popup", ImGuiPopupFlags_MouseButtonRight);
    }
    else if (selected != resource)
        goto End;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::BeginPopup("resources popup", window_flags))
        goto End;

    global_context_menu = false;

    close = false;
    if (selected_none) {
        if (ImGui::MenuItem("Import Resource", 0)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_IMPORT);
            close = true;
        }
    }
    else {
        size_t i = resource;
        Glitter::EffectGroup* eg = glt_edt->effect_group;
        ssize_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        ImGui::Text("Texture %lld (%016llX)", i + 1, rh[i]);
        ImGui::Separator();
        if (ImGui::MenuItem("Export Resource")) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_EXPORT);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (ImGui::MenuItem("Delete Resource")) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (ImGui::MenuItem("Move Resource Up", 0, false, i_idx > 1)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (ImGui::MenuItem("Move Resource Down", 0, false,
            i_idx < rc)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_resource = resource;
            close = true;
        }
    }

    if (close) {
        ImGui::CloseCurrentPopup();
        selected = 0;
    }
    ImGui::EndPopup();

End:
    if (!focused)
        return;

    glt_edt->selected_resource = resource;
}

static void glitter_editor_play_manager(glitter_editor_struct* glt_edt) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;

    if (imguiButton("Reset Camera (R)"))
        input_reset = true;

    float_t w = imguiGetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Play (T)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_T))
            glt_edt->input_play = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reload (Y)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_Y))
            glt_edt->input_reload = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Pause (F)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_F))
            glt_edt->input_pause = true;

        ImGui::TableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reset (G)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_G))
            glt_edt->input_reset = true;
        ImGui::EndTable();
    }

    ImGui::Separator();

    glt_edt->old_frame_counter = glt_edt->frame_counter;
    imguiColumnSliderFloat("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, "%.0f", 0, true);
    glt_edt->input_pause_temp = imgui_is_item_activated;

    ImGui::Text("Start/End Frame: %d/%d", glt_edt->start_frame, glt_edt->end_frame);

    ImGui::Separator();

    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    life_time = 0;
    frame = Glitter::glt_particle_manager.GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
    ImGui::Text("%.0f - %.0f/%d", max(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

    ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_QUAD);
    disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_QUAD);
    ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_LOCUS);
    disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_LOCUS);
    ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_LINE);
    disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_LINE);
    ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager.GetCtrlCount(Glitter::PARTICLE_MESH);
    disp = Glitter::glt_particle_manager.GetDispCount(Glitter::PARTICLE_MESH);
    ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);

    ImGui::Separator();

    imguiColumnSliderFloat("Emission", &Glitter::glt_particle_manager.emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    ImGui::Separator();

    imguiCheckbox("Grid", &draw_grid_3d);

    imguiCheckbox("Draw All", &Glitter::glt_particle_manager.draw_all);

    imguiDisableElementPush(Glitter::glt_particle_manager.draw_all && eg && eg->type == Glitter::X);
    imguiCheckbox("Draw All Mesh", &Glitter::glt_particle_manager.draw_all_mesh);
    imguiDisableElementPop(Glitter::glt_particle_manager.draw_all && eg && eg->type == Glitter::X);
}

static void glitter_editor_property(glitter_editor_struct* glt_edt, class_data* data) {
    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        glitter_editor_property_effect(glt_edt, data);
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        glitter_editor_property_emitter(glt_edt, data);
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        glitter_editor_property_particle(glt_edt, data);
        break;
    }
}

static void glitter_editor_property_effect(glitter_editor_struct* glt_edt, class_data* data) {
    imguiSetColumnSpace(2.0f / 5.0f);

    uint64_t hash_before = hash_fnv1a64m(glt_edt->selected_effect, sizeof(Glitter::Effect));

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Effect* effect = glt_edt->selected_effect;
    bool changed = false;

    Glitter::CurveTypeFlags flags = Glitter::effect_curve_flags;
    for (int32_t i = Glitter::CURVE_TRANSLATION_X; i <= Glitter::CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (Glitter::Curve*& j : effect->animation.curves)
            if (j && j->type == i)
                enum_and(flags, ~(1 << i));

    std::string name;
    name.reserve(0x200);
    name.assign(effect->name.c_str());
    if (imguiColumnInputText("Name", (char*)name.c_str(), name.capacity(), 0, 0, 0)) {
        effect->data.name_hash = eg->type != Glitter::FT
            ? hash_string_murmurhash(&name) : hash_string_fnv1a64m(&name);
        effect->name.assign(name.c_str());
        changed = true;
    }

    if (imguiColumnDragInt("Appear Time",
        &effect->data.appear_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Life Time",
        &effect->data.life_time, 1.0f, 0x0000, 0x7FFF - effect->data.appear_time, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Start Time",
        &effect->data.start_time, 1.0f, 0x0000, 0x7FFF - effect->data.appear_time, "%d", 0))
        changed = true;

    ImGui::Separator();

    if (imguiColumnDragVec3Flag("Translation",
        &effect->translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    vec3 rotation;
    vec3_mult_scalar(effect->rotation, RAD_TO_DEG_FLOAT, rotation);
    if (imguiColumnDragVec3Flag("Rotation",
        &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, effect->rotation);
        changed = true;
    }

    if (imguiColumnDragVec3Flag("Scale",
        &effect->scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    ImGui::Separator();

    if (imguiCheckboxFlags("Loop",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_LOOP))
        changed = true;

    if (imguiCheckboxFlags("Draw as Transparent",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_ALPHA))
        changed = true;

    if (imguiCheckboxFlags("Fog",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_FOG))
        changed = true;

    if (imguiCheckboxFlags("Fog Height",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_FOG_HEIGHT))
        changed = true;

    if (imguiCheckboxFlags("Use External Emission",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_EMISSION))
        changed = true;

    if (eg->type == Glitter::X)
        if (imguiCheckboxFlags("Use Seed",
            (uint32_t*)&effect->data.flags,
            Glitter::EFFECT_USE_SEED))
            changed = true;

    if (effect->data.flags & Glitter::EFFECT_FOG && effect->data.flags & Glitter::EFFECT_FOG_HEIGHT) {
        enum_and(effect->data.flags, ~Glitter::EFFECT_FOG_HEIGHT);
        changed = true;
    }

    if (effect->data.flags & Glitter::EFFECT_FOG_HEIGHT && effect->data.flags & Glitter::EFFECT_FOG) {
        enum_and(effect->data.flags, ~Glitter::EFFECT_FOG);
        changed = true;
    }

    if (eg->type != Glitter::X) {
        bool set_emission = effect->version == 7;
        if (imguiCheckbox("Set Emission",
            &set_emission)) {
            effect->version = set_emission ? 7 : 6;
            changed = true;
        }

        if (effect->version == 7 && imguiColumnDragFloat("Emission",
            &effect->data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
    }
    else {
        if (effect->data.flags & Glitter::EFFECT_USE_SEED
            && imguiColumnInputScalar("Seed", ImGuiDataType_U32, &effect->data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (imguiColumnDragFloat("Emission",
            &effect->data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
    }

    int32_t type;
    if (effect->data.flags & Glitter::EFFECT_LOCAL)
        type = 1;
    else if (effect->data.ext_anim)
        type = effect->data.ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM ? 2 : 3;
    else
        type = 0;

    if (imguiColumnComboBox("Type", Glitter::effect_type_name,
        4, &type, 0, false, &data->imgui_focus)) {
        if (type == 1) {
            enum_or(effect->data.flags, Glitter::EFFECT_LOCAL);
            free(effect->data.ext_anim);
        }
        else {
            enum_and(effect->data.flags, ~Glitter::EFFECT_LOCAL);
            if (type == 2 || type == 3) {
                if (!effect->data.ext_anim)
                    effect->data.ext_anim = force_malloc_s(Glitter::Effect::ExtAnim, 1);
                if (type == 2)
                    enum_or(effect->data.ext_anim->flags, Glitter::EFFECT_EXT_ANIM_CHARA_ANIM);
                else
                    enum_and(effect->data.ext_anim->flags, ~Glitter::EFFECT_EXT_ANIM_CHARA_ANIM);
            }
            else
                free(effect->data.ext_anim);
        }
        changed = true;
    }

    if (~effect->data.flags & Glitter::EFFECT_LOCAL && effect->data.ext_anim) {
        ImGui::Separator();

        bool changed = false;
        if (eg->type == Glitter::X) {
            uint64_t hash_before = hash_fnv1a64m(effect->data.ext_anim_x,
                sizeof(Glitter::Effect::ExtAnimX));

            Glitter::Effect::ExtAnimX* ext_anim = effect->data.ext_anim_x;
            if (ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = 4;
                if (imguiColumnComboBox("Chara Index", Glitter::effect_ext_anim_index_name,
                    max_chara, &ext_anim->chara_index, 0, false, &data->imgui_focus))
                    changed = true;

                int32_t node_index = ext_anim->node_index;
                if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", Glitter::effect_ext_anim_node_index_name,
                    Glitter::EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &data->imgui_focus)) {
                    node_index--;
                    if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim->node_index != node_index) {
                        ext_anim->node_index = (Glitter::EffectExtAnimCharaNode)node_index;
                        changed = true;
                    }
                }
            }
            else {
                uint64_t object_hash = ext_anim->object_hash;
                if (glitter_editor_hash_input(glt_edt, "Object Hash", &object_hash)) {
                    ext_anim->object_hash = (uint32_t)object_hash;
                    changed = true;
                }

                imguiColumnInputScalar("Instance ID", ImGuiDataType_U32, &ext_anim->instance_id,
                    0, 0, "%d", ImGuiInputTextFlags_CharsDecimal);

                uint64_t file_name_hash = ext_anim->file_name_hash;
                if (glitter_editor_hash_input(glt_edt, "File Name Hash", &file_name_hash)) {
                    ext_anim->file_name_hash = (uint32_t)file_name_hash;
                    changed = true;
                }

                const size_t mesh_name_size = sizeof(ext_anim->mesh_name);
                char mesh_name[sizeof(ext_anim->mesh_name)];
                memcpy(mesh_name, ext_anim->mesh_name, mesh_name_size);
                if (imguiColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim->mesh_name, mesh_name, mesh_name_size);
                    changed = true;
                }
            }

            if (imguiCheckboxFlags("Trans Only",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_TRANS_ONLY))
                changed = true;

            if (imguiCheckboxFlags("No Trans X",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_X))
                changed = true;

            if (imguiCheckboxFlags("No Trans Y",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_Y))
                changed = true;

            if (imguiCheckboxFlags("No Trans Z",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_Z))
                changed = true;

            if (imguiCheckboxFlags("Draw Only when Chara Visible",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA))
                changed = true;

            if (imguiCheckboxFlags("Get Then Update",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_GET_THEN_UPDATE))
                changed = true;

            uint64_t hash_after = hash_fnv1a64m(effect->data.ext_anim_x,
                sizeof(Glitter::Effect::ExtAnimX));
            if (hash_before != hash_after && changed)
                glt_edt->input_reload = true;
        }
        else {
            uint64_t hash_before = hash_fnv1a64m(effect->data.ext_anim,
                sizeof(Glitter::Effect::ExtAnim));

            Glitter::Effect::ExtAnim* ext_anim = effect->data.ext_anim;
            if (ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = eg->type == Glitter::FT ? 6 : 3;
                if (imguiColumnComboBox("Chara Index", Glitter::effect_ext_anim_index_name,
                    max_chara, &ext_anim->chara_index, 0, false, &data->imgui_focus))
                    changed = true;

                int32_t node_index = ext_anim->node_index;
                if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", Glitter::effect_ext_anim_node_index_name,
                    Glitter::EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &data->imgui_focus)) {
                    node_index--;
                    if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim->node_index != node_index) {
                        ext_anim->node_index = (Glitter::EffectExtAnimCharaNode)node_index;
                        changed = true;
                    }
                }
            }
            else {
                if (glitter_editor_hash_input(glt_edt, "Object Hash", &ext_anim->object_hash)) {
                    ext_anim->object = Glitter::Effect::ExtAnim::GetObjectInfo(ext_anim->object_hash,
                        &data_list[glt_edt->load_data_type].data_ft.obj_db);
                    changed = true;
                }

                const size_t mesh_name_size = sizeof(ext_anim->mesh_name);
                char mesh_name[sizeof(ext_anim->mesh_name)];
                memcpy(mesh_name, ext_anim->mesh_name, mesh_name_size);
                if (imguiColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim->mesh_name, mesh_name, mesh_name_size);
                    changed = true;
                }
            }

            if (imguiCheckboxFlags("Trans Only",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_TRANS_ONLY))
                changed = true;

            uint64_t hash_after = hash_fnv1a64m(effect->data.ext_anim, sizeof(Glitter::Effect::ExtAnim));
            if (hash_before != hash_after && changed)
                glt_edt->input_reload = true;
        }
    }

    uint64_t hash_after = hash_fnv1a64m(glt_edt->selected_effect, sizeof(Glitter::Effect));
    if (hash_before != hash_after && changed) {
        for (Glitter::Emitter*& i : effect->emitters) {
            if (!i)
                continue;

            Glitter::Emitter* emitter = i;
            for (Glitter::Particle*& j : emitter->particles) {
                if (!j)
                    continue;

                Glitter::Particle* particle = j;
                Glitter::ParticleFlag flags = particle->data.flags;
                if (effect->data.flags & Glitter::EFFECT_LOCAL)
                    enum_or(flags, Glitter::PARTICLE_LOCAL);
                else
                    enum_and(flags, ~Glitter::PARTICLE_LOCAL);

                if (effect->data.flags & Glitter::EFFECT_EMISSION
                    || particle->data.emission >= Glitter::min_emission)
                    enum_or(flags, Glitter::PARTICLE_EMISSION);
                else
                    enum_and(flags, ~Glitter::PARTICLE_EMISSION);
                particle->data.flags = flags;
            }
        }
        glt_edt->input_reload = true;
    }

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_emitter(glitter_editor_struct* glt_edt, class_data* data) {
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;

    imguiSetColumnSpace(2.0f / 5.0f);

    uint64_t hash_before = hash_fnv1a64m(glt_edt->selected_emitter, sizeof(Glitter::Emitter));

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Emitter* emitter = glt_edt->selected_emitter;
    bool changed = false;

    Glitter::CurveTypeFlags flags = Glitter::emitter_curve_flags;
    for (int32_t i = Glitter::CURVE_TRANSLATION_X; i <= Glitter::CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (Glitter::Curve*& j : emitter->animation.curves)
            if (j && j->type == i)
                enum_and(flags, ~(1 << i));

    ImGui::Text("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        sel_efct->data.appear_time,
        sel_efct->data.life_time,
        sel_efct->data.start_time);

    ImGui::Separator();

    if (imguiColumnDragInt("Start Time",
        &emitter->data.start_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Life Time",
        &emitter->data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Loop Start Time", &emitter->data.loop_start_time,
        1.0f, -0x0001, emitter->data.start_time + emitter->data.life_time, "%d", 0)) {
        if (emitter->data.loop_start_time < emitter->data.start_time)
            emitter->data.loop_start_time = 0;
        changed = emitter->data.loop_start_time != sel_emit->data.loop_start_time;
    }

    if (imguiColumnDragInt("Loop End Time", &emitter->data.loop_end_time,
        1.0f, -0x0001, emitter->data.start_time + emitter->data.life_time, "%d", 0)) {
        if (emitter->data.loop_end_time < 0)
            emitter->data.loop_end_time = -1;
        changed = emitter->data.loop_end_time != sel_emit->data.loop_end_time;
    }

    ImGui::Separator();

    if (imguiColumnDragVec3Flag("Translation",
        &emitter->translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    if (eg->type != Glitter::X) {
        bool draw_z_axis = emitter->data.direction == Glitter::DIRECTION_Z_AXIS;

        vec3 rotation = emitter->rotation;
        if (draw_z_axis)
            rotation.z -= (float_t)M_PI_2;
        vec3_mult_scalar(rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, rotation);
            if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            emitter->rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation;
        vec3_mult_scalar(emitter->rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, emitter->rotation);
            changed = true;
        }
    }

    vec3 rotation_add;
    vec3_mult_scalar(emitter->data.rotation_add, RAD_TO_DEG_FLOAT, rotation_add);
    if (imguiColumnDragVec3Flag("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add, DEG_TO_RAD_FLOAT, emitter->data.rotation_add);
        changed = true;
    }

    if (imguiColumnDragVec3Flag("Scale",
        &emitter->scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    ImGui::Separator();

    if (imguiCheckboxFlags("Loop",
        (uint32_t*)&emitter->data.flags,
        Glitter::EMITTER_LOOP))
        changed = true;

    if (imguiCheckboxFlags("Kill On End",
        (uint32_t*)&emitter->data.flags,
        Glitter::EMITTER_KILL_ON_END))
        changed = true;

    if (eg->type == Glitter::X
        && imguiCheckboxFlags("Use Seed",
            (uint32_t*)&emitter->data.flags,
            Glitter::EMITTER_USE_SEED))
        changed = true;

    ImGui::Separator();

    if (eg->type == Glitter::X) {
        if (emitter->data.flags & Glitter::EMITTER_USE_SEED
            && imguiColumnInputScalar("Seed", ImGuiDataType_U32, &emitter->data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (imguiColumnComboBox("Timer Type", Glitter::emitter_timer_name,
            Glitter::EMITTER_TIMER_BY_DISTANCE,
            (int32_t*)&emitter->data.timer, 0, true, &data->imgui_focus)) {
            float_t emission_interval = emitter->data.emission_interval;
            if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_TIME)
                emission_interval = clamp(floorf(emission_interval), 1.0f, FLT_MAX);
            else
                emission_interval = clamp(emission_interval, 0.0f, FLT_MAX);
            emitter->data.emission_interval = emission_interval;
            changed = true;
        }
    }

    if (eg->type == Glitter::X
        && emitter->data.timer == Glitter::EMITTER_TIMER_BY_DISTANCE) {
        if (imguiColumnDragFloatFlag("Emit Interval",
            &emitter->data.emission_interval, 0.0001f, -1.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_EMISSION_INTERVAL) >> 20))
            changed = true;
    }
    else {
        Glitter::EmitterEmission emission;
        if (emitter->data.emission_interval >= -0.000001f)
            emission = emitter->data.emission_interval <= 0.000001f
            ? Glitter::EMITTER_EMISSION_ON_START : Glitter::EMITTER_EMISSION_ON_TIMER;
        else
            emission = Glitter::EMITTER_EMISSION_ON_END;

        if (imguiColumnComboBox("Emission Type", Glitter::emitter_emission_name,
            Glitter::EMITTER_EMISSION_ON_END,
            (int32_t*)&emission, 0, true, &data->imgui_focus)) {
            float_t emission_interval;
            switch (emission) {
            case Glitter::EMITTER_EMISSION_ON_TIMER:
            default:
                emission_interval = 1.0f;
                break;
            case Glitter::EMITTER_EMISSION_ON_START:
                emission_interval = 0.0f;
                break;
            case Glitter::EMITTER_EMISSION_ON_END:
                emission_interval = -1.0f;
                break;
            }

            if (emission_interval != emitter->data.emission_interval) {
                emitter->data.emission_interval = emission_interval;
                changed = true;
            }
        }

        if (emission == Glitter::EMITTER_EMISSION_ON_TIMER
            && imguiColumnDragFloatFlag("Emit Interval",
                &emitter->data.emission_interval, 1.0f, 1.0f, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat,
                (flags & Glitter::CURVE_TYPE_EMISSION_INTERVAL) >> 20))
                changed = true;
    }

    if (imguiColumnDragFloatFlag("PTC Per Emit",
        &emitter->data.particles_per_emission, 1.0f, 0.0f, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_PARTICLES_PER_EMISSION) >> 21))
        changed = true;

    Glitter::EmitterDirection direction = Glitter::emitter_direction_default;
    Glitter::EmitterDirection prev_direction;
    for (int32_t i = 0; i < Glitter::emitter_direction_types_count; i++)
        if (Glitter::emitter_direction_types[i] == emitter->data.direction) {
            direction = (Glitter::EmitterDirection)i;
            break;
        }
    prev_direction = direction;

    if (imguiColumnComboBox("Direction", Glitter::emitter_direction_name,
        Glitter::EMITTER_DIRECTION_EFFECT_ROTATION,
        (int32_t*)&direction, 0, true, &data->imgui_focus))
        if (Glitter::emitter_direction_types[direction] != emitter->data.direction) {
            changed = true;
            if (eg->type != Glitter::X) {
                bool draw_z_axis =
                    direction == Glitter::EMITTER_DIRECTION_Z_AXIS;
                bool prev_draw_z_axis =
                    prev_direction == Glitter::EMITTER_DIRECTION_Z_AXIS;
                if (draw_z_axis && !prev_draw_z_axis) {
                    if (~flags & Glitter::CURVE_TYPE_ROTATION_Z)
                        emitter->animation.AddValue(eg->type,
                            (float_t)M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                    emitter->rotation.z += (float_t)M_PI_2;
                }
                else if (!draw_z_axis && prev_draw_z_axis) {
                    if (~flags & Glitter::CURVE_TYPE_ROTATION_Z)
                        emitter->animation.AddValue(eg->type,
                            (float_t)-M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                    emitter->rotation.z -= (float_t)M_PI_2;
                }
            }
            emitter->data.direction = Glitter::emitter_direction_types[direction];
            changed = true;
        }

    if (imguiColumnComboBox("Type", Glitter::emitter_name,
        Glitter::EMITTER_POLYGON,
        (int32_t*)&emitter->data.type, 0, true, &data->imgui_focus))
        changed = true;

    ImGui::Separator();

    switch (emitter->data.type) {
    case Glitter::EMITTER_BOX: {
        ImGui::PushID("Box");
        if (imguiColumnDragVec3("Size",
            &emitter->data.box.size, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_CYLINDER: {
        float_t start_angle, end_angle;

        ImGui::PushID("Cylinder");
        if (imguiColumnDragFloat("Radius",
            &emitter->data.cylinder.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragFloat("Height",
            &emitter->data.cylinder.height, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        start_angle = emitter->data.cylinder.start_angle * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Start Angle",
            &start_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.cylinder.start_angle = start_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        end_angle = emitter->data.cylinder.end_angle * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("End Angle",
            &end_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.cylinder.end_angle = end_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (imguiCheckbox("On Edge",
            &emitter->data.cylinder.on_edge))
            changed = true;

        if (imguiColumnComboBox("Direction",
            Glitter::emitter_emission_direction_name,
            Glitter::EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter->data.cylinder.direction, 0, true, &data->imgui_focus))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_SPHERE: {
        float_t latitude, longitude;

        ImGui::PushID("Sphere");
        if (imguiColumnDragFloat("Radius",
            &emitter->data.sphere.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        latitude = emitter->data.sphere.latitude * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Latitude",
            &latitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.sphere.latitude = latitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        longitude = emitter->data.sphere.longitude * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Longitude",
            &longitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.sphere.longitude = longitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (imguiCheckbox("On Edge",
            &emitter->data.sphere.on_edge))
            changed = true;

        if (imguiColumnComboBox("Direction",
            Glitter::emitter_emission_direction_name,
            Glitter::EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter->data.sphere.direction, 0, true, &data->imgui_focus))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_POLYGON: {
        ImGui::PushID("Polygon");
        if (imguiColumnDragFloat("Scale",
            &emitter->data.polygon.size, 0.0001f, 0.0f, FLT_MAX,
            "%g", ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragInt("Count",
            &emitter->data.polygon.count, 1.0f, 0, 0x7FFFFF, "%d", 0))
            changed = true;

        if (eg->type == Glitter::X)
            if (imguiColumnComboBox("Direction",
                Glitter::emitter_emission_direction_name,
                Glitter::EMITTER_EMISSION_DIRECTION_INWARD,
                (int32_t*)&emitter->data.cylinder.direction, 0, true, &data->imgui_focus))
                changed = true;
        ImGui::PopID();
    } break;
    }

    uint64_t hash_after = hash_fnv1a64m(glt_edt->selected_emitter, sizeof(Glitter::Emitter));
    if (hash_before != hash_after && changed)
        glt_edt->input_reload = true;

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_particle(glitter_editor_struct* glt_edt, class_data* data) {
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;

    imguiSetColumnSpace(2.0f / 5.0f);

    uint64_t hash_before = hash_fnv1a64m(glt_edt->selected_particle, sizeof(Glitter::Particle));

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Particle* particle = glt_edt->selected_particle;
    bool changed = false;

    Glitter::CurveTypeFlags flags = (Glitter::CurveTypeFlags)0;
    if (eg->type == Glitter::X)
        flags = Glitter::particle_x_curve_flags;
    else
        flags = Glitter::particle_curve_flags;

    if (particle->data.type != Glitter::PARTICLE_MESH) {
        enum_and(flags, ~Glitter::CURVE_TYPE_UV_SCROLL_2ND);
        if (particle->data.draw_type != Glitter::DIRECTION_PARTICLE_ROTATION)
            enum_and(flags, ~(Glitter::CURVE_TYPE_ROTATION_X | Glitter::CURVE_TYPE_ROTATION_Y));
    }

    if (particle->data.sub_flags & Glitter::PARTICLE_SUB_USE_CURVE)
        for (int32_t i = Glitter::CURVE_TRANSLATION_X; i <= Glitter::CURVE_V_SCROLL_ALPHA_2ND; i++)
            for (Glitter::Curve*& j : particle->animation.curves)
                if (j && j->type == i)
                    enum_and(flags, ~(1 << i));

    ImGui::Text("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        sel_efct->data.appear_time,
        sel_efct->data.life_time,
        sel_efct->data.start_time);

    ImGui::Text("Parent Emitter:\nStart Time: %d; Life Time: %d\nLoop Start Time: %d; Loop End Time: %d",
        sel_emit->data.start_time,
        sel_emit->data.life_time,
        sel_emit->data.loop_start_time,
        sel_emit->data.loop_end_time);

    ImGui::Separator();

    bool flags_changed = false;
    if (imguiCheckboxFlags("Loop",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_LOOP))
        flags_changed = true;

    if (imguiCheckboxFlags("Emitter Local",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_EMITTER_LOCAL))
        flags_changed = true;

    if (imguiCheckboxFlags("Scale Y by X",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_SCALE_Y_BY_X))
        flags_changed = true;

    if (imguiCheckboxFlags("Rebound Plane",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_REBOUND_PLANE))
        flags_changed = true;

    if (imguiCheckboxFlags("Rotate by Emitter",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_ROTATE_BY_EMITTER))
        flags_changed = true;

    if (imguiCheckboxFlags("Scale by Emitter",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_SCALE))
        flags_changed = true;

    if (imguiCheckboxFlags("Texture Mask##Flag",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_TEXTURE_MASK))
        flags_changed = true;

    if (eg->type == Glitter::X) {
        if (imguiCheckboxFlags("Disable Depth Test",
            (uint32_t*)&particle->data.flags,
            Glitter::PARTICLE_DEPTH_TEST))
            flags_changed = true;

        if (particle->data.type == Glitter::PARTICLE_LOCUS
            && imguiCheckboxFlags("Rotate Locus",
                (uint32_t*)&particle->data.flags,
                Glitter::PARTICLE_ROTATE_LOCUS))
                flags_changed = true;
    }

    if (flags_changed)
        changed = true;

    ImGui::Separator();

    if (eg->type == Glitter::X) {
        vec2i life_time;
        life_time.x = particle->data.life_time;
        life_time.y = particle->data.life_time_random;
        if (imguiColumnDragVec2I("Life Time [R]",
            &life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.life_time = life_time.x;
            particle->data.life_time_random = life_time.y;
            changed = true;
        }

        vec2i fade_in;
        fade_in.x = particle->data.fade_in;
        fade_in.y = particle->data.fade_in_random;
        if (imguiColumnDragVec2I("Fade In [R]",
            &fade_in, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.fade_in = fade_in.x;
            particle->data.fade_in_random = fade_in.y;
            changed = true;
        }

        vec2i fade_out;
        fade_out.x = particle->data.fade_out;
        fade_out.y = particle->data.fade_out_random;
        if (imguiColumnDragVec2I("Fade Out [R]",
            &fade_out, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.fade_out = fade_out.x;
            particle->data.fade_out_random = fade_out.y;
            changed = true;
        }

        if (imguiColumnComboBox("Type", Glitter::particle_name,
            Glitter::PARTICLE_MESH,
            (int32_t*)&particle->data.type, 0, true, &data->imgui_focus))
            changed = true;
    }
    else {
        if (imguiColumnDragInt("Life Time",
            &particle->data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
            changed = true;

        if (imguiColumnComboBox("Type", Glitter::particle_name,
            Glitter::PARTICLE_LOCUS,
            (int32_t*)&particle->data.type, 0, true, &data->imgui_focus))
            changed = true;
    }

    if (particle->data.type != Glitter::PARTICLE_MESH
        && imguiColumnComboBox("Pivot", Glitter::pivot_name,
            Glitter::PIVOT_BOTTOM_RIGHT,
            (int32_t*)&particle->data.pivot, 0, true, &data->imgui_focus))
        changed = true;

    if (particle->data.type == Glitter::PARTICLE_QUAD || particle->data.type == Glitter::PARTICLE_MESH) {
        Glitter::ParticleDrawType draw_type = Glitter::particle_draw_type_default;
        Glitter::ParticleDrawType prev_draw_type;
        for (int32_t i = 0; i < Glitter::particle_draw_types_count; i++)
            if (Glitter::particle_draw_types[i] == particle->data.draw_type) {
                draw_type = (Glitter::ParticleDrawType)i;
                break;
            }
        prev_draw_type = draw_type;

        if (imguiColumnComboBox("Draw Type", Glitter::particle_draw_type_name,
            Glitter::PARTICLE_DRAW_TYPE_EMIT_POSITION,
            (int32_t*)&draw_type, 0, true, &data->imgui_focus))
            if (Glitter::particle_draw_types[draw_type] != particle->data.draw_type) {
                if (eg->type != Glitter::X) {
                    bool draw_prev_pos =
                        draw_type == Glitter::PARTICLE_DRAW_TYPE_PREV_POSITION
                        || draw_type == Glitter::PARTICLE_DRAW_TYPE_PREV_POSITION_DUP;
                    bool prev_draw_prev_pos =
                        prev_draw_type == Glitter::PARTICLE_DRAW_TYPE_PREV_POSITION
                        || prev_draw_type == Glitter::PARTICLE_DRAW_TYPE_PREV_POSITION_DUP;
                    bool draw_z_axis =
                        draw_type == Glitter::PARTICLE_DRAW_TYPE_Z_AXIS;
                    bool prev_draw_z_axis =
                        prev_draw_type == Glitter::PARTICLE_DRAW_TYPE_Z_AXIS;
                    if (draw_prev_pos && !prev_draw_prev_pos)
                        particle->data.rotation.z += (float_t)M_PI;
                    else if (!draw_prev_pos && prev_draw_prev_pos)
                        particle->data.rotation.z -= (float_t)M_PI;
                    else if (draw_z_axis && !prev_draw_z_axis) {
                        if (~flags & Glitter::CURVE_TYPE_ROTATION_Z)
                            particle->animation.AddValue(eg->type,
                                (float_t)M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                        particle->data.rotation.z += (float_t)M_PI_2;
                    }
                    else if (!draw_z_axis && prev_draw_z_axis) {
                        if (~flags & Glitter::CURVE_TYPE_ROTATION_Z)
                            particle->animation.AddValue(eg->type,
                                (float_t)-M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                        particle->data.rotation.z -= (float_t)M_PI_2;
                    }
                }
                particle->data.draw_type = Glitter::particle_draw_types[draw_type];
                changed = true;
            }
    }

    if (particle->data.type == Glitter::PARTICLE_QUAD)
        if (imguiColumnDragFloat("Z Offset",
            &particle->data.z_offset, 0.0001f, -FLT_MAX, FLT_MAX,
            "%g", ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

    ImGui::Separator();

    if (eg->type != Glitter::X) {
        bool draw_prev_pos =
            particle->data.draw_type == Glitter::DIRECTION_PREV_POSITION
            || particle->data.draw_type == Glitter::DIRECTION_PREV_POSITION_DUP;
        bool draw_z_axis =
            particle->data.draw_type == Glitter::DIRECTION_Z_AXIS;

        vec3 rotation = particle->data.rotation;
        if (draw_prev_pos)
            rotation.z -= (float_t)M_PI;
        else if (draw_z_axis)
            rotation.z -= (float_t)M_PI_2;
        vec3_mult_scalar(rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, rotation);
            if (draw_prev_pos)
                rotation.z += (float_t)M_PI;
            else if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            particle->data.rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation;
        vec3_mult_scalar(particle->data.rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, particle->data.rotation);
            changed = true;
        }
    }

    vec3 rotation_random;
    vec3_mult_scalar(particle->data.rotation_random, RAD_TO_DEG_FLOAT, rotation_random);
    if (imguiColumnDragVec3Flag("Rotation Random",
        &rotation_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_random, DEG_TO_RAD_FLOAT, particle->data.rotation_random);
        changed = true;
    }

    vec3 rotation_add;
    vec3_mult_scalar(particle->data.rotation_add, RAD_TO_DEG_FLOAT, rotation_add);
    if (imguiColumnDragVec3Flag("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add, DEG_TO_RAD_FLOAT, particle->data.rotation_add);
        changed = true;
    }

    vec3 rotation_add_random;
    vec3_mult_scalar(particle->data.rotation_add_random, RAD_TO_DEG_FLOAT, rotation_add_random);
    if (imguiColumnDragVec3Flag("Rotation Add Random",
        &rotation_add_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add_random, DEG_TO_RAD_FLOAT, particle->data.rotation_add_random);
        changed = true;
    }

    imguiDisableElementPush(particle->data.type != Glitter::PARTICLE_MESH);
    if (imguiColumnDragVec2Flag("Scale",
        (vec2*)&particle->data.scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle->data.flags & Glitter::PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;

    if (imguiColumnDragVec2Flag("Scale Random",
        (vec2*)&particle->data.scale_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle->data.flags & Glitter::PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;
    imguiDisableElementPop(particle->data.type != Glitter::PARTICLE_MESH);

    ImGui::Separator();

    if (imguiColumnDragVec3("Direction",
        &particle->data.direction, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Direction Random",
        &particle->data.direction_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 speed;
    speed.x = particle->data.speed;
    speed.y = particle->data.speed_random;
    if (imguiColumnDragVec2("Speed [R]",
        &speed, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.speed = speed.x;
        particle->data.speed_random = speed.y;
        changed = true;
    }

    vec2 deceleration;
    deceleration.x = particle->data.deceleration;
    deceleration.y = particle->data.deceleration_random;
    if (imguiColumnDragVec2("Deceleration [R]",
        &deceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.deceleration = deceleration.x;
        particle->data.deceleration_random = deceleration.y;
        changed = true;
    }

    if (imguiColumnDragVec3("Gravity",
        &particle->data.gravity, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Acceleration",
        &particle->data.acceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Acceleration Random",
        &particle->data.acceleration_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 reflection_coeff;
    reflection_coeff.x = particle->data.reflection_coeff;
    reflection_coeff.y = particle->data.reflection_coeff_random;
    if (imguiColumnDragVec2("Refl Coeff [R]",
        &reflection_coeff, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.reflection_coeff = reflection_coeff.x;
        particle->data.reflection_coeff_random = reflection_coeff.y;
        changed = true;
    }

    if (imguiColumnDragFloat("Rebound Plane Y",
        &particle->data.rebound_plane_y, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    ImGui::Separator();

    vec4 color = particle->data.color;
    if (imguiColumnColorEdit4("Color", &color,
        ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
        changed = true;
    particle->data.color = color;

    if (imguiColumnComboBox("UV Index Type", Glitter::uv_index_type_name,
        Glitter::UV_INDEX_USER,
        (int32_t*)&particle->data.uv_index_type, 0, true, &data->imgui_focus))
        changed = true;

    vec2i split;
    split.x = particle->data.split_u;
    split.y = particle->data.split_v;
    if (imguiColumnSliderLogVec2I("UV Split",
        &split, 1, 128, "%d", 0)) {
        particle->data.split_u = (uint8_t)split.x;
        particle->data.split_v = (uint8_t)split.y;

        particle->data.split_uv.x = 1.0f / (float_t)particle->data.split_u;
        particle->data.split_uv.y = 1.0f / (float_t)particle->data.split_v;
        changed = true;
    }

    int32_t uv_max_count = (int32_t)(particle->data.split_u * particle->data.split_v);
    if (uv_max_count)
        uv_max_count--;

    if (imguiColumnDragInt("Frame Step UV",
        &particle->data.frame_step_uv, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index Start",
        &particle->data.uv_index_start, 1.0f, 0, uv_max_count, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index End",
        &particle->data.uv_index_end, 1.0f, particle->data.uv_index_start + 1, uv_max_count + 1, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index",
        &particle->data.uv_index, 1.0f, particle->data.uv_index_start,
        particle->data.uv_index_end - 1, "%d", 0))
        changed = true;

    if (imguiColumnDragVec2("UV Scroll Add",
        &particle->data.uv_scroll_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragFloat("UV Scroll Add Scale",
        &particle->data.uv_scroll_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (particle->data.type == Glitter::PARTICLE_MESH) {
        bool uv_2nd_add = particle->data.sub_flags & Glitter::PARTICLE_SUB_UV_2ND_ADD ? true : false;
        imguiDisableElementPush(uv_2nd_add);
        if (imguiColumnDragVec2("UV Scroll 2nd Add",
            &particle->data.uv_scroll_2nd_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragFloat("UV Scroll 2nd Add Scale",
            &particle->data.uv_scroll_2nd_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        imguiDisableElementPop(uv_2nd_add);
    }

    if (particle->data.type != Glitter::PARTICLE_MESH) {
        int32_t idx0 = 0;
        int32_t idx1 = 0;
        Glitter::EffectGroup* eg = glt_edt->effect_group;
        size_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        for (size_t i = 0; i < rc && (!idx0 || !idx1); i++) {
            if (idx0 == 0 && rh[i] == particle->data.tex_hash)
                idx0 = (int32_t)(i + 1);

            if (idx1 == 0 && rh[i] == particle->data.mask_tex_hash)
                idx1 = (int32_t)(i + 1);
        }

        char** texture_name_list = force_malloc_s(char*, rc + 1);
        char* texture_names = force_malloc_s(char, 0x40 * (rc + 1));

        texture_name_list[0] = texture_names;
        snprintf(texture_name_list[0], 0x40, "No Texture");
        for (size_t i = 0; i < rc; i++)
            snprintf(texture_name_list[i + 1] = &texture_names[0x40 * (i + 1)],
                0x40, "Texture %lld (%016llX)", i + 1, rh[i]);

        static bool tex0_anim;
        static int32_t tex0_frame;
        static int32_t tex0_index;
        static int32_t tex0_tex;
        if (glitter_editor_property_particle_texture(glt_edt, data, "Texture", texture_name_list,
            particle, &particle->data.texture, &particle->data.tex_hash,
            idx0, &tex0_anim, &tex0_frame, &tex0_index, &tex0_tex))
            changed = true;

        if (particle->data.flags & Glitter::PARTICLE_TEXTURE_MASK) {
            static bool tex11_anim;
            static int32_t tex1_frame;
            static int32_t tex1_index;
            static int32_t tex1_tex;
            if (glitter_editor_property_particle_texture(glt_edt, data, "Texture Mask", texture_name_list,
                particle, &particle->data.mask_texture, &particle->data.mask_tex_hash,
                idx1, &tex11_anim, &tex1_frame, &tex1_index, &tex1_tex))
                changed = true;
        }
        free(texture_names);
        free(texture_name_list);

        Glitter::ParticleBlendDraw blend_draw = Glitter::particle_blend_draw_default;
        for (int32_t i = 0; i < Glitter::particle_blend_mask_types_count; i++)
            if (Glitter::particle_blend_draw_types[i] == particle->data.blend_mode) {
                blend_draw = (Glitter::ParticleBlendDraw)i;
                break;
            }

        if (imguiColumnComboBox("Blend Mode", Glitter::particle_blend_draw_name,
            Glitter::PARTICLE_BLEND_DRAW_MULTIPLY,
            (int32_t*)&blend_draw, 0, true, &data->imgui_focus))
            if (Glitter::particle_blend_draw_types[blend_draw] != particle->data.blend_mode) {
                particle->data.blend_mode = Glitter::particle_blend_draw_types[blend_draw];
                changed = true;
            }

        if (particle->data.flags & Glitter::PARTICLE_TEXTURE_MASK) {
            Glitter::ParticleBlendMask blend_mask = Glitter::particle_blend_mask_default;
            for (int32_t i = 0; i < Glitter::particle_blend_mask_types_count; i++)
                if (Glitter::particle_blend_mask_types[i] == particle->data.mask_blend_mode) {
                    blend_mask = (Glitter::ParticleBlendMask)i;
                    break;
                }

            if (imguiColumnComboBox("Mask Blend Mode", Glitter::particle_blend_mask_name,
                Glitter::PARTICLE_BLEND_MASK_MULTIPLY,
                (int32_t*)&blend_mask, 0, true, &data->imgui_focus))
                if (Glitter::particle_blend_mask_types[blend_mask] != particle->data.mask_blend_mode) {
                    particle->data.mask_blend_mode = Glitter::particle_blend_mask_types[blend_mask];
                    changed = true;
                }
        }
    }

    switch (particle->data.uv_index_type) {
    case Glitter::UV_INDEX_FIXED:
    case Glitter::UV_INDEX_RANDOM:
    case Glitter::UV_INDEX_FORWARD:
    case Glitter::UV_INDEX_REVERSE:
    case Glitter::UV_INDEX_USER:
        particle->data.uv_index &= uv_max_count;
        break;
    case Glitter::UV_INDEX_INITIAL_RANDOM_FIXED:
    case Glitter::UV_INDEX_INITIAL_RANDOM_FORWARD:
    case Glitter::UV_INDEX_INITIAL_RANDOM_REVERSE:
        if (particle->data.uv_index < particle->data.uv_index_start)
            particle->data.uv_index = particle->data.uv_index_start;
        else if (particle->data.uv_index_end >= 0 && particle->data.uv_index > particle->data.uv_index_end)
            particle->data.uv_index = particle->data.uv_index_end;

        particle->data.uv_index = min(particle->data.uv_index, uv_max_count);
        break;
    }

    if (particle->data.uv_index_end >= 0)
        particle->data.uv_index_count = particle->data.uv_index_end - particle->data.uv_index_start + 1;
    else
        particle->data.uv_index_count = uv_max_count - particle->data.uv_index_start;

    if (particle->data.uv_index_count < 0)
        particle->data.uv_index_count = 0;

    if (imguiCheckboxFlags("Use Animation Curve",
        (uint32_t*)&particle->data.sub_flags,
        Glitter::PARTICLE_SUB_USE_CURVE))
        changed = true;

    if (eg->type == Glitter::X && particle->data.type == Glitter::PARTICLE_MESH)
        if (imguiCheckboxFlags("UV Add",
            (uint32_t*)&particle->data.sub_flags,
            Glitter::PARTICLE_SUB_UV_2ND_ADD))
            changed = true;

    ImGui::Separator();

    if (eg->type != Glitter::X || particle->data.type == Glitter::PARTICLE_QUAD)
        if (imguiColumnDragInt("Count",
            &particle->data.count, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

    if (imguiColumnDragFloat("Emission",
        &particle->data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        if (sel_efct->data.flags & Glitter::EFFECT_EMISSION
            || particle->data.emission >= Glitter::min_emission)
            enum_or(particle->data.flags, Glitter::PARTICLE_EMISSION);
        else
            enum_and(particle->data.flags, ~Glitter::PARTICLE_EMISSION);
        changed = true;
    }

    if (imguiColumnDragInt("Unk 0",
        &particle->data.unk0, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (eg->version >= 7 && imguiColumnDragInt("Unk 1",
        &particle->data.unk1, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (particle->data.type == Glitter::PARTICLE_LOCUS) {
        if (imguiColumnDragInt("Locus Hist Size",
            &particle->data.locus_history_size, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

        if (imguiColumnDragInt("Locus Hist Size Rand",
            &particle->data.locus_history_size_random, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;
    }
    else if (particle->data.type == Glitter::PARTICLE_MESH) {
        ImGui::Separator();

        Glitter::Particle::Mesh* mesh = &particle->data.mesh;
        uint32_t set_id = (uint32_t)mesh->object_set_name_hash;
        uint32_t obj_id = (uint32_t)mesh->object_name_hash;

        obj_set_handler* handler = object_storage_get_obj_set_handler(set_id);
        ssize_t object_set_count = object_storage_get_obj_set_count();

        imguiStartPropertyColumn("Object Set");
        if (ImGui::BeginCombo("##Object Set", handler ? handler->name.c_str() : "None", 0)) {
            ssize_t set_index = handler ? object_storage_get_obj_set_index(set_id) : -1;

            ImGui::PushID(-1);
            if (ImGui::Selectable("None", !handler)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true))
                set_index = -1;
            ImGui::PopID();

            for (ssize_t i = 0; i < object_set_count; i++) {
                obj_set_handler* handler = object_storage_get_obj_set_handler_by_index(i);
                if (!handler || !handler->obj_set)
                    continue;

                ImGui::PushID((int32_t)i);
                if (ImGui::Selectable(handler->name.c_str(), handler->set_id == set_id)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && handler->set_id != set_id))
                    set_index = i;
                ImGui::PopID();
            }

            if (set_index == -1)
                set_id = -1;
            else
                set_id = object_storage_get_obj_set_handler_by_index(set_index)->set_id;

            if (mesh->object_set_name_hash != set_id) {
                mesh->object_set_name_hash = set_id;
                mesh->object_name_hash = -1;
                changed = true;
            }

            data->imgui_focus |= true;
            ImGui::EndCombo();
        }
        imguiEndPropertyColumn();

        imguiDisableElementPush(set_id != -1);
        obj* obj = 0;
        if (handler && handler->obj_set) {
            obj_set* set = handler->obj_set;
            for (uint32_t i = 0; i < set->obj_num; i++)
                if (set->obj_data[i].id == obj_id) {
                    obj = &set->obj_data[i];
                    break;
                }
        }

        imguiStartPropertyColumn("Object");
        if (ImGui::BeginCombo("##Object", obj ? string_data(&obj->name) : "None", 0)) {
            if (set_id != -1 && handler && handler->set_id == set_id && handler->obj_set) {
                obj_set* set = handler->obj_set;
                ssize_t obj_index = -1;
                for (uint32_t i = 0; i < set->obj_num; i++)
                    if (set->obj_data[i].id == obj_id) {
                        obj_index = i;
                        break;
                    }

                ImGui::PushID(-1);
                if (ImGui::Selectable("None", !obj)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && obj))
                    obj_index = -1;
                ImGui::PopID();

                for (uint32_t i = 0; i < set->obj_num; i++) {
                    ImGui::PushID(i);
                    ::obj* obj = &set->obj_data[i];
                    if (ImGui::Selectable(string_data(&obj->name), obj->id == obj_id)
                        || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                        || (ImGui::IsItemFocused() && obj->id != obj_id))
                        obj_index = i;
                    ImGui::PopID();
                }

                if (obj_index == -1 || obj_index >= set->obj_num)
                    obj_id = -1;
                else
                    obj_id = set->obj_data[obj_index].id;

                if (mesh->object_name_hash != obj_id) {
                    mesh->object_name_hash = obj_id;
                    changed = true;
                }
            }

            data->imgui_focus |= true;
            ImGui::EndCombo();
        }
        imguiEndPropertyColumn();
        imguiDisableElementPop(set_id != -1);

        /*if (glitter_editor_hash_input("Object Set Name Hash", &mesh->object_set_name_hash))
            changed = true;

        if (glitter_editor_hash_input("Object Name Hash", &mesh->object_name_hash))
            changed = true;

        const size_t mesh_name_size = sizeof(mesh->mesh_name);
        char mesh_name[sizeof(mesh->mesh_name)];
        memcpy(mesh_name, mesh->mesh_name, mesh_name_size);
        if (imguiColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
            memcpy(mesh->mesh_name, mesh_name, mesh_name_size);
            changed = true;
        }

        if (glitter_editor_hash_input("Sub Mesh Hash", &mesh->sub_mesh_hash))
            changed = true;*/
    }

    uint64_t hash_after = hash_fnv1a64m(glt_edt->selected_particle, sizeof(Glitter::Particle));
    if (hash_before != hash_after && changed)
        glt_edt->input_reload = true;

    imguiSetDefaultColumnSpace();
}

static bool glitter_editor_property_particle_texture(glitter_editor_struct* glt_edt,
    class_data* data, const char* label, char** items, Glitter::Particle* particle,
    int32_t* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
    int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;
    size_t rc = eg->resources_count;
    texture** r = eg->resources;
    uint64_t* rh = eg->resource_hashes.data();
    txp* rt = eg->resources_tex.textures.data();

    const uint64_t empty_hash = eg->type != Glitter::FT
        ? hash_murmurhash_empty : hash_fnv1a64m_empty;

    ImVec2 uv_min;
    ImVec2 uv_max;

    imguiStartPropertyColumn(label);
    int32_t prev_tex_idx = tex_idx;
    if (ImGui::BeginCombo("", items[tex_idx], 0)) {
        int32_t uv_index = *tex_index;
        if (!*tex_anim) {
            *tex_anim = true;
            *tex_frame = 0;
            *tex_tex = tex_idx;

            uv_index = particle->data.uv_index;
            int32_t max_uv = particle->data.split_u * particle->data.split_v;
            if (max_uv > 1 && particle->data.uv_index_count > 1) {
                switch (particle->data.uv_index_type) {
                case Glitter::UV_INDEX_INITIAL_RANDOM_FIXED:
                case Glitter::UV_INDEX_INITIAL_RANDOM_FORWARD:
                case Glitter::UV_INDEX_INITIAL_RANDOM_REVERSE: {
                    LARGE_INTEGER time;
                    QueryPerformanceCounter(&time);
                    uv_index += time.LowPart % particle->data.uv_index_count;
                } break;
                }
            }
        }
        else if (particle->data.uv_index_type == Glitter::UV_INDEX_INITIAL_RANDOM_FIXED) {
            if (particle->data.uv_index_count > 0 && *tex_frame % 40 == 0) {
                uv_index = particle->data.uv_index;

                LARGE_INTEGER time;
                QueryPerformanceCounter(&time);
                uv_index += time.LowPart % particle->data.uv_index_count;
            }
        }
        else if (particle->data.frame_step_uv && *tex_frame % particle->data.frame_step_uv == 0) {
            int32_t max_uv = particle->data.split_u * particle->data.split_v;
            if (max_uv)
                max_uv--;

            switch (particle->data.uv_index_type) {
            case Glitter::UV_INDEX_RANDOM:
                uv_index = particle->data.uv_index_start;
                if (particle->data.uv_index_count > 1) {
                    LARGE_INTEGER time;
                    QueryPerformanceCounter(&time);
                    uv_index += time.LowPart % particle->data.uv_index_count;
                }
                break;
            case Glitter::UV_INDEX_FORWARD:
            case Glitter::UV_INDEX_INITIAL_RANDOM_FORWARD:
                uv_index = (uint8_t)(max_uv & (uv_index + 1));
                break;
            case Glitter::UV_INDEX_REVERSE:
            case Glitter::UV_INDEX_INITIAL_RANDOM_REVERSE:
                uv_index = (uint8_t)(max_uv & (uv_index - 1));
                break;
            }
        }
        *tex_index = uv_index;

        vec2 uv;
        uv.x = (float_t)(uv_index % particle->data.split_u) * particle->data.split_uv.x;
        uv.y = (float_t)(uv_index / particle->data.split_u) * particle->data.split_uv.y;
        uv_min.x = uv.x;
        uv_min.y = uv.y;
        uv_max.x = uv.x + particle->data.split_uv.x;
        uv_max.y = uv.y + particle->data.split_uv.y;

        for (int32_t n = 0; n <= rc; n++) {
            ImGui::PushID(n);
            if (ImGui::Selectable(items[n], tex_idx == n)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && tex_idx != n))
                tex_idx = n;

            if (ImGui::IsItemHovered(0) && n) {
                if (*tex_tex != n) {
                    *tex_anim = true;
                    *tex_frame = 0;
                    *tex_tex = n;

                    uv_index = particle->data.uv_index;
                    int32_t max_uv = particle->data.split_u * particle->data.split_v;
                    if (max_uv > 1) {
                        switch (particle->data.uv_index_type) {
                        case Glitter::UV_INDEX_INITIAL_RANDOM_FIXED:
                        case Glitter::UV_INDEX_INITIAL_RANDOM_FORWARD:
                        case Glitter::UV_INDEX_INITIAL_RANDOM_REVERSE: {
                            LARGE_INTEGER time;
                            QueryPerformanceCounter(&time);
                            uv_index = particle->data.uv_index_start
                                + time.LowPart % particle->data.uv_index_count;
                        } break;
                        }
                    }
                    *tex_index = uv_index;

                    vec2 uv;
                    uv.x = (float_t)(uv_index % particle->data.split_u) * particle->data.split_uv.x;
                    uv.y = (float_t)(uv_index / particle->data.split_u) * particle->data.split_uv.y;
                    uv_min.x = uv.x;
                    uv_min.y = uv.y;
                    uv_max.x = uv.x + particle->data.split_uv.x;
                    uv_max.y = uv.y + particle->data.split_uv.y;
                }

                txp_mipmap* rtm = rt[n - 1].mipmaps.data();
                float_t aspect1 = (float_t)rtm->width / (float_t)rtm->height;
                float_t aspect2 = aspect1;
                if (particle->data.split_u * particle->data.split_v > 1)
                    aspect2 /= (float_t)particle->data.split_u
                        / (float_t)particle->data.split_v;

                ImVec2 size1 = { 192.0f, 192.0f };
                if (aspect1 > 1.0f)
                    size1.y /= aspect1;
                else if (aspect1 < 1.0f)
                    size1.x *= aspect1;

                ImVec2 size2 = { 192.0f, 192.0f };
                if (aspect2 > 1.0f)
                    size2.y /= aspect2;
                else if (aspect2 < 1.0f)
                    size2.x *= aspect2;

                ImGui::BeginTooltip();
                ImGui::Text("Frame: %d\nUV Index %d", *tex_frame, *tex_index);
                ImGui::Text("Tex Size: %dx%d", rtm->width, rtm->height);
                ImGui::Image((void*)(size_t)r[n - 1]->tex, size1,
                    { 0, 0 }, { 1, 1 }, tint_col, border_col);
                ImGui::Text("Preview Tex Size: %gx%g",
                    particle->data.split_u > 1
                    ? (float_t)rtm->width / (float_t)particle->data.split_u
                    : (float_t)rtm->width,
                    particle->data.split_u > 1
                    ? (float_t)rtm->height / (float_t)particle->data.split_v
                    : (float_t)rtm->height);
                ImGui::Image((void*)(size_t)r[n - 1]->tex, size2,
                    uv_min, uv_max, tint_col, border_col);
                ImGui::EndTooltip();
            }

            if (tex_idx == n)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }

        data->imgui_focus |= true;
        ImGui::EndCombo();
        (*tex_frame)++;
    }
    else {
        *tex_anim = false;
        *tex_frame = 0;
        *tex_index = 0;
        *tex_tex = 0;
    }
    bool res = prev_tex_idx != tex_idx;
    imguiEndPropertyColumn();

    if (res)
        if (tex_idx) {
            *tex = r[tex_idx - 1]->tex;
            *tex_hash = rh[tex_idx - 1];
        }
        else {
            *tex = 0;
            *tex_hash = empty_hash;
        }
    return res;
}

static void glitter_editor_popups(glitter_editor_struct* glt_edt, class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    const float_t title_bar_size = font->FontSize + style.ItemSpacing.y + style.FramePadding.y * 2.0f;

    ImGui::PushID("Glitter Editor Glitter File Create Pop-up");
    glitter_editor_file_create_popup(glt_edt, data, io, style, font, title_bar_size);
    ImGui::PopID();

    ImGui::PushID("Glitter Editor Glitter File Load Pop-up");
    glitter_editor_file_load_popup(glt_edt, data, io, style, font, title_bar_size);
    ImGui::PopID();

    ImGui::PushID("Glitter Editor Glitter File Load Pop-up");
    glitter_editor_file_load_model_popup(glt_edt, data, io, style, font, title_bar_size);
    ImGui::PopID();

    ImGui::PushID("Glitter Editor Glitter File Load Error List Pop-up");
    glitter_editor_file_load_error_list_popup(glt_edt, data, io, style, font, title_bar_size);
    ImGui::PopID();

    ImGui::PushID("Glitter Editor Glitter File Save");
    glitter_editor_file_save_popup(glt_edt, data, io, style, font, title_bar_size);
    ImGui::PopID();
}

static void glitter_editor_file_create_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->create_popup) {
        ImGui::OpenPopup("Create Glitter File as...", 0);
        glt_edt->create_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    Glitter::EffectGroup* eg = glt_edt->effect_group;

    ImGuiWindowFlags window_flags;

    const float_t button_width = 70.0f;

    win_x = button_width * 2.0f + style.ItemSpacing.x * 1.0f;
    win_y = title_bar_size + font->FontSize + style.ItemSpacing.y * 1.0f + style.FramePadding.y * 2.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Create Glitter File as...", 0, window_flags)) {
        x = win_x * 0.5f - ImGui::GetContentRegionAvail().x * 0.5f;
        y = title_bar_size;

        ImGui::SetCursorPos({ x, y });
        if (ImGui::BeginTable("buttons", 2)) {
            bool close = false;

            ImGui::TableNextColumn();
            if (imguiButton("F2", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::F2;
                close = true;
            }

            ImGui::TableNextColumn();
            if (imguiButton("FT", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::FT;
                close = true;
            }

            /*ImGui::TableNextColumn();
            if (imguiButton("X", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::X;
                close = true;
            }*/

            if (close) {
                glt_edt->effect_group_add = true;
                enum_or(glt_edt->effect_flags, GLITTER_EDITOR_ADD);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndTable();
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static void glitter_editor_file_load_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->load_popup) {
        ImGui::OpenPopup("Load Glitter File as...", 0);
        glt_edt->load_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;

    const float_t button_width = 70.0f;

    win_x = button_width * 3.0f + style.ItemSpacing.x * 2.0f;
    win_y = title_bar_size + font->FontSize + style.ItemSpacing.y * 2.0f + style.FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Load Glitter File as...", 0, window_flags)) {
        x = win_x * 0.5f - ImGui::GetContentRegionAvail().x * 0.5f;
        y = title_bar_size;

        ImGui::SetCursorPos({ x, y });
        if (ImGui::BeginTable("buttons", 3)) {
            bool close = false;

            ImGui::TableNextColumn();
            if (imguiButton("F2", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::F2;
                close = true;
            }

            ImGui::TableNextColumn();
            if (imguiButton("FT", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::FT;
                close = true;
            }

            ImGui::TableNextColumn();
            if (imguiButton("X", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::X;
                close = true;
            }

            if (close) {
                glt_edt->load = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndTable();
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static void glitter_editor_file_load_model_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size) {
    if (!glt_edt->effect_group) {
        glt_edt->load_data_popup = false;
        return;
    }

    bool data_aft_enable = data_list[DATA_AFT].data_paths.size() > 0;
    bool data_f2le_enable = data_list[DATA_F2LE].data_paths.size() > 0;
    bool data_f2be_enable = data_list[DATA_F2BE].data_paths.size() > 0;
    bool data_ft_enable = data_list[DATA_FT].data_paths.size() > 0;
    bool data_m39_enable = data_list[DATA_M39].data_paths.size() > 0;
    bool data_vrfl_enable = data_list[DATA_VRFL].data_paths.size() > 0;
    bool data_x_enable = data_list[DATA_X].data_paths.size() > 0;
    bool data_xhd_enable = data_list[DATA_XHD].data_paths.size() > 0;

    int32_t data_count = 0;
    switch (glt_edt->effect_group->type) {
    case Glitter::F2:
        if (data_f2le_enable)
            data_count++;
        if (data_f2be_enable)
            data_count++;
        break;
    case Glitter::FT:
        if (data_aft_enable)
            data_count++;
        if (data_ft_enable)
            data_count++;
        if (data_m39_enable)
            data_count++;
        break;
    case Glitter::X:
        if (data_x_enable)
            data_count++;
        if (data_xhd_enable)
            data_count++;
        if (data_vrfl_enable)
            data_count++;
        break;
    }

    if (glt_edt->load_data_popup) {
        if (data_count)
            ImGui::OpenPopup("Which data should be loaded?", 0);
        glt_edt->load_data_popup = false;
    }

    if (!data_count)
        return;

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;

    float_t button_width;
    if (data_count == 3) {
        button_width = 70.0f;
        win_x = button_width * 3.0f + style.ItemSpacing.x * 2.0f;
    }
    else if (data_count == 2) {
        button_width = 105.0f;
        win_x = button_width * 2.0f + style.ItemSpacing.x;
    }
    else {
        button_width = 210.0f;
        win_x = button_width;
    }
    win_y = title_bar_size + font->FontSize + style.ItemSpacing.y * 2.0f + style.FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Which data should be loaded?", 0, window_flags)) {
        x = win_x * 0.5f - ImGui::GetContentRegionAvail().x * 0.5f;
        y = title_bar_size;

        ImGui::SetCursorPos({ x, y });
        if (ImGui::BeginTable("buttons", data_count)) {
            bool close = false;

            switch (glt_edt->effect_group->type) {
            case Glitter::F2:
                if (data_f2le_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("F2LE",  ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_F2LE;
                        close = true;
                    }
                }

                if (data_f2be_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("F2BE", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_F2BE;
                        close = true;
                    }
                }
                break;
            case Glitter::FT:
                if (data_aft_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("AFT", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_AFT;
                        close = true;
                    }
                }

                if (data_ft_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("FT", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_FT;
                        close = true;
                    }
                }

                if (data_m39_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("M39", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_M39;
                        close = true;
                    }
                }
                break;
            case Glitter::X:
                if (data_x_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("X", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_X;
                        close = true;
                    }
                }

                if (data_xhd_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("XHD", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_XHD;
                        close = true;
                    }
                }

                if (data_vrfl_enable) {
                    ImGui::TableNextColumn();
                    if (imguiButton("VRFL", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_VRFL;
                        close = true;
                    }
                }
                break;
            }

            if (close) {
                glt_edt->load_data = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndTable();
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static void glitter_editor_file_load_error_list_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->load_error_list_popup) {
        ImGui::OpenPopup("Glitter File Load Error", 0);
        glt_edt->load_error_list_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;

    win_x = 264.0f;
    win_y = title_bar_size + font->FontSize * 3.0f
        + style.ItemSpacing.y * 3.0f + style.FramePadding.y * 2.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Glitter File Load Error", 0, window_flags)) {
        ImGui::SetCursorPosY(title_bar_size);
        ImGui::Text("Can't find name in Glitter List File\n"
            "for hash in Glitter Effect File");

        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - 20.0f);

        x = 40.0f;
        y = font->FontSize + style.FramePadding.y * 2.0f;
        if (imguiButton("OK", { x, y }))
            ImGui::CloseCurrentPopup();
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static void glitter_editor_file_save_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->save_popup) {
        ImGui::OpenPopup("Save Glitter File as...", 0);
        glt_edt->save_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    ImGuiWindowFlags window_flags;

    const float_t button_width = 70.0f;

    if (eg && eg->type != Glitter::X)
        win_x = button_width * 4.0f + style.ItemSpacing.x * 3.0f;
    else
        win_x = button_width * 3.0f + style.ItemSpacing.x * 2.0f;
    win_y = title_bar_size + font->FontSize + style.ItemSpacing.y * 2.0f + style.FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Save Glitter File as...", 0, window_flags)) {
        x = win_x * 0.5f - ImGui::GetContentRegionAvail().x * 0.5f;
        y = title_bar_size;

        ImGui::SetCursorPos({ x, y });
        /*if (eg && eg->type == Glitter::X) {
            if (ImGui::BeginTable("buttons", 3)) {
                bool close = false;

                ImGui::TableNextColumn();
                if (imguiButton("X", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::X;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (imguiButton("X HD", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = Glitter::X;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (imguiButton("None", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = false;
                    glt_edt->save_compress = false;
                    close = true;
                }

                if (close)
                    ImGui::CloseCurrentPopup();
                ImGui::EndTable();
            }
        }
        else*/ {
            if (ImGui::BeginTable("buttons", 4)) {
                bool close = false;

                ImGui::TableNextColumn();
                if (imguiButton("F2 PSV", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::F2;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (imguiButton("F2 PS3", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = Glitter::F2;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (imguiButton("FT", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::FT;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (imguiButton("None", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = false;
                    glt_edt->save_compress = false;
                    close = true;
                }

                if (close)
                    ImGui::CloseCurrentPopup();
                ImGui::EndTable();
            }
        }
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static float_t convert_height_to_value(glitter_editor_struct* glt_edt,
    float_t val, float_t pos, float_t size, float_t min, float_t max) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    float_t t = (val - pos - crv_edt->height_offset)
        / (size - crv_edt->height_offset);
    return (1.0f - t) * (max - min) + min;
}
static float_t convert_value_to_height(glitter_editor_struct* glt_edt,
    float_t val, float_t pos, float_t size, float_t min, float_t max) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    float_t t = 1.0f - (val - min) / (max - min);
    return pos + t * (size - crv_edt->height_offset)
        + crv_edt->height_offset;
}

static void glitter_editor_curve_editor(glitter_editor_struct* glt_edt) {
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    if (crv_edt->type < Glitter::CURVE_TRANSLATION_X
        || crv_edt->type > Glitter::CURVE_V_SCROLL_ALPHA_2ND)
        return;

    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
    case GLITTER_EDITOR_SELECTED_EMITTER:
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        break;
    default:
        return;
    }

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Curve* curve = crv_edt->list[crv_edt->type];
    std::vector<Glitter::Curve::Key>* keys = curve ? &curve->keys_rev : 0;
    bool changed = false;

    bool fix_rot_z = eg->type != Glitter::X && curve && curve->type == Glitter::CURVE_ROTATION_Z
        && ((glt_edt->selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && sel_emit->data.direction == Glitter::DIRECTION_Z_AXIS)
            || (glt_edt->selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && sel_ptcl->data.draw_type == Glitter::DIRECTION_Z_AXIS));

    bool add_key = false;
    bool del_key = false;
    if (curve)
        glitter_editor_curve_editor_key_manager(glt_edt, keys, &add_key, &del_key);

    if (ImGui::IsWindowFocused()) {
        if (add_key && ImGui::IsKeyPressed(GLFW_KEY_INSERT))
            crv_edt->add_key = true;
        else if (del_key && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
            crv_edt->del_key = true;
        else if (ImGui::IsKeyPressed(GLFW_KEY_RIGHT))
            crv_edt->frame++;
        else if (ImGui::IsKeyPressed(GLFW_KEY_LEFT))
            crv_edt->frame--;
    }

    bool exist = true;
    if (crv_edt->add_key && add_key) {
        if (!crv_edt->list[crv_edt->type] || !keys || keys->size() == 0) {
            if (!crv_edt->list[crv_edt->type]) {
                Glitter::Curve* c = new Glitter::Curve(eg->type);
                crv_edt->animation->curves.push_back(curve);
                crv_edt->list[crv_edt->type] = c;
                curve = c;
            }

            glitter_editor_curve_editor_curve_set(glt_edt, curve, crv_edt->type);

            if (!keys)
                keys = &curve->keys_rev;

            Glitter::Curve::Key key;
            key.frame = 0;
            key.value = glitter_editor_curve_editor_get_value(glt_edt, curve->type);
            keys->push_back(key);

            crv_edt->frame = 0;
            changed = true;
        }
        else {
            std::vector<Glitter::Curve::Key>::iterator i = keys->begin();
            bool is_before_start = keys->data()[0].frame > crv_edt->frame;
            bool has_key_after = false;
            if (!is_before_start)
                for (i++; i != keys->end(); i++)
                    if (crv_edt->frame <= i->frame) {
                        has_key_after = keys->end() - i > 0;
                        break;
                    }

            ssize_t pos = i - keys->begin();
            if (!is_before_start)
                pos--;

            if (!is_before_start && has_key_after) {
                std::vector<Glitter::Curve::Key>::iterator c = i - 1;
                std::vector<Glitter::Curve::Key>::iterator n = i;

                Glitter::Curve::Key key;
                key.frame = crv_edt->frame;
                key.type = c->type;
                switch (c->type) {
                case Glitter::KEY_CONSTANT:
                    key.value = c->value;
                    key.random_range = c->random_range;
                    break;
                case Glitter::KEY_LINEAR: {
                    float_t t = (float_t)(key.frame - c->frame) / (float_t)(n->frame - c->frame);
                    key.value = lerp(c->value, n->value, t);
                    key.random_range = lerp(c->random_range, n->random_range, t);
                } break;
                case Glitter::KEY_HERMITE: {
                    int32_t df = n->frame - c->frame;
                    int32_t _df = key.frame - c->frame;
                    float_t* v_arr = 0;
                    size_t v_length = 0;
                    interpolate_chs(c->value, n->value, c->tangent2, n->tangent1, 0, df, &v_arr, &v_length);
                    key.value = v_arr[_df];
                    key.random_range = lerp(c->random_range, n->random_range, (float_t)_df / (float_t)df);
                    interpolate_chs_reverse(v_arr, v_length, &c->tangent2, &key.tangent1, 0, _df);
                    interpolate_chs_reverse(v_arr, v_length, &key.tangent2, &n->tangent1, _df, df);
                    free(v_arr);
                } break;
                }
                keys->insert(keys->begin() + ++pos, key);
            }
            else if (is_before_start) {
                std::vector<Glitter::Curve::Key>::iterator n = i;

                Glitter::Curve::Key key;
                key.frame = crv_edt->frame;
                key.type = n->type;
                key.value = n->value;
                key.random_range = n->random_range;
                keys->insert(keys->begin() + pos, key);
            }
            else {
                std::vector<Glitter::Curve::Key>::iterator c = i - 1;

                Glitter::Curve::Key key;
                key.frame = crv_edt->frame;
                key.type = c->type;
                key.value = c->value;
                key.random_range = c->random_range;
                keys->insert(keys->begin() + ++pos, key);
            }
            *crv_edt->list[crv_edt->type] = *curve;
            changed = true;
        }
    }
    else if (keys && crv_edt->del_key && del_key) {
        for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++)
            if (&i[0] == crv_edt->key) {
                bool has_key_before = keys->front().frame < i->frame;
                bool has_key_after = keys->back().frame > i->frame;
                if (has_key_before && i[-1].type == Glitter::KEY_HERMITE && has_key_after) {
                    std::vector<Glitter::Curve::Key>::iterator c = i - 1;
                    std::vector<Glitter::Curve::Key>::iterator n = i + 1;
                    int32_t df_c = i->frame - c->frame;
                    int32_t df_n = n->frame - i->frame;

                    float_t* v_arr_c = 0;
                    size_t v_length_c = 0;
                    interpolate_chs(c->value, i->value, c->tangent2,
                        i->tangent1, 0, df_c, &v_arr_c, &v_length_c);

                    float_t* v_arr_n = 0;
                    size_t v_length_n = 0;
                    if (i->type == Glitter::KEY_HERMITE)
                        interpolate_chs(i->value, n->value, i->tangent2,
                            n->tangent1, 0, df_n, &v_arr_n, &v_length_n);
                    else
                        interpolate_linear(i->value, n->value,
                            0, df_n, &v_arr_n, &v_length_n);

                    size_t v_length = v_length_c + v_length_n - 1;
                    float_t* v_arr = force_malloc_s(float_t, v_length);
                    memmove(v_arr, v_arr_c, (v_length_c - 1) * sizeof(float_t));
                    memmove(v_arr + (v_length_c - 1), v_arr_n, (v_length_n) * sizeof(float_t));
                    free(v_arr_c);
                    free(v_arr_n);
                    interpolate_chs_reverse(v_arr, v_length, &c->tangent2,
                        &n->tangent1, 0, (size_t)n->frame - c->frame);
                    free(v_arr);
                }
                keys->erase(i);
                changed = true;
                break;
            }
    }
    else if (crv_edt->add_curve) {
        Glitter::Curve* c = new Glitter::Curve(eg->type);
        crv_edt->animation->curves.push_back(c);
        crv_edt->list[crv_edt->type] = c;
        curve = c;

        glitter_editor_curve_editor_curve_set(glt_edt, curve, crv_edt->type);
        *crv_edt->list[crv_edt->type] = *curve;
        crv_edt->frame = curve->start_time;
        changed = true;
    }
    else if (crv_edt->del_curve) {
        Glitter::Curve* c = crv_edt->list[crv_edt->type];
        Glitter::Animation* anim = crv_edt->animation;
        for (std::vector<Glitter::Curve*>::iterator i = anim->curves.begin(); i != anim->curves.end(); i++)
            if (*i && *i == c) {
                delete* i;
                anim->curves.erase(i);
                break;
            }

        crv_edt->list[crv_edt->type] = 0;
        crv_edt->frame = 0;
        changed = true;
    }
    crv_edt->add_key = false;
    crv_edt->del_key = false;
    crv_edt->add_curve = false;
    crv_edt->del_curve = false;

    if (!curve || !keys)
        return;

    int32_t start_time = curve->start_time;
    int32_t end_time = curve->end_time;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    ImVec2 cont_reg_avail = ImGui::GetContentRegionAvail();

    ImVec2 canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max;
    crv_edt->draw_list = ImGui::GetWindowDrawList();
    canvas_size = cont_reg_avail;
    canvas_pos = ImGui::GetCursorScreenPos();
    canvas_pos_min.x = canvas_pos.x;
    canvas_pos_min.y = canvas_pos.y;
    canvas_pos_max.x = canvas_pos.x + canvas_size.x;
    canvas_pos_max.y = canvas_pos.y + canvas_size.y;

    ImRect boundaries;
    boundaries.Min = canvas_pos;
    boundaries.Max.x = canvas_pos.x + canvas_size.x;
    boundaries.Max.y = canvas_pos.y + canvas_size.y;

    if (crv_edt->zoom_time != crv_edt->prev_zoom_time) {
        float_t timeline_pos = crv_edt->timeline_pos + curve_editor_timeline_base_pos;
        timeline_pos *= crv_edt->zoom_time / crv_edt->prev_zoom_time;
        crv_edt->timeline_pos = timeline_pos - curve_editor_timeline_base_pos;
    }
    crv_edt->prev_zoom_time = crv_edt->zoom_time;

    float_t frame_width = crv_edt->frame_width
        * crv_edt->zoom_time;
    canvas_pos.x -= crv_edt->timeline_pos;

    ImGui::ItemSize(boundaries);
    bool hovered = ImGui::IsMouseHoveringRect(boundaries.Min, boundaries.Max);
    ImGui::RenderFrame(boundaries.Min, boundaries.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, 1.0f);

    static bool can_drag;
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left, false))
        can_drag = true;

    if (can_drag && (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))) {
        crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt, curve);
        crv_edt->frame = crv_edt->key->frame;
    }
    else
        crv_edt->key = glitter_editor_curve_editor_get_selected_key(glt_edt, curve);

    if (can_drag && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        can_drag = false;

    float_t timeline_max_offset = (end_time - start_time) * frame_width;
    timeline_max_offset = max(timeline_max_offset, 0.0f) - curve_editor_timeline_base_pos;

    crv_edt->draw_list->PushClipRect(boundaries.Min, boundaries.Max, true);

    int32_t start = (int32_t)(crv_edt->timeline_pos / frame_width) + start_time;
    int32_t end = (int32_t)((crv_edt->timeline_pos + canvas_size.x) / frame_width) + start_time;

    ImU32 line_color_valid_bold = ImGui::GetColorU32({ 0.5f, 0.5f, 0.5f, 0.75f });
    ImU32 line_color_valid = ImGui::GetColorU32({ 0.4f, 0.4f, 0.4f, 0.45f });
    ImU32 line_color_not_valid = ImGui::GetColorU32({ 0.2f, 0.2f, 0.2f, 0.75f });
    for (int32_t frame = start; frame <= end; frame++) {
        if (frame < start_time)
            continue;

        int32_t f = frame - start_time;
        float_t x = canvas_pos.x + f * frame_width;
        float_t y = canvas_pos.y + (f % 2 ? 10.0f : 0.0f) + canvas_size.y;

        bool bold_frame = f % 10 == 0 || f == 0 || frame == end_time;
        bool valid_frame = f >= 0 && frame <= end_time;

        ImU32 line_color = valid_frame ? (bold_frame ? line_color_valid_bold
            : line_color_valid) : line_color_not_valid;
        crv_edt->draw_list->AddLine({ x, y - canvas_size.y },
            { x, y }, line_color, 0.80f);

        if (bold_frame) {
            char buf[0x20];
            snprintf(buf, sizeof(buf), "%d", frame);
            crv_edt->draw_list->AddText({ x + 3, canvas_pos.y }, line_color, buf);
        }
    }

    float_t max = crv_edt->range * (1.0f / crv_edt->zoom_value);
    float_t min;
    if (curve->type == Glitter::CURVE_EMISSION_INTERVAL)
        min = -1.0f;
    else if (curve->type == Glitter::CURVE_PARTICLES_PER_EMISSION)
        min = 0.0f;
    else
        min = -max;

    for (int32_t j = 0; j < 5; j++) {
        float_t x_pos = canvas_pos_min.x;
        float_t y_pos = canvas_pos.y + j * 0.25f * (canvas_size.y
            - crv_edt->height_offset) + crv_edt->height_offset;

        ImU32 line_color = j % 2 == 1
            ? ImGui::GetColorU32({ 0.35f, 0.35f, 0.35f, 0.45f })
            : ImGui::GetColorU32({ 0.45f, 0.45f, 0.45f, 0.75f });
        bool last = j == 4;
        if (j && !last)
            crv_edt->draw_list->AddLine({ x_pos, y_pos },
                { x_pos + canvas_size.x, y_pos }, line_color, 0.80f);

        float_t val = max - j * 0.25f * (max - min);
        char buf[0x20];
        snprintf(buf, sizeof(buf), "%.2f", val);
        crv_edt->draw_list->AddText({ x_pos + 2, y_pos + (last ? -16 : -2) },
            ImGui::GetColorU32({ 0.65f, 0.65f, 0.65f, 0.85f }), buf);
    }

    if (curve->type >= Glitter::CURVE_ROTATION_X && curve->type <= Glitter::CURVE_ROTATION_Z) {
        max *= DEG_TO_RAD_FLOAT;
        min *= DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= Glitter::CURVE_COLOR_R && curve->type <= Glitter::CURVE_COLOR_RGB_SCALE_2ND) {
        max *= (float_t)(1.0 / 255.0);
        min *= (float_t)(1.0 / 255.0);
    }

    float_t base_line = convert_value_to_height(glt_edt, 0.0f, canvas_pos.y, canvas_size.y, min, max);

    ImVec2 p1, p2, p3;
    p1.x = canvas_pos_min.x;
    p1.y = canvas_pos.y + crv_edt->height_offset;
    p2.x = p1.x + canvas_size.x;
    p2.y = canvas_pos.y + crv_edt->height_offset;
    p3.x = canvas_pos_min.x;
    p3.y = canvas_pos.y;
    crv_edt->draw_list->AddLine(p1, p2,
        ImGui::GetColorU32({ 0.5f, 0.5f, 0.5f, 0.85f }), 2.0f);

    ImU32 random_range_color = ImGui::GetColorU32({ 0.5f, 0.5f, 0.0f, 0.25f });
    ImU32 key_random_range_color = ImGui::GetColorU32({ 0.5f, 0.0f, 0.5f, 0.25f });
    ImU32 default_color = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.75f });
    ImU32 selected_color = ImGui::GetColorU32({ 0.0f, 1.0f, 1.0f, 0.75f });

    if (curve->flags & Glitter::CURVE_RANDOM_RANGE
        || curve->flags & Glitter::CURVE_KEY_RANDOM_RANGE) {
        float_t random_range = curve->random_range;
        bool random_range_mult = curve->flags & Glitter::CURVE_RANDOM_RANGE_MULT;
        bool random_range_negate = curve->flags & Glitter::CURVE_RANDOM_RANGE_NEGATE;
        bool glt_type_ft = eg->type == Glitter::FT;
        for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++) {
            if (i->frame >= end_time || keys->end() - i <= 1)
                break;

            std::vector<Glitter::Curve::Key>::iterator c = i;
            std::vector<Glitter::Curve::Key>::iterator n = i + 1;
            float_t x1 = canvas_pos.x + (float_t)(c->frame - start_time) * frame_width;
            float_t x2 = canvas_pos.x + (float_t)(n->frame - start_time) * frame_width;

            if (((x1 < canvas_pos_min.x) && (x2 < canvas_pos_min.x)) ||
                (x1 > canvas_pos_max.x) && (x2 > canvas_pos_max.x))
                continue;

            x1 = max(x1, canvas_pos_min.x);
            x2 = min(x2, canvas_pos_max.x);

            Glitter::KeyType k_type = c->type;
            float_t c_frame = (float_t)c->frame;
            float_t n_frame = (float_t)n->frame;
            float_t c_value = c->value;
            float_t n_value = n->value;
            if (fix_rot_z) {
                c_value -= (float_t)M_PI_2;
                n_value -= (float_t)M_PI_2;
            }
            float_t c_random_range = c->random_range;
            float_t n_random_range = n->random_range;
            if (random_range_mult && glt_type_ft) {
                c_random_range *= c_value;
                n_random_range *= n_value;
            }

            size_t frame_width_int = (size_t)roundf(frame_width);
            if (k_type == Glitter::KEY_CONSTANT) {
                vec3 value = { c_value + c_random_range, c_value, c_value - c_random_range };
                vec3 random = { random_range, random_range, random_range };
                if (random_range_mult) {
                    vec3_mult(random, value, random);
                    if (!glt_type_ft)
                        vec3_mult_scalar(random, 0.01f, random);
                }

                if (value.x != value.y) {
                    float_t y1 = convert_value_to_height(glt_edt, value.x + random.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y2 = convert_value_to_height(glt_edt, value.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y3 = convert_value_to_height(glt_edt, value.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y4 = convert_value_to_height(glt_edt, value.y,
                        canvas_pos.y, canvas_size.y, min, max);

                    y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                    y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                    if (y1 != y2)
                        crv_edt->draw_list->AddRectFilled({ x1, y1 },
                            { x2, y2 }, random_range_color, 0.0f, 0);

                    y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                    y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                    if (y3 != y4)
                        crv_edt->draw_list->AddRectFilled({ x1, y3 },
                            { x2, y4 }, key_random_range_color, 0.0f, 0);
                }
                else {
                    float_t y1 = convert_value_to_height(glt_edt, value.y + random.y,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y2 = convert_value_to_height(glt_edt, value.y,
                        canvas_pos.y, canvas_size.y, min, max);

                    y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                    y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                    if (y1 != y2)
                        crv_edt->draw_list->AddRectFilled({ x1, y1 },
                            { x2, y2 }, random_range_color, 0.0f, 0);
                }

                if (random_range_negate)
                    if (value.y != value.z) {
                        float_t y1 = convert_value_to_height(glt_edt, value.y,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y2 = convert_value_to_height(glt_edt, value.z,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y3 = convert_value_to_height(glt_edt, value.z,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y4 = convert_value_to_height(glt_edt, value.z - random.z,
                            canvas_pos.y, canvas_size.y, min, max);

                        y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                        y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                        if (y1 != y2)
                            crv_edt->draw_list->AddRectFilled({ x1, y1 },
                                { x2, y2 }, key_random_range_color, 0.0f, 0);

                        y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                        y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                        if (y3 != y4)
                            crv_edt->draw_list->AddRectFilled({ x1, y3 },
                                { x2, y4 }, random_range_color, 0.0f, 0);
                    }
                    else {
                        float_t y1 = convert_value_to_height(glt_edt, value.y,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y2 = convert_value_to_height(glt_edt, value.y - random.y,
                            canvas_pos.y, canvas_size.y, min, max);

                        y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                        y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                        if (y1 != y2)
                            crv_edt->draw_list->AddRectFilled({ x1, y1 },
                                { x2, y2 }, random_range_color, 0.0f, 0);
                    }
            }
            else {
                float_t c_tangent2 = c->tangent2;
                float_t n_tangent1 = n->tangent1;

                vec3 c_frame_vec = { c_frame, c_frame, c_frame };
                vec3 n_frame_vec = { n_frame, n_frame, n_frame };
                vec3 c_value_vec = { c_value + c_random_range, c_value, c_value - c_random_range };
                vec3 n_value_vec = { n_value + n_random_range, n_value, n_value - n_random_range };
                vec3 random_vec = { random_range, random_range, random_range };
                vec3 c_tangent2_vec = { c_tangent2, c_tangent2, c_tangent2 };
                vec3 n_tangent1_vec = { n_tangent1, n_tangent1, n_tangent1 };
                for (size_t i = c->frame; i < n->frame; i++)
                    for (size_t j = 0; j < frame_width_int; j++) {
                        float_t frame = (float_t)i + (float_t)j / frame_width;
                        vec3 frame_vec = { frame, frame, frame };

                        vec3 value;
                        if (k_type != Glitter::KEY_HERMITE)
                            interpolate_linear_value_vec3(&c_value_vec, &n_value_vec,
                                &c_frame_vec, &n_frame_vec, &frame_vec, &value);
                        else
                            interpolate_chs_value_vec3(&c_value_vec, &n_value_vec,
                                &c_tangent2_vec, &n_tangent1_vec,
                                &c_frame_vec, &n_frame_vec, &frame_vec, &value);

                        vec3 random;
                        if (random_range_mult) {
                            vec3_mult(random_vec, value, random);
                            if (!glt_type_ft)
                                vec3_mult_scalar(random, 0.01f, random);
                        }
                        else
                            random = random_vec;

                        if (random_range_mult) {
                            vec3 random = { random_range, random_range, random_range };
                            vec3_mult(random, value, random);
                            if (!glt_type_ft)
                                vec3_mult_scalar(random, 0.01f, random);
                        }

                        float_t x = canvas_pos.x
                            + (float_t)(i - start_time) * frame_width + (float_t)j;
                        if (value.x != value.y) {
                            float_t y1 = convert_value_to_height(glt_edt, value.x + random.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y2 = convert_value_to_height(glt_edt, value.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y3 = convert_value_to_height(glt_edt, value.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y4 = convert_value_to_height(glt_edt, value.y,
                                canvas_pos.y, canvas_size.y, min, max);

                            y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                            y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                            if (y1 != y2)
                                crv_edt->draw_list->AddLine({ x, y1 },
                                    { x, y2 }, random_range_color, 0.0f);

                            y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                            y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                            if (y3 != y4)
                                crv_edt->draw_list->AddLine({ x, y3 },
                                    { x, y4 }, key_random_range_color, 0.0f);
                        }
                        else {
                            float_t y1 = convert_value_to_height(glt_edt, value.y + random.y,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y2 = convert_value_to_height(glt_edt, value.y,
                                canvas_pos.y, canvas_size.y, min, max);

                            y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                            y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                            if (y1 != y2)
                                crv_edt->draw_list->AddLine({ x, y1 },
                                    { x, y2 }, random_range_color, 0.0f);
                        }

                        if (random_range_negate)
                            if (value.y != value.z) {
                                float_t y1 = convert_value_to_height(glt_edt, value.y,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y2 = convert_value_to_height(glt_edt, value.z,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y3 = convert_value_to_height(glt_edt, value.z,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y4 = convert_value_to_height(glt_edt, value.z - random.z,
                                    canvas_pos.y, canvas_size.y, min, max);

                                y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                                y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                                if (y1 != y2)
                                    crv_edt->draw_list->AddLine({ x, y1 },
                                        { x, y2 }, key_random_range_color, 0.0f);

                                y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                                y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                                if (y3 != y4)
                                    crv_edt->draw_list->AddLine({ x, y3 },
                                        { x, y4 }, random_range_color, 0.0f);
                            }
                            else {
                                float_t y1 = convert_value_to_height(glt_edt, value.y,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y2 = convert_value_to_height(glt_edt, value.y - random.y,
                                    canvas_pos.y, canvas_size.y, min, max);

                                y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                                y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                                if (y1 != y2)
                                    crv_edt->draw_list->AddLine({ x, y1 },
                                        { x, y2 }, random_range_color, 0.0f);
                            }
                    }
            }
        }
    }

    for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++) {
        if (i->frame >= end_time || keys->end() - i <= 1)
            break;

        std::vector<Glitter::Curve::Key>::iterator c = i;
        std::vector<Glitter::Curve::Key>::iterator n = i + 1;
        float_t x1 = canvas_pos.x + (float_t)(c->frame - start_time) * frame_width;
        float_t x2 = canvas_pos.x + (float_t)(n->frame - start_time) * frame_width;

        if (((x1 < canvas_pos_min.x) && (x2 < canvas_pos_min.x)) ||
            (x1 > canvas_pos_max.x) && (x2 > canvas_pos_max.x))
            continue;

        float_t c_value = c->value;
        float_t n_value = n->value;

        if (fix_rot_z) {
            c_value -= (float_t)M_PI_2;
            n_value -= (float_t)M_PI_2;
        }

        if (i->type != Glitter::KEY_HERMITE) {
            float_t y1 = convert_value_to_height(glt_edt, c_value, canvas_pos.y, canvas_size.y, min, max);
            float_t y2 = convert_value_to_height(glt_edt, n_value, canvas_pos.y, canvas_size.y, min, max);

            if (i->type == Glitter::KEY_CONSTANT) {
                int32_t count = 0;
                ImVec2 points_temp[3];
                if (x1 != x2)
                    points_temp[count++] = { x1, y1 };
                points_temp[count++] = { x2, y1 };
                if (y1 != y2)
                    points_temp[count++] = { x2, y2 };

                if (count > 1)
                    crv_edt->draw_list->AddPolyline(points_temp,
                        count, default_color, 0, 3.0f);
            }
            else
                crv_edt->draw_list->AddLine({ x1, y1 }, { x2, y2 }, default_color, 3.0f);
        }
        else {
            float_t c_frame = (float_t)c->frame;
            float_t n_frame = (float_t)n->frame;
            float_t c_tangent2 = (float_t)c->tangent2;
            float_t n_tangent1 = (float_t)n->tangent1;
            size_t frame_width_int = (size_t)roundf(frame_width);
            size_t points_count = ((size_t)n_frame - (size_t)c_frame) * frame_width_int;
            ImVec2* points = force_malloc_s(ImVec2, points_count);
            for (size_t i = c->frame; i < n->frame; i++)
                for (size_t j = 0; j < frame_width_int; j++) {
                    float_t value = interpolate_chs_value(c_value, n_value, c_tangent2, n_tangent1,
                        c_frame, n_frame, (float_t)i + (float_t)j / frame_width);

                    ImVec2 point;
                    point.x = canvas_pos.x
                        + (float_t)(i - start_time) * frame_width + (float_t)j;
                    point.y = convert_value_to_height(glt_edt, value,
                        canvas_pos.y, canvas_size.y, min, max);
                    points[(i - c->frame) * frame_width_int + j] = point;
                }
            crv_edt->draw_list->AddPolyline(points,
                (int32_t)points_count, default_color, 0, 3.0f);
            free(points);
        }
    }

    const ImU32 key_constant_color = ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 0.75f });
    const ImU32 key_linear_color = ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 0.75f });
    const ImU32 key_hermite_color = ImGui::GetColorU32({ 0.0f, 0.0f, 1.0f, 0.75f });

    bool dragged = false;
    bool holding_tan = false;
    for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++) {
        if (!(i->frame >= start_time || i->frame <= end_time))
            continue;
        else if (i - keys->begin() > 1)
            if (i->frame < start_time && i[1].frame < start_time)
                continue;
            else if (i->frame > end_time && i[-1].frame > end_time)
                continue;

        float_t base_value = i->value;
        if (fix_rot_z)
            base_value -= (float_t)M_PI_2;
        float_t x = canvas_pos.x + ((size_t)i->frame - start_time) * frame_width;
        float_t y = convert_value_to_height(glt_edt, base_value, canvas_pos.y, canvas_size.y, min, max);

        if (!(x >= p3.x - 10.0f && x <= p3.x + canvas_size.x + 10.0f)
            || !(y >= p3.y - 10.0f && y <= p3.y + canvas_size.y + 10.0f))
            continue;

        ImU32 value_color;
        switch (i->type) {
        case Glitter::KEY_CONSTANT:
            value_color = key_constant_color;
            break;
        case Glitter::KEY_LINEAR:
        default:
            value_color = key_linear_color;
            break;
        case Glitter::KEY_HERMITE:
            value_color = key_hermite_color;
            break;
        }

        ImGui::PushID(i._Ptr);
        ImGui::SetCursorScreenPos({ x - 7.5f, y - 7.5f });
        ImGui::InvisibleButton("val", { 15.0f, 15.0f }, 0);

        if (can_drag && ImGui::IsItemActive()
            && ImGui::IsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
            float_t old_y = y;

            static int32_t base_frame;
            static float_t base_y;
            if ((ImGui::IsKeyPressed(GLFW_KEY_LEFT_SHIFT, false) && !ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || (ImGui::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT, false) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT))) {
                base_frame = i->frame;
                base_y = base_value;
            }

            if (i->frame >= start_time && i->frame <= end_time) {
                int32_t start = i - keys->begin() > 0 ? (i - 1)->frame + 1 : start_time;
                int32_t end = keys->end() - i > 1 ? (i + 1)->frame - 1 : end_time;
                crv_edt->frame = (int32_t)roundf((io.MousePos.x - canvas_pos.x) / frame_width);
                crv_edt->frame = clamp(crv_edt->frame + start_time, start, end);
                if (i->frame != crv_edt->frame) {
                    i->frame = crv_edt->frame;
                    changed = true;
                }
            }

            static float_t mouse_delta_history;
            if ((ImGui::IsKeyPressed(GLFW_KEY_LEFT_SHIFT, false) && !ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || (ImGui::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT, false) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT)))
                mouse_delta_history = io.MouseDelta.y;
            else if ((ImGui::IsKeyReleased(GLFW_KEY_LEFT_SHIFT) && !ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || ImGui::IsKeyReleased(GLFW_KEY_RIGHT_SHIFT) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
                y += mouse_delta_history;
                y += io.MouseDelta.y;
            }
            else if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                mouse_delta_history += io.MouseDelta.y;
            else
                y += io.MouseDelta.y;

            if (old_y != y) {
                float_t value = convert_height_to_value(glt_edt,
                    y, canvas_pos.y, canvas_size.y, min, max);
                dragged = true;

                if (base_value != value) {
                    if (fix_rot_z)
                        value += (float_t)M_PI_2;
                    i->value = value;
                    changed = true;
                }
                crv_edt->key = &i[0];
            }
        }

        if (i - keys->begin() > 0 && (i - 1)->type == Glitter::KEY_HERMITE) {
            ImU32 tangent1_color;
            float_t tangent1;
            ImVec2 circle_pos;
            circle_pos.x = x - 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(glt_edt, tangent1 = i->tangent1,
                canvas_pos.y, canvas_size.y, min, max);
            crv_edt->draw_list->AddLine({ x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                ImGui::SetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                ImGui::InvisibleButton("tan1", { 15.0f, 15.0f }, 0);

                if (can_drag && ImGui::IsItemActive()) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        i->tangent1 = convert_height_to_value(glt_edt,
                            circle_pos.y + io.MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent1 != tangent1)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent1_color = selected_color;
                }
                else
                    tangent1_color = default_color;

                crv_edt->draw_list->AddCircleFilled(circle_pos,
                    crv_edt->key_radius_in, tangent1_color, 12);
            }
        }

        if (keys->end() - i > 1 && i->type == Glitter::KEY_HERMITE) {
            ImU32 tangent2_color;
            float_t tangent2;
            ImVec2 circle_pos;
            circle_pos.x = x + 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(glt_edt, tangent2 = i->tangent2,
                canvas_pos.y, canvas_size.y, min, max);
            crv_edt->draw_list->AddLine({ x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                ImGui::SetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                ImGui::InvisibleButton("tan2", { 15.0f, 15.0f }, 0);

                if (can_drag && ImGui::IsItemActive()) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        i->tangent2 = convert_height_to_value(glt_edt,
                            circle_pos.y + io.MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent2 != tangent2)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent2_color = selected_color;
                }
                else
                    tangent2_color = default_color;

                crv_edt->draw_list->AddCircleFilled(circle_pos,
                    crv_edt->key_radius_in, tangent2_color, 12);
            }
        }

        crv_edt->draw_list->AddCircleFilled({ x, y },
            crv_edt->key_radius_out, crv_edt->key == &i[0] ? selected_color : default_color, 12);
        crv_edt->draw_list->AddCircleFilled({ x, y },
            crv_edt->key_radius_in, value_color, 12);
        ImGui::PopID();
    }

    if (!crv_edt->key_edit && can_drag && ImGui::IsMouseDown(ImGuiMouseButton_Left) && (!holding_tan || dragged)) {
        int32_t frame = (int32_t)roundf((io.MousePos.x - canvas_pos.x) / frame_width);
        crv_edt->frame = clamp(frame + start_time, start_time, end_time);
        if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT)) {
            crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt, curve);
            crv_edt->frame = crv_edt->key->frame;
        }
        else
            crv_edt->key = glitter_editor_curve_editor_get_selected_key(glt_edt, curve);
    }
    crv_edt->frame = clamp(crv_edt->frame, start_time, end_time);

    int32_t cursor_frame = crv_edt->frame - start_time;
    ImVec2 cursor_pos = canvas_pos;
    cursor_pos.x += cursor_frame * frame_width;
    if (cursor_pos.x >= p1.x - 10.0f && cursor_pos.x <= p1.x + canvas_size.x + 10.0f) {
        ImU32 cursor_color = ImGui::GetColorU32({ 0.8f, 0.8f, 0.8f, 0.75f });
        p1.x = cursor_pos.x;
        p1.y = canvas_pos.y + canvas_size.y;
        crv_edt->draw_list->AddLine(cursor_pos, p1, cursor_color, 2.5f);

        p1 = p2 = p3 = cursor_pos;
        p1.x -= 10.0f;
        p2.y += 10.0f;
        p3.x += 10.0f;
        crv_edt->draw_list->AddTriangleFilled(p1, p2, p3, cursor_color);
    }
    crv_edt->draw_list->PopClipRect();

    if (hovered)
        if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
            crv_edt->timeline_pos -= io.MouseWheel * 25.0f * 4.0f;
        else
            crv_edt->timeline_pos -= io.MouseWheel * 25.0f;

    crv_edt->timeline_pos = clamp(crv_edt->timeline_pos,
        -curve_editor_timeline_base_pos, timeline_max_offset);

    if (crv_edt->list[crv_edt->type] && changed) {
        curve->Recalculate(eg->type);
        *crv_edt->list[crv_edt->type] = *curve;
        glt_edt->input_reload = true;
    }
}

static void glitter_editor_curve_editor_curve_set(glitter_editor_struct* glt_edt,
    Glitter::Curve* curve, Glitter::CurveType type) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;
    curve->type = type;
    curve->repeat = false;
    curve->start_time = 0;
    curve->end_time = 0;
    curve->flags = (Glitter::CurveFlag)0;
    curve->random_range = 0.0f;
    curve->keys.clear();
#if defined(CLOUD_DEV)
    curve->keys_rev.clear();
#endif
    if (eg && eg->type == Glitter::X) {
        curve->version = 2;
        curve->keys_version = 3;
    }
    else {
        curve->version = 1;
        curve->keys_version = 2;
    }
}

static void glitter_editor_curve_editor_curves_reset(glitter_editor_struct* glt_edt) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;
    memset(&crv_edt->list, 0, sizeof(Glitter::Curve*)
        * (Glitter::CURVE_V_SCROLL_ALPHA_2ND - Glitter::CURVE_TRANSLATION_X + 1));
    crv_edt->animation = 0;
    crv_edt->key = {};
}

static Glitter::Curve::Key* glitter_editor_curve_editor_get_closest_key(
    glitter_editor_struct* glt_edt, Glitter::Curve* curve) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    std::vector<Glitter::Curve::Key>::iterator key = {};
    std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;
    for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++) {
        if (i->frame == crv_edt->frame)
            return &i[0];

        if (!key._Ptr || (key._Ptr && abs(i->frame - crv_edt->frame)
            <= abs(key->frame - crv_edt->frame)))
            key = i;
    }
    return &key[0];
}

static Glitter::Curve::Key* glitter_editor_curve_editor_get_selected_key(
    glitter_editor_struct* glt_edt, Glitter::Curve* curve) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;
    for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++)
        if (i->frame == crv_edt->frame)
            return &i[0];
    return 0;
}

static float_t glitter_editor_curve_editor_get_value(glitter_editor_struct* glt_edt, Glitter::CurveType type) {
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    float_t value = curve_base_values[type];
    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        switch (type) {
        case Glitter::CURVE_TRANSLATION_X:
            value = sel_efct->translation.x;
            break;
        case Glitter::CURVE_TRANSLATION_Y:
            value = sel_efct->translation.y;
            break;
        case Glitter::CURVE_TRANSLATION_Z:
            value = sel_efct->translation.z;
            break;
        case Glitter::CURVE_ROTATION_X:
            value = sel_efct->rotation.x;
            break;
        case Glitter::CURVE_ROTATION_Y:
            value = sel_efct->rotation.y;
            break;
        case Glitter::CURVE_ROTATION_Z:
            value = sel_efct->rotation.z;
            break;
        case Glitter::CURVE_SCALE_X:
            value = sel_efct->scale.x;
            break;
        case Glitter::CURVE_SCALE_Y:
            value = sel_efct->scale.y;
            break;
        case Glitter::CURVE_SCALE_Z:
            value = sel_efct->scale.z;
            break;
        case Glitter::CURVE_SCALE_ALL:
            break;
        }
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        switch (type) {
        case Glitter::CURVE_TRANSLATION_X:
            value = sel_emit->translation.x;
            break;
        case Glitter::CURVE_TRANSLATION_Y:
            value = sel_emit->translation.y;
            break;
        case Glitter::CURVE_TRANSLATION_Z:
            value = sel_emit->translation.z;
            break;
        case Glitter::CURVE_ROTATION_X:
            value = sel_emit->rotation.x;
            break;
        case Glitter::CURVE_ROTATION_Y:
            value = sel_emit->rotation.y;
            break;
        case Glitter::CURVE_ROTATION_Z:
            value = sel_emit->rotation.z;
            break;
        case Glitter::CURVE_SCALE_X:
            value = sel_emit->scale.x;
            break;
        case Glitter::CURVE_SCALE_Y:
            value = sel_emit->scale.y;
            break;
        case Glitter::CURVE_SCALE_Z:
            value = sel_emit->scale.z;
            break;
        case Glitter::CURVE_EMISSION_INTERVAL:
            value = sel_emit->data.emission_interval;
            break;
        case Glitter::CURVE_PARTICLES_PER_EMISSION:
            value = sel_emit->data.particles_per_emission;
            break;
        }
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        switch (type) {
        case Glitter::CURVE_ROTATION_X:
            value = sel_ptcl->data.rotation.x;
            break;
        case Glitter::CURVE_ROTATION_Y:
            value = sel_ptcl->data.rotation.y;
            break;
        case Glitter::CURVE_ROTATION_Z:
            value = sel_ptcl->data.rotation.z;
            break;
        case Glitter::CURVE_SCALE_X:
            value = sel_ptcl->data.scale.x;
            break;
        case Glitter::CURVE_SCALE_Y:
            value = sel_ptcl->data.scale.y;
            break;
        case Glitter::CURVE_SCALE_Z:
            value = sel_ptcl->data.scale.z;
            break;
        case Glitter::CURVE_COLOR_R:
            value = sel_ptcl->data.color.x;
            break;
        case Glitter::CURVE_COLOR_G:
            value = sel_ptcl->data.color.y;
            break;
        case Glitter::CURVE_COLOR_B:
            value = sel_ptcl->data.color.z;
            break;
        case Glitter::CURVE_COLOR_A:
            value = sel_ptcl->data.color.w;
            break;
        }
        break;
    }
    return value;
}

static void glitter_editor_curve_editor_key_manager(glitter_editor_struct* glt_edt,
    std::vector<Glitter::Curve::Key>* keys, bool* add_key, bool* del_key) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    Glitter::Curve* curve = crv_edt->list[crv_edt->type];
    *add_key = true;
    *del_key = false;
    for (std::vector<Glitter::Curve::Key>::iterator i = keys->begin(); i != keys->end(); i++)
        if (i->frame > curve->end_time)
            break;
        else if (i->frame == crv_edt->frame) {
            *add_key = false;
            *del_key = true;
            break;
        }
}

static void glitter_editor_curve_editor_property_window(glitter_editor_struct* glt_edt,
    class_data* data) {
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    crv_edt->key_edit = false;
    if (crv_edt->type < Glitter::CURVE_TRANSLATION_X
        || crv_edt->type > Glitter::CURVE_V_SCROLL_ALPHA_2ND)
        return;

    float_t scale;
    float_t inv_scale;
    float_t min;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Curve* curve = crv_edt->list[crv_edt->type];
    if (!curve)
        return;

    bool fix_rot_z = eg->type != Glitter::X && curve->type == Glitter::CURVE_ROTATION_Z
        && ((glt_edt->selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && sel_emit->data.direction == Glitter::DIRECTION_Z_AXIS)
            || (glt_edt->selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && sel_ptcl->data.draw_type == Glitter::DIRECTION_Z_AXIS));

    if (curve->type >= Glitter::CURVE_ROTATION_X && curve->type <= Glitter::CURVE_ROTATION_Z) {
        scale = RAD_TO_DEG_FLOAT;
        inv_scale = DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= Glitter::CURVE_COLOR_R && curve->type <= Glitter::CURVE_COLOR_RGB_SCALE_2ND) {
        scale = 255.0f;
        inv_scale = (float_t)(1.0 / 255.0);
    }
    else {
        scale = 1.0f;
        inv_scale = 1.0f;
    }

    if (curve->type == Glitter::CURVE_EMISSION_INTERVAL)
        min = -1.0f;
    else if (curve->type == Glitter::CURVE_PARTICLES_PER_EMISSION)
        min = 0.0f;
    else
        min = -FLT_MAX;

    bool random_range = curve->flags & Glitter::CURVE_RANDOM_RANGE ? true : false;
    bool key_random_range = curve->flags & Glitter::CURVE_KEY_RANDOM_RANGE ? true : false;

    bool changed = false;
    bool key_edit = false;

    float_t value = 0.0f;
    if (curve->keys_rev.size() > 1) {
        std::vector<Glitter::Curve::Key>::iterator c = curve->keys_rev.begin();
        std::vector<Glitter::Curve::Key>::iterator n = c + 1;
        while (n != curve->keys_rev.end() && n->frame < crv_edt->frame) {
            c++;
            n++;
        }

        if (c + 1 != curve->keys_rev.end())
            switch (c->type) {
            case Glitter::KEY_CONSTANT:
                value = n->frame == crv_edt->frame ? n->value : c->value;
                break;
            case Glitter::KEY_LINEAR:
                value = interpolate_linear_value(c->value, n->value,
                    (float_t)c->frame, (float_t)n->frame, (float_t)crv_edt->frame);
                break;
            case Glitter::KEY_HERMITE:
                value = interpolate_chs_value(c->value, n->value, c->tangent2, n->tangent1,
                    (float_t)c->frame, (float_t)n->frame, (float_t)crv_edt->frame);
                break;
            }
    }
    else if (curve->keys_rev.size() == 1)
        value = curve->keys_rev.data()[0].value;
    else if (curve->type >= Glitter::CURVE_TRANSLATION_X && curve->type <= Glitter::CURVE_V_SCROLL_ALPHA_2ND)
        value = glitter_editor_curve_editor_get_value(glt_edt, curve->type);
    else
        value = 0.0f;

    value *= scale;
    ImGui::Text("Frame: %d; Value: %g", crv_edt->frame, value);

    if (crv_edt->key && ImGui::TreeNodeEx("Key", ImGuiTreeNodeFlags_DefaultOpen)) {
        Glitter::Curve::Key* c = crv_edt->key;
        Glitter::Curve::Key* n = curve->keys_rev.data() + curve->keys_rev.size()
            - crv_edt->key > 1 ? &crv_edt->key[1] : 0;
        if (imguiColumnComboBox("Type", Glitter::key_name,
            Glitter::KEY_HERMITE,
            (int32_t*)&c->type, 0, true, &data->imgui_focus))
            changed = true;
        key_edit |= ImGui::IsItemFocused();

        float_t value = c->value;
        if (fix_rot_z)
            value -= (float_t)M_PI_2;
        value *= scale;
        if (imguiColumnDragFloat("Value",
            &value, 0.0001f, min, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            value *= inv_scale;
            if (fix_rot_z)
                value += (float_t)M_PI_2;
            c->value = value;
            changed = true;
        }
        key_edit |= ImGui::IsItemFocused();

        imguiDisableElementPush(key_random_range);
        float_t random = c->random_range * scale;
        if (imguiColumnDragFloat("Random Range",
            &random, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            random *= inv_scale;
            if (random != c->random_range) {
                c->random_range = random;
                changed = true;
            }
        }
        key_edit |= ImGui::IsItemFocused();
        imguiDisableElementPop(key_random_range);

        if (n && c->type == Glitter::KEY_HERMITE) {
            float_t tangent2 = c->tangent2 * scale;
            if (imguiColumnDragFloat("Tangent 1",
                &tangent2, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                c->tangent2 = tangent2 * inv_scale;
                changed = true;
            }
            key_edit |= ImGui::IsItemFocused();

            float_t tangent1 = n->tangent1 * scale;
            if (imguiColumnDragFloat("Tangent 2",
                &tangent1, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                n->tangent1 = tangent1 * inv_scale;
                changed = true;
            }
            key_edit |= ImGui::IsItemFocused();
        }
        ImGui::TreePop();
    }
    crv_edt->key_edit = key_edit;

    if (ImGui::TreeNodeEx("Curve", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (imguiCheckbox("Repeat", &curve->repeat))
            changed = true;

        int32_t start_time = curve->start_time;
        if (imguiColumnDragInt("Start Time",
            &start_time, 1, 0, 0x7FFF, "%d", 0)) {
            if (curve->end_time < start_time) {
                curve->end_time = start_time;
                changed = true;
            }

            if (curve->start_time != start_time) {
                curve->start_time = start_time;
                changed = true;
            }
        }

        int32_t end_time = curve->end_time;
        if (imguiColumnDragInt("End Time",
            &end_time, 1, 0, 0x7FFF, "%d", 0)) {
            if (curve->start_time > end_time)
                end_time = curve->start_time;

            if (curve->end_time != end_time) {
                curve->end_time = end_time;
                changed = true;
            }
        }

        imguiDisableElementPush(random_range);
        float_t random = curve->random_range * scale;
        if (imguiColumnDragFloat("Random Range",
            &random, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            random *= inv_scale;
            if (random != curve->random_range) {
                curve->random_range = random;
                changed = true;
            }
        }
        imguiDisableElementPop(random_range);

        int32_t c_type = curve->version;
        ImGui::Text("C Type:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f);
        if (ImGui::InputInt("##C Type:", &c_type, 1, 1)) {
            if (eg->type == Glitter::X)
                c_type = clamp(c_type, 1, 2);
            else
                c_type = clamp(c_type, 0, 1);

            if (c_type != curve->version)
                changed = true;
            curve->version = c_type;
        }

        int32_t k_type = curve->keys_version;
        ImGui::SameLine();
        ImGui::Text("K Type:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f);
        if (ImGui::InputInt("##K Type:", &k_type, 1, 1)) {
            if (eg->type == Glitter::X)
                k_type = clamp(k_type, 2, 3);
            else
                k_type = clamp(k_type, 0, 2);

            if (k_type != curve->keys_version)
                changed = true;
            curve->keys_version = k_type;
        }

        ImGui::Separator();

        ImGui::PushID("Flags");
        if (imguiCheckboxFlags("Baked",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_BAKED))
            changed = true;

        if (eg->type == Glitter::X) {
            imguiDisableElementPush(curve->flags & Glitter::CURVE_BAKED ? true : false);
            if (imguiCheckboxFlags("Baked Full",
                (uint32_t*)&curve->flags,
                Glitter::CURVE_BAKED_FULL))
                changed = true;
            imguiDisableElementPop(curve->flags & Glitter::CURVE_BAKED ? true : false);
        }

        ImGui::Separator();

        if (imguiCheckboxFlags("Random Range",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_RANDOM_RANGE))
            changed = true;

        if (imguiCheckboxFlags("Key Random Range",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_KEY_RANDOM_RANGE))
            changed = true;

        if (imguiCheckboxFlags("Random Range Negate",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_RANDOM_RANGE_NEGATE))
            changed = true;

        if (imguiCheckboxFlags("Step Random",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_STEP))
            changed = true;

        if (imguiCheckboxFlags("Random Negate",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_NEGATE))
            changed = true;

        if (imguiCheckboxFlags(eg->type == Glitter::FT
            ? "Scale Random by Value" : "Scale Random by Value * 0.01",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_RANDOM_RANGE_MULT))
            changed = true;
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (crv_edt->list[crv_edt->type]) {
        if (changed) {
            curve->Recalculate(eg->type);
            glt_edt->input_reload = true;
        }

        uint64_t hash1 = hash_fnv1a64m(crv_edt->list[crv_edt->type], sizeof(Glitter::Curve));
        uint64_t hash2 = hash_fnv1a64m(curve, sizeof(Glitter::Effect));
        if (hash1 != hash2)
            *crv_edt->list[crv_edt->type] = *curve;
    }
}

static void glitter_editor_curve_editor_reset_state(glitter_editor_struct* glt_edt, Glitter::CurveType type) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    if (crv_edt->type != type)
        crv_edt->type = type;

    crv_edt->frame_width = 16;
    crv_edt->zoom_time = 1.0f;
    crv_edt->prev_zoom_time = 1.0f;
    crv_edt->zoom_value = 1.0f;
    crv_edt->key_radius_in = 6.0f;
    crv_edt->key_radius_out = 8.0f;
    crv_edt->height_offset = 20.0f;
    crv_edt->frame = 0;

    crv_edt->timeline_pos = -curve_editor_timeline_base_pos;

    switch (type) {
    case Glitter::CURVE_TRANSLATION_X:
    case Glitter::CURVE_TRANSLATION_Y:
    case Glitter::CURVE_TRANSLATION_Z:
        crv_edt->range = 25.0f;
        break;
    case Glitter::CURVE_ROTATION_X:
    case Glitter::CURVE_ROTATION_Y:
    case Glitter::CURVE_ROTATION_Z:
        crv_edt->range = 360.0f;
        break;
    case Glitter::CURVE_SCALE_X:
    case Glitter::CURVE_SCALE_Y:
    case Glitter::CURVE_SCALE_Z:
    case Glitter::CURVE_SCALE_ALL:
        crv_edt->range = 5.0f;
        break;
    case Glitter::CURVE_COLOR_R:
    case Glitter::CURVE_COLOR_G:
    case Glitter::CURVE_COLOR_B:
    case Glitter::CURVE_COLOR_A:
    case Glitter::CURVE_COLOR_RGB_SCALE:
    case Glitter::CURVE_COLOR_R_2ND:
    case Glitter::CURVE_COLOR_G_2ND:
    case Glitter::CURVE_COLOR_B_2ND:
    case Glitter::CURVE_COLOR_A_2ND:
    case Glitter::CURVE_COLOR_RGB_SCALE_2ND:
        crv_edt->range = 255.0f;
        break;
    case Glitter::CURVE_EMISSION_INTERVAL:
        crv_edt->range = 20.0f;
        break;
    case Glitter::CURVE_PARTICLES_PER_EMISSION:
        crv_edt->range = 50.0f;
        break;
    case Glitter::CURVE_U_SCROLL:
    case Glitter::CURVE_V_SCROLL:
    case Glitter::CURVE_U_SCROLL_ALPHA:
    case Glitter::CURVE_V_SCROLL_ALPHA:
    case Glitter::CURVE_U_SCROLL_2ND:
    case Glitter::CURVE_V_SCROLL_2ND:
    case Glitter::CURVE_U_SCROLL_ALPHA_2ND:
    case Glitter::CURVE_V_SCROLL_ALPHA_2ND:
        crv_edt->range = 1.0f;
        break;
    default:
        crv_edt->range = 1.0f;
        break;
    }

    crv_edt->add_key = false;
    crv_edt->del_key = false;
}

static void glitter_editor_curve_editor_selector(glitter_editor_struct* glt_edt, class_data* data) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;
    int32_t sel_rsrc = glt_edt->selected_resource;
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::CurveTypeFlags flags = (Glitter::CurveTypeFlags)0;
    Glitter::Animation* anim = 0;
    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        flags = Glitter::effect_curve_flags;
        anim = &sel_efct->animation;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        flags = Glitter::emitter_curve_flags;
        anim = &sel_emit->animation;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (eg->type == Glitter::X)
            flags = Glitter::particle_x_curve_flags;
        else
            flags = Glitter::particle_curve_flags;
        anim = &sel_ptcl->animation;

        if (sel_ptcl->data.type != Glitter::PARTICLE_MESH) {
            enum_and(flags, ~Glitter::CURVE_TYPE_UV_SCROLL_2ND);
            if (sel_ptcl->data.draw_type != Glitter::DIRECTION_PARTICLE_ROTATION)
                enum_and(flags, ~(Glitter::CURVE_TYPE_ROTATION_X | Glitter::CURVE_TYPE_ROTATION_Y));
        }
        break;
    default:
        return;
    }
    crv_edt->animation = anim;

    for (int32_t i = Glitter::CURVE_TRANSLATION_X; i <= Glitter::CURVE_V_SCROLL_ALPHA_2ND; i++) {
        bool found = false;
        std::vector<Glitter::Curve*>::iterator j;
        for (j = anim->curves.begin(); j != anim->curves.end(); j++)
            if (*j && (*j)->type == i) {
                found = true;
                break;
            }
        crv_edt->list[i] = found ? *j : 0;
    }

    bool reset = false;
    ImVec2 cont_reg_avail = ImGui::GetContentRegionAvail();
    if (ImGui::BeginListBox("##Curves", { cont_reg_avail.x * 0.5f, cont_reg_avail.y })) {
        ImGuiTreeNodeFlags tree_node_flags = 0;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

#define LIST_BOX(label, curve_type) \
if (flags & Glitter::CURVE_TYPE_##curve_type){ \
    const char* _l =  crv_edt->list[Glitter::CURVE_##curve_type] ? label" [*]" : label; \
    ImGui::Selectable(_l, crv_edt->type == Glitter::CURVE_##curve_type); \
\
    if (ImGui::IsItemFocused() && crv_edt->type != Glitter::CURVE_##curve_type) { \
        glitter_editor_curve_editor_reset_state(glt_edt, Glitter::CURVE_##curve_type); \
        reset = true; \
    } \
}

        if (flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ
            && ImGui::TreeNodeEx("Translation", tree_node_flags)) {
            ImGui::PushID("T");
            LIST_BOX("X", TRANSLATION_X);
            LIST_BOX("Y", TRANSLATION_Y);
            LIST_BOX("Z", TRANSLATION_Z);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & Glitter::CURVE_TYPE_ROTATION_XYZ
            && ImGui::TreeNodeEx("Rotation", tree_node_flags)) {
            ImGui::PushID("R");
            LIST_BOX("X", ROTATION_X);
            LIST_BOX("Y", ROTATION_Y);
            LIST_BOX("Z", ROTATION_Z);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_SCALE_XYZ | Glitter::CURVE_TYPE_SCALE_ALL)
            && ImGui::TreeNodeEx("Scale", tree_node_flags)) {
            ImGui::PushID("S");
            LIST_BOX("X", SCALE_X);
            LIST_BOX("Y", SCALE_Y);
            LIST_BOX("Z", SCALE_Z);
            LIST_BOX("All", SCALE_ALL);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_COLOR_RGBA | Glitter::CURVE_TYPE_COLOR_RGB_SCALE)
            && ImGui::TreeNodeEx("Color", tree_node_flags)) {
            ImGui::PushID("C");
            LIST_BOX("R", COLOR_R);
            LIST_BOX("G", COLOR_G);
            LIST_BOX("B", COLOR_B);
            LIST_BOX("A", COLOR_A);
            LIST_BOX("RGB Scale", COLOR_RGB_SCALE);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_COLOR_RGBA_2ND | Glitter::CURVE_TYPE_COLOR_RGB_SCALE_2ND)
            && ImGui::TreeNodeEx("Color 2nd", tree_node_flags)) {
            ImGui::PushID("C2");
            LIST_BOX("R", COLOR_R_2ND);
            LIST_BOX("G", COLOR_G_2ND);
            LIST_BOX("B", COLOR_B_2ND);
            LIST_BOX("A", COLOR_A_2ND);
            LIST_BOX("RGB Scale", COLOR_RGB_SCALE_2ND);
            ImGui::PopID();
            ImGui::TreePop();
        }

        LIST_BOX("Emit Interval", EMISSION_INTERVAL);
        LIST_BOX("PTC Per Emit", PARTICLES_PER_EMISSION);

        if (flags & (Glitter::CURVE_TYPE_UV_SCROLL | Glitter::CURVE_TYPE_UV_SCROLL_ALPHA)
            && ImGui::TreeNodeEx("UV Scroll", tree_node_flags)) {
            ImGui::PushID("UV");
            LIST_BOX("U", U_SCROLL);
            LIST_BOX("V", V_SCROLL);
            LIST_BOX("U Alpha", U_SCROLL_ALPHA);
            LIST_BOX("V Alpha", V_SCROLL_ALPHA);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_UV_SCROLL_2ND | Glitter::CURVE_TYPE_UV_SCROLL_ALPHA_2ND)
            && ImGui::TreeNodeEx("UV Scroll 2nd", tree_node_flags)) {
            ImGui::PushID("UV2");
            LIST_BOX("U", U_SCROLL_2ND);
            LIST_BOX("V", V_SCROLL_2ND);
            LIST_BOX("U Alpha", U_SCROLL_ALPHA_2ND);
            LIST_BOX("V Alpha", V_SCROLL_ALPHA_2ND);
            ImGui::PopID();
            ImGui::TreePop();
        }
#undef LIST_BOX
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndListBox();
    }

    if (crv_edt->type < Glitter::CURVE_TRANSLATION_X
        || crv_edt->type > Glitter::CURVE_V_SCROLL_ALPHA_2ND)
        return;

    Glitter::Curve* curve = crv_edt->list[crv_edt->type];

    if (reset && curve)
        crv_edt->key = &glitter_editor_curve_editor_get_closest_key(glt_edt, curve)[0];

    ImGui::SameLine();
    ImGui::BeginGroup();
    imguiSetColumnSpace(3.0f / 7.0f);
    float_t zoom_time = crv_edt->zoom_time * 100.0f;
    imguiColumnSliderFloat("Zoom Time", &zoom_time, 1.0f, 5.0f, 500.0f, "%.1f%%", 0, true);
    crv_edt->zoom_time = zoom_time * 0.01f;

    int32_t zoom_value = (int32_t)roundf(crv_edt->zoom_value * 100.0f);
    imguiColumnSliderInt("Zoom Value", &zoom_value, 10, 1000, "%d%%", 0, true);
    crv_edt->zoom_value = (float_t)zoom_value * 0.01f;

    imguiColumnDragFloat("Range",
        &crv_edt->range, 0.0001f, 0.0f, FLT_MAX, "%g", 0);
    imguiSetDefaultColumnSpace();

    bool add_key = false;
    bool del_key = false;
    if (curve)
        glitter_editor_curve_editor_key_manager(glt_edt, &curve->keys_rev, &add_key, &del_key);

    ImGui::Separator();
    float_t w = imguiGetContentRegionAvailWidth();
    imguiDisableElementPush(add_key);
    if (imguiButton("Add Key", { w, 0.0f }))
        crv_edt->add_key = true;
    imguiDisableElementPop(add_key);

    imguiDisableElementPush(del_key);
    if (imguiButton("Delete Key", { w, 0.0f }))
        crv_edt->del_key = true;
    imguiDisableElementPop(del_key);

    bool has_curve_in_list = curve ? true : false;
    imguiDisableElementPush(!has_curve_in_list);
    if (imguiButton("Add Curve", { w, 0.0f }))
        crv_edt->add_curve = true;
    imguiDisableElementPop(!has_curve_in_list);

    imguiDisableElementPush(has_curve_in_list);
    if (imguiButton("Delete Curve", { w, 0.0f }))
        crv_edt->del_curve = true;
    imguiDisableElementPop(has_curve_in_list);
    ImGui::EndGroup();
}

static bool glitter_editor_hash_input(glitter_editor_struct* glt_edt, const char* label, uint64_t* hash) {
    uint64_t h = *hash;
    if (glt_edt->effect_group->type == Glitter::FT) {
        if (imguiColumnInputScalar(label, ImGuiDataType_U64, &h,
            0, 0, "%016llX", ImGuiInputTextFlags_CharsHexadecimal) && h == 0)
            h = hash_fnv1a64m_empty;
    }
    else {
        if (imguiColumnInputScalar(label, ImGuiDataType_U32, &h,
            0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal) && (uint32_t)h == 0)
            h = hash_murmurhash_empty;
    }

    if (h == *hash)
        return false;

    *hash = h;
    return true;
}

static void glitter_editor_gl_load(glitter_editor_struct* glt_edt) {
    const char* glitter_editor_wireframe_vert =
        "#version 430 core\n"
        "layout (location = 0) in vec3 a_position;\n"
        "\n"
        "uniform mat4 model;\n"
        "\n"
        "//COMMONDATA\n"
        "\n"
        "void main() {\n"
        "    vec4 pos = model * vec4(a_position, 1.0);\n"
        "    gl_Position = COMMON_DATA_VP * pos;\n"
        "}\n";

    const char* glitter_editor_wireframe_frag =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "uniform vec4 color;\n"
        "\n"
        "//COMMONDATA\n"
        "\n"
        "void main(void) {\n"
        "    result = color;\n"
        "}\n";

    shader_glsl_param param;
    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Glitter Editor Wireframe";
    glt_edt->gl_data.wireframe.shader.load(glitter_editor_wireframe_vert,
        glitter_editor_wireframe_frag, 0, &param);
}

static void glitter_editor_gl_draw(glitter_editor_struct* glt_edt) {
    if (!glt_edt->effect_group)
        return;

    if (glt_edt->draw_flags & GLITTER_EDITOR_DRAW_WIREFRAME)
        glitter_editor_gl_draw_wireframe(glt_edt);
}

static void glitter_editor_gl_process(glitter_editor_struct* glt_edt) {
    if (!glt_edt->effect_group) {
        glitter_editor_reset_draw();
        return;
    }

    glitter_editor_gl_select_particle(glt_edt);
}

static void glitter_editor_gl_free(glitter_editor_struct* glt_edt) {
    glt_edt->gl_data.wireframe.shader.unload();
}

static void glitter_editor_gl_draw_wireframe(glitter_editor_struct* glt_edt) {
    Glitter::Scene* sc = Glitter::glt_particle_manager.scene;
    Glitter::EffectInst* eff = Glitter::glt_particle_manager.effect;
    Glitter::EmitterInst* emit = Glitter::glt_particle_manager.emitter;
    Glitter::ParticleInst* ptcl = Glitter::glt_particle_manager.particle;
    if (!Glitter::glt_particle_manager.draw_selected || !eff) {
        if (!sc)
            return;

        size_t count = 0;
        for (Glitter::SceneEffect& i : sc->effects)
            if (i.disp && i.ptr)
                count += glitter_editor_gl_draw_wireframe_calc(glt_edt, i.ptr, 0);

        if (count == 0)
            return;

        for (Glitter::SceneEffect& i : sc->effects)
            if (i.disp && i.ptr)
                glitter_editor_gl_draw_wireframe_draw(glt_edt, i.ptr, 0);
    }
    else if ((eff && ptcl) || (eff && !emit)) {
        size_t count = glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, ptcl);
        if (count == 0)
            return;

        glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, ptcl);
    }
    else if (emit) {
        Glitter::F2EmitterInst* emit_f2 = dynamic_cast<Glitter::F2EmitterInst*>(emit);
        Glitter::XEmitterInst* emit_x = dynamic_cast<Glitter::XEmitterInst*>(emit);
        if (emit_f2) {
            size_t count = 0;
            for (Glitter::F2ParticleInst*& i : emit_f2->particles) {
                if (!i)
                    continue;

                Glitter::F2ParticleInst* particle = i;
                count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, particle);
                for (Glitter::F2ParticleInst*& j : particle->data.children)
                    if (j)
                        count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, j);
            }

            if (count == 0)
                return;

            for (Glitter::F2ParticleInst*& i : emit_f2->particles) {
                if (!i)
                    continue;

                Glitter::F2ParticleInst* particle = i;
                glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, particle);
                for (Glitter::F2ParticleInst*& j : particle->data.children)
                    if (j)
                        glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, j);
            }
        }
        else if (emit_x) {
            size_t count = 0;
            for (Glitter::XParticleInst*& i : emit_x->particles) {
                if (!i)
                    continue;

                Glitter::XParticleInst* particle = i;
                count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, particle);
                for (Glitter::XParticleInst*& j : particle->data.children)
                    if (j)
                        count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, j);
            }

            if (count == 0)
                return;

            for (Glitter::XParticleInst*& i : emit_x->particles) {
                if (!i)
                    continue;

                Glitter::XParticleInst* particle = i;
                glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, particle);
                for (Glitter::XParticleInst*& j : particle->data.children)
                    if (j)
                        glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, j);
            }
        }
    }
}

static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::EffectInst* eff, Glitter::ParticleInst* ptcl) {
    Glitter::F2EffectInst* e = dynamic_cast<Glitter::F2EffectInst*>(eff);
    Glitter::F2ParticleInst* p = dynamic_cast<Glitter::F2ParticleInst*>(ptcl);
    if (e)
        return glitter_editor_gl_draw_wireframe_calc(glt_edt, e, p);

    Glitter::XEffectInst* ex = dynamic_cast<Glitter::XEffectInst*>(eff);
    Glitter::XParticleInst* px = dynamic_cast<Glitter::XParticleInst*>(ptcl);
    if (ex)
        return glitter_editor_gl_draw_wireframe_calc(glt_edt, ex, px);
    return 0;
}

static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::F2EffectInst* eff, Glitter::F2ParticleInst* ptcl) {
    size_t count = 0;
    for (Glitter::F2RenderGroup*& i : eff->render_scene.groups) {
        if (!i)
            continue;

        Glitter::F2RenderGroup* rg = i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp > 0)
            count++;
    }
    return count;
}

static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    Glitter::XEffectInst* eff, Glitter::XParticleInst* ptcl) {
    size_t count = 0;
    for (Glitter::XRenderGroup*& i : eff->render_scene.groups) {
        if (!i)
            continue;

        Glitter::XRenderGroup* rg = i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp > 0)
            count++;
    }
    return count;
}

static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::EffectInst* eff, Glitter::ParticleInst* ptcl) {
    Glitter::F2EffectInst* e = dynamic_cast<Glitter::F2EffectInst*>(eff);
    Glitter::F2ParticleInst* p = dynamic_cast<Glitter::F2ParticleInst*>(ptcl);
    if (e && p) {
        glitter_editor_gl_draw_wireframe_draw(glt_edt, e, p);
        return;
    }

    Glitter::XEffectInst* ex = dynamic_cast<Glitter::XEffectInst*>(eff);
    Glitter::XParticleInst* px = dynamic_cast<Glitter::XParticleInst*>(ptcl);
    if (ex)
        glitter_editor_gl_draw_wireframe_draw(glt_edt, ex, px);
}

static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::F2EffectInst* eff, Glitter::F2ParticleInst* ptcl) {
    gl_state_disable_blend();
    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_LINE);

    gl_state_bind_vertex_array(glt_edt->vao);
    glt_edt->gl_data.wireframe.shader.use();
    shader_glsl_set_vec4_value(&glt_edt->gl_data.wireframe.shader, "color", 1.0f, 1.0f, 0.0f, 1.0f);
    for (Glitter::F2RenderGroup*& i : eff->render_scene.groups) {
        if (!i)
            continue;

        Glitter::F2RenderGroup* rg = i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp < 1)
            continue;

        shader_glsl_set_mat4(&glt_edt->gl_data.wireframe.shader, "model", false, rg->mat_draw);

        switch (rg->type) {
        case Glitter::PARTICLE_QUAD: {
            static const GLsizei buffer_size = sizeof(Glitter::Buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Glitter::Buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            gl_state_bind_element_array_buffer(rg->ebo);
            glDrawElements(GL_TRIANGLES, (GLsizei)(6 * rg->disp), GL_UNSIGNED_INT, 0);
            gl_state_bind_element_array_buffer(0);

            glDisableVertexAttribArray(0);
        } break;
        case Glitter::PARTICLE_LINE:
        case Glitter::PARTICLE_LOCUS: {
            static const GLsizei buffer_size = sizeof(Glitter::Buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Glitter::Buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            const GLenum mode = rg->type == Glitter::PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            for (std::pair<GLint, GLsizei>& i : rg->draw_list)
                glDrawArrays(mode, i.first, i.second);

            glDisableVertexAttribArray(0);
        } break;
        }
    }
    gl_state_bind_vertex_array(0);
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    gl_state_use_program(0);
}

static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    Glitter::XEffectInst* eff, Glitter::XParticleInst* ptcl) {
    gl_state_disable_blend();
    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_LINE);

    gl_state_bind_vertex_array(glt_edt->vao);
    glt_edt->gl_data.wireframe.shader.use();
    shader_glsl_set_vec4_value(&glt_edt->gl_data.wireframe.shader, "color", 1.0f, 1.0f, 0.0f, 1.0f);
    for (Glitter::XRenderGroup*& i : eff->render_scene.groups) {
        if (!i)
            continue;

        Glitter::XRenderGroup* rg = i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp < 1)
            continue;

        shader_glsl_set_mat4(&glt_edt->gl_data.wireframe.shader, "model", false, rg->mat_draw);

        switch (rg->type) {
        case Glitter::PARTICLE_QUAD: {
            static const GLsizei buffer_size = sizeof(Glitter::Buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Glitter::Buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            gl_state_bind_element_array_buffer(rg->ebo);
            glDrawElements(GL_TRIANGLES, (GLsizei)(6 * rg->disp), GL_UNSIGNED_INT, 0);
            gl_state_bind_element_array_buffer(0);

            glDisableVertexAttribArray(0);
        } break;
        case Glitter::PARTICLE_LINE:
        case Glitter::PARTICLE_LOCUS: {
            static const GLsizei buffer_size = sizeof(Glitter::Buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Glitter::Buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            const GLenum mode = rg->type == Glitter::PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            for (std::pair<GLint, GLsizei>& i : rg->draw_list)
                glDrawArrays(mode, i.first, i.second);

            glDisableVertexAttribArray(0);
        } break;
        case Glitter::PARTICLE_MESH:
            glitter_editor_gl_draw_wireframe_draw_mesh(glt_edt, rg);
            break;
        }
    }
    gl_state_bind_vertex_array(0);
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    gl_state_use_program(0);
}

static void glitter_editor_gl_draw_wireframe_draw_mesh(glitter_editor_struct* glt_edt,
    Glitter::XRenderGroup* rg) {
    Glitter::RenderElement* elem = rg->elements;
    for (size_t i = rg->ctrl, j_max = 1024; i > 0; i -= j_max) {
        j_max = min(i, j_max);
        for (size_t j = j_max; j > 0; elem++) {
            if (!elem->alive)
                continue;
            j--;

            if (!elem->disp)
                continue;

            render_context* rctx = (render_context*)Glitter::glt_particle_manager.rctx;
            object_data* object_data = &rctx->object_data;

            Glitter::Particle* particle = rg->particle->data.particle;
            object_info object_info;
            object_info.set_id = (uint32_t)particle->data.mesh.object_set_name_hash;
            object_info.id = (uint32_t)particle->data.mesh.object_name_hash;
            obj* obj = object_storage_get_obj(object_info);
            obj_mesh_vertex_buffer* obj_vertex_buffer
                = object_storage_get_obj_mesh_vertex_buffer(object_info);
            obj_mesh_index_buffer* obj_index_buffer
                = object_storage_get_obj_mesh_index_buffer(object_info);

            mat4 mat = elem->mat_draw;
            if (!obj || !obj_vertex_buffer || !obj_index_buffer || (object_data->object_culling
                && !object_bounding_sphere_check_visibility(&obj->bounding_sphere,
                    object_data, rctx->camera, &mat)))
                continue;

            shader_glsl_set_mat4(&glt_edt->gl_data.wireframe.shader, "model", false, mat);

            int32_t ttc = 0;
            texture_transform_struct* tt = object_data->texture_transform_array;
            for (uint32_t i = 0; i < obj->num_mesh; i++) {
                obj_mesh* mesh = &obj->mesh_array[i];

                if (object_data->object_culling && !object_bounding_sphere_check_visibility(
                    &mesh->bounding_sphere, object_data, rctx->camera, &mat))
                    continue;

                for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];

                    if (sub_mesh->attrib.m.flag_3)
                        continue;

                    if (object_data->object_culling) {
                        int32_t v32 = object_bounding_sphere_check_visibility(
                            &sub_mesh->bounding_sphere, object_data, rctx->camera, &mat);
                        if (v32 != 2 || (!mesh->attrib.m.billboard && !mesh->attrib.m.billboard_y_axis)) {
                            if (v32 == 2) {
                                if (object_data->object_bounding_sphere_check_func)
                                    v32 = 1;
                                else
                                    v32 = obj_axis_aligned_bounding_box_check_visibility(
                                        &sub_mesh->axis_aligned_bounding_box, rctx->camera, &mat);
                            }

                            if (!v32)
                                continue;
                        }
                    }

                    GLenum mesh_draw_mode[] = {
                        GL_ZERO, //GL_POINTS,
                        GL_LINES,
                        GL_LINE_STRIP,
                        GL_LINE_LOOP,
                        GL_TRIANGLES,
                        GL_TRIANGLE_STRIP,
                        GL_TRIANGLE_FAN,
                        GL_ZERO, //GL_QUADS,
                        GL_ZERO, //GL_QUAD_STRIP,
                        GL_ZERO, //GL_POLYGON,
                    };

                    GLenum mesh_indices_type[] = {
                        GL_ZERO,
                        GL_UNSIGNED_SHORT,
                        GL_UNSIGNED_INT,
                    };

                    GLuint array_buffer = obj_mesh_vertex_buffer_get_buffer(&obj_vertex_buffer[i]);
                    gl_state_bind_array_buffer(array_buffer);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh->size_vertex, (void*)0);
                    gl_state_bind_array_buffer(0);

                    GLuint element_array_buffer = obj_index_buffer[i].buffer;
                    gl_state_bind_element_array_buffer(element_array_buffer);
                    if (sub_mesh->primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP
                        && sub_mesh->index_format == OBJ_INDEX_U16)
                        glDrawRangeElements(GL_TRIANGLE_STRIP, sub_mesh->first_index, sub_mesh->last_index,
                            sub_mesh->indices_count, GL_UNSIGNED_SHORT, (void*)(size_t)sub_mesh->indices_offset);
                    else
                        glDrawElements(mesh_draw_mode[sub_mesh->primitive_type], sub_mesh->indices_count,
                            mesh_indices_type[sub_mesh->index_format], (void*)(size_t)sub_mesh->indices_offset);
                    gl_state_bind_element_array_buffer(0);

                    glDisableVertexAttribArray(0);
                }
            }
        }
    }
}

static void glitter_editor_gl_select_particle(glitter_editor_struct* glt_edt) {
    Glitter::glt_particle_manager.scene = 0;
    Glitter::glt_particle_manager.effect = 0;
    Glitter::glt_particle_manager.emitter = 0;
    Glitter::glt_particle_manager.particle = 0;
    Glitter::glt_particle_manager.draw_selected = false;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Scene* sc = 0;
    for (Glitter::Scene*& i : Glitter::glt_particle_manager.scenes) {
        if (!i)
            continue;

        Glitter::Scene* scene = i;
        if (scene->effect_group == eg) {
            sc = scene;
            break;
        }
    }
    if (!sc)
        return;

    Glitter::F2EffectInst* eff_f2 = 0;
    Glitter::F2EmitterInst* emit_f2 = 0;
    Glitter::F2ParticleInst* ptcl_f2 = 0;
    Glitter::XEffectInst* eff_x = 0;
    Glitter::XEmitterInst* emit_x = 0;
    Glitter::XParticleInst* ptcl_x = 0;
    for (Glitter::SceneEffect& i : sc->effects) {
        if (!i.disp || !i.ptr)
            continue;

        Glitter::EffectInst* effect = i.ptr;
        if (effect->effect != glt_edt->selected_effect)
            continue;

        eff_f2 = dynamic_cast<Glitter::F2EffectInst*>(effect);
        eff_x = dynamic_cast<Glitter::XEffectInst*>(effect);
        if (eff_f2)
            for (Glitter::F2EmitterInst*& j : eff_f2->emitters) {
                if (!j)
                    continue;

                Glitter::F2EmitterInst* emitter = j;
                if (emitter->emitter != glt_edt->selected_emitter)
                    continue;

                emit_f2 = emitter;
                for (Glitter::F2ParticleInst*& k : emitter->particles) {
                    if (!k)
                        continue;

                    Glitter::F2ParticleInst* particle = k;
                    if (particle->particle == glt_edt->selected_particle) {
                        ptcl_f2 = particle;
                        break;
                    }
                }
                break;
            }
        else if (eff_x)
            for (Glitter::XEmitterInst*& j : eff_x->emitters) {
                if (!j)
                    continue;

                Glitter::XEmitterInst* emitter = j;
                if (emitter->emitter != glt_edt->selected_emitter)
                    continue;

                emit_x = emitter;
                for (Glitter::XParticleInst*& k : emitter->particles) {
                    if (!k)
                        continue;

                    Glitter::XParticleInst* particle = k;
                    if (particle->particle == glt_edt->selected_particle) {
                        ptcl_x = particle;
                        break;
                    }
                }
                break;
            }
        break;
    }

    Glitter::glt_particle_manager.scene = sc;
    if (eff_f2) {
        Glitter::glt_particle_manager.effect = eff_f2;
        Glitter::glt_particle_manager.emitter = emit_f2;
        Glitter::glt_particle_manager.particle = ptcl_f2;
    }
    else if (eff_x) {
        Glitter::glt_particle_manager.effect = eff_x;
        Glitter::glt_particle_manager.emitter = emit_x;
        Glitter::glt_particle_manager.particle = ptcl_x;
    }
    else {
        Glitter::glt_particle_manager.effect = 0;
        Glitter::glt_particle_manager.emitter = 0;
        Glitter::glt_particle_manager.particle = 0;
    }
    Glitter::glt_particle_manager.draw_selected = glt_edt->draw_flags & GLITTER_EDITOR_DRAW_SELECTED;
}
#endif
