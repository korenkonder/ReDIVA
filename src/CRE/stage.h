/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/database/stage.h"
#include "render_context.h"
#include "auth_3d.h"

typedef struct stage {
    union {
        stage_info* stage;
        stage_info_modern* stage_modern;
    };

    bool modern;
    bool display;
    bool ring;
    bool ground;
    bool sky;
    bool effects;

    uint32_t set_id;
    uint32_t stage_set_id;
    char* light_param_name;
    vector_int32_t auth_3d_ids;
} stage;

extern void stage_init(stage* s);
extern void stage_load(stage* s, data_struct* data, auth_3d_database* auth_3d_db, object_database* obj_db,
    texture_database* tex_db, stage_database* stage_data, char* name, render_context* rctx);
extern void stage_set(stage* s, render_context* rctx);
extern void stage_update(stage* s, render_context* rctx);
extern void stage_update_modern(stage* s, render_context* rctx);
extern void stage_free(stage* s, render_context* rctx);
