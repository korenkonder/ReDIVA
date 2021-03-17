/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_effect_inst* FASTCALL glitter_effect_inst_init(glitter_effect* a1,
    glitter_scene* a2, int32_t id, bool a4);
extern void FASTCALL glitter_effect_inst_dispose(glitter_effect_inst* ei);

extern void FASTCALL Glitter__EffectInst__GetValue(glitter_effect_inst* a1, float_t frame, int32_t efct_val);
extern void FASTCALL Glitter__EffectInst__Emit(glitter_effect_inst* a1,
    glitter_scene* a2, float_t delta_frame);
extern void FASTCALL Glitter__EffectInst__EmitInit(glitter_effect_inst* a1,
    glitter_scene* a2, float_t delta_frame);
extern void FASTCALL Glitter__EffectInst__UpdateValueFrame(glitter_effect_inst* a1, float_t delta_frame);
extern void FASTCALL Glitter__EffectInst__UpdateValueInit(glitter_effect_inst* a1, float_t delta_frame);
extern void FASTCALL Glitter__EffectInst__Copy(glitter_effect_inst* a1, glitter_effect_inst* a2, glitter_scene* a3);
extern void FASTCALL Glitter__EffectInst__UpdateMat(glitter_effect_inst* a1);
extern bool FASTCALL Glitter__EffectInst__GetA3DAMat(glitter_effect_inst* effect, mat4* a2);
extern int32_t FASTCALL Glitter__EffectInst__GetAlpha(glitter_effect_inst* a1);
extern int32_t FASTCALL Glitter__EffectInst__GetFog(glitter_effect_inst* a1);
extern bool FASTCALL Glitter__EffectInst__HasEnded(glitter_effect_inst* effect, bool a2);
extern void FASTCALL Glitter__EffectInst__SetA3DAScale(glitter_effect_inst* a1, vec3* a2);
extern void FASTCALL Glitter__EffectInst__Free(glitter_effect_inst* a1, glitter_scene* a2, bool a3);
extern void FASTCALL Glitter__EffectInst__GetA3DA(glitter_effect_inst* a1);
extern void FASTCALL Glitter__EffectInst__ResetSub(glitter_effect_inst* a1, glitter_scene* a2);
extern void FASTCALL Glitter__EffectInst__Reset(glitter_effect_inst* a1, glitter_scene* a2);
