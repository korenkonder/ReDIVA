/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_context.hpp"
#include "app_system_detail.hpp"
#include "gl_rend_state.hpp"
#include "gl_state.hpp"
#include "shader_ft.hpp"
#include "sound.hpp"
#include "sprite.hpp"
#include "task.hpp"

extern float_t rob_frame;
extern render_context* rctx_ptr;

//extern void dw_gui_ctrl_disp();

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

global_material_struct::global_material_struct() {
    bump_depth = 1.0f;
    intensity = 1.0f;
    reflectivity = 1.0f;
    reflect_uv_scale = 0.1f;
    refract_uv_scale = 0.1f;
}

draw_state::render_data::render_data() : self_shadow(), shadow(), blend(), cull_front(), field_8() {
    shader_index = -1;
}

draw_state::draw_state() : wireframe(), wireframe_overlay(),
shader_debug_flag(), use_global_material(), fog_height(), ex_data_mat(), field_68() {
    shader = true;
    shader_index = -1;
    show = -1;
    fresnel = 7.0f;
}

bool draw_state::get_use_global_material() {
    return use_global_material;
}

void draw_state::stats_update() {
    stats_prev.reset();

    for (draw_state::render_data& i : rend_data) {
        stats_prev.object_draw_count += i.stats.object_draw_count;
        stats_prev.object_translucent_draw_count += i.stats.object_translucent_draw_count;
        stats_prev.object_reflect_draw_count += i.stats.object_reflect_draw_count;
        stats_prev.field_C += i.stats.field_C;
        stats_prev.field_10 += i.stats.field_10;
        stats_prev.draw_count += i.stats.draw_count;
        stats_prev.draw_triangle_count += i.stats.draw_triangle_count;
        stats_prev.field_1C += i.stats.field_1C;
        i.stats.reset();
    }
}

void draw_state::set_fog_height(bool value) {
    fog_height = value;
}

void draw_state::set_use_global_material(bool value) {
    use_global_material = value;
}

sss_data::sss_data() : init_data(), enable(), npr_contour(), param() {
    init_data = false;
    enable = true;
    npr_contour = true;
    param = 1.0f;
}

sss_data::~sss_data() {

}

void sss_data::free() {
    if (init_data)
        for (RenderTexture& i : textures)
            i.Free();
}

void sss_data::init() {
    if (init_data)
        return;

    textures[0].Init(640, 360, 0, GL_RGBA16F, GL_ZERO /*GL_DEPTH_COMPONENT32F*/);
    textures[1].Init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[2].Init(320, 180, 0, GL_RGBA16F, GL_ZERO);
    textures[3].Init(320, 180, 0, GL_RGBA16F, GL_ZERO);

    param = { 0.0f, 0.0f, 0.0f, 1.0f };
    init_data = true;
}

void sss_data::set_texture(p_gl_rend_state& p_gl_rend_st, int32_t texture_index) {
    p_gl_rend_st.active_bind_texture_2d(16, textures[texture_index].GetColorTex());
    p_gl_rend_st.active_texture(0);
}

light_proj::light_proj(int32_t width, int32_t height) : enable(), texture_id() {
    shadow_texture[0].Init(2048, 512, 0, GL_R32F, GL_DEPTH_COMPONENT32F);
    shadow_texture[1].Init(2048, 512, 0, GL_R32F, GL_ZERO);
    draw_texture.Init(width, height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
}

light_proj::~light_proj() {

}

void light_proj::resize(int32_t width, int32_t height) {
    if (!this)
        return;

    draw_texture.Init(width, height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
}

bool light_proj::set(render_data_context& rend_data_ctx, cam_data& cam) {
    if (!this)
        return false;

    static const vec4 color_clear = 0.0f;
    static const GLfloat depth_clear = 1.0f;

    shadow_texture[0].Bind(rend_data_ctx.state);
    rend_data_ctx.state.set_viewport(0, 0, 2048, 512);
    rend_data_ctx.state.enable_depth_test();
    rend_data_ctx.state.set_depth_mask(GL_TRUE);
    rend_data_ctx.state.clear_buffer(GL_COLOR, 0, (float_t*)&color_clear);
    rend_data_ctx.state.clear_buffer(GL_DEPTH, 0, &depth_clear);

    if (set_mat(rend_data_ctx, cam, false)) {
        rctx_ptr->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SIL;
        rend_data_ctx.shader_flags.arr[U_DEPTH] = 1;
        return true;
    }
    else {
        draw_texture.Bind(rend_data_ctx.state);
        draw_texture.SetViewport(rend_data_ctx.state);
        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        rend_data_ctx.state.clear_buffer(GL_COLOR, 0, (float_t*)&color_clear);
        rend_data_ctx.state.clear_buffer(GL_DEPTH, 0, &depth_clear);
    }
    return false;
}

static void light_proj_get_view_proj_mat(cam_data& cam, const vec3& view_point, const vec3& interest,
    const float_t fov, const float_t aspect, const float_t min_distance, const float_t max_distance, mat4* mat) {
    cam.set_view_point(view_point);
    cam.set_interest(interest);
    cam.set_up({ 0.0f, 1.0f, 0.0f });
    cam.calc_view_mat();
    cam.set_fov(fov * DEG_TO_RAD_FLOAT);
    cam.set_aspect(aspect);
    cam.set_min_distance(min_distance);
    cam.set_max_distance(max_distance);
    if (mat) {
        cam.calc_persp_proj_mat_offset(0.5f, 0.5f);
        cam.calc_view_proj_mat();
        *mat = cam.get_view_proj_mat();
    }
    else {
        cam.calc_persp_proj_mat();
        cam.calc_view_proj_mat();
    }
}

bool light_proj::set_mat(render_data_context& rend_data_ctx, cam_data& cam, bool set_mat) {
    light_set* set = &rctx_ptr->light_set[LIGHT_SET_MAIN];
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
        mat4 mat;
        light_proj_get_view_proj_mat(cam, position, interest,
            fov * RAD_TO_DEG_FLOAT, 4.0f, 0.1f, 10.0f, &mat);
        rend_data_ctx.set_scene_light_projection(mat);
    }
    else
        light_proj_get_view_proj_mat(cam, position, interest,
            fov * RAD_TO_DEG_FLOAT, 4.0f, 0.1f, 10.0f, 0);
    return true;
}

void render_data::obj_shader_data::reset() {
    g_shader_flags = {};
}

void render_data::obj_shader_data::set_shader_flags(const int32_t* shader_flags) {
    g_shader_flags.x.m.alpha_mask    = shader_flags[U_ALPHA_MASK];
    g_shader_flags.x.m.alpha_test    = shader_flags[U_ALPHA_TEST];
    g_shader_flags.x.m.aniso         = shader_flags[U_ANISO];
    g_shader_flags.x.m.aet_back      = shader_flags[U_AET_BACK];
    g_shader_flags.x.m.texture_blend = shader_flags[U_TEXTURE_BLEND];
    g_shader_flags.x.m.unk           = 0; //shader_flags[U_UNK_NEW_IN_MM_IDK_WHAT_TO_DO_WITH_IT];
    g_shader_flags.x.m.chara_color   = shader_flags[U_CHARA_COLOR];
    g_shader_flags.x.m.clip_plane    = shader_flags[U_CLIP_PLANE];
    g_shader_flags.x.m.u08           = shader_flags[U08];
    g_shader_flags.x.m.depth_peel    = shader_flags[U_DEPTH_PEEL];
    g_shader_flags.x.m.depth         = shader_flags[U_DEPTH];
    g_shader_flags.x.m.u0b           = shader_flags[U0B];
    g_shader_flags.x.m.alpha_blend   = shader_flags[U_ALPHA_BLEND];
    g_shader_flags.x.m.ripple_emit   = shader_flags[U_RIPPLE_EMIT];
    g_shader_flags.x.m.esm_filter    = shader_flags[U_ESM_FILTER];
    g_shader_flags.x.m.exposure      = shader_flags[U_EXPOSURE];
    g_shader_flags.x.m.scene_fade    = shader_flags[U_SCENE_FADE];
    g_shader_flags.x.m.fade          = shader_flags[U_FADE];
    g_shader_flags.x.m.stage_ambient = shader_flags[U_STAGE_AMBIENT];
    g_shader_flags.x.m.flare         = shader_flags[U_FLARE];
    g_shader_flags.y.m.fog_stage     = shader_flags[U_FOG_STAGE];
    g_shader_flags.y.m.fog_chara     = shader_flags[U_FOG_CHARA];
    g_shader_flags.y.m.u16           = shader_flags[U16];
    g_shader_flags.y.m.gauss         = shader_flags[U_GAUSS];
    g_shader_flags.y.m.eye_lens      = shader_flags[U_EYE_LENS];
    g_shader_flags.y.m.image_filter  = shader_flags[U_IMAGE_FILTER];
    g_shader_flags.y.m.instance      = shader_flags[U_INSTANCE];
    g_shader_flags.y.m.tone_curve    = shader_flags[U_TONE_CURVE];
    g_shader_flags.y.m.light_proj    = shader_flags[U_LIGHT_PROJ];
    g_shader_flags.y.m.magnify       = shader_flags[U_MAGNIFY];
    g_shader_flags.y.m.membrane      = shader_flags[U_MEMBRANE];
    g_shader_flags.y.m.mlaa          = shader_flags[U_MLAA];
    g_shader_flags.y.m.mlaa_search   = shader_flags[U_MLAA_SEARCH];
    g_shader_flags.y.m.morph_color   = shader_flags[U_MORPH_COLOR];
    g_shader_flags.y.m.morph         = shader_flags[U_MORPH];
    g_shader_flags.y.m.movie         = shader_flags[U_MOVIE];
    g_shader_flags.y.m.u24           = shader_flags[U24];
    g_shader_flags.y.m.u25           = shader_flags[U25];
    g_shader_flags.y.m.npr_normal    = shader_flags[U_NPR_NORMAL];
    g_shader_flags.z.m.npr           = shader_flags[U_NPR];
    g_shader_flags.z.m.stage_shadow2 = shader_flags[U_STAGE_SHADOW2];
    g_shader_flags.z.m.reflect       = shader_flags[U_REFLECT];
    g_shader_flags.z.m.reduce        = shader_flags[U_REDUCE];
    g_shader_flags.z.m.chara_shadow  = shader_flags[U_CHARA_SHADOW];
    g_shader_flags.z.m.chara_shadow2 = shader_flags[U_CHARA_SHADOW2];
    g_shader_flags.z.m.u2d           = shader_flags[U2D];
    g_shader_flags.z.m.u2e           = shader_flags[U2E];
    g_shader_flags.z.m.show_vector   = shader_flags[U_SHOW_VECTOR];
    g_shader_flags.z.m.skinning      = shader_flags[U_SKINNING];
    g_shader_flags.z.m.snow_particle = shader_flags[U_SNOW_PARTICLE];
    g_shader_flags.z.m.specular_ibl  = shader_flags[U_SPECULAR_IBL];
    g_shader_flags.z.m.combiner      = shader_flags[U_COMBINER];
    g_shader_flags.z.m.tex_0_type    = shader_flags[U_TEX_0_TYPE];
    g_shader_flags.z.m.tex_1_type    = shader_flags[U_TEX_1_TYPE];
    g_shader_flags.z.m.sss_filter    = shader_flags[U_SSS_FILTER];
    g_shader_flags.w.m.sss_chara     = shader_flags[U_SSS_CHARA];
    g_shader_flags.w.m.star          = shader_flags[U_STAR];
    g_shader_flags.w.m.texture_count = shader_flags[U_TEXTURE_COUNT];
    g_shader_flags.w.m.env_map       = shader_flags[U_ENV_MAP];
    g_shader_flags.w.m.ripple        = shader_flags[U_RIPPLE];
    g_shader_flags.w.m.translucency  = shader_flags[U_TRANSLUCENCY];
    g_shader_flags.w.m.normal        = shader_flags[U_NORMAL];
    g_shader_flags.w.m.transparency  = shader_flags[U_TRANSPARENCY];
    g_shader_flags.w.m.water_reflect = shader_flags[U_WATER_REFLECT];
    g_shader_flags.w.m.u40           = shader_flags[U40];
    g_shader_flags.w.m.u41           = shader_flags[U41];
    g_shader_flags.w.m.stage_shadow  = shader_flags[U_STAGE_SHADOW];
    g_shader_flags.w.m.specular      = shader_flags[U_SPECULAR];
    g_shader_flags.w.m.tone_map      = shader_flags[U_TONE_MAP];
    g_shader_flags.w.m.u45           = shader_flags[U45];
}

void render_data::obj_scene_data::reset() {
    g_irradiance_r_transforms[0] = 0.0f;
    g_irradiance_r_transforms[1] = 0.0f;
    g_irradiance_r_transforms[2] = 0.0f;
    g_irradiance_r_transforms[3] = 0.0f;
    g_irradiance_g_transforms[0] = 0.0f;
    g_irradiance_g_transforms[1] = 0.0f;
    g_irradiance_g_transforms[2] = 0.0f;
    g_irradiance_g_transforms[3] = 0.0f;
    g_irradiance_b_transforms[0] = 0.0f;
    g_irradiance_b_transforms[1] = 0.0f;
    g_irradiance_b_transforms[2] = 0.0f;
    g_irradiance_b_transforms[3] = 0.0f;
    g_light_env_stage_diffuse = 1.0f;
    g_light_env_stage_specular = 0.0f;
    g_light_env_chara_diffuse = 1.0f;
    g_light_env_chara_ambient = 0.0f;
    g_light_env_chara_specular = 0.0f;
    g_light_env_reflect_diffuse = 1.0f;
    g_light_env_reflect_ambient = 0.0f;
    g_light_env_proj_diffuse = 1.0f;
    g_light_env_proj_specular = 0.0f;
    g_light_env_proj_position = 0.0f;
    g_light_stage_dir = { 0.0f, 1.0f, 0.0f, 0.0f };
    g_light_stage_diff = 0.0f;
    g_light_stage_spec = 0.0f;
    g_light_chara_dir = { 0.0f, 1.0f, 0.0f, 0.0f };
    g_light_chara_spec = 0.0f;
    g_light_chara_luce = 0.0f;
    g_light_chara_back = 0.0f;
    g_light_face_diff = 0.0f;
    g_chara_color_rim = 0.0f;
    g_chara_color0 = 0.0f;
    g_chara_color1 = 0.0f;
    g_chara_f_dir = { 0.0f, 1.0f, 0.0f, 0.0f };
    g_chara_f_ambient = 0.0f;
    g_chara_f_diffuse = 0.0f;
    g_chara_tc_param = 0.0f;
    g_fog_depth_color = 0.0f;
    g_fog_height_params = 0.0f;
    g_fog_height_color = 0.0f;
    g_fog_bump_params = 0.0f;
    g_fog_state_params = 0.0f;
    g_normal_tangent_transforms[0] = mat4_identity.row0;
    g_normal_tangent_transforms[1] = mat4_identity.row1;
    g_normal_tangent_transforms[2] = mat4_identity.row2;
    g_esm_param = 0.0f;
    g_self_shadow_receivers[0] = 0.0f;
    g_self_shadow_receivers[1] = 0.0f;
    g_self_shadow_receivers[2] = 0.0f;
    g_self_shadow_receivers[3] = 0.0f;
    g_self_shadow_receivers[4] = 0.0f;
    g_self_shadow_receivers[5] = 0.0f;
    g_shadow_ambient = 1.0f;
    g_shadow_ambient1 = 0.0f;
    g_framebuffer_size = { 1.0f / 1280.0f, 1.0f / 720.0f, 1.0f / 1280.0f, 1.0f / 720.0f, };
    g_light_reflect_dir = { 0.0f, 1.0f, 0.0f, 0.0f };
    g_clip_plane = { 0.0f, -1.0f, 0.0f, 0.0f };
    g_npr_cloth_spec_color = 1.0f;
    g_view[0] = mat4_identity.row0;
    g_view[1] = mat4_identity.row1;
    g_view[2] = mat4_identity.row2;
    g_view_inverse[0] = mat4_identity.row0;
    g_view_inverse[1] = mat4_identity.row1;
    g_view_inverse[2] = mat4_identity.row2;
    g_projection_view[0] = mat4_identity.row0;
    g_projection_view[1] = mat4_identity.row1;
    g_projection_view[2] = mat4_identity.row2;
    g_projection_view[3] = mat4_identity.row3;
    g_view_position = 0.0f;
    g_light_projection[0] = mat4_identity.row0;
    g_light_projection[1] = mat4_identity.row1;
    g_light_projection[2] = mat4_identity.row2;
    g_light_projection[3] = mat4_identity.row3;
}

void render_data::obj_batch_data::reset() {
    g_transforms[0] = mat4_identity.row0;
    g_transforms[1] = mat4_identity.row1;
    g_transforms[2] = mat4_identity.row2;
    g_transforms[3] = mat4_identity.row3;
    g_worlds[0] = mat4_identity.row0;
    g_worlds[1] = mat4_identity.row1;
    g_worlds[2] = mat4_identity.row2;
    g_worlds_invtrans[0] = mat4_identity.row0;
    g_worlds_invtrans[1] = mat4_identity.row1;
    g_worlds_invtrans[2] = mat4_identity.row2;
    g_worldview[0] = mat4_identity.row0;
    g_worldview[1] = mat4_identity.row1;
    g_worldview[2] = mat4_identity.row2;
    g_worldview_inverse[0] = mat4_identity.row0;
    g_worldview_inverse[1] = mat4_identity.row1;
    g_worldview_inverse[2] = mat4_identity.row2;
    g_joint[0] = mat4_identity.row0;
    g_joint[1] = mat4_identity.row1;
    g_joint[2] = mat4_identity.row2;
    g_joint_inverse[0] = mat4_identity.row0;
    g_joint_inverse[1] = mat4_identity.row1;
    g_joint_inverse[2] = mat4_identity.row2;
    g_texcoord_transforms[0] = mat4_identity.row0;
    g_texcoord_transforms[1] = mat4_identity.row1;
    g_texcoord_transforms[2] = mat4_identity.row0;
    g_texcoord_transforms[3] = mat4_identity.row1;
    g_blend_color = 1.0f;
    g_offset_color = 0.0f;
    g_material_state_diffuse = 1.0f;
    g_material_state_ambient = 0.0f;
    g_material_state_emission = 0.0f;
    g_material_state_shininess = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_material_state_specular = 0.0f;
    g_fresnel_coefficients = 0.0f;
    g_texture_color_coefficients = 0.0f;
    g_texture_color_offset = 0.0f;
    g_texture_specular_coefficients = 0.0f;
    g_texture_specular_offset = 0.0f;
    g_shininess = 0.0f;
    g_max_alpha = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_morph_weight = { 1.0f, 0.0f, 0.0f, 0.0f };
    g_sss_param = 0.0f;
    g_bump_depth = 0.0f;
    g_intensity = 0.0f;
    g_reflect_uv_scale = 0.0f;
}

void render_data::init() {
    buffer_shader.Create(gl_state, sizeof(obj_shader_data));
    buffer_scene.Create(gl_state, sizeof(obj_scene_data));
    buffer_batch.Create(gl_state, sizeof(obj_batch_data));
    if (GLAD_GL_VERSION_4_3)
        buffer_skinning.Create(gl_state, sizeof(obj_skinning_data));
    else
        buffer_skinning_ubo.Create(gl_state, sizeof(obj_skinning_data));

    buffer_shader_data.reset();
    enum_or(flags, RENDER_DATA_SHADER_UPDATE);
    buffer_scene_data.reset();
    enum_or(flags, RENDER_DATA_SCENE_UPDATE);
    buffer_batch_data.reset();
    enum_or(flags, RENDER_DATA_BATCH_UPDATE);
    inv_view_mat = mat4_identity;
}

void render_data::free() {
    if (GLAD_GL_VERSION_4_3)
        buffer_skinning.Destroy();
    else
        buffer_skinning_ubo.Destroy();
    buffer_batch.Destroy();
    buffer_scene.Destroy();
    buffer_shader.Destroy();
}

void render_data::set_skinning_data(p_gl_rend_state& p_gl_rend_st, const mat4* mats, int32_t count) {
    if (count == 2) {
        mat4 temp;
        mat4_transpose(&mats[0], &temp);
        if (buffer_batch_data.g_joint[0] != temp.row0
            || buffer_batch_data.g_joint[1] != temp.row1
            || buffer_batch_data.g_joint[2] != temp.row2) {
            buffer_batch_data.g_joint[0] = temp.row0;
            buffer_batch_data.g_joint[1] = temp.row1;
            buffer_batch_data.g_joint[2] = temp.row2;
            enum_or(flags, RENDER_DATA_BATCH_UPDATE);
        }
    }

    if (sv_shared_storage_uniform_buffer) {
        GLuint buffer = 0;
        size_t offset = 0;
        size_t size = 0;
        if (rctx_ptr->get_shared_storage_uniform_buffer_data(
            (size_t)mats, buffer, offset, size, !!GLAD_GL_VERSION_4_3))
            if (GLAD_GL_VERSION_4_3)
                p_gl_rend_st.bind_shader_storage_buffer_range(0, buffer, (GLintptr)offset, (GLsizeiptr)size);
            else
                p_gl_rend_st.bind_uniform_buffer_range(6, buffer, (GLintptr)offset, (GLsizeiptr)size);
    }
    else {
        vec4* g_joint_transforms = buffer_skinning_data.g_joint_transforms;

        mat4 mat;
        for (int32_t i = 0; i < count; i++, mats++, g_joint_transforms += 3) {
            mat4_transpose(mats, &mat);
            g_joint_transforms[0] = mat.row0;
            g_joint_transforms[1] = mat.row1;
            g_joint_transforms[2] = mat.row2;
        }

        if (GLAD_GL_VERSION_4_3)
            buffer_skinning.WriteMemory(p_gl_rend_st,
                0, sizeof(vec4) * 3 * count, &buffer_skinning_data);
        else
            buffer_skinning_ubo.WriteMemory(p_gl_rend_st,
                0, sizeof(vec4) * 3 * count, &buffer_skinning_data);
    }
}

void render_data::set_state(p_gl_rend_state& p_gl_rend_st) {
    shaders_ft.set(p_gl_rend_st, shader_flags, shader_index);

    if (flags & RENDER_DATA_SHADER_UPDATE) {
        buffer_shader.WriteMemory(p_gl_rend_st, buffer_shader_data);
        enum_and(flags, ~RENDER_DATA_SHADER_UPDATE);
    }

    if (flags & RENDER_DATA_SCENE_UPDATE) {
        buffer_scene.WriteMemory(p_gl_rend_st, buffer_scene_data);
        enum_and(flags, ~RENDER_DATA_SCENE_UPDATE);
    }

    if (flags & RENDER_DATA_BATCH_UPDATE) {
        mat4 worlds;
        worlds.row0 = buffer_batch_data.g_worlds[0];
        worlds.row1 = buffer_batch_data.g_worlds[1];
        worlds.row2 = buffer_batch_data.g_worlds[2];
        worlds.row3 = mat4_identity.row3;
        mat4_transpose(&worlds, &worlds);

        mat4 temp;
        mat4_mul(&worlds, &cam.view_proj_mat, &temp);
        mat4_transpose(&temp, &temp);
        buffer_batch_data.g_transforms[0] = temp.row0;
        buffer_batch_data.g_transforms[1] = temp.row1;
        buffer_batch_data.g_transforms[2] = temp.row2;
        buffer_batch_data.g_transforms[3] = temp.row3;

        mat4 worlds_inv;
        mat4_invert(&worlds, &temp);
        buffer_batch_data.g_worlds_invtrans[0] = temp.row0;
        buffer_batch_data.g_worlds_invtrans[1] = temp.row1;
        buffer_batch_data.g_worlds_invtrans[2] = temp.row2;

        mat4_mul(&worlds, &cam.view_mat, &worlds);
        mat4_transpose(&worlds, &temp);
        buffer_batch_data.g_worldview[0] = temp.row0;
        buffer_batch_data.g_worldview[1] = temp.row1;
        buffer_batch_data.g_worldview[2] = temp.row2;

        mat4_invert(&worlds, &worlds);
        mat4_transpose(&worlds, &temp);
        buffer_batch_data.g_worldview_inverse[0] = temp.row0;
        buffer_batch_data.g_worldview_inverse[1] = temp.row1;
        buffer_batch_data.g_worldview_inverse[2] = temp.row2;

        temp.row0 = buffer_batch_data.g_joint[0];
        temp.row1 = buffer_batch_data.g_joint[1];
        temp.row2 = buffer_batch_data.g_joint[2];
        temp.row3 = mat4_identity.row3;
        mat4_invert(&temp, &temp);
        buffer_batch_data.g_joint_inverse[0] = temp.row0;
        buffer_batch_data.g_joint_inverse[1] = temp.row1;
        buffer_batch_data.g_joint_inverse[2] = temp.row2;

        buffer_batch.WriteMemory(p_gl_rend_st, buffer_batch_data);
        enum_and(flags, ~RENDER_DATA_BATCH_UPDATE);
    }

    p_gl_rend_st.bind_uniform_buffer_base(0, buffer_shader);
    p_gl_rend_st.bind_uniform_buffer_base(1, buffer_scene);
    p_gl_rend_st.bind_uniform_buffer_base(2, buffer_batch);

    if (shader_flags.arr[U_SKINNING] && !sv_shared_storage_uniform_buffer)
        if (GLAD_GL_VERSION_4_3)
            p_gl_rend_st.bind_shader_storage_buffer_base(0, buffer_skinning);
        else
            p_gl_rend_st.bind_uniform_buffer_base(6, buffer_skinning_ubo);
}

void render_data::set_shader(uint32_t index) {
    shader_index = index;

    buffer_shader_data.set_shader_flags(shader_flags.arr);
    enum_or(flags, RENDER_DATA_SHADER_UPDATE);
}

render_data_context::render_data_context(gl_rend_state_index index) : state(index), index(index),
data(rctx_ptr->data[index]), shader_flags(rctx_ptr->data[index].shader_flags){

}

void render_data_context::get_scene_fog_params(render_data_context::fog_params& value) {
    value.depth_color = data.buffer_scene_data.g_fog_depth_color;
    value.height_params = data.buffer_scene_data.g_fog_height_params;
    value.height_color = data.buffer_scene_data.g_fog_height_color;
    value.bump_params = data.buffer_scene_data.g_fog_bump_params;
    value.density = data.buffer_scene_data.g_fog_state_params.x;
    value.start = data.buffer_scene_data.g_fog_state_params.y;
    value.end = data.buffer_scene_data.g_fog_state_params.z;
}

void render_data_context::get_scene_light(vec4* light_env_stage_diffuse,
    vec4* light_env_stage_specular, vec4* light_chara_dir, vec4* light_chara_luce,
    vec4* light_env_chara_diffuse, vec4* light_env_chara_specular) {
    if (light_env_stage_diffuse)
        *light_env_stage_diffuse = data.buffer_scene_data.g_light_env_stage_diffuse;
    if (light_env_stage_specular)
        *light_env_stage_specular = data.buffer_scene_data.g_light_env_stage_specular;
    if (light_chara_dir)
        *light_chara_dir = data.buffer_scene_data.g_light_chara_dir;
    if (light_chara_luce)
        *light_chara_luce = data.buffer_scene_data.g_light_chara_luce;
    if (light_env_chara_diffuse)
        *light_env_chara_diffuse = data.buffer_scene_data.g_light_env_chara_diffuse;
    if (light_env_chara_specular)
        *light_env_chara_specular = data.buffer_scene_data.g_light_env_chara_specular;
}

void render_data_context::reset_shader_flags() {
    shader_flags.arr[U_TEXTURE_COUNT] = 0;
    shader_flags.arr[U_TEXTURE_BLEND] = 0;
    shader_flags.arr[U_NORMAL] = 0;
    shader_flags.arr[U_SPECULAR] = 0;
    shader_flags.arr[U_TRANSPARENCY] = 0;
    shader_flags.arr[U_TRANSLUCENCY] = 0;
    shader_flags.arr[U_ENV_MAP] = 0;
    shader_flags.arr[U0B] = 0;
    shader_flags.arr[U_SPECULAR_IBL] = 0;
    shader_flags.arr[U_FOG_STAGE] = 0;
    shader_flags.arr[U_FOG_CHARA] = 0;
    shader_flags.arr[U_ANISO] = 0;
    shader_flags.arr[U_STAGE_SHADOW] = 0;
    shader_flags.arr[U45] = 0;
    shader_flags.arr[U_TEX_0_TYPE] = 0;
    shader_flags.arr[U_TEX_1_TYPE] = 0;
}

void render_data_context::set_batch_alpha_threshold(const float_t value) {
    data.buffer_batch_data.g_max_alpha.z = value;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_blend_color_offset_color(
    const vec4& blend_color, const vec4& offset_color) {
    data.buffer_batch_data.g_blend_color = blend_color;
    data.buffer_batch_data.g_offset_color = offset_color;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_material_color(const vec4& diffuse, const vec4& ambient,
    const vec4& emission, const float_t material_shininess, const vec4& specular,
    const vec4& fresnel_coefficients, const vec4& texture_color_coefficients,
    const vec4& texture_color_offset, const vec4& texture_specular_coefficients,
    const vec4& texture_specular_offset, const float_t shininess) {
    data.buffer_batch_data.g_material_state_diffuse = diffuse;
    data.buffer_batch_data.g_material_state_ambient = ambient;
    data.buffer_batch_data.g_material_state_emission = emission;
    data.buffer_batch_data.g_material_state_shininess = { material_shininess, 0.0f, 0.0f, 1.0f };
    data.buffer_batch_data.g_material_state_specular = specular;
    data.buffer_batch_data.g_fresnel_coefficients = fresnel_coefficients;
    data.buffer_batch_data.g_texture_color_coefficients = texture_color_coefficients;
    data.buffer_batch_data.g_texture_color_offset = texture_color_offset;
    data.buffer_batch_data.g_texture_specular_coefficients = texture_specular_coefficients;
    data.buffer_batch_data.g_shininess.x = shininess;
    data.buffer_batch_data.g_texture_specular_offset = texture_specular_offset;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_material_color_emission(const vec4& emission) {
    data.buffer_batch_data.g_material_state_emission = emission;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_material_parameter(const vec4* specular, const vec4& bump_depth,
    const vec4& intensity, const float_t reflect_uv_scale, const float_t refract_uv_scale) {
    if (specular)
        data.buffer_batch_data.g_material_state_specular = *specular;
    data.buffer_batch_data.g_bump_depth = bump_depth;
    data.buffer_batch_data.g_intensity = intensity;
    data.buffer_batch_data.g_reflect_uv_scale.x = reflect_uv_scale;
    data.buffer_batch_data.g_reflect_uv_scale.y = reflect_uv_scale;
    data.buffer_batch_data.g_reflect_uv_scale.z = refract_uv_scale;
    data.buffer_batch_data.g_reflect_uv_scale.w = refract_uv_scale;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_min_alpha(const float_t value) {
    data.buffer_batch_data.g_max_alpha.w = value;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_morph_weight(const float_t value) {
    data.buffer_batch_data.g_morph_weight.x = value;
    data.buffer_batch_data.g_morph_weight.y = 1.0f - value;
    data.buffer_batch_data.g_morph_weight.z = 0.0f;
    data.buffer_batch_data.g_morph_weight.w = 0.0f;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_scene_camera(const cam_data& cam) {
    data.cam = cam;

    mat4_invert(&data.cam.get_view_mat(), &data.inv_view_mat);

    mat4 temp;
    mat4_transpose(&data.cam.get_view_mat(), &temp);
    data.buffer_scene_data.g_view[0] = temp.row0;
    data.buffer_scene_data.g_view[1] = temp.row1;
    data.buffer_scene_data.g_view[2] = temp.row2;

    mat4_invert(&data.cam.get_view_mat(), &temp);
    mat4_transpose(&temp, &temp);
    data.buffer_scene_data.g_view_inverse[0] = temp.row0;
    data.buffer_scene_data.g_view_inverse[1] = temp.row1;
    data.buffer_scene_data.g_view_inverse[2] = temp.row2;

    //mat4_mul(&data.cam.get_view_mat(), &data.cam.get_proj_mat(), &temp);
    mat4_transpose(&data.cam.get_view_proj_mat(), &temp);
    data.buffer_scene_data.g_projection_view[0] = temp.row0;
    data.buffer_scene_data.g_projection_view[1] = temp.row1;
    data.buffer_scene_data.g_projection_view[2] = temp.row2;
    data.buffer_scene_data.g_projection_view[3] = temp.row3;

    *(vec3*)&data.buffer_scene_data.g_view_position = cam.get_view_point();
    data.buffer_scene_data.g_view_position.w = 0.0f;
    //enum_or(data.flags, RENDER_DATA_BATCH_UPDATE | RENDER_DATA_SCENE_UPDATE);
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
}

void render_data_context::set_batch_sss_param(const vec4& sss_param) {
    data.buffer_batch_data.g_sss_param = sss_param;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_texcoord_transforms(const mat4 mats[2]) {
    mat4 temp;
    mat4_transpose(&mats[0], &temp);
    data.buffer_batch_data.g_texcoord_transforms[0] = temp.row0;
    data.buffer_batch_data.g_texcoord_transforms[1] = temp.row1;
    mat4_transpose(&mats[1], &temp);
    data.buffer_batch_data.g_texcoord_transforms[2] = temp.row0;
    data.buffer_batch_data.g_texcoord_transforms[3] = temp.row1;
    enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
}

void render_data_context::set_batch_worlds(const mat4& mat) {
    mat4 temp;
    mat4_transpose(&mat, &temp);
    if (data.buffer_batch_data.g_worlds[0] != temp.row0
        || data.buffer_batch_data.g_worlds[1] != temp.row1
        || data.buffer_batch_data.g_worlds[2] != temp.row2) {
        data.buffer_batch_data.g_worlds[0] = temp.row0;
        data.buffer_batch_data.g_worlds[1] = temp.row1;
        data.buffer_batch_data.g_worlds[2] = temp.row2;
        enum_or(data.flags, RENDER_DATA_BATCH_UPDATE);
    }
}

void render_data_context::set_glitter_render_data_state() {
    data.buffer_shader_data.set_shader_flags(data.shader_flags.arr);
    data.buffer_shader.WriteMemory(state, data.buffer_shader_data);
    state.bind_uniform_buffer_base(0, data.buffer_shader);

    if (data.flags & RENDER_DATA_SCENE_UPDATE) {
        data.buffer_scene.WriteMemory(state, data.buffer_scene_data);
        enum_and(data.flags, ~RENDER_DATA_SCENE_UPDATE);
    }
    state.bind_uniform_buffer_base(1, data.buffer_scene);
}

void render_data_context::set_render_data_state() {
    data.set_state(state);
}

void render_data_context::set_scene_fog_params(const render_data_context::fog_params& value) {
    data.buffer_scene_data.g_fog_depth_color = value.depth_color;
    data.buffer_scene_data.g_fog_height_params = value.height_params;
    data.buffer_scene_data.g_fog_height_color = value.height_color;
    data.buffer_scene_data.g_fog_bump_params = value.bump_params;
    const float_t start = value.start;
    const float_t end = value.end;
    data.buffer_scene_data.g_fog_state_params.x = value.density;
    data.buffer_scene_data.g_fog_state_params.y = value.start;
    data.buffer_scene_data.g_fog_state_params.z = value.end;
    data.buffer_scene_data.g_fog_state_params.w = end - start > 0.0f ? 1.0f / (end - start) : 0.0f;
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
}

void render_data_context::set_scene_framebuffer_size(const int32_t width, const int32_t height,
    const int32_t render_width, const int32_t render_height) {
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
    data.buffer_scene_data.g_framebuffer_size.x = 1.0f / (float_t)width;
    data.buffer_scene_data.g_framebuffer_size.y = 1.0f / (float_t)height;
    data.buffer_scene_data.g_framebuffer_size.w = 1.0f / (float_t)render_height;
    data.buffer_scene_data.g_framebuffer_size.z = 1.0f / (float_t)render_width;
}

void render_data_context::set_scene_light(const mat4& irradiance_r_transforms, const mat4& irradiance_g_transforms,
    const mat4& irradiance_b_transforms, const vec4& light_env_stage_diffuse,
    const vec4& light_env_stage_specular, const vec4& light_env_chara_diffuse,
    const vec4& light_env_chara_ambient, const vec4& light_env_chara_specular,
    const vec4& light_env_reflect_diffuse, const vec4& light_env_reflect_ambient,
    const vec4& light_env_proj_diffuse, const vec4& light_env_proj_specular,
    const vec4& light_env_proj_position, const vec4& light_stage_dir, const vec4& light_stage_diff,
    const vec4& light_stage_spec, const vec4& light_chara_dir, const vec4& light_chara_spec,
    const vec4& light_chara_luce, const vec4& light_chara_back, const vec4& light_face_diff,
    const vec4& chara_color0, const vec4& chara_color1, const vec4& chara_f_dir, const vec4& chara_f_ambient,
    const vec4& chara_f_diffuse, const vec4& chara_tc_param, const mat4& normal_tangent_transforms,
    const vec4& light_reflect_dir, const vec4& clip_plane, const vec4& npr_cloth_spec_color) {
    mat4 temp;
    mat4_transpose(&irradiance_r_transforms, &temp);
    data.buffer_scene_data.g_irradiance_r_transforms[0] = temp.row0;
    data.buffer_scene_data.g_irradiance_r_transforms[1] = temp.row1;
    data.buffer_scene_data.g_irradiance_r_transforms[2] = temp.row2;
    data.buffer_scene_data.g_irradiance_r_transforms[3] = temp.row3;
    mat4_transpose(&irradiance_g_transforms, &temp);
    data.buffer_scene_data.g_irradiance_g_transforms[0] = temp.row0;
    data.buffer_scene_data.g_irradiance_g_transforms[1] = temp.row1;
    data.buffer_scene_data.g_irradiance_g_transforms[2] = temp.row2;
    data.buffer_scene_data.g_irradiance_g_transforms[3] = temp.row3;
    mat4_transpose(&irradiance_b_transforms, &temp);
    data.buffer_scene_data.g_irradiance_b_transforms[0] = temp.row0;
    data.buffer_scene_data.g_irradiance_b_transforms[1] = temp.row1;
    data.buffer_scene_data.g_irradiance_b_transforms[2] = temp.row2;
    data.buffer_scene_data.g_irradiance_b_transforms[3] = temp.row3;
    data.buffer_scene_data.g_light_env_stage_diffuse = light_env_stage_diffuse;
    data.buffer_scene_data.g_light_env_stage_specular = light_env_stage_specular;
    data.buffer_scene_data.g_light_env_chara_diffuse = light_env_chara_diffuse;
    data.buffer_scene_data.g_light_env_chara_ambient = light_env_chara_ambient;
    data.buffer_scene_data.g_light_env_chara_specular = light_env_chara_specular;
    data.buffer_scene_data.g_light_env_reflect_diffuse = light_env_reflect_diffuse;
    data.buffer_scene_data.g_light_env_reflect_ambient = light_env_reflect_ambient;
    data.buffer_scene_data.g_light_env_proj_diffuse = light_env_proj_diffuse;
    data.buffer_scene_data.g_light_env_proj_specular = light_env_proj_specular;
    data.buffer_scene_data.g_light_env_proj_position = light_env_proj_position;
    data.buffer_scene_data.g_light_stage_dir = light_stage_dir;
    data.buffer_scene_data.g_light_stage_diff = light_stage_diff;
    data.buffer_scene_data.g_light_stage_spec = light_stage_spec;
    data.buffer_scene_data.g_light_chara_dir = light_chara_dir;
    data.buffer_scene_data.g_light_chara_spec = light_chara_spec;
    data.buffer_scene_data.g_light_chara_luce = light_chara_luce;
    data.buffer_scene_data.g_light_chara_back = light_chara_back;
    data.buffer_scene_data.g_light_face_diff = light_face_diff;
    data.buffer_scene_data.g_chara_color0 = chara_color0;
    data.buffer_scene_data.g_chara_color1 = chara_color1;
    data.buffer_scene_data.g_chara_f_dir = chara_f_dir;
    data.buffer_scene_data.g_chara_f_ambient = chara_f_ambient;
    data.buffer_scene_data.g_chara_f_diffuse = chara_f_diffuse;
    data.buffer_scene_data.g_chara_tc_param = chara_tc_param;
    mat4_transpose(&normal_tangent_transforms, &temp);
    data.buffer_scene_data.g_normal_tangent_transforms[0] = temp.row0;
    data.buffer_scene_data.g_normal_tangent_transforms[1] = temp.row1;
    data.buffer_scene_data.g_normal_tangent_transforms[2] = temp.row2;
    data.buffer_scene_data.g_light_reflect_dir = light_reflect_dir;
    data.buffer_scene_data.g_clip_plane = clip_plane;
    data.buffer_scene_data.g_npr_cloth_spec_color = npr_cloth_spec_color;
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
}

void render_data_context::set_scene_light_projection(const mat4& light_projection) {
    mat4 temp;
    mat4_transpose(&light_projection, &temp);
    data.buffer_scene_data.g_light_projection[0] = temp.row0;
    data.buffer_scene_data.g_light_projection[1] = temp.row1;
    data.buffer_scene_data.g_light_projection[2] = temp.row2;
    data.buffer_scene_data.g_light_projection[3] = temp.row3;
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
}

void render_data_context::set_scene_shadow_params(const float_t esm_param,
    const mat4 mats[2], const vec4& shadow_ambient, const vec4& shadow_ambient1) {
    data.buffer_scene_data.g_esm_param = { esm_param, 0.0f, 0.0f, 0.0f };
    mat4 temp;
    mat4_transpose(&mats[0], &temp);
    data.buffer_scene_data.g_self_shadow_receivers[0] = temp.row0;
    data.buffer_scene_data.g_self_shadow_receivers[1] = temp.row1;
    data.buffer_scene_data.g_self_shadow_receivers[2] = temp.row2;
    mat4_transpose(&mats[1], &temp);
    data.buffer_scene_data.g_self_shadow_receivers[3] = temp.row0;
    data.buffer_scene_data.g_self_shadow_receivers[4] = temp.row1;
    data.buffer_scene_data.g_self_shadow_receivers[5] = temp.row2;
    data.buffer_scene_data.g_shadow_ambient = shadow_ambient;
    data.buffer_scene_data.g_shadow_ambient1 = shadow_ambient1;
    enum_or(data.flags, RENDER_DATA_SCENE_UPDATE);
}

void render_data_context::set_self_shadow(bool value) {
    rctx_ptr->draw_state->rend_data[index].self_shadow = value;
}

void render_data_context::set_shader(uint32_t index) {
    data.set_shader(index);
}

void render_data_context::set_shadow(bool value) {
    rctx_ptr->draw_state->rend_data[index].shadow = value;
}

void render_data_context::set_skinning_data(p_gl_rend_state& p_gl_rend_st, const mat4* mats, int32_t count) {
    data.set_skinning_data(p_gl_rend_st, mats, count);
}

bool render_context::shared_storage_buffer::can_fill_data(size_t size) {
    return (size + align_val(offset, sv_min_storage_buffer_alignment)) <= this->size;
}

void render_context::shared_storage_buffer::create(size_t size) {
    if (buffer)
        return;

    size = size / sv_min_storage_buffer_alignment * sv_min_storage_buffer_alignment;

    buffer.Create(gl_state, size);
    data = force_malloc(size);
    memset(data, 0, size);
    offset = 0;
    this->size = size;
}

void render_context::shared_storage_buffer::destroy() {
    buffer.Destroy();
    free_def(data);
}

size_t render_context::shared_storage_buffer::fill_data(const void* data, size_t size) {
    size_t offset = align_val(this->offset, sv_min_storage_buffer_alignment);
    if (offset != this->offset)
        memset((void*)((size_t)this->data + this->offset), 0, offset - this->offset);
    memcpy((void*)((size_t)this->data + offset), data, size);
    this->offset = offset + size;
    return offset;
}

bool render_context::shared_uniform_buffer::can_fill_data(size_t size) {
    return (size + align_val(offset, sv_min_uniform_buffer_alignment)) <= this->size;
}

void render_context::shared_uniform_buffer::create(size_t size) {
    if (buffer)
        return;

    size = size / sv_min_uniform_buffer_alignment * sv_min_uniform_buffer_alignment;

    buffer.Create(gl_state, size);
    data = force_malloc(size);
    memset(data, 0, size);
    offset = 0;
    this->size = size;
}

void render_context::shared_uniform_buffer::destroy() {
    buffer.Destroy();
    free_def(data);
}

size_t render_context::shared_uniform_buffer::fill_data(const void* data, size_t size) {
    size_t offset = align_val(this->offset, sv_min_uniform_buffer_alignment);
    if (offset != this->offset)
        memset((void*)((size_t)this->data + this->offset), 0, offset - this->offset);
    memcpy((void*)((size_t)this->data + offset), data, size);
    this->offset = offset + size;
    return offset;
}

render_context::render_context() : litproj(), chara_reflect(), chara_refract(), box_vao(), lens_ghost_vao(),
common_vao(), empty_texture_2d(), empty_texture_cube_map(), samplers(), render_samplers(), sprite_samplers(),
sprite_width(), sprite_height(), screen_x_offset(), screen_y_offset(), screen_width(), screen_height() {
    camera = new ::camera;
    draw_state = new ::draw_state;
    disp_manager = new mdl::DispManager;
    render_manager = new rndr::RenderManager;
    sss_data = new ::sss_data;

    gl_state.get();

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
    gl_state.bind_vertex_array(box_vao);

    box_vbo.Create(gl_state, sizeof(box_texcoords), box_texcoords);
    gl_state.bind_array_buffer(box_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float_t) * 16, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float_t) * 16, (void*)(sizeof(float_t) * 4));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float_t) * 16, (void*)(sizeof(float_t) * 8));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float_t) * 16, (void*)(sizeof(float_t) * 12));

    glGenVertexArrays(1, &lens_ghost_vao);
    gl_state.bind_vertex_array(lens_ghost_vao);

    lens_ghost_vbo.Create(gl_state, sizeof(float_t) * 5 * (6 * 16));
    gl_state.bind_array_buffer(lens_ghost_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 5, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 5, (void*)(sizeof(float_t) * 2));
    gl_state.bind_array_buffer(0);
    gl_state.bind_vertex_array(0);

    glGenVertexArrays(1, &common_vao);

    camera_blur_ubo.Create(gl_state, sizeof(camera_blur_shader_data));
    contour_coef_ubo.Create(gl_state, sizeof(contour_coef_shader_data));
    contour_params_ubo.Create(gl_state, sizeof(contour_params_shader_data));
    filter_scene_ubo.Create(gl_state, sizeof(filter_scene_shader_data));
    esm_filter_batch_ubo.Create(gl_state, sizeof(esm_filter_batch_shader_data));
    imgfilter_batch_ubo.Create(gl_state, sizeof(imgfilter_batch_shader_data));
    exposure_measure_ubo.Create(gl_state, sizeof(exposure_measure_shader_data));
    gaussian_coef_ubo.Create(gl_state, sizeof(gaussian_coef_shader_data));
    glass_eye_batch_ubo.Create(gl_state, sizeof(glass_eye_batch_shader_data));
    glitter_batch_ubo.Create(gl_state, sizeof(glitter_batch_shader_data));
    quad_ubo.Create(gl_state, sizeof(quad_shader_data));
    sprite_scene_ubo.Create(gl_state, sizeof(sprite_scene_shader_data));
    sss_filter_gaussian_coef_ubo.Create(gl_state, sizeof(sss_filter_gaussian_coef_shader_data));
    sun_quad_ubo.Create(gl_state, sizeof(sun_quad_shader_data));
    tone_map_ubo.Create(gl_state, sizeof(tone_map_shader_data));
    transparency_batch_ubo.Create(gl_state, sizeof(transparency_batch_shader_data));

    for (render_data& i : data) {
        i = {};
        i.init();
    }

    static const vec4 border_color = 0.0f;

    glGenSamplers(18, samplers);
    for (int32_t i = 0; i < 18; i++) {
        GLuint sampler = samplers[i];

        glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER,
            i % 2 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

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

    GLuint sampler;
    glGenSamplers(4, render_samplers);
    sampler = render_samplers[0];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sampler = render_samplers[1];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sampler = render_samplers[2];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);

    sampler = render_samplers[3];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);

    glGenSamplers(3, sprite_samplers);
    sampler = sprite_samplers[0];
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

    sampler = sprite_samplers[1];
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

    sampler = sprite_samplers[2];
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

    max_uniform_block_size = min_def((uint32_t)sv_max_uniform_buffer_size, 64u * 1024);

    if (GLAD_GL_VERSION_4_3)
        max_storage_block_size = min_def((uint32_t)sv_max_storage_buffer_size, 1024u * 1024);
}

render_context::~render_context() {
    if (sv_shared_storage_uniform_buffer) {
        shared_uniform_buffer_entries.clear();

        for (render_context::shared_uniform_buffer& i : shared_uniform_buffers)
            i.destroy();
        shared_uniform_buffers.clear();

        if (GLAD_GL_VERSION_4_3) {
            shared_storage_buffer_entries.clear();

            for (render_context::shared_storage_buffer& i : shared_storage_buffers)
                i.destroy();
            shared_storage_buffers.clear();
        }
    }

    glDeleteSamplers(3, sprite_samplers);
    glDeleteSamplers(4, render_samplers);
    glDeleteSamplers(18, samplers);

    for (render_data& i : data)
        i.free();

    transparency_batch_ubo.Destroy();
    tone_map_ubo.Destroy();
    sun_quad_ubo.Destroy();
    sss_filter_gaussian_coef_ubo.Destroy();
    sprite_scene_ubo.Destroy();
    quad_ubo.Destroy();
    glitter_batch_ubo.Destroy();
    glass_eye_batch_ubo.Destroy();
    gaussian_coef_ubo.Destroy();
    exposure_measure_ubo.Destroy();
    imgfilter_batch_ubo.Destroy();
    esm_filter_batch_ubo.Destroy();
    filter_scene_ubo.Destroy();
    contour_params_ubo.Destroy();
    contour_coef_ubo.Destroy();
    camera_blur_ubo.Destroy();

    if (common_vao) {
        glDeleteVertexArrays(1, &common_vao);
        common_vao = 0;
    }

    if (lens_ghost_vao) {
        glDeleteVertexArrays(1, &lens_ghost_vao);
        lens_ghost_vao = 0;
    }

    lens_ghost_vbo.Destroy();

    if (box_vao) {
        glDeleteVertexArrays(1, &box_vao);
        box_vao = 0;
    }

    box_vbo.Destroy();

    if (sss_data) {
        delete sss_data;
        sss_data = 0;
    }

    if (render_manager) {
        delete render_manager;
        render_manager = 0;
    }

    if (disp_manager) {
        delete disp_manager;
        disp_manager = 0;
    }

    if (draw_state) {
        delete draw_state;
        draw_state = 0;
    }

    if (camera) {
        delete camera;
        camera = 0;
    }
}

void render_context::ctrl() {
    rctx_ptr = this;
    start_frame_process();

    extern bool game_state_get_pause();
    if (!get_pause() || !game_state_get_pause())
        app::TaskWork::ctrl();
    sound_ctrl();
    file_handler_storage_ctrl();
    finish_calc_process();
}

void render_context::disp() {
    rctx_ptr = this;
    start_render_process();
    disp_manager->refresh();
    draw_state->stats_update();
    sprite_manager_reset_req_list();
    camera->update();
    render_manager->cam.get(camera);
    app::TaskWork::disp();
    shadow_ptr_get()->ctrl();
    int32_t sprite_index = sprite_manager_get_index();
    //sprite_manager_set_index(3);
    sprite_manager_set_index(0);
    extern void dw_gui_ctrl_disp();
    dw_gui_ctrl_disp();
    sprite_manager_set_index(sprite_index);
    render_manager->rndpass_pre_proc();
    render_manager->render_all();
    render_manager->rndpass_post_proc();
    render_manager->field_31C = false;
    if (!get_pause())
        app::TaskWork::basic();
    finish_render_process();
}

void render_context::free() {
    shadow_buffer.Free();
    screen_overlay_buffer.Free();
    screen_buffer.Free();
    render_buffer.Free();
    reflect_buffer.Free();

    texture_release(empty_texture_cube_map);
    texture_release(empty_texture_2d);
}

void render_context::init() {
    static const uint8_t empty_texture_data[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    static const void* empty_texture_2d_array[] = {
        empty_texture_data,
    };

    empty_texture_2d = texture_load_tex_2d(texture_id(0x2F, 0),
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 4, 4, 0, empty_texture_2d_array, false);

    static const void* empty_texture_cube_map_array[] = {
        empty_texture_data,
        empty_texture_data,
        empty_texture_data,
        empty_texture_data,
        empty_texture_data,
        empty_texture_data,
    };

    empty_texture_cube_map = texture_load_tex_cube_map(texture_id(0x2F, 1),
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 4, 4, 0, empty_texture_cube_map_array);

    auto init_copy_buffer = [&](RenderTexture& src, RenderTexture& dst) {
        dst.Init(src.GetWidth(),
            src.GetHeight(), 0, src.color_texture->internal_format,
            src.depth_texture ? src.depth_texture->internal_format : GL_ZERO);
    };

    RenderTexture& render_buffer = render.rend_texture[0];
    RenderTexture& reflect_buffer = render_manager->get_render_texture(0);
    RenderTexture& shadow_buffer = shadow_ptr_get()->render_textures[1];
    init_copy_buffer(reflect_buffer, this->reflect_buffer);
    init_copy_buffer(render_buffer, this->render_buffer);
    init_copy_buffer(shadow_buffer, this->shadow_buffer);

    screen_buffer.Init(sprite_width, sprite_height, 0, GL_RGBA8, 0);
    screen_overlay_buffer.Init(sprite_width, sprite_height, 0, GL_RGBA8, 0);
}

void render_context::resize(int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height,
    int32_t screen_width, int32_t screen_height) {
    bool render_res_change = render.inner_width != render_width || render.inner_height != render_height;
    bool sprite_res_change = this->sprite_width != sprite_width || this->sprite_height != sprite_height;
    bool screen_res_change = this->screen_width != screen_width || this->screen_height != screen_height;
    if (!render_res_change && !sprite_res_change && !screen_res_change)
        return;

    this->sprite_width = sprite_width;
    this->sprite_height = sprite_height;

    screen_x_offset = (screen_width - sprite_width) / 2 + (screen_width - sprite_width) % 2;
    screen_y_offset = (screen_height - sprite_height) / 2 + (screen_height - sprite_height) % 2;

    this->screen_width = screen_width;
    this->screen_height = screen_height;

    if (render_res_change)
        render.resize(render_width, render_height);

    if (sprite_res_change)
        render_manager->resize(sprite_width, sprite_height);

    if (render_res_change)
        litproj->resize(render_width, render_height);

    sprite_manager_set_res((double_t)sprite_width / (double_t)sprite_height,
        sprite_width, sprite_height);

    if (render_res_change) {
        free();
        init();
    }
}

void render_context::add_shared_storage_uniform_buffer_data(size_t index,
    const void* data, size_t size, size_t buffer_size, bool storage) {
    if (storage) {
        render_context::shared_storage_buffer* buffer = 0;
        for (render_context::shared_storage_buffer& i : shared_storage_buffers)
            if (i.can_fill_data(buffer_size)) {
                buffer = &i;
                break;
            }

        if (!buffer) {
            shared_storage_buffers.push_back({});
            buffer = &shared_storage_buffers.back();
            buffer->create(max_storage_block_size);
        }

        size_t offset = buffer->fill_data(data, size);
        shared_storage_buffer_entries.insert({ index, { buffer->buffer, offset, buffer_size } });
    }
    else {
        render_context::shared_uniform_buffer* buffer = 0;
        for (render_context::shared_uniform_buffer& i : shared_uniform_buffers)
            if (i.can_fill_data(buffer_size)) {
                buffer = &i;
                break;
            }

        if (!buffer) {
            shared_uniform_buffers.push_back({});
            buffer = &shared_uniform_buffers.back();
            buffer->create(max_uniform_block_size);
        }

        size_t offset = buffer->fill_data(data, size);
        shared_uniform_buffer_entries.insert({ index, { buffer->buffer, offset, buffer_size } });
    }
}

bool render_context::get_shared_storage_uniform_buffer_data(size_t index,
    GLuint& buffer, size_t& offset, size_t& size, bool storage) {
    if (storage) {
        auto elem = shared_storage_buffer_entries.find(index);
        if (elem == shared_storage_buffer_entries.end())
            return false;

        buffer = elem->second.buffer;
        offset = elem->second.offset;
        size = elem->second.size;
        return true;
    }
    else {
        auto elem = shared_uniform_buffer_entries.find(index);
        if (elem == shared_uniform_buffer_entries.end())
            return false;

        buffer = elem->second.buffer;
        offset = elem->second.offset;
        size = elem->second.size;
        return true;
    }
}

void render_context::pre_proc() {
    if (sv_shared_storage_uniform_buffer) {
        auto add_obj_list = [](render_context* rctx, const mdl::ObjSubMeshArgs* args) {
            if (!args->mat_count || !args->mats)
                return;

            if (GLAD_GL_VERSION_4_3) {
                if (rctx->shared_storage_buffer_entries.find((size_t)args->mats)
                    != rctx->shared_storage_buffer_entries.end())
                    return;
            }
            else {
                if (rctx->shared_uniform_buffer_entries.find((size_t)args->mats)
                    != rctx->shared_uniform_buffer_entries.end())
                    return;
            }

            const int32_t mat_count = min_def(args->mat_count, 256);
            const mat4* mats = args->mats;
            vec4 g_joint_transforms[768];
            vec4* g_joint_transform = g_joint_transforms;

            mat4 mat;
            for (int32_t i = 0; i < mat_count; i++, mats++, g_joint_transform += 3) {
                mat4_transpose(mats, &mat);
                g_joint_transform[0] = mat.row0;
                g_joint_transform[1] = mat.row1;
                g_joint_transform[2] = mat.row2;
            }

            const size_t buffer_size = sizeof(vec4) * 3 * 256;
            const size_t size = sizeof(vec4) * 3 * mat_count;
            rctx->add_shared_storage_uniform_buffer_data((size_t)args->mats,
                g_joint_transforms, size, buffer_size, !!GLAD_GL_VERSION_4_3);
        };

        for (int32_t type = 0; type < mdl::OBJ_TYPE_MAX; type++)
            for (mdl::ObjData*& i : disp_manager->obj[type])
                switch (i->kind) {
                case mdl::OBJ_KIND_NORMAL:
                    add_obj_list(this, &i->args.sub_mesh);
                    break;
                case mdl::OBJ_KIND_TRANSLUCENT:
                    for (int32_t j = 0; j < i->args.translucent.count; j++)
                        add_obj_list(this, i->args.translucent.sub_mesh[j]);
                    break;
                }

        for (int32_t type = 0; type < mdl::OBJ_TYPE_LOCAL_MAX; type++)
            for (mdl::ObjData*& i : disp_manager->obj_local[type])
                switch (i->kind) {
                case mdl::OBJ_KIND_NORMAL:
                    add_obj_list(this, &i->args.sub_mesh);
                    break;
                case mdl::OBJ_KIND_TRANSLUCENT:
                    for (int32_t j = 0; j < i->args.translucent.count; j++)
                        add_obj_list(this, i->args.translucent.sub_mesh[j]);
                    break;
                }

        for (int32_t type = 0; type < mdl::OBJ_TYPE_REFLECT_MAX; type++)
            for (mdl::ObjData*& i : disp_manager->obj_reflect[type])
                switch (i->kind) {
                case mdl::OBJ_KIND_NORMAL:
                    add_obj_list(this, &i->args.sub_mesh);
                    break;
                case mdl::OBJ_KIND_TRANSLUCENT:
                    for (int32_t j = 0; j < i->args.translucent.count; j++)
                        add_obj_list(this, i->args.translucent.sub_mesh[j]);
                    break;
                }

        if (GLAD_GL_VERSION_4_3)
            for (render_context::shared_storage_buffer& i : shared_storage_buffers) {
                if (!i.offset)
                    continue;

                const size_t size = align_val(i.offset, sv_min_uniform_buffer_alignment);
                const size_t align = size - i.offset;
                if (align)
                    memset((void*)((size_t)i.data + i.offset), 0, align);
                i.buffer.WriteMemory(gl_state, 0, size, i.data);
            }

        for (render_context::shared_uniform_buffer& i : shared_uniform_buffers) {
            if (!i.offset)
                continue;

            const size_t size = align_val(i.offset, sv_min_uniform_buffer_alignment);
            const size_t align = size - i.offset;
            if (align)
                memset((void*)((size_t)i.data + i.offset), 0, align);
            i.buffer.WriteMemory(gl_state, 0, size, i.data);
        }
    }
}

void render_context::post_proc() {
    if (sv_shared_storage_uniform_buffer) {
        if (GLAD_GL_VERSION_4_3) {
            for (render_context::shared_storage_buffer& i : shared_storage_buffers)
                i.offset = 0;

            shared_storage_buffer_entries.clear();
        }

        for (render_context::shared_uniform_buffer& i : shared_uniform_buffers)
            i.offset = 0;

        shared_uniform_buffer_entries.clear();
    }
}
