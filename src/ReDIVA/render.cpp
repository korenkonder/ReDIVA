/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#include "render.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/camera.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/data.hpp"
#include "../CRE/fbo.hpp"
#include "../CRE/file_handler.hpp"
#include "../CRE/gl_state.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/lock.hpp"
#include "../CRE/mdata_manager.hpp"
#include "../CRE/object.hpp"
#include "../CRE/ogg_vorbis.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/random.hpp"
#include "../CRE/shader.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/shader_glsl.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/stage_modern.hpp"
#include "../CRE/stage_param.hpp"
#include "../CRE/static_var.hpp"
#include "../CRE/task.hpp"
#include "../CRE/task_effect.hpp"
#include "../CRE/texture.hpp"
#include "../CRE/timer.hpp"
#include "../CRE/post_process.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data_test/auth_3d_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/stage_test.hpp"
#include "game_state.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"
#include "task_window.hpp"
#include "pv_game.hpp"
#include "x_pv_game.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <timeapi.h>

#if defined(DEBUG)
#define OPENGL_DEBUG 1
#endif

#define CUBE_LINE_SIZE (0.0025f)
#define CUBE_LINE_POINT_SIZE (CUBE_LINE_SIZE * 1.5f)

#define DRAW_PASS_TIME_DISP 0

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
GLuint grid_vao;
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

bool light_chara_ambient;
vec4 npr_spec_color;

uint32_t cmn_set_id;
uint32_t dbg_set_id;

#if DRAW_PASS_TIME_DISP
static std::vector<float_t> draw_pass_cpu_time[DRAW_PASS_MAX];
static std::vector<float_t> draw_pass_gpu_time[DRAW_PASS_MAX];
#endif

static render_context* render_load();
static void render_ctrl(render_context* rctx);
static void render_disp(render_context* rctx);
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

#if defined(DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);
#endif

bool close;
bool reload_render;
lock_cs* render_lock;
HWND window_handle;
GLFWwindow* window;
ImGuiContext* imgui_context;
lock_cs* imgui_context_lock;
bool global_context_menu;
extern size_t frame_counter;
render_context* rctx_ptr;
bool task_stage_is_modern;

render_init_struct::render_init_struct() : scale_index() {

}

int32_t render_main(render_init_struct* ris) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timeBeginPeriod(1);

    render_lock = new lock_cs;
    if (!render_lock)
        return 0;

    render_timer = new timer(60.0);

    window_handle = 0;

#pragma region GLFW Init
    if (!glfwInit())
        return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ris->res.x > 0 && ris->res.x < 8192 ? ris->res.x : mode->width;
    height = ris->res.y > 0 && ris->res.y < 8192 ? ris->res.y : mode->height;

    width = (int32_t)(width / 2.0f);
    height = (int32_t)(height / 2.0f);

#if BAKE_PNG || BAKE_VIDEO
    width = 1920;
    height = 1080;
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#if defined(DEBUG) && OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);

#if BAKE_PNG || BAKE_VIDEO
    bool maximized = false;
#else
    bool maximized = mode->width == width && mode->height == height;
#endif
    glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);

    int32_t minor = 6;
    window = 0;
    while (!window || minor < 3) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor--);

        const char* glfw_titlelabel;
#if defined(DEBUG)
#if defined(ReDIVA_DEV)
        glfw_titlelabel = "ReDIVADev Debug";
#else
        glfw_titlelabel = "ReDIVA Debug";
#endif
#else
#if defined(ReDIVA_DEV)
        glfw_titlelabel = "ReDIVADev";
#else
        glfw_titlelabel = "ReDIVA";
#endif
#endif
        window = glfwCreateWindow(width, height, glfw_titlelabel, maximized ? monitor : 0, 0);
    }

    if (!window) {
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    glGetError();
    glViewport(0, 0, width, height);

    window_handle = glfwGetWin32Window(window);
    glfwShowWindow(window);
    glfwFocusWindow(window);
    glfwSetDropCallback(window, (GLFWdropfun)render_drop_glfw);
    glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)render_resize_fb_glfw);
    glfwSetWindowSize(window, width, height);
    glfwSetWindowSizeLimits(window, 896, 504, GLFW_DONT_CARE, GLFW_DONT_CARE);
#if BAKE_PNG || BAKE_VIDEO
    glfwSetWindowPos(window, 0, 0);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
#else
    glfwSetWindowPos(window, 8, 31);
#endif

    Input::SetInputs(window);

    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
#if !(BAKE_PNG || BAKE_VIDEO)
    width = window_rect.right;
    height = window_rect.bottom;
#endif

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
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#if DRAW_PASS_TIME_DISP
    for (std::vector<float_t>& i : draw_pass_cpu_time)
        i.resize(240, 0.0f);

    for (std::vector<float_t>& i : draw_pass_gpu_time)
        i.resize(240, 0.0f);
#endif

    do {
        close = false;
        reload_render = false;

        render_context* rctx = 0;
        lock_lock(render_lock);
        rctx = render_load();
        lock_unlock(render_lock);

        //sound_work_play_stream(1, "rom/sound/bgm/selector_verB_a_lp.ogg");

        frame_counter = 0;

#pragma region GL Init
#if !(BAKE_PNG || BAKE_VIDEO)
        glfwGetFramebufferSize(window, &width, &height);
#endif
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
            lock_lock(render_lock);
            render_ctrl(rctx);
            lock_unlock(render_lock);
            render_disp(rctx);
            close |= !!glfwWindowShouldClose(window);
            frame_counter++;
            Input::EndFrame();
            glfwSwapBuffers(window);
            render_timer->end_of_cycle();
        }

        //sound_work_release_stream(1);

        lock_lock(render_lock);
        render_dispose(rctx);
        lock_unlock(render_lock);
    } while (reload_render);

#if DRAW_PASS_TIME_DISP
    for (std::vector<float_t>& i : draw_pass_cpu_time) {
        i.clear();
        i.shrink_to_fit();
    }

    for (std::vector<float_t>& i : draw_pass_gpu_time) {
        i.clear();
        i.shrink_to_fit();
    }
#endif

#pragma region GLFW Dispose
    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion
    delete render_lock;
    render_lock = 0;
    delete render_timer;
    render_timer = 0;
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

    game_state_init();

    data_struct_init();
    data_struct_load("ReDIVA_data.txt");
    data_struct_load_db();

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    for (std::string& i : mdata_manager_get()->prefixes) {
        std::string osage_setting_file = i + "osage_setting.txt";
        aft_data->load_file(aft_data, "rom/skin_param/",
            osage_setting_file.c_str(), osage_setting_data_load_file);
    }

    render_shaders_load();
    sound_init();
    wave_audio_storage_init();
    ogg_file_handler_storage_init();
    ogg_playback_data_init();
    object_storage_init(aft_obj_db);
    stage_param_data_storage_init();
    pv_expression_file_storage_init();
    item_table_array_init();
    rand_state_array_init();

    app::task_work_init();

    rob_init();
    task_wind_init();
    auth_3d_data_init();
    auth_3d_test_task_init();
    task_auth_3d_init();
    task_stage_init();
    dtm_stg_init();
    task_stage_modern_init();
    light_param_data_storage_data_init();
    task_effect_init();
    task_pv_db_init();
    Glitter::glt_particle_manager_init();

    auth_3d_test_window_init();
    dtw_stg_init();
    task_data_test_glitter_particle_init();

    Glitter::glt_particle_manager->bone_data = aft_bone_data;

    if (false) {
        data_struct* aft_data = &data_list[DATA_AFT];
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        data_struct* x_data = &data_list[DATA_X];
        data_struct* xhd_data = &data_list[DATA_XHD];

        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 800; i <= 831; i++) {
            sprintf_s(buf, sizeof(buf), i == 815 ? "EFFPV%03d" : "ITMPV%03d", i);

            object_set_info* effpv_set_info;
            if (aft_obj_db->get_object_set_info(buf, &effpv_set_info)) {
                obj_set_ids.push_back(effpv_set_info->id);
                obj_set_id_name.insert({ effpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03d", i);

            object_set_info* stgpv_set_info;
            if (aft_obj_db->get_object_set_info(buf, &stgpv_set_info)) {
                obj_set_ids.push_back(stgpv_set_info->id);
                obj_set_id_name.insert({ stgpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i);

            object_set_info* stgpvhrc_set_info;
            if (aft_obj_db->get_object_set_info(buf, &stgpvhrc_set_info)) {
                obj_set_ids.push_back(stgpvhrc_set_info->id);
                obj_set_id_name.insert({ stgpvhrc_set_info->id, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                object_set_info* set_info;
                if (!aft_obj_db->get_object_set_info(i, &set_info))
                    continue;

                farc f;
                if (!aft_data->load_file(&f, "rom/objset/", set_info->archive_file_name.c_str(), farc::load_file))
                    continue;

                farc_file* obj_ff = f.read_file(set_info->object_file_name.c_str());
                if (!obj_ff)
                    continue;

                farc_file* tex_ff = f.read_file(set_info->texture_file_name.c_str());
                if (!tex_ff)
                    continue;

                prj::shared_ptr<alloc_data> alloc(new alloc_data);

                obj_set obj_set;
                obj_set.unpack_file(alloc, obj_ff->data, obj_ff->size, false);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file(tex_ff->data, false))
                    continue;

                object_material_msgpack_write("patch\\AFT_orig\\objset", obj_set_id_name[i].c_str(),
                    i, &obj_set, &txp_set, aft_obj_db, aft_tex_db);
            }

            obj_set_ids.clear();
            obj_set_id_name.clear();
        }

        for (int32_t i = 801; i <= 832; i++) {
            uint32_t effpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03d", i));
            if (x_data->check_file_exists("root+/objset", effpv_hash)) {
                obj_set_ids.push_back(effpv_hash);
                obj_set_id_name.insert({ effpv_hash, buf });
            }

            uint32_t stgpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "STGPV%03d", i % 100));
            if (x_data->check_file_exists("root+/objset", stgpv_hash)) {
                obj_set_ids.push_back(stgpv_hash);
                obj_set_id_name.insert({ stgpv_hash, buf });
            }

            uint32_t stgpvhrc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i % 100));
            if (x_data->check_file_exists("root+/objset", stgpvhrc_hash)) {
                obj_set_ids.push_back(stgpvhrc_hash);
                obj_set_id_name.insert({ stgpvhrc_hash, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                farc f;
                if (!x_data->load_file(&f, "root+/objset/", i, ".farc", farc::load_file))
                    continue;

                std::string& set = obj_set_id_name[i];

                size_t set_len = set.size();
                if (set_len >= sizeof(buf) - 4)
                    continue;

                const char* t = strrchr(set.c_str(), '.');
                if (t)
                    set_len = t - set.c_str();

                memcpy(buf, set.c_str(), set_len);
                char* ext = buf + set_len;
                size_t ext_len = sizeof(buf) - set_len;

                memcpy_s(ext, ext_len, ".osd", 5);
                farc_file* osd = f.read_file(buf);
                if (!osd)
                    continue;

                memcpy_s(ext, ext_len, ".txd", 5);
                farc_file* txd = f.read_file(buf);
                if (!txd)
                    continue;

                memcpy_s(ext, ext_len, ".osi", 5);
                farc_file* osi = f.read_file(buf);
                if (!osi)
                    continue;

                memcpy_s(ext, ext_len, ".txi", 5);
                farc_file* txi = f.read_file(buf);
                if (!txi)
                    continue;

                prj::shared_ptr<alloc_data> alloc(new alloc_data);

                obj_set obj_set;
                obj_set.unpack_file(alloc, osd->data, osd->size, true);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file_modern(txd->data, txd->size))
                    continue;

                object_database_file obj_db_file;
                obj_db_file.read(osi->data, osi->size, true);
                if (!obj_db_file.ready)
                    continue;

                texture_database_file tex_db_file;
                tex_db_file.read(txi->data, txi->size, true);
                if (!tex_db_file.ready)
                    continue;

                object_database obj_db;
                obj_db.add(&obj_db_file);

                texture_database tex_db;
                tex_db.add(&tex_db_file);

                object_material_msgpack_write("patch\\X_orig\\objset", obj_set_id_name[i].c_str(),
                    i, &obj_set, &txp_set, &obj_db, &tex_db);
            }

            obj_set_ids.clear();
            obj_set_id_name.clear();
        }

        if (false)for (int32_t i = 801; i <= 832; i++) {
            uint32_t effpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03d", i));
            if (xhd_data->check_file_exists("root+/objset", effpv_hash)) {
                obj_set_ids.push_back(effpv_hash);
                obj_set_id_name.insert({ effpv_hash, buf });
            }

            uint32_t stgpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "STGPV%03d", i % 100));
            if (xhd_data->check_file_exists("root+/objset", stgpv_hash)) {
                obj_set_ids.push_back(stgpv_hash);
                obj_set_id_name.insert({ stgpv_hash, buf });
            }

            uint32_t stgpvhrc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i % 100));
            if (xhd_data->check_file_exists("root+/objset", stgpvhrc_hash)) {
                obj_set_ids.push_back(stgpvhrc_hash);
                obj_set_id_name.insert({ stgpvhrc_hash, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                farc f;
                if (!xhd_data->load_file(&f, "root+/objset/", i, ".farc", farc::load_file))
                    continue;

                std::string& set = obj_set_id_name[i];

                size_t set_len = set.size();
                if (set_len >= sizeof(buf) - 4)
                    continue;

                const char* t = strrchr(set.c_str(), '.');
                if (t)
                    set_len = t - set.c_str();

                memcpy(buf, set.c_str(), set_len);
                char* ext = buf + set_len;
                size_t ext_len = sizeof(buf) - set_len;

                memcpy_s(ext, ext_len, ".osd", 5);
                farc_file* osd = f.read_file(buf);
                if (!osd)
                    continue;

                memcpy_s(ext, ext_len, ".txd", 5);
                farc_file* txd = f.read_file(buf);
                if (!txd)
                    continue;

                memcpy_s(ext, ext_len, ".osi", 5);
                farc_file* osi = f.read_file(buf);
                if (!osi)
                    continue;

                memcpy_s(ext, ext_len, ".txi", 5);
                farc_file* txi = f.read_file(buf);
                if (!txi)
                    continue;

                prj::shared_ptr<alloc_data> alloc(new alloc_data);

                obj_set obj_set;
                obj_set.unpack_file(alloc, osd->data, osd->size, true);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file_modern(txd->data, txd->size))
                    continue;

                object_database_file obj_db_file;
                obj_db_file.read(osi->data, osi->size, true);
                if (!obj_db_file.ready)
                    continue;

                texture_database_file tex_db_file;
                tex_db_file.read(txi->data, txi->size, true);
                if (!tex_db_file.ready)
                    continue;

                object_database obj_db;
                obj_db.add(&obj_db_file);

                texture_database tex_db;
                tex_db.add(&tex_db_file);

                object_material_msgpack_write("patch\\XHD_orig\\objset", obj_set_id_name[i].c_str(),
                    i, &obj_set, &txp_set, &obj_db, &tex_db);
            }

            obj_set_ids.clear();
            obj_set_id_name.clear();
        }
    }

    glGenBuffers(1, &common_data_ubo);

    gl_state_bind_uniform_buffer(common_data_ubo);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_UNIFORM_BUFFER, COMMON_DATA_SIZE, 0, GL_DYNAMIC_STORAGE_BIT);
    else
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
        "uniform float border_end;\n"
        "uniform float border_start;\n"
        "uniform vec3 border_color;"
        "uniform vec3 center_color;"
        "\n"
        "void main() {\n"
        "    float blend = step(border_end, frg.uv.x) * (1.0 - step(border_start, frg.uv.x))"
        " * step(border_end, frg.uv.y) * (1.0 - step(border_start, frg.uv.y));\n"
        "    result = vec4(mix(border_color, center_color, blend), 1.0);\n"
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
        "const vec4 colors[] = {\n"
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
    rctx->draw_pass.resize(internal_2d_res.x, internal_2d_res.y);

    render_resize_fb(rctx, true);

    Glitter::glt_particle_manager_append_task();
    //app::TaskWork::AppendTask(&pv_game_data, "PVGAME", 0);
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_array[i].type = ROB_CHARA_TYPE_NONE;

    for (std::string& i : mdata_manager_get()->prefixes) {
        std::string chritm_prop_file = i + "chritm_prop.farc";
        aft_data->load_file(aft_data, "rom/", chritm_prop_file.c_str(), item_table_array_load_file);
    }

    light_param_data_storage::load(aft_data);
    auth_3d_data_load_auth_3d_db(aft_auth_3d_db);

    cmn_set_id = aft_mot_db->get_motion_set_id("CMN");
    dbg_set_id = aft_obj_db->get_object_set_id("DBG");

    render_timer->reset();
    for (int32_t i = 0; i < 30; i++) {
        render_timer->start_of_cycle();
        lock_lock(render_lock);
        game_state_ctrl();
        app::TaskWork::Ctrl();
        sound_ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::Basic();
        lock_unlock(render_lock);
        render_timer->end_of_cycle();
    }

    glGenVertexArrays(1, &cube_line_vao);
    glGenBuffers(1, &cube_line_vbo);
    gl_state_bind_vertex_array(cube_line_vao);
    gl_state_bind_array_buffer(cube_line_vbo);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(vec3) * 4, 0, 0);
    else
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

    glGenVertexArrays(1, &grid_vao);
    gl_state_bind_vertex_array(grid_vao);

    glGenBuffers(1, &grid_vbo);
    gl_state_bind_array_buffer(grid_vbo);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(float_t) * 3
            * grid_vertex_count, grid_verts, 0);
    else
        glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * 3
            * grid_vertex_count, grid_verts, GL_STATIC_DRAW);

    glVertexAttrib4f(0, 0.0f, 0.0f, 0.0f, 1.0f);
    glVertexAttribI1i(1, 2);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        sizeof(float_t) * 3, (void*)0);                     // Pos
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_INT,
        sizeof(float_t) * 3, (void*)(sizeof(float_t) * 2)); // Color

    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    free(grid_verts);

    camera* cam = rctx->camera;

    cam->initialize(aspect, internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y);
    //cam->set_position(cam, { 1.35542f, 1.41634f, 1.27852f });
    //cam->rotate(cam, { -45.0, -32.5 });
    //cam->set_position(cam, { -6.67555f, 4.68882f, -3.67537f });
    //cam->rotate(cam, { 136.5, -20.5 });
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    //cam->set_fov(cam, 70.0);
    cam->set_view_point({ 0.0f, 1.375f, 1.0f });
    cam->set_interest({ 0.0f, 1.375f, 0.0f });

    imgui_context = ImGui::CreateContext();
    imgui_context_lock = new lock_cs;

    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = 0;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    lock_unlock(imgui_context_lock);

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

    shaders_ft.env_vert_set(3, 1.0f);
    shaders_ft.env_vert_set(4, 0.0f);
    classes_process_init(classes, classes_count, rctx);
    return rctx;
}

extern double_t input_move_x;
extern double_t input_move_y;
extern double_t input_rotate_x;
extern double_t input_rotate_y;
extern double_t input_roll;
extern bool input_reset;

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
    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    app::TaskWork_Window();
    classes_process_imgui(classes, classes_count);

#if DRAW_PASS_TIME_DISP
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ 1280, 636 }, ImGuiCond_Appearing);

    if (ImGui::Begin("CPU/GPU Time")) {
        static const char* draw_pass_names[] = {
            "Shadow",
            "SS SSS",
            "Pass 2",
            "Reflect",
            "Refract",
            "Pre Process",
            "Clear",
            "Pre Sprite",
            "3D",
            "Show Vector",
            "Post Process",
            "Sprite",
            "Pass 12",
        };

        char buf[0x200];
        for (int32_t i = 0; i < DRAW_PASS_MAX; i++) {
            float_t cpu_min = FLT_MAX;
            float_t cpu_max = 0.0f;
            double_t cpu_avg = 0.0f;
            float_t gpu_min = FLT_MAX;
            float_t gpu_max = 0.0f;
            double_t gpu_avg = 0.0f;

            for (float_t& j : draw_pass_cpu_time[i]) {
                if (cpu_min > j)
                    cpu_min = j;
                else if (cpu_max < j)
                    cpu_max = j;
                cpu_avg += j;
            }

            size_t cpu_time_count = draw_pass_cpu_time[i].size();
            if (cpu_time_count)
                cpu_avg /= (double_t)cpu_time_count;

            for (float_t& j : draw_pass_gpu_time[i]) {
                if (gpu_min > j)
                    gpu_min = j;
                else if (gpu_max < j)
                    gpu_max = j;
                gpu_avg += j;
            }

            size_t gpu_time_count = draw_pass_gpu_time[i].size();
            if (gpu_time_count)
                gpu_avg /= (double_t)gpu_time_count;

            sprintf_s(buf, sizeof(buf), "CPU Draw Pass % 12s; Min: % 3.4f; Max: % 3.4f; Avg: % 3.4lf",
                draw_pass_names[i], cpu_min, cpu_max, cpu_avg);
            ImGui::PlotLines(buf, draw_pass_cpu_time[i].data(), (int32_t)draw_pass_cpu_time[i].size());
            sprintf_s(buf, sizeof(buf), "GPU Draw Pass % 12s; Min: % 3.4f; Max: % 3.4f; Avg: % 3.4lf",
                draw_pass_names[i], gpu_min, gpu_max, gpu_avg);
            ImGui::PlotLines(buf, draw_pass_gpu_time[i].data(), (int32_t)draw_pass_gpu_time[i].size());
        }
        ImGui::End();
    }
#endif

    lock_unlock(imgui_context_lock);

    if (old_width != width || old_height != height || old_scale_index != scale_index) {
        render_resize_fb(rctx, true);
        cam->set_res(internal_3d_res.x, internal_3d_res.y,
            internal_2d_res.x, internal_2d_res.y);
    }
    old_width = width;
    old_height = height;
    old_scale_index = scale_index;

    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    if (global_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right)
        && !ImGui::IsItemHovered(0) && imgui_context->OpenPopupStack.Size < 1)
        ImGui::OpenPopup("Classes init context menu", 0);

    if (ImGui::BeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count, rctx);
        ImGui::EndPopup();
    }
    lock_unlock(imgui_context_lock);

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

    if (Input::IsKeyTapped(GLFW_KEY_F4))
        game_state_set_game_state_next(GAME_STATE_ADVERTISE);
    else if (Input::IsKeyTapped(GLFW_KEY_F5))
        game_state_set_game_state_next(GAME_STATE_GAME);
    else if (Input::IsKeyTapped(GLFW_KEY_F6)) {
        game_state_set_game_state_next(GAME_STATE_DATA_TEST);
        game_state_set_sub_game_state_next(SUB_GAME_STATE_DATA_TEST_AUTH_3D);
    }
    else if (Input::IsKeyTapped(GLFW_KEY_F7))
        game_state_set_game_state_next(GAME_STATE_TEST_MODE);
    else if (Input::IsKeyTapped(GLFW_KEY_F8))
        game_state_set_game_state_next(GAME_STATE_APP_ERROR);

    classes_process_ctrl(classes, classes_count);

    rctx_ptr = rctx;
    game_state_ctrl();
    rctx->ctrl();

    char buf[0x200];
    game_state_print(buf, sizeof(buf));
    glfwSetWindowTitle(window, buf);

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

static void cube_line_disp(shader_glsl* shader, camera* cam, vec3* trans, float_t line_size, vec4* color) {
    mat4 mat[2];
    mat4_translate(&trans[0], &mat[0]);
    mat4_translate(&trans[1], &mat[1]);
    mat4_mult(&cam->view, &mat[0], &mat[0]);
    mat4_mult(&cam->view, &mat[1], &mat[1]);

    vec3 t[2];
    mat4_get_translation(&mat[0], &t[0]);
    mat4_get_translation(&mat[1], &t[1]);

    vec3 d;
    *(vec2*)&d = vec2::normalize(*(vec2*)&t[1] - *(vec2*)&t[0]) * line_size;
    d.z = 0.0f;

    vec3 norm[2];
    norm[0] = { -d.y, d.x, 0.0f };
    norm[1] = { d.y, -d.x, 0.0f };
    mat4_mult_vec3(&cam->inv_view_rot, &norm[0], &norm[0]);
    mat4_mult_vec3(&cam->inv_view_rot, &norm[1], &norm[1]);

    vec3 vert_trans[4];
    vert_trans[0] = trans[0] + norm[0];
    vert_trans[1] = trans[0] + norm[1];
    vert_trans[2] = trans[1] + norm[0];
    vert_trans[3] = trans[1] + norm[1];

    shader->set("color", *color);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_trans), vert_trans);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static int cube_line_points_sort(void const* src1, void const* src2) {
    float_t d1 = ((std::pair<vec3, float_t>*)src1)->second;
    float_t d2 = ((std::pair<vec3, float_t>*)src2)->second;
    return d1 > d2 ? -1 : (d1 < d2 ? 1 : 0);
}

static bool auth_3d_disp = false;
static bool rob_disp = false;

static void render_disp(render_context* rctx) {
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

    cam->update();

    rctx->disp();

    int32_t screen_x_offset = (width - internal_2d_res.x) / 2 + (width - internal_2d_res.x) % 2;
    int32_t screen_y_offset = (height - internal_2d_res.y) / 2 + (width - internal_2d_res.x) % 2;
    glViewport(screen_x_offset, screen_y_offset, internal_2d_res.x, internal_2d_res.y);
    rctx->post_process.screen_texture.bind();
    gl_state_bind_uniform_buffer_base(0, common_data_ubo);
    classes_process_disp(classes, classes_count);

    gl_state_disable_cull_face();
    vec4 bone_color = { 1.0f, 0.0f, 0.0f, 1.0f };
    vec4 cns_color = { 1.0f, 1.0f, 0.0f, 1.0f };
    vec4 exp_color = { 0.0f, 1.0f, 0.0f, 1.0f };
    vec4 osg_color = { 0.0f, 0.0f, 1.0f, 1.0f };
    vec4 osg_node_color = { 1.0f, 0.0f, 1.0f, 1.0f };
    if (rob_disp)
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            if (rob_chara_pv_data_array[i].type == ROB_CHARA_TYPE_NONE)
                continue;

            rob_chara* rob_chr = &rob_chara_array[i];
            if (!rob_chr->get_visibility())
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

                if (trans[0] != trans[1])
                    cube_line_disp(cube_line_shader, cam, trans, CUBE_LINE_SIZE, &bone_color);
            }
            gl_state_bind_array_buffer(0);

            std::vector<std::pair<vec3, float_t>> cube_line_points;
            for (bone_node& j : rob_bone_data->nodes) {
                vec3 trans;
                vec3 vp_trans;
                mat4_get_translation(j.mat, &trans);
                mat4_mult_vec3(&cam->view_projection, &trans, &vp_trans);
                cube_line_points.push_back({ trans, vp_trans.z });
            }

            quicksort_custom(cube_line_points.data(), cube_line_points.size(),
                sizeof(std::pair<vec3, float_t>), cube_line_points_sort);

            gl_state_bind_array_buffer(cube_line_point_instance_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(std::pair<vec3, float_t>)
                * cube_line_points.size()), cube_line_points.data(), GL_STREAM_DRAW);
            gl_state_bind_array_buffer(0);

            vec3 trans[4];
            trans[0] = { -CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
            trans[1] = {  CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
            trans[2] = { -CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };
            trans[3] = {  CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };

            mat4_mult_vec3(&cam->inv_view_rot, &trans[0], &trans[0]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[1], &trans[1]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[2], &trans[2]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[3], &trans[3]);

            gl_state_bind_vertex_array(cube_line_point_vao);
            cube_line_point_shader->use();
            cube_line_point_shader->set("trans", 4, trans);
            cube_line_point_shader->set("border_end",
                ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE)));
            cube_line_point_shader->set("border_start",
                (1.0f - ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE))));
            cube_line_point_shader->set("border_color", 0.0f, 0.0f, 0.0f);
            cube_line_point_shader->set("center_color", 1.0f, 1.0f, 1.0f);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)cube_line_points.size());
            gl_state_bind_vertex_array(0);
            cube_line_points.clear();
            cube_line_points.shrink_to_fit();
        }

    if (auth_3d_disp)
        for (auth_3d& i : auth_3d_data->data) {
            if (i.id == -1 || !i.visible)
                continue;

            for (auth_3d_object_hrc& j : i.object_hrc) {
                if (!j.node[0].model_transform.visible)
                    continue;

                gl_state_bind_vertex_array(cube_line_vao);
                gl_state_bind_array_buffer(cube_line_vbo);
                cube_line_shader->use();
                for (auth_3d_object_node& k : j.node) {
                    if (k.parent == -1 )
                        continue;

                    vec3 trans[2];
                    mat4_get_translation(&j.node[k.parent].model_transform.mat, &trans[0]);
                    mat4_get_translation(&k.model_transform.mat, &trans[1]);

                    if (trans[0] != trans[1])
                        cube_line_disp(cube_line_shader, cam, trans, CUBE_LINE_SIZE, &osg_color);
                }
                gl_state_bind_array_buffer(0);

                std::vector<std::pair<vec3, float_t>> cube_line_points;
                for (auth_3d_object_node& k : j.node) {
                    vec3 trans;
                    vec3 vp_trans;
                    mat4_get_translation(&k.model_transform.mat, &trans);
                    mat4_mult_vec3(&cam->view_projection, &trans, &vp_trans);
                    cube_line_points.push_back({ trans, vp_trans.z });
                }

                quicksort_custom(cube_line_points.data(), cube_line_points.size(),
                    sizeof(std::pair<vec3, float_t>), cube_line_points_sort);

                gl_state_bind_array_buffer(cube_line_point_instance_vbo);
                glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(std::pair<vec3, float_t>)
                    * cube_line_points.size()), cube_line_points.data(), GL_STREAM_DRAW);
                gl_state_bind_array_buffer(0);

                vec3 trans[4];
                trans[0] = { -CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
                trans[1] = {  CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
                trans[2] = { -CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };
                trans[3] = {  CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };

                mat4_mult_vec3(&cam->inv_view_rot, &trans[0], &trans[0]);
                mat4_mult_vec3(&cam->inv_view_rot, &trans[1], &trans[1]);
                mat4_mult_vec3(&cam->inv_view_rot, &trans[2], &trans[2]);
                mat4_mult_vec3(&cam->inv_view_rot, &trans[3], &trans[3]);

                gl_state_bind_vertex_array(cube_line_point_vao);
                cube_line_point_shader->use();
                cube_line_point_shader->set("trans", 4, trans);
                cube_line_point_shader->set("border_end",
                    ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE)));
                cube_line_point_shader->set("border_start",
                    (1.0f - ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE))));
                cube_line_point_shader->set("border_color", 0.0f, 0.0f, 0.0f);
                cube_line_point_shader->set("center_color", 1.0f, 1.0f, 1.0f);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)cube_line_points.size());
                gl_state_bind_vertex_array(0);
                cube_line_points.clear();
                cube_line_points.shrink_to_fit();
            }
        }

    if (rob_disp)
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            if (rob_chara_pv_data_array[i].type == ROB_CHARA_TYPE_NONE)
                continue;

            rob_chara* rob_chr = &rob_chara_array[i];
            if (!rob_chr->get_visibility())
                continue;

            std::vector<std::pair<vec3, float_t>> cube_line_points;
            motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
            mot_key_set* key_set = mot->mot_key_data.mot.key_sets;
            for (bone_data& j : mot->bone_data.bones)
                if (j.type >= BONE_DATABASE_BONE_HEAD_IK_ROTATION
                    && j.type <= BONE_DATABASE_BONE_LEGS_IK_ROTATION) {
                    vec3 trans;
                    mat4_mult_vec3(&cam->view_projection, &j.ik_target, &trans);
                    cube_line_points.push_back({ j.ik_target, trans.z });
                }

            quicksort_custom(cube_line_points.data(), cube_line_points.size(),
                sizeof(std::pair<vec3, float_t>), cube_line_points_sort);

            gl_state_bind_array_buffer(cube_line_point_instance_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(std::pair<vec3, float_t>)
                * cube_line_points.size()), cube_line_points.data(), GL_STREAM_DRAW);
            gl_state_bind_array_buffer(0);

            vec3 trans[4];
            trans[0] = { -CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
            trans[1] = {  CUBE_LINE_POINT_SIZE,  CUBE_LINE_POINT_SIZE, 0.0f };
            trans[2] = { -CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };
            trans[3] = {  CUBE_LINE_POINT_SIZE, -CUBE_LINE_POINT_SIZE, 0.0f };

            mat4_mult_vec3(&cam->inv_view_rot, &trans[0], &trans[0]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[1], &trans[1]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[2], &trans[2]);
            mat4_mult_vec3(&cam->inv_view_rot, &trans[3], &trans[3]);

            gl_state_bind_vertex_array(cube_line_point_vao);
            cube_line_point_shader->use();
            cube_line_point_shader->set("trans", 4, trans);
            cube_line_point_shader->set("border_end",
                ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE)));
            cube_line_point_shader->set("border_start",
                (1.0f - ((CUBE_LINE_POINT_SIZE - (CUBE_LINE_SIZE * 1.125f)) / (2.0f * CUBE_LINE_POINT_SIZE))));
            cube_line_point_shader->set("border_color", 0.0f, 0.0f, 0.0f);
            cube_line_point_shader->set("center_color", 0.0f, 1.0f, 0.0f);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)cube_line_points.size());
            gl_state_bind_vertex_array(0);
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

#if DRAW_PASS_TIME_DISP
    for (int32_t i = 0; i < DRAW_PASS_MAX; i++) {
        float_t* cpu_time = draw_pass_cpu_time[i].data();
        size_t cpu_time_count = draw_pass_cpu_time[i].size();
        if (cpu_time_count) {
            memmove(cpu_time, cpu_time + 1, sizeof(float_t) * (cpu_time_count - 1));
            cpu_time[cpu_time_count - 1] = (float_t)rctx->draw_pass.cpu_time[i];
        }

        float_t* gpu_time = draw_pass_gpu_time[i].data();
        size_t gpu_time_count = draw_pass_gpu_time[i].size();
        if (gpu_time_count) {
            memmove(gpu_time, gpu_time + 1, sizeof(float_t) * (gpu_time_count - 1));
            gpu_time[gpu_time_count - 1] = (float_t)rctx->draw_pass.gpu_time[i];
        }
    }
#endif

    if (draw_imgui)
        render_imgui(rctx);
}

static void render_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imgui_context);
    delete imgui_context_lock;
    imgui_context_lock = 0;

    //pv_game_data.SetDest();
    Glitter::glt_particle_manager_free_task();
    task_auth_3d_free_task();
    task_pv_db_free_task();

    app::TaskWork::Dest();

    sound_work_unload_farc("rom/sound/se.farc");
    sound_work_unload_farc("rom/sound/button.farc");
    sound_work_unload_farc("rom/sound/se_cmn.farc");
    sound_work_unload_farc("rom/sound/se_sel.farc");
    sound_work_unload_farc("rom/sound/se_aime.farc");
    sound_work_unload_farc("rom/sound/pvchange.farc");
    sound_work_unload_farc("rom/sound/slide_se.farc");
    sound_work_unload_farc("rom/sound/slide_long.farc");

    object_storage_unload_set(dbg_set_id);

    //rob_chara_array_free_chara_id(0);
    render_timer->reset();
    while (app::task_work->tasks.size()) {
        render_timer->start_of_cycle();
        lock_lock(render_lock);
        game_state_ctrl();
        app::TaskWork::Ctrl();
        sound_ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::Basic();
        lock_unlock(render_lock);
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

    task_data_test_glitter_particle_free();
    dtw_stg_free();
    auth_3d_test_window_free();

    Glitter::glt_particle_manager_free();
    task_pv_db_free();
    task_effect_free();
    light_param_data_storage_data_free();
    task_stage_modern_free();
    dtm_stg_free();
    task_stage_free();
    task_auth_3d_free();
    auth_3d_test_task_free();
    auth_3d_data_free();
    task_wind_free();
    rob_free();

    glDeleteBuffers(1, &common_data_ubo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteVertexArrays(1, &grid_vao);
    glDeleteBuffers(1, &cube_line_point_instance_vbo);
    glDeleteVertexArrays(1, &cube_line_point_vao);
    glDeleteBuffers(1, &cube_line_vbo);
    glDeleteVertexArrays(1, &cube_line_vao);

    app::task_work_free();

    rand_state_array_free();
    item_table_array_free();
    pv_expression_file_storage_free();
    stage_param_data_storage_free();
    object_storage_free();
    ogg_playback_data_free();
    ogg_file_handler_storage_free();
    wave_audio_storage_free();
    sound_free();
    render_shaders_free();

    data_struct_free();

    osage_setting_data_free();
    mothead_storage_free();
    motion_storage_free();

    render_texture_data_free();

    file_handler_storage_free();
    delete rctx;
}

static void render_imgui(render_context* rctx) {
    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    lock_unlock(imgui_context_lock);
}

static bool render_load_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shader_set_data* set = (shader_set_data*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    set->load(&f, false, "ft", shader_ft_table, shader_ft_table_size,
        shader_ft_bind_func_table, shader_ft_bind_func_table_size);

    return true;
}

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths) {
    if (!count || !paths)
        return;

    classes_process_drop(classes, classes_count, count, paths);
    glfwFocusWindow(window);
}

static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h) {
#if !(BAKE_PNG || BAKE_VIDEO)
    width = w;
    height = h;
#endif
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

    internal_2d_res = vec2i::clamp(internal_res, 1, sv_max_texture_size);
#if BAKE_PNG || BAKE_VIDEO
    internal_3d_res.x = (int32_t)roundf((float_t)(internal_res.x * 2));
    internal_3d_res.y = (int32_t)roundf((float_t)(internal_res.y * 2));
#else
    internal_3d_res.x = (int32_t)roundf((float_t)(internal_res.x * render_scale_table[scale_index]));
    internal_3d_res.y = (int32_t)roundf((float_t)(internal_res.y * render_scale_table[scale_index]));
#endif
    internal_3d_res = vec2i::clamp(internal_3d_res, 1, sv_max_texture_size);

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x
        || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x
        || old_internal_3d_res.y != internal_3d_res.y
        || old_width != width || old_height != height;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    if (fb_changed && change_fb) {
        rctx->post_process.init_fbo(internal_3d_res.x, internal_3d_res.y,
            internal_2d_res.x, internal_2d_res.y, width, height);
        rctx->draw_pass.resize(internal_2d_res.x, internal_2d_res.y);
        rctx->litproj->resize(internal_3d_res.x, internal_3d_res.y);
    }
}

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx) {
#pragma warning(disable:26115)
#pragma warning(disable:26117)
    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (!c->name || ~c->flags & CLASSES_IN_CONTEXT_MENU)
            continue;

        if (c->sub_classes && c->sub_classes_count) {
            if (ImGui::BeginMenu(c->name, ~c->data.flags & CLASS_HIDDEN)) {
                render_imgui_context_menu(c->sub_classes, c->sub_classes_count, rctx);
                ImGui::EndMenu();
            }
        }
        else if (~c->data.flags & CLASS_HIDDEN)
            ImGui::MenuItem(c->name, 0, false, false);
        else if (ImGui::MenuItem(c->name, 0)) {
            if (~c->data.flags & CLASS_INIT) {
                c->data.lock = new lock_cs;
                if (c->data.lock->check_init() && c->init) {
                    lock_lock(c->data.lock);
                    if (c->init(&c->data, rctx))
                        c->data.flags = CLASS_INIT;
                    else
                        c->data.flags = (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN);
                    lock_unlock(c->data.lock);
                }
            }

            if (c->data.lock->check_init()) {
                lock_lock(c->data.lock);
                if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show))
                    enum_and(c->data.flags, ~(CLASS_HIDE | CLASS_HIDDEN | CLASS_HIDE_WINDOW));
                lock_unlock(c->data.lock);
            }
        }
    }
}

static void render_shaders_load() {
    data_list[DATA_AFT].load_file(&shaders_ft, "rom/", "ft_shaders.farc", render_load_shaders);
}

static void render_shaders_free() {
    shaders_ft.unload();
}

#if defined(DEBUG) && OPENGL_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf_debug("########################################\n");
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        printf_debug("Type: Error;                ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf_debug("Type: Deprecated Behaviour; ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf_debug("Type: Undefined Behaviour;  ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf_debug("Type: Portability;          ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf_debug("Type: Performance;          ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf_debug("Type: Marker;               ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf_debug("Type: Push Group;           ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf_debug("Type: Pop Group;            ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf_debug("Type: Other;                ");
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        printf_debug("Severity: high;   ");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf_debug("Severity: medium; ");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf_debug("Severity: low;    ");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf_debug("Severity: notif;  ");
        break;
    }

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        printf_debug("Source: API\n");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf_debug("Source: Window System\n");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf_debug("Source: Shader Compiler\n");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf_debug("Source: Third Party\n");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf_debug("Source: Application\n");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf_debug("Source: Other\n");
        break;
    }

    printf_debug("Debug message (%d): %s\n", id, message);
    printf_debug("########################################\n\n");
}
#endif
