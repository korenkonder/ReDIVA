/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include <GLFW/glfw3.h>

namespace Input {
    bool IsKeyDown(int32_t key, int32_t mods = 0);
    bool IsKeyUp(int32_t key, int32_t mods = 0);
    bool IsKeyTapped(int32_t key, int32_t mods = 0);
    bool IsKeyReleased(int32_t key, int32_t mods = 0);
    bool IsMouseButtonDown(int32_t button, int32_t mods = 0);
    bool IsMouseButtonUp(int32_t button, int32_t mods = 0);
    bool IsMouseButtonTapped(int32_t button, int32_t mods = 0);
    bool IsMouseButtonReleased(int32_t button, int32_t mods = 0);
    bool WasKeyDown(int32_t key, int32_t mods = 0);
    bool WasKeyUp(int32_t key, int32_t mods = 0);
    bool WasMouseButtonDown(int32_t button, int32_t mods = 0);
    bool WasMouseButtonUp(int32_t button, int32_t mods = 0);
    void EndFrame();
    void NewFrame();
    void SetInputs(GLFWwindow* window);
};
