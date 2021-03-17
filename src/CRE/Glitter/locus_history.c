/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "locus_history.h"

glitter_locus_history* FASTCALL glitter_locus_history_init(size_t size) {
    glitter_locus_history* lh = force_malloc(sizeof(glitter_locus_history));
    vector_glitter_locus_history_data_expand(&lh->data, size);
    return lh;
}

void FASTCALL glitter_locus_history_dispose(glitter_locus_history* lh) {
    vector_glitter_locus_history_data_clear(&lh->data);
    vector_glitter_locus_history_data_dispose(&lh->data);
    free(lh);
}
