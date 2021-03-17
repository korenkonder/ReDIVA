/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.h"
#include "../CRE/camera.h"
#include "../CRE/microui_atlas.inl"
#include "../CRE/post_process.h"
#include "../CRE/random.h"
#include "../CRE/static_var.h"
#include "../KKdLib/farc.h"
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "../CRE/fbo_dof.h"
#include "../CRE/fbo_hdr.h"
#include "../CRE/fbo_pp.h"
#include "../CRE/fbo_render.h"
#include "../CRE/Glitter/particle_manager.h"

vector_task_render tasks_render = { 0, 0, 0 };
vector_task_render_draw2d tasks_render_draw2d = { 0, 0, 0 };
vector_task_render_draw3d tasks_render_draw3d = { 0, 0, 0 };
static vector_task_render_draw2d tasks_render_draw2d_int = { 0, 0, 0 };
static vector_task_render_draw3d tasks_render_draw3d_int = { 0, 0, 0 };

vector_func(hash_ptr_gl_object)
vector_func(hash_shader_model)
vector_func(hash_texture)
vector_func(hash_texture_set)
vector_func(hash_ptr_vertex)

static vector_hash_ptr_gl_object vec_gl_obj = { 0, 0, 0 };
static vector_hash_shader_model vec_shad = { 0, 0, 0 };
static vector_hash_texture vec_tex = { 0, 0, 0 };
static vector_hash_texture_set vec_tex_set = { 0, 0, 0 };
static vector_hash_ptr_vertex vec_vert = { 0, 0, 0 };

static texture dir_lights;
static int32_t dir_lights_count;
static texture point_lights;
static int32_t point_lights_count;

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
shader_fbo mu_shader;

static int32_t fb_vao, fb_vbo;
static int32_t common_ubo = 0;
static int32_t dof_common_ubo = 0;
static int32_t dof_texcoords_ubo = 0;
static int32_t tone_map_ubo = 0;
static int32_t global_matrices_ubo = 0;

bool   micro_ui = true;
bool         ui = true;
bool   front_2d = true;
bool g_front_3d = true;
bool c_front_3d = true;
bool    back_2d = true;
bool    back_3d = true;

bool enable_dof          = false;
bool enable_dof_f2       = false;
bool enable_post_process = true;

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

render_state state;

static const double_t aspect = 16.0 / 9.0;
camera* cam;
radius* rad;
intensity* inten;
tone_map_sat_gamma* tmsg;
tone_map_data* tmd;
glitter_particle_manager* gpm;

texture mu_font;
HANDLE mu_lock = 0;
HANDLE mu_input_lock = 0;
vector_wchar_t mu_input;
vector_char mu_input_ansi;

#define MU_BUFFER_SIZE 16384
static GLfloat* mu_vert_buf;
static GLfloat* mu_uv_buf;
static GLfloat* mu_color_buf;
static GLfloat* mu_depth_buf;
static GLuint* mu_index_buf;
static size_t mu_buf_idx;
static int32_t mu_vao, mu_vert_vbo, mu_uv_vbo, mu_color_vbo, mu_depth_vbo;

/*
Camera.Struct camStruct = new Camera.Struct();
Camera.DOF dof = new Camera.DOF();*/

static mat4* dir_light_data;
static mat4* point_light_trans;
static mat4* point_light_data;

static void render_load();
static void render_update();
static void render_draw();
static void render_dispose();

static void render_micro_ui();
static void render_ui();
static void render_front_2d();
static void render_c_front_3d();
static void render_g_front_3d();
static void render_back_3d();
static void render_back_2d();

static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(bool change_fb);
static void render_input_text_glfw(GLFWwindow* window, uint32_t codepoint);
static void render_input_mouse_scroll_glfw(GLFWwindow* window, double_t xoffset, double_t yoffset);

static void render_dof_get_texcoords(float_t* a1, float_t a2);

static void render_mui_flush();
static void render_mui_push_quad(mu_Rect dst, mu_Rect src, mu_Color color);
static inline void render_mui_draw_rect(mu_Rect rect, mu_Color color);
static inline void render_mui_draw_text(const char* text, mu_Vec2 pos, mu_Color color);
static inline void render_mui_draw_icon(int id, mu_Rect rect, mu_Color color);
static inline void render_mui_set_clip_rect(mu_Rect rect);
static inline void render_mui_draw_texture();
static int render_mui_get_text_width(mu_Font font, const char* text, size_t len);
static int render_mui_get_text_height(mu_Font font);

extern bool close;
HANDLE render_lock = 0;
HWND window_handle;
GLFWwindow* window;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(render);

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(render, "Render");

    render_lock = CreateMutexW(0, 0, L"Render");
    if (!render_lock)
        goto End;

    mu_lock = CreateMutexW(0, 0, L"microui");
    if (!mu_lock)
        goto End;

    mu_input_lock = CreateMutexW(0, 0, L"microui Input");
    if (!mu_input_lock)
        goto End;

    vector_wchar_t_append(&mu_input, 1);
    mu_input.end++;
    mu_input.end[-1] = 0;
    vector_char_append(&mu_input_ansi, 1);
    mu_input_ansi.end++;
    mu_input_ansi.end[-1] = 0;

    render_init_struct* ris = (render_init_struct*)arg;
    window_handle = 0;
    state = RENDER_UNINITIALIZED;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

    window = glfwCreateWindow(width, height, "CLOUD",
        mode->width == width && mode->height == height ? monitor : 0, 0);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    window_handle = glfwGetWin32Window(window);
    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);
    glfwSetWindowSizeCallback(window, (void*)render_resize_fb_glfw);
    glfwSetCharCallback(window, (void*)render_input_text_glfw);
    glfwSetScrollCallback(window, (void*)render_input_mouse_scroll_glfw);
    glfwSetWindowSizeLimits(window, 640, 360, GLFW_DONT_CARE, GLFW_DONT_CARE);

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

    state = RENDER_INITIALIZING;
    WaitForSingleObject(render_lock, INFINITE);
    WaitForSingleObject(mu_lock, INFINITE);
    render_load();
    ReleaseMutex(mu_lock);
    ReleaseMutex(render_lock);
    state = RENDER_INITIALIZED;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    while (!close) {
        timer_calc_pre(render);
        glfwPollEvents();
        WaitForSingleObject(render_lock, INFINITE);
        render_update();
        ReleaseMutex(render_lock);
        WaitForSingleObject(mu_lock, INFINITE);
        render_draw();
        ReleaseMutex(mu_lock);
        glfwSwapBuffers(window);
        close |= glfwWindowShouldClose(window);
        double_t cycle_time = timer_calc_post(render);
        msleep(1000.0 / FREQ - cycle_time);
    }
    CloseHandle(mu_lock);
    CloseHandle(mu_input_lock);
    mu_lock = 0;
    mu_input_lock = 0;

    WaitForSingleObject(render_lock, INFINITE);
    render_dispose();
    ReleaseMutex(render_lock);

    glfwDestroyWindow(window);
    glfwTerminate();
    CloseHandle(render_lock);
    render_lock = 0;

End:
    timer_dispose(render);
    return 0;
}

static void render_load() {
    cam = camera_init();
    rad = radius_init();
    inten = intensity_init();
    tmsg = tone_map_sat_gamma_init();
    tmd = tone_map_data_init();
    gpm = glitter_particle_manager_init();
    muctx = force_malloc(sizeof(mu_Context));

    radius_initialize(rad, (vec3[]) { 2.0f, 2.0f, 2.0f });
    intensity_initialize(inten, (vec3[]) { 1.0f, 1.0f, 1.0f });
    tone_map_sat_gamma_initialize(tmsg, 1.0f, 1, 1.0f);
    tone_map_data_initialize(tmd, 2.0f, true, (vec3[]) { 0.0f, 0.0f, 0.0f}, 0.0f, 0,
        (vec3[]) { 0.0f, 0.0f, 0.0f }, (vec3[]) { 1.0f, 1.0f, 1.0f }, 0);

    mu_init(muctx);
    muctx->text_width = render_mui_get_text_width;
    muctx->text_height = render_mui_get_text_height;

    camera_initialize(cam, aspect, 70.0);
    camera_reset(cam);
    //camera_move_vec3(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
    //camera_rotate_vec2(cam, &(vec2){ -45.0f, -32.5f });
    //camera_move_vec3(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
    //camera_rotate_vec2(cam, &(vec2){ 136.5f, -20.5f });
    camera_move_vec3(cam, &(vec3){ 0.0f, 1.0f, 3.45f });

    glGenBuffers(1, &global_matrices_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, global_matrices_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * 4, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, global_matrices_ubo, 0, sizeof(mat4) * 4);

    glGenBuffers(1, &dof_common_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, dof_common_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 16 * 4, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_common_ubo, 0, 16 * 4);

    glGenBuffers(1, &common_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, common_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 4 * 4, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_ubo, 0, 4 * 4);

    glGenBuffers(1, &tone_map_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, tone_map_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 16 * 4, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, tone_map_ubo, 0, 16 * 4);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &dof_texcoords_ubo);

#pragma region LoadShaders
    char temp[0x80];
    memset(temp, 0, 0x80);

    hfbo = fbo_hdr_init();
    rfbo = fbo_render_init();
    dfbo = fbo_dof_init();
    pfbo = fbo_pp_init();

    float_t verts_quad[] = {
        -1, -1, -1, 1, 1, -1, 1, 1
    };

    glGenBuffers(1, &fb_vbo);
    glGenVertexArrays(1, &fb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fb_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, GL_STATIC_DRAW);

    glBindVertexArray(fb_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 8, 0); // Pos
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    farc* f = farc_init();
    farc_read(f, "rom\\core_shaders.farc", true, false);

    shader_param param;

    for (int32_t i = 0; i < 5; i++) {
        memset(&param, 0, sizeof(shader_param));
        param.name = L"Color FB";
        param.vert = L"rfb";
        param.frag = L"cfb";
        sprintf_s(temp, 0x80, "SAMPLES%d", 1 << i);
        param.param[0] = temp;
        shader_fbo_load(&cfbs[i], f, &param);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"Color FB";
        param.vert = L"rfb";
        param.frag = L"cfb";
        sprintf_s(temp, 0x80, "SAMPLES%d", 1 << i);
        param.param[0] = temp;
        param.param[1] = "DEPTH";
        shader_fbo_load(&cfbs[i + 5], f, &param);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"G-Buffer FB";
        param.vert = L"rfb";
        param.frag = L"gfb";
        sprintf_s(temp, 0x80, "SAMPLES%d", 1 << i);
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

    for (int32_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 5; j++) {
            memset(&param, 0, sizeof(shader_param));
            param.name = L"DOF FB";
            param.vert = L"rfb";
            param.frag = dof_shaders[j];
            sprintf_s(temp, 0x80, "USE_F2_COC (%d)", i);
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

    for (int32_t i = 0; i < 16; i++) {
        memset(&param, 0, sizeof(shader_param));
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
    param.name = L"microui";
    param.vert = L"microui";
    param.frag = L"microui";
    shader_fbo_load(&mu_shader, f, &param);

    for (int32_t i = 0; i < 10; i++) {
        shader_fbo_set_int(&cfbs[i], "ColorTexture", 0);
        if (i >= 5)
            shader_fbo_set_int(&cfbs[i], "DepthTexture", 1);
    }

    for (int32_t i = 0; i < 5; i++) {
        shader_fbo_set_int(&gfbs[i], "gPos", 0);
        shader_fbo_set_int(&gfbs[i], "gNormal", 1);
        shader_fbo_set_int(&gfbs[i], "gAlbedo", 2);
        shader_fbo_set_int(&gfbs[i], "gSpec", 3);
        shader_fbo_set_int(&gfbs[i], "dirLights", 4);
        shader_fbo_set_int(&gfbs[i], "pointLights", 5);
    }

    shader_fbo_set_int(&ffbs, "Texture", 0);

    for (int32_t i = 0; i < 2; i++)
        shader_fbo_set_int(&hfbs[i], "Color", 0);

    for (int32_t i = 0; i < 10; i += 5) {
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
        shader_fbo_set_uniform_block_binding(&dfbs[i + 1], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 2], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "DOFCommon", 1);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "DOFTexcoords", 2);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 4], "DOFCommon", 1);
    }

    for (int32_t i = 0; i < 2; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);

    for (int32_t i = 2; i < 5; i++) {
        shader_fbo_set_int(&bfbs[i], "Texture", 0);
        shader_fbo_set_int(&bfbs[i], "TexLuma", 1);
    }

    for (int32_t i = 5; i < 13; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);

    shader_fbo_set_int(&bfbs[13], "Texture0", 0);
    shader_fbo_set_int(&bfbs[13], "Texture1", 1);
    shader_fbo_set_int(&bfbs[13], "Texture2", 2);
    shader_fbo_set_int(&bfbs[13], "Texture3", 3);

    for (int32_t i = 14; i < 16; i++)
        shader_fbo_set_int(&bfbs[i], "Texture", 0);
    
    shader_fbo_set_int(&tfbs, "Texture0", 0);
    shader_fbo_set_int(&tfbs, "Texture1", 1);
    shader_fbo_set_int(&tfbs, "Texture2", 2);
    shader_fbo_set_int(&tfbs, "Texture3", 3);
    shader_fbo_set_uniform_block_binding(&tfbs, "ToneMap", 1);

    shader_fbo_set_int(&particle_shader, "Texture0", 0);
    shader_fbo_set_int(&particle_shader, "Texture1", 1);
    shader_fbo_set_uniform_block_binding(&particle_shader, "GlobalMatrices", 0);

    shader_fbo_set_int(&sprite_shader, "Texture0", 0);
    shader_fbo_set_int(&sprite_shader, "Texture1", 1);
    shader_fbo_set_int(&sprite_shader, "Texture2", 1);
    shader_fbo_set_uniform_block_binding(&sprite_shader, "Common", 0);

    shader_fbo_set_int(&mu_shader, "Texture", 0);
    shader_fbo_set_uniform_block_binding(&mu_shader, "Common", 0);

    render_resize_fb(false);

    fbo_render_initialize(rfbo, &internal_3d_res, fb_vao, cfbs, gfbs);
    fbo_hdr_initialize(hfbo, &internal_3d_res, &internal_2d_res, fb_vao, &ffbs, hfbs);
    fbo_dof_initialize(dfbo, &internal_3d_res, fb_vao, dfbs, dof_common_ubo, dof_texcoords_ubo);
    fbo_pp_initialize(pfbo, &internal_3d_res, fb_vao, bfbs, &tfbs, tone_map_ubo);

    render_resize_fb(true);

    static const int32_t len = 7 * 7 * 2;
    float_t dof_texcoords[7 * 7 * 2];
    render_dof_get_texcoords(dof_texcoords, 3.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, dof_texcoords_ubo);
    glBufferData(GL_UNIFORM_BUFFER, len * sizeof(float_t), 0, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_texcoords_ubo, 0, len * sizeof(float_t));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, len * sizeof(float_t), dof_texcoords);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    farc_dispose(f);
#pragma endregion

    memset(&dir_lights, 0, sizeof(texture));
    dir_lights_count = 0;
    memset(&point_lights, 0, sizeof(texture));
    point_lights_count = 0;

    memset(&mu_font, 0, sizeof(texture));
    texture_data muctx_font_data = {
        .type = TEXTURE_2D,
        .width = ATLAS_WIDTH,
        .height = ATLAS_HEIGHT,
        .depth = 0,
        .data = atlas_texture,
        .pixel_format = GL_RED,
        .pixel_type = GL_UNSIGNED_BYTE,
        .pixel_internal_format = GL_RED,
        .generate_mipmap = false,
        .wrap_mode_s = GL_CLAMP,
        .wrap_mode_t = GL_CLAMP,
        .wrap_mode_r = GL_CLAMP,
        .min_filter = GL_NEAREST,
        .mag_filter = GL_NEAREST,
    };
    texture_load(&mu_font, &muctx_font_data);

    glGenBuffers(1, &mu_vert_vbo);
    glGenBuffers(1, &mu_uv_vbo);
    glGenBuffers(1, &mu_color_vbo);
    glGenBuffers(1, &mu_depth_vbo);
    glGenVertexArrays(1, &mu_vao);

    mu_vert_buf = force_malloc_s(sizeof(GLfloat), MU_BUFFER_SIZE * 2 * 4);
    mu_uv_buf = force_malloc_s(sizeof(GLfloat), MU_BUFFER_SIZE * 2 * 4);
    mu_color_buf = force_malloc_s(sizeof(GLfloat), MU_BUFFER_SIZE * 4 * 4);
    mu_depth_buf = force_malloc_s(sizeof(GLfloat), MU_BUFFER_SIZE * 1 * 4);
    mu_index_buf = force_malloc_s(sizeof(GLuint), MU_BUFFER_SIZE * 6);

    glBindVertexArray(mu_vao);
    glBindBuffer(GL_ARRAY_BUFFER, mu_vert_vbo);
    glBufferData(GL_ARRAY_BUFFER, MU_BUFFER_SIZE * 2 * 4 * sizeof(GLfloat), mu_vert_buf, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 8, 0); // Pos
    glBindBuffer(GL_ARRAY_BUFFER, mu_uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, MU_BUFFER_SIZE * 2 * 4 * sizeof(GLfloat), mu_uv_buf, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 8, 0); // UV
    glBindBuffer(GL_ARRAY_BUFFER, mu_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, MU_BUFFER_SIZE * 4 * 4 * sizeof(GLfloat), mu_color_buf, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, 16, 0); // Color
    glBindBuffer(GL_ARRAY_BUFFER, mu_depth_vbo);
    glBufferData(GL_ARRAY_BUFFER, MU_BUFFER_SIZE * 1 * 4 * sizeof(GLfloat), mu_depth_buf, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, false, 4, 0); // Depth
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    background_color.color = (vec3){ 0.74117647f, 0.74117647f, 0.74117647f };

    for (size_t i = 0; i < classes_count; i++)
        if (classes[i].init && classes[i].flags & CLASSES_INIT_AT_STARTUP)
            classes[i].init();
}

extern vec2 input_move;
extern vec2 input_rotate;
extern bool input_reset;

static void render_update() {
    if (window_handle == GetForegroundWindow()) {
        if (input_reset) {
            input_reset = false;
            camera_reset(cam);
            //camera_move_vec3(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
            //camera_rotate_vec2(cam, &(vec2){ -45.0f, -32.5f });
            //camera_move_vec3(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
            //camera_rotate_vec2(cam, &(vec2){ 136.5f, -20.5f });
            camera_move_vec3(cam, &(vec3){ 0.0f, 1.0f, 3.45f });
        }
        else {
            camera_move_vec2(cam, &input_move);
            camera_rotate_vec2(cam, &input_rotate);
        }
    }

    for (task_render* i = tasks_render.begin; i != tasks_render.end; i++) {
        if (i->type == TASK_RENDER_UPDATE) {
            task_render_update update = i->update;
            switch (update.type) {
            case TASK_RENDER_UPDATE_GL_OBJECT: {
                bool found = false;
                gl_object* gl_obj = 0;
                for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (j->hash == update.hash) {
                        found = true;
                        gl_obj = j->data;
                        break;
                    }

                if (!found)
                    gl_obj = gl_object_init();

                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (j->hash == update.gl_obj.texture) {
                        gl_object_update_texture(gl_obj, &j->data);
                        break;
                    }

                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (j->hash == update.gl_obj.shader) {
                        gl_object_update_shader(gl_obj, &j->data);
                        break;
                    }

                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (j->hash == update.gl_obj.texture) {
                        gl_object_update_vert(gl_obj, j->data);
                        break;
                    }

                gl_object_update_instances(gl_obj, &update.gl_obj.instances);
                if (!found) {
                    hash_ptr_gl_object hash_gl_obj;
                    hash_gl_obj.hash = update.hash;
                    hash_gl_obj.data = gl_obj;
                    vector_hash_ptr_gl_object_append_element(&vec_gl_obj, &hash_gl_obj);
                }
            } break;
            case TASK_RENDER_UPDATE_SHADER: {
                bool found = false;
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (j->hash == update.hash) {
                        found = true;
                        break;
                    }

                if (!found) {
                    shader_model shad;
                    memset(&shad, 0, sizeof(shad));
                    shader_model_load(&shad, &update.shad);

                    hash_shader_model hash_shad;
                    hash_shad.hash = update.hash;
                    hash_shad.data = shad;
                    vector_hash_shader_model_append_element(&vec_shad, &hash_shad);
                }
                free(update.shad.frag_c);
                free(update.shad.frag_g);
                free(update.shad.vert);
                free(update.shad.geom);
            } break;
            case TASK_RENDER_UPDATE_TEXTURE: {
                bool found = false;
                texture tex ;
                for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
                    if (j->hash == update.hash) {
                        found = true;
                        tex = j->data;
                        break;
                    }

                if (!found)
                    memset(&tex, 0, sizeof(texture));

                texture_load(&tex, &update.tex);

                if (!found) {
                    hash_texture hash_tex;
                    hash_tex.hash = update.hash;
                    hash_tex.data = tex;
                    vector_hash_texture_append_element(&vec_tex, &hash_tex);
                }
            } break;
            case TASK_RENDER_UPDATE_TEXTURE_SET: {
                bool found = false;
                texture_set tex_set;
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (j->hash == update.hash) {
                        found = true;
                        tex_set = j->data;
                        break;
                    }

                if (!found)
                    memset(&tex_set, 0, sizeof(texture_set));

                texture_set_load(&tex_set, &update.tex_set);

                if (!found) {
                    hash_texture_set hash_tex_set;
                    hash_tex_set.hash = update.hash;
                    hash_tex_set.data = tex_set;
                    vector_hash_texture_set_append_element(&vec_tex_set, &hash_tex_set);
                }
            } break;
            case TASK_RENDER_UPDATE_VERT: {
                bool found = false;
                vertex* vert = 0;
                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (j->hash == update.hash) {
                        found = true;
                        vert = j->data;
                        break;
                    }

                if (!found)
                    vert = vertex_init();

                vertex_load(vert, &update.vert);

                if (!found) {
                    hash_ptr_vertex hash_vert;
                    hash_vert.hash = update.hash;
                    hash_vert.data = vert;
                    vector_hash_ptr_vertex_append_element(&vec_vert, &hash_vert);
                }
            } break;
            }
            memset(i, 0, sizeof(task_render));
        }
        else if (i->type == TASK_RENDER_FREE) {
            task_render_free free_data = i->free;
            switch (free_data.type) {
            case TASK_RENDER_UPDATE_GL_OBJECT:
                for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (j->hash == free_data.hash) {
                        gl_object_dispose(j->data);
                        vector_hash_ptr_gl_object_remove(&vec_gl_obj, j - vec_gl_obj.begin);
                        break;
                    }
                break;
            case TASK_RENDER_UPDATE_SHADER:
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (j->hash == free_data.hash) {
                        shader_model_free(&j->data);
                        vector_hash_shader_model_remove(&vec_shad, j - vec_shad.begin);
                        break;
                    }
                break;
            case TASK_RENDER_UPDATE_TEXTURE:
                for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
                    if (j->hash == free_data.hash) {
                        texture_free(&j->data);
                        vector_hash_texture_remove(&vec_tex, j - vec_tex.begin);
                        break;
                    }
                break;
            case TASK_RENDER_UPDATE_TEXTURE_SET:
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (j->hash == free_data.hash) {
                        texture_set_free(&j->data);
                        vector_hash_texture_set_remove(&vec_tex_set, j - vec_tex_set.begin);
                        break;
                    }
                break;
            case TASK_RENDER_UPDATE_VERT:
                for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (j->hash == free_data.hash) {
                        vertex_dispose(j->data);
                        vector_hash_ptr_vertex_remove(&vec_vert, j - vec_vert.begin);
                        break;
                    }
                break;
            }
            memset(i, 0, sizeof(task_render));
        }
        else if (i->type == TASK_RENDER_UNIFORM) {
            task_render_uniform uniform = i->uniform;
            for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                if (j->hash == uniform.shader_hash) {
                    shader_model* s = &j->data;
                    char* name = char_buffer_select(&uniform.name);
                    switch (uniform.type) {
                    case TASK_RENDER_UNIFORM_BOOL: {
                        task_render_uniform_bool uni_data = uniform.bool;
                        shader_model_set_bool(s, name, uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_INT32: {
                        task_render_uniform_int32 uni_data = uniform.int32;
                        shader_model_set_int(s, name, uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_FLOAT32: {
                        task_render_uniform_float32 uni_data = uniform.float32;
                        shader_model_set_float(s, name, uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC2: {
                        task_render_uniform_vec2 uni_data = uniform.vec2;
                        shader_model_set_vec2(s, name, uni_data.value.x, uni_data.value.y);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC2I: {
                        task_render_uniform_vec2i uni_data = uniform.vec2i;
                        shader_model_set_vec2i(s, name, uni_data.value.x, uni_data.value.y);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC3: {
                        task_render_uniform_vec3 uni_data = uniform.vec3;
                        shader_model_set_vec3(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC3I: {
                        task_render_uniform_vec3i uni_data = uniform.vec3i;
                        shader_model_set_vec3i(s, name, uni_data.value.x, uni_data.value.y, uni_data.value.z);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC4: {
                        task_render_uniform_vec4 uni_data = uniform.vec4;
                        shader_model_set_vec4(s, name, uni_data.value.x,
                            uni_data.value.y, uni_data.value.z, uni_data.value.w);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC4I: {
                        task_render_uniform_vec4i uni_data = uniform.vec4i;
                        shader_model_set_vec4i(s, name, uni_data.value.x,
                            uni_data.value.y, uni_data.value.z, uni_data.value.w);
                    } break;
                    case TASK_RENDER_UNIFORM_MAT3: {
                        task_render_uniform_mat3 uni_data = uniform.mat3;
                        shader_model_set_mat3(s, name, uni_data.transpose, &uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_MAT4: {
                        task_render_uniform_mat4 uni_data = uniform.mat4;
                        shader_model_set_mat4(s, name, uni_data.transpose, &uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_INT32_ARRAY: {
                        task_render_uniform_int32_array uni_data = uniform.int32_array;
                        shader_model_set_int_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_FLOAT32_ARRAY: {
                        task_render_uniform_float32_array uni_data = uniform.float32_array;
                        shader_model_set_float_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC2_ARRAY: {
                        task_render_uniform_vec2_array uni_data = uniform.vec2_array;
                        shader_model_set_vec2_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC2I_ARRAY: {
                        task_render_uniform_vec2i_array uni_data = uniform.vec2i_array;
                        shader_model_set_vec2i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC3_ARRAY: {
                        task_render_uniform_vec3_array uni_data = uniform.vec3_array;
                        shader_model_set_vec3_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC3I_ARRAY: {
                        task_render_uniform_vec3i_array uni_data = uniform.vec3i_array;
                        shader_model_set_vec3i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC4_ARRAY: {
                        task_render_uniform_vec4_array uni_data = uniform.vec4_array;
                        shader_model_set_vec4_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_VEC4I_ARRAY: {
                        task_render_uniform_vec4i_array uni_data = uniform.vec4i_array;
                        shader_model_set_vec4i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_MAT3_ARRAY: {
                        task_render_uniform_mat3_array uni_data = uniform.mat3_array;
                        shader_model_set_mat3_array(s, name, (GLsizei)uni_data.count,
                            uni_data.transpose, uni_data.value);
                        free(uni_data.value);
                    } break;
                    case TASK_RENDER_UNIFORM_MAT4_ARRAY: {
                        task_render_uniform_mat4_array uni_data = uniform.mat4_array;
                        shader_model_set_mat4_array(s, name, (GLsizei)uni_data.count,
                            uni_data.transpose, uni_data.value);
                        free(uni_data.value);
                    } break;
                    }
                    break;
                }
            char_buffer_dispose(&uniform.name);
            memset(i, 0, sizeof(task_render));
        }
    }

    if (tasks_render_draw2d.end - tasks_render_draw2d.begin > 0) {
        vector_task_render_draw2d_clear(&tasks_render_draw2d_int);
        vector_task_render_draw2d_append(&tasks_render_draw2d_int,
            tasks_render_draw2d.end - tasks_render_draw2d.begin);
        for (task_render_draw2d* i = tasks_render_draw2d.begin; i != tasks_render_draw2d.end; i++)
            vector_task_render_draw2d_append_element(&tasks_render_draw2d_int, i);
        vector_task_render_draw2d_clear(&tasks_render_draw2d);
    }

    if (tasks_render_draw3d.end - tasks_render_draw3d.begin > 0) {
        vector_task_render_draw3d_clear(&tasks_render_draw3d_int);
        vector_task_render_draw3d_append(&tasks_render_draw3d_int,
            tasks_render_draw3d.end - tasks_render_draw3d.begin);
        for (task_render_draw3d* i = tasks_render_draw3d.begin; i != tasks_render_draw3d.end; i++)
            vector_task_render_draw3d_append_element(&tasks_render_draw3d_int, i);
        vector_task_render_draw3d_clear(&tasks_render_draw3d);
    }

    if (rad->update || inten->update) {
        rad->update = false;
        inten->update = false;

        float_t r = inten->val.x;
        float_t g = inten->val.y;
        float_t b = inten->val.z;
        float_t a;
        vec3_dot(inten->val, sv_rgb_to_luma, a);
        vec4* radius = (vec4*)rad->val;

        vec4 v[GAUSSIAN_KERNEL_SIZE];
        for (int32_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
            v[i] = (vec4){ radius[i].x * r, radius[i].y * g, radius[i].z * b, radius[i].w * a };

        for (int32_t i = 6; i < 12; i++)
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

        glBindBuffer(GL_UNIFORM_BUFFER, tone_map_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float_t), tmd->val);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    for (size_t i = 0; i < classes_count; i++)
        if (classes[i].render)
            classes[i].render();

    /*cam.Set(camStruct);

    if (camStruct.DOF.HasValue)
    {
        ModelTrans dofMT = camStruct.DOF.Value;

        dof.Debug.Flags = 0;
        dof.PV.DistanceToFocus = (dofMT.Trans - cam.Pos).Length;
        dof.PV.FocusRange      = dofMT.Scale.X;
        dof.PV.FuzzingRange    = dofMT.Rot.X;
        dof.PV.Ratio           = dofMT.Rot.Y;
        dof.PV.Flags           = camStruct.DOF.Value.Rot.Z > 0.0f ? Camera.DOFPVFlags.Enable : 0;

        if ((dof.PV.Flags & Camera.DOFPVFlags.Enable) != 0)
        {
            double_t fov = cam.FOVIsHorizontal ? 2 * Math.Atan(1 / cam.Aspect * Math.Tan(cam.FOV / 2)) : cam.FOV;
            float_t[] data = GetDOFData(ref dof, (float_t)fov);

            glBindBuffer(BufferTarget.UniformBuffer, dofCommonUBO);
            glBufferSubData(BufferTarget.UniformBuffer, (IntPtr)0, data.Length * 4, data);
            glBindBuffer(BufferTarget.UniformBuffer, 0);
        }
    }*/

    shader_fbo* c_shader = rfbo->samples > 1 ? rfbo->samples > 2 ? rfbo->samples > 4 ? rfbo->samples > 8 ?
        &rfbo->c_shader[9] : &rfbo->c_shader[8] : &rfbo->c_shader[7] : &rfbo->c_shader[6] : &rfbo->c_shader[5];
    shader_fbo_set_float(c_shader, "f", (float_t)cam->max_distance);

    uint8_t global_matrices[sizeof(mat4) * 4];
    ((mat4*)global_matrices)[0] = cam->view_projection;
    ((mat4*)global_matrices)[1] = cam->view;
    ((mat4*)global_matrices)[2] = cam->projection;
    ((vec3*)(global_matrices + sizeof(mat4) * 3))[0] = cam->position;
    glBindBuffer(GL_UNIFORM_BUFFER, global_matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4) * 4, global_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void render_draw() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearStencil(0);
    glClearDepthf(1.0f);
    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(false);

    if (back_2d || back_3d || g_front_3d || c_front_3d) {
        glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);

        if (back_2d) {
            glBindFramebuffer(GL_FRAMEBUFFER, rfbo->fbo);
            glDrawBuffers(2, fbo_render_c_attachments);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            render_back_2d();
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
            glBindFramebuffer(GL_FRAMEBUFFER, hfbo->fbo[0]);
            fbo_render_draw_c(rfbo, false);
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
        if (back_3d || g_front_3d || c_front_3d) {
            glBindFramebuffer(GL_FRAMEBUFFER, rfbo->fbo);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearStencil(0);
            glClearDepthf(1.0f);
            glDepthMask(true);
            glDrawBuffers(4, fbo_render_g_attachments);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawBuffers(1, fbo_render_f_attachments);
            glClearColor(background_color.color.x, background_color.color.y, background_color.color.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDepthMask(false);
            if (back_3d)
                render_back_3d();
            if (g_front_3d)
                render_g_front_3d();
            if (c_front_3d)
                render_c_front_3d();

            glDrawBuffers(1, fbo_render_f_attachments);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);
            glitter_particle_manager_reset_scene_disp_counter(gpm);
            glitter_particle_manager_draw(gpm, 0);
            glitter_particle_manager_draw(gpm, 2);
            glitter_particle_manager_draw(gpm, 1);

            glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
            glBindFramebuffer(GL_FRAMEBUFFER, hfbo->fbo[0]);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearStencil(0);
            glClearDepthf(1.0f);
            glDepthMask(true);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDepthMask(false);
            fbo_render_draw_c(rfbo, enable_dof);

            if (enable_dof)
                fbo_dof_draw(dfbo, hfbo->tcb[0], hfbo->tcb[1], hfbo->fbo[0], enable_dof_f2);
        }

        if (enable_post_process)
            fbo_pp_draw(pfbo, hfbo->tcb[0], hfbo->fbo[0], 1, fbo_hdr_f_attachments);
        fbo_hdr_draw_aa(hfbo);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glViewport(0, 0, width, height);
        fbo_hdr_draw(hfbo);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, common_ubo);
    if (front_2d || ui) {
        glViewport((width - internal_2d_res.x) / 2, (height - internal_2d_res.y) / 2,
            internal_2d_res.x, internal_2d_res.y);
        if (front_2d)
            render_front_2d();
        if (ui)
            render_ui();
        glViewport(0, 0, width, height);
        if (micro_ui)
            render_micro_ui();
    }
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
    vector_task_render_dispose(&tasks_render);
    vector_task_render_draw2d_dispose(&tasks_render_draw2d);
    vector_task_render_draw2d_dispose(&tasks_render_draw2d_int);
    vector_task_render_draw3d_dispose(&tasks_render_draw3d);
    vector_task_render_draw3d_dispose(&tasks_render_draw3d_int);

    for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
        gl_object_dispose(j->data);
    vector_hash_ptr_gl_object_dispose(&vec_gl_obj);

    for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
        shader_model_free(&j->data);
    vector_hash_shader_model_dispose(&vec_shad);

    for (hash_texture* j = vec_tex.begin; j != vec_tex.end; j++)
        texture_free(&j->data);
    vector_hash_texture_dispose(&vec_tex);

    for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
        texture_set_free(&j->data);
    vector_hash_texture_set_dispose(&vec_tex_set);

    for (hash_ptr_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
        vertex_dispose(j->data);
    vector_hash_ptr_vertex_dispose(&vec_vert);

    for (size_t i = 0; i < classes_count; i++)
        if (classes[i].dispose)
            classes[i].dispose();

    camera_dispose(cam);
    radius_dispose(rad);
    intensity_dispose(inten);
    tone_map_sat_gamma_dispose(tmsg);
    tone_map_data_dispose(tmd);
    glitter_particle_manager_dispose(gpm);
    free(muctx);

    texture_free(&dir_lights);
    texture_free(&point_lights);
    texture_free(&mu_font);

    free(dir_light_data);
    free(point_light_trans);
    free(point_light_data);

    fbo_render_dispose(rfbo);
    fbo_hdr_dispose(hfbo);
    fbo_dof_dispose(dfbo);
    fbo_pp_dispose(pfbo);

    for (int32_t i = 0; i < 5; i++) {
        shader_fbo_free(&cfbs[i * 2]);
        shader_fbo_free(&cfbs[i * 2 + 1]);
        shader_fbo_free(&gfbs[i]);
    }

    shader_fbo_free(&ffbs);
    shader_fbo_free(&hfbs[0]);
    shader_fbo_free(&hfbs[1]);

    for (int32_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 5; j++)
            shader_fbo_free(&dfbs[i * 5 + j]);

    for (int32_t i = 0; i < 16; i++)
        shader_fbo_free(&bfbs[i]);
    
    shader_fbo_free(&tfbs);
    shader_fbo_free(&particle_shader);
    shader_fbo_free(&sprite_shader);
    shader_fbo_free(&mu_shader);

    glDeleteBuffers(1, &mu_vert_vbo);
    glDeleteBuffers(1, &mu_uv_vbo);
    glDeleteBuffers(1, &mu_color_vbo);
    glDeleteBuffers(1, &mu_depth_vbo);
    glDeleteVertexArrays(1, &mu_vao);

    glDeleteBuffers(1, &global_matrices_ubo);
    glDeleteBuffers(1, &dof_common_ubo);
    glDeleteBuffers(1, &common_ubo);
    glDeleteBuffers(1, &tone_map_ubo);
    glDeleteBuffers(1, &fb_vbo);
    glDeleteVertexArrays(1, &fb_vao);
}

static void render_micro_ui() {
    mu_Command* cmd;
    size_t count;
    mat4 mat;

    mat4_identity(&mat);
    mat.row0.x = 2.0f / width;
    mat.row1.y = -2.0f / height;
    mat.row2.z = -1.0f;
    mat.row3 = (vec4){ -1.0f, 1.0f, 0.0f, 1.0f };

    shader_fbo_set_mat4(&mu_shader, "mat", false, &mat);
    glEnablei(GL_BLEND, 0);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width, height);
    cmd = 0;
    count = 0;
    while (mu_next_command(muctx, &cmd)) {
        count++;
        switch (cmd->type) {
        case MU_COMMAND_TEXT:
            render_mui_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
            break;
        case MU_COMMAND_RECT:
            render_mui_draw_rect(cmd->rect.rect, cmd->rect.color);
            break;
        case MU_COMMAND_ICON:
            render_mui_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP:
            render_mui_set_clip_rect(cmd->clip.rect);
            break;
        case MU_COMMAND_TEX:
            render_mui_draw_texture();
            break;
        }

        if (!cmd->type)
            break;
    }
    render_mui_flush();
    glScissor(0, 0, width, height);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_SCISSOR_TEST);
}

static void render_ui() {
    size_t count = 0;
    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++)
        if (i->type == TASK_RENDER_DRAW2D_UI)
            count++;

    if (!count)
        return;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW2D_UI)
            continue;

        if (i->blend) {
            glEnablei(GL_BLEND, 0);
            glBlendFuncSeparate(i->blend_src_factor_rgb, i->blend_dst_factor_rgb,
                i->blend_src_factor_alpha, i->blend_dst_factor_alpha);
            glBlendEquationSeparate(i->blend_mode_rgb, i->blend_mode_alpha);
        }
        else
            glDisable(GL_BLEND);
    }
    glDisable(GL_BLEND);
}

static void render_front_2d() {
    size_t count = 0;
    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++)
        if (i->type == TASK_RENDER_DRAW2D_FRONT)
            count++;

    if (!count)
        return;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW2D_FRONT)
            continue;

        if (i->blend) {
            glEnablei(GL_BLEND, 0);
            glBlendFuncSeparate(i->blend_src_factor_rgb, i->blend_dst_factor_rgb,
                i->blend_src_factor_alpha, i->blend_dst_factor_alpha);
            glBlendEquationSeparate(i->blend_mode_rgb, i->blend_mode_alpha);
        }
        else
            glDisable(GL_BLEND);
    }
    glDisable(GL_BLEND);
}

static void render_c_front_3d() {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == TASK_RENDER_DRAW3D_C_FRONT)
            count++;

    if (!count)
        return;

    glDrawBuffers(2, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW3D_C_FRONT)
            continue;

        uint64_t hash = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (j->hash == hash) {
                if (i->blend) {
                    glEnablei(GL_BLEND, 0);
                    glBlendFuncSeparate(i->blend_src_factor_rgb, i->blend_dst_factor_rgb,
                        i->blend_src_factor_alpha, i->blend_dst_factor_alpha);
                    glBlendEquationSeparate(i->blend_mode_rgb, i->blend_mode_alpha);
                }
                else
                    glDisable(GL_BLEND);

                if (i->depth) {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(i->depth_func);
                    glDepthMask(i->depth_mask);
                }
                else
                    glDisable(GL_DEPTH_TEST);

                if (i->cull_face) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(i->cull_face_mode);
                }
                else
                    glDisable(GL_CULL_FACE);
                gl_object_draw_c(j->data);
                break;
            }
    }
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
}

static void render_g_front_3d() {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type != TASK_RENDER_DRAW3D_G_FRONT)
            count++;

    if (!count)
        return;

    glDrawBuffers(4, fbo_render_g_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW3D_G_FRONT)
            continue;

        uint64_t hash = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (j->hash == hash) {
                if (i->depth) {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(i->depth_func);
                    glDepthMask(i->depth_mask);
                }
                else
                    glDisable(GL_DEPTH_TEST);

                if (i->cull_face) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(i->cull_face_mode);
                }
                else
                    glDisable(GL_CULL_FACE);
                gl_object_draw_g(j->data);
                break;
            }
    }
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
    glStencilMask(0x00);
    fbo_render_draw_g(rfbo, dir_lights.id, dir_lights_count, point_lights.id, point_lights_count);
    glDisable(GL_STENCIL_TEST);
}

static void render_back_3d() {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == TASK_RENDER_DRAW3D_BACK)
            count++;

    if (!count)
        return;

    glDrawBuffers(2, fbo_render_c_attachments);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, global_matrices_ubo);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW3D_BACK)
            continue;

        uint64_t hash = i->hash;
        for (hash_ptr_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (j->hash == hash) {
                if (i->blend) {
                    glEnablei(GL_BLEND, 0);
                    glBlendFuncSeparate(i->blend_src_factor_rgb, i->blend_dst_factor_rgb,
                        i->blend_src_factor_alpha, i->blend_dst_factor_alpha);
                    glBlendEquationSeparate(i->blend_mode_rgb, i->blend_mode_alpha);
                }
                else
                    glDisable(GL_BLEND);

                if (i->depth) {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(i->depth_func);
                    glDepthMask(i->depth_mask);
                }
                else
                    glDisable(GL_DEPTH_TEST);

                if (i->cull_face) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(i->cull_face_mode);
                }
                else
                    glDisable(GL_CULL_FACE);
                gl_object_draw_c(j->data);
                break;
            }
    }
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
}

static void render_back_2d() {
    size_t count = 0;
    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++)
        if (i->type == TASK_RENDER_DRAW2D_BACK)
            count++;

    if (!count)
        return;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        if (i->type != TASK_RENDER_DRAW2D_BACK)
            continue;

        if (i->blend) {
            glEnablei(GL_BLEND, 0);
            glBlendFuncSeparate(i->blend_src_factor_rgb, i->blend_dst_factor_rgb,
                i->blend_src_factor_alpha, i->blend_dst_factor_alpha);
            glBlendEquationSeparate(i->blend_mode_rgb, i->blend_mode_alpha);
        }
        else
            glDisable(GL_BLEND);
    }
    glDisable(GL_BLEND);
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
    int32_t width = res->x;
    int32_t height = res->y;
    double_t viewAspect = (double_t)width / (double_t)height;
    res->x = (int32_t)round(height * aspect);
    res->y = (int32_t)round(width / aspect);
    if (viewAspect < aspect)
        res->x = (int32_t)round(res->y * aspect);
    else if (viewAspect > aspect)
        res->y = (int32_t)round(res->x / aspect);
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

    internal_2d_res.x = (int32_t)round((double_t)width * scale);
    internal_2d_res.y = (int32_t)round((double_t)height * scale);
    render_get_aspect_correct_res(&internal_2d_res);
    internal_2d_res.x = clamp(internal_2d_res.x, 1, sv_max_texture_size);
    internal_2d_res.y = clamp(internal_2d_res.y, 1, sv_max_texture_size);

    internal_3d_res.x = (int32_t)round((double_t)internal_res.x * scale);
    internal_3d_res.y = (int32_t)round((double_t)internal_res.y * scale);
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
        glBindBuffer(GL_UNIFORM_BUFFER, common_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float_t) * 4, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        float_t aspect = (float_t)width / (float_t)height;
        aspect /= (float_t)internal_3d_res.x / (float_t)internal_3d_res.y;

        shader_fbo_set_vec2(&ffbs, "scale",
            aspect > 1.0f ? 1.0f / aspect : 1.0f, aspect < 1.0f ? aspect : 1.0f);
    }
}

static void render_input_text_glfw(GLFWwindow* window, uint32_t codepoint) {
    WaitForSingleObject(mu_input_lock, INFINITE);
    if (codepoint >= 0x80 && codepoint < 0x10000) {
        vector_wchar_t_append(&mu_input, 1);
        mu_input.end++;
        mu_input.end[-2] = (wchar_t)codepoint;
        mu_input.end[-1] = 0;
    }
    else if (codepoint < 0x80) {
        vector_wchar_t_append(&mu_input, 1);
        mu_input.end++;
        mu_input.end[-2] = (wchar_t)codepoint;
        mu_input.end[-1] = 0;
        vector_char_append(&mu_input_ansi, 1);
        mu_input_ansi.end++;
        mu_input_ansi.end[-2] = (char)codepoint;
        mu_input_ansi.end[-1] = 0;
    }
    ReleaseMutex(mu_input_lock);
}

static void render_input_mouse_scroll_glfw(GLFWwindow* window, double_t xoffset, double_t yoffset) {
    extern void input_mouse_add_scroll_x(double_t value);
    extern void input_mouse_add_scroll_y(double_t value);

    WaitForSingleObject(mu_input_lock, INFINITE);
    input_mouse_add_scroll_x(xoffset);
    input_mouse_add_scroll_y(yoffset);
    ReleaseMutex(mu_input_lock);
}

static void render_dof_get_texcoords(float_t* a1, float_t a2) {
    int32_t v2, v3, v4;
    float_t v5, v6, v7, v8;
    double_t v9;

    v4 = 0;
    v8 = a2 * 3.0f;
    for (v2 = 0; v2 < 7; v2++) {
        v5 = v2 / 3.0f - 1.0f;
        for (v3 = 0; v3 < 7; v3++) {
            v6 = v3 / 3.0f - 1.0f;
            if (-v5 >= v6) {
                if (v5 > v6) {
                    v7 = 4.0f + v5 / v6;
                    v6 = -v6;
                }
                else if (v5 == 0.0) {
                    v7 = 0.0f;
                    v6 = 0.0f; }
                else {
                    v7 = 6.0f - v6 / v5; v6 = -v5;
                }
            }
            else {
                if (v5 < v6) {
                    v7 = v5 / v6;
                    v6 = v3 / 3.0f - 1.0f;
                }
                else {
                    v7 = 2.0f - v6 / v5;
                    v6 = v2 / 3.0f - 1.0f;
                }
            }
            v9 = v7 * 0.25 * M_PI;
            a1[v4++] = (float_t)(cos(v9) * v6 * v8);
            a1[v4++] = (float_t)(sin(v9) * v6 * v8);
        }
    }
}

static void render_mui_flush() {
    if (mu_buf_idx == 0)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, mu_vert_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat) * mu_buf_idx, mu_vert_buf, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mu_uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat) * mu_buf_idx, mu_uv_buf, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mu_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat) * mu_buf_idx, mu_color_buf, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mu_depth_vbo);
    glBufferData(GL_ARRAY_BUFFER, 1 * 4 * sizeof(GLfloat) * mu_buf_idx, mu_depth_buf, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader_fbo_use(&mu_shader);
    texture_bind(&mu_font, 0);
    glBindVertexArray(mu_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)(mu_buf_idx * 6), GL_UNSIGNED_INT, mu_index_buf);
    shader_fbo_use(0);
    texture_reset(&mu_font, 0);
    glBindVertexArray(0);
    mu_buf_idx = 0;
}

static void render_mui_push_quad(mu_Rect dst, mu_Rect src, mu_Color color) {
    if (mu_buf_idx >= MU_BUFFER_SIZE) {
        render_mui_flush();
        mu_buf_idx = 0;
    }

    size_t      uv_idx = mu_buf_idx * 8;
    size_t    vert_idx = mu_buf_idx * 8;
    size_t   color_idx = mu_buf_idx * 16;
    size_t   depth_idx = mu_buf_idx * 4;
    size_t element_idx = mu_buf_idx * 4;
    size_t   index_idx = mu_buf_idx * 6;
    mu_buf_idx++;

    GLfloat x = (GLfloat)src.x * (GLfloat)(1.0f / ATLAS_WIDTH);
    GLfloat y = (GLfloat)src.y * (GLfloat)(1.0f / ATLAS_HEIGHT);
    GLfloat w = (GLfloat)src.w * (GLfloat)(1.0f / ATLAS_WIDTH);
    GLfloat h = (GLfloat)src.h * (GLfloat)(1.0f / ATLAS_HEIGHT);
    mu_uv_buf[uv_idx + 0] = x;
    mu_uv_buf[uv_idx + 1] = y;
    mu_uv_buf[uv_idx + 2] = x + w;
    mu_uv_buf[uv_idx + 3] = y;
    mu_uv_buf[uv_idx + 4] = x;
    mu_uv_buf[uv_idx + 5] = y + h;
    mu_uv_buf[uv_idx + 6] = x + w;
    mu_uv_buf[uv_idx + 7] = y + h;

    mu_vert_buf[vert_idx + 0] = (GLfloat)dst.x;
    mu_vert_buf[vert_idx + 1] = (GLfloat)dst.y;
    mu_vert_buf[vert_idx + 2] = (GLfloat)dst.x + dst.w;
    mu_vert_buf[vert_idx + 3] = (GLfloat)dst.y;
    mu_vert_buf[vert_idx + 4] = (GLfloat)dst.x;
    mu_vert_buf[vert_idx + 5] = (GLfloat)dst.y + dst.h;
    mu_vert_buf[vert_idx + 6] = (GLfloat)dst.x + dst.w;
    mu_vert_buf[vert_idx + 7] = (GLfloat)dst.y + dst.h;

    mu_color_buf[color_idx + 0] = (GLfloat)color.r * (1.0f / 255.0f);
    mu_color_buf[color_idx + 1] = (GLfloat)color.g * (1.0f / 255.0f);
    mu_color_buf[color_idx + 2] = (GLfloat)color.b * (1.0f / 255.0f);
    mu_color_buf[color_idx + 3] = (GLfloat)color.a * (1.0f / 255.0f);
    memcpy(&mu_color_buf[color_idx + 4], &mu_color_buf[color_idx + 0], sizeof(GLfloat) * 4);
    memcpy(&mu_color_buf[color_idx + 8], &mu_color_buf[color_idx + 0], sizeof(GLfloat) * 4);
    memcpy(&mu_color_buf[color_idx + 12], &mu_color_buf[color_idx + 0], sizeof(GLfloat) * 4);

    mu_depth_buf[depth_idx + 0] = (GLfloat)(MU_BUFFER_SIZE - 1 - mu_buf_idx) * (1.0f / (MU_BUFFER_SIZE - 1));
    memcpy(&mu_depth_buf[depth_idx + 1], &mu_depth_buf[depth_idx + 0], sizeof(GLfloat));
    memcpy(&mu_depth_buf[depth_idx + 2], &mu_depth_buf[depth_idx + 0], sizeof(GLfloat));
    memcpy(&mu_depth_buf[depth_idx + 3], &mu_depth_buf[depth_idx + 0], sizeof(GLfloat));

    mu_index_buf[index_idx + 0] = (GLuint)(element_idx + 0);
    mu_index_buf[index_idx + 1] = (GLuint)(element_idx + 1);
    mu_index_buf[index_idx + 2] = (GLuint)(element_idx + 2);
    mu_index_buf[index_idx + 3] = (GLuint)(element_idx + 2);
    mu_index_buf[index_idx + 4] = (GLuint)(element_idx + 3);
    mu_index_buf[index_idx + 5] = (GLuint)(element_idx + 1);
}

static inline void render_mui_draw_rect(mu_Rect rect, mu_Color color) {
    render_mui_push_quad(rect, atlas[ATLAS_WHITE], color);
}

static inline void render_mui_draw_text(const char* text, mu_Vec2 pos, mu_Color color) {
    mu_Rect dst = { pos.x, pos.y, 0, 0 };
    for (const char* p = text; *p; p++) {
        if ((*p & 0xc0) == 0x80)
            continue;

        mu_Rect src = atlas[ATLAS_FONT + mu_min(*p, 127)];
        dst.w = src.w;
        dst.h = src.h;
        render_mui_push_quad(dst, src, color);
        dst.x += dst.w;
    }
}

static inline void render_mui_draw_icon(int id, mu_Rect rect, mu_Color color) {
    mu_Rect src = atlas[id];
    int x = rect.x + (rect.w - src.w) / 2;
    int y = rect.y + (rect.h - src.h) / 2;
    render_mui_push_quad(mu_rect(x, y, src.w, src.h), src, color);
}

static inline void render_mui_set_clip_rect(mu_Rect rect) {
    render_mui_flush();
    if (rect.w == 0x1000000 || rect.h == 0x1000000)
        glScissor(0, 0, width, height);
    else
        glScissor(rect.x, height - (rect.y + rect.h), rect.w, rect.h);
}

static inline void render_mui_draw_texture() {
}

static int render_mui_get_text_width(mu_Font font, const char* text, size_t len) {
    if (!len)
        len = strlen(text);

    int res = 0;
    for (const char* p = text; *p && len--; p++) {
        if ((*p & 0xc0) == 0x80)
            continue;

        res += atlas[ATLAS_FONT + mu_min(*p, 127)].w;
    }
    return res;
}

static int render_mui_get_text_height(mu_Font font) {
    return 18;
}
