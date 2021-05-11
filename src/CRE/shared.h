/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/str_utils.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "gl_object.h"
#include "hash.h"
#include "light.h"
#include "lock.h"
#include "shader.h"
#include "task.h"
#include "texture.h"
#include "vertex.h"

hash_ptr(gl_object)
hash_dat(light_dir)
hash_dat(light_point)
hash_dat(material)
hash_dat(shader_model)
hash_dat(texture)
hash_dat(texture_set)
hash_dat(texture_bone_mat)
hash_ptr(vertex)

vector(hash_ptr_gl_object)
vector(hash_light_dir)
vector(hash_light_point)
vector(hash_material)
vector(hash_shader_model)
vector(hash_texture)
vector(hash_texture_set)
vector(hash_texture_bone_mat)
vector(hash_ptr_vertex)
