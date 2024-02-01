/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Effect::Data::Data(GLT) : appear_time(), life_time(),
        start_time(), ext_anim(), flags(), emission(), seed(), unk() {
        color = 0xFFFFFFFF;
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
        Node::operator=(eff);
        if (data.ext_anim)
            free(data.ext_anim);

        data = eff.data;
        version = eff.version;

        if (eff.data.ext_anim)
            if (!data.ext_anim_is_x) {
                data.ext_anim = force_malloc<Effect::ExtAnim>();
                *data.ext_anim = *eff.data.ext_anim;
            }
            else {
                data.ext_anim_x = force_malloc<Effect::ExtAnimX>();
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
