/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/camera.hpp"
#include "../CRE/data.hpp"
#include "../CRE/fbo.hpp"
#include "../CRE/file_handler.hpp"
#include "../CRE/gl_state.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/lock.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/shader.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/shader_glsl.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/stage_modern.hpp"
#include "../CRE/static_var.hpp"
#include "../CRE/task.hpp"
#include "../CRE/texture.hpp"
#include "../CRE/timer.hpp"
#include "../CRE/post_process.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include "classes/imgui_helper.hpp"
#include "input.hpp"
#include "pv_game.hpp"
#include "x_pv_game.hpp"
#include <timeapi.h>

#if defined(DEBUG)
#define OPENGL_DEBUG 0
#endif

#define CUBE_LINE_SIZE (0.0025f)
#define CUBE_LINE_POINT_SIZE (CUBE_LINE_SIZE * 1.5f)

shader_glsl* cube_line_shader;
shader_glsl* cube_line_point_shader;
shader_glsl* grid_shader;

timer* render_timer;

#define grid_size 50.0f
#define grid_spacing 1.0f
size_t grid_vertex_count = ((size_t)(grid_size / grid_spacing) * 2 + 1) * 4;

GLuint cube_line_vao;
GLuint cube_line_vbo;
GLuint cube_line_point_vao;
GLuint cube_line_point_instance_vbo;
GLuint grid_vbo;
static GLuint common_data_ubo = 0;

#define COMMON_DATA_SIZE (int32_t)(sizeof(vec4) + sizeof(mat4) * 3 + sizeof(vec4))

bool draw_imgui   = true;
bool draw_grid_3d = false;

const double_t render_scale_table[] = {
     1.0 / 4.0, //  25%
     2.0 / 6.0, //  33.3%
     2.0 / 4.0, //  50%
     4.0 / 6.0, //  66.6%
     3.0 / 4.0, //  75%
     5.0 / 6.0, //  83.3%
     4.0 / 4.0, // 100%
     7.0 / 6.0, // 116.6%
     5.0 / 4.0, // 125%
     8.0 / 6.0, // 133.3%
     6.0 / 4.0, // 150%
    10.0 / 6.0, // 166.6%
     7.0 / 4.0, // 175%
    11.0 / 6.0, // 183.3%
     8.0 / 4.0, // 200%
};

static int32_t old_scale_index;
static int32_t scale_index;

static vec2i old_internal_2d_res;
static vec2i old_internal_3d_res;
vec2i internal_2d_res;
vec2i internal_3d_res;
static int32_t old_width;
static int32_t old_height;
int32_t width;
int32_t height;

static const double_t aspect = 16.0 / 9.0;
vec4u8 clear_color;
bool set_clear_color;

bool light_chara_ambient;
vec4 npr_spec_color;

static render_context* render_load();
static void render_ctrl(render_context* rctx);
static void render_draw(render_context* rctx);
static void render_dispose(render_context* rctx);

static bool render_load_shaders(void* data, const char* path, const char* file, uint32_t hash);

static void render_imgui(render_context* rctx);

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(render_context* rctx, bool change_fb);

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx);
static void render_shaders_load();
static void render_shaders_free();

#if defined (DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);
#endif

extern bool close;
bool reload_render;
lock render_lock;
HWND window_handle;
GLFWwindow* window;
ImGuiContext* imgui_context;
lock imgui_context_lock;
bool global_context_menu;
extern size_t frame_counter;
render_context* rctx_ptr;
bool task_stage_is_modern;

render_init_struct::render_init_struct() : scale_index() {

}

int32_t render_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timeBeginPeriod(1);
    render_timer = new timer(60.0);

    state = RENDER_UNINITIALIZED;

    lock_init(&render_lock);
    if (!lock_check_init(&render_lock)) {
        delete render_timer;
        return 0;
    }

    render_init_struct* ris = (render_init_struct*)arg;
    window_handle = 0;

#pragma region GLFW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
#if defined (DEBUG) && OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ris->res.x > 0 && ris->res.x < 8192 ? ris->res.x : mode->width;
    height = ris->res.y > 0 && ris->res.y < 8192 ? ris->res.y : mode->height;

    width = (int32_t)(width / 2.0f);
    height = (int32_t)(height / 2.0f);

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

    bool maximized = mode->width == width && mode->height == height;
    glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);

    window = glfwCreateWindow(width, height, glfw_titlelabel, maximized ? monitor : 0, 0);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -2;
    }
    glGetError();
    glViewport(0, 0, width, height);

    window_handle = glfwGetWin32Window(window);
    glfwFocusWindow(window);
    glfwSetDropCallback(window, (GLFWdropfun)render_drop_glfw);
    glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)render_resize_fb_glfw);
    glfwSetWindowSize(window, width, height);
    glfwSetWindowSizeLimits(window, 896, 504, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowPos(window, 8, 31);

    Input::SetInputs(window);

    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
    width = window_rect.right;
    height = window_rect.bottom;

    scale_index = ris->scale_index > 0 && ris->scale_index < RENDER_SCALE_MAX
        ? ris->scale_index : RENDER_SCALE_100;
    old_scale_index = scale_index;
#pragma endregion

#if defined(DEBUG) && OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(render_debug_output, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
#endif

    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &sv_max_texture_buffer_size);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sv_max_texture_size);
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &sv_max_texture_max_anisotropy);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    lock_lock(&state_lock);
    state = RENDER_INITIALIZING;
    enum_or(thread_flags, THREAD_RENDER);
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

        render_timer->reset();
        while (!close && !reload_render) {
            render_timer->start_of_cycle();
            glfwPollEvents();
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
            Input::NewFrame();
            lock_lock(&render_lock);
            render_ctrl(rctx);
            lock_unlock(&render_lock);
            render_draw(rctx);
            glfwSwapBuffers(window);
            close |= !!glfwWindowShouldClose(window);
            frame_counter++;
            Input::EndFrame();
            render_timer->end_of_cycle();
        }

        lock_lock(&state_lock);
        state = RENDER_DISPOSING;
        lock_unlock(&state_lock);

        bool threads_wait = false;
        render_timer->reset();
        do {
            render_timer->start_of_cycle();
            lock_lock(&state_lock);
            threads_wait = thread_flags & ~THREAD_RENDER;
            lock_unlock(&state_lock);
            render_timer->end_of_cycle();
        } while (threads_wait);

        lock_lock(&render_lock);
        render_dispose(rctx);
        lock_unlock(&render_lock);
    } while (reload_render);

    lock_lock(&state_lock);
    state = RENDER_DISPOSED;
    enum_and(thread_flags, ~THREAD_RENDER);
    lock_unlock(&state_lock);

#pragma region GLFW Dispose
    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion
    lock_free(&render_lock);
    delete render_timer;
    return 0;
}

double_t render_get_scale() {
    return render_scale_table[scale_index];
}

int32_t render_get_scale_index() {
    return scale_index;
}

void render_set_scale_index(int32_t index) {
     scale_index = index >= 0 && index < RENDER_SCALE_MAX ? index : RENDER_SCALE_100;
}

float_t rob_frame = 0.0f;

static render_context* render_load() {
    texture_storage_init();

    file_handler_storage_init();

    render_context* rctx = new render_context;
    rctx_ptr = rctx;

    gl_state_get();
    render_texture_data_init();

    motion_storage_init();
    mothead_storage_init();
    osage_setting_data_init();

    data_struct_init();
    data_struct_load("CLOUD_data.txt");

    render_shaders_load();

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    rctx->data = aft_data;

    object_storage_init(aft_obj_db);
    item_table_array_init();
    app::task_work_init();
    rob_init();
    auth_3d_data_init();
    task_stage_init();
    dtm_stg_init();
    task_stage_modern_init();
    light_param_storage_data_init();
    task_pv_db_init();

    Glitter::glt_particle_manager.bone_data = aft_bone_data;

    glGenBuffers(1, &common_data_ubo);

    gl_state_bind_uniform_buffer(common_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, COMMON_DATA_SIZE, 0, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, common_data_ubo, 0, COMMON_DATA_SIZE);
    gl_state_bind_uniform_buffer(0);

    uniform_value[U16] = 1;

    const char* cube_line_vert_shader =
        "#version 430 core\n"
        "layout(location = 0) in vec4 a_position;\n"
        "\n"
        "out VertexData {\n"
        "    vec4 color;\n"
        "} result;\n"
        "\n"
        "uniform mat4 vp;\n"
        "uniform vec4 color;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vp * a_position;\n"
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
        "layout(location = 0) in vec3 i_trans;\n"
        "\n"
        "out VertexData {\n"
        "    vec2 uv;\n"
        "} result;\n"
        "\n"
        "uniform mat4 vp;\n"
        "uniform vec3 trans[4];\n"
        "\n"
        "void main() {\n"
        "    vec4 pos;\n"
        "    pos.xyz = trans[gl_VertexID] + i_trans;\n"
        "    pos.w = 1.0;\n"
        "    gl_Position = vp * pos;\n"
        "    vec2 uv;\n"
        "    uv.x = float(gl_VertexID / 2);\n"
        "    uv.y = float(gl_VertexID % 2);\n"
        "    result.uv = uv;\n"
        "}\n";

    const char* cube_line_point_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "in VertexData {\n"
        "    vec2 uv;\n"
        "} frg;\n"
        "\n"
        "uniform float dark_border_end;\n"
        "uniform float dark_border_start;\n"
        "\n"
        "void main() {\n"
        "    float blend = step(dark_border_end, frg.uv.x) * (1.0 - step(dark_border_start, frg.uv.x))"
        " * step(dark_border_end, frg.uv.y) * (1.0 - step(dark_border_start, frg.uv.y));\n"
        "    result = vec4(blend, blend, blend, 1.0);\n"
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

    cube_line_shader = new shader_glsl;
    shader_glsl_param param = {};
    param.name = "Cube Line";
    cube_line_shader->load(cube_line_vert_shader, cube_line_frag_shader, 0, &param);

    cube_line_point_shader = new shader_glsl;
    param = {};
    param.name = "Cube Line Point";
    cube_line_point_shader->load(cube_line_point_vert_shader, cube_line_point_frag_shader, 0, &param);

    grid_shader = new shader_glsl;
    param = {};
    param.name = "Grid";
    grid_shader->load(grid_vert_shader, grid_frag_shader, 0, &param);

    render_resize_fb(rctx, false);

    rctx->post_process.init_fbo(internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y, width, height);

    render_resize_fb(rctx, true);

    task_pv_db_append_task();
    task_auth_3d_append_task();
    app::TaskWork::AppendTask(&Glitter::glt_particle_manager, "GLITTER_TASK", 2);
    //app::TaskWork::AppendTask(&pv_game_data, "PV_GAME", 0);
    app::TaskWork::AppendTask(&x_pv_game_data, "X_PV_GAME", 0);
    task_rob_manager_append_task();

    aft_data->load_file(aft_data, "rom/", "chritm_prop.farc", item_table_array_load_file);
    aft_data->load_file(aft_data, "rom/", "mdata_chritm_prop.farc", item_table_array_load_file);
    aft_data->load_file(aft_data, "rom/skin_param/", "osage_setting.txt", osage_setting_data_load_file);
    aft_data->load_file(aft_data, "rom/skin_param/", "mdata_osage_setting.txt", osage_setting_data_load_file);

    light_param_data_storage::load(aft_data);
    auth_3d_data_load_auth_3d_db(aft_auth_3d_db);

    //object_storage_load_set(aft_obj_db, "MIKITM000");
    //object_storage_load_set(aft_obj_db, "MIKITM001");
    //object_storage_load_set(aft_obj_db, "MIKITM301");
    //object_storage_load_set(aft_obj_db, "MIKITM500");
    //object_storage_load_set(aft_obj_db, "MIKITM181");
    //object_storage_load_set(aft_obj_db, "MIKITM481");
    //object_storage_load_set(aft_obj_db, "MIKITM681");
    //object_storage_load_set(aft_obj_db, "MIKITM981");

    uint32_t dbg_set_id = aft_obj_db->get_object_set_id("DBG");
    object_storage_load_set(aft_data, aft_obj_db, dbg_set_id);

    render_timer->reset();
    for (int32_t i = 0; i < 30 || object_storage_load_obj_set_check_not_read(dbg_set_id); i++) {
        render_timer->start_of_cycle();
        lock_lock(&render_lock);
        app::TaskWork::Ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::Basic();
        lock_unlock(&render_lock);
        render_timer->end_of_cycle();
    }

    motion_set_load_motion(aft_mot_db->get_motion_set_id("CMN"), 0, aft_mot_db);
    /*motion_set_load_motion(aft_mot_db->get_motion_set_id("PV824"), 0, aft_mot_db);

    rob_chara_pv_data pv_data;
    int32_t chara_id = rob_chara_array_init_chara_index(CHARA_MIKU, &pv_data, 0, true);
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
        timer_reset(&render_timer);
        while (!task_rob_manager_check_chara_loaded(chara_id)) {
            timer_start_of_cycle(&render_timer);
            lock_lock(&render_lock);
            TaskWork::Ctrl();
            file_handler_storage_ctrl();
            lock_unlock(&render_lock);
            timer_end_of_cycle(&render_timer);
        }

        int32_t motion_id = aft_mot_db->get_motion_id("PV824_STF_P1_00");
        rob_chara_set_motion_id(&rob_chara_array[chara_id], motion_id, 0.0f,
            motion_storage_get_mot_data_frame_count(motion_id, aft_mot_db),
            false, true, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chara_set_visibility(&rob_chara_array[chara_id], true);
        rob_chara_set_frame(&rob_chara_array[chara_id], 0.0f);
        rob_chara_item_equip* rob_item_equip = rob_chara_array[chara_id].item_equip;
        for (int32_t j = rob_item_equip->first_item_equip_object;
            j < rob_item_equip->max_item_equip_object; j++) {
            rob_chara_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[j];
            itm_eq_obj->osage_iterations = 60;
            for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
                if (i)
                    i->rob.osage_reset = true;
        }
    }*/

    glGenVertexArrays(1, &cube_line_vao);
    glGenBuffers(1, &cube_line_vbo);
    gl_state_bind_vertex_array(cube_line_vao);
    gl_state_bind_array_buffer(cube_line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 4, 0, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
    gl_state_bind_array_buffer(0);

    glGenVertexArrays(1, &cube_line_point_vao);
    glGenBuffers(1, &cube_line_point_instance_vbo);
    gl_state_bind_vertex_array(cube_line_point_vao);
    gl_state_bind_array_buffer(cube_line_point_instance_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(std::pair<vec3, float_t>), 0);
    glVertexAttribDivisor(0, 1);
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    float_t* grid_verts = force_malloc_s(float_t, 3 * grid_vertex_count);

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

    camera* cam = rctx->camera;

    cam->initialize(aspect, internal_3d_res.x, internal_3d_res.y);
    //cam->set_position(cam, { 1.35542f, 1.41634f, 1.27852f });
    //cam->rotate(cam, { -45.0, -32.5 });
    //cam->set_position(cam, { -6.67555f, 4.68882f, -3.67537f });
    //cam->rotate(cam, { 136.5, -20.5 });
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    //cam->set_fov(cam, 70.0);
    cam->set_view_point({ 0.0f, 1.375f, 1.0f });
    cam->set_interest({ 0.0f, 1.375f, 0.0f });

    imgui_context = ImGui::CreateContext(0);
    lock_init(&imgui_context_lock);

    lock_lock(&imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = 0;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    lock_unlock(&imgui_context_lock);

    ImGui::StyleColorsDark(0);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    clear_color = { 0x60, 0x60, 0x60, 0xFF };
    set_clear_color = true;

    chara_index charas[6];
    int32_t modules[6];

    charas[0] = CHARA_MIKU;
    charas[1] = CHARA_MIKU;//CHARA_RIN;
    charas[2] = CHARA_MIKU;//CHARA_LEN;
    charas[3] = CHARA_MIKU;//CHARA_LUKA;
    charas[4] = CHARA_MIKU;//CHARA_MEIKO;
    charas[5] = CHARA_MIKU;//CHARA_KAITO;

    modules[0] = 0;//168;
    modules[1] = 0;//46;
    modules[2] = 0;//39;
    modules[3] = 0;//41;
    modules[4] = 0;//40;
    modules[5] = 0;//31;
    //pv_game_data.Load(739, charas, modules);

    charas[0] = CHARA_KAITO;
    charas[1] = CHARA_MIKU;//CHARA_RIN;
    charas[2] = CHARA_MEIKO;//CHARA_LEN;
    charas[3] = CHARA_LUKA;//CHARA_LUKA;
    charas[4] = CHARA_LUKA;//CHARA_KAITO;
    charas[5] = CHARA_LUKA;//CHARA_MEIKO;

    modules[0] = 0;//168;
    modules[1] = 0;//46;
    modules[2] = 0;//39;
    modules[3] = 0;//41;
    modules[4] = 0;//40;
    modules[5] = 0;//31;
    x_pv_game_data.Load(824, 24, charas, modules);

    shaders_ft.env_vert_set(3, vec4_identity);
    shaders_ft.env_vert_set(4, vec4_null);
    classes_process_init(classes, classes_count, rctx);
    return rctx;
}

extern double_t input_move_x;
extern double_t input_move_y;
extern double_t input_rotate_x;
extern double_t input_rotate_y;
extern double_t input_roll;
extern bool input_reset;

int32_t global_ctrl_frames = 0;

static void render_ctrl(render_context* rctx) {
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
    ImGui::SetCurrentContext(imgui_context);
    app::TaskWork_Window();
    classes_process_imgui(classes, classes_count);
    lock_unlock(&imgui_context_lock);

    if (old_width != width || old_height != height || old_scale_index != scale_index) {
        render_resize_fb(rctx, true);
        cam->set_res(internal_3d_res.x, internal_3d_res.y);
    }
    old_width = width;
    old_height = height;
    old_scale_index = scale_index;

    lock_lock(&imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    if (global_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right)
        && !ImGui::IsItemHovered(0) && imgui_context->OpenPopupStack.Size < 1)
        ImGui::OpenPopup("Classes init context menu", 0);

    if (ImGui::BeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count, rctx);
        ImGui::EndPopup();
    }
    lock_unlock(&imgui_context_lock);

    if (window_handle == GetForegroundWindow()) {
        if (input_reset) {
            input_reset = false;
            cam->reset();
            //cam->set_position({ 1.35542f, 1.41634f, 1.27852f });
            //cam->rotate({ -45.0, -32.5 });
            //cam->set_position({ -6.67555f, 4.68882f, -3.67537f });
            //cam->rotate({ 136.5, -20.5 });
            cam->set_view_point({ 0.0f, 1.0f, 3.45f });
            cam->set_interest({ 0.0f, 1.0f, 0.0f });
            //cam->set_fov(70.0);
            cam->set_fast_change_hist0(true);
        }
        else {
            cam->rotate(input_rotate_x, input_rotate_y);
            cam->move(input_move_x, input_move_y);
            if (input_roll != 0.0)
                cam->set_roll(cam->get_roll() + input_roll);
        }
    }

    classes_process_ctrl(classes, classes_count);

    do {
        rctx->ctrl();
    } while (--global_ctrl_frames >= 0);
    global_ctrl_frames = 0;

    ImGui::Render();

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

    cube_line_shader->set("vp", false, cam->view_projection);
    cube_line_point_shader->set("vp", false, cam->view_projection);
}

static void cube_line_draw(shader_glsl* shader, camera* cam, vec3* trans, float_t line_size, vec4* color) {
    mat4 mat[2];
    mat4_translate(trans[0].x, trans[0].y, trans[0].z, &mat[0]);
    mat4_translate(trans[1].x, trans[1].y, trans[1].z, &mat[1]);
    mat4_mult(&cam->view, &mat[0], &mat[0]);
    mat4_mult(&cam->view, &mat[1], &mat[1]);

    vec3 t[2];
    mat4_get_translation(&mat[0], &t[0]);
    mat4_get_translation(&mat[1], &t[1]);

    vec3 d;
    vec2_sub(*(vec2*)&t[1], *(vec2*)&t[0], *(vec2*)&d);
    vec2_normalize(*(vec2*)&d, *(vec2*)&d);
    vec2_mult_scalar(*(vec2*)&d, line_size, *(vec2*)&d);
    d.z = 0.0f;

    vec3 norm[2];
    norm[0] = { -d.y, d.x, 0.0f };
    norm[1] = { d.y, -d.x, 0.0f };
    mat4_mult_vec3(&cam->inv_view_rot, &norm[0], &norm[0]);
    mat4_mult_vec3(&cam->inv_view_rot, &norm[1], &norm[1]);

    vec3 vert_trans[4];
    vec3_add(trans[0], norm[0], vert_trans[0]);
    vec3_add(trans[0], norm[1], vert_trans[1]);
    vec3_add(trans[1], norm[0], vert_trans[2]);
    vec3_add(trans[1], norm[1], vert_trans[3]);

    shader->set("color", *color);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_trans), vert_trans);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static int cube_line_points_sort(void const* src1, void const* src2) {
    float_t d1 = ((std::pair<vec3, float_t>*)src1)->second;
    float_t d2 = ((std::pair<vec3, float_t>*)src2)->second;
    return d1 > d2 ? -1 : (d1 < d2 ? 1 : 0);
}

static bool rob_draw = false;

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

    rctx->post_process.rend_texture.bind();
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);
    gl_state_set_depth_mask(GL_FALSE);

    if (set_clear_color) {
        vec4 _clear_color;
        vec4u8_to_vec4(clear_color, _clear_color);
        vec4_mult_scalar(_clear_color, (float_t)(1.0 / 255.0), _clear_color);
        glClearBufferfv(GL_COLOR, 0, (GLfloat*)&_clear_color);
    }

    cam->update();

    rctx->disp();

    int32_t screen_x_offset = (width - internal_2d_res.x) / 2 + (width - internal_2d_res.x) % 2;
    int32_t screen_y_offset = (height - internal_2d_res.y) / 2 + (width - internal_2d_res.x) % 2;
    glViewport(screen_x_offset, screen_y_offset, internal_2d_res.x, internal_2d_res.y);
    rctx->post_process.screen_texture.bind();
    gl_state_bind_uniform_buffer_base(0, common_data_ubo);
    classes_process_draw(classes, classes_count);

    gl_state_disable_cull_face();
    vec4 bone_color = { 1.0f, 0.0f, 0.0f, 1.0f };
    vec4 cns_color = { 1.0f, 1.0f, 0.0f, 1.0f };
    vec4 exp_color = { 0.0f, 1.0f, 0.0f, 1.0f };
    vec4 osg_color = { 0.0f, 0.0f, 1.0f, 1.0f };
    vec4 osg_node_color = { 1.0f, 0.0f, 1.0f, 1.0f };
    for (int32_t i = 0; i < ROB_CHARA_COUNT && rob_draw; i++) {
        if (rob_chara_pv_data_array[i].type == ROB_CHARA_TYPE_NONE)
            continue;

        rob_chara* rob_chr = &rob_chara_array[i];
        if (~rob_chr->data.field_0 & 1)
            continue;

        gl_state_bind_vertex_array(cube_line_vao);
        gl_state_bind_array_buffer(cube_line_vbo);
        rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
        size_t object_bone_count = rob_bone_data->object_bone_count;
        size_t total_bone_count = rob_bone_data->total_bone_count;
        size_t ik_bone_count = rob_bone_data->ik_bone_count;
        cube_line_shader->use();
        for (bone_node& j : rob_bone_data->nodes) {
            if (!j.parent)
                continue;

            vec3 trans[2];
            mat4_get_translation(j.parent->mat, &trans[0]);
            mat4_get_translation(j.mat, &trans[1]);

            if (memcmp(&trans[0], &trans[1], sizeof(vec3)))
                cube_line_draw(cube_line_shader, cam, trans, CUBE_LINE_SIZE, &bone_color);
        }
        gl_state_bind_array_buffer(0);

        std::vector<std::pair<vec3, float_t>> cube_line_points;
        for (bone_node& j : rob_bone_data->nodes) {
            vec3 trans;
            mat4_get_translation(j.mat, &trans);
            cube_line_points.push_back({ trans, 0.0f });
        }

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        mot_key_set* key_set = mot->mot_key_data.mot.key_sets;
        for (bone_data& j :mot->bone_data.bones)
            if (j.type >= BONE_DATABASE_BONE_HEAD_IK_ROTATION
                && j.type <= BONE_DATABASE_BONE_LEGS_IK_ROTATION)
                cube_line_points.push_back({ j.ik_target, 0.0f });

        for (std::pair<vec3, float_t>& i : cube_line_points) {
            vec3 trans;
            mat4_mult_vec3(&cam->view_projection, &i.first, &trans);
            i.second = trans.z;
        }

        quicksort_custom(cube_line_points.data(), cube_line_points.size(),
            sizeof(std::pair<vec3, float_t>), cube_line_points_sort);

        gl_state_bind_array_buffer(cube_line_point_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(std::pair<vec3, float_t>)
            * cube_line_points.size()), cube_line_points.data(), GL_STREAM_DRAW);
        gl_state_bind_array_buffer(0);

        vec3 trans[4];
        trans[0] = { -1.0,  1.0, 0.0 };
        trans[1] = {  1.0,  1.0, 0.0 };
        trans[2] = { -1.0, -1.0, 0.0 };
        trans[3] = {  1.0, -1.0, 0.0 };

        vec3_mult_scalar(trans[0], CUBE_LINE_POINT_SIZE, trans[0]);
        vec3_mult_scalar(trans[1], CUBE_LINE_POINT_SIZE, trans[1]);
        vec3_mult_scalar(trans[2], CUBE_LINE_POINT_SIZE, trans[2]);
        vec3_mult_scalar(trans[3], CUBE_LINE_POINT_SIZE, trans[3]);

        mat4_mult_vec3(&cam->inv_view_rot, &trans[0], &trans[0]);
        mat4_mult_vec3(&cam->inv_view_rot, &trans[1], &trans[1]);
        mat4_mult_vec3(&cam->inv_view_rot, &trans[2], &trans[2]);
        mat4_mult_vec3(&cam->inv_view_rot, &trans[3], &trans[3]);

        gl_state_bind_vertex_array(cube_line_point_vao);
        cube_line_point_shader->use();
        cube_line_point_shader->set("trans", 4, trans);
        cube_line_point_shader->set("dark_border_end",
            ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE)));
        cube_line_point_shader->set("dark_border_start",
            (1.0f - ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE))));
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)cube_line_points.size());
        cube_line_points.clear();
        cube_line_points.shrink_to_fit();
    }
    gl_state_bind_vertex_array(0);
    gl_state_enable_cull_face();
    gl_state_bind_framebuffer(0);

    if (rctx->draw_pass.enable[DRAW_PASS_POST_PROCESS])
        fbo::blit(rctx->post_process.screen_texture.fbos[0], 0,
            0, 0, width, height,
            0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    if (draw_imgui)
        render_imgui(rctx);
}

static void render_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imgui_context);
    lock_free(&imgui_context_lock);

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    motion_set_unload_motion(aft_mot_db->get_motion_set_id("CMN"));

    task_rob_manager_free_task();
    //pv_game_data.SetDest();
    x_pv_game_data.SetDest();
    Glitter::glt_particle_manager.SetDest();
    task_auth_3d_free_task();
    task_pv_db_free_task();

    //rob_chara_array_free_chara_id(0);
    render_timer->reset();
    for (int32_t i = 0; i < 30; i++) {
        render_timer->start_of_cycle();
        lock_lock(&render_lock);
        app::TaskWork::Ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::Basic();
        lock_unlock(&render_lock);
        render_timer->end_of_cycle();
    }

    light_param_data_storage::unload();

    if (grid_shader) {
        delete grid_shader;
        grid_shader = 0;
    }

    if (cube_line_point_shader) {
        delete cube_line_point_shader;
        cube_line_point_shader = 0;
    }

    if (cube_line_shader) {
        delete cube_line_shader;
        cube_line_shader = 0;
    }

    object_storage_unload_set(aft_obj_db, "DBG");

    task_pv_db_free();
    light_param_storage_data_free();
    task_stage_modern_free();
    dtm_stg_free();
    task_stage_free();
    auth_3d_data_free();
    rob_free();
    app::task_work_free();
    item_table_array_free();
    object_storage_free();
    data_struct_free();

    glDeleteBuffers(1, &common_data_ubo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteBuffers(1, &cube_line_point_instance_vbo);
    glDeleteVertexArrays(1, &cube_line_point_vao);
    glDeleteBuffers(1, &cube_line_vbo);
    glDeleteVertexArrays(1, &cube_line_vao);

    motion_storage_free();
    mothead_storage_free();
    osage_setting_data_free();

    render_texture_data_free();
    delete rctx;

    file_handler_storage_free();

    texture_storage_free();

    render_shaders_free();
}

static void render_imgui(render_context* rctx) {
    lock_lock(&imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    lock_unlock(&imgui_context_lock);
}

static bool render_load_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    std::string s = path + std::string(file);

    farc f;
    f.read(s.c_str(), true, false);
    shader_ft_load((shader_set_data*)data, &f, false);

    return true;
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
    internal_3d_res.x = (int32_t)roundf((float_t)(internal_res.x * render_scale_table[scale_index]));
    internal_3d_res.y = (int32_t)roundf((float_t)(internal_res.y * render_scale_table[scale_index]));
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
        rctx->post_process.init_fbo(internal_3d_res.x, internal_3d_res.y,
            internal_2d_res.x, internal_2d_res.y, width, height);
        rctx->litproj->resize(internal_3d_res.x, internal_3d_res.y);
    }
}

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx) {
    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (!c->name || ~c->flags & CLASSES_IN_CONTEXT_MENU)
            continue;

        if (c->sub_classes && c->sub_classes_count) {
            if (ImGui::BeginMenu(c->name, ~c->data.flags & CLASS_HIDDEN)) {
                render_imgui_context_menu(c->sub_classes,
                    c->sub_classes_count, rctx);
                ImGui::EndMenu();
            }
        }
        else if (~c->data.flags & CLASS_HIDDEN)
            ImGui::MenuItem(c->name, 0, false, false);
        else if (ImGui::MenuItem(c->name, 0, false,
            !c->shared_lock || c->shared_lock && !*c->shared_lock)) {
            if (~c->data.flags & CLASS_INIT) {
                lock_init(&c->data.lock);
                if (lock_check_init(&c->data.lock) && c->init) {
                    lock_lock(&c->data.lock);
                    if (c->init(&c->data, rctx))
                        c->data.flags = CLASS_INIT;
                    else
                        c->data.flags = (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN);
                    lock_unlock(&c->data.lock);
                }
            }

            if (lock_check_init(&c->data.lock)
                && (!c->shared_lock || c->shared_lock && !*c->shared_lock)) {
                lock_lock(&c->data.lock);
                if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show)) {
                    if (c->shared_lock)
                        *c->shared_lock = true;
                    enum_and(c->data.flags, ~(CLASS_HIDE | CLASS_HIDDEN | CLASS_HIDE_WINDOW));
                }
                lock_unlock(&c->data.lock);
            }
        }
    }
}

inline static void render_shaders_load() {
    data_list[DATA_AFT].load_file(&shaders_ft,
        "rom/", "ft_shaders.farc", render_load_shaders);
}

inline static void render_shaders_free() {
    shaders_ft.unload();
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
