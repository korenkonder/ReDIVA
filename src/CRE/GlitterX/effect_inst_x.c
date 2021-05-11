/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_inst_x.h"
#include "../Glitter/counter.h"
#include "../Glitter/random.h"
#include "curve_x.h"
#include "emitter_inst_x.h"
#include "render_group_x.h"

static int32_t FASTCALL glitter_x_effect_inst_get_ext_anim_bone_index(int32_t index);

static int32_t FASTCALL glitter_x_effect_inst_get_ext_anim_bone_index(int32_t index) {
    switch (index) {
    case 0:
        return 15;
    case 1:
        return 54;
    case 2:
        return 0;
    case 3:
        return 7;
    case 4:
        return 98;
    case 5:
        return 100;
    case 6:
        return 101;
    case 7:
        return 115;
    case 8:
        return 132;
    case 9:
        return 136;
    case 10:
        return 137;
    case 11:
        return 151;
    case 12:
        return 186;
    case 13:
        return 176;
    case 14:
        return 175;
    case 15:
        return 189;
    case 16:
        return 183;
    case 17:
        return 182;
    default:
        return 193;
    }
}