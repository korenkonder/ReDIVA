/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <mftransform.h>

namespace MoviePlayLib {
    HRESULT GetMFMediaType(IMFTransform* mf_transform, const GUID& sub_type, IMFMediaType*& ptr);
    HRESULT GetMFTransform(IMFMediaType* mf_media_type, const GUID& sub_type, IMFTransform*& ptr);
    HRESULT MFMediaEventQueryInterface(IMFMediaEvent* mf_media_event, const IID& iid, void** ppvObject);
    HRESULT MFSampleSetAllocator(IMFSample* mf_sample, IMFAsyncCallback* sample_allocator);
}
