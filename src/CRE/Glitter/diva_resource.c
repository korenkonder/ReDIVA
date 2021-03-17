/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_resource.h"
#include "parse_file.h"
#include "texture.h"

bool FASTCALL Glitter__DivaResource__ParseFile(glitter_effect_group* a1, f2_header* header) {
    if (!header || Glitter__ParseFile__ReverseSignatureEndianess(header) != 'DVRS')
        return false;

    header = Glitter__ParseFile__GetSubStructPointer(header);
    if (!header)
        return false;

    if (Glitter__ParseFile__ReverseSignatureEndianess(header) == 'TXPC')
        Glitter__Texture__Resource__UnpackFile(a1, (uint64_t)Glitter__ParseFile__GetDataPointer(header));
    return true;
}
