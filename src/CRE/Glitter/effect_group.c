/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_group.h"
#include "effect.h"
#include "parse_file.h"
#include "scene.h"
#include "texture.h"

glitter_effect_group* FASTCALL glitter_effect_group_init() {
    glitter_effect_group* eg = force_malloc(sizeof(glitter_effect_group));
    eg->emission = 1.0f;
    return eg;
}

bool FASTCALL glitter_effect_group_parse_file(glitter_file_reader* a1,
    f2_header* header, glitter_effect_group* a3) {
    uint32_t signature;
    glitter_effect* v8;

    if (!header)
        return false;

    while (header) {
        if (!header->data_size)
            continue;

        signature = glitter_parse_file_reverse_signature_endianess(header);
        if (signature == 'EFCT') {
            v8 = glitter_effect_init();
            if (!v8)
                return false;

            if (!glitter_effect_parse_file(a1, header, v8)) {
                glitter_effect_dispose(v8);
                return false;
            }

            vector_ptr_glitter_effect_append_element(&a3->effects, &v8);
        }
        else if (signature == 'DVRS' && !glitter_texture_hashes_unpack_file(a3, header))
            return false;

        header = glitter_parse_file_check_for_end_of_container(header);
    }
    return true;
}

void FASTCALL glitter_effect_group_dispose(glitter_effect_group* eg) {
    if (eg->scene)
        glitter_scene_dispose(eg->scene);
    vector_ptr_glitter_effect_clear(&eg->effects, (void*)glitter_effect_dispose);
    vector_ptr_glitter_effect_dispose(&eg->effects);
    free(eg->resource_hashes);
    if (eg->resources)
        glDeleteTextures(eg->resources_count, eg->resources);
    free(eg);
}
