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
#include "../../KKdLib/io/path.h"
#include "../../KKdLib/farc.h"
#include "../../KKdLib/str_utils.h"

glitter_file_reader* glitter_file_reader_init(GLT,
    char* path, char* file, float_t emission) {
    glitter_file_reader* fr = force_malloc(sizeof(glitter_file_reader));
    fr->path = str_utils_copy(path ? path : "rom\\particle\\");
    fr->file = str_utils_copy(file);
    fr->emission = emission;
    fr->type = GLT_VAL;
    fr->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(fr->file, 0, false)
        : hash_utf8_fnv1a64m(fr->file);
    return fr;
}

glitter_file_reader* glitter_file_reader_winit(GLT,
    wchar_t* path, wchar_t* file, float_t emission) {
    glitter_file_reader* fr = force_malloc(sizeof(glitter_file_reader));
    fr->path = utf16_to_utf8(path ? path : L"rom\\particle\\");
    fr->file = utf16_to_utf8(file);
    fr->emission = emission;
    fr->type = GLT_VAL;
    fr->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(fr->file, 0, false)
        : hash_utf8_fnv1a64m(fr->file);
    return fr;
}

bool glitter_file_reader_read(GPM, glitter_file_reader* fr, float_t emission) {
    farc f;
    farc_init(&f);
    char* file_temp;
    char* path_temp;
    file_temp = str_utils_add(fr->file, ".farc");
    path_temp = str_utils_add(fr->path, file_temp);
    free(file_temp);
    farc_read(&f, path_temp, true, false);
    free(path_temp);

    farc_file* ff;
    bool ret = false;
    file_temp = str_utils_add(fr->file, ".dve");
    ff = farc_read_file(&f, file_temp);
    free(file_temp);
    if (!ff)
        goto End;

    f2_struct st;
    f2_struct_mread(&st, ff->data, ff->size);
    if (st.header.signature == reverse_endianness_uint32_t('DVEF'))
        ret = glitter_diva_effect_parse_file(fr, &st, emission);
    else
        ret = false;
    f2_struct_free(&st);

    if (!ret)
        goto End;

    file_temp = str_utils_add(fr->file, ".drs");
    ff = farc_read_file(&f, file_temp);
    free(file_temp);

    if (ff) {
        f2_struct st;
        f2_struct_mread(&st, ff->data, ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('DVRS'))
            glitter_diva_resource_parse_file(GPM_VAL, fr->effect_group, &st);
        f2_struct_free(&st);
    }

    file_temp = str_utils_add(fr->file, ".lst");
    ff = farc_read_file(&f, file_temp);
    free(file_temp);

    if (ff) {
        f2_struct st;
        f2_struct_mread(&st, ff->data, ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('LIST'))
            glitter_diva_list_parse_file(fr->effect_group, &st);
        f2_struct_free(&st);
    }

End:
    farc_free(&f);
    return ret;
}

void glitter_file_reader_dispose(glitter_file_reader* fr) {
    free(fr->path);
    free(fr->file);
    free(fr);
}

