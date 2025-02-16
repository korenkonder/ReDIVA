/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#define BAKE_DOF (0)
#define BAKE_PNG (0)
#define BAKE_VIDEO (0)
#define BAKE_PV826 (0)
#define BAKE_X_PACK (1)
#define PV_DEBUG (1)

#define FACE_ANIM (0)

#if BAKE_PNG || BAKE_VIDEO
#define BAKE_BASE_WIDTH (1920)
#define BAKE_BASE_HEIGHT (1080)
#define BAKE_RES_SCALE (2)
#endif

#if BAKE_DOF || BAKE_PV826
#define BAKE_FAST (1)
#endif

#if BAKE_VIDEO
#define BAKE_VIDEO_ALPHA (0)
#undef BAKE_PNG
#elif BAKE_PNG
#endif
