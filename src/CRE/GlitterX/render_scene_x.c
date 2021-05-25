/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene_x.h"
#include "render_group_x.h"

void FASTCALL glitter_x_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg) {
    vector_ptr_glitter_render_group_push_back(rs, &rg);
}

void FASTCALL glitter_x_render_scene_calc_draw(GPM, glitter_render_scene* rs,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    glitter_render_group** i;
    glitter_render_group* rg;

    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg && (!glitter_x_render_group_cannot_draw(rg) || GPM_VAL->draw_all))
            glitter_x_render_group_calc_draw(GPM_VAL, rg, render_add_list_func);
    }
}

void FASTCALL glitter_x_render_scene_draw(GPM, glitter_render_scene* rs, int32_t alpha) {
    glitter_render_group** i;
    glitter_render_group* rg;

    for (i = rs->begin; i != rs->end; i++) {
        if (!*i)
            continue;

        rg = *i;
        if (rg && (rg)->alpha == alpha
            && (!glitter_x_render_group_cannot_draw(rg) || GPM_VAL->draw_all))
            glitter_x_render_group_draw(rg);
    }
}

void FASTCALL glitter_x_render_scene_free(glitter_render_scene* rs) {
    vector_ptr_glitter_render_group_free(rs, glitter_x_render_group_dispose);
}

void FASTCALL glitter_x_render_scene_update(glitter_render_scene* rs,
    float_t delta_frame, bool copy_mats) {
    for (glitter_render_group** i = rs->begin; i != rs->end; i++)
        if (*i)
            glitter_x_render_group_update(*i, delta_frame, copy_mats);
}
