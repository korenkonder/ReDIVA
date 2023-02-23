/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "stage_test.hpp"

TaskDataTestGlitterParticle* task_data_test_glitter_particle;

extern int32_t width;
extern int32_t height;
extern render_context* rctx_ptr;
extern bool input_reset;
extern bool input_locked;
extern bool draw_grid_3d;

TaskDataTestGlitterParticle::TaskDataTestGlitterParticle() : hash(), scene_counter(), frame(),
auto_and_repeat(), reload(), pv_mode(), show_grid(), rebuild_geff(), geff_index(),
file_index(), load_file(), input_play(), input_stop(), delta_frame(), stage_test() {

}

TaskDataTestGlitterParticle::~TaskDataTestGlitterParticle() {

}

bool TaskDataTestGlitterParticle::Init() {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    Glitter::glt_particle_manager->counter = (uint32_t)(time.LowPart * 0x0CAD3078ULL);

    data_list[DATA_AFT].get_directory_files("rom/particle/", files);
    for (std::vector<data_struct_file>::iterator i = files.begin(); i != files.end();)
        if (str_utils_check_ends_with(i->name.c_str(), ".farc")) {
            char* temp = str_utils_get_without_extension(i->name.c_str());
            if (temp) {
                i->name.assign(temp);
                free(temp);
            }
            else
                i->name.clear();
            i++;
        }
        else
            i = files.erase(i);

    Glitter::glt_particle_manager->emission = 1.0f;
    Glitter::glt_particle_manager->draw_all = false;
    Glitter::glt_particle_manager->draw_all_mesh = false;
    dtm_stg_load(0);

    clear_color = { 0x60, 0x60, 0x60, 0xFF };

    camera* cam = rctx_ptr->camera;
    cam->reset();
    cam->set_view_point({ 0.0f, 0.88f, 4.3f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

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
    if (load_file && file_index < files.size())
        LoadFile(files[file_index].name.c_str());
    load_file = false;

    if (input_play)
        reload = true;
    else if (input_stop)
        Glitter::glt_particle_manager->FreeSceneEffect(scene_counter);

    if (!Glitter::glt_particle_manager->CheckNoFileReaders(hash)) {
        reload = false;
        return false;
    }

    if (rebuild_geff) {
        int32_t effects_count = (int32_t)Glitter::glt_particle_manager->GetEffectsCount(this->hash);
        geff.clear();
        geff.push_back("ALL");
        for (int32_t i = 0; i < effects_count; i++)
            geff.push_back(Glitter::glt_particle_manager->GetEffectName(hash, i));
        rebuild_geff = false;
    }

    if (Glitter::glt_particle_manager->SceneHasNotEnded(scene_counter) && !Glitter::glt_particle_manager->GetPause())
        frame += sys_frame_rate.GetDeltaFrame();

    if (reload || auto_and_repeat && !Glitter::glt_particle_manager->SceneHasNotEnded(scene_counter)) {
        Glitter::glt_particle_manager->FreeSceneEffect(scene_counter);
        uint64_t effect_hash = hash_fnv1a64m_empty;

        if (geff_index)
            effect_hash = Glitter::glt_particle_manager->CalculateHash(geff[geff_index].c_str());

        if (!pv_mode)
            scene_counter = Glitter::glt_particle_manager->Load(hash, effect_hash, 0);
        else if (effect_hash == hash_fnv1a64m_empty)
            scene_counter = Glitter::glt_particle_manager->LoadScene(hash, hash_fnv1a64m_empty);
        else
            scene_counter = Glitter::glt_particle_manager->LoadScene(hash_fnv1a64m_empty, effect_hash);
        frame = 0.0f;
        reload = false;
    }

    input_play = false;
    input_stop = false;
    return false;
}

bool TaskDataTestGlitterParticle::Dest() {
    clear_color = color_black;
    Glitter::glt_particle_manager->FreeScenes();
    Glitter::glt_particle_manager->FreeEffects();
    Glitter::glt_particle_manager->SetPause(false);
    return true;
}

void TaskDataTestGlitterParticle::Disp() {
    if (show_grid) {
        mdl::EtcObj etc = {};
        etc.init(mdl::ETC_OBJ_GRID);
        etc.color = { 0.0f, 0.0f, 0.0f, 1.0f };
        etc.data.grid.w = 50;
        etc.data.grid.h = 50;
        etc.data.grid.ws = 50;
        etc.data.grid.hs = 50;
        rctx_ptr->disp_manager.entry_obj_etc(&mat4_identity, &etc);
    }
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

    if (ImGui::ColumnComboBoxConfigFile("File", files.data(),
        files.size(), &file_index, 0, false, &window_focus)) {
        load_file = true;
        input_stop = true;
    }

    if (ImGui::ButtonEnterKeyPressed("Reset Camera (R)"))
        input_reset = true;

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::ButtonEnterKeyPressed("Play (F)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_F))
        input_play = true;

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::ButtonEnterKeyPressed("Stop (V)", { w, 0.0f }) || ImGui::IsKeyPressed(ImGuiKey_V))
        input_stop = true;

    bool pause = Glitter::glt_particle_manager->GetPause();
    ImGui::CheckboxEnterKeyPressed("Pause (G)", &pause);
    if (ImGui::IsKeyPressed(ImGuiKey_G))
        pause ^= true;
    Glitter::glt_particle_manager->SetPause(pause);

    ImGui::CheckboxEnterKeyPressed("Auto (T)", &auto_and_repeat);
    if (ImGui::IsKeyPressed(ImGuiKey_T))
        auto_and_repeat ^= true;

    ImGui::CheckboxEnterKeyPressed("PV Mode (P)", &pv_mode);
    if (ImGui::IsKeyPressed(ImGuiKey_P))
        pv_mode ^= true;

    ImGui::Separator();

    ImGui::ColumnSliderFloatButton("Emission",
        &Glitter::glt_particle_manager->emission, 0.01f, 1.0f, 2.0f, 0.1f, "%.2f", 0);

    ImGui::Separator();

    ImGui::CheckboxEnterKeyPressed("Show Grid", &draw_grid_3d);

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::ButtonEnterKeyPressed("Stage", { w, 0.0f }))
        stage_test = true;

    window_focus |= ImGui::IsWindowFocused();
    ImGui::End();

    float_t win_x = min_def((float_t)width, 240.0f);
    float_t win_y = min_def((float_t)height, 96.0f);

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
        frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(scene_counter, &life_time);
        ImGui::Text("%.0f - %.0f/%d", this->frame, frame, life_time);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD);
        ImGui::Text(" Quad: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS);
        ImGui::Text("Locus: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE);
        ImGui::Text(" Line: ctrl%lld, disp%lld", ctrl, disp);

        ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH);
        disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH);
        ImGui::Text(" Mesh: ctrl%lld, disp%lld", ctrl, disp);
    }
    ImGui::PopStyleColor(2);
    ImGui::End();
}

void TaskDataTestGlitterParticle::LoadFile(const char* file) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    Glitter::glt_particle_manager->FreeSceneEffect(scene_counter);
    Glitter::glt_particle_manager->UnloadEffectGroup(hash);
    hash = Glitter::glt_particle_manager->LoadFile(Glitter::FT, aft_data, file, 0, -1.0f, true, aft_obj_db);
    rebuild_geff = true;
}

void task_data_test_glitter_particle_init() {
    task_data_test_glitter_particle = new TaskDataTestGlitterParticle;
}

void task_data_test_glitter_particle_free() {
    if (task_data_test_glitter_particle) {
        delete task_data_test_glitter_particle;
        task_data_test_glitter_particle = 0;
    }
}
