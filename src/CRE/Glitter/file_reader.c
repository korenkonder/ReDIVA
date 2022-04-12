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

GlitterFileReader::GlitterFileReader(GLT) : file_handler(), farc(), effect_group(),
load_count(), type(), path(), file(), state(), init_scene(), obj_db() {
    emission = -1.0f;
    type = GLT_VAL;
    hash = GLT_VAL != GLITTER_FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
}

GlitterFileReader::GlitterFileReader(GLT, char* path, char* file, float_t emission)
    : effect_group(), load_count() {
    this->path = std::string(path ? path
        : (GLT_VAL != GLITTER_FT ? "root+/particle/" : "rom/particle/"));
    this->file = std::string(file);
    this->emission = emission;
    this->type = GLT_VAL;
    this->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(file)
        : hash_utf8_fnv1a64m(file);
}

GlitterFileReader::GlitterFileReader(GLT, wchar_t* path, wchar_t* file, float_t emission)
    : effect_group(), load_count() {
    char* path_temp = utf16_to_utf8(path);
    char* file_temp = utf16_to_utf8(file);
    this->path = std::string(path_temp ? path_temp
        : (GLT_VAL != GLITTER_FT ? "root+/particle/" : "rom/particle/"));
    this->file = std::string(file_temp);
    this->emission = emission;
    this->type = GLT_VAL;
    this->hash = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(file_temp)
        : hash_utf8_fnv1a64m(file_temp);
    free(path_temp);
    free(file_temp);
}

GlitterFileReader::~GlitterFileReader() {
    delete farc;
    delete file_handler;
}

bool GlitterFileReader::LoadFarc(void* data, const char* path,
    const char* file, uint64_t hash, object_database* obj_db) {
    this->obj_db = obj_db;
    if (type == GLITTER_FT && this->hash != hash_fnv1a64m_empty
        || type != GLITTER_FT && this->hash != hash_murmurhash_empty)
        return false;

    this->path = std::string(path);
    this->file = std::string(file);

    std::string file_temp = this->file + ".farc";

    file_handler = new p_file_handler;
    farc = new ::farc;
    if (file_handler->read_file(data, path, file_temp.c_str())) {
        this->hash = hash;
        load_count = 1;
        return true;
    }
    return false;
}

bool GlitterFileReader::Read(GPM, object_database* obj_db) {
    std::string dve_file = file + ".dve";
    farc_file* dve_ff = farc->read_file(dve_file.c_str());
    if (!dve_ff)
        return false;

    f2_struct st;
    st.read(dve_ff->data, dve_ff->size);
    if (st.header.signature != reverse_endianness_uint32_t('DVEF'))
        return false;

    if (!glitter_diva_effect_parse_file(GPM_VAL, this, &st, obj_db)) {
        GlitterEffectGroup* eff_group = new GlitterEffectGroup(type);
        if (eff_group) {
            eff_group->not_loaded = true;
            if (!GPM_VAL->AppendEffectGroup(hash, eff_group, this))
                delete eff_group;
        }
        return false;
    }

    std::string drs_file = file + ".drs";
    farc_file* drs_ff = farc->read_file(drs_file.c_str());
    if (drs_ff) {
        f2_struct st;
        st.read(drs_ff->data, drs_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('DVRS') && effect_group)
            glitter_diva_resource_parse_file(GPM_VAL, effect_group, &st);
    }

    std::string lst_file = file + ".lst";
    farc_file* lst_ff = farc->read_file(lst_file.c_str());
    if (lst_ff) {
        f2_struct st;
        st.read(lst_ff->data, lst_ff->size);
        if (st.header.signature == reverse_endianness_uint32_t('LIST') && effect_group)
            glitter_diva_list_parse_file(effect_group, &st);
    }
    return true;
}

bool GlitterFileReader::ReadFarc(GPM) {
    if (state)
        return false;

    bool ret = false;
    if (file_handler && !file_handler->check_not_ready()) {
        farc->read(file_handler->get_data(), file_handler->get_size(), true);
        ret = true;
        if (Read(GPM_VAL, obj_db) && init_scene) {
            state = 1;
            ret = false;
        }
    }
    return ret;
}
