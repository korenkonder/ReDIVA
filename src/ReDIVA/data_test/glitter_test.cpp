/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_test.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../dw.hpp"
#include "../input_state.hpp"
#include "../print_work.hpp"
#include "stage_test.hpp"

class DataTestGlitterParticleDw : public dw::Shell {
public:
    TaskDataTestGlitterParticle* task;
    dw::ListBox* list;
    dw::ListBox* geff;
    dw::Button* pause;
    dw::Button* auto_and_repeat;
    dw::Button* pv_mode;
    dw::Button* play;
    dw::Button* stop;
    dw::Slider* emission;
    dw::Button* show_grid;
    dw::Button* stage;
    dw::Button* chara;

    DataTestGlitterParticleDw(TaskDataTestGlitterParticle* task);
    virtual ~DataTestGlitterParticleDw() override;

    void Init();

    static void AutoAndRepeatCallback(dw::Widget* data);
    static void CharaCallback(dw::Widget* data);
    static void EmissionCallback(dw::Widget* data);
    static void ListCallback(dw::Widget* data);
    static void PauseCallback(dw::Widget* data);
    static void PlayCallback(dw::Widget* data);
    static void PvModeCallback(dw::Widget* data);
    static void ShowGridCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
    static void StopCallback(dw::Widget* data);
};

TaskDataTestGlitterParticle* task_data_test_glitter_particle;

extern render_context* rctx_ptr;

TaskDataTestGlitterParticle::TaskDataTestGlitterParticle() : hash(), dw(),
frame(), auto_and_repeat(), reload(), pv_mode(), show_grid(), rebuild_geff() {

}

TaskDataTestGlitterParticle::~TaskDataTestGlitterParticle() {

}

bool TaskDataTestGlitterParticle::init() {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    Glitter::counter.value = (uint32_t)(time.LowPart * hash_murmurhash_empty);

    clear_color = 0xFF606060;

    Glitter::glt_particle_manager->emission = 1.0f;
    Glitter::glt_particle_manager->draw_all = false;
    Glitter::glt_particle_manager->draw_all_mesh = false;

    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 1.0f, 3.45f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    hash = hash_fnv1a64m_empty;
    scene_counter = 0;
    dw = new DataTestGlitterParticleDw(this);
    dw->sub_1402F38B0();
    frame = 0;
    auto_and_repeat = false;
    reload = false;
    pv_mode = false;
    show_grid = false;
    rebuild_geff = false;
    return true;
}

bool TaskDataTestGlitterParticle::ctrl() {
    InputState* input_state = input_state_get(0);
    if (input_state) {
        if (input_state->CheckTapped(43))
            SetReload();
        else if (input_state->CheckTapped(42))
            SceneFree();
    }

    if (!Glitter::glt_particle_manager->CheckNoFileReaders(hash)) {
        reload = false;
        return false;
    }

    if (rebuild_geff) {
        if (dw) {
            if (dw->geff->list)
                dw->geff->list->ClearItems();

            int32_t effects_count = (int)Glitter::glt_particle_manager->GetEffectsCount(hash);
            dw->geff->AddItem("ALL");

            std::vector<uint64_t> effect_hashes(effects_count);
            for (int32_t i = 0; i < effects_count; i++) {
                const char* effect_name = Glitter::glt_particle_manager->GetEffectName(hash, i);
                dw->geff->AddItem(effect_name);
                effect_hashes[i] = Glitter::glt_particle_manager->CalculateHash(effect_name);
            }

            dw->geff->SetItemIndex(0);
        }

        rebuild_geff = false;
    }

    if (Glitter::glt_particle_manager->SceneHasNotEnded(scene_counter)
        && !Glitter::glt_particle_manager->GetPause())
        frame += get_sys_frame_rate()->get_delta_frame();

    if (reload || auto_and_repeat && !Glitter::glt_particle_manager->SceneHasNotEnded(scene_counter)) {
        Glitter::glt_particle_manager->FreeSceneEffect(scene_counter);
        uint64_t effect_hash = hash_fnv1a64m_empty;

        if (dw) {
            dw::ListBox* geff = dw->geff;
            if (geff->list && geff->list->selected_item)
                effect_hash = Glitter::glt_particle_manager->CalculateHash(
                    geff->GetSelectedItemStr().c_str());
        }

        if (!pv_mode)
            scene_counter = Glitter::glt_particle_manager->Load(hash, effect_hash, false);
        else if (effect_hash == hash_fnv1a64m_empty)
            scene_counter = Glitter::glt_particle_manager->LoadScene(hash, hash_fnv1a64m_empty);
        else
            scene_counter = Glitter::glt_particle_manager->LoadScene(hash_fnv1a64m_empty, effect_hash);

        frame = 0.0f;
        reload = false;
    }
    return false;
}

bool TaskDataTestGlitterParticle::dest() {
    clear_color = color_black;
    Glitter::glt_particle_manager->FreeScenes();
    Glitter::glt_particle_manager->FreeEffects();
    Glitter::glt_particle_manager->SetPause(false);

    if (dw) {
        dw->Hide();
        dw = 0;
    }
    return true;
}

void TaskDataTestGlitterParticle::disp() {
    if (show_grid) {
        mdl::EtcObj etc(mdl::ETC_OBJ_GRID);
        etc.color = color_black;
        etc.data.grid.w = 50;
        etc.data.grid.h = 50;
        etc.data.grid.ws = 50;
        etc.data.grid.hs = 50;
        rctx_ptr->disp_manager->entry_obj_etc(mat4_identity, etc);
    }

    int32_t life_time = 0;
    float_t frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(scene_counter, &life_time);

    PrintWork print_work;
    font_info font(16);
    print_work.set_font(&font);
    print_work.set_position({ 0.0f, 624.0f });
    print_work.printf_align_left("%.0f - %.0f/%.0f\n", this->frame, frame, (float_t)life_time);

    size_t disp;
    size_t ctrl;

    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD);
    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD);
    print_work.printf_align_left(" Quad: ctrl%d, disp%d\n", (uint32_t)disp, (uint32_t)ctrl);

    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS);
    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS);
    print_work.printf_align_left("Locus: ctrl%d, disp%d\n", (uint32_t)disp, (uint32_t)ctrl);

    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE);
    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE);
    print_work.printf_align_left(" Line: ctrl%d, disp%d\n", (uint32_t)disp, (uint32_t)ctrl);

    // Added
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH);
    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH);
    print_work.printf_align_left(" Mesh: ctrl%d, disp%d\n", (uint32_t)disp, (uint32_t)ctrl);

}

void TaskDataTestGlitterParticle::SceneFree() {
    Glitter::glt_particle_manager->FreeSceneEffect(scene_counter);
}

void TaskDataTestGlitterParticle::SetPause(bool value) {
    Glitter::glt_particle_manager->SetPause(value);
}

void TaskDataTestGlitterParticle::SetReload() {
    reload = true;
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
    if (!task_data_test_glitter_particle)
        task_data_test_glitter_particle = new TaskDataTestGlitterParticle;
}

void task_data_test_glitter_particle_free() {
    if (task_data_test_glitter_particle) {
        delete task_data_test_glitter_particle;
        task_data_test_glitter_particle = 0;
    }
}

DataTestGlitterParticleDw::DataTestGlitterParticleDw(TaskDataTestGlitterParticle* task) {
    this->task = task;

    SetText("Glitter");

    (new dw::Label(this))->SetText("list");

    list = new dw::ListBox(this);
    list->SetMaxItems(30);
    list->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::ListCallback));

    (new dw::Label(this))->SetText("geff");

    geff = new dw::ListBox(this);
    geff->SetMaxItems(30);

    pause = new dw::Button(this, dw::CHECKBOX);
    pause->SetText("Pause");
    pause->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::PauseCallback));

    auto_and_repeat = new dw::Button(this, dw::CHECKBOX);
    auto_and_repeat->SetText("Auto & Repeat");
    auto_and_repeat->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::AutoAndRepeatCallback));

    pv_mode = new dw::Button(this, dw::CHECKBOX);
    pv_mode->SetText("PV Mode");
    pv_mode->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::PvModeCallback));

    play = new dw::Button(this, dw::FLAG_8);
    play->SetText("Play(E)");
    play->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::PlayCallback));

    stop = new dw::Button(this, dw::FLAG_8);
    stop->SetText("Stop(D)");
    stop->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::StopCallback));

    emission = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL), "Emission", "%2.2f", 150.0f);
    emission->SetParams(1.0f, 1.0f, 2.0f, 0.1f, 0.01f, 0.1f);
    emission->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::EmissionCallback));

    show_grid = new dw::Button(this, dw::CHECKBOX);
    show_grid->SetText("Show Grid");
    show_grid->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::ShowGridCallback));

    stage = new dw::Button(this, dw::FLAG_8);
    stage->SetText("Stage");
    stage->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::StageCallback));

    chara = new dw::Button(this, dw::CHECKBOX);
    chara->SetText("Chara");
    chara->AddSelectionListener(new dw::SelectionListenerOnHook(
        DataTestGlitterParticleDw::CharaCallback));

    Init();

    UpdateLayout();
}

DataTestGlitterParticleDw::~DataTestGlitterParticleDw() {

}

void DataTestGlitterParticleDw::Init() {
    pause->SetValue(false);
    auto_and_repeat->SetValue(false);
    pv_mode->SetValue(false);
    show_grid->SetValue(false);
    chara->SetValue(false);
    list->ClearItems();

    std::vector<data_struct_file> files = data_list[DATA_AFT].get_directory_files("rom/particle/");
    for (data_struct_file& i : files) {
        size_t pos = i.name.find(".farc");
        if (pos == i.name.size() - 5)
            list->AddItem(i.name.substr(0, pos));
    }
}

void DataTestGlitterParticleDw::AutoAndRepeatCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->auto_and_repeat = button->value;
    }
}

void DataTestGlitterParticleDw::CharaCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        if (dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell))
            if (button->value) {
                /*if (!app::TaskWork::check_task_ready(&data_test_chr))
                    app::TaskWork::add_task(&data_test_chr, "DATA_TEST_CHR");*/
            }
            else {
                /*if (app::TaskWork::check_task_ready(&data_test_chr))
                    data_test_chr.del();*/
            }
}

void DataTestGlitterParticleDw::EmissionCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        if (dynamic_cast<DataTestGlitterParticleDw*>(slider->parent_shell))
            Glitter::glt_particle_manager->emission = slider->GetValue();
}

void DataTestGlitterParticleDw::ListCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(list_box->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->LoadFile(list_box->GetSelectedItemStr().c_str());
    }
}

void DataTestGlitterParticleDw::PauseCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->SetPause(button->value);
    }
}

void DataTestGlitterParticleDw::PlayCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->SetReload();
    }
}

void DataTestGlitterParticleDw::PvModeCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->pv_mode = button->value;
    }
}

void DataTestGlitterParticleDw::ShowGridCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw =
            dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->show_grid = button->value;
    }
}

void DataTestGlitterParticleDw::StageCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        if (dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell))
            dtw_stg_load(0);
}

void DataTestGlitterParticleDw::StopCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        DataTestGlitterParticleDw* glt_ptcl_dw = dynamic_cast<DataTestGlitterParticleDw*>(button->parent_shell);
        if (glt_ptcl_dw)
            glt_ptcl_dw->task->SceneFree();
    }
}
