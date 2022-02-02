/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <cimgui_impl.h>
#include "../CRE/Glitter/file_reader.h"
#include "../CRE/Glitter/particle_manager.h"
#include "../CRE/camera.h"
#include "../CRE/data.h"
#include "../CRE/draw_pass.h"
#include "../CRE/fbo.h"
#include "../CRE/gl_state.h"
#include "../CRE/light_param.h"
#include "../CRE/lock.h"
#include "../CRE/object.h"
#include "../CRE/random.h"
#include "../CRE/rob.h"
#include "../CRE/shader.h"
#include "../CRE/shader_ft.h"
#include "../CRE/shader_glsl.h"
#include "../CRE/stage.h"
#include "../CRE/static_var.h"
#include "../CRE/texture.h"
#include "../CRE/timer.h"
#include "../CRE/post_process.h"
#include "../KKdLib/io/path.h"
#include "../KKdLib/dsc.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/pvpp.h"
#include "../KKdLib/pvsr.h"
#include"classes/imgui_helper.h"
#include <timeapi.h>

#if defined(DEBUG)
#define OPENGL_DEBUG 0
#endif

shader_glsl cube_line_shader;
shader_glsl cube_line_point_shader;
shader_glsl grid_shader;

timer render_timer;

#define grid_size 50.0f
#define grid_spacing 1.0f
const size_t grid_vertex_count = ((size_t)(grid_size / grid_spacing) * 2 + 1) * 4;

GLuint cube_line_vao;
GLuint cube_line_vbo;
GLuint grid_vbo;
static GLuint common_data_ubo = 0;
stage stage_stgtst;
stage stage_test_data;

#define COMMON_DATA_SIZE (int32_t)(sizeof(vec4) + sizeof(mat4) * 3 + sizeof(vec4))

bool draw_imgui   = true;
bool draw_grid_3d = false;

const double_t render_scale_table[] = {
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
static int32_t old_width;
static int32_t old_height;
int32_t width;
int32_t height;

static const double_t aspect = 16.0 / 9.0;
vec3 back3d_color;
bool set_clear_color;

bool light_chara_ambient;
vec4 npr_spec_color;

static render_context* render_load();
static bool render_load_shaders(void* data, char* path, char* file, uint32_t hash);
static void render_update(render_context* rctx);
static void render_draw(render_context* rctx);
static void render_dispose(render_context* rctx);

static void render_imgui(render_context* rctx);

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(render_context* rctx, bool change_fb);

static void render_imgui_context_menu(classes_struct* classes,
    const size_t classes_count, render_context* rctx);
static void render_shaders_load();
static void render_shaders_free();

#if defined (DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);
#endif

static void x_pv_player_init();
static void x_pv_player_load(int32_t pv_id, int32_t stage_id);
static void x_pv_player_update();
static void x_pv_player_free();

extern bool close;
bool reload_render;
lock render_lock;
HWND window_handle;
GLFWwindow* window;
ImGuiContext* imgui_context;
lock imgui_context_lock;
bool global_context_menu;
extern size_t frame_counter;
wind* wind_ptr;
object_database* obj_db_ptr;
int32_t stage_index = -1;

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timeBeginPeriod(1);
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
    glfwSetWindowSizeLimits(window, 896, 504, GLFW_DONT_CARE, GLFW_DONT_CARE);

    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
    width = window_rect.right;
    height = window_rect.bottom;
    width = 1280;
    height = 720;

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

    lock_init(&pv_lock);

    lock_lock(&state_lock);
    state = RENDER_INITIALIZING;
    lock_unlock(&state_lock);

    do {
        close = false;
        reload_render = false;

        render_context* rctx = 0;
        lock_lock(&render_lock);
        rctx = render_load();
        lock_unlock(&render_lock);

        lock_lock(&state_lock);
        frame_counter = 0;
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

        timer_reset(&render_timer);
        while (!close && !reload_render) {
            timer_start_of_cycle(&render_timer);
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            igNewFrame();
            lock_lock(&render_lock);
            render_update(rctx);
            lock_unlock(&render_lock);
            render_draw(rctx);
            glfwSwapBuffers(window);
            close |= glfwWindowShouldClose(window);
            frame_counter++;
            timer_end_of_cycle(&render_timer);
        }

        lock_lock(&state_lock);
        state = RENDER_DISPOSING;
        lock_unlock(&state_lock);

        lock_lock(&render_lock);
        render_dispose(rctx);
        lock_unlock(&render_lock);
    } while (reload_render);

    lock_lock(&state_lock);
    state = RENDER_DISPOSED;
    lock_unlock(&state_lock);

    lock_free(&pv_lock);

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

float_t rob_frame = 0.0f;

static render_context* render_load() {
    object_storage_init();
    texture_storage_init();

    render_context* rctx = render_context_init();

    gl_state_get();
    render_texture_data_init();

    auth_3d_data_init();
    light_param_storage_init();
    motion_storage_init();

    GPM_VAL = glitter_particle_manager_init();
    GPM_VAL->rctx = rctx;

    data_struct_init();
    data_struct_load("CLOUD_data.txt");

    GPM_VAL->bone_data = &data_list[DATA_AFT].data_ft.bone_data;

    glGenBuffers(1, &common_data_ubo);

    gl_state_bind_uniform_buffer(common_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, COMMON_DATA_SIZE, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_data_ubo, 0, COMMON_DATA_SIZE);
    gl_state_bind_uniform_buffer(0);

    uniform_value[U16] = 1;

    render_shaders_load();

    const char* cube_line_vert_shader =
        "#version 430 core\n"
        "layout(location = 0) in vec4 a_position;\n"
        "\n"
        "out VertexData {\n"
        "    vec4 color;\n"
        "} result;\n"
        "\n"
        "uniform mat4 vp;\n"
        "uniform vec3 trans[4];\n"
        "uniform float scale;\n"
        "uniform vec4 color;\n"
        "\n"
        "void main() {\n"
        "    vec4 pos;\n"
        "    pos.xyz = trans[gl_VertexID];\n"
        "    pos.w = 1.0;\n"
        "    gl_Position = vp * pos;\n"
        "    result.color = color;\n"
        "}\n";

    const char* cube_line_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "in VertexData {\n"
        "    vec4 color;\n"
        "} frg;\n"
        "\n"
        "void main() {\n"
        "    result = frg.color;\n"
        "}\n";
    
    const char* cube_line_point_vert_shader =
        "#version 430 core\n"
        "layout(location = 0) in vec4 a_position;\n"
        "\n"
        "out VertexData {\n"
        "    vec4 color;\n"
        "} result;\n"
        "\n"
        "uniform mat4 vp;\n"
        "uniform mat4 mat;\n"
        "uniform vec4 color;\n"
        "\n"
        "void main() {\n"
        "    vec4 pos = mat * a_position;\n"
        "    gl_Position = vp * pos;\n"
        "    result.color = color;\n"
        "}\n";

    const char* cube_line_point_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "in VertexData {\n"
        "    vec4 color;\n"
        "} frg;\n"
        "\n"
        "void main() {\n"
        "    result = frg.color;\n"
        "}\n";

    const char* grid_vert_shader =
        "#version 430 core\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in int a_color_index;\n"
        "\n"
        "out VertexData {\n"
        "    vec4 color;\n"
        "} result;\n"
        "\n"
        "uniform mat4 vp;\n"
        "\n"
        "vec4 colors[] = {\n"
        "    vec4(1.0, 0.0, 0.0, 1.0),\n"
        "    vec4(0.0, 1.0, 0.0, 1.0),\n"
        "    vec4(0.2, 0.2, 0.2, 1.0),\n"
        "};\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vp * a_position.xzyw;\n"
        "    result.color = colors[a_color_index];\n"
        "}\n";

    const char* grid_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "in VertexData {\n"
        "    vec4 color;\n"
        "} frg;\n"
        "\n"
        "void main() {\n"
        "    result = frg.color;\n"
        "}\n";

    const char* fbo_render_vert_shader =
        "#version 430 core\n"
        "void main() {\n"
        "    gl_Position.x = -1.0 + float(gl_VertexID / 2) * 4.0;\n"
        "    gl_Position.y = 1.0 - float(gl_VertexID % 2) * 4.0;\n"
        "    gl_Position.z = 0.0;\n"
        "    gl_Position.w = 1.0;\n"
        "}\n";

    const char* fbo_render_color_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "layout(binding = 0) uniform sampler2D g_color;\n"
        "\n"
        "void main() {\n"
        "    result = texelFetch(g_color, ivec2(gl_FragCoord.xy), 0);\n"
        "}\n";

    const char* fbo_render_depth_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "layout(binding = 0) uniform sampler2D g_color;\n"
        "layout(binding = 1) uniform sampler2D g_depth;\n"
        "\n"
        "void main() {\n"
        "    result = texelFetch(g_color, ivec2(gl_FragCoord.xy), 0);\n"
        "    gl_FragDepth = texelFetch(g_depth, ivec2(gl_FragCoord.xy), 0).r;\n"
        "}\n";

    shader_glsl_param param;
    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Cube Line";
    shader_glsl_load_string(&cube_line_shader,
        (char*)cube_line_vert_shader, (char*)cube_line_frag_shader, 0, &param);

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Cube Line Point";
    shader_glsl_load_string(&cube_line_point_shader,
        (char*)cube_line_point_vert_shader, (char*)cube_line_point_frag_shader, 0, &param);

    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Grid";
    shader_glsl_load_string(&grid_shader,
        (char*)grid_vert_shader, (char*)grid_frag_shader, 0, &param);

    render_resize_fb(rctx, false);

    post_process_init_fbo(&rctx->post_process, internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y, width, height);

    render_resize_fb(rctx, true);

    rob_chara_array_init();
    rob_chara_pv_data_array_init();

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    rctx->data = aft_data;

    stage_init(&stage_stgtst);
    stage_load(&stage_stgtst, aft_data, aft_auth_3d_db,
        aft_obj_db, aft_tex_db, aft_stage_data, "STGTST", rctx);
    stage_set(&stage_stgtst, rctx);

    stage_init(&stage_test_data);
    stage_test_data.set_id = -1;
    stage_test_data.stage_set_id = -1;

    object_set_info* set_info;
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM000");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM001");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM301");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM500");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM181");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM481");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM681");
    object_set_load_db_entry(&set_info, aft_data, aft_obj_db, "MIKITM981");

    motion_set_load_motion(motion_database_get_motion_set_id(aft_mot_db, "CMN"), 0, aft_mot_db);
    motion_set_load_motion(motion_database_get_motion_set_id(aft_mot_db, "PV824"), 0, aft_mot_db);

    rob_chara_pv_data pv_data;
    rob_chara_pv_data_init(&pv_data);
    pv_data.field_0 = 2;
    int32_t chara_id = rob_chara_array_set_pv_data(CHARA_MIKU, &pv_data, 0, false);
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
        rob_chara_reset_data(&rob_chara_array[chara_id], &rob_chara_array[chara_id].pv_data, aft_bone_data, aft_mot_db);
        rob_chara_reset(&rob_chara_array[chara_id], aft_bone_data, aft_data, aft_obj_db);
        rob_chara_load_motion(&rob_chara_array[chara_id],
            motion_database_get_motion_id(aft_mot_db, "PV824_STF_P1_00"), 2, aft_bone_data, aft_mot_db);
        rob_chara_set_visibility(&rob_chara_array[chara_id], true);
        //rob_chara_set_frame(&rob_chara_array[chara_id], 1000.0f);
    }

    float_t cube_line_verts[] = {
        -1.0,  1.0,
         1.0,  1.0,
        -1.0, -1.0,
         1.0, -1.0,
    };

    glGenVertexArrays(1, &cube_line_vao);
    glGenBuffers(1, &cube_line_vbo);
    gl_state_bind_vertex_array(cube_line_vao);
    gl_state_bind_array_buffer(cube_line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_line_verts), (void*)cube_line_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 2, 0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    float_t* grid_verts = force_malloc(sizeof(float_t) * 3 * grid_vertex_count);

    size_t v = 0;
    for (float_t x = -grid_size; x <= grid_size; x += grid_spacing) {
        int32_t x_color_index;
        int32_t z_color_index;
        if (x == 0) {
            x_color_index = 0;
            z_color_index = 1;
        }
        else {
            x_color_index = 2;
            z_color_index = 2;
        }

        grid_verts[v++] = x;
        grid_verts[v++] = -grid_size;
        *(int32_t*)&grid_verts[v++] = x_color_index;

        grid_verts[v++] = x;
        grid_verts[v++] = grid_size;
        *(int32_t*)&grid_verts[v++] = x_color_index;

        grid_verts[v++] = -grid_size;
        grid_verts[v++] = x;
        *(int32_t*)&grid_verts[v++] = z_color_index;

        grid_verts[v++] = grid_size;
        grid_verts[v++] = x;
        *(int32_t*)&grid_verts[v++] = z_color_index;
    }

    glGenBuffers(1, &grid_vbo);
    gl_state_bind_array_buffer(grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * 3
        * grid_vertex_count, grid_verts, GL_STATIC_DRAW);
    gl_state_bind_array_buffer(0);

    free(grid_verts);

    x_pv_player_init();
    x_pv_player_load(826, 26);

    camera* cam = rctx->camera;

    camera_initialize(cam, aspect, internal_3d_res.x, internal_3d_res.y);
    //camera_set_position(cam, &((vec3){ 1.35542f, 1.41634f, 1.27852f }));
    //camera_rotate(cam, &((vec2d){ -45.0, -32.5 }));
    //camera_set_position(cam, &((vec3){ -6.67555f, 4.68882f, -3.67537f }));
    //camera_rotate(cam, &((vec2d){ 136.5, -20.5 }));
    camera_set_view_point(cam, &((vec3) { 0.0f, 1.0f, 3.45f }));
    camera_set_interest(cam, &((vec3) { 0.0f, 1.0f, 0.0f }));
    camera_set_fov(cam, 70.0);

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

    back3d_color = (vec3){ (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0) };
    set_clear_color = true;

    shader_env_vert_set_ptr(&shaders_ft, 3, (vec4*)&vec4_identity);
    shader_env_vert_set_ptr(&shaders_ft, 4, (vec4*)&vec4_null);
    classes_process_init(classes, classes_count, rctx);
    return rctx;
}

static bool render_load_shaders(void* data, char* path, char* file, uint32_t hash) {
    string s;
    string_init(&s, path);
    string_add(&s, file);

    farc f;
    farc_init(&f);
    farc_read(&f, string_data(&s), true, false);
    shader_ft_load(data, &f, false);
    farc_free(&f);

    string_free(&s);
    return true;
}

extern vec2d input_move;
extern vec2d input_rotate;
extern double_t input_roll;
extern bool input_reset;
extern bool input_shaders_reload;

static void render_update(render_context* rctx) {
    camera* cam = rctx->camera;

    for (int32_t i = 0; i < 32; i++) {
        if (!gl_state_check_texture_binding_2d(i) && !gl_state_check_texture_binding_cube_map(i))
            continue;

        gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_bind_texture_cube_map(i, 0);
        gl_state_bind_sampler(i, 0);
    }

    global_context_menu = true;
    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    classes_process_imgui(classes, classes_count);
    lock_unlock(&imgui_context_lock);

    if (old_width != width || old_height != height || old_scale != scale) {
        render_resize_fb(rctx, true);
        camera_set_res(cam, internal_3d_res.x, internal_3d_res.y);
    }
    old_width = width;
    old_height = height;
    old_scale = scale;

    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    if (global_context_menu && igIsMouseReleased(ImGuiMouseButton_Right)
        && !igIsItemHovered(0) && imgui_context->OpenPopupStack.Size < 1)
        igOpenPopup_Str("Classes init context menu", 0);

    if (igBeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count, rctx);
        igEndPopup();
    }
    lock_unlock(&imgui_context_lock);

    if (input_shaders_reload) {
        input_shaders_reload = false;
        render_shaders_free();
        render_shaders_load();
    }

    if (window_handle == GetForegroundWindow()) {
        if (input_reset) {
            input_reset = false;
            camera_reset(cam);
            //camera_set_position(cam, &((vec3){ 1.35542f, 1.41634f, 1.27852f }));
            //camera_rotate(cam, &((vec2d){ -45.0, -32.5 }));
            //camera_set_position(cam, &((vec3){ -6.67555f, 4.68882f, -3.67537f }));
            //camera_rotate(cam, &((vec2d){ 136.5, -20.5 }));
            camera_set_view_point(cam, &((vec3) { 0.0f, 1.0f, 3.45f }));
            camera_set_interest(cam, &((vec3) { 0.0f, 1.0f, 0.0f }));
            camera_set_fov(cam, 70.0);
        }
        else {
            camera_rotate(cam, &input_rotate);
            camera_move(cam, &input_move);
            camera_roll(cam, input_roll);
        }
    }
    camera_update(cam);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (rob_chara_pv_data_array[i].field_0 == -1)
            continue;

        float_t frame = rob_chara_get_frame(&rob_chara_array[i]);
        float_t frame_count = rob_chara_get_frame_count(&rob_chara_array[i]);
        frame += get_delta_frame();
        if (frame >= frame_count)
            frame -= frame_count;
        //rob_chara_set_frame(&rob_chara_array[j], frame);
        rob_chara_set_frame(&rob_chara_array[i], rob_frame);
        rob_chara_array[i].item_equip->shadow_type = SHADOW_CHARA;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    obj_db_ptr = &aft_data->data_ft.obj_db;
    render_context_update(rctx);

    classes_process_render(classes, classes_count);

    wind_ptr = rctx->wind;
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (rob_chara_pv_data_array[i].field_0 == -1)
            continue;

        rob_chara_calc(&rob_chara_array[i]);
        rob_chara_draw(&rob_chara_array[i], rctx);
    }

    stage_update(rctx->stage, rctx);

    shadow_update(rctx->draw_pass.shadow_ptr, rctx);

    igRender();

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

    shader_glsl_set_mat4(&cube_line_shader, "vp", false, cam->view_projection);
    shader_glsl_set_mat4(&cube_line_point_shader, "vp", false, cam->view_projection);
}

static void cube_line_draw(shader_glsl* shader, camera* cam, vec3* trans, float_t line_size, vec4* color) {
    mat4 mat[2];
    mat4_translate(trans[0].x, trans[0].y, trans[0].z, &mat[0]);
    mat4_translate(trans[1].x, trans[1].y, trans[1].z, &mat[1]);
    mat4_mult(&cam->inv_view_rot, &mat[0], &mat[0]);
    mat4_mult(&cam->inv_view_rot, &mat[1], &mat[1]);
    mat4_get_translation(&mat[0], &trans[0]);
    mat4_get_translation(&mat[1], &trans[1]);

    float_t dx = trans[1].x - trans[0].x;
    float_t dy = trans[1].y - trans[0].y;
    vec3 norm[2];
    norm[0] = (vec3){ -dy, dx, 0.0f };
    norm[1] = (vec3){ dy, -dx, 0.0f };
    vec3_normalize(norm[0], norm[0]);
    vec3_normalize(norm[1], norm[1]);
    vec3_mult_scalar(norm[0], line_size, norm[0]);
    vec3_mult_scalar(norm[1], line_size, norm[1]);
    mat4_mult_vec3(&cam->inv_view_rot, &norm[0], &norm[0]);
    mat4_mult_vec3(&cam->inv_view_rot, &norm[1], &norm[1]);

    vec3 vert_trans[4];
    vec3_add(trans[0], norm[0], vert_trans[0]);
    vec3_add(trans[0], norm[1], vert_trans[1]);
    vec3_add(trans[1], norm[0], vert_trans[2]);
    vec3_add(trans[1], norm[1], vert_trans[3]);
    shader_glsl_set_vec3_array(shader, "trans", 4, vert_trans);
    shader_glsl_set_vec4(shader, "color", *color);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void cube_line_point_draw(shader_glsl* shader, camera* cam, vec3* trans,
    float_t size, float_t dark_size, vec4* color, vec4* dark_color) {
    mat4 mat;
    mat4_scale(dark_size, dark_size, dark_size, &mat);
    mat4_set_translation(&mat, trans);
    mat4_mult(&cam->inv_view_rot, &mat, &mat);
    shader_glsl_set_mat4(&cube_line_point_shader, "mat", false, mat);
    shader_glsl_set_vec4(&cube_line_point_shader, "color", *dark_color);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mat4_scale(size, size, size, &mat);
    mat4_set_translation(&mat, trans);
    mat4_mult(&cam->inv_view_rot, &mat, &mat);
    shader_glsl_set_mat4(&cube_line_point_shader, "mat", false, mat);
    shader_glsl_set_vec4(&cube_line_point_shader, "color", *color);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void render_draw(render_context* rctx) {
    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat depth_clear = 1.0f;
    static const GLint stencil_clear = 0;

    camera* cam = rctx->camera;

    for (int32_t i = 0; i < 32; i++) {
        if (!gl_state_check_texture_binding_2d(i) && !gl_state_check_texture_binding_cube_map(i))
            continue;

        gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_bind_texture_cube_map(i, 0);
        gl_state_bind_sampler(i, 0);
    }

    gl_state_bind_framebuffer(0);
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_set_stencil_mask(0xFF);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearDepthf(depth_clear);
    gl_state_set_stencil_mask(0x00);
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_bind_uniform_buffer_base(0, common_data_ubo);

    glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);

    render_texture_bind(&rctx->post_process.render_texture, 0);
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);
    gl_state_set_depth_mask(GL_FALSE);

    if (set_clear_color) {
        vec4 color;
        *(vec3*)&color = back3d_color;
        color.w = 1.0f;
        glClearBufferfv(GL_COLOR, 0, (GLfloat*)&color);
    }

    draw_pass_main(rctx);

    int32_t screen_x_offset = (width - internal_2d_res.x) / 2 + (width - internal_2d_res.x) % 2;
    int32_t screen_y_offset = (height - internal_2d_res.y) / 2 + (width - internal_2d_res.x) % 2;
    glViewport(screen_x_offset, screen_y_offset, internal_2d_res.x, internal_2d_res.y);
    render_texture_bind(&rctx->post_process.screen_texture, 0);
    gl_state_bind_uniform_buffer_base(0, common_data_ubo);
    classes_process_draw(classes, classes_count);

    static bool rob_draw = false;

    gl_state_disable_cull_face();
    gl_state_bind_vertex_array(cube_line_vao);
    const float_t line_size = 0.0025f;
    const float_t line_point_size = line_size * 1.125f;
    const float_t line_point_dark_size = line_size * 1.5f;
    vec4 bone_color = (vec4){ 1.0f, 0.0f, 0.0f, 1.0f };
    vec4 cns_color = (vec4){ 1.0f, 1.0f, 0.0f, 1.0f };
    vec4 exp_color = (vec4){ 0.0f, 1.0f, 0.0f, 1.0f };
    vec4 osg_color = (vec4){ 0.0f, 0.0f, 1.0f, 1.0f };
    vec4 osg_node_color = (vec4){ 1.0f, 0.0f, 1.0f, 1.0f };
    vec4 point_color = (vec4){ 1.0f, 1.0f, 1.0f, 1.0f };
    vec4 point_dark_color = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
    for (int32_t i = 0; i < ROB_CHARA_COUNT && rob_draw; i++) {
        if (rob_chara_pv_data_array[i].field_0 == -1)
            continue;

        rob_chara* rob_chr = &rob_chara_array[i];
        rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
        size_t object_bone_count = rob_bone_data->object_bone_count;
        size_t total_bone_count = rob_bone_data->total_bone_count;
        size_t ik_bone_count = rob_bone_data->ik_bone_count;
        vector_bone_node* nodes = &rob_bone_data->nodes;
        shader_glsl_use(&cube_line_shader);
        for (bone_node* j = nodes->begin; j != nodes->end; j++) {
            if (!j->parent)
                continue;

            vec3 trans[2];
            mat4_get_translation(j->parent->mat, &trans[0]);
            mat4_get_translation(j->mat, &trans[1]);

            if (!memcmp(&trans[0], &trans[1], sizeof(vec3)))
                continue;

            cube_line_draw(&cube_line_shader, cam, trans, line_size, &bone_color);
        }

        rob_chara_item_equip* rob_item_equip = rob_chr->item_equip;
        for (int32_t j = rob_item_equip->first_item_equip_object;
            j < rob_item_equip->max_item_equip_object; j++) {
            rob_chara_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[j];

            vector_ptr_ex_expression_block* expression_blocks = &itm_eq_obj->expression_blocks;
            for (ex_expression_block** k = expression_blocks->begin; k != expression_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_expression_block* exp = *k;

                if (!exp->base.bone_node || !exp->base.parent_bone_node)
                    continue;

                vec3 trans[2];
                mat4_get_translation(exp->base.parent_bone_node->mat, &trans[0]);
                mat4_get_translation(exp->base.bone_node->mat, &trans[1]);

                if (!memcmp(&trans[0], &trans[1], sizeof(vec3)))
                    continue;

                cube_line_draw(&cube_line_shader, cam, trans, line_size, &cns_color);
            }

            vector_ptr_ex_constraint_block* constraint_blocks = &itm_eq_obj->constraint_blocks;
            for (ex_constraint_block** k = constraint_blocks->begin; k != constraint_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_constraint_block* cns = *k;

                if (!cns->base.bone_node || !cns->base.parent_bone_node)
                    continue;

                vec3 trans[2];
                mat4_get_translation(cns->base.parent_bone_node->mat, &trans[0]);
                mat4_get_translation(cns->base.bone_node->mat, &trans[1]);

                if (!memcmp(&trans[0], &trans[1], sizeof(vec3)))
                    continue;

                cube_line_draw(&cube_line_shader, cam, trans, line_size, &exp_color);
            }

            vector_ptr_ex_osage_block* osage_blocks = &itm_eq_obj->osage_blocks;
            for (ex_osage_block** k = osage_blocks->begin; k != osage_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_osage_block* osg = *k;

                if (!osg->base.bone_node || !osg->base.parent_bone_node)
                    continue;

                vec3 trans[2];
                mat4_get_translation(osg->base.parent_bone_node->mat, &trans[0]);
                mat4_get_translation(osg->base.bone_node->mat, &trans[1]);

                if (!memcmp(&trans[0], &trans[1], sizeof(vec3)))
                    continue;

                cube_line_draw(&cube_line_shader, cam, trans, line_size, &osg_color);

                rob_osage* rob_osg = &osg->rob;
                vector_rob_osage_node* nodes = &osg->rob.nodes;
                rob_osage_node* parent_node = &osg->rob.node;
                for (rob_osage_node* l = nodes->begin; l != nodes->end; parent_node = l++) {
                    if (!l->bone_node || !parent_node->bone_node)
                        continue;

                    vec3 trans[2];
                    mat4_get_translation(parent_node->bone_node->mat, &trans[0]);
                    mat4_get_translation(l->bone_node->mat, &trans[1]);

                    if (!memcmp(&trans[0], &trans[1], sizeof(vec3)))
                        continue;

                    cube_line_draw(&cube_line_shader, cam, trans, line_size, &osg_node_color);
                }
            }
        }

        shader_glsl_use(&cube_line_point_shader);
        for (bone_node* j = nodes->begin; j != nodes->end; j++) {
            vec3 trans;
            mat4_get_translation(j->mat, &trans);
            cube_line_point_draw(&cube_line_point_shader, cam, &trans,
                line_point_size, line_point_dark_size, &point_color, &point_dark_color);
        }

        for (int32_t j = rob_item_equip->first_item_equip_object;
            j < rob_item_equip->max_item_equip_object; j++) {
            rob_chara_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[j];

            vector_ptr_ex_expression_block* expression_blocks = &itm_eq_obj->expression_blocks;
            for (ex_expression_block** k = expression_blocks->begin; k != expression_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_expression_block* exp = *k;

                if (!exp->base.bone_node)
                    continue;

                vec3 trans;
                mat4_get_translation(exp->base.bone_node->mat, &trans);
                cube_line_point_draw(&cube_line_point_shader, cam, &trans,
                    line_point_size, line_point_dark_size, &point_color, &point_dark_color);
            }

            vector_ptr_ex_constraint_block* constraint_blocks = &itm_eq_obj->constraint_blocks;
            for (ex_constraint_block** k = constraint_blocks->begin; k != constraint_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_constraint_block* cns = *k;

                if (!cns->base.bone_node)
                    continue;

                vec3 trans;
                mat4_get_translation(cns->base.bone_node->mat, &trans);
                cube_line_point_draw(&cube_line_point_shader, cam, &trans,
                    line_point_size, line_point_dark_size, &point_color, &point_dark_color);
            }

            vector_ptr_ex_osage_block* osage_blocks = &itm_eq_obj->osage_blocks;
            for (ex_osage_block** k = osage_blocks->begin; k != osage_blocks->end; k++) {
                if (!*k)
                    continue;

                ex_osage_block* osg = *k;

                if (!osg->base.bone_node)
                    continue;

                vec3 trans;
                mat4_get_translation(osg->base.bone_node->mat, &trans);
                cube_line_point_draw(&cube_line_point_shader, cam, &trans,
                    line_point_size, line_point_dark_size, &point_color, &point_dark_color);

                rob_osage* rob_osg = &osg->rob;
                vector_rob_osage_node* nodes = &osg->rob.nodes;
                for (rob_osage_node* l = nodes->begin; l != nodes->end; l++) {
                    if (!l->bone_node)
                        continue;

                    vec3 trans;
                    mat4_get_translation(l->bone_node->mat, &trans);
                    cube_line_point_draw(&cube_line_point_shader, cam, &trans,
                        line_point_size, line_point_dark_size, &point_color, &point_dark_color);
                }
            }
        }

        //rob_chara_calc(&rob_chara_array[j]);
        //rob_chara_draw(&rob_chara_array[j], rctx);
    }
    gl_state_bind_vertex_array(0);
    gl_state_enable_cull_face();

    gl_state_bind_framebuffer(0);
    render_texture_shader_set_glsl(0);
    glViewport(0, 0, width, height);
    if (rctx->draw_pass.enable[DRAW_PASS_POST_PROCESS])
        render_texture_draw(&rctx->post_process.screen_texture, false);

    if (draw_imgui)
        render_imgui(rctx);
}

static void render_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(imgui_context);
    lock_free(&imgui_context_lock);

    glitter_particle_manager_dispose(GPM_VAL);

    stage_free(&stage_test_data, rctx);
    stage_free(&stage_stgtst, rctx);

    data_struct_free();

    render_shaders_free();

    shader_glsl_free(&grid_shader);
    shader_glsl_free(&cube_line_point_shader);
    shader_glsl_free(&cube_line_shader);

    x_pv_player_free();

    glDeleteBuffers(1, &common_data_ubo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteBuffers(1, &cube_line_vbo);
    glDeleteVertexArrays(1, &cube_line_vao);

    rob_chara_array_free();

    auth_3d_data_free(rctx);
    light_param_storage_free();
    motion_storage_free();

    render_texture_data_free();
    render_context_free(rctx);

    object_storage_free();
    texture_storage_free();
}

static void render_imgui(render_context* rctx) {
    lock_lock(&imgui_context_lock);
    igSetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    lock_unlock(&imgui_context_lock);
}

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths) {
    if (!count || !paths)
        return;

    classes_process_drop(classes, classes_count, count, paths);
    glfwFocusWindow(window);
}

static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h) {
    width = w;
    height = h;
}

static void render_resize_fb(render_context* rctx, bool change_fb) {
    if (internal_3d_res.x < 20) internal_3d_res.x = 20;
    if (internal_3d_res.y < 20) internal_3d_res.y = 20;

    double_t res_width = (double_t)width;
    double_t res_height = (double_t)height;
    double_t view_aspect = res_width / res_height;
    res_width = round(res_height * aspect);
    res_height = round(res_width / aspect);
    if (view_aspect < aspect) {
        res_width = (double_t)width;
        res_height = round(res_width / aspect);
    }
    else {
        res_width = round(res_height * aspect);
        res_height = round(res_width / aspect);
        if (view_aspect > aspect)
            res_height = round(res_width / aspect);
    }

    vec2i internal_res;
    internal_res.x = (int32_t)res_width;
    internal_res.y = (int32_t)res_height;

    internal_2d_res.x = clamp(internal_res.x, 1, sv_max_texture_size);
    internal_2d_res.y = clamp(internal_res.y, 1, sv_max_texture_size);
    internal_3d_res.x = (int32_t)roundf((float_t)internal_res.x * scale);
    internal_3d_res.y = (int32_t)roundf((float_t)internal_res.y * scale);
    internal_3d_res.x = clamp(internal_3d_res.x, 1, sv_max_texture_size);
    internal_3d_res.y = clamp(internal_3d_res.y, 1, sv_max_texture_size);

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x
        || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x
        || old_internal_3d_res.y != internal_3d_res.y
        || old_width != width || old_height != height;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    bool st = false;
    lock_lock(&state_lock);
    st = state == RENDER_INITIALIZED;
    lock_unlock(&state_lock);

    if (st && fb_changed && change_fb) {
        post_process_init_fbo(&rctx->post_process, internal_3d_res.x, internal_3d_res.y,
            internal_2d_res.x, internal_2d_res.y, width, height);
        light_proj_resize(rctx->litproj, internal_3d_res.x, internal_3d_res.y);
    }
}

static void render_imgui_context_menu(classes_struct* classes,
    const size_t classes_count, render_context* rctx) {
    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (!c->name || ~c->flags & CLASSES_IN_CONTEXT_MENU)
            continue;

        if (c->sub_classes && c->sub_classes_count) {
            if (igBeginMenu(c->name, ~c->data.flags & CLASS_HIDDEN)) {
                render_imgui_context_menu(c->sub_classes,
                    c->sub_classes_count, rctx);
                igEndMenu();
            }
        }
        else if (~c->data.flags & CLASS_HIDDEN)
            igMenuItem_Bool(c->name, 0, false, false);
        else if (igMenuItem_Bool(c->name, 0, false, true)) {
            if (~c->data.flags & CLASS_INIT) {
                lock_init(&c->data.lock);
                if (lock_check_init(&c->data.lock) && c->init) {
                    lock_lock(&c->data.lock);
                    if (c->init(&c->data, rctx))
                        c->data.flags = CLASS_INIT;
                    else
                        c->data.flags = CLASS_DISPOSED | CLASS_HIDDEN;
                    lock_unlock(&c->data.lock);
                }
            }

            if (lock_check_init(&c->data.lock)) {
                lock_lock(&c->data.lock);
                if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show))
                    c->data.flags &= ~(CLASS_HIDE | CLASS_HIDDEN);
                lock_unlock(&c->data.lock);
            }
        }
    }
}

inline static void render_shaders_load() {
    data_struct_load_file(&data_list[DATA_AFT], &shaders_ft,
        "rom/", "ft_shaders.farc", render_load_shaders);
}

inline static void render_shaders_free() {
    shader_free(&shaders_ft);
}

#if defined (DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf("########################################\n");
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error;                ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour; ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour;  ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability;          ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance;          ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker;               ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group;           ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group;            ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other;                ");
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high;   ");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium; ");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low;    ");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notif;  ");
        break;
    }

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API\n");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System\n");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler\n");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party\n");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: Application\n");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf("Source: Other\n");
        break;
    }

    printf("Debug message (%d): %s\n", id, message);
    printf("########################################\n\n");
}
#endif

typedef struct x_pv_player_glitter {
    string name;
    uint32_t hash;
    glitter_effect_group* effect_group;
} x_pv_player_glitter;

typedef struct x_pv_player_stage_anim_speed {
    int32_t frame;
    int32_t bpm;
} x_pv_player_stage_anim_speed;

vector(x_pv_player_stage_anim_speed)

typedef struct x_pv_player {
    int32_t pv_id;
    int32_t stage_id;
    pvpp pp;
    pvsr sr;
    dsc dsc;

    x_pv_player_glitter pv_glitter;
    x_pv_player_glitter stage_glitter;
    vector_x_pv_player_stage_anim_speed anim_speed;
} x_pv_player;

vector_func(x_pv_player_stage_anim_speed)

static void x_pv_player_glitter_load(x_pv_player_glitter* pv_glt, char* name);
static void x_pv_player_glitter_free(x_pv_player_glitter* pv_glt);

static x_pv_player x_pv_player_data;

static void x_pv_player_init() {
    memset(&x_pv_player_data, 0, sizeof(x_pv_player));
}

static void x_pv_player_load(int32_t pv_id, int32_t stage_id) {
    x_pv_player* x_pv = &x_pv_player_data;
    x_pv->pv_id = pv_id;
    x_pv->stage_id = stage_id;

    char path_buf[0x200];
    char file_buf[0x200];

    data_struct* x_data = &data_list[DATA_X];

    sprintf_s(file_buf, sizeof(file_buf), "pv%03d.pvpp", pv_id);
    pvpp_init(&x_pv->pp);
    data_struct_load_file(x_data, &x_pv->pp, "rom/pv/", file_buf, pvpp_load_file);

    sprintf_s(file_buf, sizeof(file_buf), "stgpv%03d_param.pvsr", stage_id);
    pvsr_init(&x_pv->sr);
    data_struct_load_file(x_data, &x_pv->sr, "rom/pv_stage_rsrc/", file_buf, pvsr_load_file);

    GPM_VAL->data = x_data;
    sprintf_s(file_buf, sizeof(file_buf), "eff_pv%03d_main", pv_id);
    x_pv_player_glitter_load(&x_pv->pv_glitter, file_buf);

    sprintf_s(file_buf, sizeof(file_buf), "eff_stgpv%03d_main", stage_id);
    x_pv_player_glitter_load(&x_pv->stage_glitter, file_buf);

    dsc dsc_scene;
    dsc dsc_system;
    dsc dsc_easy;
    dsc_init(&dsc_scene);
    dsc_init(&dsc_system);
    dsc_init(&dsc_easy);

    farc dsc_common_farc;
    sprintf_s(path_buf, sizeof(path_buf), "rom/pv_script/pv%03d/", pv_id);
    sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
    farc_init(&dsc_common_farc);
    data_struct_load_file(x_data, &dsc_common_farc, path_buf, file_buf, farc_load_file);

    sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_scene.dsc", pv_id);
    farc_file* dsc_scene_ff = farc_read_file(&dsc_common_farc, file_buf);
    if (dsc_scene_ff)
        dsc_parse(&dsc_scene, dsc_scene_ff->data, dsc_scene_ff->size, DSC_X);

    sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_system.dsc", pv_id);
    farc_file* dsc_system_ff = farc_read_file(&dsc_common_farc, file_buf);
    if (dsc_system_ff)
        dsc_parse(&dsc_system, dsc_system_ff->data, dsc_system_ff->size, DSC_X);
    farc_free(&dsc_common_farc);

    sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_id);
    dsc_easy.type = DSC_X;
    data_struct_load_file(x_data, &dsc_easy, path_buf, file_buf, dsc_load_file);

    dsc_merge(&x_pv->dsc, 3, &dsc_scene, &dsc_system, &dsc_easy);

    dsc_free(&dsc_scene);
    dsc_free(&dsc_system);
    dsc_free(&dsc_easy);

    int32_t end_func_id = dsc_x_get_func_id("END");
    int32_t time_func_id = dsc_x_get_func_id("TIME");
    int32_t bar_time_set_func_id = dsc_x_get_func_id("BAR_TIME_SET");
    int32_t pv_end_func_id = dsc_x_get_func_id("PV_END");
    int32_t bar_point_func_id = dsc_x_get_func_id("BAR_POINT");

    dsc* d = &x_pv->dsc;
    int32_t time = -1;
    int32_t frame = -1;
    bool bar_set = false;
    int32_t prev_bar_point_time = -1;
    int32_t prev_bpm = -1;
    for (dsc_data* i = d->data.begin; i != d->data.end; i++)
        if (i->func == end_func_id || i->func == pv_end_func_id)
            break;
        else if (i->func == time_func_id) {
            time = (int32_t)dsc_data_get_func_data(d, i)[0];
            frame = (int32_t)roundf((float_t)time * (float_t)(60.0f / 100000.0f));
        }
        else if (i->func == bar_time_set_func_id) {
            if (prev_bar_point_time != -1)
                continue;

            uint32_t* data = dsc_data_get_func_data(d, i);
            int32_t bpm = (int32_t)data[0];
            int32_t time_signature = (int32_t)(data[1] + 1);

            if (bpm != prev_bpm) {
                x_pv_player_stage_anim_speed anim_speed;
                printf("Frame: %5d; BPM: %3d; Speed: %.9f\n", frame, bpm, (double_t)bpm * (1.0 / 120.0));
                vector_x_pv_player_stage_anim_speed_push_back(&x_pv->anim_speed, &anim_speed);
            }
            prev_bpm = bpm;
        }
        else if (i->func == bar_point_func_id) {
            if (prev_bar_point_time != -1) {
                float_t frame_speed = 200000.0f / (float_t)(time - prev_bar_point_time);
                int32_t bpm = (int32_t)roundf(frame_speed * 120.0f);
                if (bpm != prev_bpm) {
                    x_pv_player_stage_anim_speed anim_speed;
                    anim_speed.frame = frame;
                    anim_speed.bpm = bpm;
                    printf("Frame: %5d; BPM: %3d; Speed: %.9f\n", frame, bpm, (double_t)bpm * (1.0 / 120.0));
                    vector_x_pv_player_stage_anim_speed_push_back(&x_pv->anim_speed, &anim_speed);
                }
                prev_bpm = bpm;
            }
            prev_bar_point_time = time;
        }
}

static void x_pv_player_update() {

}

static void x_pv_player_free() {
    x_pv_player* x_pv = &x_pv_player_data;
    pvpp_free(&x_pv->pp);
    pvsr_free(&x_pv->sr);
    dsc_free(&x_pv->dsc);
    vector_x_pv_player_stage_anim_speed_free(&x_pv->anim_speed, 0);
}

static void x_pv_player_glitter_load(x_pv_player_glitter* pv_glt, char* name) {
    string_init(&pv_glt->name, name);
    pv_glt->hash = hash_murmurhash(string_data(&pv_glt->name), pv_glt->name.length, 0, false, false);
    glitter_file_reader* fr = glitter_file_reader_init(GLITTER_X, 0, string_data(&pv_glt->name), -1.0f);
    vector_ptr_glitter_file_reader_push_back(&GPM_VAL->file_readers, &fr);
    glitter_particle_manager_update_file_reader(GPM_VAL);
    pv_glt->effect_group = glitter_particle_manager_get_effect_group(GPM_VAL, pv_glt->hash);
}

static void x_pv_player_glitter_free(x_pv_player_glitter* pv_glt) {
    glitter_particle_manager_free_scene(GPM_VAL, pv_glt->hash);
    glitter_particle_manager_free_effect_group(GPM_VAL, pv_glt->hash);
}
