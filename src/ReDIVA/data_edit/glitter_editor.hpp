/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/data.hpp"
#include "../task_window.hpp"

enum glitter_editor_selected_enum {
    GLITTER_EDITOR_SELECTED_NONE = 0,
    GLITTER_EDITOR_SELECTED_EFFECT,
    GLITTER_EDITOR_SELECTED_EMITTER,
    GLITTER_EDITOR_SELECTED_PARTICLE,
};

enum glitter_editor_flags {
    GLITTER_EDITOR_ADD       = 0x01,
    GLITTER_EDITOR_DUPLICATE = 0x02,
    GLITTER_EDITOR_DELETE    = 0x04,
    GLITTER_EDITOR_MOVE_UP   = 0x08,
    GLITTER_EDITOR_MOVE_DOWN = 0x10,
    GLITTER_EDITOR_IMPORT    = 0x20,
    GLITTER_EDITOR_EXPORT    = 0x40,
};

enum glitter_editor_disp_flags {
    GLITTER_EDITOR_DISP_WIREFRAME    = 0x01,
    GLITTER_EDITOR_DISP_SELECTED     = 0x02,
    GLITTER_EDITOR_DISP_NO_DRAW      = 0x04,
    GLITTER_EDITOR_DISP_EMITTER_TYPE = 0x08,
};

class GlitterEditor : public app::TaskWindow {
public:
    struct CurveEditor {
        Glitter::CurveType type;
        Glitter::CurveTypeFlags type_flags;
        Glitter::Animation* animation;
        Glitter::Curve* list[Glitter::CURVE_V_SCROLL_ALPHA_2ND - Glitter::CURVE_TRANSLATION_X + 1];
        Glitter::Curve::KeyRev* key;

        int32_t frame_width;
        float_t zoom_time;
        float_t prev_zoom_time;
        float_t zoom_value;
        float_t key_radius_in;
        float_t key_radius_out;
        float_t height_offset;
        int32_t frame;

        struct ImDrawList* draw_list;
        struct ImGuiIO* io;
        float_t timeline_pos;

        float_t range;
        float_t offset;

        bool add_key;
        bool del_key;
        bool add_curve;
        bool del_curve;
        bool key_edit;

        CurveEditor();

        void ResetCurves();
        void ResetState(Glitter::CurveType type = (Glitter::CurveType)-1);
        void SetFlag(const Glitter::CurveTypeFlags type_flag);
    };

    bool test;
    bool create_popup;
    bool load;
    bool load_wait;
    bool load_popup;
    bool load_data_popup;
    bool load_error_popup;
    bool save;
    bool save_popup;
    bool save_compress;
    bool save_encrypt;
    bool close;
    bool close_editor;
    bool input_play;
    bool input_reload;
    bool input_pause;
    bool input_pause_temp;
    bool input_reset;
    bool effect_group_add;
    bool show_grid;
    glitter_editor_disp_flags draw_flags;
    glitter_editor_flags resource_flags;
    glitter_editor_flags effect_flags;
    glitter_editor_flags emitter_flags;
    glitter_editor_flags particle_flags;
    Glitter::Type load_glt_type;
    Glitter::Type save_glt_type;
    data_type load_data_type;
    float_t frame_counter;
    float_t old_frame_counter;
    int32_t start_frame;
    int32_t end_frame;
    Glitter::Random random;
    Glitter::Counter counter;
    Glitter::EffectGroup* effect_group;
    Glitter::Scene* scene;
    uint64_t hash;
    Glitter::SceneCounter scene_counter;
    std::string file;

    glitter_editor_selected_enum selected_type;
    int32_t selected_resource;
    Glitter::Effect* selected_effect;
    Glitter::Emitter* selected_emitter;
    Glitter::Particle* selected_particle;
    int32_t selected_edit_resource;
    Glitter::Effect* selected_edit_effect;
    Glitter::Emitter* selected_edit_emitter;
    Glitter::Particle* selected_edit_particle;
    CurveEditor curve_editor;

    GlitterEditor();
    virtual ~GlitterEditor() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
    virtual void window() override;

    void reset();

    static void reset_disp();
};

extern GlitterEditor glitter_editor;
