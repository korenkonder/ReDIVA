/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect.h"
#include "effect_group.h"
#include "scene.h"

bool glitter_diva_effect_parse_file(GPM, glitter_file_reader* fr, f2_struct* st, float_t emission) {
    if (!st || !st->header.data_size)
        return false;

    glitter_effect_group* effect_group = new glitter_effect_group(fr->type);
    effect_group->hash = fr->hash;
    effect_group->emission = fr->emission;
    if (fr->emission <= 0.0f)
        effect_group->emission = emission;
    effect_group->version = st->header.version;
    effect_group->type = fr->type;

    effect_group->scene = 0;
    if (!glitter_effect_group_parse_file(effect_group, st)) {
        effect_group->field_3C = true;
        if (GPM_VAL->AppendEffectGroup(fr->hash, effect_group, fr))
            return true;
    }
    else if (GPM_VAL->AppendEffectGroup(fr->hash, effect_group, fr)) {
        fr->effect_group = effect_group;
        if (!fr->init_scene)
            return true;

        int32_t id = 1;
        for (glitter_effect** i = effect_group->effects.begin; i != effect_group->effects.end; i++, id++) {
            if ((*i)->data.start_time <= 0.0f)
                continue;

            if (!effect_group->scene)
                effect_group->scene = new glitter_scene(0, fr->type == GLITTER_FT
                    ? hash_fnv1a64m_empty : hash_murmurhash_empty, effect_group, true);

            glitter_scene* scene = effect_group->scene;
            if (scene)
                glitter_scene_init_effect(GPM_VAL, scene, *i, id, true);
        }
        return true;
    }

    delete effect_group;
    return false;
}

bool glitter_diva_effect_unparse_file(GLT, glitter_effect_group* a1, f2_struct* st) {
    return glitter_effect_group_unparse_file(GLT_VAL, a1, st);
}

