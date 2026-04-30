/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

extern bool init_startup();
extern bool ctrl_startup();
extern bool dest_startup();

extern bool init_data_initialize();
extern bool ctrl_data_initialize();
extern bool dest_data_initialize();

extern bool init_system_startup();
extern bool ctrl_system_startup();
extern bool dest_system_startup();

extern bool init_system_startup_error();
extern bool ctrl_system_startup_error();
extern bool dest_system_startup_error();

extern bool init_warning();
extern bool ctrl_warning();
extern bool dest_warning();
