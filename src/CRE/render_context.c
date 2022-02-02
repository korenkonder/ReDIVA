/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_context.h"
#include "draw_task.h"
#include "shader_ft.h"

vector_func(texture_pattern_struct)
vector_func(texture_transform_struct)
vector_ptr_func(draw_task)

static void draw_pass_init(draw_pass* draw_pass);
static void draw_pass_free(draw_pass* draw_pass);
static void draw_state_init(draw_state* draw_state);
static void draw_state_stats_update(draw_state* draw_state);

static void object_data_init(object_data* data);
static void object_data_reset(object_data* data);
static void object_data_free(object_data* data);

static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level);
static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular);

static shadow* shadow_init();
static void shadow_update_inner(shadow* shad, render_context* rctx);
static int32_t shadow_init_data(shadow* shad);
static void shadow_free(shadow* shad);

static void sss_data_init(sss_data_struct* sss);
static void sss_data_free(sss_data_struct* sss);

const texture_pattern_struct texture_pattern_struct_null = { -1, -1 };

frame_rate_control sys_frame_rate = { 1.0f };
frame_rate_control diva_pv_frame_rate = { 1.0f };
frame_rate_control diva_stage_frame_rate = { 1.0f };

static const GLfloat depth_clear = 1.0f;

inline float_t frame_rate_control_get_delta_frame(frame_rate_control* control) {
    return get_delta_frame() * control->frame_speed;
}

inline void frame_rate_control_set_frame_speed(frame_rate_control* control, float_t value) {
    control->frame_speed = value;
}

light_proj* light_proj_init(int32_t width, int32_t height) {
    light_proj* litproj = force_malloc(sizeof(light_proj));
    render_texture_init(&litproj->shadow_texture[0],
        2048, 512, 0, GL_R32F, GL_DEPTH24_STENCIL8);
    render_texture_init(&litproj->shadow_texture[1],
        2048, 512, 0, GL_R32F, GL_ZERO);
    render_texture_init(&litproj->draw_texture,
        width, height, 0, GL_RGBA8, GL_DEPTH24_STENCIL8);
    return litproj;
}

void light_proj_get_proj_mat(vec3* view_point, vec3* interest, float_t fov, mat4* mat) {
    if (mat) {
        mat4 temp;
        mat4_translate(0.5f, 0.5f, 0.5f, &temp);
        mat4_scale_rot(&temp, 0.5f, 0.5f, 0.5f, &temp);

        mat4 proj;
        mat4_persp(fov, 4.0, 0.1000000014901161, 10.0, &proj);
        mat4_mult(&proj, &temp, &proj);

        mat4 view;
        mat4_look_at(view_point, interest, &((vec3) { 0.0f, 1.0f, 0.0f }), &view);
        mat4_mult(&view, &proj, mat);
    }
    else {
        mat4 proj;
        mat4_persp(fov, 4.0, 0.1000000014901161, 10.0, &proj);
        shader_state_matrix_set_projection(&shaders_ft, &proj, false);

        mat4 view;
        mat4_look_at(view_point, interest, &((vec3) { 0.0f, 1.0f, 0.0f }), &view);
        shader_state_matrix_set_modelview(&shaders_ft, 0, &view, true);
    }
}

void light_proj_resize(light_proj* litproj, int32_t width, int32_t height) {
    if (!litproj)
        return;

    render_texture_init(&litproj->draw_texture,
        width, height, 0, GL_RGBA8, GL_DEPTH24_STENCIL8);
}

bool light_proj_set(light_proj* litproj, render_context* rctx) {
    if (!litproj)
        return 0;

    render_texture_bind(&litproj->shadow_texture[0], 0);
    glViewport(0, 0, 2048, 512);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, (float_t*)&vec4_identity);
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);

    if (light_proj_set_mat(rctx, false)) {
        rctx->draw_state.shader_index = SHADER_FT_SIL;
        uniform_value[U0A] = 1;
        return true;
    }
    else {
        render_texture_bind(&litproj->draw_texture, 0);
        glViewport(0, 0, litproj->draw_texture.color_texture->width,
            litproj->draw_texture.color_texture->height);
        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);
        glClearBufferfv(GL_COLOR, 0, (float_t*)&vec4_identity);
        glClearBufferfv(GL_DEPTH, 0, &depth_clear);
        return false;
    }
}

bool light_proj_set_mat(render_context* rctx, bool set_mat) {
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
        light_proj_get_proj_mat(&position, &interest, fov, &mat);
        shader_env_vert_set_ptr_array(&shaders_ft, 24, 4, (vec4*)&mat);
    }
    else
        light_proj_get_proj_mat(&position, &interest, fov, 0);
    return true;
}

void light_proj_free(light_proj* litproj) {
    if (!litproj)
        return;

    render_texture_free(&litproj->shadow_texture[0]);
    render_texture_free(&litproj->shadow_texture[1]);
    render_texture_free(&litproj->draw_texture);
    free(litproj);
}

inline draw_task* object_data_buffer_add_draw_task(object_data_buffer* buffer, draw_task_type type) {
    if (!buffer->data)
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

    if (buffer->offset + size > buffer->size)
        return 0;

    draw_task* task = (draw_task*)((size_t)buffer->data + buffer->offset);
    buffer->offset += size;
    if (buffer->max_offset < buffer->offset)
        buffer->max_offset = buffer->offset;
    return task;
}

inline mat4u* object_data_buffer_add_mat4(object_data_buffer* buffer, int32_t count) {
    if (!buffer->data)
        return 0;

    int32_t size = sizeof(mat4u) * count;
    if (buffer->offset + size > buffer->size)
        return 0;

    mat4u* mats = (mat4u*)((size_t)buffer->data + buffer->offset);
    buffer->offset += size;
    if (buffer->max_offset < buffer->offset)
        buffer->max_offset = buffer->offset;
    return mats;
}

inline void object_data_buffer_reset(object_data_buffer* buffer) {
    buffer->offset = 0;
}

inline bool object_data_get_chara_color(object_data* object_data) {
    return object_data->chara_color;
}

inline draw_task_flags object_data_get_draw_task_flags(object_data* object_data) {
    return object_data->draw_task_flags;
}

inline void object_data_get_morph(object_data* object_data, object_info* object, float_t* value) {
    *value = object_data->morph.value;
    *object = object_data->morph.object;
}

inline shadow_type_enum object_data_get_shadow_type(object_data* object_data) {
    return object_data->shadow_type;
}

inline void object_data_get_texture_color_coeff(object_data* object_data, vec4* value) {
    *value = object_data->texture_color_coeff;
}

inline void object_data_get_texture_color_offset(object_data* object_data, vec4* value) {
    *value = object_data->texture_color_offset;
}

void object_data_get_texture_pattern(object_data* object_data,
    int32_t* count, texture_pattern_struct* value) {
    *count = object_data->texture_pattern_count;

    for (int32_t i = 0; i < *count; i++)
        value[i] = object_data->texture_pattern_array[i];
}

inline void object_data_get_texture_specular_coeff(object_data* object_data, vec4* value) {
    *value = object_data->texture_specular_coeff;
}

inline void object_data_get_texture_specular_offset(object_data* object_data, vec4* value) {
    *value = object_data->texture_specular_offset;
}

void object_data_get_texture_transform(object_data* object_data,
    int32_t* count, texture_transform_struct* value) {
    *count = object_data->texture_transform_count;

    for (int32_t i = 0; i < *count; i++)
        value[i] = object_data->texture_transform_array[i];
}

inline float_t object_data_get_wet_param(object_data* object_data) {
    return object_data->wet_param;
}

inline void object_data_set_chara_color(object_data* object_data, bool value) {
    object_data->chara_color = value;
}

inline void object_data_set_draw_task_flags(object_data* object_data, draw_task_flags flags) {
    object_data->draw_task_flags = flags;
}

inline void object_data_set_morph(object_data* object_data, object_info object, float_t value) {
    object_data->morph.value = value;
    object_data->morph.object = object;
}

inline void object_data_set_shadow_type(object_data* object_data, shadow_type_enum type) {
    if (type == SHADOW_CHARA || type == SHADOW_STAGE)
        object_data->shadow_type = type;
}

inline void object_data_set_texture_color_coeff(object_data* object_data, vec4* value) {
    object_data->texture_color_coeff = *value;
}

inline void object_data_set_texture_color_offset(object_data* object_data, vec4* value) {
    object_data->texture_color_offset = *value;
}

void object_data_set_texture_pattern(object_data* object_data,
    int32_t count, texture_pattern_struct* value) {
    if (count > TEXTURE_PATTERN_COUNT)
        return;

    object_data->texture_pattern_count = count;

    if (count)
        for (int32_t i = 0; i < count; i++)
            object_data->texture_pattern_array[i] = value[i];
    else
        for (int32_t i = 0; i < TEXTURE_PATTERN_COUNT; i++)
            object_data->texture_pattern_array[i] = texture_pattern_struct_null;
}

inline void object_data_set_texture_specular_coeff(object_data* object_data, vec4* value) {
    object_data->texture_specular_coeff = *value;
}

inline void object_data_set_texture_specular_offset(object_data* object_data, vec4* value) {
    object_data->texture_specular_offset = *value;
}

void object_data_set_texture_transform(object_data* object_data,
    int32_t count, texture_transform_struct* value) {
    if (count > TEXTURE_TRANSFORM_COUNT)
        return;

    object_data->texture_transform_count = count;

    if (count)
        for (int32_t i = 0; i < count; i++)
            object_data->texture_transform_array[i] = value[i];
    else
        for (int32_t i = count; i < TEXTURE_TRANSFORM_COUNT; i++)
            object_data->texture_transform_array[i] = (texture_transform_struct){ -1, mat4u_identity };
}

inline void object_data_set_wet_param(object_data* object_data, float_t value) {
    object_data->wet_param = value;
}

inline render_context* render_context_init() {
    render_context* rctx = force_malloc(sizeof(render_context));
    rctx->camera = camera_init();
    draw_pass_init(&rctx->draw_pass);
    draw_state_init(&rctx->draw_state);
    object_data_init(&rctx->object_data);
    glGenVertexArrays(1, &rctx->vao);
    glGenTextures(5, rctx->ibl_tex);

    face_init(&rctx->face);
    for (fog_id i = FOG_DEPTH; i < FOG_MAX; i++)
        fog_init(&rctx->fog_data[i]);
    for (light_set_id i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++)
        light_set_init(&rctx->light_set_data[i]);
    rctx->wind = wind_init();

    post_process_init(&rctx->post_process);
    return rctx;
}

void render_context_light_param_data_light_set(render_context* rctx, light_param_light* light) {
    for (light_set_id i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        light_param_light_group* group = &light->group[i];
        light_set* set = &rctx->light_set_data[i];

        for (light_id j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            light_param_light_data* data = &group->data[j];
            light_data* light = &set->lights[j];

            if (data->has_type)
                light_set_type(light, data->type);

            if (data->has_ambient) {
                vec4 ambient;
                vec4u_to_vec4(data->ambient, ambient);
                light_set_ambient(light, &ambient);
            }

            if (data->has_diffuse) {
                vec4 diffuse;
                vec4u_to_vec4(data->diffuse, diffuse);
                light_set_diffuse(light, &diffuse);
            }

            if (data->has_specular) {
                vec4 specular;
                vec4u_to_vec4(data->specular, specular);
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

void render_context_light_param_data_fog_set(render_context* rctx, light_param_fog* f) {
    for (fog_id i = FOG_DEPTH; i < FOG_MAX; i++) {
        light_param_fog_group* group = &f->group[i];
        fog* fog = &rctx->fog_data[i];

        if (group->has_type)
            fog_set_type(fog, group->type);

        if (group->has_density)
            fog_set_density(fog, group->density);

        if (group->has_linear) {
            fog_set_start(fog, group->linear_start);
            fog_set_end(fog, group->linear_end);
        }

        if (group->has_color) {
            vec4 color;
            vec4u_to_vec4(group->color, color);
            fog_set_color(fog, &color);
        }
    }
}

void render_context_light_param_data_glow_set(render_context* rctx, light_param_glow* glow) {
    post_process_struct* pp = &rctx->post_process;

    post_process_blur* blur = pp->blur;
    post_process_tone_map* tone_map = pp->tone_map;

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
        vec4 fade_color;
        vec4u_to_vec4(glow->fade_color, fade_color);
        post_process_tone_map_set_scene_fade(tone_map, &fade_color);
        post_process_tone_map_set_scene_fade_blend_func(tone_map, glow->fade_color_blend_func);
    }

    if (glow->has_tone_transform) {
        post_process_tone_map_set_tone_trans_start(tone_map, &glow->tone_transform_start);
        post_process_tone_map_set_tone_trans_end(tone_map, &glow->tone_transform_end);
    }
}

void render_context_light_param_data_ibl_set(render_context* rctx, light_param_ibl* ibl) {
    if (!ibl->ready)
        return;

    for (int32_t i = 0, j = -1; i < 5; i++, j++) {
        gl_state_bind_texture_cube_map(rctx->ibl_tex[i]);
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

    light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
    light_set_set_irradiance(set, &ibl->diff_coef[1][0], &ibl->diff_coef[1][1], &ibl->diff_coef[1][2]);

    float_t len;
    vec3 pos;

    light_data* l = &set->lights[LIGHT_CHARA];
    light_get_position(l, &pos);
    vec3_length(pos, len);
    if (fabsf(len - 1.0f) < 0.02f)
        light_set_position_vec4(l, &ibl->lit_dir[0]);

    light_set_ibl_specular(l, &ibl->lit_col[0]);
    light_set_ibl_back(l, &ibl->lit_col[2]);
    light_set_ibl_direction(l, &ibl->lit_dir[0]);

    l = &set->lights[LIGHT_STAGE];
    light_get_position(l, &pos);
    vec3_length(pos, len);
    if (fabsf(len - 1.0f) < 0.02f)
        light_set_position_vec4(l, &ibl->lit_dir[1]);

    light_set_ibl_specular(l, &ibl->lit_col[1]);
    light_set_ibl_direction(l, &ibl->lit_dir[1]);
}

void render_context_light_param_data_wind_set(render_context* rctx, light_param_wind* w) {
    wind* wind = rctx->wind;
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

void render_context_light_param_data_face_set(render_context* rctx, light_param_face* face) {
    rctx->face.offset = face->offset;
    rctx->face.scale = face->scale;
    rctx->face.position = face->position;
    rctx->face.direction = face->direction;
}

void render_context_set_light_param(render_context* rctx, light_param_data* light_param) {
    if (light_param->light.ready)
        render_context_light_param_data_light_set(rctx, &light_param->light);
    if (light_param->fog.ready)
        render_context_light_param_data_fog_set(rctx, &light_param->fog);
    if (light_param->glow.ready)
        render_context_light_param_data_glow_set(rctx, &light_param->glow);
    if (light_param->ibl.ready)
        render_context_light_param_data_ibl_set(rctx, &light_param->ibl);
    render_context_light_param_data_wind_set(rctx, &light_param->wind);
    if (light_param->face.ready)
        render_context_light_param_data_face_set(rctx, &light_param->face);
}

void render_context_unset_light_param(render_context* rctx, light_param_data* light_param) {
    if (light_param->light.ready)
        render_context_light_param_data_light_set(rctx, &light_param->light);
    if (light_param->fog.ready)
        render_context_light_param_data_fog_set(rctx, &light_param->fog);
    if (light_param->glow.ready)
        render_context_light_param_data_glow_set(rctx, &light_param->glow);
    if (light_param->ibl.ready)
        render_context_light_param_data_ibl_set(rctx, &light_param->ibl);
    render_context_light_param_data_wind_set(rctx, &light_param->wind);
    if (light_param->face.ready)
        render_context_light_param_data_face_set(rctx, &light_param->face);
}

inline void render_context_update(render_context* rctx) {
    wind_update(rctx->wind);
    draw_state_stats_update(&rctx->draw_state);
    object_data_reset(&rctx->object_data);
}

inline void render_context_free(render_context* rctx) {
    camera_dispose(rctx->camera);
    draw_pass_free(&rctx->draw_pass);
    object_data_free(&rctx->object_data);
    glDeleteVertexArrays(1, &rctx->vao);
    glDeleteTextures(5, rctx->ibl_tex);

    wind_free(rctx->wind);
    post_process_free(&rctx->post_process);
    free(rctx);
}

extern void shadow_update(shadow* shad, render_context* rctx) {
    shadow_update_inner(shad, rctx);
}

static void draw_pass_init(draw_pass* draw_pass) {
    for (draw_pass_type i = DRAW_PASS_SHADOW; i < DRAW_PASS_MAX; i++)
        draw_pass->enable[i] = true;
    draw_pass->reflect = true;
    draw_pass->refract = true;
    draw_pass->shadow = true;
    draw_pass->opaque_z_sort = true;
    draw_pass->alpha_z_sort = true;
    for (draw_pass_type i = DRAW_PASS_3D_OPAQUE; i < DRAW_PASS_3D_MAX; i++)
        draw_pass->draw_pass_3d[i] = true;
    memset(draw_pass->cpu_time, 0, sizeof(draw_pass->cpu_time));
    memset(draw_pass->gpu_time, 0, sizeof(draw_pass->gpu_time));
    time_struct_init(&draw_pass->time);
    draw_pass->shadow_ptr = shadow_init();
    if (draw_pass->shadow_ptr)
        shadow_init_data(draw_pass->shadow_ptr);
    sss_data_init(&draw_pass->sss_data);
}

static void draw_pass_free(draw_pass* draw_pass) {
    render_texture_free(&draw_pass->reflect_texture);
    shadow_free(draw_pass->shadow_ptr);
    sss_data_free(&draw_pass->sss_data);
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

static void object_data_init(object_data* data) {
    data->draw_task_flags = 0;
    data->shadow_type = 0;
    data->field_8 = 0;
    data->field_C = 0;
    data->field_230 = -1;
    memset(&data->passed, 0, sizeof(object_data_culling_info));
    memset(&data->culled, 0, sizeof(object_data_culling_info));
    memset(&data->passed_prev, 0, sizeof(object_data_culling_info));
    memset(&data->culled_prev, 0, sizeof(object_data_culling_info));
    data->show_alpha_center = false;
    data->show_mat_center = false;
    data->object_culling = true;
    data->object_sort = true;
    data->chara_color = true;
    data->morph.value = 0.0f;
    data->morph.object = object_info_null;
    data->object_bounding_sphere_check_func = 0;

    data->buffer.offset = 0;
    data->buffer.max_offset = 0;
    data->buffer.size = 0x300000;
    data->buffer.data = force_malloc(0x300000);

    if (data->buffer.data) {
        data->texture_pattern_count = 0;
        memset(data->texture_pattern_array, 0, sizeof(data->texture_pattern_array));
        data->texture_transform_count = 0;
        memset(data->texture_transform_array, 0, sizeof(data->texture_transform_array));
        data->texture_color_coeff = vec4_identity;
        data->texture_color_offset = vec4_null;
        data->texture_specular_coeff = vec4_identity;
        data->texture_specular_offset = vec4_null;
        data->wet_param = 0.0f;
        for (draw_object_type i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++)
            data->draw_task_array[i] = vector_ptr_empty(draw_task);
    }
}

static void object_data_reset(object_data* data) {
    data->passed_prev = data->passed;
    data->culled_prev = data->culled;
    data->draw_task_flags = 0;
    data->field_8 = 0;
    data->field_C = 0;
    memset(&data->passed, 0, sizeof(object_data_culling_info));
    memset(&data->culled, 0, sizeof(object_data_culling_info));

    data->texture_pattern_count = 0;
    memset(data->texture_pattern_array, 0, sizeof(data->texture_pattern_array));
    data->texture_transform_count = 0;
    memset(data->texture_transform_array, 0, sizeof(data->texture_transform_array));
    data->texture_color_coeff = vec4_identity;
    data->texture_color_offset = vec4_null;
    data->texture_specular_coeff = vec4_identity;
    data->texture_specular_offset = vec4_null;

    for (draw_object_type i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++) {
        data->draw_task_array[i].end = data->draw_task_array[i].begin;
        vector_ptr_draw_task_clear(&data->draw_task_array[i], 0);
    }
    object_data_buffer_reset(&data->buffer);
}

static void object_data_free(object_data* data) {
    for (draw_object_type i = DRAW_OBJECT_OPAQUE; i < DRAW_OBJECT_MAX; i++) {
        data->draw_task_array[i].end = data->draw_task_array[i].begin;
        vector_ptr_draw_task_free(&data->draw_task_array[i], 0);
    }
    free(data->buffer.data);
}

inline static void render_context_light_param_data_ibl_set_diffuse(light_param_ibl_diffuse* diffuse, int32_t level) {
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

inline static void render_context_light_param_data_ibl_set_specular(light_param_ibl_specular* specular) {
    int32_t size = specular->size;
    int32_t max_level = specular->max_level;
    for (int32_t i = 0; i <= max_level; i++, size /= 2) {
        half_t* data = specular->data[i];
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

static shadow* shadow_init() {
    shadow* shad = force_malloc(sizeof(shadow));
    shad->field_170 = 1.2f;
    shad->field_174 = 1.0f;
    for (int32_t i = 0; i < 2; i++) {
        shad->view_point[i] = vec3_identity;
        shad->field_1C0[i] = 0.0f;
        shad->field_1C8[i] = 0.0f;
        shad->field_200[i] = i;
    }
    shad->view_mat[0] = mat4_identity;
    shad->view_mat[1] = mat4_identity;
    shad->blur_filter = BLUR_FILTER_9;
    shad->near_blur = 1;
    shad->field_2BC = 2;
    shad->far_blur = 1;
    shad->distance = 4.0f;
    shad->field_2C4 = 0.4f;
    shad->z_near = 0.1f;
    shad->z_far = 20.0f;
    shad->field_2D0 = 1.4f;
    shad->field_2D4 = 10000.0f;
    shad->field_2D8 = 80.0f;
    shad->field_2DC = 2.0f;
    shad->field_2E0 = 0.05f;
    shad->ambient = 0.4f;
    shad->field_2EC = 0;
    shad->direction = (vec3){ 0.0f, -1.0f, -1.0f };
    vec3_mult_scalar(shad->direction, 1.0f / sqrtf(2.0f), shad->direction);
    shad->field_2E8 = false;
    shad->self_shadow = true;
    shad->blur_filter_enable[0] = true;
    shad->blur_filter_enable[1] = true;
    shad->field_2F5 = false;
    shad->field_208 = (shad->z_far - shad->z_near) * 0.5f;
    return shad;
}

static void shadow_update_inner(shadow* shad, render_context* rctx) {
    if (!shad)
        return;

    for (int32_t i = 0; i < 2; i++)
        shad->field_2F0[i] = false;

    if (rctx->draw_pass.shadow) {
        shad->view_mat[0] = rctx->camera->view;
        shad->view_mat[1] = rctx->camera->inv_view;

        light_set* set = &rctx->light_set_data[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_CHARA];

        vec3 direction;
        float_t length;
        light_get_position(data, &direction);
        vec3_negate(direction, direction);
        vec3_length(direction, length);
        if (length < 0.000001)
            shad->direction = (vec3){ 0.0f, 1.0f, 0.0f };
        else
            vec3_mult_scalar(direction, 1.0f / length, shad->direction);

        for (int32_t i = 0; i < 2; i++)
            if (draw_task_get_count(rctx, DRAW_OBJECT_SHADOW_CHARA + i))
                shad->field_2F0[i] = true;
    }

    int32_t count = 0;
    shad->field_2EC = 0;
    for (int32_t i = 0; i < 2; i++)
        if (shad->field_2F0[i] && vector_length(shad->field_1D0[i]) > 0) {
            shad->field_2EC++;
            count += (int32_t)vector_length(shad->field_1D0[i]);
        }
        else
            shad->field_2F0[i] = false;

    if (count < 3) {
        for (int32_t i = 0; i < 2; i++) {
            vector_vec3* v5 = &shad->field_1D0[i];
            shad->field_1A8[i] = vec3_null;
            shad->field_1C8[i] = 0.0f;
            if (!shad->field_2F0[i] || vector_length(*v5) < 1)
                continue;

            vec3 v7 = vec3_null;
            for (vec3* j = v5->begin; j != v5->end; j++)
                vec3_add(v7, *j, v7);

            float_t v14 = (float_t)(int32_t)vector_length(*v5);
            if (v14 < 0.0f)
                v14 += 1.8446744e19f;
            vec3_mult_scalar(v7, 1.0f / v14, v7);

            float_t v15 = 0.0f;
            vec3 direction = shad->direction;
            for (vec3* j = v5->begin; j != v5->end; j++) {
                vec3 v22;
                vec3_sub(v7, *j, v22);
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
            shad->field_1A8[i] = v7;
            shad->field_1C8[i] = v15;
        }

        if (shad->field_2EC > 0) {
            vec3 view_point = vec3_null;
            vec3 interest = vec3_null;
            for (int32_t i = 0; i < 2; i++) {
                if (!shad->field_2F0[i])
                    continue;

                vec3 v11;
                vec3_mult_scalar(shad->direction, shad->field_208, v11);
                vec3_sub(shad->field_1A8[i], v11, v11);
                float_t v9;
                vec3_distance(shad->view_point[i], v11, v9);

                float_t v12;
                vec3_distance(shad->interest[i], shad->field_1A8[i], v12);
                if (v9 > 0.1f || v12 > 0.1f) {
                    shad->view_point[i] = v11;
                    shad->interest[i] = shad->field_1A8[i];
                }

                vec3_add(view_point, shad->view_point[i], view_point);
                vec3_add(interest, shad->interest[i], interest);
            }

            vec3_mult_scalar(view_point, 1.0f / shad->field_2EC, shad->view_point_shared);
            vec3_mult_scalar(interest, 1.0f / shad->field_2EC, shad->interest_shared);
        }

        float_t v2 = max(shad->field_1C8[0], shad->field_1C8[1]);
        shad->field_2F5 = false;
        shad->field_170 = v2 + 1.2f;
        shad->field_200[0] = 0;
        shad->field_200[1] = 1;
        if (shad->field_2EC >= 2) {
            vec3 v12;
            vec3_sub(shad->field_1A8[0], shad->interest_shared, v12);

            vec3 v14;
            vec3_sub(shad->field_1A8[1], shad->interest_shared, v14);

            float_t v15;
            vec3_dot(v12, shad->direction, v15);

            vec3 v6;
            vec3_mult_scalar(shad->direction, v15, v6);
            vec3_sub(v6, v12, v6);

            float_t v16;
            vec3_length(v6, v16);
            v16 -= 0.25f;
            if (v16 < 0.0f)
                v16 = 0.0f;

            if (v16 > 1.2f) {
                shad->field_170 = v2 + 2.4f;
                shad->field_2F5 = true;
            }
            else
                shad->field_170 = v2 + 1.2f + v16;

            float_t t3;
            float_t t4;
            vec3_dot(v12, shad->direction, t3);
            vec3_dot(v14, shad->direction, t4);
            if (t3 < t4) {
                shad->field_200[1] = 0;
                shad->field_200[0] = 1;
            }
        }
    }
    else {
        vec3 v3;
        vec3 v86;
        if (shad->direction.y * shad->direction.y < 0.99f) {
            vec3_cross(shad->direction, ((vec3){ 0.0f, 1.0f, 0.0f }), v86);
            vec3_cross(v86, shad->direction, v3);
            vec3_normalize(v3, v3);
            vec3_normalize(v86, v86);
        }
        else {
            v3 = (vec3){ 0.0f, 0.0f, 1.0f };
            v86 = (vec3){ 1.0f, 0.0f, 0.0f };
        }

        for (int32_t i = 0; i < 2; i++) {
            vector_vec3* v18 = &shad->field_1D0[i];
            shad->field_1A8[i] = vec3_null;
            shad->field_1C8[i] = 0.0;
            if (!shad->field_2F0[i] || vector_length(*v18) < 1)
                continue;

            vec3 v22 = vec3_null;
            for (vec3* j = v18->begin; j != v18->end; j++)
                vec3_add(v22, *j, v22);

            int32_t v27 = (int32_t)vector_length(*v18);
            float_t v29 = (float_t)v27;
            if (v27 < 0)
                v29 += 1.8446744e19f;

            float_t v30 = 0.0f;
            vec3 v31;
            vec3_mult_scalar(v22, 1.0f / v29, v31);

            for (vec3* j = v18->begin; j != v18->end; j++) {
                vec3 v34;
                vec3_sub(v31, *j, v34);
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
            shad->field_1A8[i] = v31;
            shad->field_1C8[i] = v30;
        }

        if (shad->field_2EC > 0) {
            for (int32_t i = 0; i < 2; i++) {
                if (!shad->field_2F0[i])
                    continue;

                float_t v51;
                vec3 v53;
                float_t v54;
                vec3_mult_scalar(shad->direction, shad->field_208, v53);
                vec3_sub(shad->field_1A8[i], v53, v53);
                vec3_distance(shad->view_point[i], v53, v51);
                vec3_distance(shad->interest[i], shad->field_1A8[i], v54);
                if (v51 > 0.1f || v54 > 0.1f) {
                    shad->view_point[i] = v53;
                    shad->interest[i] = shad->field_1A8[i];
                }
            }

            vec3 view_point = vec3_null;
            vec3 interest = vec3_null;
            int32_t count = 0;;
            for (int32_t i = 0; i < 2; i++) {
                int32_t c = (int32_t)vector_length(shad->field_1D0[i]);
                vec3 view_point_temp;
                vec3 interest_temp;
                vec3_mult_scalar(shad->view_point[i], (float_t)c, view_point_temp);
                vec3_mult_scalar(shad->interest[i], (float_t)c, interest_temp);
                vec3_add(view_point, view_point_temp, view_point);
                vec3_add(interest, interest_temp, interest);
                count += c;
            }

            vec3_mult_scalar(view_point, 1.0f / (float_t)count, shad->view_point_shared);
            vec3_mult_scalar(interest, 1.0f / (float_t)count, shad->interest_shared);
        }

        float_t v2 = 0.0;
        float_t v67 = max(shad->field_1C8[0], shad->field_1C8[1]);
        shad->field_2F5 = false;
        shad->field_170 = v67 + 1.2f;
        shad->field_200[0] = 0;
        shad->field_200[1] = 1;
        if (shad->field_2EC >= 2) {
            float_t v68 = 0.0;
            float_t v69 = 0.0;
            float_t v70 = 0.0;
            for (int32_t i = 0; i < 2; i++) {
                if (!shad->field_2F0[i])
                    continue;

                vector_vec3* v72 = &shad->field_1D0[i];
                for (vec3* j = v72->begin; j != v72->end; j++) {
                    vec3 v74;
                    vec3_sub(*j, shad->interest_shared, v74);

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
                shad->field_170 = v67 + 2.4f;
                shad->field_2F5 = true;
            }
            else
                shad->field_170 = v79 + 1.2f;

            vec3 interest_chara;
            vec3 interest_stage;
            float_t interest_chara_cos;
            float_t interest_stage_cos;
            vec3_sub(shad->field_1A8[0], shad->interest_shared, interest_chara);
            vec3_dot(interest_chara, shad->direction, interest_chara_cos);
            vec3_sub(shad->field_1A8[1], shad->interest_shared, interest_stage);
            vec3_dot(interest_stage, shad->direction, interest_stage_cos);
            if (interest_chara_cos < interest_stage_cos) {
                shad->field_200[1] = 0;
                shad->field_200[0] = 1;
            }
        }
    }

    for (int32_t i = 0; i < 2; i++)
        shad->field_1D0[i].end = shad->field_1D0[i].begin;
}

static int32_t shadow_init_data(shadow* shad) {
    typedef struct shadow_texture_init_params {
        int32_t width;
        int32_t height;
        int32_t max_level;
        GLenum color_format;
        GLenum depth_format;
    } shadow_texture_init_params;

    shadow_texture_init_params init_params[] = {
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
        if (render_texture_init(&shad->field_8[i], v3->width, v3->height,
            v3->max_level, v3->color_format, v3->depth_format) < 0)
            return -1;

    for (int32_t i = 0; i < 3; i++) {
        gl_state_bind_texture_2d(shad->field_8[i].color_texture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&vec4_identity);
    }
    gl_state_bind_texture_2d(0);
    glGetError();
    return 0;
}

static void shadow_free(shadow* shad) {
    if (!shad)
        return;

    for (int32_t i = 0; i < 7; i++)
        render_texture_free(&shad->field_8[i]);

    for (int32_t i = 0; i < 2; i++)
        vector_vec3_free(&shad->field_1D0[i], 0);
    free(shad);
}

static void sss_data_init(sss_data_struct* sss) {
    memset(sss, 0, sizeof(sss_data_struct));
    sss->init = true;
    sss->enable = true;
    sss->npr_contour = true;
    render_texture_init(&sss->textures[0], 640, 360, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
    render_texture_init(&sss->textures[1], 320, 180, 0, GL_RGBA16F, GL_ZERO);
    render_texture_init(&sss->textures[2], 320, 180, 0, GL_RGBA16F, GL_ZERO);
    render_texture_init(&sss->textures[3], 320, 180, 0, GL_RGBA16F, GL_ZERO);
    sss->param = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
}

static void sss_data_free(sss_data_struct* sss) {
    render_texture_free(&sss->textures[0]);
    render_texture_free(&sss->textures[1]);
    render_texture_free(&sss->textures[2]);
    render_texture_free(&sss->textures[3]);
}
