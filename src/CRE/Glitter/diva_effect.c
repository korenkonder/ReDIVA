/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect_group.h"

bool glitter_diva_effect_parse_file(glitter_file_reader* fr, f2_struct* st, float_t emission) {
    glitter_effect_group* effect_group;

    if (!st || !st->header.data_size)
        return false;

    effect_group = glitter_effect_group_init(fr->type);
    effect_group->hash = fr->hash;
    effect_group->emission = fr->emission;
    if (fr->emission <= 0.0f)
        effect_group->emission = emission;
    effect_group->version = st->header.version;
    effect_group->type = fr->type;
    if (!glitter_effect_group_parse_file(effect_group, st)) {
        glitter_effect_group_dispose(effect_group);
        return false;
    }
    fr->effect_group = effect_group;
    return true;
}

bool glitter_diva_effect_unparse_file(GLT, glitter_effect_group* a1, f2_struct* st) {
    return glitter_effect_group_unparse_file(GLT_VAL, a1, st);
}

