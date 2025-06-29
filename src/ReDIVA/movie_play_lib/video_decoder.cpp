/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "video_decoder.hpp"
#include "dx_surface_buffer.hpp"
#include "media_foundation.hpp"
#include "tracked_sample.hpp"
#include <mfapi.h>

namespace MoviePlayLib {
    VideoDecoder::VideoDecoder(HRESULT& hr, PlayerStat_& rStat, IMediaClock* pClock,
        IMediaSource* pSource, IDirect3DDeviceManager9* pDeviceManager, IDirect3DDevice9Ex* pDevice)
        : TransformBase(hr, rStat, pClock, pSource), m_formatSubType(MFVideoFormat_NV12),
        m_pD3D9Manager(), m_pD3D9Device(), m_pProcessor(), m_cbBufferBytes(), m_yuvWidth(), m_yuvHeight(),
        m_frameWidth(), m_frameHeight(), m_frameDuration(), m_frameRate(), m_pixelAspect(), 
        m_videoArea(), m_videoDesc(), m_bltParams(), m_videoSample(), m_pool(),
        m_asynccb_OnReturnSample(&VideoDecoder::_async_callback_func, this) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        if (SUCCEEDED(hr)) {
            IMFMediaType* pType = 0;
            hr = pSource->GetVideoMediaType(&pType);
            if (SUCCEEDED(hr))
                hr = CreateDecoder(pType, m_formatSubType, m_pTransform);

            if (pType) {
                pType->Release();
                pType = 0;
            }

            if (SUCCEEDED(hr)) {
                m_pD3D9Manager = pDeviceManager;
                pDeviceManager->AddRef();
                m_pD3D9Device = pDevice;
                pDevice->AddRef();

                if (SUCCEEDED(hr)) {
                    hr = m_pTransform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)pDeviceManager);
                    if (SUCCEEDED(hr))
                        hr = _on_format_changed();
                }
            }
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    VideoDecoder::~VideoDecoder() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    void VideoDecoder::_on_shutdown() {
        if (m_pProcessor) {
            m_pProcessor->Release();
            m_pProcessor = 0;
        }

        if (m_pD3D9Device) {
            m_pD3D9Device->Release();
            m_pD3D9Device = 0;
        }

        if (m_pD3D9Manager) {
            m_pD3D9Manager->Release();
            m_pD3D9Manager = 0;
        }

        TransformBase::_on_shutdown();
    }

    HRESULT VideoDecoder::_process_output() {
        if (m_pool.size() < 3)
            return TRUE;

        IMFSample* pSample = m_pool.pop();
        if (!pSample)
            return TRUE;

        HRESULT hr = MFSampleSetAllocator(pSample, &m_asynccb_OnReturnSample);

        MFT_OUTPUT_DATA_BUFFER outputSample = {};
        DWORD outputStatus = 0;

        int64_t decodeVideoBegin;
        HRESULT hrDecodeVideo;
        int64_t decodeVideoEnd;

        int64_t hnsSampleTime;
        int64_t hnsSampleDuration;
        DWORD dwSampleFlags;
        IDirect3DSurface9* pSrcSurface;
        IDirect3DSurface9* pDstSurface;
        IMFMediaBuffer* pBuffer;
        IMFGetService* pService;
        if (FAILED(hr))
            goto End;

        decodeVideoBegin = GetTimestamp();
        hrDecodeVideo = m_pTransform->ProcessOutput(0, 1, &outputSample, &outputStatus);
        decodeVideoEnd = GetTimestamp();

        if (FAILED(hrDecodeVideo)) {
            switch (hrDecodeVideo) {
            case MF_E_TRANSFORM_STREAM_CHANGE:
                hr = _on_format_changed();
                break;
            case MF_E_TRANSFORM_NEED_MORE_INPUT:
                hr = S_OK;
                if (m_decodeState == DecodeState_DrainEndOfStream)
                    m_decodeState = DecodeState_EndOfStream;
                else
                    m_decodeState = DecodeState_NeedMoreInput;
                break;
            default:
                hr = S_FALSE;
                m_decodeState = DecodeState_NeedMoreInput;
                break;
            }
            goto End;
        }

        hnsSampleTime = 0;
        hnsSampleDuration = 0;
        dwSampleFlags = 0;
        hr = outputSample.pSample->GetSampleTime(&hnsSampleTime);
        if (SUCCEEDED(hr)) {
            hr = outputSample.pSample->GetSampleDuration(&hnsSampleDuration);
            if (SUCCEEDED(hr))
                hr = outputSample.pSample->GetSampleFlags(&dwSampleFlags);
        }

        if (hnsSampleDuration < 1)
            hnsSampleDuration = m_frameDuration;

        if (FAILED(hr))
            goto End;

        hr = pSample->SetSampleTime(hnsSampleTime);
        if (FAILED(hr))
            goto End;

        hr = pSample->SetSampleDuration(hnsSampleDuration);
        if (FAILED(hr))
            goto End;

        hr = pSample->SetSampleFlags(dwSampleFlags);
        if (FAILED(hr))
            goto End;

        pBuffer = 0;
        pDstSurface = 0;
        hr = pSample->GetBufferByIndex(0, &pBuffer);
        if (SUCCEEDED(hr))
            hr = pBuffer->QueryInterface(IID_PPV_ARGS(&pDstSurface));

        if (pBuffer) {
            pBuffer->Release();
            pBuffer = 0;
        }

        pBuffer = 0;
        pService = 0;
        pSrcSurface = 0;
        if (SUCCEEDED(hr)) {
            hr = outputSample.pSample->GetBufferByIndex(0, &pBuffer);
            if (SUCCEEDED(hr)) {
                hr = pBuffer->QueryInterface(IID_PPV_ARGS(&pService));
                if (SUCCEEDED(hr))
                    hr = pService->GetService(MR_BUFFER_SERVICE, IID_PPV_ARGS(&pSrcSurface));
            }
        }

        if (pService) {
            pService->Release();
            pService = 0;
        }

        if (pBuffer) {
            pBuffer->Release();
            pBuffer = 0;
        }

        if (SUCCEEDED(hr)) {
            m_videoSample.SrcSurface = pSrcSurface;
            const int64_t decodeVideoConvBegin = GetTimestamp();
            hr = m_pProcessor->VideoProcessBlt(pDstSurface, &m_bltParams, &m_videoSample, 1, 0);
            const int64_t decodeVideoConvEnd = GetTimestamp();
            if (SUCCEEDED(hr)) {
                m_outputQueue.push(pSample, false);

                LARGE_INTEGER freq;
                QueryPerformanceFrequency(&freq);
                m_rStat.SetVideoDecodeTime(
                    CalcTimeMsec(decodeVideoBegin, decodeVideoEnd),
                    CalcTimeMsec(decodeVideoConvBegin, decodeVideoConvEnd),
                    (double_t)hnsSampleTime * 0.0000001);
            }
        }

        if (pSrcSurface) {
            pSrcSurface->Release();
            pSrcSurface = 0;
        }

        if (pDstSurface) {
            pDstSurface->Release();
            pDstSurface = 0;
        }

    End:
        if (outputSample.pSample) {
            outputSample.pSample->Release();
            outputSample.pSample = 0;
        }

        if (outputSample.pEvents) {
            outputSample.pEvents->Release();
            outputSample.pEvents = 0;
        }

        pSample->Release();
        return hr;
    }

    void VideoDecoder::_on_input_sample(double_t sampleTime) {
        m_rStat.SetSourceVideoSampleTime(sampleTime);
    }

    HRESULT VideoDecoder::OnReturnSample(IMFAsyncResult* pAsyncResult) {
        TrackedSample* pTrackedSample = 0;
        if (SUCCEEDED(pAsyncResult->QueryInterface(__uuidof(TrackedSample),
            (void**)&pTrackedSample)) && pTrackedSample)
            m_pool.push(pTrackedSample, true);

        if (pTrackedSample) {
            pTrackedSample->Release();
            pTrackedSample = 0;
        }
        return S_OK;
    }

    inline void VideoDecoder::Destroy(VideoDecoder* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT VideoDecoder::_async_callback_func(IMFAsyncResult* pAsyncResult) {
        return OnReturnSample(pAsyncResult);
    }

    HRESULT VideoDecoder::_on_format_changed() {
        IMFMediaType* pInputType = 0;
        IMFMediaType* pOutputType = 0;

        HRESULT hr;
        hr = m_pSource->GetVideoMediaType(&pInputType);
        if (SUCCEEDED(hr))
            hr = MoviePlayLib::SelectDecoderOutputFormat(m_pTransform, m_formatSubType, pOutputType);

        uint32_t cbBufferBytes = 0;
        MFT_OUTPUT_STREAM_INFO streamInfo = {};
        if (SUCCEEDED(hr)) {
            hr = m_pTransform->GetOutputStreamInfo(0, &streamInfo);
            cbBufferBytes = streamInfo.cbSize;
        }
        m_cbBufferBytes = cbBufferBytes;

        if (SUCCEEDED(hr)) {
            uint64_t frameSize = 0;
            hr = pOutputType->GetUINT64(MF_MT_FRAME_SIZE, &frameSize);
            if (SUCCEEDED(hr)) {
                m_frameHeight = (uint32_t)frameSize;
                m_frameWidth = (uint32_t)(frameSize >> 32);
            }
        }

        m_yuvWidth = m_frameWidth;
        m_yuvHeight = m_frameHeight;

        if (SUCCEEDED(hr)) {
            uint64_t frameRate = 0;
            if (SUCCEEDED(pOutputType->GetUINT64(MF_MT_FRAME_RATE, &frameRate))) {
                m_frameRate.Denominator = (uint32_t)frameRate;
                m_frameRate.Numerator = (uint32_t)(frameRate >> 32);
            }
            else {
                m_frameRate.Denominator = 1001;
                m_frameRate.Numerator = 60000;
            }

            m_frameDuration = MFllMulDiv(m_frameRate.Denominator, 10000000, m_frameRate.Numerator, 0);

            if (SUCCEEDED(pOutputType->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE,
                (UINT8*)&m_videoArea, sizeof(m_videoArea), 0))) {
                if (m_videoArea.Area.cx > 0 && m_videoArea.Area.cy > 0) {
                    m_frameWidth = m_videoArea.Area.cx;
                    m_frameHeight = m_videoArea.Area.cy;
                }
            }
            else {
                m_videoArea.OffsetX = {};
                m_videoArea.OffsetY = {};
                m_videoArea.Area.cx = m_frameWidth;
                m_videoArea.Area.cy = m_frameHeight;
            }

            uint64_t pixelAspectRatio = 0;
            if (SUCCEEDED(pOutputType->GetUINT64(MF_MT_PIXEL_ASPECT_RATIO, &pixelAspectRatio))) {
                m_pixelAspect.Denominator = (uint32_t)pixelAspectRatio;
                m_pixelAspect.Numerator = (uint32_t)(pixelAspectRatio >> 32);
            }
            else {
                m_pixelAspect.Numerator = 1;
                m_pixelAspect.Denominator = 1;
            }

            m_videoDesc = {};
            m_videoDesc.SampleWidth = m_frameWidth;
            m_videoDesc.SampleHeight = m_frameHeight;

            m_videoDesc.SampleFormat.SampleFormat = DXVA2_SampleProgressiveFrame;
            m_videoDesc.SampleFormat.VideoChromaSubsampling
                = DXVA2_VideoChromaSubsampling_ProgressiveChroma | DXVA2_VideoChromaSubsampling_Vertically_Cosited;
            m_videoDesc.SampleFormat.VideoLighting = DXVA2_VideoLighting_dim;

            uint32_t nominalRange;
            if (FAILED(pOutputType->GetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, &nominalRange)))
                nominalRange = DXVA2_NominalRange_Normal;
            m_videoDesc.SampleFormat.NominalRange = nominalRange;

            uint32_t videoPrimaries;
            if (FAILED(pOutputType->GetUINT32(MF_MT_VIDEO_PRIMARIES, &videoPrimaries)))
                videoPrimaries = DXVA2_VideoPrimaries_BT709;
            m_videoDesc.SampleFormat.VideoPrimaries = videoPrimaries;

            uint32_t videoTransferMatrix;
            if (FAILED(pOutputType->GetUINT32(MF_MT_YUV_MATRIX, &videoTransferMatrix)))
                videoTransferMatrix = DXVA2_VideoTransferMatrix_BT709;
            m_videoDesc.SampleFormat.VideoTransferMatrix = videoTransferMatrix;

            uint32_t videoTransferFunction;
            if (FAILED(pOutputType->GetUINT32(MF_MT_TRANSFER_FUNCTION, &videoTransferFunction)))
                videoTransferFunction = DXVA2_VideoTransFunc_709;
            m_videoDesc.SampleFormat.VideoTransferFunction = videoTransferFunction;

            m_videoDesc.Format = (D3DFORMAT)m_formatSubType.Data1;
            m_videoDesc.InputSampleFreq.Numerator = m_frameRate.Numerator;
            m_videoDesc.InputSampleFreq.Denominator = m_frameRate.Denominator;
            m_videoDesc.OutputFrameFreq.Numerator = m_frameRate.Numerator;
            m_videoDesc.OutputFrameFreq.Denominator = m_frameRate.Denominator;

            if (m_pProcessor) {
                m_pProcessor->Release();
                m_pProcessor = 0;
            }

            hr = CreateVideoProcessor(m_pD3D9Manager, m_videoDesc, m_pProcessor);
            if (SUCCEEDED(hr)) {
                DXVA2_ValueRange procAmpBrightness = {};
                DXVA2_ValueRange procAmpContrast = {};
                DXVA2_ValueRange procAmpHue = {};
                DXVA2_ValueRange procAmpSaturation = {};
                m_pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Brightness, &procAmpBrightness);
                m_pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Contrast, &procAmpContrast);
                m_pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Hue, &procAmpHue);
                m_pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Saturation, &procAmpSaturation);

                m_bltParams = {};
                m_bltParams.TargetFrame = 0;
                m_bltParams.TargetRect.left = 0;
                m_bltParams.TargetRect.top = 0;
                m_bltParams.TargetRect.right = m_frameWidth;
                m_bltParams.TargetRect.bottom = m_frameHeight;
                m_bltParams.BackgroundColor = { 0x8000, 0x8000, 0x1000, 0xFFFF };
                m_bltParams.DestFormat.SampleFormat = DXVA2_SampleProgressiveFrame;
                m_bltParams.DestFormat.VideoChromaSubsampling = DXVA2_VideoChromaSubsampling_Unknown;
                m_bltParams.DestFormat.NominalRange = DXVA2_NominalRange_Normal;
                m_bltParams.DestFormat.VideoTransferMatrix = DXVA2_VideoTransferMatrix_Unknown;
                m_bltParams.DestFormat.VideoLighting = DXVA2_VideoLighting_dim;
                m_bltParams.DestFormat.VideoPrimaries = DXVA2_VideoPrimaries_Unknown;
                m_bltParams.DestFormat.VideoTransferFunction = DXVA2_VideoTransFunc_Unknown;
                m_bltParams.ProcAmpValues.Brightness = procAmpBrightness.DefaultValue;
                m_bltParams.ProcAmpValues.Contrast = procAmpContrast.DefaultValue;
                m_bltParams.ProcAmpValues.Hue = procAmpHue.DefaultValue;
                m_bltParams.ProcAmpValues.Saturation = procAmpSaturation.DefaultValue;
                m_bltParams.Alpha.ll = 0x10000;

                m_videoSample = {};
                m_videoSample.Start = 0;
                m_videoSample.End = 0;
                m_videoSample.SampleFormat = m_videoDesc.SampleFormat;
                m_videoSample.SrcSurface = 0;
                m_videoSample.SrcRect.left = m_videoArea.OffsetX.value;
                m_videoSample.SrcRect.top = m_videoArea.OffsetY.value;
                m_videoSample.SrcRect.right = m_videoArea.OffsetX.value + m_frameWidth;
                m_videoSample.SrcRect.bottom = m_videoArea.OffsetY.value + m_frameHeight;
                m_videoSample.DstRect.left = 0;
                m_videoSample.DstRect.top = 0;
                m_videoSample.DstRect.right = m_frameWidth;
                m_videoSample.DstRect.bottom = m_frameHeight;
                m_videoSample.PlanarAlpha.ll = 0x10000;

                uint32_t video_index = m_pool.clear();
                for (int32_t i = 0; i < 10; i++) {
                    IMFSample* pSample = 0;
                    IMFMediaBuffer* pBuffer = 0;
                    IDirect3DTexture9* pTexture = 0;
                    HANDLE wddmShareHandle = 0;
                    if (SUCCEEDED(hr)) {
                        hr = m_pD3D9Device->CreateTexture(m_frameWidth, m_frameHeight,
                            1, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, &wddmShareHandle);
                        if (SUCCEEDED(hr)) {
                            hr = CreateDXSurfaceBuffer(pTexture, pBuffer);
                            if (SUCCEEDED(hr)) {
                                hr = CreateTrackedSample(pBuffer, video_index, pSample);
                                if (SUCCEEDED(hr)) {
                                    hr = pSample->SetUINT64(WDDM_SHARED_HANDLE, (UINT64)wddmShareHandle);
                                    if (SUCCEEDED(hr))
                                        m_pool.push(pSample, false);
                                }
                            }
                        }

                        if (pTexture) {
                            pTexture->Release();
                            pTexture = 0;
                        }

                        if (pSample) {
                            pSample->Release();
                            pSample = 0;
                        }

                        if (pBuffer) {
                            pBuffer->Release();
                            pBuffer = 0;
                        }
                    }
                }

                if (SUCCEEDED(hr))
                    m_rStat.SetVideoParams(m_frameWidth, m_frameHeight, m_frameWidth, m_frameHeight,
                        (float_t)(int32_t)m_frameRate.Numerator / (float_t)(int32_t)m_frameRate.Denominator,
                        (float_t)(int32_t)m_pixelAspect.Numerator / (float_t)(int32_t)m_pixelAspect.Denominator);
            }
        }

        if (pOutputType) {
            pOutputType->Release();
            pOutputType = 0;
        }

        if (pInputType) {
            pInputType->Release();
            pInputType = 0;
        }
        return hr;
    }

    HRESULT CreateVideoDecoder(PlayerStat_& rStat, IMediaClock* pClock, IMediaSource* pSource,
        IDirect3DDeviceManager9* pDeviceManager, IDirect3DDevice9Ex* pDevice, IMediaTransform*& pp) {
        HRESULT hr = S_OK;
        VideoDecoder* p = new VideoDecoder(hr, rStat,
            pClock, pSource, pDeviceManager, pDevice);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            pp = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }
}
