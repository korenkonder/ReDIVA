/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "background_color.h"
#include "../../CRE/microui.h"

extern mu_Context* muctx;

extern int32_t width;
extern int32_t height;

const char* background_color_window_title = "Background Color";

background_color_struct background_color;

void background_color_dispose() {
    background_color.enabled = false;
    background_color.dispose = false;
    background_color.disposed = true;
}

void background_color_init() {
    background_color.enabled = true;
}

void background_color_control() {
    if (!background_color.enabled)
        return;
}

void background_color_input() {
    if (!background_color.enabled)
        return;
}

void background_color_mui() {
    if (!background_color.enabled)
        return;
    else if (background_color.disposed) {
        mu_Container* cnt = mu_get_container(muctx, background_color_window_title);
        muctx->hover_root = muctx->next_hover_root = cnt;
        cnt->rect = mu_rect(0, 0, 0, 0);
        cnt->open = true;
        mu_bring_to_front(muctx, cnt);
        background_color.disposed = false;
    }

    int32_t title_w = min(width / 3, 360);
    int32_t title_h = 132;

    int32_t w = title_w;
    int32_t h = title_h;

    if (!mu_begin_window_ex(muctx, background_color_window_title,
        mu_rect(0, 0, w, h),
        0 /*MU_OPT_NOINTERACT | MU_OPT_NOCLOSE*/, false, true, false)) {
        background_color.dispose = true;
        return;
    }

    mu_layout_row(muctx, 2, (int[]) { -78, -1 }, 74);

    mu_layout_begin_column(muctx);
    mu_layout_row(muctx, 2, (int[]) { 46, -1 }, 0);

    mu_Color c = mu_color_real(background_color.color.x, background_color.color.y, background_color.color.z, 1.0f);
    mu_label(muctx, "Red:");
    mu_slider_uint8_t(muctx, &c.r, 0, 255);
    mu_label(muctx, "Green:");
    mu_slider_uint8_t(muctx, &c.g, 0, 255);
    mu_label(muctx, "Blue:");
    mu_slider_uint8_t(muctx, &c.b, 0, 255);
    background_color.color.x = c.r;
    background_color.color.y = c.g;
    background_color.color.z = c.b;
    vec3_mult_scalar(background_color.color, 1.0f / 255.0f, background_color.color);

    mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
    if (mu_button(muctx, "Reset Color"))
        background_color.color = (vec3){ 0.74117647f, 0.74117647f, 0.74117647f };
    mu_layout_end_column(muctx);

    mu_Rect r = mu_layout_next(muctx);
    mu_draw_rect(muctx, r, c);
    char buf[32];
    sprintf_s(buf, 32, "#%02X%02X%02X", c.r, c.g, c.b);
    mu_draw_control_text(muctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
    mu_end_window(muctx);
}

void background_color_render() {
    if (background_color.dispose) {
        background_color_dispose();
        return;
    }
    else if (!background_color.enabled)
        return;
}

void background_color_sound() {
    if (!background_color.enabled)
        return;
}
