/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene.h"
#include "render_group.h"

void glitter_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg) {
    vector_ptr_glitter_render_group_push_back(rs, &rg);
}

void glitter_render_scene_calc_disp(GPM, glitter_render_scene* rs) {
    glitter_render_group** i;
    glitter_render_group* rg;

    glitter_effect_inst* eff = GPM_VAL->effect;
    glitter_emitter_inst* emit = GPM_VAL->emitter;
    glitter_particle_inst* ptcl = GPM_VAL->particle;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (glitter_render_group_cannot_disp(rg) && !GPM_VAL->draw_all)
            continue;

#if !defined(CRE_DEV)
        glitter_render_group_calc_disp(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            glitter_render_group_calc_disp(GPM_VAL, rg);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                glitter_render_group_calc_disp(GPM_VAL, rg);
        }
        else if (emit)
            for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
                if (!*i)
                    continue;

                glitter_particle_inst* particle = *i;
                if (rg->particle == particle)
                    glitter_render_group_calc_disp(GPM_VAL, rg);

                if (particle->data.children.begin) {
                    vector_ptr_glitter_particle_inst* children = &particle->data.children;
                    for (glitter_particle_inst** j = children->begin; j != children->end; j++) {
                        if (!*j || rg->particle != *j)
                            continue;

                        glitter_render_group_calc_disp(GPM_VAL, rg);
                    }
                }
            }
#endif
    }
}

void glitter_render_scene_ctrl(GLT, glitter_render_scene* rs,
    float_t delta_frame) {
    for (glitter_render_group** i = rs->begin; i != rs->end; i++)
        if (*i)
            glitter_render_group_ctrl(GLT_VAL, *i, delta_frame, true);
}

void glitter_render_scene_disp(GPM, glitter_render_scene* rs, draw_pass_3d_type alpha) {
    glitter_render_group** i;
    glitter_render_group* rg;

    glitter_effect_inst* eff = GPM_VAL->effect;
    glitter_emitter_inst* emit = GPM_VAL->emitter;
    glitter_particle_inst* ptcl = GPM_VAL->particle;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if ((rg)->alpha != alpha
            || (glitter_render_group_cannot_disp(rg) && !GPM_VAL->draw_all))
            continue;

#if !defined(CRE_DEV)
        glitter_render_group_draw(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            glitter_render_group_draw(GPM_VAL, rg);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                glitter_render_group_draw(GPM_VAL, rg);
        }
        else if (emit)
            for (glitter_particle_inst** i = emit->particles.begin; i != emit->particles.end; i++) {
                if (!*i)
                    continue;

                glitter_particle_inst* particle = *i;
                if (rg->particle == particle)
                    glitter_render_group_draw(GPM_VAL, rg);

                if (particle->data.children.begin) {
                    vector_ptr_glitter_particle_inst* children = &particle->data.children;
                    for (glitter_particle_inst** j = children->begin; j != children->end; j++) {
                        if (!*j || rg->particle != *j)
                            continue;

                        glitter_render_group_draw(GPM_VAL, rg);
                    }
                }
            }
#endif
    }
}

size_t glitter_render_scene_get_ctrl_count(glitter_render_scene* rs,
    glitter_particle_type type) {
    glitter_render_group** i;
    glitter_render_group* rg;
    size_t ctrl;

    ctrl = 0;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg->type == type)
            ctrl += rg->ctrl;
    }
    return ctrl;
}

size_t glitter_render_scene_get_disp_count(glitter_render_scene* rs,
    glitter_particle_type type) {
    glitter_render_group** i;
    glitter_render_group* rg;
    size_t disp;

    disp = 0;
    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg->type == type)
            disp += rg->disp;
    }
    return disp;
}

void glitter_render_scene_free(glitter_render_scene* rs) {
    vector_ptr_glitter_render_group_free(rs, glitter_render_group_dispose);
}
