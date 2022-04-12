/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "locus_history_x.h"

void GlitterLocusHistory::Append(glitter_render_element* a2, GlitterXParticleInst* a3) {
    GlitterLocusHistory::Data* data;
    GlitterLocusHistory::Data locus_history;
    GlitterXEmitterInst* emitter;
    size_t size;
    size_t i;
    vec3 temp;
    vec3 temp1;

    data = this->data.data();
    size = this->data.size();
    temp = a2->translation;
    if (a3->data.data.flags & GLITTER_PARTICLE_EMITTER_LOCAL && (emitter = a3->data.emitter)) {
        vec3 emit_trans;
        mat4_get_translation(&emitter->mat, &emit_trans);
        vec3_add(temp, emit_trans, temp);
        if (a3->data.data.flags & GLITTER_PARTICLE_ROTATE_LOCUS) {
            mat4 mat_rot = emitter->mat_rot;
            mat4_mult_vec3(&mat_rot, &temp, &temp);
        }
    }

    locus_history.color = a2->color;
    locus_history.translation = temp;
    locus_history.scale = a2->scale_particle.x * a2->scale.x * a2->scale_all;
    if (size < 1)
        this->data.push_back(locus_history);
    else if (size == 1) {
        locus_history.translation = data->translation;
        if (this->data.capacity() > 1)
            this->data.push_back(locus_history);
        data->translation = temp;
    }
    else {
        temp1 = data[size - 1].translation;

        for (i = size - 1; i > 0; i--)
            data[i].translation = data[i - 1].translation;

        if (size < this->data.capacity()) {
            locus_history.translation = temp1;
            this->data.push_back(locus_history);
        }
        data->translation = temp;
    }
}
