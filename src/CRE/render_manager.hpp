/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include <glad/glad.h>
#include "render_context.hpp"

extern void image_filter_scale(render_context* rctx, GLuint dst, GLuint src, const vec4 scale);
