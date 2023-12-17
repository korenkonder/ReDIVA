/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../GL/uniform_buffer.hpp"
#include "../camera.hpp"
#include "../render_texture.hpp"
#include "fbo.hpp"

enum dof_debug_flags {
    DOF_DEBUG_USE_UI_PARAMS   = 0x01,
    DOF_DEBUG_ENABLE_DOF      = 0x02,
    DOF_DEBUG_ENABLE_PHYS_DOF = 0x04,
    DOF_DEBUG_AUTO_FOCUS      = 0x08,
};

struct dof_f2 {
    float_t focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
};

struct dof_debug {
    dof_debug_flags flags;
    float_t focus;
    float_t focal_length;
    float_t f_number;
    dof_f2 f2;
};

struct dof_pv {
    bool enable;
    dof_f2 f2;
};

namespace renderer {
    struct DOF3 {
    private:
        int32_t width;
        int32_t height;
        GLuint textures[6];
        FBO fbo[4];
        GLuint samplers[2];
        GLuint vao;
        GL::UniformBuffer common_ubo;
        GL::UniformBuffer texcoords_ubo;

    public:
        DOF3(int32_t width, int32_t height);
        ~DOF3();

        void apply(RenderTexture* rt);
        void resize(int32_t width, int32_t height);

    private:
        void free();
        void init(int32_t width, int32_t height);

        void apply_f2(RenderTexture* rt, GLuint color_texture,
            GLuint depth_texture, float_t min_distance, float_t max_distance, float_t fov,
            float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio);
        void apply_physical(RenderTexture* rt, GLuint color_texture,
            GLuint depth_texture, float_t min_distance, float_t max_distance,
            float_t focus, float_t focal_length, float_t fov, float_t f_number);

        void render_tiles(GLuint depth_texture, bool f2);
        void downsample(GLuint color_texture, GLuint depth_texture, bool f2);
        void apply_main_filter(bool f2);
        void upsample(RenderTexture* rt, GLuint color_texture, GLuint depth_texture, bool f2);

        void update_data(float_t min_dist, float_t max_dist, float_t fov, float_t dist, float_t focal_length,
            float_t f_number, float_t focus_range, float_t fuzzing_range, float_t ratio);

        static void calculate_texcoords(vec2* data, float_t size);
    };
}

extern dof_debug dof_debug_data;
extern dof_pv dof_pv_data;

extern void dof_debug_get(dof_debug* debug);
extern void dof_debug_set(dof_debug* debug = 0);
extern void dof_pv_get(dof_pv* pv);
extern void dof_pv_set(dof_pv* pv = 0);
