/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "parse_file.h"

f2_header* FASTCALL Glitter__ParseFile__GetSubStructPointer(f2_header* header) {
    if (!Glitter__ParseFile__ShiftBy28Bits(header))
        return Glitter__ParseFile__GetDataPointer(header);
    
    return header->data_size > header->section_size
        ? (f2_header*)((char*)header + header->section_size + header->length) : 0;
}

void* FASTCALL Glitter__ParseFile__GetDataPointer(f2_header* header) {
    return Glitter__ParseFile__GetSectionSize(header) ? (char*)header + header->length : 0;
}

f2_header* FASTCALL Glitter__ParseFile__CheckForEOFC(f2_header* header) {
    void* v1;

    v1 = (void*)((char*)header + header->data_size + header->length);
    if (Glitter__ParseFile__ReverseSignatureEndianess(v1) == 'EOFC')
        v1 = 0;
    return v1;
}

uint32_t FASTCALL Glitter__ParseFile__ReverseSignatureEndianess(f2_header* header) {
    return ((header->signature >> 24) & 0xFF) | (header->signature >> 8) & 0xFF00
        | ((header->signature & 0xFF00) << 8) | ((header->signature & 0xFF) << 24);
}

uint32_t FASTCALL Glitter__ParseFile__GetVersion(f2_header* header) {
    return header->version;
}

uint32_t FASTCALL Glitter__ParseFile__GetSectionSize(f2_header* header) {
    return Glitter__ParseFile__ShiftBy28Bits(header) ? header->section_size : header->data_size;
}

uint32_t FASTCALL Glitter__ParseFile__ShiftBy28Bits(f2_header* header) {
    return (uint32_t)header->flags >> 28;
}
