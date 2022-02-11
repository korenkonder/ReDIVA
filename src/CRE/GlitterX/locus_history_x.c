/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "locus_history_x.h"

glitter_locus_history* glitter_x_locus_history_init(size_t size) {
    glitter_locus_history* lh = force_malloc_s(glitter_locus_history, 1);
    vector_glitter_locus_history_data_reserve(&lh->data, size);
    return lh;
}

void glitter_x_locus_history_append(glitter_locus_history* a1,
    glitter_render_element* a2, glitter_particle_inst* a3) {
    glitter_locus_history_data* data;
    glitter_locus_history_data locus_history;
    glitter_emitter_inst* emitter;
    ssize_t size;
    ssize_t i;
    vec3 temp;
    vec3 temp1;

    data = a1->data.begin;
    size = vector_length(a1->data);
    temp = a2->translation;
    if (a3->data.data.flags & GLITTER_PARTICLE_EMITTER_LOCAL && (emitter = a3->data.emitter)) {
        vec3 emit_trans;
        mat4_get_translation(&emitter->mat, &emit_trans);
        vec3_add(temp, emit_trans, temp);
        if (a3->data.data.flags & GLITTER_PARTICLE_ROTATE_LOCUS)
            mat4_mult_vec3(&emitter->mat_rot, &temp, &temp);
    }

    locus_history.color = a2->color;
    locus_history.translation = temp;
    locus_history.scale = a2->scale_particle.x * a2->scale.x * a2->scale_all;
    if (size < 1)
        vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
    else if (size == 1) {
        locus_history.translation = data->translation;
        if (vector_capacity(a1->data) > 1)
            vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
        data->translation = temp;
    }
    else {
        temp1 = data[size - 1].translation;

        for (i = size - 1; i > 0; i--)
            data[i].translation = data[i - 1].translation;

        if (size < vector_capacity(a1->data)) {
            locus_history.translation = temp1;
            vector_glitter_locus_history_data_push_back(&a1->data, &locus_history);
        }
        data->translation = temp;
    }
}

void glitter_x_locus_history_dispose(glitter_locus_history* lh) {
    vector_glitter_locus_history_data_free(&lh->data, 0);
    free(lh);
}
