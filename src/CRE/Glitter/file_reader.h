/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_file_reader* FASTCALL glitter_file_reader_init(GLT,
    char* path, char* file, float_t emission);
extern glitter_file_reader* FASTCALL glitter_file_reader_winit(GLT,
    wchar_t* path, wchar_t* file, float_t emission);
extern bool FASTCALL glitter_file_reader_read(glitter_file_reader* fr, float_t emission);
extern void FASTCALL glitter_file_reader_dispose(glitter_file_reader* fr);
