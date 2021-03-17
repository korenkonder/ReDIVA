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

glitter_file_reader* FASTCALL glitter_file_reader_init(wchar_t* path,
    wchar_t* file, bool f2, float_t emission) {
    glitter_file_reader* fr = force_malloc(sizeof(glitter_file_reader));
    fr->path = path_wadd_extension(path ? path : L"rom\\particle\\", 0);
    fr->file = path_wadd_extension(file ? file : 0, 0);
    fr->emission = emission;
    fr->hash = f2
        ? hash_wchar_t_murmurhash(file, 0, false)
        : hash_wchar_t_fnv1a64(file);
    return fr;
}

bool FASTCALL glitter_file_reader_read(glitter_file_reader* fr) {
    farc* f = farc_init();
    wchar_t* path_temp0;
    wchar_t* path_temp1;
    path_temp0 = path_wadd_extension(fr->file, L".farc");
    path_temp1 = path_wadd_extension(fr->path, path_temp0);
    free(path_temp0);
    farc_wread(f, path_temp1, true, false);
    free(path_temp1);

    farc_file* ff;
    bool ret = false;
    path_temp0 = path_wadd_extension(fr->file, L".dve");
    ff = farc_wread_file(f, path_temp0);
    free(path_temp0);
    if (!ff)
        goto End;

    f2_struct* st;
    st = f2_struct_init();
    f2_struct_read_memory(st, ff->data, ff->size_uncompressed);
    if (st->header.signature == 0x46455644)
        ret = glitter_diva_effect_parse_file(fr, st);
    else
        ret = false;
    f2_struct_dispose(st);

    if (!ret)
        goto End;

    path_temp0 = path_wadd_extension(fr->file, L".drs");
    ff = farc_wread_file(f, path_temp0);
    free(path_temp0);

    ret = false;
    if (!ff)
        goto End;

    st = f2_struct_init();
    f2_struct_read_memory(st, ff->data, ff->size_uncompressed);
    if (st->header.signature == 0x53525644)
        ret = glitter_diva_resource_parse_file(fr->effect_group, st);
    else
        ret = false;
    f2_struct_dispose(st);

    if (!ret)
        goto End;

    path_temp0 = path_wadd_extension(fr->file, L".lst");
    ff = farc_wread_file(f, path_temp0);
    free(path_temp0);

    if (!ff)
        goto End;

    st = f2_struct_init();
    f2_struct_read_memory(st, ff->data, ff->size_uncompressed);
    if (st->header.signature == 0x5453494C)
        glitter_diva_list_parse_file(fr->effect_group, st);
    f2_struct_dispose(st);

End:
    farc_dispose(f);
    return ret;
}

void FASTCALL glitter_file_reader_dispose(glitter_file_reader* fr) {
    free(fr->path);
    free(fr->file);
    free(fr);
}

