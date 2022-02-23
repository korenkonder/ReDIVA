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
#include "../../KKdLib/txp.h"
#include "../../KKdLib/str_utils.h"
#include "../../KKdLib/vec.h"
#include "../../CRE/Glitter/animation.h"
#include "../../CRE/Glitter/curve.h"
#include "../../CRE/Glitter/diva_effect.h"
#include "../../CRE/Glitter/diva_list.h"
#include "../../CRE/Glitter/diva_resource.h"
#include "../../CRE/Glitter/effect.h"
#include "../../CRE/Glitter/effect_group.h"
#include "../../CRE/Glitter/emitter.h"
#include "../../CRE/Glitter/file_reader.h"
#include "../../CRE/Glitter/glitter.h"
#include "../../CRE/Glitter/particle.h"
#include "../../CRE/Glitter/particle_manager.h"
#include "../../CRE/Glitter/scene.h"
#include "../../CRE/Glitter/texture.h"
#include "../../CRE/camera.h"
#include "../../CRE/data.h"
#include "../../CRE/draw_task.h"
#include "../../CRE/gl_state.h"
#include "../../CRE/render_context.h"
#include "../../CRE/shader_glsl.h"
#include "../../CRE/static_var.h"
#include "../input.h"
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include "imgui_helper.h"
#include <glad/glad.h>

typedef enum glitter_editor_selected_enum {
    GLITTER_EDITOR_SELECTED_NONE = 0,
    GLITTER_EDITOR_SELECTED_EFFECT,
    GLITTER_EDITOR_SELECTED_EMITTER,
    GLITTER_EDITOR_SELECTED_PARTICLE,
} glitter_editor_selected_enum;

typedef enum glitter_editor_flags {
    GLITTER_EDITOR_ADD       = 0x01,
    GLITTER_EDITOR_DUPLICATE = 0x02,
    GLITTER_EDITOR_DELETE    = 0x04,
    GLITTER_EDITOR_MOVE_UP   = 0x08,
    GLITTER_EDITOR_MOVE_DOWN = 0x10,
    GLITTER_EDITOR_IMPORT    = 0x20,
    GLITTER_EDITOR_EXPORT    = 0x40,
} glitter_editor_flags;

typedef enum glitter_editor_draw_flags {
    GLITTER_EDITOR_DRAW_WIREFRAME = 0x01,
    GLITTER_EDITOR_DRAW_SELECTED  = 0x02,
    GLITTER_EDITOR_DRAW_NO_DRAW   = 0x04,
} glitter_editor_draw_flags;

typedef struct glitter_curve_editor {
    glitter_curve_type type;
    glitter_animation* animation;
    glitter_curve* list[GLITTER_CURVE_V_SCROLL_ALPHA_2ND - GLITTER_CURVE_TRANSLATION_X + 1];
    glitter_curve curve;
    glitter_curve_key* key;

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
} glitter_curve_editor;

typedef struct glitter_editor_gl_wireframe {
    shader_glsl shader;
} glitter_editor_gl_wireframe;

typedef struct glitter_editor_gl {
    glitter_editor_gl_wireframe wireframe;
} glitter_editor_gl;

typedef struct glitter_editor_struct {
    bool test;
    bool create_popup;
    bool load;
    bool load_wait;
    bool load_data;
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
    glitter_type load_glt_type;
    glitter_type save_glt_type;
    data_type load_data_type;
    float_t frame_counter;
    float_t old_frame_counter;
    int32_t start_frame;
    int32_t end_frame;
    glitter_random random;
    int32_t counter;
    glitter_effect_group* effect_group;
    glitter_scene* scene;
    uint64_t hash;
    glitter_scene_counter scene_counter;
    char file[MAX_PATH * 2];
    GLuint vao;

    glitter_editor_selected_enum selected_type;
    int32_t selected_resource;
    glitter_effect* selected_effect;
    glitter_emitter* selected_emitter;
    glitter_particle* selected_particle;
    glitter_curve_editor curve_editor;

    glitter_editor_gl gl_data;
} glitter_editor_struct;

static const char* glitter_emitter_direction_name[] = {
    "Billboard",
    "Billboard (Y Axis)",
    "X Axis",
    "Y Axis",
    "Z Axis",
    "Effect Rotation",
};

static const char* glitter_particle_draw_type_name[] = {
    "Billboard",
    "Billboard (Y Axis)",
    "X Axis",
    "Y Axis",
    "Z Axis",
    "Emitter Direction",
    "Emitter Rotation",
    "Particle Rotation",
    "Prev Position",
    "Prev Position Dup",
    "Emit Position",
};

static const char* glitter_effect_type_name[] = {
    "Normal",
    "Local",
    "Chara Parent",
    "Object Parent",
};

static const char* glitter_effect_ext_anim_index_name[] = {
    "P1",
    "P2",
    "P3",
    "P4",
    "P5",
    "P6",
};

static const char* glitter_effect_ext_anim_node_index_name[] = {
    "None",
    "Head",
    "Mouth",
    "Belly",
    "Chest",
    "Left Shoulder",
    "Left Elbow",
    "Left Elbow Dup",
    "Left Hand",
    "Right Shoulder",
    "Right Elbow",
    "Right Elbow Dup",
    "Right Hand",
    "Left Thigh",
    "Left Knee",
    "Left Toe",
    "Right Thigh",
    "Right Knee",
    "Right Toe",
};

static const char* glitter_emitter_name[] = {
    "Box",
    "Cylinder",
    "Sphere",
    "Mesh",
    "Polygon",
};

static const char* glitter_emitter_emission_direction_name[] = {
    "None",
    "Outward",
    "Inward",
};

static const char* glitter_emitter_emission_name[] = {
    "On Timer",
    "On Start",
    "On End",
};

static const char* glitter_emitter_timer_name[] = {
    "By Time",
    "By Distance",
};

static const char* glitter_key_name[] = {
    "Constant",
    "Linear",
    "Hermite",
};

static const char* glitter_particle_blend_name[] = {
    "Zero",
    "Typical",
    "Add",
    "Subtract",
    "Multiply",
    "Punch Through",
};

static const char* glitter_particle_blend_draw_name[] = {
    "Typical",
    "Add",
    "Multiply",
};

static const char* glitter_particle_blend_mask_name[] = {
    "Typical",
    "Add",
    "Multiply",
};

static const char* glitter_particle_name[] = {
    "Quad",
    "Line",
    "Locus",
    "Mesh",
};

static const char* glitter_pivot_name[] = {
    "Top Left",
    "Top Center",
    "Top Right",
    "Middle Left",
    "Middle Center",
    "Middle Right",
    "Bottom Left",
    "Bottom Center",
    "Bottom Right",
};

static const char* glitter_uv_index_type_name[] = {
    "Fixed",
    "Initial Random / Fixed",
    "Random",
    "Forward",
    "Reverse",
    "Initial Random / Forward",
    "Initial Random / Reverse",
    "User",
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
static void glitter_editor_load_file(glitter_editor_struct* glt_edt, char* path, char* file);
static void glitter_editor_save_file(glitter_editor_struct* glt_edt, char* path, char* file);
static bool glitter_editor_list_open_window(glitter_effect_group* eg);
static bool glitter_editor_resource_import(glitter_editor_struct* glt_edt);
static bool glitter_editor_resource_export(glitter_editor_struct* glt_edt);

static void glitter_editor_test_window(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_effects(glitter_editor_struct* glt_edt);
static void glitter_editor_effects_context_menu(glitter_editor_struct* glt_edt,
    glitter_effect* effect, glitter_emitter* emitter, glitter_particle* particle,
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
    class_data* data, const char* label, char** items, glitter_particle* particle,
    int32_t* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
    int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex);

static void glitter_editor_popups(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_file_create_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_model_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_error_list_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_save_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size);

static void glitter_editor_curve_editor(glitter_editor_struct* glt_edt);
static void glitter_editor_curve_editor_curve_reset(glitter_editor_struct* glt_edt,
    glitter_curve* curve);
static void glitter_editor_curve_editor_curve_set(glitter_editor_struct* glt_edt,
    glitter_curve* curve, glitter_curve_type type);
static void glitter_editor_curve_editor_curves_reset(glitter_editor_struct* glt_edt);
static glitter_curve_key* glitter_editor_curve_editor_get_closest_key(glitter_editor_struct* glt_edt,
    glitter_curve* curve);
static glitter_curve_key* glitter_editor_curve_editor_get_selected_key(glitter_editor_struct* glt_edt,
    glitter_curve* curve);
static float_t glitter_editor_curve_editor_get_value(glitter_editor_struct* glt_edt,
    glitter_curve_type type);
static void glitter_editor_curve_editor_key_manager(glitter_editor_struct* glt_edt,
    vector_old_glitter_curve_key* keys, bool* add_key, bool* del_key);
static void glitter_editor_curve_editor_property_window(glitter_editor_struct* glt_edt, class_data* data);
static void glitter_editor_curve_editor_reset_state(glitter_editor_struct* glt_edt,
    glitter_curve_type type);
static void glitter_editor_curve_editor_selector(glitter_editor_struct* glt_edt, class_data* data);

static void glitter_editor_gl_load(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_draw(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_process(glitter_editor_struct* glt_edt);
static void glitter_editor_gl_free(glitter_editor_struct* glt_edt);

static void glitter_editor_gl_draw_wireframe(glitter_editor_struct* glt_edt);
static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    glitter_effect_inst* eff, glitter_particle_inst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    glitter_effect_inst* eff, glitter_particle_inst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw_mesh(glitter_editor_struct* glt_edt,
    glitter_render_group* rg);

static void glitter_editor_gl_select_particle(glitter_editor_struct* glt_edt);

static bool glitter_editor_hash_input(glitter_editor_struct* glt_edt,
    const char* label, uint64_t* hash);

bool glitter_editor_init(class_data* data, render_context* rctx) {
    if (!lock_data_init(&glitter_data_lock, &data->lock, data, (void(*)(void*))glitter_editor_dispose))
        return false;

    bool ret = false;
    lock_trylock(&pv_lock);
    data->data = force_malloc_s(glitter_editor_struct, 1);

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    GPM_VAL.random.value = (uint32_t)(time.LowPart * hash_fnv1a64m_empty);
    GPM_VAL.random.step = 1;

    QueryPerformanceCounter(&time);
    GPM_VAL.counter = (uint32_t)(time.LowPart * hash_murmurhash_empty);

    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (glt_edt) {
        glt_edt->test = false;
        glt_edt->draw_flags = (glitter_editor_draw_flags)0;
        GPM_VAL.emission = 1.0f;
        GPM_VAL.draw_all = true;
        GPM_VAL.draw_all_mesh = true;
        glGenVertexArrays(1, &glt_edt->vao);
        draw_grid_3d = true;

        glitter_editor_reset(glt_edt);
        glitter_editor_gl_load(glt_edt);
    }
    ret = true;
    lock_unlock(&pv_lock);

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
    glitter_file_reader* fr = 0;
    if (files_x.begin) {
        stream s;
        io_wopen(&s, L"name_VRFL.glitter.txt", L"rb");
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
                hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F), 0, false, false);
            }

            for (ssize_t i = 0; i < c; i++) {
                char* file_x = str_utils_get_without_extension(string_data(&files_x.begin[i]));

                char buf[0x100];
                sprintf_s(buf, 0x100, "%hs\n", file_x);
                OutputDebugStringA(buf);

                glt_edt->load_glt_type = GLITTER_X;
                fr = glitter_file_reader_init(GLITTER_X, path_x, file_x, 1.0f);
                glitter_file_reader_read(fr, GPM_VAL.emission);

                if (fr && fr->effect_group) {
                    glitter_effect_group* eg = fr->effect_group;
                    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                        if (!*i)
                            continue;

                        glitter_effect* e = *i;
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

                    glitter_effect_group_dispose(fr->effect_group);
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
    glitter_file_reader* fr = 0;
    if (files_x.begin)
        for (ssize_t i = 0; i < c; i++) {
            char* file_x = str_utils_get_without_extension(string_data(&files_x.begin[i]));

            char buf[0x100];
            sprintf_s(buf, 0x100, "%hs\n", file_x);
            OutputDebugStringA(buf);

            glt_edt->load_glt_type = GLITTER_X;
            fr = glitter_file_reader_init(GLITTER_X, path_x, file_x, 1.0f);
            glitter_file_reader_read(fr, GPM_VAL.emission);

            if (fr && fr->effect_group) {
                glitter_effect_group* eg = fr->effect_group;
                for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    glitter_effect* e = *i;
                    if (!e->data.ext_anim)
                        continue;

                    glitter_effect_ext_anim* ea = e->data.ext_anim;
                    if (ea->instance_id != 0) {
                        char buf[0x100];
                        sprintf_s(buf, 0x100, "%08llX %d\n", e->data.name_hash, ea->instance_id);
                        OutputDebugStringA(buf);
                    }
                }

                glitter_effect_group_dispose(fr->effect_group);
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
    io_wopen(&s, L"name_F2.glitter.txt", L"rb");
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
            hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F), 0, false, false);
        }

        ssize_t c = vector_old_length(files_f2);
        glitter_file_reader* fr = 0;
        if (files_f2.begin && files_ft.begin)
            for (ssize_t i = 0; i < c; i++) {
                char* file_f2 = str_utils_get_without_extension(string_data(&files_f2.begin[i]));
                char* file_ft = str_utils_get_without_extension(string_data(&files_ft.begin[i]));
                glt_edt->load_glt_type = GLITTER_F2;
                fr = glitter_file_reader_init(GLITTER_F2, path_f2, file_f2, 1.0f);
                glitter_file_reader_read(fr, GPM_VAL.emission);

                if (fr && fr->effect_group) {
                    glitter_effect_group* eg = fr->effect_group;
                    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                        if (!*i)
                            continue;

                        glitter_effect* e = *i;
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
                    //glt_edt->save_glt_type = GLITTER_FT;
                    //glitter_editor_save_file(path_ft, file_ft);
                    glitter_effect_group_dispose(fr->effect_group);
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
    return ret;
}

void glitter_editor_ctrl(class_data* data) {
    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    int32_t sel_rsrc = glt_edt->selected_resource;
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_emitter* sel_emit = glt_edt->selected_emitter;
    glitter_particle* sel_ptcl = glt_edt->selected_particle;

    if (glt_edt->effect_group_add) {
        GPM_VAL.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        glitter_effect_group* eg = new glitter_effect_group(glt_edt->load_glt_type);
        char buf[0x100];
        snprintf(buf, sizeof(buf), "eff_%016llx_main", time.LowPart * hash_fnv1a64m_empty);
        uint64_t hash = glt_edt->load_glt_type == GLITTER_FT
            ? hash_utf8_fnv1a64m(buf, false) : hash_utf8_murmurhash(buf, 0, false);
        eg->hash = hash;
        eg->load_count = 1;
        GPM_VAL.effect_groups.insert({ hash, eg });

        glt_edt->scene_counter = GPM_VAL.LoadScene(hash, eg->type != GLITTER_FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty);
        glt_edt->effect_group = eg;
        glt_edt->hash = hash;
    }

    bool has_resource = sel_rsrc != -1;
    bool has_effect = glt_edt->selected_effect != 0;
    bool has_emitter = glt_edt->selected_emitter != 0 && has_effect;
    bool has_particle = glt_edt->selected_particle != 0 && has_emitter;
    glitter_effect_group* eg = glt_edt->effect_group;

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
                vector_old_txp_erase(&eg->resources_tex, j, txp_free);
                eg->resources_count--;

                uint64_t empty_hash = eg->type != GLITTER_FT
                    ? hash_murmurhash_empty : hash_fnv1a64m_empty;
                for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    glitter_effect* effect = *i;
                    for (glitter_emitter** j = effect->emitters.begin;
                        j != effect->emitters.end; j++) {
                        if (!*j)
                            continue;

                        glitter_emitter* emitter = *j;
                        for (glitter_particle** k = emitter->particles.begin;
                            k != emitter->particles.end; k++) {
                            if (!*k)
                                continue;

                            glitter_particle* particle = *k;
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
            txp* rt = eg->resources_tex.begin;

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
            txp* rt = eg->resources_tex.begin;

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
        glitter_texture_load(&GPM_VAL, glt_edt->effect_group);
        glt_edt->input_reload = true;
    }

    if (eg && glt_edt->effect_flags & GLITTER_EDITOR_ADD) {
        ssize_t eff_count = vector_old_length(eg->effects);

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        glitter_effect* e = glitter_effect_init(eg->type);
        snprintf(e->name, sizeof(e->name), "eff_%08x",
            (uint32_t)((eff_count + 1) * time.LowPart * hash_murmurhash_empty));
        vector_old_ptr_glitter_effect_push_back(&eg->effects, &e);
        glt_edt->input_reload = true;
    }

    if (eg && has_effect)
        if (glt_edt->effect_flags & GLITTER_EDITOR_DUPLICATE) {
            ssize_t eff_count = vector_old_length(eg->effects);

            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            glitter_effect* e = glitter_effect_copy(glt_edt->selected_effect);
            snprintf(e->name, sizeof(e->name), "eff_%08x",
                (uint32_t)((eff_count + 1) * time.LowPart * hash_murmurhash_empty));
            vector_old_ptr_glitter_effect_push_back(&eg->effects, &e);
            glt_edt->input_reload = true;
        }
        else if ((glt_edt->effect_flags & GLITTER_EDITOR_DELETE)) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++)
                if (*i && *i == glt_edt->selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > -1)
                vector_old_ptr_glitter_effect_erase(&eg->effects, j, glitter_effect_dispose);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_NONE;
            glt_edt->selected_resource = -1;
            glt_edt->selected_effect = 0;
            glt_edt->selected_emitter = 0;
            glt_edt->selected_particle = 0;

            if (vector_old_length(eg->effects) < 1)
                glt_edt->close = true;
            else
                glt_edt->input_reload = true;
        }
        else if (glt_edt->effect_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin;
                i != eg->effects.end; i++)
                if (*i == glt_edt->selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > 0) {
                glitter_effect* temp = eg->effects.begin[j - 1];
                eg->effects.begin[j - 1] = eg->effects.begin[j];
                eg->effects.begin[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->effect_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin;
                i != eg->effects.end; i++)
                if (*i == glt_edt->selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > -1 && j < vector_old_length(eg->effects) - 1) {
                glitter_effect* temp = eg->effects.begin[j + 1];
                eg->effects.begin[j + 1] = eg->effects.begin[j];
                eg->effects.begin[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_ADD) {
            glitter_emitter* e = glitter_emitter_init(eg->type);
            vector_old_ptr_glitter_emitter_push_back(&sel_efct->emitters, &e);
            glt_edt->input_reload = true;
        }

    if (eg && has_emitter)
        if (glt_edt->emitter_flags & GLITTER_EDITOR_DUPLICATE) {
            glitter_emitter* e = glitter_emitter_copy(glt_edt->selected_emitter);
            vector_old_ptr_glitter_emitter_push_back(&sel_efct->emitters, &e);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = -1;
            for (glitter_emitter** i = sel_efct->emitters.begin;
                i != sel_efct->emitters.end; i++)
                if (*i && *i == glt_edt->selected_emitter) {
                    j = i - sel_efct->emitters.begin;
                    break;
                }

            if (j > -1)
                vector_old_ptr_glitter_emitter_erase(&sel_efct->emitters,
                    j, glitter_emitter_dispose);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_EFFECT;
            glt_edt->selected_emitter = 0;
            glt_edt->selected_particle = 0;
            glt_edt->input_reload = true;
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_emitter** i = sel_efct->emitters.begin;
                i != sel_efct->emitters.end; i++)
                if (*i == glt_edt->selected_emitter) {
                    j = i - sel_efct->emitters.begin;
                    break;
                }

            if (j > 0) {
                glitter_emitter* temp = sel_efct->emitters.begin[j - 1];
                sel_efct->emitters.begin[j - 1]
                    = sel_efct->emitters.begin[j];
                sel_efct->emitters.begin[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->emitter_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_emitter** i = sel_efct->emitters.begin;
                i != sel_efct->emitters.end; i++)
                if (*i == glt_edt->selected_emitter) {
                    j = i - sel_efct->emitters.begin;
                    break;
                }

            if (j > -1 && j < sel_efct->emitters.end
                - sel_efct->emitters.begin - 1) {
                glitter_emitter* temp = sel_efct->emitters.begin[j + 1];
                sel_efct->emitters.begin[j + 1]
                    = sel_efct->emitters.begin[j];
                sel_efct->emitters.begin[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_ADD) {
            glitter_particle* p = glitter_particle_init(eg->type);
            vector_old_ptr_glitter_particle_push_back(&sel_emit->particles, &p);
            glt_edt->input_reload = true;
        }

    if (eg && has_particle)
        if (glt_edt->particle_flags & GLITTER_EDITOR_DUPLICATE) {
            glitter_particle* p = glitter_particle_copy(glt_edt->selected_particle);
            vector_old_ptr_glitter_particle_push_back(&sel_emit->particles, &p);
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = (ssize_t)-1;
            for (glitter_particle** i = sel_emit->particles.begin;
                i != sel_emit->particles.end; i++)
                if (*i && *i == glt_edt->selected_particle) {
                    j = i - sel_emit->particles.begin;
                    break;
                }

            if (j > -1)
                vector_old_ptr_glitter_particle_erase(&sel_emit->particles,
                    j, glitter_particle_dispose);
            glt_edt->selected_type = GLITTER_EDITOR_SELECTED_EMITTER;
            glt_edt->selected_particle = 0;
            glt_edt->input_reload = true;
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_particle** i = sel_emit->particles.begin;
                i != sel_emit->particles.end; i++)
                if (*i == glt_edt->selected_particle) {
                    j = i - sel_emit->particles.begin;
                    break;
                }

            if (j > 0) {
                glitter_particle* temp = sel_emit->particles.begin[j - 1];
                sel_emit->particles.begin[j - 1]
                    = sel_emit->particles.begin[j];
                sel_emit->particles.begin[j] = temp;
                glt_edt->input_reload = true;
            }
        }
        else if (glt_edt->particle_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_particle** i = sel_emit->particles.begin;
                i != sel_emit->particles.end; i++)
                if (*i == glt_edt->selected_particle) {
                    j = i - sel_emit->particles.begin;
                    break;
                }

            if (j > -1 && j < sel_emit->particles.end
                - sel_emit->particles.begin - 1) {
                glitter_particle* temp = sel_emit->particles.begin[j + 1];
                sel_emit->particles.begin[j + 1]
                    = sel_emit->particles.begin[j];
                sel_emit->particles.begin[j] = temp;
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
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                if (!*i)
                    continue;

                glitter_effect* e = *i;
                memset(e->name, 0, 0x80);
                if (e->data.name_hash == hash_murmurhash_empty)
                    continue;

                uint32_t* hash = 0;
                for (uint32_t j : hashes)
                    if (e->data.name_hash == j) {
                        hash = &j;
                        break;
                    }

                if (hash == hashes.end()._Ptr) {
                    load_success = false;
                    continue;
                }

                std::string* s = &ds->glitter_list_names[hash - hashes.data()];
                size_t len = s->size();
                memcpy(e->name, s->c_str(), min(len, 0x7F));
            }

            switch (ds->type) {
            case DATA_VRFL:
            case DATA_X:
            case DATA_XHD:
                if (glitter_effect_group_check_model(eg))
                    glitter_effect_group_load_model(eg, ds);
                break;
            }
        } break;
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39: {
            std::vector<uint64_t>& hashes = ds->glitter_list_fnv1a64m;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                if (!*i)
                    continue;

                glitter_effect* e = *i;
                memset(e->name, 0, 0x80);
                if (e->data.name_hash == hash_fnv1a64m_empty)
                    continue;

                uint64_t* hash = 0;
                for (uint64_t j : hashes)
                    if (e->data.name_hash == j)
                        break;

                if (hash == hashes.end()._Ptr) {
                    load_success = false;
                    continue;
                }

                std::string* s = &ds->glitter_list_names[hash - hashes.data()];
                size_t len = s->size();
                memcpy(e->name, s->c_str(), min(len, 0x7F));
            }
        } break;
        }

        glt_edt->load_data = false;

        if (!load_success && !glitter_editor_list_open_window(glt_edt->effect_group)) {
            glt_edt->load_error_list_popup = true;
            goto effect_unload;
        }
        return;
    }
    else if (glt_edt->close) {
    effect_unload:
        GPM_VAL.FreeScenes();
        GPM_VAL.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();
        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->hash = 0;
        glt_edt->scene_counter = 0;
        memset(glt_edt->file, 0, sizeof(glt_edt->file));
        glt_edt->close = false;
        return;
    }

    GPM_VAL.GetStartEndFrame(&glt_edt->start_frame, &glt_edt->end_frame, glt_edt->hash);

    if (!glt_edt->effect_group) {
        glt_edt->input_play = false;
        glt_edt->input_reset = false;
        return;
    }

    if (!(glt_edt->input_pause || glt_edt->input_pause_temp) && GPM_VAL.scenes.size())
        glt_edt->frame_counter += get_delta_frame();

    if (glt_edt->input_reload) {
        GPM_VAL.FreeScenes();
        glt_edt->effect_group->emission = GPM_VAL.emission;
        GPM_VAL.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
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

        GPM_VAL.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, false);
        glt_edt->scene_counter = glt_edt->scene->counter;
        glt_edt->old_frame_counter = glt_edt->frame_counter;
    }

    if (glt_edt->input_play)
        glt_edt->input_pause = false;
    else if (glt_edt->input_reset) {
        GPM_VAL.counter = glt_edt->counter;
        glt_edt->frame_counter = (float_t)glt_edt->start_frame;
        GPM_VAL.SetFrame(glt_edt->effect_group, &glt_edt->scene, glt_edt->frame_counter,
            glt_edt->old_frame_counter, glt_edt->counter, &glt_edt->random, true);
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
    if (igGetIO()->WantTextInput)
        return;

    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (!glt_edt)
        return;

    if (input_is_down(VK_CONTROL) && input_is_tapped('O'))
        glitter_editor_open_window(glt_edt);
    else if (input_is_down(VK_CONTROL) && input_is_tapped(VK_F4))
        glt_edt->close = true;
    else if (input_is_down(VK_CONTROL) && input_is_tapped(VK_F3))
        enum_or(data->flags, CLASS_DISPOSE);
    else if (input_is_down(VK_CONTROL) && input_is_tapped('Q'))
        close = true;
    else if (input_is_tapped('P'))
        glt_edt->test ^= true;
    else if (input_is_tapped('T'))
        glt_edt->input_play = true;
    else if (input_is_tapped('Y'))
        glt_edt->input_reload = true;
    else if (input_is_tapped('F'))
        glt_edt->input_pause = true;
    else if (input_is_tapped('G'))
        glt_edt->input_reset = true;

    if (data->imgui_focus) {
        if (input_is_down(VK_CONTROL) && input_is_down(VK_SHIFT) && input_is_tapped('S'))
            glitter_editor_save_as_window(glt_edt);
        else if (input_is_down(VK_CONTROL) && input_is_tapped('S'))
            glitter_editor_save_window(glt_edt);
    }
}

bool glitter_editor_dispose(class_data* data) {
    GPM_VAL.FreeScenes();

    lock_data_free(&glitter_data_lock, (void(*)(void*))glitter_editor_dispose);

    glitter_editor_struct* glt_edt = (glitter_editor_struct*)data->data;
    if (glt_edt) {
        glitter_editor_gl_free(glt_edt);
        GPM_VAL.UnloadEffectGroup(glt_edt->hash);

        glt_edt->test = false;
        glt_edt->create_popup = false;
        glt_edt->load = false;
        glt_edt->load_data = false;
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
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    const float_t title_bar_size = font->FontSize
        + style->ItemSpacing.y + style->FramePadding.y * 2.0f;

    ImVec2 main_menu_bar_size = ImVec2_Empty;

    if (igBeginMainMenuBar()) {
        bool is_x = glt_edt->effect_group
            && glt_edt->effect_group->type == GLITTER_X ? true : false;
        if (igBeginMenu("File", true)) {
            if (igMenuItem_Bool("Open", "Ctrl+O", false, true))
                glitter_editor_open_window(glt_edt);
            if (igMenuItem_Bool("Save", "Ctrl+S", false, !is_x))
                glitter_editor_save_window(glt_edt);
            if (igMenuItem_Bool("Save As..", "Ctrl+Shift+S", false, !is_x))
                glitter_editor_save_as_window(glt_edt);
            if (igMenuItem_Bool("Close", "Ctrl+F4", false, true))
                glt_edt->close = true;
            if (igMenuItem_Bool("Close Editor", "Ctrl+F3", false, true))
                enum_or(data->flags, CLASS_DISPOSE);
            if (igMenuItem_Bool("Quit", "Ctrl+Q", false, true))
                close = true;
            igEndMenu();
        }

        if (igBeginMenu("Edit", false)) {
            igEndMenu();
        }

        if (igBeginMenu("Mode", glt_edt->effect_group ? true : false)) {
            glitter_effect_group* eg = glt_edt->effect_group;
            if (eg->type != GLITTER_X) {
                if (igMenuItem_Bool("F2", 0, false, eg->type != GLITTER_F2)) {
                    eg->type = GLITTER_F2;
                    glitter_editor_reload(glt_edt);
                }

                if (igMenuItem_Bool("FT", 0, false, eg->type != GLITTER_FT)) {
                    eg->type = GLITTER_FT;
                    glitter_editor_reload(glt_edt);
                }
            }
            else {
                if (igMenuItem_Bool("X", 0, false, eg->type != GLITTER_X)) {
                    eg->type = GLITTER_X;
                    glitter_editor_reload(glt_edt);
                }
            }
            igEndMenu();
        }

        if (igBeginMenu("View", true)) {
            igCheckboxFlags_UintPtr("Draw Wireframe",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
            igCheckboxFlags_UintPtr("Draw Selected",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
            igCheckboxFlags_UintPtr("No Draw",
                (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_NO_DRAW);
            igEndMenu();
        }
        data->imgui_focus |= igIsWindowFocused(0);
        igGetWindowSize(&main_menu_bar_size);
        igEndMainMenuBar();
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

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Effects Window");
    if (igBegin("Effects", 0, window_flags)) {
        if (igBeginTabBar("Effects Tab", 0)) {
            if (igBeginTabItem("Effects", 0, 0)) {
                if (glt_edt->effect_group)
                    glitter_editor_effects(glt_edt);
                igEndTabItem();
            }

            imguiDisableElementPush(glt_edt->effect_group != 0);
            if (igBeginTabItem("Resources", 0, 0)) {
                glitter_editor_resources_context_menu(glt_edt, -1, 0, true);
                if (glt_edt->effect_group)
                    glitter_editor_resources(glt_edt);
                igEndTabItem();
            }
            imguiDisableElementPop(glt_edt->effect_group != 0);

            if (igBeginTabItem("Play Manager", 0, 0)) {
                glitter_editor_play_manager(glt_edt);
                igEndTabItem();
            }
            igEndTabBar();
        }
        data->imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();

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

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Property Window");
    if (igBegin("Property", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_property(glt_edt, data);
        data->imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();

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

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Selector Window");
    if (igBegin("Curve Editor Selector", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor_selector(glt_edt, data);
        data->imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();

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

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Property Window");
    if (igBegin("Curve Editor Property", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor_property_window(glt_edt, data);
        data->imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();

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

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Window");
    if (igBegin("Curve Editor", 0, window_flags)) {
        if (glt_edt->effect_group && selected)
            glitter_editor_curve_editor(glt_edt);
        data->imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();
}

static void glitter_editor_reload(glitter_editor_struct* glt_edt) {
    if (!glt_edt->effect_group)
        return;

    glt_edt->input_reload = true;
    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_scene* sc = glt_edt->scene;
    sc->type = eg->type;
    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
        if (!*i)
            continue;

        glitter_effect* effect = *i;
        size_t len = utf8_length(effect->name);
        if (eg->type != GLITTER_FT)
            effect->data.name_hash = hash_murmurhash(effect->name,
                min(len, 0x7F), 0, false, false);
        else
            effect->data.name_hash = hash_fnv1a64m(effect->name,
                min(len, 0x7F), false);

        for (glitter_curve** c = effect->animation.begin; c != effect->animation.end; c++)
            if (*c)
                glitter_curve_recalculate(eg->type, *c);

        for (glitter_emitter** j = effect->emitters.begin;
            j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            glitter_emitter* emitter = *j;
            for (glitter_curve** c = emitter->animation.begin; c != emitter->animation.end; c++)
                if (*c)
                    glitter_curve_recalculate(eg->type, *c);

            for (glitter_particle** k = emitter->particles.begin;
                k != emitter->particles.end; k++) {
                if (!*k)
                    continue;

                glitter_particle* particle = *k;
                for (glitter_curve** c = particle->animation.begin; c != particle->animation.end; c++)
                    if (*c)
                        glitter_curve_recalculate(eg->type, *c);
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
    glitter_editor_curve_editor_reset_state(glt_edt, (glitter_curve_type)-1);
    glitter_editor_curve_editor_curves_reset(glt_edt);
    glitter_editor_reset_draw();
}

static void glitter_editor_reset_draw() {
    GPM_VAL.scene = 0;
    GPM_VAL.effect = 0;
    GPM_VAL.emitter = 0;
    GPM_VAL.particle = 0;
}

static void glitter_editor_save(glitter_editor_struct* glt_edt) {
    glt_edt->save_popup = glt_edt->effect_group
        && glt_edt->effect_group->type == GLITTER_X ? false : true;
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
        && glt_edt->effect_group->type == GLITTER_X) {
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

static void glitter_editor_load_file(glitter_editor_struct* glt_edt, char* path, char* file) {
    if (!glt_edt->load_wait) {
        GPM_VAL.FreeScenes();
        GPM_VAL.UnloadEffectGroup(glt_edt->hash);

        glitter_editor_reset_draw();

        GPM_VAL.data = &data_list[glt_edt->load_data_type];

        glt_edt->effect_group = 0;
        glt_edt->scene = 0;
        glt_edt->hash = GPM_VAL.LoadFile(glt_edt->load_glt_type, file, path, -1.0f, false);
        glt_edt->load_wait = true;
    }

    if (GPM_VAL.CheckNoFileReaders(glt_edt->hash)) {
        glitter_effect_group* eg = GPM_VAL.GetEffectGroup(glt_edt->hash);
        GPM_VAL.LoadScene(glt_edt->hash, eg->type != GLITTER_FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty, false);
        glitter_scene* sc = GPM_VAL.GetScene(glt_edt->hash);
        enum_or(sc->flags, GLITTER_SCENE_EDITOR);
        bool lst_not_valid = true;
        if (glt_edt->load_glt_type == GLITTER_FT) {
            lst_not_valid = false;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                if (!*i)
                    continue;

                glitter_effect* e = *i;
                size_t len = utf8_length(e->name);
                if (e->data.name_hash != hash_fnv1a64m(e->name, min(len, 0x80), false)) {
                    lst_not_valid = true;
                    break;
                }
            }
        }

        glitter_editor_reset_draw();
        glt_edt->counter = GPM_VAL.counter;
        glt_edt->effect_group = eg;
        glt_edt->scene = sc;
        glt_edt->scene_counter = sc->counter;

        GPM_VAL.GetStartEndFrame(&glt_edt->start_frame, &glt_edt->end_frame, glt_edt->hash);
        glt_edt->frame_counter = 0;
        glt_edt->old_frame_counter = 0;
        glt_edt->input_pause = true;
        glt_edt->load_data_popup = lst_not_valid;
        glt_edt->load = false;
        glt_edt->load_wait = false;
    }

    glitter_editor_reset(glt_edt);
}

static void glitter_editor_save_file(glitter_editor_struct* glt_edt, char* path, char* file) {
    glitter_type glt_type = glt_edt->save_glt_type;

    f2_struct st;
    farc_file ff_drs;
    farc_file ff_dve;
    farc_file ff_lst;

    char* temp;

    farc f;
    if (glitter_diva_effect_unparse_file(glt_type, glt_edt->effect_group, &st)) {
        f2_struct_mwrite(&st, &ff_dve.data, &ff_dve.size, true, false);
        ff_dve.name = std::string(file);
        ff_dve.name += ".dve";
        f2_struct_free(&st);
    }
    else
        goto End;

    if (glitter_diva_resource_unparse_file(glt_edt->effect_group, &st)) {
        f2_struct_mwrite(&st, &ff_drs.data, &ff_drs.size, true, false);
        ff_drs.name = std::string(file);
        ff_drs.name += ".drs";
    }
    f2_struct_free(&st);

    if (glt_type == GLITTER_FT)
        if (glitter_diva_list_unparse_file(glt_edt->effect_group, &st)) {
            f2_struct_mwrite(&st, &ff_lst.data, &ff_lst.size, true, false);
            ff_lst.name = std::string(file);
            ff_lst.name += ".lst";
        }
        else {
            free(ff_dve.data);
            ff_dve.name;
            free(ff_drs.data);
            ff_drs.name;
            goto End;
        }

    if (ff_drs.data)
        f.files.push_back(ff_drs);
    f.files.push_back(ff_dve);
    if (glt_type == GLITTER_FT)
        f.files.push_back(ff_lst);

    farc_compress_mode mode;
    if (glt_edt->save_compress)
        mode = glt_type != GLITTER_FT ? FARC_COMPRESS_FARC_GZIP_AES : FARC_COMPRESS_FArC;
    else
        mode = FARC_COMPRESS_FArc;

    temp = str_utils_add(path, file);
    if (glt_type != GLITTER_FT) {
        char* list_temp = str_utils_add(temp, ".glitter.txt");
        stream s;
        io_open(&s, list_temp, "wb");
        if (s.io.stream) {
            glitter_effect_group* eg = glt_edt->effect_group;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++)
                if (*i) {
                    io_write(&s, (*i)->name, utf8_length((*i)->name));
                    io_write_char(&s, '\n');
                }
        }
        io_free(&s);
        free(list_temp);
    }
    f.write(temp, mode, false);
    free(temp);

End:
    f2_struct_free(&st);
}

static bool glitter_editor_list_open_window(glitter_effect_group* eg) {
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
        io_wopen(&s, file, L"rb");
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

            uint64_t empty_hash = eg->type != GLITTER_FT
                ? hash_murmurhash_empty : hash_fnv1a64m_empty;
            uint64_t* hashes = force_malloc_s(uint64_t, count);
            if (eg->type != GLITTER_FT)
                for (size_t i = 0; i < count; i++) {
                    size_t len = utf8_length(lines[i]);
                    hashes[i] = hash_murmurhash(lines[i], min(len, 0x7F), 0, false, false);
                }
            else
                for (size_t i = 0; i < count; i++) {
                    size_t len = utf8_length(lines[i]);
                    hashes[i] = hash_fnv1a64m(lines[i], min(len, 0x7F), false);
                }

            ret = true;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                if (!*i)
                    continue;

                glitter_effect* e = *i;
                memset(e->name, 0, 0x80);
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

                size_t len = utf8_length(lines[j]);
                memcpy(e->name, lines[j], min(len, 0x7F));
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
        wchar_t* f = str_utils_wget_without_extension(file);
        wchar_t* p = str_utils_wsplit_right_get_left(file, L'.');
        dds* d = dds_init();

        uint64_t hash_ft = hash_utf16_fnv1a64m(f, false);
        uint64_t hash_f2 = hash_utf16_murmurhash(f, 0, false);

        txp* tex;
        uint32_t index;

        glitter_effect_group* eg = glt_edt->effect_group;
        int32_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        for (int32_t i = 0; i < rc; i++)
            if (rh[i] == hash_ft || rh[i] == hash_f2)
                goto DDSEnd;

        dds_wread(d, p);
        if (d->width == 0 || d->height == 0 || d->mipmaps_count == 0 || vector_old_length(d->data) < 1)
            goto DDSEnd;

        tex = vector_old_txp_reserve_back(&eg->resources_tex);
        tex->array_size = d->has_cube_map ? 6 : 1;
        tex->has_cube_map = d->has_cube_map;
        tex->mipmaps_count = d->mipmaps_count;

        vector_old_txp_mipmap_reserve(&tex->data,
            (tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
        index = 0;
        do
            for (uint32_t i = 0; i < tex->mipmaps_count; i++) {
                txp_mipmap tex_mip;
                memset(&tex_mip, 0, sizeof(txp_mipmap));
                tex_mip.width = max(d->width >> i, 1);
                tex_mip.height = max(d->height >> i, 1);
                tex_mip.format = d->format;

                uint32_t size = txp_get_size(tex_mip.format,
                    tex_mip.width, tex_mip.height);
                tex_mip.size = size;
                tex_mip.data = force_malloc(size);
                memcpy(tex_mip.data, d->data.begin[index], size);
                vector_old_txp_mipmap_push_back(&tex->data, &tex_mip);
                index++;
            }
        while (index / tex->mipmaps_count < tex->array_size);

        eg->resources_count++;
        if (eg->type == GLITTER_FT)
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
        wchar_t* p = str_utils_wsplit_right_get_left(file, L'.');
        dds* d = dds_init();

        glitter_effect_group* eg = glt_edt->effect_group;
        txp tex = eg->resources_tex.begin[sel_rsrc];

        txp_format format = tex.data.begin[0].format;
        uint32_t width = tex.data.begin[0].width;
        uint32_t height = tex.data.begin[0].height;

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
                uint32_t size = txp_get_size(format, max(width >> i, 1), max(height >> i, 1));
                void* data = force_malloc(size);
                memcpy(data, tex.data.begin[index].data, size);
                vector_old_ptr_void_push_back(&d->data, &data);
                index++;
            }
        while (index / tex.mipmaps_count < tex.array_size);
        dds_wwrite(d, p);
        ret = true;
        dds_dispose(d);
        free(p);
    }
    CoUninitialize();
    return ret;
}

static void glitter_editor_test_window(glitter_editor_struct* glt_edt, class_data* data) {
    glitter_effect_group* eg = glt_edt->effect_group;

    float_t win_x;
    float_t win_y;

    float_t x;
    float_t y;

    float_t w = min((float_t)width, 360.0f);
    float_t h = min((float_t)height, 326.0f);

    float_t frame_counter;

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_FirstUseEver, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (!igBegin("Glitter Editor Test Window", &glt_edt->test, window_flags)) {
        data->imgui_focus |= igIsWindowFocused(0);
        goto End;
    }

    if (imguiButton("Reset Camera (R)", ImVec2_Empty))
        input_reset = true;

    w = imguiGetContentRegionAvailWidth();
    if (igBeginTable("buttons", 2, 0, ImVec2_Empty, 0.0f)) {
        igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f, 0);

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Play (T)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_T, true))
            glt_edt->input_play = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reload (Y)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_Y, true))
            glt_edt->input_reload = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Pause (F)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_F, true))
            glt_edt->input_pause = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reset (G)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_G, true))
            glt_edt->input_reset = true;
        igEndTable();
    }

    igSeparator();

    frame_counter = glt_edt->frame_counter;
    glt_edt->old_frame_counter = glt_edt->frame_counter;
    imguiColumnSliderFloat("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, "%.0f", 0, true);
    glt_edt->input_pause_temp = imgui_is_item_activated;

    igText("Start/End Frame: %d/%d %d", glt_edt->start_frame, glt_edt->end_frame, (int32_t)glt_edt->input_pause_temp);

    imguiColumnSliderFloat("Emission", &GPM_VAL.emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    igSeparator();

    imguiCheckbox("Show Grid", &draw_grid_3d);

    imguiCheckbox("Draw All", &GPM_VAL.draw_all);

    imguiDisableElementPush(GPM_VAL.draw_all && eg && eg->type == GLITTER_X);
    imguiCheckbox("Draw All Mesh", &GPM_VAL.draw_all_mesh);
    imguiDisableElementPop(GPM_VAL.draw_all && eg && eg->type == GLITTER_X);

    igSeparator();

    igCheckboxFlags_UintPtr("Draw Wireframe",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
    igCheckboxFlags_UintPtr("Draw Selected",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
    igCheckboxFlags_UintPtr("No Draw",
        (uint32_t*)&glt_edt->draw_flags, GLITTER_EDITOR_DRAW_NO_DRAW);

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();

    win_x = min((float_t)width, 240.0f);
    win_y = min((float_t)height, 96.0f);

    x = 0.0f;
    y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

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

        life_time = 0;
        frame = GPM_VAL.GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
        igText("%.0f - %.0f/%d", max(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_QUAD);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_QUAD);
        igText(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LOCUS);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LOCUS);
        igText("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LINE);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LINE);
        igText(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_MESH);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_MESH);
        igText(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
    }
    igPopStyleColor(2);

End:
    igEnd();
}

static void glitter_editor_effects(glitter_editor_struct* glt_edt) {
    char buf[0x100];

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    glitter_effect_group* eg = glt_edt->effect_group;
    igPushID_Ptr(eg);
    ssize_t i_idx = 1;
    for (glitter_effect** i = eg->effects.begin;
        i != eg->effects.end; i++, i_idx++) {
        if (!*i)
            continue;

        glitter_effect* effect = *i;
        tree_node_flags = tree_node_base_flags;
        if (effect == glt_edt->selected_effect && !glt_edt->selected_emitter
            && !glt_edt->selected_particle)
            tree_node_flags |= ImGuiTreeNodeFlags_Selected;

        const char* eff_str;
        if (effect->data.ext_anim) {
            if (effect->data.ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM)
                eff_str = "%s (%08X) [C]";
            else
                eff_str = "%s (%08X) [O]";
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOCAL)
            eff_str = "%s (%08X) [L]";
        else
            eff_str = "%s (%08X)";


        igPushID_Ptr(effect);
        if (igTreeNodeEx_StrStr("effect", tree_node_flags, eff_str,
            effect->name, (uint32_t)((size_t)effect * hash_fnv1a64m_empty))) {
            glitter_editor_effects_context_menu(glt_edt, effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);

            ssize_t j_idx = 1;
            for (glitter_emitter** j = effect->emitters.begin;
                j != effect->emitters.end; j++, j_idx++) {
                if (!*j)
                    continue;

                glitter_emitter* emitter = *j;
                tree_node_flags = tree_node_base_flags;
                if (emitter == glt_edt->selected_emitter && !glt_edt->selected_particle)
                    tree_node_flags |= ImGuiTreeNodeFlags_Selected;

                const char* emit_str;
                if (emitter->data.timer == GLITTER_EMITTER_TIMER_BY_TIME) {
                    if (emitter->data.emission_interval <= -0.000001f)
                        emit_str = "Emitter %lld (%08X) [E]";
                    else if (emitter->data.emission_interval >= 0.000001f)
                        emit_str = "Emitter %lld (%08X) [T]";
                    else
                        emit_str = "Emitter %lld (%08X) [S]";
                }
                else if (emitter->data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE)
                    emit_str = "Emitter %lld (%08X) [D]";
                else
                    emit_str = "Emitter %lld (%08X)";

                igPushID_Ptr(emitter);
                if (igTreeNodeEx_StrStr("emitter", tree_node_flags, emit_str,
                    j_idx, (uint32_t)((size_t)emitter * hash_fnv1a64m_empty))) {
                    glitter_editor_effects_context_menu(glt_edt, effect, emitter, 0,
                        i_idx, j_idx, 0, GLITTER_EDITOR_SELECTED_EMITTER);

                    ssize_t k_idx = 1;
                    for (glitter_particle** k = emitter->particles.begin;
                        k != emitter->particles.end; k++, k_idx++) {
                        if (!*k)
                            continue;

                        glitter_particle* particle = *k;
                        tree_node_flags = tree_node_base_flags;

                        const char* ptcl_str;
                        switch (particle->data.type) {
                        case GLITTER_PARTICLE_QUAD:
                            ptcl_str = "Particle %lld (%08X) [Quad]";
                            break;
                        case GLITTER_PARTICLE_LINE:
                            ptcl_str = "Particle %lld (%08X) [Line]";
                            break;
                        case GLITTER_PARTICLE_LOCUS:
                            ptcl_str = "Particle %lld (%08X) [Locus]";
                            break;
                        case GLITTER_PARTICLE_MESH:
                            ptcl_str = "Particle %lld (%08X) [Mesh]";
                            break;
                        default:
                            ptcl_str = "Particle %lld (%08X)";
                            break;
                        }

                        igPushID_Ptr(particle);
                        snprintf(buf, sizeof(buf), ptcl_str,
                            k_idx, (uint32_t)((size_t)particle * hash_fnv1a64m_empty));
                        igSelectable_Bool(buf, particle == glt_edt->selected_particle,
                            0, { 0.0f, 0.0f });
                        glitter_editor_effects_context_menu(glt_edt, effect, emitter, particle,
                            i_idx, j_idx, k_idx, GLITTER_EDITOR_SELECTED_PARTICLE);
                        igPopID();
                    }
                    igTreePop();
                }
                else
                    glitter_editor_effects_context_menu(glt_edt, effect, emitter, 0,
                        i_idx, j_idx, 0, GLITTER_EDITOR_SELECTED_EMITTER);
                igPopID();
            }
            igTreePop();
        }
        else
            glitter_editor_effects_context_menu(glt_edt, effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);
        igPopID();
    }
    igPopID();
}

static void glitter_editor_effects_context_menu(glitter_editor_struct* glt_edt,
    glitter_effect* effect, glitter_emitter* emitter, glitter_particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type) {
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_emitter* sel_emit = glt_edt->selected_emitter;

    bool focused = igIsItemFocused();

    ImGuiWindowFlags window_flags;
    bool close;

    static void* selected;
    if (type == GLITTER_EDITOR_SELECTED_NONE) {
        if (igIsWindowHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
            igOpenPopup_Str("effects popup", ImGuiPopupFlags_MouseButtonRight);
            selected = 0;
        }
        else if (selected != 0)
            goto End;
    }
    else if (igIsItemHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
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
        igOpenPopup_Str("effects popup", ImGuiPopupFlags_MouseButtonRight);
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

    if (!igBeginPopup("effects popup", window_flags))
        goto End;

    global_context_menu = false;

    close = false;
    switch (type) {
    case GLITTER_EDITOR_SELECTED_NONE: {
        if (igMenuItem_Bool("Add Effect", 0, false, true)) {
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
            if (effect->data.ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM)
                eff_str = "%s (%08X) [C]";
            else
                eff_str = "%s (%08X) [O]";
        }
        else if (effect->data.flags & GLITTER_EFFECT_LOCAL)
            eff_str = "%s (%08X) [L]";
        else
            eff_str = "%s (%08X)";

        glitter_effect_group* eg = glt_edt->effect_group;
        igText(eff_str, effect->name, (uint32_t)((size_t)effect * hash_fnv1a64m_empty));
        igSeparator();
        if (igMenuItem_Bool("Duplicate Effect", 0, false, true)) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Delete Effect", 0, false, true)) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Move Effect Up", 0, false, i_idx > 1)) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Move Effect Down", 0, false, i_idx < vector_old_length(eg->effects))) {
            enum_or(glt_edt->effect_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_effect = effect;
            close = true;
        }

        igSeparator();

        if (igMenuItem_Bool("Add Emitter", 0, false, true)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_ADD);
            glt_edt->selected_effect = effect;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_EMITTER: {
        if (!glt_edt->selected_effect)
            break;

        const char* emit_str;
        if (emitter->data.timer == GLITTER_EMITTER_TIMER_BY_TIME) {
            if (emitter->data.emission_interval <= -0.000001f)
                emit_str = "Emitter %lld (%08X) [E]";
            else if (emitter->data.emission_interval >= 0.000001f)
                emit_str = "Emitter %lld (%08X) [T]";
            else
                emit_str = "Emitter %lld (%08X) [S]";
        }
        else if (emitter->data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE)
            emit_str = "Emitter %lld (%08X) [D]";
        else
            emit_str = "Emitter %lld (%08X)";

        igText(emit_str, j_idx, (size_t)emitter * hash_fnv1a64m_empty);
        igSeparator();
        if (igMenuItem_Bool("Duplicate Emitter", 0, false, true)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Delete Emitter", 0, false, true)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Move Emitter Up", 0, false, j_idx > 1)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Move Emitter Down", 0, false,
            j_idx < sel_efct->emitters.end
            - sel_efct->emitters.begin)) {
            enum_or(glt_edt->emitter_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_emitter = emitter;
            close = true;
        }

        igSeparator();

        if (igMenuItem_Bool("Add Particle", 0, false, true)) {
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
        case GLITTER_PARTICLE_QUAD:
            ptcl_str = "Particle %lld (%08X) [Quad]";
            break;
        case GLITTER_PARTICLE_LINE:
            ptcl_str = "Particle %lld (%08X) [Line]";
            break;
        case GLITTER_PARTICLE_LOCUS:
            ptcl_str = "Particle %lld (%08X) [Locus]";
            break;
        case GLITTER_PARTICLE_MESH:
            ptcl_str = "Particle %lld (%08X) [Mesh]";
            break;
        default:
            ptcl_str = "Particle %lld (%08X)";
            break;
        }

        igText(ptcl_str, k_idx, (uint32_t)((size_t)particle * hash_fnv1a64m_empty));
        igSeparator();
        if (igMenuItem_Bool("Duplicate Particle", 0, false, true)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_DUPLICATE);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Delete Particle", 0, false, true)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Move Particle Up", 0, false, k_idx > 1)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Move Particle Down", 0, false,
            k_idx < sel_emit->particles.end
            - sel_emit->particles.begin)) {
            enum_or(glt_edt->particle_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_particle = particle;
            close = true;
        }
    } break;
    }

    if (close) {
        igCloseCurrentPopup();
        selected = 0;
    }
    igEndPopup();

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

    glitter_effect_group* eg = glt_edt->effect_group;
    texture** r = eg->resources;
    int32_t rc = eg->resources_count;
    uint64_t* rh = eg->resource_hashes.data();
    txp* rt = eg->resources_tex.begin;
    igPushID_Ptr(eg);
    ssize_t i_idx = 1;
    for (int32_t i = 0; i < rc; i++, i_idx++) {
        snprintf(buf, sizeof(buf), "Texture %d (%016llX)", i + 1, rh[i]);
        igPushID_Int(i);
        igSelectable_Bool(buf, i == sel_rsrc, 0, { 0.0f, 0.0f });
        if (igIsItemHovered(0)) {
            txp_mipmap* rtm = rt[i].data.begin;
            float_t aspect = (float_t)rtm->width / (float_t)rtm->height;

            ImVec2 size = { 192.0f, 192.0f };
            if (aspect > 1.0f)
                size.y /= aspect;
            else if (aspect < 1.0f)
                size.x *= aspect;

            igBeginTooltip();
            igText("Tex Size: %dx%d", rtm->width, rtm->height);\
            igImage((void*)(size_t)r[i]->texture, size,
                ImVec2_Empty, ImVec2_Identity, tint_col, border_col);
            igEndTooltip();
        }
        glitter_editor_resources_context_menu(glt_edt, i, i_idx, false);
        igPopID();
    }
    igPopID();
}

static void glitter_editor_resources_context_menu(glitter_editor_struct* glt_edt,
    int32_t resource, ssize_t i_idx, bool selected_none) {
    bool focused = igIsItemFocused();

    ImGuiWindowFlags window_flags;
    bool close;

    static size_t selected;
    if (selected_none) {
        if (igIsWindowHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
            igOpenPopup_Str("resources popup", ImGuiPopupFlags_MouseButtonRight);
            selected = -1;
        }
        else if (selected != -1)
            goto End;
    }
    else if (igIsItemHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
        selected = resource;
        igOpenPopup_Str("resources popup", ImGuiPopupFlags_MouseButtonRight);
    }
    else if (selected != resource)
        goto End;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    if (!igBeginPopup("resources popup", window_flags))
        goto End;

    global_context_menu = false;

    close = false;
    if (selected_none) {
        if (igMenuItem_Bool("Import Resource", 0, false, true)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_IMPORT);
            close = true;
        }
    }
    else {
        size_t i = resource;
        glitter_effect_group* eg = glt_edt->effect_group;
        ssize_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        igText("Texture %lld (%016llX)", i + 1, rh[i]);
        igSeparator();
        if (igMenuItem_Bool("Export Resource", 0, false, true)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_EXPORT);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Delete Resource", 0, false, true)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_DELETE);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Move Resource Up", 0, false, i_idx > 1)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_MOVE_UP);
            glt_edt->selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Move Resource Down", 0, false,
            i_idx < rc)) {
            enum_or(glt_edt->resource_flags, GLITTER_EDITOR_MOVE_DOWN);
            glt_edt->selected_resource = resource;
            close = true;
        }
    }

    if (close) {
        igCloseCurrentPopup();
        selected = 0;
    }
    igEndPopup();

End:
    if (!focused)
        return;

    glt_edt->selected_resource = resource;
}

static void glitter_editor_play_manager(glitter_editor_struct* glt_edt) {
    glitter_effect_group* eg = glt_edt->effect_group;

    if (imguiButton("Reset Camera (R)", ImVec2_Empty))
        input_reset = true;

    float_t w = imguiGetContentRegionAvailWidth();
    if (igBeginTable("buttons", 2, 0, ImVec2_Empty, 0.0f)) {
        igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f, 0);

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Play (T)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_T, true))
            glt_edt->input_play = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reload (Y)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_Y, true))
            glt_edt->input_reload = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Pause (F)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_F, true))
            glt_edt->input_pause = true;

        igTableNextColumn();
        w = imguiGetContentRegionAvailWidth();
        if (imguiButton("Reset (G)", { w, 0.0f }) || igIsKeyPressed(GLFW_KEY_G, true))
            glt_edt->input_reset = true;
        igEndTable();
    }

    igSeparator();

    glt_edt->old_frame_counter = glt_edt->frame_counter;
    glt_edt->input_pause_temp = imguiColumnSliderFloat("Frame", &glt_edt->frame_counter, 1.0f,
        (float_t)glt_edt->start_frame, (float_t)glt_edt->end_frame, "%.0f", 0, true);

    igText("Start/End Frame: %d/%d", glt_edt->start_frame, glt_edt->end_frame);

    igSeparator();

    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    life_time = 0;
    frame = GPM_VAL.GetSceneFrameLifeTime(glt_edt->scene_counter, &life_time);
    igText("%.0f - %.0f/%d", max(glt_edt->frame_counter - glt_edt->start_frame, 0), frame, life_time);

    ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_QUAD);
    disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_QUAD);
    igText(" Quad: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LOCUS);
    disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LOCUS);
    igText("Locus: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LINE);
    disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LINE);
    igText(" Line: ctrl%lld, disp%lld", ctrl, disp);

    ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_MESH);
    disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_MESH);
    igText(" Mesh: ctrl%lld, disp%lld", ctrl, disp);

    igSeparator();

    imguiColumnSliderFloat("Emission", &GPM_VAL.emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    igSeparator();

    imguiCheckbox("Grid", &draw_grid_3d);

    imguiCheckbox("Draw All", &GPM_VAL.draw_all);

    imguiDisableElementPush(GPM_VAL.draw_all && eg && eg->type == GLITTER_X);
    imguiCheckbox("Draw All Mesh", &GPM_VAL.draw_all_mesh);
    imguiDisableElementPop(GPM_VAL.draw_all && eg && eg->type == GLITTER_X);
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

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_effect effect = *glt_edt->selected_effect;
    bool changed = false;

    glitter_curve_type_flags flags = glitter_effect_curve_flags;
    for (int32_t i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (glitter_curve** j = effect.animation.begin; j != effect.animation.end; j++)
            if (*j && (*j)->type == i)
                enum_and(flags, ~(1 << i));

    const size_t name_size = sizeof(effect.name);
    char name[sizeof(effect.name)];
    memcpy(name, effect.name, name_size);
    if (imguiColumnInputText("Name", name, name_size, 0, 0, 0)) {
        effect.data.name_hash = eg->type != GLITTER_FT
            ? hash_utf8_murmurhash(name, 0, false) : hash_utf8_fnv1a64m(name, false);
        memcpy(effect.name, name, name_size - 1);
        changed = true;
    }

    if (imguiColumnDragInt("Appear Time",
        &effect.data.appear_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Life Time",
        &effect.data.life_time, 1.0f, 0x0000, 0x7FFF - effect.data.appear_time, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Start Time",
        &effect.data.start_time, 1.0f, 0x0000, 0x7FFF - effect.data.appear_time, "%d", 0))
        changed = true;

    igSeparator();

    if (imguiColumnDragVec3Flag("Translation",
        &effect.translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & GLITTER_CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    vec3 rotation;
    vec3_mult_scalar(effect.rotation, RAD_TO_DEG_FLOAT, rotation);
    if (imguiColumnDragVec3Flag("Rotation",
        &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, effect.rotation);
        changed = true;
    }

    if (imguiColumnDragVec3Flag("Scale",
        &effect.scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    igSeparator();

    if (imguiCheckboxFlags_UintPtr("Loop",
        (uint32_t*)&effect.data.flags,
        GLITTER_EFFECT_LOOP))
        changed = true;

    if (imguiCheckboxFlags_UintPtr("Draw as Transparent",
        (uint32_t*)&effect.data.flags,
        GLITTER_EFFECT_ALPHA))
        changed = true;

    if (imguiCheckboxFlags_UintPtr("Fog",
        (uint32_t*)&effect.data.flags,
        GLITTER_EFFECT_FOG))
        changed = true;

    if (imguiCheckboxFlags_UintPtr("Fog Height",
        (uint32_t*)&effect.data.flags,
        GLITTER_EFFECT_FOG_HEIGHT))
        changed = true;

    if (imguiCheckboxFlags_UintPtr("Use External Emission",
        (uint32_t*)&effect.data.flags,
        GLITTER_EFFECT_EMISSION))
        changed = true;

    if (eg->type == GLITTER_X)
        if (imguiCheckboxFlags_UintPtr("Use Seed",
            (uint32_t*)&effect.data.flags,
            GLITTER_EFFECT_USE_SEED))
            changed = true;

    if (effect.data.flags & GLITTER_EFFECT_FOG && effect.data.flags & GLITTER_EFFECT_FOG_HEIGHT) {
        enum_and(effect.data.flags, ~GLITTER_EFFECT_FOG_HEIGHT);
        changed = true;
    }

    if (effect.data.flags & GLITTER_EFFECT_FOG_HEIGHT && effect.data.flags & GLITTER_EFFECT_FOG) {
        enum_and(effect.data.flags, ~GLITTER_EFFECT_FOG);
        changed = true;
    }

    if (eg->type != GLITTER_X) {
        bool set_emission = effect.version == 7;
        if (imguiCheckbox("Set Emission",
            &set_emission)) {
            effect.version = set_emission ? 7 : 6;
            changed = true;
        }

        if (effect.version == 7 && imguiColumnDragFloat("Emission",
            &effect.data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
    }
    else {
        if (effect.data.flags & GLITTER_EFFECT_USE_SEED
            && imguiColumnInputScalar("Seed", ImGuiDataType_U32, &effect.data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (imguiColumnDragFloat("Emission",
            &effect.data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
    }

    int32_t type;
    if (effect.data.flags & GLITTER_EFFECT_LOCAL)
        type = 1;
    else if (effect.data.ext_anim)
        type = effect.data.ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM ? 2 : 3;
    else
        type = 0;

    if (imguiColumnComboBox("Type", glitter_effect_type_name,
        4, &type, 0, false, &data->imgui_focus)) {
        if (type == 1) {
            enum_or(effect.data.flags, GLITTER_EFFECT_LOCAL);
            free(effect.data.ext_anim);
        }
        else {
            enum_and(effect.data.flags, ~GLITTER_EFFECT_LOCAL);
            if (type == 2 || type == 3) {
                if (!effect.data.ext_anim)
                    effect.data.ext_anim = force_malloc_s(glitter_effect_ext_anim, 1);
                if (type == 2)
                    enum_or(effect.data.ext_anim->flags, GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM);
                else
                    enum_and(effect.data.ext_anim->flags, ~GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM);
            }
            else
                free(effect.data.ext_anim);
        }
        changed = true;
    }

    if (~effect.data.flags & GLITTER_EFFECT_LOCAL && effect.data.ext_anim) {
        igSeparator();

        bool ext_anim_changed = false;
        if (eg->type == GLITTER_X) {
            glitter_effect_ext_anim_x ext_anim = *effect.data.ext_anim_x;
            if (ext_anim.flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = 4;
                if (imguiColumnComboBox("Chara Index", glitter_effect_ext_anim_index_name,
                    max_chara, &ext_anim.chara_index, 0, false, &data->imgui_focus))
                    ext_anim_changed = true;

                int32_t node_index = ext_anim.node_index;
                if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", glitter_effect_ext_anim_node_index_name,
                    GLITTER_EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &data->imgui_focus)) {
                    node_index--;
                    if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim.node_index != node_index) {
                        ext_anim.node_index = (glitter_effect_ext_anim_chara_node)node_index;
                        ext_anim_changed = true;
                    }
                }
            }
            else {
                uint64_t object_hash = ext_anim.object_hash;
                if (glitter_editor_hash_input(glt_edt, "Object Hash", &object_hash)) {
                    ext_anim.object_hash = (uint32_t)object_hash;
                    ext_anim_changed = true;
                }

                imguiColumnInputScalar("Instance ID", ImGuiDataType_U32, &ext_anim.instance_id,
                    0, 0, "%d", ImGuiInputTextFlags_CharsDecimal);

                uint64_t file_name_hash = ext_anim.file_name_hash;
                if (glitter_editor_hash_input(glt_edt, "File Name Hash", &file_name_hash)) {
                    ext_anim.file_name_hash = (uint32_t)file_name_hash;
                    ext_anim_changed = true;
                }

                const size_t mesh_name_size = sizeof(ext_anim.mesh_name);
                char mesh_name[sizeof(ext_anim.mesh_name)];
                memcpy(mesh_name, ext_anim.mesh_name, mesh_name_size);
                if (imguiColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim.mesh_name, mesh_name, mesh_name_size);
                    ext_anim_changed = true;
                }
            }

            if (imguiCheckboxFlags_UintPtr("Trans Only",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY))
                ext_anim_changed = true;

            if (imguiCheckboxFlags_UintPtr("No Trans X",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_NO_TRANS_X))
                ext_anim_changed = true;

            if (imguiCheckboxFlags_UintPtr("No Trans Y",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Y))
                ext_anim_changed = true;

            if (imguiCheckboxFlags_UintPtr("No Trans Z",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_NO_TRANS_Z))
                ext_anim_changed = true;

            if (imguiCheckboxFlags_UintPtr("Draw Only when Chara Visible",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_NO_DRAW_IF_NO_DATA))
                ext_anim_changed = true;

            if (imguiCheckboxFlags_UintPtr("Get Then Update",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_GET_THEN_UPDATE))
                ext_anim_changed = true;

            uint64_t hash1 = hash_fnv1a64m(effect.data.ext_anim_x, sizeof(glitter_effect_ext_anim_x), false);
            uint64_t hash2 = hash_fnv1a64m(&ext_anim, sizeof(glitter_effect_ext_anim_x), false);
            if (hash1 != hash2 && ext_anim_changed) {
                *effect.data.ext_anim_x = ext_anim;
                changed = true;
            }
        }
        else {
            glitter_effect_ext_anim ext_anim = *effect.data.ext_anim;
            if (ext_anim.flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = eg->type == GLITTER_FT ? 6 : 3;
                if (imguiColumnComboBox("Chara Index", glitter_effect_ext_anim_index_name,
                    max_chara, &ext_anim.chara_index, 0, false, &data->imgui_focus))
                    ext_anim_changed = true;

                int32_t node_index = ext_anim.node_index;
                if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", glitter_effect_ext_anim_node_index_name,
                    GLITTER_EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &data->imgui_focus)) {
                    node_index--;
                    if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim.node_index != node_index) {
                        ext_anim.node_index = (glitter_effect_ext_anim_chara_node)node_index;
                        ext_anim_changed = true;
                    }
                }
            }
            else {
                if (glitter_editor_hash_input(glt_edt, "Object Hash", &ext_anim.object_hash)) {
                    ext_anim.object = glitter_effect_ext_anim_get_object_info(ext_anim.object_hash);
                    ext_anim_changed = true;
                }

                const size_t mesh_name_size = sizeof(ext_anim.mesh_name);
                char mesh_name[sizeof(ext_anim.mesh_name)];
                memcpy(mesh_name, ext_anim.mesh_name, mesh_name_size);
                if (imguiColumnInputText("Mesh Name", mesh_name, mesh_name_size, 0, 0, 0)) {
                    memcpy(ext_anim.mesh_name, mesh_name, mesh_name_size);
                    ext_anim_changed = true;
                }
            }

            if (imguiCheckboxFlags_UintPtr("Trans Only",
                (uint32_t*)&ext_anim.flags,
                GLITTER_EFFECT_EXT_ANIM_TRANS_ONLY))
                ext_anim_changed = true;

            uint64_t hash1 = hash_fnv1a64m(effect.data.ext_anim, sizeof(glitter_effect_ext_anim), false);
            uint64_t hash2 = hash_fnv1a64m(&ext_anim, sizeof(glitter_effect_ext_anim), false);
            if (hash1 != hash2 && ext_anim_changed) {
                *effect.data.ext_anim = ext_anim;
                changed = true;
            }
        }
    }

    uint64_t hash1 = hash_fnv1a64m(glt_edt->selected_effect, sizeof(glitter_effect), false);
    uint64_t hash2 = hash_fnv1a64m(&effect, sizeof(glitter_effect), false);
    if (hash1 != hash2 && changed) {
        for (glitter_emitter** i = effect.emitters.begin; i != effect.emitters.end; i++) {
            if (!*i)
                continue;

            glitter_emitter* emitter = *i;
            for (glitter_particle** j = emitter->particles.begin; j != emitter->particles.end; j++) {
                if (!*j)
                    continue;

                glitter_particle* particle = *j;
                glitter_particle_flag flags = particle->data.flags;
                if (effect.data.flags & GLITTER_EFFECT_LOCAL)
                    enum_or(flags, GLITTER_PARTICLE_LOCAL);
                else
                    enum_and(flags, ~GLITTER_PARTICLE_LOCAL);

                if (effect.data.flags & GLITTER_EFFECT_EMISSION
                    || particle->data.emission >= glitter_min_emission)
                    enum_or(flags, GLITTER_PARTICLE_EMISSION);
                else
                    enum_and(flags, ~GLITTER_PARTICLE_EMISSION);
                particle->data.flags = flags;
            }
        }
        glt_edt->input_reload = true;
        *glt_edt->selected_effect = effect;
    }

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_emitter(glitter_editor_struct* glt_edt, class_data* data) {
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_emitter* sel_emit = glt_edt->selected_emitter;

    imguiSetColumnSpace(2.0f / 5.0f);

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_emitter emitter = *glt_edt->selected_emitter;
    bool changed = false;

    glitter_curve_type_flags flags = glitter_emitter_curve_flags;
    for (int32_t i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (glitter_curve** j = emitter.animation.begin; j != emitter.animation.end; j++)
            if (*j && (*j)->type == i)
                enum_and(flags, ~(1 << i));

    igText("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        sel_efct->data.appear_time,
        sel_efct->data.life_time,
        sel_efct->data.start_time);

    igSeparator();

    if (imguiColumnDragInt("Start Time",
        &emitter.data.start_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Life Time",
        &emitter.data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("Loop Start Time", &emitter.data.loop_start_time,
        1.0f, -0x0001, emitter.data.start_time + emitter.data.life_time, "%d", 0)) {
        if (emitter.data.loop_start_time < emitter.data.start_time)
            emitter.data.loop_start_time = 0;
        changed = emitter.data.loop_start_time != sel_emit->data.loop_start_time;
    }

    if (imguiColumnDragInt("Loop End Time", &emitter.data.loop_end_time,
        1.0f, -0x0001, emitter.data.start_time + emitter.data.life_time, "%d", 0)) {
        if (emitter.data.loop_end_time < 0)
            emitter.data.loop_end_time = -1;
        changed = emitter.data.loop_end_time != sel_emit->data.loop_end_time;
    }

    igSeparator();

    if (imguiColumnDragVec3Flag("Translation",
        &emitter.translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & GLITTER_CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    if (eg->type != GLITTER_X) {
        bool draw_z_axis = emitter.data.direction == GLITTER_DIRECTION_Z_AXIS;

        vec3 rotation = emitter.rotation;
        if (draw_z_axis)
            rotation.z -= (float_t)M_PI_2;
        vec3_mult_scalar(rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, rotation);
            if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            emitter.rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation;
        vec3_mult_scalar(emitter.rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, emitter.rotation);
            changed = true;
        }
    }

    vec3 rotation_add;
    vec3_mult_scalar(emitter.data.rotation_add, RAD_TO_DEG_FLOAT, rotation_add);
    if (imguiColumnDragVec3Flag("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add, DEG_TO_RAD_FLOAT, emitter.data.rotation_add);
        changed = true;
    }

    if (imguiColumnDragVec3Flag("Scale",
        &emitter.scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_SCALE_XYZ) >> 6))
        changed = true;

    igSeparator();

    if (imguiCheckboxFlags_UintPtr("Loop",
        (uint32_t*)&emitter.data.flags,
        GLITTER_EMITTER_LOOP))
        changed = true;

    if (imguiCheckboxFlags_UintPtr("Kill On End",
        (uint32_t*)&emitter.data.flags,
        GLITTER_EMITTER_KILL_ON_END))
        changed = true;

    if (eg->type == GLITTER_X
        && imguiCheckboxFlags_UintPtr("Use Seed",
            (uint32_t*)&emitter.data.flags,
            GLITTER_EMITTER_USE_SEED))
        changed = true;

    igSeparator();

    if (eg->type == GLITTER_X) {
        if (emitter.data.flags & GLITTER_EMITTER_USE_SEED
            && imguiColumnInputScalar("Seed", ImGuiDataType_U32, &emitter.data.seed,
                0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal))
            changed = true;

        if (imguiColumnComboBox("Timer Type", glitter_emitter_timer_name,
            GLITTER_EMITTER_TIMER_BY_DISTANCE,
            (int32_t*)&emitter.data.timer, 0, true, &data->imgui_focus)) {
            float_t emission_interval = emitter.data.emission_interval;
            if (emitter.data.timer == GLITTER_EMITTER_TIMER_BY_TIME)
                emission_interval = clamp(floorf(emission_interval), 1.0f, FLT_MAX);
            else
                emission_interval = clamp(emission_interval, 0.0f, FLT_MAX);
            emitter.data.emission_interval = emission_interval;
            changed = true;
        }
    }

    if (eg->type == GLITTER_X
        && emitter.data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE) {
        if (imguiColumnDragFloatFlag("Emit Interval",
            &emitter.data.emission_interval, 0.0001f, -1.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & GLITTER_CURVE_TYPE_EMISSION_INTERVAL) >> 20))
            changed = true;
    }
    else {
        glitter_emitter_emission emission;
        if (emitter.data.emission_interval >= -0.000001f)
            emission = emitter.data.emission_interval <= 0.000001f
            ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
        else
            emission = GLITTER_EMITTER_EMISSION_ON_END;

        if (imguiColumnComboBox("Emission Type", glitter_emitter_emission_name,
            GLITTER_EMITTER_EMISSION_ON_END,
            (int32_t*)&emission, 0, true, &data->imgui_focus)) {
            float_t emission_interval;
            switch (emission) {
            case GLITTER_EMITTER_EMISSION_ON_TIMER:
            default:
                emission_interval = 1.0f;
                break;
            case GLITTER_EMITTER_EMISSION_ON_START:
                emission_interval = 0.0f;
                break;
            case GLITTER_EMITTER_EMISSION_ON_END:
                emission_interval = -1.0f;
                break;
            }

            if (emission_interval != emitter.data.emission_interval) {
                emitter.data.emission_interval = emission_interval;
                changed = true;
            }
        }

        if (emission == GLITTER_EMITTER_EMISSION_ON_TIMER
            && imguiColumnDragFloatFlag("Emit Interval",
                &emitter.data.emission_interval, 1.0f, 1.0f, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat,
                (flags & GLITTER_CURVE_TYPE_EMISSION_INTERVAL) >> 20))
                changed = true;
    }

    if (imguiColumnDragFloatFlag("PTC Per Emit",
        &emitter.data.particles_per_emission, 1.0f, 0.0f, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_PARTICLES_PER_EMISSION) >> 21))
        changed = true;

    glitter_emitter_direction direction = glitter_emitter_direction_default;
    glitter_emitter_direction prev_direction;
    for (int32_t i = 0; i < glitter_emitter_direction_types_count; i++)
        if (glitter_emitter_direction_types[i] == emitter.data.direction) {
            direction = (glitter_emitter_direction)i;
            break;
        }
    prev_direction = direction;

    if (imguiColumnComboBox("Direction", glitter_emitter_direction_name,
        GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION,
        (int32_t*)&direction, 0, true, &data->imgui_focus))
        if (glitter_emitter_direction_types[direction] != emitter.data.direction) {
            changed = true;
            if (eg->type != GLITTER_X) {
                bool draw_z_axis =
                    direction == GLITTER_EMITTER_DIRECTION_Z_AXIS;
                bool prev_draw_z_axis =
                    prev_direction == GLITTER_EMITTER_DIRECTION_Z_AXIS;
                if (draw_z_axis && !prev_draw_z_axis) {
                    if (~flags & GLITTER_CURVE_TYPE_ROTATION_Z)
                        glitter_animation_add_value(eg->type, &emitter.animation,
                            (float_t)M_PI_2, GLITTER_CURVE_TYPE_ROTATION_Z);
                    emitter.rotation.z += (float_t)M_PI_2;
                }
                else if (!draw_z_axis && prev_draw_z_axis) {
                    if (~flags & GLITTER_CURVE_TYPE_ROTATION_Z)
                        glitter_animation_add_value(eg->type, &emitter.animation,
                            (float_t)-M_PI_2, GLITTER_CURVE_TYPE_ROTATION_Z);
                    emitter.rotation.z -= (float_t)M_PI_2;
                }
            }
            emitter.data.direction = glitter_emitter_direction_types[direction];
            changed = true;
        }

    if (imguiColumnComboBox("Type", glitter_emitter_name,
        GLITTER_EMITTER_POLYGON,
        (int32_t*)&emitter.data.type, 0, true, &data->imgui_focus))
        changed = true;

    igSeparator();

    switch (emitter.data.type) {
    case GLITTER_EMITTER_BOX: {
        igPushID_Str("Box");
        if (imguiColumnDragVec3("Size",
            &emitter.data.box.size, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        igPopID();
    } break;
    case GLITTER_EMITTER_CYLINDER: {
        float_t start_angle, end_angle;

        igPushID_Str("Cylinder");
        if (imguiColumnDragFloat("Radius",
            &emitter.data.cylinder.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragFloat("Height",
            &emitter.data.cylinder.height, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        start_angle = emitter.data.cylinder.start_angle * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Start Angle",
            &start_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter.data.cylinder.start_angle = start_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        end_angle = emitter.data.cylinder.end_angle * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("End Angle",
            &end_angle, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter.data.cylinder.end_angle = end_angle * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (imguiCheckbox("On Edge",
            &emitter.data.cylinder.on_edge))
            changed = true;

        if (imguiColumnComboBox("Direction",
            glitter_emitter_emission_direction_name,
            GLITTER_EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter.data.cylinder.direction, 0, true, &data->imgui_focus))
            changed = true;
        igPopID();
    } break;
    case GLITTER_EMITTER_SPHERE: {
        float_t latitude, longitude;

        igPushID_Str("Sphere");
        if (imguiColumnDragFloat("Radius",
            &emitter.data.sphere.radius, 0.0001f, 0.0f, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        latitude = emitter.data.sphere.latitude * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Latitude",
            &latitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter.data.sphere.latitude = latitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        longitude = emitter.data.sphere.longitude * RAD_TO_DEG_FLOAT;
        if (imguiColumnDragFloat("Longitude",
            &longitude, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat)) {
            emitter.data.sphere.longitude = longitude * DEG_TO_RAD_FLOAT;
            changed = true;
        }

        if (imguiCheckbox("On Edge",
            &emitter.data.sphere.on_edge))
            changed = true;

        if (imguiColumnComboBox("Direction",
            glitter_emitter_emission_direction_name,
            GLITTER_EMITTER_EMISSION_DIRECTION_INWARD,
            (int32_t*)&emitter.data.sphere.direction, 0, true, &data->imgui_focus))
            changed = true;
        igPopID();
    } break;
    case GLITTER_EMITTER_POLYGON: {
        igPushID_Str("Polygon");
        if (imguiColumnDragFloat("Scale",
            &emitter.data.polygon.size, 0.0001f, 0.0f, FLT_MAX,
            "%g", ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragInt("Count",
            &emitter.data.polygon.count, 1.0f, 0, 0x7FFFFF, "%d", 0))
            changed = true;

        if (eg->type == GLITTER_X)
            if (imguiColumnComboBox("Direction",
                glitter_emitter_emission_direction_name,
                GLITTER_EMITTER_EMISSION_DIRECTION_INWARD,
                (int32_t*)&emitter.data.cylinder.direction, 0, true, &data->imgui_focus))
                changed = true;
        igPopID();
    } break;
    }

    uint64_t hash1 = hash_fnv1a64m(glt_edt->selected_emitter, sizeof(glitter_emitter), false);
    uint64_t hash2 = hash_fnv1a64m(&emitter, sizeof(glitter_emitter), false);
    if (hash1 != hash2 && changed) {
        glt_edt->input_reload = true;
        *glt_edt->selected_emitter = emitter;
    }

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_particle(glitter_editor_struct* glt_edt, class_data* data) {
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_emitter* sel_emit = glt_edt->selected_emitter;

    imguiSetColumnSpace(2.0f / 5.0f);

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_particle particle = *glt_edt->selected_particle;
    bool changed = false;

    glitter_curve_type_flags flags = (glitter_curve_type_flags)0;
    if (eg->type == GLITTER_X)
        flags = glitter_particle_x_curve_flags;
    else
        flags = glitter_particle_curve_flags;

    if (particle.data.type != GLITTER_PARTICLE_MESH) {
        enum_and(flags, ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND);
        if (particle.data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
            enum_and(flags, ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y));
    }

    if (particle.data.sub_flags & GLITTER_PARTICLE_SUB_USE_CURVE)
        for (int32_t i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
            for (glitter_curve** j = particle.animation.begin; j != particle.animation.end; j++)
                if (*j && (*j)->type == i)
                    enum_and(flags, ~(1 << i));

    igText("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        sel_efct->data.appear_time,
        sel_efct->data.life_time,
        sel_efct->data.start_time);

    igText("Parent Emitter:\nStart Time: %d; Life Time: %d\nLoop Start Time: %d; Loop End Time: %d",
        sel_emit->data.start_time,
        sel_emit->data.life_time,
        sel_emit->data.loop_start_time,
        sel_emit->data.loop_end_time);

    igSeparator();

    bool flags_changed = false;
    if (imguiCheckboxFlags_UintPtr("Loop",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_LOOP))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Emitter Local",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_EMITTER_LOCAL))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Scale Y by X",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_SCALE_Y_BY_X))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Rebound Plane",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_REBOUND_PLANE))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Rotate by Emitter",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_ROTATE_BY_EMITTER))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Scale by Emitter",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_SCALE))
        flags_changed = true;

    if (imguiCheckboxFlags_UintPtr("Texture Mask##Flag",
        (uint32_t*)&particle.data.flags,
        GLITTER_PARTICLE_TEXTURE_MASK))
        flags_changed = true;

    if (eg->type == GLITTER_X) {
        if (imguiCheckboxFlags_UintPtr("Disable Depth Test",
            (uint32_t*)&particle.data.flags,
            GLITTER_PARTICLE_DEPTH_TEST))
            flags_changed = true;

        if (particle.data.type == GLITTER_PARTICLE_LOCUS
            && imguiCheckboxFlags_UintPtr("Rotate Locus",
                (uint32_t*)&particle.data.flags,
                GLITTER_PARTICLE_ROTATE_LOCUS))
                flags_changed = true;
    }

    if (flags_changed)
        changed = true;

    igSeparator();

    if (eg->type == GLITTER_X) {
        vec2i life_time;
        life_time.x = particle.data.life_time;
        life_time.y = particle.data.life_time_random;
        if (imguiColumnDragVec2I("Life Time [R]",
            &life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle.data.life_time = life_time.x;
            particle.data.life_time_random = life_time.y;
            changed = true;
        }

        vec2i fade_in;
        fade_in.x = particle.data.fade_in;
        fade_in.y = particle.data.fade_in_random;
        if (imguiColumnDragVec2I("Fade In [R]",
            &fade_in, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle.data.fade_in = fade_in.x;
            particle.data.fade_in_random = fade_in.y;
            changed = true;
        }

        vec2i fade_out;
        fade_out.x = particle.data.fade_out;
        fade_out.y = particle.data.fade_out_random;
        if (imguiColumnDragVec2I("Fade Out [R]",
            &fade_out, 1.0f, 0x0000, 0x7FFF, "%d", 0)) {
            particle.data.fade_out = fade_out.x;
            particle.data.fade_out_random = fade_out.y;
            changed = true;
        }

        if (imguiColumnComboBox("Type", glitter_particle_name,
            GLITTER_PARTICLE_MESH,
            (int32_t*)&particle.data.type, 0, true, &data->imgui_focus))
            changed = true;
    }
    else {
        if (imguiColumnDragInt("Life Time",
            &particle.data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
            changed = true;

        if (imguiColumnComboBox("Type", glitter_particle_name,
            GLITTER_PARTICLE_LOCUS,
            (int32_t*)&particle.data.type, 0, true, &data->imgui_focus))
            changed = true;
    }

    if (particle.data.type != GLITTER_PARTICLE_MESH
        && imguiColumnComboBox("Pivot", glitter_pivot_name,
            GLITTER_PIVOT_BOTTOM_RIGHT,
            (int32_t*)&particle.data.pivot, 0, true, &data->imgui_focus))
        changed = true;

    if (particle.data.type == GLITTER_PARTICLE_QUAD || particle.data.type == GLITTER_PARTICLE_MESH) {
        glitter_particle_draw_type draw_type = glitter_particle_draw_type_default;
        glitter_particle_draw_type prev_draw_type;
        for (int32_t i = 0; i < glitter_particle_draw_types_count; i++)
            if (glitter_particle_draw_types[i] == particle.data.draw_type) {
                draw_type = (glitter_particle_draw_type)i;
                break;
            }
        prev_draw_type = draw_type;

        if (imguiColumnComboBox("Draw Type", glitter_particle_draw_type_name,
            GLITTER_PARTICLE_DRAW_TYPE_EMIT_POSITION,
            (int32_t*)&draw_type, 0, true, &data->imgui_focus))
            if (glitter_particle_draw_types[draw_type] != particle.data.draw_type) {
                if (eg->type != GLITTER_X) {
                    bool draw_prev_pos =
                        draw_type == GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION
                        || draw_type == GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION_DUP;
                    bool prev_draw_prev_pos =
                        prev_draw_type == GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION
                        || prev_draw_type == GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION_DUP;
                    bool draw_z_axis =
                        draw_type == GLITTER_PARTICLE_DRAW_TYPE_Z_AXIS;
                    bool prev_draw_z_axis =
                        prev_draw_type == GLITTER_PARTICLE_DRAW_TYPE_Z_AXIS;
                    if (draw_prev_pos && !prev_draw_prev_pos)
                        particle.data.rotation.z += (float_t)M_PI;
                    else if (!draw_prev_pos && prev_draw_prev_pos)
                        particle.data.rotation.z -= (float_t)M_PI;
                    else if (draw_z_axis && !prev_draw_z_axis) {
                        if (~flags & GLITTER_CURVE_TYPE_ROTATION_Z)
                            glitter_animation_add_value(eg->type, &particle.animation,
                                (float_t)M_PI_2, GLITTER_CURVE_TYPE_ROTATION_Z);
                        particle.data.rotation.z += (float_t)M_PI_2;
                    }
                    else if (!draw_z_axis && prev_draw_z_axis) {
                        if (~flags & GLITTER_CURVE_TYPE_ROTATION_Z)
                            glitter_animation_add_value(eg->type, &particle.animation,
                                (float_t)-M_PI_2, GLITTER_CURVE_TYPE_ROTATION_Z);
                        particle.data.rotation.z -= (float_t)M_PI_2;
                    }
                }
                particle.data.draw_type = glitter_particle_draw_types[draw_type];
                changed = true;
            }
    }

    if (particle.data.type == GLITTER_PARTICLE_QUAD)
        if (imguiColumnDragFloat("Z Offset",
            &particle.data.z_offset, 0.0001f, -FLT_MAX, FLT_MAX,
            "%g", ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

    igSeparator();

    if (eg->type != GLITTER_X) {
        bool draw_prev_pos =
            particle.data.draw_type == GLITTER_DIRECTION_PREV_POSITION
            || particle.data.draw_type == GLITTER_DIRECTION_PREV_POSITION_DUP;
        bool draw_z_axis =
            particle.data.draw_type == GLITTER_DIRECTION_Z_AXIS;

        vec3 rotation = particle.data.rotation;
        if (draw_prev_pos)
            rotation.z -= (float_t)M_PI;
        else if (draw_z_axis)
            rotation.z -= (float_t)M_PI_2;
        vec3_mult_scalar(rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, rotation);
            if (draw_prev_pos)
                rotation.z += (float_t)M_PI;
            else if (draw_z_axis)
                rotation.z += (float_t)M_PI_2;
            particle.data.rotation = rotation;
            changed = true;
        }
    }
    else {
        vec3 rotation;
        vec3_mult_scalar(particle.data.rotation, RAD_TO_DEG_FLOAT, rotation);
        if (imguiColumnDragVec3Flag("Rotation",
            &rotation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat,
            (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
            vec3_mult_scalar(rotation, DEG_TO_RAD_FLOAT, particle.data.rotation);
            changed = true;
        }
    }

    vec3 rotation_random;
    vec3_mult_scalar(particle.data.rotation_random, RAD_TO_DEG_FLOAT, rotation_random);
    if (imguiColumnDragVec3Flag("Rotation Random",
        &rotation_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_random, DEG_TO_RAD_FLOAT, particle.data.rotation_random);
        changed = true;
    }

    vec3 rotation_add;
    vec3_mult_scalar(particle.data.rotation_add, RAD_TO_DEG_FLOAT, rotation_add);
    if (imguiColumnDragVec3Flag("Rotation Add",
        &rotation_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add, DEG_TO_RAD_FLOAT, particle.data.rotation_add);
        changed = true;
    }

    vec3 rotation_add_random;
    vec3_mult_scalar(particle.data.rotation_add_random, RAD_TO_DEG_FLOAT, rotation_add_random);
    if (imguiColumnDragVec3Flag("Rotation Add Random",
        &rotation_add_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ) >> 3)) {
        vec3_mult_scalar(rotation_add_random, DEG_TO_RAD_FLOAT, particle.data.rotation_add_random);
        changed = true;
    }

    imguiDisableElementPush(particle.data.type != GLITTER_PARTICLE_MESH);
    if (imguiColumnDragVec2Flag("Scale",
        (vec2*)&particle.data.scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle.data.flags & GLITTER_PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;

    if (imguiColumnDragVec2Flag("Scale Random",
        (vec2*)&particle.data.scale_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        particle.data.flags & GLITTER_PARTICLE_SCALE_Y_BY_X ? 0x01 : 0x03))
        changed = true;
    imguiDisableElementPop(particle.data.type != GLITTER_PARTICLE_MESH);

    igSeparator();

    if (imguiColumnDragVec3("Direction",
        &particle.data.direction, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Direction Random",
        &particle.data.direction_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 speed;
    speed.x = particle.data.speed;
    speed.y = particle.data.speed_random;
    if (imguiColumnSliderVec2("Speed [R]",
        &speed, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat, false)) {
        particle.data.speed = speed.x;
        particle.data.speed_random = speed.y;
        changed = true;
    }

    vec2 deceleration;
    deceleration.x = particle.data.deceleration;
    deceleration.y = particle.data.deceleration_random;
    if (imguiColumnDragVec2("Deceleration [R]",
        &deceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle.data.deceleration = deceleration.x;
        particle.data.deceleration_random = deceleration.y;
        changed = true;
    }

    if (imguiColumnDragVec3("Gravity",
        &particle.data.gravity, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Acceleration",
        &particle.data.acceleration, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragVec3("Acceleration Random",
        &particle.data.acceleration_random, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    vec2 reflection_coeff;
    reflection_coeff.x = particle.data.reflection_coeff;
    reflection_coeff.y = particle.data.reflection_coeff_random;
    if (imguiColumnDragVec2("Refl Coeff [R]",
        &reflection_coeff, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        particle.data.reflection_coeff = reflection_coeff.x;
        particle.data.reflection_coeff_random = reflection_coeff.y;
        changed = true;
    }

    if (imguiColumnDragFloat("Rebound Plane Y",
        &particle.data.rebound_plane_y, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    igSeparator();

    vec4 color = particle.data.color;
    if (imguiColumnColorEdit4("Color", &color,
        ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
        changed = true;
    particle.data.color = color;

    if (imguiColumnComboBox("UV Index Type", glitter_uv_index_type_name,
        GLITTER_UV_INDEX_USER,
        (int32_t*)&particle.data.uv_index_type, 0, true, &data->imgui_focus))
        changed = true;

    vec2i split;
    split.x = particle.data.split_u;
    split.y = particle.data.split_v;
    if (imguiColumnSliderLogVec2I("UV Split",
        &split, 1, 128, "%d", 0)) {
        particle.data.split_u = (uint8_t)split.x;
        particle.data.split_v = (uint8_t)split.y;

        particle.data.split_uv.x = 1.0f / (float_t)particle.data.split_u;
        particle.data.split_uv.y = 1.0f / (float_t)particle.data.split_v;
        changed = true;
    }

    int32_t uv_max_count = (int32_t)(particle.data.split_u * particle.data.split_v);
    if (uv_max_count)
        uv_max_count--;

    if (imguiColumnDragInt("Frame Step UV",
        &particle.data.frame_step_uv, 1.0f, 0x0000, 0x7FFF, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index Start",
        &particle.data.uv_index_start, 1.0f, 0, uv_max_count, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index End",
        &particle.data.uv_index_end, 1.0f, particle.data.uv_index_start + 1, uv_max_count + 1, "%d", 0))
        changed = true;

    if (imguiColumnDragInt("UV Index",
        &particle.data.uv_index, 1.0f, particle.data.uv_index_start,
        particle.data.uv_index_end - 1, "%d", 0))
        changed = true;

    if (imguiColumnDragVec2("UV Scroll Add",
        &particle.data.uv_scroll_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (imguiColumnDragFloat("UV Scroll Add Scale",
        &particle.data.uv_scroll_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat))
        changed = true;

    if (particle.data.type == GLITTER_PARTICLE_MESH) {
        bool uv_2nd_add = particle.data.sub_flags & GLITTER_PARTICLE_SUB_UV_2ND_ADD ? true : false;
        imguiDisableElementPush(uv_2nd_add);
        if (imguiColumnDragVec2("UV Scroll 2nd Add",
            &particle.data.uv_scroll_2nd_add, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;

        if (imguiColumnDragFloat("UV Scroll 2nd Add Scale",
            &particle.data.uv_scroll_2nd_add_scale, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
            ImGuiSliderFlags_NoRoundToFormat))
            changed = true;
        imguiDisableElementPop(uv_2nd_add);
    }

    if (particle.data.type != GLITTER_PARTICLE_MESH) {
        int32_t idx0 = 0;
        int32_t idx1 = 0;
        glitter_effect_group* eg = glt_edt->effect_group;
        size_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.data();
        for (size_t i = 0; i < rc && (!idx0 || !idx1); i++) {
            if (idx0 == 0 && rh[i] == particle.data.tex_hash)
                idx0 = (int32_t)(i + 1);

            if (idx1 == 0 && rh[i] == particle.data.mask_tex_hash)
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
            &particle, &particle.data.texture, &particle.data.tex_hash,
            idx0, &tex0_anim, &tex0_frame, &tex0_index, &tex0_tex))
            changed = true;

        if (particle.data.flags & GLITTER_PARTICLE_TEXTURE_MASK) {
            static bool tex11_anim;
            static int32_t tex1_frame;
            static int32_t tex1_index;
            static int32_t tex1_tex;
            if (glitter_editor_property_particle_texture(glt_edt, data, "Texture Mask", texture_name_list,
                &particle, &particle.data.mask_texture, &particle.data.mask_tex_hash,
                idx1, &tex11_anim, &tex1_frame, &tex1_index, &tex1_tex))
                changed = true;
        }
        free(texture_names);
        free(texture_name_list);

        glitter_particle_blend_draw blend_draw = glitter_particle_blend_draw_default;
        for (int32_t i = 0; i < glitter_particle_blend_mask_types_count; i++)
            if (glitter_particle_blend_draw_types[i] == particle.data.blend_mode) {
                blend_draw = (glitter_particle_blend_draw)i;
                break;
            }

        if (imguiColumnComboBox("Blend Mode", glitter_particle_blend_draw_name,
            GLITTER_PARTICLE_BLEND_DRAW_MULTIPLY,
            (int32_t*)&blend_draw, 0, true, &data->imgui_focus))
            if (glitter_particle_blend_draw_types[blend_draw] != particle.data.blend_mode) {
                particle.data.blend_mode = glitter_particle_blend_draw_types[blend_draw];
                changed = true;
            }

        if (particle.data.flags & GLITTER_PARTICLE_TEXTURE_MASK) {
            glitter_particle_blend_mask blend_mask = glitter_particle_blend_mask_default;
            for (int32_t i = 0; i < glitter_particle_blend_mask_types_count; i++)
                if (glitter_particle_blend_mask_types[i] == particle.data.mask_blend_mode) {
                    blend_mask = (glitter_particle_blend_mask)i;
                    break;
                }

            if (imguiColumnComboBox("Mask Blend Mode", glitter_particle_blend_mask_name,
                GLITTER_PARTICLE_BLEND_MASK_MULTIPLY,
                (int32_t*)&blend_mask, 0, true, &data->imgui_focus))
                if (glitter_particle_blend_mask_types[blend_mask] != particle.data.mask_blend_mode) {
                    particle.data.mask_blend_mode = glitter_particle_blend_mask_types[blend_mask];
                    changed = true;
                }
        }
    }

    switch (particle.data.uv_index_type) {
    case GLITTER_UV_INDEX_FIXED:
    case GLITTER_UV_INDEX_RANDOM:
    case GLITTER_UV_INDEX_FORWARD:
    case GLITTER_UV_INDEX_REVERSE:
    case GLITTER_UV_INDEX_USER:
        particle.data.uv_index &= uv_max_count;
        break;
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
        if (particle.data.uv_index < particle.data.uv_index_start)
            particle.data.uv_index = particle.data.uv_index_start;
        else if (particle.data.uv_index_end >= 0 && particle.data.uv_index > particle.data.uv_index_end)
            particle.data.uv_index = particle.data.uv_index_end;

        particle.data.uv_index = min(particle.data.uv_index, uv_max_count);
        break;
    }

    if (particle.data.uv_index_end >= 0)
        particle.data.uv_index_count = particle.data.uv_index_end - particle.data.uv_index_start + 1;
    else
        particle.data.uv_index_count = uv_max_count - particle.data.uv_index_start;

    if (particle.data.uv_index_count < 0)
        particle.data.uv_index_count = 0;

    if (imguiCheckboxFlags_UintPtr("Use Animation Curve",
        (uint32_t*)&particle.data.sub_flags,
        GLITTER_PARTICLE_SUB_USE_CURVE))
        changed = true;

    if (eg->type == GLITTER_X && particle.data.type == GLITTER_PARTICLE_MESH)
        if (imguiCheckboxFlags_UintPtr("UV Add",
            (uint32_t*)&particle.data.sub_flags,
            GLITTER_PARTICLE_SUB_UV_2ND_ADD))
            changed = true;

    igSeparator();

    if (eg->type != GLITTER_X || particle.data.type == GLITTER_PARTICLE_QUAD)
        if (imguiColumnDragInt("Count",
            &particle.data.count, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

    if (imguiColumnDragFloat("Emission",
        &particle.data.emission, 0.0001f, 0.0f, HALF_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
        if (sel_efct->data.flags & GLITTER_EFFECT_EMISSION
            || particle.data.emission >= glitter_min_emission)
            enum_or(particle.data.flags, GLITTER_PARTICLE_EMISSION);
        else
            enum_and(particle.data.flags, ~GLITTER_PARTICLE_EMISSION);
        changed = true;
    }

    if (imguiColumnDragInt("Unk 0",
        &particle.data.unk0, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (eg->version >= 7 && imguiColumnDragInt("Unk 1",
        &particle.data.unk1, 1.0f, INT_MIN, INT_MAX, "%d", 0))
        changed = true;

    if (particle.data.type == GLITTER_PARTICLE_LOCUS) {
        if (imguiColumnDragInt("Locus Hist Size",
            &particle.data.locus_history_size, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;

        if (imguiColumnDragInt("Locus Hist Size Rand",
            &particle.data.locus_history_size_random, 1.0f, 0x0000, 0x7FFFFF, "%d", 0))
            changed = true;
    }
    else if (particle.data.type == GLITTER_PARTICLE_MESH) {
        igSeparator();

        glitter_particle_mesh* mesh = &particle.data.mesh;
        uint32_t set_id = (uint32_t)mesh->object_set_name_hash;
        uint32_t obj_id = (uint32_t)mesh->object_name_hash;

        object_set* set = object_storage_get_object_set(set_id);
        ssize_t object_set_count = object_storage_get_object_set_count();

        imguiStartPropertyColumn("Object Set");
        if (igBeginCombo("##Object Set", set ? string_data(&set->name) : "None", 0)) {
            ssize_t set_index = set ? object_storage_get_object_set_index(set_id) : -1;

            igPushID_Int(-1);
            if (igSelectable_Bool("None", !set, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true))
                set_index = -1;
            igPopID();

            for (ssize_t i = 0; i < object_set_count; i++) {
                igPushID_Int((int32_t)i);
                object_set* set = object_storage_get_object_set_by_index(i);
                if (igSelectable_Bool(string_data(&set->name), set->id == set_id, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && set->id != set_id))
                    set_index = i;
                igPopID();
            }

            if (set_index == -1)
                set_id = -1;
            else
                set_id = object_storage_get_object_set_by_index(set_index)->id;

            if (mesh->object_set_name_hash != set_id) {
                mesh->object_set_name_hash = set_id;
                mesh->object_name_hash = -1;
                changed = true;
            }

            data->imgui_focus |= true;
            igEndCombo();
        }
        imguiEndPropertyColumn();

        imguiDisableElementPush(set_id != -1);
        object* obj = 0;
        if (set)
            for (int32_t i = 0; i < set->objects_count; i++)
                if (set->objects[i].id == obj_id) {
                    obj = &set->objects[i];
                    break;
                }

        imguiStartPropertyColumn("Object");
        if (igBeginCombo("##Object", obj ? string_data(&obj->name) : "None", 0)) {
            if (set && set_id != -1 && set->id == set_id) {
                ssize_t obj_index = -1;
                for (int32_t i = 0; i < set->objects_count; i++)
                    if (set->objects[i].id == obj_id) {
                        obj_index = i;
                        break;
                    }

                igPushID_Int(-1);
                if (igSelectable_Bool("None", !obj, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && obj))
                    obj_index = -1;
                igPopID();

                for (int32_t i = 0; i < set->objects_count; i++) {
                    igPushID_Int(i);
                    object* obj = &set->objects[i];
                    if (igSelectable_Bool(string_data(&obj->name), obj->id == obj_id, 0, ImVec2_Empty)
                        || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                        || (igIsItemFocused() && obj->id != obj_id))
                        obj_index = i;
                    igPopID();
                }

                if (obj_index == -1 || obj_index >= set->objects_count)
                    obj_id = -1;
                else
                    obj_id = set->objects[obj_index].id;

                if (mesh->object_name_hash != obj_id) {
                    mesh->object_name_hash = obj_id;
                    changed = true;
                }
            }

            data->imgui_focus |= true;
            igEndCombo();
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

    uint64_t hash1 = hash_fnv1a64m(glt_edt->selected_particle, sizeof(glitter_particle), false);
    uint64_t hash2 = hash_fnv1a64m(&particle, sizeof(glitter_particle), false);
    if (hash1 != hash2 && changed) {
        glt_edt->input_reload = true;
        *glt_edt->selected_particle = particle;
    }

    imguiSetDefaultColumnSpace();
}

static bool glitter_editor_property_particle_texture(glitter_editor_struct* glt_edt,
    class_data* data, const char* label, char** items, glitter_particle* particle,
    int32_t* tex, uint64_t* tex_hash, int32_t tex_idx, bool* tex_anim,
    int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex) {
    glitter_effect_group* eg = glt_edt->effect_group;
    size_t rc = eg->resources_count;
    texture** r = eg->resources;
    uint64_t* rh = eg->resource_hashes.data();
    txp* rt = eg->resources_tex.begin;

    const uint64_t empty_hash = eg->type != GLITTER_FT
        ? hash_murmurhash_empty : hash_fnv1a64m_empty;

    ImVec2 uv_min;
    ImVec2 uv_max;

    imguiStartPropertyColumn(label);
    int32_t prev_tex_idx = tex_idx;
    if (igBeginCombo("", items[tex_idx], 0)) {
        int32_t uv_index = *tex_index;
        if (!*tex_anim) {
            *tex_anim = true;
            *tex_frame = 0;
            *tex_tex = tex_idx;

            uv_index = particle->data.uv_index;
            int32_t max_uv = particle->data.split_u * particle->data.split_v;
            if (max_uv > 1 && particle->data.uv_index_count > 1) {
                switch (particle->data.uv_index_type) {
                case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
                case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
                case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE: {
                    LARGE_INTEGER time;
                    QueryPerformanceCounter(&time);
                    uv_index += time.LowPart % particle->data.uv_index_count;
                } break;
                }
            }
        }
        else if (particle->data.uv_index_type == GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED) {
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
            case GLITTER_UV_INDEX_RANDOM:
                uv_index = particle->data.uv_index_start;
                if (particle->data.uv_index_count > 1) {
                    LARGE_INTEGER time;
                    QueryPerformanceCounter(&time);
                    uv_index += time.LowPart % particle->data.uv_index_count;
                }
                break;
            case GLITTER_UV_INDEX_FORWARD:
            case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
                uv_index = (uint8_t)(max_uv & (uv_index + 1));
                break;
            case GLITTER_UV_INDEX_REVERSE:
            case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
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
            igPushID_Int(n);
            if (igSelectable_Bool(items[n], tex_idx == n, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && tex_idx != n))
                tex_idx = n;

            if (igIsItemHovered(0) && n) {
                if (*tex_tex != n) {
                    *tex_anim = true;
                    *tex_frame = 0;
                    *tex_tex = n;

                    uv_index = particle->data.uv_index;
                    int32_t max_uv = particle->data.split_u * particle->data.split_v;
                    if (max_uv > 1) {
                        switch (particle->data.uv_index_type) {
                        case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
                        case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
                        case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE: {
                            LARGE_INTEGER time;
                            QueryPerformanceCounter(&time);
                            uv_index += time.LowPart % particle->data.uv_index_count;
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


                txp_mipmap* rtm = rt[n - 1].data.begin;
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

                igBeginTooltip();
                igText("Frame: %d\nUV Index %d", *tex_frame, *tex_index);
                igText("Tex Size: %dx%d", rtm->width, rtm->height);
                igImage((void*)(size_t)r[n - 1]->texture, size1,
                    ImVec2_Empty, ImVec2_Identity, tint_col, border_col);
                igText("Preview Tex Size: %gx%g",
                    particle->data.split_u > 1
                    ? (float_t)rtm->width / (float_t)particle->data.split_u
                    : (float_t)rtm->width,
                    particle->data.split_u > 1
                    ? (float_t)rtm->height / (float_t)particle->data.split_v
                    : (float_t)rtm->height);
                igImage((void*)(size_t)r[n - 1]->texture, size2, uv_min, uv_max, tint_col, border_col);
                igEndTooltip();
            }

            if (tex_idx == n)
                igSetItemDefaultFocus();
            igPopID();
        }

        data->imgui_focus |= true;
        igEndCombo();
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
            *tex = r[tex_idx - 1]->texture;
            *tex_hash = rh[tex_idx - 1];
        }
        else {
            *tex = 0;
            *tex_hash = empty_hash;
        }
    return res;
}

static void glitter_editor_popups(glitter_editor_struct* glt_edt, class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    const float_t title_bar_size = font->FontSize + style->ItemSpacing.y + style->FramePadding.y * 2.0f;

    igPushID_Str("Glitter Editor Glitter File Create Pop-up");
    glitter_editor_file_create_popup(glt_edt, data, io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Load Pop-up");
    glitter_editor_file_load_popup(glt_edt, data, io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Load Pop-up");
    glitter_editor_file_load_model_popup(glt_edt, data, io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Load Error List Pop-up");
    glitter_editor_file_load_error_list_popup(glt_edt, data, io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Save");
    glitter_editor_file_save_popup(glt_edt, data, io, style, font, title_bar_size);
    igPopID();
}

static void glitter_editor_file_create_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->create_popup) {
        igOpenPopup_Str("Create Glitter File as...", 0);
        glt_edt->create_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    glitter_effect_group* eg = glt_edt->effect_group;

    ImGuiWindowFlags window_flags;

    const float_t button_width = 70.0f;

    win_x = button_width * 2.0f + style->ItemSpacing.x * 1.0f;
    win_y = title_bar_size + font->FontSize + style->ItemSpacing.y * 1.0f + style->FramePadding.y * 2.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Create Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos({ x, y });
        igGetContentRegionAvail(&t);
        if (igBeginTable("buttons", 2, 0, ImVec2_Empty, 0.0f)) {
            bool close = false;

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("F2", t)) {
                glt_edt->load_glt_type = GLITTER_F2;
                close = true;
            }

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("FT", t)) {
                glt_edt->load_glt_type = GLITTER_FT;
                close = true;
            }

            /*igTableNextColumn();
            igGetContentRegionAvail(&t);
            if (imguiButton("X", t)) {
                glt_edt->load_glt_type = GLITTER_X;
                close = true;
            }*/

            if (close) {
                glt_edt->effect_group_add = true;
                enum_or(glt_edt->effect_flags, GLITTER_EDITOR_ADD);
                igCloseCurrentPopup();
            }
            igEndTable();
        }
        data->imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_load_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->load_popup) {
        igOpenPopup_Str("Load Glitter File as...", 0);
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

    win_x = button_width * 3.0f + style->ItemSpacing.x * 2.0f;
    win_y = title_bar_size + font->FontSize + style->ItemSpacing.y * 2.0f + style->FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Load Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos({ x, y });
        igGetContentRegionAvail(&t);
        if (igBeginTable("buttons", 3, 0, ImVec2_Empty, 0.0f)) {
            bool close = false;

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("F2", t)) {
                glt_edt->load_glt_type = GLITTER_F2;
                close = true;
            }

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("FT", t)) {
                glt_edt->load_glt_type = GLITTER_FT;
                close = true;
            }

            igTableNextColumn();
            igGetContentRegionAvail(&t);
            if (imguiButton("X", t)) {
                glt_edt->load_glt_type = GLITTER_X;
                close = true;
            }

            if (close) {
                glt_edt->load = true;
                igCloseCurrentPopup();
            }
            igEndTable();
        }
        data->imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_load_model_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
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
    case GLITTER_F2:
        if (data_f2le_enable)
            data_count++;
        if (data_f2be_enable)
            data_count++;
        break;
    case GLITTER_FT:
        if (data_aft_enable)
            data_count++;
        if (data_ft_enable)
            data_count++;
        if (data_m39_enable)
            data_count++;
        break;
    case GLITTER_X:
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
            igOpenPopup_Str("Which data should be loaded?", 0);
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
        win_x = button_width * 3.0f + style->ItemSpacing.x * 2.0f;
    }
    else if (data_count == 2) {
        button_width = 105.0f;
        win_x = button_width * 2.0f + style->ItemSpacing.x;
    }
    else {
        button_width = 210.0f;
        win_x = button_width;
    }
    win_y = title_bar_size + font->FontSize + style->ItemSpacing.y * 2.0f + style->FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Which data should be loaded?", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos({ x, y });
        igGetContentRegionAvail(&t);
        if (igBeginTable("buttons", data_count, 0, ImVec2_Empty, 0.0f)) {
            bool close = false;

            switch (glt_edt->effect_group->type) {
            case GLITTER_F2:
                if (data_f2le_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("F2LE", t)) {
                        glt_edt->load_data_type = DATA_F2LE;
                        close = true;
                    }
                }

                if (data_f2be_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("F2BE", t)) {
                        glt_edt->load_data_type = DATA_F2BE;
                        close = true;
                    }
                }
                break;
            case GLITTER_FT:
                if (data_aft_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("AFT", t)) {
                        glt_edt->load_data_type = DATA_AFT;
                        close = true;
                    }
                }

                if (data_ft_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("FT", t)) {
                        glt_edt->load_data_type = DATA_FT;
                        close = true;
                    }
                }

                if (data_m39_enable) {
                    igTableNextColumn();
                    igGetContentRegionAvail(&t);
                    if (imguiButton("M39", t)) {
                        glt_edt->load_data_type = DATA_M39;
                        close = true;
                    }
                }
                break;
            case GLITTER_X:
                if (data_x_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("X", t)) {
                        glt_edt->load_data_type = DATA_X;
                        close = true;
                    }
                }

                if (data_xhd_enable) {
                    igTableNextColumn();;
                    igGetContentRegionAvail(&t);
                    if (imguiButton("XHD", t)) {
                        glt_edt->load_data_type = DATA_XHD;
                        close = true;
                    }
                }

                if (data_vrfl_enable) {
                    igTableNextColumn();
                    igGetContentRegionAvail(&t);
                    if (imguiButton("VRFL", t)) {
                        glt_edt->load_data_type = DATA_VRFL;
                        close = true;
                    }
                }
                break;
            }


            if (close) {
                glt_edt->load_data = true;
                igCloseCurrentPopup();
            }
            igEndTable();
        }
        data->imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_load_error_list_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->load_error_list_popup) {
        igOpenPopup_Str("Glitter File Load Error", 0);
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
        + style->ItemSpacing.y * 3.0f + style->FramePadding.y * 2.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Glitter File Load Error", 0, window_flags)) {
        igSetCursorPosY(title_bar_size);
        igText("Can't find name in Glitter List File\n"
            "for hash in Glitter Effect File");

        ImVec2 t;
        igGetContentRegionAvail(&t);
        igSetCursorPosX(t.x * 0.5f - 20.0f);

        x = 40.0f;
        y = font->FontSize + style->FramePadding.y * 2.0f;
        if (imguiButton("OK", { x, y }))
            igCloseCurrentPopup();
        data->imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_save_popup(glitter_editor_struct* glt_edt, class_data* data,
    ImGuiIO* io, ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    if (glt_edt->save_popup) {
        igOpenPopup_Str("Save Glitter File as...", 0);
        glt_edt->save_popup = false;
    }

    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    glitter_effect_group* eg = glt_edt->effect_group;
    ImGuiWindowFlags window_flags;

    const float_t button_width = 70.0f;

    if (eg && eg->type != GLITTER_X)
        win_x = button_width * 4.0f + style->ItemSpacing.x * 3.0f;
    else
        win_x = button_width * 3.0f + style->ItemSpacing.x * 2.0f;
    win_y = title_bar_size + font->FontSize + style->ItemSpacing.y * 2.0f + style->FramePadding.y * 3.0f;

    x = (float_t)width * 0.5f - win_x * 0.5f;
    y = (float_t)height * 0.5f - win_y * 0.5f;
    w = win_x;
    h = win_y;

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    igSetNextWindowPos({ x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Save Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos({ x, y });
        igGetContentRegionAvail(&t);
        /*if (eg && eg->type == GLITTER_X) {
            if (igBeginTable("buttons", 3, 0, ImVec2_Empty, 0.0f)) {
                bool close = false;

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("X", t)) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = GLITTER_X;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("X HD", t)) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = GLITTER_X;
                    close = true;
                }

                igTableNextColumn();
                igGetContentRegionAvail(&t);
                if (imguiButton("None", t)) {
                    glt_edt->save = false;
                    glt_edt->save_compress = false;
                    close = true;
                }

                if (close)
                    igCloseCurrentPopup();
                igEndTable();
            }
        }
        else*/ {
            if (igBeginTable("buttons", 4, 0, ImVec2_Empty, 0.0f)) {
                bool close = false;

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("F2 PSV", t)) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = GLITTER_F2;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("F2 PS3", t)) {
                    glt_edt->save = true;
                    glt_edt->save_compress = true;
                    glt_edt->save_glt_type = GLITTER_F2;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("FT", t)) {
                    glt_edt->save = true;
                    glt_edt->save_compress = false;
                    glt_edt->save_glt_type = GLITTER_FT;
                    close = true;
                }

                igTableNextColumn();
                igGetContentRegionAvail(&t);
                if (imguiButton("None", t)) {
                    glt_edt->save = false;
                    glt_edt->save_compress = false;
                    close = true;
                }

                if (close)
                    igCloseCurrentPopup();
                igEndTable();
            }
        }
        data->imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
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
    glitter_emitter* sel_emit = glt_edt->selected_emitter;
    glitter_particle* sel_ptcl = glt_edt->selected_particle;
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    if (crv_edt->type < GLITTER_CURVE_TRANSLATION_X
        || crv_edt->type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
    case GLITTER_EDITOR_SELECTED_EMITTER:
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        break;
    default:
        return;
    }

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_curve* curve = &crv_edt->curve;
    vector_old_glitter_curve_key* keys = &curve->keys_rev;
    bool changed = false;

    bool fix_rot_z = eg->type != GLITTER_X && curve->type == GLITTER_CURVE_ROTATION_Z
        && ((glt_edt->selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && sel_emit->data.direction == GLITTER_DIRECTION_Z_AXIS)
            || (glt_edt->selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && sel_ptcl->data.draw_type == GLITTER_DIRECTION_Z_AXIS));

    bool add_key = false;
    bool del_key = false;
    glitter_editor_curve_editor_key_manager(glt_edt, keys, &add_key, &del_key);

    if (igIsWindowFocused(0)) {
        if (add_key && igIsKeyPressed(GLFW_KEY_INSERT, true))
            crv_edt->add_key = true;
        else if (del_key && igIsKeyPressed(GLFW_KEY_DELETE, true))
            crv_edt->del_key = true;
        else if (igIsKeyPressed(GLFW_KEY_RIGHT, true))
            crv_edt->frame++;
        else if (igIsKeyPressed(GLFW_KEY_LEFT, true))
            crv_edt->frame--;
    }

    bool exist = true;
    if (crv_edt->add_key && add_key) {
        if (!crv_edt->list[crv_edt->type] || vector_old_length(*keys) == 0) {
            glitter_curve_key* key = vector_old_glitter_curve_key_reserve_back(keys);
            key->frame = crv_edt->curve.start_time;
            key->value = glitter_editor_curve_editor_get_value(glt_edt, curve->type);

            if (!crv_edt->list[crv_edt->type]) {
                glitter_curve* curve = glitter_curve_init(eg->type);
                vector_old_ptr_glitter_curve_push_back(crv_edt->animation, &curve);
                crv_edt->list[crv_edt->type] = curve;
            }

            glitter_editor_curve_editor_curve_set(glt_edt, curve, crv_edt->type);
            *crv_edt->list[crv_edt->type] = crv_edt->curve;
            crv_edt->frame = crv_edt->curve.start_time;
            changed = true;
        }
        else {
            glitter_curve_key* i = keys->begin;
            bool is_before_start = keys->begin->frame > crv_edt->frame;
            bool has_key_after = false;
            if (!is_before_start)
                for (i++; i != keys->end; i++)
                    if (crv_edt->frame <= i->frame) {
                        has_key_after = keys->end - i > 0;
                        break;
                    }

            ssize_t pos = i - keys->begin;
            if (!is_before_start)
                pos--;

            glitter_curve_key key;
            memset(&key, 0, sizeof(glitter_curve_key));
            key.frame = crv_edt->frame;
            if (!is_before_start && has_key_after) {
                glitter_curve_key* c = i - 1;
                glitter_curve_key* n = i;
                key.type = c->type;
                switch (c->type) {
                case GLITTER_KEY_CONSTANT:
                    key.value = c->value;
                    key.random_range = c->random_range;
                    break;
                case GLITTER_KEY_LINEAR: {
                    float_t t = (float_t)(key.frame - c->frame) / (float_t)(n->frame - c->frame);
                    key.value = lerp(c->value, n->value, t);
                    key.random_range = lerp(c->random_range, n->random_range, t);
                } break;
                case GLITTER_KEY_HERMITE: {
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
                vector_old_glitter_curve_key_insert(keys, ++pos, &key);
            }
            else if (is_before_start) {
                glitter_curve_key* n = i;
                key.type = n->type;
                key.value = n->value;
                key.random_range = n->random_range;
                vector_old_glitter_curve_key_insert(keys, pos, &key);
            }
            else {
                glitter_curve_key* c = i - 1;
                key.type = c->type;
                key.value = c->value;
                key.random_range = c->random_range;
                vector_old_glitter_curve_key_insert(keys, ++pos, &key);
            }
            *crv_edt->list[crv_edt->type] = *curve;
            changed = true;
        }
    }
    else if (crv_edt->del_key && del_key) {
        for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
            if (i == crv_edt->key) {
                bool has_key_before = keys->begin->frame < i->frame;
                bool has_key_after = keys->end[-1].frame > i->frame;
                if (has_key_before && i[-1].type == GLITTER_KEY_HERMITE && has_key_after) {
                    glitter_curve_key* c = i - 1;
                    glitter_curve_key* n = i + 1;
                    int32_t df_c = i->frame - c->frame;
                    int32_t df_n = n->frame - i->frame;

                    float_t* v_arr_c = 0;
                    size_t v_length_c = 0;
                    interpolate_chs(c->value, i->value, c->tangent2,
                        i->tangent1, 0, df_c, &v_arr_c, &v_length_c);

                    float_t* v_arr_n = 0;
                    size_t v_length_n = 0;
                    if (i->type == GLITTER_KEY_HERMITE)
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
                vector_old_glitter_curve_key_erase(keys, i - keys->begin, 0);
                changed = true;
                break;
            }

        glitter_curve* c = crv_edt->list[crv_edt->type];
        if (c && vector_old_length(*keys) == 0) {
            glitter_animation* anim = crv_edt->animation;
            for (glitter_curve** i = anim->begin; i != anim->end; i++)
                if (*i && *i == c) {
                    vector_old_ptr_glitter_curve_erase(anim, i - anim->begin, glitter_curve_dispose);
                    exist = false;
                    break;
                }

            glitter_editor_curve_editor_curve_reset(glt_edt, curve);
            crv_edt->list[crv_edt->type] = 0;
            changed = true;
        }
    }
    else if (crv_edt->add_curve) {
        glitter_curve_key* key = vector_old_glitter_curve_key_reserve_back(keys);
        key->frame = crv_edt->curve.start_time;
        key->value = glitter_editor_curve_editor_get_value(glt_edt, curve->type);

        glitter_curve* c = glitter_curve_init(eg->type);
        vector_old_ptr_glitter_curve_push_back(crv_edt->animation, &c);
        crv_edt->list[crv_edt->type] = c;
        glitter_editor_curve_editor_curve_set(glt_edt, curve, crv_edt->type);
        *crv_edt->list[crv_edt->type] = *curve;
        crv_edt->frame = curve->start_time;
        changed = true;
    }
    else if (crv_edt->del_curve) {
        glitter_curve* c = crv_edt->list[crv_edt->type];
        glitter_animation* anim = crv_edt->animation;
        for (glitter_curve** i = anim->begin; i != anim->end; i++)
            if (*i && *i == c) {
                vector_old_ptr_glitter_curve_erase(anim, i - anim->begin, glitter_curve_dispose);
                break;
            }

        glitter_editor_curve_editor_curve_reset(glt_edt, curve);
        crv_edt->list[crv_edt->type] = 0;
        crv_edt->frame = 0;
        changed = true;
    }
    crv_edt->add_key = false;
    crv_edt->del_key = false;
    crv_edt->add_curve = false;
    crv_edt->del_curve = false;

    int32_t start_time = curve->start_time;
    int32_t end_time = curve->end_time;

    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    ImVec2 cont_reg_avail;
    igGetContentRegionAvail(&cont_reg_avail);

    ImVec2 canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max;
    crv_edt->draw_list = igGetWindowDrawList();
    canvas_size = cont_reg_avail;
    igGetCursorScreenPos(&canvas_pos);
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

    igItemSize_Rect(boundaries, -1.0f);
    bool hovered = igIsMouseHoveringRect(boundaries.Min, boundaries.Max, true);
    igRenderFrame(boundaries.Min, boundaries.Max, igGetColorU32_Col(ImGuiCol_FrameBg, 1.0f), true, 1.0f);

    static bool can_drag;
    if (hovered && igIsMouseClicked(ImGuiMouseButton_Left, false))
        can_drag = true;

    if (can_drag && (igIsKeyDown(GLFW_KEY_LEFT_SHIFT) || igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))) {
        crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt, curve);
        crv_edt->frame = crv_edt->key->frame;
    }
    else
        crv_edt->key = glitter_editor_curve_editor_get_selected_key(glt_edt, curve);

    if (can_drag && igIsMouseReleased(ImGuiMouseButton_Left))
        can_drag = false;

    float_t timeline_max_offset = (end_time - start_time) * frame_width;
    timeline_max_offset = max(timeline_max_offset, 0.0f) - curve_editor_timeline_base_pos;

    ImDrawList_PushClipRect(crv_edt->draw_list, boundaries.Min, boundaries.Max, true);

    int32_t start = (int32_t)(crv_edt->timeline_pos / frame_width) + start_time;
    int32_t end = (int32_t)((crv_edt->timeline_pos + canvas_size.x) / frame_width) + start_time;

    ImU32 line_color_valid_bold = igGetColorU32_Vec4({ 0.5f, 0.5f, 0.5f, 0.75f });
    ImU32 line_color_valid = igGetColorU32_Vec4({ 0.4f, 0.4f, 0.4f, 0.45f });
    ImU32 line_color_not_valid = igGetColorU32_Vec4({ 0.2f, 0.2f, 0.2f, 0.75f });
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
        ImDrawList_AddLine(crv_edt->draw_list,
            { x, y - canvas_size.y }, { x, y }, line_color, 0.80f);

        if (bold_frame) {
            char buf[0x20];
            snprintf(buf, sizeof(buf), "%d", frame);
            ImDrawList_AddText_Vec2(crv_edt->draw_list,
                { x + 3, canvas_pos.y }, line_color, buf, 0);
        }
    }

    float_t max = crv_edt->range * (1.0f / crv_edt->zoom_value);
    float_t min;
    if (curve->type == GLITTER_CURVE_EMISSION_INTERVAL)
        min = -1.0f;
    else if (curve->type == GLITTER_CURVE_PARTICLES_PER_EMISSION)
        min = 0.0f;
    else
        min = -max;

    for (int32_t j = 0; j < 5; j++) {
        float_t x_pos = canvas_pos_min.x;
        float_t y_pos = canvas_pos.y + j * 0.25f * (canvas_size.y
            - crv_edt->height_offset) + crv_edt->height_offset;

        ImU32 line_color = j % 2 == 1
            ? igGetColorU32_Vec4({ 0.35f, 0.35f, 0.35f, 0.45f })
            : igGetColorU32_Vec4({ 0.45f, 0.45f, 0.45f, 0.75f });
        bool last = j == 4;
        if (j && !last)
            ImDrawList_AddLine(crv_edt->draw_list, { x_pos, y_pos },
                { x_pos + canvas_size.x, y_pos }, line_color, 0.80f);

        float_t val = max - j * 0.25f * (max - min);
        char buf[0x20];
        snprintf(buf, sizeof(buf), "%.2f", val);
        ImDrawList_AddText_Vec2(crv_edt->draw_list, { x_pos + 2, y_pos + (last ? -16 : -2) },
            igGetColorU32_Vec4({ 0.65f, 0.65f, 0.65f, 0.85f }), buf, 0);
    }

    if (curve->type >= GLITTER_CURVE_ROTATION_X && curve->type <= GLITTER_CURVE_ROTATION_Z) {
        max *= DEG_TO_RAD_FLOAT;
        min *= DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= GLITTER_CURVE_COLOR_R && curve->type <= GLITTER_CURVE_COLOR_RGB_SCALE_2ND) {
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
    ImDrawList_AddLine(crv_edt->draw_list, p1, p2,
        igGetColorU32_Vec4({ 0.5f, 0.5f, 0.5f, 0.85f }), 2.0f);

    ImU32 random_range_color = igGetColorU32_Vec4({ 0.5f, 0.5f, 0.0f, 0.25f });
    ImU32 key_random_range_color = igGetColorU32_Vec4({ 0.5f, 0.0f, 0.5f, 0.25f });
    ImU32 default_color = igGetColorU32_Vec4({ 1.0f, 1.0f, 1.0f, 0.75f });
    ImU32 selected_color = igGetColorU32_Vec4({ 0.0f, 1.0f, 1.0f, 0.75f });

    if (curve->flags & GLITTER_CURVE_RANDOM_RANGE
        || curve->flags & GLITTER_CURVE_KEY_RANDOM_RANGE) {
        float_t random_range = curve->random_range;
        bool random_range_mult = curve->flags & GLITTER_CURVE_RANDOM_RANGE_MULT;
        bool random_range_negate = curve->flags & GLITTER_CURVE_RANDOM_RANGE_NEGATE;
        bool glt_type_ft = eg->type == GLITTER_FT;
        for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
            if (i->frame >= end_time || keys->end - i <= 1)
                break;

            glitter_curve_key* c = i;
            glitter_curve_key* n = i + 1;
            float_t x1 = canvas_pos.x + (float_t)(c->frame - start_time) * frame_width;
            float_t x2 = canvas_pos.x + (float_t)(n->frame - start_time) * frame_width;

            if (((x1 < canvas_pos_min.x) && (x2 < canvas_pos_min.x)) ||
                (x1 > canvas_pos_max.x) && (x2 > canvas_pos_max.x))
                continue;

            x1 = max(x1, canvas_pos_min.x);
            x2 = min(x2, canvas_pos_max.x);

            glitter_key_type k_type = c->type;
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
            if (k_type == GLITTER_KEY_CONSTANT) {
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
                        ImDrawList_AddRectFilled(crv_edt->draw_list,
                            { x1, y1 }, { x2, y2 },
                            random_range_color, 0.0f, 0);

                    y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                    y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                    if (y3 != y4)
                        ImDrawList_AddRectFilled(crv_edt->draw_list,
                            { x1, y3 }, { x2, y4 },
                            key_random_range_color, 0.0f, 0);
                }
                else {
                    float_t y1 = convert_value_to_height(glt_edt, value.y + random.y,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y2 = convert_value_to_height(glt_edt, value.y,
                        canvas_pos.y, canvas_size.y, min, max);

                    y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                    y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                    if (y1 != y2)
                        ImDrawList_AddRectFilled(crv_edt->draw_list,
                            { x1, y1 }, { x2, y2 },
                            random_range_color, 0.0f, 0);
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
                            ImDrawList_AddRectFilled(crv_edt->draw_list,
                                { x1, y1 }, { x2, y2 },
                                key_random_range_color, 0.0f, 0);

                        y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                        y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                        if (y3 != y4)
                            ImDrawList_AddRectFilled(crv_edt->draw_list,
                                { x1, y3 }, { x2, y4 },
                                random_range_color, 0.0f, 0);
                    }
                    else {
                        float_t y1 = convert_value_to_height(glt_edt, value.y,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y2 = convert_value_to_height(glt_edt, value.y - random.y,
                            canvas_pos.y, canvas_size.y, min, max);

                        y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                        y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                        if (y1 != y2)
                            ImDrawList_AddRectFilled(crv_edt->draw_list,
                                { x1, y1 }, { x2, y2 },
                                random_range_color, 0.0f, 0);
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
                        if (k_type != GLITTER_KEY_HERMITE)
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
                                ImDrawList_AddLine(crv_edt->draw_list,
                                    { x, y1 }, { x, y2 },
                                    random_range_color, 0.0f);

                            y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                            y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                            if (y3 != y4)
                                ImDrawList_AddLine(crv_edt->draw_list,
                                    { x, y3 }, { x, y4 },
                                    key_random_range_color, 0.0f);
                        }
                        else {
                            float_t y1 = convert_value_to_height(glt_edt, value.y + random.y,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y2 = convert_value_to_height(glt_edt, value.y,
                                canvas_pos.y, canvas_size.y, min, max);

                            y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                            y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                            if (y1 != y2)
                                ImDrawList_AddLine(crv_edt->draw_list,
                                    { x, y1 }, { x, y2 },
                                    random_range_color, 0.0f);
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
                                    ImDrawList_AddLine(crv_edt->draw_list,
                                        { x, y1 }, { x, y2 },
                                        key_random_range_color, 0.0f);

                                y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                                y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                                if (y3 != y4)
                                    ImDrawList_AddLine(crv_edt->draw_list,
                                        { x, y3 }, { x, y4 },
                                        random_range_color, 0.0f);
                            }
                            else {
                                float_t y1 = convert_value_to_height(glt_edt, value.y,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y2 = convert_value_to_height(glt_edt, value.y - random.y,
                                    canvas_pos.y, canvas_size.y, min, max);

                                y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                                y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                                if (y1 != y2)
                                    ImDrawList_AddLine(crv_edt->draw_list,
                                        { x, y1 }, { x, y2 },
                                        random_range_color, 0.0f);
                            }
                    }
            }
        }
    }

    for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
        if (i->frame >= end_time || keys->end - i <= 1)
            break;

        glitter_curve_key* c = i;
        glitter_curve_key* n = i + 1;
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

        if (i->type != GLITTER_KEY_HERMITE) {
            float_t y1 = convert_value_to_height(glt_edt, c_value, canvas_pos.y, canvas_size.y, min, max);
            float_t y2 = convert_value_to_height(glt_edt, n_value, canvas_pos.y, canvas_size.y, min, max);

            if (i->type == GLITTER_KEY_CONSTANT) {
                ImVec2 points_temp[4];
                points_temp[0] = { x1, y1 };
                points_temp[1] = { x2, y1 };
                points_temp[2] = { x2, y1 };
                points_temp[3] = { x2, y2 };
                ImDrawList_AddPolyline(crv_edt->draw_list, points_temp, 4, default_color, 0, 3.0f);
            }
            else
                ImDrawList_AddLine(crv_edt->draw_list,
                    { x1, y1 }, { x2, y2 }, default_color, 3.0f);
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
            ImDrawList_AddPolyline(crv_edt->draw_list, points,
                (int32_t)points_count, default_color, 0, 3.0f);
            free(points);
        }
    }

    const ImU32 key_constant_color = igGetColorU32_Vec4({ 1.0f, 0.0f, 0.0f, 0.75f });
    const ImU32 key_linear_color = igGetColorU32_Vec4({ 0.0f, 1.0f, 0.0f, 0.75f });
    const ImU32 key_hermite_color = igGetColorU32_Vec4({ 0.0f, 0.0f, 1.0f, 0.75f });

    bool dragged = false;
    bool holding_tan = false;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
        if (!(i->frame >= start_time || i->frame <= end_time))
            continue;
        else if (i - keys->begin > 1)
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
        case GLITTER_KEY_CONSTANT:
            value_color = key_constant_color;
            break;
        case GLITTER_KEY_LINEAR:
        default:
            value_color = key_linear_color;
            break;
        case GLITTER_KEY_HERMITE:
            value_color = key_hermite_color;
            break;
        }

        igPushID_Ptr(i);
        igSetCursorScreenPos({ x - 7.5f, y - 7.5f });
        igInvisibleButton("val", { 15.0f, 15.0f }, 0);

        if (can_drag && igIsItemActive() && igIsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
            float_t old_y = y;

            static int32_t base_frame;
            static float_t base_y;
            if ((igIsKeyPressed(GLFW_KEY_LEFT_SHIFT, false) && !igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || (igIsKeyPressed(GLFW_KEY_RIGHT_SHIFT, false) && !igIsKeyDown(GLFW_KEY_LEFT_SHIFT))) {
                base_frame = i->frame;
                base_y = base_value;
            }

            if (i->frame >= start_time && i->frame <= end_time) {
                int32_t start = i - keys->begin > 0 ? (i - 1)->frame + 1 : start_time;
                int32_t end = keys->end - i > 1 ? (i + 1)->frame - 1 : end_time;
                crv_edt->frame = (int32_t)roundf((io->MousePos.x - canvas_pos.x) / frame_width);
                crv_edt->frame = clamp(crv_edt->frame + start_time, start, end);
                if (i->frame != crv_edt->frame) {
                    i->frame = crv_edt->frame;
                    changed = true;
                }
            }

            static float_t mouse_delta_history;
            if ((igIsKeyPressed(GLFW_KEY_LEFT_SHIFT, false) && !igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || (igIsKeyPressed(GLFW_KEY_RIGHT_SHIFT, false) && !igIsKeyDown(GLFW_KEY_LEFT_SHIFT)))
                mouse_delta_history = io->MouseDelta.y;
            else if ((igIsKeyReleased(GLFW_KEY_LEFT_SHIFT) && !igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                || igIsKeyReleased(GLFW_KEY_RIGHT_SHIFT) && !igIsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
                y += mouse_delta_history;
                y += io->MouseDelta.y;
            }
            else if (igIsKeyDown(GLFW_KEY_LEFT_SHIFT) || igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
                mouse_delta_history += io->MouseDelta.y;
            else
                y += io->MouseDelta.y;

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
                crv_edt->key = i;
            }
        }

        if (i - keys->begin > 0 && (i - 1)->type == GLITTER_KEY_HERMITE) {
            ImU32 tangent1_color;
            float_t tangent1;
            ImVec2 circle_pos;
            circle_pos.x = x - 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(glt_edt, tangent1 = i->tangent1,
                canvas_pos.y, canvas_size.y, min, max);
            ImDrawList_AddLine(crv_edt->draw_list, { x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                igSetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                igInvisibleButton("tan1", { 15.0f, 15.0f }, 0);

                if (can_drag && igIsItemActive()) {
                    if (igIsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
                        i->tangent1 = convert_height_to_value(glt_edt,
                            circle_pos.y + io->MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent1 != tangent1)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent1_color = selected_color;
                }
                else
                    tangent1_color = default_color;

                ImDrawList_AddCircleFilled(crv_edt->draw_list, circle_pos,
                    crv_edt->key_radius_in, tangent1_color, 12);
            }
        }

        if (keys->end - i > 1 && i->type == GLITTER_KEY_HERMITE) {
            ImU32 tangent2_color;
            float_t tangent2;
            ImVec2 circle_pos;
            circle_pos.x = x + 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(glt_edt, tangent2 = i->tangent2,
                canvas_pos.y, canvas_size.y, min, max);
            ImDrawList_AddLine(crv_edt->draw_list, { x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                igSetCursorScreenPos({ circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                igInvisibleButton("tan2", { 15.0f, 15.0f }, 0);

                if (can_drag && igIsItemActive()) {
                    if (igIsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
                        i->tangent2 = convert_height_to_value(glt_edt,
                            circle_pos.y + io->MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent2 != tangent2)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent2_color = selected_color;
                }
                else
                    tangent2_color = default_color;

                ImDrawList_AddCircleFilled(crv_edt->draw_list, circle_pos,
                    crv_edt->key_radius_in, tangent2_color, 12);
            }
        }

        ImDrawList_AddCircleFilled(crv_edt->draw_list, { x, y },
            crv_edt->key_radius_out, crv_edt->key == i ? selected_color : default_color, 12);
        ImDrawList_AddCircleFilled(crv_edt->draw_list, { x, y },
            crv_edt->key_radius_in, value_color, 12);
        igPopID();
    }

    if (!crv_edt->key_edit && can_drag && io->MouseDown[0] && (!holding_tan || dragged)) {
        int32_t frame = (int32_t)roundf((io->MousePos.x - canvas_pos.x) / frame_width);
        crv_edt->frame = clamp(frame + start_time, start_time, end_time);
        if (igIsKeyDown(GLFW_KEY_LEFT_SHIFT) || igIsKeyDown(GLFW_KEY_RIGHT_SHIFT)) {
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
        ImU32 cursor_color = igGetColorU32_Vec4({ 0.8f, 0.8f, 0.8f, 0.75f });
        p1.x = cursor_pos.x;
        p1.y = canvas_pos.y + canvas_size.y;
        ImDrawList_AddLine(crv_edt->draw_list, cursor_pos, p1, cursor_color, 2.5f);

        p1 = p2 = p3 = cursor_pos;
        p1.x -= 10.0f;
        p2.y += 10.0f;
        p3.x += 10.0f;
        ImDrawList_AddTriangleFilled(crv_edt->draw_list, p1, p2, p3, cursor_color);
    }
    ImDrawList_PopClipRect(crv_edt->draw_list);

    if (hovered)
        if (igIsKeyDown(GLFW_KEY_LEFT_SHIFT) || igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
            crv_edt->timeline_pos -= io->MouseWheel * 25.0f * 4.0f;
        else
            crv_edt->timeline_pos -= io->MouseWheel * 25.0f;

    crv_edt->timeline_pos = clamp(crv_edt->timeline_pos,
        -curve_editor_timeline_base_pos, timeline_max_offset);

    if (crv_edt->list[crv_edt->type] && changed) {
        glitter_curve_recalculate(eg->type, curve);
        *crv_edt->list[crv_edt->type] = *curve;
        glt_edt->input_reload = true;
    }
}

static void glitter_editor_curve_editor_curve_reset(glitter_editor_struct* glt_edt, glitter_curve* curve) {
    memset(curve, 0, sizeof(glitter_curve));
    glitter_editor_curve_editor_curve_set(glt_edt, curve, (glitter_curve_type)-1);
}

static void glitter_editor_curve_editor_curve_set(glitter_editor_struct* glt_edt,
    glitter_curve* curve, glitter_curve_type type) {
    glitter_effect_group* eg = glt_edt->effect_group;
    curve->type = type;
    if (eg && eg->type == GLITTER_X) {
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

    glitter_editor_curve_editor_curve_reset(glt_edt, &crv_edt->curve);
    memset(&crv_edt->list, 0, sizeof(glitter_curve*)
        * GLITTER_CURVE_V_SCROLL_ALPHA_2ND - GLITTER_CURVE_TRANSLATION_X + 1);
    crv_edt->animation = 0;
    crv_edt->key = 0;
}

static glitter_curve_key* glitter_editor_curve_editor_get_closest_key(glitter_editor_struct* glt_edt,
    glitter_curve* curve) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    glitter_curve_key* key = 0;
    vector_old_glitter_curve_key* keys = &curve->keys_rev;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
        if (i->frame == crv_edt->frame)
            return i;

        if (!key || (key && abs(i->frame - crv_edt->frame)
            <= abs(key->frame - crv_edt->frame)))
            key = i;
    }
    return key;
}

static glitter_curve_key* glitter_editor_curve_editor_get_selected_key(glitter_editor_struct* glt_edt, glitter_curve* curve) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    vector_old_glitter_curve_key* keys = &curve->keys_rev;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
        if (i->frame == crv_edt->frame)
            return i;
    return 0;
}

static float_t glitter_editor_curve_editor_get_value(glitter_editor_struct* glt_edt, glitter_curve_type type) {
    glitter_emitter* sel_emit = glt_edt->selected_emitter;
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_particle* sel_ptcl = glt_edt->selected_particle;

    float_t value = curve_base_values[type];
    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        switch (type) {
        case GLITTER_CURVE_TRANSLATION_X:
            value = sel_efct->translation.x;
            break;
        case GLITTER_CURVE_TRANSLATION_Y:
            value = sel_efct->translation.y;
            break;
        case GLITTER_CURVE_TRANSLATION_Z:
            value = sel_efct->translation.z;
            break;
        case GLITTER_CURVE_ROTATION_X:
            value = sel_efct->rotation.x;
            break;
        case GLITTER_CURVE_ROTATION_Y:
            value = sel_efct->rotation.y;
            break;
        case GLITTER_CURVE_ROTATION_Z:
            value = sel_efct->rotation.z;
            break;
        case GLITTER_CURVE_SCALE_X:
            value = sel_efct->scale.x;
            break;
        case GLITTER_CURVE_SCALE_Y:
            value = sel_efct->scale.y;
            break;
        case GLITTER_CURVE_SCALE_Z:
            value = sel_efct->scale.z;
            break;
        case GLITTER_CURVE_SCALE_ALL:
            break;
        }
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        switch (type) {
        case GLITTER_CURVE_TRANSLATION_X:
            value = sel_emit->translation.x;
            break;
        case GLITTER_CURVE_TRANSLATION_Y:
            value = sel_emit->translation.y;
            break;
        case GLITTER_CURVE_TRANSLATION_Z:
            value = sel_emit->translation.z;
            break;
        case GLITTER_CURVE_ROTATION_X:
            value = sel_emit->rotation.x;
            break;
        case GLITTER_CURVE_ROTATION_Y:
            value = sel_emit->rotation.y;
            break;
        case GLITTER_CURVE_ROTATION_Z:
            value = sel_emit->rotation.z;
            break;
        case GLITTER_CURVE_SCALE_X:
            value = sel_emit->scale.x;
            break;
        case GLITTER_CURVE_SCALE_Y:
            value = sel_emit->scale.y;
            break;
        case GLITTER_CURVE_SCALE_Z:
            value = sel_emit->scale.z;
            break;
        case GLITTER_CURVE_EMISSION_INTERVAL:
            value = sel_emit->data.emission_interval;
            break;
        case GLITTER_CURVE_PARTICLES_PER_EMISSION:
            value = sel_emit->data.particles_per_emission;
            break;
        }
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        switch (type) {
        case GLITTER_CURVE_ROTATION_X:
            value = sel_ptcl->data.rotation.x;
            break;
        case GLITTER_CURVE_ROTATION_Y:
            value = sel_ptcl->data.rotation.y;
            break;
        case GLITTER_CURVE_ROTATION_Z:
            value = sel_ptcl->data.rotation.z;
            break;
        case GLITTER_CURVE_SCALE_X:
            value = sel_ptcl->data.scale.x;
            break;
        case GLITTER_CURVE_SCALE_Y:
            value = sel_ptcl->data.scale.y;
            break;
        case GLITTER_CURVE_SCALE_Z:
            value = sel_ptcl->data.scale.z;
            break;
        case GLITTER_CURVE_COLOR_R:
            value = sel_ptcl->data.color.x;
            break;
        case GLITTER_CURVE_COLOR_G:
            value = sel_ptcl->data.color.y;
            break;
        case GLITTER_CURVE_COLOR_B:
            value = sel_ptcl->data.color.z;
            break;
        case GLITTER_CURVE_COLOR_A:
            value = sel_ptcl->data.color.w;
            break;
        }
        break;
    }
    return value;
}

static void glitter_editor_curve_editor_key_manager(glitter_editor_struct* glt_edt,
    vector_old_glitter_curve_key* keys, bool* add_key, bool* del_key) {
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;

    glitter_curve* curve = &crv_edt->curve;
    *add_key = true;
    *del_key = false;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
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
    glitter_emitter* sel_emit = glt_edt->selected_emitter;
    glitter_curve_editor* crv_edt = &glt_edt->curve_editor;
    glitter_particle* sel_ptcl = glt_edt->selected_particle;

    crv_edt->key_edit = false;
    if (crv_edt->type < GLITTER_CURVE_TRANSLATION_X
        || crv_edt->type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    float_t scale;
    float_t inv_scale;
    float_t min;

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_curve* curve = &crv_edt->curve;

    bool fix_rot_z = eg->type != GLITTER_X && curve->type == GLITTER_CURVE_ROTATION_Z
        && ((glt_edt->selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && sel_emit->data.direction == GLITTER_DIRECTION_Z_AXIS)
            || (glt_edt->selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && sel_ptcl->data.draw_type == GLITTER_DIRECTION_Z_AXIS));

    if (curve->type >= GLITTER_CURVE_ROTATION_X && curve->type <= GLITTER_CURVE_ROTATION_Z) {
        scale = RAD_TO_DEG_FLOAT;
        inv_scale = DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= GLITTER_CURVE_COLOR_R && curve->type <= GLITTER_CURVE_COLOR_RGB_SCALE_2ND) {
        scale = 255.0f;
        inv_scale = (float_t)(1.0 / 255.0);
    }
    else {
        scale = 1.0f;
        inv_scale = 1.0f;
    }

    if (curve->type == GLITTER_CURVE_EMISSION_INTERVAL)
        min = -1.0f;
    else if (curve->type == GLITTER_CURVE_PARTICLES_PER_EMISSION)
        min = 0.0f;
    else
        min = -FLT_MAX;

    bool random_range = curve->flags & GLITTER_CURVE_RANDOM_RANGE ? true : false;
    bool key_random_range = curve->flags & GLITTER_CURVE_KEY_RANDOM_RANGE ? true : false;

    bool changed = false;
    bool key_edit = false;

    float_t value;
    if (vector_old_length(curve->keys_rev) > 1) {
        glitter_curve_key* c = curve->keys_rev.begin;
        glitter_curve_key* n = vector_old_length(curve->keys_rev) > 1 ? c + 1 : 0;
        while (n != curve->keys_rev.end && n->frame < crv_edt->frame) {
            c++;
            n++;
        }

        if (c + 1 != curve->keys_rev.end)
            switch (c->type) {
            case GLITTER_KEY_CONSTANT:
                value = n->frame == crv_edt->frame ? n->value : c->value;
                break;
            case GLITTER_KEY_LINEAR:
                value = interpolate_linear_value(c->value, n->value,
                    (float_t)c->frame, (float_t)n->frame, (float_t)crv_edt->frame);
                break;
            case GLITTER_KEY_HERMITE:
                value = interpolate_chs_value(c->value, n->value, c->tangent2, n->tangent1,
                    (float_t)c->frame, (float_t)n->frame, (float_t)crv_edt->frame);
                break;
            }
    }
    else if (vector_old_length(curve->keys_rev) == 1)
        value = curve->keys_rev.begin->value;
    else if (curve->type >= GLITTER_CURVE_TRANSLATION_X && curve->type <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        value = glitter_editor_curve_editor_get_value(glt_edt, curve->type);
    else
        value = 0.0f;

    value *= scale;
    igText("Frame: %d; Value: %g", crv_edt->frame, value);

    if (crv_edt->key && igTreeNodeEx_Str("Key", ImGuiTreeNodeFlags_DefaultOpen)) {
        glitter_curve_key* c = crv_edt->key;
        glitter_curve_key* n = curve->keys_rev.end - c > 1 ? crv_edt->key + 1 : 0;
        if (imguiColumnComboBox("Type", glitter_key_name,
            GLITTER_KEY_HERMITE,
            (int32_t*)&c->type, 0, true, &data->imgui_focus))
            changed = true;
        key_edit |= igIsItemFocused();

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
        key_edit |= igIsItemFocused();

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
        key_edit |= igIsItemFocused();
        imguiDisableElementPop(key_random_range);

        if (n && c->type == GLITTER_KEY_HERMITE) {
            float_t tangent2 = c->tangent2 * scale;
            if (imguiColumnDragFloat("Tangent 1",
                &tangent2, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                c->tangent2 = tangent2 * inv_scale;
                changed = true;
            }
            key_edit |= igIsItemFocused();

            float_t tangent1 = n->tangent1 * scale;
            if (imguiColumnDragFloat("Tangent 2",
                &tangent1, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                n->tangent1 = tangent1 * inv_scale;
                changed = true;
            }
            key_edit |= igIsItemFocused();
        }
        igTreePop();
    }
    crv_edt->key_edit = key_edit;

    if (igTreeNodeEx_Str("Curve", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (imguiCheckbox("Repeat", &curve->repeat))
            changed = true;

        int32_t start_time = curve->start_time;
        if (imguiColumnInputInt("Start Time",
            &start_time, 1, 1, 0)) {
            start_time = clamp(start_time, 0, 0x7FFF);
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
        if (imguiColumnInputInt("End Time",
            &end_time, 1, 1, 0)) {
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
        igText("C Type:");
        igSameLine(0.0f, -1.0f);
        igSetNextItemWidth(60.0f);
        if (igInputInt("##C Type:", &c_type, 1, 1, 0)) {
            if (eg->type == GLITTER_X)
                c_type = clamp(c_type, 1, 2);
            else
                c_type = clamp(c_type, 0, 1);

            if (c_type != curve->version)
                changed = true;
            curve->version = c_type;
        }

        int32_t k_type = curve->keys_version;
        igSameLine(0.0f, -1.0f);
        igText("K Type:");
        igSameLine(0.0f, -1.0f);
        igSetNextItemWidth(60.0f);
        if (igInputInt("##K Type:", &k_type, 1, 1, 0)) {
            if (eg->type == GLITTER_X)
                k_type = clamp(k_type, 2, 3);
            else
                k_type = clamp(k_type, 0, 2);

            if (k_type != curve->keys_version)
                changed = true;
            curve->keys_version = k_type;
        }

        igSeparator();

        igPushID_Str("Flags");
        if (imguiCheckboxFlags_UintPtr("Baked",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_BAKED))
            changed = true;

        if (eg->type == GLITTER_X) {
            imguiDisableElementPush(curve->flags & GLITTER_CURVE_BAKED ? true : false);
            if (imguiCheckboxFlags_UintPtr("Baked Full",
                (uint32_t*)&curve->flags,
                GLITTER_CURVE_BAKED_FULL))
                changed = true;
            imguiDisableElementPop(curve->flags & GLITTER_CURVE_BAKED ? true : false);
        }

        igSeparator();

        if (imguiCheckboxFlags_UintPtr("Random Range",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_RANDOM_RANGE))
            changed = true;

        if (imguiCheckboxFlags_UintPtr("Key Random Range",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_KEY_RANDOM_RANGE))
            changed = true;

        if (imguiCheckboxFlags_UintPtr("Random Range Negate",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_RANDOM_RANGE_NEGATE))
            changed = true;

        if (imguiCheckboxFlags_UintPtr("Step Random",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_STEP))
            changed = true;

        if (imguiCheckboxFlags_UintPtr("Random Negate",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_NEGATE))
            changed = true;

        if (imguiCheckboxFlags_UintPtr(eg->type == GLITTER_FT
            ? "Scale Random by Value" : "Scale Random by Value * 0.01",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_RANDOM_RANGE_MULT))
            changed = true;
        igPopID();
        igTreePop();
    }

    if (crv_edt->list[crv_edt->type]) {
        if (changed) {
            glitter_curve_recalculate(eg->type, curve);
            glt_edt->input_reload = true;
        }

        uint64_t hash1 = hash_fnv1a64m(crv_edt->list[crv_edt->type], sizeof(glitter_curve), false);
        uint64_t hash2 = hash_fnv1a64m(curve, sizeof(glitter_effect), false);
        if (hash1 != hash2)
            *crv_edt->list[crv_edt->type] = *curve;
    }
}

static void glitter_editor_curve_editor_reset_state(glitter_editor_struct* glt_edt, glitter_curve_type type) {
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
    case GLITTER_CURVE_TRANSLATION_X:
    case GLITTER_CURVE_TRANSLATION_Y:
    case GLITTER_CURVE_TRANSLATION_Z:
        crv_edt->range = 25.0f;
        break;
    case GLITTER_CURVE_ROTATION_X:
    case GLITTER_CURVE_ROTATION_Y:
    case GLITTER_CURVE_ROTATION_Z:
        crv_edt->range = 360.0f;
        break;
    case GLITTER_CURVE_SCALE_X:
    case GLITTER_CURVE_SCALE_Y:
    case GLITTER_CURVE_SCALE_Z:
    case GLITTER_CURVE_SCALE_ALL:
        crv_edt->range = 5.0f;
        break;
    case GLITTER_CURVE_COLOR_R:
    case GLITTER_CURVE_COLOR_G:
    case GLITTER_CURVE_COLOR_B:
    case GLITTER_CURVE_COLOR_A:
    case GLITTER_CURVE_COLOR_RGB_SCALE:
    case GLITTER_CURVE_COLOR_R_2ND:
    case GLITTER_CURVE_COLOR_G_2ND:
    case GLITTER_CURVE_COLOR_B_2ND:
    case GLITTER_CURVE_COLOR_A_2ND:
    case GLITTER_CURVE_COLOR_RGB_SCALE_2ND:
        crv_edt->range = 255.0f;
        break;
    case GLITTER_CURVE_EMISSION_INTERVAL:
        crv_edt->range = 20.0f;
        break;
    case GLITTER_CURVE_PARTICLES_PER_EMISSION:
        crv_edt->range = 50.0f;
        break;
    case GLITTER_CURVE_U_SCROLL:
    case GLITTER_CURVE_V_SCROLL:
    case GLITTER_CURVE_U_SCROLL_ALPHA:
    case GLITTER_CURVE_V_SCROLL_ALPHA:
    case GLITTER_CURVE_U_SCROLL_2ND:
    case GLITTER_CURVE_V_SCROLL_2ND:
    case GLITTER_CURVE_U_SCROLL_ALPHA_2ND:
    case GLITTER_CURVE_V_SCROLL_ALPHA_2ND:
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
    glitter_effect* sel_efct = glt_edt->selected_effect;
    glitter_emitter* sel_emit = glt_edt->selected_emitter;
    glitter_particle* sel_ptcl = glt_edt->selected_particle;

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_curve_type_flags flags = (glitter_curve_type_flags)0;
    glitter_animation* anim = 0;
    switch (glt_edt->selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        flags = glitter_effect_curve_flags;
        anim = &sel_efct->animation;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        flags = glitter_emitter_curve_flags;
        anim = &sel_emit->animation;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (eg->type == GLITTER_X)
            flags = glitter_particle_x_curve_flags;
        else
            flags = glitter_particle_curve_flags;
        anim = &sel_ptcl->animation;

        if (sel_ptcl->data.type != GLITTER_PARTICLE_MESH) {
            enum_and(flags, ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND);
            if (sel_ptcl->data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
                enum_and(flags, ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y));
        }
        break;
    default:
        return;
    }
    crv_edt->animation = anim;

    for (int32_t i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++) {
        bool found = false;
        glitter_curve** j;
        for (j = anim->begin; j != anim->end; j++)
            if (*j && (*j)->type == i) {
                found = true;
                break;
            }
        crv_edt->list[i] = found ? *j : 0;
    }

    bool reset = false;
    ImVec2 cont_reg_avail;
    igGetContentRegionAvail(&cont_reg_avail);
    if (igBeginListBox("##Curves", { cont_reg_avail.x * 0.5f, cont_reg_avail.y })) {
        ImGuiTreeNodeFlags tree_node_flags = 0;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

#define LIST_BOX(label, curve_type) \
if (flags & GLITTER_CURVE_TYPE_##curve_type){ \
    const char* _l =  crv_edt->list[GLITTER_CURVE_##curve_type] ? label" [*]" : label; \
    igSelectable_Bool(_l, crv_edt->type == GLITTER_CURVE_##curve_type, 0, ImVec2_Empty); \
\
    if (igIsItemFocused() && crv_edt->type != GLITTER_CURVE_##curve_type) { \
        glitter_editor_curve_editor_reset_state(glt_edt, GLITTER_CURVE_##curve_type); \
        reset = true; \
    } \
}

        if (flags & GLITTER_CURVE_TYPE_TRANSLATION_XYZ
            && igTreeNodeEx_Str("Translation", tree_node_flags)) {
            igPushID_Str("T");
            LIST_BOX("X", TRANSLATION_X);
            LIST_BOX("Y", TRANSLATION_Y);
            LIST_BOX("Z", TRANSLATION_Z);
            igPopID();
            igTreePop();
        }

        if (flags & GLITTER_CURVE_TYPE_ROTATION_XYZ
            && igTreeNodeEx_Str("Rotation", tree_node_flags)) {
            igPushID_Str("R");
            LIST_BOX("X", ROTATION_X);
            LIST_BOX("Y", ROTATION_Y);
            LIST_BOX("Z", ROTATION_Z);
            igPopID();
            igTreePop();
        }

        if (flags & (GLITTER_CURVE_TYPE_SCALE_XYZ | GLITTER_CURVE_TYPE_SCALE_ALL)
            && igTreeNodeEx_Str("Scale", tree_node_flags)) {
            igPushID_Str("S");
            LIST_BOX("X", SCALE_X);
            LIST_BOX("Y", SCALE_Y);
            LIST_BOX("Z", SCALE_Z);
            LIST_BOX("All", SCALE_ALL);
            igPopID();
            igTreePop();
        }

        if (flags & (GLITTER_CURVE_TYPE_COLOR_RGBA | GLITTER_CURVE_TYPE_COLOR_RGB_SCALE)
            && igTreeNodeEx_Str("Color", tree_node_flags)) {
            igPushID_Str("C");
            LIST_BOX("R", COLOR_R);
            LIST_BOX("G", COLOR_G);
            LIST_BOX("B", COLOR_B);
            LIST_BOX("A", COLOR_A);
            LIST_BOX("RGB Scale", COLOR_RGB_SCALE);
            igPopID();
            igTreePop();
        }

        if (flags & (GLITTER_CURVE_TYPE_COLOR_RGBA_2ND | GLITTER_CURVE_TYPE_COLOR_RGB_SCALE_2ND)
            && igTreeNodeEx_Str("Color 2nd", tree_node_flags)) {
            igPushID_Str("C2");
            LIST_BOX("R", COLOR_R_2ND);
            LIST_BOX("G", COLOR_G_2ND);
            LIST_BOX("B", COLOR_B_2ND);
            LIST_BOX("A", COLOR_A_2ND);
            LIST_BOX("RGB Scale", COLOR_RGB_SCALE_2ND);
            igPopID();
            igTreePop();
        }

        LIST_BOX("Emit Interval", EMISSION_INTERVAL);
        LIST_BOX("PTC Per Emit", PARTICLES_PER_EMISSION);

        if (flags & (GLITTER_CURVE_TYPE_UV_SCROLL | GLITTER_CURVE_TYPE_UV_SCROLL_ALPHA)
            && igTreeNodeEx_Str("UV Scroll", tree_node_flags)) {
            igPushID_Str("UV");
            LIST_BOX("U", U_SCROLL);
            LIST_BOX("V", V_SCROLL);
            LIST_BOX("U Alpha", U_SCROLL_ALPHA);
            LIST_BOX("V Alpha", V_SCROLL_ALPHA);
            igPopID();
            igTreePop();
        }

        if (flags & (GLITTER_CURVE_TYPE_UV_SCROLL_2ND | GLITTER_CURVE_TYPE_UV_SCROLL_ALPHA_2ND)
            && igTreeNodeEx_Str("UV Scroll 2nd", tree_node_flags)) {
            igPushID_Str("UV2");
            LIST_BOX("U", U_SCROLL_2ND);
            LIST_BOX("V", V_SCROLL_2ND);
            LIST_BOX("U Alpha", U_SCROLL_ALPHA_2ND);
            LIST_BOX("V Alpha", V_SCROLL_ALPHA_2ND);
            igPopID();
            igTreePop();
        }
#undef LIST_BOX
        data->imgui_focus |= igIsWindowFocused(0);
        igEndListBox();
    }

    if (crv_edt->type < GLITTER_CURVE_TRANSLATION_X
        || crv_edt->type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    if (crv_edt->list[crv_edt->type])
        crv_edt->curve = *crv_edt->list[crv_edt->type];
    else if (crv_edt->curve.type != crv_edt->type) {
        memset(&crv_edt->curve, 0, sizeof(glitter_curve));
        glitter_editor_curve_editor_curve_set(glt_edt, &crv_edt->curve, crv_edt->type);
    }
    glitter_curve* curve = &crv_edt->curve;

    if (reset)
        crv_edt->key = glitter_editor_curve_editor_get_closest_key(glt_edt,
            &crv_edt->curve);

    igSameLine(0.0f, -1.0f);
    igBeginGroup();
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
    glitter_editor_curve_editor_key_manager(glt_edt, &curve->keys_rev, &add_key, &del_key);

    igSeparator();
    float_t w = imguiGetContentRegionAvailWidth();
    imguiDisableElementPush(add_key);
    if (imguiButton("Add Key", { w, 0.0f }))
        crv_edt->add_key = true;
    imguiDisableElementPop(add_key);

    imguiDisableElementPush(del_key);
    if (imguiButton("Delete Key", { w, 0.0f }))
        crv_edt->del_key = true;
    imguiDisableElementPop(del_key);

    bool has_curve_in_list = crv_edt->list[crv_edt->type] ? true : false;
    imguiDisableElementPush(!has_curve_in_list);
    if (imguiButton("Add Curve", { w, 0.0f }))
        crv_edt->add_curve = true;
    imguiDisableElementPop(!has_curve_in_list);

    imguiDisableElementPush(has_curve_in_list);
    if (imguiButton("Delete Curve", { w, 0.0f }))
        crv_edt->del_curve = true;
    imguiDisableElementPop(has_curve_in_list);
}

static bool glitter_editor_hash_input(glitter_editor_struct* glt_edt, const char* label, uint64_t* hash) {
    uint64_t h = *hash;
    if (glt_edt->effect_group->type == GLITTER_FT) {
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
    shader_glsl_load_string(&glt_edt->gl_data.wireframe.shader,
        (char*)glitter_editor_wireframe_vert, (char*)glitter_editor_wireframe_frag, 0, &param);
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
    shader_glsl_free(&glt_edt->gl_data.wireframe.shader);
}

static void glitter_editor_gl_draw_wireframe(glitter_editor_struct* glt_edt) {
    glitter_scene* sc = GPM_VAL.scene;
    glitter_effect_inst* eff = GPM_VAL.effect;
    glitter_emitter_inst* emit = GPM_VAL.emitter;
    glitter_particle_inst* ptcl = GPM_VAL.particle;
    if (!GPM_VAL.draw_selected || !eff) {
        if (!sc)
            return;

        size_t count = 0;
        for (glitter_scene_effect* i = sc->effects.begin; i != sc->effects.end; i++)
            if (i->disp && i->ptr)
                count += glitter_editor_gl_draw_wireframe_calc(glt_edt, i->ptr, 0);

        if (count == 0)
            return;

        for (glitter_scene_effect* i = sc->effects.begin; i != sc->effects.end; i++)
            if (i->disp && i->ptr)
                glitter_editor_gl_draw_wireframe_draw(glt_edt, i->ptr, 0);
    }
    else if ((eff && ptcl) || (eff && !emit)) {
        size_t count = glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, ptcl);
        if (count == 0)
            return;

        glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, ptcl);
    }
    else if (emit) {
        size_t count = 0;
        for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, particle);
            if (particle->data.children.begin) {
                vector_old_ptr_glitter_particle_inst* children = &particle->data.children;
                for (glitter_particle_inst** j = children->begin; j != children->end; j++)
                    if (*j)
                        count += glitter_editor_gl_draw_wireframe_calc(glt_edt, eff, *j);
            }
        }

        if (count == 0)
            return;

        for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, particle);
            if (particle->data.children.begin) {
                vector_old_ptr_glitter_particle_inst* children = &particle->data.children;
                for (glitter_particle_inst** j = children->begin; j != children->end; j++)
                    if (*j)
                        glitter_editor_gl_draw_wireframe_draw(glt_edt, eff, *j);
            }
        }
    }
}

static size_t glitter_editor_gl_draw_wireframe_calc(glitter_editor_struct* glt_edt,
    glitter_effect_inst* eff, glitter_particle_inst* ptcl) {
    size_t count = 0;
    for (glitter_render_group** i = eff->render_scene.begin; i != eff->render_scene.end; i++) {
        if (!*i)
            continue;

        glitter_render_group* rg = *i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp > 0)
            count++;
    }
    return count;
}

static void glitter_editor_gl_draw_wireframe_draw(glitter_editor_struct* glt_edt,
    glitter_effect_inst* eff, glitter_particle_inst* ptcl) {
    gl_state_disable_blend();
    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_LINE);

    gl_state_bind_vertex_array(glt_edt->vao);
    shader_glsl_use(&glt_edt->gl_data.wireframe.shader);
    shader_glsl_set_vec4_value(&glt_edt->gl_data.wireframe.shader, "color", 1.0f, 1.0f, 0.0f, 1.0f);
    for (glitter_render_group** i = eff->render_scene.begin; i != eff->render_scene.end; i++) {
        if (!*i)
            continue;

        glitter_render_group* rg = *i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp < 1)
            continue;

        shader_glsl_set_mat4(&glt_edt->gl_data.wireframe.shader, "model", false, rg->mat_draw);

        switch (rg->type) {
        case GLITTER_PARTICLE_QUAD: {
            static const GLsizei buffer_size = sizeof(glitter_buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(glitter_buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            gl_state_bind_element_array_buffer(rg->ebo);
            glDrawElements(GL_TRIANGLES, (GLsizei)(6 * rg->disp), GL_UNSIGNED_INT, 0);
            gl_state_bind_element_array_buffer(0);

            glDisableVertexAttribArray(0);
        } break;
        case GLITTER_PARTICLE_LINE:
        case GLITTER_PARTICLE_LOCUS: {
            static const GLsizei buffer_size = sizeof(glitter_buffer);

            gl_state_bind_array_buffer(rg->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(glitter_buffer, position)); // Pos
            gl_state_bind_array_buffer(0);

            const GLenum mode = rg->type == GLITTER_PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            const size_t count = vector_old_length(rg->vec_key);
            for (size_t i = 0; i < count; i++)
                glDrawArrays(mode, rg->vec_key.begin[i], rg->vec_val.begin[i]);

            glDisableVertexAttribArray(0);
        } break;
        case GLITTER_PARTICLE_MESH:
            glitter_editor_gl_draw_wireframe_draw_mesh(glt_edt, rg);
            break;
        }
    }
    gl_state_bind_vertex_array(0);
    gl_state_set_polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    shader_glsl_use(0);
}

static void glitter_editor_gl_draw_wireframe_draw_mesh(glitter_editor_struct* glt_edt,
    glitter_render_group* rg) {
    glitter_render_element* elem = rg->elements;
    for (size_t i = rg->ctrl, j_max = 1024; i > 0; i -= j_max) {
        j_max = min(i, j_max);
        for (size_t j = j_max; j > 0; elem++) {
            if (!elem->alive)
                continue;
            j--;

            if (!elem->disp)
                continue;

            render_context* rctx = (render_context*)GPM_VAL.rctx;
            object_data* object_data = &rctx->object_data;

            glitter_particle* particle = rg->particle->data.particle;
            object_info object_info;
            object_info.set_id = (uint32_t)particle->data.mesh.object_set_name_hash;
            object_info.id = (uint32_t)particle->data.mesh.object_name_hash;
            object* obj = object_storage_get_object(object_info);
            object_mesh_vertex_buffer* obj_vertex_buffer
                = object_storage_get_object_mesh_vertex_buffer(object_info);
            object_mesh_index_buffer* obj_index_buffer
                = object_storage_get_object_mesh_index_buffer(object_info);

            mat4 mat = elem->mat_draw;
            if (!obj || !obj_vertex_buffer || !obj_index_buffer || (object_data->object_culling
                && !object_bounding_sphere_check_visibility(&obj->bounding_sphere,
                    object_data, rctx->camera, &mat)))
                continue;

            shader_glsl_set_mat4(&glt_edt->gl_data.wireframe.shader, "model", false, mat);

            int32_t ttc = 0;
            texture_transform_struct* tt = object_data->texture_transform_array;
            for (int32_t i = 0; i < obj->meshes_count; i++) {
                object_mesh* mesh = &obj->meshes[i];

                if (object_data->object_culling && !object_bounding_sphere_check_visibility(
                    &mesh->bounding_sphere, object_data, rctx->camera, &mat))
                    continue;

                for (int32_t j = 0; j < mesh->sub_meshes_count; j++) {
                    object_sub_mesh* sub_mesh = &mesh->sub_meshes[j];

                    if (sub_mesh->flags & OBJECT_SUB_MESH_FLAG_8)
                        continue;

                    if (object_data->object_culling) {
                        int32_t v32 = object_bounding_sphere_check_visibility(
                            &sub_mesh->bounding_sphere, object_data, rctx->camera, &mat);
                        if (v32 != 2 || (~mesh->flags & OBJECT_MESH_BILLBOARD &&
                            ~mesh->flags & OBJECT_MESH_BILLBOARD_Y_AXIS)) {
                            if (v32 == 2) {
                                if (object_data->object_bounding_sphere_check_func)
                                    v32 = 1;
                                else
                                    v32 = object_axis_aligned_bounding_box_check_visibility(
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

                    GLuint array_buffer = object_mesh_vertex_buffer_get_buffer(&obj_vertex_buffer[i]);
                    gl_state_bind_array_buffer(array_buffer);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh->vertex_size, (void*)0);
                    gl_state_bind_array_buffer(0);

                    GLuint element_array_buffer = obj_index_buffer[i];
                    gl_state_bind_element_array_buffer(element_array_buffer);
                    if (sub_mesh->primitive_type == OBJECT_PRIMITIVE_TRIANGLE_STRIP
                        && sub_mesh->index_format == OBJECT_INDEX_U16)
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
    GPM_VAL.scene = 0;
    GPM_VAL.effect = 0;
    GPM_VAL.emitter = 0;
    GPM_VAL.particle = 0;
    GPM_VAL.draw_selected = false;

    glitter_effect_group* eg = glt_edt->effect_group;
    glitter_scene* sc = 0;
    for (glitter_scene*& i : GPM_VAL.scenes) {
        if (!i)
            continue;

        glitter_scene* scene = i;
        if (scene->effect_group == eg) {
            sc = scene;
            break;
        }
    }
    if (!sc)
        return;

    glitter_effect_inst* eff = 0;
    glitter_emitter_inst* emit = 0;
    glitter_particle_inst* ptcl = 0;
    for (glitter_scene_effect* i = sc->effects.begin; i != sc->effects.end; i++) {
        if (!i->disp || !i->ptr)
            continue;

        glitter_effect_inst* effect = i->ptr;
        if (effect->effect != glt_edt->selected_effect)
            continue;

        eff = effect;
        for (glitter_emitter_inst** j = effect->emitters.begin; j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            glitter_emitter_inst* emitter = *j;
            if (emitter->emitter != glt_edt->selected_emitter)
                continue;

            emit = emitter;
            for (glitter_particle_inst** k = emitter->particles.begin; k != emitter->particles.end; k++) {
                if (!*k)
                    continue;

                glitter_particle_inst* particle = *k;
                if (particle->particle == glt_edt->selected_particle) {
                    ptcl = particle;
                    break;
                }
            }
            break;
        }
        break;
    }

    if (ptcl && ptcl->data.children.begin) {
        vector_old_ptr_glitter_particle_inst* children = &ptcl->data.children;
        ptcl = 0;
        for (glitter_particle_inst** i = children->begin; i != children->end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            if (particle->particle == glt_edt->selected_particle) {
                ptcl = particle;
                break;
            }
        }

        if (!ptcl)
            return;
    }

    GPM_VAL.scene = sc;
    GPM_VAL.effect = eff;
    GPM_VAL.emitter = emit;
    GPM_VAL.particle = ptcl;
    GPM_VAL.draw_selected = glt_edt->draw_flags & GLITTER_EDITOR_DRAW_SELECTED;
}
#endif
