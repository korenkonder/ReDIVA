/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

struct GLFWwindow;

namespace Input {
    extern void NewFrame();
    extern void SetCallbacks(struct GLFWwindow* window);
};
