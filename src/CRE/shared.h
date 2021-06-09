/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/str_utils.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "bone_matrix.h"
#include "gl_object.h"
#include "hash.h"
#include "lock.h"
#include "shader.h"
#include "task.h"
#include "texture.h"
#include "vertex.h"

hash_dat(bone_matrix)
hash_dat(gl_object)
hash_dat(material)
hash_dat(shader_model)
hash_dat(texture)
hash_dat(texture_set)
hash_dat(vertex)

vector(hash_bone_matrix)
vector(hash_gl_object)
vector(hash_material)
vector(hash_shader_model)
vector(hash_texture)
vector(hash_texture_set)
vector(hash_vertex)
