/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene_x.h"
#include "render_group_x.h"

void glitter_x_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg) {
    vector_ptr_glitter_render_group_push_back(rs, &rg);
}

void glitter_x_render_scene_calc_draw(GPM, glitter_render_scene* rs) {
    glitter_render_group** i;
    glitter_render_group* rg;

    glitter_effect_inst* eff = GPM_VAL->effect;
    glitter_emitter_inst* emit = GPM_VAL->emitter;
    glitter_particle_inst* ptcl = GPM_VAL->particle;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (glitter_x_render_group_cannot_draw(rg) && !GPM_VAL->draw_all)
            continue;

#if !defined(CRE_DEV)
        glitter_x_render_group_calc_draw(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff)
            glitter_x_render_group_calc_draw(GPM_VAL, rg);
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                glitter_x_render_group_calc_draw(GPM_VAL, rg);
        }
        else if (emit)
            for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
                if (!*i)
                    continue;

                glitter_particle_inst* particle = *i;
                if (rg->particle == particle)
                    glitter_x_render_group_calc_draw(GPM_VAL, rg);

                if (particle->data.children.begin) {
                    vector_ptr_glitter_particle_inst* children = &particle->data.children;
                    for (glitter_particle_inst** j = children->begin; j != children->end; j++) {
                        if (!*j || rg->particle != *j)
                            continue;

                        glitter_x_render_group_calc_draw(GPM_VAL, rg);
                    }
                }
            }
#endif
    }
}

void glitter_x_render_scene_draw(GPM, glitter_render_scene* rs, draw_pass_3d_type alpha) {
    glitter_render_group** i;
    glitter_render_group* rg;

    glitter_effect_inst* eff = GPM_VAL->effect;
    glitter_emitter_inst* emit = GPM_VAL->emitter;
    glitter_particle_inst* ptcl = GPM_VAL->particle;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if ((rg)->alpha != alpha || glitter_x_render_group_cannot_draw(rg) && !GPM_VAL->draw_all)
            continue;

#if !defined(CRE_DEV)
        glitter_x_render_group_draw(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            glitter_x_render_group_draw(GPM_VAL, rg);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                glitter_x_render_group_draw(GPM_VAL, rg);
        }
        else if (emit)
            for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
                if (!*i)
                    continue;

                glitter_particle_inst* particle = *i;
                if (rg->particle == particle)
                    glitter_x_render_group_draw(GPM_VAL, rg);

                if (particle->data.children.begin) {
                    vector_ptr_glitter_particle_inst* children = &particle->data.children;
                    for (glitter_particle_inst** j = children->begin; j != children->end; j++) {
                        if (!*j || rg->particle != *j)
                            continue;

                        glitter_x_render_group_draw(GPM_VAL, rg);
                    }
                }
            }
#endif
    }
}

void glitter_x_render_scene_free(glitter_render_scene* rs) {
    vector_ptr_glitter_render_group_free(rs, glitter_x_render_group_dispose);
}

void glitter_x_render_scene_update(glitter_render_scene* rs,
    float_t delta_frame, bool copy_mats) {
    for (glitter_render_group** i = rs->begin; i != rs->end; i++)
        if (*i)
            glitter_x_render_group_update(*i, delta_frame, copy_mats);
}
