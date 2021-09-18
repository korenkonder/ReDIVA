/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(VIDEO)
#include "video_x264.h"
#include <sysinfoapi.h>

static void video_x264_pic_convert8(video_x264* x);
static void video_x264_pic_convert10(video_x264* x);

vector_func(hash_video_x264)

bool video_x264_init(video_x264* x, video_x264_init_data* init) {
    if (!x)
        return false;

    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);
    int32_t threads = sysinf.dwNumberOfProcessors / 2;
    if (threads < 1)
        threads = 1;

    memset(x, 0x00, sizeof(*x));

    const char* tune = x264_tune_names[init->tune >= VIDEO_X264_TUNE_FILM
        && init->tune <= VIDEO_X264_TUNE_ZEROLATENCY ? init->tune : VIDEO_X264_TUNE_MAX];
    const char* preset = x264_preset_names[init->preset >= VIDEO_X264_PRESET_ULTRAFAST
        && init->preset <= VIDEO_X264_PRESET_PLACEBO ? init->preset : VIDEO_X264_PRESET_MEDIUM];

    lock_init(&x->lock);
    x->error_state = 0;
    if (!lock_check_init(&x->lock) || x264_param_default_preset(&x->state.param, preset, tune) < 0) {
        x->error_state = -1;
        return false;
    }

    int32_t i_csp = init->color_space;
    if (i_csp == VIDEO_X264_COLOR_SPACE_I400)
        i_csp = X264_CSP_I400;
    else if (i_csp >= VIDEO_X264_COLOR_SPACE_I420 && i_csp <= VIDEO_X264_COLOR_SPACE_NV21)
        i_csp = X264_CSP_I420;
    else if (i_csp >= VIDEO_X264_COLOR_SPACE_I422 && i_csp <= VIDEO_X264_COLOR_SPACE_NV16)
        i_csp = X264_CSP_I422;
    else if (i_csp >= VIDEO_X264_COLOR_SPACE_I444 && i_csp <= VIDEO_X264_COLOR_SPACE_RGB)
        i_csp = X264_CSP_I444;
    else
        i_csp = X264_CSP_NONE;

    int32_t i_rc_method = init->rate_control.method;
    float_t f_rf_constant = 0.0f;
    if (i_rc_method < X264_RC_CQP || i_rc_method > X264_RC_ABR) {
        i_rc_method = X264_RC_CRF;
        f_rf_constant = 20.0f;
    }
    else if (i_rc_method == X264_RC_CRF)
        f_rf_constant = init->rate_control.crf;

    x->state.param.i_threads = threads;
    x->state.param.i_width = init->width;
    x->state.param.i_height = init->height;
    x->state.param.i_csp = i_csp;
    x->state.param.i_bitdepth = 8;
    if (init->bit_depth == VIDEO_X264_BIT_DEPTH_10_BIT) {
        x->state.param.i_csp |= X264_CSP_HIGH_DEPTH;
        x->state.param.i_bitdepth = 10;
    }

    if (init->flip)
        x->state.param.i_csp |= X264_CSP_VFLIP;

    const char* profile;
    switch (i_csp) {
    case VIDEO_X264_COLOR_SPACE_I400:
        if (init->bit_depth == VIDEO_X264_BIT_DEPTH_10_BIT)
            profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH + 1];
        else
            profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH];
        break;
    case VIDEO_X264_COLOR_SPACE_I420:
        if (init->bit_depth == VIDEO_X264_BIT_DEPTH_10_BIT)
            profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH + 1];
        else
            profile = x264_profile_names[init->profile >= VIDEO_X264_PROFILE_BASELINE
            && init->profile <= VIDEO_X264_PROFILE_HIGH ? init->profile : VIDEO_X264_PROFILE_MAIN];
        break;
    case VIDEO_X264_COLOR_SPACE_I422:
        profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH + 2];
        break;
    case VIDEO_X264_COLOR_SPACE_I444:
        profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH + 3];
        break;
    default:
        profile = 0;
        break;
    }

    if (f_rf_constant == 0.0f)
        profile = x264_profile_names[VIDEO_X264_PROFILE_HIGH + 3];

    x->state.param.vui.i_sar_height = 1;
    x->state.param.vui.i_sar_width = 1;
    x->state.param.vui.i_overscan = 1;
    x->state.param.vui.i_vidformat = 5;
    x->state.param.vui.b_fullrange = true;
    x->state.param.vui.i_colorprim = 2;
    x->state.param.vui.i_transfer = 2;
    x->state.param.vui.i_colmatrix = 1;
    x->state.param.vui.i_chroma_loc = 0;

    double_t fps = init->fps;
    int32_t i_keyint_max = (int32_t)ceil(fps);
    int32_t i_fps_num = 0;
    int32_t i_fps_den = 0;
    for (int32_t i = 0, j = 1; i < 6; i++, j *= 10) {
        i_fps_num = (int32_t)(fps * (double_t)j);
        i_fps_den = j;

        if (j < 10)
            continue;

        double_t f0 = (double_t)i_fps_num / (double_t)i_fps_den;
        double_t f1 = (double_t)(i_fps_num / 10) / (double_t)(i_fps_den / 10);
        if (fabs(f0 - f1) <= 0.0001 && fabs(f1 - fps) <= 0.0001) {
            i_fps_num /= 10;
            i_fps_den /= 10;
            break;
        }
    }

    x->state.param.i_keyint_max = i_keyint_max;
    x->state.param.i_bframe = 4;
    x->state.param.i_bframe_adaptive = 2;
    x->state.param.i_bframe_pyramid = 2;
    x->state.param.b_open_gop = true;
    x->state.param.b_repeat_headers = true;
    x->state.param.b_annexb = true;
    x->state.param.b_vfr_input = false;

    x->state.param.i_fps_num = i_fps_num;
    x->state.param.i_fps_den = i_fps_den;

    x->state.param.analyse.i_direct_mv_pred = X264_DIRECT_PRED_AUTO;
    x->state.param.analyse.i_me_method = X264_ME_UMH;
    x->state.param.analyse.i_me_range = 24;
    x->state.param.analyse.i_subpel_refine = 9;

    x->state.param.rc.i_rc_method = i_rc_method;
    switch (i_rc_method) {
    case X264_RC_CQP:
        x->state.param.rc.i_qp_constant = init->rate_control.cqp;
        break;
    case X264_RC_CRF:
        x->state.param.rc.f_rf_constant = f_rf_constant;
        break;
    case X264_RC_ABR:
        x->state.param.rc.i_bitrate = init->rate_control.abr;
        break;
    }
    x->state.param.rc.f_ip_factor = 1.0f;
    x->state.param.rc.f_pb_factor = 1.0f;

    x->state.param.rc.i_aq_mode = X264_AQ_AUTOVARIANCE;

    if (x264_param_apply_profile(&x->state.param, profile) < 0) {
        x->error_state = -2;
        return false;
    }

    x->state.enc = x264_encoder_open(&x->state.param);
    if (!x->state.enc) {
        x->error_state = -3;
        return false;
    }

    if (x264_picture_alloc(&x->state.pic, x->state.param.i_csp,
        x->state.param.i_width, x->state.param.i_height) < 0) {
        x->error_state = -4;
        return false;
    }

    x->data = force_malloc((size_t)x->state.param.i_width * (size_t)x->state.param.i_height * 8);
    x->state.buffer = force_malloc((size_t)x->state.param.i_width
        * (size_t)x->state.param.i_height * (init->bit_depth == VIDEO_X264_BIT_DEPTH_10_BIT ? 6 : 3));
    return true;
}

bool video_x264_encode(video_x264* x, int32_t frame) {
    if (!x || x->error_state < 0 || !x->state.enc)
        return false;

    x->state.pic.i_type = X264_TYPE_AUTO;
    x->state.pic.i_pts = frame;
    int32_t i_frame_size = x264_encoder_encode(x->state.enc,
        &x->state.nal, &x->state.i_nal, &x->state.pic, &x->state.pic_out);
    if (i_frame_size < 0) {
        x->error_state = -5;
        return false;
    }
    else if (i_frame_size)
        if (!io_write(&x->io, x->state.nal->p_payload, i_frame_size)) {
            x->error_state = -6;
            return false;
        }
    return true;
}

void video_x264_load_image_data(video_x264* x) {
    if (!x || x->error_state < 0 || !x->state.enc)
        return;

    if (x->state.param.i_csp & X264_CSP_HIGH_DEPTH)
        video_x264_pic_convert10(x);
    else
        video_x264_pic_convert8(x);
}

bool video_x264_flush(video_x264* x) {
    if (!x || x->error_state < 0 || !x->state.enc)
        return false;

    while (x264_encoder_delayed_frames(x->state.enc)) {
        int32_t i_frame_size = x264_encoder_encode(x->state.enc,
            &x->state.nal, &x->state.i_nal, 0, &x->state.pic_out);
        if (i_frame_size < 0) {
            x->error_state = -7;
            return true;
        }
        else if (i_frame_size)
            if (!io_write(&x->io, x->state.nal->p_payload, i_frame_size)) {
                x->error_state = -8;
                return true;
            }
    }
    return true;
}

void video_x264_close(video_x264* x) {
    if (!x)
        return;

    if (x->error_state <= -4 || x->error_state >= 0) {
        if (x->error_state <= -5 || x->error_state >= 0) {
            free(x->state.buffer);
            free(x->data);
            x264_picture_clean(&x->state.pic);
        }

        if (x->state.enc)
            x264_encoder_close(x->state.enc);
    }
    lock_free(&x->lock);
}

static void video_x264_pic_convert8(video_x264* x) {
    size_t count = (size_t)x->state.param.i_width * (size_t)x->state.param.i_height;
    uint8_t* buffer = x->state.buffer;
    lock_lock(&x->lock);
    switch (x->fmt) {
    case VIDEO_X264_PIC_FORMAT_RGB8: {
        uint8_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = ptr[0];
            buffer[1] = ptr[1];
            buffer[2] = ptr[2];
            ptr += 3;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB10: {
        uint8_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            uint32_t rgb_a = *(uint32_t*)ptr;
            buffer[0] = (uint8_t)((rgb_a >> 2) & 0xFF);
            buffer[1] = (uint8_t)((rgb_a >> 12) & 0xFF);
            buffer[2] = (uint8_t)((rgb_a >> 22) & 0xFF);
            ptr += 4;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB16: {
        uint32_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            uint32_t rgb = *ptr++;
            buffer[0] = (uint8_t)((ptr[0] >> 8) & 0xFF);
            buffer[1] = (uint8_t)((ptr[1] >> 8) & 0xFF);
            buffer[2] = (uint8_t)((ptr[2] >> 8) & 0xFF);
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGBA8: {
        uint8_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = ptr[0];
            buffer[1] = ptr[1];
            buffer[2] = ptr[2];
            ptr += 4;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB10_A2: {
        uint32_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            uint32_t rgb_a = *ptr++;
            buffer[0] = (uint8_t)((rgb_a >> 2) & 0xFF);
            buffer[1] = (uint8_t)((rgb_a >> 12) & 0xFF);
            buffer[2] = (uint8_t)((rgb_a >> 22) & 0xFF);
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGBA16: {
        uint16_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = (uint8_t)((ptr[0] >> 8) & 0xFF);
            buffer[1] = (uint8_t)((ptr[1] >> 8) & 0xFF);
            buffer[2] = (uint8_t)((ptr[2] >> 8) & 0xFF);
            ptr += 4;
            buffer += 3;
        }
    } break;
    }
    lock_unlock(&x->lock);

    const int32_t rgb2y_val[3] = {  21260,  71520,   7220 };
    const int32_t rgb2u_val[3] = { -11458, -38542,  50000 };
    const int32_t rgb2v_val[3] = {  50000, -45415,  -4585 };

#define rgb2y(r, g, b) ((r * rgb2y_val[0] + g * rgb2y_val[1] + b * rgb2y_val[2]) / 100000)
#define rgb2u(r, g, b) ((r * rgb2u_val[0] + g * rgb2u_val[1] + b * rgb2u_val[2]) / 100000)
#define rgb2v(r, g, b) ((r * rgb2v_val[0] + g * rgb2v_val[1] + b * rgb2v_val[2]) / 100000)
#define yclamp(y) (uint8_t)clamp(y, 0, 255)
#define uclamp(u) (uint8_t)clamp(u + 128, 0, 255)
#define vclamp(v) (uint8_t)clamp(v + 128, 0, 255);

    const size_t width = x->state.param.i_width;
    const size_t height = x->state.param.i_height;
    const size_t width2 = width / 2 * 2;
    const size_t height2 = height / 2 * 2;
    const size_t width3 = width * 3;
    const size_t height3 = height * 3;
    buffer = x->state.buffer;
    switch (x->state.param.i_csp & X264_CSP_MASK) {
    case X264_CSP_I400: {
        uint8_t* y_plane = x->state.pic.img.plane[0];
        for (size_t i = 0; i < height; i++)
            for (size_t j = 0; j < width; j++) {
                uint8_t r = buffer[0];
                uint8_t g = buffer[1];
                uint8_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                *y_plane++ = yclamp(y);
            }
    } break;
    case X264_CSP_I420: {
        uint8_t* y_plane = x->state.pic.img.plane[0];
        uint8_t* u_plane = x->state.pic.img.plane[1];
        uint8_t* v_plane = x->state.pic.img.plane[2];

        size_t i = 0;
        for (; i < height2; i += 2) {
            size_t j = 0;
            for (; j < width2; j += 2) {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                int32_t y;
                int32_t u[4];
                int32_t v[4];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[2] = rgb2u(r, g, b);
                v[2] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[3] = rgb2u(r, g, b);
                v[3] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1] + u[2] + u[3]) / 4);
                *v_plane = vclamp((v[0] + v[1] + v[2] + v[3]) / 4);
                u_plane++;
                v_plane++;
            }

            for (; j < width; j++) {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }
            buffer += width3;
            y_plane += width;
        }

        for (; i < height; i++) {
            size_t x = 0;
            for (; x < width2; x += 2) {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }

            for (; x < width; x++) {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                int32_t y;
                int32_t u;
                int32_t v;
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u = rgb2u(r, g, b);
                v = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp(u);
                *v_plane = vclamp(v);
                u_plane++;
                v_plane++;
            }
            buffer += width3;
            y_plane += width;
        }
    } break;
    case X264_CSP_I422: {
        uint8_t* y_plane = x->state.pic.img.plane[0];
        uint8_t* u_plane = x->state.pic.img.plane[1];
        uint8_t* v_plane = x->state.pic.img.plane[2];

        for (size_t i = 0; i < height; i++) {
            size_t j = 0;
            for (; j < width2; j += 2) {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];
                buffer += 3;

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];
                buffer += 3;

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }

            for (; j < width; j++) {
                uint8_t r = buffer[0];
                uint8_t g = buffer[1];
                uint8_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                int32_t u = rgb2u(r, g, b);
                int32_t v = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);
                *u_plane = uclamp(u);
                *v_plane = vclamp(v);
                u_plane++;
                v_plane++;
            }
        }
    } break;
    case X264_CSP_I444: {
        uint8_t* y_plane = x->state.pic.img.plane[0];
        uint8_t* u_plane = x->state.pic.img.plane[1];
        uint8_t* v_plane = x->state.pic.img.plane[2];

        for (size_t i = 0; i < height; i++)
            for (size_t j = 0; j < width; j++) {
                uint8_t r = buffer[0];
                uint8_t g = buffer[1];
                uint8_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                int32_t u = rgb2u(r, g, b);
                int32_t v = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);
                *u_plane++ = uclamp(u);
                *v_plane++ = vclamp(v);
            }
    } break;
    }
#undef rgb2y
#undef rgb2u
#undef rgb2v
#undef yclamp
#undef uclamp
#undef vclamp
}

static void video_x264_pic_convert10(video_x264* x) {
    size_t count = (size_t)x->state.param.i_width * (size_t)x->state.param.i_height;
    uint16_t* buffer = x->state.buffer;
    lock_lock(&x->lock);
    switch (x->fmt) {
    case VIDEO_X264_PIC_FORMAT_RGB8: {
        uint16_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = (uint16_t)(((ptr[0] << 2) | (ptr[0] >> 6)) & 0x3FF);
            buffer[1] = (uint16_t)(((ptr[1] << 2) | (ptr[1] >> 6)) & 0x3FF);
            buffer[2] = (uint16_t)(((ptr[2] << 2) | (ptr[2] >> 6)) & 0x3FF);
            ptr += 3;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB10: {
        uint32_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            uint32_t rgb = *ptr++;
            buffer[0] = (uint16_t)(rgb & 0x3FF);
            buffer[1] = (uint16_t)((rgb >> 10) & 0x3FF);
            buffer[2] = (uint16_t)((rgb >> 20) & 0x3FF);
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB16: {
        uint16_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = (uint16_t)((ptr[0] >> 6) & 0x3FF);
            buffer[1] = (uint16_t)((ptr[1] >> 6) & 0x3FF);
            buffer[2] = (uint16_t)((ptr[2] >> 6) & 0x3FF);
            ptr += 3;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGBA8: {
        uint16_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = (uint16_t)(((ptr[0] << 2) | (ptr[0] >> 6)) & 0x3FF);
            buffer[1] = (uint16_t)(((ptr[1] << 2) | (ptr[1] >> 6)) & 0x3FF);
            buffer[2] = (uint16_t)(((ptr[2] << 2) | (ptr[2] >> 6)) & 0x3FF);
            ptr += 4;
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGB10_A2: {
        uint32_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            uint32_t rgb_a = *ptr++;
            buffer[0] = (uint16_t)(rgb_a & 0x3FF);
            buffer[1] = (uint16_t)((rgb_a >> 10) & 0x3FF);
            buffer[2] = (uint16_t)((rgb_a >> 20) & 0x3FF);
            buffer += 3;
        }
    } break;
    case VIDEO_X264_PIC_FORMAT_RGBA16: {
        uint16_t* ptr = x->data;
        for (size_t i = 0; i < count; i++) {
            buffer[0] = (uint16_t)((ptr[0] >> 6) & 0x3FF);
            buffer[1] = (uint16_t)((ptr[1] >> 6) & 0x3FF);
            buffer[2] = (uint16_t)((ptr[2] >> 6) & 0x3FF);
            ptr += 4;
            buffer += 3;
        }
    } break;
    }
    lock_unlock(&x->lock);

    const int32_t rgb2y_val[3] = {  21260,  71520,   7220 };
    const int32_t rgb2u_val[3] = { -11458, -38542,  50000 };
    const int32_t rgb2v_val[3] = {  50000, -45415,  -4585 };

#define rgb2y(r, g, b) ((r * rgb2y_val[0] + g * rgb2y_val[1] + b * rgb2y_val[2]) / 100000)
#define rgb2u(r, g, b) ((r * rgb2u_val[0] + g * rgb2u_val[1] + b * rgb2u_val[2]) / 100000)
#define rgb2v(r, g, b) ((r * rgb2v_val[0] + g * rgb2v_val[1] + b * rgb2v_val[2]) / 100000)
#define yclamp(y) (uint16_t)clamp(y, 0, 1023)
#define uclamp(u) (uint16_t)clamp(u + 512, 0, 1023)
#define vclamp(v) (uint16_t)clamp(v + 512, 0, 1023);

    const size_t width = x->state.param.i_width;
    const size_t height = x->state.param.i_height;
    const size_t width2 = width / 2 * 2;
    const size_t height2 = height / 2 * 2;
    const size_t width3 = width * 3;
    const size_t height3 = height * 3;
    buffer = x->state.buffer;
    switch (x->state.param.i_csp & X264_CSP_MASK) {
    case X264_CSP_I400: {
        uint16_t* y_plane = (uint16_t*)x->state.pic.img.plane[0];
        for (size_t i = 0; i < height; i++)
            for (size_t j = 0; j < width; j++) {
                uint16_t r = buffer[0];
                uint16_t g = buffer[1];
                uint16_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                *y_plane++ = yclamp(y);
            }
    } break;
    case X264_CSP_I420: {
        uint16_t* y_plane = (uint16_t*)x->state.pic.img.plane[0];
        uint16_t* u_plane = (uint16_t*)x->state.pic.img.plane[1];
        uint16_t* v_plane = (uint16_t*)x->state.pic.img.plane[2];

        size_t i = 0;
        for (; i < height2; i += 2) {
            size_t j = 0;
            for (; j < width2; j += 2) {
                uint16_t r;
                uint16_t g;
                uint16_t b;
                int32_t y;
                int32_t u[4];
                int32_t v[4];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[2] = rgb2u(r, g, b);
                v[2] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[3] = rgb2u(r, g, b);
                v[3] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1] + u[2] + u[3]) / 4);
                *v_plane = vclamp((v[0] + v[1] + v[2] + v[3]) / 4);
                u_plane++;
                v_plane++;
            }

            for (; j < width; j++) {
                uint16_t r;
                uint16_t g;
                uint16_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                r = buffer[width3 + 0];
                g = buffer[width3 + 1];
                b = buffer[width3 + 2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                y_plane[width] = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }
            buffer += width3;
            y_plane += width;
        }

        for (; i < height; i++) {
            size_t x = 0;
            for (; x < width2; x += 2) {
                uint16_t r;
                uint16_t g;
                uint16_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }

            for (; x < width; x++) {
                uint16_t r;
                uint16_t g;
                uint16_t b;
                int32_t y;
                int32_t u;
                int32_t v;
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];

                y = rgb2y(r, g, b);
                u = rgb2u(r, g, b);
                v = rgb2v(r, g, b);
                *y_plane = yclamp(y);

                buffer += 3;
                y_plane++;

                *u_plane = uclamp(u);
                *v_plane = vclamp(v);
                u_plane++;
                v_plane++;
            }
            buffer += width3;
            y_plane += width;
        }
    } break;
    case X264_CSP_I422: {
        uint16_t* y_plane = (uint16_t*)x->state.pic.img.plane[0];
        uint16_t* u_plane = (uint16_t*)x->state.pic.img.plane[1];
        uint16_t* v_plane = (uint16_t*)x->state.pic.img.plane[2];

        for (size_t i = 0; i < height; i++) {
            size_t j = 0;
            for (; j < width2; j += 2) {
                uint16_t r;
                uint16_t g;
                uint16_t b;
                int32_t y;
                int32_t u[2];
                int32_t v[2];
                r = buffer[0];
                g = buffer[1];
                b = buffer[2];
                buffer += 3;

                y = rgb2y(r, g, b);
                u[0] = rgb2u(r, g, b);
                v[0] = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);

                r = buffer[0];
                g = buffer[1];
                b = buffer[2];
                buffer += 3;

                y = rgb2y(r, g, b);
                u[1] = rgb2u(r, g, b);
                v[1] = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);

                *u_plane = uclamp((u[0] + u[1]) / 2);
                *v_plane = vclamp((v[0] + v[1]) / 2);
                u_plane++;
                v_plane++;
            }

            for (; j < width; j++) {
                uint16_t r = buffer[0];
                uint16_t g = buffer[1];
                uint16_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                int32_t u = rgb2u(r, g, b);
                int32_t v = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);
                *u_plane = uclamp(u);
                *v_plane = vclamp(v);
                u_plane++;
                v_plane++;
            }
        }
    } break;
    case X264_CSP_I444: {
        uint16_t* y_plane = (uint16_t*)x->state.pic.img.plane[0];
        uint16_t* u_plane = (uint16_t*)x->state.pic.img.plane[1];
        uint16_t* v_plane = (uint16_t*)x->state.pic.img.plane[2];

        for (size_t i = 0; i < height; i++)
            for (size_t j = 0; j < width; j++) {
                uint16_t r = buffer[0];
                uint16_t g = buffer[1];
                uint16_t b = buffer[2];
                buffer += 3;

                int32_t y = rgb2y(r, g, b);
                int32_t u = rgb2u(r, g, b);
                int32_t v = rgb2v(r, g, b);
                *y_plane++ = yclamp(y);
                *u_plane++ = uclamp(u);
                *v_plane++ = vclamp(v);
            }
    } break;
    }
#undef rgb2y
#undef rgb2u
#undef rgb2v
#undef yclamp
#undef uclamp
#undef vclamp
}
#endif
