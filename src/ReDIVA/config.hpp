/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(ReDIVA_DEV)
#define BAKE_PNG (0)
#define BAKE_VIDEO (0)
#define BAKE_PV826 (0)
#define BAKE_X_PACK (1)
#define DW_TRANSLATE (0)
#define PV_DEBUG (1)

#define DATA_EDIT (0)

#if BAKE_PNG || BAKE_VIDEO
#define BAKE_BASE_WIDTH (1920)
#define BAKE_BASE_HEIGHT (1080)
#define BAKE_RES_SCALE (2)
#endif

#if BAKE_VIDEO
#undef BAKE_PNG
#elif BAKE_PNG
#endif
#endif
