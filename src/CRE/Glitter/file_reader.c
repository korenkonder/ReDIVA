/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "file_reader.h"
#include "diva_effect.h"
#include "diva_list.h"
#include "diva_resource.h"
#include "effect_group.h"
#include "particle_manager.h"
#include "scene.h"

glitter_file_reader* FASTCALL glitter_file_reader_init(GPM,
    char* path, char* file, float_t emission) {
    glitter_file_reader* fr = force_malloc(sizeof(glitter_file_reader));
    fr->path = char_string_to_wchar_t_string(path ? path : "rom\\particle\\");
    fr->file = char_string_to_wchar_t_string(file ? file : 0);
    fr->emission = emission;
    fr->hash = glt_type != GLITTER_AFT
        ? hash_wchar_t_murmurhash(fr->file, 0, false)
        : hash_wchar_t_fnv1a64(fr->file);
    return fr;
}

glitter_file_reader* FASTCALL glitter_file_reader_winit(GPM,
    wchar_t* path, wchar_t* file, float_t emission) {
    glitter_file_reader* fr = force_malloc(sizeof(glitter_file_reader));
    fr->path = str_utils_wcopy(path ? path : L"rom\\particle\\");
    fr->file = str_utils_wcopy(file);
    fr->emission = emission;
    fr->hash = glt_type != GLITTER_AFT
        ? hash_wchar_t_murmurhash(fr->file, 0, false)
        : hash_wchar_t_fnv1a64(fr->file);
    return fr;
}

bool FASTCALL glitter_file_reader_read(GPM, glitter_file_reader* fr) {
    farc* f = farc_init();
    wchar_t* file_temp;
    wchar_t* path_temp;
    file_temp = str_utils_wadd(fr->file, L".farc");
    path_temp = str_utils_wadd(fr->path, file_temp);
    free(file_temp);
    farc_wread(f, path_temp, true, false);
    free(path_temp);

    farc_file* ff;
    bool ret = false;
    file_temp = str_utils_wadd(fr->file, L".dve");
    ff = farc_wread_file(f, file_temp);
    free(file_temp);
    if (!ff)
        goto End;

    f2_struct st;
    f2_struct_read_memory(&st, ff->data, ff->size);
    if (st.header.signature == 0x46455644)
        ret = glitter_diva_effect_parse_file(GPM_VAL, fr, &st);
    else
        ret = false;
    f2_struct_free(&st);

    if (!ret)
        goto End;

    file_temp = str_utils_wadd(fr->file, L".drs");
    ff = farc_wread_file(f, file_temp);
    free(file_temp);

    if (ff) {
        f2_struct st;
        f2_struct_read_memory(&st, ff->data, ff->size);
        if (st.header.signature == 0x53525644)
            glitter_diva_resource_parse_file(fr->effect_group, &st);
        f2_struct_free(&st);
    }

    file_temp = str_utils_wadd(fr->file, L".lst");
    ff = farc_wread_file(f, file_temp);
    free(file_temp);

    if (ff) {
        f2_struct st;
        f2_struct_read_memory(&st, ff->data, ff->size);
        if (st.header.signature == 0x5453494C)
            glitter_diva_list_parse_file(fr->effect_group, &st);
        f2_struct_free(&st);
    }

End:
    farc_dispose(f);
    return ret;
}

void FASTCALL glitter_file_reader_dispose(glitter_file_reader* fr) {
    free(fr->path);
    free(fr->file);
    free(fr);
}

