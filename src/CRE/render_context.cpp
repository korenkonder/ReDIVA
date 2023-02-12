/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_context.hpp"
#include "../KKdLib/sort.hpp"
#include "Glitter/glitter.hpp"
#include "rob/rob.hpp"
#include "render_manager.hpp"
#include "file_handler.hpp"
#include "shader_ft.hpp"
#include "sound.hpp"
#include "stage.hpp"

float_t delta_frame_history = 0;
int32_t delta_frame_history_int = 0;

extern render_context* rctx_ptr;

static void object_data_get_vertex_attrib_buffer_bindings(const mdl::ObjSubMeshArgs* args,
    int32_t texcoord_array[2], GLuint vertex_attrib_buffer_binding[16]);

static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level);
static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular);

static const GLuint        POSITION_INDEX =  0;
static const GLuint     BONE_WEIGHT_INDEX =  1;
static const GLuint          NORMAL_INDEX =  2;
static const GLuint          COLOR0_INDEX =  3;
static const GLuint          COLOR1_INDEX =  4;
static const GLuint     MORPH_COLOR_INDEX =  5;
static const GLuint         TANGENT_INDEX =  6;
static const GLuint         UNKNOWN_INDEX =  7;
static const GLuint       TEXCOORD0_INDEX =  8;
static const GLuint       TEXCOORD1_INDEX =  9;
static const GLuint  MORPH_POSITION_INDEX = 10;
static const GLuint    MORPH_NORMAL_INDEX = 11;
static const GLuint   MORPH_TANGENT_INDEX = 12;
static const GLuint MORPH_TEXCOORD0_INDEX = 13;
static const GLuint MORPH_TEXCOORD1_INDEX = 14;
static const GLuint      BONE_INDEX_INDEX = 15;

struct struc_189 {
    GLenum type;
    int32_t width;
    int32_t height;
    int32_t max_level;
    GLenum color_format;
    GLenum depth_format;
};

static const struc_189 stru_140A24420[] = {
    { GL_TEXTURE_2D, 0x200, 0x100, 0, GL_RGBA8, GL_DEPTH_COMPONENT24 },
    { GL_TEXTURE_2D, 0x200, 0x100, 0, GL_RGBA16F, GL_DEPTH_COMPONENT24 },
    { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8, GL_ZERO },
    { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8, GL_ZERO },
    { GL_TEXTURE_2D, 0x400, 0x400, 0, GL_RGBA8, GL_ZERO },
    { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA8, GL_ZERO },
    { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA32F, GL_ZERO },
    { GL_TEXTURE_2D, 0x100, 0x100, 0, GL_RGBA8, GL_ZERO },
    { GL_TEXTURE_2D, 0x200, 0x100, 0, GL_RGBA16F, GL_ZERO }, // Extra for buf
};

static const int32_t stru_140A244E0[][3] = {
    { 1, 1, 1 },
    { 0, 0, 0 },
    { 6, 6, 6 },
    { 6, 6, 6 },
    { 6, 6, 6 },
    { 2, 2, 2 },
    { 3, 3, 3 },
    { 4, 4, 4 },
    { 5, 5, 5 },
    { 6, 6, 6 },
    { 7, 7, 7 },
    { 8, 8, 8 }, // Extra for buf
};

draw_state_stats::draw_state_stats() : object_draw_count(), object_translucent_draw_count(),
object_reflect_draw_count(), field_C(), field_10(), draw_count(), draw_triangle_count(), field_1C() {

}

void draw_state_stats::reset() {
    object_draw_count = 0;
    object_translucent_draw_count = 0;
    object_reflect_draw_count = 0;
    field_C = 0;
    field_10 = 0;
    draw_count = 0;
    draw_triangle_count = 0;
    field_1C = 0;
}

sss_data::sss_data() : init(), enable(), npr_contour(), param() {
    init = true;
    enable = true;
    param = { 0.0f, 0.0f, 0.0f, 1.0f };

    textures[0].init(640, 360, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
    textures[1].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[2].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[3].init(320, 180, 0, GL_RGBA16F, GL_ZERO);
}

sss_data::~sss_data() {

}

draw_state::draw_state() : wireframe(), wireframe_overlay(), light(), self_shadow(),
field_45(), use_global_material(), fog_height(), ex_data_mat(), field_68() {
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

void draw_state::set_fog_height(bool value) {
    fog_height = value;
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

namespace mdl {
    void ObjData::init_sub_mesh(DispManager* disp_manager, const mat4* mat,
        float_t radius, obj_sub_mesh* sub_mesh, obj_mesh* mesh, obj_material_data* material,
        std::vector<texture*>* textures, int32_t mat_count, mat4* mats, GLuint array_buffer,
        GLuint element_array_buffer, vec4* blend_color, vec4* emission, int32_t morph_array_buffer,
        int32_t instances_count, mat4* instances_mat, void(*func)(const ObjSubMeshArgs*)) {
        kind = mdl::OBJ_KIND_NORMAL;
        this->mat = *mat;
        this->radius = radius;

        mdl::ObjSubMeshArgs* args = &this->args.sub_mesh;
        args->mesh = mesh;
        args->morph_weight = disp_manager->morph.weight;
        args->material = material;
        args->sub_mesh = sub_mesh;
        args->textures = textures;
        args->mat_count = mat_count;
        args->mats = mats;
        args->array_buffer = array_buffer;
        args->element_array_buffer = element_array_buffer;
        args->morph_array_buffer = morph_array_buffer;

        args->texture_pattern_count = disp_manager->texture_pattern_count;
        for (int32_t i = 0; i < disp_manager->texture_pattern_count && i < TEXTURE_PATTERN_COUNT; i++)
            args->texture_pattern_array[i] = disp_manager->texture_pattern_array[i];

        args->texture_transform_count = disp_manager->texture_transform_count;
        for (int32_t i = 0; i < disp_manager->texture_transform_count && i < TEXTURE_TRANSFORM_COUNT; i++)
            args->texture_transform_array[i] = disp_manager->texture_transform_array[i];

        if (blend_color && *blend_color != 1.0f) {
            args->set_blend_color = true;
            args->blend_color = *blend_color;
        }
        else {
            args->set_blend_color = false;
            args->blend_color = 1.0f;
        }

        args->emission = *emission;

        args->chara_color = disp_manager->chara_color;
        args->self_shadow = disp_manager->draw_task_flags & (DRAW_TASK_8 | DRAW_TASK_4) ? 1 : 0;
        args->shadow = disp_manager->shadow_type;
        args->texture_color_coefficients = disp_manager->texture_color_coefficients;
        args->texture_color_coefficients.w = disp_manager->wet_param;
        args->texture_color_offset = disp_manager->texture_color_offset;
        args->texture_specular_coefficients = disp_manager->texture_specular_coefficients;
        args->texture_specular_offset = disp_manager->texture_specular_offset;
        args->instances_count = instances_count;
        args->instances_mat = instances_mat;
        args->func = func;

        disp_manager->add_vertex_array(args);
    }

    void ObjData::init_translucent(const mat4* mat, ObjTranslucentArgs* translucent) {
        kind = mdl::OBJ_KIND_TRANSLUCENT;
        this->mat = *mat;
        args.translucent = *translucent;
    }

    void ObjData::init_user(const mat4* mat, UserArgsFunc func, void* data) {
        kind = OBJ_KIND_USER;
        this->mat = *mat;
        args.user.func = func;
        args.user.data = data;
    }
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

    static const vec4 color_clear = 1.0f;
    static const GLfloat depth_clear = 1.0f;

    shadow_texture[0].bind();
    glViewport(0, 0, 2048, 512);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, (float_t*)&color_clear);
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
        glClearBufferfv(GL_COLOR, 0, (float_t*)&color_clear);
        glClearBufferfv(GL_DEPTH, 0, &depth_clear);
    }
    return false;
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
    float_t fov = atanf(tanf(spot_cutoff * DEG_TO_RAD_FLOAT) * 0.25f) * 2.0f;

    vec3 interest = position + spot_direction;
    if (set_mat) {
        mat4 temp;
        mat4_translate(0.5f, 0.5f, 0.5f, &temp);
        mat4_scale_rot(&temp, 0.5f, 0.5f, 0.5f, &temp);

        mat4 proj;
        mat4_persp(fov, 4.0f, 0.1f, 10.0f, &proj);
        mat4_mult(&proj, &temp, &proj);

        mat4 view;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        mat4_look_at(&position, &interest, &up, &view);

        mat4 mat;
        mat4_mult(&view, &proj, &mat);
        rctx->obj_scene.set_g_light_projection(mat);
    }
    else {
        mat4_persp(fov, 4.0f, 0.1f, 10.0f, &rctx->proj_mat);

        vec3 up = { 0.0f, 1.0f, 0.0f };
        mat4_look_at(&position, &interest, &up, &rctx->view_mat);
        rctx->obj_scene.set_projection_view(rctx->view_mat, rctx->proj_mat);
    }
    return true;
}

morph_struct::morph_struct() : weight() {

}

texture_data_struct::texture_data_struct() : field_0() {

}

namespace mdl {
    DispManager::DispManager() : draw_task_flags(), shadow_type(), field_8(), field_C(),
        culling(), show_alpha_center(), show_mat_center(),  texture_pattern_count(),
        texture_pattern_array(), wet_param(), texture_transform_count(),
        texture_transform_array(), material_list_count(), material_list_array() {
        field_230 = -1;
        object_culling = true;
        object_sort = true;
        chara_color = true;
        buff_offset = 0;
        buff_max = 0;
        buff_size = 0x300000;
        buff = force_malloc(0x300000);
        texture_color_coefficients = 1.0f;
        texture_color_offset = 0.0f;
        texture_specular_coefficients = 1.0f;
        texture_specular_offset = 0.0f;
    }

    DispManager::~DispManager() {
        for (DispManager::vertex_array& i : vertex_array_cache)
            glDeleteVertexArrays(1, &i.vertex_array);
        vertex_array_cache.clear();
        free_def(buff);
    }

    void DispManager::add_vertex_array(const ObjSubMeshArgs* args) {
        const obj_mesh* mesh = args->mesh;
        const obj_sub_mesh* sub_mesh = args->sub_mesh;
        const obj_material_data* material = args->material;

        GLuint array_buffer = args->array_buffer;
        GLuint morph_array_buffer = args->morph_array_buffer;
        GLuint element_array_buffer = args->element_array_buffer;

        int32_t texcoord_array[2] = { -1, -1 };
        int32_t color_tex_index = 0;
        for (const obj_material_texture_data& i : material->material.texdata) {
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

        GLuint vertex_attrib_buffer_binding[16] = {};
        object_data_get_vertex_attrib_buffer_bindings(args,
            texcoord_array, vertex_attrib_buffer_binding);

        bool compressed = mesh->attrib.m.compressed;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        DispManager::vertex_array* vertex_array = 0;
        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time >= 0 && i.array_buffer == array_buffer && i.morph_array_buffer == morph_array_buffer
                && i.element_array_buffer == element_array_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compressed == compressed
                && !memcmp(i.vertex_attrib_buffer_binding,
                    vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding))
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                if (i.vertex_array) {
                    i.alive_time = 60;
                    return;
                }
                else {
                    vertex_array = &i;
                    break;
                }


        if (!vertex_array)
            for (DispManager::vertex_array& i : vertex_array_cache)
                if (i.alive_time <= 0) {
                    vertex_array = &i;
                    break;
                }

        if (!vertex_array) {
            vertex_array_cache.push_back({});
            vertex_array = &vertex_array_cache.back();
        }

        if (!vertex_array->vertex_array) {
            glGenVertexArrays(1, &vertex_array->vertex_array);

            gl_state_bind_vertex_array(vertex_array->vertex_array);
            glVertexAttrib4f(       POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(    BONE_WEIGHT_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
            glVertexAttrib4f(         NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(         COLOR0_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(         COLOR1_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(    MORPH_COLOR_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(        TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(        UNKNOWN_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f( MORPH_POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(   MORPH_NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(  MORPH_TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(     BONE_INDEX_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        vertex_array->array_buffer = array_buffer;
        vertex_array->morph_array_buffer = morph_array_buffer;
        vertex_array->element_array_buffer = element_array_buffer;
        vertex_array->alive_time = 60;
        vertex_array->vertex_format = vertex_format;
        vertex_array->size_vertex = size_vertex;
        vertex_array->compressed = compressed;
        memcpy(&vertex_array->texcoord_array, texcoord_array, sizeof(texcoord_array));
        memcpy(&vertex_array->vertex_attrib_buffer_binding,
            vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding));

        gl_state_bind_vertex_array(vertex_array->vertex_array);
        gl_state_bind_array_buffer(array_buffer);
        if (element_array_buffer)
            gl_state_bind_element_array_buffer(element_array_buffer, true);

        size_t offset = 0;
        if (vertex_format & OBJ_VERTEX_POSITION) {
            if (!vertex_array->vertex_attrib_array[POSITION_INDEX]) {
                glEnableVertexAttribArray(POSITION_INDEX);
                vertex_array->vertex_attrib_array[POSITION_INDEX] = true;
            }

            glVertexAttribPointer(POSITION_INDEX,
                3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else if (vertex_array->vertex_attrib_array[POSITION_INDEX]) {
            glDisableVertexAttribArray(POSITION_INDEX);
            vertex_array->vertex_attrib_array[POSITION_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_NORMAL) {
            if (!vertex_array->vertex_attrib_array[NORMAL_INDEX]) {
                glEnableVertexAttribArray(NORMAL_INDEX);
                vertex_array->vertex_attrib_array[NORMAL_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }
            else {
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[NORMAL_INDEX]) {
            glDisableVertexAttribArray(NORMAL_INDEX);
            vertex_array->vertex_attrib_array[NORMAL_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_TANGENT) {
            if (!vertex_array->vertex_attrib_array[TANGENT_INDEX]) {
                glEnableVertexAttribArray(TANGENT_INDEX);
                vertex_array->vertex_attrib_array[TANGENT_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[TANGENT_INDEX]) {
            glDisableVertexAttribArray(TANGENT_INDEX);
            vertex_array->vertex_attrib_array[TANGENT_INDEX] = false;
        }

        if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
            offset += 12;

        for (int32_t i = 0; i < 2; i++) {
            int32_t texcoord_index = texcoord_array[i];
            if (texcoord_index < 0) {
                if (vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i]) {
                    glDisableVertexAttribArray(TEXCOORD0_INDEX + i);
                    vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i] = false;
                }
                continue;
            }

            if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index)) {
                if (!vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i]) {
                    glEnableVertexAttribArray(TEXCOORD0_INDEX + i);
                    vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i] = true;
                }

                if (!compressed)
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_FLOAT, GL_FALSE,
                        size_vertex, (void*)(offset + 8ULL * texcoord_index));
                else
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_HALF_FLOAT, GL_FALSE,
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
            if (!vertex_array->vertex_attrib_array[COLOR0_INDEX]) {
                glEnableVertexAttribArray(COLOR0_INDEX);
                vertex_array->vertex_attrib_array[COLOR0_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[COLOR0_INDEX]) {
            glDisableVertexAttribArray(COLOR0_INDEX);
            vertex_array->vertex_attrib_array[COLOR0_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_COLOR1)
            offset += 16;

        if (vertex_format & OBJ_VERTEX_BONE_DATA) {
            if (!vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX]) {
                glEnableVertexAttribArray(BONE_WEIGHT_INDEX);
                vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }

            if (!vertex_array->vertex_attrib_array[BONE_INDEX_INDEX]) {
                glEnableVertexAttribArray(BONE_INDEX_INDEX);
                vertex_array->vertex_attrib_array[BONE_INDEX_INDEX] = true;
            }

            glVertexAttribIPointer(BONE_INDEX_INDEX,
                4, GL_SHORT, size_vertex, (void*)offset);
            offset += 8;
        }
        else {
            if (vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX]) {
                glDisableVertexAttribArray(BONE_WEIGHT_INDEX);
                vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[BONE_INDEX_INDEX]) {
                glDisableVertexAttribArray(BONE_INDEX_INDEX);
                vertex_array->vertex_attrib_array[BONE_INDEX_INDEX] = false;
            }
        }

        if (!compressed && vertex_format & OBJ_VERTEX_UNKNOWN) {
            if (!vertex_array->vertex_attrib_array[UNKNOWN_INDEX]) {
                glEnableVertexAttribArray(UNKNOWN_INDEX);
                vertex_array->vertex_attrib_array[UNKNOWN_INDEX] = true;
            }

            glVertexAttribPointer(UNKNOWN_INDEX,
                4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else if (vertex_array->vertex_attrib_array[UNKNOWN_INDEX]) {
            glDisableVertexAttribArray(UNKNOWN_INDEX);
            vertex_array->vertex_attrib_array[UNKNOWN_INDEX] = false;
        }

        if (morph_array_buffer) {
            gl_state_bind_array_buffer(morph_array_buffer);

            offset = 0;
            if (vertex_format & OBJ_VERTEX_POSITION) {
                if (!vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                    glEnableVertexAttribArray(MORPH_POSITION_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = true;
                }

                glVertexAttribPointer(MORPH_POSITION_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }
            else if (vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                glDisableVertexAttribArray(MORPH_POSITION_INDEX);
                vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                if (!vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                    glEnableVertexAttribArray(MORPH_NORMAL_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 12;
                }
                else {
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                glDisableVertexAttribArray(MORPH_NORMAL_INDEX);
                vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                if (!vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TANGENT_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                }
                else {
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                glDisableVertexAttribArray(MORPH_TANGENT_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = false;
            }

            if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
                offset += 12;

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                if (!vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
                else {
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                if (!vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
                else {
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = false;
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

                if (!vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                    glEnableVertexAttribArray(MORPH_COLOR_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                }
                else {
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                glDisableVertexAttribArray(MORPH_COLOR_INDEX);
                vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = false;
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
            if (vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                glDisableVertexAttribArray(MORPH_POSITION_INDEX);
                vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                glDisableVertexAttribArray(MORPH_NORMAL_INDEX);
                vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                glDisableVertexAttribArray(MORPH_TANGENT_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                glDisableVertexAttribArray(MORPH_COLOR_INDEX);
                vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = false;
            }
        }

        gl_state_bind_array_buffer(0);
        gl_state_bind_vertex_array(0);
        if (element_array_buffer)
            gl_state_bind_element_array_buffer(0);
    }

    ObjData* DispManager::alloc_data(ObjKind kind) {
        if (!buff)
            return 0;

        int32_t size = align_val(sizeof(ObjKind) + sizeof(mat4) + sizeof(float_t) * 2, 0x08);
        switch (kind) {
        case OBJ_KIND_NORMAL:
            size += sizeof(ObjSubMeshArgs);
            break;
        case OBJ_KIND_ETC:
            size += sizeof(EtcObj);
            break;
        case OBJ_KIND_USER:
            size += sizeof(UserArgs);
            break;
        case OBJ_KIND_TRANSLUCENT:
            size += sizeof(ObjTranslucentArgs);
            break;
        default:
            return 0;
        }

        if (buff_offset + size > buff_size)
            return 0;

        ObjData* data = (ObjData*)((size_t)buff + buff_offset);
        buff_offset += size;
        if (buff_max < buff_offset)
            buff_max = buff_offset;
        return data;
    }

    mat4* DispManager::alloc_data(int32_t count) {
        if (!buff)
            return 0;

        int32_t size = sizeof(mat4) * count;
        if (buff_offset + size > buff_size)
            return 0;

        mat4* mats = (mat4*)((size_t)buff + buff_offset);
        buff_offset += size;
        if (buff_max < buff_offset)
            buff_max = buff_offset;
        return mats;
    }

    void DispManager::buffer_reset() {
        buff_offset = 0;
    }

    void DispManager::check_vertex_arrays() {
        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time > 0 && --i.alive_time <= 0) {
                i.array_buffer = 0;
                i.morph_array_buffer = 0;
            }
    }

    void DispManager::draw(ObjType type, int32_t depth_mask, bool a4) {
        if (get_obj_count(type) < 1)
            return;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        bool reflect = uniform_value[U_REFLECT] == 1;
        void(*func)(render_context * rctx, const ObjSubMeshArgs * args) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 6; i++)
            gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        rctx_ptr->obj_scene_ubo.Bind(0);
        rctx_ptr->obj_batch_ubo.Bind(1);
        rctx_ptr->obj_skinning_ubo.Bind(3);

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_NO_SHADOW:
        case OBJ_TYPE_TRANSLUCENT_LOCAL:
            if (depth_mask)
                func = draw_sub_mesh_translucent;
            else
                gl_state_set_depth_mask(GL_FALSE);
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_TRANSPARENT:
        case OBJ_TYPE_TRANSPARENT_LOCAL:
            alpha_test = 1;
            min_alpha = 0.1f;
            break;
        case OBJ_TYPE_SHADOW_CHARA:
        case OBJ_TYPE_SHADOW_STAGE:
        case OBJ_TYPE_SHADOW_OBJECT_CHARA:
        case OBJ_TYPE_SHADOW_OBJECT_STAGE:
            func = draw_sub_mesh_shadow;
            min_alpha = 0.5f;
            break;
        case OBJ_TYPE_TYPE_6:
            func = draw_sub_mesh_translucent;
            gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx_ptr->draw_state.shader_index = SHADER_FT_SIL;
            break;
        case OBJ_TYPE_TYPE_7:
            func = draw_sub_mesh_translucent;
            gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx_ptr->draw_state.shader_index = SHADER_FT_SIL;
            alpha_test = 1;
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx_ptr->render_manager.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx_ptr->render_manager.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx_ptr->render_manager.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            alpha_test = 1;
            min_alpha = 0.1f;
            break;
        case OBJ_TYPE_REFLECT_OPAQUE:
            alpha_test = 1;
            if (!a4)
                func = draw_sub_mesh_reflect_reflect_map;
            break;
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            gl_state_set_depth_mask(GL_FALSE);
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_REFLECT_TRANSPARENT:
        case OBJ_TYPE_REFRACT_TRANSPARENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            break;
        case OBJ_TYPE_SSS:
            func = draw_sub_mesh_sss;
            break;
        case OBJ_TYPE_USER:
            func = draw_sub_mesh_translucent;
            break;
        default:
            break;
        }
        rctx_ptr->obj_batch.g_max_alpha = { 0.0f, 0.0f, 0.0f, min_alpha };
        uniform_value[U_ALPHA_TEST] = alpha_test;

        for (ObjData*& i : obj[type]) {
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                draw_sub_mesh(rctx_ptr, &i->args.sub_mesh, &i->mat, func);
            } break;
            case OBJ_KIND_ETC: {
                draw_object_model_mat_load(rctx_ptr, i->mat);
                //draw_primitive_draw(&i->data.primitive);
            } break;
            case OBJ_KIND_USER: {
                draw_object_model_mat_load(rctx_ptr, i->mat);
                i->args.user.func(rctx_ptr, i->args.user.data);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (uint32_t j = 0; j < i->args.translucent.count; j++)
                    draw_sub_mesh(rctx_ptr, i->args.translucent.sub_mesh[j], &i->mat, func);
            } break;
            }
        }

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_NO_SHADOW:
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            if (!depth_mask)
                gl_state_set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_TYPE_6:
        case OBJ_TYPE_TYPE_7:
            rctx_ptr->draw_state.shader_index = -1;
            gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            gl_state_set_cull_face_mode(GL_BACK);
            break;
        }

        uniform_value_reset();
        shader_opengl::unbind();
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
    }

    void DispManager::draw_translucent(ObjType type, int32_t alpha) {
        if (get_obj_count(type) < 1)
            return;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        bool reflect = uniform_value[U_REFLECT] == 1;
        void(*func)(render_context * rctx, const ObjSubMeshArgs * args) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 6; i++)
            gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL:
            gl_state_set_depth_mask(GL_FALSE);
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL:
            alpha_test = 1;
            min_alpha = 0.1f;
            break;
        default:
            break;
        }
        rctx_ptr->obj_batch.g_max_alpha = { 0.0f, 0.0f, 0.0f, min_alpha };
        uniform_value[U_ALPHA_TEST] = alpha_test;

        for (ObjData*& i : obj[type]) {
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                int32_t a = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
                a = clamp_def(a, 0, 255);
                if (a == alpha) {
                    draw_sub_mesh(rctx_ptr, &i->args.sub_mesh, &i->mat, func);
                }
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (uint32_t j = 0; j < i->args.translucent.count; j++) {
                    ObjSubMeshArgs* args = i->args.translucent.sub_mesh[j];
                    int32_t a = (int32_t)(args->blend_color.w * 255.0f);
                    a = clamp_def(a, 0, 255);
                    if (a == alpha)
                        draw_sub_mesh(rctx_ptr, args, &i->mat, func);
                }
            } break;
            }
        }

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3:
            gl_state_set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3:
            gl_state_set_cull_face_mode(GL_BACK);
            break;
        }

        uniform_value_reset();
        shader_opengl::unbind();
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
    }

    /*void DispManager::draw_show_vector(ObjType type, int32_t show_vector) {
        if (get_obj_count(type) < 1)
            return;

        for (int32_t i = 0; i < 6; i++)
            gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        for (ObjData*& i : obj[type]) {
            switch (i->type) {
            case OBJ_KIND_NORMAL: {
                draw_sub_mesh_show_vector(rctx_ptr, &i->args.sub_mesh,
                    &i->mat, show_vector);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (uint32_t j = 0; j < i->args.translucent.count; j++)
                    draw_sub_mesh_show_vector(rctx_ptr, i->args.translucent.sub_mesh[j],
                        &i->mat, show_vector);
            } break;
            }
        }
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
    }*/

    void DispManager::entry_list(ObjType type, ObjData* data) {
        obj[type].push_back(data);
    }

    static int32_t obj_axis_aligned_bounding_box_check_visibility(
        obj_axis_aligned_bounding_box* aabb, camera* cam, const mat4* mat) {
        vec3 points[8];
        points[0] = aabb->center + (aabb->size ^ vec3(0.0f, 0.0f, 0.0f));
        points[1] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, -0.0f));
        points[2] = aabb->center + (aabb->size ^ vec3(-0.0f, 0.0f, 0.0f));
        points[3] = aabb->center + (aabb->size ^ vec3(0.0f, -0.0f, -0.0f));
        points[4] = aabb->center + (aabb->size ^ vec3(0.0f, -0.0f, 0.0f));
        points[5] = aabb->center + (aabb->size ^ vec3(-0.0f, 0.0f, -0.0f));
        points[6] = aabb->center + (aabb->size ^ vec3(0.0f, 0.0f, -0.0f));
        points[7] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, 0.0f));

        mat4 view_mat;
        mat4_mult(mat, &cam->view, &view_mat);
        for (int32_t i = 0; i < 8; i++)
            mat4_mult_vec3_trans(&view_mat, &points[i], &points[i]);

        vec4 v2[6];
        *(vec3*)&v2[0] = { 0.0f, 0.0f, -1.0f };
        v2[0].w = (float_t)-cam->min_distance;
        *(vec3*)&v2[1] = cam->field_1E4;
        v2[1].w = 0.0f;
        *(vec3*)&v2[2] = cam->field_1F0;
        v2[2].w = 0.0f;
        *(vec3*)&v2[3] = cam->field_1FC;
        v2[3].w = 0.0f;
        *(vec3*)&v2[4] = cam->field_208;
        v2[4].w = 0.0f;
        *(vec3*)&v2[5] = { 0.0f, 0.0f, 1.0f };
        v2[5].w = (float_t)cam->max_distance;

        for (int32_t i = 0; i < 6; i++)
            for (int32_t j = 0; j < 8; j++) {
                float_t v34 = vec3::dot(*(vec3*)&v2[i], points[j]) + v2[i].w;
                if (v34 > 0.0f)
                    break;

                if (j == 7)
                    return 0;
            }
        return 1;
    }

    static int32_t obj_bounding_sphere_check_visibility(obj_bounding_sphere* sphere,
        CullingCheck* culling, camera* cam, const mat4* mat) {
        if (culling->func)
            return culling->func(sphere, &cam->view);

        vec3 center;
        mat4_mult_vec3_trans(mat, &sphere->center, &center);
        mat4_mult_vec3_trans(&cam->view, &center, &center);
        float_t radius = mat4_get_max_scale(mat) * sphere->radius;

        double_t min_depth = (double_t)center.z - (double_t)radius;
        double_t max_depth = (double_t)center.z + (double_t)radius;
        if (-cam->min_distance < min_depth || -cam->max_distance > max_depth)
            return 0;

        float_t v5 = vec3::dot(cam->field_1E4, center);
        if (v5 < -radius)
            return 0;

        float_t v6 = vec3::dot(cam->field_1F0, center);
        if (v6 < -radius)
            return 0;

        float_t v7 = vec3::dot(cam->field_1FC, center);
        if (v7 < -radius)
            return 0;

        float_t v8 = vec3::dot(cam->field_208, center);
        if (v8 < -radius)
            return 0;

        if (-cam->min_distance >= max_depth && -cam->max_distance <= min_depth
            && v5 >= radius && v6 >= radius && v7 >= radius && v8 >= radius)
            return 1;
        return 2;
    }

    bool DispManager::entry_obj(::obj* object, obj_mesh_vertex_buffer* obj_vertex_buf,
        obj_mesh_index_buffer* obj_index_buf, const mat4* mat,
        std::vector<texture*>* textures, vec4* blend_color, mat4* bone_mat, ::obj* object_morph,
        obj_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
        mat4* instances_mat, void(*func)(const ObjSubMeshArgs*), bool enable_bone_mat, bool local) {
        ::camera* cam = rctx_ptr->camera;
        cam->update_data();

        if (!local && object_culling && !instances_count && !bone_mat && (!object
            || !obj_bounding_sphere_check_visibility(
                &object->bounding_sphere, &culling, cam, mat))) {
            culling.culled.objects++;
            return false;
        }
        culling.passed.objects++;

        for (uint32_t i = 0; i < object->num_mesh; i++) {
            obj_mesh* mesh = &object->mesh_array[i];
            obj_mesh* mesh_morph = 0;
            if (obj_vertex_buf && obj_morph_vertex_buf) {
                if (obj_vertex_buf[i].get_size() != obj_morph_vertex_buf[i].get_size())
                    continue;

                if (object_morph && i < object_morph->num_mesh)
                    mesh_morph = &object_morph->mesh_array[i];
            }

            if (!local && object_culling && !instances_count && !bone_mat
                && !obj_bounding_sphere_check_visibility(
                    &mesh->bounding_sphere, &culling, cam, mat)
                && (!mesh_morph || !obj_bounding_sphere_check_visibility(
                    &mesh_morph->bounding_sphere, &culling, cam, mat))) {
                culling.culled.meshes++;
                continue;
            }
            culling.passed.meshes++;

            ObjSubMeshArgs* translucent_priority[40];
            int32_t translucent_priority_count = 0;

            for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                obj_sub_mesh* sub_mesh_morph = 0;
                if (sub_mesh->attrib.m.cloth)
                    continue;

                if (!local && object_culling && !instances_count && !bone_mat) {
                    int32_t v32 = obj_bounding_sphere_check_visibility(
                        &sub_mesh->bounding_sphere, &culling, cam, mat);
                    if (v32 != 2 || (!mesh->attrib.m.billboard && !mesh->attrib.m.billboard_y_axis)) {
                        if (v32 == 2) {
                            if (culling.func)
                                v32 = 1;
                            else
                                v32 = obj_axis_aligned_bounding_box_check_visibility(
                                    &sub_mesh->axis_aligned_bounding_box, cam, mat);
                        }

                        if (!v32) {
                            if (!mesh_morph || j >= mesh_morph->num_submesh) {
                                culling.culled.submesh_array++;
                                continue;
                            }

                            sub_mesh_morph = &mesh_morph->submesh_array[j];
                            if (!sub_mesh_morph) {
                                culling.culled.submesh_array++;
                                continue;
                            }

                            v32 = obj_bounding_sphere_check_visibility(
                                &sub_mesh_morph->bounding_sphere, &culling, cam, mat);
                            if (v32 == 2) {
                                if (culling.func)
                                    v32 = 1;
                                else
                                    v32 = obj_axis_aligned_bounding_box_check_visibility(
                                        &sub_mesh_morph->axis_aligned_bounding_box, cam, mat);
                            }

                            if (!v32) {
                                culling.culled.submesh_array++;
                                continue;
                            }
                        }
                    }
                }
                culling.passed.submesh_array++;

                int32_t num_bone_index = sub_mesh->num_bone_index;
                uint16_t* bone_index = sub_mesh->bone_index_array;

                mat4* mats;
                if (num_bone_index && enable_bone_mat) {
                    mats = alloc_data(num_bone_index);
                    if (bone_mat)
                        for (int32_t k = 0; k < num_bone_index; k++, bone_index++)
                            mats[k] = bone_mat[*bone_index];
                    else
                        for (int32_t k = 0; k < num_bone_index; k++)
                            mats[k] = mat4_identity;
                }
                else {
                    mats = 0;
                    num_bone_index = 0;
                }

                obj_material_data* material = &object->material_array[sub_mesh->material_index];
                ObjData* data = alloc_data(OBJ_KIND_NORMAL);
                if (!data)
                    continue;

                GLuint morph_array_buffer = 0;
                if (obj_morph_vertex_buf)
                    morph_array_buffer = obj_morph_vertex_buf[i].get_buffer();

                GLuint element_array_buffer = 0;
                if (obj_index_buf)
                    element_array_buffer = obj_index_buf[i].buffer;

                GLuint array_buffer = 0;
                if (obj_vertex_buf)
                    array_buffer = obj_vertex_buf[i].get_buffer();

                uint32_t material_hash = hash_utf8_murmurhash(material->material.name);

                material_list_struct* mat_list = 0;
                for (int32_t k = 0; k < material_list_count; k++)
                    if (material_list_array[k].hash == material_hash) {
                        mat_list = &material_list_array[k];
                        break;
                    }

                vec4 _blend_color = 1.0f;
                vec4 _emission = 0.0f;

                if (mat_list) {
                    bool has_blend_color = false;
                    if (mat_list->has_blend_color.x) {
                        _blend_color.x = mat_list->blend_color.x;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.y) {
                        _blend_color.y = mat_list->blend_color.y;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.z) {
                        _blend_color.z = mat_list->blend_color.z;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.w) {
                        _blend_color.w = mat_list->blend_color.w;
                        has_blend_color = true;
                    }

                    if (blend_color) {
                        if (!has_blend_color)
                            _blend_color = *blend_color;
                        else
                            _blend_color *= *blend_color;
                    }

                    bool has_emission = false;
                    if (mat_list->has_emission.x) {
                        _emission.x = mat_list->emission.x;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.y) {
                        _emission.y = mat_list->emission.y;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.z) {
                        _emission.z = mat_list->emission.z;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.w) {
                        _emission.w = mat_list->emission.w;
                        has_emission = true;
                    }

                    if (!has_emission)
                        _emission = material->material.color.emission;
                }
                else {
                    if (blend_color)
                        _blend_color = *blend_color;
                    _emission = material->material.color.emission;
                }

                data->init_sub_mesh(this, mat, object->bounding_sphere.radius, sub_mesh,
                    mesh, material, textures, num_bone_index, mats, array_buffer, element_array_buffer,
                    &_blend_color, &_emission, morph_array_buffer, instances_count, instances_mat, func);

                if (draw_task_flags & DRAW_TASK_SHADOW_OBJECT) {
                    entry_list((ObjType)(OBJ_TYPE_SHADOW_OBJECT_CHARA
                        + shadow_type), data);
                    if (draw_task_flags & DRAW_TASK_USER)
                        entry_list(OBJ_TYPE_USER, data);
                    continue;
                }

                obj_material_attrib_member attrib = material->material.attrib.m;
                if (draw_task_flags & (DRAW_TASK_ALPHA_ORDER_1 | DRAW_TASK_ALPHA_ORDER_2 | DRAW_TASK_ALPHA_ORDER_3)
                    && data->args.sub_mesh.blend_color.w < 1.0f) {
                    if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)) {
                        if (attrib.flag_28 || (attrib.punch_through
                            || !(attrib.alpha_texture | attrib.alpha_material))
                            && !sub_mesh->attrib.m.transparent) {
                            if (!attrib.punch_through) {
                                if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_1, data);
                                else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                                    entry_list(local ? OBJ_TYPE_OPAQUE_ALPHA_ORDER_2_LOCAL
                                        : OBJ_TYPE_OPAQUE_ALPHA_ORDER_2, data);
                                else
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_3, data);
                            }
                            else {
                                if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1, data);
                                else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                                    entry_list(local ? OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL
                                        : OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2, data);
                                else
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3, data);
                            }

                            if (draw_task_flags & DRAW_TASK_SSS)
                                entry_list(OBJ_TYPE_SSS, data);
                        }

                        if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                            entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1, data);
                        else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                            entry_list(local ? OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                                : OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2, data);
                        else
                            entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3, data);
                    }
                }
                else {
                    if (attrib.flag_28 || data->args.sub_mesh.blend_color.w >= 1.0f
                        && (attrib.punch_through || !(attrib.alpha_texture | attrib.alpha_material))
                        && !sub_mesh->attrib.m.transparent) {
                        if (draw_task_flags & DRAW_TASK_SHADOW)
                            entry_list((ObjType)(OBJ_TYPE_SHADOW_CHARA + shadow_type), data);

                        if (draw_task_flags & DRAW_TASK_SSS)
                            entry_list(OBJ_TYPE_SSS, data);

                        if (attrib.punch_through) {
                            if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY))
                                entry_list(local ? OBJ_TYPE_TRANSPARENT_LOCAL
                                    : OBJ_TYPE_TRANSPARENT, data);

                            if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);

                            if (draw_task_flags & DRAW_TASK_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);

                            if (draw_task_flags & DRAW_TASK_REFRACT)
                                entry_list(OBJ_TYPE_REFRACT_TRANSPARENT, data);
                        }
                        else {
                            if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY))
                                entry_list(local ? OBJ_TYPE_OPAQUE_LOCAL
                                    : OBJ_TYPE_OPAQUE, data);

                            if (draw_task_flags & DRAW_TASK_20)
                                entry_list(OBJ_TYPE_TYPE_6, data);

                            if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);

                            if (draw_task_flags & DRAW_TASK_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);

                            if (draw_task_flags & DRAW_TASK_REFRACT)
                                entry_list(OBJ_TYPE_REFRACT_OPAQUE, data);
                        }

                        if (draw_task_flags & DRAW_TASK_USER)
                            entry_list(OBJ_TYPE_USER, data);
                        continue;
                    }
                    else if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)) {
                        if (!attrib.translucent_priority)
                            if (local)
                                entry_list(OBJ_TYPE_TRANSLUCENT_LOCAL, data);
                            else if (mesh->attrib.m.translucent_no_shadow
                                || draw_task_flags & DRAW_TASK_TRANSLUCENT_NO_SHADOW) {
                                entry_list(OBJ_TYPE_TRANSLUCENT_NO_SHADOW, data);
                            }
                            else {
                                entry_list(OBJ_TYPE_TRANSLUCENT, data);
                                entry_list(OBJ_TYPE_TRANSLUCENT_LITPROJ_0, data);
                                entry_list(OBJ_TYPE_TRANSLUCENT_LITPROJ_1, data);
                            }
                        else if (translucent_priority_count < 40)
                            translucent_priority[translucent_priority_count++] = &data->args.sub_mesh;
                    }
                }

                if (draw_task_flags & DRAW_TASK_SHADOW)
                    entry_list((ObjType)(OBJ_TYPE_SHADOW_CHARA
                        + shadow_type), data);
                if (draw_task_flags & DRAW_TASK_40)
                    entry_list(OBJ_TYPE_TYPE_7, data);
                if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                    entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);
                if (draw_task_flags & DRAW_TASK_REFLECT) {
                    if (rctx_ptr->render_manager.reflect_type != STAGE_DATA_REFLECT_REFLECT_MAP)
                        entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);
                    else
                        entry_list(OBJ_TYPE_REFLECT_TRANSLUCENT, data);
                }
                if (draw_task_flags & DRAW_TASK_REFRACT)
                    entry_list(OBJ_TYPE_REFRACT_TRANSLUCENT, data);
                if (draw_task_flags & DRAW_TASK_USER)
                    entry_list(OBJ_TYPE_USER, data);
            }

            if (!translucent_priority_count)
                continue;

            ObjTranslucentArgs translucent_args;
            translucent_args.count = 0;
            for (int32_t j = 62; j; j--)
                for (int32_t k = 0; k < translucent_priority_count; k++) {
                    ObjSubMeshArgs* sub_mesh = translucent_priority[k];
                    if (sub_mesh->material->material.attrib.m.translucent_priority != j)
                        continue;

                    translucent_args.sub_mesh[translucent_args.count] = sub_mesh;
                    translucent_args.count++;
                }

            ObjData* data = alloc_data(OBJ_KIND_TRANSLUCENT);
            if (!data)
                continue;

            data->init_translucent(mat, &translucent_args);
            if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1, data);
            else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                entry_list(local ? OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                    : OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2, data);
            else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_3)
                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3, data);
            else
                entry_list(local ? OBJ_TYPE_TRANSLUCENT_LOCAL
                    : OBJ_TYPE_TRANSLUCENT, data);
        }
        return true;
    }

    void DispManager::entry_obj_by_obj(const mat4* mat,
        ::obj* obj, std::vector<texture*>* textures, obj_mesh_vertex_buffer* obj_vert_buf,
        obj_mesh_index_buffer* obj_index_buf, mat4* bone_mat, float_t alpha) {
        if (!obj)
            return;

        vec4 blend_color = { 1.0f, 1.0f, 1.0f, alpha };
        vec4* blend_color_ptr = alpha < 1.0f ? &blend_color : 0;

        entry_obj(obj, obj_vert_buf, obj_index_buf, mat, textures, blend_color_ptr,
            bone_mat, 0, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info, mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        vec4* blend_color, mat4* bone_mat, int32_t instances_count,
        mat4* instances_mat, void(*func)(const ObjSubMeshArgs*), bool enable_bone_mat, bool local) {
        if (obj_info.id == -1 && obj_info.set_id == -1)
            return false;

        ::obj* object = object_storage_get_obj(obj_info);
        if (!object)
            return false;

        std::vector<texture*>* textures = object_storage_get_obj_set_textures(obj_info.set_id);
        obj_mesh_vertex_buffer* obj_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(obj_info);
        obj_mesh_index_buffer* obj_index_buffer = object_storage_get_obj_mesh_index_buffer(obj_info);

        ::obj* obj_morph = 0;
        obj_mesh_vertex_buffer* obj_morph_vertex_buffer = 0;
        if (morph.object.set_id != -1) {
            obj_morph = object_storage_get_obj(morph.object);
            obj_morph_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(morph.object);
        }

        return entry_obj(object, obj_vertex_buffer, obj_index_buffer,
            mat, textures, blend_color, bone_mat, obj_morph, obj_morph_vertex_buffer,
            instances_count, instances_mat, func, enable_bone_mat, local);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat,
        object_info obj_info, float_t alpha, mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        blend_color.w = alpha;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        float_t r, float_t g, float_t b, float_t a, mat4* bone_mat, bool local) {
        vec4 blend_color = { r, g, b, a };
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true, local);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        vec4* blend_color, mat4* bone_mat, bool local) {
        return entry_obj_by_object_info(mat, obj_info, blend_color, 0, 0, 0, 0, false, local);
    }

    void DispManager::entry_obj_by_object_info_object_skin(object_info obj_info,
        std::vector<texture_pattern_struct>* texture_pattern, texture_data_struct* texture_data, float_t alpha,
        mat4* matrices, mat4* ex_data_matrices, const mat4* mat, const mat4* global_mat) {
        obj_skin* skin = object_storage_get_obj_skin(obj_info);
        if (!skin)
            return;

        obj_skin_set_matrix_buffer(skin, matrices, ex_data_matrices, rctx_ptr->matrix_buffer, mat, global_mat);

        vec4 texture_color_coefficients;
        vec4 texture_color_offset;
        vec4 texture_specular_coefficients;
        vec4 texture_specular_offset;
        if (texture_data && !texture_data->field_0) {
            vec4 value;
            get_texture_color_coeff(texture_color_coefficients);
            *(vec3*)&value = texture_data->texture_color_coefficients * *(vec3*)&texture_color_coefficients;
            value.w = 0.0f;
            set_texture_color_coefficients(value);

            get_texture_color_offset(texture_color_offset);
            *(vec3*)&value = texture_data->texture_color_offset;
            value.w = 0.0f;
            set_texture_color_offset(value);

            get_texture_specular_coeff(texture_specular_coefficients);
            *(vec3*)&value = texture_data->texture_specular_coefficients * *(vec3*)&texture_specular_coefficients;
            value.w = 0.0f;
            set_texture_specular_coefficients(value);

            get_texture_specular_offset(texture_specular_offset);
            *(vec3*)&value = texture_data->texture_specular_offset;
            value.w = 0.0f;
            set_texture_specular_offset(value);
        }

        size_t texture_pattern_count = texture_pattern ? texture_pattern->size() : 0;
        if (texture_pattern && texture_pattern_count)
            set_texture_pattern((int32_t)texture_pattern_count, texture_pattern->data());

        if (fabsf(alpha - 1.0f) > 0.000001f)
            entry_obj_by_object_info(global_mat, obj_info, alpha, rctx_ptr->matrix_buffer);
        else
            entry_obj_by_object_info(global_mat, obj_info, rctx_ptr->matrix_buffer);

        if (texture_pattern && texture_pattern_count)
            set_texture_pattern();

        if (texture_data && !texture_data->field_0) {
            set_texture_color_coefficients(texture_color_coefficients);
            set_texture_color_offset(texture_color_offset);
            set_texture_specular_coefficients(texture_specular_coefficients);
            set_texture_specular_offset(texture_specular_offset);
        }
    }

    void DispManager::entry_obj_user(const mat4* mat, UserArgsFunc func, void* data, ObjType type) {
        ObjData* _data = alloc_data(OBJ_KIND_USER);
        if (_data) {
            _data->init_user(mat, func, data);
            entry_list(type, _data);
        }
    }

    GLuint DispManager::get_vertex_array(const ObjSubMeshArgs* args) {
        const obj_mesh* mesh = args->mesh;
        const obj_sub_mesh* sub_mesh = args->sub_mesh;
        const obj_material_data* material = args->material;

        GLuint array_buffer = args->array_buffer;
        GLuint morph_array_buffer = args->morph_array_buffer;
        GLuint element_array_buffer = args->element_array_buffer;

        int32_t texcoord_array[2] = { -1, -1 };
        int32_t color_tex_index = 0;
        for (const obj_material_texture_data& i : material->material.texdata) {
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

        GLuint vertex_attrib_buffer_binding[16] = {};
        object_data_get_vertex_attrib_buffer_bindings(args,
            texcoord_array, vertex_attrib_buffer_binding);

        bool compressed = mesh->attrib.m.compressed;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        DispManager::vertex_array* vertex_array = 0;
        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time > 0 && i.array_buffer == array_buffer && i.morph_array_buffer == morph_array_buffer
                && i.element_array_buffer == element_array_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compressed == compressed
                && !memcmp(i.vertex_attrib_buffer_binding,
                    vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding))
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                return i.vertex_array;
        return 0;
    }

    bool DispManager::get_chara_color() {
        return chara_color;
    }

    ::draw_task_flags DispManager::get_draw_task_flags() {
        return draw_task_flags;
    }

    void DispManager::get_material_list(int32_t& count, material_list_struct*& value) {
        count = material_list_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = material_list_array[i];
    }

    void DispManager::get_morph(object_info& object, float_t& weight) {
        weight = morph.weight;
        object = morph.object;
    }

    int32_t DispManager::get_obj_count(ObjType type) {
        return (int32_t)obj[type].size();
    }

    shadow_type_enum DispManager::get_shadow_type() {
        return shadow_type;
    }

    void DispManager::get_texture_color_coeff(vec4& value) {
        value = texture_color_coefficients;
    }

    void DispManager::get_texture_color_offset(vec4& value) {
        value = texture_color_offset;
    }

    void DispManager::get_texture_pattern(int32_t& count, texture_pattern_struct*& value) {
        count = texture_pattern_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = texture_pattern_array[i];
    }

    void DispManager::get_texture_specular_coeff(vec4& value) {
        value = texture_specular_coefficients;
    }

    void DispManager::get_texture_specular_offset(vec4& value) {
        value = texture_specular_offset;
    }

    void DispManager::get_texture_transform(int32_t& count, texture_transform_struct*& value) {
        count = texture_transform_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = texture_transform_array[i];
    }

    float_t DispManager::get_wet_param() {
        return wet_param;
    }

    static int mdl_obj_data_sort_quicksort_compare0(void const* src1, void const* src2) {
        float_t d1 = (*(ObjData**)src1)->view_z;
        float_t d2 = (*(ObjData**)src2)->view_z;
        return d1 > d2 ? -1 : (d1 < d2 ? 1 : 0);
    }

    static int mdl_obj_data_sort_quicksort_compare1(void const* src1, void const* src2) {
        float_t d1 = (*(ObjData**)src1)->view_z;
        float_t d2 = (*(ObjData**)src2)->view_z;
        return d1 < d2 ? -1 : (d1 > d2 ? 1 : 0);
    }

    static int mdl_obj_data_sort_quicksort_compare2(void const* src1, void const* src2) {
        float_t r1 = (*(ObjData**)src1)->radius;
        float_t r2 = (*(ObjData**)src2)->radius;
        return r1 > r2 ? -1 : (r1 < r2 ? 1 : 0);
    }

    void DispManager::obj_sort(mat4* view, ObjType type, int32_t compare_func) {
        std::vector<ObjData*>& vec = obj[type];
        if (vec.size() < 1)
            return;

        for (ObjData*& i : vec) {
            vec3 center;
            mat4_get_translation(&i->mat, &center);
            if (i->kind == OBJ_KIND_NORMAL) {
                mat4 mat = i->mat;
                if (i->args.sub_mesh.mesh->attrib.m.billboard)
                    model_mat_face_camera_view(view, &mat, &mat);
                else if (i->args.sub_mesh.mesh->attrib.m.billboard_y_axis)
                    model_mat_face_camera_position(view, &mat, &mat);

                const obj_sub_mesh* sub_mesh = i->args.sub_mesh.sub_mesh;
                if (i->args.sub_mesh.mat_count < 1 || !sub_mesh->num_bone_index)
                    mat4_mult_vec3_trans(&mat, &sub_mesh->bounding_sphere.center, &center);
                else {
                    vec3 center_sum = 0.0f;
                    for (uint32_t j = 0; j < sub_mesh->num_bone_index; j++) {
                        center = sub_mesh->bounding_sphere.center;
                        mat4_mult_vec3_trans(&i->args.sub_mesh.mats[j], &center, &center);
                        center_sum += center;
                    }
                    center_sum *= 1.0f / (float_t)sub_mesh->num_bone_index;
                }
                i->radius = i->args.sub_mesh.mesh->bounding_sphere.radius;
            }

            mat4_mult_vec3_trans(view, &center, &center);
            i->view_z = center.z;
        }

        switch (compare_func) {
        case 0:
            quicksort_custom(vec.data(), vec.size(),
                sizeof(ObjData*), mdl_obj_data_sort_quicksort_compare0);
            break;
        case 1:
            quicksort_custom(vec.data(), vec.size(),
                sizeof(ObjData*), mdl_obj_data_sort_quicksort_compare1);
            break;
        case 2:
            quicksort_custom(vec.data(), vec.size(),
                sizeof(ObjData*), mdl_obj_data_sort_quicksort_compare2);
            break;
        }
    }

    void DispManager::refresh() {
        culling.passed_prev = culling.passed;
        culling.culled_prev = culling.culled;
        culling.passed = {};
        culling.culled = {};

        draw_task_flags = (::draw_task_flags)0;
        field_8 = 0;
        field_C = 0;

        texture_pattern_count = 0;
        memset(texture_pattern_array, 0, sizeof(texture_pattern_array));
        texture_transform_count = 0;
        memset(texture_transform_array, 0, sizeof(texture_transform_array));
        texture_color_coefficients = 1.0f;
        texture_color_offset = 0.0f;
        texture_specular_coefficients = 1.0f;
        texture_specular_offset = 0.0f;

        for (std::vector<ObjData*>& i : obj)
            i.clear();

        buffer_reset();
    }

    void DispManager::set_chara_color(bool value) {
        chara_color = value;
    }

    void DispManager::set_draw_task_flags(::draw_task_flags flags) {
        draw_task_flags = flags;
    }

    void DispManager::set_material_list(int32_t count, material_list_struct* value) {
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

    void DispManager::set_morph(object_info object, float_t weight) {
        morph.weight = weight;
        morph.object = object;
    }

    void DispManager::set_culling_finc(bool(*func)(obj_bounding_sphere*, mat4*)) {
        culling.func = func;
    }

    void DispManager::set_shadow_type(shadow_type_enum type) {
        if (type == SHADOW_CHARA || type == SHADOW_STAGE)
            shadow_type = type;
    }

    void DispManager::set_texture_color_coefficients(vec4& value) {
        texture_color_coefficients = value;
    }

    void DispManager::set_texture_color_offset(vec4& value) {
        texture_color_offset = value;
    }

    void DispManager::set_texture_pattern(int32_t count, texture_pattern_struct* value) {
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

    void DispManager::set_texture_specular_coefficients(vec4& value) {
        texture_specular_coefficients = value;
    }

    void DispManager::set_texture_specular_offset(vec4& value) {
        texture_specular_offset = value;
    }

    void DispManager::set_texture_transform(int32_t count, texture_transform_struct* value) {
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

    void DispManager::set_wet_param(float_t value) {
        wet_param = value;
    }
}

namespace rndr {
    RenderManager::RenderManager() : pass_sw(), reflect_blur_num(), reflect_blur_filter(), sync_gpu(),
        cpu_time(), gpu_time(), time(), draw_pass_3d(), reflect_type(), tex_index(),
        multisample_framebuffer(), multisample_renderbuffer(), multisample(), show_vector_flags(),
        show_vector_length(), show_vector_z_offset(), field_2F8(), effect_texture(), npr_param(),
        field_31C(), field_31D(), field_31E(), field_31F(), field_320(), npr(), samplers() {
        for (bool& i : pass_sw)
            i = true;
        reflect = true;
        refract = true;
        shadow = true;
        opaque_z_sort = true;
        alpha_z_sort = true;
        for (bool& i : draw_pass_3d)
            i = true;

        shadow_ptr = new ::shadow;
        if (shadow_ptr)
            shadow_ptr->init_data();

        for (int32_t i = 0; i < 9; i++) {
            const struc_189* v2 = &stru_140A24420[i];
            if (v2->type != GL_TEXTURE_2D)
                continue;

            render_texture& rt = render_textures[i];
            rt.init(v2->width, v2->height, v2->max_level, v2->color_format, v2->depth_format);
            rt.bind();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        gl_state_bind_framebuffer(0);

        glCreateSamplers(18, samplers);
        for (int32_t i = 0; i < 18; i++) {
            GLuint sampler = samplers[i];

            static const vec4 border_color = 0.0f;
            glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
            glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER,
                i % 2 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLuint wrap_s;
            switch (i / 2 % 3) {
            case 0:
                wrap_s = GL_CLAMP_TO_EDGE;
                break;
            case 1:
                wrap_s = GL_REPEAT;
                break;
            case 2:
                wrap_s = GL_MIRRORED_REPEAT;
                break;
            }
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap_s);

            GLenum wrap_t;
            switch (i / 6 % 3) {
            case 0:
                wrap_t = GL_CLAMP_TO_EDGE;
                break;
            case 1:
                wrap_t = GL_REPEAT;
                break;
            case 2:
                wrap_t = GL_MIRRORED_REPEAT;
                break;
            }
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap_t);
        }
    }

    RenderManager::~RenderManager() {
        glDeleteSamplers(18, samplers);

        if (multisample_framebuffer) {
            glDeleteFramebuffers(1, &multisample_framebuffer);
            multisample_framebuffer = 0;
        }

        if (multisample_renderbuffer) {
            glDeleteRenderbuffers(1, &multisample_renderbuffer);
            multisample_renderbuffer = 0;
        }

        delete shadow_ptr;
    }

    void RenderManager::add_user(int32_t type, void(*func)(void*), void* data) {
        user.push_back({ type, func, data });
    }

    void RenderManager::clear_user(int32_t type) {
        for (std::list<draw_user>::iterator i = user.begin(); i != user.end(); i++)
            if (i->type == type) {
                user.erase(i);
                break;
            }
    }

    render_texture& RenderManager::get_render_texture(int32_t index) {
        return render_textures[stru_140A244E0[index][tex_index[index]]];
    }

    void RenderManager::resize(int32_t width, int32_t height) {
        if (!multisample_framebuffer)
            glGenFramebuffers(1, &multisample_framebuffer);

        if (!multisample_renderbuffer)
            glGenRenderbuffers(1, &multisample_renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, multisample_framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, multisample_renderbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisample_renderbuffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderManager::set_effect_texture(texture* value) {
        effect_texture = value;
    }

    void RenderManager::set_multisample(bool value) {
        multisample = value;
    }

    void RenderManager::set_npr_param(int32_t value) {
        npr_param = value;
    }

    void RenderManager::set_pass_sw(RenderPassID id, bool value) {
        pass_sw[id] = value;
    }

    void RenderManager::set_reflect(bool value) {
        reflect = value;
    }

    void RenderManager::set_reflect_blur(int32_t reflect_blur_num, blur_filter_mode reflect_blur_filter) {
        this->reflect_blur_num = reflect_blur_num;
        this->reflect_blur_filter = reflect_blur_filter;
    }

    void RenderManager::set_reflect_resolution_mode(reflect_refract_resolution_mode mode) {
        tex_index[0] = mode;
    }

    void RenderManager::set_reflect_type(stage_data_reflect_type type) {
        reflect_type = type;
    }

    void RenderManager::set_refract(bool value) {
        refract = value;
    }

    void RenderManager::set_refract_resolution_mode(reflect_refract_resolution_mode mode) {
        tex_index[1] = mode;
    }

    void RenderManager::set_shadow_false() {
        shadow = false;
    }

    void RenderManager::set_shadow_true() {
        shadow = true;
    }
}

extern float_t rob_frame;
extern render_context* rctx_ptr;

void obj_scene_shader_data::set_g_irradiance_r_transforms(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_irradiance_r_transforms[0] = temp.row0;
    g_irradiance_r_transforms[1] = temp.row1;
    g_irradiance_r_transforms[2] = temp.row2;
    g_irradiance_r_transforms[3] = temp.row3;
}

void obj_scene_shader_data::set_g_irradiance_g_transforms(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_irradiance_g_transforms[0] = temp.row0;
    g_irradiance_g_transforms[1] = temp.row1;
    g_irradiance_g_transforms[2] = temp.row2;
    g_irradiance_g_transforms[3] = temp.row3;
}

void obj_scene_shader_data::set_g_irradiance_b_transforms(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_irradiance_b_transforms[0] = temp.row0;
    g_irradiance_b_transforms[1] = temp.row1;
    g_irradiance_b_transforms[2] = temp.row2;
    g_irradiance_b_transforms[3] = temp.row3;
}

void obj_scene_shader_data::set_g_normal_tangent_transforms(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_normal_tangent_transforms[0] = temp.row0;
    g_normal_tangent_transforms[1] = temp.row1;
    g_normal_tangent_transforms[2] = temp.row2;
}

void obj_scene_shader_data::set_g_self_shadow_receivers(int32_t index, const mat4& mat) {
    size_t _index = index * 3LL;

    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_self_shadow_receivers[_index + 0] = temp.row0;
    g_self_shadow_receivers[_index + 1] = temp.row1;
    g_self_shadow_receivers[_index + 2] = temp.row2;
}

void obj_scene_shader_data::set_g_light_projection(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_light_projection[0] = temp.row0;
    g_light_projection[1] = temp.row1;
    g_light_projection[2] = temp.row2;
    g_light_projection[3] = temp.row3;
}

void obj_scene_shader_data::set_projection_view(const mat4& view, const mat4& proj) {
    mat4 temp;
    mat4_transpose(&view, &temp);
    g_view[0] = temp.row0;
    g_view[1] = temp.row1;
    g_view[2] = temp.row2;

    mat4_inverse(&view, &temp);
    mat4_transpose(&temp, &temp);
    g_view_inverse[0] = temp.row0;
    g_view_inverse[1] = temp.row1;
    g_view_inverse[2] = temp.row2;

    mat4_mult(&view, &proj, &temp);
    mat4_transpose(&temp, &temp);
    g_projection_view[0] = temp.row0;
    g_projection_view[1] = temp.row1;
    g_projection_view[2] = temp.row2;
    g_projection_view[3] = temp.row3;
}

void obj_batch_shader_data::set_g_joint(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_joint[0] = temp.row0;
    g_joint[1] = temp.row1;
    g_joint[2] = temp.row2;

    mat4_inverse(&mat, &temp);
    mat4_transpose(&temp, &temp);
    g_joint_inverse[0] = temp.row0;
    g_joint_inverse[1] = temp.row1;
    g_joint_inverse[2] = temp.row2;
}

void obj_batch_shader_data::set_g_texcoord_transforms(int32_t index, const mat4& mat) {
    size_t _index = index * 2LL;

    mat4 temp;
    mat4_transpose(&mat, &temp);
    g_texcoord_transforms[_index + 0] = temp.row0;
    g_texcoord_transforms[_index + 1] = temp.row1;
}

void obj_batch_shader_data::set_transforms(const mat4& model, const mat4& view, const mat4& proj) {
    mat4 temp;
    mat4_transpose(&model, &temp);
    g_worlds[0] = temp.row0;
    g_worlds[1] = temp.row1;
    g_worlds[2] = temp.row2;

    mat4_inverse(&model, &temp);
    g_worlds_invtrans[0] = temp.row0;
    g_worlds_invtrans[1] = temp.row1;
    g_worlds_invtrans[2] = temp.row2;

    mat4 mv;
    mat4_mult(&model, &view, &mv);

    mat4_transpose(&mv, &temp);
    g_worldview[0] = temp.row0;
    g_worldview[1] = temp.row1;
    g_worldview[2] = temp.row2;

    mat4_inverse(&mv, &temp);
    mat4_transpose(&temp, &temp);
    g_worldview_inverse[0] = temp.row0;
    g_worldview_inverse[1] = temp.row1;
    g_worldview_inverse[2] = temp.row2;

    mat4_mult(&mv, &proj, &temp);
    mat4_transpose(&temp, &temp);
    g_transforms[0] = temp.row0;
    g_transforms[1] = temp.row1;
    g_transforms[2] = temp.row2;
    g_transforms[3] = temp.row3;
}

render_context::render_context() : litproj(), chara_reflect(),
chara_refract(), view_mat(), matrix_buffer(), box_vao(), box_vbo() {
    camera = new ::camera;

    static const float_t box_texcoords[] = {
         1.0f,  0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -1.5f, -1.5f,  0.5f,  1.5f, -0.5f, -0.5f,  1.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -1.5f, -1.5f,  0.5f,  1.5f, -0.5f, -0.5f,  1.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -1.5f, -1.5f,  0.5f,  1.5f, -0.5f, -0.5f,  1.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -1.5f, -1.5f,  0.5f,  1.5f, -0.5f, -0.5f,  1.5f,
         0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         3.5f, -3.5f, -1.5f, -3.5f,  1.5f, -1.5f, -3.5f, -1.5f,
         3.5f,  1.5f, -1.5f,  1.5f,  1.5f,  3.5f, -3.5f,  3.5f,
         3.5f, -3.5f, -1.5f, -3.5f,  1.5f, -1.5f, -3.5f, -1.5f,
         3.5f,  1.5f, -1.5f,  1.5f,  1.5f,  3.5f, -3.5f,  3.5f,
         3.5f, -3.5f, -1.5f, -3.5f,  1.5f, -1.5f, -3.5f, -1.5f,
         3.5f,  1.5f, -1.5f,  1.5f,  1.5f,  3.5f, -3.5f,  3.5f,
         3.5f, -3.5f, -1.5f, -3.5f,  1.5f, -1.5f, -3.5f, -1.5f,
         3.5f,  1.5f, -1.5f,  1.5f,  1.5f,  3.5f, -3.5f,  3.5f,
    };

    glGenVertexArrays(1, &box_vao);
    gl_state_bind_vertex_array(box_vao);

    glGenBuffers(1, &box_vbo);
    gl_state_bind_array_buffer(box_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(box_texcoords), box_texcoords, 0);
    else
        glBufferData(GL_ARRAY_BUFFER, sizeof(box_texcoords), box_texcoords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 64, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 64, (void*)16);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 64, (void*)32);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 64, (void*)48);
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    contour_coef_ubo.Create(sizeof(contour_coef_shader_data));
    contour_params_ubo.Create(sizeof(contour_params_shader_data));
    filter_scene_ubo.Create(sizeof(filter_scene_shader_data));
    esm_filter_batch_ubo.Create(sizeof(esm_filter_batch_shader_data));
    imgfilter_batch_ubo.Create(sizeof(imgfilter_batch_shader_data));
    glass_eye_batch_ubo.Create(sizeof(glass_eye_batch_shader_data));
    quad_ubo.Create(sizeof(quad_shader_data));
    sss_filter_gaussian_coef_ubo.Create(sizeof(sss_filter_gaussian_coef_shader_data));
    transparency_batch_ubo.Create(sizeof(transparency_batch_shader_data));

    obj_scene = {};
    obj_batch = {};
    obj_skinning = {};
    obj_scene_ubo.Create(sizeof(obj_scene_shader_data));
    obj_batch_ubo.Create(sizeof(obj_batch_shader_data));
    obj_skinning_ubo.Create(sizeof(obj_skinning_shader_data));
}

render_context::~render_context() {
    obj_skinning_ubo.Destroy();
    obj_batch_ubo.Destroy();
    obj_scene_ubo.Destroy();

    transparency_batch_ubo.Destroy();
    sss_filter_gaussian_coef_ubo.Destroy();
    quad_ubo.Destroy();
    glass_eye_batch_ubo.Destroy();
    imgfilter_batch_ubo.Destroy();
    esm_filter_batch_ubo.Destroy();
    filter_scene_ubo.Destroy();
    contour_params_ubo.Destroy();
    contour_coef_ubo.Destroy();

    glDeleteVertexArrays(1, &box_vao);
    glDeleteBuffers(1, &box_vbo);

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
}

void render_context::disp() {
    rctx_ptr = this;
    disp_manager.refresh();
    draw_state.stats_prev = draw_state.stats;
    draw_state.stats.reset();
    app::TaskWork::Disp();
    render_manager.shadow_ptr->ctrl(this);
    post_process.ctrl(camera);
    render_manager.render_all(this);
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
    tone_map->set_scene_fade(0.0f);
    tone_map->set_scene_fade_blend_func(0);
    tone_map->set_tone_trans_start(0.0f);
    tone_map->set_tone_trans_end(1.0f);

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

    l->set_ibl_color0(ibl->lit_col[0]);
    l->set_ibl_color1(ibl->lit_col[2]);
    l->set_ibl_direction(ibl->lit_dir[0]);

    l = &set->lights[LIGHT_STAGE];
    l->get_position(pos);
    len = vec3::length(pos);
    if (fabsf(len - 1.0f) < 0.02f)
        l->set_position(ibl->lit_dir[1]);

    l->set_ibl_color0(ibl->lit_col[1]);
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
        view_point[i] = 1.0f;
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

    if (rctx->render_manager.shadow) {
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
            if (rctx->disp_manager.get_obj_count((mdl::ObjType)((int32_t)mdl::OBJ_TYPE_SHADOW_CHARA + i)))
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
            field_1A8[i] = 0.0f;
            field_1C8[i] = 0.0f;
            if (!field_2F0[i] || field_1D0[i].size() < 1)
                continue;

            vec3 v7 = 0.0f;
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
            vec3 view_point = 0.0f;
            vec3 interest = 0.0f;
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

            view_point_shared = view_point * (1.0f / (float_t)field_2EC);
            interest_shared = interest * (1.0f / (float_t)field_2EC);
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
            field_1A8[i] = 0.0f;
            field_1C8[i] = 0.0;
            if (!field_2F0[i] || field_1D0[i].size() < 1)
                continue;

            vec3 v22 = 0.0f;
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

            vec3 view_point = 0.0f;
            vec3 interest = 0.0f;
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
        { 0x200, 0x200, 3, GL_RGBA8, GL_ZERO }, // Extra for buf
    };

    shadow_texture_init_params* v3 = init_params;
    for (int32_t i = 0; i < 8; i++, v3++)
        if (field_8[i].init(v3->width, v3->height,
            v3->max_level, v3->color_format, v3->depth_format) < 0)
            return -1;

    for (int32_t i = 0; i < 4; i++) {
        gl_state_bind_texture_2d(field_8[i == 3 ? 7 : i].color_texture->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        static const vec4 border_color = 1.0f;
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
    }
    gl_state_bind_texture_2d(0);
    glGetError();
    return 0;
}

static void object_data_get_vertex_attrib_buffer_bindings(const mdl::ObjSubMeshArgs* args,
    int32_t texcoord_array[2], GLuint vertex_attrib_buffer_binding[16]) {
    const obj_mesh* mesh = args->mesh;
    const obj_sub_mesh* sub_mesh = args->sub_mesh;

    GLuint array_buffer = args->array_buffer;
    GLuint morph_array_buffer = args->morph_array_buffer;

    bool compressed = mesh->attrib.m.compressed;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (vertex_format & OBJ_VERTEX_POSITION)
        vertex_attrib_buffer_binding[POSITION_INDEX] = array_buffer;
    if (vertex_format & OBJ_VERTEX_NORMAL)
        vertex_attrib_buffer_binding[NORMAL_INDEX] = array_buffer;
    if (vertex_format & OBJ_VERTEX_TANGENT)
        vertex_attrib_buffer_binding[TANGENT_INDEX] = array_buffer;

    for (int32_t i = 0; i < 2; i++) {
        int32_t texcoord_index = texcoord_array[i];
        if (texcoord_index >= 0)
            if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index))
                vertex_attrib_buffer_binding[TEXCOORD0_INDEX + i] = array_buffer;
    }

    if (vertex_format & OBJ_VERTEX_COLOR0)
        vertex_attrib_buffer_binding[COLOR0_INDEX] = array_buffer;

    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        vertex_attrib_buffer_binding[BONE_WEIGHT_INDEX] = array_buffer;
        vertex_attrib_buffer_binding[BONE_INDEX_INDEX] = array_buffer;
    }

    if (!compressed && vertex_format & OBJ_VERTEX_UNKNOWN)
        vertex_attrib_buffer_binding[UNKNOWN_INDEX] = array_buffer;

    if (args->morph_array_buffer) {
        if (vertex_format & OBJ_VERTEX_POSITION)
            vertex_attrib_buffer_binding[MORPH_POSITION_INDEX] = morph_array_buffer;
        if (vertex_format & OBJ_VERTEX_NORMAL)
            vertex_attrib_buffer_binding[MORPH_NORMAL_INDEX] = morph_array_buffer;
        if (vertex_format & OBJ_VERTEX_TANGENT)
            vertex_attrib_buffer_binding[MORPH_TANGENT_INDEX] = morph_array_buffer;
        if (vertex_format & OBJ_VERTEX_TEXCOORD0)
            vertex_attrib_buffer_binding[MORPH_TEXCOORD0_INDEX] = morph_array_buffer;
        if (vertex_format & OBJ_VERTEX_TEXCOORD1)
            vertex_attrib_buffer_binding[MORPH_TEXCOORD1_INDEX] = morph_array_buffer;
        if (vertex_format & OBJ_VERTEX_COLOR0)
            vertex_attrib_buffer_binding[MORPH_COLOR_INDEX] = morph_array_buffer;
    }
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
