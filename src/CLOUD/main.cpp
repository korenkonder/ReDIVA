/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/a3da.hpp"
#include "../KKdLib/dsc.hpp"
#include "../KKdLib/f2/struct.hpp"
#include "../CRE/auth_3d.hpp"
#include "main.hpp"
#include "shared.hpp"
#include <intrin.h>
#include <processthreadsapi.h>
#include <timeapi.h>

bool aes_ni;

inline static void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state) {
    if (!arr || length < 1)
        return;

    for (size_t i = 0; i < length; i++) {
        uint32_t x = *state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state = x;
        arr[i] = (uint8_t)x;
    }
}

struct dof_data {
    int32_t flags;
    float_t focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
    float_t quality;
};

struct dft_dsc_data{
    int32_t frames;
    std::vector<dof_data> dof;
    std::vector<int32_t> dof_index;

    dft_dsc_data();
    ~dft_dsc_data();
};

dft_dsc_data::dft_dsc_data() : frames() {

}

dft_dsc_data::~dft_dsc_data() {

}

static bool a3da_to_dof_data(const char* a3da_path, dft_dsc_data& data) {
    a3da cam_a3da_file;
    a3da::load_file(&cam_a3da_file, "DOF\\auth_3d\\", a3da_path, 0);

    if (!cam_a3da_file.dof.has_dof) {
        data.frames = 0;
        return false;
    }

    auth_3d cam_a3da;
    cam_a3da.load(&cam_a3da_file, 0, 0);

    data.frames = (int32_t)cam_a3da.play_control.size;

    data.dof.reserve(data.frames + 1ULL);
    data.dof_index.reserve(data.frames);

    data.dof.push_back({ 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });

    auth_3d_camera_root* cr = &cam_a3da.camera_root[0];

    for (int32_t i = 0; i < data.frames; i++) {
        cam_a3da.dof.model_transform.interpolate((float_t)i);
        bool enable = fabs(cam_a3da.dof.model_transform.rotation_value.z) > 0.000001f;
        if (!enable) {
            data.dof_index.push_back(0);
            continue;
        }

        float_t focus;
        vec3_distance(cam_a3da.dof.model_transform.translation_value, cr->view_point_value, focus);

        dof_data d;
        d.flags = 0x1F;
        d.focus = focus;
        d.focus_range = cam_a3da.dof.model_transform.scale_value.y;
        d.fuzzing_range = cam_a3da.dof.model_transform.rotation_value.y;
        d.ratio = cam_a3da.dof.model_transform.rotation_value.z;
        d.quality = 1.0f;

        bool found = false;
        for (dof_data& j : data.dof)
            if (!memcmp(&d, &j, sizeof(dof_data))) {
                data.dof_index.push_back((int32_t)(&j - data.dof.data()));
                found = true;
                break;
            }

        if (found)
            continue;

        data.dof_index.push_back((int32_t)data.dof.size());
        data.dof.push_back(d);
    }
    return true;
}

static void a3da_to_dft_dsc(int32_t pv_id) {
    char buf[0x100];

    sprintf_s(buf, sizeof(buf), "DOF\\script\\pv_%03d_hard.dsc", pv_id);
    stream s_src_dsc;
    s_src_dsc.open(buf, "rb");

    size_t src_dsc_length = s_src_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_src_dsc.read(src_dsc_data, src_dsc_length);
    s_src_dsc.close();

    dsc d_src = dsc();
    d_src.parse(src_dsc_data, src_dsc_length, DSC_FT);
    free(src_dsc_data);
    d_src.rebuild();

    dsc_get_func_id src_get_func_id = dsc_type_get_dsc_get_func_id(d_src.type);

    int32_t src_time_func_id = src_get_func_id("TIME");
    int32_t src_pv_branch_mode_func_id = src_get_func_id("PV_BRANCH_MODE");
    int32_t src_data_camera_func_id = src_get_func_id("DATA_CAMERA");

    int32_t time = -1;
    int32_t pv_branch_mode = -1;
    int32_t pv_branch_mode_2nd_camera_time = -1;
    for (dsc_data& i : d_src.data) {
        if (i.func == src_time_func_id) {
            time = ((int32_t*)d_src.get_func_data(&i))[0];
            continue;
        }
        else if (i.func == src_pv_branch_mode_func_id)
            pv_branch_mode = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_data_camera_func_id) {
            int32_t camera = ((int32_t*)d_src.get_func_data(&i))[0];
            if (camera && pv_branch_mode == 2 && pv_branch_mode_2nd_camera_time == -1) {
                pv_branch_mode_2nd_camera_time = time;
                break;
            }
        }
    }

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE.a3da", pv_id);

    dft_dsc_data dft_data;
    bool base_cam = a3da_to_dof_data(buf, dft_data);

    size_t base_index_end = dft_data.dof_index.size();
    int32_t base_frames = dft_data.frames;

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_PARTS02.a3da", pv_id);

    bool parts02_cam = a3da_to_dof_data(buf, dft_data);

    size_t parts02_index_end = dft_data.dof_index.size();
    int32_t parts02_frames = dft_data.frames;

    dsc d = dsc(DSC_FT, 0x15122517);

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(d.type);
    dsc_get_func_length get_func_length = d.get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(d.type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");
    int32_t dof_func_id = get_func_id("DOF");

    const char* time_func_name = get_func_name(time_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);
    const char* dof_func_name = get_func_name(dof_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);
    int32_t dof_func_length = get_func_length(dof_func_id);

    *d.add_func(pv_branch_mode_func_name,
        pv_branch_mode_func_id, pv_branch_mode_func_length) = 0;

    int32_t last_dof_index = -1;
    for (int32_t i = 0; i < base_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        if (time >= pv_branch_mode_2nd_camera_time) {
            int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
                pv_branch_mode_func_id, pv_branch_mode_func_length);
            pv_branch_mode_data[0] = 1;
        }

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i];
    }

    last_dof_index = -1;
    for (int32_t i = 0; i < parts02_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i + base_index_end])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000
        time += pv_branch_mode_2nd_camera_time;

        if (time < pv_branch_mode_2nd_camera_time)
            continue;

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
            pv_branch_mode_func_id, pv_branch_mode_func_length);
        pv_branch_mode_data[0] = 2;

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i + base_index_end] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i + base_index_end];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i + base_index_end];
    }

    d.rebuild();

    void* dsc_data = 0;
    size_t dsc_length = 0;
    d.unparse(&dsc_data, &dsc_length);

    sprintf_s(buf, sizeof(buf), "DOF\\pv_script\\pv_%03d_dof.dsc", pv_id);
    stream s_dsc;
    s_dsc.open(buf, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();

    free(dsc_data);

    stream s_dft;
    s_dft.open();
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    ee = { 0, 1, 8, 1 };
    ee.sub.push_back({ 0, 2, ENRS_DWORD });
    e.vec.push_back(ee);
    off = 8;
    off = align_val(off, 0x10);

    ee = { off, 1, 24, (uint32_t)dft_data.dof.size() };
    ee.sub.push_back({ 0, 6, ENRS_DWORD });
    e.vec.push_back(ee);
    off = (uint32_t)(dft_data.dof.size() * 24ULL);
    off = align_val(off, 0x10);

    s_dft.write_uint32_t((int32_t)dft_data.dof.size());
    io_write_offset_f2_pof_add(s_dft, 0x10, 0x40, &pof);
    s_dft.align_write(0x10);
    s_dft.write(dft_data.dof.data(), sizeof(dof_data) * dft_data.dof.size());

    f2_struct st;
    s_dft.align_write(0x10);
    s_dft.copy(st.data);
    s_dft.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('DOFT');
    st.header.length = 0x40;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;
    st.header.inner_signature = 0x03;

    sprintf_s(buf, sizeof(buf), "DOF\\post_process_table\\pv%03d.dft", pv_id);
    st.write(buf, true, false);
}

bool close;

struct thread {
    HANDLE handle;
    DWORD id;
};

int32_t wmain(int32_t argc, wchar_t** argv) {
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    timeBeginPeriod(1);
    SetProcessDPIAware();

    int32_t cpuid_data[4];
    __cpuid(cpuid_data, 1);
    aes_ni = cpuid_data[2] & 0x2000000 ? true : false;

    //a3da_to_dft_dsc(269);

    render_init_struct ris;
    ris.res.x = 0;
    ris.res.y = 0;
    ris.scale = 0.0f;

    thread render, sound;
    HANDLE h[2];
    h[0] = render.handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)render_main, &ris, 0, &render.id);
    h[1] = sound.handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)sound_main, 0, 0, &sound.id);

    if (render.handle)
        SetThreadDescription(render.handle, L"Render Thread");
    if (sound.handle)
        SetThreadDescription(sound.handle, L"Sound Thread");

    lock_init(&state_lock);
    state = RENDER_UNINITIALIZED;
    if (render.handle && sound.handle)
        WaitForMultipleObjects(2, h, true, INFINITE);
    lock_free(&state_lock);

    if (render.handle)
        CloseHandle(render.handle);
    if (sound.handle)
        CloseHandle(sound.handle);
    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
