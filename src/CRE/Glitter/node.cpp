/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Node::Node() : translation(), rotation() {
        scale = vec3_identity;
        scale_all = 1.0f;
    }

    Node::~Node() {

    }
}
