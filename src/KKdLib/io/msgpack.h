/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../msgpack.h"
#include "stream.h"

extern void io_msgpack_read(stream* s, msgpack* msg);
extern void io_msgpack_write(stream* s, msgpack* msg);
