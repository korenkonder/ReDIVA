/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    const GUID TextureSharedHandleGUID = {
        0xB4127AF5, 0xA697, 0x4037, 0xAB, 0x09, 0xC8, 0xF4, 0xCF, 0x0F, 0x80, 0xAC,
    };

    const GUID VideoIndexGUID = {
        0x1F42D8B6, 0x6A11, 0x4E2C, 0x91, 0x70, 0xF2, 0x01, 0x5D, 0xA7, 0x31, 0xF2
    };

    void MediaStatsLock::GetStats(MediaStats& value) {
        lock.Acquire();
        value = stats;
        lock.Release();
    }

    void MediaStatsLock::SetAudioParams(uint32_t sample_rate, uint32_t channels) {
        lock.Acquire();
        stats.sample_rate = sample_rate;
        stats.channels = channels;
        lock.Release();
    }

    void MediaStatsLock::SetAudioProcessOutput(double_t output_time, double_t sample_time) {
        lock.Acquire();
        stats.audio_process_output_time = output_time;
        stats.audio_process_sample_time = sample_time;
        lock.Release();
    }

    void MediaStatsLock::SetAudioSampleTime(double_t value) {
        lock.Acquire();
        stats.audio_sample_time = value;
        lock.Release();
    }

    void MediaStatsLock::SetAudioTimeAdvance(double_t value) {
        lock.Acquire();
        stats.audio_time_advance = value;
        lock.Release();
    }

    bool MediaStatsLock::HasError() {
        lock.Acquire();
        bool has_error = FAILED(stats.curr_error);
        lock.Release();
        return has_error;
    }

    void MediaStatsLock::SetCurrError(HRESULT hr, IUnknown* object) {
        lock.Acquire();
        stats.curr_error = hr;
        stats.curr_object = object;
        lock.Release();
    }

    void MediaStatsLock::SetMFSamplesCount(
        uint32_t audio_mf_samples_wait_count, uint32_t video_mf_samples_wait_count,
        uint32_t audio_mf_samples_count, uint32_t video_mf_samples_count) {
        lock.Acquire();
        stats.audio_mf_samples_wait_count = audio_mf_samples_wait_count;
        stats.video_mf_samples_wait_count = video_mf_samples_wait_count;
        stats.audio_mf_samples_count = audio_mf_samples_count;
        stats.video_mf_samples_count = video_mf_samples_count;
        lock.Release();
    }

    void MediaStatsLock::SetSessionInfo(double_t duration, bool audio_present, bool video_present) {
        lock.Acquire();
        stats.duration = duration;
        stats.audio_present = audio_present;
        stats.video_present = video_present;
        lock.Release();
    }

    void MediaStatsLock::SetVideoOutputTime(double_t value) {
        lock.Acquire();
        stats.video_output_time = value;
        lock.Release();
    }

    void MediaStatsLock::SetVideoParams(uint32_t width, uint32_t height, uint32_t frame_size_width,
        uint32_t frame_size_height, float_t frame_rate, float_t pixel_aspect_ratio) {
        lock.Acquire();
        stats.width = width;
        stats.height = height;
        stats.frame_size_width = frame_size_width;
        stats.frame_size_height = frame_size_height;
        stats.frame_rate = frame_rate;
        stats.pixel_aspect_ratio = pixel_aspect_ratio;
        lock.Release();
    }

    void MediaStatsLock::SetVideoProcessOutput(double_t output_time, double_t blt_time, double_t sample_time) {
        lock.Acquire();
        stats.video_process_output_time = output_time;
        stats.video_process_blt_time = blt_time;
        stats.video_process_sample_time = sample_time;
        lock.Release();
    }

    void MediaStatsLock::SetVideoSampleTime(double_t value) {
        lock.Acquire();
        stats.video_sample_time = value;
        lock.Release();
    }

    void MediaStatsLock::SetVideoTimeAdvance(double_t value) {
        lock.Acquire();
        stats.video_time_advance = value;
        lock.Release();
    }

    void PrintDebug(const wchar_t* fmt, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vwprintf(fmt, args);
        va_end(args);
#endif
    }
}
