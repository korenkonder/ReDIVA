/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/auth_2d.hpp"
#include "../dw.hpp"
#include "../print_work.hpp"
#include "../task_window.hpp"

class DtmAet : public app::Task {
public:
    std::vector<std::string> sound_farcs;
    PrintWork print_work;
    font_info font;
    uint32_t curr_set_index;
    uint32_t set_index;
    uint32_t id_index;
    uint32_t layer_index;
    uint32_t marker_index;
    uint8_t type;
    std::vector<std::string> comp_layers;
    AetComp comp;
    std::vector<std::string> markers;
    float_t frame;
    float_t start_time;
    float_t end_time;
    bool lock;
    bool loop;
    bool centering;
    int32_t state;

    DtmAet();
    virtual ~DtmAet() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void GetSoundFarcs();
    virtual void SetSetIndex(int32_t value);
    virtual void SetIdIndex(int32_t value);
    virtual void SetLayerIndex(int32_t value);
    virtual void SetMarkerIndex(int32_t value);
    virtual void SetType(uint8_t value);
    virtual void SetFrame(float_t value);
    virtual void SetLock(bool value);
    virtual void SetLoop(bool value);
    virtual void SetCentering(bool value);
};

class DtwAet : public dw::Shell {
public:
    dw::ListBox* set;
    dw::ListBox* id;
    dw::ListBox* layer;
    dw::ListBox* marker;
    dw::Label* marker_frame;
    dw::ListBox* type;
    dw::Slider* frame;

    DtwAet();
    virtual ~DtwAet() override;

    virtual void Hide() override;

    static void CenteringCallback(dw::Widget* data);
    static void FrameCallback(dw::Widget* data);
    static void IdCallback(dw::Widget* data);
    static void LayerCallback(dw::Widget* data);
    static void LockCallback(dw::Widget* data);
    static void LoopCallback(dw::Widget* data);
    static void MarkerCallback(dw::Widget* data);
    static void SetCallback(dw::Widget* data);
    static void TypeCallback(dw::Widget* data);
};

extern DtmAet* dtm_aet;

extern DtwAet* dtw_aet;

extern void dtm_aet_init();
extern void dtm_aet_load();
extern void dtm_aet_unload();
extern void dtm_aet_free();
