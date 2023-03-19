/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Curve Editor code based on Animation Timeline code from https://github.com/crash5band/Glitter
*/

#if defined(ReDIVA_DEV)
#include "glitter_editor.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/dds.hpp"
#include "../../KKdLib/interpolation.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../KKdLib/txp.hpp"
#include "../../KKdLib/vec.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/camera.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/gl_state.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/static_var.hpp"
#include "../data_test/stage_test.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
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
    GLITTER_EDITOR_DRAW_SELECTED = 0x01,
    GLITTER_EDITOR_DRAW_NO_DRAW  = 0x02,
};

struct GlitterEditor;

struct GlitterEditor {
    struct CurveEditor {
        Glitter::CurveType type;
        Glitter::CurveTypeFlags type_flags;
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
        float_t offset;

        bool add_key;
        bool del_key;
        bool add_curve;
        bool del_curve;
        bool key_edit;

        CurveEditor();

        void ResetCurves();
        void ResetState(Glitter::CurveType type = (Glitter::CurveType)-1);
        void SetFlag(const Glitter::CurveTypeFlags type_flag);
    };

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
    std::string file;

    glitter_editor_selected_enum selected_type;
    int32_t selected_resource;
    Glitter::Effect* selected_effect;
    Glitter::Emitter* selected_emitter;
    Glitter::Particle* selected_particle;
    Glitter::Effect* selected_edit_effect;
    Glitter::Emitter* selected_edit_emitter;
    Glitter::Particle* selected_edit_particle;
    CurveEditor curve_editor;

    GlitterEditor();
    ~GlitterEditor();

    void Reset();

    static void ResetDisp();
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

extern render_context* rctx_ptr;

static const char* glitter_editor_window_title = "Glitter Editor";

static void glitter_editor_select_particle(GlitterEditor* glt_edt);
static void glitter_editor_windows(GlitterEditor* glt_edt, class_data* data);

static void glitter_editor_reload(GlitterEditor* glt_edt);
static void glitter_editor_save(GlitterEditor* glt_edt);
static void glitter_editor_open_window(GlitterEditor* glt_edt);
static void glitter_editor_save_window(GlitterEditor* glt_edt);
static void glitter_editor_save_as_window(GlitterEditor* glt_edt);
static void glitter_editor_load_file(GlitterEditor* glt_edt, const char* path, const char* file);
static void glitter_editor_save_file(GlitterEditor* glt_edt, const char* path, const char* file);
static bool glitter_editor_list_open_window(Glitter::EffectGroup* eg);
static bool glitter_editor_resource_import(GlitterEditor* glt_edt);
static bool glitter_editor_resource_export(GlitterEditor* glt_edt);

static void glitter_editor_test_window(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_effects(GlitterEditor* glt_edt);
static void glitter_editor_effects_context_menu(GlitterEditor* glt_edt,
    Glitter::Effect* effect, Glitter::Emitter* emitter, Glitter::Particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type);
static void glitter_editor_resources(GlitterEditor* glt_edt);
static void glitter_editor_resources_context_menu(GlitterEditor* glt_edt,
    int32_t resource, ssize_t i_idx, bool selected_none);
static void glitter_editor_play_manager(GlitterEditor* glt_edt);
static void glitter_editor_property(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_property_effect(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_property_emitter(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_property_particle(GlitterEditor* glt_edt, class_data* data);
static bool glitter_editor_property_particle_texture(GlitterEditor* glt_edt,
    class_data* data, const char* label, char** items, Glitter::Particle* particle,
    GLuint* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
    int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex);

static void glitter_editor_popups(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_file_create_popup(GlitterEditor* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_popup(GlitterEditor* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_model_popup(GlitterEditor* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_error_list_popup(GlitterEditor* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_save_popup(GlitterEditor* glt_edt, class_data* data,
    ImGuiIO& io, ImGuiStyle& style, ImFont* font, const float_t title_bar_size);

static void glitter_editor_curve_editor_curve_set(GlitterEditor* glt_edt,
    Glitter::Curve* curve, Glitter::CurveType type);
static Glitter::Curve::Key* glitter_editor_curve_editor_get_closest_key(
    GlitterEditor* glt_edt, Glitter::Curve* curve);
static Glitter::Curve::Key* glitter_editor_curve_editor_get_selected_key(
    GlitterEditor* glt_edt, Glitter::Curve* curve);
static float_t glitter_editor_curve_editor_get_value(GlitterEditor* glt_edt,
    Glitter::CurveType type);
static void glitter_editor_curve_editor_key_manager(GlitterEditor* glt_edt,
    std::vector<Glitter::Curve::Key>* keys, bool* add_key, bool* del_key);
static void glitter_editor_curve_editor_property_window(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_curve_editor_selector(GlitterEditor* glt_edt, class_data* data);
static void glitter_editor_curve_editor_window(GlitterEditor* glt_edt);
static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::CurveType type,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z = false);
static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::Curve* curve,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z = false);
static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::Curve* curve,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z, const int32_t start_time, const int32_t end_time,
    const int32_t base_start_time, const int32_t base_end_time, std::vector<ImVec2>& points, bool loop = false);
static void glitter_editor_curve_editor_window_draw_random_range(
    GlitterEditor::CurveEditor* crv_edt, const float_t x1, const float_t x2,
    const vec3 value, const vec3 random, const float_t min, const float_t max, const bool random_range_negate,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max);
static void glitter_editor_curve_editor_window_draw_random_range(
    GlitterEditor::CurveEditor* crv_edt, const float_t x1, const float_t x2,
    const vec3 value1, const vec3 value2, const vec3 random1, const vec3 random2,
    const float_t min, const float_t max, const bool random_range_negate,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max);

static bool glitter_editor_hash_input(GlitterEditor* glt_edt,
    const char* label, uint64_t* hash);

bool glitter_editor_init(class_data* data, render_context* rctx) {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    Glitter::glt_particle_manager->random.value = (uint32_t)(time.LowPart * hash_fnv1a64m_empty);
    Glitter::glt_particle_manager->random.step = 1;

    QueryPerformanceCounter(&time);
    Glitter::glt_particle_manager->counter = (uint32_t)(time.LowPart * hash_murmurhash_empty);

    GlitterEditor* glt_edt = new GlitterEditor;
    if (glt_edt) {
        Glitter::glt_particle_manager->emission = 1.0f;
        Glitter::glt_particle_manager->draw_all = true;
        Glitter::glt_particle_manager->draw_all_mesh = true;
        draw_grid_3d = true;

        glt_edt->Reset();
        dtm_stg_load(0);
        dtw_stg_load(true);
    }
    data->data = glt_edt;

    input_reset = true;

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
        s.open(L"name_VRFL.glitter.txt", L"rb");
        size_t length = s.length;
        uint8_t* data = force_malloc(length);
        s.read(data, length);
        s.close();

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
                hashes[i] = hash_murmurhash(lines[i], min_def(len, 0x7F));
            }

            for (ssize_t i = 0; i < c; i++) {
                char* file_x = str_utils_get_without_extension(string_data(&files_x.begin[i]));

                char buf[0x200];
                sprintf_s(buf, 0x100, "%hs\n", file_x);
                OutputDebugStringA(buf);

                glt_edt->load_glt_type = Glitter::X;
                fr = glitter_file_reader_init(Glitter::X, path_x, file_x, 1.0f);
                glitter_file_reader_read(fr, Glitter::glt_particle_manager->emission);

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
                            char buf[0x200];
                            sprintf_s(buf, 0x100, "%08llX\n", e->data.name_hash);
                            OutputDebugStringA(buf);
                        }
                    }

                    Glitter::EffectGroup_dispose(fr->effect_group);
                }
                free_def(file_x);
            }
            free_def(lines);
            free_def(hashes);
        }
        free_def(data);
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

            char buf[0x200];
            sprintf_s(buf, 0x100, "%hs\n", file_x);
            OutputDebugStringA(buf);

            glt_edt->load_glt_type = Glitter::X;
            fr = glitter_file_reader_init(Glitter::X, path_x, file_x, 1.0f);
            glitter_file_reader_read(fr, Glitter::glt_particle_manager->emission);

            if (fr && fr->effect_group) {
                Glitter::EffectGroup* eg = fr->effect_group;
                for (Glitter::Effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    Glitter::Effect* e = *i;
                    if (!e->data.ext_anim)
                        continue;

                    Glitter::Effect_ext_anim* ea = e->data.ext_anim;
                    if (ea->instance_id) {
                        char buf[0x200];
                        sprintf_s(buf, 0x100, "%08llX %d\n", e->data.name_hash, ea->instance_id);
                        OutputDebugStringA(buf);
                    }
                }

                Glitter::EffectGroup_dispose(fr->effect_group);
            }
            free_def(file_x);
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
    s.open(L"name_F2.glitter.txt", L"rb");
    size_t length = s.length;
    uint8_t* data = force_malloc(length);
    s.read(data, length);
    s.close();

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
            hashes[i] = hash_murmurhash(lines[i], min_def(len, 0x7F));
        }

        ssize_t c = vector_old_length(files_f2);
        GlitterFileReader* fr = 0;
        if (files_f2.begin && files_ft.begin)
            for (ssize_t i = 0; i < c; i++) {
                char* file_f2 = str_utils_get_without_extension(string_data(&files_f2.begin[i]));
                char* file_ft = str_utils_get_without_extension(string_data(&files_ft.begin[i]));
                glt_edt->load_glt_type = Glitter::F2;
                fr = glitter_file_reader_init(Glitter::F2, path_f2, file_f2, 1.0f);
                glitter_file_reader_read(fr, Glitter::glt_particle_manager->emission);

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
                        memcpy(e->name, lines[j], min_def(len, 0x7F));
                    }

                    glt_edt->effect_group = fr->effect_group;
                    //glt_edt->save_glt_type = Glitter::FT;
                    //glitter_editor_save_file(path_ft, file_ft);
                    Glitter::EffectGroup_dispose(fr->effect_group);
                }
                free_def(file_f2);
                free_def(file_ft);
            }
        glitter_file_reader_dispose(fr);

        free_def(buf);
        free_def(lines);
        free_def(hashes);
    }
    for (string* i = files_f2.begin; i != files_f2.end; i++)
        string_free(i);
    vector_old_string_free(&files_f2);

    for (string* i = files_ft.begin; i != files_ft.end; i++)
        string_free(i);
    vector_old_string_free(&files_ft);
    free_def(data);
    glt_edt->effect_group = 0;*/
    return true;
}

void glitter_editor_ctrl(class_data* data) {
    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
    if (!glt_edt)
        return;

    int32_t sel_rsrc = glt_edt->selected_resource;
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    if (glt_edt->effect_group_add) {
        Glitter::glt_particle_manager->UnloadEffectGroup(glt_edt->hash);

        GlitterEditor::ResetDisp();

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        Glitter::EffectGroup* eg = new Glitter::EffectGroup(glt_edt->load_glt_type);
        char buf[0x200];
        snprintf(buf, sizeof(buf), "eff_%016llx_main", time.LowPart * hash_fnv1a64m_empty);
        uint64_t hash = glt_edt->load_glt_type == Glitter::FT
            ? hash_utf8_fnv1a64m(buf) : hash_utf8_murmurhash(buf);
        eg->hash = hash;
        eg->load_count = 1;
        Glitter::glt_particle_manager->effect_groups.insert({ hash, eg });

        glt_edt->scene_counter = Glitter::glt_particle_manager->LoadScene(hash, eg->type != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty);
        glt_edt->effect_group = eg;
        glt_edt->hash = hash;
    }

    bool has_resource = sel_rsrc != -1;
    bool has_effect = !!glt_edt->selected_effect;
    bool has_emitter = !!glt_edt->selected_emitter && has_effect;
    bool has_particle = !!glt_edt->selected_particle && has_emitter;
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
            *e = *glt_edt->selected_edit_effect;

            char name[0x80];
            sprintf_s(name, sizeof(name), "eff_%08x",
                (uint32_t)((eff_count + 1)* time.LowPart* hash_murmurhash_empty));
            e->name.assign(name);
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
            *p = *glt_edt->selected_edit_particle;
            sel_emit->particles.push_back(p);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = (ssize_t)-1;
            for (Glitter::Particle*& i : sel_emit->particles)
                if (i && i == glt_edt->selected_edit_particle) {
                    j = &i - sel_emit->particles.data();
                    break;
                }

            if (j > -1)
                sel_emit->particles.erase(sel_emit->particles.begin() + j);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_EMITTER;
            glt_edt->selected_edit_particle = 0;
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (Glitter::Particle*& i : sel_emit->particles)
                if (i == glt_edt->selected_edit_particle) {
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
                if (i == glt_edt->selected_edit_particle) {
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
        char* file = str_utils_get_without_extension(glt_edt->file.c_str());
        char* path = str_utils_split_right_get_left_include(glt_edt->file.c_str(), '\\');

        if (glt_edt->load)
            glitter_editor_load_file(glt_edt, path, file);
        else if (glt_edt->save && glt_edt->effect_group)
            glitter_editor_save_file(glt_edt, path, file);

        glt_edt->save = false;
        glt_edt->save_compress = false;
        free_def(path);
        free_def(file);
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

                e->name.assign(ds->glitter_list_names[hash - hashes.begin()]);
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

                e->name.assign(ds->glitter_list_names[hash - hashes.begin()]);
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
            glt_edt->load = false;
            glt_edt->load_data = false;
            glt_edt->load_data_wait = false;
            glt_edt->load_popup = false;
            glt_edt->load_data_popup = false;
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

        Glitter::glt_particle_manager->FreeSceneEffect(glt_edt->scene_counter);
        Glitter::glt_particle_manager->UnloadEffectGroup(glt_edt->hash);

        GlitterEditor::ResetDisp();
        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->hash = 0;
        glt_edt->scene_counter = 0;
        glt_edt->file.clear();
        glt_edt->close = false;
        return;
    }

    Glitter::glt_particle_manager->GetSceneStartEndFrame(
        &glt_edt->start_frame, &glt_edt->end_frame, glt_edt->scene_counter);

    if (!glt_edt->effect_group) {
        glt_edt->input_play = false;
        glt_edt->input_reset = false;
        return;
    }

    if (!(glt_edt->input_pause || glt_edt->input_pause_temp) && Glitter::glt_particle_manager->scenes.size())
        glt_edt->frame_counter += get_delta_frame();

    if (glt_edt->input_reload) {
        Glitter::glt_particle_manager->FreeSceneEffect(glt_edt->scene_counter);
        glt_edt->effect_group->emission = Glitter::glt_particle_manager->emission;
        Glitter::glt_particle_manager->SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
        Glitter::glt_particle_manager->selected_scene = glt_edt->scene;
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

        Glitter::glt_particle_manager->SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, false);
        Glitter::glt_particle_manager->selected_scene = glt_edt->scene;
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
    }

    if (glt_edt->input_play)
        glt_edt->input_pause = false;
    else if (glt_edt->input_reset) {
        Glitter::glt_particle_manager->counter = glt_edt->counter;
        glt_edt->frame_counter = (float_t)glt_edt->start_frame;
        Glitter::glt_particle_manager->SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
        Glitter::glt_particle_manager->selected_scene = glt_edt->scene;
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
        glt_edt->input_pause = true;
    }
    glt_edt->input_play = false;
    glt_edt->input_reset = false;

    rob_frame = glt_edt->frame_counter;

    if (!glt_edt->effect_group) {
        GlitterEditor::ResetDisp();
        return;
    }

    glitter_editor_select_particle(glt_edt);
}

void glitter_editor_disp(class_data* data) {
    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
    if (!glt_edt)
        return;
}

void glitter_editor_drop(class_data* data, size_t count, char** paths) {
    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
    if (!glt_edt)
        return;

    glt_edt->file.assign(paths[0]);
    glt_edt->load_popup = true;
}

void glitter_editor_imgui(class_data* data) {
    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
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

    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
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
    GlitterEditor* glt_edt = (GlitterEditor*)data->data;
    if (glt_edt) {
        dtw_stg_unload();
        dtm_stg_unload();
        Glitter::glt_particle_manager->FreeSceneEffect(glt_edt->scene_counter);
        Glitter::glt_particle_manager->UnloadEffectGroup(glt_edt->hash);
        delete glt_edt;
    }
    draw_grid_3d = false;

    data->data = 0;
    data->flags = (class_flags)(CLASS_HIDDEN | CLASS_DISPOSED);
    data->imgui_focus = false;
    return true;
}

GlitterEditor::CurveEditor::CurveEditor() : type(), type_flags(), animation(), list(),
key(), frame_width(), zoom_time(), prev_zoom_time(), zoom_value(), key_radius_in(),
key_radius_out(), height_offset(), frame(), draw_list(), io(), timeline_pos(),
range(), offset(), add_key(), del_key(), add_curve(), del_curve(), key_edit() {

}

void GlitterEditor::CurveEditor::ResetCurves() {
    memset(&list, 0, sizeof(list));
    animation = 0;
    key = {};
}

void GlitterEditor::CurveEditor::ResetState(Glitter::CurveType type) {
    if (this->type != type)
        this->type = type;

    if (this->type != -1)
        type_flags = (Glitter::CurveTypeFlags)(1 << this->type);
    else
        type_flags = (Glitter::CurveTypeFlags)0;

    frame_width = 16;
    zoom_time = 1.0f;
    prev_zoom_time = 1.0f;
    zoom_value = 1.0f;
    key_radius_in = 6.0f;
    key_radius_out = 8.0f;
    height_offset = 20.0f;
    frame = 0;

    timeline_pos = -curve_editor_timeline_base_pos;

    switch (type) {
    case Glitter::CURVE_TRANSLATION_X:
    case Glitter::CURVE_TRANSLATION_Y:
    case Glitter::CURVE_TRANSLATION_Z:
        range = 25.0f;
        break;
    case Glitter::CURVE_ROTATION_X:
    case Glitter::CURVE_ROTATION_Y:
    case Glitter::CURVE_ROTATION_Z:
        range = 360.0f;
        break;
    case Glitter::CURVE_SCALE_X:
    case Glitter::CURVE_SCALE_Y:
    case Glitter::CURVE_SCALE_Z:
    case Glitter::CURVE_SCALE_ALL:
        range = 5.0f;
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
        range = 255.0f;
        break;
    case Glitter::CURVE_EMISSION_INTERVAL:
        range = 20.0f;
        break;
    case Glitter::CURVE_PARTICLES_PER_EMISSION:
        range = 50.0f;
        break;
    case Glitter::CURVE_U_SCROLL:
    case Glitter::CURVE_V_SCROLL:
    case Glitter::CURVE_U_SCROLL_ALPHA:
    case Glitter::CURVE_V_SCROLL_ALPHA:
    case Glitter::CURVE_U_SCROLL_2ND:
    case Glitter::CURVE_V_SCROLL_2ND:
    case Glitter::CURVE_U_SCROLL_ALPHA_2ND:
    case Glitter::CURVE_V_SCROLL_ALPHA_2ND:
        range = 1.0f;
        break;
    default:
        range = 1.0f;
        break;
    }

    offset = 0.0f;

    add_key = false;
    del_key = false;
}

void GlitterEditor::CurveEditor::SetFlag(const Glitter::CurveTypeFlags type_flag) {
    switch (type) {
    case Glitter::CURVE_TRANSLATION_X:
    case Glitter::CURVE_TRANSLATION_Y:
    case Glitter::CURVE_TRANSLATION_Z:
        if (type_flag & Glitter::CURVE_TYPE_TRANSLATION_XYZ)
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_ROTATION_X:
    case Glitter::CURVE_ROTATION_Y:
    case Glitter::CURVE_ROTATION_Z:
        if (type_flag & Glitter::CURVE_TYPE_ROTATION_XYZ)
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_SCALE_X:
    case Glitter::CURVE_SCALE_Y:
    case Glitter::CURVE_SCALE_Z:
    case Glitter::CURVE_SCALE_ALL:
        if (type_flag & (Glitter::CURVE_TYPE_SCALE_XYZ | Glitter::CURVE_TYPE_SCALE_ALL))
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_COLOR_R:
    case Glitter::CURVE_COLOR_G:
    case Glitter::CURVE_COLOR_B:
    case Glitter::CURVE_COLOR_A:
    case Glitter::CURVE_COLOR_RGB_SCALE:
        if (type_flag & (Glitter::CURVE_TYPE_COLOR_RGBA | Glitter::CURVE_TYPE_COLOR_RGB_SCALE))
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_COLOR_R_2ND:
    case Glitter::CURVE_COLOR_G_2ND:
    case Glitter::CURVE_COLOR_B_2ND:
    case Glitter::CURVE_COLOR_A_2ND:
    case Glitter::CURVE_COLOR_RGB_SCALE_2ND:
        if (type_flag & (Glitter::CURVE_TYPE_COLOR_RGBA_2ND | Glitter::CURVE_TYPE_COLOR_RGB_SCALE_2ND))
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_U_SCROLL:
    case Glitter::CURVE_V_SCROLL:
        if (type_flag & Glitter::CURVE_TYPE_UV_SCROLL)
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_U_SCROLL_ALPHA:
    case Glitter::CURVE_V_SCROLL_ALPHA:
        if (type_flag & Glitter::CURVE_TYPE_UV_SCROLL_ALPHA)
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_U_SCROLL_2ND:
    case Glitter::CURVE_V_SCROLL_2ND:
        if (type_flag & Glitter::CURVE_TYPE_UV_SCROLL_2ND)
            enum_xor(type_flags, type_flag);
        break;
    case Glitter::CURVE_U_SCROLL_ALPHA_2ND:
    case Glitter::CURVE_V_SCROLL_ALPHA_2ND:
        if (type_flag & Glitter::CURVE_TYPE_UV_SCROLL_ALPHA_2ND)
            enum_xor(type_flags, type_flag);
        break;
    }
    enum_or(type_flags, (1 << type));
}

GlitterEditor::GlitterEditor() : test(), create_popup(), load(), load_wait(), load_data(),
load_data_wait(), load_popup(), load_data_popup(), load_error_list_popup(), save(), save_popup(),
save_compress(), close(), input_play(), input_reload(), input_pause(), input_pause_temp(), input_reset(),
effect_group_add(), draw_flags(), resource_flags(), effect_flags(), emitter_flags(), particle_flags(),
load_glt_type(), save_glt_type(), load_data_type(), frame_counter(), old_frame_counter(),
start_frame(), end_frame(),  counter(), effect_group(), scene(), hash(), selected_type(),
selected_resource(), selected_effect(), selected_emitter(), selected_particle(),
selected_edit_effect(), selected_edit_emitter(), selected_edit_particle() {

}

GlitterEditor::~GlitterEditor() {

}

void GlitterEditor::Reset() {
    selected_type = GLITTER_EDITOR_SELECTED_NONE;
    selected_resource = -1;
    selected_effect = 0;
    selected_emitter = 0;
    selected_particle = 0;
    curve_editor.ResetState();
    curve_editor.ResetCurves();
}

void GlitterEditor::ResetDisp() {
    Glitter::glt_particle_manager->selected_scene = 0;
    Glitter::glt_particle_manager->selected_effect = 0;
    Glitter::glt_particle_manager->selected_emitter = 0;
    Glitter::glt_particle_manager->selected_particle = 0;
}

static void glitter_editor_select_particle(GlitterEditor* glt_edt) {
    Glitter::glt_particle_manager->selected_scene = 0;
    Glitter::glt_particle_manager->selected_effect = 0;
    Glitter::glt_particle_manager->selected_emitter = 0;
    Glitter::glt_particle_manager->selected_particle = 0;
    Glitter::glt_particle_manager->draw_selected = false;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Scene* sc = 0;
    for (Glitter::Scene*& i : Glitter::glt_particle_manager->scenes) {
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
    if (sc->type != Glitter::X)
        for (Glitter::SceneEffect& i : sc->effects) {
            if (!i.disp || !i.ptr)
                continue;

            Glitter::EffectInst* effect = i.ptr;
            if (effect->effect != glt_edt->selected_effect)
                continue;

            eff_f2 = (Glitter::F2EffectInst*)effect;
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
        }
    else
        for (Glitter::SceneEffect& i : sc->effects) {
            if (!i.disp || !i.ptr)
                continue;

            Glitter::EffectInst* effect = i.ptr;
            if (effect->effect != glt_edt->selected_effect)
                continue;

            eff_x = (Glitter::XEffectInst*)effect;
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
        }

    Glitter::glt_particle_manager->selected_scene = sc;
    if (eff_f2) {
        Glitter::glt_particle_manager->selected_effect = eff_f2;
        Glitter::glt_particle_manager->selected_emitter = emit_f2;
        Glitter::glt_particle_manager->selected_particle = ptcl_f2;
    }
    else if (eff_x) {
        Glitter::glt_particle_manager->selected_effect = eff_x;
        Glitter::glt_particle_manager->selected_emitter = emit_x;
        Glitter::glt_particle_manager->selected_particle = ptcl_x;
    }
    else {
        Glitter::glt_particle_manager->selected_effect = 0;
        Glitter::glt_particle_manager->selected_emitter = 0;
        Glitter::glt_particle_manager->selected_particle = 0;
    }
    Glitter::glt_particle_manager->draw_selected = glt_edt->draw_flags & GLITTER_EDITOR_DRAW_SELECTED;
}

static void glitter_editor_windows(GlitterEditor* glt_edt, class_data* data) {
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
            ImGui::CheckboxFlagsEnterKeyPressed("Draw Selected",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
            ImGui::CheckboxFlagsEnterKeyPressed("No Draw",
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

    win_x = min_def((float_t)width / 4.0f, 400.0f);

    x = 0;
    y = main_menu_bar_size.y;
    w = min_def((float_t)width / 4.0f, 400.0f);
    h = (float_t)height - y - min_def((float_t)height / 3.0f, 320.0f);

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

            ImGui::DisableElementPush(!!glt_edt->effect_group);
            if (ImGui::BeginTabItem("Resources", 0, 0)) {
                glitter_editor_resources_context_menu(glt_edt, -1, 0, true);
                if (glt_edt->effect_group)
                    glitter_editor_resources(glt_edt);
                ImGui::EndTabItem();
            }
            ImGui::DisableElementPop(!!glt_edt->effect_group);

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

    win_x = min_def((float_t)width / 4.0f, 400.0f);

    x = (float_t)width - win_x;
    y = main_menu_bar_size.y;
    w = win_x;
    h = (float_t)height - y - min_def((float_t)height / 3.0f, 320.0f);

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

    win_x = min_def((float_t)width / 4.0f, 400.0f);
    win_y = min_def((float_t)height / 3.0f, 320.0f);

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

    win_x = min_def((float_t)width / 4.0f, 400.0f);
    win_y = min_def((float_t)height / 3.0f, 320.0f);

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

    win_x = min_def((float_t)width / 4.0f, 400.0f);
    win_y = min_def((float_t)height / 3.0f, 320.0f);

    x = win_x;
    y = (float_t)height - win_y;
    w = (float_t)width - win_x * 2.0f;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGui::PushID("Glitter Editor Curve Editor Window");
    if (ImGui::Begin("Curve Editor", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor_window(glt_edt);
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::End();
    }
    ImGui::PopID();
}

static void glitter_editor_reload(GlitterEditor* glt_edt) {
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
            effect->data.name_hash = hash_string_murmurhash(effect->name);
        else
            effect->data.name_hash = hash_string_fnv1a64m(effect->name);

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

static void glitter_editor_save(GlitterEditor* glt_edt) {
    glt_edt->save_popup = glt_edt->effect_group
        && glt_edt->effect_group->type == Glitter::X ? false : true;
}

static void glitter_editor_open_window(GlitterEditor* glt_edt) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return;

    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        char* file_temp = utf16_to_utf8(file);
        glt_edt->file.assign(file_temp);
        free_def(file_temp);
        glt_edt->load_popup = true;
    }
    CoUninitialize();
}

static void glitter_editor_save_window(GlitterEditor* glt_edt) {
    if (!glt_edt->effect_group || !glt_edt->scene)
        return;

    if (glt_edt->file[0])
        glitter_editor_save(glt_edt);
    else
        glitter_editor_save_as_window(glt_edt);
}

static void glitter_editor_save_as_window(GlitterEditor* glt_edt) {
    if (glt_edt->effect_group
        && glt_edt->effect_group->type == Glitter::X) {
        glt_edt->save_popup = false;
        return;
    }

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        glt_edt->save_popup = false;
        return;
    }

    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Save";
    ofn.Flags = OFN_NONETWORKBUTTON;
    if (GetSaveFileNameW(&ofn)) {
        char* file_temp = utf16_to_utf8(file);
        glt_edt->file.assign(file_temp);
        free_def(file_temp);
        glitter_editor_save(glt_edt);
    }
    else
        glt_edt->save_popup = false;
    CoUninitialize();
}

static void glitter_editor_load_file(GlitterEditor* glt_edt, const char* path, const char* file) {
    if (!glt_edt->load_wait) {
        Glitter::glt_particle_manager->FreeSceneEffect(glt_edt->scene_counter);
        Glitter::glt_particle_manager->UnloadEffectGroup(glt_edt->hash);

        GlitterEditor::ResetDisp();

        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->load_wait = true;
    }

    if (!glt_edt->load_data_wait && Glitter::glt_particle_manager->GetEffectGroup(glt_edt->hash))
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
        glt_edt->hash = Glitter::glt_particle_manager->LoadFile(glt_edt->load_glt_type,
            &data_list[glt_edt->load_data_type], file, path, -1.0f, false, obj_db);
        glt_edt->load_data_wait = true;
    }
    else if (Glitter::glt_particle_manager->CheckNoFileReaders(glt_edt->hash)) {
        Glitter::EffectGroup* eg = Glitter::glt_particle_manager->GetEffectGroup(glt_edt->hash);
        if (eg) {
            Glitter::glt_particle_manager->LoadScene(glt_edt->hash, eg->type != Glitter::FT
                ? hash_murmurhash_empty : hash_fnv1a64m_empty, false);
            Glitter::Scene* sc = Glitter::glt_particle_manager->GetScene(glt_edt->hash);
            if (sc) {
                bool lst_not_valid = true;
                enum_or(sc->flags, Glitter::SCENE_EDITOR);
                if (glt_edt->load_glt_type == Glitter::FT) {
                    lst_not_valid = false;
                    for (Glitter::Effect*& i : eg->effects) {
                        if (!i)
                            continue;

                        Glitter::Effect* e = i;
                        if (e->data.name_hash != hash_string_fnv1a64m(e->name)) {
                            lst_not_valid = true;
                            break;
                        }
                    }
                }
                glt_edt->counter = Glitter::glt_particle_manager->counter;
                glt_edt->effect_group = eg;
                glt_edt->scene = sc;
                glt_edt->scene_counter = sc->counter;
                glt_edt->load_data_popup = lst_not_valid;
            }
            else {
                Glitter::glt_particle_manager->UnloadEffectGroup(eg->hash);
                glt_edt->counter = 0;
                glt_edt->effect_group = 0;
                glt_edt->scene = 0;
                glt_edt->scene_counter = 0;
            }
        }

        GlitterEditor::ResetDisp();

        Glitter::glt_particle_manager->GetSceneStartEndFrame(
            &glt_edt->start_frame, &glt_edt->end_frame, glt_edt->scene_counter);
        glt_edt->frame_counter = 0;
        glt_edt->old_frame_counter = 0;
        glt_edt->input_pause = true;
        glt_edt->load = false;
        glt_edt->load_wait = false;
        glt_edt->load_data_wait = false;
    }
    else if (!Glitter::glt_particle_manager->CheckHasFileReader(glt_edt->hash)) {
        glt_edt->load = false;
        glt_edt->load_wait = false;
        glt_edt->load_data_wait = false;
    }

    glt_edt->Reset();
}

static void glitter_editor_save_file(GlitterEditor* glt_edt, const char* path, const char* file) {
    Glitter::FileWriter::Write(glt_edt->save_glt_type,
        glt_edt->effect_group, path, file, glt_edt->save_compress);
}

static bool glitter_editor_list_open_window(Glitter::EffectGroup* eg) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"Glitter UTF-8 List Files (*.glitter.txt)\0*.glitter.txt\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        file_stream s;
        s.open(file, L"rb");
        size_t length = s.length;
        uint8_t* data = force_malloc_s(uint8_t, length);
        s.read(data, length);
        s.close();

        char* buf;
        char** lines;
        size_t count;
        if (str_utils_text_file_parse(data, length, buf, lines, count)) {
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
                    hashes[i] = hash_murmurhash(lines[i], min_def(len, 0x7F));
                }
            else
                for (size_t i = 0; i < count; i++) {
                    size_t len = utf8_length(lines[i]);
                    hashes[i] = hash_fnv1a64m(lines[i], min_def(len, 0x7F));
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

                e->name.assign(lines[j]);
            }

            free_def(buf);
            free_def(lines);
            free_def(hashes);
        }
        free_def(data);
    }
    CoUninitialize();
    return ret;
}

static bool glitter_editor_resource_import(GlitterEditor* glt_edt) {
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"DirectDraw Sufrace Files (*.dds)\0*.dds\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Open";
    if (GetOpenFileNameW(&ofn)) {
        wchar_t* f = str_utils_get_without_extension(file);
        wchar_t* p = str_utils_split_right_get_left(file, L'.');

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
        {
            dds d;
            d.read(p);
            if (!d.width || !d.height || !d.mipmaps_count || d.data.size() < 1)
                goto DDSEnd;

            eg->resources_tex.textures.push_back({});
            tex = &eg->resources_tex.textures.back();
            tex->array_size = d.has_cube_map ? 6 : 1;
            tex->has_cube_map = d.has_cube_map;
            tex->mipmaps_count = d.mipmaps_count;

            tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
            index = 0;
            do
                for (uint32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1);
                    tex_mip.height = max_def(d.height >> i, 1);
                    tex_mip.format = d.format;

                    uint32_t size = txp::get_size(tex_mip.format, tex_mip.width, tex_mip.height);
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
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
        }
    DDSEnd:
        if (f)
            free(f);
        if (p)
            free(p);
    }
    CoUninitialize();
    return ret;
}

static bool glitter_editor_resource_export(GlitterEditor* glt_edt) {
    int32_t sel_rsrc = glt_edt->selected_resource;

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return false;

    bool ret = false;

    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.lpstrFilter = L"DirectDraw Sufrace Files (*.dds)\0*.dds\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"File to Save";
    ofn.Flags = OFN_NONETWORKBUTTON;
    if (GetSaveFileNameW(&ofn)) {
        wchar_t* p = str_utils_split_right_get_left(file, L'.');

        Glitter::EffectGroup* eg = glt_edt->effect_group;
        txp& tex = eg->resources_tex.textures[sel_rsrc];

        txp_format format = tex.mipmaps[0].format;
        uint32_t width = tex.mipmaps[0].width;
        uint32_t height = tex.mipmaps[0].height;

        dds d;
        d.format = format;
        d.width = width;
        d.height = height;
        d.mipmaps_count = tex.mipmaps_count;
        d.has_cube_map = tex.has_cube_map;
        d.data.reserve((tex.has_cube_map ? 6LL : 1LL) * tex.mipmaps_count);
        uint32_t index = 0;
        do
            for (uint32_t i = 0; i < tex.mipmaps_count; i++) {
                uint32_t size = txp::get_size(format, max_def(width >> i, 1), max_def(height >> i, 1));
                void* data = force_malloc(size);
                memcpy(data, tex.mipmaps[index].data.data(), size);
                d.data.push_back(data);
                index++;
            }
        while (index / tex.mipmaps_count < tex.array_size);
        d.write(p);
        ret = true;
        if (p)
            free(p);
    }
    CoUninitialize();
    return ret;
}

static void glitter_editor_test_window(GlitterEditor* glt_edt, class_data* data) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;

    float_t win_x;
    float_t win_y;

    float_t x;
    float_t y;

    float_t w = 280.0f;
    float_t h = 330.0f;

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Glitter Editor Test Window", &glt_edt->test, window_flags)) {
        data->imgui_focus |= ImGui::IsWindowFocused();
        return;
    }

    if (ImGui::ButtonEnterKeyPressed("Reset Camera (R)"))
        input_reset = true;

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Play (T)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_T))
            glt_edt->input_play = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Reload (Y)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_Y))
            glt_edt->input_reload = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Pause (F)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_F))
            glt_edt->input_pause = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Reset (G)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_G))
            glt_edt->input_reset = true;
        ImGui::EndTable();
    }

    ImGui::Separator();

    glt_edt->old_frame_counter = glt_edt->frame_counter;
    ImGui::ColumnSliderFloatButton("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, 10.0f, "%.0f", 0);
    glt_edt->input_pause_temp = ImGui::IsItemActivatedAccum;

    ImGui::Text("Start/End Frame: %d/%d", glt_edt->start_frame, glt_edt->end_frame);

    ImGui::ColumnSliderFloatButton("Emission",
        &Glitter::glt_particle_manager->emission, 0.01f, 1.0f, 2.0f, 0.1f, "%.2f", 0);

    ImGui::Separator();

    ImGui::CheckboxEnterKeyPressed("Show Grid", &draw_grid_3d);

    ImGui::CheckboxEnterKeyPressed("Draw All", &Glitter::glt_particle_manager->draw_all);

    ImGui::DisableElementPush(Glitter::glt_particle_manager->draw_all && eg && eg->type == Glitter::X);
    ImGui::CheckboxEnterKeyPressed("Draw All Mesh", &Glitter::glt_particle_manager->draw_all_mesh);
    ImGui::DisableElementPop(Glitter::glt_particle_manager->draw_all && eg && eg->type == Glitter::X);

    ImGui::Separator();

    ImGui::CheckboxFlagsEnterKeyPressed("Draw Selected",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
    ImGui::CheckboxFlagsEnterKeyPressed("No Draw",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_NO_DRAW);

    ImGui::Separator();

    camera* cam = rctx_ptr->camera;

    static const double_t fov_min = 0.0;
    static const double_t fov_max = 180.0;
    double_t fov = cam->get_fov();
    if (ImGui::SliderScalar("Camera FOV", ImGuiDataType_Double, &fov, &fov_min, &fov_max, "%g"))
        cam->set_fov(fov);

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();

    win_x = min_def((float_t)width, 240.0f);
    win_y = min_def((float_t)height, 96.0f);

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
        frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
        ImGui::Text("%.0f - %.0f/%d", max_def(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD);
        ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS);
        ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE);
        ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH);
        ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
        ImGui::End();
    }
    ImGui::PopStyleColor(2);
}

static void glitter_editor_effects(GlitterEditor* glt_edt) {
    char buf[0x200];

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

static void glitter_editor_effects_context_menu(GlitterEditor* glt_edt,
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
        else if (selected)
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
            glt_edt->selected_edit_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Delete Effect")) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_edit_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Move Effect Up", 0, false, i_idx > 1)) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_edit_effect = effect;
            close = true;
        }

        if (ImGui::MenuItem("Move Effect Down", 0, false, i_idx < (ssize_t)eg->effects.size())) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_edit_effect = effect;
            close = true;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Add Emitter")) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_ADD);
            glt_edt->selected_edit_effect = effect;
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
            glt_edt->selected_edit_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Delete Emitter")) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_edit_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Move Emitter Up", 0, false, j_idx > 1)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_edit_emitter = emitter;
            close = true;
        }

        if (ImGui::MenuItem("Move Emitter Down", 0, false,
            j_idx < (ssize_t)sel_efct->emitters.size())) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_edit_emitter = emitter;
            close = true;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Add Particle")) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_ADD);
            glt_edt->selected_edit_emitter = emitter;
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
            glt_edt->selected_edit_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Delete Particle")) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_edit_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Move Particle Up", 0, false, k_idx > 1)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_edit_particle = particle;
            close = true;
        }

        if (ImGui::MenuItem("Move Particle Down", 0, false,
            k_idx < (ssize_t)sel_emit->particles.size())) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_edit_particle = particle;
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
            glt_edt->Reset();
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = 0;
        glt_edt->selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        if (glt_edt->selected_type != type
            || glt_edt->selected_emitter != emitter)
            glt_edt->Reset();
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = emitter;
        glt_edt->selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (glt_edt->selected_type != type
            || glt_edt->selected_particle != particle)
            glt_edt->Reset();
        glt_edt->selected_effect = effect;
        glt_edt->selected_emitter = emitter;
        glt_edt->selected_particle = particle;
        break;
    }
    glt_edt->selected_type = type;
}

static void glitter_editor_resources(GlitterEditor* glt_edt) {
    int32_t sel_rsrc = glt_edt->selected_resource;

    char buf[0x200];

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

static void glitter_editor_resources_context_menu(GlitterEditor* glt_edt,
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

static void glitter_editor_play_manager(GlitterEditor* glt_edt) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;

    if (ImGui::ButtonEnterKeyPressed("Reset Camera (R)"))
        input_reset = true;

    float_t w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Play (T)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_T))
            glt_edt->input_play = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Reload (Y)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_Y))
            glt_edt->input_reload = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Pause (F)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_F))
            glt_edt->input_pause = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Reset (G)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_G))
            glt_edt->input_reset = true;
        ImGui::EndTable();
    }

    ImGui::Separator();

    glt_edt->old_frame_counter = glt_edt->frame_counter;
    ImGui::ColumnSliderFloatButton("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, 10.0f, "%.0f", 0);
    glt_edt->input_pause_temp = ImGui::IsItemActivatedAccum;

    ImGui::Text("Start/End Frame: %d/%d", glt_edt->start_frame, glt_edt->end_frame);

    ImGui::Separator();

    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    life_time = 0;
    frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
    ImGui::Text("%.0f - %.0f/%d", max_def(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD);
    ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS);
    ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE);
    ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH);
    ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);

    ImGui::Separator();

    ImGui::ColumnSliderFloatButton("Emission",
        &Glitter::glt_particle_manager->emission, 0.01f, 1.0f, 2.0f, 0.1f, "%.2f", 0);

    ImGui::Separator();

    ImGui::CheckboxEnterKeyPressed("Grid", &draw_grid_3d);

    ImGui::CheckboxEnterKeyPressed("Draw All", &Glitter::glt_particle_manager->draw_all);

    ImGui::DisableElementPush(Glitter::glt_particle_manager->draw_all && eg && eg->type == Glitter::X);
    ImGui::CheckboxEnterKeyPressed("Draw All Mesh", &Glitter::glt_particle_manager->draw_all_mesh);
    ImGui::DisableElementPop(Glitter::glt_particle_manager->draw_all && eg && eg->type == Glitter::X);

    ImGui::Separator();

    camera* cam = rctx_ptr->camera;

    static const double_t fov_min = 0.0;
    static const double_t fov_max = 180.0;
    double_t fov = cam->get_fov();
    if (ImGui::SliderScalar("Camera FOV", ImGuiDataType_Double, &fov, &fov_min, &fov_max, "%g"))
        cam->set_fov(fov);
}

static void glitter_editor_property(GlitterEditor* glt_edt, class_data* data) {
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

static void glitter_editor_property_effect(GlitterEditor* glt_edt, class_data* data) {
    ImGui::SetColumnSpace(2.0f / 5.0f);

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
    if (ImGui::ColumnInputText("Name", (char*)name.c_str(), name.capacity(), 0, 0, 0)) {
        effect->data.name_hash = eg->type != Glitter::FT
            ? hash_string_murmurhash(name) : hash_string_fnv1a64m(name);
        effect->name.assign(name.c_str());
        changed = true;
    }

    if (ImGui::ColumnDragInt("Appear Time",
        &effect->data.appear_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("Life Time",
        &effect->data.life_time, 1.0f, 0x0000, 0x7FFF - effect->data.appear_time, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("Start Time",
        &effect->data.start_time, 1.0f, 0x0000, 0x7FFF - effect->data.appear_time, "%d", 0))
        changed = true;

    ImGui::Separator();

    if (ImGui::ColumnDragVec3("Translation",
        &effect->translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    vec3 rotation = effect->rotation * RAD_TO_DEG_FLOAT;
    if (ImGui::ColumnDragVec3("Rotation",
        &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        effect->rotation = rotation * DEG_TO_RAD_FLOAT;
        changed = true;
    }

    if (ImGui::ColumnDragVec3("Scale",
        &effect->scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    ImGui::Separator();

    if (ImGui::CheckboxFlagsEnterKeyPressed("Loop",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_LOOP))
        changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Draw as Transparent",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_ALPHA))
        changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Fog",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_FOG))
        changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Fog Height",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_FOG_HEIGHT))
        changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Use External Emission",
        (uint32_t*)&effect->data.flags,
        Glitter::EFFECT_EMISSION))
        changed = true;

    if (eg->type == Glitter::X)
        if (ImGui::CheckboxFlagsEnterKeyPressed("Use Seed",
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
        if (ImGui::CheckboxEnterKeyPressed("Set Emission",
            &set_emission)) {
            effect->version = set_emission ? 7 : 6;
            changed = true;
        }

        if (effect->version == 7 && ImGui::ColumnDragFloat("Emission",
            &effect->data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
    }
    else {
        if (effect->data.flags & Glitter::EFFECT_USE_SEED
            && ImGui::ColumnInputScalar("Seed", ImGuiDataType_U32, &effect->data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (ImGui::ColumnDragFloat("Emission",
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

    if (ImGui::ColumnComboBox("Type", Glitter::effect_type_name,
        4, &type, 0, false, &data->imgui_focus)) {
        if (type == 1) {
            enum_or(effect->data.flags, Glitter::EFFECT_LOCAL);
            delete effect->data.ext_anim;
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
                delete effect->data.ext_anim;
        }
        changed = true;
    }

    if (!(effect->data.flags & Glitter::EFFECT_LOCAL) && effect->data.ext_anim) {
        ImGui::Separator();

        bool changed = false;
        if (eg->type == Glitter::X) {
            uint64_t hash_before = hash_fnv1a64m(effect->data.ext_anim_x,
                sizeof(Glitter::Effect::ExtAnimX));

            Glitter::Effect::ExtAnimX* ext_anim = effect->data.ext_anim_x;
            if (ext_anim->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = 4;
                if (ImGui::ColumnComboBox("Chara Index", Glitter::effect_ext_anim_index_name,
                    max_chara, &ext_anim->chara_index, 0, false, &data->imgui_focus))
                    changed = true;

                int32_t node_index = ext_anim->node_index;
                if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (ImGui::ColumnComboBox("Parent Bone", Glitter::effect_ext_anim_node_index_name,
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

                ImGui::ColumnInputScalar("Instance ID", ImGuiDataType_U32, &ext_anim->instance_id,
                    0, 0, "%d", ImGuiInputTextFlags_CharsDecimal);

                uint64_t file_name_hash = ext_anim->file_name_hash;
                if (glitter_editor_hash_input(glt_edt, "File Name Hash", &file_name_hash)) {
                    ext_anim->file_name_hash = (uint32_t)file_name_hash;
                    changed = true;
                }

                const size_t mesh_name_size = sizeof(ext_anim->mesh_name);
                char mesh_name[sizeof(ext_anim->mesh_name)];
                memcpy(mesh_name, ext_anim->mesh_name, mesh_name_size);
                if (ImGui::ColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim->mesh_name, mesh_name, mesh_name_size);
                    changed = true;
                }
            }

            if (ImGui::CheckboxFlagsEnterKeyPressed("Trans Only",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_TRANS_ONLY))
                changed = true;

            if (ImGui::CheckboxFlagsEnterKeyPressed("No Trans X",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_X))
                changed = true;

            if (ImGui::CheckboxFlagsEnterKeyPressed("No Trans Y",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_Y))
                changed = true;

            if (ImGui::CheckboxFlagsEnterKeyPressed("No Trans Z",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_TRANS_Z))
                changed = true;

            if (ImGui::CheckboxFlagsEnterKeyPressed("Draw Only when Chara Visible",
                (uint32_t*)&ext_anim->flags,
                Glitter::EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA))
                changed = true;

            if (ImGui::CheckboxFlagsEnterKeyPressed("Get Then Update",
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
                if (ImGui::ColumnComboBox("Chara Index", Glitter::effect_ext_anim_index_name,
                    max_chara, &ext_anim->chara_index, 0, false, &data->imgui_focus))
                    changed = true;

                int32_t node_index = ext_anim->node_index;
                if (node_index < Glitter::EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > Glitter::EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = Glitter::EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (ImGui::ColumnComboBox("Parent Bone", Glitter::effect_ext_anim_node_index_name,
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
                if (ImGui::ColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim->mesh_name, mesh_name, mesh_name_size);
                    changed = true;
                }
            }

            if (ImGui::CheckboxFlagsEnterKeyPressed("Trans Only",
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

    ImGui::SetDefaultColumnSpace();
}

static void glitter_editor_property_emitter(GlitterEditor* glt_edt, class_data* data) {
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;

    ImGui::SetColumnSpace(2.0f / 5.0f);

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

    if (ImGui::ColumnDragInt("Start Time",
        &emitter->data.start_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("Life Time",
        &emitter->data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("Loop Start Time", &emitter->data.loop_start_time,
        1.0f, -0x0001, emitter->data.start_time + emitter->data.life_time, "%d", 0)) {
        if (emitter->data.loop_start_time < emitter->data.start_time)
            emitter->data.loop_start_time = 0;
        changed = emitter->data.loop_start_time != sel_emit->data.loop_start_time;
    }

    if (ImGui::ColumnDragInt("Loop End Time", &emitter->data.loop_end_time,
        1.0f, -0x0001, emitter->data.start_time + emitter->data.life_time, "%d", 0)) {
        if (emitter->data.loop_end_time < 0)
            emitter->data.loop_end_time = -1;
        changed = emitter->data.loop_end_time != sel_emit->data.loop_end_time;
    }

    ImGui::Separator();

    if (ImGui::ColumnDragVec3("Translation",
        &emitter->translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    if (eg->type != Glitter::X) {
        bool draw_z_axis = emitter->data.direction == Glitter::DIRECTION_Z_AXIS;

        vec3 rotation = emitter->rotation;
        if (draw_z_axis)
            rotation.z -= (float_t)M_PI_2;
        rotation *= RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragVec3("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            rotation *= DEG_TO_RAD_FLOAT;
            if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            emitter->rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation = emitter->rotation * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragVec3("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            emitter->rotation = rotation * DEG_TO_RAD_FLOAT;
            changed = true;
        }
    }

    vec3 rotation_add = emitter->data.rotation_add * RAD_TO_DEG_FLOAT;
    if (ImGui::ColumnDragVec3("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        emitter->data.rotation_add = rotation_add * DEG_TO_RAD_FLOAT;
        changed = true;
    }

    if (ImGui::ColumnDragVec3("Scale",
        &emitter->scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    ImGui::Separator();

    if (ImGui::CheckboxFlagsEnterKeyPressed("Loop",
        (uint32_t*)&emitter->data.flags,
        Glitter::EMITTER_LOOP))
        changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Kill On End",
        (uint32_t*)&emitter->data.flags,
        Glitter::EMITTER_KILL_ON_END))
        changed = true;

    if (eg->type == Glitter::X
        && ImGui::CheckboxFlagsEnterKeyPressed("Use Seed",
            (uint32_t*)&emitter->data.flags,
            Glitter::EMITTER_USE_SEED))
        changed = true;

    ImGui::Separator();

    if (eg->type == Glitter::X) {
        if (emitter->data.flags & Glitter::EMITTER_USE_SEED
            && ImGui::ColumnInputScalar("Seed", ImGuiDataType_U32, &emitter->data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (ImGui::ColumnComboBox("Timer Type", Glitter::emitter_timer_name,
            Glitter::EMITTER_TIMER_BY_DISTANCE,
            (int32_t*)&emitter->data.timer, 0, true, &data->imgui_focus)) {
            float_t emission_interval = emitter->data.emission_interval;
            if (emitter->data.timer == Glitter::EMITTER_TIMER_BY_TIME)
                emission_interval = clamp_def(floorf(emission_interval), 1.0f, FLT_MAX);
            else
                emission_interval = clamp_def(emission_interval, 0.0f, FLT_MAX);
            emitter->data.emission_interval = emission_interval;
            changed = true;
        }
    }

    if (eg->type == Glitter::X
        && emitter->data.timer == Glitter::EMITTER_TIMER_BY_DISTANCE) {
        if (ImGui::ColumnDragFloat("Emit Interval",
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

        if (ImGui::ColumnComboBox("Emission Type", Glitter::emitter_emission_name,
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
            && ImGui::ColumnDragFloat("Emit Interval",
                &emitter->data.emission_interval, 1.0f, 1.0f, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat,
                (flags & Glitter::CURVE_TYPE_EMISSION_INTERVAL) >> 20))
                changed = true;
    }

    if (ImGui::ColumnDragFloat("PTC Per Emit",
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

    if (ImGui::ColumnComboBox("Direction", Glitter::emitter_direction_name,
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
                    if (!(flags & Glitter::CURVE_TYPE_ROTATION_Z))
                        emitter->animation.AddValue(eg->type,
                            (float_t)M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                    emitter->rotation.z += (float_t)M_PI_2;
                }
                else if (!draw_z_axis && prev_draw_z_axis) {
                    if (!(flags & Glitter::CURVE_TYPE_ROTATION_Z))
                        emitter->animation.AddValue(eg->type,
                            (float_t)-M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                    emitter->rotation.z -= (float_t)M_PI_2;
                }
            }
            emitter->data.direction = Glitter::emitter_direction_types[direction];
            changed = true;
        }

    if (ImGui::ColumnComboBox("Type", Glitter::emitter_name,
        Glitter::EMITTER_POLYGON,
        (int32_t*)&emitter->data.type, 0, true, &data->imgui_focus))
        changed = true;

    ImGui::Separator();

    switch (emitter->data.type) {
    case Glitter::EMITTER_BOX: {
        ImGui::PushID("Box");
        if (ImGui::ColumnDragVec3("Size",
            &emitter->data.box.size, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_CYLINDER: {
        float_t start_angle, end_angle;

        ImGui::PushID("Cylinder");
        if (ImGui::ColumnDragFloat("Radius",
            &emitter->data.cylinder.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (ImGui::ColumnDragFloat("Height",
            &emitter->data.cylinder.height, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        start_angle = emitter->data.cylinder.start_angle * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragFloat("Start Angle",
            &start_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.cylinder.start_angle = start_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        end_angle = emitter->data.cylinder.end_angle * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragFloat("End Angle",
            &end_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.cylinder.end_angle = end_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (ImGui::CheckboxEnterKeyPressed("On Edge",
            &emitter->data.cylinder.on_edge))
            changed = true;

        if (ImGui::ColumnComboBox("Direction",
            Glitter::emitter_emission_direction_name,
            Glitter::EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter->data.cylinder.direction, 0, true, &data->imgui_focus))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_SPHERE: {
        float_t latitude, longitude;

        ImGui::PushID("Sphere");
        if (ImGui::ColumnDragFloat("Radius",
            &emitter->data.sphere.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        latitude = emitter->data.sphere.latitude * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragFloat("Latitude",
            &latitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.sphere.latitude = latitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        longitude = emitter->data.sphere.longitude * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragFloat("Longitude",
            &longitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter->data.sphere.longitude = longitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (ImGui::CheckboxEnterKeyPressed("On Edge",
            &emitter->data.sphere.on_edge))
            changed = true;

        if (ImGui::ColumnComboBox("Direction",
            Glitter::emitter_emission_direction_name,
            Glitter::EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter->data.sphere.direction, 0, true, &data->imgui_focus))
            changed = true;
        ImGui::PopID();
    } break;
    case Glitter::EMITTER_POLYGON: {
        ImGui::PushID("Polygon");
        if (ImGui::ColumnDragFloat("Scale",
            &emitter->data.polygon.size, 0.0001f, 0.0f, FLT_MAX,
            "%g", ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (ImGui::ColumnDragInt("Count",
            &emitter->data.polygon.count, 1.0f, 0, 0x7FFFFF, "%d", 0))
            changed = true;

        if (eg->type == Glitter::X)
            if (ImGui::ColumnComboBox("Direction",
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

    ImGui::SetDefaultColumnSpace();
}

static void glitter_editor_property_particle(GlitterEditor* glt_edt, class_data* data) {
    Glitter::Effect* sel_efct = glt_edt->selected_effect;
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;

    ImGui::SetColumnSpace(2.0f / 5.0f);

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
    if (ImGui::CheckboxFlagsEnterKeyPressed("Loop",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_LOOP))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Emitter Local",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_EMITTER_LOCAL))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Scale Y by X",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_SCALE_Y_BY_X))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Rebound Plane",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_REBOUND_PLANE))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Rotate by Emitter",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_ROTATE_BY_EMITTER))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Scale by Emitter",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_SCALE))
        flags_changed = true;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Texture Mask##Flag",
        (uint32_t*)&particle->data.flags,
        Glitter::PARTICLE_TEXTURE_MASK))
        flags_changed = true;

    if (eg->type == Glitter::X) {
        if (ImGui::CheckboxFlagsEnterKeyPressed("Disable Depth Test",
            (uint32_t*)&particle->data.flags,
            Glitter::PARTICLE_DEPTH_TEST))
            flags_changed = true;

        if (particle->data.type == Glitter::PARTICLE_LOCUS
            && ImGui::CheckboxFlagsEnterKeyPressed("Rotate Locus",
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
        if (ImGui::ColumnDragVec2I("Life Time [R]",
            &life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.life_time = life_time.x;
            particle->data.life_time_random = life_time.y;
            changed = true;
        }

        vec2i fade_in;
        fade_in.x = particle->data.fade_in;
        fade_in.y = particle->data.fade_in_random;
        if (ImGui::ColumnDragVec2I("Fade In [R]",
            &fade_in, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.fade_in = fade_in.x;
            particle->data.fade_in_random = fade_in.y;
            changed = true;
        }

        vec2i fade_out;
        fade_out.x = particle->data.fade_out;
        fade_out.y = particle->data.fade_out_random;
        if (ImGui::ColumnDragVec2I("Fade Out [R]",
            &fade_out, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle->data.fade_out = fade_out.x;
            particle->data.fade_out_random = fade_out.y;
            changed = true;
        }

        if (ImGui::ColumnComboBox("Type", Glitter::particle_name,
            Glitter::PARTICLE_MESH,
            (int32_t*)&particle->data.type, 0, true, &data->imgui_focus))
            changed = true;
    }
    else {
        if (ImGui::ColumnDragInt("Life Time",
            &particle->data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
            changed = true;

        if (ImGui::ColumnComboBox("Type", Glitter::particle_name,
            Glitter::PARTICLE_LOCUS,
            (int32_t*)&particle->data.type, 0, true, &data->imgui_focus))
            changed = true;
    }

    if (particle->data.type != Glitter::PARTICLE_MESH
        && ImGui::ColumnComboBox("Pivot", Glitter::pivot_name,
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

        if (ImGui::ColumnComboBox("Draw Type", Glitter::particle_draw_type_name,
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
                        if (!(flags & Glitter::CURVE_TYPE_ROTATION_Z))
                            particle->animation.AddValue(eg->type,
                                (float_t)M_PI_2, Glitter::CURVE_TYPE_ROTATION_Z);
                        particle->data.rotation.z += (float_t)M_PI_2;
                    }
                    else if (!draw_z_axis && prev_draw_z_axis) {
                        if (!(flags & Glitter::CURVE_TYPE_ROTATION_Z))
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
        if (ImGui::ColumnDragFloat("Z Offset",
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
        rotation *= RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragVec3("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            rotation *= DEG_TO_RAD_FLOAT;
            if (draw_prev_pos)
                rotation.z += (float_t)M_PI;
            else if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            particle->data.rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation = particle->data.rotation * RAD_TO_DEG_FLOAT;
        if (ImGui::ColumnDragVec3("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            particle->data.rotation = rotation * DEG_TO_RAD_FLOAT;
            changed = true;
        }
    }

    vec3 rotation_random = particle->data.rotation_random * RAD_TO_DEG_FLOAT;
    if (ImGui::ColumnDragVec3("Rotation Random",
        &rotation_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        particle->data.rotation_random = rotation_random  * DEG_TO_RAD_FLOAT;
        changed = true;
    }

    vec3 rotation_add = particle->data.rotation_add * RAD_TO_DEG_FLOAT;
    if (ImGui::ColumnDragVec3("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        particle->data.rotation_add = rotation_add * DEG_TO_RAD_FLOAT;
        changed = true;
    }

    vec3 rotation_add_random = particle->data.rotation_add_random * RAD_TO_DEG_FLOAT;
    if (ImGui::ColumnDragVec3("Rotation Add Random",
        &rotation_add_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & Glitter::CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        particle->data.rotation_add_random = rotation_add_random * DEG_TO_RAD_FLOAT;
        changed = true;
    }

    ImGui::DisableElementPush(particle->data.type != Glitter::PARTICLE_MESH);
    if (ImGui::ColumnDragVec2("Scale",
        (vec2*)&particle->data.scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle->data.flags & Glitter::PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;

    if (ImGui::ColumnDragVec2("Scale Random",
        (vec2*)&particle->data.scale_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle->data.flags & Glitter::PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;
    ImGui::DisableElementPop(particle->data.type != Glitter::PARTICLE_MESH);

    ImGui::Separator();

    if (ImGui::ColumnDragVec3("Direction",
        &particle->data.direction, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (ImGui::ColumnDragVec3("Direction Random",
        &particle->data.direction_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 speed;
    speed.x = particle->data.speed;
    speed.y = particle->data.speed_random;
    if (ImGui::ColumnDragVec2("Speed [R]",
        &speed, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.speed = speed.x;
        particle->data.speed_random = speed.y;
        changed = true;
    }

    vec2 deceleration;
    deceleration.x = particle->data.deceleration;
    deceleration.y = particle->data.deceleration_random;
    if (ImGui::ColumnDragVec2("Deceleration [R]",
        &deceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.deceleration = deceleration.x;
        particle->data.deceleration_random = deceleration.y;
        changed = true;
    }

    if (ImGui::ColumnDragVec3("Gravity",
        &particle->data.gravity, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (ImGui::ColumnDragVec3("Acceleration",
        &particle->data.acceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (ImGui::ColumnDragVec3("Acceleration Random",
        &particle->data.acceleration_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 reflection_coeff;
    reflection_coeff.x = particle->data.reflection_coeff;
    reflection_coeff.y = particle->data.reflection_coeff_random;
    if (ImGui::ColumnDragVec2("Refl Coeff [R]",
        &reflection_coeff, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle->data.reflection_coeff = reflection_coeff.x;
        particle->data.reflection_coeff_random = reflection_coeff.y;
        changed = true;
    }

    if (ImGui::ColumnDragFloat("Rebound Plane Y",
        &particle->data.rebound_plane_y, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    ImGui::Separator();

    vec4 color = particle->data.color;
    if (ImGui::ColumnColorEdit4("Color", &color,
        ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
        changed = true;
    particle->data.color = color;

    if (ImGui::ColumnComboBox("UV Index Type", Glitter::uv_index_type_name,
        Glitter::UV_INDEX_USER,
        (int32_t*)&particle->data.uv_index_type, 0, true, &data->imgui_focus))
        changed = true;

    vec2i split;
    split.x = particle->data.split_u;
    split.y = particle->data.split_v;
    if (ImGui::ColumnSliderLogVec2I("UV Split",
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

    if (ImGui::ColumnDragInt("Frame Step UV",
        &particle->data.frame_step_uv, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("UV Index Start",
        &particle->data.uv_index_start, 1.0f, 0, uv_max_count, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("UV Index End",
        &particle->data.uv_index_end, 1.0f, particle->data.uv_index_start + 1, uv_max_count + 1, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragInt("UV Index",
        &particle->data.uv_index, 1.0f, particle->data.uv_index_start,
        particle->data.uv_index_end - 1, "%d", 0))
        changed = true;

    if (ImGui::ColumnDragVec2("UV Scroll Add",
        &particle->data.uv_scroll_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (ImGui::ColumnDragFloat("UV Scroll Add Scale",
        &particle->data.uv_scroll_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (particle->data.type == Glitter::PARTICLE_MESH) {
        bool uv_2nd_add = particle->data.sub_flags & Glitter::PARTICLE_SUB_UV_2ND_ADD ? true : false;
        ImGui::DisableElementPush(uv_2nd_add);
        if (ImGui::ColumnDragVec2("UV Scroll 2nd Add",
            &particle->data.uv_scroll_2nd_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (ImGui::ColumnDragFloat("UV Scroll 2nd Add Scale",
            &particle->data.uv_scroll_2nd_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        ImGui::DisableElementPop(uv_2nd_add);
    }

    if (particle->data.type != Glitter::PARTICLE_MESH) {
        int32_t idx0 = 0;
        int32_t idx1 = 0;
        Glitter::EffectGroup* eg = glt_edt->effect_group;
        size_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        for (size_t i = 0; i < rc && (!idx0 || !idx1); i++) {
            if (!idx0 && rh[i] == particle->data.tex_hash)
                idx0 = (int32_t)(i + 1);

            if (!idx1 && rh[i] == particle->data.mask_tex_hash)
                idx1 = (int32_t)(i + 1);
        }

        char** texture_name_list = new char*[rc + 1];
        char* texture_names = new char[0x40 * (rc + 1)];

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
        delete[] texture_names;
        delete[] texture_name_list;

        Glitter::ParticleBlendDraw blend_draw = Glitter::particle_blend_draw_default;
        for (int32_t i = 0; i < Glitter::particle_blend_mask_types_count; i++)
            if (Glitter::particle_blend_draw_types[i] == particle->data.blend_mode) {
                blend_draw = (Glitter::ParticleBlendDraw)i;
                break;
            }

        if (ImGui::ColumnComboBox("Blend Mode", Glitter::particle_blend_draw_name,
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

            if (ImGui::ColumnComboBox("Mask Blend Mode", Glitter::particle_blend_mask_name,
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

        particle->data.uv_index = min_def(particle->data.uv_index, uv_max_count);
        break;
    }

    if (particle->data.uv_index_end >= 0)
        particle->data.uv_index_count = particle->data.uv_index_end - particle->data.uv_index_start + 1;
    else
        particle->data.uv_index_count = uv_max_count - particle->data.uv_index_start;

    if (particle->data.uv_index_count < 0)
        particle->data.uv_index_count = 0;

    if (ImGui::CheckboxFlagsEnterKeyPressed("Use Animation Curve",
        (uint32_t*)&particle->data.sub_flags,
        Glitter::PARTICLE_SUB_USE_CURVE))
        changed = true;

    if (eg->type == Glitter::X && particle->data.type == Glitter::PARTICLE_MESH)
        if (ImGui::CheckboxFlagsEnterKeyPressed("UV Add",
            (uint32_t*)&particle->data.sub_flags,
            Glitter::PARTICLE_SUB_UV_2ND_ADD))
            changed = true;

    ImGui::Separator();

    if (eg->type != Glitter::X || particle->data.type == Glitter::PARTICLE_QUAD)
        if (ImGui::ColumnDragInt("Count",
            &particle->data.count, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

    if (ImGui::ColumnDragFloat("Emission",
        &particle->data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        if (sel_efct->data.flags & Glitter::EFFECT_EMISSION
            || particle->data.emission >= Glitter::min_emission)
            enum_or(particle->data.flags, Glitter::PARTICLE_EMISSION);
        else
            enum_and(particle->data.flags, ~Glitter::PARTICLE_EMISSION);
        changed = true;
    }

    if (ImGui::ColumnDragInt("Unk 0",
        &particle->data.unk0, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (eg->version >= 7 && ImGui::ColumnDragInt("Unk 1",
        &particle->data.unk1, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (particle->data.type == Glitter::PARTICLE_LOCUS) {
        if (ImGui::ColumnDragInt("Locus Hist Size",
            &particle->data.locus_history_size, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

        if (ImGui::ColumnDragInt("Locus Hist Size Rand",
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

        ImGui::StartPropertyColumn("Object Set");
        if (ImGui::BeginCombo("##Object Set", handler ? handler->name.c_str() : "None", 0)) {
            ssize_t set_index = handler ? object_storage_get_obj_set_index(set_id) : -1;

            ImGui::PushID(-1);
            if (ImGui::Selectable("None", !handler)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter))
                set_index = -1;
            ImGui::PopID();

            for (ssize_t i = 0; i < object_set_count; i++) {
                obj_set_handler* handler = object_storage_get_obj_set_handler_by_index(i);
                if (!handler || !handler->obj_set)
                    continue;

                ImGui::PushID((int32_t)i);
                if (ImGui::Selectable(handler->name.c_str(), handler->set_id == set_id)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
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
        ImGui::EndPropertyColumn();

        ImGui::DisableElementPush(set_id != -1);
        obj* obj = 0;
        if (handler && handler->obj_set) {
            obj_set* set = handler->obj_set;
            for (uint32_t i = 0; i < set->obj_num; i++)
                if (set->obj_data[i].id == obj_id) {
                    obj = &set->obj_data[i];
                    break;
                }
        }

        ImGui::StartPropertyColumn("Object");
        if (ImGui::BeginCombo("##Object", obj ? obj->name : "None", 0)) {
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
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && obj))
                    obj_index = -1;
                ImGui::PopID();

                for (uint32_t i = 0; i < set->obj_num; i++) {
                    ImGui::PushID(i);
                    ::obj* obj = &set->obj_data[i];
                    if (ImGui::Selectable(obj->name, obj->id == obj_id)
                        || ImGui::ItemKeyPressed(ImGuiKey_Enter)
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
        ImGui::EndPropertyColumn();
        ImGui::DisableElementPop(set_id != -1);

        /*if (glitter_editor_hash_input("Object Set Name Hash", &mesh->object_set_name_hash))
            changed = true;

        if (glitter_editor_hash_input("Object Name Hash", &mesh->object_name_hash))
            changed = true;

        const size_t mesh_name_size = sizeof(mesh->mesh_name);
        char mesh_name[sizeof(mesh->mesh_name)];
        memcpy(mesh_name, mesh->mesh_name, mesh_name_size);
        if (ImGui::ColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
            memcpy(mesh->mesh_name, mesh_name, mesh_name_size);
            changed = true;
        }

        if (glitter_editor_hash_input("Sub Mesh Hash", &mesh->sub_mesh_hash))
            changed = true;*/
    }

    uint64_t hash_after = hash_fnv1a64m(glt_edt->selected_particle, sizeof(Glitter::Particle));
    if (hash_before != hash_after && changed)
        glt_edt->input_reload = true;

    ImGui::SetDefaultColumnSpace();
}

static bool glitter_editor_property_particle_texture(GlitterEditor* glt_edt,
    class_data* data, const char* label, char** items, Glitter::Particle* particle,
    GLuint* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
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

    ImGui::StartPropertyColumn(label);
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
                    uv_index = particle->data.uv_index_start
                        + time.LowPart % particle->data.uv_index_count;
                } break;
                }
                uv_index = min_def(uv_index, particle->data.uv_index_end);
            }
        }
        else if (particle->data.uv_index_type == Glitter::UV_INDEX_INITIAL_RANDOM_FIXED) {
            if (particle->data.uv_index_count > 0 && !(*tex_frame % 40)) {
                uv_index = particle->data.uv_index;

                LARGE_INTEGER time;
                QueryPerformanceCounter(&time);
                uv_index += time.LowPart % particle->data.uv_index_count;
            }
        }
        else if (particle->data.frame_step_uv && !(*tex_frame % particle->data.frame_step_uv)) {
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
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
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
    ImGui::EndPropertyColumn();

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

static void glitter_editor_popups(GlitterEditor* glt_edt, class_data* data) {
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

static void glitter_editor_file_create_popup(GlitterEditor* glt_edt, class_data* data,
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
            if (ImGui::ButtonEnterKeyPressed("F2", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::F2;
                close = true;
            }

            ImGui::TableNextColumn();
            if (ImGui::ButtonEnterKeyPressed("FT", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::FT;
                close = true;
            }

            /*ImGui::TableNextColumn();
            if (ImGui::ButtonEnterKeyPressed("X", ImGui::GetContentRegionAvail())) {
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

static void glitter_editor_file_load_popup(GlitterEditor* glt_edt, class_data* data,
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
            if (ImGui::ButtonEnterKeyPressed("F2", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::F2;
                close = true;
            }

            ImGui::TableNextColumn();
            if (ImGui::ButtonEnterKeyPressed("FT", ImGui::GetContentRegionAvail())) {
                glt_edt->load_glt_type = Glitter::FT;
                close = true;
            }

            ImGui::TableNextColumn();
            if (ImGui::ButtonEnterKeyPressed("X", ImGui::GetContentRegionAvail())) {
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

static void glitter_editor_file_load_model_popup(GlitterEditor* glt_edt, class_data* data,
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
                    if (ImGui::ButtonEnterKeyPressed("F2LE",  ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_F2LE;
                        close = true;
                    }
                }

                if (data_f2be_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("F2BE", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_F2BE;
                        close = true;
                    }
                }
                break;
            case Glitter::FT:
                if (data_aft_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("AFT", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_AFT;
                        close = true;
                    }
                }

                if (data_ft_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("FT", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_FT;
                        close = true;
                    }
                }

                if (data_m39_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("M39", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_M39;
                        close = true;
                    }
                }
                break;
            case Glitter::X:
                if (data_x_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("X", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_X;
                        close = true;
                    }
                }

                if (data_xhd_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("XHD", ImGui::GetContentRegionAvail())) {
                        glt_edt->load_data_type = DATA_XHD;
                        close = true;
                    }
                }

                if (data_vrfl_enable) {
                    ImGui::TableNextColumn();
                    if (ImGui::ButtonEnterKeyPressed("VRFL", ImGui::GetContentRegionAvail())) {
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

static void glitter_editor_file_load_error_list_popup(GlitterEditor* glt_edt, class_data* data,
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
        if (ImGui::ButtonEnterKeyPressed("OK", { x, y }))
            ImGui::CloseCurrentPopup();
        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

static void glitter_editor_file_save_popup(GlitterEditor* glt_edt, class_data* data,
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
                if (ImGui::ButtonEnterKeyPressed("X", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::X;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (ImGui::ButtonEnterKeyPressed("X HD", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = Glitter::X;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (ImGui::ButtonEnterKeyPressed("None", ImGui::GetContentRegionAvail())) {
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
                if (ImGui::ButtonEnterKeyPressed("F2 PSV", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::F2;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (ImGui::ButtonEnterKeyPressed("F2 PS3", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = Glitter::F2;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (ImGui::ButtonEnterKeyPressed("FT", ImGui::GetContentRegionAvail())) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = Glitter::FT;
                    close = true;
                }

                ImGui::TableNextColumn();
                if (ImGui::ButtonEnterKeyPressed("None", ImGui::GetContentRegionAvail())) {
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

inline static float_t convert_height_to_value(GlitterEditor::CurveEditor* crv_edt,
    float_t val, float_t pos, float_t size, float_t min, float_t max) {
    float_t t = (val - pos - crv_edt->height_offset) / (size - crv_edt->height_offset);
    return (1.0f - t) * (max - min) + min;
}
inline static float_t convert_value_to_height(GlitterEditor::CurveEditor* crv_edt,
    float_t val, float_t pos, float_t size, float_t min, float_t max) {
    float_t t = 1.0f - (val - min) / (max - min);
    return pos + t * (size - crv_edt->height_offset) + crv_edt->height_offset;
}

static void glitter_editor_curve_editor_curve_set(GlitterEditor* glt_edt,
    Glitter::Curve* curve, Glitter::CurveType type) {
    Glitter::EffectGroup* eg = glt_edt->effect_group;
    curve->type = type;
    curve->repeat = false;
    curve->start_time = 0;
    curve->end_time = 0;
    curve->flags = (Glitter::CurveFlag)0;
    curve->random_range = 0.0f;
    curve->keys.clear();
#if defined(ReDIVA_DEV)
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

static Glitter::Curve::Key* glitter_editor_curve_editor_get_closest_key(
    GlitterEditor* glt_edt, Glitter::Curve* curve) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

    Glitter::Curve::Key* key = 0;
    std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;
    for (Glitter::Curve::Key& i : *keys) {
        if (i.frame == crv_edt->frame)
            return &i;

        if (!key || (key && abs(i.frame - crv_edt->frame)
            <= abs(key->frame - crv_edt->frame)))
            key = &i;
    }
    return &key[0];
}

static Glitter::Curve::Key* glitter_editor_curve_editor_get_selected_key(
    GlitterEditor* glt_edt, Glitter::Curve* curve) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

    std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;
    for (Glitter::Curve::Key& i : *keys)
        if (i.frame == crv_edt->frame)
            return &i;
    return 0;
}

static float_t glitter_editor_curve_editor_get_value(GlitterEditor* glt_edt, Glitter::CurveType type) {
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

static void glitter_editor_curve_editor_key_manager(GlitterEditor* glt_edt,
    std::vector<Glitter::Curve::Key>* keys, bool* add_key, bool* del_key) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

    Glitter::Curve* curve = crv_edt->list[crv_edt->type];
    *add_key = true;
    *del_key = false;
    for (Glitter::Curve::Key& i : *keys)
        if (i.frame > curve->end_time)
            break;
        else if (i.frame == crv_edt->frame) {
            *add_key = false;
            *del_key = true;
            break;
        }
}

static void glitter_editor_curve_editor_property_window(GlitterEditor* glt_edt,
    class_data* data) {
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;

    crv_edt->key_edit = false;
    if (crv_edt->type < Glitter::CURVE_TRANSLATION_X
        || crv_edt->type > Glitter::CURVE_V_SCROLL_ALPHA_2ND)
        return;

    Glitter::EffectGroup* eg = glt_edt->effect_group;
    Glitter::Curve* curve = crv_edt->list[crv_edt->type];
    if (!curve)
        return;

    const bool fix_rot_z = eg->type != Glitter::X && curve->type == Glitter::CURVE_ROTATION_Z
        && ((glt_edt->selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && sel_emit->data.direction == Glitter::DIRECTION_Z_AXIS)
            || (glt_edt->selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && sel_ptcl->data.draw_type == Glitter::DIRECTION_Z_AXIS));

    float_t scale;
    float_t inv_scale;
    float_t min;
    float_t min_random;

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

    if (curve->type == Glitter::CURVE_EMISSION_INTERVAL) {
        min = -1.0f;
        min_random = 0.0f;
    }
    else if (curve->type == Glitter::CURVE_PARTICLES_PER_EMISSION) {
        min = 0.0f;
        min_random = 0.0f;
    }
    else {
        min = -FLT_MAX;
        min_random = -FLT_MAX;
    }

    bool random_range = curve->flags & Glitter::CURVE_RANDOM_RANGE ? true : false;
    bool key_random_range = curve->flags & Glitter::CURVE_KEY_RANDOM_RANGE ? true : false;

    bool changed = false;
    bool key_edit = false;

    float_t value = 0.0f;
    if (curve->keys_rev.size() > 1) {
        Glitter::Curve::Key* i_begin = curve->keys_rev.data();
        Glitter::Curve::Key* i_end = curve->keys_rev.data() + curve->keys_rev.size();
        Glitter::Curve::Key* c = i_begin;
        Glitter::Curve::Key* n = c + 1;
        while (n != i_end && n->frame < crv_edt->frame) {
            c++;
            n++;
        }

        if (c + 1 != i_end)
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
        if (ImGui::ColumnComboBox("Type", Glitter::key_name,
            Glitter::KEY_HERMITE,
            (int32_t*)&c->type, 0, true, &data->imgui_focus))
            changed = true;
        key_edit |= ImGui::IsItemFocused();

        float_t value = c->value;
        if (fix_rot_z)
            value -= (float_t)M_PI_2;
        value *= scale;
        if (ImGui::ColumnDragFloat("Value",
            &value, 0.0001f, min, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            value *= inv_scale;
            if (fix_rot_z)
                value += (float_t)M_PI_2;
            c->value = value;
            changed = true;
        }
        key_edit |= ImGui::IsItemFocused();

        ImGui::DisableElementPush(key_random_range);
        float_t random = c->random_range * scale;
        if (ImGui::ColumnDragFloat("Random Range",
            &random, 0.0001f, min_random, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            random *= inv_scale;
            if (random != c->random_range) {
                c->random_range = random;
                changed = true;
            }
        }
        key_edit |= ImGui::IsItemFocused();
        ImGui::DisableElementPop(key_random_range);

        if (n && c->type == Glitter::KEY_HERMITE) {
            float_t tangent2 = c->tangent2 * scale;
            if (ImGui::ColumnDragFloat("Tangent 1",
                &tangent2, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                c->tangent2 = tangent2 * inv_scale;
                changed = true;
            }
            key_edit |= ImGui::IsItemFocused();

            float_t tangent1 = n->tangent1 * scale;
            if (ImGui::ColumnDragFloat("Tangent 2",
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
        if (ImGui::CheckboxEnterKeyPressed("Repeat", &curve->repeat))
            changed = true;

        int32_t start_time = curve->start_time;
        if (ImGui::ColumnDragInt("Start Time",
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
        if (ImGui::ColumnDragInt("End Time",
            &end_time, 1, 0, 0x7FFF, "%d", 0)) {
            if (curve->start_time > end_time)
                end_time = curve->start_time;

            if (curve->end_time != end_time) {
                curve->end_time = end_time;
                changed = true;
            }
        }

        ImGui::DisableElementPush(random_range);
        float_t random = curve->random_range * scale;
        if (ImGui::ColumnDragFloat("Random Range",
            &random, 0.0001f, min_random, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            random *= inv_scale;
            if (random != curve->random_range) {
                curve->random_range = random;
                changed = true;
            }
        }
        ImGui::DisableElementPop(random_range);

        int32_t c_type = curve->version;
        ImGui::Text("C Type:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f);
        if (ImGui::InputInt("##C Type:", &c_type, 1, 1)) {
            if (eg->type == Glitter::X)
                c_type = clamp_def(c_type, 1, 2);
            else
                c_type = clamp_def(c_type, 0, 1);

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
                k_type = clamp_def(k_type, 2, 3);
            else
                k_type = clamp_def(k_type, 0, 2);

            if (k_type != curve->keys_version)
                changed = true;
            curve->keys_version = k_type;
        }

        ImGui::Separator();

        ImGui::PushID("Flags");
        if (ImGui::CheckboxFlagsEnterKeyPressed("Baked",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_BAKED))
            changed = true;

        if (eg->type == Glitter::X) {
            ImGui::DisableElementPush(curve->flags & Glitter::CURVE_BAKED ? true : false);
            if (ImGui::CheckboxFlagsEnterKeyPressed("Baked Full",
                (uint32_t*)&curve->flags,
                Glitter::CURVE_BAKED_FULL))
                changed = true;
            ImGui::DisableElementPop(curve->flags & Glitter::CURVE_BAKED ? true : false);
        }

        ImGui::Separator();

        if (ImGui::CheckboxFlagsEnterKeyPressed("Random Range",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_RANDOM_RANGE))
            changed = true;

        if (ImGui::CheckboxFlagsEnterKeyPressed("Key Random Range",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_KEY_RANDOM_RANGE))
            changed = true;

        if (ImGui::CheckboxFlagsEnterKeyPressed("Random Range Negate",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_RANDOM_RANGE_NEGATE))
            changed = true;

        if (ImGui::CheckboxFlagsEnterKeyPressed("Step Random",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_STEP))
            changed = true;

        if (ImGui::CheckboxFlagsEnterKeyPressed("Curve Negate",
            (uint32_t*)&curve->flags,
            Glitter::CURVE_NEGATE))
            changed = true;

        if (ImGui::CheckboxFlagsEnterKeyPressed(eg->type == Glitter::FT
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

static void glitter_editor_curve_editor_selector_list_box_selectable(GlitterEditor* glt_edt,
    const Glitter::CurveTypeFlags flags, const char* label, const Glitter::CurveType type, bool& reset) {
    if (!(flags & type))
        return;

    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

    char buf[0x40];
    sprintf_s(buf, sizeof(buf), crv_edt->list[type] ? "%s [*]" : "%s", label);
    if (!ImGui::Selectable(buf, crv_edt->type == type))
        return;

    if (crv_edt->type != type) {
        glt_edt->curve_editor.ResetState(type);
        reset = true;
    }
}

static void glitter_editor_curve_editor_selector_list_box_multi_selectable(GlitterEditor* glt_edt,
    const Glitter::CurveTypeFlags flags, const char* label, const Glitter::CurveType type, bool& reset) {
    if (!(flags & (1 << type)))
        return;

    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

    char buf[0x40];
    sprintf_s(buf, sizeof(buf), crv_edt->type_flags & (1 << type)
        ? (crv_edt->list[type] ? "[V] %s [*]" : "[V] %s")
        : (crv_edt->list[type] ? "    %s [*]" : "    %s"), label);
    if (!ImGui::Selectable(buf, crv_edt->type == type))
        return;

    if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || Input::IsKeyDown(GLFW_KEY_RIGHT_CONTROL))
        glt_edt->curve_editor.SetFlag((Glitter::CurveTypeFlags)(1 << type));
    else if (crv_edt->type != type) {
        glt_edt->curve_editor.ResetState(type);
        reset = true;
    }
}

static void glitter_editor_curve_editor_selector(GlitterEditor* glt_edt, class_data* data) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;
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

        if (flags & Glitter::CURVE_TYPE_TRANSLATION_XYZ
            && ImGui::TreeNodeEx("Translation", tree_node_flags)) {
            ImGui::PushID("T");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "X",
                Glitter::CURVE_TRANSLATION_X, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Y",
                Glitter::CURVE_TRANSLATION_Y, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Z",
                Glitter::CURVE_TRANSLATION_Z, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & Glitter::CURVE_TYPE_ROTATION_XYZ
            && ImGui::TreeNodeEx("Rotation", tree_node_flags)) {
            ImGui::PushID("R");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "X",
                Glitter::CURVE_ROTATION_X, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Y",
                Glitter::CURVE_ROTATION_Y, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Z",
                Glitter::CURVE_ROTATION_Z, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_SCALE_XYZ | Glitter::CURVE_TYPE_SCALE_ALL)
            && ImGui::TreeNodeEx("Scale", tree_node_flags)) {
            ImGui::PushID("S");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "X",
                Glitter::CURVE_SCALE_X, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Y",
                Glitter::CURVE_SCALE_Y, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Z",
                Glitter::CURVE_SCALE_Z, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "All",
                Glitter::CURVE_SCALE_ALL, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_COLOR_RGBA | Glitter::CURVE_TYPE_COLOR_RGB_SCALE)
            && ImGui::TreeNodeEx("Color", tree_node_flags)) {
            ImGui::PushID("C");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "R",
                Glitter::CURVE_COLOR_R, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "G",
                Glitter::CURVE_COLOR_G, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "B",
                Glitter::CURVE_COLOR_B, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "A",
                Glitter::CURVE_COLOR_A, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "RGB Scale",
                Glitter::CURVE_COLOR_RGB_SCALE, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_COLOR_RGBA_2ND | Glitter::CURVE_TYPE_COLOR_RGB_SCALE_2ND)
            && ImGui::TreeNodeEx("Color 2nd", tree_node_flags)) {
            ImGui::PushID("C2");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "R",
                Glitter::CURVE_COLOR_R_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "G",
                Glitter::CURVE_COLOR_G_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "B",
                Glitter::CURVE_COLOR_B_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "A",
                Glitter::CURVE_COLOR_A_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "RGB Scale",
                Glitter::CURVE_COLOR_RGB_SCALE_2ND, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        glitter_editor_curve_editor_selector_list_box_selectable(glt_edt, flags, "Emit Interval",
            Glitter::CURVE_EMISSION_INTERVAL, reset);
        glitter_editor_curve_editor_selector_list_box_selectable(glt_edt, flags, "PTC Per Emit",
            Glitter::CURVE_PARTICLES_PER_EMISSION, reset);

        if (flags & (Glitter::CURVE_TYPE_UV_SCROLL | Glitter::CURVE_TYPE_UV_SCROLL_ALPHA)
            && ImGui::TreeNodeEx("UV Scroll", tree_node_flags)) {
            ImGui::PushID("UV");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "Y",
                Glitter::CURVE_U_SCROLL, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "V",
                Glitter::CURVE_V_SCROLL, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "U Alpha",
                Glitter::CURVE_U_SCROLL_ALPHA, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "V Alpha",
                Glitter::CURVE_V_SCROLL_ALPHA, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }

        if (flags & (Glitter::CURVE_TYPE_UV_SCROLL_2ND | Glitter::CURVE_TYPE_UV_SCROLL_ALPHA_2ND)
            && ImGui::TreeNodeEx("UV Scroll 2nd", tree_node_flags)) {
            ImGui::PushID("UV2");
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "U",
                Glitter::CURVE_U_SCROLL_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "V",
                Glitter::CURVE_V_SCROLL_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "U Alpha",
                Glitter::CURVE_U_SCROLL_ALPHA_2ND, reset);
            glitter_editor_curve_editor_selector_list_box_multi_selectable(glt_edt, flags, "V Alpha",
                Glitter::CURVE_V_SCROLL_ALPHA_2ND, reset);
            ImGui::PopID();
            ImGui::TreePop();
        }
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
    ImGui::SetColumnSpace(3.0f / 7.0f);
    float_t zoom_time = crv_edt->zoom_time * 100.0f;
    ImGui::ColumnSliderFloatButton("Zoom Time",
        &zoom_time, 1.0f, 5.0f, 500.0f, 10.0f, "%.1f%%", 0);
    crv_edt->zoom_time = zoom_time * 0.01f;

    int32_t zoom_value = (int32_t)roundf(crv_edt->zoom_value * 100.0f);
    ImGui::ColumnSliderIntButton("Zoom Value", &zoom_value, 10, 1000, "%d%%", 0);
    crv_edt->zoom_value = (float_t)zoom_value * 0.01f;

    ImGui::ColumnDragFloat("Range",
        &crv_edt->range, 0.0001f, 0.0f, FLT_MAX, "%g", 0);
    ImGui::ColumnDragFloat("Offset",
        &crv_edt->offset, 0.0001f, -FLT_MAX, FLT_MAX, "%g", 0);
    ImGui::SetDefaultColumnSpace();

    bool add_key = false;
    bool del_key = false;
    if (curve)
        glitter_editor_curve_editor_key_manager(glt_edt, &curve->keys_rev, &add_key, &del_key);

    ImGui::Separator();
    float_t w = ImGui::GetContentRegionAvailWidth();
    ImGui::DisableElementPush(add_key);
    if (ImGui::ButtonEnterKeyPressed("Add Key", { w, 0.0f }))
        crv_edt->add_key = true;
    ImGui::DisableElementPop(add_key);

    ImGui::DisableElementPush(del_key);
    if (ImGui::ButtonEnterKeyPressed("Delete Key", { w, 0.0f }))
        crv_edt->del_key = true;
    ImGui::DisableElementPop(del_key);

    bool has_curve_in_list = curve ? true : false;
    ImGui::DisableElementPush(!has_curve_in_list);
    if (ImGui::ButtonEnterKeyPressed("Add Curve", { w, 0.0f }))
        crv_edt->add_curve = true;
    ImGui::DisableElementPop(!has_curve_in_list);

    ImGui::DisableElementPush(has_curve_in_list);
    if (ImGui::ButtonEnterKeyPressed("Delete Curve", { w, 0.0f }))
        crv_edt->del_curve = true;
    ImGui::DisableElementPop(has_curve_in_list);
    ImGui::EndGroup();
}

static void glitter_editor_curve_editor_window(GlitterEditor* glt_edt) {
    Glitter::Emitter* sel_emit = glt_edt->selected_emitter;
    Glitter::Particle* sel_ptcl = glt_edt->selected_particle;
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;

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
        if (add_key && ImGui::IsKeyPressed(ImGuiKey_Insert))
            crv_edt->add_key = true;
        else if (del_key && ImGui::IsKeyPressed(ImGuiKey_Delete))
            crv_edt->del_key = true;
        else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            crv_edt->frame++;
        else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            crv_edt->frame--;

        if (curve)
            crv_edt->frame = clamp_def(crv_edt->frame, curve->start_time, curve->end_time);
    }

    bool exist = true;
    if (crv_edt->add_key && add_key) {
        if (!crv_edt->list[crv_edt->type] || !keys || !keys->size()) {
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
            Glitter::Curve::Key* i_begin = keys->data();
            Glitter::Curve::Key* i_end = keys->data() + keys->size();
            Glitter::Curve::Key* i = i_begin;
            bool is_before_start = keys->data()[0].frame > crv_edt->frame;
            bool has_key_after = false;
            if (!is_before_start)
                for (i++; i != i_end; i++)
                    if (crv_edt->frame <= i->frame) {
                        has_key_after = i_end - i > 0;
                        break;
                    }

            ssize_t pos = i - i_begin;
            if (!is_before_start)
                pos--;

            if (!is_before_start && has_key_after) {
                Glitter::Curve::Key* c = i - 1;
                Glitter::Curve::Key* n = i;

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
                    key.value = lerp_def(c->value, n->value, t);
                    key.random_range = lerp_def(c->random_range, n->random_range, t);
                } break;
                case Glitter::KEY_HERMITE: {
                    int32_t df = n->frame - c->frame;
                    int32_t _df = key.frame - c->frame;
                    std::vector<float_t> v_arr = interpolate_chs(c->value, n->value, c->tangent2, n->tangent1, 0, df);
                    key.value = v_arr[_df];
                    key.random_range = lerp_def(c->random_range, n->random_range, (float_t)_df / (float_t)df);
                    interpolate_chs_reverse(v_arr.data(), v_arr.size(), c->tangent2, key.tangent1, 0, _df);
                    interpolate_chs_reverse(v_arr.data(), v_arr.size(), key.tangent2, n->tangent1, _df, df);
                } break;
                }
                keys->insert(keys->begin() + ++pos, key);
            }
            else if (is_before_start) {
                Glitter::Curve::Key* n = i;

                Glitter::Curve::Key key;
                key.frame = crv_edt->frame;
                key.type = n->type;
                key.value = n->value;
                key.random_range = n->random_range;
                keys->insert(keys->begin() + pos, key);
            }
            else {
                Glitter::Curve::Key* c = i - 1;

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
        Glitter::Curve::Key* i_begin = keys->data();
        Glitter::Curve::Key* i_end = keys->data() + keys->size();
        for (Glitter::Curve::Key* i = i_begin; i != i_end; i++)
            if (&i[0] == crv_edt->key) {
                bool has_key_before = keys->front().frame < i->frame;
                bool has_key_after = keys->back().frame > i->frame;
                if (has_key_before && i[-1].type == Glitter::KEY_HERMITE && has_key_after) {
                    Glitter::Curve::Key* c = i - 1;
                    Glitter::Curve::Key* n = i + 1;
                    int32_t df_c = i->frame - c->frame;
                    int32_t df_n = n->frame - i->frame;

                    std::vector<float_t> v_arr_c = interpolate_chs(c->value,
                        i->value, c->tangent2, i->tangent1, 0, df_c);

                    std::vector<float_t> v_arr_n = i->type == Glitter::KEY_HERMITE
                        ? interpolate_chs(i->value, n->value, i->tangent2, n->tangent1, 0, df_n)
                        : interpolate_linear(i->value, n->value, 0, df_n);

                    size_t v_length = v_arr_c.size() + v_arr_n.size() - 1;
                    float_t* v_arr = new float_t[v_length];
                    memmove(v_arr, v_arr_c.data(), (v_arr_c.size() - 1) * sizeof(float_t));
                    memmove(v_arr + (v_arr_c.size() - 1), v_arr_n.data(), v_arr_n.size() * sizeof(float_t));
                    interpolate_chs_reverse(v_arr, v_length, c->tangent2,
                        n->tangent1, 0, (size_t)n->frame - c->frame);
                    delete[] v_arr;
                }
                keys->erase(keys->begin() + (i - i_begin));
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
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    crv_edt->draw_list = draw_list;
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

    float_t frame_width = crv_edt->frame_width * crv_edt->zoom_time;
    canvas_pos.x -= crv_edt->timeline_pos;

    ImGui::ItemSize(boundaries);
    bool hovered = ImGui::IsMouseHoveringRect(boundaries.Min, boundaries.Max);
    ImGui::RenderFrame(boundaries.Min, boundaries.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, 1.0f);

    static bool can_drag;
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left, false))
        can_drag = true;

    if (can_drag && (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))) {
        crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt, curve);
        crv_edt->frame = crv_edt->key->frame;
    }
    else
        crv_edt->key = glitter_editor_curve_editor_get_selected_key(glt_edt, curve);

    if (can_drag && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        can_drag = false;

    float_t timeline_max_offset = (end_time - start_time) * frame_width;
    timeline_max_offset = max_def(timeline_max_offset, 0.0f) - curve_editor_timeline_base_pos;

    draw_list->PushClipRect(boundaries.Min, boundaries.Max, true);

    const int32_t graph_start = start_time + (int32_t)((crv_edt->timeline_pos
        + frame_width - 1.0f) / frame_width);
    const int32_t graph_end   = start_time + (int32_t)((crv_edt->timeline_pos + canvas_size.x
        + frame_width - 1.0f) / frame_width);

    const ImU32 line_color_valid_bold      = ImGui::GetColorU32({ 0.5f, 0.5f, 0.5f, 0.75f });
    const ImU32 line_color_valid           = ImGui::GetColorU32({ 0.4f, 0.4f, 0.4f, 0.45f });
    const ImU32 line_color_not_valid_bold  = ImGui::GetColorU32({ 0.3f, 0.3f, 0.3f, 0.75f });
    const ImU32 line_color_not_valid       = ImGui::GetColorU32({ 0.2f, 0.2f, 0.2f, 0.45f });

    for (int32_t frame = graph_start; frame <= graph_end; frame++) {
        if (frame < start_time)
            continue;

        int32_t f = frame - start_time;
        float_t x = canvas_pos.x + f * frame_width;
        float_t y = canvas_pos.y + (f % 2 ? 10.0f : 0.0f) + canvas_size.y;

        bool bold_frame = !(f % 10) || !f || frame == end_time;
        bool valid_frame = f >= 0 && frame <= end_time;

        ImU32 line_color = bold_frame
            ? (valid_frame ? line_color_valid_bold : line_color_not_valid_bold)
            : (valid_frame ? line_color_valid      : line_color_not_valid     );
        draw_list->AddLine({ x, y - canvas_size.y },
            { x, y }, line_color, 0.80f);

        if (bold_frame) {
            char buf[0x20];
            snprintf(buf, sizeof(buf), "%d", frame);
            draw_list->AddText({ x + 3, canvas_pos.y }, line_color, buf);
        }
    }

    float_t scale;
    if (curve->type >= Glitter::CURVE_ROTATION_X && curve->type <= Glitter::CURVE_ROTATION_Z)
        scale = RAD_TO_DEG_FLOAT;
    else if (curve->type >= Glitter::CURVE_COLOR_R && curve->type <= Glitter::CURVE_COLOR_RGB_SCALE_2ND)
        scale = 255.0f;
    else
        scale = 1.0f;

    float_t max = crv_edt->range * (1.0f / crv_edt->zoom_value);
    float_t min;
    if (curve->type == Glitter::CURVE_EMISSION_INTERVAL) {
        min = -1.0f;
        if (crv_edt->offset > 0.0f) {
            min += crv_edt->offset * (1.0f / crv_edt->zoom_value);
            max += crv_edt->offset * (1.0f / crv_edt->zoom_value);
        }
    }
    else if (curve->type == Glitter::CURVE_PARTICLES_PER_EMISSION) {
        min = 0.0f;
        if (crv_edt->offset > 0.0f) {
            min += crv_edt->offset * (1.0f / crv_edt->zoom_value);
            max += crv_edt->offset * (1.0f / crv_edt->zoom_value);
        }
    }
    else {
        min = -max;
        min += crv_edt->offset * (1.0f / crv_edt->zoom_value);
        max += crv_edt->offset * (1.0f / crv_edt->zoom_value);
    }

    if (curve->type >= Glitter::CURVE_ROTATION_X && curve->type <= Glitter::CURVE_ROTATION_Z) {
        max *= DEG_TO_RAD_FLOAT;
        min *= DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= Glitter::CURVE_COLOR_R && curve->type <= Glitter::CURVE_COLOR_RGB_SCALE_2ND) {
        max *= (float_t)(1.0 / 255.0);
        min *= (float_t)(1.0 / 255.0);
    }

    float_t base_line = convert_value_to_height(crv_edt, 0.0f, canvas_pos.y, canvas_size.y, min, max);

    ImVec2 p1, p2, p3;
    p1.x = canvas_pos_min.x;
    p1.y = canvas_pos.y + crv_edt->height_offset;
    p2.x = p1.x + canvas_size.x;
    p2.y = canvas_pos.y + crv_edt->height_offset;
    p3.x = canvas_pos_min.x;
    p3.y = canvas_pos.y;
    draw_list->AddLine(p1, p2,
        ImGui::GetColorU32({ 0.5f, 0.5f, 0.5f, 0.85f }), 2.0f);

    const ImU32    red_color = ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.00f });
    const ImU32  green_color = ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 1.00f });
    const ImU32   blue_color = ImGui::GetColorU32({ 0.0f, 0.0f, 1.0f, 1.00f });
    const ImU32  white_color = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.00f });
    const ImU32 nwhite_color = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.75f });
    const ImU32    red_bcolor = ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 0.50f });
    const ImU32  green_bcolor = ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 0.50f });
    const ImU32   blue_bcolor = ImGui::GetColorU32({ 0.0f, 0.0f, 1.0f, 0.50f });
    const ImU32  white_bcolor = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.50f });
    const ImU32 nwhite_bcolor = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.25f });

    const ImU32  default_color = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f });
    const ImU32 selected_color = ImGui::GetColorU32({ 0.0f, 1.0f, 1.0f, 1.0f });

    switch (crv_edt->type) {
    case Glitter::CURVE_TRANSLATION_X:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Y, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Z,  blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_X,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_TRANSLATION_Y:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_X,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Z,  blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Y, green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_TRANSLATION_Z:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_X,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Y, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_TRANSLATION_Z,  blue_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);

        break;
    case Glitter::CURVE_ROTATION_X:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Y, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Z,  blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_X,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_ROTATION_Y:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_X,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Z,  blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Y, green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_ROTATION_Z:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_X,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Y, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_ROTATION_Z,  blue_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_SCALE_X:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Y  ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Z  ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_ALL, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_X  ,    red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_SCALE_Y:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_X  ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Z  ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_ALL, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Y  ,  green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_SCALE_Z:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_X  ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Y  ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_ALL, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Z  ,   blue_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_SCALE_ALL:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_X  ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Y  ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_Z  ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_SCALE_ALL, nwhite_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_R:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R        ,    red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_G:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G        ,   blue_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_B:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B        ,  green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_A:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A        ,  white_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_RGB_SCALE:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE, nwhite_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_R_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G_2ND        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B_2ND        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A_2ND        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE_2ND, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R_2ND        ,    red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);

        break;
    case Glitter::CURVE_COLOR_G_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R_2ND        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B_2ND        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A_2ND        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE_2ND, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G_2ND        ,   blue_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_B_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R_2ND        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G_2ND        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A_2ND        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE_2ND, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B_2ND        ,  green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_A_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R_2ND        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G_2ND        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B_2ND        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE_2ND, nwhite_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A_2ND        ,  white_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_COLOR_RGB_SCALE_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_R_2ND        ,    red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_G_2ND        ,   blue_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_B_2ND        ,  green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_A_2ND        ,  white_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_COLOR_RGB_SCALE_2ND, nwhite_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_U_SCROLL:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_V_SCROLL:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL, green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_U_SCROLL_ALPHA:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL_ALPHA, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL_ALPHA,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_V_SCROLL_ALPHA:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL_ALPHA,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL_ALPHA, green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_U_SCROLL_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_V_SCROLL_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL_2ND,   red_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL_2ND, green_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_U_SCROLL_ALPHA_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL_ALPHA_2ND, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL_ALPHA_2ND,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    case Glitter::CURVE_V_SCROLL_ALPHA_2ND:
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_V_SCROLL_ALPHA_2ND, green_bcolor, false,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        glitter_editor_curve_editor_window_draw(glt_edt, Glitter::CURVE_U_SCROLL_ALPHA_2ND,   red_color ,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    default:
        glitter_editor_curve_editor_window_draw(glt_edt, curve, default_color,  true,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        break;
    }

    for (int32_t j = 0; j < 5; j++) {
        float_t x_pos = canvas_pos_min.x;
        float_t y_pos = canvas_pos.y + j * 0.25f * (canvas_size.y
            - crv_edt->height_offset) + crv_edt->height_offset;

        ImU32 line_color = j % 2 == 1
            ? ImGui::GetColorU32({ 0.35f, 0.35f, 0.35f, 0.45f })
            : ImGui::GetColorU32({ 0.45f, 0.45f, 0.45f, 0.75f });
        bool last = j == 4;
        if (j && !last)
            draw_list->AddLine({ x_pos, y_pos },
                { x_pos + canvas_size.x, y_pos }, line_color, 0.80f);

        float_t val = (max - j * 0.25f * (max - min)) * scale;
        char buf[0x20];
        snprintf(buf, sizeof(buf), "%.2f", val);
        draw_list->AddText({ x_pos + 2, y_pos + (last ? -16 : -2) },
            ImGui::GetColorU32({ 0.65f, 0.65f, 0.65f, 0.85f }), buf);
    }

    const ImU32 key_constant_color = ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f });
    const ImU32   key_linear_color = ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 1.0f });
    const ImU32  key_hermite_color = ImGui::GetColorU32({ 0.0f, 0.0f, 1.0f, 1.0f });

    bool dragged = false;
    bool holding_tan = false;
    Glitter::Curve::Key* i_begin = keys->data();
    Glitter::Curve::Key* i_end = keys->data() + keys->size();
    for (Glitter::Curve::Key* i = i_begin; i != i_end; i++) {
        if (i->frame < graph_start)
            continue;
        else if (i->frame > graph_end)
            break;

        float_t base_value = i->value;
        if (fix_rot_z)
            base_value -= (float_t)M_PI_2;
        float_t x = canvas_pos.x + ((size_t)i->frame - start_time) * frame_width;
        float_t y = convert_value_to_height(crv_edt, base_value, canvas_pos.y, canvas_size.y, min, max);

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

        ImGui::PushID(i);
        ImGui::SetCursorScreenPos({ x - 7.5f, y - 7.5f });
        ImGui::InvisibleButton("val", { 15.0f, 15.0f }, 0);

        if (can_drag && ImGui::IsItemActive()
            && ImGui::IsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
            float_t old_y = y;

            static int32_t base_frame;
            static float_t base_y;
            if ((ImGui::IsKeyPressed(ImGuiKey_LeftShift, false) && !ImGui::IsKeyDown(ImGuiKey_RightShift))
                || (ImGui::IsKeyPressed(ImGuiKey_RightShift, false) && !ImGui::IsKeyDown(ImGuiKey_LeftShift))) {
                base_frame = i->frame;
                base_y = base_value;
            }

            if (i->frame >= start_time && i->frame <= end_time) {
                int32_t start = i - i_begin > 0 ? (i - 1)->frame + 1 : start_time;
                int32_t end = i_end - i > 1 ? (i + 1)->frame - 1 : end_time;
                crv_edt->frame = (int32_t)roundf((io.MousePos.x - canvas_pos.x) / frame_width);
                crv_edt->frame = clamp_def(crv_edt->frame + start_time, start, end);
                if (i->frame != crv_edt->frame) {
                    i->frame = crv_edt->frame;
                    changed = true;
                }
            }

            static float_t mouse_delta_history;
            if ((ImGui::IsKeyPressed(ImGuiKey_LeftShift, false) && !ImGui::IsKeyDown(ImGuiKey_RightShift))
                || (ImGui::IsKeyPressed(ImGuiKey_RightShift, false) && !ImGui::IsKeyDown(ImGuiKey_LeftShift)))
                mouse_delta_history = io.MouseDelta.y;
            else if ((ImGui::IsKeyReleased(ImGuiKey_LeftShift) && !ImGui::IsKeyDown(ImGuiKey_RightShift))
                || ImGui::IsKeyReleased(ImGuiKey_RightShift) && !ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                y += mouse_delta_history;
                y += io.MouseDelta.y;
            }
            else if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
                mouse_delta_history += io.MouseDelta.y;
            else
                y += io.MouseDelta.y;

            if (old_y != y) {
                float_t value = convert_height_to_value(crv_edt,
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

        if (i - i_begin > 0 && (i - 1)->type == Glitter::KEY_HERMITE) {
            ImU32 tangent1_color;
            float_t tangent1;
            ImVec2 circle_pos;
            circle_pos.x = x - 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(crv_edt, tangent1 = i->tangent1,
                canvas_pos.y, canvas_size.y, min, max);
            draw_list->AddLine({ x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                ImGui::SetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                ImGui::InvisibleButton("tan1", { 15.0f, 15.0f }, 0);

                if (can_drag && ImGui::IsItemActive()) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        i->tangent1 = convert_height_to_value(crv_edt,
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

                draw_list->AddCircleFilled(circle_pos,
                    crv_edt->key_radius_in, tangent1_color, 12);
            }
        }

        if (i_end - i > 1 && i->type == Glitter::KEY_HERMITE) {
            ImU32 tangent2_color;
            float_t tangent2;
            ImVec2 circle_pos;
            circle_pos.x = x + 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(crv_edt, tangent2 = i->tangent2,
                canvas_pos.y, canvas_size.y, min, max);
            draw_list->AddLine({ x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                ImGui::SetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                ImGui::InvisibleButton("tan2", { 15.0f, 15.0f }, 0);

                if (can_drag && ImGui::IsItemActive()) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        i->tangent2 = convert_height_to_value(crv_edt,
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

                draw_list->AddCircleFilled(circle_pos,
                    crv_edt->key_radius_in, tangent2_color, 12);
            }
        }

        draw_list->AddCircleFilled({ x, y },
            crv_edt->key_radius_out, crv_edt->key == &i[0] ? selected_color : default_color, 12);
        draw_list->AddCircleFilled({ x, y },
            crv_edt->key_radius_in, value_color, 12);
        ImGui::PopID();
    }

    if (!crv_edt->key_edit && can_drag && ImGui::IsMouseDown(ImGuiMouseButton_Left) && (!holding_tan || dragged)) {
        int32_t frame = (int32_t)roundf((io.MousePos.x - canvas_pos.x) / frame_width);
        crv_edt->frame = clamp_def(frame + start_time, start_time, end_time);
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
            crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt, curve);
            crv_edt->frame = crv_edt->key->frame;
        }
        else
            crv_edt->key = glitter_editor_curve_editor_get_selected_key(glt_edt, curve);
    }
    crv_edt->frame = clamp_def(crv_edt->frame, start_time, end_time);

    int32_t cursor_frame = crv_edt->frame - start_time;
    ImVec2 cursor_pos = canvas_pos;
    cursor_pos.x += cursor_frame * frame_width;
    if (cursor_pos.x >= p1.x - 10.0f && cursor_pos.x <= p1.x + canvas_size.x + 10.0f) {
        ImU32 cursor_color = ImGui::GetColorU32({ 0.8f, 0.8f, 0.8f, 0.75f });
        p1.x = cursor_pos.x;
        p1.y = canvas_pos.y + canvas_size.y;
        draw_list->AddLine(cursor_pos, p1, cursor_color, 2.5f);

        p1 = p2 = p3 = cursor_pos;
        p1.x -= 10.0f;
        p2.y += 10.0f;
        p3.x += 10.0f;
        draw_list->AddTriangleFilled(p1, p2, p3, cursor_color);
    }
    draw_list->PopClipRect();

    if (hovered)
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
            crv_edt->timeline_pos -= io.MouseWheel * 25.0f * 4.0f;
        else
            crv_edt->timeline_pos -= io.MouseWheel * 25.0f;

    crv_edt->timeline_pos = clamp_def(crv_edt->timeline_pos,
        -curve_editor_timeline_base_pos, timeline_max_offset);

    if (crv_edt->list[crv_edt->type] && changed) {
        curve->Recalculate(eg->type);
        *crv_edt->list[crv_edt->type] = *curve;
        glt_edt->input_reload = true;
    }
}

inline static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::CurveType type,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;
    if (!(crv_edt->type_flags & (1 << type)) || !crv_edt->list[type])
        return;

    glitter_editor_curve_editor_window_draw(glt_edt, crv_edt->list[type], line_color, line_front,
        min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max, fix_rot_z);
}

static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::Curve* curve,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;
    ImDrawList* draw_list = crv_edt->draw_list;
    const ImU32     random_range_color = ImGui::GetColorU32({ 0.5f, 0.5f, 0.0f, 0.25f });
    const ImU32 key_random_range_color = ImGui::GetColorU32({ 0.5f, 0.0f, 0.5f, 0.25f });

    const std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;
    if (!keys->size())
        return;

    const float_t frame_width = crv_edt->frame_width * crv_edt->zoom_time;

    const float_t line_thickness = line_front ? 3.0f : 1.0f;

    if (keys->size() == 1) {
        if (line_front && (curve->flags
            & (Glitter::CURVE_RANDOM_RANGE | Glitter::CURVE_KEY_RANDOM_RANGE))) {
            const float_t random_range = curve->random_range;
            const bool random_range_mult   = curve->flags & Glitter::CURVE_RANDOM_RANGE_MULT;
            const bool random_range_negate = curve->flags & Glitter::CURVE_RANDOM_RANGE_NEGATE;
            const bool glt_type_ft = glt_edt->effect_group->type == Glitter::FT;

            const Glitter::Curve::Key* c = &keys->data()[0];
            const Glitter::KeyType k_type = c->type;
            const float_t c_frame = (float_t)c->frame;
            const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
            const float_t c_random_range = random_range_mult && glt_type_ft
                ? c->random_range * c_value : c->random_range;

            const vec3 value = { c_value + c_random_range, c_value, c_value - c_random_range };
            const vec3 random = !random_range_mult
                ? random_range : glt_type_ft
                ? (random_range * value) : (random_range * value * 0.01f);

            glitter_editor_curve_editor_window_draw_random_range(crv_edt,
                canvas_pos_min.x, canvas_pos_max.x,
                value, random, min, max, random_range_negate,
                canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
        }

        const Glitter::Curve::Key* c = &keys->data()[0];
        const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;

        float_t y = convert_value_to_height(crv_edt, c_value,
            canvas_pos.y, canvas_size.y, min, max);

        draw_list->AddLine({ canvas_pos_min.x, y },
            { canvas_pos_max.x, y }, line_color, line_thickness);
        return;
    }

    const Glitter::Curve* base_curve = crv_edt->list[crv_edt->type];
    const int32_t start_time = curve->start_time;
    const int32_t   end_time = curve->  end_time;
    const int32_t base_start_time = base_curve->start_time;
    const int32_t   base_end_time = base_curve->  end_time;

    std::vector<ImVec2> points;
    if (curve->repeat && end_time - start_time) {
        const int32_t graph_start = base_start_time + (int32_t)((crv_edt->timeline_pos
            + frame_width - 1.0f) / frame_width);
        const int32_t graph_end = base_start_time + (int32_t)((crv_edt->timeline_pos + canvas_size.x
            + frame_width - 1.0f) / frame_width);

        const int32_t delta_time = end_time - start_time;
        int32_t repeat_count_pre  = (start_time - graph_start + delta_time - 1) / delta_time;
        int32_t repeat_count_post = (graph_end  -   end_time  + delta_time - 1) / delta_time;
        repeat_count_pre  = max_def(repeat_count_pre , 0);
        repeat_count_post = max_def(repeat_count_post, 0);

        for (int32_t i = repeat_count_pre; i >= 1; i--)
            glitter_editor_curve_editor_window_draw(glt_edt, curve, line_color, line_front,
                min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max,
                fix_rot_z, start_time, end_time,
                base_start_time + delta_time * i, base_end_time + delta_time * i, points, true);
        glitter_editor_curve_editor_window_draw(glt_edt, curve, line_color, line_front,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max,
            fix_rot_z, start_time, end_time, base_start_time, base_end_time, points, true);
        for (int32_t i = 1; i <= repeat_count_post; i++)
            glitter_editor_curve_editor_window_draw(glt_edt, curve, line_color, line_front,
                min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max,
                fix_rot_z, start_time, end_time,
                base_start_time - delta_time * i, base_end_time - delta_time * i, points, true);
    }
    else {
        glitter_editor_curve_editor_window_draw(glt_edt, curve, line_color, line_front,
            min, max, canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max,
            fix_rot_z, start_time, end_time, base_start_time, base_end_time, points);
    }
    draw_list->AddPolyline(points.data(),
        (int32_t)points.size(), line_color, 0, line_thickness);
}

static void glitter_editor_curve_editor_window_draw(GlitterEditor* glt_edt, const Glitter::Curve* curve,
    const ImU32 line_color, const bool line_front, const float_t min, const float_t max,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max,
    const bool fix_rot_z, const int32_t start_time, const int32_t end_time,
    const int32_t base_start_time, const int32_t base_end_time, std::vector<ImVec2>& points, bool loop) {
    GlitterEditor::CurveEditor* crv_edt = &glt_edt->curve_editor;
    ImDrawList* draw_list = crv_edt->draw_list;
    const ImU32     random_range_color = ImGui::GetColorU32({ 0.5f, 0.5f, 0.0f, 0.25f });
    const ImU32 key_random_range_color = ImGui::GetColorU32({ 0.5f, 0.0f, 0.5f, 0.25f });

    const std::vector<Glitter::Curve::Key>* keys = &curve->keys_rev;

    const float_t frame_width = crv_edt->frame_width * crv_edt->zoom_time;

    const float_t line_thickness = line_front ? 3.0f : 1.0f;

    if (line_front && (curve->flags
        & (Glitter::CURVE_RANDOM_RANGE | Glitter::CURVE_KEY_RANDOM_RANGE))) {
        const float_t random_range = curve->random_range;
        const bool random_range_mult   = curve->flags & Glitter::CURVE_RANDOM_RANGE_MULT;
        const bool random_range_negate = curve->flags & Glitter::CURVE_RANDOM_RANGE_NEGATE;
        const bool glt_type_ft = glt_edt->effect_group->type == Glitter::FT;

        bool first_frame_found = false;

        const Glitter::Curve::Key* i_begin = keys->data();
        const Glitter::Curve::Key* i_end = keys->data() + keys->size();
        for (const Glitter::Curve::Key* i = i_begin; i != i_end; i++) {
            if (i->frame <= start_time || i->frame > start_time && !first_frame_found) {
                if (i_end - i <= 1)
                    break;
                else if (i[1].frame <= start_time)
                    continue;

                const Glitter::Curve::Key* c = i;
                const Glitter::Curve::Key* n = i + 1;
                const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
                const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;
                const float_t c_random_range = random_range_mult && glt_type_ft
                    ? c->random_range * c_value : c->random_range;
                const float_t n_random_range = random_range_mult && glt_type_ft
                    ? n->random_range * n_value : n->random_range;

                const vec3 c_value_vec = { c_value + c_random_range, c_value, c_value - c_random_range };
                const vec3 n_value_vec = { n_value + n_random_range, n_value, n_value - n_random_range };

                vec3 value;
                if (i->frame < start_time)
                    switch (c->type) {
                    case Glitter::KEY_CONSTANT:
                        value = c_value_vec;
                        break;
                    case Glitter::KEY_LINEAR:
                    default:
                        value = interpolate_linear_value(c_value_vec, n_value_vec,
                            (float_t)c->frame, (float_t)n->frame, (float_t)start_time);
                        break;
                    case Glitter::KEY_HERMITE:
                        value = interpolate_chs_value(c_value_vec, n_value_vec,
                            (float_t)c->tangent2, (float_t)n->tangent1,
                            (float_t)c->frame, (float_t)n->frame, (float_t)start_time);
                        break;
                    }
                else
                    value = c_value_vec;

                const vec3 random = !random_range_mult
                    ? random_range : glt_type_ft
                    ? (random_range * value) : (random_range * value * 0.01f);

                const float_t x1 = loop
                    ? canvas_pos.x + (float_t)(start_time - base_start_time) * frame_width
                    : canvas_pos_min.x;
                const float_t x2 = canvas_pos.x + (float_t)(max_def(i->frame,
                    start_time) - base_start_time) * frame_width;
                glitter_editor_curve_editor_window_draw_random_range(crv_edt, x1, x2,
                    value, random, min, max, random_range_negate,
                    canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
                first_frame_found = true;
            }
            else if (i->frame > end_time || i->frame == end_time || i_end - i <= 1 && i->frame <= end_time) {
                vec3 value;
                if (i->frame > end_time) {
                    const Glitter::Curve::Key* c = i - 1;
                    const Glitter::Curve::Key* n = i;
                    const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
                    const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;
                    const float_t c_random_range = random_range_mult && glt_type_ft
                        ? c->random_range * c_value : c->random_range;
                    const float_t n_random_range = random_range_mult && glt_type_ft
                        ? n->random_range * n_value : n->random_range;

                    const vec3 c_value_vec = { c_value + c_random_range, c_value, c_value - c_random_range };
                    const vec3 n_value_vec = { n_value + n_random_range, n_value, n_value - n_random_range };

                    switch (c->type) {
                    case Glitter::KEY_CONSTANT:
                        value = c_value_vec;
                        break;
                    case Glitter::KEY_LINEAR:
                    default:
                        value = interpolate_linear_value(c_value_vec, n_value_vec,
                            (float_t)c->frame, (float_t)n->frame, (float_t)end_time);
                        break;
                    case Glitter::KEY_HERMITE:
                        value = interpolate_chs_value(c_value_vec, n_value_vec,
                            (float_t)c->tangent2, (float_t)n->tangent1,
                            (float_t)c->frame, (float_t)n->frame, (float_t)end_time);
                        break;
                    }
                }
                else {
                    const Glitter::Curve::Key* c = i;
                    const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
                    const float_t c_random_range = random_range_mult && glt_type_ft
                        ? c->random_range * c_value : c->random_range;
                    value = { c_value + c_random_range, c_value, c_value - c_random_range };
                }

                const vec3 random = !random_range_mult
                    ? random_range : glt_type_ft
                    ? (random_range * value) : (random_range * value * 0.01f);

                const float_t x1 = canvas_pos.x + (float_t)(min_def(i->frame,
                    end_time) - base_start_time) * frame_width;
                const float_t x2 = loop
                    ? canvas_pos.x + (float_t)(end_time - base_start_time) * frame_width
                    : canvas_pos_max.x;
                glitter_editor_curve_editor_window_draw_random_range(crv_edt, x1, x2,
                    value, random, min, max, random_range_negate,
                    canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
                break;
            }
            else if (i_end - i <= 1)
                break;

            const Glitter::Curve::Key* c = i;
            const Glitter::Curve::Key* n = i + 1;
            const Glitter::KeyType k_type = c->type;
            const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
            const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;
            const float_t c_random_range = random_range_mult && glt_type_ft
                ? c->random_range * c_value : c->random_range;
            const float_t n_random_range = random_range_mult && glt_type_ft
                ? n->random_range * n_value : n->random_range;

            switch (k_type) {
            case Glitter::KEY_CONSTANT: {
                const vec3 value = { c_value + c_random_range, c_value, c_value - c_random_range };
                const vec3 random = !random_range_mult
                    ? random_range : glt_type_ft
                    ? (random_range * value) : (random_range * value * 0.01f);

                const int32_t c_frame = max_def(c->frame, start_time);
                const int32_t n_frame = min_def(n->frame,   end_time);
                const float_t x1 = canvas_pos.x + (float_t)(c_frame - base_start_time) * frame_width;
                const float_t x2 = canvas_pos.x + (float_t)(n_frame - base_start_time) * frame_width;
                glitter_editor_curve_editor_window_draw_random_range(crv_edt, x1, x2,
                    value, random, min, max, random_range_negate,
                    canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
            } break;
            case Glitter::KEY_LINEAR: {
                const int32_t c_frame = max_def(c->frame, start_time);
                const int32_t n_frame = min_def(n->frame,   end_time);
                const float_t x1 = canvas_pos.x + (float_t)(c_frame - base_start_time) * frame_width;
                const float_t x2 = canvas_pos.x + (float_t)(n_frame - base_start_time) * frame_width;

                const float_t _c_value = interpolate_linear_value(c_value, n_value,
                    (float_t)c->frame, (float_t)n->frame, (float_t)c_frame);
                const float_t _n_value = interpolate_linear_value(c_value, n_value,
                    (float_t)c->frame, (float_t)n->frame, (float_t)n_frame);

                const vec3 c_value_vec = { _c_value + c_random_range, _c_value, _c_value - c_random_range };
                const vec3 n_value_vec = { _n_value + n_random_range, _n_value, _n_value - n_random_range };
                const vec3 c_random = !random_range_mult
                    ? random_range : glt_type_ft
                    ? (random_range * c_value_vec) : (random_range * c_value_vec * 0.01f);
                const vec3 n_random = !random_range_mult
                    ? random_range : glt_type_ft
                    ? (random_range * n_value_vec) : (random_range * n_value_vec * 0.01f);

                glitter_editor_curve_editor_window_draw_random_range(crv_edt, x1, x2,
                    c_value_vec, n_value_vec, c_random, n_random, min, max, random_range_negate,
                    canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
            } break;
            case Glitter::KEY_HERMITE: {
                const float_t c_frame = (float_t)c->frame;
                const float_t n_frame = (float_t)n->frame;
                const float_t c_tangent2 = c->tangent2;
                const float_t n_tangent1 = n->tangent1;

                const vec3 c_frame_vec = c_frame;
                const vec3 n_frame_vec = n_frame;
                const vec3 c_value_vec = { c_value + c_random_range, c_value, c_value - c_random_range };
                const vec3 n_value_vec = { n_value + n_random_range, n_value, n_value - n_random_range };
                const vec3 c_tangent2_vec = c_tangent2;
                const vec3 n_tangent1_vec = n_tangent1;
                const size_t frame_width_int = (size_t)roundf(frame_width);
                for (size_t j = c->frame; j < n->frame; j++) {
                    if (j < start_time)
                        continue;
                    else if (j >= end_time)
                        break;

                    for (size_t k = 0; k < frame_width_int; k++) {
                        const float_t frame = (float_t)(int32_t)j + (float_t)(int32_t)k / frame_width;
                        const vec3 frame_vec = frame;

                        const vec3 value = interpolate_chs_value(c_value_vec, n_value_vec,
                            c_tangent2_vec, n_tangent1_vec, c_frame_vec, n_frame_vec, frame_vec);
                        const vec3 random = !random_range_mult
                            ? random_range : glt_type_ft
                            ? (random_range * value) : (random_range * value * 0.01f);

                        const float_t x1 = canvas_pos.x
                            + (float_t)(int32_t)(j - base_start_time) * frame_width + (float_t)(int32_t)k;
                        const float_t x2 = canvas_pos.x
                            + (float_t)(int32_t)(j - base_start_time) * frame_width + (float_t)(int32_t)(k + 1);
                        glitter_editor_curve_editor_window_draw_random_range(crv_edt, x1, x2,
                            value, random, min, max, random_range_negate,
                            canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max);
                    }
                }
            } break;
            }
        }
    }

    bool first_frame_found = false;

    const Glitter::Curve::Key* i_begin = keys->data();
    const Glitter::Curve::Key* i_end = keys->data() + keys->size();
    for (const Glitter::Curve::Key* i = i_begin; i != i_end; i++) {
        if (i->frame <= start_time || i->frame > start_time && !first_frame_found) {
            if (i_end - i <= 1)
                break;
            else if (i[1].frame <= start_time)
                continue;

            const Glitter::Curve::Key* c = i;
            const Glitter::Curve::Key* n = i + 1;
            const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
            const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;

            float_t value;
            if (i->frame < start_time)
                switch (c->type) {
                case Glitter::KEY_CONSTANT:
                    value = c_value;
                    break;
                case Glitter::KEY_LINEAR:
                default:
                    value = interpolate_linear_value(c_value, n_value,
                        (float_t)c->frame, (float_t)n->frame, (float_t)start_time);
                    break;
                case Glitter::KEY_HERMITE:
                    value = interpolate_chs_value(c_value, n_value,
                        (float_t)c->tangent2, (float_t)n->tangent1,
                        (float_t)c->frame, (float_t)n->frame, (float_t)start_time);
                    break;
                }
            else
                value = c_value;

            const float_t x1 = loop
                ? canvas_pos.x + (float_t)(start_time - base_start_time) * frame_width
                : canvas_pos_min.x;
            const float_t x2 = canvas_pos.x + (float_t)(max_def(i->frame,
                start_time) - base_start_time) * frame_width;
            const float_t y = convert_value_to_height(crv_edt, value,
                canvas_pos.y, canvas_size.y, min, max);

            if ((x1 > canvas_pos_min.x || x2 > canvas_pos_min.x)
                && (x1 < canvas_pos_max.x || x2 < canvas_pos_max.x)) {
                if (!points.size() || points.back().x < x1 || points.back().y != y)
                    points.push_back({ x1, y });
                if (points.back().x < x2 || points.back().y != y)
                    points.push_back({ x2, y });
            }
            first_frame_found = true;
        }
        else if (i->frame > end_time || i->frame == end_time || i_end - i <= 1 && i->frame <= end_time) {
            float_t value;
            if (i->frame > end_time) {
                const Glitter::Curve::Key* c = i - 1;
                const Glitter::Curve::Key* n = i;
                const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
                const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;

                switch (c->type) {
                case Glitter::KEY_CONSTANT:
                    value = c_value;
                    break;
                case Glitter::KEY_LINEAR:
                default:
                    value = interpolate_linear_value(c_value, n_value,
                        (float_t)c->frame, (float_t)n->frame, (float_t)end_time);
                    break;
                case Glitter::KEY_HERMITE:
                    value = interpolate_chs_value(c_value, n_value,
                        (float_t)c->tangent2, (float_t)n->tangent1,
                        (float_t)c->frame, (float_t)n->frame, (float_t)end_time);
                    break;
                }
            }
            else
                value = i->value;

            const float_t x1 = canvas_pos.x + (float_t)(min_def(i->frame,
                end_time) - base_start_time) * frame_width;
            const float_t x2 = loop
                ? canvas_pos.x + (float_t)(end_time - base_start_time) * frame_width
                : canvas_pos_max.x;
            const float_t y = convert_value_to_height(crv_edt, value,
                canvas_pos.y, canvas_size.y, min, max);
            if ((x1 > canvas_pos_min.x || x2 > canvas_pos_min.x)
                && (x1 < canvas_pos_max.x || x2 < canvas_pos_max.x)) {
                if (!points.size() || points.back().x < x1 || points.back().y != y)
                    points.push_back({ x1, y });
                if (points.back().x < x2 || points.back().y != y)
                    points.push_back({ x2, y });
            }
            break;
        }
        else if (i_end - i <= 1)
            break;

        const Glitter::Curve::Key* c = i;
        const Glitter::Curve::Key* n = i + 1;
        const float_t c_value = fix_rot_z ? c->value - (float_t)M_PI_2 : c->value;
        const float_t n_value = fix_rot_z ? n->value - (float_t)M_PI_2 : n->value;

        if (c->type == Glitter::KEY_CONSTANT) {
            const int32_t c_frame = max_def(c->frame, start_time);
            const int32_t n_frame = min_def(n->frame,   end_time);
            const float_t x1 = canvas_pos.x + (float_t)(c_frame - base_start_time) * frame_width;
            const float_t x2 = canvas_pos.x + (float_t)(n_frame - base_start_time) * frame_width;

            const float_t y1 = convert_value_to_height(crv_edt, c_value,
                canvas_pos.y, canvas_size.y, min, max);
            const float_t y2 = convert_value_to_height(crv_edt, n_value,
                canvas_pos.y, canvas_size.y, min, max);

            if ((x1 > canvas_pos_min.x || x2 > canvas_pos_min.x)
                && (x1 < canvas_pos_max.x || x2 < canvas_pos_max.x)) {
                if (!points.size() || points.back().x < x1 || points.back().y != y1)
                    points.push_back({ x1, y1 });
                if (points.back().x < x2 || points.back().y != y1)
                    points.push_back({ x2, y1 });
                if (points.back().x < x2 || points.back().y != y2)
                    points.push_back({ x2, y2 });
            }
        }
        else if (c->type == Glitter::KEY_LINEAR) {
            const int32_t c_frame = max_def(c->frame, start_time);
            const int32_t n_frame = min_def(n->frame,   end_time);
            const float_t x1 = canvas_pos.x + (float_t)(c_frame - base_start_time) * frame_width;
            const float_t x2 = canvas_pos.x + (float_t)(n_frame - base_start_time) * frame_width;

            const float_t _c_value = interpolate_linear_value(c_value, n_value,
                (float_t)c->frame, (float_t)n->frame, (float_t)c_frame);
            const float_t _n_value = interpolate_linear_value(c_value, n_value,
                (float_t)c->frame, (float_t)n->frame, (float_t)n_frame);

            const float_t y1 = convert_value_to_height(crv_edt, _c_value,
                canvas_pos.y, canvas_size.y, min, max);
            const float_t y2 = convert_value_to_height(crv_edt, _n_value,
                canvas_pos.y, canvas_size.y, min, max);

            if ((x1 > canvas_pos_min.x || x2 > canvas_pos_min.x)
                && (x1 < canvas_pos_max.x || x2 < canvas_pos_max.x)) {
                if (!points.size() || points.back().x < x1 || points.back().y != y1)
                    points.push_back({ x1, y1 });
                if (points.back().x < x2 || points.back().y != y2)
                    points.push_back({ x2, y2 });
            }
        }
        else if (c->type == Glitter::KEY_HERMITE) {
            const float_t c_frame = (float_t)c->frame;
            const float_t n_frame = (float_t)n->frame;
            const float_t c_tangent2 = (float_t)c->tangent2;
            const float_t n_tangent1 = (float_t)n->tangent1;

            float_t value = interpolate_chs_value(c_value, n_value,
                c_tangent2, n_tangent1,
                c_frame, n_frame, (float_t)max_def(c->frame, start_time));
            float_t x = canvas_pos.x + (float_t)(max_def(c->frame, start_time)
                - base_start_time) * frame_width;
            float_t y = convert_value_to_height(crv_edt, value,
                canvas_pos.y, canvas_size.y, min, max);
            if (!points.size() || points.back().x < x || points.back().y != y)
                points.push_back({ x, y });

            const size_t frame_width_int = (size_t)roundf(frame_width);
            for (size_t j = c->frame; j < n->frame; j++) {
                if (j < start_time)
                    continue;
                else if (j >= end_time)
                    break;

                for (size_t k = 0; k < frame_width_int; k++) {
                    const float_t value = interpolate_chs_value(c_value, n_value, c_tangent2, n_tangent1,
                        c_frame, n_frame, (float_t)(int32_t)j + (float_t)(int32_t)k / frame_width);

                    float_t x = canvas_pos.x
                        + (float_t)(int32_t)(j - base_start_time) * frame_width + (float_t)(int32_t)k;
                    const float_t y = convert_value_to_height(crv_edt, value,
                        canvas_pos.y, canvas_size.y, min, max);

                    if ((x > canvas_pos_min.x || points.back().x < canvas_pos_max.x
                        && canvas_pos_max.x < x) && points.back().x < x)
                        points.push_back({ x, y });
                }
            }
        }
    }
}

static void glitter_editor_curve_editor_window_draw_random_range(
    GlitterEditor::CurveEditor* crv_edt, const float_t x1, const float_t x2,
    const vec3 value, const vec3 random, const float_t min, const float_t max, const bool random_range_negate,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max) {
    const ImU32     random_range_color = ImGui::GetColorU32({ 0.5f, 0.5f, 0.0f, 0.25f });
    const ImU32 key_random_range_color = ImGui::GetColorU32({ 0.5f, 0.0f, 0.5f, 0.25f });

    if ((x1 <= canvas_pos_min.x && x2 <= canvas_pos_min.x) || (x1 >= canvas_pos_max.x && x2 >= canvas_pos_max.x))
        return;

    ImDrawList* draw_list = crv_edt->draw_list;
    if (value.x < value.y || value.y > value.z) {
        float_t y1 = convert_value_to_height(crv_edt, value.x + random.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y2 = convert_value_to_height(crv_edt, value.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y3 = y2;
        float_t y4;
        float_t y5;
        float_t y6;
        if (!random_range_negate)
            y4 = convert_value_to_height(crv_edt, value.y,
                canvas_pos.y, canvas_size.y, min, max);
        else {
            y4 = convert_value_to_height(crv_edt, value.z,
                canvas_pos.y, canvas_size.y, min, max);
            y5 = y4;
            y6 = convert_value_to_height(crv_edt, value.z - random.z,
                canvas_pos.y, canvas_size.y, min, max);
        }

        if (y1 > y2) {
            float_t t = y2;
            y2 = y1;
            y1 = t;
        }

        if (y3 > y4) {
            float_t t = y4;
            y4 = y3;
            y3 = t;
        }

        if (random_range_negate && y5 > y6) {
            float_t t = y6;
            y6 = y5;
            y5 = t;
        }

        y1 = clamp_def(y1, canvas_pos_min.y, canvas_pos_max.y);
        y2 = clamp_def(y2, canvas_pos_min.y, canvas_pos_max.y);
        y3 = clamp_def(y3, canvas_pos_min.y, canvas_pos_max.y);
        y4 = clamp_def(y4, canvas_pos_min.y, canvas_pos_max.y);

        if (random_range_negate) {
            y5 = clamp_def(y5, canvas_pos_min.y, canvas_pos_max.y);
            y6 = clamp_def(y6, canvas_pos_min.y, canvas_pos_max.y);
        }

        if (y1 != y2)
            draw_list->AddRectFilled({ x1, y1 }, { x2, y2 }, random_range_color);

        if (y3 != y4)
            draw_list->AddRectFilled({ x1, y3 }, { x2, y4 }, key_random_range_color);

        if (random_range_negate && y5 != y6)
            draw_list->AddRectFilled({ x1, y5 }, { x2, y6 }, random_range_color);
    }
    else {
        float_t y1 = convert_value_to_height(crv_edt, value.y + random.y,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y2;
        if (!random_range_negate)
            y2 = convert_value_to_height(crv_edt, value.y,
                canvas_pos.y, canvas_size.y, min, max);
        else
            y2 = convert_value_to_height(crv_edt, value.y - random.y,
                canvas_pos.y, canvas_size.y, min, max);

        if (y1 > y2) {
            float_t t = y2;
            y2 = y1;
            y1 = t;
        }

        y1 = clamp_def(y1, canvas_pos_min.y, canvas_pos_max.y);
        y2 = clamp_def(y2, canvas_pos_min.y, canvas_pos_max.y);
        if (y1 != y2)
            draw_list->AddRectFilled({ x1, y1 }, { x2, y2 }, random_range_color);
    }
}

static void glitter_editor_curve_editor_window_draw_random_range(
    GlitterEditor::CurveEditor* crv_edt, const float_t x1, const float_t x2,
    const vec3 value1, const vec3 value2, const vec3 random1, const vec3 random2,
    const float_t min, const float_t max, const bool random_range_negate,
    const ImVec2 canvas_size, const ImVec2 canvas_pos, const ImVec2 canvas_pos_min, const ImVec2 canvas_pos_max) {
    const ImU32     random_range_color = ImGui::GetColorU32({ 0.5f, 0.5f, 0.0f, 0.25f });
    const ImU32 key_random_range_color = ImGui::GetColorU32({ 0.5f, 0.0f, 0.5f, 0.25f });

    if ((x1 <= canvas_pos_min.x && x2 <= canvas_pos_min.x) || (x1 >= canvas_pos_max.x && x2 >= canvas_pos_max.x))
        return;

    ImDrawList* draw_list = crv_edt->draw_list;
    if (value1.x < value1.y || value1.y != value1.z
        || value2.x < value2.y || value2.y != value2.z) {
        float_t y11 = convert_value_to_height(crv_edt, value1.x + random1.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y21 = convert_value_to_height(crv_edt, value2.x + random2.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y12 = convert_value_to_height(crv_edt, value1.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y22 = convert_value_to_height(crv_edt, value2.x,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y13 = y12;
        float_t y23 = y22;
        float_t y14;
        float_t y24;
        float_t y15;
        float_t y25;
        float_t y16;
        float_t y26;
        if (!random_range_negate) {
            y14 = convert_value_to_height(crv_edt, value1.y,
                canvas_pos.y, canvas_size.y, min, max);
            y24 = convert_value_to_height(crv_edt, value2.y,
                canvas_pos.y, canvas_size.y, min, max);
        }
        else {
            y14 = convert_value_to_height(crv_edt, value1.z,
                canvas_pos.y, canvas_size.y, min, max);
            y24 = convert_value_to_height(crv_edt, value2.z,
                canvas_pos.y, canvas_size.y, min, max);
            y15 = y14;
            y25 = y24;
            y16 = convert_value_to_height(crv_edt, value1.z - random1.z,
                canvas_pos.y, canvas_size.y, min, max);
            y26 = convert_value_to_height(crv_edt, value2.z - random2.z,
                canvas_pos.y, canvas_size.y, min, max);
        }

        if (y11 > y12) {
            float_t t = y12;
            y12 = y11;
            y11 = t;
        }

        if (y21 > y22) {
            float_t t = y22;
            y22 = y21;
            y21 = t;
        }

        if (y13 > y14) {
            float_t t = y14;
            y14 = y13;
            y13 = t;
        }

        if (y23 > y24) {
            float_t t = y24;
            y24 = y23;
            y23 = t;
        }

        if (random_range_negate) {
            if (y15 > y16) {
                float_t t = y16;
                y16 = y15;
                y15 = t;
            }

            if (y25 > y26) {
                float_t t = y26;
                y26 = y25;
                y25 = t;
            }
        }

        if (y11 != y12 || y21 != y22)
            draw_list->AddQuadFilled({ x1, y11 }, { x2, y21 }, { x2, y22 }, { x1, y12 }, random_range_color);

        if (y13 != y14 || y23 != y24)
            draw_list->AddQuadFilled({ x1, y13 }, { x2, y23 }, { x2, y24 }, { x1, y14 }, key_random_range_color);

        if (random_range_negate && (y15 != y16 || y25 != y26))
            draw_list->AddQuadFilled({ x1, y15 }, { x2, y25 }, { x2, y26 }, { x1, y16 }, random_range_color);
    }
    else {
        float_t y11 = convert_value_to_height(crv_edt, value1.y + random1.y,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y21 = convert_value_to_height(crv_edt, value2.y + random2.y,
            canvas_pos.y, canvas_size.y, min, max);
        float_t y12;
        float_t y22;
        if (!random_range_negate) {
            y12 = convert_value_to_height(crv_edt, value1.y,
                canvas_pos.y, canvas_size.y, min, max);
            y22 = convert_value_to_height(crv_edt, value2.y,
                canvas_pos.y, canvas_size.y, min, max);
        }
        else {
            y12 = convert_value_to_height(crv_edt, value1.y - random1.y,
                canvas_pos.y, canvas_size.y, min, max);
            y22 = convert_value_to_height(crv_edt, value2.y - random2.y,
                canvas_pos.y, canvas_size.y, min, max);
        }

        if (y11 > y12) {
            float_t t = y12;
            y12 = y11;
            y11 = t;
        }

        if (y21 > y22) {
            float_t t = y22;
            y22 = y21;
            y21 = t;
        }

        if (y11 != y12 || y21 != y22)
            draw_list->AddQuadFilled({ x1, y11 }, { x1, y21 }, { x2, y22 }, { x2, y12 }, random_range_color);
    }
}

static bool glitter_editor_hash_input(GlitterEditor* glt_edt, const char* label, uint64_t* hash) {
    uint64_t h = *hash;
    if (glt_edt->effect_group->type == Glitter::FT) {
        if (ImGui::ColumnInputScalar(label, ImGuiDataType_U64, &h,
            0, 0, "%016llX", ImGuiInputTextFlags_CharsHexadecimal) && !h)
            h = hash_fnv1a64m_empty;
    }
    else {
        if (ImGui::ColumnInputScalar(label, ImGuiDataType_U32, &h,
            0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal) && !h)
            h = hash_murmurhash_empty;
    }

    if (h == *hash)
        return false;

    *hash = h;
    return true;
}
#endif
