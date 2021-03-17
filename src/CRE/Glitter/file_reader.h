/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_file_reader* FASTCALL glitter_file_reader_init(wchar_t* path,
    wchar_t* file, bool f2, float_t emission);
extern bool FASTCALL glitter_file_reader_read(glitter_file_reader* fr);
extern void FASTCALL glitter_file_reader_dispose(glitter_file_reader* fr);
