/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect_group.h"
#include "parse_file.h"
#include "particle_manager.h"
#include "scene.h"

bool FASTCALL Glitter__DivaEffect__ParseFile(glitter_file_reader* a1, f2_header* header) {
    glitter_effect_group* effect_group;

    if (!header->data_size || Glitter__ParseFile__ReverseSignatureEndianess(header) != 'DVEF')
        return false;

    a1->version = Glitter__ParseFile__GetVersion(header);
    effect_group = glitter_effect_group_init();
    effect_group->hash = a1->hash;
    if (!Glitter__EffectGroup__ParseFile(a1,
        Glitter__ParseFile__GetSubStructPointer(header), effect_group)) {
        glitter_effect_group_dispose(effect_group);
        return false;
    }
    a1->effect_group = effect_group;
    return true;
}
