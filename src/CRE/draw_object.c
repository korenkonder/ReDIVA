/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_object.h"
#include "light_param/light.h"
#include "shader_ft.h"
#include "static_var.h"
#include "texture.h"

#define enable_vertex_attrib_array(index) \
glEnableVertexAttribArray(index); \
draw->vertex_attrib_array[index] = true

static bool draw_object_blend_set(render_context* rctx,
    draw_object* draw, object_material_shader_lighting_type lighting_type);
static void draw_object_chara_color_fog_set(render_context* rctx,
    draw_object* draw, bool disable_fog);
static void draw_object_material_reset_default(object_material_data* mat_data);
static void draw_object_material_reset_reflect();
static void draw_object_material_set_default(render_context* rctx,
    draw_object* draw, bool use_shader);
static void draw_object_material_set_parameter(render_context* rctx,
    object_material_data* mat_data);
static void draw_object_material_set_reflect(render_context* rctx, draw_object* draw);
static void draw_object_material_set_uniform(render_context* rctx,
    object_material_data* mat_data, bool disable_color_l);
static void draw_object_vertex_attrib_reset_default(draw_object* draw);
static void draw_object_vertex_attrib_reset_default_compressed(draw_object* draw);
static void draw_object_vertex_attrib_reset_reflect(draw_object* draw);
static void draw_object_vertex_attrib_reset_reflect_compressed(draw_object* draw);
static void draw_object_vertex_attrib_set_default(draw_object* draw);
static void draw_object_vertex_attrib_set_default_compressed(draw_object* draw);
static void draw_object_vertex_attrib_set_reflect(draw_object* draw);
static void draw_object_vertex_attrib_set_reflect_compressed(draw_object* draw);

void draw_object_draw(render_context* rctx, draw_object* draw, mat4* model,
    void(*draw_object_func)(render_context* rctx, draw_object* draw), int32_t show_vector) {
    if (draw->mats) {
        mat4u* mats = draw->mats;
        mat4 mat;
        for (int32_t i = 0; i < draw->mat_count; i++, mats++) {
            mat4u_to_mat4_transpose(mats, &mat);
            shader_buffer_set_ptr_array(&shaders_ft, (size_t)i * 3, 3, (vec4*)&mat);
        }
        uniform_value[U_BONE_MAT] = 1;
    }
    else {
        for (int32_t i = 0; i < SHADER_MAX_PROGRAM_BUFFER_PARAMETERS / 3; i++)
            shader_buffer_set_ptr_array(&shaders_ft, (size_t)i * 3, 3, (vec4*)&mat4_identity);
        uniform_value[U_BONE_MAT] = 0;
    }

    mat4 mat;
    if (draw->mesh->flags & OBJECT_MESH_BILLBOARD)
        model_mat_face_camera_view(&rctx->camera->view, model, &mat);
    else if (draw->mesh->flags & OBJECT_MESH_BILLBOARD_Y_AXIS)
        model_mat_face_camera_position(&rctx->camera->view, model, &mat);
    else
        mat = *model;
    draw_object_model_mat_load(rctx, &mat);

    if (!show_vector)
        draw_object_func(rctx, draw);
    uniform_value[U_BONE_MAT] = 0;
}

void draw_object_draw_default(render_context* rctx, draw_object* draw) {
    if (draw->set_blend_color) {
        vec4 blend_color;
        vec4u_to_vec4(draw->blend_color, blend_color);
        shader_env_vert_set_ptr(&shaders_ft, 3, &blend_color);
        shader_env_vert_set_ptr(&shaders_ft, 4, (vec4*)&vec4_null);
    }

    if (!draw->draw_object_func)
        draw_object_vertex_attrib_set_default(draw);

    draw_object_material_set_default(rctx, draw, rctx->draw_state.shader);
    if (!draw->instances_count)
        object_sub_mesh_draw(rctx,
            draw->sub_mesh->primitive_type,
            draw->sub_mesh->indices_count,
            draw->sub_mesh->first_index,
            draw->sub_mesh->last_index,
            draw->sub_mesh->index_format,
            draw->sub_mesh->indices_offset);
    else
        for (int32_t i = 0; i < draw->instances_count; i++) {
            mat4 mat;
            mat4_transpose(&draw->instances_mat[i], &mat);
            glVertexAttrib4fv(12, (const GLfloat*)&mat.row0);
            glVertexAttrib4fv(13, (const GLfloat*)&mat.row1);
            glVertexAttrib4fv(14, (const GLfloat*)&mat.row2);
            glVertexAttrib4fv(15, (const GLfloat*)&mat.row3);
            object_sub_mesh_draw(rctx,
                draw->sub_mesh->primitive_type,
                draw->sub_mesh->indices_count,
                draw->sub_mesh->first_index,
                draw->sub_mesh->last_index,
                draw->sub_mesh->index_format,
                draw->sub_mesh->indices_offset);
        }

    draw_object_material_reset_default(draw->material);
    if (!draw->draw_object_func)
        draw_object_vertex_attrib_reset_default(draw);

    if (draw->set_blend_color) {
        shader_env_vert_set_ptr(&shaders_ft, 3, (vec4*)&vec4_identity);
        shader_env_vert_set_ptr(&shaders_ft, 4, (vec4*)&vec4_null);
    }

    rctx->draw_state.stats.object_draw_count++;
}

void draw_object_draw_sss(render_context* rctx, draw_object* draw) {
    object_material_blend_flags blend_flags = draw->material->material.blend_flags;
    uniform_value[U_ALPHA_TEST] = (!blend_flags.flag_28 && (draw->blend_color.w < 1.0f
            || (blend_flags.alpha_texture || blend_flags.alpha_material) && !blend_flags.punch_through
            || draw->sub_mesh->flags & OBJECT_SUB_MESH_TRANSPARENT)
            || blend_flags.punch_through) ? 1 : 0;

    bool aniso = false;
    object_material_data* material = draw->material;
    switch (material->material.shader_index) {
    case SHADER_FT_SKIN:
        uniform_value[U26] = 1;
        break;
    case SHADER_FT_CLOTH:
        uniform_value[U26] = 1;
        if (rctx->draw_pass.npr_param || material->material.ambient.w >= 1.0f
            || (material->material.shader_flags.aniso_direction & 0x03) != OBJECT_MATERIAL_ANISO_DIRECTION_NORMAL)
            aniso = true;
        break;
    case SHADER_FT_TIGHTS:
        uniform_value[U26] = 1;
        if (rctx->draw_pass.npr_param)
            aniso = true;
        break;
    case SHADER_FT_EYEBALL:
        uniform_value[U26] = 0;
        return;
    }

    vec3 sss_param;
    if (!aniso) {
        uniform_value[U37] = 1;
        sss_param = *(vec3*)&rctx->draw_pass.sss_data.param;
    }
    else {
        uniform_value[U37] = 0;
        sss_param = vec3_null;
    }

    shader_env_frag_set(&shaders_ft, 25, sss_param.x, sss_param.y, sss_param.z, 0.5f);
    draw_object_draw_default(rctx, draw);
}

void draw_object_draw_reflect(render_context* rctx, draw_object* draw) {
    object_material_data* material = draw->material;
    draw_object_vertex_attrib_set_reflect(draw);
    object_material_shader_lighting_type lighting_type =
        object_material_shader_get_lighting_type(&material->material.shader_flags);
    bool disable_fog = draw_object_blend_set(rctx, draw, lighting_type);
    draw_object_chara_color_fog_set(rctx, draw, disable_fog);
    draw_object_material_set_reflect(rctx, draw);

    object_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJECT_INDEX_U8)
        object_sub_mesh_draw(rctx,
            sub_mesh->primitive_type,
            sub_mesh->indices_count,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->indices_offset);

    draw_object_material_reset_reflect();
    draw_object_vertex_attrib_reset_reflect(draw);
    uniform_value_reset();

    rctx->draw_state.stats.object_reflect_draw_count++;
}

void draw_object_draw_reflect_type_2(render_context* rctx, draw_object* draw) {
    draw_object_vertex_attrib_set_reflect(draw);
    draw_object_material_set_reflect(rctx, draw);

    object_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJECT_INDEX_U8)
        object_sub_mesh_draw(rctx,
            sub_mesh->primitive_type,
            sub_mesh->indices_count,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->indices_offset);

    draw_object_material_reset_reflect();
    draw_object_vertex_attrib_reset_reflect(draw);

    rctx->draw_state.stats.object_reflect_draw_count++;
}

void draw_object_draw_shadow(render_context* rctx, draw_object* draw) {
    object_material_blend_flags blend_flags = draw->material->material.blend_flags;
    if (!blend_flags.flag_28 && (draw->blend_color.w < 1.0f
        || (blend_flags.alpha_texture || blend_flags.alpha_material) && !blend_flags.punch_through
        || draw->sub_mesh->flags & OBJECT_SUB_MESH_TRANSPARENT)
        || blend_flags.punch_through) {
        uniform_value[U_ALPHA_TEST] = 1;
        draw_object_draw_translucent(rctx, draw);
    }
    else {
        uniform_value[U_ALPHA_TEST] = 0;
        draw_object_draw_translucent(rctx, draw);
    }
}

void draw_object_draw_translucent(render_context* rctx, draw_object* draw) {
    object_material_data* material = draw->material;
    GLuint* textures = draw->textures;
    if (rctx->draw_state.shader_index != -1) {
        vec4 emission;
        vec4u_to_vec4(material->material.emission, emission);
        shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);
        draw_object_material_set_uniform(rctx, material, 0);
        if (material->material.blend_flags.alpha_texture)
            uniform_value[U_TEXTURE_COUNT] = 0;
        shader_set(&shaders_ft, rctx->draw_state.shader_index);
    }

    draw_object_vertex_attrib_set_default(draw);
    if (draw->material->material.blend_flags.double_sided)
        gl_state_disable_cull_face();

    GLuint tex_id = -1;
    int32_t tex_index = 0;
    if (material->material.blend_flags.alpha_texture) {
        object_material_texture* mat_tex = material->material.textures;
        uint32_t texture_id = -1;
        uint32_t texture_index = -1;
        if (~material->material.flags & OBJECT_MATERIAL_TRANSPARENCY) {
            for (int32_t i = 0; i < 8; i++, mat_tex++)
                if (mat_tex->texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR) {
                    texture_id = mat_tex->texture_id;
                    texture_index = mat_tex->texture_index;
                    break;
                }
            tex_index = 0;
        }
        else {
            for (int32_t i = 0; i < 8; i++, mat_tex++)
                if (mat_tex->texture_flags.type == OBJECT_MATERIAL_TEXTURE_TRANSPARENCY) {
                    if (mat_tex->sampler_flags.flag_29) {
                        texture_id = mat_tex->texture_id;
                        texture_index = mat_tex->texture_index;
                    }
                    break;
                }
            tex_index = 4;
        }

        if (texture_id != -1) {
            for (int32_t j = 0; j < draw->texture_pattern_count; j++)
                if (draw->texture_pattern_array[j].src == texture_id) {
                    texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                    if (tex)
                        tex_id = tex->texture;
                    break;
                }

            if (tex_id == -1)
                tex_id = textures[mat_tex->texture_index];

            if (tex_id == -1)
                tex_id = 0;

            gl_state_active_bind_texture_2d(tex_index, tex_id);
        }
    }

    object_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJECT_INDEX_U8)
        object_sub_mesh_draw(rctx,
            sub_mesh->primitive_type,
            sub_mesh->indices_count,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->indices_offset);

    if (tex_id != -1)
        gl_state_active_bind_texture_2d(tex_index, 0);

    gl_state_enable_cull_face();
    draw_object_vertex_attrib_reset_default(draw);

    if (rctx->draw_state.shader_index != -1)
        uniform_value_reset();

    rctx->draw_state.stats.object_translucent_draw_count++;
}

inline void draw_object_model_mat_load(render_context* rctx, mat4* mat) {
    shader_state_matrix_set_modelview_separate(&shaders_ft, 0, mat, &rctx->view_mat, true);
    shader_state_matrix_set_program(&shaders_ft, 6, mat);
}

inline void model_mat_face_camera_position(mat4* view, mat4* src, mat4* dst) {
    vec3 trans;
    mat4_mult_vec3_inv(view, (vec3*)&view->row3, &trans);
    vec3_negate(trans, trans);

    vec3 dir;
    vec3_sub(trans, *(vec3*)&src->row3, dir);
    vec3_normalize(dir, dir);

    vec3 x_rot;
    vec3 y_rot;
    vec3 z_rot;

    y_rot = *(vec3*)&src->row1;
    vec3_cross(y_rot, dir, x_rot);
    vec3_normalize(x_rot, x_rot);
    vec3_cross(x_rot, y_rot, z_rot);

    *(vec3*)&dst->row0 = x_rot;
    *(vec3*)&dst->row1 = y_rot;
    *(vec3*)&dst->row2 = z_rot;
    *(vec3*)&dst->row3 = *(vec3*)&src->row3;
    dst->row0.w = 0.0f;
    dst->row1.w = 0.0f;
    dst->row2.w = 0.0f;
    dst->row3.w = 1.0f;
}

inline void model_mat_face_camera_view(mat4* view, mat4* src, mat4* dst) {
    mat3 mat;
    mat3_from_mat4(view, &mat);
    mat3_inverse(&mat, &mat);
    mat4_from_mat3(&mat, dst);
    mat4_mult(dst, src, dst);
}

void object_sub_mesh_draw(render_context* rctx, object_primitive_type primitive_type,
    uint32_t count, uint16_t start, uint16_t end, object_index_format index_format, size_t indices) {
    GLenum mesh_draw_mode[] = {
        [OBJECT_PRIMITIVE_POINTS        ] = GL_ZERO, //GL_POINTS,
        [OBJECT_PRIMITIVE_LINES         ] = GL_LINES,
        [OBJECT_PRIMITIVE_LINE_STRIP    ] = GL_LINE_STRIP,
        [OBJECT_PRIMITIVE_LINE_LOOP     ] = GL_LINE_LOOP,
        [OBJECT_PRIMITIVE_TRIANGLES     ] = GL_TRIANGLES,
        [OBJECT_PRIMITIVE_TRIANGLE_STRIP] = GL_TRIANGLE_STRIP,
        [OBJECT_PRIMITIVE_TRIANGLE_FAN  ] = GL_TRIANGLE_FAN,
        [OBJECT_PRIMITIVE_QUADS         ] = GL_ZERO, //GL_QUADS,
        [OBJECT_PRIMITIVE_QUAD_STRIP    ] = GL_ZERO, //GL_QUAD_STRIP,
        [OBJECT_PRIMITIVE_POLYGON       ] = GL_ZERO, //GL_POLYGON,
    };

    GLenum mesh_indices_type[] = {
        [OBJECT_INDEX_U8 ] = GL_ZERO,
        [OBJECT_INDEX_U16] = GL_UNSIGNED_SHORT,
        [OBJECT_INDEX_U32] = GL_UNSIGNED_INT,
    };

    if (primitive_type == OBJECT_PRIMITIVE_TRIANGLE_STRIP && index_format == OBJECT_INDEX_U16) {
        gl_state_enable_primitive_restart();
        glPrimitiveRestartIndex(0xFFFF);
        shader_draw_range_elements(&shaders_ft, GL_TRIANGLE_STRIP,
            start, end, count, GL_UNSIGNED_SHORT, (void*)indices);
        gl_state_disable_primitive_restart();
    }
    else
        shader_draw_elements(&shaders_ft, mesh_draw_mode[primitive_type],
            count, mesh_indices_type[index_format], (void*)indices);

    rctx->draw_state.stats.draw_count += count;
    if (primitive_type == OBJECT_PRIMITIVE_TRIANGLES)
        rctx->draw_state.stats.draw_triangle_count += count / 3;
    else if (primitive_type == OBJECT_PRIMITIVE_TRIANGLE_STRIP)
        rctx->draw_state.stats.draw_triangle_count += count - 2;
}

static bool draw_object_blend_set(render_context* rctx, draw_object* draw, object_material_shader_lighting_type lighting_type) {
    GLenum blend_factor_table[] = {
        [OBJECT_MATERIAL_BLEND_ZERO]              = GL_ZERO,
        [OBJECT_MATERIAL_BLEND_ONE]               = GL_ONE,
        [OBJECT_MATERIAL_BLEND_SRC_COLOR]         = GL_SRC_COLOR,
        [OBJECT_MATERIAL_BLEND_INVERSE_SRC_COLOR] = GL_ONE_MINUS_SRC_COLOR,
        [OBJECT_MATERIAL_BLEND_SRC_ALPHA]         = GL_SRC_ALPHA,
        [OBJECT_MATERIAL_BLEND_INVERSE_SRC_ALPHA] = GL_ONE_MINUS_SRC_ALPHA,
        [OBJECT_MATERIAL_BLEND_DST_ALPHA]         = GL_DST_ALPHA,
        [OBJECT_MATERIAL_BLEND_INVERSE_DST_ALPHA] = GL_ONE_MINUS_DST_ALPHA,
        [OBJECT_MATERIAL_BLEND_DST_COLOR]         = GL_DST_COLOR,
        [OBJECT_MATERIAL_BLEND_INVERSE_DST_COLOR] = GL_ONE_MINUS_DST_COLOR,
        [OBJECT_MATERIAL_BLEND_ALPHA_SATURATE]    = GL_SRC_ALPHA_SATURATE,
        [11]                                      = GL_ZERO,
        [12]                                      = GL_ZERO,
        [13]                                      = GL_ZERO,
        [14]                                      = GL_ZERO,
        [15]                                      = GL_ZERO,
    };

    object_material_data* material = draw->material;
    object_material_blend_flags blend_flags = material->material.blend_flags;
    if ((!blend_flags.alpha_texture && !blend_flags.alpha_material) || blend_flags.punch_through)
        return false;

    GLenum src_blend_factor = blend_factor_table[blend_flags.src_blend_factor & 0xF];
    GLenum dst_blend_factor = blend_factor_table[blend_flags.dst_blend_factor & 0xF];
    if (draw->chara_color) {
        light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
        light_data* chara_color = &set->lights[LIGHT_CHARA_COLOR];
        vec4 specular;
        light_get_specular(chara_color, &specular);
        if (specular.w >= 4.0f && light_get_type(chara_color) == LIGHT_PARALLEL
            && (src_blend_factor != GL_ONE || dst_blend_factor)) {
            int32_t shader_index = rctx->draw_state.shader_index;
            if (rctx->draw_state.shader_index == -1) {
                shader_index = material->material.shader_index;
                if (shader_index == SHADER_FT_BLINN) {
                    if (lighting_type == OBJECT_MATERIAL_SHADER_LIGHTING_CONSTANT)
                        shader_index = SHADER_FT_CONSTANT;
                    if (lighting_type == OBJECT_MATERIAL_SHADER_LIGHTING_LAMBERT)
                        shader_index = SHADER_FT_LAMBERT;
                }
            }

            switch (shader_index) {
            case SHADER_FT_ITEM:
            case SHADER_FT_SKIN:
            case SHADER_FT_HAIR:
            case SHADER_FT_CLOTH:
            case SHADER_FT_TIGHTS:
            case SHADER_FT_GLASEYE:
                src_blend_factor = GL_SRC_ALPHA;
                dst_blend_factor = GL_ONE_MINUS_SRC_ALPHA;
                break;
            }
        }
    }
    gl_state_set_blend_func(src_blend_factor, dst_blend_factor);
    return dst_blend_factor == GL_ONE;
}

static void draw_object_chara_color_fog_set(render_context* rctx, draw_object* draw, bool disable_fog) {
    uniform_value[U_CHARA_COLOR] = 0;
    if (draw->chara_color) {
        light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
        light_data* chara_color = &set->lights[LIGHT_CHARA_COLOR];
        vec4 specular;
        light_get_specular(chara_color, &specular);
        if (specular.w > 0.0f && light_get_type(chara_color) == LIGHT_PARALLEL)
            uniform_value[U_CHARA_COLOR] = 1;
    }

    object_material_blend_flags blend_flags = draw->material->material.blend_flags;
    if (!blend_flags.no_fog && !disable_fog) {
        if (blend_flags.has_fog_height)
            uniform_value[U_FOG_HEIGHT] = 2 + blend_flags.fog_height;
        else
            uniform_value[U_FOG_HEIGHT] = 1;
        uniform_value[U_FOG] = rctx->draw_state.fog_height ? 2 : 1;
    }
}

static void draw_object_material_reset_default(object_material_data* mat_data) {
    if (mat_data) {
        gl_state_enable_cull_face();
        object_material_blend_flags blend_flags = mat_data->material.blend_flags;
        if ((blend_flags.alpha_texture || blend_flags.alpha_material) && !blend_flags.punch_through)
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    uniform_value_reset();
}

static void draw_object_material_reset_reflect() {
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_enable_cull_face();
    uniform_value_reset();
}

static void draw_object_material_set_default(render_context* rctx, draw_object* draw, bool use_shader) {
    GLuint* textures = draw->textures;
    object_material_data* material = draw->material;
    object_material_shader_lighting_type lighting_type =
        object_material_shader_get_lighting_type(&material->material.shader_flags);
    bool disable_fog = draw_object_blend_set(rctx, draw, lighting_type);
    draw_object_material_set_uniform(rctx, material, false);
    if (!rctx->draw_state.light)
        uniform_value[U_LIGHT_0] = 0;
    else if (draw->self_shadow)
        uniform_value[U_LIGHT_0] = draw->self_shadow ? 1 : 1;
    else
        uniform_value[U_LIGHT_0] = draw->sub_mesh->flags & OBJECT_SUB_MESH_RECIEVE_SHADOW ? 1 : 0;
    uniform_value[U_SHADOW] = 0;
    uniform_value[U_SELF_SHADOW] = rctx->draw_state.self_shadow ? 1 : 0;

    object_material_texture* mat_tex = material->material.textures;
    uniform_value[U_SHADOW] = draw->shadow > SHADOW_CHARA;
    for (int32_t i = 0, j = 0; i < 8; i++, mat_tex++) {
        if (mat_tex->texture_id == -1)
            continue;

        GLuint tex_id = -1;
        uint32_t texture_id = mat_tex->texture_id;
        for (int32_t j = 0; j < draw->texture_pattern_count; j++)
            if (draw->texture_pattern_array[j].src == texture_id) {
                texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                if (tex)
                    tex_id = tex->texture;
                break;
            }

        if (tex_id == -1)
            tex_id = textures[mat_tex->texture_index];

        if (tex_id == -1)
            continue;

        int32_t tex_index = object_material_texture_type_get_texture_index(mat_tex->texture_flags.type, j);
        if (tex_index < 0)
            continue;

        object_material_texture_type tex_type = mat_tex->texture_flags.type;
        if (tex_type == OBJECT_MATERIAL_TEXTURE_COLOR)
            j++;

        if (mat_tex->sampler_flags.flag_29) {
            switch (tex_type) {
            case OBJECT_MATERIAL_TEXTURE_NORMAL:
                uniform_value[U_NORMAL] = 0;
                break;
            case OBJECT_MATERIAL_TEXTURE_SPECULAR:
                uniform_value[U_SPECULAR] = 0;
                break;
            case OBJECT_MATERIAL_TEXTURE_TRANSLUCENCY:
                uniform_value[U_TRANSLUCENCY] = 0;
                break;
            case OBJECT_MATERIAL_TEXTURE_TRANSPARENCY:
                uniform_value[U_TRANSPARENCY] = 0;
                break;
            }
            continue;
        }

        if (tex_type == OBJECT_MATERIAL_TEXTURE_ENVIRONMENT_CUBE)
            gl_state_active_bind_texture_cube_map(tex_index, tex_id);
        else {
            gl_state_active_bind_texture_2d(tex_index, tex_id);

            GLenum wrap_s = 0;
            if (mat_tex->sampler_flags.mirror_u)
                wrap_s = GL_MIRRORED_REPEAT;
            else if (mat_tex->sampler_flags.repeat_u)
                wrap_s = GL_REPEAT;

            GLenum wrap_t = 0;
            if (mat_tex->sampler_flags.mirror_v)
                wrap_t = GL_MIRRORED_REPEAT;
            else if (mat_tex->sampler_flags.repeat_v)
                wrap_t = GL_REPEAT;

            if (wrap_s)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
            if (wrap_t)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
        }

        if (material->material.shader_index == SHADER_FT_SKY) {
            uniform_name uni_type = U_TEX_0_TYPE;
            if (tex_index == 1)
                uni_type = U_TEX_1_TYPE;

            texture* tex = texture_storage_get_texture(mat_tex->texture_id);
            if (!tex)
                uniform_value[uni_type] = 1;
            else if (tex->internal_format == GL_COMPRESSED_RED_RGTC1_EXT)
                uniform_value[uni_type] = 3;
            else if (tex->internal_format == GL_COMPRESSED_RED_GREEN_RGTC2_EXT)
                uniform_value[uni_type] = 2;
            else
                uniform_value[uni_type] = 1;
        }

        int32_t texture_blend = object_material_texture_get_blend(mat_tex);
        if (tex_index == 0 || tex_index == 1)
            uniform_value[U_TEXTURE_BLEND] = texture_blend;
    }

    if (material->material.blend_flags.double_sided) {
        gl_state_disable_cull_face();
        if (!material->material.blend_flags.normal_direction_light)
            uniform_value[U0B] = 1;
    }

    vec4 ambient;
    *(vec3*)&ambient = *(vec3*)&material->material.ambient;
    if (rctx->draw_pass.sss_data.enable)
        ambient.w = material->material.ambient.w;
    else
        ambient.w = 1.0f;

    vec4 diffuse;
    vec4 emission;
    vec4u_to_vec4(material->material.diffuse, diffuse);
    vec4u_to_vec4(material->material.emission, emission);
    draw_object_chara_color_fog_set(rctx, draw, disable_fog);
    shader_state_material_set_ambient_ptr(&shaders_ft, false, &ambient);
    shader_state_material_set_diffuse_ptr(&shaders_ft, false, &diffuse);
    shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);

    float_t line_light;
    if (lighting_type == OBJECT_MATERIAL_SHADER_LIGHTING_PHONG) {
        vec4 specular;
        vec4u_to_vec4(material->material.specular, specular);
        shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);

        float_t luma;
        vec3_dot(*(vec3*)&specular, ((vec3) {0.30f, 0.59f, 0.11f }), luma);
        if (luma >= 0.0099999998f || draw->texture_color_coeff.w >= 0.1f)
            uniform_value[U_SPECULAR_IBL] = 1;
        else
            uniform_value[U_SPECULAR_IBL] = 0;

        if (!material->material.shader_flags.fresnel)
            uniform_value[U_TRANSLUCENCY] = 0;

        line_light = (float_t)material->material.shader_flags.line_light * (float_t)(1.0 / 9.0);
    }
    else
        line_light = 0.0;

    if (!use_shader)
        shader_set(&shaders_ft, SHADER_FT_SIMPLE);
    else if (rctx->draw_state.shader_index != -1)
        shader_set(&shaders_ft, rctx->draw_state.shader_index);
    else if (material->material.shader_index != -1) {
        if (material->material.shader_index != SHADER_FT_BLINN)
            shader_set(&shaders_ft, material->material.shader_index);
        else if (lighting_type == OBJECT_MATERIAL_SHADER_LIGHTING_LAMBERT)
            shader_set(&shaders_ft, SHADER_FT_LAMBERT);
        else if (lighting_type == OBJECT_MATERIAL_SHADER_LIGHTING_PHONG)
            shader_set(&shaders_ft, SHADER_FT_BLINN);
        else
            shader_set(&shaders_ft, SHADER_FT_CONSTANT);
    }
    else
        shader_set(&shaders_ft, SHADER_FT_CONSTANT);

    if (lighting_type != OBJECT_MATERIAL_SHADER_LIGHTING_CONSTANT) {
        float_t shininess;
        if (material->material.shader_index == SHADER_FT_EYEBALL)
            shininess = 10.0f;
        else {
            shininess = (material->material.shininess - 16.0f) * (float_t)(1.0 / 112.0);
            shininess = max(shininess, 0.0f);
        }
        shader_state_material_set_shininess(&shaders_ft, false, shininess, 0.0f, 0.0f, 1.0f);

        float_t fresnel = (float_t)material->material.shader_flags.fresnel;
        if (fresnel > 9.0)
            fresnel = 9.0;
        else if (fresnel == 0.0f)
            fresnel = rctx->draw_state.fresnel;
        fresnel = (fresnel - 1.0f) * (0.12f * 0.82f);

        shader_local_vert_set(&shaders_ft, 0, fresnel, 0.18f, line_light, 0.0f);
        shader_local_frag_set(&shaders_ft, 9, fresnel, 0.18f, line_light, 0.0f);

        shininess = max(material->material.shininess, 1.0f);
        shader_local_vert_set(&shaders_ft, 1, shininess, 0.0f, 0.0f, 0.0f);
        shader_local_frag_set(&shaders_ft, 3, shininess, 0.0f, 0.0f, 0.0f);

        switch (material->material.shader_index) {
        case SHADER_FT_SKIN: {
            vec4 texture_color_coeff;
            vec4 texture_color_offset;
            vec4 texture_specular_coeff;
            vec4 texture_specular_offset;
            vec4u_to_vec4(draw->texture_color_coeff, texture_color_coeff);
            vec4u_to_vec4(draw->texture_color_offset, texture_color_offset);
            vec4u_to_vec4(draw->texture_specular_coeff, texture_specular_coeff);
            vec4u_to_vec4(draw->texture_specular_offset, texture_specular_offset);

            texture_color_coeff.w *= 0.015f;
            texture_specular_coeff.w *= 0.015f;

            shader_local_frag_set_ptr(&shaders_ft, 5, &texture_color_coeff);
            shader_local_frag_set_ptr(&shaders_ft, 6, &texture_color_offset);
            shader_local_frag_set_ptr(&shaders_ft, 7, &texture_specular_coeff);
            shader_local_frag_set_ptr(&shaders_ft, 8, &texture_specular_offset);
        } break;
        case SHADER_FT_HAIR:
        case SHADER_FT_CLOTH:
        case SHADER_FT_TIGHTS:
            shader_local_frag_set(&shaders_ft, 5, 1.0f - draw->texture_color_coeff.w * 0.4f,
                0.0f, 0.0f, draw->texture_color_coeff.w * 0.02f);
            break;
        }
    }

    draw_object_material_set_parameter(rctx, material);
}

static void draw_object_material_set_parameter(render_context* rctx, object_material_data* mat_data) {
    float_t bump_depth;
    float_t intensity;
    float_t reflect_uv_scale;
    float_t refract_uv_scale;
    float_t inv_bump_depth;
    if (rctx->draw_state.use_global_material) {
        bump_depth = rctx->draw_state.bump_depth;
        intensity = rctx->draw_state.intensity;
        reflect_uv_scale = rctx->draw_state.reflect_uv_scale;
        refract_uv_scale = rctx->draw_state.refract_uv_scale;
        inv_bump_depth = (1.0f - rctx->draw_state.bump_depth) * 64.0f + 1.0f;

        vec4 specular;
        vec4_to_vec4u(mat_data->material.specular, specular);
        specular.w = rctx->draw_state.specular_alpha;
        shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);
    }
    else {
        bump_depth = mat_data->material.bump_depth;
        reflect_uv_scale = 0.1f;
        intensity = mat_data->material.intensity;
        refract_uv_scale = 0.1f;
        inv_bump_depth = (1.0f - bump_depth) * 256.0f + 1.0f;
    }

    intensity = max(intensity, 1.0f);

    shader_local_vert_set(&shaders_ft, 2, inv_bump_depth, bump_depth, 0.0f, 0.0f);
    shader_local_frag_set(&shaders_ft, 0, inv_bump_depth, bump_depth, 0.0f, 0.0f);

    shader_local_frag_set(&shaders_ft, 1, intensity, intensity, intensity * 25.5f, 1.0f);
    shader_local_frag_set(&shaders_ft, 2,
        reflect_uv_scale, reflect_uv_scale, refract_uv_scale, refract_uv_scale);

    vec4 specular;
    shader_env_vert_get_ptr(&shaders_ft, 17, &specular);
    vec3_mult(*(vec3*)&specular, *(vec3*)&mat_data->material.specular, *(vec3*)&specular);
    shader_env_vert_set_ptr(&shaders_ft, 18, &specular);
}

static void draw_object_material_set_reflect(render_context* rctx, draw_object* draw) {
    object_material_data* material = draw->material;
    GLuint* textures = draw->textures;
    if (material->material.blend_flags.double_sided)
        gl_state_disable_cull_face();

    object_material_texture* mat_tex = material->material.textures;
    for (int32_t i = 0; i < 1; i++, mat_tex++) {
        uint32_t texture_id = mat_tex->texture_id;
        if (mat_tex->texture_id == -1)
            break;

        GLuint tex_id = -1;
        for (int32_t j = 0; j < draw->texture_pattern_count; j++)
            if (draw->texture_pattern_array[j].src == texture_id) {
                texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                if (tex)
                    tex_id = tex->texture;
                break;
            }

        if (tex_id == -1)
            tex_id = textures[mat_tex->texture_index];

        if (tex_id == -1)
            tex_id = 0;

        gl_state_active_bind_texture_2d(i, tex_id);

        GLenum wrap_s = 0;
        if (mat_tex->sampler_flags.mirror_u)
            wrap_s = GL_MIRRORED_REPEAT;
        else if (mat_tex->sampler_flags.repeat_u)
            wrap_s = GL_REPEAT;

        GLenum wrap_t = 0;
        if (mat_tex->sampler_flags.mirror_v)
            wrap_t = GL_MIRRORED_REPEAT;
        else if (mat_tex->sampler_flags.repeat_v)
            wrap_t = GL_REPEAT;

        if (wrap_s)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        if (wrap_t)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    }

    vec4 ambient;
    vec4 diffuse;
    vec4 emission;
    vec4 specular;
    if (material->material.shader_index == -1) {
        ambient = vec4_identity;
        vec4u_to_vec4(material->material.diffuse, diffuse);
        emission = vec4_identity;
        vec4u_to_vec4(material->material.specular, specular);
    }
    else {
        vec4u_to_vec4(material->material.ambient, ambient);
        vec4u_to_vec4(material->material.diffuse, diffuse);
        vec4u_to_vec4(material->material.emission, emission);
        vec4u_to_vec4(material->material.specular, specular);
    }

    draw_object_material_set_uniform(rctx, material, true);
    shader_set(&shaders_ft, rctx->draw_state.shader_index);
    shader_state_material_set_ambient_ptr(&shaders_ft, false, &ambient);
    shader_state_material_set_diffuse_ptr(&shaders_ft, false, &diffuse);
    shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);
    shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);
    draw_object_material_set_parameter(rctx, draw->material);
}

static void draw_object_material_set_uniform(render_context* rctx, object_material_data* mat_data, bool disable_color_l) {
    object_material_flags flags = mat_data->material.flags;
    object_material_shader_flags shader_flags = mat_data->material.shader_flags;

    if (disable_color_l) {
        if (flags & OBJECT_MATERIAL_COLOR)
            flags &= ~OBJECT_MATERIAL_COLOR_L1;
        flags &= ~OBJECT_MATERIAL_COLOR_L2;
    }

    int32_t v4 = 0;
    if (flags & OBJECT_MATERIAL_COLOR) {
        if (~flags & OBJECT_MATERIAL_COLOR_L2)
            if (flags & OBJECT_MATERIAL_COLOR_L1) {
                uniform_value[U_TEXTURE_COUNT] = 2;
                v4 = 3;
            }
            else {
                uniform_value[U_TEXTURE_COUNT] = 1;
                v4 = 1;
            }
    }
    else if (flags & OBJECT_MATERIAL_COLOR_L1 && ~flags & OBJECT_MATERIAL_COLOR_L2) {
        uniform_value[U_TEXTURE_COUNT] = 1;
        v4 = 1;
    }
    else
        uniform_value[U_TEXTURE_COUNT] = 0;

    if (flags & OBJECT_MATERIAL_NORMAL)
        uniform_value[U_NORMAL] = 1;
    if (flags & OBJECT_MATERIAL_SPECULAR)
        uniform_value[U_SPECULAR] = 1;
    if (flags & OBJECT_MATERIAL_TRANSPARENCY) {
        uniform_value[U_TRANSPARENCY] = 1;
        v4 |= 2;
    }
    if (flags & OBJECT_MATERIAL_TRANSLUCENCY) {
        uniform_value[U_TRANSLUCENCY] = 1;
        v4 |= 2;
    }
    if (flags & OBJECT_MATERIAL_OVERRIDE_IBL)
        uniform_value[U_ENV_MAP] = 1;

    if ((shader_flags.aniso_direction & 0x03) != OBJECT_MATERIAL_ANISO_DIRECTION_NORMAL)
        uniform_value[U_ANISO] = shader_flags.aniso_direction & 0x03;

    if (v4 == 1)
        uniform_value[U45] = 0;
    else if (v4 == 2 || v4 == 3)
        uniform_value[U45] = 1;
}

static void draw_object_vertex_attrib_reset_default(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    if (mesh->compressed) {
        draw_object_vertex_attrib_reset_default_compressed(draw);
        return;
    }

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }
    uniform_value[U_INSTANCE] = 0;

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, (mat4*)&mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_reset_default_compressed(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }
    uniform_value[U_INSTANCE] = 0;

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, (mat4*)&mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_reset_reflect(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    if (mesh->compressed) {
        draw_object_vertex_attrib_reset_reflect_compressed(draw);
        return;
    }

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    gl_state_active_texture(0);
}

inline static void draw_object_vertex_attrib_reset_reflect_compressed(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, (mat4*)&mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_set_default(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei vertex_size = (GLsizei)mesh->vertex_size;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    if (mesh->compressed) {
        draw_object_vertex_attrib_set_default_compressed(draw);
        return;
    }

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_flags & OBJECT_VERTEX_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TANGENT) {
        enable_vertex_attrib_array(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(5, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_BINORMAL)
        offset += 12;

    bool texcoord_mat_set[4] = { false };
    object_material_data* material = draw->material;
    for (int32_t i = 0, j = 0, l = 0; i < 4; i++) {
        if (material->material.textures[i].texture_id == -1)
            continue;

        int32_t texcoord_index = object_material_texture_type_get_texcoord_index(
            material->material.textures[i].texture_flags.type, j);
        if (texcoord_index < 0)
            continue;

        if (material->material.textures[i].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            j++;

        if (vertex_flags & (OBJECT_VERTEX_TEXCOORD0 << sub_mesh->texcoord_indices[l])) {
            enable_vertex_attrib_array(8 + texcoord_index);
            glVertexAttribPointer(8 + texcoord_index, 2, GL_FLOAT, GL_FALSE,
                vertex_size, (void*)(offset + 8ULL * sub_mesh->texcoord_indices[l]));
        }
        l++;

        if (texcoord_mat_set[texcoord_index])
            continue;

        int32_t texture_id = material->material.textures[i].texture_id;

        mat4 mat;
        mat4u_to_mat4(&material->material.textures[i].tex_coord_mat, &mat);
        shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
        if (material->material.textures[i].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            for (int32_t k = 0; k < draw->texture_transform_count; k++)
                if (draw->texture_transform_array[k].id == texture_id) {
                    mat4u_to_mat4(&draw->texture_transform_array[k].mat, &mat);
                    shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
                    texcoord_mat_set[texcoord_index] = true;
                    break;
                }
    }

    for (int32_t i = 0; i < 4; i++)
        if (!draw->vertex_attrib_array[8 + i])
            glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TEXCOORD0)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD1)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
        offset += 8;

    if (vertex_flags & OBJECT_VERTEX_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_COLOR1)
        offset += 16;

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (vertex_flags & OBJECT_VERTEX_UNKNOWN) {
        enable_vertex_attrib_array(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(7, 0.0f, 0.0f, 0.0f, 1.0f);

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_flags & OBJECT_VERTEX_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_BINORMAL)
            offset += 12;

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
            offset += 8;
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
            offset += 8;

        if (vertex_flags & OBJECT_VERTEX_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_COLOR1)
            offset += 16;

        if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
            offset += 32;

        if (vertex_flags & OBJECT_VERTEX_UNKNOWN)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);

    if (draw->instances_count)
        uniform_value[U_INSTANCE] = 1;
}

inline static void draw_object_vertex_attrib_set_default_compressed(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei vertex_size = (GLsizei)mesh->vertex_size;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_flags & OBJECT_VERTEX_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TANGENT) {
        enable_vertex_attrib_array(6);
        glVertexAttribPointer(6, 4, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(6, 0.0f, 0.0f, 0.0f, 1.0f);

    bool texcoord_mat_set[4] = { false };
    object_material_data* material = draw->material;
    for (int32_t i = 0, j = 0, l = 0; i < 4; i++) {
        if (material->material.textures[i].texture_id == -1)
            continue;

        int32_t texcoord_index = object_material_texture_type_get_texcoord_index(
            material->material.textures[i].texture_flags.type, j);
        if (texcoord_index < 0)
            continue;

        if (material->material.textures[i].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            j++;

        if (!strcmp(mesh->name, "body_MZ"))
            printf("");
        if (!strcmp(mesh->name, "kao_MZ"))
            printf("");
        if (!strcmp(mesh->name, "MZ_ami1") && !strcmp(material->material.name, "test_YZ_frame_2s"))
            printf("");

        if (vertex_flags & (OBJECT_VERTEX_TEXCOORD0 << sub_mesh->texcoord_indices[l])) {
            enable_vertex_attrib_array(8 + texcoord_index);
            glVertexAttribPointer(8 + texcoord_index, 2, GL_HALF_FLOAT, GL_FALSE,
                vertex_size, (void*)(offset + 4ULL * sub_mesh->texcoord_indices[l]));
        }
        l++;

        if (texcoord_mat_set[texcoord_index])
            continue;

        int32_t texture_id = material->material.textures[i].texture_id;

        mat4 mat;
        mat4u_to_mat4(&material->material.textures[i].tex_coord_mat, &mat);
        shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
        if (material->material.textures[i].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            for (int32_t k = 0; k < draw->texture_transform_count; k++)
                if (draw->texture_transform_array[k].id == texture_id) {
                    mat4u_to_mat4(&draw->texture_transform_array[k].mat, &mat);
                    shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
                    texcoord_mat_set[texcoord_index] = true;
                    break;
                }
    }

    for (int32_t i = 0; i < 4; i++)
        if (!draw->vertex_attrib_array[8 + i])
            glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TEXCOORD0)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD1)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
        offset += 4;

    if (vertex_flags & OBJECT_VERTEX_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_TRUE, vertex_size, (void*)offset);
        offset += 8;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertex_size, (void*)offset);
        offset += 4;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_flags & OBJECT_VERTEX_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
            offset += 4;
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
            offset += 4;

        if (vertex_flags & OBJECT_VERTEX_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
            offset += 12;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);

    if (draw->instances_count)
        uniform_value[U_INSTANCE] = 1;
}

static void draw_object_vertex_attrib_set_reflect(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei vertex_size = (GLsizei)mesh->vertex_size;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    if (mesh->compressed) {
        draw_object_vertex_attrib_set_reflect_compressed(draw);
        return;
    }

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_flags & OBJECT_VERTEX_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TANGENT)
        offset += 16;

    if (vertex_flags & OBJECT_VERTEX_BINORMAL)
        offset += 12;

    object_material_data* material = draw->material;
    if (material->material.textures[0].texture_id != -1) {
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE,
                vertex_size, (void*)(offset + 8ULL * sub_mesh->texcoord_indices[0]));
        }
        else
            glVertexAttrib4f(8, 0.0f, 0.0f, 0.0f, 1.0f);

        int32_t texture_id = material->material.textures[0].texture_id;

        mat4 mat;
        mat4u_to_mat4(&material->material.textures[0].tex_coord_mat, &mat);
        shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
        if (material->material.textures[0].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            for (int32_t j = 0; j < draw->texture_transform_count; j++)
                if (draw->texture_transform_array[j].id == texture_id) {
                    mat4u_to_mat4(&draw->texture_transform_array[j].mat, &mat);
                    shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
                    break;
                }
    }

    for (int32_t i = 1; i < 4; i++)
        glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TEXCOORD0)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD1)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
        offset += 8;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
        offset += 8;

    if (vertex_flags & OBJECT_VERTEX_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_COLOR1)
        offset += 16;

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 16;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (vertex_flags & OBJECT_VERTEX_UNKNOWN)
        offset += 16;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_flags & OBJECT_VERTEX_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_BINORMAL)
            offset += 12;

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
            offset += 8;
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
            offset += 8;

        if (vertex_flags & OBJECT_VERTEX_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_COLOR1)
            offset += 16;

        if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
            offset += 32;

        if (vertex_flags & OBJECT_VERTEX_UNKNOWN)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);
}

inline static void draw_object_vertex_attrib_set_reflect_compressed(draw_object* draw) {
    object_mesh* mesh = draw->mesh;
    object_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei vertex_size = (GLsizei)mesh->vertex_size;
    object_vertex_flags vertex_flags = mesh->vertex_flags;

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_flags & OBJECT_VERTEX_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TANGENT)
        offset += 8;

    object_material_data* material = draw->material;
    if (material->material.textures[0].texture_id != -1) {
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(8);
            glVertexAttribPointer(8, 2, GL_HALF_FLOAT, GL_FALSE,
                vertex_size, (void*)(offset + 4ULL * sub_mesh->texcoord_indices[0]));
        }
        else
            glVertexAttrib4f(8, 0.0f, 0.0f, 0.0f, 1.0f);

        int32_t texture_id = material->material.textures[0].texture_id;

        mat4 mat;
        mat4u_to_mat4(&material->material.textures[0].tex_coord_mat, &mat);
        shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
        if (material->material.textures[0].texture_flags.type == OBJECT_MATERIAL_TEXTURE_COLOR)
            for (int32_t j = 0; j < draw->texture_transform_count; j++)
                if (draw->texture_transform_array[j].id == texture_id) {
                    mat4u_to_mat4(&draw->texture_transform_array[j].mat, &mat);
                    shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
                    break;
                }
    }

    for (int32_t i = 1; i < 4; i++)
        glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_TEXCOORD0)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD1)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
        offset += 4;
    if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
        offset += 4;

    if (vertex_flags & OBJECT_VERTEX_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_flags & OBJECT_VERTEX_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_TRUE, vertex_size, (void*)offset);
        offset += 8;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertex_size, (void*)offset);
        offset += 4;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_flags & OBJECT_VERTEX_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_SHORT, GL_TRUE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_TEXCOORD2)
            offset += 4;
        if (vertex_flags & OBJECT_VERTEX_TEXCOORD3)
            offset += 4;

        if (vertex_flags & OBJECT_VERTEX_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_HALF_FLOAT, GL_FALSE, vertex_size, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(5, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_flags & OBJECT_VERTEX_BONE_DATA)
            offset += 12;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);
}
