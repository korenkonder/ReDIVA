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
        for (Mesh& i : meshes)
            if (i.object_set_hash != hash_murmurhash_empty) {
                objset_info_storage_unload_set(i.object_set_hash);
                i.load = false;
            }

        meshes.clear();

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

    void EffectGroup::GetStartEndFrame(int32_t* start_frame, int32_t* end_frame) {
        if (start_frame)
            *start_frame = 0;

        if (end_frame)
            *end_frame = 0;

        for (Effect*& i : effects) {
            if (!i)
                continue;

            Effect* effect = i;
            int32_t life_time = effect->data.life_time;
            if (start_frame && effect->data.appear_time < *start_frame)
                *start_frame = effect->data.appear_time;

            for (Emitter*& j : effect->emitters) {
                if (!j)
                    continue;

                Emitter* emitter = j;
                if (life_time < emitter->data.life_time)
                    life_time = emitter->data.life_time;
            }

            life_time += effect->data.appear_time;

            if (end_frame && life_time > *end_frame)
                *end_frame = life_time;
        }
    }

}
