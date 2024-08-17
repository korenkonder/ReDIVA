/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#define GLFW_EXPOSE_NATIVE_WIN32
#include "app.hpp"
#include "config.hpp"
#include "../CRE/GL/uniform_buffer.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/camera.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/config.hpp"
#include "../CRE/customize_item_table.hpp"
#include "../CRE/data.hpp"
#include "../CRE/effect.hpp"
#include "../CRE/file_handler.hpp"
#include "../CRE/font.hpp"
#include "../CRE/gl_state.hpp"
#include "../CRE/hand_item.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/mdata_manager.hpp"
#include "../CRE/module_table.hpp"
#include "../CRE/object.hpp"
#include "../CRE/ogg_vorbis.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/random.hpp"
#include "../CRE/render.hpp"
#include "../CRE/render_manager.hpp"
#include "../CRE/shader.hpp"
#include "../CRE/shader_dev.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/sprite.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/stage_modern.hpp"
#include "../CRE/stage_param.hpp"
#include "../CRE/static_var.hpp"
#include "../CRE/task.hpp"
#include "../CRE/texture.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/timer.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data_edit/glitter_editor.hpp"
#include "data_edit/rob_chara_adjust.hpp"
#include "data_edit/selector.hpp"
#include "data_test/auth_2d_test.hpp"
#include "data_test/auth_3d_test.hpp"
#include "data_test/equip_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/motion_test.hpp"
#include "data_test/object_test.hpp"
#include "data_test/opd_test.hpp"
#include "data_test/rob_osage_test.hpp"
#include "data_test/selector.hpp"
#include "data_test/stage_test.hpp"
#include "information/dw_console.hpp"
#include "pv_game/pv_game.hpp"
#include "classes.hpp"
#include "game_state.hpp"
#include "font_info.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"
#include "input_state.hpp"
#include "shared.hpp"
#include "task_window.hpp"
#include "x_pv_game.hpp"
#if BAKE_VIDEO
#include <glad/glad_wgl.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <timeapi.h>

#ifdef DEBUG
#define RENDER_DEBUG 1
#endif

#define CUBE_LINE_SIZE (0.0025f)
#define CUBE_LINE_POINT_SIZE (CUBE_LINE_SIZE * 1.5f)

timer* render_timer;

#define grid_size 50.0f
#define grid_spacing 1.0f
size_t grid_vertex_count = ((size_t)(grid_size / grid_spacing) * 2 + 1) * 4;

struct common_data_struct {
    vec4 res; //x=width, y=height, z=1/width, w=1/height
    mat4 vp;
    mat4 view;
    mat4 projection;
    vec3 view_pos;
};

struct render_data {
    render_data();
    ~render_data();

    bool load();
    void unload();
};

common_data_struct common_data;

render_data* render;

ImFont* imgui_font_arial;

bool draw_imgui = true;

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

#if defined(DEBUG)
static const char* application_name = "ReDIVA Debug";
#else
static const char* application_name = "ReDIVA";
#endif

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

static const int32_t fast_loader_speed = 60;

bool light_chara_ambient;
vec4 npr_cloth_spec_color = 1.0f;

uint32_t cmn_set_id;            // 2
uint32_t dbg_set_id;            // 2

uint32_t aet_gam_loadsc_set_id; // 26
uint32_t spr_gam_loadsc_set_id; // 32

uint32_t aet_cmn_all_set_id;    // 35
uint32_t spr_cmn_all_set_id;    // 34

uint32_t aet_gam_cmn_set_id;    // 3
uint32_t spr_gam_cmn_set_id;    // 9

uint32_t spr_fnt_24_set_id;     // 4
uint32_t spr_fnt_bold24_set_id; // 472
uint32_t spr_fnt_cmn_set_id;    // 43

#if DISPLAY_IBL
struct cubemap_display_batch_shader_data {
    vec4 g_vp[4];
    vec4 g_texture_lod;
};

bool display_ibl = false;
int32_t ibl_index = 0;
int32_t ibl_scale = 1;
GLuint ibl_vao;
GL::ArrayBuffer ibl_vbo;
GL::UniformBuffer cubemap_display_ubo;
#endif

static bool app_init(const app_init_struct& ais);
static void app_main_loop(render_context* rctx);
static void app_free();

static render_context* render_context_load();
static void render_context_ctrl(render_context* rctx);
static void render_context_disp(render_context* rctx);
static void render_context_imgui(render_context* rctx);
static void render_context_dispose(render_context* rctx);

static void render_shaders_load();
static void render_shaders_free();

static bool app_load_ft_shaders(void* data, const char* path, const char* file, uint32_t hash);
static bool app_load_dev_shaders(void* data, const char* path, const char* file, uint32_t hash);

static void app_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void app_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void app_resize_fb(render_context* rctx, bool change_fb);

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx);

#if RENDER_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);
#endif

bool close;
bool reload_render;
HWND window_handle;
GLFWwindow* window;
ImGuiContext* imgui_context;
bool global_context_menu;
render_context* rctx_ptr;
bool task_stage_is_modern;

#if BAKE_VIDEO
ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_context;
HANDLE d3d_gl_handle;
#endif

int32_t app_main(const app_init_struct& ais) {
#if BAKE_FAST
    render_timer = new timer(600.0);
#else
    render_timer = new timer(60.0);
#endif

    window_handle = 0;

    if (!glfwInit())
        return -1;

    if (app_init(ais)) {
        window_handle = glfwGetWin32Window(window);
        glfwShowWindow(window);
        glfwFocusWindow(window);
        glfwSetDropCallback(window, (GLFWdropfun)app_drop_glfw);
        glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)app_resize_fb_glfw);
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
        old_width = width;
        old_height = height;
#endif

        scale_index = ais.scale_index > 0 && ais.scale_index < RENDER_SCALE_MAX
            ? ais.scale_index : RENDER_SCALE_100;
        old_scale_index = scale_index;

#if RENDER_DEBUG
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

        do {
            close = false;
            reload_render = false;

            render_context* rctx = render_context_load();

#if !(BAKE_PNG || BAKE_VIDEO)
            glfwGetFramebufferSize(window, &width, &height);
#endif
            gl_state_set_viewport(0, 0, width, height);
            gl_state_disable_blend();
            gl_state_disable_depth_test();
            gl_state_set_depth_mask(GL_FALSE);
            gl_state_disable_cull_face();
            gl_state_disable_stencil_test();

            glfwSwapInterval(0);

            app_main_loop(rctx);

            render_context_dispose(rctx);
        } while (reload_render);
    }

    app_free();
    glfwTerminate();

    delete render_timer;
    render_timer = 0;
    return 0;
}

double_t app_get_render_scale() {
    return render_scale_table[scale_index];
}

int32_t app_get_render_scale_index() {
    return scale_index;
}

void app_set_render_scale_index(int32_t index) {
    scale_index = index >= 0 && index < RENDER_SCALE_MAX ? index : RENDER_SCALE_100;
}

void app_swap_buffers() {
    glfwSwapBuffers(window);
}

float_t rob_frame = 0.0f;

render_data::render_data() {

}

render_data::~render_data() {

}

bool render_data::load() {
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return false;

#if BAKE_VIDEO
    if (!gladLoadWGLLoader((GLADloadproc)glfwGetProcAddress, GetDC(window_handle)))
        return false;

    D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0,
        D3D11_SDK_VERSION, &d3d_device, 0, &d3d_device_context);

    if (GLAD_WGL_NV_DX_interop2)
        d3d_gl_handle = wglDXOpenDeviceNV(d3d_device);
#endif

    gl_state_get_error();
    gl_state_set_viewport(0, 0, width, height);

#if DISPLAY_IBL
    const float_t box_vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &ibl_vao);
    gl_state_bind_vertex_array(ibl_vao, true);

    ibl_vbo.Create(sizeof(box_vertices), box_vertices);
    ibl_vbo.Bind(true);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 3, (void*)0);

    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    cubemap_display_ubo.Create(sizeof(cubemap_display_batch_shader_data));
#endif

    return true;
}

void render_data::unload() {
#if DISPLAY_IBL
    cubemap_display_ubo.Destroy();

    ibl_vbo.Destroy();

    if (ibl_vao) {
        glDeleteVertexArrays(1, &ibl_vao);
        ibl_vao = 0;
    }
#endif

#if BAKE_VIDEO
    if (GLAD_WGL_NV_DX_interop2)
        wglDXCloseDeviceNV(d3d_device);

    d3d_device_context->Release();
    d3d_device_context = 0;
    d3d_device->Release();
    d3d_device = 0;
#endif
}

static bool app_init(const app_init_struct& ais) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ais.res.x > 0 && ais.res.x < 8192 ? ais.res.x : mode->width;
    height = ais.res.y > 0 && ais.res.y < 8192 ? ais.res.y : mode->height;

    width = (int32_t)(width / 2.0f);
    height = (int32_t)(height / 2.0f);

    width = 1280;
    height = 720;

    res_window_set(RESOLUTION_MODE_HD);

#if BAKE_PNG || BAKE_VIDEO
    width = BAKE_BASE_WIDTH;
    height = BAKE_BASE_HEIGHT;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#if RENDER_DEBUG
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

        window = glfwCreateWindow(width, height, application_name, maximized ? monitor : 0, 0);
    }

    if (!window)
        return false;

    render = new render_data;
    if (!render->load()) {
        glfwDestroyWindow(window);
        return false;
    }
    return true;
}

static void app_main_loop(render_context* rctx) {
    render_timer->reset();
    while (!close && !reload_render) {
        render_timer->start_of_cycle();
        glfwPollEvents();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        Input::NewFrame();

        render_context_ctrl(rctx);
        render_context_disp(rctx);

        close |= !!glfwWindowShouldClose(window);

        Input::EndFrame();

        app_swap_buffers();
        render_timer->end_of_cycle();
    }
}

static void app_free() {
    render->unload();
    delete render;

    glfwDestroyWindow(window);
}

static render_context* render_context_load() {
    system_work_init(13);

    // Enable the dw_gui widgets
    sub_140194880(1);

    data_struct_init();
    data_struct_load("ReDIVA_data.txt");
    data_struct_load_db();

    render_shaders_load();

    texture_manager_init();

    file_handler_storage_init();

    render_context* rctx = new render_context;
    rctx_ptr = rctx;

    gl_state_get();

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    app::task_work_init();
    motion_init();
    skin_param_data_init();

    skin_param_data_load();

    input_state_init();
    fontmap_data_init();
    font_info_default_init();
    dw_console_c_buff_array_init();
    sound_init();
    wave_audio_storage_init();
    ogg_file_handler_storage_init();
    ogg_playback_data_init();

    game_state_init();

    render_manager_init_data(0, 0, 0, false);
    /*render_manager_init_data(
        stru_140EDA5B0.ssaa,
        stru_140EDA5B0.hd_res,
        stru_140EDA5B0.ss_alpha_mask,
        stru_140EDA5B0.screen_shot_4x == 1);*/

    objset_info_storage_init(aft_obj_db);
    stage_param_data_init();
    pv_expression_file_storage_init();
    item_table_handler_array_init();
    rand_state_array_init();

    rob_init();
    task_wind_init();
    aet_manager_init();
    sprite_manager_init();
    auth_3d_data_init();
    task_auth_3d_init();
    task_stage_init();
    task_stage_modern_init();
    light_param_data_storage_data_init();
    effect_init();
    task_pv_db_init();
    Glitter::glt_particle_manager_init();

    dw_init();

    data_edit_sel_init();
    data_test_sel_init();
    auth_3d_test_task_init();
    dtm_aet_init();
    dtm_stg_init();
    motion_test_init();
    object_test_init();
    opd_test_init();
    equip_test_init();
    rob_osage_test_init();
    rob_chara_adjust_init();
    task_data_test_glitter_particle_init();
    x_pv_game_data_init();

    osage_play_database_load();

    Glitter::glt_particle_manager->bone_data = aft_bone_data;

    if (false) {
        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 800; i <= 831; i++) {
            sprintf_s(buf, sizeof(buf), i == 815 ? "EFFPV%03d" : "ITMPV%03d", i);

            const object_set_info* effpv_set_info = aft_obj_db->get_object_set_info(buf);
            if (effpv_set_info) {
                obj_set_ids.push_back(effpv_set_info->id);
                obj_set_id_name.insert({ effpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03d", i);

            const object_set_info* stgpv_set_info = aft_obj_db->get_object_set_info(buf);
            if (stgpv_set_info) {
                obj_set_ids.push_back(stgpv_set_info->id);
                obj_set_id_name.insert({ stgpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i);

            const object_set_info* stgpvhrc_set_info = aft_obj_db->get_object_set_info(buf);
            if (stgpvhrc_set_info) {
                obj_set_ids.push_back(stgpvhrc_set_info->id);
                obj_set_id_name.insert({ stgpvhrc_set_info->id, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                const object_set_info* set_info = aft_obj_db->get_object_set_info(i);
                if (!set_info)
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

                prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);

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
        }
    }

    if (false) {
        const std::string mmp_base_path("R:\\SteamLibrary\\steamapps\\common\\"
            "Hatsune Miku Project DIVA Mega Mix Plus\\");

        const std::pair<const char*, const char*> mmp_paths[] = {
            {   "diva_main\\rom_steam\\",       "" },
            { "diva_main\\rom_ps4_dlc\\", "mdata_" },
            {   "mods\\F2nd Song Pack\\",   "mod_" },
            {      "mods\\X Song Pack\\",   "mod_" },
        };

        const std::string mmp_x_pack_path = mmp_base_path + "mods\\X Song Pack\\rom\\objset\\";

        object_database obj_db;
        for (auto& i : mmp_paths) {
            object_database_file obj_db_file;
            obj_db_file.read((mmp_base_path + i.first + "rom\\objset\\" + i.second + "obj_db").c_str(), false);
            obj_db.add(&obj_db_file);
        }

        texture_database tex_db;
        for (auto& i : mmp_paths) {
            texture_database_file tex_db_file;
            tex_db_file.read((mmp_base_path + i.first + "rom\\objset\\" + i.second + "tex_db").c_str(), false);
            tex_db.add(&tex_db_file);
        }

        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 800; i <= 831; i++) {
            sprintf_s(buf, sizeof(buf), i == 815 ? "EFFPV%03d" : "ITMPV%03d", i);

            const object_set_info* effpv_set_info = obj_db.get_object_set_info(buf);
            if (effpv_set_info) {
                obj_set_ids.push_back(effpv_set_info->id);
                obj_set_id_name.insert({ effpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03d", i);

            const object_set_info* stgpv_set_info = obj_db.get_object_set_info(buf);
            if (stgpv_set_info) {
                obj_set_ids.push_back(stgpv_set_info->id);
                obj_set_id_name.insert({ stgpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i);

            const object_set_info* stgpvhrc_set_info = obj_db.get_object_set_info(buf);
            if (stgpvhrc_set_info) {
                obj_set_ids.push_back(stgpvhrc_set_info->id);
                obj_set_id_name.insert({ stgpvhrc_set_info->id, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                const object_set_info* set_info = obj_db.get_object_set_info(i);
                if (!set_info)
                    continue;

                farc f;
                if (!farc::load_file(&f, mmp_x_pack_path.c_str(), set_info->archive_file_name.c_str(), hash_murmurhash_empty))
                    continue;

                farc_file* obj_ff = f.read_file(set_info->object_file_name.c_str());
                if (!obj_ff)
                    continue;

                farc_file* tex_ff = f.read_file(set_info->texture_file_name.c_str());
                if (!tex_ff)
                    continue;

                prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);

                obj_set obj_set;
                obj_set.unpack_file(alloc, obj_ff->data, obj_ff->size, false);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file(tex_ff->data, false))
                    continue;

                object_material_msgpack_write("patch\\MMp_orig\\objset", obj_set_id_name[i].c_str(),
                    i, &obj_set, &txp_set, &obj_db, &tex_db);
            }
        }
    }

    if (false) {
        data_struct* x_data = &data_list[DATA_X];

        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 801; i <= 832; i++) {
            uint32_t effpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03d", i));
            if (x_data->check_file_exists("root+/objset", effpv_hash)) {
                obj_set_ids.push_back(effpv_hash);
                obj_set_id_name.insert({ effpv_hash, buf });
            }

            uint32_t effpvptc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03dPTC", i));
            if (x_data->check_file_exists("root+/objset", effpvptc_hash)) {
                obj_set_ids.push_back(effpvptc_hash);
                obj_set_id_name.insert({ effpvptc_hash, buf });
            }

            uint32_t effstgpvptc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFSTGPV%03dPTC", i % 100));
            if (x_data->check_file_exists("root+/objset", effstgpvptc_hash)) {
                obj_set_ids.push_back(effstgpvptc_hash);
                obj_set_id_name.insert({ effstgpvptc_hash, buf });
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

                prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);

                obj_set obj_set;
                obj_set.unpack_file(alloc, osd->data, osd->size, true);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file_modern(txd->data, txd->size, 'MTXD'))
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
        }
    }

    if (false) {
        data_struct* xhd_data = &data_list[DATA_XHD];

        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 801; i <= 832; i++) {
            uint32_t effpv_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03d", i));
            if (xhd_data->check_file_exists("root+/objset", effpv_hash)) {
                obj_set_ids.push_back(effpv_hash);
                obj_set_id_name.insert({ effpv_hash, buf });
            }

            uint32_t effpvptc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFPV%03dPTC", i));
            if (xhd_data->check_file_exists("root+/objset", effpvptc_hash)) {
                obj_set_ids.push_back(effpvptc_hash);
                obj_set_id_name.insert({ effpvptc_hash, buf });
            }

            uint32_t effstgpvptc_hash = hash_murmurhash(buf, sprintf_s(buf, sizeof(buf), "EFFSTGPV%03dPTC", i % 100));
            if (xhd_data->check_file_exists("root+/objset", effstgpvptc_hash)) {
                obj_set_ids.push_back(effstgpvptc_hash);
                obj_set_id_name.insert({ effstgpvptc_hash, buf });
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

                prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);

                obj_set obj_set;
                obj_set.unpack_file(alloc, osd->data, osd->size, true);
                if (!obj_set.ready)
                    continue;

                txp_set txp_set;
                if (!txp_set.unpack_file_modern(txd->data, txd->size, 'MTXD'))
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

    app_resize_fb(rctx, false);

    rctx->resize(internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y, width, height);

    rctx->camera->initialize(aspect);

    app_resize_fb(rctx, true);

    rctx->init();

    Glitter::glt_particle_manager_add_task();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_array_reset_pv_data(i);

    light_param_data_storage_data_load();

    auth_3d_data_load_auth_3d_db(aft_auth_3d_db);

    cmn_set_id = aft_mot_db->get_motion_set_id("CMN");
    dbg_set_id = aft_obj_db->get_object_set_id("DBG");

    aet_gam_loadsc_set_id = aft_aet_db->get_aet_set_id_by_name("AET_GAM_LOADSC");
    spr_gam_loadsc_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_GAM_LOADSC");

    aet_cmn_all_set_id = aft_aet_db->get_aet_set_id_by_name("AET_CMN_ALL");
    spr_cmn_all_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_CMN_ALL");

    aet_gam_cmn_set_id = aft_aet_db->get_aet_set_id_by_name("AET_GAM_CMN");
    spr_gam_cmn_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_GAM_CMN");

    spr_fnt_24_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_FNT_24");
    spr_fnt_bold24_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_FNT_BOLD24");
    spr_fnt_cmn_set_id = aft_spr_db->get_spr_set_id_by_name("SPR_FNT_CMN");

    hand_item_handler_data_init();
    module_table_handler_data_init();
    module_data_handler_data_init();
    customize_item_table_handler_data_init();
    customize_item_data_handler_data_init();

    aet_manager_add_aet_sets(aft_aet_db);
    sprite_manager_add_spr_sets(aft_spr_db);

    render_timer->reset();
    for (int32_t i = 0; i < 30; i++) {
        render_timer->start_of_cycle();
        game_state_ctrl();
        app::TaskWork::ctrl();
        sound_ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::basic();
        render_timer->end_of_cycle();
    }

    camera* cam = rctx->camera;

    //cam->set_position({ 1.35542f, 1.41634f, 1.27852f });
    //cam->rotate({ -45.0, -32.5 });
    //cam->set_position({ -6.67555f, 4.68882f, -3.67537f });
    //cam->rotate({ 136.5, -20.5 });
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    //cam->set_fov(70.0);
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f });

    uniform_value[U16] = 1;

    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = 0;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontDefault();

    const char* font_file = "C:\\Windows\\Fonts\\ArialUni.ttf";

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
    builder.AddChar((ImWchar)0x2200);
    builder.BuildRanges(&ranges);

    imgui_font_arial = io.Fonts->AddFontFromFileTTF(font_file, 14.0f, 0, ranges.Data);
    io.Fonts->Build();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    clear_color = 0xFF000000;
    set_clear_color = true;

    rctx->obj_batch.g_blend_color = 1.0f;
    rctx->obj_batch.g_offset_color = 0.0f;
    classes_process_init(classes, classes_count, rctx);
    return rctx;
}

extern double_t input_move_x;
extern double_t input_move_y;
extern double_t input_rotate_x;
extern double_t input_rotate_y;
extern double_t input_roll;
extern bool input_reset;
#if PV_DEBUG
extern bool pv_x;
#endif

static void render_context_ctrl(render_context* rctx) {
    camera* cam = rctx->camera;

    for (int32_t i = 0; i < 32; i++) {
        if (!gl_state_check_texture_binding_2d(i) && !gl_state_check_texture_binding_cube_map(i))
            continue;

        gl_state_active_bind_texture_2d(i, 0);
        gl_state_active_bind_texture_cube_map(i, 0);
        gl_state_bind_sampler(i, 0);
    }

    global_context_menu = true;
    ImGui::SetCurrentContext(imgui_context);
    app::TaskWork_window();
    classes_process_imgui(classes, classes_count);

    if (old_width != width || old_height != height || old_scale_index != scale_index)
        app_resize_fb(rctx, true);
    old_width = width;
    old_height = height;
    old_scale_index = scale_index;

    ImGui::SetCurrentContext(imgui_context);
    if (global_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right)
        && !ImGui::IsItemHovered(0) && imgui_context->OpenPopupStack.Size < 1)
        ImGui::OpenPopup("Classes init context menu", 0);

    if (ImGui::BeginPopupContextItem("Classes init context menu", 0)) {
        render_imgui_context_menu(classes, classes_count, rctx);
        ImGui::EndPopup();
    }

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
            cam->set_view_point({ 0.0f, 1.4f, 1.0f });
            cam->set_interest({ 0.0f, 1.4f, 0.0f });
        }
        else {
            cam->rotate((float_t)input_rotate_x, (float_t)input_rotate_y);
            cam->move((float_t)input_move_x, (float_t)input_move_y);
            if (input_roll != 0.0)
                cam->set_roll(cam->get_roll() + (float_t)input_roll);
        }
    }

    if (Input::IsKeyTapped(GLFW_KEY_F4))
        game_state_set_game_state_next(GAME_STATE_ADVERTISE);
#if PV_DEBUG
    else if (Input::IsKeyTapped(GLFW_KEY_F5, GLFW_MOD_CONTROL)) {
        pv_x = false;
        game_state_set_game_state_next(GAME_STATE_GAME);
    }
    else if (Input::IsKeyTapped(GLFW_KEY_F5)) {
        pv_x = true;
        game_state_set_game_state_next(GAME_STATE_GAME);
    }
#else
    else if (Input::IsKeyTapped(GLFW_KEY_F5))
        game_state_set_game_state_next(GAME_STATE_GAME);
#endif
    else if (Input::IsKeyTapped(GLFW_KEY_F6))
        game_state_set_game_state_next(GAME_STATE_DATA_TEST);
    else if (Input::IsKeyTapped(GLFW_KEY_F7))
        game_state_set_game_state_next(GAME_STATE_TEST_MODE);
    else if (Input::IsKeyTapped(GLFW_KEY_F8))
        game_state_set_game_state_next(GAME_STATE_APP_ERROR);
    else if (Input::IsKeyTapped(GLFW_KEY_F9)) // Added
        game_state_set_game_state_next(GAME_STATE_DATA_EDIT); // Added
#if DISPLAY_IBL
    else if (Input::IsKeyTapped(GLFW_KEY_L))
        display_ibl ^= true;
    else if (Input::IsKeyTapped(GLFW_KEY_1))
        ibl_index = 0;
    else if (Input::IsKeyTapped(GLFW_KEY_2))
        ibl_index = 1;
    else if (Input::IsKeyTapped(GLFW_KEY_3))
        ibl_index = 2;
    else if (Input::IsKeyTapped(GLFW_KEY_4))
        ibl_index = 3;
    else if (Input::IsKeyTapped(GLFW_KEY_5))
        ibl_index = 4;
    else if (Input::IsKeyTapped(GLFW_KEY_6))
        ibl_index = 5;
    else if (Input::IsKeyTapped(GLFW_KEY_7))
        ibl_scale = max_def(ibl_scale - 1, 1);
    else if (Input::IsKeyTapped(GLFW_KEY_8))
        ibl_scale = min_def(ibl_scale + 1, 64);
#endif

    classes_process_ctrl(classes, classes_count);

    rctx_ptr = rctx;
    input_state_ctrl();
    if (!get_pause() || !game_state_get_pause())
        game_state_ctrl();
    rctx->ctrl();

    if (fast_loader_speed > 1 && game_state_get_game_state() == GAME_STATE_STARTUP)
        for (int32_t i = 1; i < fast_loader_speed; i++) {
            rctx_ptr = rctx;
            input_state_ctrl();
            if (!get_pause() || !game_state_get_pause())
                game_state_ctrl();
            rctx->ctrl();
        }

    char buf[0x200];
    game_state_print(buf, sizeof(buf));
    glfwSetWindowTitle(window, buf);

    ImGui::Render();
}

static void render_context_disp(render_context* rctx) {
    static const vec4 color_clear = 0.0f;
    static const GLfloat depth_clear = 1.0f;
    static const GLint stencil_clear = 0;

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
    glClearBufferfv(GL_COLOR, 0, (float_t*)&color_clear);
    glClearDepthf(depth_clear);
    gl_state_set_stencil_mask(0x00);
    gl_state_set_depth_mask(GL_FALSE);

    gl_state_set_viewport(0, 0, internal_3d_res.x, internal_3d_res.y);

    rctx->disp();

#if DISPLAY_IBL
    if (display_ibl) {
        rctx->screen_buffer.Bind();

        mat4 mat;
        mat4_translate_y(1.4f, &mat);
        mat4_scale_rot(&mat, 1.125f, 1.125f, 1.125f, &mat);
        mat4_mul(&mat, &rctx->camera->view, &mat);
        //mat4_clear_trans(&mat, &mat);
        mat4_mul(&mat, &rctx->camera->projection, &mat);

        cubemap_display_batch_shader_data shader_data = {};
        mat4_transpose(&mat, &mat);
        shader_data.g_vp[0] = mat.row0;
        shader_data.g_vp[1] = mat.row1;
        shader_data.g_vp[2] = mat.row2;
        shader_data.g_vp[3] = mat.row3;
        shader_data.g_texture_lod.x = ibl_index == 1 ? 1.0f : 0.0f;
        shader_data.g_texture_lod.y = 1.0f / (float_t)ibl_scale;
        rctx_ptr->glitter_batch_ubo.WriteMemory(shader_data);

        rctx_ptr->glitter_batch_ubo.Bind(3);
        shaders_dev.set(SHADER_DEV_CUBEMAP_DISPLAY);
        gl_state_bind_vertex_array(ibl_vao);
        gl_state_active_bind_texture_cube_map(0,
            light_param_data_storage_data_get_ibl_texture(max_def(ibl_index - 1, 0)));
        shaders_dev.draw_arrays(GL_TRIANGLES, 0, 36);
        gl_state_bind_vertex_array(0);
    }
#endif

#if BAKE_PNG || BAKE_VIDEO
    fbo_blit(rctx->screen_buffer.fbos[0], 0,
        0, 0, rctx->sprite_width, rctx->sprite_height,
        0, 0, rctx->screen_width, rctx->screen_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#else
    fbo_blit(rctx->screen_buffer.fbos[0], 0,
        0, 0, rctx->sprite_width, rctx->sprite_height,
        rctx->screen_x_offset, rctx->screen_y_offset,
        rctx->sprite_width, rctx->sprite_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif

    gl_state_set_viewport(0, 0, rctx->screen_width, rctx->screen_height);

    if (draw_imgui)
        render_context_imgui(rctx);
}

static void render_context_imgui(render_context* rctx) {
    ImGui::SetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void render_context_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imgui_context);

    Glitter::glt_particle_manager_del_task();

    rctx->free();

    task_auth_3d_del_task();
    aet_manager_del_task();
    task_pv_db_del_task();

    app::TaskWork::dest();

    sound_work_unload_farc("rom/sound/se.farc");
    sound_work_unload_farc("rom/sound/button.farc");
    sound_work_unload_farc("rom/sound/se_cmn.farc");
    sound_work_unload_farc("rom/sound/se_sel.farc");
    sound_work_unload_farc("rom/sound/se_aime.farc");
    sound_work_unload_farc("rom/sound/pvchange.farc");
    sound_work_unload_farc("rom/sound/slide_se.farc");
    sound_work_unload_farc("rom/sound/slide_long.farc");

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    objset_info_storage_unload_set(dbg_set_id);
    aet_manager_unload_set(aet_gam_loadsc_set_id, aft_aet_db);
    sprite_manager_unload_set(spr_gam_loadsc_set_id, aft_spr_db);
    aet_manager_unload_set(aet_cmn_all_set_id, aft_aet_db);
    sprite_manager_unload_set(spr_cmn_all_set_id, aft_spr_db);
    sprite_manager_unload_set(spr_fnt_24_set_id, aft_spr_db);
    sprite_manager_unload_set(spr_fnt_bold24_set_id, aft_spr_db);
    sprite_manager_unload_set(spr_fnt_cmn_set_id, aft_spr_db);

    sprite_manager_remove_spr_sets(aft_spr_db);
    aet_manager_remove_aet_sets(aft_aet_db);

    customize_item_data_handler_data_free();
    customize_item_table_handler_data_free();
    module_data_handler_data_free();
    module_table_handler_data_free();
    hand_item_handler_data_free();

    render_timer->reset();
    while (app::task_work->tasks.size()) {
        render_timer->start_of_cycle();
        game_state_ctrl();
        app::TaskWork::ctrl();
        sound_ctrl();
        file_handler_storage_ctrl();
        app::TaskWork::basic();
        render_timer->end_of_cycle();
    }

    light_param_data_storage_data_unload();

    x_pv_game_data_free();
    task_data_test_glitter_particle_free();
    rob_chara_adjust_free();
    rob_osage_test_free();
    equip_test_free();
    opd_test_free();
    object_test_free();
    motion_test_free();
    dtm_stg_free();
    dtm_aet_free();
    auth_3d_test_task_free();
    data_test_sel_free();
    data_edit_sel_free();

    render_manager_free_data();

    dw_free();

    Glitter::glt_particle_manager_free();
    task_pv_db_free();
    effect_free();
    light_param_data_storage_data_free();
    task_stage_modern_free();
    task_stage_free();
    task_auth_3d_free();
    auth_3d_data_free();
    sprite_manager_free();
    aet_manager_free();
    task_wind_free();
    rob_free();

    rand_state_array_free();
    item_table_handler_array_free();
    pv_expression_file_storage_free();
    stage_param_data_free();
    objset_info_storage_free();

    ogg_playback_data_free();
    ogg_file_handler_storage_free();
    wave_audio_storage_free();
    sound_free();
    dw_console_c_buff_array_free();
    font_info_default_free();
    fontmap_data_free();
    input_state_free();

    skin_param_data_free();
    motion_free();

    app::task_work_free();

    delete rctx;

    file_handler_storage_free();

    texture_manager_free();

    render_shaders_free();

    data_struct_free();
}

struct shaders_load_struct {
    shader_set_data* set;
    const char* name;
};

static void render_shaders_load() {
    shaders_load_struct load_ft = { &shaders_ft, "ft" };
    data_list[DATA_AFT].load_file(&load_ft, "rom/", "ft_shaders.farc", app_load_ft_shaders);

    shaders_load_struct load_dev = { &shaders_dev, "dev" };
    data_list[DATA_AFT].load_file(&load_dev, "rom/", "dev_shaders.farc", app_load_dev_shaders);
}

static void render_shaders_free() {
    shaders_dev.unload();
    shaders_ft.unload();
}

static bool app_load_ft_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shaders_load_struct* load = (shaders_load_struct*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    load->set->load(&f, false, load->name, shader_ft_table, shader_ft_table_size,
        shader_ft_bind_func_table, shader_ft_bind_func_table_size,
        shader_ft_get_index_by_name, shader_ft_get_name_by_index);
    return true;
}

static bool app_load_dev_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shaders_load_struct* load = (shaders_load_struct*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    load->set->load(&f, false, load->name, shader_dev_table, shader_dev_table_size,
        0, 0, 0, 0);
    return true;
}

static void app_drop_glfw(GLFWwindow* window, int32_t count, char** paths) {
    if (!count || !paths)
        return;

    if (app::TaskWork::has_task(&glitter_editor)) {
        glitter_editor.file.assign(paths[0]);
        glitter_editor.load_popup = true;
    }

    glfwFocusWindow(window);
}

static void app_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h) {
#if !(BAKE_PNG || BAKE_VIDEO)
    width = w;
    height = h;
#endif
}

static void app_resize_fb(render_context* rctx, bool change_fb) {
    internal_3d_res = vec2i::max(internal_3d_res, 20);

    double_t res_width = (double_t)width;
    double_t res_height = (double_t)height;
    double_t view_aspect = res_width / res_height;
    if (view_aspect < aspect)
        res_height = round(res_width / aspect);
    else if (view_aspect > aspect)
        res_width = round(res_height * aspect);

#if BAKE_PNG || BAKE_VIDEO
    vec2i internal_res = { (int32_t)res_width * BAKE_RES_SCALE, (int32_t)res_height * BAKE_RES_SCALE };
    internal_2d_res = vec2i::clamp(internal_res, 1, sv_max_texture_size);
    internal_3d_res.x = (int32_t)prj::roundf((float_t)internal_res.x);
    internal_3d_res.y = (int32_t)prj::roundf((float_t)internal_res.y);
    internal_3d_res = vec2i::clamp(internal_3d_res, 1, sv_max_texture_size);
#else
    vec2i internal_res = { (int32_t)res_width, (int32_t)res_height };
    internal_2d_res = vec2i::clamp(internal_res, 1, sv_max_texture_size);
    internal_3d_res.x = (int32_t)prj::roundf((float_t)(internal_res.x * render_scale_table[scale_index]));
    internal_3d_res.y = (int32_t)prj::roundf((float_t)(internal_res.y * render_scale_table[scale_index]));
    internal_3d_res = vec2i::clamp(internal_3d_res, 1, sv_max_texture_size);
#endif

    bool fb_changed = old_internal_2d_res.x != internal_2d_res.x
        || old_internal_2d_res.y != internal_2d_res.y
        || old_internal_3d_res.x != internal_3d_res.x
        || old_internal_3d_res.y != internal_3d_res.y
        || old_width != width || old_height != height;
    old_internal_2d_res = internal_2d_res;
    old_internal_3d_res = internal_3d_res;

    if (fb_changed && change_fb)
        rctx->resize(internal_3d_res.x, internal_3d_res.y,
            internal_2d_res.x, internal_2d_res.y, width, height);
}

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx) {
#pragma warning(disable:26115)
#pragma warning(disable:26117)
    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (!c->name || !(c->flags & CLASSES_IN_CONTEXT_MENU))
            continue;

        if (c->sub_classes && c->sub_classes_count) {
            if (ImGui::BeginMenu(c->name, !(c->data.flags & CLASS_HIDDEN))) {
                render_imgui_context_menu(c->sub_classes, c->sub_classes_count, rctx);
                ImGui::EndMenu();
            }
        }
        else if (c->data.flags & CLASS_DW) {
            if (ImGui::MenuItem(c->name, 0))
                if (c->init)
                    c->init(0, 0);
        }
        else if (!(c->data.flags & CLASS_HIDDEN))
            ImGui::MenuItem(c->name, 0, false, false);
        else if (ImGui::MenuItem(c->name, 0)) {
            if (!(c->data.flags & CLASS_INIT)) {
                if (c->init) {
                    if (c->init(&c->data, rctx))
                        c->data.flags = CLASS_INIT;
                    else
                        c->data.flags = (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN);
                }
            }

            if (c->data.flags & CLASS_INIT)
                enum_and(c->data.flags, ~(CLASS_HIDE | CLASS_HIDDEN | CLASS_HIDE_WINDOW));        }
    }
}

#if RENDER_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (!id && severity == GL_DEBUG_SEVERITY_NOTIFICATION
        || id == 131169 || id == 131185 || id == 131218 || id == 131204)
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
