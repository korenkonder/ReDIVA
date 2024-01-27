/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Counter::Counter() : value() {

    }

    void Counter::Increment() {
        value++;
    }

    uint32_t Counter::GetValue() {
        return value;
    }

    void Counter::Reset() {
        value = 0;
    }
}