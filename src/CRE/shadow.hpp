/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/image.hpp"
#include "../KKdLib/time.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "render_texture.hpp"
#include <vector>

class Shadow {
private:
    RenderTexture rtex_table[7];
    RenderTexture* rtex_shadowmap;
    RenderTexture* rtex_soft[2];
    float_t m_view_region;
    float_t m_shadow_range;
    vec3 m_light_pos[2];
    vec3 m_light_target[2];
    vec3 m_proj_dist_base[2];
    float_t m_ground_ypos[2];
    float_t m_proj_dist_base_range[2];
    std::vector<vec3> m_proj_dist_base_vec[2];
    int32_t m_group[2];
    float_t m_shadow_depth_range;
    vec3 m_light_dir;
    vec3 m_avg_light_pos;
    vec3 m_avg_light_target;
    mat4 m_view_mtx;
    mat4 m_iview_mtx;
    uint32_t m_proj_near_repeat;
    ImgfBoxSampl m_proj_near_sampl;
    uint32_t m_proj_far_repeat;
    ImgfBoxSampl m_proj_far_sampl;
    float_t m_proj_dist_attn;
    float_t m_proj_dist_attn_offset;
    float_t m_shadowmap_near_clip;
    float_t m_shadowmap_far_clip;
    float_t m_polyoffset_factor;
    float_t m_polyoffset_unit;
    float_t m_esm_exponent;
    float_t m_gauss_sigma;
    float_t m_gauss_offset;
    float_t m_shadow_ambient;
    bool m_enable_show_textures;
    int32_t m_num_group;
    bool m_enable_group[2];
    bool m_enable_self_shadow;
    bool m_enable[2];
    bool m_separate;

    void init_members();

    static bool check_culling0(const struct obj_bounding_sphere* sphere, const mat4* view);
    static bool check_culling1(const struct obj_bounding_sphere* sphere, const mat4* view);
    static bool check_culling_sub(const struct obj_bounding_sphere* sphere, const mat4* view, const mat4* lview);

protected:
    void set_light_view_proj_mtx(struct cam_data& cam, const vec3* light_pos,
        const vec3* light_target, float_t range, float_t offset, float_t scale);
    void calc_tex_mtx(const vec3* light_pos,
        const vec3* light_target, float_t range, float_t offset, mat4* tex_mtx);
    void calc_dist_base();
    void calc_light_coord();
    void calc_shadowmap_region();
    void calc_shadowmap_region_multi();

public:
    Shadow();
    virtual ~Shadow();

    int32_t create();
    void destroy();
    void set_light_dir(const vec3* dir);
    void enable_group(int32_t group);
    void set_dist_base(int32_t group, const vec3* pos);
    const vec3* get_dist_base(int32_t group) const;
    void set_ground_ypos(int32_t group, float_t ground_ypos);
    void set_view_mtx(const mat4* view_mtx);
    void set_dist_attn(float_t dist_attn);
    float_t get_dist_attn() const;
    void set_dist_attn_offset(float_t dist_attn_offset);
    float_t get_dist_attn_offset() const;
    void set_near_blur_repeat(uint32_t near_blur_repeat);
    uint32_t get_near_blur_repeat() const;
    void set_far_blur_repeat(uint32_t far_blur_repeat);
    uint32_t get_far_blur_repeat() const;
    void set_polyoffset(float_t polyoffset_factor, float_t polyoffset_unit);
    void get_polyoffset(float_t* polyoffset_factor, float_t* polyoffset_unit);
    void set_shadow_ambient(float_t shadow_ambient);
    float_t get_shadow_ambient() const;
    void set_shadow_range(float_t shadow_range);
    float_t get_shadow_range() const;
    void begin_make_shadow_textures(int32_t group[2]);
    void end_make_shadow_textures(struct p_gl_rend_state& p_gl_rend_st);
    void begin_make_silhouette_map(struct render_data_context& rend_data_ctx, struct cam_data& cam, int32_t group, int32_t index);
    void end_make_silhouette_map(struct render_data_context& rend_data_ctx, int32_t group, int32_t index);
    void bind_shadow(struct render_data_context& rend_data_ctx, struct render_context* rctx);
    void unbind_shadow(struct render_data_context& rend_data_ctx, struct render_context* rctx);
    void clear(struct p_gl_rend_state& p_gl_rend_st);
    void reset_group();
    void set_enable(int32_t group, bool enable);
    void reset_enable();
    void enable_show_textures();
    void disable_show_textures();
    bool is_enable_show_textures() const;
    void dbg_show_textures();
    void set_cull_func(int32_t group);
    void set_self_shadow_sw(bool sw);
    bool get_self_shadow_sw() const;

    void calc_shadowmap();
    float_t get_distance() const;
    RenderTexture& get_rtex(int32_t index);
};

extern void finish_shadow();
extern Shadow* get_shadow();
extern void init_shadow();

// Inlined
inline void Shadow::set_ground_ypos(int32_t group, float_t ground_ypos) {
    m_ground_ypos[group] = ground_ypos;
}

// Inlined
inline float_t Shadow::get_dist_attn() const {
    return m_proj_dist_attn;
}

// Inlined
inline void Shadow::set_dist_attn_offset(float_t dist_attn_offset) {
    m_proj_dist_attn_offset = dist_attn_offset;
}

// Inlined
inline float_t Shadow::get_dist_attn_offset() const {
    return m_proj_dist_attn_offset;
}

// Inlined
inline void Shadow::set_near_blur_repeat(uint32_t near_blur_repeat) {
    m_proj_near_repeat = near_blur_repeat;
}

// Inlined
inline uint32_t Shadow::get_near_blur_repeat() const {
    return m_proj_near_repeat;
}

// Inlined
inline void Shadow::set_far_blur_repeat(uint32_t far_blur_repeat) {
    m_proj_far_repeat = far_blur_repeat;
}

// Inlined
inline uint32_t Shadow::get_far_blur_repeat() const {
    return m_proj_far_repeat;
}

// Inlined
inline void Shadow::set_shadow_ambient(float_t shadow_ambient) {
    m_shadow_ambient = shadow_ambient;
}

// Inlined
inline float_t Shadow::get_shadow_ambient() const {
    return m_shadow_ambient;
}

// Inlined
inline void Shadow::set_shadow_range(float_t shadow_range) {
    m_shadow_range = shadow_range;
}

// Inlined
inline float_t Shadow::get_shadow_range() const {
    return m_shadow_range;
}

// Inlined
inline void Shadow::set_enable(int32_t group, bool enable) {
    if (group >= 0 && group < 2)
        m_enable[group] = enable;
}

// Inlined
inline void Shadow::reset_enable() {
    for (int32_t i = 0; i < 2; i++)
        m_enable[i] = true;
}

// Inlined
inline void Shadow::enable_show_textures() {
    m_enable_show_textures = true;
}

// Inlined
inline void Shadow::disable_show_textures() {
    m_enable_show_textures = false;
}

// Inlined
inline bool Shadow::is_enable_show_textures() const {
    return m_enable_show_textures;
}

// Inlined
inline void Shadow::set_self_shadow_sw(bool sw) {
    m_enable_self_shadow = sw;
}

// Inlined
inline bool Shadow::get_self_shadow_sw() const {
    return m_enable_self_shadow;
}

// Inlined
inline RenderTexture& Shadow::get_rtex(int32_t index) {
    return rtex_table[index];
}
