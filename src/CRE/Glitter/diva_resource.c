/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_resource.h"
#include "parse_file.h"
#include "texture.h"

bool FASTCALL glitter_diva_resource_parse_file(glitter_effect_group* a1, f2_header* header) {
    if (!header || glitter_parse_file_reverse_signature_endianess(header) != 'DVRS')
        return false;

    header = glitter_parse_file_get_sub_struct_ptr(header);
    if (!header)
        return false;

    if (glitter_parse_file_reverse_signature_endianess(header) == 'TXPC')
        glitter_texture_resource_unpack_file(a1, (uint64_t)glitter_parse_file_get_data_ptr(header));
    return true;
}
