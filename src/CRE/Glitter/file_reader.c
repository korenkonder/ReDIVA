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

glitter_file_reader::glitter_file_reader(GLT) : farc(), effect_group(),
load_count(), type(), path(), file(), state(), init_scene() {
    emission = -1.0f;
    type = GLT_VAL;
    hash = GLT_VAL != GLITTER_FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
}

glitter_file_reader::glitter_file_reader(GLT, char* path, char* file, float_t emission)
    : effect_group(), load_count() {
    this->path = std::string(path ? path : "rom/particle/");
    this->file = std::string(file);
    this->emission = emission;
    this->type = GLT_VAL;
    this->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(file, 0, false)
        : hash_utf8_fnv1a64m(file, false);
}

glitter_file_reader::glitter_file_reader(GLT, wchar_t* path, wchar_t* file, float_t emission)
    : effect_group(), load_count() {
    char* path_temp = utf16_to_utf8(path);
    char* file_temp = utf16_to_utf8(file);
    this->path = std::string(path_temp ? path_temp : "rom/particle/");
    this->file = std::string(file_temp);
    this->emission = emission;
    this->type = GLT_VAL;
    this->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(file_temp, 0, false)
        : hash_utf8_fnv1a64m(file_temp, false);
    free(path_temp);
    free(file_temp);
}

glitter_file_reader::~glitter_file_reader() {

}

bool glitter_file_reader::Read(GPM, float_t emission) {
    ::farc f;
    char* farc_file_temp = str_utils_add(this->file.c_str(), ".farc");
    bool ret = data_struct_load_file((data_struct*)GPM_VAL->data,
        &f, this->path.c_str(), farc_file_temp, farc::load_file);
    free(farc_file_temp);

    if (!ret)
        return false;

    ret = false;
    char* dve_file_temp = str_utils_add(this->file.c_str(), ".dve");
    farc_file* dve_ff = f.read_file(dve_file_temp);
    free(dve_file_temp);
    if (!dve_ff)
        return false;

    f2_struct st;
    f2_struct_mread(&st, dve_ff->data, dve_ff->size);
    if (st.header.signature == reverse_endianness_uint32_t('DVEF'))
        ret = glitter_diva_effect_parse_file(GPM_VAL, this, &st, emission);
    else
        ret = false;
    f2_struct_free(&st);

    if (!ret)
        return false;

    char* drs_file_temp = str_utils_add(this->file.c_str(), ".drs");
    farc_file* drs_ff = f.read_file(drs_file_temp);
    free(drs_file_temp);

    if (drs_ff) {
        f2_struct st;
        f2_struct_mread(&st, drs_ff->data, drs_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('DVRS'))
            glitter_diva_resource_parse_file(GPM_VAL, this->effect_group, &st);
        f2_struct_free(&st);
    }

    char* lst_file_temp = str_utils_add(this->file.c_str(), ".lst");
    farc_file* lst_ff = f.read_file(lst_file_temp);
    free(lst_file_temp);

    if (lst_ff) {
        f2_struct st;
        f2_struct_mread(&st, lst_ff->data, lst_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('LIST'))
            glitter_diva_list_parse_file(this->effect_group, &st);
        f2_struct_free(&st);
    }
    return true;
}

bool glitter_file_reader::ReadFarc(GPM, float_t emission) {
    if (state == 0 && Read(GPM_VAL, emission) && init_scene) {
        state = 1;
        return false;
    }
    return true;
}

bool glitter_file_reader_load_farc(glitter_file_reader* fr,
    const char* path, const char* file, uint64_t hash) {
    if (fr->type == GLITTER_FT && fr->hash != hash_fnv1a64m_empty
        || fr->type != GLITTER_FT && fr->hash != hash_murmurhash_empty)
        return false;

    fr->path = std::string(path);
    fr->file = std::string(file);

    fr->farc = new farc;

    std::string file_temp = fr->file + ".farc";

    if (data_struct_load_file((data_struct*)(GPM_VAL.data),
        fr->farc, path, file_temp.c_str(), farc::load_file)) {
        fr->hash = hash;
        fr->load_count = 1;
        return true;
    }
    else
        return false;
}
