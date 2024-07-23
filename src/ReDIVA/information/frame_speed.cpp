/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "frame_speed.hpp"
#include "../../CRE/app_system_detail.hpp"
#include "../dw.hpp"

class FrameSpeedWindow : public dw::Shell {
public:
    class SpeedSliderSelectionListener : public dw::SelectionAdapter {
    public:
        SpeedSliderSelectionListener();
        virtual ~SpeedSliderSelectionListener() override;

        virtual void Callback(SelectionListener::CallbackData* data) override;
    };

    dw::Slider* slider;
    SpeedSliderSelectionListener speed_slider_selection_listener;

    FrameSpeedWindow();
    virtual ~FrameSpeedWindow() override;

    virtual void Hide() override;
};

FrameSpeedWindow* frame_speed_window;

void frame_speed_window_init() {
    if (!frame_speed_window) {
        frame_speed_window = new FrameSpeedWindow;
        frame_speed_window->sub_1402F38B0();
        frame_speed_window->slider->SetValue(get_delta_frame());
    }
    else {
        frame_speed_window->slider->SetValue(get_delta_frame());
        frame_speed_window->Disp();
    }
}

FrameSpeedWindow::SpeedSliderSelectionListener::SpeedSliderSelectionListener() {

}

FrameSpeedWindow::SpeedSliderSelectionListener::~SpeedSliderSelectionListener() {

}

void FrameSpeedWindow::SpeedSliderSelectionListener::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        set_next_frame_speed(slider->GetValue());
}

FrameSpeedWindow::FrameSpeedWindow() {
    slider = dw::Slider::Create(this, dw::HORIZONTAL, 0.0f, 0.0f, 128.0f, 0.0f, "frame speed");
    slider->SetParams(1.0f, 0.0f, 3.0f, 1.0f, 0.01f, 0.1f);
    slider->format = "% 5.2f";
    slider->AddSelectionListener(&speed_slider_selection_listener);
    slider->SetRound(true);

    SetText("FrameSpeedWindow");

    UpdateLayout();
}

FrameSpeedWindow::~FrameSpeedWindow() {

}

void FrameSpeedWindow::Hide() {
    SetDisp(false);
}
