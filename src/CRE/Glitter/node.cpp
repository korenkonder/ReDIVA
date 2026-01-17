/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Node::Node() : translation(), rotation() {
        scale = 1.0f;
        uniform_scale = 1.0f;
    }

    Node::~Node() {

    }

    Node& Node::operator=(const Node& node) {
        ItemBase::operator=(node);
        translation = node.translation;
        rotation = node.rotation;
        scale = node.scale;
        uniform_scale = node.uniform_scale;
        return *this;
    }
}
