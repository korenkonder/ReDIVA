/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "transform_base.hpp"

namespace MoviePlayLib {
    class AudioDecoder : public TransformBase {
    protected:
        GUID output_sub_type;
        uint32_t sample_rate;
        uint32_t channels;
        uint32_t buffer_size;

    public:

        AudioDecoder(HRESULT& hr, MediaStatsLock* media_stats_lock,
            IMediaClock* media_clock, IMediaSource* media_source);
        virtual ~AudioDecoder() override;

        virtual HRESULT ProcessOutput() override;
        virtual void SetSampleTime(double_t value) override;

        HRESULT InitFromMediaSource();

        static HRESULT Create(MediaStatsLock* media_stats_lock,
            IMediaClock* media_clock, IMediaSource* media_source, IMediaTransform*& ptr);
        static void Destroy(AudioDecoder* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
