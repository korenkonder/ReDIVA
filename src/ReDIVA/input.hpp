/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include <GLFW/glfw3.h>

namespace Input {
    extern POINT pos;
    extern double_t scroll;
    extern POINT pos_prev;
    extern double_t scroll_prev;

    extern bool IsKeyDown(int32_t key, int32_t mods = 0);
    extern bool IsKeyUp(int32_t key, int32_t mods = 0);
    extern bool IsKeyTapped(int32_t key, int32_t mods = 0);
    extern bool IsKeyReleased(int32_t key, int32_t mods = 0);
    extern bool IsMouseButtonDown(int32_t button, int32_t mods = 0);
    extern bool IsMouseButtonUp(int32_t button, int32_t mods = 0);
    extern bool IsMouseButtonTapped(int32_t button, int32_t mods = 0);
    extern bool IsMouseButtonReleased(int32_t button, int32_t mods = 0);
    extern bool WasKeyDown(int32_t key, int32_t mods = 0);
    extern bool WasKeyUp(int32_t key, int32_t mods = 0);
    extern bool WasMouseButtonDown(int32_t button, int32_t mods = 0);
    extern bool WasMouseButtonUp(int32_t button, int32_t mods = 0);
    extern void EndFrame();
    extern void NewFrame();
    extern void SetInputs(GLFWwindow* window);
};
