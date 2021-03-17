/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern f2_header* FASTCALL Glitter__ParseFile__GetSubStructPointer(f2_header* header);
extern void* FASTCALL Glitter__ParseFile__GetDataPointer(f2_header* header);
extern f2_header* FASTCALL Glitter__ParseFile__CheckForEOFC(f2_header* header);
extern uint32_t FASTCALL Glitter__ParseFile__ReverseSignatureEndianess(f2_header* header);
extern uint32_t FASTCALL Glitter__ParseFile__GetVersion(f2_header* header);
extern uint32_t FASTCALL Glitter__ParseFile__GetSectionSize(f2_header* header);
extern uint32_t FASTCALL Glitter__ParseFile__ShiftBy28Bits(f2_header* header);
