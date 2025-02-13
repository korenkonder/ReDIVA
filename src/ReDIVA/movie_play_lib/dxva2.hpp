/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <dxva2api.h>

namespace MoviePlayLib {
    HRESULT GetDirectXVideoProcessor(IDirect3DDeviceManager9* d3d_device_manager,
        const DXVA2_VideoDesc* video_desc, IDirectXVideoProcessor*& ptr);
}
