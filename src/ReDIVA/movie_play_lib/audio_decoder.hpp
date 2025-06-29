/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "transform_base.hpp"

namespace MoviePlayLib {
    class AudioDecoder : public TransformBase {
    protected:
        GUID m_formatSubType;
        uint32_t m_sampleFrequency;
        uint32_t m_channelCount;
        uint32_t m_cbBufferBytes;

    public:
        AudioDecoder(HRESULT& hr, PlayerStat_& rStat,
            IMediaClock* pClock, IMediaSource* pSource);
        virtual ~AudioDecoder() override;

    protected:
        virtual HRESULT _process_output() override;
        virtual void _on_input_sample(double_t sampleTime) override;

    public:
        static void Destroy(AudioDecoder* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        HRESULT _on_format_changed();
    };

    extern HRESULT CreateAudioDecoder(PlayerStat_& rStat,
        IMediaClock* pClock, IMediaSource* pSource, IMediaTransform*& pp);
}
