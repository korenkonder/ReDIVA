/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect_group.h"

bool FASTCALL glitter_diva_effect_parse_file(GPM, glitter_file_reader* fr, f2_struct* st) {
    glitter_effect_group* effect_group;

    if (!st || !st->header.data_size)
        return false;

    effect_group = glitter_effect_group_init(GPM_VAL);
    effect_group->hash = fr->hash;
    effect_group->emission = fr->emission;
    if (fr->emission <= 0.0f)
        effect_group->emission = gpm->emission;
    effect_group->version = st->header.version;
    if (!glitter_effect_group_parse_file(GPM_VAL, effect_group, st)) {
        glitter_effect_group_dispose(effect_group);
        return false;
    }
    fr->effect_group = effect_group;
    return true;
}

bool FASTCALL glitter_diva_effect_unparse_file(GPM, glitter_effect_group* a1, f2_struct* st) {
    return glitter_effect_group_unparse_file(GPM_VAL, a1, st);
}

