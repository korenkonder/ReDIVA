/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    ItemBase::ItemBase() {

    }

    ItemBase::~ItemBase() {

    }

    ItemBase& ItemBase::operator=(const ItemBase& item_base) {
        name.assign(item_base.name);
        animation = item_base.animation;
        return *this;
    }
}
