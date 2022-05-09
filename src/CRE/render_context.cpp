/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_context.hpp"
#include "Glitter/glitter.hpp"
#include "draw_pass.h"
#include "draw_task.h"
#include "file_handler.hpp"
#include "rob.hpp"
#include "shader_ft.h"
#include "stage.hpp"

float_t delta_frame_history = 0;
int32_t delta_frame_history_int = 0;

static void draw_state_init(draw_state* draw_state);
static void draw_state_stats_update(draw_state* draw_state);

static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level);
static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular);

sss_data::sss_data() : init(), enable(), npr_contour(), param() {
    init = true;
    enable = true;
    npr_contour = true;
    param = { 0.0f, 0.0f, 0.0f, 1.0f };

    render_texture_init(&textures[0], 640, 360, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
    render_texture_init(&textures[1], 320, 180, 0, GL_RGBA16F, GL_ZERO);
    render_texture_init(&textures[2], 320, 180, 0, GL_RGBA16F, GL_ZERO);
    render_texture_init(&textures[3], 320, 180, 0, GL_RGBA16F, GL_ZERO);
}

sss_data::~sss_data() {
    render_texture_free(&textures[0]);
    render_texture_free(&textures[1]);
    render_texture_free(&textures[2]);
    render_texture_free(&textures[3]);
}

draw_pass::draw_pass() : reflect_blur_num(), reflect_blur_filter(), wait_for_gpu(), time(),
reflect_type(), show_vector_flags(), show_vector_length(), show_vector_z_offset(), field_2F8(),
sss_texture(), npr_param(), field_31C(), field_31E(), field_31F(), field_320(), npr() {
    for (bool& i : enable)
        i = true;
    reflect = true;
    refract = true;
    shadow = true;
    opaque_z_sort = true;
    alpha_z_sort = true;
    for (bool& i : draw_pass_3d)
        i = true;
    render_texture_init(&reflect_texture, 512, 256, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
    render_texture_init(&refract_texture, 512, 256, 0, GL_RGBA8, GL_DEPTH24_STENCIL8);
    memset(cpu_time, 0, sizeof(cpu_time));
    memset(gpu_time, 0, sizeof(gpu_time));
    time_struct_init(&time);
    shadow_ptr = new ::shadow;
    if (shadow_ptr)
        shadow_ptr->init_data();
}

draw_pass::~draw_pass() {
    render_texture_free(&reflect_texture);
    render_texture_free(&refract_texture);
    delete shadow_ptr;
}

texture_pattern_struct::texture_pattern_struct() : src(), dst() {

}

texture_pattern_struct::texture_pattern_struct(texture_id src, texture_id dst) : src(src), dst(dst) {

}

light_proj::light_proj(int32_t width, int32_t height) : enable(), texture_id() {
    render_texture_init(&shadow_texture[0],
        2048, 512, 0, GL_R32F, GL_DEPTH24_STENCIL8);
    render_texture_init(&shadow_texture[1],
        2048, 512, 0, GL_R32F, GL_ZERO);
    render_texture_init(&draw_texture,
        width, height, 0, GL_RGBA8, GL_DEPTH24_STENCIL8);
}

light_proj::~light_proj() {
    render_texture_free(&shadow_texture[0]);
    render_texture_free(&shadow_texture[1]);
    render_texture_free(&draw_texture);
}

void light_proj::resize(int32_t width, int32_t height) {
    if (!this)
        return;

    render_texture_init(&draw_texture,
        width, height, 0, GL_RGBA8, GL_DEPTH24_STENCIL8);
}

bool light_proj::set(render_context* rctx) {
    if (!this)
        return false;

    static const GLfloat depth_clear = 1.0f;

    render_texture_bind(&shadow_texture[0], 0);
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
        render_texture_bind(&draw_texture, 0);
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
        shader_state_matrix_set_projection(&shaders_ft, &proj, false);

        mat4 view;
        vec3 up = { 0.0f, 1.0f, 0.0f };
        mat4_look_at(view_point, interest, &up, &view);
        shader_state_matrix_set_modelview(&shaders_ft, 0, &view, true);
    }
}

bool light_proj::set_mat(render_context* rctx, bool set_mat) {
    light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
    light_data* data = &set->lights[LIGHT_PROJECTION];
    if (light_get_type(data) != LIGHT_SPOT)
        return false;

    vec3 position;
    vec3 spot_direction;
    light_get_position(data, &position);
    light_get_spot_direction(data, &spot_direction);

    float_t length;
    vec3_length_squared(spot_direction, length);
    if (length <= 0.000001f)
        return false;

    float_t spot_cutoff = light_get_spot_cutoff(data);
    float_t fov = atanf(tanf(spot_cutoff * DEG_TO_RAD_FLOAT) * 0.25f) * (RAD_TO_DEG_FLOAT * 2.0f);

    vec3 interest;
    vec3_add(position, spot_direction, interest);
    if (set_mat) {
        mat4 mat;
        get_proj_mat(&position, &interest, fov, &mat);
        shader_env_vert_set_ptr_array(&shaders_ft, 24, 4, (vec4*)&mat);
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
    free(data);
}

draw_task* object_data_buffer::add_draw_task(draw_task_type type) {
    if (!data)
        return 0;

    int32_t size = align_val(sizeof(draw_task_type) + sizeof(mat4u) + sizeof(float_t) * 2, 0x08);
    switch (type) {
    case DRAW_TASK_TYPE_OBJECT:
        size += sizeof(draw_object);
        break;
    case DRAW_TASK_TYPE_PRIMITIVE:
        size += sizeof(draw_primitive);
        break;
    case DRAW_TASK_TYPE_PREPROCESS:
        size += sizeof(draw_task_preprocess);
        break;
    case DRAW_TASK_TYPE_OBJECT_TRANSLUCENT:
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

mat4u* object_data_buffer::add_mat4(int32_t count) {
    if (!data)
        return 0;

    int32_t size = sizeof(mat4u) * count;
    if (offset + size > this->size)
        return 0;

    mat4u* mats = (mat4u*)((size_t)data + offset);
    offset += size;
    if (max_offset < offset)
        max_offset = offset;
    return mats;
}

void object_data_buffer::reset() {
    offset = 0;
}

object_data::object_data() {
    draw_task_flags = (::draw_task_flags)0;
    shadow_type = (shadow_type_enum)0;
    field_8 = 0;
    field_C = 0;
    field_230 = -1;
    memset(&passed, 0, sizeof(object_data_culling_info));
    memset(&culled, 0, sizeof(object_data_culling_info));
    memset(&passed_prev, 0, sizeof(object_data_culling_info));
    memset(&culled_prev, 0, sizeof(object_data_culling_info));
    show_alpha_center = false;
    show_mat_center = false;
    object_culling = true;
    object_sort = true;
    chara_color = true;
    morph.value = 0.0f;
    morph.object = object_info();
    object_bounding_sphere_check_func = 0;

    if (buffer.data) {
        texture_pattern_count = 0;
        memset(texture_pattern_array, 0, sizeof(texture_pattern_array));
        texture_transform_count = 0;
        memset(texture_transform_array, 0, sizeof(texture_transform_array));
        texture_color_coeff = vec4_identity;
        texture_color_offset = vec4_null;
        texture_specular_coeff = vec4_identity;
        texture_specular_offset = vec4_null;
        wet_param = 0.0f;
    }
}

object_data::~object_data() {

}

bool object_data::get_chara_color() {
    return chara_color;
}

::draw_task_flags object_data::get_draw_task_flags() {
    return draw_task_flags;
}

void object_data::get_morph(object_info* object, float_t* value) {
    *value = morph.value;
    *object = morph.object;
}

shadow_type_enum object_data::get_shadow_type() {
    return shadow_type;
}

void object_data::get_texture_color_coeff(vec4* value) {
    *value = texture_color_coeff;
}

void object_data::get_texture_color_offset(vec4* value) {
    *value = texture_color_offset;
}

void object_data::get_texture_pattern(int32_t* count, texture_pattern_struct* value) {
    *count = texture_pattern_count;

    for (int32_t i = 0; i < *count; i++)
        value[i] = texture_pattern_array[i];
}

void object_data::get_texture_specular_coeff(vec4* value) {
    *value = texture_specular_coeff;
}

void object_data::get_texture_specular_offset(vec4* value) {
    *value = texture_specular_offset;
}

void object_data::get_texture_transform(int32_t* count, texture_transform_struct* value) {
    *count = texture_transform_count;

    for (int32_t i = 0; i < *count; i++)
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

void object_data::set_texture_color_coeff(vec4* value) {
    texture_color_coeff = *value;
}

void object_data::set_texture_color_offset(vec4* value) {
    texture_color_offset = *value;
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
            texture_pattern_array[i] = texture_pattern_struct();
}

void object_data::set_texture_specular_coeff(vec4* value) {
    texture_specular_coeff = *value;
}

void object_data::set_texture_specular_offset(vec4* value) {
    texture_specular_offset = *value;
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
            texture_transform_array[i] = { -1, mat4u_identity };
}

void object_data::set_wet_param(float_t value) {
    wet_param = value;
}

extern float_t rob_frame;
extern render_context* rctx_ptr;

render_context::render_context() : litproj(), data(),
chara_reflect(), chara_refract(), view_mat(), matrix_buffer()  {
    camera = camera_init();
    draw_state_init(&draw_state);
    glGenVertexArrays(1, &vao);

    face_init(&face);
    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++)
        fog_init(&fog_data[i]);
    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
        light_set_init(&light_set_data[i]);
}

render_context::~render_context() {
    camera_dispose(camera);
    glDeleteVertexArrays(1, &vao);
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

    Glitter::glt_particle_manager.cam_projection = camera->projection;
    Glitter::glt_particle_manager.cam_view = camera->view;
    Glitter::glt_particle_manager.cam_inv_view = camera->inv_view;
    Glitter::glt_particle_manager.cam_inv_view_mat3 = camera->inv_view_mat3;
    Glitter::glt_particle_manager.cam_view_point = camera->view_point;
    Glitter::glt_particle_manager.cam_rotation_y = camera->rotation.y;
    Glitter::glt_particle_manager.rctx = this;

    rctx_ptr = this;
    TaskWork::Ctrl();

    file_handler_storage_ctrl();

    draw_pass.shadow_ptr->ctrl(this);
    draw_state_stats_update(&draw_state);
    object_data.reset();
}

void render_context::disp() {
    rctx_ptr = this;
    TaskWork::Disp();

    post_process.ctrl(camera);
    draw_pass_main(this);
    TaskWork::Basic();
}

void render_context::light_param_data_light_set(light_param_light * light) {
    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        light_param_light_group* group = &light->group[i];
        light_set* set = &light_set_data[i];

        for (int32_t j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            light_param_light_data* data = &group->data[j];
            light_data* light = &set->lights[j];

            if (data->has_type)
                light_set_type(light, data->type);

            if (data->has_ambient) {
                vec4 ambient = data->ambient;
                light_set_ambient(light, &ambient);
            }

            if (data->has_diffuse) {
                vec4 diffuse = data->diffuse;
                light_set_diffuse(light, &diffuse);
            }

            if (data->has_specular) {
                vec4 specular = data->specular;
                light_set_specular(light, &specular);
            }

            if (data->has_position)
                light_set_position(light, &data->position);

            if (data->has_spot_direction)
                light_set_spot_direction(light, &data->spot_direction);

            if (data->has_spot_exponent)
                light_set_spot_exponent(light, data->spot_exponent);

            if (data->has_spot_cutoff)
                light_set_spot_cutoff(light, data->spot_cutoff);

            if (data->has_attenuation)
                light_set_attenuation(light, &data->attenuation);

            light_set_clip_plane(light, data->clip_plane);

            if (data->has_tone_curve)
                light_set_tone_curve(light, &data->tone_curve);
        }
    }
}

void render_context::light_param_data_fog_set(light_param_fog* f) {
    for (int32_t i = FOG_DEPTH; i < FOG_MAX; i++) {
        light_param_fog_group* group = &f->group[i];
        fog* fog = &fog_data[i];

        if (group->has_type)
            fog_set_type(fog, group->type);

        if (group->has_density)
            fog_set_density(fog, group->density);

        if (group->has_linear) {
            fog_set_start(fog, group->linear_start);
            fog_set_end(fog, group->linear_end);
        }

        if (group->has_color) {
            vec4 color = group->color;
            fog_set_color(fog, &color);
        }
    }
}

void render_context::light_param_data_glow_set(light_param_glow* glow) {
    post_process_blur* blur = post_process.blur;
    post_process_tone_map* tone_map = post_process.tone_map;

    post_process_tone_map_set_auto_exposure(tone_map, true);
    post_process_tone_map_set_tone_map_method(tone_map, TONE_MAP_YCC_EXPONENT);
    post_process_tone_map_set_saturate_coeff(tone_map, 1.0f);
    post_process_tone_map_set_scene_fade(tone_map, (vec4*)&vec4_null);
    post_process_tone_map_set_scene_fade_blend_func(tone_map, 0);
    post_process_tone_map_set_tone_trans_start(tone_map, (vec3*)&vec3_null);
    post_process_tone_map_set_tone_trans_end(tone_map, (vec3*)&vec3_identity);

    if (glow->has_exposure)
        post_process_tone_map_set_exposure(tone_map, glow->exposure);

    if (glow->has_gamma)
        post_process_tone_map_set_gamma(tone_map, glow->gamma);

    if (glow->has_saturate_power)
        post_process_tone_map_set_saturate_power(tone_map, glow->saturate_power);

    if (glow->has_saturate_coef)
        post_process_tone_map_set_saturate_coeff(tone_map, glow->saturate_coef);

    if (glow->has_flare) {
        post_process_tone_map_set_lens_flare(tone_map, glow->flare.x);
        post_process_tone_map_set_lens_shaft(tone_map, glow->flare.x);
        post_process_tone_map_set_lens_ghost(tone_map, glow->flare.x);
    }

    if (glow->has_sigma)
        post_process_blur_set_radius(blur, &glow->sigma);

    if (glow->has_intensity)
        post_process_blur_set_intensity(blur, &glow->intensity);

    if (glow->has_auto_exposure)
        post_process_tone_map_set_auto_exposure(tone_map, glow->auto_exposure);

    if (glow->has_tone_map_method)
        post_process_tone_map_set_tone_map_method(tone_map, glow->tone_map_method);

    if (glow->has_fade_color) {
        vec4 fade_color = glow->fade_color;
        post_process_tone_map_set_scene_fade(tone_map, &fade_color);
        post_process_tone_map_set_scene_fade_blend_func(tone_map, glow->fade_color_blend_func);
    }

    if (glow->has_tone_transform) {
        post_process_tone_map_set_tone_trans_start(tone_map, &glow->tone_transform_start);
        post_process_tone_map_set_tone_trans_end(tone_map, &glow->tone_transform_end);
    }
}

void render_context::light_param_data_ibl_set(
    light_param_ibl * ibl, light_param_data_storage * storage) {
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

    light_set* set = &light_set_data[LIGHT_SET_MAIN];
    mat4 irradiance[3];
    irradiance[0] = ibl->diff_coef[1][0];
    irradiance[1] = ibl->diff_coef[1][1];
    irradiance[2] = ibl->diff_coef[1][2];
    light_set_set_irradiance(set, &irradiance[0], &irradiance[1], &irradiance[2]);

    float_t len;
    vec3 pos;

    light_data* l = &set->lights[LIGHT_CHARA];
    light_get_position(l, &pos);
    vec3_length(pos, len);
    if (fabsf(len - 1.0f) < 0.02f) {
        vec4 position = ibl->lit_dir[0];
        light_set_position_vec4(l, &position);
    }

    vec4 ibl_specular = ibl->lit_col[0];
    light_set_ibl_specular(l, &ibl_specular);
    vec4 ibl_back = ibl->lit_col[2];
    light_set_ibl_back(l, &ibl_back);
    vec4 ibl_direction = ibl->lit_dir[0];
    light_set_ibl_direction(l, &ibl_direction);

    l = &set->lights[LIGHT_STAGE];
    light_get_position(l, &pos);
    vec3_length(pos, len);
    if (fabsf(len - 1.0f) < 0.02f) {
        vec4 position = ibl->lit_dir[1];
        light_set_position_vec4(l, &position);
    }

    ibl_specular = ibl->lit_col[1];
    light_set_ibl_specular(l, &ibl_specular);
    ibl_direction = ibl->lit_dir[1];
    light_set_ibl_direction(l, &ibl_specular);
}

void render_context::light_param_data_wind_set(light_param_wind* w) {
    wind* wind = task_wind.ptr;
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
    field_170 = 1.2f;
    field_174 = 1.0f;
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
    direction = { 0.0f, -1.0f, -1.0f };
    vec3_mult_scalar(direction, 1.0f / sqrtf(2.0f), direction);
    field_2E8 = false;
    self_shadow = true;
    blur_filter_enable[0] = true;
    blur_filter_enable[1] = true;
    field_2F5 = false;
    field_208 = (z_far - z_near) * 0.5f;
}

shadow::~shadow() {
    for (render_texture& i : field_8)
        render_texture_free(&i);
}

void shadow::ctrl(render_context* rctx) {
    for (int32_t i = 0; i < 2; i++)
        field_2F0[i] = false;

    if (rctx->draw_pass.shadow) {
        view_mat[0] = rctx->camera->view;
        view_mat[1] = rctx->camera->inv_view;

        light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_CHARA];

        vec3 direction;
        float_t length;
        light_get_position(data, &direction);
        vec3_negate(direction, direction);
        vec3_length(direction, length);
        if (length < 0.000001)
            direction = { 0.0f, 1.0f, 0.0f };
        else
            vec3_mult_scalar(direction, 1.0f / length, direction);

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
                vec3_add(v7, j, v7);

            float_t v14 = (float_t)(int32_t)field_1D0[i].size();
            if (v14 < 0.0f)
                v14 += 1.8446744e19f;
            vec3_mult_scalar(v7, 1.0f / v14, v7);

            float_t v15 = 0.0f;
            for (vec3& j : field_1D0[i]) {
                vec3 v22;
                vec3_sub(v7, j, v22);
                float_t v23;
                vec3_dot(v22, direction, v23);
                vec3 v25;
                vec3_mult_scalar(direction, v23, v25);
                float_t v24;
                vec3_distance(v25, v22, v24);
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

                vec3 v11;
                vec3_mult_scalar(direction, field_208, v11);
                vec3_sub(field_1A8[i], v11, v11);
                float_t v9;
                vec3_distance(this->view_point[i], v11, v9);

                float_t v12;
                vec3_distance(this->interest[i], field_1A8[i], v12);
                if (v9 > 0.1f || v12 > 0.1f) {
                    this->view_point[i] = v11;
                    this->interest[i] = field_1A8[i];
                }

                vec3_add(view_point, this->view_point[i], view_point);
                vec3_add(interest, this->interest[i], interest);
            }

            vec3_mult_scalar(view_point, 1.0f / field_2EC, view_point_shared);
            vec3_mult_scalar(interest, 1.0f / field_2EC, interest_shared);
        }

        float_t v2 = max(field_1C8[0], field_1C8[1]);
        field_2F5 = false;
        field_170 = v2 + 1.2f;
        field_200[0] = 0;
        field_200[1] = 1;
        if (field_2EC >= 2) {
            vec3 v12;
            vec3_sub(field_1A8[0], interest_shared, v12);

            vec3 v14;
            vec3_sub(field_1A8[1], interest_shared, v14);

            float_t v15;
            vec3_dot(v12, direction, v15);

            vec3 v6;
            vec3_mult_scalar(direction, v15, v6);
            vec3_sub(v6, v12, v6);

            float_t v16;
            vec3_length(v6, v16);
            v16 -= 0.25f;
            if (v16 < 0.0f)
                v16 = 0.0f;

            if (v16 > 1.2f) {
                field_170 = v2 + 2.4f;
                field_2F5 = true;
            }
            else
                field_170 = v2 + 1.2f + v16;

            float_t t3;
            float_t t4;
            vec3_dot(v12, direction, t3);
            vec3_dot(v14, direction, t4);
            if (t3 < t4) {
                field_200[1] = 0;
                field_200[0] = 1;
            }
        }
    }
    else {
        vec3 v3;
        vec3 v86;
        if (direction.y * direction.y < 0.99f) {
            vec3 up = { 0.0f, 1.0f, 0.0f };
            vec3_cross(direction, up, v86);
            vec3_cross(v86, direction, v3);
            vec3_normalize(v3, v3);
            vec3_normalize(v86, v86);
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
                vec3_add(v22, j, v22);

            int32_t v27 = (int32_t)field_1D0[i].size();
            float_t v29 = (float_t)v27;
            if (v27 < 0)
                v29 += 1.8446744e19f;

            float_t v30 = 0.0f;
            vec3 v31;
            vec3_mult_scalar(v22, 1.0f / v29, v31);

            for (vec3& j : field_1D0[i]) {
                vec3 v34;
                vec3_sub(v31, j, v34);
                float_t v38;
                float_t v39;
                vec3_dot(v34, v3, v38);
                vec3_dot(v34, v86, v39);
                v38 = fabsf(v38);
                v39 = fabsf(v39);
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

                float_t v51;
                vec3 v53;
                float_t v54;
                vec3_mult_scalar(direction, field_208, v53);
                vec3_sub(field_1A8[i], v53, v53);
                vec3_distance(view_point[i], v53, v51);
                vec3_distance(interest[i], field_1A8[i], v54);
                if (v51 > 0.1f || v54 > 0.1f) {
                    view_point[i] = v53;
                    interest[i] = field_1A8[i];
                }
            }

            vec3 view_point = vec3_null;
            vec3 interest = vec3_null;
            int32_t count = 0;;
            for (int32_t i = 0; i < 2; i++) {
                int32_t c = (int32_t)field_1D0[i].size();
                vec3 view_point_temp;
                vec3 interest_temp;
                vec3_mult_scalar(this->view_point[i], (float_t)c, view_point_temp);
                vec3_mult_scalar(this->interest[i], (float_t)c, interest_temp);
                vec3_add(view_point, view_point_temp, view_point);
                vec3_add(interest, interest_temp, interest);
                count += c;
            }

            vec3_mult_scalar(view_point, 1.0f / (float_t)count, view_point_shared);
            vec3_mult_scalar(interest, 1.0f / (float_t)count, interest_shared);
        }

        float_t v2 = 0.0;
        float_t v67 = max(field_1C8[0], field_1C8[1]);
        field_2F5 = false;
        field_170 = v67 + 1.2f;
        field_200[0] = 0;
        field_200[1] = 1;
        if (field_2EC >= 2) {
            float_t v68 = 0.0;
            float_t v69 = 0.0;
            float_t v70 = 0.0;
            for (int32_t i = 0; i < 2; i++) {
                if (!field_2F0[i])
                    continue;

                for (vec3& j : field_1D0[i]) {
                    vec3 v74;
                    vec3_sub(j, interest_shared, v74);

                    float_t v77;
                    vec3_dot(v74, v86, v77);
                    if (v77 < v2)
                        v2 = v77;
                    else if (v69 < v77)
                        v69 = v77;

                    float_t v78;
                    vec3_dot(v74, v3, v78);
                    if (v78 < v68)
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
                field_170 = v67 + 2.4f;
                field_2F5 = true;
            }
            else
                field_170 = v79 + 1.2f;

            vec3 interest_chara;
            vec3 interest_stage;
            float_t interest_chara_cos;
            float_t interest_stage_cos;
            vec3_sub(field_1A8[0], interest_shared, interest_chara);
            vec3_dot(interest_chara, direction, interest_chara_cos);
            vec3_sub(field_1A8[1], interest_shared, interest_stage);
            vec3_dot(interest_stage, direction, interest_stage_cos);
            if (interest_chara_cos < interest_stage_cos) {
                field_200[1] = 0;
                field_200[0] = 1;
            }
        }
    }

    for (std::vector<vec3> i : field_1D0)
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
        { 0x800, 0x800, 0, GL_RGBA8, GL_DEPTH24_STENCIL8 },
        { 0x200, 0x200, 3, GL_RGBA8, GL_ZERO },
        { 0x200, 0x200, 3, GL_RGBA8, GL_ZERO },
        { 0x800, 0x800, 0, GL_R32F, GL_ZERO },
        { 0x800, 0x800, 0, GL_R32F, GL_ZERO },
        { 0x200, 0x200, 0, GL_R32F, GL_ZERO },
        { 0x200, 0x200, 0, GL_R32F, GL_ZERO },
    };

    shadow_texture_init_params* v3 = init_params;
    for (int32_t i = 0; i < 7; i++, v3++)
        if (render_texture_init(&field_8[i], v3->width, v3->height,
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

static void draw_state_init(draw_state* draw_state) {
    memset(draw_state, 0, sizeof(*draw_state));
    draw_state->self_shadow = false;
    draw_state->field_45 = false;
    draw_state->use_global_material = false;
    draw_state->fog_height = false;
    draw_state->ex_data_mat = false;
    draw_state->shader = true;
    draw_state->shader_index = -1;
    draw_state->field_50 = -1;
    draw_state->bump_depth = 1.0f;
    draw_state->intensity = 1.0f;
    draw_state->specular_alpha = 1.0f;
    draw_state->reflect_uv_scale = 0.1f;
    draw_state->refract_uv_scale = 0.1f;
    draw_state->field_68 = 0;
    draw_state->fresnel = 7.0f;
}

static void draw_state_stats_update(draw_state* draw_state) {
    draw_state->stats_prev = draw_state->stats;
    memset(&draw_state->stats, 0, sizeof(draw_state_stats));
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
