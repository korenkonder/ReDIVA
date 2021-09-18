/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.h"
#include "../CRE/camera.h"
#include "../CRE/dof.h"
#include "../CRE/gl_state.h"
#include "../CRE/post_process.h"
#include "../CRE/random.h"
#include "../CRE/shader.h"
#include "../CRE/shader_aft.h"
#include "../CRE/static_var.h"
#include "../KKdLib/farc.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <cimgui_impl.h>
#include "../CRE/fbo_hdr.h"
#include "../CRE/fbo_pp.h"
#include "../CRE/fbo_render.h"
#include "../CRE/fbo_helper.h"
#include "../CRE/lock.h"
#include "../CRE/shader_glsl.h"
#include "../CRE/timer.h"
#include "../CRE/Glitter/particle_manager.h"
#include "../CRE/post_process/dof.h"
#include "../CRE/fbo_hdr.h"

#if defined(DEBUG)
#define OPENGL_DEBUG 0
#endif

fbo_render* rfbo;
fbo_hdr* hfbo;
fbo_pp* pfbo;

post_process_dof pp_dof;

shader_set_data shaders_aft;

static shader_glsl bfbs[14];
static shader_glsl cfbs[2];
static shader_glsl dfbs[9];
static shader_glsl fxaas[3];
static shader_glsl ffbs;
shader_glsl grid_shader;

timer render_timer;

#define grid_size 50.0f
#define grid_spacing 0.5f
#define grid_vertex_count ((size_t)((grid_size * 2.0f) / grid_spacing) + 1) * 4

static int32_t fb_vao, fb_vbo;
static int32_t grid_vao, grid_vbo;
static int32_t common_data_ubo = 0;

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
static void render_2d();
static void render_3d();
static void render_3d_translucent();
static void render_grid_3d();
static void render_glitter_3d(alpha_pass_type alpha);

static void render_get_aspect_correct_2d_res(vec2i* res);
static void render_get_aspect_correct_3d_res(vec2i* res);

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(bool change_fb);

static void render_imgui_context_menu(classes_struct* classes, const size_t classes_count);
static bool render_glitter_mesh_update(glitter_render_element* element,
    glitter_particle_mesh* mesh_data, vec4* color, mat4* mat, mat4* uv_mat);

#if defined (DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);
#endif

extern bool close;
lock render_lock;
HWND window_handle;
GLFWwindow* window;
ImGuiContext* imgui_context;
lock imgui_context_lock;
bool global_context_menu;

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(&render_timer, 60.0);

    state = RENDER_UNINITIALIZED;

    lock_init(&render_lock);
    if (!lock_check_init(&render_lock))
        goto End;

    render_init_struct* ris = (render_init_struct*)arg;
    window_handle = 0;

#pragma region GLFW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#if defined (DEBUG) && OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

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
#if defined(DEBUG)
#if defined(CLOUD_DEV)
    glfw_titlelabel = "CLOUDDev Debug";
#else
    glfw_titlelabel = "CLOUD Debug";
#endif
#else
#if defined(CLOUD_DEV)
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -2;
    }
#pragma endregion

#if defined(DEBUG) && OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(render_debug_output, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
#endif

    glEnable(GL_MULTISAMPLE);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &sv_max_texture_buffer_size);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sv_max_texture_size);
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &sv_max_texture_max_anisotropy);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    lock_lock(&state_lock);
    state = RENDER_INITIALIZING;
    lock_unlock(&state_lock);

    lock_lock(&render_lock);
    render_load();
    lock_unlock(&render_lock);

    lock_lock(&state_lock);
    state = RENDER_INITIALIZED;
    lock_unlock(&state_lock);

#pragma region GL Init
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);

    gl_state_disable_blend();
    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();
    gl_state_disable_stencil_test();
#pragma endregion

    while (!close) {
        timer_start_of_cycle(&render_timer);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();
        lock_lock(&render_lock);
        render_update();
        lock_unlock(&render_lock);
        render_draw();
        glfwSwapBuffers(window);
        close |= glfwWindowShouldClose(window);
        timer_end_of_cycle(&render_timer);
    }

    lock_lock(&state_lock);
    state = RENDER_DISPOSING;
    lock_unlock(&state_lock);

    lock_lock(&render_lock);
    render_dispose();
    lock_unlock(&render_lock);

    lock_lock(&state_lock);
    state = RENDER_DISPOSED;
    lock_unlock(&state_lock);

#pragma region GLFW Dispose
    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion
    lock_free(&render_lock);

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
    gl_state_get();

    cam = camera_init();
    dof = dof_init();
    rad = radius_init();
    inten = intensity_init();
    tm = tone_map_init();
    gpm = glitter_particle_manager_init();

    dof_initialize(dof, 0, 0);
    radius_initialize(rad, (vec3[]) { 2.0f, 2.0f, 2.0f });
    intensity_initialize(inten, (vec3[]) { 1.0f, 1.0f, 1.0f });
    tone_map_initialize(tm, 2.0f, true, 1.0f, 1, 1.0f, (vec3[]) { 0.0f, 0.0f, 0.0f},
        0.0f, 0, (vec3[]) { 0.0f, 0.0f, 0.0f }, (vec3[]) { 1.0f, 1.0f, 1.0f }, 0);

    camera_initialize(cam, aspect, 70.0);
    camera_reset(cam);
    //camera_set_position(cam, &(vec3){ 1.35542f, 1.41634f, 1.27852f });
    //camera_rotate(cam, &(vec2d){ -45.0, -32.5 });
    //camera_set_position(cam, &(vec3){ -6.67555f, 4.68882f, -3.67537f });
    //camera_rotate(cam, &(vec2d){ 136.5, -20.5 });
    camera_set_position(cam, &(vec3){ 0.0f, 1.0f, 3.45f });

    glGenBuffers(1, &common_data_ubo);

    gl_state_bind_uniform_buffer(common_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, COMMON_DATA_SIZE, 0, GL_STREAM_DRAW);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_data_ubo, 0, COMMON_DATA_SIZE);

#pragma region Load Shaders
    char temp[0x80];
    memset(temp, 0, sizeof(temp));

    hfbo = fbo_hdr_init();
    rfbo = fbo_render_init();
    pfbo = fbo_pp_init();

    post_process_dof_init(&pp_dof);

    const float_t verts_quad[] = {
        -1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -3.0f,  0.0f, -1.0f,
         3.0f,  1.0f,  2.0f,  1.0f,
    };

    glGenBuffers(1, &fb_vbo);
    glGenVertexArrays(1, &fb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fb_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, GL_STATIC_DRAW);

    gl_state_bind_vertex_array(fb_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);  // Pos
    glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color0
    glVertexAttrib4f(4, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord0
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord1
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord2
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord3
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord4
    glEnableVertexAttribArray(13);
    glVertexAttribPointer(13, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord5
    glEnableVertexAttribArray(14);
    glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord6
    glEnableVertexAttribArray(15);
    glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, 16, (void*)8); // TexCoord7
    gl_state_bind_vertex_array(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    farc* f = farc_init();
    farc_read(f, "rom\\aft_shaders.farc", true, false);
    shader_aft_load(&shaders_aft, f, false);
    farc_dispose(f);

    f = farc_init();
    farc_read(f, "rom\\core_shaders.farc", true, false);

    shader_glsl_param param;

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Color FB Color";
    param.vert = "rfb";
    param.frag = "cfbc";
    shader_glsl_load(&cfbs[0], f, &param);

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Color FB Depth";
    param.vert = "rfb";
    param.frag = "cfbd";
    shader_glsl_load(&cfbs[1], f, &param);

    for (int32_t i = 3; i < 6; i++) {
        memset(&param, 0, sizeof(shader_glsl_param));
        param.name = "FXAA";
        param.vert = "rfb";
        param.frag = "fxaa";
        snprintf(temp, sizeof(temp), "FXAA_PRESET %d", i);
        param.param[0] = temp;
        shader_glsl_load(&fxaas[i - 3], f, &param);
    }

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Final FB";
    param.vert = "rfb";
    param.frag = "rfb";
    shader_glsl_load(&ffbs, f, &param);

    char* post_process_vert_shaders[] = {
        "rfb",
        "rfb",
        "rfb",
        "exposure1",
        "rfb",
        "rfb",
        "rfb",
        "rfb",
        "rfb",
        "rfb",
        "blur5",
        "rfb",
        "exposure2",
        "exposure3",
    };

    char* post_process_frag_shaders[] = {
        "blur1",
        "blur2",
        "blur3",
        "exposure1",
        "blur4",
        "blur4",
        "blur4",
        "blur4",
        "blur4",
        "blur4",
        "blur5",
        "blur6",
        "exposure2",
        "exposure3",
    };

    for (int32_t i = 0; i < 14; i++) {
        memset(&param, 0, sizeof(shader_glsl_param));
        param.param[0] = temp;
        param.name = "Post Process FB";
        param.vert = post_process_vert_shaders[i];
        param.frag = post_process_frag_shaders[i];
        shader_glsl_load(&bfbs[i], f, &param);
    }

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Grid";
    param.vert = "grid";
    param.frag = "grid";
    shader_glsl_load(&grid_shader, f, &param);
    farc_dispose(f);

    shader_glsl_set_int(&cfbs[0], "g_color", 0);
    shader_glsl_set_int(&cfbs[1], "g_color", 0);
    shader_glsl_set_int(&cfbs[1], "g_depth", 1);

    for (int32_t i = 0; i < 3; i++)
        shader_glsl_set_int(&fxaas[i], "g_color", 0);

   shader_glsl_set_int(&ffbs, "g_color", 0);

    for (int32_t i = 0; i < 2; i++)
        shader_glsl_set_int(&bfbs[i], "g_color", 0);

    shader_glsl_set_int(&bfbs[2], "g_color", 0);

    shader_glsl_set_int(&bfbs[3], "g_color", 0);

    for (int32_t i = 4; i < 11; i++)
        shader_glsl_set_int(&bfbs[i], "g_color", 0);

    shader_glsl_set_int(&bfbs[11], "g_bloom_full", 0);
    shader_glsl_set_int(&bfbs[11], "g_bloom_half", 1);
    shader_glsl_set_int(&bfbs[11], "g_bloom_quat", 2);
    shader_glsl_set_int(&bfbs[11], "g_bloom_eight", 3);
    shader_glsl_set_int(&bfbs[12], "g_luma", 0);
    shader_glsl_set_int(&bfbs[13], "g_exp_hist", 0);


    render_resize_fb(false);

    fbo_render_initialize(rfbo, &internal_3d_res, fb_vao, &cfbs[0], &cfbs[1]);
    fbo_hdr_initialize(hfbo, &internal_3d_res, fb_vao, fxaas);
    fbo_pp_initialize(pfbo, &internal_3d_res, fb_vao, bfbs);

    post_process_dof_init_fbo(&pp_dof, internal_3d_res.x, internal_3d_res.y);

    render_resize_fb(true);
#pragma endregion

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
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&z_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&z_color.z;

        grid_verts[v++] = grid_size;
        grid_verts[v++] = x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&z_color.x;
        *(vec2h*)&grid_verts[v++] = *(vec2h*)&z_color.z;
    }

    glGenBuffers(1, &grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float_t)
        * grid_vertex_count * 4, grid_verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &grid_vao);
    gl_state_bind_vertex_array(grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        sizeof(float_t) * 4, (void*)0); // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_HALF_FLOAT, GL_FALSE,
        sizeof(float_t) * 4, (void*)(sizeof(float_t) * 2)); // Color
    gl_state_bind_vertex_array(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    free(grid_verts);

    imgui_context = igCreateContext(0);
    lock_init(&imgui_context_lock);

    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    ImGuiIO* io = igGetIO();
    io->IniFilename = 0;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    lock_unlock(&imgui_context_lock);

    igStyleColorsDark(0);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    back3d_color = (vec3){ 0.74117647f, 0.74117647f, 0.74117647f };
    set_clear_color = true;

    shader_env_vert_set_ptr(&shaders_aft, 3, (vec4*)&vec4_identity);
    shader_env_vert_set_ptr(&shaders_aft, 4, (vec4*)&vec4_null);
    classes_process_init(classes, classes_count);
}

extern vec2d input_move;
extern vec2d input_rotate;
extern double_t input_roll;
extern bool input_reset;

static void render_update() {
    global_context_menu = true;
    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    classes_process_imgui(classes, classes_count);
    lock_unlock(&imgui_context_lock);

    if (sv_fxaa_changed)
        sv_fxaa_changed = false;

    if (sv_fxaa_preset_changed)
        sv_fxaa_preset_changed = false;

    if (old_scale != scale)
        render_resize_fb(true);
    old_scale = scale;

    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    if (global_context_menu && igIsMouseReleased(ImGuiMouseButton_Right)
        && !igIsItemHovered(0) && imgui_context->OpenPopupStack.Size <= 0)
        igOpenPopup("Classes init context menu", 0);

    if (igBeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count);
        igEndPopup();
    }
    lock_unlock(&imgui_context_lock);

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

    glitter_render_mesh_update_func = render_glitter_mesh_update;
    classes_process_render(classes, classes_count);

    GPM_VAL->updated = false;

    if (memcmp(&GPM_VAL->cam_projection, &cam->projection, sizeof(GPM_VAL->cam_projection)))
        GPM_VAL->cam_projection = cam->projection;

    if (memcmp(&GPM_VAL->cam_view, &cam->view, sizeof(GPM_VAL->cam_view))) {
        GPM_VAL->cam_view = cam->view;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_inv_view, &cam->inv_view, sizeof(GPM_VAL->cam_inv_view))) {
        GPM_VAL->cam_inv_view = cam->inv_view;
        GPM_VAL->updated = true;
    }

    if (memcmp(&GPM_VAL->cam_inv_view_mat3, &cam->inv_view_mat3,
        sizeof(GPM_VAL->cam_inv_view_mat3))) {
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

    glitter_render_mesh_update_func = render_glitter_mesh_update;
    glitter_particle_manager_calc_draw(GPM_VAL);

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
            double fov = cam.FOVIsHorizontal ? 2
              * Math.Atan(1 / cam.Aspect * Math.Tan(cam.FOV / 2)) : cam.FOV;
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

        vec3 rgb = inten->val;
        vec3* radius = rad->val;

        float_t intensity_scale = 1.0f;
        vec3_mult_scalar(rgb, intensity_scale, rgb);

        vec3 v[GAUSSIAN_KERNEL_SIZE];
        for (int32_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
            vec3_mult(radius[i], rgb, v[i]);

        for (int32_t i = 4; i < 10; i++)
            shader_glsl_set_vec3_array(&bfbs[i], "gaussian_kernel", GAUSSIAN_KERNEL_SIZE, v);
    }

    if (tm->update_tex) {
        tm->update_tex = false;
        fbo_pp_tone_map_set(pfbo, tm->tex, 16 * TONE_MAP_SAT_GAMMA_SAMPLES);
    }

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

    gl_state_bind_uniform_buffer(common_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, COMMON_DATA_SIZE, &common_data);
    gl_state_bind_uniform_buffer(0);
}

static void render_draw() {
    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat depth_clear = 1.0f;
    static const GLint stencil_clear = 0;

    gl_state_bind_framebuffer(0);
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_set_stencil_mask(0xFF);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearDepthf(depth_clear);
    gl_state_set_stencil_mask(0x00);
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_bind_uniform_buffer_base(0, common_data_ubo);

    for (int32_t i = 0; i < 32; i++) {
        if (!gl_state_check_texture_binding_2d(i) && !gl_state_check_texture_binding_cube_map(i))
            continue;

        gl_state_active_texture(i);
        gl_state_bind_texture_2d(0);
        gl_state_bind_texture_cube_map(0);
        gl_state_bind_sampler(i, 0);
    }

    if (back_2d || front_3d || glitter_3d) {
        glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);

        if (back_2d) {
            gl_state_bind_framebuffer(hfbo->back_2d.fbos[0]);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            render_2d();
        }

        if (front_3d || glitter_3d) {
            gl_state_bind_framebuffer(rfbo->tex.fbos[0]);
            glDrawBuffers(1, fbo_render_attachments);
            gl_state_set_depth_mask(GL_TRUE);
            gl_state_set_stencil_mask(0xFF);
            glClearBufferfi(GL_DEPTH_STENCIL, 0, depth_clear, stencil_clear);
            gl_state_set_stencil_mask(0x00);
            gl_state_set_depth_mask(GL_FALSE);

            if (set_clear_color) {
                vec4 color;
                *(vec3*)&color = back3d_color;
                color.w = 1.0f;
                glClearBufferfv(GL_COLOR, 0, (GLfloat*)&color);
            }

            if (grid_3d)
                render_grid_3d();

            if (front_3d)
                render_3d();

            if (glitter_3d)
                render_glitter_3d(ALPHA_PASS_OPAQUE);

            if (glitter_3d) {
                gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
                render_glitter_3d(ALPHA_PASS_TRANSPARENT);
                gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }

            if (glitter_3d) {
                gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
                render_glitter_3d(ALPHA_PASS_TRANSLUCENT);
                gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }

            if (front_3d)
                render_3d_translucent();

            gl_state_bind_framebuffer(hfbo->color.fbos[0]);
            gl_state_set_depth_mask(GL_TRUE);
            glClearBufferfv(GL_COLOR, 0, color_clear);
            glClearBufferfv(GL_DEPTH, 0, &depth_clear);
            gl_state_set_depth_mask(GL_FALSE);

            fbo_render_draw(rfbo, true);
            post_process_apply_dof(&pp_dof, &hfbo->color, dof);
        }

        if (enable_post_process)
            fbo_pp_draw(pfbo, tm, hfbo->color.color_texture, 0, hfbo->back_2d.color_texture,
                hfbo, fbo_hdr_set_fbo_begin, fbo_hdr_set_fbo_end);

        if (sv_fxaa)
            fbo_hdr_draw_fxaa(hfbo, sv_fxaa_preset);

        gl_state_bind_framebuffer(hfbo->color.fbos[0]);
        gl_state_bind_uniform_buffer_base(0, common_data_ubo);
        classes_process_draw(classes, classes_count);

        vec2i bb_offset;
        bb_offset.x = (width - internal_2d_res.x) / 2;
        bb_offset.y = (height - internal_2d_res.y) / 2;

        glViewport(bb_offset.x, bb_offset.y, internal_2d_res.x, internal_2d_res.y);
        gl_state_bind_framebuffer(0);
        shader_glsl_use(&ffbs);
        gl_state_active_bind_texture_2d(0, hfbo->color.color_texture->texture);
        gl_state_bind_vertex_array(fb_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    }

    if (front_2d || ui) {
        glViewport((width - internal_2d_res.x) / 2, (height - internal_2d_res.y) / 2,
            internal_2d_res.x, internal_2d_res.y);
        if (front_2d)
            render_2d();
        if (ui)
            render_2d();
        glViewport(0, 0, width, height);
    }

    if (imgui)
        render_imgui();
}

static void render_dispose() {
    classes_process_dispose(classes, classes_count);

    camera_dispose(cam);
    dof_dispose(dof);
    radius_dispose(rad);
    intensity_dispose(inten);
    tone_map_dispose(tm);
    glitter_particle_manager_dispose(gpm);

    fbo_render_dispose(rfbo);
    fbo_hdr_dispose(hfbo);
    fbo_pp_dispose(pfbo);

    post_process_dof_free(&pp_dof);

    shader_free(&shaders_aft);

    shader_glsl_free(&cfbs[0]);
    shader_glsl_free(&cfbs[1]);

    for (int32_t i = 0; i < 3; i++)
        shader_glsl_free(&fxaas[i]);

    shader_glsl_free(&ffbs);

    shader_glsl_free(&dfbs[0]);

    for (int32_t i = 0; i < 2; i++)
        for (int32_t j = 0; j < 4; j++)
            shader_glsl_free(&dfbs[1 + i * 4 + j]);

    for (int32_t i = 0; i < 14; i++)
        shader_glsl_free(&bfbs[i]);

    shader_glsl_free(&grid_shader);

    glDeleteBuffers(1, &common_data_ubo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteVertexArrays(1, &grid_vao);
    glDeleteBuffers(1, &fb_vbo);
    glDeleteVertexArrays(1, &fb_vao);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(imgui_context);
    lock_free(&imgui_context_lock);
}

static void render_imgui() {
    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    lock_unlock(&imgui_context_lock);
}

static void render_2d() {
}

static void render_3d() {
}

static void render_3d_translucent() {
}

static void render_grid_3d() {
    glDrawBuffers(1, fbo_render_attachments);

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_set_blend_equation(GL_FUNC_ADD);
    gl_state_enable_depth_test();
    gl_state_set_depth_func(GL_LESS);
    gl_state_set_depth_mask(GL_TRUE);

    shader_glsl_use(&grid_shader);
    gl_state_bind_vertex_array(grid_vao);
    glDrawArrays(GL_LINES, 0, grid_vertex_count);
    shader_glsl_use(0);

    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_blend();
}

static void render_glitter_3d(alpha_pass_type alpha) {
    glitter_particle_manager_draw(GPM_VAL, alpha);
}

inline static void render_get_aspect_correct_2d_res(vec2i* res) {
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

inline static void render_get_aspect_correct_3d_res(vec2i* res) {
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

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths) {
    if (!count || !paths)
        return;

    classes_process_drop(classes, classes_count, count, paths);
    glfwFocusWindow(window);
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
    render_get_aspect_correct_2d_res(&internal_2d_res);
    internal_2d_res.x = clamp(internal_2d_res.x, 1, sv_max_texture_size);
    internal_2d_res.y = clamp(internal_2d_res.y, 1, sv_max_texture_size);

    internal_3d_res.x = width;
    internal_3d_res.y = height;
    render_get_aspect_correct_3d_res(&internal_3d_res);
    internal_3d_res.x = (int32_t)roundf((float_t)internal_3d_res.x * scale);
    internal_3d_res.y = (int32_t)roundf((float_t)internal_3d_res.y * scale);
    internal_3d_res.x = clamp(internal_3d_res.x, 1, sv_max_texture_size);
    internal_3d_res.y = clamp(internal_3d_res.y, 1, sv_max_texture_size);

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x
        || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x
        || old_internal_3d_res.y != internal_3d_res.y;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    bool st = false;
    lock_lock(&state_lock);
    st = state == RENDER_INITIALIZED;
    lock_unlock(&state_lock);

    if (st && change_fb) {
        fbo_render_resize(rfbo, &internal_3d_res);
        fbo_hdr_resize(hfbo, &internal_3d_res);
        fbo_pp_resize(pfbo, &internal_3d_res);

        post_process_dof_init_fbo(&pp_dof, internal_3d_res.x, internal_3d_res.y);
    }

    st = false;
    lock_lock(&state_lock);
    st = state == RENDER_INITIALIZING || state == RENDER_INITIALIZED;
    lock_unlock(&state_lock);
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

static bool render_glitter_mesh_update(glitter_render_element* element,
    glitter_particle_mesh* mesh_data, vec4* color, mat4* model, mat4* uv_mat) {
    return true;
}

#if defined (DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf("########################################\n");
    printf("Debug message (%d): %s\n", id, message);

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf("Source: Other");
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error\n");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour\n");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour\n");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability\n");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance\n");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker\n");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group\n");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group\n");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other\n");
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high\n");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium\n");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low\n");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification\n");
        break;
    }
    printf("########################################\n\n");
}
#endif