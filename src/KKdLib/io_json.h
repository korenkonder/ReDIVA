/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "msgpack.h"

msgpack* io_json_read(stream* s);
extern void io_json_write(stream* s, msgpack* msg);
