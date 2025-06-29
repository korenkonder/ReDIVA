/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <mftransform.h>

namespace MoviePlayLib {
    HRESULT CreateDecoder(IMFMediaType* pInputType, const GUID& rOutputSubType, IMFTransform*& ppOutTransform);
    HRESULT MFMediaEventQueryInterface(IMFMediaEvent* pEvent, const IID& iid, void** ppvObject);
    HRESULT MFSampleSetAllocator(IMFSample* pSample, IMFAsyncCallback* pAsyncCallback);
    HRESULT SelectDecoderOutputFormat(IMFTransform* pTransform,
        const GUID& outputSubType, IMFMediaType*& ppOutMediaType);
}
