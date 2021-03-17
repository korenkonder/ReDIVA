/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_render_group* FASTCALL glitter_render_group_init(int32_t count, glitter_particle_inst* a2);
extern glitter_render_group_sub* FASTCALL glitter_render_group_add_control(glitter_render_group* a1,
    glitter_render_group_sub* a2);
extern void FASTCALL glitter_render_group_copy(glitter_render_group* a1, glitter_render_group* a2);
extern void FASTCALL glitter_render_group_copy_from_particle(glitter_render_group* render_group,
    float_t delta_frame, bool copy_mats);
extern void FASTCALL glitter_render_group_delete_buffers(glitter_render_group* a1, bool a2);
extern void FASTCALL glitter_render_group_draw(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL glitter_render_group_draw_line(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL glitter_render_group_draw_locus(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL glitter_render_group_draw_quad(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL glitter_render_group_draw_quad_rotate_to_prev_position(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3);
extern void FASTCALL glitter_render_group_draw_quad_rotate_to_position(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3);
extern void FASTCALL glitter_render_group_draw_quad_sub1(glitter_scene_sub* a1,
    glitter_render_group* a2, mat4* a3, mat4* a4);
extern void FASTCALL glitter_render_group_draw_quad_sub2(glitter_scene_sub* a1,
    glitter_render_group* a2, mat4* a3, mat4* a4,
    void(FASTCALL* a5)(mat4*, glitter_render_group*, glitter_render_group_sub*));
extern void FASTCALL glitter_render_group_draw_quads(glitter_scene_sub* a1, int32_t count);
extern void FASTCALL glitter_render_group_draw_set_pivot(glitter_pivot pivot,
    float_t a2, float_t a3, float_t* v00, float_t* v01, float_t* v10, float_t* v11);
extern void FASTCALL glitter_render_group_emit(glitter_render_group* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t a4, int32_t count);
extern void FASTCALL glitter_render_group_free(glitter_render_group* a1);
extern bool FASTCALL glitter_render_group_get_a3da_scale(glitter_render_group* a1, vec3* a2);
extern void FASTCALL glitter_render_group_sub_accelerate(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t rebound_time, float_t delta_frame);
extern void FASTCALL glitter_render_group_sub_clamp_color(glitter_particle_inst* a1,
    float_t* r, float_t* g, float_t* b, float_t* a);
extern void FASTCALL glitter_render_group_sub_emit(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, glitter_emitter_inst* a3, glitter_particle_inst_data* a4, int32_t a5);
extern void FASTCALL glitter_render_group_sub_get_color(glitter_particle_inst* a1, glitter_render_group_sub* a2);
extern void FASTCALL glitter_render_group_sub_get_value(glitter_render_group* a1,
    glitter_render_group_sub* a2, float_t delta_frame);
extern void FASTCALL glitter_render_group_sub_step_uv(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t delta_frame);
extern void FASTCALL glitter_render_group_dispose(glitter_render_group* rg);
