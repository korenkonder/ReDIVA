/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_pass.h"
#include "post_process.h"
#include "render_texture.h"
#include "texture.h"
#include "time.h"
#include "static_var.h"

typedef enum reflect_blur_filter_mode {
    REFLECT_BLUR_FILTER_4  = 0x00,
    REFLECT_BLUR_FILTER_9  = 0x01,
    REFLECT_BLUR_FILTER_16 = 0x02,
    REFLECT_BLUR_FILTER_32 = 0x03,
} reflect_blur_filter_mode;

typedef enum draw_pass_type {
    DRAW_PASS_SHADOW       = 0x00,
    DRAW_PASS_SSS_SKIN     = 0x01,
    DRAW_PASS_TYPE_2       = 0x02,
    DRAW_PASS_REFLECT      = 0x03,
    DRAW_PASS_REFRACT      = 0x04,
    DRAW_PASS_PREPROCESS   = 0x05,
    DRAW_PASS_TYPE_6       = 0x06,
    DRAW_PASS_TYPE_7       = 0x07,
    DRAW_PASS_3D           = 0x08,
    DRAW_PASS_SHOW_VECTOR  = 0x09,
    DRAW_PASS_POST_PROCESS = 0x0A,
    DRAW_PASS_SPRITE       = 0x0B,
    DRAW_PASS_TYPE_12      = 0x0C,
    DRAW_PASS_MAX          = 0x0D,
} draw_pass_type;

typedef struct draw_preprocess {
    int32_t field_0;
    void(__fastcall* draw_func)(void*);
    void* data;
} draw_preprocess;

typedef struct list_draw_preprocess_node list_draw_preprocess_node;

struct list_draw_preprocess_node{
    list_draw_preprocess_node* next;
    list_draw_preprocess_node* prev;
    draw_preprocess value;
};

typedef struct list_draw_preprocess {
    list_draw_preprocess_node* head;
    size_t size;
} list_draw_preprocess;


typedef struct draw_pass {
    bool enable[DRAW_PASS_MAX];
    //shadow* shadow_ptr;
    bool reflect;
    bool refract;
    int32_t reflect_blur_num;
    reflect_blur_filter_mode reflect_blur_filter;
    //post_process_struct* post_process;
    char wait_for_gpu;
    double_t cpu_time[13];
    double_t gpu_time[13];
    time_struct time;
    char shadow;
    bool opaque_z_sort;
    bool alpha_z_sort;
    bool draw_pass_3d[3];
    char field_11F;
    char field_120;
    char field_121;
    int32_t reflect_type;
    char field_128;
    int tex_index[11];
    render_texture field_158[8];
    GLsizei width;
    GLsizei height;
    GLuint field_2E0;
    GLuint field_2E4;
    char field_2E8;
    char field_2E9;
    int field_2EC;
    float_t field_2F0;
    float_t field_2F4;
    char field_2F8;
    //list_draw_preprocess preprocess;
    texture* field_310;
    int field_318;
    bool field_31C;
    bool field_31D;
    bool reflect_object;
    bool field_31F;
    bool field_320;
    bool field_321;
} draw_pass;

draw_pass draw_pass_data;

static void draw_pass_begin(draw_pass* a1);
static void draw_pass_end(draw_pass* pass, draw_pass_type type);

/*void draw_pass_init()
{
    memset(draw_pass_data.cpu_time, 0, sizeof(draw_pass_data.cpu_time));
    memset(draw_pass_data.gpu_time, 0, sizeof(draw_pass_data.gpu_time));
    time_struct_init(&draw_pass_data.time);
    `eh vector constructor iterator'(
        draw_pass_data.field_158,
        48i64,
        8,
        (void(__fastcall*)(void*))render_texture_init);
        draw_pass_data.field_300 = 0i64;
        draw_pass_data.field_308 = 0i64;
        draw_pass_data.field_300 = sub_140500860(&draw_pass_data.field_300, 0i64, 0i64);
        atexit(draw_pass_free);
}*/

void draw_pass_main() {
    for (draw_pass_type i = DRAW_PASS_SHADOW; i <= DRAW_PASS_TYPE_12; i++) {
        draw_pass_data.cpu_time[i] = 0.0;
        draw_pass_data.gpu_time[i] = 0.0;
        if (!draw_pass_data.enable[i]) {
            glGetError();
            continue;
        }

        draw_pass_begin(&draw_pass_data);
        switch (i) {
        case DRAW_PASS_SHADOW:
            //draw_pass_shadow(&draw_pass_data);
            break;
        case DRAW_PASS_SSS_SKIN:
            //draw_pass_sss_skin(&draw_pass_data);
            break;
        case DRAW_PASS_REFLECT:
            //draw_pass_reflect(&draw_pass_data);
            break;
        case DRAW_PASS_REFRACT:
            //draw_pass_refract(&draw_pass_data);
            break;
        case DRAW_PASS_PREPROCESS:
            //draw_pass_preprocess();
            break;
        case DRAW_PASS_TYPE_6:
            //sub_140501470(&draw_pass_data);
            break;
        case DRAW_PASS_TYPE_7:
            //sub_140501600(&draw_pass_data);
            break;
        case DRAW_PASS_3D:
            //draw_pass_3d(&draw_pass_data);
            break;
        case DRAW_PASS_SHOW_VECTOR:
            //draw_pass_show_vector(&draw_pass_data);
            break;
        case DRAW_PASS_POST_PROCESS:
            //draw_pass_post_process(&draw_pass_data);
            break;
        case DRAW_PASS_SPRITE:
            //draw_pass_sprite(&draw_pass_data);
            break;
        }
        draw_pass_end(&draw_pass_data, i);
        glGetError();
    }
}

/*void draw_pass_free()
{
    list_struc_125* v0; // rdi
    list_struc_125* v1; // rsi
    list_struc_125* v2; // rbx
    char v3; // [rsp+40h] [rbp+8h] BYREF
    char v4; // [rsp+48h] [rbp+10h] BYREF
    char v5; // [rsp+50h] [rbp+18h] BYREF
    char v6; // [rsp+58h] [rbp+20h] BYREF

    v0 = draw_pass_data.field_300->next;
    draw_pass_data.field_300->next = draw_pass_data.field_300;
    draw_pass_data.field_300->prev = draw_pass_data.field_300;
    v1 = draw_pass_data.field_300;
    draw_pass_data.field_308 = 0i64;
    if (v0 != draw_pass_data.field_300)
    {
        do
        {
            v2 = v0->next;
            sub_1405005A0(&v3);
            nullsub_151(&v3, v0);
            sub_1405005A0(&v4);
            nullsub_151(&v4, &v0->prev);
            sub_1405005A0(&v5);
            nullsub_152(&v5, &v0->value);
            sub_1405005A0(&v6);
            j_free_2(v0);
            v1 = draw_pass_data.field_300;
            v0 = v2;
        } while (v2 != draw_pass_data.field_300);
    }
    sub_1405005A0(&v3);
    nullsub_151(&v3, v1);
    sub_1405005A0(&v3);
    nullsub_151(&v3, &v1->prev);
    sub_1405005A0(&v3);
    j_free_2(v1);
    `eh vector destructor iterator'(
        draw_pass_data.field_158,
        48i64,
        8,
        (void(__fastcall*)(void*))render_texture_free_0);
}*/

inline static void draw_pass_begin(draw_pass* a1) {
    if (a1->wait_for_gpu)
        glFinish();
    time_struct_get_timestamp(&a1->time);
}

inline static void draw_pass_end(draw_pass* pass, draw_pass_type type) {
    pass->cpu_time[type] = time_struct_calc_time(&pass->time);
    if (pass->wait_for_gpu) {
        time_struct t;
        time_struct_get_timestamp(&t);
        glFinish();
        pass->gpu_time[type] = time_struct_calc_time(&t);
    }
    else
        pass->gpu_time[type] = 0;
}