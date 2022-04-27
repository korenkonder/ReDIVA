/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../object.h"
#include "../texture.h"

namespace Glitter {
    EffectGroup::EffectGroup(GLT) : effects(), load_count(), hash(), scene(),
        resources_count(), resources_tex(), resources(), not_loaded(), scene_init() {
        emission = 1.0f;
        version = GLT_VAL == Glitter::X ? 0x0C : 0x09;
        type = GLT_VAL;
    }

    EffectGroup::~EffectGroup() {
#if defined(CRE_DEV)
        FreeModel();
#endif

        if (resources) {
            for (texture** i = resources; *i; i++)
                texture_free(*i);
            free(resources);
            resources = 0;
        }

        for (Effect*& i : effects)
            if (i) {
                delete i;
                i = 0;
            }
        effects.clear();

        if (scene) {
            delete scene;
            scene = 0;
        }
    }

#if defined(CRE_DEV)
    bool EffectGroup::CheckLoadModel() {
        if (type != Glitter::X)
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

    bool EffectGroup::CheckModel() {
        if (type != Glitter::X)
            return false;

        bool has_model = false;
        for (Effect*& i : effects) {
            if (!i)
                continue;

            for (Emitter*& j : i->emitters) {
                if (!j)
                    continue;

                for (Particle*& k : j->particles) {
                    if (!k)
                        continue;

                    if (k->data.type != PARTICLE_MESH)
                        continue;

                    has_model = true;
                    uint32_t object_set_hash = (uint32_t)k->data.mesh.object_set_name_hash;
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

    void EffectGroup::FreeModel() {
        for (uint32_t& i : object_set_ids)
            object_storage_unload_set(i);
        object_set_ids.clear();
        object_set_ids.shrink_to_fit();
    }

    void EffectGroup::LoadModel(void* data) {
        if (type != Glitter::X)
            return;

        for (uint32_t& i : object_set_ids)
            object_storage_load_set_hash(data, i);
    }
#endif
}

/*
#include "effect_group.hpp"
#include "../../KKdLib/database/object.h"
#include "../../KKdLib/database/texture.h"
#include "../../KKdLib/io/path.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/farc.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/str_utils.h"
#include "../data.h"
#include "../object.h"
#include "effect.hpp"
#include "scene.hpp"
#include "texture.hpp"

bool EffectGroup::ParseFile(f2_struct* st, object_database* obj_db) {
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
*/