/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_context.hpp"
#include "Glitter/glitter.hpp"
#include "rob/rob.hpp"
#include "draw_pass.hpp"
#include "draw_task.hpp"
#include "file_handler.hpp"
#include "shader_ft.hpp"
#include "sound.hpp"
#include "stage.hpp"

float_t delta_frame_history = 0;
int32_t delta_frame_history_int = 0;

static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level);
static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular);

sss_data::sss_data() : init(), enable(), npr_contour(), param() {
    init = true;
    enable = true;
    npr_contour = true;
    param = { 0.0f, 0.0f, 0.0f, 1.0f };

    textures[0].init(640, 360, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
    textures[1].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[2].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[3].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
}

sss_data::~sss_data() {

}

draw_pass::draw_pass() :enable(), reflect_blur_num(), reflect_blur_filter(), wait_for_gpu(), cpu_time(), gpu_time(),
time(), draw_pass_3d(), reflect_type(), show_vector_flags(), show_vector_length(), show_vector_z_offset(),
field_2F8(), sss_texture(), npr_param(), field_31C(), field_31D(), field_31E(), field_31F(), field_320(), npr() {
    for (bool& i : enable)
        i = true;
    reflect = true;
    refract = true;
    shadow = true;
    opaque_z_sort = true;
    alpha_z_sort = true;
    for (bool& i : draw_pass_3d)
        i = true;
    reflect_texture.init(512, 256, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
    refract_texture.init(512, 256, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
    shadow_ptr = new ::shadow;
    if (shadow_ptr)
        shadow_ptr->init_data();
}

draw_pass::~draw_pass() {
    delete shadow_ptr;
}

draw_state::draw_state() : stats(), stats_prev(), wireframe(), wireframe_overlay(), light(),
self_shadow(), field_45(), use_global_material(), fog_height(), ex_data_mat(), field_68() {
    shader = true;
    shader_index = -1;
    field_50 = -1;
    bump_depth = 1.0f;
    intensity = 1.0f;
    reflectivity = 1.0f;
    reflect_uv_scale = 0.1f;
    refract_uv_scale = 0.1f;
    fresnel = 7.0f;
}

material_list_struct::material_list_struct() : blend_color(),
has_blend_color(), emission(), has_emission() {
    hash = (uint32_t)-1;
}

material_list_struct::material_list_struct(uint32_t hash, vec4& blend_color,
    vec4u8& has_blend_color, vec4& emission, vec4u8& has_emission) : hash(hash), blend_color(blend_color),
    has_blend_color(has_blend_color), emission(emission), has_emission(has_emission) {

}

texture_pattern_struct::texture_pattern_struct() : src(), dst() {

}

texture_pattern_struct::texture_pattern_struct(texture_id src, texture_id dst) : src(src), dst(dst) {

}

texture_transform_struct::texture_transform_struct() {
    id = (uint32_t)-1;
    mat = mat4_identity;
}

texture_transform_struct::texture_transform_struct(uint32_t id, mat4& mat) : id(id), mat(mat) {

}

light_proj::light_proj(int32_t width, int32_t height) : enable(), texture_id() {
    shadow_texture[0].init(2048, 512, 0, GL_R32F, GL_DEPTH_COMPONENT32F);
    shadow_texture[1].init(2048, 512, 0, GL_R32F, GL_ZERO);
    draw_texture.init(width, height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
}

light_proj::~light_proj() {

}

void light_proj::resize(int32_t width, int32_t height) {
    if (!this)
        return;

    draw_texture.init(width, height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
}

bool light_proj::set(render_context* rctx) {
    if (!this)
        return false;

    static const GLfloat depth_clear = 1.0f;

    shadow_texture[0].bind();
    glViewport(0, 0, 2048, 512);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, (float_t*)&vec4_identity);
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);

    if (set_mat(rctx, false)) {
        rctx->draw_state.shader_index = SHADER_FT_SIL;
        uniform_value[U0A] = 1;
        return true;
    }
    else {
        draw_texture.bind();
        glViewport(0, 0, draw_texture.color_texture->width,
            draw_texture.color_texture->height);
        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        glClearBufferfv(GL_COLOR, 0, (float_t*)&vec4_identity);
        glClearBufferfv(GL_DEPTH, 0, &depth_clear);
    }
    return false;
}

void light_proj::get_proj_mat(vec3* view_point, vec3* interest, float_t fov, mat4* mat) {
    if (mat) {
        mat4 temp;
        mat4_translate(0.5f, 0.5f, 0.5f, &temp);
        mat4_scale_rot(&temp, 0.5f, 0.5f, 0.5f, &temp);

        mat4 proj;
        mat4_persp(fov, 4.0, 0.1000000014901161, 10.0, &proj);
        mat4_mult(&proj, &temp, &proj);

        mat4 view;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        mat4_look_at(view_point, interest, &up, &view);
        mat4_mult(&view, &proj, mat);
    }
    else {
        mat4 proj;
        mat4_persp(fov, 4.0, 0.1000000014901161, 10.0, &proj);
        shaders_ft.state_matrix_set_projection(proj, false);

        mat4 view;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        mat4_look_at(view_point, interest, &up, &view);
        shaders_ft.state_matrix_set_modelview(0, view, true);
    }
}

bool light_proj::set_mat(render_context* rctx, bool set_mat) {
    light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
    light_data* data = &set->lights[LIGHT_PROJECTION];
    if (data->get_type() != LIGHT_SPOT)
        return false;

    vec3 position;
    vec3 spot_direction;
    data->get_position(position);
    data->get_spot_direction(spot_direction);

    if (vec3::length_squared(spot_direction) <= 0.000001f)
        return false;

    float_t spot_cutoff = data->get_spot_cutoff();
    float_t fov = atanf(tanf(spot_cutoff * DEG_TO_RAD_FLOAT) * 0.25f) * (RAD_TO_DEG_FLOAT * 2.0f);

    vec3 interest = position + spot_direction;
    if (set_mat) {
        mat4 mat;
        get_proj_mat(&position, &interest, fov, &mat);
        shaders_ft.env_vert_set(24, 4, &mat.row0);
    }
    else
        get_proj_mat(&position, &interest, fov, 0);
    return true;
}

morph_struct::morph_struct() : value() {

}

object_data_buffer::object_data_buffer() {
    offset = 0;
    max_offset = 0;
    size = 0x300000;
    data = force_malloc(0x300000);
}

object_data_buffer::~object_data_buffer() {
    free_def(data);
}

draw_task* object_data_buffer::add_draw_task(draw_task_type type) {
    if (!data)
        return 0;

    int32_t size = align_val(sizeof(draw_task_type) + sizeof(mat4) + sizeof(float_t) * 2, 0x08);
    switch (type) {
    case DRAW_TASK_OBJECT:
        size += sizeof(draw_object);
        break;
    case DRAW_TASK_OBJECT_PRIMITIVE:
        size += sizeof(draw_primitive);
        break;
    case DRAW_TASK_OBJECT_PREPROCESS:
        size += sizeof(draw_task_preprocess);
        break;
    case DRAW_TASK_OBJECT_TRANSLUCENT:
        size += sizeof(draw_task_object_translucent);
        break;
    default:
        return 0;
    }

    if (offset + size > this->size)
        return 0;

    draw_task* task = (draw_task*)((size_t)data + offset);
    offset += size;
    if (max_offset < offset)
        max_offset = offset;
    return task;
}

mat4* object_data_buffer::add_mat4(int32_t count) {
    if (!data)
        return 0;

    int32_t size = sizeof(mat4) * count;
    if (offset + size > this->size)
        return 0;

    mat4* mats = (mat4*)((size_t)data + offset);
    offset += size;
    if (max_offset < offset)
        max_offset = offset;
    return mats;
}

void object_data_buffer::reset() {
    offset = 0;
}

object_data::object_data() : draw_task_flags(), shadow_type(), field_8(), field_C(), passed(), culled(),
passed_prev(), culled_prev(), show_alpha_center(), show_mat_center(), buffer(), texture_pattern_count(),
texture_pattern_array(), wet_param(), texture_transform_count(), texture_transform_array(),
material_list_count(), material_list_array(), object_bounding_sphere_check_func() {
    field_230 = -1;
    object_culling = true;
    object_sort = true;
    chara_color = true;
    texture_color_coeff = vec4_identity;
    texture_color_offset = vec4_null;
    texture_specular_coeff = vec4_identity;
    texture_specular_offset = vec4_null;
}

object_data::~object_data() {
    for (object_data_vertex_array& i : vertex_array_cache)
        glDeleteVertexArrays(1, &i.vertex_array);
    vertex_array_cache.clear();
}

void object_data::add_vertex_array(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    obj_material_data* material = draw->material;

    GLuint array_buffer = draw->array_buffer;
    GLuint morph_array_buffer = draw->morph_array_buffer;

    int32_t texcoord_array[2] = { -1, -1 };
    int32_t color_tex_index = 0;
    for (obj_material_texture_data& i : material->material.texdata) {
        if (i.tex_index == -1)
            continue;

        int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
            i.shader_info.m.tex_type, color_tex_index);
        if (texcoord_index < 0)
            continue;

        texcoord_array[texcoord_index] = sub_mesh->uv_index[&i - material->material.texdata];

        if (i.shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            color_tex_index++;
    }

    object_data_vertex_array* vertex_array_data = 0;
    for (object_data_vertex_array& i : vertex_array_cache)
        if (i.alive_time >= 0 && i.array_buffer == array_buffer && i.morph_array_buffer == morph_array_buffer
            && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array))) {
            if (!i.vertex_array) {
                vertex_array_data = &i;
                break;
            }

            i.alive_time = 2;
            return;
        }

    if (!vertex_array_data)
        for (object_data_vertex_array& i : vertex_array_cache)
            if (i.alive_time <= 0) {
                vertex_array_data = &i;
                break;
            }

    if (!vertex_array_data) {
        vertex_array_cache.push_back({});
        vertex_array_data = &vertex_array_cache.back();
        *vertex_array_data = {};
    }

    if (!vertex_array_data->vertex_array)
        glGenVertexArrays(1, &vertex_array_data->vertex_array);

    vertex_array_data->array_buffer = array_buffer;
    vertex_array_data->morph_array_buffer = morph_array_buffer;
    vertex_array_data->alive_time = 2;
    memcpy(&vertex_array_data->texcoord_array, texcoord_array, sizeof(texcoord_array));

    bool compressed = mesh->attrib.m.compressed;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    gl_state_bind_vertex_array(vertex_array_data->vertex_array);
    gl_state_bind_array_buffer(draw->array_buffer);
    gl_state_bind_element_array_buffer(draw->element_array_buffer);

    size_t offset = 0;
    if (vertex_format & OBJ_VERTEX_POSITION) {
        if (!vertex_array_data->vertex_attrib_array[0]) {
            glEnableVertexAttribArray(0);
            vertex_array_data->vertex_attrib_array[0] = true;
        }

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 12;
    }
    else if (vertex_array_data->vertex_attrib_array[0]) {
        glDisableVertexAttribArray(0);
        glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);
        vertex_array_data->vertex_attrib_array[0] = false;
    }

    if (vertex_format & OBJ_VERTEX_NORMAL) {
        if (!vertex_array_data->vertex_attrib_array[2]) {
            glEnableVertexAttribArray(2);
            vertex_array_data->vertex_attrib_array[2] = true;
        }

        if (!compressed) {
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else {
            glVertexAttribPointer(2, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
    }
    else if (vertex_array_data->vertex_attrib_array[2]) {
        glDisableVertexAttribArray(2);
        glVertexAttrib4f(2, 0.0f, 0.0f, 0.0f, 1.0f);
        vertex_array_data->vertex_attrib_array[2] = false;
    }

    if (vertex_format & OBJ_VERTEX_TANGENT) {
        if (!vertex_array_data->vertex_attrib_array[6]) {
            glEnableVertexAttribArray(6);
            vertex_array_data->vertex_attrib_array[6] = true;
        }

        if (!compressed) {
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else {
            glVertexAttribPointer(6, 4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }
    }
    else if (vertex_array_data->vertex_attrib_array[6]) {
        glDisableVertexAttribArray(6);
        glVertexAttrib4f(6, 0.0f, 0.0f, 0.0f, 1.0f);
        vertex_array_data->vertex_attrib_array[6] = false;
    }

    if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
        offset += 12;

    for (int32_t i = 0; i < 2; i++) {
        int32_t texcoord_index = texcoord_array[i];
        if (texcoord_index < 0) {
            if (vertex_array_data->vertex_attrib_array[8 + i]) {
                glDisableVertexAttribArray(8 + i);
                glVertexAttrib4f(8 + i, 0.0f, 0.0f, 0.0f, 1.0f);
                vertex_array_data->vertex_attrib_array[8 + i] = false;
            }
            continue;
        }

        if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << i)) {
            if (!vertex_array_data->vertex_attrib_array[8 + i]) {
                glEnableVertexAttribArray(8 + i);
                vertex_array_data->vertex_attrib_array[8 + i] = true;
            }

            if (!compressed)
                glVertexAttribPointer(8 + i, 2, GL_FLOAT, GL_FALSE,
                    size_vertex, (void*)(offset + 8ULL * texcoord_index));
            else
                glVertexAttribPointer(8 + i, 2, GL_HALF_FLOAT, GL_FALSE,
                    size_vertex, (void*)(offset + 4ULL * texcoord_index));
        }
    }

    if (!compressed) {
        if (vertex_format & OBJ_VERTEX_TEXCOORD0)
            offset += 8;
        if (vertex_format & OBJ_VERTEX_TEXCOORD1)
            offset += 8;
        if (vertex_format & OBJ_VERTEX_TEXCOORD2)
            offset += 8;
        if (vertex_format & OBJ_VERTEX_TEXCOORD3)
            offset += 8;
    }
    else {
        if (vertex_format & OBJ_VERTEX_TEXCOORD0)
            offset += 4;
        if (vertex_format & OBJ_VERTEX_TEXCOORD1)
            offset += 4;
        if (vertex_format & OBJ_VERTEX_TEXCOORD2)
            offset += 4;
        if (vertex_format & OBJ_VERTEX_TEXCOORD3)
            offset += 4;
    }

    if (vertex_format & OBJ_VERTEX_COLOR0) {
        if (!vertex_array_data->vertex_attrib_array[3]) {
            glEnableVertexAttribArray(3);
            vertex_array_data->vertex_attrib_array[3] = true;
        }

        if (!compressed) {
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else {
            glVertexAttribPointer(3, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
    }
    else if (vertex_array_data->vertex_attrib_array[3]) {
        glDisableVertexAttribArray(3);
        glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);
        vertex_array_data->vertex_attrib_array[3] = false;
    }

    if (vertex_format & OBJ_VERTEX_COLOR1)
        offset += 16;

    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        if (!vertex_array_data->vertex_attrib_array[1]) {
            glEnableVertexAttribArray(1);
            vertex_array_data->vertex_attrib_array[1] = true;
        }

        if (!compressed) {
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else {
            glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
            offset += 8;
        }

        if (!vertex_array_data->vertex_attrib_array[15]) {
            glEnableVertexAttribArray(15);
            vertex_array_data->vertex_attrib_array[15] = true;
        }

        if (!compressed) {
            glVertexAttribPointer(15, 4, GL_INT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else {
            glVertexAttribPointer(15, 4, GL_UNSIGNED_SHORT, GL_FALSE, size_vertex, (void*)offset);
            offset += 8;
        }
    }
    else {
        if (vertex_array_data->vertex_attrib_array[1]) {
            glDisableVertexAttribArray(1);
            glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 0.0f);
            vertex_array_data->vertex_attrib_array[1] = false;
        }

        if (vertex_array_data->vertex_attrib_array[15]) {
            glDisableVertexAttribArray(15);
            glVertexAttrib4f(15, 0.0f, 0.0f, 0.0f, 0.0f);
            vertex_array_data->vertex_attrib_array[15] = false;
        }
    }

    if (!compressed && vertex_format & OBJ_VERTEX_UNKNOWN) {
        if (!vertex_array_data->vertex_attrib_array[7]) {
            glEnableVertexAttribArray(7);
            vertex_array_data->vertex_attrib_array[7] = true;
        }

        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
        offset += 16;
    }
    else if (vertex_array_data->vertex_attrib_array[7]) {
        glDisableVertexAttribArray(7);
        glVertexAttrib4f(7, 0.0f, 0.0f, 0.0f, 1.0f);
        vertex_array_data->vertex_attrib_array[7] = false;
    }

    if (draw->morph_array_buffer) {
        gl_state_bind_array_buffer(draw->morph_array_buffer);

        offset = 0;
        if (vertex_format & OBJ_VERTEX_POSITION) {
            if (!vertex_array_data->vertex_attrib_array[10]) {
                glEnableVertexAttribArray(10);
                vertex_array_data->vertex_attrib_array[10] = true;
            }

            glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else if (vertex_array_data->vertex_attrib_array[10]) {
            glDisableVertexAttribArray(10);
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[10] = false;
        }

        if (vertex_format & OBJ_VERTEX_NORMAL) {
            if (!vertex_array_data->vertex_attrib_array[11]) {
                glEnableVertexAttribArray(11);
                vertex_array_data->vertex_attrib_array[11] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }
            else {
                glVertexAttribPointer(11, 3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array_data->vertex_attrib_array[11]) {
            glDisableVertexAttribArray(11);
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[11] = false;
        }

        if (vertex_format & OBJ_VERTEX_TANGENT) {
            if (!vertex_array_data->vertex_attrib_array[12]) {
                glEnableVertexAttribArray(12);
                vertex_array_data->vertex_attrib_array[12] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(12, 4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array_data->vertex_attrib_array[12]) {
            glDisableVertexAttribArray(12);
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[12] = false;
        }

        if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
            offset += 12;

        if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
            if (!vertex_array_data->vertex_attrib_array[13]) {
                glEnableVertexAttribArray(13);
                vertex_array_data->vertex_attrib_array[13] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
            }
            else {
                glVertexAttribPointer(13, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 4;
            }
        }
        else if (vertex_array_data->vertex_attrib_array[13]) {
            glDisableVertexAttribArray(13);
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[13] = false;
        }

        if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
            if (!vertex_array_data->vertex_attrib_array[14]) {
                glEnableVertexAttribArray(14);
                vertex_array_data->vertex_attrib_array[14] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
            }
            else {
                glVertexAttribPointer(14, 2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 4;
            }
        }
        else if (vertex_array_data->vertex_attrib_array[14]) {
            glDisableVertexAttribArray(14);
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[14] = false;
        }

        if (!compressed) {
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 8;
        }
        else {
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 4;
        }

        if (vertex_format & OBJ_VERTEX_COLOR0) {
            uniform_value[U_MORPH_COLOR] = 1;

            if (!vertex_array_data->vertex_attrib_array[5]) {
                glEnableVertexAttribArray(5);
                vertex_array_data->vertex_attrib_array[5] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(5, 4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array_data->vertex_attrib_array[5]) {
            glDisableVertexAttribArray(5);
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[5] = false;
        }

        if (!compressed) {
            if (vertex_format & OBJ_VERTEX_COLOR1)
                offset += 16;

            if (vertex_format & OBJ_VERTEX_BONE_DATA)
                offset += 32;

            if (vertex_format & OBJ_VERTEX_UNKNOWN)
                offset += 16;
        }
        else {
            if (vertex_format & OBJ_VERTEX_BONE_DATA)
                offset += 16;
        }
    }
    else {
        if (vertex_array_data->vertex_attrib_array[10]) {
            glDisableVertexAttribArray(10);
            glVertexAttrib4f(10, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[10] = false;
        }

        if (vertex_array_data->vertex_attrib_array[11]) {
            glDisableVertexAttribArray(11);
            glVertexAttrib4f(11, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[11] = false;
        }

        if (vertex_array_data->vertex_attrib_array[12]) {
            glDisableVertexAttribArray(12);
            glVertexAttrib4f(12, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[12] = false;
        }

        if (vertex_array_data->vertex_attrib_array[13]) {
            glDisableVertexAttribArray(13);
            glVertexAttrib4f(13, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[13] = false;
        }

        if (vertex_array_data->vertex_attrib_array[14]) {
            glDisableVertexAttribArray(14);
            glVertexAttrib4f(14, 0.0f, 0.0f, 0.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[14] = false;
        }

        if (vertex_array_data->vertex_attrib_array[5]) {
            glDisableVertexAttribArray(5);
            glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);
            vertex_array_data->vertex_attrib_array[5] = false;
        }
    }

    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);
    gl_state_bind_element_array_buffer(0);

}

void object_data::check_vertex_arrays() {
    for (object_data_vertex_array& i : vertex_array_cache)
        if (i.alive_time > 0 && --i.alive_time <= 0) {
            i.array_buffer = 0;
            i.morph_array_buffer = 0;
        }
}

GLuint object_data::get_vertex_array(draw_object* draw) {
    obj_mesh* mesh = draw->mesh;
    obj_sub_mesh* sub_mesh = draw->sub_mesh;
    obj_material_data* material = draw->material;

    GLuint array_buffer = draw->array_buffer;
    GLuint morph_array_buffer = draw->morph_array_buffer;

    int32_t texcoord_array[2] = { -1, -1 };
    int32_t color_tex_index = 0;
    for (obj_material_texture_data& i : material->material.texdata) {
        if (i.tex_index == -1)
            continue;

        int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
            i.shader_info.m.tex_type, color_tex_index);
        if (texcoord_index < 0)
            continue;

        texcoord_array[texcoord_index] = sub_mesh->uv_index[&i - material->material.texdata];

        if (i.shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
            color_tex_index++;
    }

    object_data_vertex_array* vertex_array_data = 0;
    for (object_data_vertex_array& i : vertex_array_cache)
        if (i.alive_time > 0 && i.array_buffer == array_buffer && i.morph_array_buffer == morph_array_buffer
            && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
            return i.vertex_array;
    return 0;
}

bool object_data::get_chara_color() {
    return chara_color;
}

::draw_task_flags object_data::get_draw_task_flags() {
    return draw_task_flags;
}

void object_data::get_material_list(int32_t& count, material_list_struct*& value) {
    count = material_list_count;

    for (int32_t i = 0; i < count; i++)
        value[i] = material_list_array[i];
}

void object_data::get_morph(object_info* object, float_t* value) {
    *value = morph.value;
    *object = morph.object;
}

shadow_type_enum object_data::get_shadow_type() {
    return shadow_type;
}

void object_data::get_texture_color_coeff(vec4& value) {
    value = texture_color_coeff;
}

void object_data::get_texture_color_offset(vec4& value) {
    value = texture_color_offset;
}

void object_data::get_texture_pattern(int32_t& count, texture_pattern_struct*& value) {
    count = texture_pattern_count;

    for (int32_t i = 0; i < count; i++)
        value[i] = texture_pattern_array[i];
}

void object_data::get_texture_specular_coeff(vec4& value) {
    value = texture_specular_coeff;
}

void object_data::get_texture_specular_offset(vec4& value) {
    value = texture_specular_offset;
}

void object_data::get_texture_transform(int32_t& count, texture_transform_struct*& value) {
    count = texture_transform_count;

    for (int32_t i = 0; i < count; i++)
        value[i] = texture_transform_array[i];
}

float_t object_data::get_wet_param() {
    return wet_param;
}

void object_data::reset() {
    passed_prev = passed;
    culled_prev = culled;
    draw_task_flags = (::draw_task_flags)0;
    field_8 = 0;
    field_C = 0;
    memset(&passed, 0, sizeof(object_data_culling_info));
    memset(&culled, 0, sizeof(object_data_culling_info));

    texture_pattern_count = 0;
    memset(texture_pattern_array, 0, sizeof(texture_pattern_array));
    texture_transform_count = 0;
    memset(texture_transform_array, 0, sizeof(texture_transform_array));
    texture_color_coeff = vec4_identity;
    texture_color_offset = vec4_null;
    texture_specular_coeff = vec4_identity;
    texture_specular_offset = vec4_null;

    for (std::vector<draw_task*>& i : draw_task_array)
        i.clear();
    buffer.reset();
}

void object_data::set_chara_color(bool value) {
    chara_color = value;
}

void object_data::set_draw_task_flags(::draw_task_flags flags) {
    draw_task_flags = flags;
}

void object_data::set_material_list(int32_t count, material_list_struct* value) {
    if (count > MATERIAL_LIST_COUNT)
        return;

    material_list_count = count;

    if (count)
        for (int32_t i = 0; i < count; i++)
            material_list_array[i] = value[i];
    else
        for (int32_t i = 0; i < MATERIAL_LIST_COUNT; i++)
            material_list_array[i] = {};
}

void object_data::set_morph(object_info object, float_t value) {
    morph.value = value;
    morph.object = object;
}

void object_data::set_object_bounding_sphere_check_func(
    bool(*func)(obj_bounding_sphere*, camera*)) {
    object_bounding_sphere_check_func = func;
}

void object_data::set_shadow_type(shadow_type_enum type) {
    if (type == SHADOW_CHARA || type == SHADOW_STAGE)
        shadow_type = type;
}

void object_data::set_texture_color_coeff(vec4& value) {
    texture_color_coeff = value;
}

void object_data::set_texture_color_offset(vec4& value) {
    texture_color_offset = value;
}

void object_data::set_texture_pattern(int32_t count, texture_pattern_struct* value) {
    if (count > TEXTURE_PATTERN_COUNT)
        return;

    texture_pattern_count = count;

    if (count)
        for (int32_t i = 0; i < count; i++)
            texture_pattern_array[i] = value[i];
    else
        for (int32_t i = 0; i < TEXTURE_PATTERN_COUNT; i++)
            texture_pattern_array[i] = {};
}

void object_data::set_texture_specular_coeff(vec4& value) {
    texture_specular_coeff = value;
}

void object_data::set_texture_specular_offset(vec4& value) {
    texture_specular_offset = value;
}

void object_data::set_texture_transform(int32_t count, texture_transform_struct* value) {
    if (count > TEXTURE_TRANSFORM_COUNT)
        return;

    texture_transform_count = count;

    if (count)
        for (int32_t i = 0; i < count; i++)
            texture_transform_array[i] = value[i];
    else
        for (int32_t i = count; i < TEXTURE_TRANSFORM_COUNT; i++)
            texture_transform_array[i] = {};
}

void object_data::set_wet_param(float_t value) {
    wet_param = value;
}

extern float_t rob_frame;
extern render_context* rctx_ptr;

render_context::render_context() : litproj(),
chara_reflect(), chara_refract(), view_mat(), matrix_buffer()  {
    camera = new ::camera;
}

render_context::~render_context() {
    if (camera) {
        delete camera;
        camera = 0;
    }
}

void render_context::ctrl() {
    delta_frame_history += get_delta_frame();
    float_t v1;
    delta_frame_history = modff(delta_frame_history, &v1);
    delta_frame_history_int = (int32_t)v1;
    if (delta_frame_history < 0.001f)
        delta_frame_history = 0.0f;
    else if (1.0f - delta_frame_history < 0.001f)
        delta_frame_history_int++;

    /*for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (!task_rob_manager_check_chara_loaded(rob_chara_array[i].chara_id)
            || rob_chara_pv_data_array[i].type == ROB_CHARA_TYPE_NONE)
            continue;

        float_t frame = rob_chara_get_frame(&rob_chara_array[i]);
        float_t frame_count = rob_chara_get_frame_count(&rob_chara_array[i]);
        frame += get_delta_frame();
        if (frame >= frame_count) {
            frame = 0.0f;
            rob_chara_item_equip* rob_item_equip = rob_chara_array[i].item_equip;
            for (int32_t j = rob_item_equip->first_item_equip_object;
                j < rob_item_equip->max_item_equip_object; j++) {
                rob_chara_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[j];
                itm_eq_obj->osage_iterations = 60;
                for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
                    if (i)
                        i->rob.osage_reset = true;
            }
        }
        //rob_chara_set_frame(&rob_chara_array[i], frame);
        rob_chara_set_frame(&rob_chara_array[i], rob_frame);
        rob_chara_array[i].item_equip->shadow_type = SHADOW_CHARA;
    }*/

    rctx_ptr = this;
    app::TaskWork::Ctrl();
    sound_ctrl();
    file_handler_storage_ctrl();

    draw_pass.shadow_ptr->ctrl(this);
    draw_state.stats_prev = draw_state.stats;
    draw_state.stats = {};
    object_data.reset();
}

void render_context::disp() {
    rctx_ptr = this;
    app::TaskWork::Disp();

    post_process.ctrl(camera);
    draw_pass_main(this);
    app::TaskWork::Basic();
}

void render_context::light_param_data_light_set(light_param_light * light) {
    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        light_param_light_group* group = &light->group[i];
        ::light_set* set = &light_set[i];

        for (int32_t j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            light_param_light_data* data = &group->data[j];
            light_data* light = &set->lights[j];

            if (data->has_type)
                light->set_type(data->type);

            if (data->has_ambient)
                light->set_ambient(data->ambient);

            if (data->has_diffuse)
                light->set_diffuse(data->diffuse);

            if (data->has_specular)
                light->set_specular(data->specular);

            if (data->has_position)
                light->set_position(data->position);

            if (data->has_spot_direction)
                light->set_spot_direction(data->spot_direction);

            if (data->has_spot_exponent)
                light->set_spot_exponent(data->spot_exponent);

            if (data->has_spot_cutoff)
                light->set_spot_cutoff(data->spot_cutoff);

            if (data->has_attenuation)
                light->set_attenuation(data->attenuation);

            light->set_clip_plane(data->clip_plane);

            if (data->has_tone_curve)
                light->set_tone_curve(data->tone_curve);
        }
    }
}

void render_context::light_param_data_fog_set(light_param_fog* f) {
    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++) {
        light_param_fog_group* group = &f->group[i];
        ::fog* fog = &this->fog[i];

        if (group->has_type)
            fog->set_type(group->type);

        if (group->has_density)
            fog->set_density(group->density);

        if (group->has_linear) {
            fog->set_start(group->linear_start);
            fog->set_end(group->linear_end);
        }

        if (group->has_color)
            fog->set_color(group->color);
    }
}

void render_context::light_param_data_glow_set(light_param_glow* glow) {
    post_process_blur* blur = post_process.blur;
    post_process_tone_map* tone_map = post_process.tone_map;

    tone_map->set_auto_exposure(true);
    tone_map->set_tone_map_method(TONE_MAP_YCC_EXPONENT);
    tone_map->set_saturate_coeff(1.0f);
    tone_map->set_scene_fade(vec4_null);
    tone_map->set_scene_fade_blend_func(0);
    tone_map->set_tone_trans_start(vec3_null);
    tone_map->set_tone_trans_end(vec3_identity);

    if (glow->has_exposure)
        tone_map->set_exposure(glow->exposure);

    if (glow->has_gamma)
        tone_map->set_gamma(glow->gamma);

    if (glow->has_saturate_power)
        tone_map->set_saturate_power(glow->saturate_power);

    if (glow->has_saturate_coef)
        tone_map->set_saturate_coeff(glow->saturate_coef);

    if (glow->has_flare) {
        tone_map->set_lens_flare(glow->flare.x);
        tone_map->set_lens_shaft(glow->flare.y);
        tone_map->set_lens_ghost(glow->flare.z);
    }

    if (glow->has_sigma)
        blur->set_radius(glow->sigma);

    if (glow->has_intensity)
        blur->set_intensity(glow->intensity);

    if (glow->has_auto_exposure)
        tone_map->set_auto_exposure(glow->auto_exposure);

    if (glow->has_tone_map_method)
        tone_map->set_tone_map_method(glow->tone_map_method);

    if (glow->has_fade_color) {
        vec4 fade_color = glow->fade_color;
        tone_map->set_scene_fade(fade_color);
        tone_map->set_scene_fade_blend_func(glow->fade_color_blend_func);
    }

    if (glow->has_tone_transform)
        tone_map->set_tone_trans(glow->tone_transform_start, glow->tone_transform_end);
}

void render_context::light_param_data_ibl_set(
    light_param_ibl * ibl, light_param_data_storage* storage) {
    if (!ibl->ready)
        return;

    for (int32_t i = 0, j = -1; i < 5; i++, j++) {
        gl_state_bind_texture_cube_map(storage->textures[i]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        if (i == 0) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            render_context_light_param_data_ibl_set_diffuse(&ibl->diffuse[0], 0);
            render_context_light_param_data_ibl_set_diffuse(&ibl->diffuse[1], 1);
        }
        else {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, ibl->specular[j].max_level);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, 0.0f);
            render_context_light_param_data_ibl_set_specular(&ibl->specular[j]);
        }
    }
    gl_state_bind_texture_cube_map(0);

    ::light_set* set = &light_set[LIGHT_SET_MAIN];
    set->set_irradiance(ibl->diff_coef[1][0], ibl->diff_coef[1][1], ibl->diff_coef[1][2]);

    float_t len;
    vec3 pos;

    light_data* l = &set->lights[LIGHT_CHARA];
    l->get_position(pos);
    len = vec3::length(pos);
    if (fabsf(len - 1.0f) < 0.02f)
        l->set_position(ibl->lit_dir[0]);

    l->set_ibl_specular(ibl->lit_col[0]);
    l->set_ibl_back(ibl->lit_col[2]);
    l->set_ibl_direction(ibl->lit_dir[0]);

    l = &set->lights[LIGHT_STAGE];
    l->get_position(pos);
    len = vec3::length(pos);
    if (fabsf(len - 1.0f) < 0.02f)
        l->set_position(ibl->lit_dir[1]);

    l->set_ibl_specular(ibl->lit_col[1]);
    l->set_ibl_direction(ibl->lit_dir[1]);
}

void render_context::light_param_data_wind_set(light_param_wind* w) {
    wind* wind = task_wind->ptr;
    if (w->has_scale)
        wind->scale = w->scale;

    if (w->has_cycle)
        wind->cycle = w->cycle;

    if (w->has_rot) {
        wind->rot_y = w->rot_y;
        wind->rot_z = w->rot_z;
    }

    if (w->has_bias)
        wind->bias = w->bias;

    for (int32_t i = 0; i < 16; i++)
        if (w->has_spc[i]) {
            wind->spc[i].cos = w->spc[i].cos;
            wind->spc[i].sin = w->spc[i].sin;
        }
}

void render_context::light_param_data_face_set(light_param_face* face) {
    this->face.offset = face->offset;
    this->face.scale = face->scale;
    this->face.position = face->position;
    this->face.direction = face->direction;
}

shadow::shadow() : field_8(), field_158(), view_point(), interest(),
field_1A8(), view_point_shared(), interest_shared(), field_2F0() {
    view_region = 1.2f;
    range = 1.0f;
    for (int32_t i = 0; i < 2; i++) {
        view_point[i] = vec3_identity;
        field_1C0[i] = 0.0f;
        field_1C8[i] = 0.0f;
        field_200[i] = i;
    }
    view_mat[0] = mat4_identity;
    view_mat[1] = mat4_identity;
    blur_filter = BLUR_FILTER_9;
    near_blur = 1;
    field_2BC = 2;
    far_blur = 1;
    distance = 4.0f;
    field_2C4 = 0.4f;
    z_near = 0.1f;
    z_far = 20.0f;
    field_2D0 = 1.4f;
    field_2D4 = 10000.0f;
    field_2D8 = 80.0f;
    field_2DC = 2.0f;
    field_2E0 = 0.05f;
    ambient = 0.4f;
    field_2EC = 0;
    direction = vec3(0.0f, -1.0f, -1.0f) * (1.0f / sqrtf(2.0f));
    field_2E8 = false;
    self_shadow = true;
    blur_filter_enable[0] = true;
    blur_filter_enable[1] = true;
    field_2F5 = false;
    field_208 = (z_far - z_near) * 0.5f;
}

shadow::~shadow() {

}

void shadow::ctrl(render_context* rctx) {
    for (int32_t i = 0; i < 2; i++)
        field_2F0[i] = false;

    if (rctx->draw_pass.shadow) {
        view_mat[0] = rctx->camera->view;
        view_mat[1] = rctx->camera->inv_view;

        ::light_set* set = &rctx->light_set[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_CHARA];

        vec3 position;
        data->get_position(position);
        float_t length = vec3::length(position);
        if (length < 0.000001f)
            direction = { 0.0f, 1.0f, 0.0f };
        else
            direction = -position * (1.0f / length);

        for (int32_t i = 0; i < 2; i++)
            if (draw_task_get_count(rctx, (draw_object_type)((int32_t)DRAW_OBJECT_SHADOW_CHARA + i)))
                field_2F0[i] = true;
    }

    int32_t count = 0;
    field_2EC = 0;
    for (int32_t i = 0; i < 2; i++)
        if (field_2F0[i] && field_1D0[i].size() > 0) {
            field_2EC++;
            count += (int32_t)field_1D0[i].size();
        }
        else
            field_2F0[i] = false;

    if (count < 3) {
        for (int32_t i = 0; i < 2; i++) {
            field_1A8[i] = vec3_null;
            field_1C8[i] = 0.0f;
            if (!field_2F0[i] || field_1D0[i].size() < 1)
                continue;

            vec3 v7 = vec3_null;
            for (vec3& j : field_1D0[i])
                v7 += j;

            float_t v14 = (float_t)(int32_t)field_1D0[i].size();
            if (v14 < 0.0f)
                v14 += 1.8446744e19f;
            v7 *= 1.0f / v14;

            float_t v15 = 0.0f;
            for (vec3& j : field_1D0[i]) {
                vec3 v22 = v7 - j;
                vec3 v25 = direction * vec3::dot(v22, direction);
                float_t v24 = vec3::distance(v25, v22);
                v24 -= 0.25f;
                if (v24 < 0.0f)
                    v24 = 0.0f;
                if (v15 < v24)
                    v15 = v24;
            }
            field_1A8[i] = v7;
            field_1C8[i] = v15;
        }

        if (field_2EC > 0) {
            vec3 view_point = vec3_null;
            vec3 interest = vec3_null;
            for (int32_t i = 0; i < 2; i++) {
                if (!field_2F0[i])
                    continue;

                vec3 v11 = field_1A8[i] - direction * field_208;
                float_t v9 = vec3::distance(this->view_point[i], v11);
                float_t v12 = vec3::distance(this->interest[i], field_1A8[i]);
                if (v9 > 0.1f || v12 > 0.1f) {
                    this->view_point[i] = v11;
                    this->interest[i] = field_1A8[i];
                }

                view_point += this->view_point[i];
                interest += this->interest[i];
            }

            view_point_shared = view_point * (1.0f / field_2EC);
            interest_shared = interest * (1.0f / field_2EC);
        }

        float_t v2 = max_def(field_1C8[0], field_1C8[1]);
        field_2F5 = false;
        view_region = v2 + 1.2f;
        field_200[0] = 0;
        field_200[1] = 1;
        if (field_2EC >= 2) {
            vec3 v12 = field_1A8[0] - interest_shared;
            vec3 v14 = field_1A8[1] - interest_shared;
            float_t v15 = vec3::dot(v12, direction);

            vec3 v6 = direction * v15 - v12;

            float_t v16 = vec3::length(v6) - 0.25f;
            if (v16 < 0.0f)
                v16 = 0.0f;

            if (v16 > 1.2f) {
                view_region = v2 + 2.4f;
                field_2F5 = true;
            }
            else
                view_region = v2 + 1.2f + v16;

            if (vec3::dot(v12, direction) < vec3::dot(v14, direction)) {
                field_200[1] = 0;
                field_200[0] = 1;
            }
        }
    }
    else {
        vec3 v3;
        vec3 v86;
        if (direction.y * direction.y < 0.99f) {
            v86 = vec3::cross(direction, vec3(0.0f, 1.0f, 0.0f));
            v3 = vec3::normalize(vec3::cross(v86, direction));
            v86 = vec3::normalize(v86);
        }
        else {
            v3 = { 0.0f, 0.0f, 1.0f };
            v86 = { 1.0f, 0.0f, 0.0f };
        }

        for (int32_t i = 0; i < 2; i++) {
            field_1A8[i] = vec3_null;
            field_1C8[i] = 0.0;
            if (!field_2F0[i] || field_1D0[i].size() < 1)
                continue;

            vec3 v22 = vec3_null;
            for (vec3& j : field_1D0[i])
                v22 += j;

            int32_t v27 = (int32_t)field_1D0[i].size();
            float_t v29 = (float_t)v27;
            if (v27 < 0)
                v29 += 1.8446744e19f;

            float_t v30 = 0.0f;
            vec3 v31 = v22 * (1.0f / v29);

            for (vec3& j : field_1D0[i]) {
                vec3 v34 = v31 - j;
                float_t v38 = fabsf(vec3::dot(v34, v3));
                float_t v39 = fabsf(vec3::dot(v34, v86));
                if (v39 >= v38)
                    v38 = v39;
                if (v30 < v38)
                    v30 = v38;
            }
            field_1A8[i] = v31;
            field_1C8[i] = v30;
        }

        if (field_2EC > 0) {
            for (int32_t i = 0; i < 2; i++) {
                if (!field_2F0[i])
                    continue;

                vec3 v53 = field_1A8[i] -  direction * field_208;
                float_t v51 = vec3::distance(view_point[i], v53);
                float_t v54 = vec3::distance(interest[i], field_1A8[i]);
                if (v51 > 0.1f || v54 > 0.1f) {
                    view_point[i] = v53;
                    interest[i] = field_1A8[i];
                }
            }

            vec3 view_point = vec3_null;
            vec3 interest = vec3_null;
            int32_t count = 0;
            for (int32_t i = 0; i < 2; i++) {
                int32_t c = (int32_t)field_1D0[i].size();
                view_point += this->view_point[i] * (float_t)c;
                interest += this->interest[i] * (float_t)c;
                count += c;
            }

            view_point_shared = view_point * (1.0f / (float_t)count);
            interest_shared = interest * (1.0f / (float_t)count);
        }

        float_t v2 = 0.0f;
        float_t v67 = max_def(field_1C8[0], field_1C8[1]);
        field_2F5 = false;
        view_region = v67 + 1.2f;
        field_200[0] = 0;
        field_200[1] = 1;
        if (field_2EC >= 2) {
            float_t v68 = 0.0f;
            float_t v69 = 0.0f;
            float_t v70 = 0.0f;
            for (int32_t i = 0; i < 2; i++) {
                if (!field_2F0[i])
                    continue;

                for (vec3& j : field_1D0[i]) {
                    vec3 v74 = j - interest_shared;

                    float_t v77 = vec3::dot(v74, v86);
                    if (v2 > v77)
                        v2 = v77;
                    else if (v69 < v77)
                        v69 = v77;

                    float_t v78 = vec3::dot(v74, v3);
                    if (v68 > v78)
                        v68 = v78;
                    else if (v70 < v78)
                        v70 = v78;
                }
            }

            float_t v79 = -v2;
            if (v79 < v69)
                v79 = v69;
            if (v79 < -v68)
                v79 = -v68;
            if (v79 < v70)
                v79 = v70;

            if (v79 > v67 + 1.2f) {
                view_region = v67 + 2.4f;
                field_2F5 = true;
            }
            else
                view_region = v79 + 1.2f;

            if (vec3::dot(field_1A8[0] - interest_shared, direction)
                < vec3::dot(field_1A8[1] - interest_shared, direction)) {
                field_200[1] = 0;
                field_200[0] = 1;
            }
        }
    }

    for (std::vector<vec3>& i : field_1D0)
        i.clear();
}

int32_t shadow::init_data() {
    struct shadow_texture_init_params {
        int32_t width;
        int32_t height;
        int32_t max_level;
        GLenum color_format;
        GLenum depth_format;
    } init_params[] = {
        { 0x800, 0x800, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F },
        { 0x200, 0x200, 3, GL_RGBA8, GL_ZERO },
        { 0x200, 0x200, 3, GL_RGBA8, GL_ZERO },
        { 0x800, 0x800, 0, GL_R32F, GL_ZERO },
        { 0x800, 0x800, 0, GL_R32F, GL_ZERO },
        { 0x200, 0x200, 0, GL_R32F, GL_ZERO },
        { 0x200, 0x200, 0, GL_R32F, GL_ZERO },
    };

    shadow_texture_init_params* v3 = init_params;
    for (int32_t i = 0; i < 7; i++, v3++)
        if (field_8[i].init(v3->width, v3->height,
            v3->max_level, v3->color_format, v3->depth_format) < 0)
            return -1;

    for (int32_t i = 0; i < 3; i++) {
        gl_state_bind_texture_2d(field_8[i].color_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&vec4_identity);
    }
    gl_state_bind_texture_2d(0);
    glGetError();
    return 0;
}

static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level) {
    int32_t size = diffuse->size;
    size_t data_size = size;
    data_size = 4 * data_size * data_size;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 3]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, GL_RGBA16F,
        size, size, 0, GL_RGBA, GL_HALF_FLOAT, &diffuse->data[data_size * 5]);
}

static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular) {
    int32_t size = specular->size;
    int32_t max_level = specular->max_level;
    for (int32_t i = 0; i <= max_level; i++, size /= 2) {
        std::vector<half_t>& data = specular->data[i];
        size_t data_size = size;
        data_size = 4 * data_size * data_size;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 0]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 2]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 3]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 4]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, GL_RGBA16F,
            size, size, 0, GL_RGBA, GL_HALF_FLOAT, &data[data_size * 5]);
    }
}
