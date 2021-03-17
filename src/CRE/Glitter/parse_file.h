/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern f2_header* FASTCALL glitter_parse_file_check_for_end_of_container(f2_header* header);
extern void* FASTCALL glitter_parse_file_get_data_ptr(f2_header* header);
extern f2_header* FASTCALL glitter_parse_file_get_sub_struct_ptr(f2_header* header);
extern uint32_t FASTCALL glitter_parse_file_reverse_signature_endianess(f2_header* header);
