/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "app_system_detail.hpp"
#include "../KKdLib/time.hpp"
#include "resolution_mode.hpp"

struct time_pair {
    int64_t value;
    int64_t max;

    inline time_pair() : value(), max() {

    }

    inline void reset() {
        value = 0;
        max = 0;
    }

    inline void reset_max() {
        max = 0;
    }

    inline void set_value(int64_t value) {
        this->value = value;
        max = max_def(max, value);
    }
};

namespace app_system_detail {
    namespace SystemInfo {
        struct MeasureFps {
            int32_t state;
            int64_t max_time;
            time_struct time_start;
            int32_t main_timer;
            float_t fps;
            int64_t retry_count;

            MeasureFps();

            inline void init(int32_t max_time_sec) {
                state = 1;
                max_time = 1000000LL * max_time_sec;
                fps = 60.0f;
                retry_count = 0;
            }

            void exec();
        };

        MeasureFps measure_fps;
    }

    struct SystemWork {
        bool field_0;
        bool field_1;
        bool pause;
        uint32_t main_timer;
        int32_t field_8;
        float_t frame_speed;
        float_t delta_frame_history_frac;
        int32_t delta_frame_history_int;
        bool select_frame_speed;
        float_t frame_rate;
        float_t fps;
        time_pair time_ctrl;
        time_pair time_file;
        time_pair time_calc;
        time_pair time_disp;
        time_pair time_vblank;
        time_pair time_swap;
        time_struct time_ctrl_start;
        time_struct time_file_start;
        time_struct time_calc_start;
        time_struct time_disp_start;
        time_struct time_rndr_start;
        bool vsync_emulation;
        uint32_t field_DC;
        int64_t field_E0;
        float_t next_frame_speed;
        bool next_select_frame_speed;
        int32_t field_F0;
        bool field_F4;

        SystemWork();
    };

    SystemWork system_work;
}

bool main_thread_id_init;
int32_t main_thread_id;

static void delta_frame_history_ctrl();

static void main_thread_id_get();

static void sub_140193580();
static void sub_140194B70();

void finish_calc_process() {
    app_system_detail::system_work.time_calc.set_value(app_system_detail::system_work.time_calc_start.calc_time_int());
}

void finish_render_process() {
    app_system_detail::system_work.time_disp.set_value(app_system_detail::system_work.time_disp_start.calc_time_int());

    time_struct time_swap_start;

    /*extern void app_swap_buffers();
    app_swap_buffers();*/

    app_system_detail::system_work.time_swap.set_value(time_swap_start.calc_time_int());

    app_system_detail::system_work.time_ctrl.set_value(app_system_detail::system_work.time_ctrl_start.calc_time_int());

    sub_140194B70();

    app_system_detail::system_work.main_timer++;
    app_system_detail::SystemInfo::measure_fps.exec();
}

float_t get_anim_frame_speed() {
    return 60.0f / app_system_detail::SystemInfo::measure_fps.fps;
}

float_t get_delta_frame() {
    if (!app_system_detail::system_work.select_frame_speed)
        return app_system_detail::system_work.frame_speed;

    float_t freq_ratio = app_system_detail::system_work.frame_rate / get_fps();
    freq_ratio = max_def(freq_ratio, 1.0f);
    return freq_ratio * app_system_detail::system_work.frame_speed;
}

int32_t get_delta_frame_history_int() {
    return app_system_detail::system_work.delta_frame_history_int;
}

float_t get_fps() {
    return app_system_detail::system_work.fps;
}

float_t get_frame_speed() {
    return app_system_detail::system_work.frame_speed;
}

uint32_t get_main_timer() {
    return app_system_detail::system_work.main_timer;
}

float_t get_measured_fps() {
    return app_system_detail::SystemInfo::measure_fps.fps;
}

bool get_pause() {
    return app_system_detail::system_work.pause;
}

void measure_fps_init(int32_t max_time_sec) {
    app_system_detail::SystemInfo::measure_fps.init(max_time_sec);
}

bool measure_fps_check_state() {
    return app_system_detail::SystemInfo::measure_fps.state == 3;
}

void set_next_frame_speed(float_t value) {
    app_system_detail::system_work.next_frame_speed = value;
}

void set_vsync_emulation(bool value) {
    app_system_detail::system_work.vsync_emulation = value;
}

void start_frame_process() {
    app_system_detail::system_work.time_ctrl_start.get_timestamp();
    app_system_detail::system_work.time_calc_start.get_timestamp();

    app_system_detail::system_work.select_frame_speed = app_system_detail::system_work.next_select_frame_speed;
    app_system_detail::system_work.frame_speed = app_system_detail::system_work.next_frame_speed;

    if (!get_pause())
        delta_frame_history_ctrl();
}

void start_render_process() {
    app_system_detail::system_work.time_disp_start.get_timestamp();
}

void system_work_init(uint32_t a1) {
    app_system_detail::system_work.field_0 = false;
    app_system_detail::system_work.field_1 = false;
    app_system_detail::system_work.pause = false;
    app_system_detail::system_work.main_timer = 0;
    app_system_detail::system_work.field_8 = 0;
    app_system_detail::system_work.frame_speed = 1.0f;
    app_system_detail::system_work.delta_frame_history_frac = 0.0f;
    app_system_detail::system_work.delta_frame_history_int = 0;
    app_system_detail::system_work.select_frame_speed = false;
    app_system_detail::system_work.frame_rate = 60.0f;
    app_system_detail::system_work.fps = 60.0f;
    app_system_detail::system_work.time_ctrl.reset();
    app_system_detail::system_work.time_file.reset();
    app_system_detail::system_work.time_calc.reset();
    app_system_detail::system_work.time_disp.reset();
    app_system_detail::system_work.time_vblank.reset();
    app_system_detail::system_work.time_swap.reset();
    app_system_detail::system_work.next_frame_speed = 1.0f;
    app_system_detail::system_work.next_select_frame_speed = false;
    app_system_detail::system_work.field_F0 = 0;
    app_system_detail::system_work.field_F4 = 0;

    main_thread_id_get();
    //sub_1400DE570();
    sub_140193580();
    /*sub_140194A90();
    sub_1401932F0();
    sub_1400BEAB0();
    sub_14066D890(a1);
    sub_140670680();
    sub_14019AA80();
    sub_1405E2EA0();
    CoInitializeEx(0, 0);*/
}

bool sub_1401921D0() {
    return app_system_detail::system_work.field_0;
}

int32_t sub_140192D00() {
    return app_system_detail::system_work.field_8;
}

void sub_140194880(int32_t a1) {
    if (a1 > 2)
        a1 = 0;
    app_system_detail::system_work.field_8 = a1;
}

app_system_detail::SystemInfo::MeasureFps::MeasureFps()
    : state(), max_time(), main_timer(), fps(), retry_count() {
    init(0);
}

void app_system_detail::SystemInfo::MeasureFps::exec() {
    switch (state) {
    case 1:
        time_start.get_timestamp();
        state = 2;
        main_timer = get_main_timer();
        break;
    case 2: {
        /*int64_t time = time_start.calc_time_int();
        float_t fps = roundf((float_t)(1000000 * (get_main_timer() - main_timer)) / (float_t)time * 100.0f) * 0.01f;
        if (max_time >= time)
            break;

        if (res_window_get()->resolution_mode == RESOLUTION_MODE_HD && (fps < 59.940002f || fps > 60.060001f)) {
            printf_debug("%s: %f(%zu) -> retry\n", "app_system_detail::SystemInfo::MeasureFps::exec", fps, retry_count);
            retry_count++;
            if (retry_count < 3) {
                state = 1;
                break;
            }

            fps = 60.0f;
        }

        this->fps = fps;
        state = 3;
        printf_debug("%s: %f(%zu)\n", "app_system_detail::SystemInfo::MeasureFps::exec", fps, retry_count);*/

        this->fps = 60.0;
        state = 3;
    } break;
    }
}

app_system_detail::SystemWork::SystemWork() : field_0(), field_1(), pause(), main_timer(), field_8(), frame_speed(),
delta_frame_history_frac(), delta_frame_history_int(), select_frame_speed(), frame_rate(), fps(), vsync_emulation(),
field_DC(), field_E0(), next_frame_speed(), next_select_frame_speed(), field_F0(), field_F4() {

}

static void delta_frame_history_ctrl() {
    float_t& delta_frame_history_frac = app_system_detail::system_work.delta_frame_history_frac;
    int32_t& delta_frame_history_int = app_system_detail::system_work.delta_frame_history_int;

    delta_frame_history_frac += get_delta_frame();

    float_t mod;
    delta_frame_history_frac = modff(delta_frame_history_frac, &mod);
    delta_frame_history_int = (int32_t)mod;

    if (delta_frame_history_frac < 0.001f)
        delta_frame_history_frac = 0.0f;
    else if (1.0f - delta_frame_history_frac < 0.001f) {
        delta_frame_history_int++;
        delta_frame_history_frac = 0.0f;
    }
}

static void main_thread_id_get() {
    main_thread_id = GetCurrentThreadId();
    main_thread_id_init = true;
}

static void sub_140193580() {
    set_vsync_emulation(false);
    app_system_detail::system_work.field_E0 = 0;
    app_system_detail::system_work.time_rndr_start.get_timestamp();
}

static void sub_140194B70() {
    int64_t time_rndr = app_system_detail::system_work.time_rndr_start.calc_time_int_get_timestamp();
    app_system_detail::system_work.fps = (float_t)1000000LL / (float_t)time_rndr;

    time_struct time_vblank_start;
    if (app_system_detail::system_work.vsync_emulation) {
        app_system_detail::system_work.field_E0 += time_rndr;
        app_system_detail::system_work.field_DC++;
        app_system_detail::system_work.fps = (float_t)(1000000LL * app_system_detail::system_work.field_DC)
            / (float_t)app_system_detail::system_work.field_E0;
        if (app_system_detail::system_work.field_DC >= 60) {
            app_system_detail::system_work.field_DC /= 2;
            app_system_detail::system_work.field_E0 /= 2;
        }
    }

    app_system_detail::system_work.time_vblank.set_value(time_vblank_start.calc_time_int());
}