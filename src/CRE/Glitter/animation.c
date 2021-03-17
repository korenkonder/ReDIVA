/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "animation.h"
#include "curve.h"
#include "parse_file.h"

void FASTCALL Glitter__Animation__ParseFile(glitter_file_reader* a1,
    f2_header* header, vector_ptr_glitter_curve* a3) {
    f2_header* i;

    if (!header || !header->data_size || Glitter__ParseFile__ReverseSignatureEndianess(header) != 'ANIM')
        return;

    for (i = Glitter__ParseFile__GetSubStructPointer(header); i; i = Glitter__ParseFile__CheckForEOFC(i)) {
        if (!i->data_size)
            continue;

        if (Glitter__ParseFile__ReverseSignatureEndianess(i) == 'CURV')
            Glitter__Curve__ParseFile(a1, i, a3);
    }
}