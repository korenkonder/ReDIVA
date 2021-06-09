/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "msgpack.h"

extern void io_msgpack_read(stream* s, msgpack* msg);
extern void io_msgpack_write(stream* s, msgpack* msg);
