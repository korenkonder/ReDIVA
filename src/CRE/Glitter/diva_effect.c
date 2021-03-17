/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect_group.h"
#include "parse_file.h"
#include "particle_manager.h"
#include "scene.h"

bool FASTCALL glitter_diva_effect_parse_file(glitter_file_reader* a1, f2_header* header) {
    glitter_effect_group* effect_group;

    if (!header->data_size || glitter_parse_file_reverse_signature_endianess(header) != 'DVEF')
        return false;

    a1->version = header->version;
    effect_group = glitter_effect_group_init();
    effect_group->hash = a1->hash;
    if (!glitter_effect_group_parse_file(a1,
        glitter_parse_file_get_sub_struct_ptr(header), effect_group)) {
        glitter_effect_group_dispose(effect_group);
        return false;
    }
    a1->effect_group = effect_group;
    return true;
}
