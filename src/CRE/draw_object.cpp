/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_object.hpp"
#include "light_param/light.hpp"
#include "shader_ft.hpp"
#include "static_var.hpp"
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
static void draw_object_vertex_attrib_reset_reflect(draw_object* draw);
static void draw_object_vertex_attrib_set_default(draw_object* draw);
static void draw_object_vertex_attrib_set_reflect(draw_object* draw);

texture_data_struct::texture_data_struct() : field_0() {

}

void draw_object_draw(render_context* rctx, draw_object* draw, mat4* model,
    void(*draw_object_func)(render_context* rctx, draw_object* draw), int32_t show_vector) {
    GLuint vao = rctx->object_data.get_vertex_array(draw);
    if (!vao)
        return;

    if (draw->mats) {
        mat4* mats = draw->mats;
        if (draw->mat_count == 2)
            shaders_ft.state_matrix_set_program(7, *mats);

        mat4 mat;
        for (int32_t i = 0; i < draw->mat_count; i++, mats++) {
            mat4_transpose(mats, &mat);
            shaders_ft.buffer_set((size_t)i * 3, 3, &mat.row0);
        }

        shaders_ft.state_matrix_set_modelview(0, rctx->view_mat, true);
        shaders_ft.state_matrix_set_program(6, mat4_identity);
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


    if (!show_vector) {
        gl_state_bind_vertex_array(vao);
        draw_object_func(rctx, draw);
    }
    uniform_value[U_BONE_MAT] = 0;
}

void draw_object_draw_default(render_context* rctx, draw_object* draw) {
    if (draw->set_blend_color) {
        shaders_ft.env_vert_set(3, draw->blend_color);
        shaders_ft.env_vert_set(4, 0.0f);
    }

    if (!draw->draw_object_func)
        draw_object_vertex_attrib_set_default(draw);

    draw_object_material_set_default(rctx, draw, rctx->draw_state.shader);

    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    if (!draw->instances_count)
        obj_sub_mesh_draw(rctx,
            sub_mesh->primitive_type,
            sub_mesh->num_index,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->index_offset);
    else {
        mat4 mat;
        for (int32_t i = 0; i < draw->instances_count; i++) {
            mat4_transpose(&draw->instances_mat[i], &mat);
            glVertexAttrib4fv(12, (const GLfloat*)&mat.row0);
            glVertexAttrib4fv(13, (const GLfloat*)&mat.row1);
            glVertexAttrib4fv(14, (const GLfloat*)&mat.row2);
            glVertexAttrib4fv(15, (const GLfloat*)&mat.row3);
            obj_sub_mesh_draw(rctx,
                sub_mesh->primitive_type,
                sub_mesh->num_index,
                sub_mesh->first_index,
                sub_mesh->last_index,
                sub_mesh->index_format,
                sub_mesh->index_offset);
        }
    }

    draw_object_material_reset_default(draw->material);
    if (!draw->draw_object_func)
        draw_object_vertex_attrib_reset_default(draw);

    if (draw->set_blend_color) {
        shaders_ft.env_vert_set(3, 1.0f);
        shaders_ft.env_vert_set(4, 0.0f);
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
        shaders_ft.env_frag_set(25, 0.0f, 0.0f, 0.0f, 0.5f);
        uniform_value[U37] = 1;
    }
    else {
        vec4& sss_param = rctx->draw_pass.sss_data.param;
        shaders_ft.env_frag_set(25, sss_param.x, sss_param.y, sss_param.z, 0.5f);
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
            sub_mesh->num_index,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->index_offset);

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
            sub_mesh->num_index,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->index_offset);

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
        shaders_ft.state_material_set_emission(false, draw->emission);
        draw_object_material_set_uniform(rctx, material, 0);
        if (material->material.attrib.m.alpha_texture)
            uniform_value[U_TEXTURE_COUNT] = 0;
        shaders_ft.set(rctx->draw_state.shader_index);
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
            sub_mesh->num_index,
            sub_mesh->first_index,
            sub_mesh->last_index,
            sub_mesh->index_format,
            sub_mesh->index_offset);

    if (tex_id != -1)
        gl_state_active_bind_texture_2d(tex_index, 0);

    gl_state_enable_cull_face();
    draw_object_vertex_attrib_reset_default(draw);

    if (rctx->draw_state.shader_index != -1)
        uniform_value_reset();

    rctx->draw_state.stats.object_translucent_draw_count++;
}

inline void draw_object_model_mat_load(render_context* rctx, mat4* mat) {
    shaders_ft.state_matrix_set_modelview(0, *mat, rctx->view_mat, true);
    shaders_ft.state_matrix_set_program(6, *mat);
}

inline void model_mat_face_camera_position(mat4* view, mat4* src, mat4* dst) {
    vec3 trans;
    mat4_get_translation(view, &trans);
    mat4_mult_vec3_inv_trans(view, &trans, &trans);
    trans = -trans;

    vec3 dir = vec3::normalize(trans - *(vec3*)&src->row3);

    vec3 x_rot;
    vec3 y_rot;
    vec3 z_rot;

    y_rot = *(vec3*)&src->row1;
    x_rot = vec3::normalize(vec3::cross(y_rot, dir));
    z_rot = vec3::cross(x_rot, y_rot);

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
        shaders_ft.enable_primitive_restart();
        shaders_ft.set_primitive_restart_index(0xFFFF);
        shaders_ft.draw_range_elements(GL_TRIANGLE_STRIP,
            start, end, count, GL_UNSIGNED_SHORT, (void*)indices);
        shaders_ft.disable_primitive_restart();
    }
    else
        shaders_ft.draw_elements(mesh_draw_mode[primitive_type],
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
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        light_data* chara_color = &set->lights[LIGHT_CHARA_COLOR];
        vec4 specular;
        chara_color->get_specular(specular);
        if (specular.w >= 4.0f && chara_color->get_type() == LIGHT_PARALLEL
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
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        light_data* chara_color = &set->lights[LIGHT_CHARA_COLOR];
        vec4 specular;
        chara_color->get_specular(specular);
        if (specular.w > 0.0f && chara_color->get_type() == LIGHT_PARALLEL)
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
        uniform_value[U_LIGHT_0] = 1;
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

        if (tex_type == OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE) {
            gl_state_active_bind_texture_cube_map(tex_index, tex_id);
            gl_state_bind_sampler(tex_index, 0);
        }
        else {
            gl_state_active_bind_texture_2d(tex_index, tex_id);

            int32_t wrap_s;
            if (texdata->attrib.m.mirror_u)
                wrap_s = 2;
            else if (texdata->attrib.m.repeat_u)
                wrap_s = 1;
            else
                wrap_s = 0;

            int32_t wrap_t;
            if (texdata->attrib.m.mirror_v)
                wrap_t = 2;
            else if (texdata->attrib.m.repeat_v)
                wrap_t = 1;
            else
                wrap_t = 0;

            texture* tex = texture_storage_get_texture(::texture_id(0, texture_id));
            gl_state_bind_sampler(tex_index, rctx->draw_pass.samplers[(wrap_t * 3
                + wrap_s) * 2 + (tex->max_mipmap_level > 0 ? 1 : 0)]);
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
    shaders_ft.state_material_set_ambient(false, ambient);
    shaders_ft.state_material_set_diffuse(false, diffuse);
    shaders_ft.state_material_set_emission(false, emission);

    float_t line_light;
    if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_PHONG) {
        vec4 specular = material->material.color.specular;
        shaders_ft.state_material_set_specular(false, specular);

        vec3 luma_coeff = { 0.30f, 0.59f, 0.11f };
        float_t luma = vec3::dot(*(vec3*)&specular, luma_coeff);
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
        shaders_ft.set(SHADER_FT_SIMPLE);
    else if (rctx->draw_state.shader_index != -1)
        shaders_ft.set(rctx->draw_state.shader_index);
    else if (material->material.shader.index != -1) {
        if (material->material.shader.index != SHADER_FT_BLINN)
            shaders_ft.set(material->material.shader.index);
        else if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT)
            shaders_ft.set(SHADER_FT_LAMBERT);
        else if (lighting_type == OBJ_MATERIAL_SHADER_LIGHTING_PHONG)
            shaders_ft.set(SHADER_FT_BLINN);
        else
            shaders_ft.set(SHADER_FT_CONSTANT);
    }
    else
        shaders_ft.set(SHADER_FT_CONSTANT);

    if (lighting_type != OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT) {
        float_t shininess;
        if (material->material.shader.index == SHADER_FT_EYEBALL)
            shininess = 10.0f;
        else {
            shininess = (material->material.color.shininess - 16.0f) * (float_t)(1.0 / 112.0);
            shininess = max_def(shininess, 0.0f);
        }
        shaders_ft.state_material_set_shininess(false, shininess, 0.0f, 0.0f, 1.0f);

        float_t fresnel = (float_t)material->material.shader_info.m.fresnel_type;
        if (fresnel > 9.0)
            fresnel = 9.0;
        else if (fresnel == 0.0f)
            fresnel = rctx->draw_state.fresnel;
        fresnel = (fresnel - 1.0f) * (0.12f * 0.82f);

        shaders_ft.local_vert_set(0, fresnel, 0.18f, line_light, 0.0f);
        shaders_ft.local_frag_set(9, fresnel, 0.18f, line_light, 0.0f);

        shininess = max_def(material->material.color.shininess, 1.0f);
        shaders_ft.local_vert_set(1, shininess, 0.0f, 0.0f, 0.0f);
        shaders_ft.local_frag_set(3, shininess, 0.0f, 0.0f, 0.0f);

        switch (material->material.shader.index) {
        case SHADER_FT_SKIN: {
            vec4 texture_color_coeff = draw->texture_color_coeff;
            vec4 texture_color_offset = draw->texture_color_offset;
            vec4 texture_specular_coeff = draw->texture_specular_coeff;
            vec4 texture_specular_offset = draw->texture_specular_offset;

            texture_color_coeff.w *= 0.015f;
            texture_specular_coeff.w *= 0.015f;

            shaders_ft.local_frag_set(5, texture_color_coeff);
            shaders_ft.local_frag_set(6, texture_color_offset);
            shaders_ft.local_frag_set(7, texture_specular_coeff);
            shaders_ft.local_frag_set(8, texture_specular_offset);
        } break;
        case SHADER_FT_HAIR:
        case SHADER_FT_CLOTH:
        case SHADER_FT_TIGHTS:
            shaders_ft.local_frag_set(5, 1.0f - draw->texture_color_coeff.w * 0.4f,
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
        specular.w = rctx->draw_state.reflectivity;
        shaders_ft.state_material_set_specular(false, specular);
    }
    else {
        bump_depth = mat_data->material.bump_depth;
        reflect_uv_scale = 0.1f;
        intensity = mat_data->material.color.intensity;
        refract_uv_scale = 0.1f;
        inv_bump_depth = (1.0f - bump_depth) * 256.0f + 1.0f;
    }

    intensity = max_def(intensity, 1.0f);

    shaders_ft.local_vert_set(2, inv_bump_depth, bump_depth, 0.0f, 0.0f);
    shaders_ft.local_frag_set(0, inv_bump_depth, bump_depth, 0.0f, 0.0f);

    shaders_ft.local_frag_set(1, intensity, intensity, intensity * 25.5f, 1.0f);
    shaders_ft.local_frag_set(2,
        reflect_uv_scale, reflect_uv_scale, refract_uv_scale, refract_uv_scale);

    vec4 specular;
    shaders_ft.env_vert_get(17, specular);
    *(vec3*)&specular = *(vec3*)&specular * *(vec3*)&mat_data->material.color.specular;
    shaders_ft.env_vert_set(18, specular);
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

        int32_t wrap_s;
        if (texdata->attrib.m.mirror_u)
            wrap_s = 2;
        else if (texdata->attrib.m.repeat_u)
            wrap_s = 1;
        else
            wrap_s = 0;

        int32_t wrap_t;
        if (texdata->attrib.m.mirror_v)
            wrap_t = 2;
        else if (texdata->attrib.m.repeat_v)
            wrap_t = 1;
        else
            wrap_t = 0;

        texture* tex = texture_storage_get_texture(::texture_id(0, texture_id));
        gl_state_bind_sampler(i, rctx->draw_pass.samplers[(wrap_t * 3
            + wrap_s) * 2 + (tex->max_mipmap_level > 0 ? 1 : 0)]);
    }

    vec4 ambient;
    vec4 diffuse;
    vec4 emission;
    vec4 specular;
    if (material->material.shader.index == -1) {
        ambient = 1.0f;
        diffuse = material->material.color.diffuse;
        emission = 1.0f;
        specular = material->material.color.specular;
    }
    else {
        ambient = material->material.color.ambient;
        diffuse = material->material.color.diffuse;
        emission = draw->emission;
        specular = material->material.color.specular;
    }

    draw_object_material_set_uniform(rctx, material, true);
    shaders_ft.set(rctx->draw_state.shader_index);
    shaders_ft.state_material_set_ambient(false, ambient);
    shaders_ft.state_material_set_diffuse(false, diffuse);
    shaders_ft.state_material_set_emission(false, emission);
    shaders_ft.state_material_set_specular(false, specular);
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

    if (vertex_format & OBJ_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }
    uniform_value[U_INSTANCE] = 0;

    shaders_ft.state_matrix_set_texture(0, mat4_identity);
    shaders_ft.state_matrix_set_texture(1, mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_reset_reflect(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (vertex_format & OBJ_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;
    }

    shaders_ft.state_matrix_set_texture(0, mat4_identity);
    gl_state_active_texture(0);
}

static void draw_object_vertex_attrib_set_default(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

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

        l++;

        if (texcoord_mat_set[texcoord_index])
            continue;

        int32_t texture_id = material->material.texdata[i].tex_index;

        shaders_ft.state_matrix_set_texture(texcoord_index,
            material->material.texdata[i].tex_coord_mat);
        if (material->material.texdata[i].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t k = 0; k < draw->texture_transform_count; k++)
                if (draw->texture_transform_array[k].id == texture_id) {
                    shaders_ft.state_matrix_set_texture(texcoord_index,
                        draw->texture_transform_array[k].mat);
                    texcoord_mat_set[texcoord_index] = true;
                    break;
                }
    }

    if (vertex_format & OBJ_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 1;
    else
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        if (vertex_format & OBJ_VERTEX_COLOR0)
            uniform_value[U_MORPH_COLOR] = 1;
        else
            uniform_value[U_MORPH_COLOR] = 0;

        shaders_ft.env_vert_set(13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    else {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;

        shaders_ft.env_vert_set(13, 0.0f, 1.0f, 0.0f, 0.0f);
    }

    if (draw->instances_count)
        uniform_value[U_INSTANCE] = 1;
    else
        uniform_value[U_INSTANCE] = 0;
}

static void draw_object_vertex_attrib_set_reflect(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    obj_material_data* material = draw->material;
    if (material->material.texdata[0].tex_index != -1) {
        int32_t tex_index = material->material.texdata[0].tex_index;

        shaders_ft.state_matrix_set_texture(0,
            material->material.texdata[0].tex_coord_mat);
        if (material->material.texdata[0].shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            for (int32_t j = 0; j < draw->texture_transform_count; j++)
                if (draw->texture_transform_array[j].id == tex_index) {
                    shaders_ft.state_matrix_set_texture(0,
                        draw->texture_transform_array[j].mat);
                    break;
                }
    }

    if (vertex_format & OBJ_VERTEX_BONE_DATA)
        uniform_value[U_BONE_MAT] = 1;
    else
        uniform_value[U_BONE_MAT] = 0;

    if (draw->morph_array_buffer) {
        uniform_value[U_MORPH] = 1;

        if (vertex_format & OBJ_VERTEX_COLOR0)
            uniform_value[U_MORPH_COLOR] = 1;
        else
            uniform_value[U_MORPH_COLOR] = 0;

        shaders_ft.env_vert_set(13, draw->morph_value, 1.0f - draw->morph_value, 0.0f, 0.0f);
    }
    else {
        uniform_value[U_MORPH] = 0;
        uniform_value[U_MORPH_COLOR] = 0;

        shaders_ft.env_vert_set(13, 0.0f, 1.0f, 0.0f, 0.0f);
    }
}
