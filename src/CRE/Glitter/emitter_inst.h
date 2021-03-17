/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(glitter_emitter* a1,
    glitter_scene* a2, glitter_effect_inst* a3);
extern void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei);

extern void FASTCALL Glitter__EmitterInst__GetValue(glitter_emitter_inst* a1,
    float_t frame, int32_t efct_val);
extern void FASTCALL Glitter__EmitterInst__EmitStep(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame);
extern void FASTCALL Glitter__EmitterInst__EmitInit(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame);
extern void FASTCALL Glitter__EmitterInst__RenderGroupInit(glitter_emitter_inst* emitter, float_t delta_frame);
extern void FASTCALL Glitter__EmitterInst__UpdateValueFrame(glitter_emitter_inst* a1,
    glitter_effect_inst* a2, float_t delta_frame);
extern void FASTCALL Glitter__EmitterInst__UpdateValueInit(glitter_emitter_inst* a1,
    float_t frame, float_t delta_frame);
extern void FASTCALL Glitter__EmitterInst__Copy(glitter_emitter_inst* a1,
    glitter_emitter_inst* a2, glitter_scene* a3);
extern void FASTCALL Glitter__EmitterInst__Emit(glitter_emitter_inst* a1, glitter_scene* a2);
extern void FASTCALL Glitter__EmitterInst__UpdateMat(glitter_emitter_inst* a1, glitter_effect_inst* a2);
extern bool FASTCALL Glitter__EmitterInst__HasEnded(glitter_emitter_inst* emitter, bool a2);
extern void FASTCALL Glitter__EmitterInst__Reset(glitter_emitter_inst* a1);
extern void FASTCALL Glitter__EmitterInst__Free(glitter_emitter_inst* a1, glitter_scene* a2, bool free);
extern void FASTCALL Glitter__EmitterInst__EffectVal__GetFloatType(glitter_emitter_inst* a1,
    int32_t a2, vec3* scale, vec3* a4, vec3* direction);
