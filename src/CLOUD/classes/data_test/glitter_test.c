/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.h"
#include "../../../KKdLib/io/path.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../KKdLib/vector.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/effect_group.h"
#include "../../../CRE/Glitter/file_reader.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/Glitter/scene.h"
#include "../../../CRE/data.h"
#include "../../../CRE/render_context.h"
#include "../../../CRE/stage.h"
#include "../../input.h"
#include "../data_test.h"
#include "../imgui_helper.h"
#include "stage_test.h"

class TaskDataTestGlitterParticle : public TaskWindow {
public:
    uint64_t hash;
    GlitterSceneCounter scene_counter;
    float_t frame;
    bool auto_and_repeat;
    bool reload;
    bool pv_mode;
    bool show_grid;
    bool rebuild_geff;

    std::vector<std::string> geff;
    size_t geff_index;

    std::vector<data_struct_file> files;
    size_t file_index;
    bool load_file;

    bool input_play;
    bool input_stop;
    double_t delta_frame;

    bool stage_test;

    TaskDataTestGlitterParticle();
    virtual ~TaskDataTestGlitterParticle();
    virtual bool Init();
    virtual bool Ctrl();
    virtual bool Dest();
    virtual void Window();

    void LoadFile(const char* file);
};

TaskDataTestGlitterParticle task_data_test_glitter_particle;

extern int32_t width;
extern int32_t height;
extern render_context* rctx_ptr;
extern vec3 clear_color;
extern bool input_reset;
extern bool input_locked;
extern bool draw_grid_3d;

bool task_data_test_glitter_particle_load() {
    TaskWork::AppendTask(&task_data_test_glitter_particle, "DATA_TEST_PARTICLE");
    dtm_stg_load(0);
    //dtw_stg_load(1);
    return true;
}

bool task_data_test_glitter_particle_unload() {
    //dtw_stg_unload();
    dtm_stg_unload();
    //if (data_test_chr.CheckTaskReady())
    //    data_test_chr.SetDest();
    task_data_test_glitter_particle.SetDest();
    return true;
}

TaskDataTestGlitterParticle::TaskDataTestGlitterParticle() : hash(), scene_counter(), frame(),
auto_and_repeat(), reload(), pv_mode(), show_grid(), rebuild_geff(), geff_index(),
file_index(), load_file(), input_play(), input_stop(), delta_frame(), stage_test() {

}

TaskDataTestGlitterParticle::~TaskDataTestGlitterParticle() {

}

bool TaskDataTestGlitterParticle::Init() {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    GPM_VAL.counter = (uint32_t)(time.LowPart * 0x0CAD3078ULL);

    data_list[DATA_AFT].get_directory_files("rom/particle/", &files);
    for (std::vector<data_struct_file>::iterator i = files.begin(); i != files.end();)
        if (str_utils_check_ends_with(i->name.c_str(), ".farc")) {
            char* temp = str_utils_get_without_extension(i->name.c_str());
            i->name = temp ? std::string(temp) : std::string();
            free(temp);
            i++;
        }
        else
            i = files.erase(i);

    GPM_VAL.emission = 1.0f;
    GPM_VAL.draw_all = false;
    GPM_VAL.draw_all_mesh = false;
    dtm_stg_load(0);

    clear_color = { (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0) };

    camera* cam = rctx_ptr->camera;
    camera_reset(cam);
    vec3 view_point = { 0.0f, 0.88f, 4.3f };
    camera_set_view_point(cam, &view_point);
    vec3 interest = { 0.0f, 1.0f, 0.0f };
    camera_set_interest(cam, &interest);

    hash = hash_fnv1a64m_empty;
    scene_counter = 0;
    frame = 0.0f;
    auto_and_repeat = false;
    reload = false;
    pv_mode = false;
    show_grid = false;
    rebuild_geff = false;
    file_index = -1;
    return true;
}

bool TaskDataTestGlitterParticle::Ctrl() {
    GPM_VAL.rctx = rctx_ptr;

    if (load_file && file_index < files.size())
        LoadFile(files[file_index].name.c_str());
    load_file = false;

    if (input_play)
        reload = true;
    else if (input_stop)
        GPM_VAL.FreeSceneEffect(scene_counter, hash_fnv1a64m_empty, true);

    if (!GPM_VAL.CheckNoFileReaders(hash)) {
        reload = false;
        return false;
    }

    if (rebuild_geff) {
        int32_t effects_count = (int32_t)GPM_VAL.GetEffectsCount(this->hash);
        geff.clear();
        geff.push_back(std::string("ALL"));
        for (int32_t i = 0; i < effects_count; i++)
            geff.push_back(std::string(GPM_VAL.GetEffectName(hash, i)));
        rebuild_geff = false;
    }

    if (GPM_VAL.SceneHasNotEnded(scene_counter) && !GPM_VAL.GetPause())
        frame += sys_frame_rate.frame_speed;

    if (reload || auto_and_repeat && !GPM_VAL.SceneHasNotEnded(scene_counter)) {
        GPM_VAL.FreeSceneEffect(scene_counter, hash_fnv1a64m_empty, true);
        uint64_t effect_hash = hash_fnv1a64m_empty;

        if (geff_index)
            effect_hash = GPM_VAL.CalculateHash(geff[geff_index].c_str());

        if (!pv_mode)
            scene_counter = GPM_VAL.Load(hash, effect_hash, 0);
        else if (effect_hash == hash_fnv1a64m_empty)
            scene_counter = GPM_VAL.LoadScene(hash, hash_fnv1a64m_empty);
        else
            scene_counter = GPM_VAL.LoadScene(hash_fnv1a64m_empty, effect_hash);
        frame = 0.0f;
        reload = false;
    }

    input_play = false;
    input_stop = false;
    return false;
}

bool TaskDataTestGlitterParticle::Dest() {
    GPM_VAL.FreeScenes();
    GPM_VAL.FreeEffects();
    GPM_VAL.SetPause(false);
    show_window = false;
    return true;
}

void TaskDataTestGlitterParticle::Window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 280.0f;
    float_t h = 278.0f;

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    if (!ImGui::Begin("Glitter Test##Data Test", 0, window_flags)) {
        ImGui::End();
        return;
    }

    if (imguiColumnComboBoxConfigFile("File", files.data(),
        files.size(), &file_index, 0, false, &window_focus)) {
        load_file = true;
        input_stop = true;
    }

    if (imguiButton("Reset Camera (R)"))
        input_reset = true;

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Play (F)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_F))
        input_play = true;

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Stop (V)", { w, 0.0f }) || ImGui::IsKeyPressed(GLFW_KEY_V))
        input_stop = true;

    bool pause = GPM_VAL.GetPause();
    imguiCheckbox("Pause (G)", &pause);
    if (ImGui::IsKeyPressed(GLFW_KEY_G))
        pause ^= true;
    GPM_VAL.SetPause(pause);

    imguiCheckbox("Auto (T)", &auto_and_repeat);
    if (ImGui::IsKeyPressed(GLFW_KEY_T))
        auto_and_repeat ^= true;

    imguiCheckbox("PV Mode (P)", &pv_mode);
    if (ImGui::IsKeyPressed(GLFW_KEY_P))
        pv_mode ^= true;

    ImGui::Separator();

    imguiColumnSliderFloat("Emission", &GPM_VAL.emission, 0.01f, 1.0f, 2.0f, "%.2f", 0, true);

    ImGui::Separator();

    imguiCheckbox("Show Grid", &draw_grid_3d);

    w = imguiGetContentRegionAvailWidth();
    if (imguiButton("Stage", { w, 0.0f }))
        stage_test = true;

    window_focus |= ImGui::IsWindowFocused();
    ImGui::End();

    float_t win_x = min((float_t)width, 240.0f);
    float_t win_y = min((float_t)height, 96.0f);

    float_t x = 0.0f;
    float_t y = (float_t)height - win_y;
    w = win_x;
    h = win_y;

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMouseInputs;
    window_flags |= ImGuiWindowFlags_NoNavInputs;
    window_flags |= ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleColor(ImGuiCol_Border, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
    if (ImGui::Begin("Glitter Test Sub##Data Test", 0, window_flags)) {
        size_t ctrl;
        size_t disp;
        float_t frame;
        int32_t life_time;

        life_time = 0;
        frame = GPM_VAL.GetSceneFrameLifeTime(scene_counter, &life_time);
        ImGui::Text("%.0f - %.0f/%d", this->frame, frame, life_time);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_QUAD);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_QUAD);
        ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LOCUS);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LOCUS);
        ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_LINE);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_LINE);
        ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = GPM_VAL.GetCtrlCount(GLITTER_PARTICLE_MESH);
        disp = GPM_VAL.GetDispCount(GLITTER_PARTICLE_MESH);
        ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
    }
    ImGui::PopStyleColor(2);
    ImGui::End();
}

void TaskDataTestGlitterParticle::LoadFile(const char* file) {
    GPM_VAL.FreeSceneEffect(scene_counter, hash_fnv1a64m_empty, true);
    GPM_VAL.UnloadEffectGroup(hash);
    hash = GPM_VAL.LoadFile(GLITTER_FT, rctx_ptr->data, file, 0, -1.0f, true, &rctx_ptr->data->data_ft.obj_db);
    rebuild_geff = true;
}

bool data_test_glitter_test_init(class_data* data, render_context* rctx) {
    task_data_test_glitter_particle_load();
    return true;
}

void data_test_glitter_test_ctrl(class_data* data) {

}

void data_test_glitter_test_imgui(class_data* data) {

}

bool data_test_glitter_test_dispose(class_data* data) {
    task_data_test_glitter_particle_unload();
    return true;
}
