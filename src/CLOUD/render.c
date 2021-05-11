/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.h"
#include "../CRE/camera.h"
#include "../CRE/dof.h"
#include "../CRE/post_process.h"
#include "../CRE/random.h"
#include "../CRE/shared.h"
#include "../CRE/static_var.h"
#include "../KKdLib/farc.h"
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <cimgui_impl.h>
#include "../CRE/fbo_dof.h"
#include "../CRE/fbo_hdr.h"
#include "../CRE/fbo_pp.h"
#include "../CRE/fbo_render.h"
#include "../CRE/lock.h"
#include "../CRE/Glitter/particle_manager.h"

vector(gl_object)
vector_func(gl_object)

vector_task_render tasks_render = { 0, 0, 0 };
vector_task_render_draw2d tasks_render_draw2d = { 0, 0, 0 };
vector_task_render_draw3d tasks_render_draw3d = { 0, 0, 0 };
static vector_task_render_draw2d tasks_render_draw2d_int = { 0, 0, 0 };
static vector_task_render_draw3d tasks_render_draw3d_int = { 0, 0, 0 };

static vector_task_render_draw3d temp_draw3d_tasks = { 0, 0, 0 };
static vector_gl_object temp_draw3d_objects = { 0, 0, 0 };

static vector_hash_ptr_gl_object vec_gl_obj = { 0, 0, 0 };
static vector_hash_light_dir vec_light_dir = { 0, 0, 0 };
static vector_hash_light_point vec_light_point = { 0, 0, 0 };
static vector_hash_material vec_mat = { 0, 0, 0 };
static vector_hash_shader_model vec_shad = { 0, 0, 0 };
static vector_hash_texture vec_tex = { 0, 0, 0 };
static vector_hash_texture_set vec_tex_set = { 0, 0, 0 };
static vector_hash_texture_bone_mat vec_tex_bone_mat = { 0, 0, 0 };
static vector_hash_ptr_vertex vec_vert = { 0, 0, 0 };

static texture light_dir_tex;
static texture light_point_tex;

static fbo_render* rfbo;
static fbo_hdr* hfbo;
static fbo_dof* dfbo;
static fbo_pp* pfbo;

static shader_fbo ffbs;
static shader_fbo bfbs[16];
static shader_fbo cfbs[10];
static shader_fbo dfbs[10];
static shader_fbo gfbs[5];
static shader_fbo hfbs[2];
static shader_fbo tfbs;
shader_fbo particle_shader;
shader_fbo sprite_shader;
shader_fbo grid_shader;

shader_model default_shader;
material default_material;
texture_set default_texture;
texture_bone_mat default_bone_mat;
vertex* default_vertex;

#define grid_size 50.0f
#define grid_spacing 0.5f
#define grid_vertex_count ((size_t)((grid_size * 2.0f) / grid_spacing) + 1) * 4

static int32_t fb_vao, fb_vbo;
static int32_t grid_vao, grid_vbo;
static int32_t common_ubo = 0;
static int32_t dof_common_ubo = 0;
static int32_t dof_texcoords_ubo = 0;
static int32_t tone_map_ubo = 0;
static int32_t global_matrices_ubo = 0;

bool      imgui = true;
bool         ui = true;
bool   front_2d = true;
bool g_front_3d = true;
bool    grid_3d = false;
bool c_front_3d = true;
bool    back_2d = true;
bool  g_back_3d = true;
bool  c_back_3d = true;
bool glitter_3d = true;

bool enable_post_process = true;

static const float_t light_dir_data_default[6] = {
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
};

static const float_t light_point_data_default[12] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

static const double_t const_scale[] = {
     1.0 / 8.0, //  12.5%
     2.0 / 8.0, //  25%
     3.0 / 8.0, //  37.5%
     4.0 / 8.0, //  50%
     5.0 / 8.0, //  62.5%
     6.0 / 8.0, //  75%
     7.0 / 8.0, //  87.5%
     8.0 / 8.0, // 100%
     9.0 / 8.0, // 112.5%
    10.0 / 8.0, // 125%
    11.0 / 8.0, // 137.5%
    12.0 / 8.0, // 150%
    13.0 / 8.0, // 162.5%
    14.0 / 8.0, // 175%
    15.0 / 8.0, // 187.5%
    16.0 / 8.0, // 200%
};

static float_t old_scale, scale;

static vec2i old_internal_2d_res;
static vec2i old_internal_3d_res;
vec2i internal_2d_res;
vec2i internal_3d_res;
vec2i internal_res;
int32_t width;
int32_t height;

static const double_t aspect = 16.0 / 9.0;
camera* cam;
dof_struct* dof;
radius* rad;
intensity* inten;
tone_map_sat_gamma* tmsg;
tone_map_data* tmd;
glitter_particle_manager* gpm;
glitter_type glt_type;
vec3 back3d_color;

static void render_load();
static void render_update();
static void render_draw();
static void render_dispose();

static void render_imgui();
static void render_2d(task_render_draw2d_type type);
static void render_c_3d(task_render_draw3d_type type);
static void render_c_3d_translucent(task_render_draw3d_type type);
static void render_g_3d(task_render_draw3d_type type);
static void render_grid_3d();
static void render_glitter_3d(int32_t alpha);

static void render_drop_glfw(GLFWwindow* window, int count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(bool change_fb);

static void render_dof_get_texcoords(vec2* data, float_t a2);
static void render_imgui_context_menu(classes_struct* classes, const size_t classes_count);
static bool render_glitter_mesh_add_list(glitter_particle_mesh* mesh, vec4* color, mat4* model, mat4* uv_mat);
static void render_update_uniform_c(task_render_uniform* uniform, shader_model* s);
static void render_update_uniform_g(task_render_uniform* uniform, shader_model* s);

extern bool close;
lock_val(render_lock);
HWND window_handle;
GLFWwindow* window;
ImGuiContext* ig;
bool global_context_menu;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(render);

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(render, "Render");

    lock_init(render_lock);
    if (!lock_check_init(render_lock))
        goto End;

    render_init_struct* ris = (render_init_struct*)arg;
    window_handle = 0;
    state = RENDER_UNINITIALIZED;

#pragma region GLFW GLEW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ris->res.x > 0 && ris->res.x < 8192 ? ris->res.x : mode->width;
    height = ris->res.y > 0 && ris->res.y < 8192 ? ris->res.y : mode->height;

    width = (int32_t)(width / 1.5f);
    height = (int32_t)(height / 1.5f);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, FREQ);

    const char* glfw_titlelabel;
#ifdef CLOUD_DEV
    glfw_titlelabel = "CLOUDDev";
#else
    glfw_titlelabel = "CLOUD";
#endif

    window = glfwCreateWindow(width, height, glfw_titlelabel,
        mode->width == width && mode->height == height ? monitor : 0, 0);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    window_handle = glfwGetWin32Window(window);
    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);
    glfwSetDropCallback(window, (void*)render_drop_glfw);
    glfwSetWindowSizeCallback(window, (void*)render_resize_fb_glfw);
    glfwSetWindowSizeLimits(window, 960, 540, GLFW_DONT_CARE, GLFW_DONT_CARE);

    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
    width = window_rect.right;
    height = window_rect.bottom;

    old_scale = scale = ris->scale > 0 ? ris->scale : 1.0f;
    internal_res.x = ris->internal_res.x > 0 && ris->internal_res.x < 8192 ? ris->internal_res.x : width;
    internal_res.y = ris->internal_res.y > 0 && ris->internal_res.y < 8192 ? ris->internal_res.y : height;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -2;
    }

    glGetIntegerv(GL_MAX_SAMPLES, &sv_max_samples);
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &sv_max_texture_buffer_size);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sv_max_texture_size);
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &sv_max_texture_max_anisotropy);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#pragma endregion

    state = RENDER_INITIALIZING;
    lock_lock(render_lock);
    render_load();
    lock_unlock(render_lock);
    state = RENDER_INITIALIZED;

#pragma region GL Init
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
#pragma endregion

    while (!close) {
        timer_calc_pre(render);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();
        lock_lock(render_lock);
        render_update();
        lock_unlock(render_lock);
        render_draw();
        glfwSwapBuffers(window);
        close |= glfwWindowShouldClose(window);
        double_t cycle_time = timer_calc_post(render);
        msleep(render_timer, 1000.0 / FREQ - cycle_time);
    }

    state = RENDER_DISPOSING;

    lock_lock(render_lock);
    render_dispose();
    lock_unlock(render_lock);

    state = RENDER_DISPOSED;

#pragma region GLFW Dispose
    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion
    lock_dispose(render_lock);

End:
    timer_dispose(render);
    return 0;
}

static void render_load() {
    tasks_render = (vector_task_render){ 0, 0, 0 };
    tasks_render_draw2d = (vector_task_render_draw2d){ 0, 0, 0 };
    tasks_render_draw3d = (vector_task_render_draw3d){ 0, 0, 0 };
    tasks_render_draw2d_int = (vector_task_render_draw2d){ 0, 0, 0 };
    tasks_render_draw3d_int = (vector_task_render_draw3d){ 0, 0, 0 };

    temp_draw3d_tasks = (vector_task_render_draw3d){ 0, 0, 0 };
    temp_draw3d_objects = (vector_gl_object){ 0, 0, 0 };

    vec_gl_obj = (vector_hash_ptr_gl_object){ 0, 0, 0 };
    vec_light_dir = (vector_hash_light_dir){ 0, 0, 0 };
    vec_light_point = (vector_hash_light_point){ 0, 0, 0 };
    vec_mat =  (vector_hash_material){ 0, 0, 0 };
    vec_shad = (vector_hash_shader_model){ 0, 0, 0 };
    vec_tex = (vector_hash_texture){ 0, 0, 0 };
    vec_tex_set = (vector_hash_texture_set){ 0, 0, 0 };
    vec_tex_bone_mat = (vector_hash_texture_bone_mat){ 0, 0, 0 };
    vec_vert = (vector_hash_ptr_vertex){ 0, 0, 0 };

    cam = camera_init();
    dof = dof_init();
    rad = radius_init();
    inten = intensity_init();
    tmsg = tone_map_sat_gamma_init();
    tmd = tone_map_data_init();
    gpm = glitter_particle_manager_init();

    dof_initialize(dof, 0, 0);
    radius_initialize(rad, (vec3[]) { 2.0f, 2.0f, 2.0f });
    intensity_initialize(inten, (vec3[]) { 1.0f, 1.0f, 1.0f });
    tone_map_sat_gamma_initialize(tmsg, 1.0f, 1, 1.0f);
    tone_map_data_initialize(tmd, 2.0f, true, (vec3[]) { 0.0f, 0.0f, 0.0f}, 0.0f, 0,
        (vec3[]) { 0.0f, 0.0f, 0.0f }, (vec3[]) { 1.0f, 1.0f, 1.0f }, 0);

    camera_initialize(cam, aspect, 70.0);
    camera_reset(cam);
    //camera_set_position(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
    //camera_rotate(cam, &(vec2d){ -45.0, -32.5 });
    //camera_set_position(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
    //camera_rotate(cam, &(vec2d){ 136.5, -20.5 });
    camera_set_position(cam, &(vec3){ 0.0f, 1.0f, 3.45f });

    glGenBuffers(1, &global_matrices_ubo);
    glGenBuffers(1, &dof_common_ubo);
    glGenBuffers(1, &common_ubo);
    glGenBuffers(1, &tone_map_ubo);
    glGenBuffers(1, &dof_texcoords_ubo);

    bind_uniform_buffer(global_matrices_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * 4, 0, GL_STREAM_DRAW);

    bind_uniform_buffer(dof_common_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 16 * 4, 0, GL_STREAM_DRAW);

    bind_uniform_buffer(common_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 4 * 4, 0, GL_STREAM_DRAW);

    bind_uniform_buffer(tone_map_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 16 * 4, 0, GL_STREAM_DRAW);
    bind_uniform_buffer(0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, global_matrices_ubo, 0, sizeof(mat4) * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_common_ubo, 0, 16 * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_ubo, 0, 4 * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, tone_map_ubo, 0, 16 * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_texcoords_ubo, 0, align_val(7 * 7, 2) * sizeof(vec2));

#pragma region Load Shaders
    char temp[0x80];
    memset(temp, 0, sizeof(temp));

    hfbo = fbo_hdr_init();
    rfbo = fbo_render_init();
    dfbo = fbo_dof_init();
    pfbo = fbo_pp_init();

    float_t verts_quad[] = {
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f
    };

    glGenBuffers(1, &fb_vbo);
    glGenVertexArrays(1, &fb_vao);
    bind_array_buffer(fb_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, GL_STATIC_DRAW);

    bind_vertex_array(fb_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0); // Pos
    bind_vertex_array(0);
    bind_array_buffer(0);

    farc* f = farc_init();
    farc_read(f, "rom\\core_shaders.farc", true, false);

    shader_param param;

    for (size_t i = 0; i < 5; i++) {
        memset(&param, 0, sizeof(shader_param));
        param.name = L"Color FB";
        param.vert = L"rfb";
        param.frag = L"cfb";
        sprintf_s(temp, sizeof(temp), "SAMPLES%d", 1 << i);
        param.param[0] = temp;
        shader_fbo_load(&cfbs[i], f, &param);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"Color FB";
        param.vert = L"rfb";
        param.frag = L"cfb";
        sprintf_s(temp, sizeof(temp), "SAMPLES%d", 1 << i);
        param.param[0] = temp;
        param.param[1] = "DEPTH";
        shader_fbo_load(&cfbs[i + 5], f, &param);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"G-Buffer FB";
        param.vert = L"rfb";
        param.frag = L"gfb";
        sprintf_s(temp, sizeof(temp), "SAMPLES%d", 1 << i);
        param.param[0] = temp;
        shader_fbo_load(&gfbs[i], f, &param);
    }

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Final FB";
    param.vert = L"ffb";
    param.frag = L"ffb";
    shader_fbo_load(&ffbs, f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"HDR FB";
    param.vert = L"hfb";
    param.frag = L"hfb";
    shader_fbo_load(&hfbs[0], f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"HDR FB";
    param.vert = L"hfb";
    param.frag = L"hfb";
    param.param[0] = "FXAA";
    shader_fbo_load(&hfbs[1], f, &param);

    wchar_t* dof_shaders[] = {
        L"dof1",
        L"dof2",
        L"dof3",
        L"dof4",
        L"dof5",
    };

    for (size_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 5; j++) {
            memset(&param, 0, sizeof(shader_param));
            param.name = L"DOF FB";
            param.vert = L"rfb";
            param.frag = dof_shaders[j];
            sprintf_s(temp, sizeof(temp), "USE_F2_COC (%lld)", i);
            param.param[0] = temp;
            shader_fbo_load(&dfbs[i * 5 + j], f, &param);
        }

    wchar_t* post_process_shaders[] = {
        L"blur1",
        L"blur2",
        L"blur3",
        L"blur3",
        L"blur3",
        L"exposure1",
        L"blur4",
        L"blur4",
        L"blur4",
        L"blur4",
        L"blur4",
        L"blur4",
        L"blur5",
        L"blur6",
        L"exposure2",
        L"exposure3",
    };

    for (size_t i = 0; i < 16; i++) {
        memset(&param, 0, sizeof(shader_param));
        param.param[0] = temp;
        param.name = L"Post Process FB";
        param.vert = post_process_shaders[i];
        param.frag = post_process_shaders[i];
        shader_fbo_load(&bfbs[i], f, &param);
    }

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Tone Map FB";
    param.vert = L"tone_map";
    param.frag = L"tone_map";
    shader_fbo_load(&tfbs, f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Glitter Particle";
    param.vert = L"glitter_particle";
    param.frag = L"glitter_particle";
    shader_fbo_load(&particle_shader, f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Sprite";
    param.vert = L"sprite";
    param.frag = L"sprite";
    shader_fbo_load(&sprite_shader, f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Grid";
    param.vert = L"grid";
    param.frag = L"grid";
    shader_fbo_load(&grid_shader, f, &param);
    farc_dispose(f);

    {
        uint8_t tex_dat_diffuse[] = {
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
        };

        texture_data tex_data_diffuse = {
            .type = TEXTURE_2D,
            .width = 8,
            .height = 8,
            .depth = 0,
            .data = tex_dat_diffuse,
            .pixel_type = GL_UNSIGNED_BYTE,
            .pixel_format = GL_RGB,
            .pixel_internal_format = GL_RGB8,
            .generate_mipmap = false,
            .wrap_mode_s = GL_REPEAT,
            .wrap_mode_t = GL_REPEAT,
            .wrap_mode_r = GL_REPEAT,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST,
        };

        uint8_t tex_dat_other[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };

        texture_data tex_data_other = {
            .type = TEXTURE_2D,
            .width = 8,
            .height = 8,
            .depth = 0,
            .data = tex_dat_other,
            .pixel_type = GL_UNSIGNED_BYTE,
            .pixel_format = GL_RGB,
            .pixel_internal_format = GL_RGB8,
            .generate_mipmap = false,
            .wrap_mode_s = GL_REPEAT,
            .wrap_mode_t = GL_REPEAT,
            .wrap_mode_r = GL_REPEAT,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST,
        };

        texture_set_data tex_set;
        memset(&tex_set, 0, sizeof(texture_set_data));
        tex_set.diffuse = &tex_data_diffuse;
        tex_set.specular = &tex_data_other;
        tex_set.displacement = &tex_data_other;
        tex_set.normal = &tex_data_other;
        tex_set.albedo = &tex_data_other;
        tex_set.ao = &tex_data_other;
        tex_set.metallic = &tex_data_other;
        tex_set.roughness = &tex_data_other;
        texture_set_load(&default_texture, &tex_set);

        default_material.texture = default_texture;
        default_material.blend = material_blend_default;
        default_material.translucent = false;

        shader_model_update param_model;
        memset(&param_model, 0, sizeof(shader_model_update));

        stream* s;
        size_t l;

        s = io_wopen(L"rom\\shaders\\0000.vert", L"r");
        l = s->length;
        param_model.vert = force_malloc(l + 1);
        io_read(s, param_model.vert, l);
        param_model.vert[l] = 0;
        io_dispose(s);

        s = io_wopen(L"rom\\shaders\\0000_c.frag", L"r");
        l = s->length;
        param_model.frag_c = force_malloc(l + 1);
        io_read(s, param_model.frag_c, l);
        param_model.frag_c[l] = 0;
        io_dispose(s);

        s = io_wopen(L"rom\\shaders\\0000_g.frag", L"r");
        l = s->length;
        param_model.frag_g = force_malloc(l+ 1);
        io_read(s, param_model.frag_g, l);
        param_model.frag_g[l] = 0;
        io_dispose(s);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"Default";
        param_model.param = param;
        shader_model_load(&default_shader, &param_model);
        free(param_model.vert);
        free(param_model.frag_c);
        free(param_model.frag_g);

        float_t verts_cube[] = {
            -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
             1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,

            -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,

             1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
             1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

            -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
             1.0f,  1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        };

        vertex_update vert_upd;
        memset(&vert_upd, 0, sizeof(vertex_update));
        vert_upd.data = verts_cube;
        vert_upd.length = sizeof(verts_cube) / sizeof(float_t);
        vert_upd.uv = true;
        vert_upd.normal = true;

        default_vertex = vertex_init();
        vertex_load(default_vertex, &vert_upd);

        texture_bone_mat_data tex_bone_mat;
        memset(&tex_bone_mat, 0, sizeof(texture_bone_mat_data));
        tex_bone_mat.count = 1;
        tex_bone_mat.data = (mat4*)&mat4_identity;
        texture_bone_mat_load(&default_bone_mat, &tex_bone_mat);

        texture_data light_dir_tex_data = {
            .type = TEXTURE_2D,
            .width = 2,
            .height = 1,
            .depth = 0,
            .data = (void*)light_dir_data_default,
            .pixel_type = GL_FLOAT,
            .pixel_format = GL_RGB,
            .pixel_internal_format = GL_RGB32F,
            .generate_mipmap = false,
            .wrap_mode_s = GL_REPEAT,
            .wrap_mode_t = GL_REPEAT,
            .wrap_mode_r = GL_REPEAT,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST,
        };

        memset(&light_dir_tex, 0, sizeof(texture));
        texture_load(&light_dir_tex, &light_dir_tex_data);

        texture_data light_point_tex_data = {
            .type = TEXTURE_2D,
            .width = 3,
            .height = 1,
            .depth = 0,
            .data = (void*)light_point_data_default,
            .pixel_type = GL_FLOAT,
            .pixel_format = GL_RGBA,
            .pixel_internal_format = GL_RGBA32F,
            .generate_mipmap = false,
            .wrap_mode_s = GL_REPEAT,
            .wrap_mode_t = GL_REPEAT,
            .wrap_mode_r = GL_REPEAT,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST,
        };

        memset(&light_point_tex, 0, sizeof(texture));
        texture_load(&light_point_tex, &light_point_tex_data);
    }

    for (size_t i = 0; i < 10; i++) {
        shader_fbo_set_int(&cfbs[i], "ColorTexture", 0);
        if (i >= 5)
            shader_fbo_set_int(&cfbs[i], "DepthTexture", 1);
    }

    for (size_t i = 0; i < 5; i++) {
        shader_fbo_set_int(&gfbs[i], "gPos", 0);
        shader_fbo_set_int(&gfbs[i], "gAlbedo", 1);
        shader_fbo_set_int(&gfbs[i], "gSpec", 2);
        shader_fbo_set_int(&gfbs[i], "gNormal", 3);
        shader_fbo_set_int(&gfbs[i], "lightDir", 4);
        shader_fbo_set_int(&gfbs[i], "lightPoint", 5);
    }

    shader_fbo_set_int(&ffbs, "Texture", 0);

    for (size_t i = 0; i < 2; i++)
        shader_fbo_set_int(&hfbs[i], "Color", 0);

    for (size_t i = 0; i < 10; i += 5) {
        shader_fbo_set_int(&dfbs[i], "Depth", 0);
        shader_fbo_set_int(&dfbs[i + 1], "Tile", 0);
        shader_fbo_set_int(&dfbs[i + 2], "Depth", 0);
        shader_fbo_set_int(&dfbs[i + 2], "Color", 1);
        shader_fbo_set_int(&dfbs[i + 2], "Tile", 2);
        shader_fbo_set_int(&dfbs[i + 3], "PreSort", 0);
        shader_fbo_set_int(&dfbs[i + 3], "PreColor", 1);
        shader_fbo_set_int(&dfbs[i + 3], "Tile", 2);
        shader_fbo_set_int(&dfbs[i + 4], "FiltColor", 0);
        shader_fbo_set_int(&dfbs[i + 4], "FiltAlpha", 1);
        shader_fbo_set_int(&dfbs[i + 4], "Tile", 2);
        shader_fbo_set_int(&dfbs[i + 4], "Color", 3);
        shader_fbo_set_int(&dfbs[i + 4], "Depth", 4);

        shader_fbo_set_uniform_block_binding(&dfbs[i], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 2], "Common", 0);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 2], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "Common", 0);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "DOFTexcoords", 2);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 4], "Common", 0);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 4], "DOFCommon", 1);
    }

    for (size_t i = 0; i < 2; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);

    for (size_t i = 2; i < 5; i++) {
        shader_fbo_set_int(&bfbs[i], "Texture", 0);
        shader_fbo_set_int(&bfbs[i], "TexLuma", 1);
    }

    for (size_t i = 5; i < 13; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);

    shader_fbo_set_int(&bfbs[13], "Texture0", 0);
    shader_fbo_set_int(&bfbs[13], "Texture1", 1);
    shader_fbo_set_int(&bfbs[13], "Texture2", 2);
    shader_fbo_set_int(&bfbs[13], "Texture3", 3);

    for (size_t i = 14; i < 16; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);

    shader_fbo_set_int(&tfbs, "Texture0", 0);
    shader_fbo_set_int(&tfbs, "Texture1", 1);
    shader_fbo_set_int(&tfbs, "Texture2", 2);
    shader_fbo_set_int(&tfbs, "Texture3", 3);
    shader_fbo_set_uniform_block_binding(&tfbs, "ToneMap", 1);

    shader_fbo_set_int(&particle_shader, "Texture0", 0);
    shader_fbo_set_int(&particle_shader, "Texture1", 1);
    shader_fbo_set_uniform_block_binding(&particle_shader, "GlobalMatrices", 0);

    shader_fbo_set_int_array(&sprite_shader, "Texture", 3, ((int32_t[]){ 0, 1, 2 }));
    shader_fbo_set_uniform_block_binding(&sprite_shader, "Common", 0);

    shader_fbo_set_uniform_block_binding(&grid_shader, "GlobalMatrices", 0);

    render_resize_fb(false);

    fbo_render_initialize(rfbo, &internal_3d_res, fb_vao, cfbs, gfbs);
    fbo_hdr_initialize(hfbo, &internal_3d_res, &internal_2d_res, fb_vao, &ffbs, hfbs);
    fbo_dof_initialize(dfbo, &internal_3d_res, fb_vao, dfbs, dof_common_ubo, dof_texcoords_ubo);
    fbo_pp_initialize(pfbo, &internal_3d_res, fb_vao, bfbs, &tfbs, tone_map_ubo);

    render_resize_fb(true);
#pragma endregion

    const int32_t dof_tex_len = align_val(7 * 7, 2);
    vec2 dof_texcoords[align_val(7 * 7, 2)];
    memset(dof_texcoords, 0, dof_tex_len * sizeof(vec2));
    render_dof_get_texcoords(dof_texcoords, 3.0f);
    bind_uniform_buffer(dof_texcoords_ubo);
    glBufferData(GL_UNIFORM_BUFFER, dof_tex_len * sizeof(vec2), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, dof_tex_len * sizeof(vec2), dof_texcoords);
    bind_uniform_buffer(0);

    float_t* grid_verts = force_malloc_s(float_t, grid_vertex_count * 7);

    size_t v = 0;
    for (float_t x = -grid_size; x <= grid_size; x += grid_spacing) {
        vec4 x_color;
        vec4 z_color;
        if (x == 0) {
            x_color = (vec4){ 1.0f, 0.0f, 0.0f, 1.0f };
            z_color = (vec4){ 0.0f, 1.0f, 0.0f, 1.0f };
        }
        else if (abs((int32_t)(x / grid_spacing)) % 2 == 0) {
            x_color = (vec4){ 0.2f, 0.2f, 0.2f, 0.6f };
            z_color = (vec4){ 0.2f, 0.2f, 0.2f, 0.6f };
        }
        else {
            x_color = (vec4){ 0.5f, 0.5f, 0.5f, 0.6f };
            z_color = (vec4){ 0.5f, 0.5f, 0.5f, 0.6f };
        }

        grid_verts[v++] = x;
        grid_verts[v++] = 0.0f;
        grid_verts[v++] = -grid_size;
        grid_verts[v++] = x_color.x;
        grid_verts[v++] = x_color.y;
        grid_verts[v++] = x_color.z;
        grid_verts[v++] = x_color.w;

        grid_verts[v++] = x;
        grid_verts[v++] = 0.0f;
        grid_verts[v++] = grid_size;
        grid_verts[v++] = x_color.x;
        grid_verts[v++] = x_color.y;
        grid_verts[v++] = x_color.z;
        grid_verts[v++] = x_color.w;

        grid_verts[v++] = -grid_size;
        grid_verts[v++] = 0.0f;
        grid_verts[v++] = x;
        grid_verts[v++] = z_color.x;
        grid_verts[v++] = z_color.y;
        grid_verts[v++] = z_color.z;
        grid_verts[v++] = z_color.w;

        grid_verts[v++] = grid_size;
        grid_verts[v++] = 0.0f;
        grid_verts[v++] = x;
        grid_verts[v++] = z_color.x;
        grid_verts[v++] = z_color.y;
        grid_verts[v++] = z_color.z;
        grid_verts[v++] = z_color.w;
    }

    glGenBuffers(1, &grid_vbo);
    glGenVertexArrays(1, &grid_vao);
    bind_array_buffer(grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * grid_vertex_count * 7, grid_verts, GL_STATIC_DRAW);

    bind_vertex_array(grid_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 7, (void*)0); // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float_t) * 7, (void*)(sizeof(float_t) * 3)); // Color
    bind_vertex_array(0);
    bind_array_buffer(0);

    free(grid_verts);

    ig = igCreateContext(0);
    ImGuiIO* io = igGetIO();
    io->IniFilename = 0;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    igStyleColorsDark(0);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    back3d_color = (vec3){ 0.74117647f, 0.74117647f, 0.74117647f };

    classes_process_init(classes, classes_count);
}

extern vec2d input_move;
extern vec2d input_rotate;
extern double_t input_roll;
extern bool input_reset;

static void render_update() {
    global_context_menu = true;
    classes_process_imgui(classes, classes_count);

    if (global_context_menu && igIsMouseReleased(ImGuiMouseButton_Right)
        && !igIsItemHovered(0) && ig->OpenPopupStack.Size <= 0)
        igOpenPopup("Classes init context menu", 0);

    if (igBeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count);
        igEndPopup();
    }

    igRender();

    if (window_handle == GetForegroundWindow()) {
        if (input_reset) {
            input_reset = false;
            camera_reset(cam);
            //camera_set_position(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
            //camera_rotate(cam, &(vec2d){ -45.0, -32.5 });
            //camera_set_position(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
            //camera_rotate(cam, &(vec2d){ 136.5, -20.5 });
            camera_set_position(cam, &(vec3){ 0.0f, 1.0f, 3.45f });
        }
        else {
            camera_rotate(cam, &input_rotate);
            camera_move(cam, &input_move);
            camera_roll(cam, input_roll);
        }
    }
    camera_update(cam);

    shader_model_c_set_int_array(&default_shader, "material", 8, ((int32_t[]){ 0, 1, 2, 3, 4, 5, 6, 7 }));
    shader_model_c_set_int_array(&default_shader, "tex_mode", 8, ((int32_t[]){ 0, 0, 0, 0, 0, 0, 0, 0 }));
    shader_model_c_set_bool(&default_shader, "use_bones", false);
    shader_model_c_set_bool(&default_shader, "write_only_depth", false);
    shader_model_c_set_mat4(&default_shader, "model", false, &mat4_identity);
    shader_model_c_set_mat4(&default_shader, "model_normal", false, &mat3_identity);
    shader_model_c_set_mat4(&default_shader, "uv_mat", false, &mat4_identity);
    shader_model_c_set_vec4(&default_shader, "color", 1.0f, 1.0f, 1.0f, 1.0f);
    shader_model_c_set_int(&default_shader, "BoneMatrix", 8);

    shader_model_g_set_int_array(&default_shader, "material", 8, ((int32_t[]){ 0, 1, 2, 3, 4, 5, 6, 7 }));
    shader_model_g_set_int_array(&default_shader, "tex_mode", 8, ((int32_t[]){ 0, 0, 0, 0, 0, 0, 0, 0 }));
    shader_model_g_set_bool(&default_shader, "use_bones", false);
    shader_model_g_set_mat4(&default_shader, "model", false, &mat4_identity);
    shader_model_g_set_mat4(&default_shader, "model_normal", false, &mat3_identity);
    shader_model_g_set_mat4(&default_shader, "uv_mat", false, &mat4_identity);
    shader_model_g_set_vec4(&default_shader, "color", 1.0f, 1.0f, 1.0f, 1.0f);
    shader_model_g_set_int(&default_shader, "BoneMatrix", 8);

    shader_model_set_uniform_block_binding(&default_shader, "GlobalMatrices", 0);

    classes_process_render(classes, classes_count);

    glitter_particle_manager_calc_draw(gpm, glt_type, render_glitter_mesh_add_list);

    bool light_dir_tex_update = false;
    bool light_point_tex_update = false;
    for (task_render* i = tasks_render.begin; i != tasks_render.end; ) {
        if (i->type == TASK_RENDER_UPDATE) {
            task_render_update update = i->update;
            switch (update.type) {
            case TASK_RENDER_UPDATE_GL_OBJECT: {
                bool found = false;
                gl_object* gl_obj = 0;
                for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        gl_obj = j->data;
                        break;
                    }

                if (!found)
                    gl_obj = gl_object_init();

                bool found_vert = false;
                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.vert)) {
                        found_vert = true;
                        gl_object_update_vert(gl_obj, j->data);
                        break;
                    }

                if (!found_vert)
                    gl_object_update_vert(gl_obj, default_vertex);

                bool found_material = false;
                for (hash_material* j = vec_mat.begin; j != vec_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.material)) {
                        found_material = true;
                        gl_object_update_material(gl_obj, &j->data);
                        break;
                    }

                if (!found_material)
                    gl_object_update_material(gl_obj, &default_material);

                bool found_shader = false;
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.shader)) {
                        found_shader = true;
                        gl_object_update_shader(gl_obj, &j->data);
                        break;
                    }

                if (!found_shader)
                    gl_object_update_shader(gl_obj, &default_shader);

                bool found_bone_mat = false;
                for (hash_texture_bone_mat* j = vec_tex_bone_mat.begin; j != vec_tex_bone_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.bone_mat)) {
                        found_bone_mat = true;
                        gl_object_update_bone_mat(gl_obj, &j->data);
                        break;
                    }

                if (!found_bone_mat)
                    gl_object_update_bone_mat(gl_obj, &default_bone_mat);

                if (!found) {
                    hash_ptr_gl_object hash_gl_obj;
                    hash_gl_obj.hash = update.hash;
                    hash_gl_obj.data = gl_obj;
                    vector_hash_ptr_gl_object_push_back(&vec_gl_obj, &hash_gl_obj);
                }

                gl_object_update_cull_face(gl_obj, update.gl_obj.cull_face);
            } break;
            case TASK_RENDER_UPDATE_MATERIAL: {
                bool found = false;
                material* mat;
                for (hash_material* j = vec_mat.begin; j != vec_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        mat = &j->data;
                        break;
                    }

                if (!found) {
                    material m;
                    memset(&m, 0, sizeof(material));
                    mat = &m;
                }

                mat->blend = update.mat.blend;
                mat->translucent = update.mat.translucent;

                bool found_texture = false;
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (HASH_COMPARE(j->hash, update.mat.texture)) {
                        found_texture = true;
                        mat->texture = j->data;
                        break;
                    }

                if (!found_texture)
                    mat->texture = default_texture;

                if (!found) {
                    hash_material hash_mat;
                    hash_mat.hash = update.hash;
                    hash_mat.data = *mat;
                    vector_hash_material_push_back(&vec_mat, &hash_mat);
                }
            } break;
            case TASK_RENDER_UPDATE_LIGHT_DIR: {
                bool found = false;
                light_dir* light;
                for (hash_light_dir* j = vec_light_dir.begin; j != vec_light_dir.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        light = &j->data;
                        break;
                    }

                light_dir_update* l = &update.light_dir;

                vec3 color;
                vec3 dir;
                color = *(vec3*)&l->color;
                vec3_mult_scalar(color, l->color.w, color);
                vec3_normalize(l->dir, dir);

                if (!found) {
                    hash_light_dir hash_light_dir;
                    hash_light_dir.hash = update.hash;
                    hash_light_dir.data.dir = dir;
                    hash_light_dir.data.color = color;
                    vector_hash_light_dir_push_back(&vec_light_dir, &hash_light_dir);
                }
                else {
                    light->dir = dir;
                    light->color = color;
                }
                light_dir_tex_update = true;
            } break;
            case TASK_RENDER_UPDATE_LIGHT_POINT: {
                bool found = false;
                light_point* light;
                for (hash_light_point* j = vec_light_point.begin; j != vec_light_point.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        light = &j->data;
                        break;
                    }

                light_point_update* l = &update.light_point;

                vec3 color;
                color = *(vec3*)&l->color;
                vec3_mult_scalar(color, l->color.w, color);

                float_t radius = light_point_calculate_radius(&color,
                    l->constant, l->linear, l->quadratic);

                if (!found) {
                    hash_light_point hash_light_point;
                    hash_light_point.hash = update.hash;
                    hash_light_point.data.position = l->position;
                    hash_light_point.data.color = color;
                    hash_light_point.data.constant = l->constant;
                    hash_light_point.data.linear = l->linear;
                    hash_light_point.data.quadratic = l->quadratic;
                    hash_light_point.data.radius = radius;
                    vector_hash_light_point_push_back(&vec_light_point, &hash_light_point);
                }
                else {
                    light->position = l->position;
                    light->color = color;
                    light->constant = l->constant;
                    light->linear = l->linear;
                    light->quadratic = l->quadratic;
                    light->radius = radius;
                }
                light_point_tex_update = true;
            } break;
            case TASK_RENDER_UPDATE_SHADER: {
                bool found = false;
                shader_model* shad;
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        shad = &j->data;
                        break;
                    }

                if (!found) {
                    shader_model s;
                    memset(&s, 0, sizeof(s));
                    shader_model_load(&s, &update.shad);

                    hash_shader_model hash_shad;
                    hash_shad.hash = update.hash;
                    hash_shad.data = s;
                    vector_hash_shader_model_push_back(&vec_shad, &hash_shad);
                }
                else {
                    shader_model_free(shad);
                    shader_model_load(shad, &update.shad);
                }
                free(update.shad.frag_c);
                free(update.shad.frag_g);
                free(update.shad.vert);
                free(update.shad.geom);
            } break;
            case TASK_RENDER_UPDATE_TEXTURE: {
                bool found = false;
                texture* tex;
                for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        tex = &j->data;
                        break;
                    }

                if (!found) {
                    texture t;
                    memset(&t, 0, sizeof(texture));
                    texture_load(&t, &update.tex);
                    hash_texture hash_tex;
                    hash_tex.hash = update.hash;
                    hash_tex.data = t;
                    vector_hash_texture_push_back(&vec_tex, &hash_tex);
                }
                else
                    texture_load(tex, &update.tex);
            } break;
            case TASK_RENDER_UPDATE_TEXTURE_SET: {
                bool found = false;
                texture_set* tex_set;
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        tex_set = &j->data;
                        break;
                    }

                if (!found) {
                    texture_set t;
                    memset(&t, 0, sizeof(texture_set));
                    texture_set_load(&t, &update.tex_set);

                    hash_texture_set hash_tex_set;
                    hash_tex_set.hash = update.hash;
                    hash_tex_set.data = t;
                    vector_hash_texture_set_push_back(&vec_tex_set, &hash_tex_set);
                }
                else
                    texture_set_load(tex_set, &update.tex_set);
            } break;
            case TASK_RENDER_UPDATE_VERT: {
                bool found = false;
                vertex* vert = 0;
                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        vert = j->data;
                        break;
                    }

                if (!found) {
                    vert = vertex_init();
                    vertex_load(vert, &update.vert);

                    hash_ptr_vertex hash_vert;
                    hash_vert.hash = update.hash;
                    hash_vert.data = vert;
                    vector_hash_ptr_vertex_push_back(&vec_vert, &hash_vert);
                }
                else
                    vertex_load(vert, &update.vert);
            } break;
            }
        }
        else if (i->type == TASK_RENDER_FREE) {
            task_render_free free_data = i->free;
            switch (free_data.type) {
            case TASK_RENDER_FREE_GL_OBJECT:
                for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        gl_object_dispose(j->data);
                        vector_hash_ptr_gl_object_erase(&vec_gl_obj, j - vec_gl_obj.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_LIGHT_DIR: {
                bool found = false;
                for (hash_light_dir* j = vec_light_dir.begin; j != vec_light_dir.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        found = true;
                        vector_hash_light_dir_erase(&vec_light_dir, j - vec_light_dir.begin);
                        break;
                    }
            } break;
            case TASK_RENDER_FREE_LIGHT_POINT: {
                for (hash_light_point* j = vec_light_point.begin; j != vec_light_point.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        vector_hash_light_point_erase(&vec_light_point, j - vec_light_point.begin);
                        break;
                    }
            } break;
            case TASK_RENDER_FREE_MATERIAL:
                for (hash_material* j = vec_mat.begin; j != vec_mat.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        vector_hash_material_erase(&vec_mat, j - vec_mat.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_SHADER:
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        shader_model_free(&j->data);
                        vector_hash_shader_model_erase(&vec_shad, j - vec_shad.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_TEXTURE:
                for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        texture_free(&j->data);
                        vector_hash_texture_erase(&vec_tex, j - vec_tex.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_TEXTURE_SET:
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        texture_set_free(&j->data);
                        vector_hash_texture_set_erase(&vec_tex_set, j - vec_tex_set.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_VERT:
                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        vertex_dispose(j->data);
                        vector_hash_ptr_vertex_erase(&vec_vert, j - vec_vert.begin);
                        break;
                    }
                break;
            }
        }
        else if (i->type == TASK_RENDER_UNIFORM) {
            task_render_uniform uni = i->uniform;
            shader_model* s = 0;
            for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                if (HASH_COMPARE(j->hash, uni.shader_hash)) {
                    s = &j->data;
                    break;
                }

            if (!s)
                s = &default_shader;

            render_update_uniform_c(&uni, s);
            render_update_uniform_g(&uni, s);
            char_buffer_dispose(&uni.name);
        }
        memset(i, 0, sizeof(task_render));
        vector_task_render_erase(&tasks_render, 0);
    }

    if (light_dir_tex_update) {
        size_t light_dir_count = vec_light_dir.end - vec_light_dir.begin;
        if (light_dir_count) {
            const size_t light_dir_size = 6;
            float_t* light_dir_data = force_malloc_s(sizeof(float_t), light_dir_size * light_dir_count);

            for (size_t i = 0; i < light_dir_count; i++) {
                light_dir* light = &vec_light_dir.begin[i].data;
                light_dir_data[i * light_dir_size] = light->dir.x;
                light_dir_data[i * light_dir_size + 1] = light->dir.y;
                light_dir_data[i * light_dir_size + 2] = light->dir.z;
                light_dir_data[i * light_dir_size + 3] = light->color.x;
                light_dir_data[i * light_dir_size + 4] = light->color.y;
                light_dir_data[i * light_dir_size + 5] = light->color.z;
            }

            texture_data light_dir_tex_data = {
                .type = TEXTURE_2D,
                .width = 2,
                .height = (int32_t)light_dir_count,
                .depth = 0,
                .data = light_dir_data,
                .pixel_type = GL_FLOAT,
                .pixel_format = GL_RGB,
                .pixel_internal_format = GL_RGB32F,
                .generate_mipmap = false,
                .wrap_mode_s = GL_REPEAT,
                .wrap_mode_t = GL_REPEAT,
                .wrap_mode_r = GL_REPEAT,
                .min_filter = GL_NEAREST,
                .mag_filter = GL_NEAREST,
            };

            texture_load(&light_dir_tex, &light_dir_tex_data);
            free(light_dir_data);
        }
        else {
            texture_data light_dir_tex_data = {
                .type = TEXTURE_2D,
                .width = 2,
                .height = 1,
                .depth = 0,
                .data = (void*)light_dir_data_default,
                .pixel_type = GL_FLOAT,
                .pixel_format = GL_RGBA,
                .pixel_internal_format = GL_RGBA32F,
                .generate_mipmap = false,
                .wrap_mode_s = GL_REPEAT,
                .wrap_mode_t = GL_REPEAT,
                .wrap_mode_r = GL_REPEAT,
                .min_filter = GL_NEAREST,
                .mag_filter = GL_NEAREST,
            };

            texture_load(&light_dir_tex, &light_dir_tex_data);
        }
    }

    if (light_point_tex_update) {
        size_t light_point_count = vec_light_point.end - vec_light_point.begin;

        if (light_point_count) {
            const size_t light_point_size = 12;
            float_t* light_point_data = force_malloc_s(sizeof(float_t), light_point_size * light_point_count);

            for (size_t i = 0; i < light_point_count; i++) {
                light_point* light = &vec_light_point.begin[i].data;
                light_point_data[i * light_point_size] = light->position.x;
                light_point_data[i * light_point_size + 1] = light->position.y;
                light_point_data[i * light_point_size + 2] = light->position.z;
                light_point_data[i * light_point_size + 3] = 0.0f;
                light_point_data[i * light_point_size + 4] = light->color.x;
                light_point_data[i * light_point_size + 5] = light->color.y;
                light_point_data[i * light_point_size + 6] = light->color.z;
                light_point_data[i * light_point_size + 7] = 0.0f;
                light_point_data[i * light_point_size + 8] = light->constant;
                light_point_data[i * light_point_size + 9] = light->linear;
                light_point_data[i * light_point_size + 10] = light->quadratic;
                light_point_data[i * light_point_size + 11] = light->radius;
            }

            texture_data light_point_tex_data = {
                .type = TEXTURE_2D,
                .width = 3,
                .height = (int32_t)light_point_count,
                .depth = 0,
                .data = light_point_data,
                .pixel_type = GL_FLOAT,
                .pixel_format = GL_RGBA,
                .pixel_internal_format = GL_RGBA32F,
                .generate_mipmap = false,
                .wrap_mode_s = GL_REPEAT,
                .wrap_mode_t = GL_REPEAT,
                .wrap_mode_r = GL_REPEAT,
                .min_filter = GL_NEAREST,
                .mag_filter = GL_NEAREST,
            };

            texture_load(&light_point_tex, &light_point_tex_data);
            free(light_point_data);
        }
        else {
            texture_data light_point_tex_data = {
                .type = TEXTURE_2D,
                .width = 3,
                .height = 1,
                .depth = 0,
                .data = (void*)light_point_data_default,
                .pixel_type = GL_FLOAT,
                .pixel_format = GL_RGBA,
                .pixel_internal_format = GL_RGBA32F,
                .generate_mipmap = false,
                .wrap_mode_s = GL_REPEAT,
                .wrap_mode_t = GL_REPEAT,
                .wrap_mode_r = GL_REPEAT,
                .min_filter = GL_NEAREST,
                .mag_filter = GL_NEAREST,
            };

            texture_load(&light_point_tex, &light_point_tex_data);
        }
    }

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw2d_clear(&tasks_render_draw2d_int);

    if (tasks_render_draw2d.end - tasks_render_draw2d.begin > 0) {
        vector_task_render_draw2d_append(&tasks_render_draw2d_int,
            tasks_render_draw2d.end - tasks_render_draw2d.begin);
        for (task_render_draw2d* i = tasks_render_draw2d.begin; i != tasks_render_draw2d.end; i++)
            vector_task_render_draw2d_push_back(&tasks_render_draw2d_int, i);
    }
    vector_task_render_draw2d_clear(&tasks_render_draw2d);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw3d_clear(&tasks_render_draw3d_int);

    if (tasks_render_draw3d.end - tasks_render_draw3d.begin > 0) {
        vector_task_render_draw3d_append(&tasks_render_draw3d_int,
            tasks_render_draw3d.end - tasks_render_draw3d.begin);
        for (task_render_draw3d* i = tasks_render_draw3d.begin; i != tasks_render_draw3d.end; i++)
            vector_task_render_draw3d_push_back(&tasks_render_draw3d_int, i);
    }
    vector_task_render_draw3d_clear(&tasks_render_draw3d);

    /*cam.Set(camStruct);

    if (camStruct.DOF.HasValue)
    {
        ModelTrans dofMT = camStruct.DOF.Value;

        dof.Debug.Flags = 0;
        dof.PV.DistanceToFocus = (dofMT.Trans - cam.Pos).Length;
        dof.PV.FocusRange = dofMT.Scale.X;
        dof.PV.FuzzingRange = dofMT.Rot.X;
        dof.PV.Ratio = dofMT.Rot.Y;
        dof.PV.Enable = camStruct.DOF.Value.Rot.Z > 0.000001f;

        if (dof.PV.Ratio > 0.0f && dof.PV.Enable)
        {
            double fov = cam.FOVIsHorizontal ? 2 * Math.Atan(1 / cam.Aspect * Math.Tan(cam.FOV / 2)) : cam.FOV;
            float[] data = GetDOFData(ref dof, (float)fov);

            GL.BindBuffer(BufferTarget.UniformBuffer, dofCommonUBO);
            GL.BufferSubData(BufferTarget.UniformBuffer, (IntPtr)0, data.Length * 4, data);
            GL.BindBuffer(BufferTarget.UniformBuffer, 0);
        }
    }*/

    //static bool show_demo_window;
    //igShowDemoWindow(&show_demo_window);

    if (rad->update || inten->update) {
        rad->update = false;
        inten->update = false;

        vec4 rgba;
        *(vec3*)&rgba = inten->val;
        vec3_dot(inten->val, sv_rgb_to_luma, rgba.w);
        vec4* radius = rad->val;

        vec4 v[GAUSSIAN_KERNEL_SIZE];
        for (size_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
            vec4_mult(radius[i], rgba, v[i]);

        for (size_t i = 6; i < 12; i++)
            shader_fbo_set_vec4_array(&bfbs[i], "gaussianKernel", GAUSSIAN_KERNEL_SIZE, v);
    }

    if (tmsg->update) {
        tmsg->update = false;
        fbo_pp_tone_map_set(pfbo, tmsg->val, 16 * TONE_MAP_SAT_GAMMA_SAMPLES);
    }

    if (tmd->update) {
        tmd->update = false;
        pfbo->tone_map_method = tmd->tone_map_method;
        pfbo->scene_fade = tmd->scene_fade_alpha > 0.009999999f;

        bind_uniform_buffer(tone_map_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float_t), tmd->val);
        bind_uniform_buffer(0);
    }

    bool use_dof_f2 = false;
    if (dof->debug.flags & DOF_DEBUG_USE_UI_PARAMS) {
        if (dof->debug.flags & DOF_DEBUG_ENABLE_DOF) {
            if (dof->debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF) {
                float_t dist_to_focus = dof->debug.distance_to_focus;
                if (dof->debug.flags & DOF_DEBUG_AUTO_FOCUS && false) {
                    mat4 view_transpose;
                    mat4_transpose(&cam->view, &view_transpose);
                    vec3 chara_trans = vec3_null;
                    vec3_dot(cam->view_point, chara_trans, dist_to_focus);
                    dist_to_focus = -dist_to_focus - 0.1f;
                }

                dist_to_focus = max(dist_to_focus, (float_t)cam->min_distance);
                dof_calculate_physical(dof, internal_3d_res.y,
                    cam->min_distance, cam->max_distance, cam->fov, dist_to_focus,
                    dof->debug.focal_length, dof->debug.f_number);
            }
            else {
                float_t fuzzing_range = max(dof->debug.f2.fuzzing_range, 0.01f);
                dof_calculate_f2(dof, internal_3d_res.y,
                    cam->min_distance, cam->max_distance, cam->fov, dof->debug.f2.distance_to_focus,
                    dof->debug.f2.focus_range, fuzzing_range, dof->debug.f2.ratio);
                use_dof_f2 = true;
            }
        }
    }
    else if (dof->pv.enable && dof->pv.f2.ratio > 0.0f) {
        float_t fuzzing_range = max(dof->pv.f2.fuzzing_range, 0.01f);
        dof_calculate_f2(dof, internal_3d_res.y,
            cam->min_distance, cam->max_distance, cam->fov, dof->pv.f2.distance_to_focus,
            dof->pv.f2.focus_range, fuzzing_range, dof->pv.f2.ratio);
        dof->debug.flags |= DOF_DEBUG_ENABLE_DOF;
        dof->debug.f2.distance_to_focus = dof->pv.f2.distance_to_focus;
        dof->debug.f2.focus_range = dof->pv.f2.focus_range;
        dof->debug.f2.fuzzing_range = dof->pv.f2.fuzzing_range;
        dof->debug.f2.ratio = dof->pv.f2.ratio;
        use_dof_f2 = true;
    }
    else
        dof->debug.flags &= ~DOF_DEBUG_ENABLE_DOF;

    mat4 inv_vp;
    mat4_inverse(&cam->view_projection, &inv_vp);

    shader_fbo* g_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
        &rfbo->g_shader[4] : &rfbo->g_shader[3] : &rfbo->g_shader[2] : &rfbo->g_shader[1] : &rfbo->g_shader[0];
    shader_fbo_set_vec3(g_shader, "viewPos", cam->view_point.x, cam->view_point.y, cam->view_point.z);
    shader_fbo_set_mat4(g_shader, "inv_vp", false, &inv_vp);

    uint8_t global_matrices[sizeof(mat4) * 4];
    ((mat4*)global_matrices)[0] = cam->view_projection;
    ((mat4*)global_matrices)[1] = cam->view;
    ((mat4*)global_matrices)[2] = cam->projection;
    ((vec3*)(global_matrices + sizeof(mat4) * 3))[0] = cam->view_point;
    bind_uniform_buffer(global_matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4) * 4, global_matrices);

    bind_uniform_buffer(dof_common_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(dof->data), dof->data);
    bind_uniform_buffer(0);
}

static void render_draw() {
    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat depth_clear = 1.0f;
    static const GLint stencil_clear = 0;

    bind_framebuffer(0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glDepthMask(true);
    glStencilMask(0xFF);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
    glStencilMask(0);
    glDepthMask(false);

    for (int32_t i = 0; i < 32; i++) {
        active_texture(i);
        bind_tex1d(0);
        bind_tex2d(0);
        bind_tex2dms(0);
        bind_tex3d(0);
        bind_texcube(0);
    }

    if (back_2d || g_back_3d || c_back_3d || g_front_3d || c_front_3d || glitter_3d) {
        glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);

        if (back_2d) {
            bind_framebuffer(rfbo->fbo);
            glDrawBuffers(1, fbo_render_c_attachments);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            render_2d(TASK_RENDER_DRAW2D_BACK);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
            bind_framebuffer(hfbo->fbo[0]);
            fbo_render_draw_c(rfbo, false);
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
        if (g_back_3d || c_back_3d || g_front_3d || c_front_3d || glitter_3d) {
            bind_framebuffer(rfbo->fbo);
            glDepthMask(true);
            glStencilMask(0xFF);
            glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
            glStencilMask(0);
            glDepthMask(false);

            vec4 color;
            *(vec3*)&color = back3d_color;
            color.w = 1.0f;
            glDrawBuffers(1, fbo_render_c_attachments);
            glClearBufferfv(GL_COLOR, 0, (GLfloat*)&color);

            if (c_back_3d) {
                render_c_3d(TASK_RENDER_DRAW3D_BACK);
                render_c_3d_translucent(TASK_RENDER_DRAW3D_BACK);

                glDepthMask(true);
                glStencilMask(0xFF);
                glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
                glStencilMask(0);
                glDepthMask(false);
            }

            if (g_front_3d)
                render_g_3d(TASK_RENDER_DRAW3D_G_FRONT);

            if (grid_3d)
                render_grid_3d();

            if (c_front_3d)
                render_c_3d(TASK_RENDER_DRAW3D_C_FRONT);

            if (glitter_3d)
                render_glitter_3d(0);

            if (glitter_3d)
                render_glitter_3d(2);

            if (glitter_3d)
                render_glitter_3d(1);

            if (c_front_3d)
                render_c_3d_translucent(TASK_RENDER_DRAW3D_C_FRONT);

            glDrawBuffers(1, fbo_render_c_attachments);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

            glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
            bind_framebuffer(hfbo->fbo[0]);
            glDepthMask(true);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            glClearBufferfv(GL_DEPTH, 0, &depth_clear);
            glDepthMask(false);

            if (dof->debug.flags & DOF_DEBUG_USE_UI_PARAMS ? true : false) {
                if (dof->debug.flags & DOF_DEBUG_ENABLE_DOF) {
                    fbo_render_draw_c(rfbo, true);
                    if (dof->debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF)
                        fbo_dof_draw(dfbo, hfbo->tcb[0], hfbo->tcb[1], hfbo->fbo[0], false);
                    else
                        fbo_dof_draw(dfbo, hfbo->tcb[0], hfbo->tcb[1], hfbo->fbo[0], true);
                }
                else
                    fbo_render_draw_c(rfbo, false);
            }
            else if (dof->pv.enable && dof->pv.f2.ratio > 0.0f) {
                fbo_render_draw_c(rfbo, true);
                fbo_dof_draw(dfbo, hfbo->tcb[0], hfbo->tcb[1], hfbo->fbo[0], true);
            }
            else
                fbo_render_draw_c(rfbo, false);
        }

        if (enable_post_process)
            fbo_pp_draw(pfbo, hfbo->tcb[0], hfbo->fbo[0], 1, fbo_hdr_f_attachments);
        fbo_hdr_draw_aa(hfbo);

        bind_framebuffer(0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glViewport(0, 0, width, height);
        fbo_hdr_draw(hfbo);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
    if (front_2d || ui) {
        glViewport((width - internal_2d_res.x) / 2, (height - internal_2d_res.y) / 2,
            internal_2d_res.x, internal_2d_res.y);
        if (front_2d)
            render_2d(TASK_RENDER_DRAW2D_FRONT);
        if (ui)
            render_2d(TASK_RENDER_DRAW2D_UI);
        glViewport(0, 0, width, height);
    }

    if (imgui)
        render_imgui();
}

static void render_dispose() {
    for (task_render* i = tasks_render.begin; i != tasks_render.end; i++) {
        if (i->type == TASK_RENDER_UPDATE) {
            task_render_update update = i->update;
            switch (update.type) {
            case TASK_RENDER_UPDATE_SHADER:
                free(update.shad.frag_c);
                free(update.shad.frag_g);
                free(update.shad.vert);
                free(update.shad.geom);
                break;
            }
        }
        else if (i->type == TASK_RENDER_UNIFORM) {
            task_render_uniform uniform = i->uniform;
            char_buffer_dispose(&uniform.name);
        }
    }

    vector_task_render_free(&tasks_render);

    for (task_render_draw2d* i = tasks_render_draw2d.begin; i != tasks_render_draw2d.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw2d_free(&tasks_render_draw2d);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw2d_free(&tasks_render_draw2d_int);

    for (task_render_draw3d* i = tasks_render_draw3d.begin; i != tasks_render_draw3d.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw3d_free(&tasks_render_draw3d);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            char_buffer_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw3d_free(&tasks_render_draw3d_int);

    vector_task_render_draw3d_free(&temp_draw3d_tasks);
    vector_gl_object_free(&temp_draw3d_objects);

    for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
        gl_object_dispose(j->data);
    vector_hash_ptr_gl_object_free(&vec_gl_obj);

    vector_hash_light_dir_free(&vec_light_dir);

    vector_hash_light_point_free(&vec_light_point);

    vector_hash_material_free(&vec_mat);

    for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
        shader_model_free(&j->data);
    vector_hash_shader_model_free(&vec_shad);

    for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
        texture_set_free(&j->data);
    vector_hash_texture_set_free(&vec_tex_set);

    for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
        texture_free(&j->data);
    vector_hash_texture_free(&vec_tex);

    for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
        vertex_dispose(j->data);
    vector_hash_ptr_vertex_free(&vec_vert);

    classes_process_dispose(classes, classes_count);

    camera_dispose(cam);
    dof_dispose(dof);
    radius_dispose(rad);
    intensity_dispose(inten);
    tone_map_sat_gamma_dispose(tmsg);
    tone_map_data_dispose(tmd);
    glitter_particle_manager_dispose(gpm);

    texture_free(&light_dir_tex);
    texture_free(&light_point_tex);

    fbo_render_dispose(rfbo);
    fbo_hdr_dispose(hfbo);
    fbo_dof_dispose(dfbo);
    fbo_pp_dispose(pfbo);

    for (size_t i = 0; i < 5; i++) {
        shader_fbo_free(&cfbs[i * 2]);
        shader_fbo_free(&cfbs[i * 2 + 1]);
        shader_fbo_free(&gfbs[i]);
    }

    shader_fbo_free(&ffbs);
    shader_fbo_free(&hfbs[0]);
    shader_fbo_free(&hfbs[1]);

    for (size_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 5; j++)
            shader_fbo_free(&dfbs[i * 5 + j]);

    for (size_t i = 0; i < 16; i++)
        shader_fbo_free(&bfbs[i]);

    shader_fbo_free(&tfbs);
    shader_fbo_free(&particle_shader);
    shader_fbo_free(&sprite_shader);
    shader_fbo_free(&grid_shader);

    vertex_dispose(default_vertex);
    texture_bone_mat_free(&default_bone_mat);
    texture_set_free(&default_texture);
    memset(&default_material, 0, sizeof(material));
    shader_model_free(&default_shader);

    glDeleteBuffers(1, &global_matrices_ubo);
    glDeleteBuffers(1, &dof_common_ubo);
    glDeleteBuffers(1, &common_ubo);
    glDeleteBuffers(1, &tone_map_ubo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteVertexArrays(1, &grid_vao);
    glDeleteBuffers(1, &fb_vbo);
    glDeleteVertexArrays(1, &fb_vao);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ig);
}

static void render_imgui() {
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

static void render_2d(task_render_draw2d_type type) {
    size_t count = 0;
    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++)
        if (i->type == type)
            count++;

    if (!count)
        return;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        if (i->type != type)
            continue;

    }
    glDisable(GL_BLEND);
}

static void render_c_3d(task_render_draw3d_type type) {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == type && !i->translucent)
            count++;

    if (!count)
        return;

    glDrawBuffers(1, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != type || i->translucent)
            continue;

        hash h = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (HASH_COMPARE(j->hash, h)) {
                shader_model* shad = &j->data->shader;
                for (task_render_uniform* k = i->uniforms.begin; k != i->uniforms.end; k++)
                    render_update_uniform_c(k, shad);

                shader_model_c_set_mat4(shad, "model", GL_FALSE, &i->model);
                shader_model_c_set_mat3(shad, "model_normal", GL_FALSE, &i->model_normal);
                shader_model_c_set_mat4(shad, "uv_mat", GL_FALSE, &i->uv_mat);
                shader_model_c_set_vec4(shad, "color", i->color.x, i->color.y, i->color.z, i->color.w);
                gl_object_draw_c(j->data);
                break;
            }
    }
    bind_vertex_array(0);
    shader_model_c_use(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
}

static void render_c_3d_translucent(task_render_draw3d_type type) {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == type && i->translucent)
            count++;

    if (!count)
        return;

    vector_task_render_draw3d_append(&temp_draw3d_tasks, count);
    vector_gl_object_append(&temp_draw3d_objects, count);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != type || !i->translucent)
            continue;

        hash h = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (HASH_COMPARE(j->hash, h)) {
                vector_task_render_draw3d_push_back(&temp_draw3d_tasks, i);
                vector_gl_object_push_back(&temp_draw3d_objects, j->data);
                break;
            }
    }

    ssize_t draw_count = temp_draw3d_tasks.end - temp_draw3d_tasks.begin;
    if (draw_count < 1) {
        vector_task_render_draw3d_clear(&temp_draw3d_tasks);
        vector_gl_object_clear(&temp_draw3d_objects);
        return;
    }

    glDrawBuffers(1, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    for (ssize_t i = 0; i < draw_count; i++) {
        task_render_draw3d* task = &temp_draw3d_tasks.begin[i];
        gl_object* obj = &temp_draw3d_objects.begin[i];
        shader_model* shad = &obj->shader;
        for (task_render_uniform* k = task->uniforms.begin; k != task->uniforms.end; k++)
            render_update_uniform_c(k, shad);

        shader_model_c_set_mat4(shad, "model", GL_FALSE, &task->model);
        shader_model_c_set_mat3(shad, "model_normal", GL_FALSE, &task->model_normal);
        shader_model_c_set_mat4(shad, "uv_mat", GL_FALSE, &task->uv_mat);
        shader_model_c_set_vec4(shad, "color", task->color.x, task->color.y, task->color.z, task->color.w);
        gl_object_draw_c_translucent_first_part(obj);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnablei(GL_BLEND, 0);

    for (ssize_t i = 0; i < draw_count; i++) {
        task_render_draw3d* task = &temp_draw3d_tasks.begin[i];
        gl_object* obj = &temp_draw3d_objects.begin[i];
        shader_model* shad = &obj->shader;
        for (task_render_uniform* k = task->uniforms.begin; k != task->uniforms.end; k++)
            render_update_uniform_c(k, shad);

        shader_model_c_set_mat4(shad, "model", GL_FALSE, &task->model);
        shader_model_c_set_mat3(shad, "model_normal", GL_FALSE, &task->model_normal);
        shader_model_c_set_mat4(shad, "uv_mat", GL_FALSE, &task->uv_mat);
        shader_model_c_set_vec4(shad, "color", task->color.x, task->color.y, task->color.z, task->color.w);
        gl_object_draw_c_translucent_second_part(obj);
    }
    bind_vertex_array(0);
    shader_model_c_use(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    vector_task_render_draw3d_clear(&temp_draw3d_tasks);
    vector_gl_object_clear(&temp_draw3d_objects);
}

static void render_grid_3d() {
    glDrawBuffers(1, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    glEnablei(GL_BLEND, 0);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(true);

    shader_fbo_use(&grid_shader);
    bind_vertex_array(grid_vao);
    glDrawArrays(GL_LINES, 0, grid_vertex_count);
    shader_fbo_use(0);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_BLEND);
}

static void render_g_3d(task_render_draw3d_type type) {
    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat specular_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat normal_clear[] = { 0.5f, 0.5f, 0.5f, 0.0f };

    glDrawBuffers(3, fbo_render_g_attachments);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfv(GL_COLOR, 1, specular_clear);
    glClearBufferfv(GL_COLOR, 2, normal_clear);

    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == type)
            count++;

    if (!count)
        return;

    glDrawBuffers(3, fbo_render_g_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != type)
            continue;

        hash h = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (HASH_COMPARE(j->hash, h)) {
                shader_model* shad = &j->data->shader;
                for (task_render_uniform* k = i->uniforms.begin; k != i->uniforms.end; k++)
                    render_update_uniform_g(k, shad);

                shader_model_g_set_mat4(shad, "model", GL_FALSE, &i->model);
                shader_model_g_set_mat3(shad, "model_normal", GL_FALSE, &i->model_normal);
                shader_model_g_set_mat4(shad, "uv_mat", GL_FALSE, &i->uv_mat);
                shader_model_g_set_vec4(shad, "color", i->color.x, i->color.y, i->color.z, i->color.w);
                gl_object_draw_g(j->data);
                break;
            }
    }
    bind_vertex_array(0);
    shader_model_g_use(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
    glStencilMask(0x00);
    fbo_render_draw_g(rfbo,
        light_dir_tex.id, (int32_t)(vec_light_dir.end - vec_light_dir.begin),
        light_point_tex.id, (int32_t)(vec_light_point.end - vec_light_point.begin));
    glDisable(GL_STENCIL_TEST);
}

void render_glitter_3d(int32_t alpha) {
    glDrawBuffers(1, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);
    glitter_particle_manager_draw(GPM_VAL, alpha);
}

void render_set_scale(double_t value) {
    size_t i;
    const size_t c = sizeof(const_scale) / sizeof(double_t);
    for (i = 0; i < c; i++)
        if (value <= const_scale[i]) break;

    if (i < c)
        value = const_scale[i];
    else
        value = const_scale[c - 1];

    scale = (float_t)value;
    if (old_scale != scale)
        render_resize_fb(true);
    old_scale = scale;
    render_resize_fb(true);
}

static void render_get_aspect_correct_res(vec2i* res) {
    double_t width = (double_t)res->x;
    double_t height = (double_t)res->y;
    double_t view_aspect = width / height;
    width = round(height * aspect);
    height = round(width / aspect);
    if (view_aspect < aspect)
        width = round(height * aspect);
    else if (view_aspect > aspect)
        height = round(width / aspect);
    res->x = (int32_t)width;
    res->y = (int32_t)height;
}

static void render_drop_glfw(GLFWwindow* window, int count, char** paths) {
    if (!count || !paths)
        return;

    size_t c = count;
    wchar_t** wpaths = force_malloc_s(wchar_t*, count);
    for (size_t i = 0, j = 0; i < count; i++)
        if (paths[i])
            wpaths[j++] = char_string_to_wchar_t_string(paths[i]);
        else
            c--;
    if (c) {
        classes_process_drop(classes, classes_count, c, wpaths);
        glfwFocusWindow(window);
    }
    for (size_t i = 0; i < c; i++)
        free(wpaths[i]);
    free(wpaths);
}

static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h) {
    bool change_fb = width != w || height != h;
    width = w;
    height = h;
    render_resize_fb(change_fb);
}

static void render_resize_fb(bool change_fb) {
    if (internal_3d_res.x < 20) internal_3d_res.x = 20;
    if (internal_3d_res.y < 20) internal_3d_res.y = 20;

    internal_2d_res.x = (int32_t)roundf((float_t)width * scale);
    internal_2d_res.y = (int32_t)roundf((float_t)height * scale);
    render_get_aspect_correct_res(&internal_2d_res);
    internal_2d_res.x = clamp(internal_2d_res.x, 1, sv_max_texture_size);
    internal_2d_res.y = clamp(internal_2d_res.y, 1, sv_max_texture_size);

    internal_3d_res.x = (int32_t)roundf((float_t)internal_res.x * scale);
    internal_3d_res.y = (int32_t)roundf((float_t)internal_res.y * scale);
    render_get_aspect_correct_res(&internal_3d_res);
    internal_3d_res.x = clamp(internal_3d_res.x, 1, sv_max_texture_size);
    internal_3d_res.y = clamp(internal_3d_res.y, 1, sv_max_texture_size);

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x || old_internal_3d_res.y != internal_3d_res.y;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    if (state == RENDER_INITIALIZED) {
        if (sv_fxaa_changed) {
            sv_fxaa_changed = false;
            hfbo->fxaa = sv_fxaa;
            fb_changed = true;
        }

        if (sv_samples_changed) {
            sv_samples_changed = false;
            rfbo->samples = sv_samples;
            fb_changed = true;
        }

        if (fb_changed && change_fb) {
            fbo_render_resize(rfbo, &internal_3d_res);
            fbo_hdr_resize(hfbo, &internal_3d_res, &internal_2d_res);
            fbo_dof_resize(dfbo, &internal_3d_res);
            fbo_pp_resize(pfbo, &internal_3d_res);
        }
    }

    if (state == RENDER_INITIALIZING || state == RENDER_INITIALIZED) {
        float_t data[4] = {
            (float_t)internal_3d_res.x,
            (float_t)internal_3d_res.y,
            1.0f / (float_t)internal_3d_res.x,
            1.0f / (float_t)internal_3d_res.y,
        };
        bind_uniform_buffer(common_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float_t) * 4, data);
        bind_uniform_buffer(0);

        float_t aspect;
        aspect = (float_t)width / (float_t)height;
        aspect /= (float_t)internal_3d_res.x / (float_t)internal_3d_res.y;

        if (aspect > 1.0f)
            shader_fbo_set_vec2(&ffbs, "scale", 1.0f / aspect, 1.0f);
        else if (aspect < 1.0f)
            shader_fbo_set_vec2(&ffbs, "scale", 1.0f, aspect);
        else
            shader_fbo_set_vec2(&ffbs, "scale", 1.0f, 1.0f);
    }
}

static void render_dof_get_texcoords(vec2* data, float_t a2) {
    size_t i;
    size_t j;
    float_t v6;
    float_t v7;
    float_t v8;
    float_t v9;
    double_t v11;
    float_t v12;

    const float_t t = (float_t)(1.0 / 3.0);
    v12 = a2 * 3.0f;
    for (i = 0; i < 7; i++) {
        v6 = (float_t)i * t - 1.0f;
        for (j = 0; j < 7; j++) {
            v7 = (float_t)j * t - 1.0f;
            if (-v6 >= v7) {
                if (v7 < v6) {
                    v8 = -v7;
                    v9 = (v6 / v7) + 4.0f;
                }
                else if (v6 == 0.0f) {
                    v8 = 0.0f;
                    v9 = 0.0f;
                }
                else {
                    v8 = -v6;
                    v9 = 6.0f - (v7 / v6);
                }
            }
            else if (v6 < v7) {
                v8 = (float_t)j * t - 1.0f;
                v9 = v6 / v7;
            }
            else {
                v8 = (float_t)i * t - 1.0f;
                v9 = 2.0f - (v7 / v6);
            }
            v8 *= v12;
            v11 = (double_t)v9 * (M_PI * 0.25);
            vec2_mult_scalar(((vec2) { (float_t)cos(v11), (float_t)sin(v11) }), v8, *data++);
        }
    }
}

static void render_imgui_context_menu(classes_struct* classes, const size_t classes_count) {
    for (size_t i = 0; i < classes_count; i++) {
        if (!classes[i].enabled || !classes[i].name || ~classes[i].flags & CLASSES_IN_CONTEXT_MENU)
            continue;

        if (classes[i].sub_classes && classes[i].sub_classes_count) {
            if (igBeginMenu(classes[i].name, *classes[i].enabled)) {
                render_imgui_context_menu(classes[i].sub_classes, classes[i].sub_classes_count);
                igEndMenu();
            }
        }
        else if (*classes[i].enabled)
            igMenuItem_Bool(classes[i].name, 0, false, false);
        else if (igMenuItem_Bool(classes[i].name, 0, false, true))
            if (classes[i].init)
                classes[i].init();
    }
}

static bool render_glitter_mesh_add_list(glitter_particle_mesh* mesh, vec4* color, mat4* model, mat4* uv_mat) {
    bool found = false;
    hash h;
    gl_object* obj;

    hash object_mesh_hash = (hash){ .f = mesh->object_name_hash, .m = (uint32_t)mesh->object_name_hash };
    for (hash_ptr_gl_object* i = vec_gl_obj.begin; i != vec_gl_obj.end; i++)
        if (HASH_COMPARE(i->hash, object_mesh_hash)) {
            found = true;
            h = i->hash;
            obj = i->data;
            break;
        }

    if (!found) {
        if (!gpm->draw_all || !gpm->draw_all_mesh)
            return false;

        found = false;
        hash dummy_hash = hash_char("Glitter Editor Dummy");
        for (hash_ptr_gl_object* i = vec_gl_obj.begin; i != vec_gl_obj.end; i++)
            if (HASH_COMPARE(i->hash, dummy_hash)) {
                found = true;
                h = i->hash;
                obj = i->data;
                break;
            }

        if (!found)
            return false;

        found = false;
    }

    mat4 m;
    mat4 temp;
    mat3 mn;
    if (found)
        m = *model;
    else
        mat4_scale_rot(model, 0.05f, 0.05f, 0.05f, &m);
    mat4_invtrans(&m, &temp);
    mat3_from_mat4(&temp, &mn);

    task_render_draw3d task_draw;
    memset(&task_draw, 0, sizeof(task_draw));
    task_draw.hash = h;
    task_draw.type = TASK_RENDER_DRAW3D_C_FRONT;
    task_draw.translucent = color->w < 1.0f || obj->vertex.translucent || obj->material.translucent;
    task_draw.model = m;
    task_draw.model_normal = mn;
    task_draw.uv_mat = *uv_mat;
    task_draw.color = *color;
    task_draw.uniforms = (vector_task_render_uniform){ 0, 0, 0 };
    vector_task_render_draw3d_push_back(&tasks_render_draw3d, &task_draw);
    return true;
}

static void render_update_uniform_c(task_render_uniform* uniform, shader_model* s) {
    task_render_uniform uni = *uniform;
    char* name = char_buffer_select(&uni.name);
    switch (uni.type) {
    case TASK_RENDER_UNIFORM_BOOL: {
        task_render_uniform_bool uni_data = uni.boolean;
        shader_model_c_set_bool(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32: {
        task_render_uniform_int32 uni_data = uni.int32;
        shader_model_c_set_int(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32: {
        task_render_uniform_float32 uni_data = uni.float32;
        shader_model_c_set_float(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2: {
        task_render_uniform_vec2 uni_data = uni.vec2;
        shader_model_c_set_vec2(s, name, uni_data.value.x, uni_data.value.y);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I: {
        task_render_uniform_vec2i uni_data = uni.vec2i;
        shader_model_c_set_vec2i(s, name, uni_data.value.x, uni_data.value.y);
    } break;
    case TASK_RENDER_UNIFORM_VEC3: {
        task_render_uniform_vec3 uni_data = uni.vec3;
        shader_model_c_set_vec3(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I: {
        task_render_uniform_vec3i uni_data = uni.vec3i;
        shader_model_c_set_vec3i(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
    } break;
    case TASK_RENDER_UNIFORM_VEC4: {
        task_render_uniform_vec4 uni_data = uni.vec4;
        shader_model_c_set_vec4(s, name, uni_data.value.x,
            uni_data.value.y, uni_data.value.z, uni_data.value.w);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I: {
        task_render_uniform_vec4i uni_data = uni.vec4i;
        shader_model_c_set_vec4i(s, name, uni_data.value.x,
            uni_data.value.y, uni_data.value.z, uni_data.value.w);
    } break;
    case TASK_RENDER_UNIFORM_MAT3: {
        task_render_uniform_mat3 uni_data = uni.mat3;
        shader_model_c_set_mat3(s, name, uni_data.transpose, &uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4: {
        task_render_uniform_mat4 uni_data = uni.mat4;
        shader_model_c_set_mat4(s, name, uni_data.transpose, &uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32_ARRAY: {
        task_render_uniform_int32_array uni_data = uni.int32_array;
        shader_model_c_set_int_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32_ARRAY: {
        task_render_uniform_float32_array uni_data = uni.float32_array;
        shader_model_c_set_float_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2_ARRAY: {
        task_render_uniform_vec2_array uni_data = uni.vec2_array;
        shader_model_c_set_vec2_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I_ARRAY: {
        task_render_uniform_vec2i_array uni_data = uni.vec2i_array;
        shader_model_c_set_vec2i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3_ARRAY: {
        task_render_uniform_vec3_array uni_data = uni.vec3_array;
        shader_model_c_set_vec3_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I_ARRAY: {
        task_render_uniform_vec3i_array uni_data = uni.vec3i_array;
        shader_model_c_set_vec3i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4_ARRAY: {
        task_render_uniform_vec4_array uni_data = uni.vec4_array;
        shader_model_c_set_vec4_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I_ARRAY: {
        task_render_uniform_vec4i_array uni_data = uni.vec4i_array;
        shader_model_c_set_vec4i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT3_ARRAY: {
        task_render_uniform_mat3_array uni_data = uni.mat3_array;
        shader_model_c_set_mat3_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4_ARRAY: {
        task_render_uniform_mat4_array uni_data = uni.mat4_array;
        shader_model_c_set_mat4_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    }
}

static void render_update_uniform_g(task_render_uniform* uniform, shader_model* s) {
    task_render_uniform uni = *uniform;
    char* name = char_buffer_select(&uni.name);
    switch (uni.type) {
    case TASK_RENDER_UNIFORM_BOOL: {
        task_render_uniform_bool uni_data = uni.boolean;
        shader_model_g_set_bool(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32: {
        task_render_uniform_int32 uni_data = uni.int32;
        shader_model_g_set_int(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32: {
        task_render_uniform_float32 uni_data = uni.float32;
        shader_model_g_set_float(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2: {
        task_render_uniform_vec2 uni_data = uni.vec2;
        shader_model_g_set_vec2(s, name, uni_data.value.x, uni_data.value.y);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I: {
        task_render_uniform_vec2i uni_data = uni.vec2i;
        shader_model_g_set_vec2i(s, name, uni_data.value.x, uni_data.value.y);
    } break;
    case TASK_RENDER_UNIFORM_VEC3: {
        task_render_uniform_vec3 uni_data = uni.vec3;
        shader_model_g_set_vec3(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I: {
        task_render_uniform_vec3i uni_data = uni.vec3i;
        shader_model_g_set_vec3i(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
    } break;
    case TASK_RENDER_UNIFORM_VEC4: {
        task_render_uniform_vec4 uni_data = uni.vec4;
        shader_model_g_set_vec4(s, name, uni_data.value.x,
            uni_data.value.y, uni_data.value.z, uni_data.value.w);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I: {
        task_render_uniform_vec4i uni_data = uni.vec4i;
        shader_model_g_set_vec4i(s, name, uni_data.value.x,
            uni_data.value.y, uni_data.value.z, uni_data.value.w);
    } break;
    case TASK_RENDER_UNIFORM_MAT3: {
        task_render_uniform_mat3 uni_data = uni.mat3;
        shader_model_g_set_mat3(s, name, uni_data.transpose, &uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4: {
        task_render_uniform_mat4 uni_data = uni.mat4;
        shader_model_g_set_mat4(s, name, uni_data.transpose, &uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32_ARRAY: {
        task_render_uniform_int32_array uni_data = uni.int32_array;
        shader_model_g_set_int_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32_ARRAY: {
        task_render_uniform_float32_array uni_data = uni.float32_array;
        shader_model_g_set_float_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2_ARRAY: {
        task_render_uniform_vec2_array uni_data = uni.vec2_array;
        shader_model_g_set_vec2_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I_ARRAY: {
        task_render_uniform_vec2i_array uni_data = uni.vec2i_array;
        shader_model_g_set_vec2i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3_ARRAY: {
        task_render_uniform_vec3_array uni_data = uni.vec3_array;
        shader_model_g_set_vec3_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I_ARRAY: {
        task_render_uniform_vec3i_array uni_data = uni.vec3i_array;
        shader_model_g_set_vec3i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4_ARRAY: {
        task_render_uniform_vec4_array uni_data = uni.vec4_array;
        shader_model_g_set_vec4_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I_ARRAY: {
        task_render_uniform_vec4i_array uni_data = uni.vec4i_array;
        shader_model_g_set_vec4i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT3_ARRAY: {
        task_render_uniform_mat3_array uni_data = uni.mat3_array;
        shader_model_g_set_mat3_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4_ARRAY: {
        task_render_uniform_mat4_array uni_data = uni.mat4_array;
        shader_model_g_set_mat4_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    }
}
