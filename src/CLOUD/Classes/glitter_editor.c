/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_editor.h"
#include "../../CRE/Glitter/diva_effect.h"
#include "../../CRE/Glitter/diva_list.h"
#include "../../CRE/Glitter/diva_resource.h"
#include "../../CRE/Glitter/effect_group.h"
#include "../../CRE/Glitter/file_reader.h"
#include "../../CRE/Glitter/particle_manager.h"
#include "../../CRE/Glitter/scene.h"
#include "../../CRE/microui.h"
#include "../input.h"
#include <windows.h>
#include <commdlg.h> 
#include <shobjidl.h> 

extern mu_Context* muctx;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern float_t frame_speed;

extern glitter_particle_manager* gpm;

extern bool close;

const char* glitter_editor_window_title = "Glitter Editor";

glitter_editor_struct glitter_editor;

static void glitter_editor_load();
static void glitter_editor_save();
static void glitter_editor_open_window();
static void glitter_editor_save_window();
static void glitter_editor_save_as_window();
static void glitter_editor_load_file(wchar_t* path, wchar_t* file);
static void glitter_editor_save_file(wchar_t* path, wchar_t* file);
static bool glitter_editor_list_open_window(glitter_effect_group* eg);
static bool glitter_editor_list_parse(char* data, size_t length, char*** lines, size_t* count);

static void glitter_editor_mui_show_animation(vector_ptr_glitter_curve* curve);

static glitter_effect* selected_effect;
static glitter_emitter* selected_emitter;
static glitter_particle* selected_particle;
static vector_ptr_glitter_curve* selected_animation;
static glitter_curve* selected_curve;

void glitter_editor_dispose() {
    vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)glitter_effect_group_dispose);
    glitter_editor.file[0] = 0;
    glitter_editor.load = false;
    glitter_editor.save = false;
    glitter_editor.save_big_endian = false;
    glitter_editor.input_play = false;
    glitter_editor.input_stop = false;
    glitter_editor.input_pause = true;
    glitter_editor.input_reload = false;
    glitter_editor.frame_counter = 0.0f;
    glitter_editor.old_frame_counter = 0.0f;
    glitter_editor.effect_group = 0;
    glitter_editor.scene = 0;
    glitter_editor.enabled = false;
    glitter_editor.dispose = false;
    glitter_editor.disposed = true;
}

void glitter_editor_init() {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    gpm->counter = (uint32_t)time.LowPart;
    gpm->random = (uint32_t)(time.LowPart * 0x0CAD3078);
    glitter_editor.enabled = true;
    gpm->emission = 1.0f;
}

void glitter_editor_control() {
    if (!glitter_editor.enabled)
        return;
}

void glitter_editor_input() {
    if (!glitter_editor.enabled)
        return;

    if (input_is_down(VK_CONTROL) && input_is_tapped('O'))
        glitter_editor_open_window();
    else if (input_is_down(VK_CONTROL) && input_is_down(VK_SHIFT) && input_is_tapped('S'))
        glitter_editor_save_as_window();
    else if (input_is_down(VK_CONTROL) && input_is_tapped('S'))
        glitter_editor_save_window();
}

void glitter_editor_mui() {
    if (!glitter_editor.enabled)
        return;
    else if (glitter_editor.disposed) {
        mu_Container* cnt = mu_get_container(muctx, glitter_editor_window_title);
        muctx->hover_root = muctx->next_hover_root = cnt;
        cnt->rect = mu_rect(0, 0, 0, 0);
        cnt->open = true;
        mu_bring_to_front(muctx, cnt);
        glitter_editor.disposed = false;
    }

    char buf[0x80];

    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;

    sprintf_s(buf, sizeof(buf), "Start Frame: %.0f / End Frame: %.0f", glitter_editor.start_frame, glitter_editor.end_frame);
    int32_t top_bar_end_frame_text_width = muctx->text_width(muctx->style->font, buf, strlen(buf));

    x = min(width / 3, 360);
    y = 0;
    w = width - min(width / 3, 360) * 2;
    h = muctx->style->title_height + muctx->style->spacing + 2;

    if (w < (600 + top_bar_end_frame_text_width + (muctx->style->spacing + 2) * 11))
        h += muctx->style->scrollbar_size;

    if (mu_begin_window_ex(muctx, "Glitter Editor Top Bar",
        mu_rect(x, y, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NOCLOSE | MU_OPT_NOTITLE, true, true, true)) {
        mu_layout_row(muctx, 11, (int[]) { 80, 40, 40, 40, 60, 80, 80, 80, 40, 60, top_bar_end_frame_text_width }, 0);
        if (mu_button(muctx, "Reset Camera"))
            input_reset = true;

        if (mu_button(muctx, "Play"))
            glitter_editor.input_play = true;

        if (mu_button(muctx, "Pause"))
            glitter_editor.input_pause = true;

        if (mu_button(muctx, "Stop"))
            glitter_editor.input_stop = true;

        mu_label(muctx, "Emission:");
        mu_slider_step(muctx, &gpm->emission, 1.0f, 2.0f, 0.01f);

        mu_label(muctx, "Frame Speed:");
        mu_slider_step(muctx, &frame_speed, 0.0f, 3.0f, 0.01f);

        float_t frame_counter = (float_t)(int32_t)glitter_editor.frame_counter;
        mu_label(muctx, "Frame: ");
        if (mu_slider_ex(muctx, &frame_counter, glitter_editor.start_frame,
            glitter_editor.end_frame, 1.0f, "%.0f", MU_OPT_ALIGNCENTER) & MU_RES_CHANGE) {
            glitter_editor.old_frame_counter = glitter_editor.frame_counter;
            glitter_editor.frame_counter = (float_t)(int32_t)frame_counter;
        }

        mu_text(muctx, buf);
        mu_end_window(muctx);
    }

    x = 0;
    y = 0;
    w = min(width / 3, 360);
    h = height - y;

    selected_effect = 0;
    selected_emitter = 0;
    selected_particle = 0;
    selected_animation = 0;
    selected_curve = 0;

    if (mu_begin_window_ex(muctx, "Glitter Editor Left Window",
        mu_rect(x, y, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NOCLOSE | MU_OPT_NOTITLE, true, true, false)) {
        if (!glitter_editor.effect_group)
            goto editor_left_window_end;

        glitter_effect** i;
        glitter_emitter** j;
        glitter_particle** k;
        glitter_effect_group* eg = glitter_editor.effect_group;
        size_t i_idx = 1;
        for (i = eg->effects.begin; i != eg->effects.end; i++, i_idx++) {
            if (!*i)
                continue;

            glitter_effect* effect = *i;
            mu_push_id(muctx, &i_idx, sizeof(i_idx));
            sprintf_s(buf, sizeof(buf), "Effect %lld", i_idx);
            if (mu_begin_treenode(muctx, buf)) {
                if (!selected_effect)
                    selected_effect = effect;

                glitter_editor_mui_show_animation(&effect->curve);

                size_t j_idx = 1;
                for (j = effect->emitters.begin; j != effect->emitters.end; j++, j_idx++) {
                    if (!*j)
                        continue;

                    glitter_emitter* emitter = *j;
                    mu_push_id(muctx, &j_idx, sizeof(j_idx));
                    sprintf_s(buf, sizeof(buf), "Emitter %lld", j_idx);
                    if (mu_begin_treenode(muctx, buf)) {
                        if (!selected_emitter)
                            selected_emitter = emitter;

                        glitter_editor_mui_show_animation(&emitter->curve);

                        size_t k_idx = 1;
                        for (k = emitter->particles.begin; k != emitter->particles.end; k++, k_idx++) {
                            if (!*k)
                                continue;

                            glitter_particle* particle = *k;
                            mu_push_id(muctx, &k_idx, sizeof(k_idx));
                            sprintf_s(buf, sizeof(buf), "Particle %lld", k_idx);
                            if (mu_begin_treenode(muctx, buf)) {
                                if (!selected_particle)
                                    selected_particle = particle;

                                glitter_editor_mui_show_animation(&particle->curve);

                                mu_end_treenode(muctx);
                            }
                            mu_pop_id(muctx);
                        }
                        mu_end_treenode(muctx);
                    }
                    mu_pop_id(muctx);
                }
                mu_end_treenode(muctx);
            }
            mu_pop_id(muctx);
        }

    editor_left_window_end:
        mu_end_window(muctx);
    }

    const char* glitter_direction_name[] = {
        [GLITTER_DIRECTION_BILLBOARD]         = "Billboard",
        [GLITTER_DIRECTION_EMITTER_DIRECTION] = "Emitter Direction",
        [GLITTER_DIRECTION_TYPE_2_DIR_ANGLE]  = "Type 2 Dir Angle",
        [GLITTER_DIRECTION_TYPE_3_DIR_ANGLE]  = "Type 3 Dir Angle",
        [GLITTER_DIRECTION_Y_AXIS]            = "Y Axis",
        [GLITTER_DIRECTION_X_AXIS]            = "X Axis",
        [GLITTER_DIRECTION_Z_AXIS]            = "Z Axis",
        [GLITTER_DIRECTION_BILLBOARD_Y_ONLY]  = "Billboard Y Only",
        [GLITTER_DIRECTION_TYPE_8_DIR_ANGLE]  = "Type 8 Dir Angle",
        [GLITTER_DIRECTION_EMITTER_ROTATION]  = "Emitter Rotation",
        [GLITTER_DIRECTION_EFFECT_ROTATION]   = "Effect Rotation",
        [GLITTER_DIRECTION_PARTICLE_ROTATION] = "Particle Rotation",
    };

    const char* glitter_emitter_name[] = {
        [GLITTER_EMITTER_BOX]      = "Box",
        [GLITTER_EMITTER_CYLINDER] = "Cylinder",
        [GLITTER_EMITTER_SPHERE]   = "Sphere",
        [GLITTER_EMITTER_MESH]     = "Mesh",
        [GLITTER_EMITTER_POLYGON]  = "Polygon",
    };

    const char* glitter_emitter_emission_direction_name[] = {
        [GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY] = "Particle Velocity",
        [GLITTER_EMITTER_EMISSION_DIRECTION_INWARD]            = "Inward",
        [GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD]           = "Outward",
    };

    const char* glitter_particle_blend_name[] = {
        [GLITTER_PARTICLE_BLEND_ZERO]          = "Zero",
        [GLITTER_PARTICLE_BLEND_TYPICAL]       = "Typical",
        [GLITTER_PARTICLE_BLEND_ADD]           = "Add",
        [GLITTER_PARTICLE_BLEND_SUBTRACT]      = "Subtract",
        [GLITTER_PARTICLE_BLEND_MULTIPLY]      = "Multiply",
        [GLITTER_PARTICLE_BLEND_PUNCH_THROUGH] = "Punch Through",
    };

    const char* glitter_particle_name[] = {
        [GLITTER_PARTICLE_QUAD]  = "Quad",
        [GLITTER_PARTICLE_LINE]  = "Line",
        [GLITTER_PARTICLE_LOCUS] = "Locus",
        [GLITTER_PARTICLE_MESH]  = "Mesh",
    };

    const char* glitter_pivot_name[] = {
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

    const char* glitter_uv_index_type_name[] = {
        [GLITTER_UV_INDEX_FIXED]                  = "Fixed",
        [GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED]   = "Initial Random / Fixed",
        [GLITTER_UV_INDEX_RANDOM]                 = "Random",
        [GLITTER_UV_INDEX_FORWARD]                = "Forward",
        [GLITTER_UV_INDEX_REVERSE]                = "Reverse",
        [GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD] = "Initial Random / Forward",
        [GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE] = "Initial Random / Reverse",
        [GLITTER_UV_INDEX_USER]                   = "User",
    };

    x = width - min(width / 3, 360);
    y = 0;
    w = min(width / 3, 360);
    h = height - y;

    if (mu_begin_window_ex(muctx, "Glitter Editor Right Window",
        mu_rect(x, y, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NOCLOSE | MU_OPT_NOTITLE, true, true, false)) {
        if (!glitter_editor.effect_group || !selected_effect)
            goto editor_right_window_end;

        mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
        mu_push_id(muctx, &selected_effect, sizeof(glitter_effect*));
        if (selected_effect && mu_header_ex(muctx, "Effect", MU_OPT_EXPANDED)) {
            int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Name:", "Translation", "Rotation", "Scale",
                    "Appear Time:", "Life Time:", "Start Time:", "Emission:"
            }, 8);
            int32_t tw1 = mu_misc_get_max_text_width(muctx, (char* []) {
                "X:"
            }, 1);
            int32_t tw2 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Y:"
            }, 1);
            int32_t tw3 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Z:"
            }, 1);
            int32_t sw = (int)((body.w - (tw0 + 4) - (tw1 + 2) - (tw2 + 2) - (tw3 + 2) - (muctx->style->spacing + 2) * 3) / 3.0f);

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
            glitter_effect effect = *selected_effect;
            char name[0x80];
            memcpy(name, effect.name, 0x80);
            mu_label(muctx, "Name:");
            if (mu_button(muctx, name)) {
                effect.data.name_hash = gpm->f2 ? hash_char_murmurhash(name, 0, false) : hash_char_fnv1a64(name);
                memcpy(effect.name, name, 0x80);
            }

            mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

            vec3 translation = effect.translation;
            mu_label(muctx, "Translation");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &translation.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &translation.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &translation.z, 0.0001f, "%g");
            effect.translation = translation;

            vec3 rotation = effect.rotation;
            vec3_mult_scalar(rotation, (float_t)(180.0 / M_PI), rotation);
            mu_label(muctx, "Rotation");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation, (float_t)(M_PI / 180.0), rotation);
            effect.rotation = rotation;

            vec3 scale = effect.scale;
            mu_label(muctx, "Scale");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &scale.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &scale.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &scale.z, 0.0001f, "%g");
            effect.scale = scale;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t appear_time = (float_t)(int32_t)effect.data.appear_time;
            mu_label(muctx, "Appear Time:");
            mu_number_fmt(muctx, &appear_time, 1.0f, "%.0f");
            appear_time = clamp(appear_time, 0.0f, 32767.0f);
            effect.data.appear_time = (float_t)(int32_t)appear_time;

            float_t life_time = (float_t)(int32_t)effect.data.life_time;
            mu_label(muctx, "Life Time:");
            mu_number_fmt(muctx, &life_time, 1.0f, "%.0f");
            life_time = clamp(life_time, 0.0f, 32767.0f - appear_time);
            effect.data.life_time = (float_t)(int32_t)life_time;

            float_t start_time = (float_t)(int32_t)effect.data.start_time;
            mu_label(muctx, "Start Time:");
            mu_number_fmt(muctx, &start_time, 1.0f, "%.0f");
            start_time = clamp(start_time, 0.0f, 32767.0f - appear_time);
            effect.data.start_time = (float_t)(int32_t)start_time;

            mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

            glitter_effect_flag flags = effect.data.flags;
            bool loop = flags & GLITTER_EFFECT_FLAG_LOOP ? true : false;
            mu_checkbox(muctx, "Loop", &loop);
            if (loop)
                flags |= GLITTER_EFFECT_FLAG_LOOP;
            else
                flags &= ~GLITTER_EFFECT_FLAG_LOOP;

            bool local = flags & GLITTER_EFFECT_FLAG_LOCAL ? true : false;
            mu_checkbox(muctx, "Local", &local);
            if (local)
                flags |= GLITTER_EFFECT_FLAG_LOCAL;
            else
                flags &= ~GLITTER_EFFECT_FLAG_LOCAL;

            bool alpha = flags & GLITTER_EFFECT_FLAG_ALPHA ? true : false;
            mu_checkbox(muctx, "Alpha", &alpha);
            if (alpha)
                flags |= GLITTER_EFFECT_FLAG_ALPHA;
            else
                flags &= ~GLITTER_EFFECT_FLAG_ALPHA;

            bool fog = flags & GLITTER_EFFECT_FLAG_FOG ? true : false;
            mu_checkbox(muctx, "Fog", &fog);
            if (fog) {
                flags |= GLITTER_EFFECT_FLAG_FOG;
                flags &= ~GLITTER_EFFECT_FLAG_FOG_HEIGHT;
            }
            else
                flags &= ~GLITTER_EFFECT_FLAG_FOG;

            bool fog_height = flags & GLITTER_EFFECT_FLAG_FOG_HEIGHT ? true : false;
            mu_checkbox(muctx, "Fog Height", &fog_height);
            if (fog_height) {
                flags |= GLITTER_EFFECT_FLAG_FOG_HEIGHT;
                flags &= ~GLITTER_EFFECT_FLAG_FOG;
            }
            else
                flags &= ~GLITTER_EFFECT_FLAG_FOG_HEIGHT;

            bool use_emission = flags & GLITTER_EFFECT_FLAG_EMISSION ? true : false;
            mu_checkbox(muctx, "Use Emission", &use_emission);
            if (use_emission)
                flags |= GLITTER_EFFECT_FLAG_EMISSION;
            else
                flags &= ~GLITTER_EFFECT_FLAG_EMISSION;
            effect.data.flags = flags;

            uint32_t version = effect.version;
            bool set_emission = version == 7 ? true : false;
            mu_checkbox(muctx, "Set Emission", &set_emission);
            effect.version = set_emission ? 7 : 6;

            if (version == 7) {
                float_t emission = effect.data.emission;
                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
                mu_label(muctx, "Emission:");
                mu_number_fmt(muctx, &emission, 0.0001f, "%g");
                effect.data.emission = max(emission, 0.0f);
            }
            
            uint64_t hash1 = hash_fnv1a64((void*)selected_effect, sizeof(glitter_effect));
            uint64_t hash2 = hash_fnv1a64((void*)&effect, sizeof(glitter_effect));
            if (hash1 != hash2) {
                for (glitter_emitter** i = selected_effect->emitters.begin; i != selected_effect->emitters.end; i++) {
                    if (!*i)
                        continue;

                    glitter_emitter* emitter = *i;
                    for (glitter_particle** j = emitter->particles.begin; j != emitter->particles.end; j++) {
                        if (!*j)
                            continue;

                        glitter_particle* particle = *j;
                        if (effect.data.flags & GLITTER_EFFECT_FLAG_LOCAL)
                            particle->data.data.flags |= GLITTER_PARTICLE_FLAG_LOCAL;
                        else
                            particle->data.data.flags &= ~GLITTER_PARTICLE_FLAG_LOCAL;

                        if (effect.data.flags & GLITTER_EFFECT_FLAG_EMISSION)
                            particle->data.data.flags |= GLITTER_PARTICLE_FLAG_EMISSION;
                        else
                            particle->data.data.flags &= ~GLITTER_PARTICLE_FLAG_EMISSION;
                    }
                }
                glitter_editor.input_reload = true;
            }
            *selected_effect = effect;
        }
        mu_pop_id(muctx);

        mu_push_id(muctx, &selected_emitter, sizeof(glitter_emitter*));
        if (selected_emitter && mu_header_ex(muctx, "Emitter", MU_OPT_EXPANDED)) {
            int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Translation", "Rotation", "Scale", "Rotation Add", "Start Time:", "Life Time:",
                    "Loop Start Time:", "Loop End Time:", "Emission Interval:", "Particles Per Emission:",
                    "Direction:", "Loop:", "Size", "Radius:", "Height:", "Start Angle:", "End Angle:",
                    "Plain:", "Direction:", "Latitude:", "Longitude:",  "Scale:", "Count:"
            }, 23);
            int32_t tw1 = mu_misc_get_max_text_width(muctx, (char* []) {
                "X:"
            }, 1);
            int32_t tw2 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Y:"
            }, 1);
            int32_t tw3 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Z:"
            }, 1);
            int32_t sw = (int)((body.w - (tw0 + 4) - (tw1 + 2) - (tw2 + 2) - (tw3 + 2) - (muctx->style->spacing + 2) * 3) / 3.0f);

            glitter_emitter emitter = *selected_emitter;
            mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

            vec3 translation = emitter.translation;
            mu_label(muctx, "Translation");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &translation.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &translation.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &translation.z, 0.0001f, "%g");
            emitter.translation = translation;

            vec3 rotation = emitter.rotation;
            vec3_mult_scalar(rotation, (float_t)(180.0 / M_PI), rotation);
            mu_label(muctx, "Rotation");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation, (float_t)(M_PI / 180.0), rotation);
            emitter.rotation = rotation;

            vec3 scale = emitter.scale;
            mu_label(muctx, "Scale");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &scale.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &scale.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &scale.z, 0.0001f, "%g");
            emitter.scale = scale;

            vec3 rotation_add = emitter.data.rotation_add;
            vec3_mult_scalar(rotation_add, (float_t)(180.0 / M_PI), rotation_add);
            mu_label(muctx, "Rotation Add");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation_add.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation_add.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation_add.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation_add, (float_t)(M_PI / 180.0), rotation_add);
            emitter.data.rotation_add = rotation_add;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t start_time = (float_t)(int32_t)emitter.data.start_time;
            mu_label(muctx, "Start Time:");
            mu_number_fmt(muctx, &start_time, 1.0f, "%.0f");
            start_time = clamp(start_time, 0.0f, 32767.0f);
            emitter.data.start_time = (float_t)(int32_t)start_time;

            float_t life_time = (float_t)(int32_t)emitter.data.life_time;
            mu_label(muctx, "Life Time:");
            mu_number_fmt(muctx, &life_time, 1.0f, "%.0f");
            life_time = clamp(life_time, 0.0f, 32767.0f);
            emitter.data.life_time = (float_t)(int32_t)life_time;

            float_t loop_start_time = (float_t)(int32_t)emitter.data.loop_start_time;
            mu_label(muctx, "Loop Start Time:");
            mu_number_fmt(muctx, &loop_start_time, 1.0f, "%.0f");
            if (loop_start_time < start_time)
                loop_start_time = 0.0f;
            else
                loop_start_time = clamp(loop_start_time, start_time, start_time + life_time);
            emitter.data.loop_start_time = (float_t)(int32_t)loop_start_time;

            float_t loop_end_time = (float_t)(int32_t)emitter.data.loop_end_time;
            mu_label(muctx, "Loop End Time:");
            mu_number_fmt(muctx, &loop_end_time, 1.0f, "%.0f");
            if (loop_end_time < 0.0f)
                loop_end_time = -1.0f;
            else
                loop_end_time = clamp(loop_end_time, loop_start_time, start_time + life_time);
            emitter.data.loop_end_time = (float_t)(int32_t)loop_end_time;

            float_t emission_interval = emitter.data.emission_interval;
            mu_label(muctx, "Emission Interval:");
            mu_number_fmt(muctx, &emission_interval, 1.0f, "%.0f");
            emitter.data.emission_interval = clamp(emission_interval, 0.0f, life_time);

            float_t particles_per_emission = emitter.data.particles_per_emission;
            mu_label(muctx, "Particles Per Emission:");
            mu_number_fmt(muctx, &particles_per_emission, 1.0f, "%.0f");
            emitter.data.particles_per_emission = max(particles_per_emission, 0.0f);

            float_t direction = (float_t)emitter.data.direction;
            direction = clamp(direction, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION);
            mu_label(muctx, "Direction:");
            mu_slider_ex(muctx, &direction, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION,
                1.0f, glitter_direction_name[emitter.data.direction], MU_OPT_ALIGNCENTER);
            direction = clamp(direction, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION);
            emitter.data.direction = (int32_t)direction;

            mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

            glitter_emitter_flag flags = emitter.data.flags;
            bool loop = flags & GLITTER_EMITTER_FLAG_LOOP ? true : false;
            mu_checkbox(muctx, "Loop", &loop);
            if (loop)
                flags |= GLITTER_EMITTER_FLAG_LOOP;
            else
                flags &= ~GLITTER_EMITTER_FLAG_LOOP;
            emitter.data.flags = flags;

            float_t type = (float_t)emitter.data.type;
            type = clamp(type, GLITTER_EMITTER_BOX, GLITTER_EMITTER_POLYGON);
            mu_label(muctx, "Type:");
            mu_slider_ex(muctx, &type, GLITTER_EMITTER_BOX, GLITTER_EMITTER_POLYGON,
                1.0f, glitter_emitter_name[emitter.data.type], MU_OPT_ALIGNCENTER);
            type = clamp(type, GLITTER_EMITTER_BOX, GLITTER_EMITTER_POLYGON);
            emitter.data.type = (int32_t)type;

            switch (emitter.data.type) {
            case GLITTER_EMITTER_BOX: {
                mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);
                vec3 size = emitter.data.box.size;
                mu_label(muctx, "Size");
                mu_label(muctx, "X:");
                mu_number_fmt(muctx, &size.x, 0.0001f, "%g");
                size.x = max(size.x, 0.0f);
                mu_label(muctx, "Y:");
                mu_number_fmt(muctx, &size.y, 0.0001f, "%g");
                size.y = max(size.y, 0.0f);
                mu_label(muctx, "Z:");
                mu_number_fmt(muctx, &size.z, 0.0001f, "%g");
                size.z = max(size.z, 0.0f);
                emitter.data.box.size = size;
            } break;
            case GLITTER_EMITTER_CYLINDER: {
                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
                float_t radius = emitter.data.cylinder.radius;
                mu_label(muctx, "Radius:");
                mu_number_fmt(muctx, &radius, 0.0001f, "%g");
                emitter.data.cylinder.radius = max(radius, 0.0f);

                float_t height = emitter.data.cylinder.height;
                mu_label(muctx, "Height:");
                mu_number_fmt(muctx, &height, 0.0001f, "%g");
                emitter.data.cylinder.height = max(height, 0.0f);

                float_t start_angle = emitter.data.cylinder.start_angle * (float_t)(180.0 / M_PI);
                mu_label(muctx, "Start Angle:");
                mu_number_fmt(muctx, &start_angle, 0.0001f, "%g");
                emitter.data.cylinder.start_angle = start_angle * (float_t)(M_PI / 180.0);

                float_t end_angle = emitter.data.cylinder.end_angle * (float_t)(180.0 / M_PI);
                mu_label(muctx, "End Angle:");
                mu_number_fmt(muctx, &end_angle, 0.0001f, "%g");
                emitter.data.cylinder.end_angle = end_angle * (float_t)(M_PI / 180.0);

                mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

                bool plain = emitter.data.cylinder.plain;
                mu_checkbox(muctx, "Plain", &plain);
                emitter.data.cylinder.plain = plain;

                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

                float_t direction = (float_t)emitter.data.cylinder.direction;
                direction = clamp(direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD);
                mu_label(muctx, "Direction:");
                mu_slider_ex(muctx, &direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD, 1.0f,
                    glitter_emitter_emission_direction_name[emitter.data.cylinder.direction], MU_OPT_ALIGNCENTER);
                direction = clamp(direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD);
                emitter.data.cylinder.direction = (int32_t)direction;
            } break;
            case GLITTER_EMITTER_SPHERE: {
                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
                float_t radius = emitter.data.sphere.radius;
                mu_label(muctx, "Radius:");
                mu_number_fmt(muctx, &radius, 0.0001f, "%g");
                emitter.data.sphere.radius = max(radius, 0.0f);

                float_t latitude = emitter.data.sphere.latitude * (float_t)(180.0 / M_PI);
                mu_label(muctx, "Latitude:");
                mu_number_fmt(muctx, &latitude, 0.0001f, "%g");
                emitter.data.sphere.latitude = latitude * (float_t)(M_PI / 180.0);

                float_t longitude = emitter.data.sphere.longitude * (float_t)(180.0 / M_PI);
                mu_label(muctx, "Longitude:");
                mu_number_fmt(muctx, &longitude, 0.0001f, "%g");
                emitter.data.sphere.longitude = longitude * (float_t)(M_PI / 180.0);

                mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

                bool plain = emitter.data.sphere.plain;
                mu_checkbox(muctx, "Plain", &plain);
                emitter.data.sphere.plain = plain;

                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

                float_t direction = (float_t)emitter.data.sphere.direction;
                direction = clamp(direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD);
                mu_label(muctx, "Direction:");
                mu_slider_ex(muctx, &direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD, 1.0f,
                    glitter_emitter_emission_direction_name[emitter.data.cylinder.direction], MU_OPT_ALIGNCENTER);
                direction = clamp(direction, GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY,
                    GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD);
                emitter.data.sphere.direction = (int32_t)direction;
            } break;
            case GLITTER_EMITTER_MESH: {

            } break;
            case GLITTER_EMITTER_POLYGON: {
                mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
                float_t scale = emitter.data.polygon.scale;
                mu_label(muctx, "Scale:");
                mu_number_fmt(muctx, &scale, 0.0001f, "%g");
                emitter.data.polygon.scale = max(scale, 0.0f);

                float_t count = (float_t)emitter.data.polygon.count;
                mu_label(muctx, "Count:");
                mu_number_fmt(muctx, &count, 1.0f, "%.0f");
                emitter.data.polygon.count = (int32_t)max(count, 0.0f);
            } break;
            }

            uint64_t hash1 = hash_fnv1a64((void*)selected_emitter, sizeof(glitter_emitter));
            uint64_t hash2 = hash_fnv1a64((void*)&emitter, sizeof(glitter_emitter));
            if (hash1 != hash2)
                glitter_editor.input_reload = true;
            *selected_emitter = emitter;
        }
        mu_pop_id(muctx);

        mu_push_id(muctx, &selected_particle, sizeof(glitter_particle*));
        if (selected_particle && mu_header_ex(muctx, "Particle", MU_OPT_EXPANDED)) {
            int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Life Time:", "Type:", "Pivot:", "Draw Type:", "Z Offset:", "Rotation", "Rotation Random",
                    "Rotation Add", "Rotation Add Random", "Scale", "Scale Random", "Direction:",
                    "Direction Random", "Speed:", "Speed Random:", "Deceleration:", "Deceleration Random:",
                    "Gravitational Acceleration", "External Acceleration", "External Acceleration Random",
                    "Reflection Coeff:", "Reflection Coeff Random:", "Rebound Plane Y:", "Color R:", "Color G:",
                    "Color B:", "Color A:", "UV Index Type:", "UV Split", "UV Index:", "Frame Step UV:",
                    "UV Index Start:", "UV Index End:", "UV Scroll Add", "UV Scroll Add Scale:", "Blend Mode 0:",
                    "Blend Mode 1:", "Count:", "Locus History Size:", "Locus History Size Random:", "Emission:"
            }, 41);
            int32_t tw1 = mu_misc_get_max_text_width(muctx, (char* []) {
                "X:", "U:"
            }, 2);
            int32_t tw2 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Y:", "V:"
            }, 2);
            int32_t tw3 = mu_misc_get_max_text_width(muctx, (char* []) {
                "Z:"
            }, 1);
            int32_t sw = (int)((body.w - (tw0 + 4) - (tw1 + 2) - (tw2 + 2) - (tw3 + 2) - (muctx->style->spacing + 2) * 3) / 3.0f);

            glitter_particle particle = *selected_particle;
            mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

            glitter_particle_flag flags = particle.data.data.flags;
            bool loop = flags & GLITTER_PARTICLE_FLAG_LOOP ? true : false;
            mu_checkbox(muctx, "Loop", &loop);
            if (loop)
                flags |= GLITTER_PARTICLE_FLAG_LOOP;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_LOOP;

            bool use_model_mat = flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT ? true : false;
            mu_checkbox(muctx, "Use Model Mat", &use_model_mat);
            if (use_model_mat)
                flags |= GLITTER_PARTICLE_FLAG_USE_MODEL_MAT;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_USE_MODEL_MAT;

            bool scale_y_by_x = flags & GLITTER_PARTICLE_FLAG_SCALE_Y_BY_X ? true : false;
            mu_checkbox(muctx, "Scale Y by X", &scale_y_by_x);
            if (scale_y_by_x)
                flags |= GLITTER_PARTICLE_FLAG_SCALE_Y_BY_X;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_SCALE_Y_BY_X;

            bool rebound_plane = flags & GLITTER_PARTICLE_FLAG_REBOUND_PLANE ? true : false;
            mu_checkbox(muctx, "Rebound Plane", &rebound_plane);
            if (rebound_plane)
                flags |= GLITTER_PARTICLE_FLAG_REBOUND_PLANE;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_REBOUND_PLANE;

            bool translate_by_emitter = flags & GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER ? true : false;
            mu_checkbox(muctx, "Translate by Emitter", &translate_by_emitter);
            if (translate_by_emitter)
                flags |= GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER;

            bool use_scale = flags & GLITTER_PARTICLE_FLAG_SCALE ? true : false;
            mu_checkbox(muctx, "Use Scale", &use_scale);
            if (use_scale)
                flags |= GLITTER_PARTICLE_FLAG_SCALE;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_SCALE;

            bool second_texture = flags & GLITTER_PARTICLE_FLAG_SECOND_TEXTURE ? true : false;
            mu_checkbox(muctx, "Second Texture", &second_texture);
            if (second_texture)
                flags |= GLITTER_PARTICLE_FLAG_SECOND_TEXTURE;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_SECOND_TEXTURE;

            bool local = flags & GLITTER_PARTICLE_FLAG_LOCAL ? true : false;
            mu_checkbox(muctx, "Local", &local);
            if (local)
                flags |= GLITTER_PARTICLE_FLAG_LOCAL;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_LOCAL;

            bool use_emission = flags & GLITTER_PARTICLE_FLAG_EMISSION ? true : false;
            mu_checkbox(muctx, "Use Emission", &use_emission);
            if (use_emission)
                flags |= GLITTER_PARTICLE_FLAG_EMISSION;
            else
                flags &= ~GLITTER_PARTICLE_FLAG_EMISSION;
            particle.data.data.flags = flags;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
            float_t life_time = particle.data.data.life_time;
            mu_label(muctx, "Life Time:");
            mu_number_fmt(muctx, &life_time, 0.0001f, "%g");
            particle.data.data.life_time = max(life_time, 0.0f);

            float_t type = (float_t)particle.data.data.type;
            type = clamp(type, GLITTER_PARTICLE_QUAD, GLITTER_PARTICLE_MESH);
            mu_label(muctx, "Type:");
            mu_slider_ex(muctx, &type, GLITTER_PARTICLE_QUAD, GLITTER_PARTICLE_MESH,
                1.0f, glitter_particle_name[particle.data.data.type], MU_OPT_ALIGNCENTER);
            type = clamp(type, GLITTER_PARTICLE_QUAD, GLITTER_PARTICLE_MESH);
            particle.data.data.type = (int32_t)type;

            float_t pivot = (float_t)particle.data.data.pivot;
            pivot = clamp(pivot, GLITTER_PIVOT_TOP_LEFT, GLITTER_PIVOT_BOTTOM_RIGHT);
            mu_label(muctx, "Pivot:");
            mu_slider_ex(muctx, &pivot, GLITTER_PIVOT_TOP_LEFT, GLITTER_PIVOT_BOTTOM_RIGHT,
                1.0f, glitter_pivot_name[particle.data.data.pivot], MU_OPT_ALIGNCENTER);
            pivot = clamp(pivot, GLITTER_PIVOT_TOP_LEFT, GLITTER_PIVOT_BOTTOM_RIGHT);
            particle.data.data.pivot = (int32_t)pivot;

            float_t draw_type = (float_t)particle.data.data.draw_type;
            draw_type = clamp(draw_type, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION);
            mu_label(muctx, "Draw Type:");
            mu_slider_ex(muctx, &draw_type, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION,
                1.0f, glitter_direction_name[particle.data.data.draw_type], MU_OPT_ALIGNCENTER);
            draw_type = clamp(draw_type, GLITTER_DIRECTION_BILLBOARD, GLITTER_DIRECTION_PARTICLE_ROTATION);
            particle.data.data.draw_type = (int32_t)draw_type;

            float_t z_offset = particle.data.data.z_offset;
            mu_label(muctx, "Z Offset:");
            mu_number_fmt(muctx, &z_offset, 0.0001f, "%g");
            particle.data.data.z_offset = max(z_offset, 0.0f);

            mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

            vec3 rotation = particle.data.data.rotation;
            vec3_mult_scalar(rotation, (float_t)(180.0 / M_PI), rotation);
            mu_label(muctx, "Rotation");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation, (float_t)(M_PI / 180.0), rotation);
            particle.data.data.rotation = rotation;

            vec3 rotation_random = particle.data.data.rotation_random;
            vec3_mult_scalar(rotation_random, (float_t)(180.0 / M_PI), rotation_random);
            mu_label(muctx, "Rotation Random");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation_random.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation_random.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation_random.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation_random, (float_t)(M_PI / 180.0), rotation_random);
            particle.data.data.rotation_random = rotation_random;

            vec3 rotation_add = particle.data.data.rotation_add;
            vec3_mult_scalar(rotation_add, (float_t)(180.0 / M_PI), rotation_add);
            mu_label(muctx, "Rotation Add");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation_add.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation_add.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation_add.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation_add, (float_t)(M_PI / 180.0), rotation_add);
            particle.data.data.rotation_add = rotation_add;

            vec3 rotation_add_random = particle.data.data.rotation_add_random;
            vec3_mult_scalar(rotation_add_random, (float_t)(180.0 / M_PI), rotation_add_random);
            mu_label(muctx, "Rotation Add Random");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &rotation_add_random.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &rotation_add_random.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &rotation_add_random.z, 0.0001f, "%g");
            vec3_mult_scalar(rotation_add_random, (float_t)(M_PI / 180.0), rotation_add_random);
            particle.data.data.rotation_add_random = rotation_add_random;

            mu_layout_row(muctx, 5, (int[]) { tw0, tw1, sw, tw2, sw }, 0);

            vec3 scale = particle.data.data.scale;
            mu_label(muctx, "Scale");
            mu_label(muctx, "X:");;
            mu_number_fmt(muctx, &scale.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &scale.y, 0.0001f, "%g");
            particle.data.data.scale = scale;

            vec3 scale_random = particle.data.data.scale_random;
            mu_label(muctx, "Scale Random");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &scale_random.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &scale_random.y, 0.0001f, "%g");
            particle.data.data.scale_random = scale_random;

            mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

            vec3 direction = particle.data.data.direction;
            mu_label(muctx, "Direction");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &direction.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &direction.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &direction.z, 0.0001f, "%g");
            particle.data.data.direction = direction;

            vec3 direction_random = particle.data.data.direction_random;
            mu_label(muctx, "Direction Random");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &direction_random.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &direction_random.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &direction_random.z, 0.0001f, "%g");
            particle.data.data.direction_random = direction_random;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t speed = particle.data.data.speed;
            mu_label(muctx, "Speed:");
            mu_number_fmt(muctx, &speed, 0.0001f, "%g");
            particle.data.data.speed = max(speed, 0.0f);

            float_t speed_random = particle.data.data.speed_random;
            mu_label(muctx, "Speed Random:");
            mu_number_fmt(muctx, &speed_random, 0.0001f, "%g");
            particle.data.data.speed_random = max(speed_random, 0.0f);

            float_t deceleration = particle.data.data.deceleration;
            mu_label(muctx, "Deceleration:");
            mu_number_fmt(muctx, &deceleration, 0.0001f, "%g");
            particle.data.data.deceleration = max(deceleration, 0.0f);

            float_t deceleration_random = particle.data.data.deceleration_random;
            mu_label(muctx, "Deceleration Random:");
            mu_number_fmt(muctx, &deceleration_random, 0.0001f, "%g");
            particle.data.data.deceleration_random = max(deceleration_random, 0.0f);

            mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

            vec3 gravitational_acceleration = particle.data.data.gravitational_acceleration;
            mu_label(muctx, "Gravitational Acceleration");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &gravitational_acceleration.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &gravitational_acceleration.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &gravitational_acceleration.z, 0.0001f, "%g");
            particle.data.data.gravitational_acceleration = gravitational_acceleration;

            vec3 external_acceleration = particle.data.data.external_acceleration;
            mu_label(muctx, "External Acceleration");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &external_acceleration.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &external_acceleration.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &external_acceleration.z, 0.0001f, "%g");
            particle.data.data.external_acceleration = external_acceleration;

            vec3 external_acceleration_random = particle.data.data.external_acceleration_random;
            mu_label(muctx, "External Acceleration Random");
            mu_label(muctx, "X:");
            mu_number_fmt(muctx, &external_acceleration_random.x, 0.0001f, "%g");
            mu_label(muctx, "Y:");
            mu_number_fmt(muctx, &external_acceleration_random.y, 0.0001f, "%g");
            mu_label(muctx, "Z:");
            mu_number_fmt(muctx, &external_acceleration_random.z, 0.0001f, "%g");
            particle.data.data.external_acceleration_random = external_acceleration_random;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t reflection_coeff = particle.data.data.reflection_coeff;
            mu_label(muctx, "Reflection Coeff:");
            mu_number_fmt(muctx, &reflection_coeff, 0.0001f, "%g");
            particle.data.data.reflection_coeff = reflection_coeff;

            float_t reflection_coeff_random = particle.data.data.reflection_coeff_random;
            mu_label(muctx, "Reflection Coeff Random:");
            mu_number_fmt(muctx, &reflection_coeff_random, 0.0001f, "%g");
            particle.data.data.reflection_coeff_random = reflection_coeff_random;

            float_t rebound_plane_y = particle.data.data.rebound_plane_y;
            mu_label(muctx, "Rebound Plane Y:");
            mu_number_fmt(muctx, &rebound_plane_y, 0.0001f, "%g");
            particle.data.data.rebound_plane_y = rebound_plane_y;

            float_t color_r = particle.data.data.color.x * 255.0f;
            mu_label(muctx, "Color R:");
            mu_slider_fmt(muctx, &color_r, 0.0f, 255.0f, 1.0f, "%.0f");
            particle.data.data.color.x = color_r * (float_t)(1.0 / 255.0);

            float_t color_g = particle.data.data.color.y * 255.0f;
            mu_label(muctx, "Color G:");
            mu_slider_fmt(muctx, &color_g, 0.0f, 255.0f, 1.0f, "%.0f");
            particle.data.data.color.y = color_g * (float_t)(1.0 / 255.0);

            float_t color_b = particle.data.data.color.z * 255.0f;
            mu_label(muctx, "Color B:");
            mu_slider_fmt(muctx, &color_b, 0.0f, 255.0f, 1.0f, "%.0f");
            particle.data.data.color.z = color_b * (float_t)(1.0 / 255.0);

            float_t color_a = particle.data.data.color.w * 255.0f;
            mu_label(muctx, "Color A:");
            mu_slider_fmt(muctx, &color_a, 0.0f, 255.0f, 1.0f, "%.0f");
            particle.data.data.color.w = color_a * (float_t)(1.0 / 255.0);

            float_t uv_index_type = (float_t)particle.data.data.uv_index_type;
            uv_index_type = clamp(uv_index_type, GLITTER_UV_INDEX_FIXED, GLITTER_UV_INDEX_USER);
            mu_label(muctx, "UV Index Type:");
            mu_slider_ex(muctx, &uv_index_type, GLITTER_UV_INDEX_FIXED, GLITTER_UV_INDEX_USER,
                1.0f, glitter_uv_index_type_name[particle.data.data.uv_index_type], MU_OPT_ALIGNCENTER);
            uv_index_type = clamp(uv_index_type, GLITTER_UV_INDEX_FIXED, GLITTER_UV_INDEX_USER);
            particle.data.data.uv_index_type = (int32_t)uv_index_type;

            mu_layout_row(muctx, 5, (int[]) { tw0, tw1, sw, tw2, sw }, 0);

            float_t split_u = (float_t)particle.data.data.split_u;
            mu_label(muctx, "UV Split");
            mu_label(muctx, "U:");
            mu_number_fmt(muctx, &split_u, 1.0f, "%.0f");
            particle.data.data.split_u = (uint8_t)(1 << (int32_t)log2f(clamp(split_u, 1.0f, 128.0f)));

            float_t split_v = (float_t)particle.data.data.split_v;
            mu_label(muctx, "V:");
            mu_number_fmt(muctx, &split_v, 1.0f, "%.0f");
            particle.data.data.split_v = (uint8_t)(1 << (int32_t)log2f(clamp(split_v, 1.0f, 128.0f)));

            particle.data.data.split_uv.x = 1.0f / (float_t)particle.data.data.split_u;
            particle.data.data.split_uv.y = 1.0f / (float_t)particle.data.data.split_v;

            int32_t uv_max_count = (int32_t)(particle.data.data.split_u * particle.data.data.split_v);
            if (uv_max_count)
                uv_max_count--;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t frame_step_uv = (float_t)(int16_t)particle.data.data.frame_step_uv;
            mu_label(muctx, "Frame Step UV:");
            mu_number_fmt(muctx, &frame_step_uv, 1.0f, "%.0f");
            particle.data.data.frame_step_uv = (float_t)(int16_t)clamp(frame_step_uv, 0.0f, 32767.0f);

            float_t uv_index_start = (float_t)particle.data.data.uv_index_start;
            mu_label(muctx, "UV Index Start:");
            mu_slider_ex(muctx, &uv_index_start, 0.0f, (float_t)uv_max_count, 1.0f, "%.0f", MU_OPT_ALIGNCENTER);
            particle.data.data.uv_index_start = (int32_t)uv_index_start;

            float_t uv_index_end = (float_t)particle.data.data.uv_index_end;
            mu_label(muctx, "UV Index End:");
            mu_slider_ex(muctx, &uv_index_end, uv_index_start, (float_t)uv_max_count, 1.0f, "%.0f", MU_OPT_ALIGNCENTER);
            particle.data.data.uv_index_end = (int32_t)uv_index_end;

            float_t uv_index = (float_t)particle.data.data.uv_index;
            mu_label(muctx, "UV Index:");
            mu_slider_ex(muctx, &uv_index, uv_index_start, uv_index_end, 1.0f, "%.0f", MU_OPT_ALIGNCENTER);
            particle.data.data.uv_index = (int32_t)uv_index;

            mu_layout_row(muctx, 5, (int[]) { tw0, tw1, sw, tw2, sw }, 0);

            vec2 uv_scroll_add = particle.data.data.uv_scroll_add;
            mu_label(muctx, "UV Scroll Add");
            mu_label(muctx, "U:");
            mu_number_fmt(muctx, &uv_scroll_add.x, 0.0001f, "%g");
            mu_label(muctx, "V:");
            mu_number_fmt(muctx, &uv_scroll_add.y, 0.0001f, "%g");
            particle.data.data.uv_scroll_add = uv_scroll_add;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t uv_scroll_add_scale = particle.data.data.uv_scroll_add_scale;
            mu_label(muctx, "UV Scroll Add Scale:");
            mu_number_fmt(muctx, &uv_scroll_add_scale, 0.0001f, "%g");
            particle.data.data.uv_scroll_add_scale = uv_scroll_add_scale;

            float_t blend_mode0 = (float_t)particle.data.data.blend_mode0;
            blend_mode0 = clamp(blend_mode0, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH);
            mu_label(muctx, "Blend Mode 0:");
            mu_slider_ex(muctx, &blend_mode0, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH,
                1.0f, glitter_particle_blend_name[particle.data.data.blend_mode0], MU_OPT_ALIGNCENTER);
            blend_mode0 = clamp(blend_mode0, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH);
            particle.data.data.blend_mode0 = (int32_t)blend_mode0;

            if (particle.data.data.flags & GLITTER_PARTICLE_FLAG_SECOND_TEXTURE) {
                float_t blend_mode1 = (float_t)particle.data.data.blend_mode1;
                blend_mode1 = clamp(blend_mode1, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH);
                mu_label(muctx, "Blend Mode 1:");
                mu_slider_ex(muctx, &blend_mode1, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH,
                    1.0f, glitter_particle_blend_name[particle.data.data.blend_mode1], MU_OPT_ALIGNCENTER);
                blend_mode1 = clamp(blend_mode1, GLITTER_PARTICLE_BLEND_ZERO, GLITTER_PARTICLE_BLEND_PUNCH_THROUGH);
                particle.data.data.blend_mode1 = (int32_t)blend_mode1;
            }

            switch (particle.data.data.uv_index_type) {
            case GLITTER_UV_INDEX_FIXED:
            case GLITTER_UV_INDEX_RANDOM:
            case GLITTER_UV_INDEX_FORWARD:
            case GLITTER_UV_INDEX_REVERSE:
            case GLITTER_UV_INDEX_USER:
                particle.data.data.uv_index &= uv_max_count;
                break;
            case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
            case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
            case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
                if (particle.data.data.uv_index < particle.data.data.uv_index_start)
                    particle.data.data.uv_index = particle.data.data.uv_index_start;
                else if (particle.data.data.uv_index_end >= 0
                    && particle.data.data.uv_index > particle.data.data.uv_index_end)
                    particle.data.data.uv_index = particle.data.data.uv_index_end;

                particle.data.data.uv_index = max(particle.data.data.uv_index, uv_max_count);
                break;
            }

            if (particle.data.data.uv_index_end >= 0)
                particle.data.data.uv_index_count = particle.data.data.uv_index_end - particle.data.data.uv_index_start + 1;
            else
                particle.data.data.uv_index_count = uv_max_count - particle.data.data.uv_index_start;

            if (particle.data.data.uv_index_count < 0)
                particle.data.data.uv_index_count = 0;

            mu_layout_row(muctx, 1, (int[]) { -1 }, 0);

            glitter_particle_flag sub_flags = particle.data.data.sub_flags;
            bool use_curve = sub_flags & GLITTER_PARTICLE_SUB_FLAG_USE_CURVE ? true : false;
            mu_checkbox(muctx, "Use Curve", &use_curve);
            if (use_curve)
                sub_flags |= GLITTER_PARTICLE_SUB_FLAG_USE_CURVE;
            else
                sub_flags &= ~GLITTER_PARTICLE_SUB_FLAG_USE_CURVE;
            particle.data.data.sub_flags = sub_flags;

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

            float_t count = (float_t)particle.data.data.count;
            mu_label(muctx, "Count:");
            mu_number_fmt(muctx, &count, 1.0f, "%.0f");
            particle.data.data.count = (int32_t)max(count, 0.0f);

            float_t locus_history_size = (float_t)particle.data.data.locus_history_size;
            mu_label(muctx, "Locus History Size:");
            mu_number_fmt(muctx, &locus_history_size, 1.0f, "%.0f");
            particle.data.data.locus_history_size = (int32_t)max(locus_history_size, 0.0f);

            float_t locus_history_size_random = (float_t)particle.data.data.locus_history_size_random;
            mu_label(muctx, "Locus History Size Random:");
            mu_number_fmt(muctx, &locus_history_size_random, 1.0f, "%.0f");
            particle.data.data.locus_history_size_random = (int32_t)max(locus_history_size_random, 0.0f);

            mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
            float_t emission = particle.data.data.emission;
            mu_label(muctx, "Emission:");
            mu_number_fmt(muctx, &emission, 0.0001f, "%g");
            particle.data.data.emission = max(emission, 0.0f);

            uint64_t hash1 = hash_fnv1a64((void*)selected_particle, sizeof(glitter_particle));
            uint64_t hash2 = hash_fnv1a64((void*)&particle, sizeof(glitter_particle));
            if (hash1 != hash2)
                glitter_editor.input_reload = true;
            *selected_particle = particle;
        }
        mu_pop_id(muctx);

        mu_push_id(muctx, &selected_curve, sizeof(glitter_curve*));
        if (selected_curve && mu_header_ex(muctx, "Curve", MU_OPT_EXPANDED)) {
            glitter_curve curve = *selected_curve;

            uint64_t hash1 = hash_fnv1a64((void*)selected_curve, sizeof(glitter_curve));
            uint64_t hash2 = hash_fnv1a64((void*)&curve, sizeof(glitter_curve));
            if (hash1 != hash2)
                glitter_editor.input_reload = true;
            *selected_curve = curve;
        }
        mu_pop_id(muctx);

    editor_right_window_end:
        mu_end_window(muctx);
    }

    x = min(width / 3, 360);
    y = height - min(height / 3, 240);
    w = width - min(width / 3, 360) * 2;
    h = min(height / 3, 240);

    if (mu_begin_window_ex(muctx, "Glitter Editor Bottom Window",
        mu_rect(x, y, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NOCLOSE | MU_OPT_NOTITLE, true, true, false)) {
        if (!glitter_editor.effect_group)
            goto editor_bottom_window_end;

    editor_bottom_window_end:
        mu_end_window(muctx);
    }
}

void glitter_editor_render() {
    if (glitter_editor.dispose) {
        glitter_editor_dispose();
        return;
    }
    else if (!glitter_editor.enabled)
        return;

    if (glitter_editor.load || glitter_editor.save) {
        wchar_t* temp = path_wget_without_extension(glitter_editor.file);
        wchar_t* file = wcsrchr(temp, L'\\');
        wchar_t* path;
        if (file) {
            path = path_wget_without_extension(glitter_editor.file);
            wcsrchr(path, L'\\')[1] = 0;
            *file++ = 0;
        }
        else {
            path = 0;
            file = temp;
        }

        if (glitter_editor.load)
            glitter_editor_load_file(path, file);
        else if (glitter_editor.save && glitter_editor.effect_group)
            glitter_editor_save_file(path, file);

        glitter_editor.load = false;
        glitter_editor.save = false;
        glitter_editor.save_big_endian = false;
        file = 0;
        free(path);
        free(temp);
        return;
    }

    glitter_particle_manager_get_start_end_frame(gpm, &glitter_editor.start_frame, &glitter_editor.end_frame);

    if (!glitter_editor.effect_group)
        return;

    if (!glitter_editor.input_pause && glitter_editor.old_frame_counter == glitter_editor.frame_counter) {
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
        if (gpm->scenes.end != gpm->scenes.begin)
            glitter_editor.frame_counter += get_frame_speed();
    }

    if (glitter_editor.old_frame_counter != glitter_editor.frame_counter) {
        if (glitter_editor.end_frame > 0.0f)
            while (glitter_editor.frame_counter >= glitter_editor.end_frame)
                glitter_editor.frame_counter -= glitter_editor.end_frame;

        if (glitter_editor.frame_counter < glitter_editor.start_frame)
            glitter_editor.frame_counter = glitter_editor.start_frame;

        glitter_particle_manager_set_frame(gpm, glitter_editor.effect_group, &glitter_editor.scene,
            glitter_editor.frame_counter, glitter_editor.old_frame_counter,
            glitter_editor.random, glitter_editor.counter, false);
        glitter_editor.old_frame_counter = glitter_editor.frame_counter;
    }

    if (glitter_editor.input_reload) {
        vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
        glitter_editor.scene = glitter_scene_init(glitter_editor.effect_group);
        vector_ptr_glitter_scene_push_back(&gpm->scenes, &glitter_editor.scene);
        gpm->random = glitter_editor.random;
        gpm->counter = glitter_editor.counter;
        glitter_particle_manager_set_frame(gpm, glitter_editor.effect_group, &glitter_editor.scene,
            glitter_editor.frame_counter, 0.0f, glitter_editor.random, glitter_editor.counter, true);
        glitter_editor.input_reload = false;
    }
    else if (glitter_editor.input_play)
        glitter_editor.input_pause = false;
    else if (glitter_editor.input_stop) {
        gpm->random = glitter_editor.random;
        gpm->counter = glitter_editor.counter;
        glitter_particle_manager_set_frame(gpm, glitter_editor.effect_group, &glitter_editor.scene,
            0.0f, 0.0f, glitter_editor.random, glitter_editor.counter, true);
        glitter_editor.frame_counter = 0.0f;
        glitter_editor.old_frame_counter = 0.0f;
        glitter_editor.input_pause = true;
    }
    glitter_editor.input_play = false;
    glitter_editor.input_stop = false;
}

void glitter_editor_sound() {
    if (!glitter_editor.enabled)
        return;
}

static void glitter_editor_load() {
    int msgboxID = MessageBoxW(0, L"Load as F2nd Glitter file?", L"Glitter Load", MB_YESNO);
    gpm->f2 = msgboxID == IDYES ? true : false;
    glitter_editor.load = true;
}

static void glitter_editor_save() {
    if (gpm->f2) {
        int msgboxID = MessageBoxW(0, L"Save as F2nd Glitter file for PS3?", L"Glitter Save", MB_YESNO);
        glitter_editor.save_big_endian = msgboxID == IDYES ? true : false;
    }
    else
        glitter_editor.save_big_endian = false;
    glitter_editor.save = true;
}

static void glitter_editor_open_window() {
    if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        wchar_t file[MAX_PATH];
        OPENFILENAME ofn;
        memset(&file, 0, sizeof(file));
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
        ofn.lpstrFile = file;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = L"File to Open";
        if (GetOpenFileNameW(&ofn)) {
            memcpy(glitter_editor.file, file, sizeof(file));
            glitter_editor_load();
        }
        CoUninitialize();
    }
}

static void glitter_editor_save_window() {
    if (glitter_editor.file[0])
        glitter_editor_save();
    else
        glitter_editor_save_as_window();
}

static void glitter_editor_save_as_window() {
    if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        wchar_t file[MAX_PATH];
        OPENFILENAME ofn;
        memset(&file, 0, sizeof(file));
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"Glitter Farc Files (*.farc)\0*.farc\0\0";
        ofn.lpstrFile = file;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = L"File to Save";
        ofn.Flags = OFN_NONETWORKBUTTON;
        if (GetSaveFileNameW(&ofn)) {
            memcpy(glitter_editor.file, file, sizeof(file));
            glitter_editor_save();
        }
        CoUninitialize();
    }
}

static void glitter_editor_load_file(wchar_t* path, wchar_t* file) {
    vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)glitter_effect_group_dispose);

    glitter_editor.effect_group = 0;
    glitter_editor.scene = 0;

    glitter_file_reader* fr = glitter_file_reader_init(path, file, gpm->f2, -1.0f);
    if (glitter_file_reader_read(fr)) {

        if (!gpm->f2) {
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
                MessageBoxW(0, L"There was an error while parsing Glitter file.\n"
                    L"Invalid Glitter List File", L"Open Error", MB_ICONERROR);
            }
            else
                goto add_effect_group;
        }
        else if (!glitter_editor_list_open_window(fr->effect_group)) {
            glitter_effect_group_dispose(fr->effect_group);
            MessageBoxW(0, L"There was an error while parsing Glitter file.\n"
                L"Invalid Glitter List File", L"Open Error", MB_ICONERROR);
        }
        else {
        add_effect_group:
            glitter_editor.random = gpm->random;
            glitter_editor.counter = gpm->counter;
            glitter_editor.effect_group = fr->effect_group;
            glitter_editor.scene = glitter_scene_init(fr->effect_group);

            glitter_effect_group* eg = glitter_editor.effect_group;
            glitter_effect** i;
            size_t id = 1;
            for (i = eg->effects.begin, id = 1; i != eg->effects.end; i++, id++) {
                if (!*i)
                    continue;

                glitter_scene_init_effect(glitter_editor.scene, *i, id, false);
            }

            vector_ptr_glitter_effect_group_push_back(&gpm->effect_groups, &glitter_editor.effect_group);
            vector_ptr_glitter_scene_push_back(&gpm->scenes, &glitter_editor.scene);
            glitter_editor.frame_counter = 0.0f;
            glitter_editor.old_frame_counter = 0.0f;
            glitter_editor.input_pause = true;
        }
    }
    else if (fr->effect_group) {
        glitter_effect_group_dispose(fr->effect_group);
        MessageBoxW(0, L"There was an error while parsing Glitter file.\n"
            L"Invalid Glitter FARC", L"Open Error", MB_ICONERROR);
    }

    glitter_file_reader_dispose(fr);
}

static void glitter_editor_save_file(wchar_t* path, wchar_t* file) {
    f2_struct* st;
    farc_file ff_drs;
    farc_file ff_dve;
    farc_file ff_lst;
    bool use_big_endian;
    
    use_big_endian = gpm->f2 && glitter_editor.save_big_endian;

    memset(&ff_drs, 0, sizeof(farc_file));
    memset(&ff_dve, 0, sizeof(farc_file));
    memset(&ff_lst, 0, sizeof(farc_file));

    farc* f = farc_init();
    st = f2_struct_init();
    if (glitter_diva_effect_unparse_file(glitter_editor.effect_group, st, use_big_endian)) {
        f2_struct_write_memory(st, &ff_dve.data, &ff_dve.size_uncompressed, true, false);
        ff_dve.name = path_wadd_extension(file, L".dve");
    }
    else
        goto End;
    f2_struct_dispose(st);

    st = f2_struct_init();
    if (glitter_diva_resource_unparse_file(glitter_editor.effect_group, st, use_big_endian)) {
        f2_struct_write_memory(st, &ff_drs.data, &ff_drs.size_uncompressed, true, false);
        ff_drs.name = path_wadd_extension(file, L".drs");
    }
    else {
        free(ff_dve.data);
        free(ff_dve.name);
        goto End;
    }
    f2_struct_dispose(st);

    if (!gpm->f2) {
        st = f2_struct_init();
        if (glitter_diva_list_unparse_file(glitter_editor.effect_group, st, use_big_endian)) {
            f2_struct_write_memory(st, &ff_lst.data, &ff_lst.size_uncompressed, true, false);
            ff_lst.name = path_wadd_extension(file, L".lst");
        }
        else {
            free(ff_dve.data);
            free(ff_dve.name);
            free(ff_drs.data);
            free(ff_drs.name);
            goto End;
        }
        f2_struct_dispose(st);
    }

    vector_farc_file_push_back(&f->files, &ff_drs);
    vector_farc_file_push_back(&f->files, &ff_dve);
    if (!gpm->f2)
        vector_farc_file_push_back(&f->files, &ff_lst);

    farc_compress_mode mode;
    if (gpm->f2 && use_big_endian)
        mode = FARC_COMPRESS_FARC_GZIP_AES;
    else
        mode = FARC_COMPRESS_FArc;

    wchar_t* temp = path_wadd_extension(path, file);
    if (gpm->f2) {
        wchar_t* list_temp = path_wadd_extension(temp, L".glitter.txt");
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

    farc_dispose(f);
    return;
End:
    f2_struct_dispose(st);
    farc_dispose(f);
}

static bool glitter_editor_list_open_window(glitter_effect_group* eg) {
    bool ret = false;
    if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        wchar_t file[MAX_PATH];
        OPENFILENAME ofn;
        memset(&file, 0, sizeof(file));
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"Glitter UTF-8 List Files (*.glitter.txt)\0*.glitter.txt\0\0";
        ofn.lpstrFile = file;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = L"File to Open";
        ofn.Flags = OFN_NONETWORKBUTTON;
        if (GetOpenFileNameW(&ofn)) {
            stream* s = io_wopen(file, L"rb");
            size_t length = s->length;
            char* data = force_malloc(length);
            io_read(s, data, length);
            io_dispose(s);

            char** lines;
            size_t count;
            if (glitter_editor_list_parse(data, length, &lines, &count)) {
                uint64_t* hashes = force_malloc_s(sizeof(uint64_t), count);
                if (gpm->f2)
                    for (size_t i = 0; i < count; i++)
                        hashes[i] = hash_murmurhash(lines[i], min(strlen(lines[i]), 0x7F), 0, false, false);
                else
                    for (size_t i = 0; i < count; i++)
                        hashes[i] = hash_fnv1a64(lines[i], min(strlen(lines[i]), 0x7F));

                ret = true;
                for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
                    if (!*i)
                        continue;

                    size_t j;
                    glitter_effect* e = *i;
                    for (j = 0; j < count; j++)
                        if (e->data.name_hash == hashes[j])
                            break;

                    if (j == count) {
                        ret = false;
                        break;
                    }

                    memset(e->name, 0, 0x80);
                    memcpy(e->name, lines[j], min(strlen(lines[j]), 0x7F));
                }

                for (size_t i = 0; i < count; i++)
                    free(lines[i]);
                free(lines);
                free(hashes);
            }
            free(data);
        }
        CoUninitialize();
    }
    return ret;
}

static bool glitter_editor_list_parse(char* data, size_t length, char*** lines, size_t* count) {
    if (!data || !length || !lines || !count)
        return false;

    size_t c;
    *lines = 0;
    *count = 0;
    if (data[0] == 0xFF || data[0] == 0xFE || data[0] == 0x00)
        return false;
    else if (data[0] == 0xEF) {
        if (length == 1 || data[1] != 0xBB || length == 2 || data[2] != 0xBF || length == 3)
            return false;

        data += 3;
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
            ch = data[i];
            while ((ch == '\r' || ch == '\n') && ++i < length) {
                ch = data[i];
                lf = true;
            }

            if (lf && i < length) {
                c++;
                lf = false;
            }
        }

        lf = false;
        size_t* line_lengths = force_malloc_s(sizeof(size_t), c);
        size_t* line_offsets = force_malloc_s(sizeof(size_t), c);
        line_lengths[0] = 0;
        line_offsets[0] = 0;
        for (size_t i = 0, j = 0, l = 0; i < length && j < c; ) {
            ch = data[i];
            while ((ch == '\r' || ch == '\n') && ++i < length) {
                ch = data[i];
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

        char** temp_lines = force_malloc_s(sizeof(char*), c);
        for (size_t i = 0, l = 0; i < c; i++) {
            l = line_lengths[i];
            char* temp = force_malloc(l + 1);
            memcpy(temp, data + line_offsets[i], l);
            temp[l] = 0;
            temp_lines[i] = temp;
        }
        *lines = temp_lines;
        *count = c;
        free(line_offsets);
        free(line_lengths);
    }
    return true;
}

static void glitter_editor_mui_show_animation(vector_ptr_glitter_curve* curve) {
    if (!mu_begin_treenode(muctx, "Animation"))
        return;

    if (!selected_animation)
        selected_animation = curve;

    char buf[0x80];
    size_t i_idx = 1;
    for (glitter_curve** i = curve->begin; i != curve->end; i++, i_idx++) {
        if (!*i)
            continue;

        glitter_curve* curve = *i;
        mu_push_id(muctx, &i_idx, sizeof(i_idx));
        switch (curve->type) {
        case GLITTER_CURVE_TRANSLATION_X:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Translation X");
            break;
        case GLITTER_CURVE_TRANSLATION_Y:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Translation Y");
            break;
        case GLITTER_CURVE_TRANSLATION_Z:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Translation Z");
            break;
        case GLITTER_CURVE_ROTATION_X:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Rotation X");
            break;
        case GLITTER_CURVE_ROTATION_Y:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Rotation Y");
            break;
        case GLITTER_CURVE_ROTATION_Z:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Rotation Z");
            break;
        case GLITTER_CURVE_SCALE_X:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Scale X");
            break;
        case GLITTER_CURVE_SCALE_Y:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Scale Y");
            break;
        case GLITTER_CURVE_SCALE_Z:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Scale Z");
            break;
        case GLITTER_CURVE_SCALE_ALL:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Scale All");
            break;
        case GLITTER_CURVE_COLOR_R:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Color Red");
            break;
        case GLITTER_CURVE_COLOR_G:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Color Green");
            break;
        case GLITTER_CURVE_COLOR_B:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Color Blue");
            break;
        case GLITTER_CURVE_COLOR_A:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Color Alpha");
            break;
        case GLITTER_CURVE_EMISSION_INTERVAL:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Emission Interval");
            break;
        case GLITTER_CURVE_PARTICLES_PER_EMISSION:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "Particles Per Emission");
            break;
        case GLITTER_CURVE_U_SCROLL:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "U Scroll");
            break;
        case GLITTER_CURVE_V_SCROLL:
            sprintf_s(buf, sizeof(buf), "Curve: %s", "V Scroll");
            break;
        default:
            sprintf_s(buf, sizeof(buf), "Curve: Unknown ID %d", curve->type);
            break;
        }

        if (mu_begin_treenode(muctx, buf)) {
            if (!selected_curve)
                selected_curve = curve;
            mu_end_treenode(muctx);
        }
        mu_pop_id(muctx);
    }
    mu_end_treenode(muctx);
}
