/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1000000
#include "render.hpp"
#include "config.hpp"
#include "../CRE/GL/uniform_buffer.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/Vulkan/command_buffer.hpp"
#include "../CRE/Vulkan/fence.hpp"
#include "../CRE/Vulkan/framebuffer.hpp"
#include "../CRE/Vulkan/image.hpp"
#include "../CRE/Vulkan/image_view.hpp"
#include "../CRE/Vulkan/semaphore.hpp"
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
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_vulkan.h>
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

struct render_opengl_data {
    GLuint grid_vao;
    GLuint grid_vbo;
    GL::UniformBuffer common_data_ubo;

    render_opengl_data();
    ~render_opengl_data();

    bool load();
    void unload();

    void load_common_data();
    void unload_common_data();
    void update_common_data(common_data_struct* common_data, camera* cam);
};

struct render_vulkan_data {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;

        swapchain_support_details();
        ~swapchain_support_details();
    };

    struct pipeline_builder_data {
        VkPipelineShaderStageCreateInfo shader_stages[2];
        VkPipelineVertexInputStateCreateInfo vertex_input;
        VkPipelineInputAssemblyStateCreateInfo input_assembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayout pipeline_layout;
        VkPipelineDepthStencilStateCreateInfo depth_stencil;

        pipeline_builder_data();

        VkResult build_pipeline(VkDevice device, VkRenderPass render_pass, VkPipeline& pipeline);
    };

    VkInstance instance;
#if RENDER_DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    VkSurfaceKHR surface;

    VkPhysicalDevice physical_device;
    VkDevice device;

    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    VkQueue present_queue;
    uint32_t present_queue_family;

    VkSwapchainKHR swapchain;
    VkExtent2D swapchain_extent;
    VkFormat swapchain_image_format;
    std::vector<VkImage> swapchain_images;
    std::vector<Vulkan::ImageView> swapchain_image_views;

    VkRenderPass render_pass;

    std::vector<Vulkan::Framebuffer> swapchain_framebuffers;

    VkCommandPool command_pool;
    Vulkan::CommandBuffer main_command_buffer;

    Vulkan::Semaphore present_semaphore;
    Vulkan::Semaphore render_semaphore;
    Vulkan::Fence render_fence;

    VmaAllocator allocator;

    pipeline_builder_data pipeline_builder;

    VkPipelineLayout triangle_pipeline_layout;

    VkPipeline triangle_pipeline;

    render_vulkan_data();
    ~render_vulkan_data();

    bool load();
    void unload();

    void load_common_data();
    void unload_common_data();
    void update_common_data(common_data_struct* common_data, camera* cam);

    static bool check_device_extension_support(VkPhysicalDevice device);
    static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
    static VkPresentModeKHR choose_swap_present_mode(
        const std::vector<VkPresentModeKHR>& available_present_modes);
    static VkSurfaceFormatKHR choose_swap_surface_format(
        const std::vector<VkSurfaceFormatKHR>& available_formats);
    static std::pair<uint32_t, uint32_t> find_queue_families(
        VkPhysicalDevice device, VkSurfaceKHR surface);
    static bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    static swapchain_support_details query_swapchain_support(
        VkPhysicalDevice device, VkSurfaceKHR surface);

#if RENDER_DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif
};

common_data_struct common_data;

render_opengl_data* render_opengl;
render_vulkan_data* render_vulkan;

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

#if RENDER_DEBUG
static const char* render_vulkan_validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

static const char* render_vulkan_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
vec4 npr_cloth_spec_color;

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

static void render_opengl_shaders_load();
static void render_opengl_shaders_free();
static void render_vulkan_shaders_load();
static void render_vulkan_shaders_free();

static bool render_opengl_load_shaders(void* data, const char* path, const char* file, uint32_t hash);
static bool render_vulkan_load_shaders(void* data, const char* path, const char* file, uint32_t hash);

static void render_drop_glfw(GLFWwindow* window, int32_t count, char** paths);
static void render_resize_fb_glfw(GLFWwindow* window, int32_t w, int32_t h);
static void render_resize_fb(render_context* rctx, bool change_fb);

static void render_imgui_context_menu(classes_data* classes,
    const size_t classes_count, render_context* rctx);

#if RENDER_DEBUG
static void APIENTRY render_debug_output(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length, const char* message, const void* userParam);

static VkResult vkGetInstanceProcAddrCreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
static void vkGetInstanceProcAddrDestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);
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
bool vulkan_render;

int32_t render_main(render_init_struct* ris) {
    render_lock = new lock_cs;
    if (!render_lock)
        return 0;

    render_timer = new timer(60.0);

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

        if (!vulkan_render) {
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
        }

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
            if (!vulkan_render) {
                glViewport(0, 0, width, height);
                gl_state_disable_blend();
                gl_state_disable_depth_test();
                gl_state_set_depth_mask(GL_FALSE);
                gl_state_disable_cull_face();
                gl_state_disable_stencil_test();
            }

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

    if (!vulkan_render) {
        gl_state_enable_blend();
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl_state_set_blend_equation(GL_FUNC_ADD);
        gl_state_enable_depth_test();
        gl_state_set_depth_mask(GL_TRUE);

        shaders_ft.set_opengl_shader(SHADER_FT_GRID);
        render_opengl->common_data_ubo.Bind(0);
        gl_state_bind_vertex_array(render_opengl->grid_vao);
        glDrawArrays(GL_LINES, 0, (GLsizei)grid_vertex_count);
        gl_state_use_program(0);

        gl_state_disable_depth_test();
        gl_state_set_depth_mask(GL_FALSE);
        gl_state_disable_blend();
    }
}

float_t rob_frame = 0.0f;

render_opengl_data::render_opengl_data() : grid_vao(), grid_vbo(), common_data_ubo() {

}

render_opengl_data::~render_opengl_data() {

}

bool render_opengl_data::load() {
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return false;

    glGetError();
    glViewport(0, 0, width, height);
    return true;
}

void render_opengl_data::unload() {

}

void render_opengl_data::load_common_data() {
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

void render_opengl_data::unload_common_data() {
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

void render_opengl_data::update_common_data(common_data_struct* common_data, camera* cam) {
    common_data_ubo.WriteMapMemory(*common_data);
}

render_vulkan_data::swapchain_support_details::swapchain_support_details() : capabilities() {

}

render_vulkan_data::swapchain_support_details::~swapchain_support_details() {

}

render_vulkan_data::pipeline_builder_data::pipeline_builder_data() : shader_stages(),
vertex_input(), input_assembly(), rasterizer(), color_blend_attachment(),
multisampling(), pipeline_layout(), depth_stencil() {

}

VkResult render_vulkan_data::pipeline_builder_data::build_pipeline(VkDevice device,
    VkRenderPass render_pass, VkPipeline& pipeline) {
    VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo color_blending_create_info = {};
    color_blending_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_create_info.logicOpEnable = VK_FALSE;
    color_blending_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blending_create_info.attachmentCount = 1;
    color_blending_create_info.pAttachments = &color_blend_attachment;

    const VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.dynamicStateCount = 2;
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    VkGraphicsPipelineCreateInfo pipeline_info_create_info = {};
    pipeline_info_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info_create_info.stageCount = 2;
    pipeline_info_create_info.pStages = shader_stages;
    pipeline_info_create_info.pVertexInputState = &vertex_input;
    pipeline_info_create_info.pInputAssemblyState = &input_assembly;
    pipeline_info_create_info.pViewportState = &viewport_state_create_info;
    pipeline_info_create_info.pRasterizationState = &rasterizer;
    pipeline_info_create_info.pMultisampleState = &multisampling;
    pipeline_info_create_info.pDepthStencilState = &depth_stencil;
    pipeline_info_create_info.pColorBlendState = &color_blending_create_info;
    pipeline_info_create_info.pDynamicState = &dynamic_state_create_info;
    pipeline_info_create_info.layout = pipeline_layout;
    pipeline_info_create_info.renderPass = render_pass;

    return vkCreateGraphicsPipelines(device, 0, 1, &pipeline_info_create_info, 0, &pipeline);
}

render_vulkan_data::render_vulkan_data() : instance(), surface(), physical_device(), device(),
graphics_queue(), present_queue(), swapchain(), swapchain_extent(), swapchain_image_format(),
render_pass(), command_pool(), triangle_pipeline_layout(), triangle_pipeline(), allocator() {
#if RENDER_DEBUG
    debug_messenger = 0;
#endif
    graphics_queue_family = -1;
    present_queue_family = -1;
}

render_vulkan_data::~render_vulkan_data() {

}

bool render_vulkan_data::load() {
#pragma region Vulkan Validation Layers Support
#if RENDER_DEBUG
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, 0);

    std::vector<VkLayerProperties> available_layers;
    available_layers.resize(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* i : render_vulkan_validation_layers) {
        bool found = false;
        for (const VkLayerProperties& j : available_layers)
            if (!strcmp(i, j.layerName)) {
                found = true;
                break;
            }

        if (!found)
            return false;
    }
#endif
#pragma endregion

#pragma region Vulkan Instance
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = application_name;
    application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;

    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
#if RENDER_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    instance_create_info.enabledExtensionCount = (uint32_t)extensions.size();
    instance_create_info.ppEnabledExtensionNames = extensions.data();

#if RENDER_DEBUG
    instance_create_info.enabledLayerCount = (uint32_t)(
        sizeof(render_vulkan_validation_layers) / sizeof(const char*));
    instance_create_info.ppEnabledLayerNames = render_vulkan_validation_layers;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = render_vulkan_data::debug_callback;

    instance_create_info.pNext = &debug_create_info;
#else
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.pNext = 0;
#endif

    if (vkCreateInstance(&instance_create_info, 0, &instance) != VK_SUCCESS)
        return false;
#pragma endregion

#pragma region Vulkan Debug
#if RENDER_DEBUG
    if (vkGetInstanceProcAddrCreateDebugUtilsMessengerEXT(instance,
        &debug_create_info, 0, &debug_messenger) != VK_SUCCESS)
        return false;
#endif
#pragma endregion

#pragma region Vulkan Surface
    if (glfwCreateWindowSurface(instance, window, 0, &surface) != VK_SUCCESS)
        return false;
#pragma endregion

#pragma region Vulkan Physical Device
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, 0);

    if (!device_count)
        return false;

    std::vector<VkPhysicalDevice> devices;
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for (const VkPhysicalDevice& i : devices)
        if (render_vulkan_data::is_device_suitable(i, surface)) {
            physical_device = i;
            break;
        }

    if (!physical_device)
        return false;
#pragma endregion

#pragma region Vulkan Device
    std::pair<uint32_t, uint32_t> indices = find_queue_families(physical_device, surface);
    if (indices.first == -1 || indices.second == -1)
        return false;

    std::set<uint32_t> unique_queue_families;
    unique_queue_families.insert(indices.first);
    unique_queue_families.insert(indices.second);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.reserve(unique_queue_families.size());

    float_t queue_priority = 1.0f;
    for (uint32_t i : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = i;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    device_create_info.queueCreateInfoCount = (uint32_t)queue_create_infos.size();
    device_create_info.pQueueCreateInfos = queue_create_infos.data();

    device_create_info.pEnabledFeatures = &device_features;

    device_create_info.enabledExtensionCount = (uint32_t)(sizeof(render_vulkan_device_extensions) / sizeof(const char*));
    device_create_info.ppEnabledExtensionNames = render_vulkan_device_extensions;

#if RENDER_DEBUG
    device_create_info.enabledLayerCount = (uint32_t)(
        sizeof(render_vulkan_validation_layers) / sizeof(const char*));
    device_create_info.ppEnabledLayerNames = render_vulkan_validation_layers;
#else
    device_create_info.enabledLayerCount = 0;
#endif

    if (vkCreateDevice(physical_device, &device_create_info, 0, &device) != VK_SUCCESS)
        return false;

    graphics_queue_family = indices.first;
    vkGetDeviceQueue(device, indices.first, 0, &graphics_queue);

    present_queue_family = indices.second;
    vkGetDeviceQueue(device, indices.second, 0, &present_queue);
#pragma endregion

#pragma region Vulkan Swapchain
    swapchain_support_details swapchain_support = query_swapchain_support(physical_device, surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swapchain_support.present_modes);
    VkExtent2D extent = choose_swap_extent(swapchain_support.capabilities);

    uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
    if (swapchain_support.capabilities.maxImageCount > 0
        && image_count > swapchain_support.capabilities.maxImageCount)
        image_count = swapchain_support.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface;

    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[2];
    queue_family_indices[0] = graphics_queue_family;
    queue_family_indices[1] = present_queue_family;

    if (graphics_queue_family != present_queue_family) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = sizeof(queue_family_indices) / sizeof(uint32_t);
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    swapchain_create_info.preTransform = swapchain_support.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;

    swapchain_create_info.oldSwapchain = 0;

    if (vkCreateSwapchainKHR(device, &swapchain_create_info, 0, &swapchain) != VK_SUCCESS)
        return false;

    vkGetSwapchainImagesKHR(device, swapchain, &image_count, 0);

    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

    swapchain_image_format = surface_format.format;
    swapchain_extent = extent;

    size_t swapchain_image_count = swapchain_images.size();
    swapchain_image_views.resize(swapchain_image_count);

    VkComponentMapping components;
    components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange sub_resource_range;
    sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    sub_resource_range.baseMipLevel = 0;
    sub_resource_range.levelCount = 1;
    sub_resource_range.baseArrayLayer = 0;
    sub_resource_range.layerCount = 1;

    for (size_t i = 0; i < swapchain_image_count; i++)
        if (!swapchain_image_views[i].Create(device, 0, swapchain_images[i],
            VK_IMAGE_VIEW_TYPE_2D, swapchain_image_format, components, sub_resource_range))
            return false;
#pragma endregion

#pragma region Vulkan Render Pass
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = swapchain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;

    if (vkCreateRenderPass(device, &render_pass_create_info, 0, &render_pass) != VK_SUCCESS)
        return false;
#pragma endregion

#pragma region Vulkan Framebuffers
    size_t swapchain_image_view_count = swapchain_image_views.size();
    swapchain_framebuffers.resize(swapchain_image_view_count);

    for (size_t i = 0; i < swapchain_image_view_count; i++)
        if (!swapchain_framebuffers[i].Create(device, 0, render_pass, 1,
            &swapchain_image_views[i].data, swapchain_extent.width, swapchain_extent.height, 1))
            return false;
#pragma endregion

#pragma region Vulkan Command Pool
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = graphics_queue_family;

    if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
        return false;
#pragma endregion

#pragma region Vulkan Main Command Buffer
    if (main_command_buffer.Allocate(device, command_pool))
        return false;
#pragma endregion

#pragma region Vulkan Sync
    if (!present_semaphore.Create(device)
        || !render_semaphore.Create(device)
        || !render_fence.Create(device, VK_FENCE_CREATE_SIGNALED_BIT))
        return false;
#pragma endregion

#pragma region Vulkan Allocator
    VmaAllocatorCreateInfo allocator_create_info = {};
    allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_0;
    allocator_create_info.physicalDevice = physical_device;
    allocator_create_info.device = device;
    allocator_create_info.instance = instance;

    if (vmaCreateAllocator(&allocator_create_info, &allocator) != VK_SUCCESS)
        return false;
#pragma endregion
    return true;
}

void render_vulkan_data::unload() {
    vkDeviceWaitIdle(device);

#pragma region Vulkan Allocator
    if (allocator) {
        vmaDestroyAllocator(allocator);
        allocator = 0;
    }
#pragma endregion

#pragma region Vulkan Sync
    render_fence.Destroy();
    render_semaphore.Destroy();
    present_semaphore.Destroy();
#pragma endregion

#pragma region Vulkan Main Command Buffer
    main_command_buffer.Free();
#pragma endregion

#pragma region Vulkan Command Pool
    if (command_pool) {
        vkDestroyCommandPool(device, command_pool, 0);
        command_pool = 0;
    }
#pragma endregion

#pragma region Vulkan Framebuffers
    for (Vulkan::Framebuffer& i : swapchain_framebuffers)
        i.Destroy();
    swapchain_framebuffers.clear();
#pragma endregion

#pragma region Vulkan Render Pass
    if (render_pass) {
        vkDestroyRenderPass(device, render_pass, 0);
        render_pass = 0;
    }
#pragma endregion

#pragma region Vulkan Swapchain
    for (Vulkan::ImageView& i : swapchain_image_views)
        i.Destroy();

    swapchain_images.clear();

    if (swapchain) {
        vkDestroySwapchainKHR(device, swapchain, 0);
        swapchain = 0;
    }
#pragma endregion

#pragma region Vulkan Device
    present_queue_family = -1;
    present_queue = 0;

    graphics_queue_family = -1;
    graphics_queue = 0;

    if (device) {
        vkDestroyDevice(device, 0);
        device = 0;
    }
#pragma endregion

#pragma region Vulkan Physical Device
    physical_device = 0;
#pragma endregion

#pragma region Vulkan Surface
    if (surface) {
        vkDestroySurfaceKHR(instance, surface, 0);
        surface = 0;
    }
#pragma endregion

#pragma region Vulkan Debug
#if RENDER_DEBUG
    if (debug_messenger) {
        vkGetInstanceProcAddrDestroyDebugUtilsMessengerEXT(instance, debug_messenger, 0);
        debug_messenger = 0;
    }
#endif
#pragma endregion

#pragma region Vulkan Instance
    if (instance) {
        vkDestroyInstance(instance, 0);
        instance = 0;
    }
#pragma endregion
}

void render_vulkan_data::load_common_data() {

}

void render_vulkan_data::unload_common_data() {

}

void render_vulkan_data::update_common_data(common_data_struct* common_data, camera* cam) {

}

bool render_vulkan_data::check_device_extension_support(VkPhysicalDevice device) {
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, 0, &extension_count, 0);

    if (!extension_count)
        return true;

    std::vector<VkExtensionProperties> available_extensions;
    available_extensions.resize(extension_count);
    vkEnumerateDeviceExtensionProperties(device, 0, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(render_vulkan_device_extensions,
        render_vulkan_device_extensions + sizeof(render_vulkan_device_extensions) / sizeof(const char*));
    for (const VkExtensionProperties& i : available_extensions)
        required_extensions.erase(i.extensionName);
    return !required_extensions.size();
}

VkExtent2D render_vulkan_data::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    int32_t width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent{ (uint32_t)width, (uint32_t)height, };

    actual_extent.width = clamp_def(actual_extent.width,
        capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = clamp_def(actual_extent.height,
        capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}

VkPresentModeKHR render_vulkan_data::choose_swap_present_mode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
    for (const VkPresentModeKHR& i : available_present_modes) // It'll uncap framerates
        if (i == VK_PRESENT_MODE_MAILBOX_KHR)
            return i;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR render_vulkan_data::choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
    for (const VkSurfaceFormatKHR& i : available_formats)
        if (i.format == VK_FORMAT_B8G8R8A8_UNORM
            && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return i;
    return available_formats[0];
}

std::pair<uint32_t, uint32_t> render_vulkan_data::find_queue_families(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);

    std::vector<VkQueueFamilyProperties> queue_families;
    queue_families.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int32_t j = 0;
    std::pair<uint32_t, uint32_t> indices = { -1, -1 };
    for (const VkQueueFamilyProperties& i : queue_families) {
        if (i.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.first = j;

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &present_support);

        if (present_support)
            indices.second = j;

        if (indices.first != -1 && indices.second != -1)
            return indices;
        j++;
    }
    return { -1, -1 };
}

bool render_vulkan_data::is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    std::pair<uint32_t, uint32_t> indices = find_queue_families(device, surface);

    bool extensions_supported = check_device_extension_support(device);

    bool swapchain_adequate = false;
    if (extensions_supported) {
        swapchain_support_details swapchain_support = query_swapchain_support(device, surface);
        swapchain_adequate = swapchain_support.formats.size() && swapchain_support.present_modes.size();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    return indices.first != -1 && indices.second != -1 && extensions_supported
        && swapchain_adequate && supported_features.samplerAnisotropy;
}

render_vulkan_data::swapchain_support_details render_vulkan_data::query_swapchain_support(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    swapchain_support_details details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, 0);

    if (format_count) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, 0);

    if (present_mode_count) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
            &present_mode_count, details.present_modes.data());
    }

    return details;
}

#if RENDER_DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL render_vulkan_data::debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(console, &csbi);
        SetConsoleTextAttribute(console, FOREGROUND_INTENSITY | FOREGROUND_RED
            | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
        printf_debug("validation layer error: %s\n\n", pCallbackData->pMessage);
        SetConsoleTextAttribute(console, csbi.wAttributes);
    }
    else
        printf_debug("validation layer msg:   %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}
#endif

static bool render_init(render_init_struct* ris) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = ris->res.x > 0 && ris->res.x < 8192 ? ris->res.x : mode->width;
    height = ris->res.y > 0 && ris->res.y < 8192 ? ris->res.y : mode->height;

    vulkan_render = ris->vulkan_render;

    width = (int32_t)(width / 2.0f);
    height = (int32_t)(height / 2.0f);

#if BAKE_PNG || BAKE_VIDEO
    width = 1920;
    height = 1080;
#endif
    if (vulkan_render)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    else
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#if RENDER_DEBUG
    if (!vulkan_render)
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

    if (vulkan_render)
        window = glfwCreateWindow(width, height, application_name, maximized ? monitor : 0, 0);
    else {
        int32_t minor = 6;
        window = 0;
        while (!window || minor < 3) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor--);

            window = glfwCreateWindow(width, height, application_name, maximized ? monitor : 0, 0);
        }
    }

    if (!window)
        return false;

    if (vulkan_render) {
        render_vulkan = new render_vulkan_data;
        if (!render_vulkan->load()) {
            glfwDestroyWindow(window);
            return false;
        }
    }
    else {
        render_opengl = new render_opengl_data;
        if (!render_opengl->load()) {
            glfwDestroyWindow(window);
            return false;
        }
    }
    return true;
}

static void render_main_loop(render_context* rctx) {
    render_timer->reset();
    while (!close && !reload_render) {
        render_timer->start_of_cycle();
        glfwPollEvents();

        if (!vulkan_render) ImGui_ImplGlfw_NewFrame();
        if (vulkan_render);
            //ImGui_ImplVulkan_NewFrame();
        else
            ImGui_ImplOpenGL3_NewFrame();
        if (!vulkan_render) ImGui::NewFrame();

        Input::NewFrame();

        lock_lock(render_lock);
        if (!vulkan_render) render_context_ctrl(rctx);
        lock_unlock(render_lock);
        render_context_disp(rctx);

        close |= !!glfwWindowShouldClose(window);
        frame_counter++;

        Input::EndFrame();

        if (!vulkan_render)
            glfwSwapBuffers(window);
        render_timer->end_of_cycle();
    }
}

static void render_free() {
    if (vulkan_render) {
        render_vulkan->unload();
        delete render_vulkan;
    }
    else {
        render_opengl->unload();
        delete render_opengl;
    }

    glfwDestroyWindow(window);
}

static render_context* render_context_load() {
    data_struct_init();
    data_struct_load("ReDIVA_data.txt");
    data_struct_load_db();

    if (vulkan_render)
        render_vulkan_shaders_load();
    else
        render_opengl_shaders_load();

    texture_storage_init();

    file_handler_storage_init();

    render_context* rctx = new render_context;
    rctx_ptr = rctx;

    if (vulkan_render)
        render_texture_vulkan_data_init(render_vulkan->device,
            render_vulkan->allocator, render_vulkan->command_pool, render_vulkan->graphics_queue);
    else {
        gl_state_get();
        render_texture_opengl_data_init();
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    if (!vulkan_render) { app::task_work_init();
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
        data_struct* x_data = &data_list[DATA_X];
        data_struct* xhd_data = &data_list[DATA_XHD];

        char buf[0x200];
        std::vector<uint32_t> obj_set_ids;
        std::map<uint32_t, std::string> obj_set_id_name;
        for (int32_t i = 800; i <= 831; i++) {
            sprintf_s(buf, sizeof(buf), i == 815 ? "EFFPV%03d" : "ITMPV%03d", i);

            const object_set_info* effpv_set_info;
            if (aft_obj_db->get_object_set_info(buf, &effpv_set_info)) {
                obj_set_ids.push_back(effpv_set_info->id);
                obj_set_id_name.insert({ effpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03d", i);

            const object_set_info* stgpv_set_info;
            if (aft_obj_db->get_object_set_info(buf, &stgpv_set_info)) {
                obj_set_ids.push_back(stgpv_set_info->id);
                obj_set_id_name.insert({ stgpv_set_info->id, buf });
            }

            sprintf_s(buf, sizeof(buf), "STGPV%03dHRC", i);

            const object_set_info* stgpvhrc_set_info;
            if (aft_obj_db->get_object_set_info(buf, &stgpvhrc_set_info)) {
                obj_set_ids.push_back(stgpvhrc_set_info->id);
                obj_set_id_name.insert({ stgpvhrc_set_info->id, buf });
            }

            for (uint32_t& i : obj_set_ids) {
                const object_set_info* set_info;
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

                prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);

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

    render_resize_fb(rctx, false);

    rctx->post_process.init_fbo(internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y, width, height);
    rctx->render_manager.resize(internal_2d_res.x, internal_2d_res.y);

    render_resize_fb(rctx, true);

    Glitter::glt_particle_manager_add_task();
    //app::TaskWork::AddTask(&pv_game_data, "PVGAME", 0);
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_array[i].type = ROB_CHARA_TYPE_NONE;

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

    camera* cam = rctx->camera;

    cam->initialize(aspect, internal_3d_res.x, internal_3d_res.y,
        internal_2d_res.x, internal_2d_res.y);
    //cam->set_position({ 1.35542f, 1.41634f, 1.27852f });
    //cam->rotate({ -45.0, -32.5 });
    //cam->set_position({ -6.67555f, 4.68882f, -3.67537f });
    //cam->rotate({ 136.5, -20.5 });
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });
    //cam->set_fov(70.0);
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f }); }

    if (vulkan_render)
        render_vulkan->load_common_data();
    else
        render_opengl->load_common_data();

    uniform_value[U16] = 1;

    if (vulkan_render) {
#pragma region Vulkan Pipeline Layout
        VkPipelineLayoutCreateInfo triangle_pipeline_layout_create_info = {};
        triangle_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        vkCreatePipelineLayout(render_vulkan->device, &triangle_pipeline_layout_create_info,
            0, &render_vulkan->triangle_pipeline_layout);
#pragma endregion

#pragma region Vulkan Pipeline
        shader_vulkan_pair shader_sun_no_textured = shaders_ft.get_vulkan_shader(SHADER_FT_SUN_NO_TEXTURED);

        render_vulkan_data::pipeline_builder_data& pipeline_builder = render_vulkan->pipeline_builder;

        VkPipelineShaderStageCreateInfo& shader_stage_vert = pipeline_builder.shader_stages[0];
        shader_stage_vert = {};
        shader_stage_vert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_vert.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stage_vert.module = shader_sun_no_textured.first;
        shader_stage_vert.pName = "main";

        VkPipelineShaderStageCreateInfo& shader_stage_frag = pipeline_builder.shader_stages[1];
        shader_stage_frag = {};
        shader_stage_frag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_frag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stage_frag.module = shader_sun_no_textured.second;
        shader_stage_frag.pName = "main";

        VkPipelineVertexInputStateCreateInfo& vertex_input = pipeline_builder.vertex_input;
        vertex_input = {};
        vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        render_texture_vulkan_data_get_vertex_input_binding_descriptions(
            vertex_input.pVertexBindingDescriptions,
            vertex_input.vertexBindingDescriptionCount);
        render_texture_vulkan_data_get_vertex_input_attribute_descriptions(
            vertex_input.pVertexAttributeDescriptions,
            vertex_input.vertexAttributeDescriptionCount);

        VkPipelineInputAssemblyStateCreateInfo& input_assembly = pipeline_builder.input_assembly;
        input_assembly = {};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo& rasterizer = pipeline_builder.rasterizer;
        rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState& color_blend_attachment = pipeline_builder.color_blend_attachment;
        color_blend_attachment = {};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT
            | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineMultisampleStateCreateInfo& multisampling = pipeline_builder.multisampling;
        multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo& depth_stencil = pipeline_builder.depth_stencil;
        depth_stencil = {};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        render_vulkan->pipeline_builder.build_pipeline(render_vulkan->device,
            render_vulkan->render_pass, render_vulkan->triangle_pipeline);
#pragma endregion
    }

    imgui_context_lock = new lock_cs;
    lock_lock(imgui_context_lock);
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = 0;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    lock_unlock(imgui_context_lock);

    ImGui::StyleColorsDark();
    if (vulkan_render) {
        /*ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = render_vulkan->instance;
        init_info.PhysicalDevice = render_vulkan->physical_device;
        init_info.Device = render_vulkan->device;
        init_info.Queue = render_vulkan->graphics_queue;
        init_info.DescriptorPool = render_vulkan->descriptor_pool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_Init(&init_info, render_vulkan->render_pass);*/
    }
    else {
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

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
    app::TaskWork_Window();
    classes_process_imgui(classes, classes_count);
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

    common_data_struct common_data;

    common_data.res.x = (float_t)internal_3d_res.x;
    common_data.res.y = (float_t)internal_3d_res.y;
    common_data.res.z = 1.0f / (float_t)internal_3d_res.x;
    common_data.res.w = 1.0f / (float_t)internal_3d_res.y;
    common_data.vp = cam->view_projection;
    common_data.view = cam->view;
    common_data.projection = cam->projection;
    common_data.view_pos = cam->view_point;

    if (vulkan_render)
        render_vulkan->update_common_data(&common_data, cam);
    else
        render_opengl->update_common_data(&common_data, cam);
}

static void render_context_disp(render_context* rctx) {
    //camera* cam = rctx->camera;

    if (vulkan_render) {
        render_vulkan->render_fence.WaitFor();
        render_vulkan->render_fence.Reset();

        uint32_t swapchain_image_index;
        VkResult result = vkAcquireNextImageKHR(render_vulkan->device, render_vulkan->swapchain,
            UINT64_MAX, render_vulkan->present_semaphore.data, 0, &swapchain_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //vulkanTestRecreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            return;

        VkCommandBuffer cmd = render_vulkan->main_command_buffer.data;

        vkResetCommandBuffer(cmd, 0);

        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        //command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(cmd, &command_buffer_begin_info) != VK_SUCCESS)
            return;

        VkClearValue clear_value;
        float_t flash = (float_t)abs(sin(frame_counter / 120.0));
        clear_value.color = { { 0.0f, 0.0f, flash, 1.0f } };

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        render_pass_begin_info.renderPass = render_vulkan->render_pass;
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent = render_vulkan->swapchain_extent;
        render_pass_begin_info.framebuffer = render_vulkan->swapchain_framebuffers[swapchain_image_index].data;

        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, render_vulkan->triangle_pipeline);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float_t)render_vulkan->swapchain_extent.width;
        viewport.height = (float_t)render_vulkan->swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = render_vulkan->swapchain_extent;
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdEndRenderPass(cmd);
        vkEndCommandBuffer(cmd);

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;

        VkPipelineStageFlags wait_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit_info.pWaitDstStageMask = &wait_stage_flags;

        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &render_vulkan->present_semaphore.data;

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &render_vulkan->render_semaphore.data;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;

        if (vkQueueSubmit(render_vulkan->graphics_queue, 1,
            &submit_info, render_vulkan->render_fence.data) == VK_SUCCESS) {
            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pNext = nullptr;

            present_info.pSwapchains = &render_vulkan->swapchain;
            present_info.swapchainCount = 1;

            present_info.pWaitSemaphores = &render_vulkan->render_semaphore.data;
            present_info.waitSemaphoreCount = 1;

            present_info.pImageIndices = &swapchain_image_index;

            vkQueuePresentKHR(render_vulkan->graphics_queue, &present_info);
        }
    }
    else {
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

        int32_t screen_x_offset = (width - internal_2d_res.x) / 2 + (width - internal_2d_res.x) % 2;
        int32_t screen_y_offset = (height - internal_2d_res.y) / 2 + (width - internal_2d_res.x) % 2;
        glViewport(screen_x_offset, screen_y_offset, internal_2d_res.x, internal_2d_res.y);
        rctx->post_process.screen_texture.bind();
        classes_process_disp(classes, classes_count);
        gl_state_bind_framebuffer(0);

        if (rctx->render_manager.pass_sw[rndr::RND_PASSID_POSTPROCESS])
            fbo::blit(rctx->post_process.screen_texture.fbos[0], 0,
                0, 0, width, height,
                0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        if (draw_imgui)
            render_context_imgui(rctx);
    }
}

static void render_context_imgui(render_context* rctx) {
    lock_lock(imgui_context_lock);
    ImGui::SetCurrentContext(imgui_context);
    if (vulkan_render);
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), render_vulkan->main_command_buffer);
    else
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    lock_unlock(imgui_context_lock);
}

static void render_context_dispose(render_context* rctx) {
    classes_process_dispose(classes, classes_count);

    if (vulkan_render);
        //ImGui_ImplVulkan_Shutdown();
    else
        ImGui_ImplOpenGL3_Shutdown();
    if (!vulkan_render) ImGui_ImplGlfw_Shutdown();
    lock_lock(imgui_context_lock);
    ImGui::DestroyContext(imgui_context);
    lock_unlock(imgui_context_lock);
    delete imgui_context_lock;
    imgui_context_lock = 0;

    if (vulkan_render) {
#pragma region Vulkan Pipeline
        if (render_vulkan->triangle_pipeline) {
            vkDestroyPipeline(render_vulkan->device,
                render_vulkan->triangle_pipeline, 0);
            render_vulkan->triangle_pipeline = 0;
        }
#pragma endregion

#pragma region Vulkan Pipeline Layout
        if (render_vulkan->triangle_pipeline_layout) {
            vkDestroyPipelineLayout(render_vulkan->device,
                render_vulkan->triangle_pipeline_layout, 0);
            render_vulkan->triangle_pipeline_layout = 0;
        }
#pragma endregion
    }

    if (vulkan_render)
        render_vulkan->unload_common_data();
    else
        render_opengl->unload_common_data();

    //pv_game_data.DelTask();
    if (!vulkan_render) { Glitter::glt_particle_manager_del_task();
    task_auth_3d_del_task();
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

    app::task_work_free();}

    if (vulkan_render)
        render_texture_vulkan_data_free(render_vulkan->device, render_vulkan->allocator);
    else
        render_texture_opengl_data_free();

    delete rctx;

    file_handler_storage_free();

    texture_storage_free();

    if (vulkan_render)
        render_vulkan_shaders_free();
    else
        render_opengl_shaders_free();

    data_struct_free();
}

static void render_opengl_shaders_load() {
    data_list[DATA_AFT].load_file(&shaders_ft, "rom/", "ft_shaders.farc", render_opengl_load_shaders);
}

static void render_opengl_shaders_free() {
    shaders_ft.unload_opengl();
}

static void render_vulkan_shaders_load() {
    data_list[DATA_AFT].load_file(&shaders_ft, "rom/", "ft_shaders_spirv.farc", render_vulkan_load_shaders);
}

static void render_vulkan_shaders_free() {
    shaders_ft.unload_vulkan();
}

static bool render_opengl_load_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shader_set_data* set = (shader_set_data*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    set->load_opengl(&f, false, "ft", shader_ft_table, shader_ft_table_size,
        shader_ft_opengl_bind_func_table, shader_ft_opengl_bind_func_table_size,
        shader_ft_get_index_by_name);
    return true;
}

static bool render_vulkan_load_shaders(void* data, const char* path, const char* file, uint32_t hash) {
    shader_set_data* set = (shader_set_data*)data;
    std::string s;
    s.assign(path);
    s.append(file);

    farc f;
    f.read(s.c_str(), true, false);
    set->load_vulkan(render_vulkan->device, render_vulkan->allocator,
        &f, shader_ft_table, shader_ft_table_size,
        shader_ft_vulkan_get_func_table, shader_ft_vulkan_get_func_table_size,
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
        rctx->render_manager.resize(internal_2d_res.x, internal_2d_res.y);
        rctx->litproj->resize(internal_3d_res.x, internal_3d_res.y);
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

static VkResult vkGetInstanceProcAddrCreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    static PFN_vkCreateDebugUtilsMessengerEXT _vkCreateDebugUtilsMessengerEXT;
    if (!_vkCreateDebugUtilsMessengerEXT)
        _vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (_vkCreateDebugUtilsMessengerEXT)
        return _vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void vkGetInstanceProcAddrDestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
    static PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessengerEXT;
    if (!_vkDestroyDebugUtilsMessengerEXT)
        _vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (_vkDestroyDebugUtilsMessengerEXT)
        _vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
#endif
