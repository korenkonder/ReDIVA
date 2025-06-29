/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    const GUID SAMPLE_RESET_TOKEN = {
        0x1F42D8B6, 0x6A11, 0x4E2C, 0x91, 0x70, 0xF2, 0x01, 0x5D, 0xA7, 0x31, 0xF2
    };

    const GUID WDDM_SHARED_HANDLE = {
        0xB4127AF5, 0xA697, 0x4037, 0xAB, 0x09, 0xC8, 0xF4, 0xCF, 0x0F, 0x80, 0xAC,
    };

    void PlayerStat_::GetStats(PlayerStat& value) {
        m_lock.Acquire();
        value = m_st;
        m_lock.Release();
    }

    void PlayerStat_::SetAudioParams(uint32_t sample_freq, uint32_t channels) {
        m_lock.Acquire();
        m_st.sample_freq = sample_freq;
        m_st.channels = channels;
        m_lock.Release();
    }

    void PlayerStat_::SetAudioDecodeTime(
        double_t decode_audio_elapse, double_t decode_audio_sample_time) {
        m_lock.Acquire();
        m_st.decode_audio_elapse = decode_audio_elapse;
        m_st.decode_audio_sample_time = decode_audio_sample_time;
        m_lock.Release();
    }

    void PlayerStat_::SetSourceAudioSampleTime(double_t value) {
        m_lock.Acquire();
        m_st.source_audio_sample_time = value;
        m_lock.Release();
    }

    void PlayerStat_::SetScheduleAudioDrift(double_t value) {
        m_lock.Acquire();
        m_st.schedule_audio_drift = value;
        m_lock.Release();
    }

    bool PlayerStat_::HasError() {
        m_lock.Acquire();
        bool error = FAILED(m_st.error);
        m_lock.Release();
        return error;
    }

    void PlayerStat_::SetError(HRESULT error, void* error_object) {
        m_lock.Acquire();
        m_st.error = error;
        m_st.error_object = error_object;
        m_lock.Release();
    }

    void PlayerStat_::SetSamplesCount(
        uint32_t source_audio_queue_count, uint32_t source_video_queue_count,
        uint32_t decode_audio_queue_count, uint32_t decode_video_queue_count) {
        m_lock.Acquire();
        m_st.source_audio_queue_count = source_audio_queue_count;
        m_st.source_video_queue_count = source_video_queue_count;
        m_st.decode_audio_queue_count = decode_audio_queue_count;
        m_st.decode_video_queue_count = decode_video_queue_count;
        m_lock.Release();
    }

    void PlayerStat_::SetSessionInfo(double_t duration, bool enable_audio, bool enable_video) {
        m_lock.Acquire();
        m_st.duration = duration;
        m_st.enable_audio = enable_audio;
        m_st.enable_video = enable_video;
        m_lock.Release();
    }

    void PlayerStat_::SetVideoLockElapse(double_t value) {
        m_lock.Acquire();
        m_st.video_lock_elapse = value;
        m_lock.Release();
    }

    void PlayerStat_::SetVideoParams(uint32_t present_width, uint32_t present_height,
        uint32_t raw_width, uint32_t raw_height, float_t framerate, float_t pixelaspect) {
        m_lock.Acquire();
        m_st.present_width = present_width;
        m_st.present_height = present_height;
        m_st.raw_width = raw_width;
        m_st.raw_height = raw_height;
        m_st.framerate = framerate;
        m_st.pixelaspect = pixelaspect;
        m_lock.Release();
    }

    void PlayerStat_::SetVideoDecodeTime(double_t decode_video_elapse,
        double_t decode_video_conv_elapse, double_t decode_video_sample_time) {
        m_lock.Acquire();
        m_st.decode_video_elapse = decode_video_elapse;
        m_st.decode_video_conv_elapse = decode_video_conv_elapse;
        m_st.decode_video_sample_time = decode_video_sample_time;
        m_lock.Release();
    }

    void PlayerStat_::SetSourceVideoSampleTime(double_t value) {
        m_lock.Acquire();
        m_st.source_video_sample_time = value;
        m_lock.Release();
    }

    void PlayerStat_::SetScheduleVideoDrift(double_t value) {
        m_lock.Acquire();
        m_st.schedule_video_drift = value;
        m_lock.Release();
    }

    void trace(const wchar_t* fmt, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vwprintf(fmt, args);
        va_end(args);
#endif
    }
}
