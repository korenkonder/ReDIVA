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
#include "../data.h"

glitter_file_reader* glitter_file_reader_init(GLT,
    char* path, char* file, float_t emission) {
    glitter_file_reader* fr = force_malloc_s(glitter_file_reader, 1);
    fr->path = str_utils_copy(path ? path : "rom/particle/");
    fr->file = str_utils_copy(file);
    fr->emission = emission;
    fr->type = GLT_VAL;
    fr->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(fr->file, 0, false)
        : hash_utf8_fnv1a64m(fr->file, false);
    return fr;
}

glitter_file_reader* glitter_file_reader_winit(GLT,
    wchar_t* path, wchar_t* file, float_t emission) {
    glitter_file_reader* fr = force_malloc_s(glitter_file_reader, 1);
    fr->path = path ? utf16_to_utf8(path) : str_utils_copy("rom/particle/");
    fr->file = utf16_to_utf8(file);
    fr->emission = emission;
    fr->type = GLT_VAL;
    fr->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(fr->file, 0, false)
        : hash_utf8_fnv1a64m(fr->file, false);
    return fr;
}

bool glitter_file_reader_read(GPM, glitter_file_reader* fr, float_t emission) {
    farc f;
    farc_init(&f);
    char* farc_file_temp = str_utils_add(fr->file, ".farc");
    bool ret = data_struct_load_file((data_struct*)GPM_VAL->data, &f, fr->path, farc_file_temp, farc_load_file);
    free(farc_file_temp);

    if (!ret) {
        farc_free(&f);
        return false;
    }

    ret = false;
    char* dve_file_temp = str_utils_add(fr->file, ".dve");
    farc_file* dve_ff = farc_read_file(&f, dve_file_temp);
    free(dve_file_temp);
    if (!dve_ff) {
        farc_free(&f);
        return false;
    }

    f2_struct st;
    f2_struct_mread(&st, dve_ff->data, dve_ff->size);
    if (st.header.signature == reverse_endianness_uint32_t('DVEF'))
        ret = glitter_diva_effect_parse_file(fr, &st, emission);
    else
        ret = false;
    f2_struct_free(&st);

    if (!ret) {
        farc_free(&f);
        return false;
    }

    char* drs_file_temp = str_utils_add(fr->file, ".drs");
    farc_file* drs_ff = farc_read_file(&f, drs_file_temp);
    free(drs_file_temp);

    if (drs_ff) {
        f2_struct st;
        f2_struct_mread(&st, drs_ff->data, drs_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('DVRS'))
            glitter_diva_resource_parse_file(GPM_VAL, fr->effect_group, &st);
        f2_struct_free(&st);
    }

    char* lst_file_temp = str_utils_add(fr->file, ".lst");
    farc_file* lst_ff = farc_read_file(&f, lst_file_temp);
    free(lst_file_temp);

    if (lst_ff) {
        f2_struct st;
        f2_struct_mread(&st, lst_ff->data, lst_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('LIST'))
            glitter_diva_list_parse_file(fr->effect_group, &st);
        f2_struct_free(&st);
    }

    farc_free(&f);
    return true;
}

void glitter_file_reader_dispose(glitter_file_reader* fr) {
    free(fr->path);
    free(fr->file);
    free(fr);
}

