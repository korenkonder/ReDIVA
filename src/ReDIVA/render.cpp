/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#define GLFW_EXPOSE_NATIVE_WIN32
#include "render.hpp"
#include "config.hpp"
#include "../CRE/GL/uniform_buffer.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/camera.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/data.hpp"
#include "../CRE/fbo.hpp"
#include "../CRE/file_handler.hpp"
#include "../CRE/gl_state.hpp"
#include "../CRE/hand_item.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/lock.hpp"
#include "../CRE/mdata_manager.hpp"
#include "../CRE/object.hpp"
#include "../CRE/ogg_vorbis.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/random.hpp"
#include "../CRE/shader.hpp"
#include "../CRE/shader_dev.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/sprite.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/stage_modern.hpp"
#include "../CRE/stage_param.hpp"
#include "../CRE/static_var.hpp"
#include "../CRE/task.hpp"
#include "../CRE/task_effect.hpp"
#include "../CRE/texture.hpp"
#include "../CRE/post_process.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/timer.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data_test/auth_2d_test.hpp"
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
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <timeapi.h>

#if defined(DEBUG)
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
    GLuint grid_vao;
    GLuint grid_vbo;
    GL::UniformBuffer common_data_ubo;

    render_data();
    ~render_data();

    bool load();
    void unload();

    void load_common_data();
    void unload_common_data();
    void update_common_data(common_data_struct* common_data, camera* cam);
};

common_data_struct common_data;

render_data* render;

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

#if defined(DEBUG)
#if defined(ReDIVA_DEV)
static const char* application_name = "ReDIVADev Debug";
#else
static const char* application_name = "ReDIVA Debug";
#endif
#else
#if defined(ReDIVA_DEV)
static const char* application_name = "ReDIVADev";
#else
static const char* application_name = "ReDIVA";
#endif
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

bool light_chara_ambient;
vec4 npr_cloth_spec_color = 1.0f;

uint32_t cmn_set_id;
uint32_t dbg_set_id;

static bool render_init(render_init_struct* ris);
static void render_main_loop(render_context* rctx);
static void render_free();

static render_context* render_context_load();
static void render_context_ctrl(render_context* rctx);
static void render_context_disp(render_context* rctx);
static void render_context_imgui(render_context* rctx);
static void render_context_dispose(render_context* rctx);

static void render_shaders_load();
static void render_shaders_free();

static bool render_load_shaders(void* data, const char* path, const char* file, uint32_t hash);

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(render_context* rctx, bool change_fb);

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx);

#if RENDER_DEBUG
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

int32_t render_main(render_init_struct* ris) {
    render_lock = new lock_cs;
    if (!render_lock)
        return 0;

    render_timer = new timer(600.0);

    window_handle = 0;

    if (!glfwInit())
        return -1;

    if (render_init(ris)) {
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

            render_context* rctx = 0;
            lock_lock(render_lock);
            rctx = render_context_load();
            lock_unlock(render_lock);

            frame_counter = 0;

#if !(BAKE_PNG || BAKE_VIDEO)
            glfwGetFramebufferSize(window, &width, &height);
#endif
            glViewport(0, 0, width, height);
            gl_state_disable_blend();
            gl_state_disable_depth_test();
            gl_state_set_depth_mask(GL_FALSE);
            gl_state_disable_cull_face();
            gl_state_disable_stencil_test();

            glfwSwapInterval(0);

            render_main_loop(rctx);

            lock_lock(render_lock);
            render_context_dispose(rctx);
            lock_unlock(render_lock);
        } while (reload_render);
    }

    render_free();
    glfwTerminate();

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

void draw_pass_3d_grid(render_context* rctx) {
    rctx->camera->update_data();

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_set_blend_equation(GL_FUNC_ADD);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);

    shaders_ft.set(SHADER_FT_GRID);
    render->common_data_ubo.Bind(0);
    gl_state_bind_vertex_array(render->grid_vao);
    glDrawArrays(GL_LINES, 0, (GLsizei)grid_vertex_count);
    gl_state_use_program(0);

    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_blend();
}

float_t rob_frame = 0.0f;

render_data::render_data() : grid_vao(), grid_vbo(), common_data_ubo() {

}

render_data::~render_data() {

}

bool render_data::load() {
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return false;

    gl_state_get_error();
    glViewport(0, 0, width, height);
    return true;
}

void render_data::unload() {

}

void render_data::load_common_data() {
    common_data_ubo.Create(sizeof(common_data_struct));

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
}

void render_data::unload_common_data() {
    common_data_ubo.Destroy();

    if (grid_vbo) {
        glDeleteBuffers(1, &grid_vbo);
        grid_vbo = 0;
    }

    if (grid_vao) {
        glDeleteVertexArrays(1, &grid_vao);
        grid_vao = 0;
    }
}

void render_data::update_common_data(common_data_struct* common_data, camera* cam) {
    common_data_ubo.WriteMapMemory(*common_data);
}

static bool render_init(render_init_struct* ris) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ris->res.x > 0 && ris->res.x < 8192 ? ris->res.x : mode->width;
    height = ris->res.y > 0 && ris->res.y < 8192 ? ris->res.y : mode->height;

    width = (int32_t)(width / 2.0f);
    height = (int32_t)(height / 2.0f);

    res_window_set(RESOLUTION_MODE_HD);

#if BAKE_PNG || BAKE_VIDEO
    width = 1920;
    height = 1080;
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

static void render_main_loop(render_context* rctx) {
    render_timer->reset();
    while (!close && !reload_render) {
        render_timer->start_of_cycle();
        glfwPollEvents();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        Input::NewFrame();

        lock_lock(render_lock);
        render_context_ctrl(rctx);
        lock_unlock(render_lock);
        render_context_disp(rctx);

        close |= !!glfwWindowShouldClose(window);
        frame_counter++;

        Input::EndFrame();

        glfwSwapBuffers(window);
        render_timer->end_of_cycle();
    }
}

static void render_free() {
    render->unload();
    delete render;

    glfwDestroyWindow(window);
}

static render_context* render_context_load() {
    data_struct_init();
    data_struct_load("ReDIVA_data.txt");
    data_struct_load_db();

    render_shaders_load();

    texture_storage_init();

    file_handler_storage_init();

    render_context* rctx = new render_context;
    rctx_ptr = rctx;

    gl_state_get();
    render_texture_data_init();

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
    mothead_storage_init();
    skin_param_data_init();

    game_state_init();

    skin_param_data_load();

    sound_init();
    wave_audio_storage_init();
    ogg_file_handler_storage_init();
    ogg_playback_data_init();
    object_storage_init(aft_obj_db);
    stage_param_data_storage_init();
    pv_expression_file_storage_init();
    item_table_handler_array_init();
    rand_state_array_init();

    rob_init();
    task_wind_init();
    aet_manager_init();
    sprite_manager_init();
    auth_3d_data_init();
    auth_3d_test_task_init();
    task_auth_3d_init();
    task_stage_init();
    task_stage_modern_init();
    light_param_data_storage_data_init();
    task_effect_init();
    task_pv_db_init();
    Glitter::glt_particle_manager_init();

    dw_gui_detail_display_init();

    auth_3d_test_window_init();
    dtm_aet_init();
    dtm_stg_init();
    task_data_test_glitter_particle_init();

    Glitter::glt_particle_manager->bone_data = aft_bone_data;

    if (false) {
        data_struct* x_data = &data_list[DATA_X];
        data_struct* xhd_data = &data_list[DATA_XHD];

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

    render_resize_fb(rctx, false);

    rctx->post_process.init_fbo(internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y, width, height);
    rctx->render_manager.resize(internal_2d_res.x, internal_2d_res.y);
    rctx->litproj->resize(internal_3d_res.x, internal_3d_res.y);

    render_resize_fb(rctx, true);

    Glitter::glt_particle_manager_add_task();
    //app::TaskWork::AddTask(&pv_game_data, "PVGAME", 0);
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_array[i].type = ROB_CHARA_TYPE_NONE;

    light_param_data_storage::load(aft_data);
    auth_3d_data_load_auth_3d_db(aft_auth_3d_db);

    cmn_set_id = aft_mot_db->get_motion_set_id("CMN");
    dbg_set_id = aft_obj_db->get_object_set_id("DBG");

    hand_item_handler_data_init();

    aet_manager_add_aet_sets(aft_aet_db);
    sprite_manager_add_spr_sets(aft_spr_db);

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

    camera* cam = rctx->camera;

    resolution_struct* res_wind = res_window_get();
    cam->initialize(aspect, internal_3d_res.x, internal_3d_res.y,
        res_wind->width, res_wind->height);
    //cam->set_position({ 1.35542f, 1.41634f, 1.27852f });
    //cam->rotate({ -45.0, -32.5 });
    //cam->set_position({ -6.67555f, 4.68882f, -3.67537f });
    //cam->rotate({ 136.5, -20.5 });
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    //cam->set_fov(70.0);
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f });

    render->load_common_data();

    uniform_value[U16] = 1;

    imgui_context_lock = new lock_cs;
    lock_lock(imgui_context_lock);
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = 0;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    lock_unlock(imgui_context_lock);

    ImGui::StyleColorsDark();
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
    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    dw_gui_ctrl_disp();
    app::TaskWork_Window();
    classes_process_imgui(classes, classes_count);
    lock_unlock(imgui_context_lock);

    if (old_width != width || old_height != height || old_scale_index != scale_index) {
        render_resize_fb(rctx, true);
        resolution_struct* res_wind = res_window_get();
        cam->set_res(internal_3d_res.x, internal_3d_res.y,
            res_wind->width, res_wind->height);
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
            cam->set_view_point({ 0.0f, 1.4f, 1.0f });
            cam->set_interest({ 0.0f, 1.4f, 0.0f });
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
#if DATA_EDIT
        game_state_set_game_state_next(GAME_STATE_DATA_EDIT); // Added
    else if (Input::IsKeyTapped(GLFW_KEY_F9)) // Added
#endif
        game_state_set_game_state_next(GAME_STATE_APP_ERROR);

    classes_process_ctrl(classes, classes_count);

    rctx_ptr = rctx;
    game_state_ctrl();
    rctx->ctrl();

    char buf[0x200];
    game_state_print(buf, sizeof(buf));
    glfwSetWindowTitle(window, buf);

    ImGui::Render();

    common_data_struct common_data = {};
    common_data.res.x = (float_t)internal_3d_res.x;
    common_data.res.y = (float_t)internal_3d_res.y;
    common_data.res.z = 1.0f / (float_t)internal_3d_res.x;
    common_data.res.w = 1.0f / (float_t)internal_3d_res.y;
    common_data.vp = cam->view_projection;
    common_data.view = cam->view;
    common_data.projection = cam->projection;
    common_data.view_pos = cam->view_point;
    render->update_common_data(&common_data, cam);
}

static void render_context_disp(render_context* rctx) {
    camera* cam = rctx->camera;

    static const GLfloat color_clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
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
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearDepthf(depth_clear);
    gl_state_set_stencil_mask(0x00);
    gl_state_set_depth_mask(GL_FALSE);

    glViewport(0, 0, internal_3d_res.x, internal_3d_res.y);

    rctx->post_process.rend_texture.bind();
    gl_state_set_depth_mask(GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, color_clear);
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);
    gl_state_set_depth_mask(GL_FALSE);

    cam->update();

    rctx->disp();

#if BAKE_PNG || BAKE_VIDEO
    fbo::blit(rctx->post_process.screen_texture.fbos[0], 0,
        0, 0, rctx->post_process.sprite_width, rctx->post_process.sprite_height,
        0, 0, rctx->post_process.screen_width, rctx->post_process.screen_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#else
    fbo::blit(rctx->post_process.screen_texture.fbos[0], 0,
        0, 0, rctx->post_process.sprite_width, rctx->post_process.sprite_height,
        rctx->post_process.screen_x_offset, rctx->post_process.screen_y_offset,
        rctx->post_process.sprite_width, rctx->post_process.sprite_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif

    glViewport(0, 0, rctx->post_process.screen_width, rctx->post_process.screen_height);
    classes_process_disp(classes, classes_count);

    if (draw_imgui)
        render_context_imgui(rctx);
}

static void render_context_imgui(render_context* rctx) {
    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    lock_unlock(imgui_context_lock);
}

static void render_context_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    lock_lock(imgui_context_lock);
    ImGui::DestroyContext(imgui_context);
    lock_unlock(imgui_context_lock);
    delete imgui_context_lock;
    imgui_context_lock = 0;

    render->unload_common_data();

    //pv_game_data.DelTask();
    Glitter::glt_particle_manager_del_task();
    task_auth_3d_del_task();
    aet_manager_del_task();
    task_pv_db_del_task();

    app::TaskWork::Dest();

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

    object_storage_unload_set(dbg_set_id);
    aet_manager_unload_set(26, aft_aet_db);
    sprite_manager_unload_set(32, aft_spr_db);
    aet_manager_unload_set(35, aft_aet_db);
    sprite_manager_unload_set(34, aft_spr_db);
    sprite_manager_unload_set(4, aft_spr_db);
    sprite_manager_unload_set(472, aft_spr_db);
    sprite_manager_unload_set(43, aft_spr_db);

    sprite_manager_remove_spr_sets(aft_spr_db);
    aet_manager_remove_aet_sets(aft_aet_db);

    hand_item_handler_data_free();

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

    task_data_test_glitter_particle_free();
    dtm_stg_free();
    dtm_aet_free();
    auth_3d_test_window_free();

    dw_gui_detail_display_free();

    Glitter::glt_particle_manager_free();
    task_pv_db_free();
    task_effect_free();
    light_param_data_storage_data_free();
    task_stage_modern_free();
    task_stage_free();
    task_auth_3d_free();
    auth_3d_test_task_free();
    auth_3d_data_free();
    sprite_manager_free();
    aet_manager_free();
    task_wind_free();
    rob_free();

    rand_state_array_free();
    item_table_handler_array_free();
    pv_expression_file_storage_free();
    stage_param_data_storage_free();
    object_storage_free();
    ogg_playback_data_free();
    ogg_file_handler_storage_free();
    wave_audio_storage_free();
    sound_free();

    skin_param_data_free();
    mothead_storage_free();
    motion_free();

    app::task_work_free();

    render_texture_data_free();

    delete rctx;

    file_handler_storage_free();

    texture_storage_free();

    render_shaders_free();

    data_struct_free();
}

struct shaders_load_struct {
    shader_set_data* set;
    const char* name;
};

static void render_shaders_load() {
    shaders_load_struct load_ft = { &shaders_ft, "ft" };
    data_list[DATA_AFT].load_file(&load_ft, "rom/", "ft_shaders.farc", render_load_shaders);
#if ReDIVA_DEV
    shaders_load_struct load_dev = { &shaders_dev, "dev" };
    data_list[DATA_AFT].load_file(&load_dev, "rom/", "dev_shaders.farc", render_load_shaders);
#endif
}

static void render_shaders_free() {
#if ReDIVA_DEV
    shaders_dev.unload();
#endif
    shaders_ft.unload();
}

static bool render_load_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shaders_load_struct* load = (shaders_load_struct*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    load->set->load(&f, false, load->name, shader_ft_table, shader_ft_table_size,
        shader_ft_bind_func_table, shader_ft_bind_func_table_size,
        shader_ft_get_index_by_name);
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
    if (internal_3d_res.x < 20)
        internal_3d_res.x = 20;
    if (internal_3d_res.y < 20)
        internal_3d_res.y = 20;

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

#if BAKE_PNG || BAKE_VIDEO
    internal_2d_res.x = clamp_def(internal_res.x * 2, 1, sv_max_texture_size);
    internal_2d_res.y = clamp_def(internal_res.y * 2, 1, sv_max_texture_size);
    internal_3d_res.x = (int32_t)roundf((float_t)(internal_res.x * 2));
    internal_3d_res.y = (int32_t)roundf((float_t)(internal_res.y * 2));
#else
    internal_2d_res = vec2i::clamp(internal_res, 1, sv_max_texture_size);
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
        rctx->render_manager.resize(internal_2d_res.x, internal_2d_res.y);
        rctx->litproj->resize(internal_3d_res.x, internal_3d_res.y);
        sprite_manager_set_res((double_t)internal_2d_res.x / (double_t)internal_2d_res.y,
            internal_2d_res.x, internal_2d_res.y);
    }
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
        else if (!(c->data.flags & CLASS_HIDDEN))
            ImGui::MenuItem(c->name, 0, false, false);
        else if (ImGui::MenuItem(c->name, 0)) {
            if (!(c->data.flags & CLASS_INIT)) {
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

#if RENDER_DEBUG
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
