/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_table.hpp"
#include "../KKdLib/mat.hpp"
#include "gl.hpp"

enum uniform_name {
    U_NONE          = 0x00,
    U01             = 0x01,
    U_ALPHA_TEST    = 0x02,
    U_ANISO         = 0x03,
    U_AET_BACK      = 0x04,
    U_TEXTURE_BLEND = 0x05,
    U_CHARA_COLOR   = 0x06,
    U_CLIP_PLANE    = 0x07,
    U08             = 0x08,
    U_DEPTH_PEEL    = 0x09,
    U0A             = 0x0A,
    U0B             = 0x0B,
    U_ALPHA_BLEND   = 0x0C,
    U_RIPPLE_EMIT   = 0x0D,
    U_ESM_FILTER    = 0x0E,
    U_EXPOSURE      = 0x0F,
    U_SCENE_FADE    = 0x10,
    U_FADE          = 0x11,
    U12             = 0x12,
    U_FLARE         = 0x13,
    U_FOG_HEIGHT    = 0x14,
    U_FOG           = 0x15,
    U16             = 0x16,
    U_GAUSS         = 0x17,
    U18             = 0x18,
    U_IMAGE_FILTER  = 0x19,
    U_INSTANCE      = 0x1A,
    U_TONE_CURVE    = 0x1B,
    U_LIGHT_PROJ    = 0x1C,
    U_MAGNIFY       = 0x1D,
    U_MEMBRANE      = 0x1E,
    U_MLAA          = 0x1F,
    U20             = 0x20,
    U_MORPH_COLOR   = 0x21,
    U_MORPH         = 0x22,
    U_MOVIE         = 0x23,
    U24             = 0x24,
    U25             = 0x25,
    U26             = 0x26,
    U_NPR           = 0x27,
    U_LIGHT_1       = 0x28,
    U_REFLECT       = 0x29,
    U_REDUCE        = 0x2A,
    U_SELF_SHADOW   = 0x2B,
    U_SHADOW        = 0x2C,
    U2D             = 0x2D,
    U2E             = 0x2E,
    U_SHOW_VECTOR   = 0x2F,
    U_BONE_MAT      = 0x30,
    U_SNOW_PARTICLE = 0x31,
    U_SPECULAR_IBL  = 0x32,
    U_COMBINER      = 0x33,
    U_TEX_0_TYPE    = 0x34,
    U_TEX_1_TYPE    = 0x35,
    U_SSS_FILTER    = 0x36,
    U37             = 0x37,
    U_STAR          = 0x38,
    U_TEXTURE_COUNT = 0x39,
    U_ENV_MAP       = 0x3A,
    U_RIPPLE        = 0x3B,
    U_TRANSLUCENCY  = 0x3C,
    U_NORMAL        = 0x3D,
    U_TRANSPARENCY  = 0x3E,
    U_WATER_REFLECT = 0x3F,
    U40             = 0x40,
    U41             = 0x41,
    U_LIGHT_0       = 0x42,
    U_SPECULAR      = 0x43,
    U_TONE_MAP      = 0x44,
    U45             = 0x45,
    U_MAX           = 0x46,
    U_INVALID       = 0xFFFFFFFF,
};

struct uniform_array {
    int32_t array[70];
};

extern uniform_array* uniform;

shader* shaders = (shader*)0x000000014CC57C10;
shader_table_struct* shader_table = (shader_table_struct*)0x0000000140A41D20;
shader_bind_func* shader_name_bind_func_table = (shader_bind_func*)0x0000000140A40F50;
uniform_array* uniform = (uniform_array*)0x000000014CC57AF0;
int32_t* current_vp = (int32_t*)0x000000014CC587B0;
int32_t* current_fp = (int32_t*)0x000000014CC587B4;

#define GL_VERTEX_PROGRAM_ARB 0x8620
#define GL_FRAGMENT_PROGRAM_ARB 0x8804
typedef void (GLAPIENTRY* PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (GLAPIENTRY* PFNGLPROGRAMLOCALPARAMETER4FARBPROC)
    (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (GLAPIENTRY* PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint* programs);

void FASTCALL shader_set(shader_enum name) {
    if (name == shader_enum::SHADER_FFP) {
        shader_unbind();
        return;
    }

    PFNGLPROGRAMLOCALPARAMETER4FARBPROC _glProgramEnvParameter4fARB
        = *(PFNGLPROGRAMLOCALPARAMETER4FARBPROC*)0x00000001411A4A88;
    _glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 24,
        (float_t)uniform->array[U_TEXTURE_BLEND], 0.0f, 0.0f, 0.0f);

    shader* shad = &shaders[(size_t)name];
    void (FASTCALL * bind_func)(shader*) = (void (*)(shader*))shad->bind_func;
    if (bind_func)
        bind_func(shad);
    else
        shader_bind(shad, shad->subs->sub_name);
}

void FASTCALL shader_bind_blinn(shader* shad) {
    shader_bind(shad, uniform->array[U_NORMAL]
        ? shader_sub_enum::BLINN_FRAG : shader_sub_enum::BLINN_VERT);
}

void FASTCALL shader_bind_cloth(shader* shad) {
    shader_bind(shad, uniform->array[U_NPR] ? shader_sub_enum::CLOTH_NPR1
        : (uniform->array[U_ANISO] ? shader_sub_enum::CLOTH_ANISO : shader_sub_enum::CLOTH_DEFAULT));
}

void FASTCALL shader_bind_hair(shader* shad) {
    shader_bind(shad, uniform->array[U_NPR] ? shader_sub_enum::HAIR_NPR1
        : (uniform->array[U_ANISO] ? shader_sub_enum::HAIR_ANISO : shader_sub_enum::HAIR_DEFAULT));
}

void FASTCALL shader_bind_membrane(shader* shad) {
    uniform->array[U_MEMBRANE] = 3;
    if (shader_bind(shad, shader_sub_enum::MEMBRANE) < 0)
        return;

    uint32_t(FASTCALL * sub_140192E00)() = (uint32_t(*)())0x0000000140192E00;
    uint32_t v1 = sub_140192E00();
    mat4 mat = mat4_identity;
    mat4_mul_rotate_x(&mat, (float_t)((v1 & 0x1FF) * (M_PI / 256.0)), &mat);
    mat4_mul_rotate_z(&mat, (float_t)((v1 % 0x168) * (M_PI / 180.0)), &mat);

    vec4 vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    mat4_transform_vector(&mat, &vec, &vec);

    PFNGLPROGRAMLOCALPARAMETER4FARBPROC _glProgramLocalParameter4fARB
        = *(PFNGLPROGRAMLOCALPARAMETER4FARBPROC*)0x00000001411A4AA8;
    _glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 10, vec.x, vec.y, vec.z, 0.0);
}

void FASTCALL shader_bind_eye_ball(shader* shader) {
    uniform->array[U18] = 0;
    if (shader_bind(&shaders[(size_t)shader_enum::_GLASEYE], shader_sub_enum::GLASS_EYE) >= 0) {
        float_t* glass_eye = (float_t*)0x0000000140CA2D70;
        void (FASTCALL * glass_eye_calc)(float_t * glass_eye) = (void (*)(float_t*))0x00000001405E53D0;
        void (FASTCALL * glass_eye_set)(float_t * glass_eye) = (void (*)(float_t*))0x00000001405E4750;
        glass_eye_calc(glass_eye);
        glass_eye_set(glass_eye);
    }
}

void FASTCALL shader_bind_tone_map(shader* shader) {
    shader_bind(shader, uniform->array[U_NPR] == 1
        ? shader_sub_enum::TONEMAP_NPR1 : shader_sub_enum::TONEMAP);
}

int32_t FASTCALL shader_bind(shader* shader, shader_sub_enum name) {
    int32_t num_sub = shader->num_sub;
    int32_t subshader_index = 0;
    if (num_sub < 1)
        return -1;

    for (shader_sub* i = shader->subs; i->sub_name != name; i++) {
        subshader_index++;
        if (subshader_index >= num_sub)
            return -1;
    }

    shader_sub* subshader = &shader->subs[subshader_index];
    if (!subshader)
        return -1;

    int32_t use_unis_count = shader->num_uniform;
    int32_t unival_vp_curr = 1;
    int32_t unival_fp_curr = 1;
    int32_t unival_vp = 0;
    int32_t unival_fp = 0;
    if (use_unis_count > 0) {
        int32_t* vp_unival_max = subshader->vp_unival_max;
        int32_t* fp_unival_max = subshader->fp_unival_max;
        for (size_t i = 0; i < use_unis_count; i++) {
            int32_t unival = uniform->array[shader->use_uniform[i]];
            unival_vp += unival_vp_curr * min_def(unival, vp_unival_max[i]);
            unival_fp += unival_fp_curr * min_def(unival, fp_unival_max[i]);
            unival_vp_curr *= vp_unival_max[i] + 1;
            unival_fp_curr *= fp_unival_max[i] + 1;
        }
    }

    int32_t vp = subshader->vp[unival_vp];
    int32_t fp = subshader->fp[unival_fp];

    PFNGLBINDPROGRAMARBPROC _glBindProgramARB = *(PFNGLBINDPROGRAMARBPROC*)0x00000001411A4A60;
    if (*current_vp != vp) {
        _glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vp);
        *current_vp = vp;
    }

    if (*current_fp != fp) {
        _glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fp);
        *current_fp = fp;
    }

    _glEnable(GL_VERTEX_PROGRAM_ARB);
    _glEnable(GL_FRAGMENT_PROGRAM_ARB);
    return 0;
}

void FASTCALL shader_unbind() {
    if (current_vp)
        _glDisable(GL_VERTEX_PROGRAM_ARB);
    if (current_fp)
        _glDisable(GL_FRAGMENT_PROGRAM_ARB);
    current_fp = 0;
    current_vp = 0;
}

void FASTCALL shader_load_all_shaders() {
    PFNGLGENPROGRAMSARBPROC _glGenProgramsARB = *(PFNGLGENPROGRAMSARBPROC*)0x00000001411A4A70;
    void (FASTCALL * shader_load_program_file)(GLuint program, const char* file_name, bool vertex_program)
        = (void (*)(GLuint, const char*, bool))0x00000001405E52D0;

    shader_table_struct* shaders_table = &shader_table[(int32_t)shader_enum::_BLINN];
    shader* shader = &shaders[(int32_t)shader_enum::_BLINN];
    int32_t curr_shader = (int32_t)shader_enum::SHADER_END - 1;
    do {
        shader->name = shaders_table->name;
        shader->name_enum = shaders_table->name_enum;
        shader->num_sub = shaders_table->num_sub;
        int32_t num_sub = shaders_table->num_sub;
        void* v9 = force_malloc(sizeof(shader_sub) * num_sub + 8);
        *(int64_t*)v9 = num_sub;

        const shader_sub_table_struct* sub = shaders_table->sub;
        shader_sub* subs = (shader_sub*)((char*)v9 + 8);
        for (int32_t i = 0; i < num_sub; i++)
            subs[i] = { };

        shader->subs = subs;
        shader->num_uniform = shaders_table->num_uniform;
        shader->use_uniform = shaders_table->use_uniform;

        for (int32_t i = 0; i < num_sub; i++, sub++, subs++) {
            subs->sub_name = sub->sub_name;
            subs->vp_unival_max = (int32_t*)sub->vp_unival_max;
            subs->fp_unival_max = (int32_t*)sub->fp_unival_max;
            subs->num_vp = sub->num_vp;
            subs->num_fp = sub->num_fp;
            subs->vp = force_malloc<int32_t>(sub->num_vp);
            subs->fp = force_malloc<int32_t>(sub->num_fp);

            _glGenProgramsARB(sub->num_vp, (GLuint*)subs->vp);
            _glGenProgramsARB(sub->num_fp, (GLuint*)subs->fp);

            for (int32_t j = 0; j < sub->num_vp; j++)
                shader_load_program_file(subs->vp[j], sub->vp[j], true);

            for (int32_t j = 0; j < sub->num_fp; j++)
                shader_load_program_file(subs->fp[j], sub->fp[j], false);
        }
        shader->bind_func = 0;

        shader_bind_func* bind_func_table = shader_name_bind_func_table;
        shader_enum bind_func_name = shader_enum::_BLINN;
        while (shader->name_enum != bind_func_name) {
            bind_func_name = (++bind_func_table)->name;
            if (bind_func_table->name == shader_enum::SHADER_END)
                break;
        }
        shader->bind_func = bind_func_table->bind_func;
        shader++;
        shaders_table++;
        curr_shader--;
    } while (curr_shader);
}
