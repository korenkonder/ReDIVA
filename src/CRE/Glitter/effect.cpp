/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    object_info Effect::ExtAnim::GetObjectInfo(uint64_t hash, object_database* obj_db) {
        for (object_set_info& i : obj_db->object_set) {
            for (object_info_data& j : i.object)
                if (hash == j.name_hash_murmurhash || hash == j.name_hash_fnv1a64m_upper)
                    return { j.id, i.id };
        }
        return object_info();
    }

    Effect::Data::Data(GLT) : appear_time(), life_time(),
        start_time(), ext_anim(), flags(), emission(), seed() {
        color = { 0xFF, 0xFF, 0xFF, 0xFF };
        name_hash = GLT_VAL != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty;
        ext_anim_is_x = GLT_VAL == Glitter::X;
    }

    Effect::Effect(GLT) : data(GLT_VAL) {
        version = GLT_VAL == Glitter::X ? 0x0C : 0x07;
    }

    Effect::~Effect() {
        for (Emitter*& i : emitters)
            delete i;

        if (data.ext_anim)
            free(data.ext_anim);
    }

    Effect& Effect::operator=(const Effect& eff) {
        if (data.ext_anim)
            free(data.ext_anim);

        data = eff.data;
        version = eff.version;

        if (eff.data.ext_anim)
            if (!data.ext_anim_is_x) {
                data.ext_anim = force_malloc_s(Effect::ExtAnim, 1);
                *data.ext_anim = *eff.data.ext_anim;
            }
            else {
                data.ext_anim_x = force_malloc_s(Effect::ExtAnimX, 1);
                *data.ext_anim_x = *eff.data.ext_anim_x;
            }

        for (Emitter*& i : emitters)
            delete i;
        emitters.clear();

        emitters.reserve(eff.emitters.size());
        for (Emitter* i : eff.emitters) {
            Emitter* emit = new Emitter(i->version > 0x02 ? Glitter::X : Glitter::F2);
            *emit = *i;
            emitters.push_back(emit);
        }
        return *this;
    }
}
