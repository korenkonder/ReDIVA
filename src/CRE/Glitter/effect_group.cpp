/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../object.hpp"
#include "../texture.hpp"

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
            texture_array_free(resources);
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

                for (uint32_t i = 0; i < set->obj_num; i++) {
                    obj* obj = set->obj_data[i];
                    for (uint32_t j = 0; j < obj->num_material; j++) {
                        obj_material* mat = &obj->material_array[j].material;
                        if (*(vec3*)&mat->color.emission == 0.0f && mat->color.emission.w == 1.0f)
                            mat->color.emission = 1.0f;
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
                    object_set_ids.push_back((uint32_t)k->data.mesh.object_set_name_hash);
                }
            }
        }

        prj::sort_unique(object_set_ids);
        return has_model;
    }

    void EffectGroup::FreeModel() {
        if (type != Glitter::X)
            return;

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
