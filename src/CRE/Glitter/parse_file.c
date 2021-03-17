/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "parse_file.h"

f2_header* FASTCALL glitter_parse_file_check_for_end_of_container(f2_header* header) {
    header = (void*)((char*)header + header->data_size + header->length);
    return glitter_parse_file_reverse_signature_endianess(header) == 'EOFC' ? 0 : header;
}

void* FASTCALL glitter_parse_file_get_data_ptr(f2_header* header) {
    return (header->use_section_size ? header->section_size : header->data_size)
        ? (char*)header + header->length : 0;
}

f2_header* FASTCALL glitter_parse_file_get_sub_struct_ptr(f2_header* header) {
    if (!header->use_section_size)
        return glitter_parse_file_get_data_ptr(header);
    
    return header->data_size > header->section_size
        ? (f2_header*)((char*)header + header->section_size + header->length) : 0;
}

uint32_t FASTCALL glitter_parse_file_reverse_signature_endianess(f2_header* header) {
    uint32_t signature;

    signature = header->signature;
    reverse_endianess_uint32_t(signature);
    return signature;
}
