/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <dxva2api.h>

namespace MoviePlayLib {
    HRESULT CreateVideoProcessor(IDirect3DDeviceManager9* pDeviceManager,
        const DXVA2_VideoDesc& videoDesc, IDirectXVideoProcessor*& ppOutProcessor);
}
