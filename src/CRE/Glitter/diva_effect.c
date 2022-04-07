/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_effect.h"
#include "effect.h"
#include "effect_group.h"
#include "scene.h"

bool glitter_diva_effect_parse_file(GPM,
    GlitterFileReader* fr, f2_struct* st, object_database* obj_db) {
    if (!st || !st->header.data_size)
        return false;

    GlitterEffectGroup* effect_group = new GlitterEffectGroup(fr->type);
    effect_group->hash = fr->hash;
    effect_group->version = st->header.version;
    effect_group->type = fr->type;

    effect_group->scene = 0;
    if (!effect_group->ParseFile(st, obj_db)) {
        effect_group->not_loaded = true;
        if (GPM_VAL->AppendEffectGroup(fr->hash, effect_group, fr))
            return true;
    }
    else if (GPM_VAL->AppendEffectGroup(fr->hash, effect_group, fr)) {
        fr->effect_group = effect_group;
        if (!fr->init_scene)
            return true;

        int32_t id = 1;
        for (glitter_effect*& i : effect_group->effects) {
            if (i->data.start_time <= 0.0f) {
                id++;
                continue;
            }

            if (!effect_group->scene)
                effect_group->scene = new GlitterScene(0, fr->type == GLITTER_FT
                    ? hash_fnv1a64m_empty : hash_murmurhash_empty, effect_group, true);

            GlitterScene* scene = effect_group->scene;
            if (scene)
                scene->InitEffect(GPM_VAL, i, id, true);
            id++;
        }
        return true;
    }

    delete effect_group;
    return false;
}

bool glitter_diva_effect_unparse_file(GLT, GlitterEffectGroup* a1, f2_struct* st) {
    return a1->UnparseFile(GLT_VAL, st);
}

