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
#include "../CRE/fbo_helper.h"
#include "../CRE/lock.h"
#include "../CRE/timer.h"
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

static vector_hash_gl_object vec_gl_obj = { 0, 0, 0 };
static vector_hash_material vec_mat = { 0, 0, 0 };
static vector_hash_shader_model vec_shad = { 0, 0, 0 };
static vector_hash_texture vec_tex = { 0, 0, 0 };
static vector_hash_texture_set vec_tex_set = { 0, 0, 0 };
static vector_hash_bone_matrix vec_bone_mat = { 0, 0, 0 };
static vector_hash_vertex vec_vert = { 0, 0, 0 };

static fbo_render* rfbo;
static fbo_hdr* hfbo;
static fbo_dof* dfbo;
static fbo_pp* pfbo;

static shader_fbo bfbs[14];
static shader_fbo cfbs[2];
static shader_fbo dfbs[10];
static shader_fbo fxaas[3];
static shader_fbo tfbs;
shader_fbo particle_shader;
shader_fbo sprite_shader;
shader_fbo grid_shader;

shader_model default_shader;
material default_material;
texture_set default_texture;
bone_matrix default_bone_mat;
vertex default_vertex;

timer render_timer;

#define grid_size 50.0f
#define grid_spacing 0.5f
#define grid_vertex_count ((size_t)((grid_size * 2.0f) / grid_spacing) + 1) * 4

static int32_t fb_vao, fb_vbo;
static int32_t grid_vao, grid_vbo;
static int32_t common_data_ubo = 0;
static int32_t dof_common_ubo = 0;
static int32_t dof_texcoords_ubo = 0;
static int32_t tone_map_ubo = 0;

int32_t dof_common_ubo_binding = 1;
int32_t dof_texcoords_ubo_binding = 2;
int32_t tone_map_ubo_binding = 1;

#define COMMON_DATA_SIZE (int32_t)(sizeof(vec4) + sizeof(mat4) * 3 + sizeof(vec4))

bool      imgui = true;
bool         ui = true;
bool   front_2d = true;
bool    grid_3d = false;
bool   front_3d = true;
bool    back_2d = true;
bool glitter_3d = true;

bool enable_post_process = true;

extern const double_t render_scale_table[] = {
     1.0 / 4.0, //  25%
     2.0 / 4.0, //  50%
     3.0 / 4.0, //  75%
     4.0 / 4.0, // 100%
     5.0 / 4.0, // 125%
     6.0 / 4.0, // 150%
     7.0 / 4.0, // 175%
     8.0 / 4.0, // 200%
};

static float_t old_scale, scale;

static vec2i old_internal_2d_res;
static vec2i old_internal_3d_res;
vec2i internal_2d_res;
vec2i internal_3d_res;
int32_t width;
int32_t height;

static const double_t aspect = 16.0 / 9.0;
camera* cam;
dof_struct* dof;
radius* rad;
intensity* inten;
tone_map* tm;
glitter_particle_manager* gpm;
vec3 back3d_color;
bool set_clear_color;

static void render_load();
static void render_update();
static void render_draw();
static void render_dispose();

static void render_imgui();
static void render_2d(task_render_draw2d_type type);
static void render_3d(task_render_draw3d_type type);
static void render_3d_translucent(task_render_draw3d_type type);
static void render_grid_3d();
static void render_glitter_3d(int32_t alpha);

static void render_drop_glfw(GLFWwindow* window, int count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(bool change_fb);

static void render_dof_get_texcoords(vec2* data, float_t a2);
static void render_imgui_context_menu(classes_struct* classes, const size_t classes_count);
static bool render_glitter_mesh_add_list(glitter_particle_mesh* mesh, vec4* color, mat4* model, mat4* uv_mat);
static void render_update_uniform(task_render_uniform* uniform, shader_model* s);

extern bool close;
lock_val(render_lock);
HWND window_handle;
GLFWwindow* window;
ImGuiContext* ig;
bool global_context_menu;

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(&render_timer, 60.0);

    lock_init(render_lock);
    if (!lock_check_init(render_lock))
        goto End;

    render_init_struct* ris = (render_init_struct*)arg;
    window_handle = 0;

    lock_lock(state_lock);
    state = RENDER_UNINITIALIZED;
    lock_unlock(state_lock);

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

    const char* glfw_titlelabel;
#ifdef DEBUG
#ifdef CLOUD_DEV
    glfw_titlelabel = "CLOUDDev Debug";
#else
    glfw_titlelabel = "CLOUD Debug";
#endif
#else
#ifdef CLOUD_DEV
    glfw_titlelabel = "CLOUDDev";
#else
    glfw_titlelabel = "CLOUD";
#endif
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

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -2;
    }
#pragma endregion

    glEnable(GL_MULTISAMPLE);
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &sv_max_texture_buffer_size);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sv_max_texture_size);
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &sv_max_texture_max_anisotropy);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    lock_lock(state_lock);
    state = RENDER_INITIALIZING;
    lock_unlock(state_lock);

    lock_lock(render_lock);
    render_load();
    lock_unlock(render_lock);

    lock_lock(state_lock);
    state = RENDER_INITIALIZED;
    lock_unlock(state_lock);

#pragma region GL Init
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
#pragma endregion

    while (!close) {
        timer_start_of_cycle(&render_timer);
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
        timer_end_of_cycle(&render_timer);
    }

    lock_lock(state_lock);
    state = RENDER_DISPOSING;
    lock_unlock(state_lock);

    lock_lock(render_lock);
    render_dispose();
    lock_unlock(render_lock);

    lock_lock(state_lock);
    state = RENDER_DISPOSED;
    lock_unlock(state_lock);

#pragma region GLFW Dispose
    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion
    lock_dispose(render_lock);

End:
    timer_dispose(&render_timer);
    return 0;
}

double_t render_get_scale() {
    return scale;
}

void render_set_scale(double_t value) {
    size_t i;
    const size_t c = sizeof(render_scale_table) / sizeof(double_t);
    for (i = 0; i < c; i++)
        if (value <= render_scale_table[i]) break;

    if (i < c)
        value = render_scale_table[i];
    else
        value = render_scale_table[c - 1];

    scale = (float_t)value;
}

int32_t render_get_scale_index() {
    size_t i;
    const size_t c = sizeof(render_scale_table) / sizeof(double_t);
    for (i = 0; i < c; i++)
        if (scale <= render_scale_table[i]) break;

    int32_t index;
    if (i < c)
        index = (int32_t)i;
    else
        index = (int32_t)(c - 1);
    return index;
}

void render_set_scale_index(int32_t index) {
    const size_t c = sizeof(render_scale_table) / sizeof(double_t);
    double_t value;
    if (index < 0)
        value = render_scale_table[0];
    else if (index < c)
        value = render_scale_table[index];
    else
        value = render_scale_table[c - 1];

    scale = (float_t)value;
}

static void render_load() {
    tasks_render = (vector_task_render){ 0, 0, 0 };
    tasks_render_draw2d = (vector_task_render_draw2d){ 0, 0, 0 };
    tasks_render_draw3d = (vector_task_render_draw3d){ 0, 0, 0 };
    tasks_render_draw2d_int = (vector_task_render_draw2d){ 0, 0, 0 };
    tasks_render_draw3d_int = (vector_task_render_draw3d){ 0, 0, 0 };

    temp_draw3d_tasks = (vector_task_render_draw3d){ 0, 0, 0 };
    temp_draw3d_objects = (vector_gl_object){ 0, 0, 0 };

    vec_gl_obj = (vector_hash_gl_object){ 0, 0, 0 };
    vec_mat =  (vector_hash_material){ 0, 0, 0 };
    vec_shad = (vector_hash_shader_model){ 0, 0, 0 };
    vec_tex = (vector_hash_texture){ 0, 0, 0 };
    vec_tex_set = (vector_hash_texture_set){ 0, 0, 0 };
    vec_bone_mat = (vector_hash_bone_matrix){ 0, 0, 0 };
    vec_vert = (vector_hash_vertex){ 0, 0, 0 };

    cam = camera_init();
    dof = dof_init();
    rad = radius_init();
    inten = intensity_init();
    tm = tone_map_init();
    gpm = glitter_particle_manager_init();

    dof_initialize(dof, 0, 0);
    radius_initialize(rad, (vec3[]) { 2.0f, 2.0f, 2.0f });
    intensity_initialize(inten, (vec3[]) { 1.0f, 1.0f, 1.0f });
    tone_map_initialize(tm, 2.0f, true, 1.0f, 1, 1.0f, (vec3[]) { 0.0f, 0.0f, 0.0f}, 0.0f, 0,
        (vec3[]) { 0.0f, 0.0f, 0.0f }, (vec3[]) { 1.0f, 1.0f, 1.0f }, 0);

    camera_initialize(cam, aspect, 70.0);
    camera_reset(cam);
    //camera_set_position(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
    //camera_rotate(cam, &(vec2d){ -45.0, -32.5 });
    //camera_set_position(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
    //camera_rotate(cam, &(vec2d){ 136.5, -20.5 });
    camera_set_position(cam, &(vec3){ 0.0f, 1.0f, 3.45f });

    glGenBuffers(1, &common_data_ubo);
    glGenBuffers(1, &dof_common_ubo);
    glGenBuffers(1, &tone_map_ubo);
    glGenBuffers(1, &dof_texcoords_ubo);

    bind_uniform_buffer(common_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, COMMON_DATA_SIZE, 0, GL_STREAM_DRAW);

    bind_uniform_buffer(dof_common_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(dof_struct), 0, GL_STREAM_DRAW);

    bind_uniform_buffer(tone_map_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(tone_map), 0, GL_STREAM_DRAW);
    bind_uniform_buffer(0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_common_ubo, 0, 16 * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_data_ubo, 0, COMMON_DATA_SIZE);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, tone_map_ubo, 0, 16 * 4);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, dof_texcoords_ubo, 0, align_val(7 * 7, 2) * sizeof(vec2));

#pragma region Load Shaders
    char temp[0x80];
    memset(temp, 0, sizeof(temp));

    hfbo = fbo_hdr_init();
    rfbo = fbo_render_init();
    dfbo = fbo_dof_init();
    pfbo = fbo_pp_init();

    const float_t verts_quad[] = {
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

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Color FB Color";
    param.vert = L"rfb";
    param.frag = L"cfbc";
    shader_fbo_load(&cfbs[0], f, &param);

    memset(&param, 0, sizeof(shader_param));
    param.name = L"Color FB Depth";
    param.vert = L"rfb";
    param.frag = L"cfbd";
    shader_fbo_load(&cfbs[1], f, &param);

    for (int32_t i = 3; i < 6; i++) {
        memset(&param, 0, sizeof(shader_param));
        param.name = L"FXAA";
        param.vert = L"rfb";
        param.frag = L"fxaa";
        snprintf(temp, sizeof(temp), "FXAA_PRESET %d", i);
        param.param[0] = temp;
        shader_fbo_load(&fxaas[i - 3], f, &param);
    }

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
            snprintf(temp, sizeof(temp), "USE_F2_COC (%d)", i);
            param.param[0] = temp;
            shader_fbo_load(&dfbs[i * 5 + j], f, &param);
        }

    wchar_t* post_process_vert_shaders[] = {
        L"rfb",
        L"rfb",
        L"rfb",
        L"exposure1",
        L"rfb",
        L"rfb",
        L"rfb",
        L"rfb",
        L"rfb",
        L"rfb",
        L"blur5",
        L"rfb",
        L"exposure2",
        L"exposure3",
    };
    
    wchar_t* post_process_frag_shaders[] = {
        L"blur1",
        L"blur2",
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

    for (int32_t i = 0; i < 14; i++) {
        memset(&param, 0, sizeof(shader_param));
        param.param[0] = temp;
        param.name = L"Post Process FB";
        param.vert = post_process_vert_shaders[i];
        param.frag = post_process_frag_shaders[i];
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
        uint8_t tex_dat_color[] = {
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

        texture_data tex_data_color = {
            .type = TEXTURE_2D,
            .width = 8,
            .height = 8,
            .depth = 0,
            .data = tex_dat_color,
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
        
        uint8_t tex_dat_white[] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        };

        texture_data tex_data_white = {
            .type = TEXTURE_2D,
            .width = 8,
            .height = 8,
            .depth = 0,
            .data = tex_dat_white,
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

        uint8_t tex_dat_black[] = {
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

        texture_data tex_data_black = {
            .type = TEXTURE_2D,
            .width = 8,
            .height = 8,
            .depth = 0,
            .data = tex_dat_black,
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

        texture_cube_sub_data tex_data_cube_sub = {
            .width = 8,
            .height = 8,
            .data = tex_dat_white,
            .pixel_type = GL_UNSIGNED_BYTE,
            .pixel_format = GL_RGB,
            .pixel_internal_format = GL_RGB8,
        };
        
        texture_cube_data tex_data_cube = {
            .type = TEXTURE_CUBE,
            .px = tex_data_cube_sub,
            .nx = tex_data_cube_sub,
            .py = tex_data_cube_sub,
            .ny = tex_data_cube_sub,
            .pz = tex_data_cube_sub,
            .nz = tex_data_cube_sub,
            .generate_mipmap = false,
            .wrap_mode_s = GL_REPEAT,
            .wrap_mode_t = GL_REPEAT,
            .wrap_mode_r = GL_REPEAT,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST,
        };

        texture_set_data tex_set;
        memset(&tex_set, 0, sizeof(texture_set_data));
        tex_set.color = tex_data_color;
        tex_set.color_mask = tex_data_white;
        tex_set.normal = tex_data_black;
        tex_set.specular = tex_data_black;
        tex_set.transparency = tex_data_white;
        tex_set.env_map = tex_data_cube;
        texture_set_load(&default_texture, &tex_set);

        default_material.texture = &default_texture;
        default_material.blend = material_blend_default;
        default_material.param = material_param_default;
        default_material.translucent = false;

        shader_model_data param_model;
        memset(&param_model, 0, sizeof(shader_model_data));

        stream* s;
        size_t l;

        s = io_wopen(L"rom\\shaders\\0000.vert", L"r");
        l = s->length;
        param_model.vert = force_malloc(l + 1);
        io_read(s, param_model.vert, l);
        param_model.vert[l] = 0;
        io_dispose(s);

        s = io_wopen(L"rom\\shaders\\0000.frag", L"r");
        l = s->length;
        param_model.frag = force_malloc(l + 1);
        io_read(s, param_model.frag, l);
        param_model.frag[l] = 0;
        io_dispose(s);

        memset(&param, 0, sizeof(shader_param));
        param.name = L"Default";
        param_model.param = param;
        shader_model_load(&default_shader, &param_model);
        free(param_model.vert);
        free(param_model.frag);

        bone_matrix_init(&default_bone_mat, 0xFFFF);
        mat4* bone_mat = force_malloc_s(mat4, 0xFFFF);
        for (size_t i = 0; i < 0xFFFF; i++)
            bone_mat[i] = mat4_identity;
        bone_matrix_load(&default_bone_mat, 0, 0xFFFF, bone_mat);
        free(bone_mat);

        vec3 verts_cube_position[] = {
            { -1.0f, -1.0f, -1.0f },
            {  1.0f,  1.0f, -1.0f },
            {  1.0f, -1.0f, -1.0f },
            {  1.0f,  1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f,  1.0f, -1.0f },

            { -1.0f, -1.0f,  1.0f },
            {  1.0f, -1.0f,  1.0f },
            {  1.0f,  1.0f,  1.0f },
            {  1.0f,  1.0f,  1.0f },
            { -1.0f,  1.0f,  1.0f },
            { -1.0f, -1.0f,  1.0f },

            { -1.0f,  1.0f,  1.0f },
            { -1.0f,  1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f,  1.0f },
            { -1.0f,  1.0f,  1.0f },

            {  1.0f,  1.0f, -1.0f },
            {  1.0f,  1.0f,  1.0f },
            {  1.0f, -1.0f, -1.0f },
            {  1.0f, -1.0f,  1.0f },
            {  1.0f, -1.0f, -1.0f },
            {  1.0f,  1.0f,  1.0f },

            { -1.0f, -1.0f, -1.0f },
            {  1.0f, -1.0f, -1.0f },
            {  1.0f, -1.0f,  1.0f },
            {  1.0f, -1.0f,  1.0f },
            { -1.0f, -1.0f,  1.0f },
            { -1.0f, -1.0f, -1.0f },

            { -1.0f,  1.0f, -1.0f },
            {  1.0f,  1.0f,  1.0f },
            {  1.0f,  1.0f, -1.0f },
            {  1.0f,  1.0f,  1.0f },
            { -1.0f,  1.0f, -1.0f },
            { -1.0f,  1.0f,  1.0f },
        };
        
        vec2 verts_cube_texcoord[] = {
            { 1.0f, 1.0f },
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },

            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f },
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },

            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f },

            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 0.0f },

            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },

            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },
        };

        vec3 verts_cube_normal[] = {
            {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f, -1.0f },

            {  0.0f,  0.0f,  1.0f },
            {  0.0f,  0.0f,  1.0f },
            {  0.0f,  0.0f,  1.0f },
            {  0.0f,  0.0f,  1.0f },
            {  0.0f,  0.0f,  1.0f },
            {  0.0f,  0.0f,  1.0f },

            { -1.0f,  0.0f,  0.0f },
            { -1.0f,  0.0f,  0.0f },
            { -1.0f,  0.0f,  0.0f },
            { -1.0f,  0.0f,  0.0f },
            { -1.0f,  0.0f,  0.0f },
            { -1.0f,  0.0f,  0.0f },

            {  1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },

            {  0.0f, -1.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },

            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f },
        };

        vertex_data vert_upd;
        memset(&vert_upd, 0, sizeof(vertex_data));
        vert_upd.length = sizeof(verts_cube_position) / sizeof(vec3);
        vert_upd.position = verts_cube_position;
        vert_upd.texcoord0 = verts_cube_texcoord;
        vert_upd.normal = verts_cube_normal;

        memset(&default_vertex, 0, sizeof(vertex));
        vertex_init(&default_vertex);
        vertex_load(&default_vertex, &vert_upd);
    }

    shader_fbo_set_int(&cfbs[0], "g_color", 0);
    shader_fbo_set_int(&cfbs[1], "g_color", 0);
    shader_fbo_set_int(&cfbs[1], "g_depth", 1);

    for (int32_t i = 0; i < 3; i++)
        shader_fbo_set_int(&fxaas[i], "g_color", 0);

    for (int32_t i = 0; i < 10; i += 5) {
        shader_fbo_set_int(&dfbs[i], "g_depth", 0);
        shader_fbo_set_int(&dfbs[i + 1], "g_tile", 0);
        shader_fbo_set_int(&dfbs[i + 2], "g_depth_point_sampler", 0);
        shader_fbo_set_int(&dfbs[i + 2], "g_color_linear_sampler", 1);
        shader_fbo_set_int(&dfbs[i + 2], "g_tile_sampler", 2);
        shader_fbo_set_int(&dfbs[i + 3], "g_presort", 0);
        shader_fbo_set_int(&dfbs[i + 3], "g_color", 1);
        shader_fbo_set_int(&dfbs[i + 3], "g_tile", 2);
        shader_fbo_set_int(&dfbs[i + 4], "g_filtered_rgb_sampler", 0);
        shader_fbo_set_int(&dfbs[i + 4], "g_filtered_a_sampler", 1);
        shader_fbo_set_int(&dfbs[i + 4], "g_tile_sampler", 2);
        shader_fbo_set_int(&dfbs[i + 4], "g_full_res_color_sampler", 3);
        shader_fbo_set_int(&dfbs[i + 4], "g_full_res_depth_sampler", 4);

        shader_fbo_set_uniform_block_binding(&dfbs[i], "dof_common", dof_common_ubo_binding);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 2], "dof_common", dof_common_ubo_binding);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "dof_common", dof_common_ubo_binding);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 3], "dof_texcoords", dof_texcoords_ubo_binding);
        shader_fbo_set_uniform_block_binding(&dfbs[i + 4], "dof_common", dof_common_ubo_binding);
    }

    for (int32_t i = 0; i < 2; i++)
        shader_fbo_set_int(&bfbs[i], "g_color", 0);

    shader_fbo_set_int(&bfbs[2], "g_color", 0);
    shader_fbo_set_int(&bfbs[2], "g_luma", 1);

    shader_fbo_set_int(&bfbs[3], "g_luma", 0);

    for (int32_t i = 4; i < 11; i++)
        shader_fbo_set_int(&bfbs[i], "g_color", 0);

    shader_fbo_set_int(&bfbs[11], "g_bloom_full", 0);
    shader_fbo_set_int(&bfbs[11], "g_bloom_half", 1);
    shader_fbo_set_int(&bfbs[11], "g_bloom_quat", 2);
    shader_fbo_set_int(&bfbs[11], "g_bloom_eight", 3);
    shader_fbo_set_int(&bfbs[12], "g_luma", 0);
    shader_fbo_set_int(&bfbs[13], "g_exp_hist", 0);

    shader_fbo_set_int(&tfbs, "g_color", 0);
    shader_fbo_set_int(&tfbs, "g_bloom", 1);
    shader_fbo_set_int(&tfbs, "g_tone_map", 2);
    shader_fbo_set_int(&tfbs, "g_exposure", 3);
    shader_fbo_set_int(&tfbs, "g_flare_shaft", 4);
    shader_fbo_set_int(&tfbs, "g_flare_ghost", 5);
    shader_fbo_set_int(&tfbs, "g_back", 6);
    shader_fbo_set_uniform_block_binding(&tfbs, "tone_map", tone_map_ubo_binding);

    shader_fbo_set_int(&particle_shader, "g_color", 0);
    shader_fbo_set_int(&particle_shader, "g_mask", 1);

    shader_fbo_set_int(&sprite_shader, "g_color0", 0);
    shader_fbo_set_int(&sprite_shader, "g_color1", 1);
    shader_fbo_set_int(&sprite_shader, "g_tex", 2);

    render_resize_fb(false);

    fbo_render_initialize(rfbo, &internal_3d_res, fb_vao, &cfbs[0], &cfbs[1]);
    fbo_hdr_initialize(hfbo, &internal_3d_res, fb_vao, fxaas);
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

    float_t* grid_verts = force_malloc_s(float_t, grid_vertex_count * 4);

    vec4h r_color, g_color, b_color, m_color;
    vec4_to_vec4h(((vec4) { 1.0f, 0.0f, 0.0f, 1.0f }), r_color);
    vec4_to_vec4h(((vec4) { 0.0f, 1.0f, 0.0f, 1.0f }), g_color);
    vec4_to_vec4h(((vec4) { 0.2f, 0.2f, 0.2f, 0.6f }), b_color);
    vec4_to_vec4h(((vec4) { 0.5f, 0.5f, 0.5f, 0.6f }), m_color);

    size_t v = 0;
    for (float_t x = -grid_size; x <= grid_size; x += grid_spacing) {
        vec4h x_color;
        vec4h z_color;
        if (x == 0) {
            x_color = r_color;
            z_color = g_color;
        }
        else if (abs((int32_t)(x / grid_spacing)) % 2 == 0) {
            x_color = b_color;
            z_color = b_color;
        }
        else {
            x_color = m_color;
            z_color = m_color;
        }

        grid_verts[v++] = x;
        grid_verts[v++] = -grid_size;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.z;

        grid_verts[v++] = x;
        grid_verts[v++] = grid_size;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.z;

        grid_verts[v++] = -grid_size;
        grid_verts[v++] = x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.z;

        grid_verts[v++] = grid_size;
        grid_verts[v++] = x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&x_color.z;
    }

    glGenBuffers(1, &grid_vbo);
    glGenVertexArrays(1, &grid_vao);
    bind_array_buffer(grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * grid_vertex_count * 4, grid_verts, GL_STATIC_DRAW);

    bind_vertex_array(grid_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 4, (void*)0); // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_HALF_FLOAT, GL_FALSE, sizeof(float_t) * 4, (void*)(sizeof(float_t) * 2)); // Color
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
    set_clear_color = true;

    classes_process_init(classes, classes_count);
}

extern vec2d input_move;
extern vec2d input_rotate;
extern double_t input_roll;
extern bool input_reset;

static void render_update() {
    global_context_menu = true;
    classes_process_imgui(classes, classes_count);
    
    if (sv_fxaa_changed)
        sv_fxaa_changed = false;
    
    if (sv_fxaa_preset_changed)
        sv_fxaa_preset_changed = false;

    if (old_scale != scale)
        render_resize_fb(true);
    old_scale = scale;

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

    mat4 default_uv_mat[4] = {
        mat4_identity,
        mat4_identity,
        mat4_identity,
        mat4_identity,
    };

    shader_model_set_int_array(&default_shader, "material", 6, ((int32_t[]){ 0, 1, 2, 3, 4, 5 }));
    shader_model_set_int_array(&default_shader, "tex_mode", 6, ((int32_t[]){ 0, 0, 0, 0, 0, 0 }));
    shader_model_set_bool(&default_shader, "use_bones", false);
    shader_model_set_bool(&default_shader, "write_only_depth", false);
    shader_model_set_mat4(&default_shader, "model", false, mat4_identity);
    shader_model_set_mat4(&default_shader, "model_normal", false, mat3_identity);
    shader_model_set_mat4_array(&default_shader, "uv_mat", 4, false, default_uv_mat);
    shader_model_set_vec4(&default_shader, "color", vec4_identity);
    shader_model_set_vec4(&default_shader, "material_param.ambient", default_material.param.ambient);
    shader_model_set_vec4(&default_shader, "material_param.diffuse", default_material.param.diffuse);
    shader_model_set_vec4(&default_shader, "material_param.specular", default_material.param.specular);
    shader_model_set_vec4(&default_shader, "material_param.emission", default_material.param.emission);
    shader_model_set_float(&default_shader, "material_param.shininess", default_material.param.shininess);

    classes_process_render(classes, classes_count);

    GPM_VAL->updated = false;

    if (memcmp(&GPM_VAL->cam_view, &cam->view, sizeof(GPM_VAL->cam_view))) {
        GPM_VAL->cam_view = cam->view;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_inv_view, &cam->inv_view, sizeof(GPM_VAL->cam_inv_view))) {
        GPM_VAL->cam_inv_view = cam->inv_view;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_inv_view_mat3, &cam->inv_view_mat3, sizeof(GPM_VAL->cam_inv_view_mat3))) {
        GPM_VAL->cam_inv_view_mat3 = cam->inv_view_mat3;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_view_point, &cam->view_point, sizeof(GPM_VAL->cam_view_point))) {
        GPM_VAL->cam_view_point = cam->view_point;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_rotation_y, &cam->rotation.y, sizeof(GPM_VAL->cam_rotation_y))) {
        GPM_VAL->cam_rotation_y = cam->rotation.y;
        GPM_VAL->updated = true;
    }

    glitter_particle_manager_calc_draw(GPM_VAL, render_glitter_mesh_add_list);

    for (task_render* i = tasks_render.begin; i != tasks_render.end; ) {
        if (i->type == TASK_RENDER_UPDATE) {
            bone_matrix bm;
            gl_object glo;
            material m;
            shader_model s;
            texture t;
            texture_set ts;
            vertex v;

            task_render_update update = i->update;
            switch (update.type) {
            case TASK_RENDER_UPDATE_BONE_MATRIX: {
                bool found = false;
                bone_matrix* bone_mat;
                for (hash_bone_matrix* j = vec_bone_mat.begin; j != vec_bone_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        bone_mat = &j->data;
                        break;
                    }

                if (!found) {
                    memset(&bm, 0, sizeof(bm));
                    bone_matrix_init(&bm, update.bone_mat.count);
                    bone_mat = &bm;
                }

                bone_matrix_load(bone_mat, update.bone_mat.index, update.bone_mat.count,
                    update.bone_mat.count > 1 ? update.bone_mat.data : &update.bone_mat.mat);

                if (!found) {
                    hash_bone_matrix hash_bone_mat;
                    hash_bone_mat.hash = update.hash;
                    hash_bone_mat.data = *bone_mat;
                    vector_hash_bone_matrix_push_back(&vec_bone_mat, &hash_bone_mat);
                }

                if (update.bone_mat.count > 1)
                    free(update.bone_mat.data);
            } break;
            case TASK_RENDER_UPDATE_GL_OBJECT: {
                bool found = false;
                gl_object* gl_obj = 0;
                for (hash_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        gl_obj = &j->data;
                        break;
                    }

                if (!found) {
                    memset(&glo, 0, sizeof(gl_object));
                    gl_object_init(&glo);
                    gl_obj = &glo;
                }

                bool found_vert = false;
                for (hash_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.vert)) {
                        found_vert = true;
                        gl_object_load_vert(gl_obj, &j->data);
                        break;
                    }

                if (!found_vert)
                    gl_object_load_vert(gl_obj, &default_vertex);

                bool found_material = false;
                for (hash_material* j = vec_mat.begin; j != vec_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.material)) {
                        found_material = true;
                        gl_object_load_material(gl_obj, &j->data);
                        break;
                    }

                if (!found_material)
                    gl_object_load_material(gl_obj, &default_material);

                bool found_shader = false;
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.shader)) {
                        found_shader = true;
                        gl_object_load_shader(gl_obj, &j->data);
                        break;
                    }

                if (!found_shader)
                    gl_object_load_shader(gl_obj, &default_shader);

                bool found_bone_matrix = false;
                for (hash_bone_matrix* j = vec_bone_mat.begin; j != vec_bone_mat.end; j++)
                    if (HASH_COMPARE(j->hash, update.gl_obj.bone_mat)) {
                        found_bone_matrix = true;
                        gl_object_load_bone_matrix(gl_obj, &j->data);
                        break;
                    }

                if (!found_bone_matrix)
                    gl_object_load_bone_matrix(gl_obj, &default_bone_mat);

                if (!found) {
                    hash_gl_object hash_gl_obj;
                    hash_gl_obj.hash = update.hash;
                    hash_gl_obj.data = *gl_obj;
                    vector_hash_gl_object_push_back(&vec_gl_obj, &hash_gl_obj);
                }

                gl_object_load_cull_face(gl_obj, &update.gl_obj.cull_face);
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
                    memset(&m, 0, sizeof(material));
                    mat = &m;
                }

                mat->blend = update.mat.blend;
                mat->translucent = update.mat.translucent;

                bool found_texture = false;
                for (hash_texture_set* j = vec_tex_set.begin; j != vec_tex_set.end; j++)
                    if (HASH_COMPARE(j->hash, update.mat.texture)) {
                        found_texture = true;
                        mat->texture = &j->data;
                        break;
                    }

                if (!found_texture)
                    mat->texture = &default_texture;

                if (!found) {
                    hash_material hash_mat;
                    hash_mat.hash = update.hash;
                    hash_mat.data = *mat;
                    vector_hash_material_push_back(&vec_mat, &hash_mat);
                }
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
                    memset(&s, 0, sizeof(s));
                    shad = &s;
                }

                shader_model_load(shad, &update.shad);

                if (!found) {
                    hash_shader_model hash_shad;
                    hash_shad.hash = update.hash;
                    hash_shad.data = *shad;
                    vector_hash_shader_model_push_back(&vec_shad, &hash_shad);
                }
                else

                free(update.shad.frag);
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
                    memset(&t, 0, sizeof(texture));
                    tex = &t;
                }
                
                texture_load(tex, &update.tex);

                if (!found) {
                    hash_texture hash_tex;
                    hash_tex.hash = update.hash;
                    hash_tex.data = *tex;
                    vector_hash_texture_push_back(&vec_tex, &hash_tex);
                }

                free(update.tex.data);
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
                    memset(&ts, 0, sizeof(texture_set));
                    tex_set = &ts;
                }
                
                texture_set_load(tex_set, &update.tex_set);

                if (!found) {
                    hash_texture_set hash_tex_set;
                    hash_tex_set.hash = update.hash;
                    hash_tex_set.data = ts;
                    vector_hash_texture_set_push_back(&vec_tex_set, &hash_tex_set);
                }

                free(update.tex_set.color.data);
                free(update.tex_set.color_mask.data);
                free(update.tex_set.normal.data);
                free(update.tex_set.specular.data);
                free(update.tex_set.transparency.data);
                free(update.tex_set.env_map.px.data);
                free(update.tex_set.env_map.nx.data);
                free(update.tex_set.env_map.py.data);
                free(update.tex_set.env_map.ny.data);
                free(update.tex_set.env_map.pz.data);
                free(update.tex_set.env_map.nz.data);
            } break;
            case TASK_RENDER_UPDATE_VERTEX: {
                bool found = false;
                vertex* vert = 0;
                for (hash_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, update.hash)) {
                        found = true;
                        vert = &j->data;
                        break;
                    }

                if (!found) {
                    memset(&v, 0, sizeof(vertex));
                    vertex_init(&v);
                    vert = &v;
                }

                vertex_load(vert, &update.vert);
                    
                if (!found) {
                    hash_vertex hash_vert;
                    hash_vert.hash = update.hash;
                    hash_vert.data = *vert;
                    vector_hash_vertex_push_back(&vec_vert, &hash_vert);
                }

                free(update.vert.position);
                free(update.vert.texcoord0);
                free(update.vert.texcoord1);
                free(update.vert.texcoord2);
                free(update.vert.texcoord3);
                free(update.vert.color);
                free(update.vert.normal);
                free(update.vert.bone_index);
                free(update.vert.bone_weight);
            } break;
            }
        }
        else if (i->type == TASK_RENDER_FREE) {
            task_render_free free_data = i->free;
            switch (free_data.type) {
            case TASK_RENDER_FREE_BONE_MATRIX:
                for (hash_bone_matrix* j = vec_bone_mat.begin; j != vec_bone_mat.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        for (hash_gl_object* k = vec_gl_obj.begin; k != vec_gl_obj.end; k++)
                            if (k->data.bone_mat == &j->data)
                                k->data.bone_mat = &default_bone_mat;

                        bone_matrix_free(&j->data);
                        vector_hash_bone_matrix_erase(&vec_bone_mat, j - vec_bone_mat.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_GL_OBJECT:
                for (hash_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        gl_object_free(&j->data);
                        vector_hash_gl_object_erase(&vec_gl_obj, j - vec_gl_obj.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_MATERIAL:
                for (hash_material* j = vec_mat.begin; j != vec_mat.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        for (hash_gl_object* k = vec_gl_obj.begin; k != vec_gl_obj.end; k++)
                            if (k->data.material == &j->data)
                                k->data.material = &default_material;

                        vector_hash_material_erase(&vec_mat, j - vec_mat.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_SHADER:
                for (hash_shader_model* j = vec_shad.begin; j != vec_shad.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        for (hash_gl_object* k = vec_gl_obj.begin; k != vec_gl_obj.end; k++)
                            if (k->data.shader == &j->data)
                                k->data.shader = &default_shader;

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
                        for (hash_material* k = vec_mat.begin; k != vec_mat.end; k++)
                            if (k->data.texture == &j->data)
                                k->data.texture = &default_texture;

                        texture_set_free(&j->data);
                        vector_hash_texture_set_erase(&vec_tex_set, j - vec_tex_set.begin);
                        break;
                    }
                break;
            case TASK_RENDER_FREE_VERTEX:
                for (hash_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
                    if (HASH_COMPARE(j->hash, free_data.hash)) {
                        for (hash_gl_object* k = vec_gl_obj.begin; k != vec_gl_obj.end; k++)
                            if (k->data.vertex == &j->data)
                                k->data.vertex = &default_vertex;

                        vertex_free(&j->data);
                        vector_hash_vertex_erase(&vec_vert, j - vec_vert.begin);
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

            render_update_uniform(&uni, s);
            string_dispose(&uni.name);
        }
        memset(i, 0, sizeof(task_render));
        vector_task_render_erase(&tasks_render, 0);
    }

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            string_dispose(&j->name);
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
            string_dispose(&j->name);
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
        for (int32_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
            vec4_mult(radius[i], rgba, v[i]);

        for (int32_t i = 4; i < 10; i++)
            shader_fbo_set_vec4_array(&bfbs[i], "gaussian_kernel", GAUSSIAN_KERNEL_SIZE, v);
    }

    if (tm->update_tex) {
        tm->update_tex = false;
        fbo_pp_tone_map_set(pfbo, tm->tex, 16 * TONE_MAP_SAT_GAMMA_SAMPLES);
    }

    if (tm->update_data) {
        tm->update_data = false;
        uniform_value[U_TONE_MAP] = tm->tone_map_method;
        uniform_value[U_SCENE_FADE] = tm->scene_fade_alpha > 0.009999999f ? 1 : 0;

        bind_uniform_buffer(tone_map_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(tone_map), &tm->data);
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

    struct common_data_struct {
        vec4 res; //x=width, y=height, z=1/width, w=1/height
        mat4 vp;
        mat4 view;
        mat4 projection;
        vec3 view_pos;
    } common_data;

    common_data.res.x = (float_t)internal_3d_res.x;
    common_data.res.y = (float_t)internal_3d_res.y;
    common_data.res.z = 1.0f / (float_t)internal_3d_res.x;
    common_data.res.w = 1.0f / (float_t)internal_3d_res.y;
    common_data.vp = cam->view_projection;
    common_data.view = cam->view;
    common_data.projection = cam->projection;
    common_data.view_pos = cam->view_point;

    bind_uniform_buffer(common_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, COMMON_DATA_SIZE, &common_data);
    bind_uniform_buffer(0);

    bind_uniform_buffer(dof_common_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(dof_struct), dof->data);
    bind_uniform_buffer(0);
}

static void render_draw() {
    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat depth_clear = 1.0f;
    static const GLint stencil_clear = 0;

    bind_framebuffer(0);
    glDepthMask(true);
    glStencilMask(0xFF);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
    glStencilMask(0);
    glDepthMask(false);
    bind_uniform_buffer_base(0, common_data_ubo);

    for (int32_t i = 0; i < 32; i++) {
        if (!check_index_tex1d_set(i) && !check_index_tex2d_set(i)
            && !check_index_tex3d_set(i)
            && !check_index_texcube_set(i))
            continue;

        active_texture(i);
        bind_tex1d(0);
        bind_tex2d(0);
        bind_tex3d(0);
        bind_texcube(0);
    }

    if (back_2d || front_3d || glitter_3d) {
        glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);

        if (back_2d) {
            bind_framebuffer(rfbo->fbo);
            glDrawBuffers(1, fbo_render_attachments);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            render_2d(TASK_RENDER_DRAW2D_BACK);
            bind_framebuffer(hfbo->fbo);
            fbo_render_draw(rfbo, false);
        }

        if (front_3d || glitter_3d) {
            bind_framebuffer(rfbo->fbo);
            glDepthMask(true);
            glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
            glDepthMask(false);

            if (set_clear_color) {
                vec4 color;
                *(vec3*)&color = back3d_color;
                color.w = 1.0f;
                glDrawBuffers(1, fbo_render_attachments);
                glClearBufferfv(GL_COLOR, 0, (GLfloat*)&color);
            }

            if (grid_3d)
                render_grid_3d();

            if (front_3d)
                render_3d(TASK_RENDER_DRAW3D_FRONT);

            if (glitter_3d)
                render_glitter_3d(0);

            if (glitter_3d)
                render_glitter_3d(2);

            if (glitter_3d)
                render_glitter_3d(1);

            if (front_3d)
                render_3d_translucent(TASK_RENDER_DRAW3D_FRONT);

            bind_framebuffer(hfbo->fbo);
            glDrawBuffers(2, fbo_hdr_c_attachments);
            glDepthMask(true);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            glClearBufferfv(GL_DEPTH, 0, &depth_clear);
            glDepthMask(false);

            if (dof->debug.flags & DOF_DEBUG_USE_UI_PARAMS) {
                if (dof->debug.flags & DOF_DEBUG_ENABLE_DOF) {
                    fbo_render_draw(rfbo, true);
                    if (dof->debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF)
                        fbo_dof_draw(dfbo, hfbo->color_tcb, hfbo->depth_tcb, false,
                            hfbo, fbo_hdr_set_fbo_begin, fbo_hdr_set_fbo_end);
                    else
                        fbo_dof_draw(dfbo, hfbo->color_tcb, hfbo->depth_tcb, true,
                            hfbo, fbo_hdr_set_fbo_begin, fbo_hdr_set_fbo_end);
                }
                else
                    fbo_render_draw(rfbo, false);
            }
            else if (dof->pv.enable && dof->pv.f2.ratio > 0.0f) {
                fbo_render_draw(rfbo, true);
                fbo_dof_draw(dfbo, hfbo->color_tcb, hfbo->depth_tcb, true,
                    hfbo, fbo_hdr_set_fbo_begin, fbo_hdr_set_fbo_end);
            }
            else
                fbo_render_draw(rfbo, false);
        }

        if (enable_post_process)
            fbo_pp_draw(pfbo, hfbo->color_tcb, hfbo, fbo_hdr_set_fbo_begin, fbo_hdr_set_fbo_end);

        if (sv_fxaa)
            fbo_hdr_draw_fxaa(hfbo, sv_fxaa_preset);

        bind_framebuffer(hfbo->fbo);
        glDrawBuffers(1, fbo_hdr_f_attachments);
        classes_process_draw(classes, classes_count);

        vec2i bb_offset;
        bb_offset.x = (width - internal_2d_res.x) / 2;
        bb_offset.y = (height - internal_2d_res.y) / 2;

        bind_framebuffer(0);
        fbo_helper_blit(hfbo->fbo, GL_COLOR_ATTACHMENT0, 0, GL_COLOR_ATTACHMENT0,
            0, 0, hfbo->res.x, hfbo->res.y,
            bb_offset.x, bb_offset.y, internal_2d_res.x, internal_2d_res.y,
            GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

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
                free(update.shad.frag);
                free(update.shad.vert);
                free(update.shad.geom);
                break;
            }
        }
        else if (i->type == TASK_RENDER_UNIFORM) {
            task_render_uniform uniform = i->uniform;
            string_dispose(&uniform.name);
        }
    }

    vector_task_render_free(&tasks_render);

    for (task_render_draw2d* i = tasks_render_draw2d.begin; i != tasks_render_draw2d.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            string_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw2d_free(&tasks_render_draw2d);

    for (task_render_draw2d* i = tasks_render_draw2d_int.begin; i != tasks_render_draw2d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            string_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw2d_free(&tasks_render_draw2d_int);

    for (task_render_draw3d* i = tasks_render_draw3d.begin; i != tasks_render_draw3d.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            string_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw3d_free(&tasks_render_draw3d);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        for (task_render_uniform* j = i->uniforms.begin; j != i->uniforms.end; j++)
            string_dispose(&j->name);
        vector_task_render_uniform_free(&i->uniforms);
    }
    vector_task_render_draw3d_free(&tasks_render_draw3d_int);

    vector_task_render_draw3d_free(&temp_draw3d_tasks);
    vector_gl_object_free(&temp_draw3d_objects);

    for (hash_bone_matrix* j = vec_bone_mat.begin; j != vec_bone_mat.end; j++)
        bone_matrix_free(&j->data);
    vector_hash_bone_matrix_free(&vec_bone_mat);

    for (hash_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
        gl_object_free(&j->data);
    vector_hash_gl_object_free(&vec_gl_obj);

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

    for (hash_vertex* j = vec_vert.begin; j != vec_vert.end; j++)
        vertex_free(&j->data);
    vector_hash_vertex_free(&vec_vert);

    classes_process_dispose(classes, classes_count);

    camera_dispose(cam);
    dof_dispose(dof);
    radius_dispose(rad);
    intensity_dispose(inten);
    tone_map_dispose(tm);
    glitter_particle_manager_dispose(gpm);

    fbo_render_dispose(rfbo);
    fbo_hdr_dispose(hfbo);
    fbo_dof_dispose(dfbo);
    fbo_pp_dispose(pfbo);

    shader_fbo_free(&cfbs[0]);
    shader_fbo_free(&cfbs[1]);

    for (int32_t i = 0; i < 3; i++)
        shader_fbo_free(&fxaas[i]);

    for (int32_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 5; j++)
            shader_fbo_free(&dfbs[i * 5 + j]);

    for (int32_t i = 0; i < 14; i++)
        shader_fbo_free(&bfbs[i]);

    shader_fbo_free(&tfbs);
    shader_fbo_free(&particle_shader);
    shader_fbo_free(&sprite_shader);
    shader_fbo_free(&grid_shader);

    vertex_free(&default_vertex);
    texture_set_free(&default_texture);
    memset(&default_material, 0, sizeof(material));
    bone_matrix_free(&default_bone_mat);
    shader_model_free(&default_shader);

    glDeleteBuffers(1, &dof_common_ubo);
    glDeleteBuffers(1, &common_data_ubo);
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

static void render_3d(task_render_draw3d_type type) {
    size_t count = 0;
    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++)
        if (i->type == type && !i->translucent)
            count++;

    if (!count)
        return;

    glDrawBuffers(1, fbo_render_attachments);

    for (task_render_draw3d* i = tasks_render_draw3d_int.begin; i != tasks_render_draw3d_int.end; i++) {
        if (i->type != type || i->translucent)
            continue;

        hash h = i->hash;
        for (hash_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (HASH_COMPARE(j->hash, h)) {
                gl_object* obj = &j->data;
                shader_model* shad = obj->shader;
                for (task_render_uniform* k = i->uniforms.begin; k != i->uniforms.end; k++)
                    render_update_uniform(k, shad);

                shader_model_set_mat4(shad, "model", GL_FALSE, i->model);
                shader_model_set_mat3(shad, "model_normal", GL_FALSE, i->model_normal);
                shader_model_set_mat4_array(shad, "uv_mat", 4, GL_FALSE, i->uv_mat);
                shader_model_set_vec4(shad, "color", i->color);
                gl_object_draw(obj);
                break;
            }
    }
    bind_vertex_array(0);
    shader_model_use(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
}

static void render_3d_translucent(task_render_draw3d_type type) {
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
        for (hash_gl_object* j = vec_gl_obj.begin; j != vec_gl_obj.end; j++)
            if (HASH_COMPARE(j->hash, h)) {
                vector_task_render_draw3d_push_back(&temp_draw3d_tasks, i);
                vector_gl_object_push_back(&temp_draw3d_objects, &j->data);
                break;
            }
    }

    ssize_t draw_count = temp_draw3d_tasks.end - temp_draw3d_tasks.begin;
    if (draw_count < 1) {
        vector_task_render_draw3d_clear(&temp_draw3d_tasks);
        vector_gl_object_clear(&temp_draw3d_objects);
        return;
    }

    glDrawBuffers(1, fbo_render_attachments);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    for (ssize_t i = 0; i < draw_count; i++) {
        task_render_draw3d* task = &temp_draw3d_tasks.begin[i];
        gl_object* obj = &temp_draw3d_objects.begin[i];
        shader_model* shad = obj->shader;
        for (task_render_uniform* k = task->uniforms.begin; k != task->uniforms.end; k++)
            render_update_uniform(k, shad);

        shader_model_set_mat4(shad, "model", GL_FALSE, task->model);
        shader_model_set_mat3(shad, "model_normal", GL_FALSE, task->model_normal);
        shader_model_set_mat4_array(shad, "uv_mat", 4, GL_FALSE, task->uv_mat);
        shader_model_set_vec4(shad, "color", task->color);
        gl_object_draw_translucent_first_part(obj);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);

    for (ssize_t i = 0; i < draw_count; i++) {
        task_render_draw3d* task = &temp_draw3d_tasks.begin[i];
        gl_object* obj = &temp_draw3d_objects.begin[i];
        shader_model* shad = obj->shader;
        for (task_render_uniform* k = task->uniforms.begin; k != task->uniforms.end; k++)
            render_update_uniform(k, shad);

        shader_model_set_mat4(shad, "model", GL_FALSE, task->model);
        shader_model_set_mat3(shad, "model_normal", GL_FALSE, task->model_normal);
        shader_model_set_mat4_array(shad, "uv_mat", 4, GL_FALSE, task->uv_mat);
        shader_model_set_vec4(shad, "color", task->color);
        gl_object_draw_translucent_second_part(obj);
    }
    bind_vertex_array(0);
    shader_model_use(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);

    vector_task_render_draw3d_clear(&temp_draw3d_tasks);
    vector_gl_object_clear(&temp_draw3d_objects);
}

static void render_grid_3d() {
    glDrawBuffers(1, fbo_render_attachments);

    glEnable(GL_BLEND);
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

static void render_glitter_3d(int32_t alpha) {
    glDrawBuffers(1, fbo_render_attachments);
    glitter_particle_manager_draw(GPM_VAL, alpha);
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

    internal_2d_res.x = width;
    internal_2d_res.y = height;
    render_get_aspect_correct_res(&internal_2d_res);
    internal_2d_res.x = clamp(internal_2d_res.x, 1, sv_max_texture_size);
    internal_2d_res.y = clamp(internal_2d_res.y, 1, sv_max_texture_size);

    internal_3d_res.x = (int32_t)roundf((float_t)width * scale);
    internal_3d_res.y = (int32_t)roundf((float_t)height * scale);
    render_get_aspect_correct_res(&internal_3d_res);
    internal_3d_res.x = clamp(internal_3d_res.x, 1, sv_max_texture_size);
    internal_3d_res.y = clamp(internal_3d_res.y, 1, sv_max_texture_size);

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x || old_internal_3d_res.y != internal_3d_res.y;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    bool st = false;
    lock_lock(state_lock);
    st = state == RENDER_INITIALIZED;
    lock_unlock(state_lock);

    if (st && change_fb) {
        fbo_render_resize(rfbo, &internal_3d_res);
        fbo_hdr_resize(hfbo, &internal_3d_res);
        fbo_dof_resize(dfbo, &internal_3d_res);
        fbo_pp_resize(pfbo, &internal_3d_res);
    }

    st = false;
    lock_lock(state_lock);
    st = state == RENDER_INITIALIZING || state == RENDER_INITIALIZED;
    lock_unlock(state_lock);
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
    for (hash_gl_object* i = vec_gl_obj.begin; i != vec_gl_obj.end; i++)
        if (HASH_COMPARE(i->hash, object_mesh_hash)) {
            found = true;
            h = i->hash;
            obj = &i->data;
            break;
        }

    if (!found) {
        if (!GPM_VAL->draw_all || !GPM_VAL->draw_all_mesh)
            return false;

        found = false;
        hash dummy_hash = hash_char("Glitter Editor Dummy");
        for (hash_gl_object* i = vec_gl_obj.begin; i != vec_gl_obj.end; i++)
            if (HASH_COMPARE(i->hash, dummy_hash)) {
                found = true;
                h = i->hash;
                obj = &i->data;
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

    vertex* vert = obj->vertex;
    material* mat = obj->material;

    task_render_draw3d task_draw;
    memset(&task_draw, 0, sizeof(task_draw));
    task_draw.hash = h;
    task_draw.type = TASK_RENDER_DRAW3D_FRONT;
    task_draw.translucent = color->w < 1.0f || vert->translucent || mat->translucent;
    task_draw.model = m;
    task_draw.model_normal = mn;
    for (int32_t i = 0; i < 2; i++)
        task_draw.uv_mat[i] = uv_mat[i];
    for (int32_t i = 2; i < 4; i++)
        task_draw.uv_mat[i] = mat4_identity;
    task_draw.color = *color;
    task_draw.uniforms = (vector_task_render_uniform){ 0, 0, 0 };
    vector_task_render_draw3d_push_back(&tasks_render_draw3d, &task_draw);
    return true;
}

static void render_update_uniform(task_render_uniform* uniform, shader_model* s) {
    task_render_uniform uni = *uniform;
    char* name = string_access(&uni.name);
    switch (uni.type) {
    case TASK_RENDER_UNIFORM_BOOL: {
        task_render_uniform_bool uni_data = uni.boolean;
        shader_model_set_bool(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32: {
        task_render_uniform_int32 uni_data = uni.int32;
        shader_model_set_int(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32: {
        task_render_uniform_float32 uni_data = uni.float32;
        shader_model_set_float(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2: {
        task_render_uniform_vec2 uni_data = uni.vec2;
        shader_model_set_vec2(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I: {
        task_render_uniform_vec2i uni_data = uni.vec2i;
        shader_model_set_vec2i(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3: {
        task_render_uniform_vec3 uni_data = uni.vec3;
        shader_model_set_vec3(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I: {
        task_render_uniform_vec3i uni_data = uni.vec3i;
        shader_model_set_vec3i(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4: {
        task_render_uniform_vec4 uni_data = uni.vec4;
        shader_model_set_vec4(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I: {
        task_render_uniform_vec4i uni_data = uni.vec4i;
        shader_model_set_vec4i(s, name, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT3: {
        task_render_uniform_mat3 uni_data = uni.mat3;
        shader_model_set_mat3(s, name, uni_data.transpose, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4: {
        task_render_uniform_mat4 uni_data = uni.mat4;
        shader_model_set_mat4(s, name, uni_data.transpose, uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_INT32_ARRAY: {
        task_render_uniform_int32_array uni_data = uni.int32_array;
        shader_model_set_int_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_FLOAT32_ARRAY: {
        task_render_uniform_float32_array uni_data = uni.float32_array;
        shader_model_set_float_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2_ARRAY: {
        task_render_uniform_vec2_array uni_data = uni.vec2_array;
        shader_model_set_vec2_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC2I_ARRAY: {
        task_render_uniform_vec2i_array uni_data = uni.vec2i_array;
        shader_model_set_vec2i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3_ARRAY: {
        task_render_uniform_vec3_array uni_data = uni.vec3_array;
        shader_model_set_vec3_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC3I_ARRAY: {
        task_render_uniform_vec3i_array uni_data = uni.vec3i_array;
        shader_model_set_vec3i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4_ARRAY: {
        task_render_uniform_vec4_array uni_data = uni.vec4_array;
        shader_model_set_vec4_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_VEC4I_ARRAY: {
        task_render_uniform_vec4i_array uni_data = uni.vec4i_array;
        shader_model_set_vec4i_array(s, name, (GLsizei)uni_data.count, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT3_ARRAY: {
        task_render_uniform_mat3_array uni_data = uni.mat3_array;
        shader_model_set_mat3_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    case TASK_RENDER_UNIFORM_MAT4_ARRAY: {
        task_render_uniform_mat4_array uni_data = uni.mat4_array;
        shader_model_set_mat4_array(s, name, (GLsizei)uni_data.count,
            uni_data.transpose, uni_data.value);
        free(uni_data.value);
    } break;
    }
}
