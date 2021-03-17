/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "locus_history.h"

glitter_locus_history* FASTCALL glitter_locus_history_init(size_t size) {
    glitter_locus_history* lh = force_malloc(sizeof(glitter_locus_history));
    vector_glitter_locus_history_data_resize(&lh->data, size);
    return lh;
}

void FASTCALL glitter_locus_history_append(glitter_locus_history* a1,
    glitter_render_group_sub* a2, glitter_particle_inst* a3) {
    glitter_locus_history_data* v5;
    int64_t size;
    int64_t i;
    glitter_locus_history_data locus_history;
    vec3 temp;
    vec3 temp1;

    v5 = a1->data.begin;
    size = a1->data.end - a1->data.begin;
    if (a3->sub.data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
        temp = *(vec3*)&a3->mat.row3;
    else
        temp = a2->translation;

    locus_history.translation = temp;
    locus_history.color = a2->color;
    locus_history.scale = a2->scale_particle.x * a2->scale.x * a2->scale_all;
    if (size < 1)
        vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
    else if (size == 1) {
        locus_history.translation = v5->translation;
        if (a1->data.capacity_end - a1->data.begin > 1)
            vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
        v5->translation = temp;
    }
    else {
        temp1 = v5[size - 1].translation;

        for (i = size - 1; i > 0; i--)
            v5[i].translation = v5[i - 1].translation;

        if (size < a1->data.capacity_end - a1->data.begin) {
            locus_history.translation = temp1;
            vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
        }
        v5->translation = temp;
    }
}

void FASTCALL glitter_locus_history_dispose(glitter_locus_history* lh) {
    vector_glitter_locus_history_data_free(&lh->data);
    free(lh);
}
