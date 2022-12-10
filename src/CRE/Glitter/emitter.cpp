/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Emitter::Box::Box() {

    }

    Emitter::Cylinder::Cylinder() : radius(), height(),
        start_angle(), end_angle(), on_edge(), direction() {

    }

    Emitter::Polygon::Polygon() : size(), count(), direction() {

    }

    Emitter::Sphere::Sphere() : radius(), latitude(), longitude(), on_edge(), direction() {

    }

    Emitter::Data::Data() : start_time(), life_time(),
        flags(), timer(), emission_interval(), type(), seed() {
        loop_start_time = 0;
        loop_end_time = -1;
        particles_per_emission = 1.0f;
        direction = DIRECTION_EFFECT_ROTATION;
    }

    Emitter::Emitter(GLT) {
        version = GLT_VAL == Glitter::X ? 0x04 : 0x02;
    }

    Emitter::~Emitter() {
        for (Particle*& i : particles)
            delete i;
    }

    Emitter& Emitter::operator=(const Emitter& emit) {
        data = emit.data;
        version = emit.version;

        for (Particle*& i : particles)
            delete i;
        particles.clear();

        particles.reserve(emit.particles.size());
        for (Particle* i : emit.particles) {
            Particle* ptcl = new Particle(i->version > 0x03 ? Glitter::X : Glitter::F2);
            *ptcl = *i;
            particles.push_back(ptcl);
        }
        return *this;
    }
}
