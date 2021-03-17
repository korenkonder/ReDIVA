/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect_group.h"

extern glitter_particle_manager* gpm;

bool FASTCALL glitter_diva_effect_parse_file(glitter_file_reader* fr, f2_struct* st) {
    glitter_effect_group* effect_group;

    if (!st || !st->header.data_size)
        return false;

    effect_group = glitter_effect_group_init();
    effect_group->hash = fr->hash;
    effect_group->emission = fr->emission;
    if (fr->emission <= 0.0)
        effect_group->emission = gpm->emission;
    effect_group->version = st->header.version;
    if (!glitter_effect_group_parse_file(effect_group, st)) {
        glitter_effect_group_dispose(effect_group);
        return false;
    }
    fr->effect_group = effect_group;
    return true;
}

bool FASTCALL glitter_diva_effect_unparse_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian) {
    return glitter_effect_group_unparse_file(a1, st, use_big_endian);
}

