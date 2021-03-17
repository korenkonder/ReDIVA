/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect.h"
#include "animation.h"
#include "curve.h"
#include "emitter.h"
#include "parse_file.h"

glitter_effect* FASTCALL glitter_effect_init() {
    glitter_effect* e = force_malloc(sizeof(glitter_effect));
    e->data.color = 0xFFFFFFFF;
    e->data.name_hash = 0xCBF29CE44FD0BFC1;
    return e;
}

void FASTCALL glitter_effect_dispose(glitter_effect* e) {
    free(e->data.a3da);
    vector_ptr_glitter_curve_clear(&e->curve, (void*)&glitter_curve_dispose);
    vector_ptr_glitter_curve_dispose(&e->curve);
    vector_ptr_glitter_emitter_clear(&e->emitters, (void*)&glitter_emitter_dispose);
    vector_ptr_glitter_emitter_dispose(&e->emitters);
    free(e);
}

bool FASTCALL Glitter__Effect__ParseFile(glitter_file_reader* a1, f2_header* header, glitter_effect* a3) {
    f2_header* v9;

    if (!Glitter__Effect__UnpackFile(a1, Glitter__ParseFile__GetDataPointer(header),
        a3, Glitter__ParseFile__GetVersion(header)))
        return false;

    v9 = Glitter__ParseFile__GetSubStructPointer(header);
    if (v9) {
        while (Glitter__ParseFile__ReverseSignatureEndianess(v9) == 'ENRS') {
            v9 = Glitter__ParseFile__CheckForEOFC(v9);
            if (!v9)
                return true;
        }

        Glitter__Animation__ParseFile(a1, v9, &a3->curve);
        while (v9) {
            Glitter__Emitter__ParseFile(a1, v9, a3);
            v9 = Glitter__ParseFile__CheckForEOFC(v9);
        }
    }
    return true;
}

int32_t FASTCALL Glitter__Effect__UnpackFile__GetIndex(glitter_file_reader* a1, int32_t a2) {
    switch (a2) {
    case 0:
        return 15;
    case 1:
        return 54;
    case 2:
        return 0;
    case 3:
        return 7;
    case 4:
        return 106;
    case 5:
        return 108;
    case 6:
        return 109;
    case 7:
        return 123;
    case 8:
        return 142;
    case 9:
        return 144;
    case 10:
        return 145;
    case 11:
        return 159;
    case 12:
        return 194;
    case 13:
        return 184;
    case 14:
        return 183;
    case 15:
        return 197;
    case 16:
        return 191;
    case 17:
        return 190;
    default:
        return 201;
    }
}

bool FASTCALL Glitter__Effect__UnpackFile(glitter_file_reader* a1,
    int32_t* data, glitter_effect* a3, uint32_t efct_version) {
    glitter_effect_a3da* v11; // rax

    a3->scale.x = 1.0;
    a3->scale.y = 1.0;
    a3->scale.z = 1.0;
    a3->scale.w = 1.0;
    a3->data.start_time = 0.0;
    a3->data.a3da = 0;
    a3->data.flags = 0;
    a3->data.name_hash = 0xCBF29CE44FD0BFC1;
    if (efct_version != 6 && efct_version != 7)
        return false;

    a3->data.name_hash = *(uint64_t*)data;
    a3->data.appear_time = (float_t)data[2];
    a3->data.life_time = (float_t)data[3];
    a3->data.start_time = (float_t)data[4];
    a3->data.color = *((uint8_t*)data + 22) | (*((uint8_t*)data + 23) << 24)
        | ((*((uint8_t*)data + 21) | (*((uint8_t*)data + 20) << 8)) << 8);
    if (data[6])
        a3->data.flags |= GLITTER_EFFECT_FLAG_LOOP;
    a3->translation = *(vec3*)(data + 7);
    a3->rotation = *(vec3*)(data + 10);

    if (data[13] & GLITTER_EFFECT_FILE_FLAG_ALPHA)
        a3->data.flags |= GLITTER_EFFECT_FLAG_ALPHA;

    if (data[13] & GLITTER_EFFECT_FILE_FLAG_FOG)
        a3->data.flags |= GLITTER_EFFECT_FLAG_FOG;
    else if (data[13] & GLITTER_EFFECT_FILE_FLAG_FOG_HEIGHT)
        a3->data.flags |= GLITTER_EFFECT_FLAG_FOG_HEIGHT;

    if (data[13] & GLITTER_EFFECT_FILE_FLAG_EMISSION)
        a3->data.flags |= GLITTER_EFFECT_FLAG_EMISSION;

    if (efct_version == 7) {
        a3->data.emission = *(float_t*)(data + 14);
        data++;
    }

    if (data[14] == 1)
        a3->data.flags |= GLITTER_EFFECT_FLAG_LOCAL;
    else if (data[14] == 2) {
        data = data + 15;
        v11 = force_malloc(sizeof(glitter_effect_a3da));
        a3->data.a3da = v11;
        if (v11) {
            v11->object_index = *data;
            v11->flags = data[1];
            v11->flags |= 0x10000;
            v11->index = Glitter__Effect__UnpackFile__GetIndex(a1, data[2]);
            v11->mesh_name[0] = 0;
        }
    }
    /*else if (data[14] == 3) {
        data = data + 15;
        v10 = sub_14045A570(*(uint64_t*)data);
        if (v10 != -1) {
            v11 = force_malloc(sizeof(glitter_effect_a3da));
            a3->sub.a3da = v11;
            if (v11) {
                v11->object_index = v10;
                v11->flags = data[2];
                v11->index = 201;
                memcpy(v11->mesh_name, (char*)(data + 12), 128);
            }
        }
    }*/
    return true;
}
