/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_file_reader* FASTCALL glitter_file_reader_init(wchar_t* path, wchar_t* file, bool f2);
extern void FASTCALL glitter_file_reader_dispose(glitter_file_reader* fr);

extern bool FASTCALL Glitter__FileReader__Read(glitter_file_reader* fr);
