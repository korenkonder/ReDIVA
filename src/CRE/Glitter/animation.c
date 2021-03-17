/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "animation.h"
#include "curve.h"
#include "parse_file.h"

void FASTCALL glitter_animation_parse_file(glitter_file_reader* a1,
    f2_header* header, vector_ptr_glitter_curve* a3) {
    f2_header* i;

    if (!header || !header->data_size || glitter_parse_file_reverse_signature_endianess(header) != 'ANIM')
        return;

    for (i = glitter_parse_file_get_sub_struct_ptr(header); i; i = glitter_parse_file_check_for_end_of_container(i)) {
        if (!i->data_size)
            continue;

        if (glitter_parse_file_reverse_signature_endianess(i) == 'CURV')
            glitter_curve_parse_file(a1, i, a3);
    }
}