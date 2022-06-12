/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../msgpack.hpp"
#include "stream.hpp"

extern void io_msgpack_read(stream& s, msgpack* msg);
extern void io_msgpack_write(stream& s, msgpack* msg);
