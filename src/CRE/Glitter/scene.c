/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "scene.h"
#include "effect_inst.h"
#include "../GlitterX/effect_inst_x.h"

glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg) {
    glitter_scene* s = force_malloc(sizeof(glitter_scene));
    s->emission = 1.0f;
    s->effect_group = eg;
    s->hash = eg->hash;
    if (eg) {
        vector_ptr_glitter_effect_inst_append(&s->effects, eg->effects.end - eg->effects.begin);
        s->emission = eg->emission;
    }
    return s;
}

bool FASTCALL glitter_scene_has_ended(glitter_scene* scene, bool a2) {
    glitter_effect_inst** i;

    for (i = scene->effects.begin; i != scene->effects.end; i++)
        if (*i && !glitter_effect_inst_has_ended(*i, a2))
            return false;
    return true;
}

void FASTCALL glitter_scene_init_effect(GPM, glitter_scene* a1,
    glitter_effect* a2, size_t id, bool appear_now) {
    glitter_effect_inst** i;
    glitter_effect_inst* effect;

    if (!a2)
        return;

    /*if (glt_type == GLITTER_X) {
        for (i = a1->effects.begin; i != a1->effects.end; i++)
            if (*i && (*i)->id == id) {
                glitter_x_effect_inst_reset(GPM_VAL, *i, a1->emission);
                return;
            }

        effect = glitter_x_effect_inst_init(GPM_VAL, a2, id, a1->emission, appear_now);
        vector_ptr_glitter_effect_inst_push_back(&a1->effects, &effect);
    }
    else*/ {
        for (i = a1->effects.begin; i != a1->effects.end; i++)
            if (*i && (*i)->id == id) {
                glitter_effect_inst_reset(GPM_VAL, *i, a1->emission);
                return;
            }

        effect = glitter_effect_inst_init(GPM_VAL, a2, id, a1->emission, appear_now);
        vector_ptr_glitter_effect_inst_push_back(&a1->effects, &effect);
    }
}

void FASTCALL glitter_scene_update_value_frame(GPM, glitter_scene* a1, float_t delta_frame) {
    glitter_effect_inst** i;
    glitter_effect_inst* effect;

    /*if (glt_type == GLITTER_X)
        for (i = a1->effects.begin; i != a1->effects.end; ++i) {
            if (!*i)
                continue;

            effect = *i;
            glitter_x_effect_inst_update_value_frame(GPM_VAL, effect, delta_frame, a1->emission);
        }
    else*/
        for (i = a1->effects.begin; i != a1->effects.end; ++i) {
            if (!*i)
                continue;

            effect = *i;
            glitter_effect_inst_update_value_frame(GPM_VAL, effect, delta_frame, a1->emission);
        }
}

void FASTCALL glitter_scene_dispose(glitter_scene* s) {
    vector_ptr_glitter_effect_inst_free(&s->effects, glitter_effect_inst_dispose);
    free(s);
}
