/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "msgpack.h"

msgpack* io_msgpack_read(stream* s);
extern void io_msgpack_write(stream* s, msgpack* msg);
