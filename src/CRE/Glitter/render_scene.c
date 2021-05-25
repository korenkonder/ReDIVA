/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene.h"
#include "render_group.h"

void FASTCALL glitter_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg) {
    vector_ptr_glitter_render_group_push_back(rs, &rg);
}

void FASTCALL glitter_render_scene_calc_draw(GPM, glitter_render_scene* rs) {
    glitter_render_group** i;
    glitter_render_group* rg;

    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg && (!glitter_render_group_cannot_draw(rg) || GPM_VAL->draw_all))
            glitter_render_group_calc_draw(GPM_VAL, rg);
    }
}

void FASTCALL glitter_render_scene_draw(GPM, glitter_render_scene* rs, int32_t alpha) {
    glitter_render_group** i;
    glitter_render_group* rg;

    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg && (rg)->alpha == alpha
            && (!glitter_render_group_cannot_draw(rg) || GPM_VAL->draw_all))
            glitter_render_group_draw(rg);
    }
}

size_t FASTCALL glitter_render_scene_get_ctrl_count(glitter_render_scene* rs,
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

size_t FASTCALL glitter_render_scene_get_disp_count(glitter_render_scene* rs,
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

void FASTCALL glitter_render_scene_free(glitter_render_scene* rs) {
    vector_ptr_glitter_render_group_free(rs, glitter_render_group_dispose);
}

void FASTCALL glitter_render_scene_update(GLT, glitter_render_scene* rs,
    float_t delta_frame) {
    for (glitter_render_group** i = rs->begin; i != rs->end; i++)
        if (*i)
            glitter_render_group_update(GLT_VAL, *i, delta_frame, true);
}