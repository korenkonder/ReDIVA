/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_render_group* FASTCALL glitter_render_group_init(int32_t count, glitter_particle_inst* a2);
extern void FASTCALL glitter_render_group_dispose(glitter_render_group* rg);

extern void FASTCALL Glitter__RenderGroup__LocusHistoryAppend(glitter_locus_history* a1,
    glitter_render_group_sub* a2, glitter_particle_inst* a3);
extern glitter_render_group_sub* FASTCALL Glitter__RenderGroup__AddControl(glitter_render_group* a1,
    glitter_render_group_sub* a2);
extern void FASTCALL Glitter__RenderGroup__Free(glitter_render_group* a1);
extern void FASTCALL Glitter__RenderGroup__Copy(glitter_render_group* a1, glitter_render_group* a2);
extern void FASTCALL Glitter__RenderGroup__Emit(glitter_render_group* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t a4, int32_t count);
extern bool FASTCALL Glitter__RenderGroup__GetA3DAScale(glitter_render_group* a1, vec3* a2);
extern void FASTCALL Glitter__RenderGroup__sub_1403A6CE0(glitter_render_group_sub* a1,
    glitter_emitter_inst* a2, glitter_particle_inst_data* a3, int32_t index);
extern void FASTCALL Glitter__RenderGroup__EmitInit(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, glitter_emitter_inst* a3, glitter_particle_inst_data* a4, int32_t a5);
extern void FASTCALL Glitter__RenderGroup__ClampColor(glitter_particle_inst* a1,
    float_t* r, float_t* g, float_t* b, float_t* a);
extern void FASTCALL Glitter__RenderGroup__GetColor(glitter_particle_inst* a1, glitter_render_group_sub* a2);
extern void FASTCALL Glitter__RenderGroup__Accelerate(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t rebound_time, float_t delta_frame);
extern void FASTCALL Glitter__RenderGroup__CopyFromParticle(glitter_render_group* render_group,
    float_t delta_frame, bool a3);
extern void FASTCALL Glitter__RenderGroup__GetValue(glitter_render_group* a1,
    glitter_render_group_sub* a2, float_t delta_frame);
extern void FASTCALL Glitter__RenderGroup__StepUV(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t delta_frame);
extern void FASTCALL Glitter__RenderGroup__DeleteBuffers(glitter_render_group* a1, bool a2);
extern void FASTCALL Glitter__RenderGroup__DrawQuad__RotateToAxis1(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3);
extern void FASTCALL Glitter__RenderGroup__DrawQuad__RotateToAxis2(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3);
extern void FASTCALL Glitter__RenderGroup__Draw__SetPivot(glitter_pivot a1,
    float_t a2, float_t a3, float_t* a4, float_t* a5, float_t* a6, float_t* a7);
extern void FASTCALL Glitter__RenderGroup__Draw(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL Glitter__RenderGroup__DrawLine(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL Glitter__RenderGroup__DrawLocus(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL Glitter__RenderGroup__DrawQuad(glitter_scene_sub* a1, glitter_render_group* a2);
extern void FASTCALL Glitter__RenderGroup__DrawQuadSub1(glitter_scene_sub* a1,
    glitter_render_group* a2, mat4* a3, mat4* a4);
extern void FASTCALL Glitter__RenderGroup__DrawQuadSub2(glitter_scene_sub* a1,
    glitter_render_group* a2, mat4* a3, mat4* a4,
    void(FASTCALL* a5)(mat4*, glitter_render_group*, glitter_render_group_sub*));
extern void FASTCALL Glitter__RenderGroup__DrawQuads(glitter_scene_sub* a1, int32_t count);
