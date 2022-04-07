/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_group.h"
#include "../../KKdLib/database/object.h"
#include "../../KKdLib/database/texture.h"
#include "../../KKdLib/io/path.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/farc.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/str_utils.h"
#include "../data.h"
#include "../object.h"
#include "../texture.h"
#include "effect.h"
#include "scene.h"
#include "texture.h"

GlitterEffectGroup::GlitterEffectGroup(GLT) : effects(), load_count(), hash(),
scene(), resources_count(), resources_tex(), resources(), not_loaded(), scene_init() {
    emission = 1.0f;
    version = GLT_VAL == GLITTER_X ? 0x0C : 0x09;
    type = GLT_VAL;

}

GlitterEffectGroup::~GlitterEffectGroup() {
#if defined(CRE_DEV)
    FreeModel();
#endif
    glitter_texture_unload(this);
    for (glitter_effect*& i : effects)
        delete i;
}

#if defined(CRE_DEV)
bool GlitterEffectGroup::CheckModel() {
    if (type != GLITTER_X)
        return false;

    bool has_model = false;
    for (glitter_effect*& i : effects) {
        if (!i)
            continue;

        glitter_effect* efct = i;
        for (glitter_emitter*& j : efct->emitters) {
            if (!j)
                continue;

            glitter_emitter* emit = j;
            for (glitter_particle*& k : emit->particles) {
                if (!k)
                    continue;

                glitter_particle* ptcl = k;
                if (ptcl->data.type != GLITTER_PARTICLE_MESH)
                    continue;

                has_model = true;
                uint32_t object_set_hash = (uint32_t)ptcl->data.mesh.object_set_name_hash;
                bool found = false;
                for (uint32_t& l : object_set_ids)
                    if (l == object_set_hash) {
                        found = true;
                        break;
                    }

                if (!found)
                    object_set_ids.push_back(object_set_hash);
            }
        }
    }
    return has_model;
}

bool GlitterEffectGroup::CheckLoadModel() {
    if (type != GLITTER_X)
        return false;

    bool ret = false;
    for (uint32_t& i : object_set_ids)
        ret |= object_storage_load_obj_set_check_not_read(i);

    if (!ret)
        for (uint32_t& i : object_set_ids) {
            obj_set* set = object_storage_get_obj_set(i);
            if (!set)
                continue;

            for (int32_t i = 0; i < set->objects_count; i++) {
                obj* obj = &set->objects[i];
                for (int32_t j = 0; j < obj->materials_count; j++) {
                    obj_material* mat = &obj->materials[j].material;
                    if (!memcmp(&mat->emission, &vec3_null, sizeof(vec3)) && mat->emission.w == 1.0f)
                        mat->emission = vec4u_identity;
                }
            }
        }
    return ret;
}

void GlitterEffectGroup::LoadModel(void* data) {
    if (type != GLITTER_X)
        return;

    for (uint32_t& i : object_set_ids)
        object_storage_load_set_hash(data, i);
}

void GlitterEffectGroup::FreeModel() {
    for (uint32_t& i : object_set_ids)
        object_storage_unload_set(i);
    object_set_ids.clear();
    object_set_ids.shrink_to_fit();
}
#endif

bool GlitterEffectGroup::ParseFile(f2_struct* st, object_database* obj_db) {
    for (f2_struct& i : st->sub_structs) {
        if (!i.header.data_size)
            continue;

        if (i.header.signature == reverse_endianness_uint32_t('EFCT')
            && !glitter_effect_parse_file(this, &i, &effects, obj_db))
            return false;
        else if (i.header.signature == reverse_endianness_uint32_t('DVRS')
            && !glitter_texture_hashes_unpack_file(this, &i))
            return false;
    }
    return true;
}

bool GlitterEffectGroup::UnparseFile(GLT, f2_struct* st) {
    for (glitter_effect*& i : effects) {
        if (!i)
            continue;

        f2_struct s;
        if (glitter_effect_unparse_file(GLT_VAL, this, &s, i))
            st->sub_structs.push_back(s);
    }

    f2_struct s;
    if (glitter_texture_hashes_pack_file(this, &s))
        st->sub_structs.push_back(s);

    st->header.signature = reverse_endianness_uint32_t('DVEF');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = version;
    return true;
}
