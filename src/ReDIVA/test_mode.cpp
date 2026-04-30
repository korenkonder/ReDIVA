/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "test_mode.hpp"
#include "../CRE/sound.hpp"
#include "mask_screen.hpp"
#include "wait_screen.hpp"

static bool test_mode_start;

bool is_test_mode_start() {
    return test_mode_start;
}

void sys_am_set_test_mode_starting(bool value) {
    test_mode_start = value;
}
