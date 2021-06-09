/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Curve Editor code based on Animation Timeline code from https://github.com/crash5band/Glitter
*/

#ifdef CLOUD_DEV
#include "glitter_editor.h"
#include "../../KKdLib/dds.h"
#include "../../KKdLib/interpolation.h"
#include "../../KKdLib/io_path.h"
#include "../../KKdLib/io_stream.h"
#include "../../KKdLib/str_utils.h"
#include "../../KKdLib/txp.h"
#include "../../CRE/Glitter/animation.h"
#include "../../CRE/Glitter/curve.h"
#include "../../CRE/Glitter/diva_effect.h"
#include "../../CRE/Glitter/diva_list.h"
#include "../../CRE/Glitter/diva_resource.h"
#include "../../CRE/Glitter/effect.h"
#include "../../CRE/Glitter/effect_group.h"
#include "../../CRE/Glitter/emitter.h"
#include "../../CRE/Glitter/file_reader.h"
#include "../../CRE/Glitter/particle.h"
#include "../../CRE/Glitter/particle_manager.h"
#include "../../CRE/Glitter/scene.h"
#include "../../CRE/Glitter/texture.h"
#include "../../CRE/camera.h"
#include "../../CRE/shared.h"
#include "../../CRE/static_var.h"
#include "../../CRE/task.h"
#include "../input.h"
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include "imgui_helper.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

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
} glitter_editor_draw_flags;

typedef struct glitter_editor_struct {
    bool dispose;
    bool disposed;
    bool test;
    bool imgui_focus;
    bool create_popup;
    bool load;
    bool load_popup;
    bool load_error_list_popup;
    bool save;
    bool save_popup;
    bool save_compress;
    bool close;
    bool input_play;
    bool input_reload;
    bool input_pause;
    bool input_reset;
    bool effect_group_add;
    glitter_editor_draw_flags draw_flags;
    glitter_editor_flags resource_flags;
    glitter_editor_flags effect_flags;
    glitter_editor_flags emitter_flags;
    glitter_editor_flags particle_flags;
    glitter_type load_glt_type;
    glitter_type save_glt_type;
    int32_t frame_counter;
    int32_t old_frame_counter;
    double_t delta_frame;
    int32_t start_frame;
    int32_t end_frame;
    glitter_random random;
    int32_t counter;
    glitter_effect_group* effect_group;
    glitter_scene* scene;
    wchar_t file[MAX_PATH];
} glitter_editor_struct;

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
} glitter_curve_editor;

typedef struct glitter_editor_gl_wireframe {
    shader_fbo shader;
} glitter_editor_gl_wireframe;

typedef struct glitter_editor_gl {
    glitter_editor_gl_wireframe wireframe;
} glitter_editor_gl;

static const char* glitter_emitter_direction_name[] = {
    [GLITTER_EMITTER_DIRECTION_BILLBOARD]        = "Billboard",
    [GLITTER_EMITTER_DIRECTION_BILLBOARD_Y_ONLY] = "Billboard Y Only",
    [GLITTER_EMITTER_DIRECTION_X_AXIS]           = "X Axis",
    [GLITTER_EMITTER_DIRECTION_Y_AXIS]           = "Y Axis",
    [GLITTER_EMITTER_DIRECTION_Z_AXIS]           = "Z Axis",
    [GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION]  = "Effect Rotation",
};

static const char* glitter_particle_draw_type_name[] = {
    [GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD]         = "Billboard",
    [GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD_Y_ONLY]  = "Billboard Y Only",
    [GLITTER_PARTICLE_DRAW_TYPE_X_AXIS]            = "X Axis",
    [GLITTER_PARTICLE_DRAW_TYPE_Y_AXIS]            = "Y Axis",
    [GLITTER_PARTICLE_DRAW_TYPE_Z_AXIS]            = "Z Axis",
    [GLITTER_PARTICLE_DRAW_TYPE_EMITTER_DIRECTION] = "Emitter Direction",
    [GLITTER_PARTICLE_DRAW_TYPE_EMITTER_ROTATION]  = "Emitter Rotation",
    [GLITTER_PARTICLE_DRAW_TYPE_PARTICLE_ROTATION] = "Particle Rotation",
    [GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION]     = "Prev Position",
    [GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION_DUP] = "Prev Position Dup",
    [GLITTER_PARTICLE_DRAW_TYPE_EMIT_POSITION]     = "Emit Position",
};

static const char* glitter_effect_type_name[] = {
    [0] = "Normal",
    [1] = "Local",
    [2] = "Chara Parent",
    [3] = "Object Parent",
};

static const char* glitter_effect_ext_anim_index_name[] = {
    [0] = "P1",
    [1] = "P2",
    [2] = "P3",
    [3] = "P4",
    [4] = "P5",
    [5] = "P6",
};

static const char* glitter_effect_ext_anim_node_index_name[] = {
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_NONE]            = "None",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD]            = "Head",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_MOUTH]           = "Mouth",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_BELLY]           = "Belly",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_CHEST]           = "Chest",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_SHOULDER]   = "Left Shoulder",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW]      = "Left Elbow",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_ELBOW_DUP]  = "Left Elbow Dup",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_HAND]       = "Left Hand",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_SHOULDER]  = "Right Shoulder",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW]     = "Right Elbow",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_ELBOW_DUP] = "Right Elbow Dup",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_HAND]      = "Right Hand",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_THIGH]      = "Left Thigh",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_KNEE]       = "Left Knee",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_LEFT_TOE]        = "Left Toe",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_THIGH]     = "Right Thigh",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_KNEE]      = "Right Knee",
    [1 + GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE]       = "Right Toe",
};

static const char* glitter_emitter_name[] = {
    [GLITTER_EMITTER_BOX]      = "Box",
    [GLITTER_EMITTER_CYLINDER] = "Cylinder",
    [GLITTER_EMITTER_SPHERE]   = "Sphere",
    [GLITTER_EMITTER_MESH]     = "Mesh",
    [GLITTER_EMITTER_POLYGON]  = "Polygon",
};

static const char* glitter_emitter_emission_direction_name[] = {
    [GLITTER_EMITTER_EMISSION_DIRECTION_NONE]    = "None",
    [GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD] = "Outward",
    [GLITTER_EMITTER_EMISSION_DIRECTION_INWARD]  = "Inward",
};

static const char* glitter_emitter_emission_name[] = {
    [GLITTER_EMITTER_EMISSION_ON_TIMER] = "On Timer",
    [GLITTER_EMITTER_EMISSION_ON_START] = "On Start",
    [GLITTER_EMITTER_EMISSION_ON_END]   = "On End",
};

static const char* glitter_emitter_timer_name[] = {
    [GLITTER_EMITTER_TIMER_BY_TIME]     = "By Time",
    [GLITTER_EMITTER_TIMER_BY_DISTANCE] = "By Distance",
};

static const char* glitter_key_name[] = {
    [GLITTER_KEY_CONSTANT] = "Constant",
    [GLITTER_KEY_LINEAR]   = "Linear",
    [GLITTER_KEY_HERMITE]  = "Hermite",
};

static const char* glitter_particle_blend_name[] = {
    [GLITTER_PARTICLE_BLEND_ZERO]          = "Zero",
    [GLITTER_PARTICLE_BLEND_TYPICAL]       = "Typical",
    [GLITTER_PARTICLE_BLEND_ADD]           = "Add",
    [GLITTER_PARTICLE_BLEND_SUBTRACT]      = "Subtract",
    [GLITTER_PARTICLE_BLEND_MULTIPLY]      = "Multiply",
    [GLITTER_PARTICLE_BLEND_PUNCH_THROUGH] = "Punch Through",
};

static const char* glitter_particle_blend_draw_name[] = {
    [GLITTER_PARTICLE_BLEND_DRAW_TYPICAL]  = "Typical",
    [GLITTER_PARTICLE_BLEND_DRAW_ADD]      = "Add",
    [GLITTER_PARTICLE_BLEND_DRAW_MULTIPLY] = "Multiply",
};

static const char* glitter_particle_blend_mask_name[] = {
    [GLITTER_PARTICLE_BLEND_MASK_TYPICAL]  = "Typical",
    [GLITTER_PARTICLE_BLEND_MASK_ADD]      = "Add",
    [GLITTER_PARTICLE_BLEND_MASK_MULTIPLY] = "Multiply",
};

static const char* glitter_particle_name[] = {
    [GLITTER_PARTICLE_QUAD]  = "Quad",
    [GLITTER_PARTICLE_LINE]  = "Line",
    [GLITTER_PARTICLE_LOCUS] = "Locus",
    [GLITTER_PARTICLE_MESH]  = "Mesh",
};

static const char* glitter_pivot_name[] = {
    [GLITTER_PIVOT_TOP_LEFT]      = "Top Left",
    [GLITTER_PIVOT_TOP_CENTER]    = "Top Center",
    [GLITTER_PIVOT_TOP_RIGHT]     = "Top Right",
    [GLITTER_PIVOT_MIDDLE_LEFT]   = "Middle Left",
    [GLITTER_PIVOT_MIDDLE_CENTER] = "Middle Center",
    [GLITTER_PIVOT_MIDDLE_RIGHT]  = "Middle Right",
    [GLITTER_PIVOT_BOTTOM_LEFT]   = "Bottom Left",
    [GLITTER_PIVOT_BOTTOM_CENTER] = "Bottom Center",
    [GLITTER_PIVOT_BOTTOM_RIGHT]  = "Bottom Right",
};

static const char* glitter_uv_index_type_name[] = {
    [GLITTER_UV_INDEX_FIXED]                  = "Fixed",
    [GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED]   = "Initial Random / Fixed",
    [GLITTER_UV_INDEX_RANDOM]                 = "Random",
    [GLITTER_UV_INDEX_FORWARD]                = "Forward",
    [GLITTER_UV_INDEX_REVERSE]                = "Reverse",
    [GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD] = "Initial Random / Forward",
    [GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE] = "Initial Random / Reverse",
    [GLITTER_UV_INDEX_USER]                   = "User",
};

static const float_t curve_base_values[] = {
    [GLITTER_CURVE_TRANSLATION_X]          = 0.0f,
    [GLITTER_CURVE_TRANSLATION_Y]          = 0.0f,
    [GLITTER_CURVE_TRANSLATION_Z]          = 0.0f,
    [GLITTER_CURVE_ROTATION_X]             = 0.0f,
    [GLITTER_CURVE_ROTATION_Y]             = 0.0f,
    [GLITTER_CURVE_ROTATION_Z]             = 0.0f,
    [GLITTER_CURVE_SCALE_X]                = 1.0f,
    [GLITTER_CURVE_SCALE_Y]                = 1.0f,
    [GLITTER_CURVE_SCALE_Z]                = 1.0f,
    [GLITTER_CURVE_SCALE_ALL]              = 1.0f,
    [GLITTER_CURVE_COLOR_R]                = 1.0f,
    [GLITTER_CURVE_COLOR_G]                = 1.0f,
    [GLITTER_CURVE_COLOR_B]                = 1.0f,
    [GLITTER_CURVE_COLOR_A]                = 1.0f,
    [GLITTER_CURVE_COLOR_RGB_SCALE]        = 1.0f,
    [GLITTER_CURVE_COLOR_R_2ND]            = 1.0f,
    [GLITTER_CURVE_COLOR_G_2ND]            = 1.0f,
    [GLITTER_CURVE_COLOR_B_2ND]            = 1.0f,
    [GLITTER_CURVE_COLOR_A_2ND]            = 1.0f,
    [GLITTER_CURVE_COLOR_RGB_SCALE_2ND]    = 1.0f,
    [GLITTER_CURVE_EMISSION_INTERVAL]      = 0.0f,
    [GLITTER_CURVE_PARTICLES_PER_EMISSION] = 0.0f,
    [GLITTER_CURVE_U_SCROLL]               = 0.0f,
    [GLITTER_CURVE_V_SCROLL]               = 0.0f,
    [GLITTER_CURVE_U_SCROLL_ALPHA]         = 0.0f,
    [GLITTER_CURVE_V_SCROLL_ALPHA]         = 0.0f,
    [GLITTER_CURVE_U_SCROLL_2ND]           = 0.0f,
    [GLITTER_CURVE_V_SCROLL_2ND]           = 0.0f,
    [GLITTER_CURVE_U_SCROLL_ALPHA_2ND]     = 0.0f,
    [GLITTER_CURVE_V_SCROLL_ALPHA_2ND]     = 0.0f,
};

static const float_t curve_editor_timeline_base_pos = 50.0f;
static ImVec4 tint_col = { 1.0f, 1.0f, 1.0f, 1.0f };
static ImVec4 border_col = { 1.0f, 1.0f, 1.0f, 0.5f };

extern vector_task_render tasks_render;
extern vector_task_render_draw3d tasks_render_draw3d;

extern int32_t width;
extern int32_t height;
extern vec2i internal_2d_res;
extern vec2i internal_3d_res;
extern bool input_reset;
extern bool input_locked;
extern float_t frame_speed;
extern bool grid_3d;

extern camera* cam;

extern GPM;

extern bool close;
extern bool global_context_menu;

const char* glitter_editor_window_title = "Glitter Editor";

bool glitter_editor_enabled;
static glitter_editor_struct glitter_editor;

static glitter_editor_selected_enum selected_type;
static int32_t selected_resource;
static glitter_effect* selected_effect;
static glitter_emitter* selected_emitter;
static glitter_particle* selected_particle;
static glitter_curve_editor curve_editor;

static glitter_editor_gl gl_data;

static void glitter_editor_windows();

static void glitter_editor_reload();
static void glitter_editor_reset();
static void glitter_editor_reset_draw();
static void glitter_editor_save();
static void glitter_editor_open_window();
static void glitter_editor_save_window();
static void glitter_editor_save_as_window();
static void glitter_editor_load_file(wchar_t* path, wchar_t* file);
static void glitter_editor_save_file(wchar_t* path, wchar_t* file);
static bool glitter_editor_list_open_window(glitter_effect_group* eg);
static bool glitter_editor_list_parse(uint8_t* data, size_t length, char** buf, char*** lines, size_t* count);
static bool glitter_editor_resource_import();
static bool glitter_editor_resource_export();

static void glitter_editor_test_window();
static void glitter_editor_effects();
static void glitter_editor_effects_context_menu(glitter_effect* effect,
    glitter_emitter* emitter, glitter_particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type);
static void glitter_editor_resources();
static void glitter_editor_resources_context_menu(int32_t resource,
    ssize_t i_idx, bool selected_none);
static void glitter_editor_play_manager();
static void glitter_editor_property();
static void glitter_editor_property_effect();
static void glitter_editor_property_emitter();
static void glitter_editor_property_particle();
static bool glitter_editor_property_particle_texture(char* label, char** items,
    glitter_particle* particle, int32_t* tex, uint64_t* tex_hash, int32_t texture,
    bool* tex_anim, int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex);

static void glitter_editor_popups();
static void glitter_editor_file_create_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_load_error_list_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size);
static void glitter_editor_file_save_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size);

static void glitter_editor_curve_editor();
static void glitter_editor_curve_editor_curve_reset(glitter_curve* curve);
static void glitter_editor_curve_editor_curve_set(glitter_curve* curve, glitter_curve_type type);
static void glitter_editor_curve_editor_curves_reset();
static glitter_curve_key* glitter_editor_curve_editor_get_selected_key(glitter_curve* curve);
static void glitter_editor_curve_editor_get_scroll_bar(float_t max_pos, float_t canvas_size,
    float_t frame_width, ImVec2 scrollbar_size, ImVec2 scrollbar_pos,
    ImVec2 bar_offset, ImVec2* bar_size, ImVec2* bar_pos, ImVec2* bar_max);
static void glitter_editor_curve_editor_key_manager(vector_glitter_curve_key* keys, bool* add_key, bool* del_key);
static void glitter_editor_curve_editor_property_window();
static void glitter_editor_curve_editor_reset_state(glitter_curve_type type);
static void glitter_editor_curve_editor_selector();

static void glitter_editor_gl_load();
static void glitter_editor_gl_draw();
static void glitter_editor_gl_process();
static void glitter_editor_gl_free();

static void glitter_editor_gl_draw_wireframe();
static size_t glitter_editor_gl_draw_wireframe_calc(glitter_effect_inst* eff,
    glitter_particle_inst* ptcl);
static void glitter_editor_gl_draw_wireframe_draw(glitter_effect_inst* eff,
    glitter_particle_inst* ptcl);

static void glitter_editor_gl_select_particle();

static bool glitter_editor_hash_input(const char* label, uint64_t* hash);

void glitter_editor_dispose() {
    task_render task;
    memset(&task, 0, sizeof(task_render));
    task.free.type = TASK_RENDER_FREE_GL_OBJECT;
    task.free.hash = hash_char("Glitter Editor Dummy");
    vector_task_render_push_back(&tasks_render, &task);
    glitter_editor_gl_free();

    vector_ptr_glitter_scene_free(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_effect_group_free(&GPM_VAL->effect_groups, glitter_effect_group_dispose);

    glitter_editor_enabled = false;
    glitter_editor.test = false;
    glitter_editor.imgui_focus = false;
    glitter_editor.create_popup = false;
    glitter_editor.load = false;
    glitter_editor.load_popup = false;
    glitter_editor.load_error_list_popup = false;
    glitter_editor.save = false;
    glitter_editor.save_popup = false;
    glitter_editor.save_compress = false;
    glitter_editor.close = false;
    glitter_editor.input_play = false;
    glitter_editor.input_reload = false;
    glitter_editor.input_pause = true;
    glitter_editor.input_reset = false;
    glitter_editor.effect_group_add = false;
    glitter_editor.draw_flags = 0;
    glitter_editor.resource_flags = 0;
    glitter_editor.effect_flags = 0;
    glitter_editor.emitter_flags = 0;
    glitter_editor.particle_flags = 0;
    glitter_editor.frame_counter = 0;
    glitter_editor.old_frame_counter = 0;
    glitter_editor.delta_frame = 0.0;
    glitter_editor.start_frame = 0;
    glitter_editor.end_frame = 0;
    glitter_editor.random.value = 0;
    glitter_editor.random.step = 1;
    glitter_editor.counter = 0;
    glitter_editor.effect_group = 0;
    glitter_editor.scene = 0;
    memset(glitter_editor.file, 0, sizeof(glitter_editor.file));
    glitter_editor.dispose = false;
    glitter_editor.disposed = true;
}

void glitter_editor_init() {
    if (glitter_editor_enabled || glitter_editor.dispose)
        glitter_editor_dispose();

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    GPM_VAL->random.value = (uint32_t)(time.LowPart * hash_fnv1a64_empty);
    GPM_VAL->random.step = 1;

    QueryPerformanceCounter(&time);
    GPM_VAL->counter = (uint32_t)(time.LowPart * hash_murmurhash_empty);
    glitter_editor_enabled = true;
    glitter_editor.test = false;
    glitter_editor.draw_flags = 0;
    GPM_VAL->emission = 1.0f;
    GPM_VAL->draw_all = true;
    GPM_VAL->draw_all_mesh = true;
    grid_3d = true;

    gl_object_data gl_obj;
    memset(&gl_obj, 0, sizeof(gl_object_data));
    gl_obj.cull_face = gl_object_cull_face_default;

    task_render task;
    memset(&task, 0, sizeof(task_render));
    task.type = TASK_RENDER_UPDATE;
    task.update.type = TASK_RENDER_UPDATE_GL_OBJECT;
    task.update.hash = hash_char("Glitter Editor Dummy");
    task.update.gl_obj = gl_obj;
    vector_task_render_push_back(&tasks_render, &task);
    glitter_editor_reset();
    glitter_editor_gl_load();
    /*return;

    wchar_t* path_x = L"X\\";
    vector_ptr_wchar_t files_x = { 0, 0, 0 };
    path_wget_files(&files_x, path_x);
    for (wchar_t** i = files_x.begin; i != files_x.end;)
        if (str_utils_wcheck_ends_with(*i, L".farc"))
            i++;
        else
            vector_ptr_wchar_t_erase(&files_x, i - files_x.begin, 0);

    ssize_t c = files_x.end - files_x.begin;
    glitter_file_reader* fr = 0;
    if (files_x.begin)
        for (ssize_t i = 0; i < c; i++) {
            if (!files_x.begin[i])
                continue;

            wchar_t* file_x = str_utils_wget_without_extension(files_x.begin[i]);

            wchar_t buf[0x100];
            swprintf_s(buf, 0x100, L"%ls\n", file_x);
            OutputDebugStringW(buf);

            glitter_editor.load_glt_type = GLITTER_X;
            fr = glitter_file_reader_winit(GLITTER_X, path_x, file_x, 1.0f);
            glitter_file_reader_read(fr, GPM_VAL->emission);

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
                        wchar_t buf[0x100];
                        swprintf_s(buf, 0x100, L"%08llX %d\n", e->data.name_hash, ea->instance_id);
                        OutputDebugStringW(buf);
                    }
                }

                glitter_effect_group_dispose(fr->effect_group);
            }
            free(file_x);
        }
    glitter_file_reader_dispose(fr);
    glitter_editor.effect_group = 0;*/
    return;

    wchar_t* path_f2 = L"F2\\";
    wchar_t* path_aft = L"AFT\\";
    vector_ptr_wchar_t files_f2 = { 0, 0, 0 };
    vector_ptr_wchar_t files_aft = { 0, 0, 0 };
    path_wget_files(&files_f2, path_f2);
    path_wget_files(&files_aft, path_aft);
    for (wchar_t** i = files_f2.begin; i != files_f2.end;)
        if (str_utils_wcheck_ends_with(*i, L".farc"))
            i++;
        else
            vector_ptr_wchar_t_erase(&files_f2, i - files_f2.begin, 0);

    for (wchar_t** i = files_aft.begin; i != files_aft.end;)
        if (str_utils_wcheck_ends_with(*i, L".farc"))
            i++;
        else
            vector_ptr_wchar_t_erase(&files_aft, i - files_aft.begin, 0);

    stream* s = io_wopen(L"name_F2nd.glitter.txt", L"rb");
    size_t length = s->length;
    uint8_t* data = force_malloc(length);
    io_read(s, data, length);
    io_dispose(s);

    char* buf;
    char** lines;
    size_t count;
    if (glitter_editor_list_parse(data, length, &buf, &lines, &count)) {
        uint64_t* hashes = force_malloc_s(uint64_t, count);
        for (size_t i = 0; i < count; i++)
            hashes[i] = hash_murmurhash(lines[i], min(strlen(lines[i]), 0x7F), 0, false, false);

        ssize_t c = files_f2.end - files_f2.begin;
        glitter_file_reader* fr = 0;
        if (files_f2.begin && files_aft.begin)
            for (ssize_t i = 0; i < c; i++) {
                if (!files_f2.begin[i])
                    continue;

                wchar_t* file_f2 = str_utils_wget_without_extension(files_f2.begin[i]);
                wchar_t* file_aft = str_utils_wget_without_extension(files_aft.begin[i]);
                glitter_editor.load_glt_type = GLITTER_F2;
                fr = glitter_file_reader_winit(GLITTER_F2, path_f2, file_f2, 1.0f);
                glitter_file_reader_read(fr, GPM_VAL->emission);

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
                        memcpy(e->name, lines[j], min(strlen(lines[j]), 0x7F));

                        for (glitter_emitter** j = e->emitters.begin;
                            j != e->emitters.end; j++) {
                            if (!*j)
                                continue;

                            glitter_emitter* emitter = *j;
                            for (glitter_particle** k = emitter->particles.begin;
                                k != emitter->particles.end; k++) {
                                if (!*k)
                                    continue;

                                glitter_particle* particle = *k;
                                if (particle->data.deceleration != 0.0f
                                    || particle->data.deceleration_random != 0.0f) {
                                    char buf[0x100];
                                    snprintf(buf, 0x100, particle->data.deceleration_random != 0.0f
                                        ? "DR %s %lld %lld %g %g\n" : "D  %s %lld %lld %g\n", e->name,
                                        j - e->emitters.begin, k - emitter->particles.begin,
                                        particle->data.deceleration, particle->data.deceleration_random);
                                    OutputDebugStringA(buf);
                                }
                            }
                        }
                    }

                    glitter_editor.effect_group = fr->effect_group;
                    //glitter_editor.save_glt_type = GLITTER_AFT;
                    //glitter_editor_save_file(path_aft, file_aft);
                    glitter_effect_group_dispose(fr->effect_group);
                }
                free(file_f2);
                free(file_aft);
            }
        glitter_file_reader_dispose(fr);

        free(buf);
        free(lines);
        free(hashes);
    }
    free(data);
    glitter_editor.effect_group = 0;
}

void glitter_editor_draw() {
    glitter_editor_gl_draw();
}

void glitter_editor_drop(size_t count, wchar_t** paths) {
    if (!glitter_editor_enabled)
        return;

    if (count < 1 || !paths[0])
        return;

    memcpy(glitter_editor.file, paths[0],
        min((wcslen(paths[0]) + 1) * sizeof(wchar_t), sizeof(glitter_editor.file)));
    glitter_editor.load_popup = true;
}

void glitter_editor_imgui() {
    if (!glitter_editor_enabled) {
        if (!glitter_editor.disposed)
            glitter_editor.dispose = true;
        return;
    }
    else if (glitter_editor.disposed) {
        glitter_editor_enabled = false;
        glitter_editor.disposed = false;
        return;
    }

    glitter_editor.imgui_focus = false;
    if (glitter_editor.test)
        glitter_editor_test_window();
    else
        glitter_editor_windows();
    glitter_editor_popups();
}

void glitter_editor_input() {
    if (!glitter_editor_enabled)
        return;

    input_locked |= glitter_editor.imgui_focus;

    ImGuiIO* io = igGetIO();
    if (io->WantTextInput)
        return;

    if (input_is_down(VK_CONTROL) && input_is_tapped('O'))
        glitter_editor_open_window();
    else if (input_is_down(VK_CONTROL) && input_is_tapped(VK_F4))
        glitter_editor.close = true;
    else if (input_is_tapped('P'))
        glitter_editor.test ^= true;
    else if (input_is_tapped('T'))
        glitter_editor.input_play = true;
    else if (input_is_tapped('Y'))
        glitter_editor.input_reload = true;
    else if (input_is_tapped('F'))
        glitter_editor.input_pause = true;
    else if (input_is_tapped('G'))
        glitter_editor.input_reset = true;

    if (glitter_editor.imgui_focus) {
        if (input_is_down(VK_CONTROL) && input_is_down(VK_SHIFT) && input_is_tapped('S'))
            glitter_editor_save_as_window();
        else if (input_is_down(VK_CONTROL) && input_is_tapped('S'))
            glitter_editor_save_window();
    }
}

void glitter_editor_render() {
    if (glitter_editor.dispose) {
        glitter_editor_dispose();
        return;
    }
    else if (!glitter_editor_enabled)
        return;

    if (glitter_editor.effect_group_add) {
        glitter_editor_reset_draw();
        glitter_editor.effect_group = glitter_effect_group_init(glitter_editor.load_glt_type);
        vector_ptr_glitter_effect_group_push_back(&GPM_VAL->effect_groups, &glitter_editor.effect_group);
    }

    bool has_resource = selected_resource != -1;
    bool has_effect = selected_effect != 0;
    bool has_emitter = selected_emitter != 0 && has_effect;
    bool has_particle = selected_particle != 0 && has_emitter;
    glitter_effect_group* eg = glitter_editor.effect_group;

    bool tex_reload = false;
    if (glitter_editor.resource_flags & GLITTER_EDITOR_IMPORT)
        if (glitter_editor_resource_import())
            tex_reload = true;

    if (has_resource) {
        if (glitter_editor.resource_flags & GLITTER_EDITOR_EXPORT)
            glitter_editor_resource_export();
        else if (glitter_editor.resource_flags & GLITTER_EDITOR_DELETE) {
            size_t rc = eg->resources_count;

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == selected_resource) {
                    j = i;
                    break;
                }

            if (j > -1) {
                uint64_t hash = eg->resource_hashes.begin[j];
                txp_free(&eg->resources_tex.begin[j]);
                vector_uint64_t_erase(&eg->resource_hashes, j);
                vector_txp_erase(&eg->resources_tex, j);
                eg->resources_count--;

                uint64_t empty_hash = eg->type != GLITTER_AFT
                    ? hash_murmurhash_empty : hash_fnv1a64_empty;
                for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    glitter_effect* effect = *i;
                    for (glitter_emitter** j = effect->emitters.begin; j != effect->emitters.end; j++) {
                        if (!*j)
                            continue;

                        glitter_emitter* emitter = *j;
                        for (glitter_particle** k = emitter->particles.begin; k != emitter->particles.end; k++) {
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
            selected_resource = -1;
            tex_reload = true;
        }
        else if (glitter_editor.resource_flags & GLITTER_EDITOR_MOVE_UP) {
            size_t rc = eg->resources_count;
            uint64_t* rh = eg->resource_hashes.begin;
            txp* rt = eg->resources_tex.begin;

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == selected_resource) {
                    j = i;
                    break;
                }

            if (j > 0) {
                uint64_t temp_hash = rh[j - 1];
                txp temp_txp = rt[j - 1];
                rh[j - 1] = rh[j];
                rt[j - 1] = rt[j];
                rh[j] = temp_hash;
                rt[j] = temp_txp;
                glitter_editor.input_reload = true;
            }
            tex_reload = true;
        }
        else if (glitter_editor.resource_flags & GLITTER_EDITOR_MOVE_DOWN) {
            size_t rc = eg->resources_count;
            uint64_t* rh = eg->resource_hashes.begin;
            txp* rt = eg->resources_tex.begin;

            ssize_t j = -1;
            for (size_t i = 0; i < rc; i++)
                if (i == selected_resource) {
                    j = i;
                    break;
                }

            if (j > -1 && j < (ssize_t)rc - 1) {
                uint64_t temp_hash = rh[j + 1];
                txp temp_txp = rt[j + 1];
                rh[j + 1] = rh[j];
                rt[j + 1] = rt[j];
                rh[j] = temp_hash;
                rt[j] = temp_txp;
                glitter_editor.input_reload = true;
            }
            tex_reload = true;
        }
    }

    if (tex_reload && glitter_editor.effect_group) {
        glitter_texture_load(glitter_editor.effect_group);
        glitter_editor.input_reload = true;
    }

    if (eg && glitter_editor.effect_flags & GLITTER_EDITOR_ADD) {
        ssize_t eff_count = eg->effects.end - eg->effects.begin;

        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        glitter_effect* e = glitter_effect_init(eg->type);
        snprintf(e->name, sizeof(e->name), "eff_%08x",
            (uint32_t)((eff_count + 1) * time.LowPart * hash_murmurhash_empty));
        vector_ptr_glitter_effect_push_back(&eg->effects, &e);
        glitter_editor.input_reload = true;
    }

    if (has_effect)
        if (glitter_editor.effect_flags & GLITTER_EDITOR_DUPLICATE) {
            ssize_t eff_count = eg->effects.end - eg->effects.begin;

            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            glitter_effect* e = glitter_effect_copy(selected_effect);
            snprintf(e->name, sizeof(e->name), "eff_%08x",
                (uint32_t)((eff_count + 1) * time.LowPart * hash_murmurhash_empty));
            vector_ptr_glitter_effect_push_back(&eg->effects, &e);
            glitter_editor.input_reload = true;
        }
        else if ((glitter_editor.effect_flags & GLITTER_EDITOR_DELETE)) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++)
                if (*i && *i == selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > -1)
                vector_ptr_glitter_effect_erase(&eg->effects, j, glitter_effect_dispose);
            selected_type = GLITTER_EDITOR_SELECTED_NONE;
            selected_resource = -1;
            selected_effect = 0;
            selected_emitter = 0;
            selected_particle = 0;

            if (eg->effects.end - eg->effects.begin < 1)
                glitter_editor.close = true;
            else
                glitter_editor.input_reload = true;
        }
        else if (glitter_editor.effect_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin;
                i != eg->effects.end; i++)
                if (*i == selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > 0) {
                glitter_effect* temp = eg->effects.begin[j - 1];
                eg->effects.begin[j - 1] = eg->effects.begin[j];
                eg->effects.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }
        else if (glitter_editor.effect_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_effect** i = eg->effects.begin;
                i != eg->effects.end; i++)
                if (*i == selected_effect) {
                    j = i - eg->effects.begin;
                    break;
                }

            if (j > -1 && j < eg->effects.end - eg->effects.begin - 1) {
                glitter_effect* temp = eg->effects.begin[j + 1];
                eg->effects.begin[j + 1] = eg->effects.begin[j];
                eg->effects.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }
        else if (glitter_editor.emitter_flags & GLITTER_EDITOR_ADD) {
            glitter_emitter* e = glitter_emitter_init(eg->type);
            vector_ptr_glitter_emitter_push_back(&selected_effect->emitters, &e);
            glitter_editor.input_reload = true;
        }

    if (has_emitter)
        if (glitter_editor.emitter_flags & GLITTER_EDITOR_DUPLICATE) {
            glitter_emitter* e = glitter_emitter_copy(selected_emitter);
            vector_ptr_glitter_emitter_push_back(&selected_effect->emitters, &e);
            glitter_editor.input_reload = true;
        }
        else if (glitter_editor.emitter_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = -1;
            for (glitter_emitter** i = selected_effect->emitters.begin;
                i != selected_effect->emitters.end; i++)
                if (*i && *i == selected_emitter) {
                    j = i - selected_effect->emitters.begin;
                    break;
                }

            if (j > -1)
                vector_ptr_glitter_emitter_erase(&selected_effect->emitters, j, glitter_emitter_dispose);
            selected_type = GLITTER_EDITOR_SELECTED_EFFECT;
            selected_emitter = 0;
            selected_particle = 0;
            glitter_editor.input_reload = true;
        }
        else if (glitter_editor.emitter_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_emitter** i = selected_effect->emitters.begin;
                i != selected_effect->emitters.end; i++)
                if (*i == selected_emitter) {
                    j = i - selected_effect->emitters.begin;
                    break;
                }

            if (j > 0) {
                glitter_emitter* temp = selected_effect->emitters.begin[j - 1];
                selected_effect->emitters.begin[j - 1] = selected_effect->emitters.begin[j];
                selected_effect->emitters.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }
        else if (glitter_editor.emitter_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_emitter** i = selected_effect->emitters.begin;
                i != selected_effect->emitters.end; i++)
                if (*i == selected_emitter) {
                    j = i - selected_effect->emitters.begin;
                    break;
                }

            if (j > -1 && j < selected_effect->emitters.end - selected_effect->emitters.begin - 1) {
                glitter_emitter* temp = selected_effect->emitters.begin[j + 1];
                selected_effect->emitters.begin[j + 1] = selected_effect->emitters.begin[j];
                selected_effect->emitters.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }
        else if (glitter_editor.particle_flags & GLITTER_EDITOR_ADD) {
            glitter_particle* p = glitter_particle_init(eg->type);
            vector_ptr_glitter_particle_push_back(&selected_emitter->particles, &p);
            glitter_editor.input_reload = true;
        }

    if (has_particle)
        if (glitter_editor.particle_flags & GLITTER_EDITOR_DUPLICATE) {
            glitter_particle* p = glitter_particle_copy(selected_particle);
            vector_ptr_glitter_particle_push_back(&selected_emitter->particles, &p);
            glitter_editor.input_reload = true;
        }
        else if (glitter_editor.particle_flags & GLITTER_EDITOR_DELETE) {
            ssize_t j = (ssize_t)-1;
            for (glitter_particle** i = selected_emitter->particles.begin;
                i != selected_emitter->particles.end; i++)
                if (*i && *i == selected_particle) {
                    j = i - selected_emitter->particles.begin;
                    break;
                }

            if (j > -1)
                vector_ptr_glitter_particle_erase(&selected_emitter->particles, j, glitter_particle_dispose);
            selected_type = GLITTER_EDITOR_SELECTED_EMITTER;
            selected_particle = 0;
            glitter_editor.input_reload = true;
        }
        else if (glitter_editor.particle_flags & GLITTER_EDITOR_MOVE_UP) {
            ssize_t j = -1;
            for (glitter_particle** i = selected_emitter->particles.begin;
                i != selected_emitter->particles.end; i++)
                if (*i == selected_particle) {
                    j = i - selected_emitter->particles.begin;
                    break;
                }

            if (j > 0) {
                glitter_particle* temp = selected_emitter->particles.begin[j - 1];
                selected_emitter->particles.begin[j - 1] = selected_emitter->particles.begin[j];
                selected_emitter->particles.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }
        else if (glitter_editor.particle_flags & GLITTER_EDITOR_MOVE_DOWN) {
            ssize_t j = -1;
            for (glitter_particle** i = selected_emitter->particles.begin;
                i != selected_emitter->particles.end; i++)
                if (*i == selected_particle) {
                    j = i - selected_emitter->particles.begin;
                    break;
                }

            if (j > -1 && j < selected_emitter->particles.end - selected_emitter->particles.begin - 1) {
                glitter_particle* temp = selected_emitter->particles.begin[j + 1];
                selected_emitter->particles.begin[j + 1] = selected_emitter->particles.begin[j];
                selected_emitter->particles.begin[j] = temp;
                glitter_editor.input_reload = true;
            }
        }

    glitter_editor.effect_group_add = false;
    glitter_editor.resource_flags = 0;
    glitter_editor.effect_flags = 0;
    glitter_editor.emitter_flags = 0;
    glitter_editor.particle_flags = 0;

    if (glitter_editor.load || glitter_editor.save) {
        wchar_t* file = str_utils_wget_without_extension(glitter_editor.file);
        wchar_t* path = str_utils_wsplit_right_get_left_include(glitter_editor.file, L'\\');

        if (glitter_editor.load)
            glitter_editor_load_file(path, file);
        else if (glitter_editor.save && glitter_editor.effect_group)
            glitter_editor_save_file(path, file);

        glitter_editor.load = false;
        glitter_editor.save = false;
        glitter_editor.save_compress = false;
        free(path);
        free(file);
        return;
    }
    else if (glitter_editor.close) {
        vector_ptr_glitter_scene_clear(&GPM_VAL->scenes, glitter_scene_dispose);
        vector_ptr_glitter_effect_group_clear(&GPM_VAL->effect_groups, glitter_effect_group_dispose);

        glitter_editor_reset_draw();
        glitter_editor.effect_group = 0;
        glitter_editor.scene = 0;
        memset(glitter_editor.file, 0, sizeof(glitter_editor.file));
        glitter_editor.close = false;
        return;
    }

    glitter_particle_manager_get_start_end_frame(GPM_VAL,
        &glitter_editor.start_frame, &glitter_editor.end_frame);

    if (!glitter_editor.effect_group) {
        glitter_editor.input_play = false;
        glitter_editor.input_reset = false;
        return;
    }

    if (!glitter_editor.input_pause && glitter_editor.old_frame_counter == glitter_editor.frame_counter) {
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        if (GPM_VAL->scenes.end != GPM_VAL->scenes.begin)
            glitter_editor.delta_frame += get_frame_speed();

        while (glitter_editor.delta_frame >= 1.0) {
            glitter_editor.frame_counter++;
            glitter_editor.delta_frame -= 1.0;
        }
    }

    if (glitter_editor.input_reload) {
        vector_ptr_glitter_scene_clear(&GPM_VAL->scenes, glitter_scene_dispose);
        glitter_editor.effect_group->emission = GPM_VAL->emission;
        glitter_particle_manager_set_frame(GPM_VAL, glitter_editor.effect_group,
            &glitter_editor.scene, (float_t)glitter_editor.frame_counter,
            (float_t)glitter_editor.old_frame_counter,
            glitter_editor.counter, &glitter_editor.random, true);
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        glitter_editor.input_reload = false;
    }

    if (glitter_editor.old_frame_counter != glitter_editor.frame_counter) {
        if (glitter_editor.end_frame > 0)
            while (glitter_editor.frame_counter >= glitter_editor.end_frame)
                glitter_editor.frame_counter -= glitter_editor.end_frame - glitter_editor.start_frame;

        if (glitter_editor.frame_counter < glitter_editor.start_frame)
            glitter_editor.frame_counter = glitter_editor.start_frame;

        glitter_particle_manager_set_frame(GPM_VAL, glitter_editor.effect_group,
            &glitter_editor.scene, (float_t)glitter_editor.frame_counter,
            (float_t)glitter_editor.old_frame_counter,
            glitter_editor.counter, &glitter_editor.random, false);
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
    }

    if (glitter_editor.input_play)
        glitter_editor.input_pause = false;
    else if (glitter_editor.input_reset) {
        GPM_VAL->counter = glitter_editor.counter;
        glitter_editor.frame_counter = glitter_editor.start_frame;
        glitter_particle_manager_set_frame(GPM_VAL, glitter_editor.effect_group,
            &glitter_editor.scene, (float_t)glitter_editor.frame_counter,
            (float_t)glitter_editor.old_frame_counter,
            glitter_editor.counter, &glitter_editor.random, true);
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        glitter_editor.input_pause = true;
    }
    glitter_editor.input_play = false;
    glitter_editor.input_reset = false;

    glitter_editor_gl_process();
}

static void glitter_editor_windows() {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    const float_t title_bar_size = font->FontSize + style->ItemSpacing.y + style->FramePadding.y * 2.0f;

    ImVec2 main_menu_bar_size = ImVec2_Empty;

    if (igBeginMainMenuBar()) {
        bool is_x = glitter_editor.effect_group
            && glitter_editor.effect_group->type == GLITTER_X ? true : false;
        if (igBeginMenu("File", true)) {
            if (igMenuItem_Bool("Open", "Ctrl+O", false, true))
                glitter_editor_open_window();
            if (igMenuItem_Bool("Save", "Ctrl+S", false, !is_x))
                glitter_editor_save_window();
            if (igMenuItem_Bool("Save As..", "Ctrl+Shift+S", false, !is_x))
                glitter_editor_save_as_window();
            if (igMenuItem_Bool("Close", "Ctrl+F4", false, true))
                glitter_editor.close = true;
            if (igMenuItem_Bool("Quit", 0, false, true))
                close = true;
            igEndMenu();
        }

        if (igBeginMenu("Edit", false)) {
            igEndMenu();
        }

        if (igBeginMenu("Mode", glitter_editor.effect_group ? true : false)) {
            glitter_effect_group* eg = glitter_editor.effect_group;
            if (eg->type != GLITTER_X) {
                if (igMenuItem_Bool("F2nd", 0, false, eg->type != GLITTER_F2)) {
                    eg->type = GLITTER_F2;
                    glitter_editor_reload();
                }

                if (igMenuItem_Bool("AFT", 0, false, eg->type != GLITTER_AFT)) {
                    eg->type = GLITTER_AFT;
                    glitter_editor_reload();
                }
            }
            else {
                if (igMenuItem_Bool("X", 0, false, eg->type != GLITTER_X)) {
                    eg->type = GLITTER_X;
                    glitter_editor_reload();
                }
            }
            igEndMenu();
        }

        if (igBeginMenu("View", glitter_editor.effect_group ? true : false)) {
            igCheckboxFlags_UintPtr("Draw Wireframe",
                (uint32_t*)&glitter_editor.draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
            igCheckboxFlags_UintPtr("Draw Selected",
                (uint32_t*)&glitter_editor.draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
            igEndMenu();
        }
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
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
    bool selected = selected_effect || selected_emitter || selected_particle ? true : false;

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

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Effects Window");
    if (igBegin("Effects", 0, window_flags)) {
        if (igBeginTabBar("Effects Tab", 0)) {
            if (igBeginTabItem("Effects", 0, 0)) {
                glitter_editor_effects_context_menu(0, 0, 0, 0, 0, 0, GLITTER_EDITOR_SELECTED_NONE);
                if (glitter_editor.effect_group)
                    glitter_editor_effects();
                igEndTabItem();
            }

            imguiDisableElementPush(glitter_editor.effect_group != 0);
            if (igBeginTabItem("Resources", 0, 0)) {
                glitter_editor_resources_context_menu(-1, 0, true);
                if (glitter_editor.effect_group)
                    glitter_editor_resources();
                igEndTabItem();
            }
            imguiDisableElementPop(glitter_editor.effect_group != 0);

            if (igBeginTabItem("Play Manager", 0, 0)) {
                glitter_editor_play_manager();
                igEndTabItem();
            }
            igEndTabBar();
        }
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
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

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Property Window");
    if (igBegin("Property", 0, window_flags)) {
        if (glitter_editor.effect_group && selected)
            glitter_editor_property();
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
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

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Selector Window");
    if (igBegin("Curve Editor Selector", 0, window_flags)) {
        if (glitter_editor.effect_group && selected)
            glitter_editor_curve_editor_selector();
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
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

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Window");
    if (igBegin("Curve Editor", 0, window_flags)) {
        if (glitter_editor.effect_group && selected)
            glitter_editor_curve_editor();
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
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

    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);

    igPushID_Str("Glitter Editor Curve Editor Property Window");
    if (igBegin("Curve Editor Property", 0, window_flags)) {
        if (glitter_editor.effect_group && selected)
            glitter_editor_curve_editor_property_window();
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
    }
    igEnd();
    igPopID();
}

static void glitter_editor_reload() {
    if (!glitter_editor.effect_group)
        return;

    glitter_editor.input_reload = true;
    glitter_effect_group* eg = glitter_editor.effect_group;
    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
        if (!*i)
            continue;

        glitter_effect* effect = *i;
        if (eg->type != GLITTER_AFT)
            effect->data.name_hash = hash_murmurhash(effect->name, min(strlen(effect->name), 0x7F), 0, false, false);
        else
            effect->data.name_hash = hash_fnv1a64(effect->name, min(strlen(effect->name), 0x7F));

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

static void glitter_editor_reset() {
    selected_type = GLITTER_EDITOR_SELECTED_NONE;
    selected_resource = -1;
    selected_effect = 0;
    selected_emitter = 0;
    selected_particle = 0;
    glitter_editor_curve_editor_reset_state(-1);
    glitter_editor_curve_editor_curves_reset();
    glitter_editor_reset_draw();
}

static void glitter_editor_reset_draw() {
    GPM_VAL->scene = 0;
    GPM_VAL->effect = 0;
    GPM_VAL->emitter = 0;
    GPM_VAL->particle = 0;
}

static void glitter_editor_save() {
    glitter_editor.save_popup = glitter_editor.effect_group
        && glitter_editor.effect_group->type == GLITTER_X ? false : true;
}

static void glitter_editor_open_window() {
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
        memcpy(glitter_editor.file, file, sizeof(file));
        glitter_editor.load_popup = true;
    }
    CoUninitialize();
}

static void glitter_editor_save_window() {
    if (!glitter_editor.effect_group || !glitter_editor.scene)
        return;

    if (glitter_editor.file[0])
        glitter_editor_save();
    else
        glitter_editor_save_as_window();
}

static void glitter_editor_save_as_window() {
    if (glitter_editor.effect_group
        && glitter_editor.effect_group->type == GLITTER_X) {
        glitter_editor.save_popup = false;
        return;
    }

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        glitter_editor.save_popup = false;
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
        memcpy(glitter_editor.file, file, sizeof(file));
        glitter_editor_save();
    }
    else
        glitter_editor.save_popup = false;
    CoUninitialize();
}

static void glitter_editor_load_file(wchar_t* path, wchar_t* file) {
    vector_ptr_glitter_scene_clear(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_effect_group_clear(&GPM_VAL->effect_groups, glitter_effect_group_dispose);

    glitter_editor_reset_draw();
    glitter_editor.effect_group = 0;
    glitter_editor.scene = 0;

    glitter_file_reader* fr = glitter_file_reader_winit(glitter_editor.load_glt_type, path, file, -1.0f);
    if (glitter_file_reader_read(fr, GPM_VAL->emission)) {
        if (fr->type == GLITTER_AFT) {
            bool lst_valid = true;
            glitter_effect_group* eg = fr->effect_group;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                if (!*i)
                    continue;

                glitter_effect* e = *i;
                if (e->data.name_hash != hash_fnv1a64(e->name, min(strlen(e->name), 0x80))) {
                    lst_valid = false;
                    break;
                }
            }

            if (!lst_valid && !glitter_editor_list_open_window(fr->effect_group)) {
                glitter_effect_group_dispose(fr->effect_group);
                glitter_editor.load_error_list_popup = true;
            }
            else
                goto add_effect_group;
        }
        else if (!glitter_editor_list_open_window(fr->effect_group)) {
            glitter_effect_group_dispose(fr->effect_group);
            glitter_editor.load_error_list_popup = true;
        }
        else {
        add_effect_group:
            glitter_editor_reset_draw();
            glitter_editor.counter = GPM_VAL->counter;
            glitter_editor.effect_group = fr->effect_group;
            glitter_editor.scene = glitter_scene_init(fr->effect_group);

            glitter_effect_group* eg = glitter_editor.effect_group;
            glitter_effect** i;
            size_t id = 1;
            for (i = eg->effects.begin, id = 1; i != eg->effects.end; i++, id++) {
                if (!*i)
                    continue;

                glitter_scene_init_effect(GPM_VAL, glitter_editor.scene, *i, &GPM_VAL->random, id, false);
            }

            vector_ptr_glitter_effect_group_push_back(&GPM_VAL->effect_groups, &glitter_editor.effect_group);
            vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &glitter_editor.scene);
            glitter_editor.frame_counter = 0;
            glitter_editor.old_frame_counter = 0;
            glitter_editor.input_pause = true;
        }
    }
    else if (fr->effect_group)
        glitter_effect_group_dispose(fr->effect_group);

    glitter_file_reader_dispose(fr);
    glitter_editor_reset();
}

static void glitter_editor_save_file(wchar_t* path, wchar_t* file) {
    glitter_type glt_type = glitter_editor.save_glt_type;

    f2_struct st;
    farc_file ff_drs;
    farc_file ff_dve;
    farc_file ff_lst;

    memset(&ff_drs, 0, sizeof(farc_file));
    memset(&ff_dve, 0, sizeof(farc_file));
    memset(&ff_lst, 0, sizeof(farc_file));

    farc* f = farc_init();
    if (glitter_diva_effect_unparse_file(glt_type, glitter_editor.effect_group, &st)) {
        f2_struct_write_memory(&st, &ff_dve.data, &ff_dve.size, true, false);
        ff_dve.name = str_utils_wadd(file, L".dve");
        f2_struct_free(&st);
    }
    else
        goto End;

    if (glitter_diva_resource_unparse_file(glitter_editor.effect_group, &st)) {
        f2_struct_write_memory(&st, &ff_drs.data, &ff_drs.size, true, false);
        ff_drs.name = str_utils_wadd(file, L".drs");
    }
    f2_struct_free(&st);

    if (glt_type == GLITTER_AFT) {
        if (glitter_diva_list_unparse_file(glitter_editor.effect_group, &st)) {
            f2_struct_write_memory(&st, &ff_lst.data, &ff_lst.size, true, false);
            ff_lst.name = str_utils_wadd(file, L".lst");
        }
        else {
            free(ff_dve.data);
            free(ff_dve.name);
            free(ff_drs.data);
            free(ff_drs.name);
            goto End;
        }
    }

    if (ff_drs.data)
        vector_farc_file_push_back(&f->files, &ff_drs);
    vector_farc_file_push_back(&f->files, &ff_dve);
    if (glt_type == GLITTER_AFT)
        vector_farc_file_push_back(&f->files, &ff_lst);

    farc_compress_mode mode;
    if (glitter_editor.save_compress)
        mode = glt_type != GLITTER_AFT ? FARC_COMPRESS_FARC_GZIP_AES : FARC_COMPRESS_FArC;
    else
        mode = FARC_COMPRESS_FArc;

    wchar_t* temp = str_utils_wadd(path, file);
    if (glt_type != GLITTER_AFT) {
        wchar_t* list_temp = str_utils_wadd(temp, L".glitter.txt");
        stream* s = io_wopen(list_temp, L"wb");
        if (s->io.stream) {
            glitter_effect_group* eg = glitter_editor.effect_group;
            for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++)
                if (*i) {
                    io_write(s, (*i)->name, strlen((*i)->name));
                    io_write_char(s, '\n');
                }
        }
        io_dispose(s);
        free(list_temp);
    }
    farc_wwrite(f, temp, mode, false);
    free(temp);

End:
    f2_struct_free(&st);
    farc_dispose(f);
}

static bool glitter_editor_list_open_window(glitter_effect_group* eg) {
    bool ret = false;
    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        return ret;

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
        stream* s = io_wopen(file, L"rb");
        size_t length = s->length;
        uint8_t* data = force_malloc(length);
        io_read(s, data, length);
        io_dispose(s);

        char* buf;
        char** lines;
        size_t count;
        if (glitter_editor_list_parse(data, length, &buf, &lines, &count)) {
            uint64_t empty_hash = eg->type != GLITTER_AFT
                ? hash_murmurhash_empty : hash_fnv1a64_empty;
            uint64_t* hashes = force_malloc_s(uint64_t, count);
            if (eg->type != GLITTER_AFT)
                for (size_t i = 0; i < count; i++)
                    hashes[i] = hash_murmurhash(lines[i], min(strlen(lines[i]), 0x7F), 0, false, false);
            else
                for (size_t i = 0; i < count; i++)
                    hashes[i] = hash_fnv1a64(lines[i], min(strlen(lines[i]), 0x7F));

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

                memcpy(e->name, lines[j], min(strlen(lines[j]), 0x7F));
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

static bool glitter_editor_list_parse(uint8_t* data, size_t length, char** buf, char*** lines, size_t* count) {
    if (!data || !length || !buf || !lines || !count)
        return false;

    char* d = data;
    bool del = false;
    size_t c;
    *buf = 0;
    *lines = 0;
    *count = 0;
    if (data[0] == 0x00)
        return false;
    else if (data[0] == 0xFF) {
        if (length == 1 || data[1] != 0xFE || length == 2)
            return false;

        wchar_t* w_d = (wchar_t*)data + 1;
        d = utf8_encode(w_d);
        length = strlen(d);
        del = true;
        goto decode_utf8_ansi;
    }
    else if (data[0] == 0xFE) {
        if (length == 1 || data[1] != 0xFF || length == 2)
            return false;

        length /= 2;
        wchar_t* w_d = (wchar_t*)data + 1;
        w_d = str_utils_wcopy(w_d);
        for (size_t i = 0; i < length; i++)
            w_d[i] = (wchar_t)reverse_endianness_uint16_t((uint16_t)w_d[i]);
        d = utf8_encode(w_d);
        length = strlen(d);
        del = true;
        free(w_d);
        goto decode_utf8_ansi;
    }
    else if (data[0] == 0xEF) {
        if (length == 1 || data[1] != 0xBB || length == 2 || data[2] != 0xBF || length == 3)
            return false;

        d += 3;
        length -= 3;
        goto decode_utf8_ansi;
    }
    else {
    decode_utf8_ansi:
        c = 1;
        bool lf;
        char ch;
        lf = false;
        for (size_t i = 0; i < length; i++) {
            ch = d[i];
            while ((ch == '\r' || ch == '\n') && ++i < length) {
                ch = d[i];
                lf = true;
            }

            if (lf && i < length) {
                c++;
                lf = false;
            }
        }

        lf = false;
        size_t* line_lengths = force_malloc_s(size_t, c);
        size_t* line_offsets = force_malloc_s(size_t, c);
        line_lengths[0] = 0;
        line_offsets[0] = 0;
        for (size_t i = 0, j = 0, l = 0; j < c; ) {
            ch = d[i];
            while ((ch == '\r' || ch == '\n') && ++i < length) {
                ch = d[i];
                lf = true;
            }

            if (i >= length) {
                line_lengths[j] = l;
                break;
            }
            else if (lf) {
                line_lengths[j++] = l;
                line_offsets[j] = i;
                l = 0;
                lf = false;
            }
            else {
                l++;
                i++;
            }
        }

        size_t buf_len = 0;
        for (size_t i = 0; i < c; i++)
            buf_len += line_lengths[i] + 1;

        *buf = force_malloc(buf_len);
        char* b = *buf;

        char** temp_lines = force_malloc_s(char*, c);
        for (size_t i = 0, l = 0; i < c; i++) {
            l = line_lengths[i];
            memcpy(b, d + line_offsets[i], l);
            b[l] = 0;

            char* t;
            if (t = strstr(b, "#(?)"))
                *t = 0;

            temp_lines[i] = b;
            b += l + 1;
        }
        *lines = temp_lines;
        *count = c;
        free(line_offsets);
        free(line_lengths);
    }

    if (del)
        free(d);
    return true;
}

static bool glitter_editor_resource_import() {
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

        uint64_t hash_aft = hash_wchar_t_fnv1a64(f);
        uint64_t hash_f2 = hash_wchar_t_murmurhash(f, 0, false);

        glitter_effect_group* eg = glitter_editor.effect_group;
        int32_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.begin;
        for (int32_t i = 0; i < rc; i++)
            if (rh[i] == hash_aft || rh[i] == hash_f2)
                goto DDSEnd;

        dds_wread(d, p);
        if (d->width == 0 || d->height == 0 || d->mipmaps_count == 0 || d->data.end - d->data.begin < 1)
            goto DDSEnd;

        txp tex;
        memset(&tex, 0, sizeof(txp));
        tex.array_size = d->has_cubemap ? 6 : 1;
        tex.has_cubemap = d->has_cubemap;
        tex.mipmaps_count = d->mipmaps_count;

        vector_txp_mipmap_append(&tex.data, (tex.has_cubemap ? 6LL : 1LL) * tex.mipmaps_count);
        uint32_t index = 0;
        do
            for (uint32_t i = 0; i < tex.mipmaps_count; i++) {
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
                vector_txp_mipmap_push_back(&tex.data, &tex_mip);
                index++;
            }
        while (index / tex.mipmaps_count < tex.array_size);

        eg->resources_count++;
        if (eg->type == GLITTER_AFT)
            vector_uint64_t_push_back(&eg->resource_hashes, &hash_aft);
        else
            vector_uint64_t_push_back(&eg->resource_hashes, &hash_f2);
        vector_txp_push_back(&eg->resources_tex, &tex);
        ret = true;
    DDSEnd:
        dds_dispose(d);
        free(f);
        free(p);
    }
    CoUninitialize();
    return ret;
}

static bool glitter_editor_resource_export() {
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

        glitter_effect_group* eg = glitter_editor.effect_group;
        txp tex = eg->resources_tex.begin[selected_resource];

        txp_format format = tex.data.begin[0].format;
        uint32_t width = tex.data.begin[0].width;
        uint32_t height = tex.data.begin[0].height;

        d->format = format;
        d->width = width;
        d->height = height;
        d->mipmaps_count = tex.mipmaps_count;
        d->has_cubemap = tex.has_cubemap;
        d->data = (vector_ptr_void){ 0, 0, 0 };

        vector_ptr_void_append(&d->data, (tex.has_cubemap ? 6LL : 1LL) * tex.mipmaps_count);
        uint32_t index = 0;
        do
            for (uint32_t i = 0; i < tex.mipmaps_count; i++) {
                uint32_t size = txp_get_size(format,
                    max(width >> i, 1), max(height >> i, 1));
                void* data = force_malloc(size);
                memcpy(data, tex.data.begin[index].data, size);
                vector_ptr_void_push_back(&d->data, &data);
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

static void glitter_editor_test_window() {
    glitter_effect_group* eg = glitter_editor.effect_group;

    float_t w = min((float_t)width / 4.0f, 360.0f);
    float_t h = 424.0f;

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_FirstUseEver, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_FirstUseEver);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    bool test = glitter_editor.test;
    if (!igBegin("Glitter Editor Test Window", &test, window_flags)) {
        glitter_editor.test = false;
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEnd();
        return;
    }

    if (imguiButton("Reset Camera (R)", ImVec2_Empty))
        input_reset = true;

    ImVec2 t;
    igGetContentRegionAvail(&t);
    if (igBeginTable("buttons", 2, 0, ImVec2_Empty, 0.0f)) {
        igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, t.x * 0.5f, 0);

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Play (T)", (ImVec2) { t.x, 0.0f }) || igIsKeyPressed(GLFW_KEY_T, true))
            glitter_editor.input_play = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Reload (Y)", (ImVec2) { t.x, 0.0f }) || igIsKeyPressed(GLFW_KEY_Y, true))
            glitter_editor.input_reload = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Pause (F)", (ImVec2) { t.x, 0.0f }) || igIsKeyPressed(GLFW_KEY_F, true))
            glitter_editor.input_pause = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Reset (G)", (ImVec2) { t.x, 0.0f }) || igIsKeyPressed(GLFW_KEY_G, true))
            glitter_editor.input_reset = true;
        igEndTable();
    }

    igSeparator();

    int32_t frame_counter = glitter_editor.frame_counter;
    if (imguiColumnSliderInt("Frame", &frame_counter, glitter_editor.start_frame,
        glitter_editor.end_frame, "%d", 0)) {
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        glitter_editor.frame_counter = frame_counter;
    }

    igText("Start/End Frame: %d/%d", glitter_editor.start_frame, glitter_editor.end_frame);

    igSeparator();


    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    frame = 0.0f;
    life_time = 0;
    glitter_particle_manager_get_frame(GPM_VAL, &frame, &life_time);
    igText("%d - %.0f/%d", glitter_editor.frame_counter - glitter_editor.start_frame, frame, life_time);

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

    imguiCheckbox("Draw All", &GPM_VAL->draw_all);

    imguiDisableElementPush(GPM_VAL->draw_all && eg && eg->type == GLITTER_X);
    imguiCheckbox("Draw All Mesh", &GPM_VAL->draw_all_mesh);
    imguiDisableElementPop(GPM_VAL->draw_all && eg && eg->type == GLITTER_X);

    igSeparator();

    igCheckboxFlags_UintPtr("Draw Wireframe",
        (uint32_t*)&glitter_editor.draw_flags, GLITTER_EDITOR_DRAW_WIREFRAME);
    igCheckboxFlags_UintPtr("Draw Selected",
        (uint32_t*)&glitter_editor.draw_flags, GLITTER_EDITOR_DRAW_SELECTED);
    glitter_editor.imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

static void glitter_editor_effects() {
    char buf[0x100];

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    glitter_effect_group* eg = glitter_editor.effect_group;
    igPushID_Ptr(eg);
    ssize_t i_idx = 1;
    for (glitter_effect** i = eg->effects.begin;
        i != eg->effects.end; i++, i_idx++) {
        if (!*i)
            continue;

        glitter_effect* effect = *i;
        tree_node_flags = tree_node_base_flags;
        if (effect == selected_effect && !selected_emitter && !selected_particle)
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
            effect->name, (uint32_t)((size_t)effect * hash_fnv1a64_empty))) {
            glitter_editor_effects_context_menu(effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);

            ssize_t j_idx = 1;
            for (glitter_emitter** j = effect->emitters.begin;
                j != effect->emitters.end; j++, j_idx++) {
                if (!*j)
                    continue;

                glitter_emitter* emitter = *j;
                tree_node_flags = tree_node_base_flags;
                if (emitter == selected_emitter && !selected_particle)
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
                    j_idx, (uint32_t)((size_t)emitter * hash_fnv1a64_empty))) {
                    glitter_editor_effects_context_menu(effect, emitter, 0,
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
                            k_idx, (uint32_t)((size_t)particle * hash_fnv1a64_empty));
                        igSelectable_Bool(buf, particle == selected_particle, 0, (ImVec2) { 0.0f, 0.0f });
                        glitter_editor_effects_context_menu(effect, emitter, particle,
                            i_idx, j_idx, k_idx, GLITTER_EDITOR_SELECTED_PARTICLE);
                        igPopID();
                    }
                    igTreePop();
                }
                else
                    glitter_editor_effects_context_menu(effect, emitter, 0,
                        i_idx, j_idx, 0, GLITTER_EDITOR_SELECTED_EMITTER);
                igPopID();
            }
            igTreePop();
        }
        else
            glitter_editor_effects_context_menu(effect, 0, 0,
                i_idx, 0, 0, GLITTER_EDITOR_SELECTED_EFFECT);
        igPopID();
    }
    igPopID();
}

static void glitter_editor_effects_context_menu(glitter_effect* effect,
    glitter_emitter* emitter, glitter_particle* particle,
    ssize_t i_idx, ssize_t j_idx, ssize_t k_idx,
    glitter_editor_selected_enum type) {
    bool focused = igIsItemFocused();

    static void* selected;
    if (type == GLITTER_EDITOR_SELECTED_NONE) {
        if (igIsWindowHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
            igOpenPopup("effects popup", ImGuiPopupFlags_MouseButtonRight);
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
        igOpenPopup("effects popup", ImGuiPopupFlags_MouseButtonRight);
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

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    if (!igBeginPopup("effects popup", window_flags))
        goto End;

    global_context_menu = false;

    bool close = false;
    switch (type) {
    case GLITTER_EDITOR_SELECTED_NONE: {
        if (igMenuItem_Bool("Add Effect", 0, false, true)) {
            if (!glitter_editor.effect_group)
                glitter_editor.create_popup = true;
            else
                glitter_editor.effect_flags |= GLITTER_EDITOR_ADD;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_EFFECT: {
        if (!glitter_editor.effect_group)
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

        glitter_effect_group* eg = glitter_editor.effect_group;
        igText(eff_str, effect->name, (uint32_t)((size_t)effect * hash_fnv1a64_empty));
        igSeparator();
        if (igMenuItem_Bool("Duplicate Effect", 0, false, true)) {
            glitter_editor.effect_flags |= GLITTER_EDITOR_DUPLICATE;
            selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Delete Effect", 0, false, true)) {
            glitter_editor.effect_flags |= GLITTER_EDITOR_DELETE;
            selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Move Effect Up", 0, false, i_idx > 1)) {
            glitter_editor.effect_flags |= GLITTER_EDITOR_MOVE_UP;
            selected_effect = effect;
            close = true;
        }

        if (igMenuItem_Bool("Move Effect Down", 0, false,
            i_idx < eg->effects.end - eg->effects.begin)) {
            glitter_editor.effect_flags |= GLITTER_EDITOR_MOVE_DOWN;
            selected_effect = effect;
            close = true;
        }

        igSeparator();

        if (igMenuItem_Bool("Add Emitter", 0, false, true)) {
            glitter_editor.emitter_flags |= GLITTER_EDITOR_ADD;
            selected_effect = effect;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_EMITTER: {
        if (!selected_effect)
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

        igText(emit_str, j_idx, (size_t)emitter * hash_fnv1a64_empty);
        igSeparator();
        if (igMenuItem_Bool("Duplicate Emitter", 0, false, true)) {
            glitter_editor.emitter_flags |= GLITTER_EDITOR_DUPLICATE;
            selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Delete Emitter", 0, false, true)) {
            glitter_editor.emitter_flags |= GLITTER_EDITOR_DELETE;
            selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Move Emitter Up", 0, false, j_idx > 1)) {
            glitter_editor.emitter_flags |= GLITTER_EDITOR_MOVE_UP;
            selected_emitter = emitter;
            close = true;
        }

        if (igMenuItem_Bool("Move Emitter Down", 0, false,
            j_idx < selected_effect->emitters.end - selected_effect->emitters.begin)) {
            glitter_editor.emitter_flags |= GLITTER_EDITOR_MOVE_DOWN;
            selected_emitter = emitter;
            close = true;
        }

        igSeparator();

        if (igMenuItem_Bool("Add Particle", 0, false, true)) {
            glitter_editor.particle_flags |= GLITTER_EDITOR_ADD;
            selected_emitter = emitter;
            close = true;
        }
    } break;
    case GLITTER_EDITOR_SELECTED_PARTICLE: {
        if (!selected_emitter)
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

        igText(ptcl_str, k_idx, (uint32_t)((size_t)particle * hash_fnv1a64_empty));
        igSeparator();
        if (igMenuItem_Bool("Duplicate Particle", 0, false, true)) {
            glitter_editor.particle_flags |= GLITTER_EDITOR_DUPLICATE;
            selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Delete Particle", 0, false, true)) {
            glitter_editor.particle_flags |= GLITTER_EDITOR_DELETE;
            selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Move Particle Up", 0, false, k_idx > 1)) {
            glitter_editor.particle_flags |= GLITTER_EDITOR_MOVE_UP;
            selected_particle = particle;
            close = true;
        }

        if (igMenuItem_Bool("Move Particle Down", 0, false,
            k_idx < selected_emitter->particles.end - selected_emitter->particles.begin)) {
            glitter_editor.particle_flags |= GLITTER_EDITOR_MOVE_DOWN;
            selected_particle = particle;
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
        if (selected_type != type || selected_effect != effect)
            glitter_editor_reset();
        selected_effect = effect;
        selected_emitter = 0;
        selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        if (selected_type != type || selected_emitter != emitter)
            glitter_editor_reset();
        selected_effect = effect;
        selected_emitter = emitter;
        selected_particle = 0;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (selected_type != type || selected_particle != particle)
            glitter_editor_reset();
        selected_effect = effect;
        selected_emitter = emitter;
        selected_particle = particle;
        break;
    }
    selected_type = type;
}

static void glitter_editor_resources() {
    char buf[0x100];

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    glitter_effect_group* eg = glitter_editor.effect_group;
    int32_t* r = eg->resources.begin;
    int32_t rc = eg->resources_count;
    uint64_t* rh = eg->resource_hashes.begin;
    txp* rt = eg->resources_tex.begin;
    igPushID_Ptr(eg);
    ssize_t i_idx = 1;
    for (int32_t i = 0; i < rc; i++, i_idx++) {
        snprintf(buf, sizeof(buf), "Texture %d (%016llX)", i + 1, rh[i]);
        igPushID_Int(i);
        igSelectable_Bool(buf, i == selected_resource, 0, (ImVec2) { 0.0f, 0.0f });
        if (igIsItemHovered(0)) {
            igBeginTooltip();
            igText("Tex Size: %dx%d", rt[i].data.begin[0].width, rt[i].data.begin[0].height);
            igImage((void*)(size_t)r[i], (ImVec2) { 192.0f, 192.0f },
                ImVec2_Empty, ImVec2_Identity, tint_col, border_col);
            igEndTooltip();
        }
        glitter_editor_resources_context_menu(i, i_idx, false);
        igPopID();
    }
    igPopID();
}

static void glitter_editor_resources_context_menu(int32_t resource, ssize_t i_idx, bool selected_none) {
    bool focused = igIsItemFocused();

    static size_t selected;
    if (selected_none) {
        if (igIsWindowHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
            igOpenPopup("resources popup", ImGuiPopupFlags_MouseButtonRight);
            selected = -1;
        }
        else if (selected != -1)
            goto End;
    }
    else if (igIsItemHovered(0) && igIsMouseReleased(ImGuiMouseButton_Right)) {
        selected = resource;
        igOpenPopup("resources popup", ImGuiPopupFlags_MouseButtonRight);
    }
    else if (selected != resource)
        goto End;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

    if (!igBeginPopup("resources popup", window_flags))
        goto End;

    global_context_menu = false;

    bool close = false;
    if (selected_none) {
        if (igMenuItem_Bool("Import Resource", 0, false, true)) {
            glitter_editor.resource_flags |= GLITTER_EDITOR_IMPORT;
            close = true;
        }
    }
    else {
        size_t i = resource;
        glitter_effect_group* eg = glitter_editor.effect_group;
        ssize_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.begin;
        igText("Texture %lld (%016llX)", i + 1, rh[i]);
        igSeparator();
        if (igMenuItem_Bool("Export Resource", 0, false, true)) {
            glitter_editor.resource_flags |= GLITTER_EDITOR_EXPORT;
            selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Delete Resource", 0, false, true)) {
            glitter_editor.resource_flags |= GLITTER_EDITOR_DELETE;
            selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Move Resource Up", 0, false, i_idx > 1)) {
            glitter_editor.resource_flags |= GLITTER_EDITOR_MOVE_UP;
            selected_resource = resource;
            close = true;
        }

        if (igMenuItem_Bool("Move Resource Down", 0, false,
            i_idx < rc)) {
            glitter_editor.resource_flags |= GLITTER_EDITOR_MOVE_DOWN;
            selected_resource = resource;
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

    selected_resource = resource;
}

static void glitter_editor_play_manager() {
    glitter_effect_group* eg = glitter_editor.effect_group;

    if (imguiButton("Reset Camera", ImVec2_Empty))
        input_reset = true;

    ImVec2 t;
    igGetContentRegionAvail(&t);
    if (igBeginTable("buttons", 2, 0, ImVec2_Empty, 0.0f)) {
        igTableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, t.x * 0.5f, 0);

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Play (T)", (ImVec2) { t.x, 0.0f }))
            glitter_editor.input_play = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Reload (Y)", (ImVec2) { t.x, 0.0f }))
            glitter_editor.input_reload = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Pause (F)", (ImVec2) { t.x, 0.0f }))
            glitter_editor.input_pause = true;

        igTableNextColumn();
        igGetContentRegionAvail(&t);
        if (imguiButton("Reset (G)", (ImVec2) { t.x, 0.0f }))
            glitter_editor.input_reset = true;
        igEndTable();
    }

    igSeparator();

    int32_t frame_counter = glitter_editor.frame_counter;
    if (imguiColumnSliderInt("Frame", &frame_counter, glitter_editor.start_frame,
        glitter_editor.end_frame, "%d", 0)) {
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        glitter_editor.frame_counter = frame_counter;
    }

    igText("Start/End Frame: %d/%d", glitter_editor.start_frame, glitter_editor.end_frame);

    igSeparator();


    size_t ctrl;
    size_t disp;
    float_t frame;
    int32_t life_time;

    frame = 0.0f;
    life_time = 0;
    glitter_particle_manager_get_frame(GPM_VAL, &frame, &life_time);
    igText("%d - %.0f/%d", glitter_editor.frame_counter - glitter_editor.start_frame, frame, life_time);

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

    imguiCheckbox("Draw All", &GPM_VAL->draw_all);

    imguiDisableElementPush(GPM_VAL->draw_all && eg && eg->type == GLITTER_X);
    imguiCheckbox("Draw All Mesh", &GPM_VAL->draw_all_mesh);
    imguiDisableElementPop(GPM_VAL->draw_all && eg && eg->type == GLITTER_X);
}

static void glitter_editor_property() {
    switch (selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        glitter_editor_property_effect();
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        glitter_editor_property_emitter();
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        glitter_editor_property_particle();
        break;
    }
}

static void glitter_editor_property_effect() {
    imguiSetColumnSpace(2.0f / 5.0f);

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_effect effect = *selected_effect;
    bool changed = false;

    glitter_curve_type_flags flags = glitter_effect_curve_flags;
    for (glitter_curve_type i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (glitter_curve** j = effect.animation.begin; j != effect.animation.end; j++)
            if (*j && (*j)->type == i)
                flags &= ~(1 << i);

    const size_t name_size = sizeof(effect.name);
    char name[sizeof(effect.name)];
    memcpy(name, effect.name, name_size);
    if (imguiColumnInputText("Name", name, name_size, 0, 0, 0)) {
        effect.data.name_hash = eg->type != GLITTER_AFT
            ? hash_char_murmurhash(name, 0, false) : hash_char_fnv1a64(name);
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

    if (imguiCheckboxFlags_UintPtr("Alpha",
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
        effect.data.flags &= ~GLITTER_EFFECT_FOG_HEIGHT;
        changed = true;
    }

    if (effect.data.flags & GLITTER_EFFECT_FOG_HEIGHT && effect.data.flags & GLITTER_EFFECT_FOG) {
        effect.data.flags &= ~GLITTER_EFFECT_FOG;
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
        4, &type, 0, false, &glitter_editor.imgui_focus)) {
        if (type == 1) {
            effect.data.flags |= GLITTER_EFFECT_LOCAL;
            free(effect.data.ext_anim);
        }
        else {
            effect.data.flags &= ~GLITTER_EFFECT_LOCAL;
            if (type == 2 || type == 3) {
                if (!effect.data.ext_anim)
                    effect.data.ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
                if (type == 2)
                    effect.data.ext_anim->flags |= GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM;
                else
                    effect.data.ext_anim->flags &= ~GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM;
            }
            else
                free(effect.data.ext_anim);
        }
        changed = true;
    }

    if (~effect.data.flags & GLITTER_EFFECT_LOCAL && effect.data.ext_anim) {
        igSeparator();

        bool ext_anim_changed = false;
        glitter_effect_ext_anim ext_anim = *effect.data.ext_anim;
        if (eg->type == GLITTER_X) {
            if (ext_anim.flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = 4;
                if (imguiColumnComboBox("Chara Index", glitter_effect_ext_anim_index_name,
                    max_chara, &ext_anim.index, 0, false, &glitter_editor.imgui_focus))
                    ext_anim_changed = true;

                int32_t node_index = ext_anim.node_index;
                if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", glitter_effect_ext_anim_node_index_name,
                    GLITTER_EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &glitter_editor.imgui_focus)) {
                    node_index--;
                    if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim.node_index != node_index) {
                        ext_anim.node_index = node_index;
                        ext_anim_changed = true;
                    }
                }
            }
            else {
                uint32_t object_hash = (uint32_t)ext_anim.object_hash;
                if (glitter_editor_hash_input("Object Hash", &ext_anim.object_hash))
                    ext_anim_changed = true;

                imguiColumnInputScalar("Instance ID", ImGuiDataType_U32, &ext_anim.instance_id,
                    0, 0, "%d", ImGuiInputTextFlags_CharsDecimal);

                if (glitter_editor_hash_input("File Name Hash", &ext_anim.file_name_hash))
                    ext_anim_changed = true;

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
        }
        else {
            if (ext_anim.flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
                const int32_t max_chara = eg->type == GLITTER_AFT ? 6 : 3;
                if (imguiColumnComboBox("Chara Index", glitter_effect_ext_anim_index_name,
                    max_chara, &ext_anim.index, 0, false, &glitter_editor.imgui_focus))
                    ext_anim_changed = true;

                int32_t node_index = ext_anim.node_index;
                if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                    || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                    node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;
                node_index++;

                if (imguiColumnComboBox("Parent Bone", glitter_effect_ext_anim_node_index_name,
                    GLITTER_EFFECT_EXT_ANIM_CHARA_MAX,
                    &node_index, 0, true, &glitter_editor.imgui_focus)) {
                    node_index--;
                    if (node_index < GLITTER_EFFECT_EXT_ANIM_CHARA_HEAD
                        || node_index > GLITTER_EFFECT_EXT_ANIM_CHARA_RIGHT_TOE)
                        node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_NONE;

                    if (ext_anim.node_index != node_index) {
                        ext_anim.node_index = node_index;
                        ext_anim_changed = true;
                    }
                }
            }
            else {
                if (glitter_editor_hash_input("Object Hash", &ext_anim.object_hash))
                    ext_anim_changed = true;

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
        }

        uint64_t hash1 = hash_fnv1a64((void*)effect.data.ext_anim, sizeof(glitter_effect_ext_anim));
        uint64_t hash2 = hash_fnv1a64((void*)&ext_anim, sizeof(glitter_effect_ext_anim));
        if (hash1 != hash2 && ext_anim_changed) {
            *effect.data.ext_anim = ext_anim;
            changed = true;
        }
    }

    uint64_t hash1 = hash_fnv1a64((void*)selected_effect, sizeof(glitter_effect));
    uint64_t hash2 = hash_fnv1a64((void*)&effect, sizeof(glitter_effect));
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
                    flags |= GLITTER_PARTICLE_LOCAL;
                else
                    flags &= ~GLITTER_PARTICLE_LOCAL;

                if (effect.data.flags & GLITTER_EFFECT_EMISSION
                    || particle->data.emission >= glitter_min_emission)
                    flags |= GLITTER_PARTICLE_EMISSION;
                else
                    flags &= ~GLITTER_PARTICLE_EMISSION;
                particle->data.flags = flags;
            }
        }
        glitter_editor.input_reload = true;
        *selected_effect = effect;
    }

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_emitter() {
    imguiSetColumnSpace(2.0f / 5.0f);

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_emitter emitter = *selected_emitter;
    bool changed = false;

    glitter_curve_type_flags flags = glitter_emitter_curve_flags;
    for (glitter_curve_type i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
        for (glitter_curve** j = emitter.animation.begin; j != emitter.animation.end; j++)
            if (*j && (*j)->type == i)
                flags &= ~(1 << i);

    igText("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        selected_effect->data.appear_time, selected_effect->data.life_time, selected_effect->data.start_time);

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
        changed = emitter.data.loop_start_time != selected_emitter->data.loop_start_time;
    }

    if (imguiColumnDragInt("Loop End Time", &emitter.data.loop_end_time,
        1.0f, -0x0001, emitter.data.start_time + emitter.data.life_time, "%d", 0)) {
        if (emitter.data.loop_end_time < 0)
            emitter.data.loop_end_time = -1;
        changed = emitter.data.loop_end_time != selected_emitter->data.loop_end_time;
    }

    igSeparator();

    if (imguiColumnDragVec3Flag("Translation",
        &emitter.translation, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat,
        flags & GLITTER_CURVE_TYPE_TRANSLATION_XYZ))
        changed = true;

    if (eg->type != GLITTER_X) {
        bool draw_z_axis =
            emitter.data.direction == GLITTER_DIRECTION_Z_AXIS;

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
            (int32_t*)&emitter.data.timer, 0, true, &glitter_editor.imgui_focus)) {
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
            (int32_t*)&emission, 0, true, &glitter_editor.imgui_focus)) {
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
            direction = i;
            break;
        }
    prev_direction = direction;
    
    if (imguiColumnComboBox("Direction", glitter_emitter_direction_name,
        GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION,
        (int32_t*)&direction, 0, true, &glitter_editor.imgui_focus))
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
        (int32_t*)&emitter.data.type, 0, true, &glitter_editor.imgui_focus))
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
            (int32_t*)&emitter.data.cylinder.direction, 0, true, &glitter_editor.imgui_focus))
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
            (int32_t*)&emitter.data.sphere.direction, 0, true, &glitter_editor.imgui_focus))
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
                (int32_t*)&emitter.data.cylinder.direction, 0, true, &glitter_editor.imgui_focus))
                changed = true;
        igPopID();
    } break;
    }

    uint64_t hash1 = hash_fnv1a64((void*)selected_emitter, sizeof(glitter_emitter));
    uint64_t hash2 = hash_fnv1a64((void*)&emitter, sizeof(glitter_emitter));
    if (hash1 != hash2 && changed) {
        glitter_editor.input_reload = true;
        *selected_emitter = emitter;
    }

    imguiSetDefaultColumnSpace();
}

static void glitter_editor_property_particle() {
    imguiSetColumnSpace(2.0f / 5.0f);

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_particle particle = *selected_particle;
    bool changed = false;

    glitter_curve_type_flags flags = 0;
    if (eg->type == GLITTER_X)
        flags = glitter_particle_x_curve_flags;
    else
        flags = glitter_particle_curve_flags;

    if (particle.data.type != GLITTER_PARTICLE_MESH) {
        flags &= ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND;
        if (particle.data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
            flags &= ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y);
    }

    if (particle.data.sub_flags & GLITTER_PARTICLE_SUB_USE_CURVE)
        for (glitter_curve_type i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++)
            for (glitter_curve** j = particle.animation.begin; j != particle.animation.end; j++)
                if (*j && (*j)->type == i)
                    flags &= ~(1 << i);

    igText("Parent Effect:\nAppear Time: %d; Life Time: %d; Start Time: %d",
        selected_effect->data.appear_time, selected_effect->data.life_time, selected_effect->data.start_time);

    igText("Parent Emitter:\nStart Time: %d; Life Time: %d\nLoop Start Time: %d; Loop End Time: %d",
        selected_emitter->data.start_time, selected_emitter->data.life_time,
        selected_emitter->data.loop_start_time, selected_emitter->data.loop_end_time);

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
            (int32_t*)&particle.data.type, 0, true, &glitter_editor.imgui_focus))
            changed = true;
    }
    else {
        if (imguiColumnDragInt("Life Time",
            &particle.data.life_time, 1.0f, 0x0000, 0x7FFF, "%d", 0))
            changed = true;

        if (imguiColumnComboBox("Type", glitter_particle_name,
            GLITTER_PARTICLE_LOCUS,
            (int32_t*)&particle.data.type, 0, true, &glitter_editor.imgui_focus))
            changed = true;
    }

    if (particle.data.type != GLITTER_PARTICLE_MESH
        && imguiColumnComboBox("Pivot", glitter_pivot_name,
            GLITTER_PIVOT_BOTTOM_RIGHT,
            (int32_t*)&particle.data.pivot, 0, true, &glitter_editor.imgui_focus))
        changed = true;

    if (particle.data.type == GLITTER_PARTICLE_QUAD || particle.data.type == GLITTER_PARTICLE_MESH) {
        glitter_particle_draw_type draw_type = glitter_particle_draw_type_default;
        glitter_particle_draw_type prev_draw_type;
        for (int32_t i = 0; i < glitter_particle_draw_types_count; i++)
            if (glitter_particle_draw_types[i] == particle.data.draw_type) {
                draw_type = i;
                break;
            }
        prev_draw_type = draw_type;

        if (imguiColumnComboBox("Draw Type", glitter_particle_draw_type_name,
            GLITTER_PARTICLE_DRAW_TYPE_EMIT_POSITION,
            (int32_t*)&draw_type, 0, true, &glitter_editor.imgui_focus))
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
    if (imguiColumnDragVec2("Speed [R]",
        &speed, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
        ImGuiSliderFlags_NoRoundToFormat)) {
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

    if (imguiColumnColorEdit4("Color", &particle.data.color,
        ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
        changed = true;

    if (imguiColumnComboBox("UV Index Type", glitter_uv_index_type_name,
        GLITTER_UV_INDEX_USER,
        (int32_t*)&particle.data.uv_index_type, 0, true, &glitter_editor.imgui_focus))
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
        glitter_effect_group* eg = glitter_editor.effect_group;
        size_t rc = eg->resources_count;
        uint64_t* rh = eg->resource_hashes.begin;
        for (size_t i = 0; i < rc && (!idx0 || !idx1); i++) {
            if (idx0 == 0 && rh[i] == particle.data.tex_hash)
                idx0 = (int32_t)(i + 1);

            if (idx1 == 0 && rh[i] == particle.data.mask_tex_hash)
                idx1 = (int32_t)(i + 1);
        }

        char** texture_name_list = force_malloc_s(char*, rc + 1);
        char* texture_names = force_malloc_s(char[0x40], rc + 1);

        texture_name_list[0] = texture_names;
        snprintf(texture_name_list[0], 0x40, "No Texture");
        for (size_t i = 0; i < rc; i++)
            snprintf(texture_name_list[i + 1] = &texture_names[0x40 * (i + 1)],
                0x40, "Texture %lld (%016llX)", i + 1, rh[i]);

        static bool tex0_anim;
        static int32_t tex0_frame;
        static int32_t tex0_index;
        static int32_t tex0_tex;
        if (glitter_editor_property_particle_texture("Texture", texture_name_list,
            &particle, &particle.data.texture, &particle.data.tex_hash,
            idx0, &tex0_anim, &tex0_frame, &tex0_index, &tex0_tex))
            changed = true;

        if (particle.data.flags & GLITTER_PARTICLE_TEXTURE_MASK) {
            static bool tex11_anim;
            static int32_t tex1_frame;
            static int32_t tex1_index;
            static int32_t tex1_tex;
            if (glitter_editor_property_particle_texture("Texture Mask", texture_name_list,
                &particle, &particle.data.mask_texture, &particle.data.mask_tex_hash,
                idx1, &tex11_anim, &tex1_frame, &tex1_index, &tex1_tex))
                changed = true;
        }
        free(texture_names);
        free(texture_name_list);

        glitter_particle_blend_mask blend_draw = glitter_particle_blend_draw_default;
        for (int32_t i = 0; i < glitter_particle_blend_mask_types_count; i++)
            if (glitter_particle_blend_draw_types[i] == particle.data.blend_mode) {
                blend_draw = i;
                break;
            }

        if (imguiColumnComboBox("Blend Mode", glitter_particle_blend_draw_name,
            GLITTER_PARTICLE_BLEND_DRAW_MULTIPLY,
            (int32_t*)&blend_draw, 0, true, &glitter_editor.imgui_focus))
            if (glitter_particle_blend_draw_types[blend_draw] != particle.data.blend_mode) {
                particle.data.blend_mode = glitter_particle_blend_draw_types[blend_draw];
                changed = true;
            }

        if (particle.data.flags & GLITTER_PARTICLE_TEXTURE_MASK) {
            glitter_particle_blend_mask blend_mask = glitter_particle_blend_mask_default;
            for (int32_t i = 0; i < glitter_particle_blend_mask_types_count; i++)
                if (glitter_particle_blend_mask_types[i] == particle.data.mask_blend_mode) {
                    blend_mask = i;
                    break;
                }

            if (imguiColumnComboBox("Mask Blend Mode", glitter_particle_blend_mask_name,
                GLITTER_PARTICLE_BLEND_MASK_MULTIPLY,
                (int32_t*)&blend_mask, 0, true, &glitter_editor.imgui_focus))
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
        if (selected_effect->data.flags & GLITTER_EFFECT_EMISSION
            || particle.data.emission >= glitter_min_emission)
            particle.data.flags |= GLITTER_PARTICLE_EMISSION;
        else
            particle.data.flags &= ~GLITTER_PARTICLE_EMISSION;
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
        if (glitter_editor_hash_input("Object Name Hash", &mesh->object_name_hash))
            changed = true;

        if (glitter_editor_hash_input("Object File Hash", &mesh->object_file_hash))
            changed = true;

        const size_t object_mesh_name_size = sizeof(mesh->object_mesh_name);
        char object_mesh_name[sizeof(mesh->object_mesh_name)];
        memcpy(object_mesh_name, mesh->object_mesh_name, object_mesh_name_size);
        if (imguiColumnInputText("Object Mesh Name", object_mesh_name, object_mesh_name_size, 0, 0, 0)) {
            memcpy(mesh->object_mesh_name, object_mesh_name, object_mesh_name_size);
            changed = true;
        }

        if (glitter_editor_hash_input("Some Hash", &mesh->some_hash))
            changed = true;
    }

    uint64_t hash1 = hash_fnv1a64((void*)selected_particle, sizeof(glitter_particle));
    uint64_t hash2 = hash_fnv1a64((void*)&particle, sizeof(glitter_particle));
    if (hash1 != hash2 && changed) {
        glitter_editor.input_reload = true;
        *selected_particle = particle;
    }

    imguiSetDefaultColumnSpace();
}

static bool glitter_editor_property_particle_texture(char* label, char** items,
    glitter_particle* particle, int32_t* tex, uint64_t* tex_hash, int32_t texture,
    bool* tex_anim, int32_t* tex_frame, int32_t* tex_index, int32_t* tex_tex) {
    glitter_effect_group* eg = glitter_editor.effect_group;
    size_t rc = eg->resources_count;
    int32_t* r = eg->resources.begin;
    uint64_t* rh = eg->resource_hashes.begin;
    txp* rt = eg->resources_tex.begin;

    const uint64_t empty_hash = eg->type != GLITTER_AFT
        ? hash_murmurhash_empty : hash_fnv1a64_empty;

    ImVec2 uv_min;
    ImVec2 uv_max;

    const char* temp_label = imguiStartPropertyColumn(label);
    int32_t prev_texture = texture;
    if (igBeginCombo(temp_label, items[texture], 0)) {
        int32_t uv_index = *tex_index;
        if (!*tex_anim) {
            *tex_anim = true;
            *tex_frame = 0;
            *tex_tex = texture;

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
            if (igSelectable_Bool(items[n], texture == n, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true))
                texture = n;

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

                ImVec2 size1 = (ImVec2){ 192.0f, 192.0f };
                ImVec2 size2 = (ImVec2){ 192.0f, 192.0f };

                txp_mipmap* rtm = rt[n - 1].data.begin;
                float_t aspect1 = (float_t)rtm->width / (float_t)rtm->height;
                float_t aspect2 = aspect1;
                if (particle->data.split_u * particle->data.split_v > 1)
                    aspect2 /= (float_t)particle->data.split_u
                        / (float_t)particle->data.split_v;

                if (aspect1 > 1.0f)
                    size1.y /= aspect1;
                else if (aspect1 < 1.0f)
                    size1.x *= aspect1;

                if (aspect2 > 1.0f)
                    size2.y /= aspect2;
                else if (aspect2 < 1.0f)
                    size2.x *= aspect2;

                igBeginTooltip();
                igText("Frame: %d\nUV Index %d", *tex_frame, *tex_index);
                igText("Tex Size: %dx%d", rtm->width, rtm->height);
                igImage((void*)(size_t)r[n - 1], size1, ImVec2_Empty, ImVec2_Identity, tint_col, border_col);
                igText("Preview Tex Size: %gx%g",
                    particle->data.split_u > 1
                    ? (float_t)rtm->width / (float_t)particle->data.split_u
                    : (float_t)rtm->width,
                    particle->data.split_u > 1
                    ? (float_t)rtm->height / (float_t)particle->data.split_v
                    : (float_t)rtm->height);
                igImage((void*)(size_t)r[n - 1], size2, uv_min, uv_max, tint_col, border_col);
                igEndTooltip();
            }

            if (texture == n)
                igSetItemDefaultFocus();
            igPopID();
        }

        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndCombo();
        (*tex_frame)++;
    }
    else {
        *tex_anim = false;
        *tex_frame = 0;
        *tex_index = 0;
        *tex_tex = 0;
    }
    bool res = prev_texture != texture;
    imguiEndPropertyColumn(temp_label);

    if (res)
        if (texture) {
            *tex = r[texture - 1];
            *tex_hash = rh[texture - 1];
        }
        else {
            *tex = 0;
            *tex_hash = empty_hash;
        }
    return res;
}

static void glitter_editor_popups() {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    const float_t title_bar_size = font->FontSize + style->ItemSpacing.y + style->FramePadding.y * 2.0f;

    igPushID_Str("Glitter Editor Glitter File Create Pop-up");
    glitter_editor_file_create_popup(io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Load Pop-up");
    glitter_editor_file_load_popup(io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Load Error List Pop-up");
    glitter_editor_file_load_error_list_popup(io, style, font, title_bar_size);
    igPopID();

    igPushID_Str("Glitter Editor Glitter File Save");
    glitter_editor_file_save_popup(io, style, font, title_bar_size);
    igPopID();
}

static void glitter_editor_file_create_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    glitter_effect_group* eg = glitter_editor.effect_group;

    ImGuiWindowFlags window_flags;

    if (glitter_editor.create_popup) {
        igOpenPopup("Create Glitter File as...", 0);
        glitter_editor.create_popup = false;
    }

    const float_t button_width = 60.0f;

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

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, (ImVec2) { 0.5f, 0.5f });
    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Create Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos((ImVec2) { x, y });
        igGetContentRegionAvail(&t);
        if (igBeginTable("buttons", 3, 0, ImVec2_Empty, 0.0f)) {
            bool close = false;

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("F2nd", t)) {
                glitter_editor.load_glt_type = GLITTER_F2;
                close = true;
            }

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("AFT", t)) {
                glitter_editor.load_glt_type = GLITTER_AFT;
                close = true;
            }

            igTableNextColumn();
            igGetContentRegionAvail(&t);
            if (imguiButton("X", t)) {
                glitter_editor.load_glt_type = GLITTER_X;
                close = true;
            }

            if (close) {
                glitter_editor.effect_group_add = true;
                glitter_editor.effect_flags |= GLITTER_EDITOR_ADD;
                igCloseCurrentPopup();
            }
            igEndTable();
        }
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_load_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;

    if (glitter_editor.load_popup) {
        igOpenPopup("Load Glitter File as...", 0);
        glitter_editor.load_popup = false;
    }

    const float_t button_width = 60.0f;

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

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, (ImVec2) { 0.5f, 0.5f });
    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Load Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos((ImVec2) { x, y });
        igGetContentRegionAvail(&t);
        if (igBeginTable("buttons", 3, 0, ImVec2_Empty, 0.0f)) {
            bool close = false;

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("F2nd", t)) {
                glitter_editor.load_glt_type = GLITTER_F2;
                close = true;
            }

            igTableNextColumn();;
            igGetContentRegionAvail(&t);
            if (imguiButton("AFT", t)) {
                glitter_editor.load_glt_type = GLITTER_AFT;
                close = true;
            }

            igTableNextColumn();
            igGetContentRegionAvail(&t);
            if (imguiButton("X", t)) {
                glitter_editor.load_glt_type = GLITTER_X;
                close = true;
            }

            if (close) {
                glitter_editor.load = true;
                igCloseCurrentPopup();
            }
            igEndTable();
        }
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_load_error_list_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    ImGuiWindowFlags window_flags;

    if (glitter_editor.load_error_list_popup) {
        igOpenPopup("Glitter File Load Error", 0);
        glitter_editor.load_error_list_popup = false;
    }

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

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, (ImVec2) { 0.5f, 0.5f });
    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Glitter File Load Error", 0, window_flags)) {
        igSetCursorPosY(title_bar_size);
        igText("Can't find name in Glitter List File\n"
            "for hash in Glitter Effect File");

        ImVec2 t;
        igGetContentRegionAvail(&t);
        igSetCursorPosX(t.x * 0.5f - 20.0f);

        x = 40.0f;
        y = font->FontSize + style->FramePadding.y * 2.0f;
        if (imguiButton("OK", (ImVec2) { x, y }))
            igCloseCurrentPopup();
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static void glitter_editor_file_save_popup(ImGuiIO* io,
    ImGuiStyle* style, ImFont* font, const float_t title_bar_size) {
    float_t x;
    float_t y;
    float_t w;
    float_t h;
    float_t win_x;
    float_t win_y;

    glitter_effect_group* eg = glitter_editor.effect_group;
    ImGuiWindowFlags window_flags;

    if (glitter_editor.save_popup) {
        igOpenPopup("Save Glitter File as...", 0);
        glitter_editor.save_popup = false;
    }

    const float_t button_width = 60.0f;

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

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowTitleAlign, (ImVec2) { 0.5f, 0.5f });
    igSetNextWindowPos((ImVec2) { x, y }, ImGuiCond_Always, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Always);
    if (igBeginPopupModal("Save Glitter File as...", 0, window_flags)) {
        ImVec2 t;
        igGetContentRegionAvail(&t);
        x = win_x * 0.5f - t.x * 0.5f;
        y = title_bar_size;

        igSetCursorPos((ImVec2) { x, y });
        igGetContentRegionAvail(&t);
        /*if (eg && eg->type == GLITTER_X) {
            if (igBeginTable("buttons", 3, 0, ImVec2_Empty, 0.0f)) {
                bool close = false;

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("X", t)) {
                    glitter_editor.save = true;
                    glitter_editor.save_compress = false;
                    glitter_editor.save_glt_type = GLITTER_X;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("X HD", t)) {
                    glitter_editor.save = true;
                    glitter_editor.save_compress = true;
                    glitter_editor.save_glt_type = GLITTER_X;
                    close = true;
                }

                igTableNextColumn();
                igGetContentRegionAvail(&t);
                if (imguiButton("None", t)) {
                    glitter_editor.save = false;
                    glitter_editor.save_compress = false;
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
                    glitter_editor.save = true;
                    glitter_editor.save_compress = false;
                    glitter_editor.save_glt_type = GLITTER_F2;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("F2 PS3", t)) {
                    glitter_editor.save = true;
                    glitter_editor.save_compress = true;
                    glitter_editor.save_glt_type = GLITTER_F2;
                    close = true;
                }

                igTableNextColumn();;
                igGetContentRegionAvail(&t);
                if (imguiButton("AFT", t)) {
                    glitter_editor.save = true;
                    glitter_editor.save_compress = false;
                    glitter_editor.save_glt_type = GLITTER_AFT;
                    close = true;
                }

                igTableNextColumn();
                igGetContentRegionAvail(&t);
                if (imguiButton("None", t)) {
                    glitter_editor.save = false;
                    glitter_editor.save_compress = false;
                    close = true;
                }

                if (close)
                    igCloseCurrentPopup();
                igEndTable();
            }
        }
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndPopup();
    }
    igEnd();
    igPopStyleVar(1);
}

static float_t convert_height_to_value(float_t val, float_t pos, float_t size, float_t min, float_t max) {
    float_t t = (val - pos - curve_editor.height_offset) / (size - curve_editor.height_offset);
    return (1.0f - t) * (max - min) + min;
}
static float_t convert_value_to_height(float_t val, float_t pos, float_t size, float_t min, float_t max) {
    float_t t = 1.0f - (val - min) / (max - min);
    return pos + t * (size - curve_editor.height_offset) + curve_editor.height_offset;
}

static void glitter_editor_curve_editor() {
    if (curve_editor.type < GLITTER_CURVE_TRANSLATION_X
        || curve_editor.type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    switch (selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
    case GLITTER_EDITOR_SELECTED_EMITTER:
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        break;
    default:
        return;
    }

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_curve* curve = &curve_editor.curve;
    vector_glitter_curve_key* keys = &curve->keys_rev;
    bool changed = false;

    bool fix_rot_z = eg->type != GLITTER_X && curve->type == GLITTER_CURVE_ROTATION_Z
        && ((selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && selected_emitter->data.direction == GLITTER_DIRECTION_Z_AXIS)
            || (selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && selected_particle->data.draw_type == GLITTER_DIRECTION_Z_AXIS));

    bool add_key = false;
    bool del_key = false;
    glitter_editor_curve_editor_key_manager(keys, &add_key, &del_key);

    if (igIsWindowFocused(0)) {
        if (add_key && igIsKeyPressed(GLFW_KEY_INSERT, true))
            curve_editor.add_key = true;
        else if (del_key && igIsKeyPressed(GLFW_KEY_DELETE, true))
            curve_editor.del_key = true;
        else if (igIsKeyPressed(GLFW_KEY_RIGHT, true))
            curve_editor.frame++;
        else if (igIsKeyPressed(GLFW_KEY_LEFT, true))
            curve_editor.frame--;
    }

    bool exist = true;
    if (curve_editor.add_key && add_key) {
        if (!curve_editor.list[curve_editor.type] || keys->end - keys->begin == 0) {
            glitter_curve_key key;
            memset(&key, 0, sizeof(glitter_curve_key));
            key.frame = curve_editor.curve.start_time;

            if (!curve_editor.list[curve_editor.type]) {
                glitter_curve* curve = glitter_curve_init(eg->type);
                vector_ptr_glitter_curve_push_back(curve_editor.animation, &curve);
                curve_editor.list[curve_editor.type] = curve;
            }

            glitter_editor_curve_editor_curve_set(curve, curve_editor.type);
            vector_glitter_curve_key_push_back(keys, &key);
            *curve_editor.list[curve_editor.type] = curve_editor.curve;
            curve_editor.frame = curve_editor.curve.start_time;
            changed = true;
        }
        else {
            glitter_curve_key* i = keys->begin;
            bool is_before_start = keys->begin->frame > curve_editor.frame;
            bool has_key_after = false;
            if (!is_before_start)
                for (i++; i != keys->end; i++)
                    if (curve_editor.frame <= i->frame) {
                        has_key_after = keys->end - i > 0;
                        break;
                    }

            ssize_t pos = i - keys->begin;
            if (!is_before_start)
                pos--;

            glitter_curve_key key;
            memset(&key, 0, sizeof(glitter_curve_key));
            key.frame = curve_editor.frame;
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
                vector_glitter_curve_key_insert(keys, ++pos, &key);
            }
            else if (is_before_start) {
                glitter_curve_key* n = i;
                key.type = n->type;
                key.value = n->value;
                key.random_range = n->random_range;
                vector_glitter_curve_key_insert(keys, pos, &key);
            }
            else {
                glitter_curve_key* c = i - 1;
                key.type = c->type;
                key.value = c->value;
                key.random_range = c->random_range;
                vector_glitter_curve_key_insert(keys, ++pos, &key);
            }
            *curve_editor.list[curve_editor.type] = *curve;
            changed = true;
        }
    }
    else if (curve_editor.del_key && del_key) {
        for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
            if (i == curve_editor.key) {
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
                vector_glitter_curve_key_erase(keys, i - keys->begin);
                changed = true;
                break;
            }

        glitter_curve* c = curve_editor.list[curve_editor.type];
        if (c && keys->end - keys->begin == 0) {
            glitter_animation* anim = curve_editor.animation;
            for (glitter_curve** i = anim->begin; i != anim->end; i++)
                if (*i && *i == c) {
                    vector_ptr_glitter_curve_erase(anim, i - anim->begin, glitter_curve_dispose);
                    exist = false;
                    break;
                }

            glitter_editor_curve_editor_curve_reset(curve);
            curve_editor.list[curve_editor.type] = 0;
            changed = true;
        }
    }
    else if (curve_editor.add_curve) {
        glitter_curve_key key;
        memset(&key, 0, sizeof(glitter_curve_key));
        key.frame = curve->start_time;
        glitter_curve* c = glitter_curve_init(eg->type);
        vector_ptr_glitter_curve_push_back(curve_editor.animation, &c);
        curve_editor.list[curve_editor.type] = c;
        glitter_editor_curve_editor_curve_set(curve, curve_editor.type);
        vector_glitter_curve_key_push_back(keys, &key);
        *curve_editor.list[curve_editor.type] = *curve;
        curve_editor.frame = curve->start_time;
        changed = true;
    }
    else if (curve_editor.del_curve) {
        glitter_curve* c = curve_editor.list[curve_editor.type];
        glitter_animation* anim = curve_editor.animation;
        for (glitter_curve** i = anim->begin; i != anim->end; i++)
            if (*i && *i == c) {
                vector_ptr_glitter_curve_erase(anim, i - anim->begin, glitter_curve_dispose);
                break;
            }

        glitter_editor_curve_editor_curve_reset(curve);
        curve_editor.list[curve_editor.type] = 0;
        curve_editor.frame = 0;
        changed = true;
    }
    curve_editor.add_key = false;
    curve_editor.del_key = false;
    curve_editor.add_curve = false;
    curve_editor.del_curve = false;

    curve_editor.key = glitter_editor_curve_editor_get_selected_key(curve);

    int32_t start_time = curve->start_time;
    int32_t end_time = curve->end_time;

    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    ImVec2 cont_reg_avail;
    igGetContentRegionAvail(&cont_reg_avail);

    ImVec2 canvas_size, canvas_pos, canvas_pos_min, canvas_pos_max;
    curve_editor.draw_list = igGetWindowDrawList();
    canvas_size = cont_reg_avail;
    igGetCursorScreenPos(&canvas_pos);
    canvas_pos_min.x = canvas_pos.x;
    canvas_pos_min.y = canvas_pos.y;
    canvas_pos_max.x = canvas_pos.x + canvas_size.x;
    canvas_pos_max.y = canvas_pos.y + canvas_size.y;
    canvas_size.y -= style->ScrollbarSize;

    ImRect boundaries;
    boundaries.Min = canvas_pos;
    boundaries.Max.x = canvas_pos.x + canvas_size.x;
    boundaries.Max.y = canvas_pos.y + canvas_size.y;

    if (curve_editor.zoom_time != curve_editor.prev_zoom_time) {
        float_t timeline_pos = curve_editor.timeline_pos + curve_editor_timeline_base_pos;
        timeline_pos *= curve_editor.zoom_time / curve_editor.prev_zoom_time;
        curve_editor.timeline_pos = timeline_pos - curve_editor_timeline_base_pos;
    }
    curve_editor.prev_zoom_time = curve_editor.zoom_time;

    float_t frame_width = curve_editor.frame_width * curve_editor.zoom_time;
    canvas_pos.x -= curve_editor.timeline_pos;

    igItemSize_Rect(boundaries, -1.0f);
    bool hovered = igIsMouseHoveringRect(boundaries.Min, boundaries.Max, true);
    igRenderFrame(boundaries.Min, boundaries.Max, igGetColorU32_Col(ImGuiCol_FrameBg, 1.0f), true, 1.0f);

    static bool can_drag;
    if (hovered && igIsMouseClicked(ImGuiMouseButton_Left, false))
        can_drag = true;

    if (can_drag && igIsMouseReleased(ImGuiMouseButton_Left))
        can_drag = false;

    float_t timeline_max_offset = (end_time - start_time) * frame_width;
    timeline_max_offset = max(timeline_max_offset, 0.0f) - curve_editor_timeline_base_pos;

    ImVec2 scrollbar_size, scrollbar_pos;
    scrollbar_size.x = canvas_size.x;
    scrollbar_size.y = style->ScrollbarSize;
    scrollbar_pos.x = boundaries.Min.x;
    scrollbar_pos.y = boundaries.Max.y;
    ImRect scroll_boundaries;
    scroll_boundaries.Min = scrollbar_pos;
    scroll_boundaries.Max.x = scrollbar_pos.x + scrollbar_size.x;
    scroll_boundaries.Max.y = scrollbar_pos.y + scrollbar_size.y;

    igRenderFrame(scroll_boundaries.Min, scroll_boundaries.Max,
        igGetColorU32_Col(ImGuiCol_ScrollbarBg, 1.0f), false, 0.0f);

    static const ImVec2 bar_offset = { 5.0f, 2.5f };
    ImVec2 bar_size, bar_pos, bar_max;
    glitter_editor_curve_editor_get_scroll_bar(timeline_max_offset,
        canvas_size.x, frame_width, scrollbar_size, scrollbar_pos,
        bar_offset, &bar_size, &bar_pos, &bar_max);

    igSetCursorScreenPos(bar_pos);
    igInvisibleButton("scroll_handle", bar_size, 0);
    bool scroolbar_move = igIsItemActive();

    ImDrawList_AddRectFilled(curve_editor.draw_list, bar_pos, bar_max,
        igGetColorU32_Col(ImGuiCol_ScrollbarGrab, 1.0f), style->ScrollbarRounding, 0);

    ImDrawList_PushClipRect(curve_editor.draw_list, boundaries.Min, boundaries.Max, true);

    int32_t start = (int32_t)(curve_editor.timeline_pos / frame_width) + start_time;
    int32_t end = (int32_t)((curve_editor.timeline_pos + canvas_size.x) / frame_width) + start_time;

    ImU32 line_color_valid_bold = igGetColorU32_Vec4((ImVec4) { 0.5f, 0.5f, 0.5f, 0.75f });
    ImU32 line_color_valid = igGetColorU32_Vec4((ImVec4) { 0.4f, 0.4f, 0.4f, 0.45f });
    ImU32 line_color_not_valid = igGetColorU32_Vec4((ImVec4) { 0.2f, 0.2f, 0.2f, 0.75f });
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
        ImDrawList_AddLine(curve_editor.draw_list,
            (ImVec2) { x, y - canvas_size.y }, (ImVec2) { x, y }, line_color, 0.80f);

        if (bold_frame) {
            char buf[0x20];
            snprintf(buf, sizeof(buf), "%d", frame);
            ImDrawList_AddText_Vec2(curve_editor.draw_list,
                (ImVec2) { x + 3, canvas_pos.y }, line_color, buf, 0);
        }
    }

    float_t max = curve_editor.range * (1.0f / curve_editor.zoom_value);
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
            - curve_editor.height_offset) + curve_editor.height_offset;

        ImU32 line_color = j % 2 == 1
            ? igGetColorU32_Vec4((ImVec4) { 0.35f, 0.35f, 0.35f, 0.45f })
            : igGetColorU32_Vec4((ImVec4) { 0.45f, 0.45f, 0.45f, 0.75f });
        bool last = j == 4;
        if (j && !last)
            ImDrawList_AddLine(curve_editor.draw_list, (ImVec2) { x_pos, y_pos },
                (ImVec2) { x_pos + canvas_size.x, y_pos }, line_color, 0.80f);

        float_t val = max - j * 0.25f * (max - min);
        char buf[0x20];
        snprintf(buf, sizeof(buf), "%.2f", val);
        ImDrawList_AddText_Vec2(curve_editor.draw_list, (ImVec2) { x_pos + 2, y_pos + (last ? -16 : -2) },
            igGetColorU32_Vec4((ImVec4) { 0.65f, 0.65f, 0.65f, 0.85f }), buf, 0);
    }

    if (curve->type >= GLITTER_CURVE_ROTATION_X && curve->type <= GLITTER_CURVE_ROTATION_Z) {
        max *= DEG_TO_RAD_FLOAT;
        min *= DEG_TO_RAD_FLOAT;
    }
    else if (curve->type >= GLITTER_CURVE_COLOR_R && curve->type <= GLITTER_CURVE_COLOR_RGB_SCALE_2ND) {
        max *= (float_t)(1.0 / 255.0);
        min *= (float_t)(1.0 / 255.0);
    }

    float_t base_line = convert_value_to_height(0.0f, canvas_pos.y, canvas_size.y, min, max);

    ImVec2 p1, p2, p3;
    p1.x = canvas_pos_min.x;
    p1.y = canvas_pos.y + curve_editor.height_offset;
    p2.x = p1.x + canvas_size.x;
    p2.y = canvas_pos.y + curve_editor.height_offset;
    p3.x = canvas_pos_min.x;
    p3.y = canvas_pos.y;
    ImDrawList_AddLine(curve_editor.draw_list, p1, p2,
        igGetColorU32_Vec4((ImVec4) { 0.5f, 0.5f, 0.5f, 0.85f }), 2.0f);

    ImU32 random_range_color = igGetColorU32_Vec4((ImVec4) { 0.5f, 0.5f, 0.0f, 0.25f });
    ImU32 key_random_range_color = igGetColorU32_Vec4((ImVec4) { 0.5f, 0.0f, 0.5f, 0.25f });
    ImU32 default_color = igGetColorU32_Vec4((ImVec4) { 1.0f, 1.0f, 1.0f, 0.75f });
    ImU32 selected_color = igGetColorU32_Vec4((ImVec4) { 0.0f, 1.0f, 1.0f, 0.75f });

    if (curve->flags & GLITTER_CURVE_RANDOM_RANGE
        || curve->flags & GLITTER_CURVE_KEY_RANDOM_RANGE) {
        float_t random_range = curve->random_range;
        bool random_range_mult = curve->flags & GLITTER_CURVE_RANDOM_RANGE_MULT;
        bool random_range_negate = curve->flags & GLITTER_CURVE_RANDOM_RANGE_NEGATE;
        bool glt_type_aft = eg->type == GLITTER_AFT;
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
            if (random_range_mult && glt_type_aft) {
                c_random_range *= c_value;
                n_random_range *= n_value;
            }

            size_t frame_width_int = (size_t)roundf(frame_width);
            if (k_type == GLITTER_KEY_CONSTANT) {
                vec3 value = { c_value + c_random_range, c_value, c_value - c_random_range };
                vec3 random = { random_range, random_range, random_range };
                if (random_range_mult) {
                    vec3_mult(random, value, random);
                    if (!glt_type_aft)
                        vec3_mult_scalar(random, 0.01f, random);
                }

                if (value.x != value.y) {
                    float_t y1 = convert_value_to_height(value.x + random.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y2 = convert_value_to_height(value.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y3 = convert_value_to_height(value.x,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y4 = convert_value_to_height(value.y,
                        canvas_pos.y, canvas_size.y, min, max);

                    y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                    y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                    if (y1 != y2)
                        ImDrawList_AddRectFilled(curve_editor.draw_list,
                            (ImVec2) { x1, y1 }, (ImVec2) { x2, y2 },
                            random_range_color, 0.0f, 0);

                    y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                    y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                    if (y3 != y4)
                        ImDrawList_AddRectFilled(curve_editor.draw_list,
                            (ImVec2) { x1, y3 }, (ImVec2) { x2, y4 },
                            key_random_range_color, 0.0f, 0);
                }
                else {
                    float_t y1 = convert_value_to_height(value.y + random.y,
                        canvas_pos.y, canvas_size.y, min, max);
                    float_t y2 = convert_value_to_height(value.y,
                        canvas_pos.y, canvas_size.y, min, max);

                    y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                    y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                    if (y1 != y2)
                        ImDrawList_AddRectFilled(curve_editor.draw_list,
                            (ImVec2) { x1, y1 }, (ImVec2) { x2, y2 },
                            random_range_color, 0.0f, 0);
                }

                if (random_range_negate)
                    if (value.y != value.z) {
                        float_t y1 = convert_value_to_height(value.y,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y2 = convert_value_to_height(value.z,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y3 = convert_value_to_height(value.z,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y4 = convert_value_to_height(value.z - random.z,
                            canvas_pos.y, canvas_size.y, min, max);

                        y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                        y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                        if (y1 != y2)
                            ImDrawList_AddRectFilled(curve_editor.draw_list,
                                (ImVec2) { x1, y1 }, (ImVec2) { x2, y2 },
                                key_random_range_color, 0.0f, 0);

                        y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                        y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                        if (y3 != y4)
                            ImDrawList_AddRectFilled(curve_editor.draw_list,
                                (ImVec2) { x1, y3 }, (ImVec2) { x2, y4 },
                                random_range_color, 0.0f, 0);
                    }
                    else {
                        float_t y1 = convert_value_to_height(value.y,
                            canvas_pos.y, canvas_size.y, min, max);
                        float_t y2 = convert_value_to_height(value.y - random.y,
                            canvas_pos.y, canvas_size.y, min, max);

                        y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                        y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                        if (y1 != y2)
                            ImDrawList_AddRectFilled(curve_editor.draw_list,
                                (ImVec2) { x1, y1 }, (ImVec2) { x2, y2 },
                                random_range_color, 0.0f, 0);
                    }
            }
            else {
                float_t c_tangent2 = c->tangent2;
                float_t n_tangent1 = n->tangent1;

                vec3 c_frame_vec = { c_frame, c_frame, c_frame };
                vec3 n_frame_vec = { n_frame, n_frame, n_frame };
                vec3 c_value_vec = (vec3){ c_value + c_random_range, c_value, c_value - c_random_range };
                vec3 n_value_vec = (vec3){ n_value + n_random_range, n_value, n_value - n_random_range };
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
                            if (!glt_type_aft)
                                vec3_mult_scalar(random, 0.01f, random);
                        }
                        else
                            random = random_vec;

                        if (random_range_mult) {
                            vec3 random = { random_range, random_range, random_range };
                            vec3_mult(random, value, random);
                            if (!glt_type_aft)
                                vec3_mult_scalar(random, 0.01f, random);
                        }

                        float_t x = canvas_pos.x
                            + (float_t)(i - start_time) * frame_width + (float_t)j;
                        if (value.x != value.y) {
                            float_t y1 = convert_value_to_height(value.x + random.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y2 = convert_value_to_height(value.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y3 = convert_value_to_height(value.x,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y4 = convert_value_to_height(value.y,
                                canvas_pos.y, canvas_size.y, min, max);

                            y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                            y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                            if (y1 != y2)
                                ImDrawList_AddLine(curve_editor.draw_list,
                                    (ImVec2) { x, y1 }, (ImVec2) { x, y2 },
                                    random_range_color, 0.0f);

                            y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                            y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                            if (y3 != y4)
                                ImDrawList_AddLine(curve_editor.draw_list,
                                    (ImVec2) { x, y3 }, (ImVec2) { x, y4 },
                                    key_random_range_color, 0.0f);
                        }
                        else {
                            float_t y1 = convert_value_to_height(value.y + random.y,
                                canvas_pos.y, canvas_size.y, min, max);
                            float_t y2 = convert_value_to_height(value.y,
                                canvas_pos.y, canvas_size.y, min, max);

                            y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                            y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                            if (y1 != y2)
                                ImDrawList_AddLine(curve_editor.draw_list,
                                    (ImVec2) { x, y1 }, (ImVec2) { x, y2 },
                                    random_range_color, 0.0f);
                        }

                        if (random_range_negate)
                            if (value.y != value.z) {
                                float_t y1 = convert_value_to_height(value.y,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y2 = convert_value_to_height(value.z,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y3 = convert_value_to_height(value.z,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y4 = convert_value_to_height(value.z - random.z,
                                    canvas_pos.y, canvas_size.y, min, max);

                                y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                                y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                                if (y1 != y2)
                                    ImDrawList_AddLine(curve_editor.draw_list,
                                        (ImVec2) { x, y1 }, (ImVec2) { x, y2 },
                                        key_random_range_color, 0.0f);

                                y3 = clamp(y3, canvas_pos_min.y, canvas_pos_max.y);
                                y4 = clamp(y4, canvas_pos_min.y, canvas_pos_max.y);
                                if (y3 != y4)
                                    ImDrawList_AddLine(curve_editor.draw_list,
                                        (ImVec2) { x, y3 }, (ImVec2) { x, y4 },
                                        random_range_color, 0.0f);
                            }
                            else {
                                float_t y1 = convert_value_to_height(value.y,
                                    canvas_pos.y, canvas_size.y, min, max);
                                float_t y2 = convert_value_to_height(value.y - random.y,
                                    canvas_pos.y, canvas_size.y, min, max);

                                y1 = clamp(y1, canvas_pos_min.y, canvas_pos_max.y);
                                y2 = clamp(y2, canvas_pos_min.y, canvas_pos_max.y);
                                if (y1 != y2)
                                    ImDrawList_AddLine(curve_editor.draw_list,
                                        (ImVec2) { x, y1 }, (ImVec2) { x, y2 },
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
            float_t y1 = convert_value_to_height(c_value, canvas_pos.y, canvas_size.y, min, max);
            float_t y2 = convert_value_to_height(n_value, canvas_pos.y, canvas_size.y, min, max);

            if (i->type == GLITTER_KEY_CONSTANT) {
                ImVec2 points_temp[4];
                points_temp[0] = (ImVec2){ x1, y1 };
                points_temp[1] = (ImVec2){ x2, y1 };
                points_temp[2] = (ImVec2){ x2, y1 };
                points_temp[3] = (ImVec2){ x2, y2 };
                ImDrawList_AddPolyline(curve_editor.draw_list, points_temp, 4, default_color, 0, 3.0f);
            }
            else
                ImDrawList_AddLine(curve_editor.draw_list,
                    (ImVec2) { x1, y1 }, (ImVec2) { x2, y2 }, default_color, 3.0f);
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
                    point.y = convert_value_to_height(value,
                        canvas_pos.y, canvas_size.y, min, max);
                    points[(i - c->frame) * frame_width_int + j] = point;
                }
            ImDrawList_AddPolyline(curve_editor.draw_list, points,
                (int32_t)points_count, default_color, 0, 3.0f);
            free(points);
        }
    }

    const ImU32 key_constant_color = igGetColorU32_Vec4((ImVec4) { 1.0f, 0.0f, 0.0f, 0.75f });
    const ImU32 key_linear_color = igGetColorU32_Vec4((ImVec4) { 0.0f, 1.0f, 0.0f, 0.75f });
    const ImU32 key_hermite_color = igGetColorU32_Vec4((ImVec4) { 0.0f, 0.0f, 1.0f, 0.75f });

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
        float_t y = convert_value_to_height(base_value, canvas_pos.y, canvas_size.y, min, max);

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
        igSetCursorScreenPos((ImVec2) { x - 7.5f, y - 7.5f });
        igInvisibleButton("val", (ImVec2) { 15.0f, 15.0f }, 0);

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
                curve_editor.frame = (int32_t)roundf((io->MousePos.x - canvas_pos.x) / frame_width);
                curve_editor.frame = clamp(curve_editor.frame + start_time, start, end);
                if (i->frame != curve_editor.frame) {
                    i->frame = curve_editor.frame;
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
                float_t value = convert_height_to_value(y, canvas_pos.y, canvas_size.y, min, max);
                dragged = true;

                if (base_value != value) {
                    if (fix_rot_z)
                        value += (float_t)M_PI_2;
                    i->value = value;
                    changed = true;
                }
                curve_editor.key = i;
            }
        }

        if (i - keys->begin > 0 && (i - 1)->type == GLITTER_KEY_HERMITE) {
            ImU32 tangent1_color;
            float_t tangent1;
            ImVec2 circle_pos;
            circle_pos.x = x - 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(tangent1 = i->tangent1,
                canvas_pos.y, canvas_size.y, min, max);
            ImDrawList_AddLine(curve_editor.draw_list, (ImVec2) { x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                igSetCursorScreenPos((ImVec2) { circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                igInvisibleButton("tan1", (ImVec2) { 15.0f, 15.0f }, 0);

                if (can_drag && igIsItemActive()) {
                    if (igIsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
                        i->tangent1 = convert_height_to_value(circle_pos.y + io->MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent1 != tangent1)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent1_color = selected_color;
                }
                else
                    tangent1_color = default_color;

                ImDrawList_AddCircleFilled(curve_editor.draw_list, circle_pos,
                    curve_editor.key_radius_in, tangent1_color, 12);
            }
        }

        if (keys->end - i > 1 && i->type == GLITTER_KEY_HERMITE) {
            ImU32 tangent2_color;
            float_t tangent2;
            ImVec2 circle_pos;
            circle_pos.x = x + 25.0f;
            circle_pos.y = y - base_line + convert_value_to_height(tangent2 = i->tangent2,
                canvas_pos.y, canvas_size.y, min, max);
            ImDrawList_AddLine(curve_editor.draw_list, (ImVec2) { x, y }, circle_pos, default_color, 1.5f);

            if ((circle_pos.x >= p3.x - 10.0f && circle_pos.x <= p3.x + canvas_size.x + 10.0f)
                && (circle_pos.y >= p3.y - 10.0f && circle_pos.y <= p3.y + canvas_size.y + 10.0f)) {
                igSetCursorScreenPos((ImVec2) { circle_pos.x - 7.5f, circle_pos.y - 7.5f });
                igInvisibleButton("tan2", (ImVec2) { 15.0f, 15.0f }, 0);

                if (can_drag && igIsItemActive()) {
                    if (igIsMouseDragging(ImGuiMouseButton_Left, -1.0f)) {
                        i->tangent2 = convert_height_to_value(circle_pos.y + io->MouseDelta.y - y + base_line,
                            canvas_pos.y, canvas_size.y, min, max);

                        if (i->tangent2 != tangent2)
                            changed = true;
                    }
                    holding_tan = true;
                    tangent2_color = selected_color;
                }
                else
                    tangent2_color = default_color;

                ImDrawList_AddCircleFilled(curve_editor.draw_list, circle_pos,
                    curve_editor.key_radius_in, tangent2_color, 12);
            }
        }

        ImDrawList_AddCircleFilled(curve_editor.draw_list, (ImVec2) { x, y },
            curve_editor.key_radius_out, curve_editor.key == i ? selected_color : default_color, 12);
        ImDrawList_AddCircleFilled(curve_editor.draw_list, (ImVec2) { x, y },
            curve_editor.key_radius_in, value_color, 12);
        igPopID();
    }

    if (can_drag && !scroolbar_move && io->MouseDown[0] && (!holding_tan || dragged)) {
        int32_t frame = (int32_t)roundf((io->MousePos.x - canvas_pos.x) / frame_width);
        curve_editor.frame = clamp(frame + start_time, start_time, end_time);
        curve_editor.key = glitter_editor_curve_editor_get_selected_key(curve);
    }
    curve_editor.frame = clamp(curve_editor.frame, start_time, end_time);

    int32_t cursor_frame = curve_editor.frame - start_time;
    ImVec2 cursor_pos = canvas_pos;
    cursor_pos.x += cursor_frame * frame_width;
    if (cursor_pos.x >= p1.x - 10.0f && cursor_pos.x <= p1.x + canvas_size.x + 10.0f) {
        ImU32 cursor_color = igGetColorU32_Vec4((ImVec4) { 0.8f, 0.8f, 0.8f, 0.75f });
        p1.x = cursor_pos.x;
        p1.y = canvas_pos.y + canvas_size.y;
        ImDrawList_AddLine(curve_editor.draw_list, cursor_pos, p1, cursor_color, 2.5f);

        p1 = p2 = p3 = cursor_pos;
        p1.x -= 10.0f;
        p2.y += 10.0f;
        p3.x += 10.0f;
        ImDrawList_AddTriangleFilled(curve_editor.draw_list, p1, p2, p3, cursor_color);
    }
    ImDrawList_PopClipRect(curve_editor.draw_list);

    if (hovered)
        if (igIsKeyDown(GLFW_KEY_LEFT_SHIFT) || igIsKeyDown(GLFW_KEY_RIGHT_SHIFT))
            curve_editor.timeline_pos -= io->MouseWheel * 25.0f * 4.0f;
        else
            curve_editor.timeline_pos -= io->MouseWheel * 25.0f;

    if (scroolbar_move && io->MouseDelta.x != 0.0f)
        curve_editor.timeline_pos += io->MouseDelta.x * curve_editor.zoom_time;

    curve_editor.timeline_pos = clamp(curve_editor.timeline_pos,
        -curve_editor_timeline_base_pos, timeline_max_offset);

    if (curve_editor.list[curve_editor.type] && changed) {
        glitter_curve_recalculate(eg->type, curve);
        *curve_editor.list[curve_editor.type] = *curve;
        glitter_editor.input_reload = true;
    }
}

static void glitter_editor_curve_editor_curve_reset(glitter_curve* curve) {
    memset(curve, 0, sizeof(glitter_curve));
    glitter_editor_curve_editor_curve_set(curve, -1);
}

static void glitter_editor_curve_editor_curve_set(glitter_curve* curve, glitter_curve_type type) {
    glitter_effect_group* eg = glitter_editor.effect_group;
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

static void glitter_editor_curve_editor_curves_reset() {
    glitter_editor_curve_editor_curve_reset(&curve_editor.curve);
    memset(&curve_editor.list, 0, sizeof(glitter_curve*)
        * GLITTER_CURVE_V_SCROLL_ALPHA_2ND - GLITTER_CURVE_TRANSLATION_X + 1);
    curve_editor.animation = 0;
    curve_editor.key = 0;
}

static glitter_curve_key* glitter_editor_curve_editor_get_selected_key(glitter_curve* curve) {
    vector_glitter_curve_key* keys = &curve->keys_rev;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
        if (i->frame == curve_editor.frame)
            return i;
    }
    return 0;
}

static void glitter_editor_curve_editor_get_scroll_bar(float_t max_pos, float_t canvas_size,
    float_t frame_width, ImVec2 scrollbar_size, ImVec2 scrollbar_pos,
    ImVec2 bar_offset, ImVec2* bar_size, ImVec2* bar_pos, ImVec2* bar_max) {
    scrollbar_size.x -= bar_offset.x * 2.0f;
    scrollbar_size.y -= bar_offset.y * 2.0f;

    float_t cur_pos = curve_editor.timeline_pos + curve_editor_timeline_base_pos;
    float_t visible_frames = canvas_size / frame_width;
    float_t max_frames = ((max_pos <= 0.0f ? 0.0f : max_pos) + canvas_size) / frame_width;
    float_t size_x = scrollbar_size.x * (floorf(visible_frames) / floorf(max_frames));
    float_t size_y = scrollbar_size.y;

    bar_size->x = size_x;
    bar_size->y = size_y;
    bar_pos->x = scrollbar_pos.x + bar_offset.x + (max_pos <= 0.0f ? 1.0f
        : min(cur_pos / max_pos, 1.0f)) * (scrollbar_size.x - size_x);
    bar_pos->y = scrollbar_pos.y + bar_offset.y;
    bar_max->x = bar_pos->x + bar_size->x;
    bar_max->y = bar_pos->y + bar_size->y;
}

static void glitter_editor_curve_editor_key_manager(vector_glitter_curve_key* keys, bool* add_key, bool* del_key) {
    glitter_curve* curve = &curve_editor.curve;
    *add_key = true;
    *del_key = false;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
        if (i->frame > curve->end_time)
            break;
        else if (i->frame == curve_editor.frame) {
            *add_key = false;
            *del_key = true;
            break;
        }
}

static void glitter_editor_curve_editor_property_window() {
    if (curve_editor.type < GLITTER_CURVE_TRANSLATION_X
        || curve_editor.type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    float_t scale;
    float_t inv_scale;
    float_t min;

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_curve_type_flags flags = 0;
    switch (selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        flags = glitter_effect_curve_flags;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        flags = glitter_emitter_curve_flags;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (eg->type == GLITTER_X)
            flags = glitter_particle_x_curve_flags;
        else
            flags = glitter_particle_curve_flags;

        if (selected_particle->data.type != GLITTER_PARTICLE_MESH) {
            flags &= ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND;
            if (selected_particle->data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
                flags &= ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y);
        }
        break;
    default:
        return;
    }

    glitter_curve* curve = &curve_editor.curve;

    bool fix_rot_z = eg->type != GLITTER_X && curve->type == GLITTER_CURVE_ROTATION_Z
        && ((selected_type == GLITTER_EDITOR_SELECTED_EMITTER
            && selected_emitter->data.direction == GLITTER_DIRECTION_Z_AXIS)
            || (selected_type == GLITTER_EDITOR_SELECTED_PARTICLE
                && selected_particle->data.draw_type == GLITTER_DIRECTION_Z_AXIS));

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

    igText("Frame: %d", curve_editor.frame);

    if (curve_editor.key && igTreeNodeEx_Str("Key", ImGuiTreeNodeFlags_DefaultOpen)) {
        glitter_curve_key* c = curve_editor.key;
        glitter_curve_key* n = curve->keys_rev.end - c > 1 ? curve_editor.key + 1 : 0;
        if (imguiColumnComboBox("Type", glitter_key_name,
            GLITTER_KEY_HERMITE,
            (int32_t*)&c->type, 0, true, &glitter_editor.imgui_focus))
            changed = true;

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
        imguiDisableElementPop(key_random_range);

        if (n && c->type == GLITTER_KEY_HERMITE) {
            float_t tangent2 = c->tangent2 * scale;
            if (imguiColumnDragFloat("Tangent 1",
                &tangent2, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                c->tangent2 = tangent2 * inv_scale;
                changed = true;
            }

            float_t tangent1 = n->tangent1 * scale;
            if (imguiColumnDragFloat("Tangent 2",
                &tangent1, 0.0001f, -FLT_MAX, FLT_MAX, "%g",
                ImGuiSliderFlags_NoRoundToFormat)) {
                n->tangent1 = tangent1 * inv_scale;
                changed = true;
            }
        }
        igTreePop();
    }

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

        if (imguiCheckboxFlags_UintPtr(eg->type == GLITTER_AFT
            ? "Scale Random by Value" : "Scale Random by Value * 0.01",
            (uint32_t*)&curve->flags,
            GLITTER_CURVE_RANDOM_RANGE_MULT))
            changed = true;
        igPopID();
        igTreePop();
    }

    if (curve_editor.list[curve_editor.type]) {
        if (changed) {
            glitter_curve_recalculate(eg->type, curve);
            glitter_editor.input_reload = true;
        }

        uint64_t hash1 = hash_fnv1a64((void*)curve_editor.list[curve_editor.type], sizeof(glitter_curve));
        uint64_t hash2 = hash_fnv1a64((void*)curve, sizeof(glitter_effect));
        if (hash1 != hash2)
            *curve_editor.list[curve_editor.type] = *curve;
    }
}

static void glitter_editor_curve_editor_reset_state(glitter_curve_type type) {
    if (curve_editor.type != type)
        curve_editor.type = type;

    curve_editor.frame_width = 16;
    curve_editor.zoom_time = 1.0f;
    curve_editor.prev_zoom_time = 1.0f;
    curve_editor.zoom_value = 1.0f;
    curve_editor.key_radius_in = 6.0f;
    curve_editor.key_radius_out = 8.0f;
    curve_editor.height_offset = 20.0f;
    curve_editor.frame = 0;
    curve_editor.key = 0;

    curve_editor.timeline_pos = -curve_editor_timeline_base_pos;

    switch (type) {
    case GLITTER_CURVE_TRANSLATION_X:
    case GLITTER_CURVE_TRANSLATION_Y:
    case GLITTER_CURVE_TRANSLATION_Z:
        curve_editor.range = 25.0f;
        break;
    case GLITTER_CURVE_ROTATION_X:
    case GLITTER_CURVE_ROTATION_Y:
    case GLITTER_CURVE_ROTATION_Z:
        curve_editor.range = 360.0f;
        break;
    case GLITTER_CURVE_SCALE_X:
    case GLITTER_CURVE_SCALE_Y:
    case GLITTER_CURVE_SCALE_Z:
    case GLITTER_CURVE_SCALE_ALL:
        curve_editor.range = 5.0f;
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
        curve_editor.range = 255.0f;
        break;
    case GLITTER_CURVE_EMISSION_INTERVAL:
        curve_editor.range = 20.0f;
        break;
    case GLITTER_CURVE_PARTICLES_PER_EMISSION:
        curve_editor.range = 50.0f;
        break;
    case GLITTER_CURVE_U_SCROLL:
    case GLITTER_CURVE_V_SCROLL:
    case GLITTER_CURVE_U_SCROLL_ALPHA:
    case GLITTER_CURVE_V_SCROLL_ALPHA:
    case GLITTER_CURVE_U_SCROLL_2ND:
    case GLITTER_CURVE_V_SCROLL_2ND:
    case GLITTER_CURVE_U_SCROLL_ALPHA_2ND:
    case GLITTER_CURVE_V_SCROLL_ALPHA_2ND:
        curve_editor.range = 1.0f;
        break;
    default:
        curve_editor.range = 1.0f;
        break;
    }

    curve_editor.add_key = false;
    curve_editor.del_key = false;
}

static void glitter_editor_curve_editor_selector() {
    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_curve_type_flags flags = 0;
    glitter_animation* anim = 0;
    switch (selected_type) {
    case GLITTER_EDITOR_SELECTED_EFFECT:
        flags = glitter_effect_curve_flags;
        anim = &selected_effect->animation;
        break;
    case GLITTER_EDITOR_SELECTED_EMITTER:
        flags = glitter_emitter_curve_flags;
        anim = &selected_emitter->animation;
        break;
    case GLITTER_EDITOR_SELECTED_PARTICLE:
        if (eg->type == GLITTER_X)
            flags = glitter_particle_x_curve_flags;
        else
            flags = glitter_particle_curve_flags;
        anim = &selected_particle->animation;

        if (selected_particle->data.type != GLITTER_PARTICLE_MESH) {
            flags &= ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND;
            if (selected_particle->data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
                flags &= ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y);
        }
        break;
    default:
        return;
    }
    curve_editor.animation = anim;

    for (glitter_curve_type i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++) {
        bool found = false;
        glitter_curve** j;
        for (j = anim->begin; j != anim->end; j++)
            if (*j && (*j)->type == i) {
                found = true;
                break;
            }
        curve_editor.list[i] = found ? *j : 0;
    }

    ImVec2 cont_reg_avail;
    igGetContentRegionAvail(&cont_reg_avail);
    if (igBeginListBox("##Curves", (ImVec2) { cont_reg_avail.x * 0.5f, cont_reg_avail.y })) {
        ImGuiTreeNodeFlags tree_node_flags = 0;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
        tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

#define LIST_BOX(label, curve_type) \
if (flags & GLITTER_CURVE_TYPE_##curve_type){ \
    char* _l =  curve_editor.list[GLITTER_CURVE_##curve_type] ? label" [*]" : label; \
    if (igSelectable_Bool(_l, curve_editor.type == GLITTER_CURVE_##curve_type, 0, ImVec2_Empty)) \
        glitter_editor_curve_editor_reset_state(GLITTER_CURVE_##curve_type); \
\
    if (igIsItemFocused() && curve_editor.type != GLITTER_CURVE_##curve_type) \
        glitter_editor_curve_editor_reset_state(GLITTER_CURVE_##curve_type); \
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
        glitter_editor.imgui_focus |= igIsWindowFocused(0);
        igEndListBox();
    }

    if (curve_editor.type < GLITTER_CURVE_TRANSLATION_X || curve_editor.type > GLITTER_CURVE_V_SCROLL_ALPHA_2ND)
        return;

    if (curve_editor.list[curve_editor.type])
        curve_editor.curve = *curve_editor.list[curve_editor.type];
    else if (curve_editor.curve.type != curve_editor.type) {
        memset(&curve_editor.curve, 0, sizeof(glitter_curve));
        glitter_editor_curve_editor_curve_set(&curve_editor.curve, curve_editor.type);
    }
    glitter_curve* curve = &curve_editor.curve;

    igSameLine(0.0f, -1.0f);
    igBeginGroup();
    imguiSetColumnSpace(3.0f / 7.0f);
    float_t zoom_time = curve_editor.zoom_time * 100.0f;
    imguiColumnSliderFloat("Zoom Time", &zoom_time, 5.0f, 500.0f, "%.1f%%", 0);
    curve_editor.zoom_time = zoom_time * 0.01f;

    int32_t zoom_value = (int32_t)roundf(curve_editor.zoom_value * 100.0f);
    imguiColumnSliderInt("Zoom Value", &zoom_value, 10, 1000, "%d%%", 0);
    curve_editor.zoom_value = (float_t)zoom_value * 0.01f;

    imguiColumnDragFloat("Range",
        &curve_editor.range, 0.0001f, 0.0f, FLT_MAX, "%g", 0);
    imguiSetDefaultColumnSpace();

    bool add_key = false;
    bool del_key = false;
    glitter_editor_curve_editor_key_manager(&curve->keys_rev, &add_key, &del_key);

    igSeparator();
    ImVec2 t;
    igGetContentRegionAvail(&t);
    imguiDisableElementPush(add_key);
    if (imguiButton("Add Key", (ImVec2) { t.x, 0.0f }))
        curve_editor.add_key = true;
    imguiDisableElementPop(add_key);

    imguiDisableElementPush(del_key);
    if (imguiButton("Delete Key", (ImVec2) { t.x, 0.0f }))
        curve_editor.del_key = true;
    imguiDisableElementPop(del_key);

    bool has_curve_in_list = curve_editor.list[curve_editor.type] ? true : false;
    imguiDisableElementPush(!has_curve_in_list);
    if (imguiButton("Add Curve", (ImVec2) { t.x, 0.0f }))
        curve_editor.add_curve = true;
    imguiDisableElementPop(!has_curve_in_list);

    imguiDisableElementPush(has_curve_in_list);
    if (imguiButton("Delete Curve", (ImVec2) { t.x, 0.0f }))
        curve_editor.del_curve = true;
    imguiDisableElementPop(has_curve_in_list);
}

static bool glitter_editor_hash_input(const char* label, uint64_t* hash) {
    uint64_t h = *hash;
    if (imguiColumnInputScalar(label, ImGuiDataType_U64, &h,
        0, 0, "%016llX", ImGuiInputTextFlags_CharsHexadecimal)) {
        if (h == 0)
            h = hash_fnv1a64_empty;

        if (h != *hash) {
            *hash = h;
            return true;
        }
    }
    return false;
}

static void glitter_editor_gl_load() {
    shader_param param;

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Glitter Editor Wireframe";
    shader_fbo_wload_file(&gl_data.wireframe.shader,
        L"rom\\shaders\\glt_edt_wrfrm.vert",
        L"rom\\shaders\\glt_edt_wrfrm.frag",
        0, &param);
}

static void glitter_editor_gl_draw() {
    if (!glitter_editor.effect_group)
        return;

    if (glitter_editor.draw_flags & GLITTER_EDITOR_DRAW_WIREFRAME)
        glitter_editor_gl_draw_wireframe();
}

static void glitter_editor_gl_process() {
    if (!glitter_editor.effect_group) {
        glitter_editor_reset_draw();
        return;
    }

    glitter_editor_gl_select_particle();
}

static void glitter_editor_gl_free() {
    shader_fbo_free(&gl_data.wireframe.shader);
}

static void glitter_editor_gl_draw_wireframe() {
    glitter_scene* sc = GPM_VAL->scene;
    glitter_effect_inst* eff = GPM_VAL->effect;
    glitter_emitter_inst* emit = GPM_VAL->emitter;
    glitter_particle_inst* ptcl = GPM_VAL->particle;
    if (!GPM_VAL->draw_selected || !eff) {
        if (!sc)
            return;

        size_t count = 0;
        for (glitter_scene_effect* i = sc->effects.begin; i != sc->effects.end; i++)
            if (i->draw && i->ptr)
                count += glitter_editor_gl_draw_wireframe_calc(i->ptr, 0);

        if (count == 0)
            return;

        for (glitter_scene_effect* i = sc->effects.begin; i != sc->effects.end; i++)
            if (i->draw && i->ptr)
                glitter_editor_gl_draw_wireframe_draw(i->ptr, 0);
    }
    else if ((eff && ptcl) || (eff && !emit)) {
        size_t count = glitter_editor_gl_draw_wireframe_calc(eff, ptcl);
        if (count == 0)
            return;

        glitter_editor_gl_draw_wireframe_draw(eff, ptcl);
    }
    else if (emit) {
        size_t count = 0;
        for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            count += glitter_editor_gl_draw_wireframe_calc(eff, particle);
            if (particle->data.children.begin) {
                vector_ptr_glitter_particle_inst* children = &particle->data.children;
                for (glitter_particle_inst** j = children->begin; j != children->end; j++)
                    if (*j)
                        count += glitter_editor_gl_draw_wireframe_calc(eff, *j);
            }
        }

        if (count == 0)
            return;

        for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            glitter_editor_gl_draw_wireframe_draw(eff, particle);
            if (particle->data.children.begin) {
                vector_ptr_glitter_particle_inst* children = &particle->data.children;
                for (glitter_particle_inst** j = children->begin; j != children->end; j++)
                    if (*j)
                        glitter_editor_gl_draw_wireframe_draw(eff, *j);
            }
        }
    }
}

static size_t glitter_editor_gl_draw_wireframe_calc(glitter_effect_inst* eff,
    glitter_particle_inst* ptcl) {
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

static void glitter_editor_gl_draw_wireframe_draw(glitter_effect_inst* eff,
    glitter_particle_inst* ptcl) {
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shader_fbo_use(&gl_data.wireframe.shader);
    shader_fbo_set_vec4(&gl_data.wireframe.shader, "color", ((vec4) { 1.0f, 1.0f, 0.0f, 1.0f }));
    for (glitter_render_group** i = eff->render_scene.begin; i != eff->render_scene.end; i++) {
        if (!*i)
            continue;

        glitter_render_group* rg = *i;
        if (ptcl && rg->particle != ptcl)
            continue;
        else if (rg->disp < 1)
            continue;

        shader_fbo_set_mat4(&gl_data.wireframe.shader, "model", false, rg->mat_draw);

        bind_vertex_array(rg->vao);
        switch (rg->type) {
        case GLITTER_PARTICLE_QUAD:
            glDrawElements(GL_TRIANGLES, (GLsizei)(6 * rg->disp), GL_UNSIGNED_INT, 0);
            break;
        case GLITTER_PARTICLE_LINE:
        case GLITTER_PARTICLE_LOCUS: {
            const GLenum mode = rg->type == GLITTER_PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            const size_t count = rg->vec_key.end - rg->vec_key.begin;
            for (size_t i = 0; i < count; i++)
                glDrawArrays(mode, rg->vec_key.begin[i], rg->vec_val.begin[i]);
        } break;
        }
    }
    bind_vertex_array(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader_fbo_use(0);
}

static void glitter_editor_gl_select_particle() {
    GPM_VAL->scene = 0;
    GPM_VAL->effect = 0;
    GPM_VAL->emitter = 0;
    GPM_VAL->particle = 0;
    GPM_VAL->draw_selected = false;

    glitter_effect_group* eg = glitter_editor.effect_group;
    glitter_scene* sc = 0;
    for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i)
            continue;

        glitter_scene* scene = *i;
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
        if (!i->draw || !i->ptr)
            continue;

        glitter_effect_inst* effect = i->ptr;
        if (effect->effect != selected_effect)
            continue;

        eff = effect;
        for (glitter_emitter_inst** j = effect->emitters.begin; j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            glitter_emitter_inst* emitter = *j;
            if (emitter->emitter != selected_emitter)
                continue;

            emit = emitter;
            for (glitter_particle_inst** k = emitter->particles.begin; k != emitter->particles.end; k++) {
                if (!*k)
                    continue;

                glitter_particle_inst* particle = *k;
                if (particle->particle == selected_particle) {
                    ptcl = particle;
                    break;
                }
            }
            break;
        }
        break;
    }

    if (ptcl && ptcl->data.children.begin) {
        vector_ptr_glitter_particle_inst* children = &ptcl->data.children;
        ptcl = 0;
        for (glitter_particle_inst** i = children->begin; i != children->end; i++) {
            if (!*i)
                continue;

            glitter_particle_inst* particle = *i;
            if (particle->particle == selected_particle) {
                ptcl = particle;
                break;
            }
        }

        if (!ptcl)
            return;
    }

    GPM_VAL->scene = sc;
    GPM_VAL->effect = eff;
    GPM_VAL->emitter = emit;
    GPM_VAL->particle = ptcl;
    GPM_VAL->draw_selected = glitter_editor.draw_flags & GLITTER_EDITOR_DRAW_SELECTED;
}
#endif
