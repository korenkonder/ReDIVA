/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_object.h"
#include "light_param/light.h"
#include "shader_ft.h"
#include "static_var.h"
#include "texture.hpp"

#define enable_vertex_attrib_array(index) \
glEnableVertexAttribArray(index); \
draw->vertex_attrib_array[index] = true

static bool draw_object_blend_set(render_context* rctx,
    draw_object* draw, obj_material_shader_lighting_type lighting_type);
static void draw_object_chara_color_fog_set(render_context* rctx,
    draw_object* draw, bool disable_fog);
static void draw_object_material_reset_default(obj_material_data* mat_data);
static void draw_object_material_reset_reflect();
static void draw_object_material_set_default(render_context* rctx,
    draw_object* draw, bool use_shader);
static void draw_object_material_set_parameter(render_context* rctx,
    obj_material_data* mat_data);
static void draw_object_material_set_reflect(render_context* rctx, draw_object* draw);
static void draw_object_material_set_uniform(render_context* rctx,
    obj_material_data* mat_data, bool disable_color_l);
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
        if (draw->mat_count == 2) {
            mat4 mat = *mats;
            shader_state_matrix_set_program(&shaders_ft, 7, &mat);
        }

        for (int32_t i = 0; i < draw->mat_count; i++, mats++) {
            union {
                mat4 m;
                vec4 v[4];
            } mat;

            mat4_transpose(mats, &mat.m);
            shader_buffer_set_ptr_array(&shaders_ft, (size_t)i * 3, 3, mat.v);
        }

        shader_state_matrix_set_modelview(&shaders_ft, 0, &rctx->view_mat, true);
        shader_state_matrix_set_program(&shaders_ft, 6, &mat4_identity);
        uniform_value[U_BONE_MAT] = 1;
    }
    else {
        mat4 mat;
        if (draw->mesh->attrib.m.billboard)
            model_mat_face_camera_view(&rctx->camera->inv_view_rot, model, &mat);
        else if (draw->mesh->attrib.m.billboard_y_axis)
            model_mat_face_camera_position(&rctx->camera->view, model, &mat);
        else
            mat = *model;
        draw_object_model_mat_load(rctx, &mat);
        uniform_value[U_BONE_MAT] = 0;
    }


    if (!show_vector)
        draw_object_func(rctx, draw);
    uniform_value[U_BONE_MAT] = 0;
}

void draw_object_draw_default(render_context* rctx, draw_object* draw) {
    if (draw->set_blend_color) {
        vec4 blend_color = draw->blend_color;
        shader_env_vert_set_ptr(&shaders_ft, 3, &blend_color);
        shader_env_vert_set_ptr(&shaders_ft, 4, &vec4_null);
    }

    if (!draw->draw_object_func)
        draw_object_vertex_attrib_set_default(draw);

    draw_object_material_set_default(rctx, draw, rctx->draw_state.shader);
    if (!draw->instances_count)
        obj_sub_mesh_draw(rctx,
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
            obj_sub_mesh_draw(rctx,
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
        shader_env_vert_set_ptr(&shaders_ft, 3, &vec4_identity);
        shader_env_vert_set_ptr(&shaders_ft, 4, &vec4_null);
    }

    rctx->draw_state.stats.object_draw_count++;
}

void draw_object_draw_sss(render_context* rctx, draw_object* draw) {
    obj_material_attrib_member attrib = draw->material->material.attrib.m;
    uniform_value[U_ALPHA_TEST] = (!attrib.flag_28 && (draw->blend_color.w < 1.0f
        || (attrib.alpha_texture || attrib.alpha_material) && !attrib.punch_through
        || draw->sub_mesh->attrib.m.transparent)
        || attrib.punch_through) ? 1 : 0;

    uniform_value[U26] = 1;
    bool aniso = false;
    obj_material_data* material = draw->material;
    switch (material->material.shader.index) {
    case SHADER_FT_CLOTH:
        if (!rctx->draw_pass.npr_param && material->material.color.ambient.w < 1.0f
            && material->material.shader_info.m.aniso_direction == OBJ_MATERIAL_ANISO_DIRECTION_NORMAL)
            aniso = true;
        break;
    case SHADER_FT_TIGHTS:
        if (!rctx->draw_pass.npr_param)
            aniso = true;
        break;
    case SHADER_FT_EYEBALL:
        uniform_value[U26] = 0;
        aniso = true;
        break;
    case SHADER_FT_SKIN:
        aniso = true;
        break;
    }

    if (aniso) {
        shader_env_frag_set(&shaders_ft, 25, 0.0f, 0.0f, 0.0f, 0.5f);
        uniform_value[U37] = 1;
    }
    else {
        vec4 sss_param = rctx->draw_pass.sss_data.param;
        shader_env_frag_set(&shaders_ft, 25, sss_param.x, sss_param.y, sss_param.z, 0.5f);
        uniform_value[U37] = 0;
    }
    draw_object_draw_default(rctx, draw);
}

void draw_object_draw_reflect(render_context* rctx, draw_object* draw) {
    obj_material_data* material = draw->material;
    draw_object_vertex_attrib_set_reflect(draw);
    obj_material_shader_lighting_type lighting_type
        = material->material.shader_info.get_lighting_type();
    bool disable_fog = draw_object_blend_set(rctx, draw, lighting_type);
    draw_object_chara_color_fog_set(rctx, draw, disable_fog);
    draw_object_material_set_reflect(rctx, draw);

    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJ_INDEX_U8)
        obj_sub_mesh_draw(rctx,
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

void draw_object_draw_reflect_reflect_map(render_context* rctx, draw_object* draw) {
    draw_object_vertex_attrib_set_reflect(draw);
    draw_object_material_set_reflect(rctx, draw);

    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJ_INDEX_U8)
        obj_sub_mesh_draw(rctx,
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
    obj_material_attrib_member attrib = draw->material->material.attrib.m;
    if (!attrib.flag_28 && (draw->blend_color.w < 1.0f
        || (attrib.alpha_texture || attrib.alpha_material) && !attrib.punch_through
        || draw->sub_mesh->attrib.m.transparent)
        || attrib.punch_through) {
        uniform_value[U_ALPHA_TEST] = 1;
        draw_object_draw_translucent(rctx, draw);
    }
    else {
        uniform_value[U_ALPHA_TEST] = 0;
        draw_object_draw_translucent(rctx, draw);
    }
}

void draw_object_draw_translucent(render_context* rctx, draw_object* draw) {
    obj_material_data* material = draw->material;
    std::vector<GLuint>* textures = draw->textures;
    if (rctx->draw_state.shader_index != -1) {
        vec4 emission = draw->emission;
        shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);
        draw_object_material_set_uniform(rctx, material, 0);
        if (material->material.attrib.m.alpha_texture)
            uniform_value[U_TEXTURE_COUNT] = 0;
        shader_set(&shaders_ft, rctx->draw_state.shader_index);
    }

    draw_object_vertex_attrib_set_default(draw);
    if (draw->material->material.attrib.m.double_sided)
        gl_state_disable_cull_face();

    GLuint tex_id = -1;
    int32_t tex_index = 0;
    if (material->material.attrib.m.alpha_texture) {
        obj_material_texture_data* texdata = material->material.texdata;
        uint32_t texture_id = -1;
        uint32_t texture_index = -1;
        if (~material->material.shader_compo.m.transparency) {
            for (int32_t i = 0; i < 8; i++, texdata++)
                if (texdata->shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR) {
                    texture_id = texdata->tex_index;
                    texture_index = texdata->texture_index;
                    break;
                }
            tex_index = 0;
        }
        else {
            for (int32_t i = 0; i < 8; i++, texdata++)
                if (texdata->shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_TRANSPARENCY) {
                    if (texdata->attrib.m.flag_29) {
                        texture_id = texdata->tex_index;
                        texture_index = texdata->texture_index;
                    }
                    break;
                }
            tex_index = 4;
        }

        if (texture_id != -1) {
            for (int32_t j = 0; j < draw->texture_pattern_count; j++)
                if (draw->texture_pattern_array[j].src == ::texture_id(0, texture_id)) {
                    texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                    if (tex)
                        tex_id = tex->tex;
                    break;
                }

            if (tex_id == -1)
                tex_id = (*textures)[texdata->texture_index];

            if (tex_id == -1)
                tex_id = 0;

            gl_state_active_bind_texture_2d(tex_index, tex_id);
        }
    }

    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    if (sub_mesh->index_format != OBJ_INDEX_U8)
        obj_sub_mesh_draw(rctx,
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
    mat4_get_translation(view, &trans);
    mat4_mult_vec3_inv_trans(view, &trans, &trans);
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

inline void model_mat_face_camera_view(mat4* inv_view_rot, mat4* src, mat4* dst) {
    mat4_mult(inv_view_rot, src, dst);
}

void obj_sub_mesh_draw(render_context* rctx, obj_primitive_type primitive_type,
    uint32_t count, uint16_t start, uint16_t end, obj_index_format index_format, size_t indices) {
    GLenum mesh_draw_mode[] = {
        GL_ZERO, //GL_POINTS,
        GL_LINES,
        GL_LINE_STRIP,
        GL_LINE_LOOP,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
        GL_ZERO, //GL_QUADS,
        GL_ZERO, //GL_QUAD_STRIP,
        GL_ZERO, //GL_POLYGON,
    };

    GLenum mesh_indices_type[] = {
        GL_ZERO,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT,
    };

    if (primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP && index_format == OBJ_INDEX_U16) {
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
    if (primitive_type == OBJ_PRIMITIVE_TRIANGLES)
        rctx->draw_state.stats.draw_triangle_count += count / 3;
    else if (primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP)
        rctx->draw_state.stats.draw_triangle_count += count - 2;
}

static bool draw_object_blend_set(render_context* rctx,
    draw_object* draw, obj_material_shader_lighting_type lighting_type) {
    GLenum blend_factor_table[] = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA_SATURATE,
        GL_ZERO,
        GL_ZERO,
        GL_ZERO,
        GL_ZERO,
        GL_ZERO,
    };

    obj_material_data* material = draw->material;
    obj_material_attrib_member attrib = material->material.attrib.m;
    if ((!attrib.alpha_texture && !attrib.alpha_material) || attrib.punch_through)
        return false;

    GLenum src_blend_factor = blend_factor_table[attrib.src_blend_factor];
    GLenum dst_blend_factor = blend_factor_table[attrib.dst_blend_factor];
    if (draw->chara_color) {
        light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
        light_data* chara_color = &set->lights[LIGHT_CHARA_COLOR];
        vec4 specular;
        light_get_specular(chara_color, &specular);
        if (specular.w >= 4.0f && light_get_type(chara_color) == LIGHT_PARALLEL
            && (src_blend_factor != GL_ONE || dst_blend_factor)) {
            int32_t shader_index = rctx->draw_state.shader_index;
            if (rctx->draw_state.shader_index == -1) {
                shader_index = material->material.shader.index;
                if (shader_index == SHADER_FT_BLINN) {
                    if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT)
                        shader_index = SHADER_FT_CONSTANT;
                    if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT)
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

    obj_material_attrib_member attrib = draw->material->material.attrib.m;
    if (!attrib.no_fog && !disable_fog) {
        if (attrib.has_fog_height)
            uniform_value[U_FOG_HEIGHT] = 2 + attrib.fog_height;
        else
            uniform_value[U_FOG_HEIGHT] = 1;
        uniform_value[U_FOG] = rctx->draw_state.fog_height ? 2 : 1;
    }
}

static void draw_object_material_reset_default(obj_material_data* mat_data) {
    if (mat_data) {
        gl_state_enable_cull_face();
        obj_material_attrib_member attrib = mat_data->material.attrib.m;
        if ((attrib.alpha_texture || attrib.alpha_material) && !attrib.punch_through)
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
    std::vector<GLuint>* textures = draw->textures;
    obj_material_data* material = draw->material;
    obj_material_shader_lighting_type lighting_type =
        material->material.shader_info.get_lighting_type();
    bool disable_fog = draw_object_blend_set(rctx, draw, lighting_type);
    draw_object_material_set_uniform(rctx, material, false);
    if (!rctx->draw_state.light)
        uniform_value[U_LIGHT_0] = 0;
    else if (draw->self_shadow)
        uniform_value[U_LIGHT_0] = draw->self_shadow ? 1 : 1;
    else
        uniform_value[U_LIGHT_0] = draw->sub_mesh->attrib.m.recieve_shadow ? 1 : 0;
    uniform_value[U_SHADOW] = 0;
    uniform_value[U_SELF_SHADOW] = rctx->draw_state.self_shadow ? 1 : 0;

    obj_material_texture_data* texdata = material->material.texdata;
    uniform_value[U_SHADOW] = draw->shadow > SHADOW_CHARA;
    for (int32_t i = 0, j = 0; i < 8; i++, texdata++) {
        if (texdata->tex_index == -1)
            continue;

        GLuint tex_id = -1;
        uint32_t texture_id = texdata->tex_index;
        for (int32_t j = 0; j < draw->texture_pattern_count; j++)
            if (draw->texture_pattern_array[j].src == ::texture_id(0, texture_id)) {
                texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                if (tex)
                    tex_id = tex->tex;
                break;
            }

        if (tex_id == -1)
            tex_id = (*textures)[texdata->texture_index];

        if (tex_id == -1)
            continue;

        int32_t tex_index = obj_material_texture_type_get_texture_index(texdata->shader_info.m.tex_type, j);
        if (tex_index < 0)
            continue;

        obj_material_texture_type tex_type = texdata->shader_info.m.tex_type;
        if (tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            j++;

        if (texdata->attrib.m.flag_29) {
            switch (tex_type) {
            case OBJ_MATERIAL_TEXTURE_NORMAL:
                uniform_value[U_NORMAL] = 0;
                break;
            case OBJ_MATERIAL_TEXTURE_SPECULAR:
                uniform_value[U_SPECULAR] = 0;
                break;
            case OBJ_MATERIAL_TEXTURE_TRANSLUCENCY:
                uniform_value[U_TRANSLUCENCY] = 0;
                break;
            case OBJ_MATERIAL_TEXTURE_TRANSPARENCY:
                uniform_value[U_TRANSPARENCY] = 0;
                break;
            }
            continue;
        }

        if (tex_type == OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE)
            gl_state_active_bind_texture_cube_map(tex_index, tex_id);
        else {
            gl_state_active_bind_texture_2d(tex_index, tex_id);

            GLenum wrap_s = 0;
            if (texdata->attrib.m.mirror_u)
                wrap_s = GL_MIRRORED_REPEAT;
            else if (texdata->attrib.m.repeat_u)
                wrap_s = GL_REPEAT;

            GLenum wrap_t = 0;
            if (texdata->attrib.m.mirror_v)
                wrap_t = GL_MIRRORED_REPEAT;
            else if (texdata->attrib.m.repeat_v)
                wrap_t = GL_REPEAT;

            if (wrap_s)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
            if (wrap_t)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
        }

        if (material->material.shader.index == SHADER_FT_SKY) {
            uniform_name uni_type = U_TEX_0_TYPE;
            if (tex_index == 1)
                uni_type = U_TEX_1_TYPE;

            texture* tex = texture_storage_get_texture(texdata->tex_index);
            if (!tex)
                uniform_value[uni_type] = 1;
            else if (tex->internal_format == GL_COMPRESSED_RED_RGTC1_EXT)
                uniform_value[uni_type] = 3;
            else if (tex->internal_format == GL_COMPRESSED_RED_GREEN_RGTC2_EXT)
                uniform_value[uni_type] = 2;
            else
                uniform_value[uni_type] = 1;
        }

        if (tex_index == 0 || tex_index == 1)
            uniform_value[U_TEXTURE_BLEND] = texdata->attrib.get_blend();
    }

    if (material->material.attrib.m.double_sided) {
        gl_state_disable_cull_face();
        if (!material->material.attrib.m.normal_dir_light)
            uniform_value[U0B] = 1;
    }

    vec4 ambient;
    *(vec3*)&ambient = *(vec3*)&material->material.color.ambient;
    if (rctx->draw_pass.sss_data.enable)
        ambient.w = material->material.color.ambient.w;
    else
        ambient.w = 1.0f;

    vec4 diffuse = material->material.color.diffuse;
    vec4 emission = draw->emission;
    draw_object_chara_color_fog_set(rctx, draw, disable_fog);
    shader_state_material_set_ambient_ptr(&shaders_ft, false, &ambient);
    shader_state_material_set_diffuse_ptr(&shaders_ft, false, &diffuse);
    shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);

    float_t line_light;
    if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_PHONG) {
        vec4 specular = material->material.color.specular;
        shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);

        float_t luma;
        vec3 luma_coeff = { 0.30f, 0.59f, 0.11f };
        vec3_dot(*(vec3*)&specular, luma_coeff, luma);
        if (luma >= 0.0099999998f || draw->texture_color_coeff.w >= 0.1f)
            uniform_value[U_SPECULAR_IBL] = 1;
        else
            uniform_value[U_SPECULAR_IBL] = 0;

        if (!material->material.shader_info.m.fresnel_type)
            uniform_value[U_TRANSLUCENCY] = 0;

        line_light = (float_t)material->material.shader_info.m.line_light * (float_t)(1.0 / 9.0);
    }
    else
        line_light = 0.0;

    if (!use_shader)
        shader_set(&shaders_ft, SHADER_FT_SIMPLE);
    else if (rctx->draw_state.shader_index != -1)
        shader_set(&shaders_ft, rctx->draw_state.shader_index);
    else if (material->material.shader.index != -1) {
        if (material->material.shader.index != SHADER_FT_BLINN)
            shader_set(&shaders_ft, material->material.shader.index);
        else if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT)
            shader_set(&shaders_ft, SHADER_FT_LAMBERT);
        else if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_PHONG)
            shader_set(&shaders_ft, SHADER_FT_BLINN);
        else
            shader_set(&shaders_ft, SHADER_FT_CONSTANT);
    }
    else
        shader_set(&shaders_ft, SHADER_FT_CONSTANT);

    if (lighting_type != OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT) {
        float_t shininess;
        if (material->material.shader.index == SHADER_FT_EYEBALL)
            shininess = 10.0f;
        else {
            shininess = (material->material.color.shininess - 16.0f) * (float_t)(1.0 / 112.0);
            shininess = max(shininess, 0.0f);
        }
        shader_state_material_set_shininess(&shaders_ft, false, shininess, 0.0f, 0.0f, 1.0f);

        float_t fresnel = (float_t)material->material.shader_info.m.fresnel_type;
        if (fresnel > 9.0)
            fresnel = 9.0;
        else if (fresnel == 0.0f)
            fresnel = rctx->draw_state.fresnel;
        fresnel = (fresnel - 1.0f) * (0.12f * 0.82f);

        shader_local_vert_set(&shaders_ft, 0, fresnel, 0.18f, line_light, 0.0f);
        shader_local_frag_set(&shaders_ft, 9, fresnel, 0.18f, line_light, 0.0f);

        shininess = max(material->material.color.shininess, 1.0f);
        shader_local_vert_set(&shaders_ft, 1, shininess, 0.0f, 0.0f, 0.0f);
        shader_local_frag_set(&shaders_ft, 3, shininess, 0.0f, 0.0f, 0.0f);

        switch (material->material.shader.index) {
        case SHADER_FT_SKIN: {
            vec4 texture_color_coeff = draw->texture_color_coeff;
            vec4 texture_color_offset = draw->texture_color_offset;
            vec4 texture_specular_coeff = draw->texture_specular_coeff;
            vec4 texture_specular_offset = draw->texture_specular_offset;

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

static void draw_object_material_set_parameter(render_context* rctx, obj_material_data* mat_data) {
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

        vec4 specular = mat_data->material.color.specular;
        specular.w = rctx->draw_state.specular_alpha;
        shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);
    }
    else {
        bump_depth = mat_data->material.bump_depth;
        reflect_uv_scale = 0.1f;
        intensity = mat_data->material.color.intensity;
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
    vec3_mult(*(vec3*)&specular, *(vec3*)&mat_data->material.color.specular, *(vec3*)&specular);
    shader_env_vert_set_ptr(&shaders_ft, 18, &specular);
}

static void draw_object_material_set_reflect(render_context* rctx, draw_object* draw) {
    obj_material_data* material = draw->material;
    std::vector<GLuint>* textures = draw->textures;
    if (material->material.attrib.m.double_sided)
        gl_state_disable_cull_face();

    obj_material_texture_data* texdata = material->material.texdata;
    for (int32_t i = 0; i < 1; i++, texdata++) {
        uint32_t texture_id = texdata->tex_index;
        if (texdata->tex_index == -1)
            break;

        GLuint tex_id = -1;
        for (int32_t j = 0; j < draw->texture_pattern_count; j++)
            if (draw->texture_pattern_array[j].src == ::texture_id(0, texture_id)) {
                texture* tex = texture_storage_get_texture(draw->texture_pattern_array[j].dst);
                if (tex)
                    tex_id = tex->tex;
                break;
            }

        if (tex_id == -1)
            tex_id = (*textures)[texdata->texture_index];

        if (tex_id == -1)
            tex_id = 0;

        gl_state_active_bind_texture_2d(i, tex_id);

        GLenum wrap_s = 0;
        if (texdata->attrib.m.mirror_u)
            wrap_s = GL_MIRRORED_REPEAT;
        else if (texdata->attrib.m.repeat_u)
            wrap_s = GL_REPEAT;

        GLenum wrap_t = 0;
        if (texdata->attrib.m.mirror_v)
            wrap_t = GL_MIRRORED_REPEAT;
        else if (texdata->attrib.m.repeat_v)
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
    if (material->material.shader.index == -1) {
        ambient = vec4_identity;
        diffuse = material->material.color.diffuse;
        emission = vec4_identity;
        specular = material->material.color.specular;
    }
    else {
        ambient = material->material.color.ambient;
        diffuse = material->material.color.diffuse;
        emission = draw->emission;
        specular = material->material.color.specular;
    }

    draw_object_material_set_uniform(rctx, material, true);
    shader_set(&shaders_ft, rctx->draw_state.shader_index);
    shader_state_material_set_ambient_ptr(&shaders_ft, false, &ambient);
    shader_state_material_set_diffuse_ptr(&shaders_ft, false, &diffuse);
    shader_state_material_set_emission_ptr(&shaders_ft, false, &emission);
    shader_state_material_set_specular_ptr(&shaders_ft, false, &specular);
    draw_object_material_set_parameter(rctx, draw->material);
}

static void draw_object_material_set_uniform(render_context* rctx, obj_material_data* mat_data, bool disable_color_l) {
    obj_shader_compo_member shader_compo = mat_data->material.shader_compo.m;
    obj_material_shader_attrib_member shader_info = mat_data->material.shader_info.m;

    if (disable_color_l) {
        if (shader_compo.color)
            shader_compo.color_l1 = 0;
        shader_compo.color_l2 = 0;
    }

    int32_t v4 = 0;
    if (shader_compo.color) {
        if (!shader_compo.color_l2)
            if (shader_compo.color_l1) {
                uniform_value[U_TEXTURE_COUNT] = 2;
                v4 = 3;
            }
            else {
                uniform_value[U_TEXTURE_COUNT] = 1;
                v4 = 1;
            }
    }
    else if (shader_compo.color_l1 && !shader_compo.color_l2) {
        uniform_value[U_TEXTURE_COUNT] = 1;
        v4 = 1;
    }
    else
        uniform_value[U_TEXTURE_COUNT] = 0;

    if (shader_compo.normal_01)
        uniform_value[U_NORMAL] = 1;
    if (shader_compo.specular)
        uniform_value[U_SPECULAR] = 1;
    if (shader_compo.transparency) {
        uniform_value[U_TRANSPARENCY] = 1;
        v4 |= 2;
    }
    if (shader_compo.transparency) {
        uniform_value[U_TRANSLUCENCY] = 1;
        v4 |= 2;
    }
    if (shader_compo.override_ibl)
        uniform_value[U_ENV_MAP] = 1;

    if (shader_info.aniso_direction != OBJ_MATERIAL_ANISO_DIRECTION_NORMAL)
        uniform_value[U_ANISO] = shader_info.aniso_direction;

    if (v4 == 1)
        uniform_value[U45] = 0;
    else if (v4 == 2 || v4 == 3)
        uniform_value[U45] = 1;
}

static void draw_object_vertex_attrib_reset_default(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (mesh->attrib.m.compressed) {
        draw_object_vertex_attrib_reset_default_compressed(draw);
        return;
    }

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }
    uniform_value[U_INSTANCE] = 0;

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, &mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, &mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_reset_default_compressed(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_vertex_format vertex_format = mesh->vertex_format;

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }
    uniform_value[U_INSTANCE] = 0;

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, &mat4_identity);
    shader_state_matrix_set_texture(&shaders_ft, 1, &mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_reset_reflect(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (mesh->attrib.m.compressed) {
        draw_object_vertex_attrib_reset_reflect_compressed(draw);
        return;
    }

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, &mat4_identity);
    gl_state_active_texture(0);
}

inline static void draw_object_vertex_attrib_reset_reflect_compressed(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_vertex_format vertex_format = mesh->vertex_format;

    for (int32_t i = 0; i < 16; i++) {
        if (draw->vertex_attrib_array[i])
            glDisableVertexAttribArray(i);
        draw->vertex_attrib_array[i] = false;
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }

    gl_state_bind_element_array_buffer(0);
    shader_state_matrix_set_texture(&shaders_ft, 0, &mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_set_default(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (mesh->attrib.m.compressed) {
        draw_object_vertex_attrib_set_default_compressed(draw);
        return;
    }

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
        enable_vertex_attrib_array(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(5, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL)
        offset += 12;

    bool texcoord_mat_set[4] = { false };
    obj_material_data* material = draw->material;
    for (int32_t i = 0, j = 0, l = 0; i < 4; i++) {
        if (material->material.texdata[i].tex_index == -1)
            continue;

        int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
            material->material.texdata[i].shader_info.m.tex_type, j);
        if (texcoord_index < 0)
            continue;

        if (material->material.texdata[i].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            j++;

        if (vertex_format & (OBJ_VERTEX_FORMAT_TEXCOORD0 << sub_mesh->uv_index[l])) {
            enable_vertex_attrib_array(8 + texcoord_index);
            glVertexAttribPointer(8 + texcoord_index, 2, GL_FLOAT, GL_FALSE,
                size_vertex, (void*)(offset + 8ULL * sub_mesh->uv_index[l]));
        }
        l++;

        if (texcoord_mat_set[texcoord_index])
            continue;

        int32_t texture_id = material->material.texdata[i].tex_index;

        mat4 mat = material->material.texdata[i].tex_coord_mat;
        shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
        if (material->material.texdata[i].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t k = 0; k < draw->texture_transform_count; k++)
                if (draw->texture_transform_array[k].id == texture_id) {
                    mat = draw->texture_transform_array[k].mat;
                    shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
                    texcoord_mat_set[texcoord_index] = true;
                    break;
                }
    }

    for (int32_t i = 0; i < 4; i++)
        if (!draw->vertex_attrib_array[8 + i])
            glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        offset += 8;

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1)
        offset += 16;

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_INT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN) {
        enable_vertex_attrib_array(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(7, 0.0f, 0.0f, 0.0f, 1.0f);

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL)
            offset += 12;

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
            offset += 8;
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
            offset += 8;

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1)
            offset += 16;

        if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
            offset += 32;

        if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);

    if (draw->instances_count)
        uniform_value[U_INSTANCE] = 1;
}

inline static void draw_object_vertex_attrib_set_default_compressed(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
        enable_vertex_attrib_array(6);
        glVertexAttribPointer(6, 4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(6, 0.0f, 0.0f, 0.0f, 1.0f);

    bool texcoord_mat_set[4] = { false };
    obj_material_data* material = draw->material;
    for (int32_t i = 0, j = 0, l = 0; i < 4; i++) {
        if (material->material.texdata[i].tex_index == -1)
            continue;

        int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
            material->material.texdata[i].shader_info.m.tex_type, j);
        if (texcoord_index < 0)
            continue;

        if (material->material.texdata[i].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            j++;

        if (vertex_format & (OBJ_VERTEX_FORMAT_TEXCOORD0 << sub_mesh->uv_index[l])) {
            enable_vertex_attrib_array(8 + texcoord_index);
            glVertexAttribPointer(8 + texcoord_index, 2, GL_HALF_FLOAT, GL_FALSE,
                size_vertex, (void*)(offset + 4ULL * sub_mesh->uv_index[l]));
        }
        l++;

        if (texcoord_mat_set[texcoord_index])
            continue;

        int32_t tex_index = material->material.texdata[i].tex_index;

        mat4 mat = material->material.texdata[i].tex_coord_mat;
        shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
        if (material->material.texdata[i].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t k = 0; k < draw->texture_transform_count; k++)
                if (draw->texture_transform_array[k].id == tex_index) {
                    mat = draw->texture_transform_array[k].mat;
                    shader_state_matrix_set_texture(&shaders_ft, texcoord_index, &mat);
                    texcoord_mat_set[texcoord_index] = true;
                    break;
                }
    }

    for (int32_t i = 0; i < 4; i++)
        if (!draw->vertex_attrib_array[8 + i])
            glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        offset += 4;

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
        offset += 8;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_UNSIGNED_SHORT, GL_FALSE, size_vertex, (void*)offset);
        offset += 8;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
            offset += 4;
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
            offset += 4;

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);

    if (draw->instances_count)
        uniform_value[U_INSTANCE] = 1;
}

static void draw_object_vertex_attrib_set_reflect(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (mesh->attrib.m.compressed) {
        draw_object_vertex_attrib_set_reflect_compressed(draw);
        return;
    }

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT)
        offset += 16;

    if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL)
        offset += 12;

    obj_material_data* material = draw->material;
    if (material->material.texdata[0].tex_index != -1) {
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE,
                size_vertex, (void*)(offset + 8ULL * sub_mesh->uv_index[0]));
        }
        else
            glVertexAttrib4f(8, 0.0f, 0.0f, 0.0f, 1.0f);

        int32_t tex_index = material->material.texdata[0].tex_index;

        mat4 mat = material->material.texdata[0].tex_coord_mat;
        shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
        if (material->material.texdata[0].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t j = 0; j < draw->texture_transform_count; j++)
                if (draw->texture_transform_array[j].id == tex_index) {
                    mat = draw->texture_transform_array[j].mat;
                    shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
                    break;
                }
    }

    for (int32_t i = 1; i < 4; i++)
        glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        offset += 8;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        offset += 8;

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1)
        offset += 16;

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_INT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN)
        offset += 16;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_BINORMAL)
            offset += 12;

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
            offset += 8;
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
            offset += 8;

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR1)
            offset += 16;

        if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
            offset += 32;

        if (vertex_format & OBJ_VERTEX_FORMAT_UNKNOWN)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);
}

inline static void draw_object_vertex_attrib_set_reflect_compressed(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    gl_state_bind_array_buffer(draw->array_buffer);

    size_t offset = 0;
    if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
        enable_vertex_attrib_array(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
        enable_vertex_attrib_array(2);
        glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT)
        offset += 8;

    obj_material_data* material = draw->material;
    if (material->material.texdata[0].tex_index != -1) {
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(8);
            glVertexAttribPointer(8, 2, GL_HALF_FLOAT, GL_FALSE,
                size_vertex, (void*)(offset + 4ULL * sub_mesh->uv_index[0]));
        }
        else
            glVertexAttrib4f(8, 0.0f, 0.0f, 0.0f, 1.0f);

        int32_t tex_index = material->material.texdata[0].tex_index;

        mat4 mat = material->material.texdata[0].tex_coord_mat;
        shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
        if (material->material.texdata[0].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t j = 0; j < draw->texture_transform_count; j++)
                if (draw->texture_transform_array[j].id == tex_index) {
                    mat = draw->texture_transform_array[j].mat;
                    shader_state_matrix_set_texture(&shaders_ft, 0, &mat);
                    break;
                }
    }

    for (int32_t i = 1; i < 4; i++)
        glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
        offset += 4;
    if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
        offset += 4;

    if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
        enable_vertex_attrib_array(3);
        glVertexAttribPointer(3, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 8;
    }
    else
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);

    if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA) {
        uniform_value[U_BONE_MAT] = 1;

        enable_vertex_attrib_array(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
        offset += 8;

        enable_vertex_attrib_array(15);
        glVertexAttribPointer(15, 4, GL_UNSIGNED_SHORT, GL_FALSE, size_vertex, (void*)offset);
        offset += 8;
    }
    else {
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_format & OBJ_VERTEX_FORMAT_POSITION) {
            enable_vertex_attrib_array(10);
            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_NORMAL) {
            enable_vertex_attrib_array(11);
            glVertexAttribPointer(11, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TANGENT) {
            enable_vertex_attrib_array(12);
            glVertexAttribPointer(12, 4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD0) {
            enable_vertex_attrib_array(13);
            glVertexAttribPointer(13, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD1) {
            enable_vertex_attrib_array(14);
            glVertexAttribPointer(14, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 4;
        }
        else
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD2)
            offset += 4;
        if (vertex_format & OBJ_VERTEX_FORMAT_TEXCOORD3)
            offset += 4;

        if (vertex_format & OBJ_VERTEX_FORMAT_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            enable_vertex_attrib_array(5);
            glVertexAttribPointer(5, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
        else
            glVertexAttrib4f(5, 0.0f, 0.0f, 0.0f, 1.0f);

        if (vertex_format & OBJ_VERTEX_FORMAT_BONE_DATA)
            offset += 16;

        shader_env_vert_set(&shaders_ft, 13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    gl_state_bind_array_buffer(0);

    gl_state_bind_element_array_buffer(draw->element_array_buffer);
}
