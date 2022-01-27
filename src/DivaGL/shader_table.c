/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_table.h"
#include "gl.h"
#include "../KKdLib/mat.h"

typedef struct uniform_array {
    int32_t array[70];
} uniform_array;

extern uniform_array* uniform;

aft_shader* shaders = (void*)0x000000014CC57C10;
aft_shader_table_struct* aft_shader_table = (void*)0x0000000140A41D20;
aft_shader_bind_func* shader_name_bind_func_table = (void*)0x0000000140A40F50;
uniform_array* uniform = (void*)0x000000014CC57AF0;
int32_t* current_vp = (void*)0x000000014CC587B0;
int32_t* current_fp = (void*)0x000000014CC587B4;

#define GL_VERTEX_PROGRAM_ARB 0x8620
#define GL_FRAGMENT_PROGRAM_ARB 0x8804
typedef void (GLAPIENTRY* PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (GLAPIENTRY* PFNGLPROGRAMLOCALPARAMETER4FARBPROC)
    (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (GLAPIENTRY* PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint* programs);

void FASTCALL aft_shader_set(aft_shader_enum name) {
    if (!name) {
        aft_shader_unbind();
        return;
    }

    PFNGLPROGRAMLOCALPARAMETER4FARBPROC _glProgramEnvParameter4fARB = *(void**)0x00000001411A4A88;
    _glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 24,
        (float_t)uniform->array[U_TEXTURE_BLEND], 0.0f, 0.0f, 0.0f);

    aft_shader* shad = &shaders[name];
    void (FASTCALL * bind_func)(aft_shader*) = shad->bind_func;
    if (bind_func)
        bind_func(shad);
    else
        aft_shader_bind(shad, shad->subs->sub_name);
}

void FASTCALL aft_shader_bind_blinn(aft_shader* shad) {
    aft_shader_bind(shad, uniform->array[U_NORMAL] ? BLINN_FRAG : BLINN_VERT);
}

void FASTCALL aft_shader_bind_cloth(aft_shader* shad) {
    aft_shader_bind(shad, uniform->array[U_NPR] ? CLOTH_NPR1
        : (uniform->array[U_ANISO] ? CLOTH_ANISO : CLOTH_DEFAULT));
}

void FASTCALL aft_shader_bind_hair(aft_shader* shad) {
    aft_shader_bind(shad, uniform->array[U_NPR] ? HAIR_NPR1
        : (uniform->array[U_ANISO] ? HAIR_ANISO : HAIR_DEFAULT));
}

void FASTCALL aft_shader_bind_membrane(aft_shader* shad) {
    uniform->array[U_MEMBRANE] = 3;
    if (aft_shader_bind(shad, MEMBRANE) < 0)
        return;

    uint32_t(FASTCALL * sub_140192E00)() = (void*)0x0000000140192E00;
    uint32_t v1 = sub_140192E00();
    mat4 mat = mat4_identity;
    mat4_rotate_x_mult(&mat, (float_t)((v1 & 0x1FF) * (M_PI / 256.0)), &mat);
    mat4_rotate_z_mult(&mat, (float_t)((v1 % 0x168) * (M_PI / 180.0)), &mat);

    vec4 vec = (vec4){ 1.0f, 0.0f, 0.0f, 0.0f };
    mat4_mult_vec(&mat, &vec, &vec);

    PFNGLPROGRAMLOCALPARAMETER4FARBPROC _glProgramLocalParameter4fARB = *(void**)0x00000001411A4AA8;
    _glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 10, vec.x, vec.y, vec.z, 0.0);
}

void FASTCALL aft_shader_bind_eye_ball(aft_shader* shader) {
    uniform->array[U18] = 0;
    if (aft_shader_bind(&shaders[_GLASEYE], GLASS_EYE) >= 0) {
        float_t* glass_eye = (float_t*)0x0000000140CA2D70;
        void (FASTCALL * glass_eye_calc)(float_t * glass_eye) = (void*)0x00000001405E53D0;
        void (FASTCALL * glass_eye_set)(float_t * glass_eye) = (void*)0x00000001405E4750;
        glass_eye_calc(glass_eye);
        glass_eye_set(glass_eye);
    }
}

void FASTCALL aft_shader_bind_tone_map(aft_shader* shader) {
    aft_shader_bind(shader, uniform->array[U_NPR] == 1 ? TONEMAP_NPR1 : TONEMAP);
}

int32_t FASTCALL aft_shader_bind(aft_shader* shader, aft_shader_sub_enum name) {
    int32_t num_sub = shader->num_sub;
    int32_t subshader_index = 0;
    if (num_sub < 1)
        return -1;

    for (aft_shader_sub* i = shader->subs; i->sub_name != name; i++) {
        subshader_index++;
        if (subshader_index >= num_sub)
            return -1;
    }

    aft_shader_sub* subshader = &shader->subs[subshader_index];
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
            unival_vp += unival_vp_curr * min(unival, vp_unival_max[i]);
            unival_fp += unival_fp_curr * min(unival, fp_unival_max[i]);
            unival_vp_curr *= vp_unival_max[i] + 1;
            unival_fp_curr *= fp_unival_max[i] + 1;
        }
    }

    int32_t vp = subshader->vp[unival_vp];
    int32_t fp = subshader->fp[unival_fp];

    PFNGLBINDPROGRAMARBPROC _glBindProgramARB = *(void**)0x00000001411A4A60;
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

void FASTCALL aft_shader_unbind() {
    if (current_vp)
        _glDisable(GL_VERTEX_PROGRAM_ARB);
    if (current_fp)
        _glDisable(GL_FRAGMENT_PROGRAM_ARB);
    current_fp = 0;
    current_vp = 0;
}

void FASTCALL aft_shader_load_all_shaders() {
    PFNGLGENPROGRAMSARBPROC _glGenProgramsARB = *(void**)0x00000001411A4A70;
    void (FASTCALL * shader_load_program_file)(GLuint program, const char* file_name, bool vertex_program)
        = (void*)0x00000001405E52D0;

    aft_shader_table_struct* shaders_table = &aft_shader_table[_BLINN];
    aft_shader* shader = &shaders[_BLINN];
    aft_shader_enum curr_shader = SHADER_END - 1;
    do {
        shader->name = shaders_table->name;
        shader->name_enum = shaders_table->name_enum;
        shader->num_sub = shaders_table->num_sub;
        int32_t num_sub = shaders_table->num_sub;
        void* v9 = force_malloc(sizeof(aft_shader_sub) * num_sub + 8);
        *(int64_t*)v9 = num_sub;

        const aft_shader_sub_table_struct* sub = shaders_table->sub;
        aft_shader_sub* subs = (aft_shader_sub*)((char*)v9 + 8);
        for (int32_t i = 0; i < num_sub; i++)
            subs[i] = (aft_shader_sub){ 0 };

        shader->subs = subs;
        shader->num_uniform = shaders_table->num_uniform;
        shader->use_uniform = shaders_table->use_uniform;

        for (int32_t i = 0; i < num_sub; i++, sub++, subs++) {
            subs->sub_name = sub->sub_name;
            subs->vp_unival_max = (int32_t*)sub->vp_unival_max;
            subs->fp_unival_max = (int32_t*)sub->fp_unival_max;
            subs->num_vp = sub->num_vp;
            subs->num_fp = sub->num_fp;
            subs->vp = force_malloc(sizeof(int32_t) * sub->num_vp);
            subs->fp = force_malloc(sizeof(int32_t) * sub->num_fp);

            _glGenProgramsARB(sub->num_vp, subs->vp);
            _glGenProgramsARB(sub->num_fp, subs->fp);

            for (int32_t j = 0; j < sub->num_vp; j++)
                shader_load_program_file(subs->vp[j], sub->vp[j], true);

            for (int32_t j = 0; j < sub->num_fp; j++)
                shader_load_program_file(subs->fp[j], sub->fp[j], false);
        }
        shader->bind_func = 0;

        aft_shader_bind_func* bind_func_table = shader_name_bind_func_table;
        aft_shader_enum bind_func_name = _BLINN;
        while (shader->name_enum != bind_func_name) {
            bind_func_name = (++bind_func_table)->name;
            if (bind_func_table->name == SHADER_END)
                break;
        }
        shader->bind_func = bind_func_table->bind_func;
        shader++;
        shaders_table++;
        curr_shader--;
    } while (curr_shader);
}
