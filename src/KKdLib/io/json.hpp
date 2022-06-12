/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../msgpack.hpp"
#include "stream.hpp"

extern void io_json_read(stream& s, msgpack* msg);
extern void io_json_write(stream& s, msgpack* msg);
