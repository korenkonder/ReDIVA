/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sprite.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/prj/stack_allocator.hpp"
#include "../KKdLib/txp.hpp"
#include "GL/array_buffer.hpp"
#include "GL/element_array_buffer.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "shader_ft.hpp"
#include "texture.hpp"
#include <glad/glad.h>

class SprSet {
public:
    int32_t flag;
    uint32_t index;
    ::spr_set* spr_set;
    texture** textures;
    int32_t load_count;
    p_file_handler file_handler;
    bool ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    std::string file;

    std::string name;
    std::vector<uint32_t> sprite_ids;
    uint32_t hash;

    SprSet(uint32_t index);
    virtual ~SprSet();

    const char* GetName(spr_info info);
    rectangle GetRectangle(spr_info info);
    resolution_mode GetResolutionMode(spr_info info);
    texture* GetTexture(spr_info info);
    bool LoadData();
    bool LoadDataModern(const void* data, size_t size);
    bool LoadFile();
    bool LoadFileModern(sprite_database* spr_db);
    bool LoadTexture();
    bool LoadTextureModern(const void* data, size_t size);
    void ReadFile(const char* file, std::string& mdata_dir, void* data);
    void ReadFileModern(uint32_t set_hash, void* data);
    void Unload();
    void UnloadModern(sprite_database* spr_db);
    bool UnloadTexture();
};

struct sprite_draw_vertex {
    vec3 pos;
    vec2 uv[2];
    color4u8 color;
};

struct sprite_draw_param {
    bool blend;
    GLenum blend_src_rgb;
    GLenum blend_src_alpha;
    GLenum blend_dst_rgb;
    GLenum blend_dst_alpha;

    bool copy_texture;
    GLint copy_texture_x;
    GLint copy_texture_y;
    GLint copy_texture_width;
    GLint copy_texture_height;

    int32_t shader;
    int32_t tex_0_type;
    int32_t tex_1_type;
    int32_t combiner;

    GLuint texture[2];
    GLuint sampler;

    GLenum mode;
    union {
        GLint first;
        struct {
            GLuint start;
            GLuint end;
        };
    };
    GLsizei count;
    GLintptr offset;
};

namespace spr {
    struct TexCoord {
        struct UV {
            float_t u;
            float_t v;

            inline UV() : u(), v() {

            }
        };

        UV uv[4];

        inline TexCoord() {

        }
    };

    struct TexParam {
        texture* texture;
        TexCoord texcoord;
        //int32_t pad[2];

        inline TexParam() : texture()/*, pad()*/ {

        }
    };

    struct SpriteManager {
        struct RenderData {
            std::vector<sprite_draw_param> draw_param_buffer;
            std::vector<sprite_draw_vertex> vertex_buffer;
            std::vector<uint32_t> index_buffer;
            GLuint vao;
            GL::ArrayBuffer vbo;
            size_t vbo_vertex_count;
            GL::ElementArrayBuffer ebo;
            size_t ebo_index_count;

            RenderData();
            ~RenderData();

            void Clear();
            void Update();
        };

        std::map<uint32_t, SprSet> sets;
        std::list<SprArgs> reqlist[4][2][SPR_PRIO_MAX];
        float_t aspect[2];
        std::pair<resolution_mode, rectangle> field_1018[2];
        int32_t index;
        mat4 view_projection;
        mat4 mat;
        resolution_mode resolution_mode;

        uint32_t set_counter;

        RenderData render_data;

        SpriteManager();
        ~SpriteManager();

        uint32_t AddSetModern();
        void AddSprSets(const sprite_database* spr_db);
        void Clear();
        void Draw(int32_t index, bool font, texture* tex, const mat4& vp);
        SprSet* GetSet(uint32_t index);
        bool GetSetReady(uint32_t index);
        uint32_t GetSetSpriteNum(uint32_t index);
        uint32_t GetSetTextureNum(uint32_t index);
        const char* GetSprName(spr_info info);
        rectangle GetSprRectangle(spr_info info);
        texture* GetSprTexture(spr_info info);
        bool LoadFile(uint32_t index);
        bool LoadFileModern(uint32_t index, sprite_database* spr_db);
        SprArgs* PutSprite(const SprArgs& args, const sprite_database* spr_db);
        void ReadFile(uint32_t index, const char* file, std::string& mdata_dir, void* data);
        void ReadFileModern(uint32_t index, uint32_t set_hash, void* data, sprite_database* spr_db);
        void RemoveSprSets(const sprite_database* spr_db);
        void ResetIndex();
        void ResetReqList();
        void ResetResData();
        void UnloadSet(uint32_t index);
        void UnloadSetModern(uint32_t index, sprite_database* spr_db);
    };

    static void calc_sprite_vertex(spr::SprArgs* args, vec3* vtx, mat4* mat, bool font);
    static int32_t calc_sprite_texture_param(SprArgs* args, spr::TexParam* param, vec3* vtx, bool font);

    static void draw_sprite(render_context* rctx, SprArgs& args, bool font,
        const mat4& mat, int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max,
        std::vector<sprite_draw_param>& draw_param_buffer,
        std::vector<sprite_draw_vertex>& vertex_buffer, std::vector<uint32_t>& index_buffer);
    static void draw_sprite_begin(render_context* rctx);
    static void draw_sprite_end();
    static void draw_sprite_scale(spr::SprArgs* args);
}

spr::SpriteManager* sprite_manager;

#if BREAK_SPRITE_VERTEX_LIMIT
size_t sprite_vertex_array_max_count = 0x2000;

spr::SpriteVertex* sprite_vertex_array;
#else
const size_t sprite_vertex_array_max_count = 0x2000;

spr::SpriteVertex sprite_vertex_array[sprite_vertex_array_max_count];
#endif
size_t sprite_vertex_array_count;

extern render_context* rctx_ptr;

static const GLenum spr_blend_param[6][4] = {
    { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE  },
    { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE  },
    { GL_SRC_ALPHA, GL_ONE,                 GL_ZERO, GL_ONE  },
    { GL_DST_COLOR, GL_ZERO,                GL_ZERO, GL_ONE  },
    { GL_ONE,       GL_ZERO,                GL_ONE,  GL_ZERO },
    { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE  },
};

namespace spr {
    SprArgs::SprArgs() : kind(), attr(), blend(), index(), layer(),
        prio(), resolution_mode_screen(), resolution_mode_sprite(), texture(),
        shader(), vertex_array(), num_vertex(), flags(), field_CC(), next() {
        Reset();
    }

    void SprArgs::Reset() {
        kind = SPR_KIND_NORMAL;
        id = {};
        color = 0xFFFFFFFF;
        attr = (SprAttr)0;
        blend = 0;
        index = -1;
        layer = 0;
        prio = SPR_PRIO_DEFAULT;
        resolution_mode_screen = RESOLUTION_MODE_HD;
        resolution_mode_sprite = RESOLUTION_MODE_HD;
        center = 0.0f;
        trans = 0.0f;
        scale = 1.0f;
        rot = 0.0f;
        skew_angle = 0.0f;
        mat = mat4_identity;
        texture = 0;
        shader = SHADER_FT_FFP;
#if BREAK_SPRITE_VERTEX_LIMIT
        vertex_array = -1;
#else
        vertex_array = 0;
#endif
        num_vertex = 0;
        flags = (SprArgs::Flags)0;
        sprite_size = 0.0f;
        field_CC = 0;
        texture_pos = 0.0f;
        texture_size = 1.0f;
        next = 0;
    }

    void SprArgs::SetSpriteSize(vec2 size) {
        if (flags & SPRITE_SIZE)
            return;

        enum_or(flags, SPRITE_SIZE);
        sprite_size = size;
        field_CC = 0;
    }

    void SprArgs::SetTexturePosSize(float_t x, float_t y, float_t width, float_t height) {
        if (flags & TEXTURE_POS_SIZE)
            return;

        enum_or(flags, TEXTURE_POS_SIZE);
        texture_pos.x = x;
        texture_pos.y = y;
        texture_size.x = width;
        texture_size.y = height;
    }

    void SprArgs::SetVertexArray(SpriteVertex* vertex_array, size_t num_vertex) {
#if BREAK_SPRITE_VERTEX_LIMIT
        if (sprite_vertex_array_count + num_vertex >= sprite_vertex_array_max_count) {
            while (sprite_vertex_array_count + num_vertex >= sprite_vertex_array_max_count)
                sprite_vertex_array_max_count *= 2;

            spr::SpriteVertex* _sprite_vertex_array = new spr::SpriteVertex[sprite_vertex_array_max_count * 2];
            memmove(_sprite_vertex_array, sprite_vertex_array, sizeof(SpriteVertex) * sprite_vertex_array_count);
            delete[] sprite_vertex_array;
            sprite_vertex_array = _sprite_vertex_array;
        }
#else
        if (sprite_vertex_array_count + num_vertex >= sprite_vertex_array_max_count)
            return;
#endif

        this->num_vertex = num_vertex;
#if BREAK_SPRITE_VERTEX_LIMIT
        this->vertex_array = sprite_vertex_array_count;
        memmove(sprite_vertex_array + this->vertex_array, vertex_array, sizeof(SpriteVertex) * num_vertex);
#else
        this->vertex_array = &sprite_vertex_array[sprite_vertex_array_count];
        memmove(this->vertex_array, vertex_array, sizeof(SpriteVertex) * num_vertex);
#endif
        sprite_vertex_array_count += num_vertex;
    }

    void SprArgs::SetNext(SprArgs* args, SprArgs* next) {
        while (args->next)
            args = args->next;
        args->next = next;
        next->kind = SPR_KIND_LINE;
    }

    vec2 proj_sprite_3d_line(vec3 vec, bool offset) {
        camera* cam = rctx_ptr->camera;

        mat4_transform_point(&cam->view, &vec, &vec);
        if (fabsf(vec.z) < 1.0e-10f)
            return 0.0f;

        vec2 sc_vec = cam->depth * *(vec2*)&vec.x * (1.0f / vec.z);

        resolution_struct* res_wind_int = res_window_internal_get();
        sc_vec.x = (float_t)res_wind_int->width * 0.5f - sc_vec.x;
        sc_vec.y = (float_t)res_wind_int->height * 0.5f + sc_vec.y;
        if (offset) {
            resolution_struct* res_wind = res_window_get();
            sc_vec.x = (float_t)res_wind_int->x_offset + sc_vec.x;
            sc_vec.y = (float_t)(res_wind->height
                - res_wind_int->y_offset - res_wind_int->height) + sc_vec.y;
        }
        return sc_vec;
    }

    void put_cross(const mat4& mat, color4u8 color_x, color4u8 color_y, color4u8 color_z) {
        vec3 v5 = { 0.0f, 0.0f, 0.0f };
        vec3 v6;
        mat4_transform_point(&mat, &v5, &v6);

        vec3 v7 = v6;

        v5 = { 1.0f, 0.0f, 0.0f };
        mat4_transform_point(&mat, &v5, &v6);
        put_sprite_3d_line(v7, v6, color_x);

        v5 = { 0.0f, 1.0f, 0.0f };
        mat4_transform_point(&mat, &v5, &v6);
        put_sprite_3d_line(v7, v6, color_y);

        v5 = { 0.0f, 0.0f, 1.0f };
        mat4_transform_point(&mat, &v5, &v6);
        put_sprite_3d_line(v7, v6, color_z);
    }

    void put_rgb_cross(const mat4& mat) {
        spr::put_cross(mat, color_red, color_green, color_blue);
    }

    spr::SprArgs* put_sprite(const spr::SprArgs& args, const sprite_database* spr_db) {
        return sprite_manager->PutSprite(args, spr_db);
    }

    void put_sprite_3d_line(vec3 p1, vec3 p2, color4u8 color) {
        vec2 sc_p1 = proj_sprite_3d_line(p1, true);
        vec2 sc_p2 = proj_sprite_3d_line(p2, true);
        spr::put_sprite_line(sc_p1, sc_p2, RESOLUTION_MODE_MAX, SPR_PRIO_DEBUG, color, 0);
    }

    void put_sprite_line(vec2 p1, vec2 p2, resolution_mode mode, spr::SprPrio prio, color4u8 color, int32_t layer) {
        spr::SprArgs args;
        args.trans.x = p1.x;
        args.trans.y = p1.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_LINES;
        args.resolution_mode_screen = mode;
        args.resolution_mode_sprite = mode;
        args.prio = prio;
        args.color = color;
        args.SetSpriteSize({ p2.x - p1.x, p2.y - p1.y });
        enum_or(args.attr, SPR_ATTR_MATRIX);
        sprite_manager->PutSprite(args, 0);
    }

    void put_sprite_line_list(vec2* points, size_t count, resolution_mode mode,
        spr::SprPrio prio, color4u8 color, int32_t layer) {
        spr::SprArgs args;
        args.kind = SPR_KIND_ARROW_AB;
        args.layer = layer;
        args.resolution_mode_screen = mode;
        args.resolution_mode_sprite = mode;
        args.prio = prio;
        args.color = color;

        std::vector<spr::SpriteVertex> vertex_array;
        vertex_array.reserve(count);

        for (size_t i = 0; i < count; i++, points++) {
            spr::SpriteVertex vert = {};
            *(vec2*)&vert.pos = *points;
            vertex_array.push_back(vert);
        }

        args.SetVertexArray(vertex_array.data(), vertex_array.size());
        sprite_manager->PutSprite(args, 0);
    }

    void put_sprite_multi(rectangle rect, resolution_mode mode, spr::SprPrio prio, color4u8 color, int32_t layer) {
        spr::SprArgs args;
        args.trans.x = rect.pos.x;
        args.trans.y = rect.pos.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_MULTI;
        args.resolution_mode_screen = mode;
        args.resolution_mode_sprite = mode;
        args.prio = prio;
        args.color = color;
        args.SetSpriteSize(rect.size);
        enum_or(args.attr, SPR_ATTR_MATRIX);
        sprite_manager->PutSprite(args, 0);
    }

    void put_sprite_rect(rectangle rect, resolution_mode mode, spr::SprPrio prio, color4u8 color, int32_t layer) {
        spr::SprArgs args;
        args.trans.x = rect.pos.x;
        args.trans.y = rect.pos.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_RECT;
        args.resolution_mode_screen = mode;
        args.resolution_mode_sprite = mode;
        args.prio = prio;
        args.color = color;
        args.SetSpriteSize(rect.size);
        enum_or(args.attr, SPR_ATTR_MATRIX);
        sprite_manager->PutSprite(args, 0);
    }

    void put_sprite_triangles(SpriteVertex* vert, size_t num, resolution_mode mode,
        SprPrio prio, int32_t spr_id, int32_t layer, const sprite_database* spr_db) {
        spr::SprArgs args;
        args.layer = layer;
        args.kind = SPR_KIND_TRIANGLE;
        args.resolution_mode_screen = mode;
        args.resolution_mode_sprite = mode;
        args.prio = prio;
        args.color = vert[0].color;
        if (spr_id != -1)
            args.id.info = spr_db->get_spr_by_id(spr_id)->info;
        enum_or(args.attr, SPR_ATTR_MATRIX);
        args.SetVertexArray(vert, num);
        sprite_manager->PutSprite(args, spr_db);
    }
}

void sprite_manager_init() {
    if (!sprite_manager)
        sprite_manager = new spr::SpriteManager;

#if BREAK_SPRITE_VERTEX_LIMIT
    if (!sprite_vertex_array)
        sprite_vertex_array = new spr::SpriteVertex[sprite_vertex_array_max_count];
#endif
}

void sprite_manager_add_spr_sets(const sprite_database* spr_db) {
    sprite_manager->AddSprSets(spr_db);
}

void sprite_manager_clear() {
    sprite_manager->Clear();
}

void sprite_manager_draw(int32_t index, bool font, texture* tex, const mat4& vp) {
    sprite_manager->Draw(index, font, tex, vp);
}

int32_t sprite_manager_get_index() {
    return sprite_manager->index;
}

size_t sprite_manager_get_reqlist_count(int32_t index) {
    size_t count = 0;
    for (const auto& i : sprite_manager->reqlist[index])
        for (const auto& j : i)
            count += j.size();
    return count;
}

::spr_set* sprite_manager_get_set(uint32_t spr_id, const sprite_database* spr_db) {
    SprSet* set = sprite_manager->GetSet(spr_db->get_spr_set_by_id(spr_id)->index);
    if (set)
        return set->spr_set;
    return 0;
}

bool sprite_manager_get_set_ready(uint32_t set_id, const sprite_database* spr_db) {
    return sprite_manager->GetSetReady(spr_db->get_spr_set_by_id(set_id)->index);
}

uint32_t sprite_manager_get_set_sprite_num(uint32_t set_id, const sprite_database* spr_db) {
    return sprite_manager->GetSetSpriteNum(spr_db->get_spr_set_by_id(set_id)->index);
}

uint32_t sprite_manager_get_set_texture_num(uint32_t set_id, const sprite_database* spr_db) {
    return sprite_manager->GetSetTextureNum(spr_db->get_spr_set_by_id(set_id)->index);
}

const char* sprite_manager_get_spr_name(uint32_t spr_id, const sprite_database* spr_db) {
    return sprite_manager->GetSprName(spr_db->get_spr_by_id(spr_id)->info);
}

rectangle sprite_manager_get_spr_rectangle(uint32_t spr_id, const sprite_database* spr_db) {
    return sprite_manager->GetSprRectangle(spr_db->get_spr_by_id(spr_id)->info);
}

texture* sprite_manager_get_spr_texture(uint32_t spr_id, const sprite_database* spr_db) {
    return sprite_manager->GetSprTexture(spr_db->get_spr_by_id(spr_id)->info);
}

bool sprite_manager_load_file(uint32_t set_id, const sprite_database* spr_db) {
    return sprite_manager->LoadFile(spr_db->get_spr_set_by_id(set_id)->index);
}

bool sprite_manager_load_file_modern(uint32_t set_hash, sprite_database* spr_db) {
    return sprite_manager->LoadFileModern(spr_db->get_spr_set_by_id(set_hash)->index, spr_db);
}

void sprite_manager_read_file(uint32_t set_id,
    std::string& mdata_dir, void* data, const sprite_database* spr_db) {
    const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_id(set_id);
    sprite_manager->ReadFile(spr_set->index, spr_set->file_name.c_str(), mdata_dir, data);
}

void sprite_manager_read_file(uint32_t set_id,
    std::string&& mdata_dir, void* data, const sprite_database* spr_db) {
    const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_id(set_id);
    sprite_manager->ReadFile(spr_set->index, spr_set->file_name.c_str(), mdata_dir, data);
}

void sprite_manager_read_file_modern(uint32_t set_hash, void* data, sprite_database* spr_db) {
    const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_id(set_hash);
    sprite_manager->ReadFileModern(spr_set->index, set_hash, data, spr_db);
}

void sprite_manager_remove_spr_sets(const sprite_database* spr_db) {
    sprite_manager->RemoveSprSets(spr_db);
}

void sprite_manager_reset_req_list() {
    sprite_manager->ResetReqList();
}

void sprite_manager_reset_res_data() {
    sprite_manager->ResetResData();
}

void sprite_manager_set_index(int32_t value) {
    sprite_manager->index = value;
}

void sprite_manager_set_res(double_t aspect, int32_t width, int32_t height) {
    if (!sprite_manager)
        return;

    sprite_manager->aspect[0] = (float_t)aspect;
    sprite_manager->field_1018[0].second = { 0.0f, 0.0f, (float_t)width, (float_t)height };
    sprite_manager->aspect[1] = (float_t)aspect;
    sprite_manager->field_1018[1].second = { 0.0f, 0.0f, (float_t)width, (float_t)height };
}

void sprite_manager_unload_set(uint32_t set_id, const sprite_database* spr_db) {
    sprite_manager->UnloadSet(spr_db->get_spr_set_by_id(set_id)->index);
}

void sprite_manager_unload_set_modern(uint32_t set_hash, sprite_database* spr_db) {
    sprite_manager->UnloadSetModern(spr_db->get_spr_set_by_id(set_hash)->index, spr_db);
}

void sprite_manager_free() {
#if BREAK_SPRITE_VERTEX_LIMIT
    if (sprite_vertex_array) {
        delete[] sprite_vertex_array;
        sprite_vertex_array = 0;
    }
#endif

    if (sprite_manager) {
        delete sprite_manager;
        sprite_manager = 0;
    }
}

namespace spr {
    SpriteManager::RenderData::RenderData() : vao() {
        glGenVertexArrays(1, &vao);
        gl_state_bind_vertex_array(vao, true);

        vbo_vertex_count = 4096;

        static const GLsizei buffer_size = sizeof(sprite_draw_vertex);

        vbo.Create(buffer_size * vbo_vertex_count);
        vbo.Bind();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(sprite_draw_vertex, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size,
            (void*)offsetof(sprite_draw_vertex, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(sprite_draw_vertex, uv[0]));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, buffer_size,
            (void*)offsetof(sprite_draw_vertex, uv[1]));

        ebo_index_count = 4096;

        ebo.Create(sizeof(uint32_t) * ebo_index_count);
        ebo.Bind();

        gl_state_bind_array_buffer(0);
        gl_state_bind_vertex_array(0);
        gl_state_bind_element_array_buffer(0);
    }

    SpriteManager::RenderData::~RenderData() {
        ebo.Destroy();
        vbo.Destroy();

        if (vao) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }
    }

    void SpriteManager::RenderData::Clear() {
        draw_param_buffer.clear();
        vertex_buffer.clear();
        index_buffer.clear();
    }

    void SpriteManager::RenderData::Update() {
        static const GLsizei buffer_size = sizeof(sprite_draw_vertex);

        if (vbo_vertex_count < vertex_buffer.size()) {
            while (vbo_vertex_count < vertex_buffer.size())
                vbo_vertex_count *= 2;

            vbo.Destroy();

            gl_state_bind_vertex_array(vao, true);

            vbo.Create(buffer_size * vbo_vertex_count);
            vbo.Bind();

            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(buffer_size
                * vertex_buffer.size()), vertex_buffer.data());

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(sprite_draw_vertex, pos));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size,
                (void*)offsetof(sprite_draw_vertex, color));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(sprite_draw_vertex, uv[0]));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(sprite_draw_vertex, uv[1]));

            gl_state_bind_array_buffer(0);
            gl_state_bind_vertex_array(0);
        }
        else
            vbo.WriteMemory(0, buffer_size * vertex_buffer.size(), vertex_buffer.data());

        if (ebo_index_count < index_buffer.size()) {
            while (ebo_index_count < index_buffer.size())
                ebo_index_count *= 2;

            ebo.Destroy();

            gl_state_bind_vertex_array(vao, true);

            ebo.Create(sizeof(uint32_t) * ebo_index_count);
            ebo.Bind();

            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(uint32_t)
                * index_buffer.size()), index_buffer.data());

            gl_state_bind_vertex_array(0);
            gl_state_bind_element_array_buffer(0);
        }
        else
            ebo.WriteMemory(0, sizeof(uint32_t) * index_buffer.size(), index_buffer.data());
    }

    SpriteManager::SpriteManager() : aspect(), index(), set_counter() {
        ResetIndex();
        resolution_mode = RESOLUTION_MODE_MAX;

        resolution_struct res = resolution_struct(RESOLUTION_MODE_HD);

        aspect[0] = (float_t)res.aspect;
        field_1018[0].first = res.resolution_mode;
        field_1018[0].second = { 0.0f, 0.0f, (float_t)res.width, (float_t)res.height };

        aspect[1] = (float_t)res.aspect;
        field_1018[1].first = res.resolution_mode;
        field_1018[1].second = { 0.0f, 0.0f, (float_t)res.width, (float_t)res.height };

        view_projection = mat4_identity;
        mat = mat4_identity;
    }

    SpriteManager::~SpriteManager() {
        Clear();
    }

    uint32_t SpriteManager::AddSetModern() {
        uint32_t index = this->set_counter;
        //for (; index <= 0x0FFF; index++) {
        for (; index <= 0x3FFF; index++) {
            auto elem = sets.find(0x8000 | index);
            if (elem == sets.end())
                break;
        }

        //if (!index || index >= 0x0FFF) {
        if (!index || index > 0x3FFF) {
            //for (index = 1; index <= 0x0FFF; index++) {
            for (index = 1; index <= 0x3FFF; index++) {
                auto elem = sets.find(0x8000 | index);
                if (elem == sets.end())
                    break;
            }

            //if (!index || index > 0x0FFF)
            if (!index || index > 0x3FFF)
                return 0x8000;
        }

        sets.insert({ 0x8000 | index, SprSet(0x8000 | index) });
        set_counter = index + 1;
        //if (index + 1 > 0x0FFF)
        if (index + 1 > 0x3FFF)
            set_counter = 1;
        return 0x8000 | index;
    }

    void SpriteManager::AddSprSets(const sprite_database* spr_db) {
        for (auto& i : spr_db->spr_set_indices)
            sets.insert({ i.first, SprSet(i.first) });
    }

    void SpriteManager::Clear() {
        for (int32_t i = 0; i < 4; i++)
            for (int32_t j = 0; j < 2; j++)
            for (int32_t k = 0; k < SPR_PRIO_MAX; k++)
                reqlist[i][j][k].clear();

        for (auto i : sets)
            i.second.Unload();
        sets.clear();

        render_data.Clear();
    }

    void SpriteManager::Draw(int32_t index, bool font, texture* tex, const mat4& vp) {
        render_context* rctx = rctx_ptr;
        draw_sprite_begin(rctx);

        ::resolution_mode mode = res_window_get()->resolution_mode;
        if (index == 2 && resolution_mode != RESOLUTION_MODE_MAX)
            res_window_get()->resolution_mode = resolution_mode;

        gl_state_rect viewport_rect = gl_state_get_viewport();

        for (int32_t i = 0; i < 2; i++) {
            auto reqlist = this->reqlist[index][i];
            int32_t x_min;
            int32_t y_min;
            int32_t x_max;
            int32_t y_max;
            if (index == 0 || index == 3) {
                resolution_struct* res_wind = res_window_get();

                float_t sprite_half_width = (float_t)res_wind->width * 0.5f;
                float_t sprite_half_height = (float_t)res_wind->height * 0.5f;

                float_t aet_depth = rctx->camera->aet_depth;
                float_t aet_depth_1 = 1.0f / aet_depth;

                float_t v15a = sprite_half_height * aspect[i] * 0.2f * aet_depth_1;
                float_t v15b = sprite_half_height * 0.2f * aet_depth_1;

                mat4 proj;
                mat4_frustrum(-v15a, v15a, v15b, -v15b, 0.2f, 3000.0f, &proj);

                vec3 eye = { sprite_half_width, sprite_half_height, aet_depth };
                vec3 target = { sprite_half_width, sprite_half_height, 0.0f };
                vec3 up = { 0.0f, 1.0f, 0.0f };

                mat4 view;
                mat4_look_at(&eye, &target, &up, &view);
                mat4_mul(&view, &proj, &view_projection);

                vec2 min;
                vec2 max;
                resolution_mode_scale_pos(min, mode, field_1018[i].second.pos, field_1018[i].first);
                resolution_mode_scale_pos(max, mode,
                    field_1018[i].second.pos + field_1018[i].second.size, field_1018[i].first);
                x_min = (int32_t)min.x;
                y_min = (int32_t)min.y;
                x_max = (int32_t)(max.x - min.x);
                y_max = (int32_t)(max.y - min.y);
                gl_state_set_viewport(x_min, y_min, x_max, y_max);
            }
            else {
                view_projection = vp;

                x_min = viewport_rect.x;
                y_min = viewport_rect.y;
                x_max = viewport_rect.width;
                y_max = viewport_rect.height;
            }

            float_t v23 = (float_t)x_max * 0.5f;
            float_t v24 = (float_t)x_min + v23;
            float_t v25 = (float_t)y_max * 0.5f;
            float_t v26 = (float_t)y_min + v25;

            mat = view_projection;
            mat.row0.x = v23 * view_projection.row0.x + v24 * view_projection.row0.w;
            mat.row1.x = v23 * view_projection.row1.x + v24 * view_projection.row1.w;
            mat.row2.x = v23 * view_projection.row2.x + v24 * view_projection.row2.w;
            mat.row3.x = v23 * view_projection.row3.x + v24 * view_projection.row3.w;
            mat.row0.y = v25 * view_projection.row0.y + v26 * view_projection.row0.w;
            mat.row1.y = v25 * view_projection.row1.y + v26 * view_projection.row1.w;
            mat.row2.y = v25 * view_projection.row2.y + v26 * view_projection.row2.w;
            mat.row3.y = v25 * view_projection.row3.y + v26 * view_projection.row3.w;

            sprite_scene_shader_data shader_data = {};
            shader_data.g_framebuffer_size = {
                1.0f / (float_t)tex->width,
                1.0f / (float_t)tex->height, 0.0f, 0.0f
            };
            mat4_transpose(&view_projection, &shader_data.g_transform);
            rctx->sprite_scene_ubo.WriteMemory(shader_data);

            render_data.Clear();

            for (uint32_t j = SPR_PRIO_MAX; j; j--, reqlist++)
                for (auto& k : *reqlist)
                    draw_sprite(rctx, k, font, mat, x_min, y_min, x_max, y_max,
                        render_data.draw_param_buffer,
                        render_data.vertex_buffer, render_data.index_buffer);

            render_data.Update();

            rctx->sprite_scene_ubo.Bind(0);

            gl_state_active_bind_texture_2d(7, tex->glid);

            gl_state_bind_vertex_array(render_data.vao);
            for (sprite_draw_param& j : render_data.draw_param_buffer) {
                if (j.blend) {
                    gl_state_enable_blend();
                    gl_state_set_blend_func_separate(
                        j.blend_src_rgb, j.blend_dst_rgb,
                        j.blend_src_alpha, j.blend_dst_alpha);

                    if (j.copy_texture) {
                        gl_state_active_texture(7);
                        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, j.copy_texture_x, j.copy_texture_y,
                            j.copy_texture_x, j.copy_texture_y, j.copy_texture_width, j.copy_texture_height);
                    }
                }
                else
                    gl_state_disable_blend();

                if (j.shader == SHADER_FT_SPRITE) {
                    uniform_value[U_TEX_0_TYPE] = j.tex_0_type;
                    uniform_value[U_TEX_1_TYPE] = j.tex_1_type;
                    uniform_value[U_COMBINER] = j.combiner;
                }

                if (j.texture[0]) {
                    gl_state_active_bind_texture_2d(0, j.texture[0]);
                    gl_state_bind_sampler(0, j.sampler);

                    if (j.texture[1]) {
                        gl_state_active_bind_texture_2d(1, j.texture[1]);
                        gl_state_bind_sampler(1, j.sampler);
                    }
                    else if (!gl_state.texture_binding_2d[1])
                        gl_state_active_bind_texture_2d(1, rctx->empty_texture_2d);
                }
                else {
                    if (!gl_state.texture_binding_2d[0])
                        gl_state_active_bind_texture_2d(0, rctx->empty_texture_2d);
                    if (!gl_state.texture_binding_2d[1])
                        gl_state_active_bind_texture_2d(1, rctx->empty_texture_2d);
                }

                shaders_ft.set(j.shader);
                if (j.mode != GL_TRIANGLES)
                    shaders_ft.draw_arrays(j.mode, j.first, j.count);
                else
                    shaders_ft.draw_range_elements(j.mode, j.start, j.end,
                        j.count, GL_UNSIGNED_INT, (void*)j.offset);
            }
            gl_state_bind_vertex_array(0);
        }

        if (index == 2 && resolution_mode != RESOLUTION_MODE_MAX)
            res_window_get()->resolution_mode = mode;

        draw_sprite_end();
    }

    SprSet* SpriteManager::GetSet(uint32_t index) {
        auto elem = sets.find(index);
        if (elem != sets.end())
            return &elem->second;
        return 0;
    }

    bool SpriteManager::GetSetReady(uint32_t index) {
        SprSet* set = GetSet(index);
        if (set)
            return set->ready;
        return false;
    }

    uint32_t SpriteManager::GetSetSpriteNum(uint32_t index) {
        SprSet* set = GetSet(index);
        if (set)
            return set->spr_set->num_of_sprite;
        return 0;
    }

    uint32_t SpriteManager::GetSetTextureNum(uint32_t index) {
        SprSet* set = GetSet(index);
        if (set)
            return set->spr_set->num_of_texture;
        return 0;
    }

    const char* SpriteManager::GetSprName(spr_info info) {
        SprSet* set = GetSet(info.set_index & 0x8FFF);
        if (set)
            return set->GetName(info);
        return "(null)";
    }

    rectangle SpriteManager::GetSprRectangle(spr_info info) {
        SprSet* set = GetSet(info.set_index & 0x8FFF);
        if (set)
            return set->GetRectangle(info);
        return {};
    }

    texture* SpriteManager::GetSprTexture(spr_info info) {
        SprSet* set = GetSet(info.set_index & 0x8FFF);
        if (set)
            return set->GetTexture(info);
        return 0;
    }

    bool SpriteManager::LoadFile(uint32_t index) {
        SprSet* set = GetSet(index);
        if (set)
            return set->LoadFile();
        return false;
    }

    bool SpriteManager::LoadFileModern(uint32_t index, sprite_database* spr_db) {
        SprSet* set = GetSet(index);
        if (set)
            return set->LoadFileModern(spr_db);
        return false;
    }

    SprArgs* SpriteManager::PutSprite(const SprArgs& args, const sprite_database* spr_db) {
        int32_t index = args.index;
        if (index == -1)
            index = this->index;

        auto reqlist = &this->reqlist[index][args.layer][args.prio];
        spr_info info = args.id.info;
        if (args.kind == SPR_KIND_NORMAL)
            info = spr_db->get_spr_by_id(args.id.id)->info;

        if (info.not_null()) {
            SprSet* set = GetSet(info.set_index & 0x8FFF);
            if (!set || !set->ready)
                return 0;

            reqlist->push_back(args);
            SprArgs& _args = reqlist->back();
            _args.id.info = info;
            _args.texture = set->GetTexture(info);
            rectangle rect = set->GetRectangle(info);
            _args.SetSpriteSize({ rect.size.x, rect.size.y });
            _args.SetTexturePosSize(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
            if (_args.resolution_mode_sprite != RESOLUTION_MODE_MAX)
                _args.resolution_mode_sprite = set->GetResolutionMode(_args.id.info);
            return &_args;
        }

        if (args.texture) {
            reqlist->push_back(args);
            SprArgs& _args = reqlist->back();
            _args.id.index = -1;
            _args.SetSpriteSize({ (float_t)_args.texture->width, (float_t)_args.texture->height });
            _args.SetTexturePosSize(0.0, 0.0, (float_t)_args.texture->width, (float_t)_args.texture->height);
            return &_args;
        }
        else {
            reqlist->push_back(args);
            SprArgs& _args = reqlist->back();
            _args.id.index = -1;
            return &_args;
        }
    }

    void SpriteManager::ReadFile(uint32_t index,
        const char* file, std::string& mdata_dir, void* data) {
        SprSet* set = GetSet(index);
        if (set)
            set->ReadFile(file, mdata_dir, data);
    }

    void SpriteManager::ReadFileModern(uint32_t index,
        uint32_t set_hash, void* data, sprite_database* spr_db) {
        if (index == -1) {
            index = AddSetModern();
            spr_db->add_spr_set(set_hash, index);
        }

        SprSet* set = GetSet(index);
        if (set)
            set->ReadFileModern(set_hash, data);
    }

    void SpriteManager::ResetIndex() {
        index = 0;
    }

    void SpriteManager::RemoveSprSets(const sprite_database* spr_db) {
        for (auto& i : spr_db->spr_set_indices)
            sets.erase(i.first);
    }

    void SpriteManager::ResetReqList() {
        for (auto& i : reqlist)
            for (auto& j : i)
                for (auto& k : j)
                    k.clear();
        ResetIndex();
        sprite_vertex_array_count = 0;
    }

    void SpriteManager::ResetResData() {
        ResetIndex();
        resolution_mode = RESOLUTION_MODE_MAX;

        resolution_struct* res = res_window_get();

        aspect[0] = (float_t)res->aspect;
        field_1018[0].first = res->resolution_mode;
        field_1018[0].second = { 0.0f, 0.0f, (float_t)res->width, (float_t)res->height };

        aspect[1] = (float_t)res->aspect;
        field_1018[1].first = res->resolution_mode;
        field_1018[1].second = { 0.0f, 0.0f, (float_t)res->width, (float_t)res->height };

        view_projection = mat4_identity;
        mat = mat4_identity;
    }

    void SpriteManager::UnloadSet(uint32_t index) {
        SprSet* set = GetSet(index);
        if (set)
            set->Unload();
    }

    void SpriteManager::UnloadSetModern(uint32_t index, sprite_database* spr_db) {
        SprSet* set = GetSet(index);
        if (set) {
            set->UnloadModern(spr_db);
            if (set->load_count <= 0) {
                auto elem = sets.find(index);
                if (elem != sets.end())
                    sets.erase(elem);
            }
        }
    }

    static void calc_sprite_vertex(spr::SprArgs* args, vec3* vtx, mat4* mat, bool font) {
        vtx[0].x = 0.0f;
        vtx[0].y = 0.0f;
        vtx[1].x = 0.0f;
        vtx[1].y = args->sprite_size.y;
        vtx[1].z = 0.0f;
        vtx[2].x = args->sprite_size.x;
        vtx[2].y = args->sprite_size.y;
        vtx[2].z = 0.0f;
        vtx[3].x = args->sprite_size.x;
        vtx[3].y = 0.0f;
        vtx[3].z = 0.0f;

        mat4 m;
        SprAttr attr = args->attr;
        if (!(attr & SPR_ATTR_MATRIX)) {
            if (attr & SPR_ATTR_CTR) {
                if (attr & SPR_ATTR_CTR_LT)
                    args->center = 0.0f;
                else if (attr & SPR_ATTR_CTR_LC) {
                    args->center.x = 0.0f;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_LB) {
                    args->center.x = 0.0f;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CT) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = 0.0f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CC) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CB) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RT) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = 0.0f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RC) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RB) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
            }

            if (font) {
                if (attr & SPR_ATTR_FLIP_H) {
                    args->scale.x = -args->scale.x;
                    args->rot.z = -args->rot.z;
                }
                if (attr & SPR_ATTR_FLIP_V) {
                    args->scale.y = -args->scale.y;
                    args->rot.z = -args->rot.z;
                }
            }

            if (fabsf(args->skew_angle.x) > 0.000001f) {
                float_t skew_width = tanf(args->skew_angle.x) * args->sprite_size.y * 0.5f;
                vtx[0].x = vtx[0].x + skew_width;
                vtx[1].x = vtx[1].x - skew_width;
                vtx[2].x = vtx[2].x - skew_width;
                vtx[3].x = vtx[3].x + skew_width;
            }

            if (fabsf(args->skew_angle.y) > 0.000001f) {
                float_t skew_height = tanf(args->skew_angle.y) * args->sprite_size.x * 0.5f;
                vtx[0].y = vtx[0].y - skew_height;
                vtx[1].y = vtx[1].y - skew_height;
                vtx[2].y = vtx[2].y + skew_height;
                vtx[3].y = vtx[3].y + skew_height;
            }

            mat4_translate(&args->trans, &m);
            if (fabsf(args->rot.x) > 0.000001f)
                mat4_mul_rotate_x(&m, args->rot.x, &m);
            if (fabsf(args->rot.y) > 0.000001f)
                mat4_mul_rotate_y(&m, args->rot.y, &m);
            if (fabsf(args->rot.z) > 0.000001f)
                mat4_mul_rotate_z(&m, args->rot.z, &m);
            mat4_scale_rot(&m, &args->scale, &m);
            const vec3 center = -args->center;
            mat4_mul_translate(&m, &center, &m);
        }
        else {
            mat4_translate(&args->trans, &m);
            mat4_scale_rot(&m, &args->scale, &m);
        }

        mat4_mul(&args->mat, &m, &m);

        mat4_transform_point(&m, &vtx[0], &vtx[0]);
        mat4_transform_point(&m, &vtx[1], &vtx[1]);
        mat4_transform_point(&m, &vtx[2], &vtx[2]);
        mat4_transform_point(&m, &vtx[3], &vtx[3]);

        if (mat)
            *mat = m;
    }

    static int32_t calc_sprite_texture_param(SprArgs* args, spr::TexParam* param, vec3* vtx, bool font) {
        int32_t tex_param_count = 0;
        texture* tex = args->texture;
        while (args) {
            param->texture = args->texture;

            float_t width = (float_t)args->texture->width;
            float_t height = (float_t)args->texture->height;

            float_t u_scale = 1.0f / width;
            float_t v_scale = 1.0f / height;

            if (!args->num_vertex) {
                vec2 uv00;
                vec2 uv01;
                vec2 uv10;
                vec2 uv11;
                if (args->kind == SPR_KIND_LINE) {
                    vec3 v42[4];
                    mat4 mat;
                    calc_sprite_vertex(args, v42, &mat, font);
                    mat4_invert(&mat, &mat);
                    mat4_transform_point(&mat, &vtx[0], &v42[0]);
                    mat4_transform_point(&mat, &vtx[1], &v42[1]);
                    mat4_transform_point(&mat, &vtx[2], &v42[2]);
                    mat4_transform_point(&mat, &vtx[3], &v42[3]);
                    uv00 = *(vec2*)&v42[0].x;
                    uv01 = *(vec2*)&v42[1].x;
                    uv10 = *(vec2*)&v42[2].x;
                    uv11 = *(vec2*)&v42[3].x;
                }
                else {
                    uv00.x = 0.0f;
                    uv00.y = 0.0f;
                    uv01.x = 0.0f;
                    uv01.y = args->texture_size.y;
                    uv10.x = args->texture_size.x;
                    uv10.y = args->texture_size.y;
                    uv11.x = args->texture_size.x;
                    uv11.y = 0.0f;
                }

                vec2 texture_pos = args->texture_pos;
                param->texcoord.uv[0].u = (uv00.x + texture_pos.x) * u_scale;
                param->texcoord.uv[0].v = (height - (uv00.y + texture_pos.y)) * v_scale;
                param->texcoord.uv[1].u = (uv01.x + texture_pos.x) * u_scale;
                param->texcoord.uv[1].v = (height - (uv01.y + texture_pos.y)) * v_scale;
                param->texcoord.uv[2].u = (uv10.x + texture_pos.x) * u_scale;
                param->texcoord.uv[2].v = (height - (uv10.y + texture_pos.y)) * v_scale;
                param->texcoord.uv[3].u = (uv11.x + texture_pos.x) * u_scale;
                param->texcoord.uv[3].v = (height - (uv11.y + texture_pos.y)) * v_scale;
            }
            else if (font) {
#if BREAK_SPRITE_VERTEX_LIMIT
                SpriteVertex* vtx = sprite_vertex_array + args->vertex_array;
#else
                SpriteVertex* vtx = args->vertex_array;
#endif
                for (size_t i = args->num_vertex; i; i--, vtx++) {
                    vtx->uv.x = vtx->uv.x * u_scale;
                    vtx->uv.y = (height - vtx->uv.y) * v_scale;
                }
            }

            args = args->next;
            param++;
            tex_param_count++;
        }
        return tex_param_count;
    }

    static void draw_sprite(render_context* rctx, SprArgs& args, bool font,
        const mat4& mat, int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max,
        std::vector<sprite_draw_param>& draw_param_buffer,
        std::vector<sprite_draw_vertex>& vertex_buffer, std::vector<uint32_t>& index_buffer) {
        if (args.kind == SPR_KIND_LINE)
            return;

        const color4u8 color = args.color;

        if (font)
            draw_sprite_scale(&args);

        vec3 vtx[4] = {};
        spr::TexParam tex_param[4] = {};

        if (args.flags & SprArgs::SPRITE_SIZE)
            calc_sprite_vertex(&args, vtx, 0, font);

        int32_t tex_param_count = 0;
        if ((args.flags & SprArgs::TEXTURE_POS_SIZE) && args.texture) {
            tex_param_count = calc_sprite_texture_param(&args, tex_param, vtx, font);
            tex_param_count = min_def(tex_param_count, 2);
        }

        mat4 mat_t;
        mat4_transpose(&mat, &mat_t);

        draw_param_buffer.push_back({});
        sprite_draw_param& draw_param = draw_param_buffer.back();

        if (!(args.attr & SPR_ATTR_NOBLEND)) {
            const GLenum* blend = spr_blend_param[args.blend];
            draw_param.blend = true;
            draw_param.blend_src_rgb = blend[0];
            draw_param.blend_dst_rgb = blend[1];
            draw_param.blend_src_alpha = blend[2];
            draw_param.blend_dst_alpha = blend[3];

            if (args.blend == 5 && !args.num_vertex && args.kind == SPR_KIND_NORMAL) {
                float_t v25 = 0.0f;
                float_t v26 = 0.0f;
                float_t v27 = 0.0f;
                float_t v28 = 0.0f;
                for (int32_t i = 0, j = 0; i < 4; i++) {
                    vec4 v;
                    *(vec3*)&v = vtx[i];
                    v.w = 1.0f;

                    float_t v31 = vec4::dot(mat_t.row3, v);
                    if (v31 * v31 <= 0.0001f)
                        continue;

                    float_t v33 = vec4::dot(mat_t.row0, v) * (1.0f / v31);
                    float_t v34 = vec4::dot(mat_t.row1, v) * (1.0f / v31);
                    if (j) {
                        if (v25 > v33)
                            v25 = v33;
                        else if (v27 < v33)
                            v27 = v33;

                        if (v26 > v34)
                            v26 = v34;
                        else if (v28 < v34)
                            v28 = v34;
                    }
                    else {
                        v25 = v33;
                        v27 = v33;
                        v26 = v34;
                        v28 = v34;
                    }
                    j++;
                }

                int32_t v36 = (int32_t)v27 + 1;
                int32_t v37 = (int32_t)v28 + 1;
                int32_t v38 = (int32_t)v25 - 1;
                int32_t v39 = (int32_t)v26 - 1;

                v36 = clamp_def(v36, x_min, x_min + x_max - 1);
                v37 = clamp_def(v37, y_min, y_min + y_max - 1);
                v38 = clamp_def(v38, x_min, x_min + x_max - 1);
                v39 = clamp_def(v39, y_min, y_min + y_max - 1);

                int32_t v42 = v36 - v38 + 1;
                int32_t v43 = v37 - v39 + 1;
                if (v42 * v43 > 0) {
                    draw_param.copy_texture = true;
                    draw_param.copy_texture_x = v38;
                    draw_param.copy_texture_y = v39;
                    draw_param.copy_texture_width = v42;
                    draw_param.copy_texture_height = v43;
                }
            }
        }

        draw_param.shader = args.shader;
        if (draw_param.shader == SHADER_FT_FFP) {
            draw_param.shader = SHADER_FT_SPRITE;
            draw_param.tex_0_type = 0;
            draw_param.tex_1_type = 0;

            if (tex_param_count == 1 || tex_param_count == 2) {
                if (tex_param[0].texture->internal_format == GL_COMPRESSED_RED_RGTC1_EXT)
                    draw_param.tex_0_type = 3;
                else if (tex_param[0].texture->internal_format == GL_COMPRESSED_RED_GREEN_RGTC2_EXT)
                    draw_param.tex_0_type = 2;
                else
                    draw_param.tex_0_type = 1;

                if (tex_param_count == 2) {
                    if (tex_param[1].texture->internal_format == GL_COMPRESSED_RED_RGTC1_EXT)
                        draw_param.tex_1_type = 3;
                    else if (tex_param[1].texture->internal_format == GL_COMPRESSED_RED_GREEN_RGTC2_EXT)
                        draw_param.tex_1_type = 2;
                    else
                        draw_param.tex_1_type = 1;
                }
            }

            if (args.attr & SPR_ATTR_NOBLEND)
                draw_param.combiner = 0;
            else if (args.blend != 5)
                draw_param.combiner = args.blend == 3 ? 1 : 0;
            else
                draw_param.combiner = 2;

        }

        switch (tex_param_count) {
        case 0:
            switch (args.kind) {
            case SPR_KIND_LINES: {
                if (vtx[0] == 0.0f && vtx[2] == 0.0f) {
                    draw_param_buffer.pop_back();
                    return;
                }

                sprite_draw_vertex spr_vtx[2] = {};
                spr_vtx[0].pos = vtx[0];
                spr_vtx[0].color = color;

                spr_vtx[1].pos = vtx[2];
                spr_vtx[1].color = color;

                draw_param.mode = GL_LINES;
                draw_param.first = (GLint)vertex_buffer.size();
                draw_param.count = 2;

                vertex_buffer.reserve(2);
                vertex_buffer.push_back(spr_vtx[0]);
                vertex_buffer.push_back(spr_vtx[1]);
            } break;
            case SPR_KIND_RECT: {
                if (vtx[0] == 0.0f && vtx[1] == 0.0f && vtx[2] == 0.0f && vtx[3] == 0.0f) {
                    draw_param_buffer.pop_back();
                    return;
                }

                sprite_draw_vertex spr_vtx[4] = {};
                spr_vtx[0].pos = vtx[0];
                spr_vtx[0].color = color;

                spr_vtx[1].pos = vtx[1];
                spr_vtx[1].color = color;

                spr_vtx[2].pos = vtx[2];
                spr_vtx[2].color = color;

                spr_vtx[3].pos = vtx[3];
                spr_vtx[3].color = color;

                draw_param.mode = GL_TRIANGLES;
                draw_param.start = (GLuint)vertex_buffer.size();
                draw_param.end = draw_param.start + 3;
                draw_param.count = 6;
                draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                uint32_t start_vertex_index = (uint32_t)vertex_buffer.size();

                vertex_buffer.reserve(4);
                vertex_buffer.push_back(spr_vtx[0]); // LB
                vertex_buffer.push_back(spr_vtx[3]); // RB
                vertex_buffer.push_back(spr_vtx[2]); // RT
                vertex_buffer.push_back(spr_vtx[1]); // LT

                index_buffer.reserve(6);
                index_buffer.push_back(start_vertex_index + 0); // LB
                index_buffer.push_back(start_vertex_index + 3); // LT
                index_buffer.push_back(start_vertex_index + 1); // RB
                index_buffer.push_back(start_vertex_index + 1); // RB
                index_buffer.push_back(start_vertex_index + 3); // LT
                index_buffer.push_back(start_vertex_index + 2); // RT
            } break;
            case SPR_KIND_MULTI: {
                if (vtx[0] == 0.0f && vtx[1] == 0.0f && vtx[2] == 0.0f && vtx[3] == 0.0f) {
                    draw_param_buffer.pop_back();
                    return;
                }

                sprite_draw_vertex spr_vtx[4] = {};
                spr_vtx[0].pos = vtx[0];
                spr_vtx[0].color = color;

                spr_vtx[1].pos = vtx[1];
                spr_vtx[1].color = color;

                spr_vtx[2].pos = vtx[2];
                spr_vtx[2].color = color;

                spr_vtx[3].pos = vtx[3];
                spr_vtx[3].color = color;

                draw_param.mode = GL_LINE_LOOP;
                draw_param.first = (GLint)vertex_buffer.size();
                draw_param.count = 4;

                vertex_buffer.reserve(4);
                vertex_buffer.push_back(spr_vtx[0]);
                vertex_buffer.push_back(spr_vtx[1]);
                vertex_buffer.push_back(spr_vtx[2]);
                vertex_buffer.push_back(spr_vtx[3]);
            } break;
            case SPR_KIND_ARROW_B: {
                sprite_draw_vertex spr_vtx[2] = {};
                spr_vtx[0].color = color;
                spr_vtx[1].color = color;

                if (args.num_vertex) {
                    draw_param.mode = GL_LINES;
                    draw_param.first = (GLint)vertex_buffer.size();
                    draw_param.count = (uint32_t)(args.num_vertex / 2 * 2);

                    size_t vertex_buffer_size = vertex_buffer.size();
                    vertex_buffer.reserve(args.num_vertex / 2 * 2);

#if BREAK_SPRITE_VERTEX_LIMIT
                    SpriteVertex* vtx = sprite_vertex_array + args.vertex_array;
#else
                    SpriteVertex* vtx = args.vertex_array;
#endif
                    for (size_t i = args.num_vertex / 2; i; i--, vtx += 2) {
                        if (vtx[0].pos == 0.0f && vtx[1].pos == 0.0f)
                            continue;

                        spr_vtx[0].pos = vtx[0].pos;
                        spr_vtx[1].pos = vtx[1].pos;
                        vertex_buffer.push_back(spr_vtx[0]);
                        vertex_buffer.push_back(spr_vtx[1]);
                    }

                    if (vertex_buffer_size == vertex_buffer.size()) {
                        draw_param_buffer.pop_back();
                        return;
                    }
                }
                else {
                    if (vtx[0] == 0.0f && vtx[2] == 0.0f) {
                        draw_param_buffer.pop_back();
                        return;
                    }

                    spr_vtx[0].pos = vtx[0];
                    spr_vtx[1].pos = vtx[2];

                    draw_param.mode = GL_LINES;
                    draw_param.first = (GLint)vertex_buffer.size();
                    draw_param.count = 2;

                    vertex_buffer.reserve(2);
                    vertex_buffer.push_back(spr_vtx[0]);
                    vertex_buffer.push_back(spr_vtx[1]);
                }
            } break;
            case SPR_KIND_ARROW_AB: {
                if (!args.num_vertex) {
                    draw_param_buffer.pop_back();
                    return;
                }

                draw_param.mode = GL_LINE_STRIP;
                draw_param.first = (GLint)vertex_buffer.size();
                draw_param.count = (GLsizei)args.num_vertex;

                vertex_buffer.reserve(args.num_vertex);

                sprite_draw_vertex spr_vtx = {};
                spr_vtx.color = color;

#if BREAK_SPRITE_VERTEX_LIMIT
                SpriteVertex* vtx = sprite_vertex_array + args.vertex_array;
#else
                SpriteVertex* vtx = args.vertex_array;
#endif
                for (size_t i = args.num_vertex; i; i--, vtx++) {
                    spr_vtx.pos = vtx->pos;
                    vertex_buffer.push_back(spr_vtx);
                }
            } break;
            case SPR_KIND_TRIANGLE: {
                if (!args.num_vertex) {
                    draw_param_buffer.pop_back();
                    return;
                }

                draw_param.mode = GL_TRIANGLE_STRIP;
                draw_param.first = (GLint)vertex_buffer.size();
                draw_param.count = (GLsizei)args.num_vertex;

                vertex_buffer.reserve(args.num_vertex);

                sprite_draw_vertex spr_vtx = {};
                spr_vtx.color = color;

#if BREAK_SPRITE_VERTEX_LIMIT
                SpriteVertex* vtx = sprite_vertex_array + args.vertex_array;
#else
                SpriteVertex* vtx = args.vertex_array;
#endif
                for (size_t i = args.num_vertex; i; i--, vtx++) {
                    spr_vtx.pos = vtx->pos;
                    vertex_buffer.push_back(spr_vtx);
                }
            } break;
            default:
                draw_param_buffer.pop_back();
                return;
            }
            break;
        case 1:
            draw_param.texture[0] = tex_param[0].texture ? tex_param[0].texture->glid : 0;
            draw_param.sampler = rctx->sprite_samplers[0];

            if (args.num_vertex) {
                if (args.kind == SPR_KIND_TRIANGLE)
                    draw_param.sampler = rctx->sprite_samplers[1];

                if (args.kind == SPR_KIND_TRIANGLE) {
                    draw_param.mode = GL_TRIANGLE_STRIP;
                    draw_param.first = (GLint)vertex_buffer.size();
                    draw_param.count = (GLsizei)args.num_vertex;

                    vertex_buffer.reserve(args.num_vertex);

                    sprite_draw_vertex spr_vtx = {};

#if BREAK_SPRITE_VERTEX_LIMIT
                    SpriteVertex* vtx = sprite_vertex_array + args.vertex_array;
#else
                    SpriteVertex* vtx = args.vertex_array;
#endif
                    for (size_t i = args.num_vertex; i; i--, vtx++) {
                        spr_vtx.pos = vtx->pos;
                        spr_vtx.uv[0] = vtx->uv;
                        spr_vtx.color = vtx->color;
                        vertex_buffer.push_back(spr_vtx);
                    }
                }
                else {
                    size_t num_vertex = args.num_vertex / 4 * 6;

                    draw_param.mode = GL_TRIANGLES;
                    draw_param.start = (GLuint)vertex_buffer.size();
                    draw_param.end = draw_param.start + (GLuint)(args.num_vertex - 1);
                    draw_param.count = (GLsizei)num_vertex;
                    draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                    uint32_t start_vertex_index = (uint32_t)vertex_buffer.size();

                    size_t vertex_buffer_size = vertex_buffer.size();
                    vertex_buffer.reserve(args.num_vertex);
                    index_buffer.reserve(num_vertex);

                    sprite_draw_vertex spr_vtx[4] = {};

#if BREAK_SPRITE_VERTEX_LIMIT
                    SpriteVertex* vtx = sprite_vertex_array + args.vertex_array;
#else
                    SpriteVertex* vtx = args.vertex_array;
#endif
                    for (size_t i = num_vertex / 6, j = 0; i; i--, j += 4, vtx += 4) {
                        if (vtx[0].pos == 0.0f && vtx[1].pos == 0.0f
                            && vtx[2].pos == 0.0f && vtx[3].pos == 0.0f)
                            continue;

                        spr_vtx[0].pos = vtx[0].pos;
                        spr_vtx[0].uv[0] = vtx[0].uv;
                        spr_vtx[0].color = vtx[0].color;
                        spr_vtx[1].pos = vtx[1].pos;
                        spr_vtx[1].uv[0] = vtx[1].uv;
                        spr_vtx[1].color = vtx[1].color;
                        spr_vtx[2].pos = vtx[2].pos;
                        spr_vtx[2].uv[0] = vtx[2].uv;
                        spr_vtx[2].color = vtx[2].color;
                        spr_vtx[3].pos = vtx[3].pos;
                        spr_vtx[3].uv[0] = vtx[3].uv;
                        spr_vtx[3].color = vtx[3].color;

                        vertex_buffer.push_back(spr_vtx[0]); // LB
                        vertex_buffer.push_back(spr_vtx[3]); // RB
                        vertex_buffer.push_back(spr_vtx[2]); // RT
                        vertex_buffer.push_back(spr_vtx[1]); // LT

                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 0); // LB
                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 3); // LT
                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 1); // RB
                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 1); // RB
                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 3); // LT
                        index_buffer.push_back(start_vertex_index + (uint32_t)j + 2); // RT
                    }

                    if (vertex_buffer_size == vertex_buffer.size()) {
                        draw_param_buffer.pop_back();
                        return;
                    }
                }
            }
            else {
                if (vtx[0] == 0.0f && vtx[1] == 0.0f && vtx[2] == 0.0f && vtx[3] == 0.0f) {
                    draw_param_buffer.pop_back();
                    return;
                }

                sprite_draw_vertex spr_vtx[4] = {};
                spr_vtx[0].pos = vtx[0];
                spr_vtx[0].uv[0].x = tex_param[0].texcoord.uv[0].u;
                spr_vtx[0].uv[0].y = tex_param[0].texcoord.uv[0].v;
                spr_vtx[0].color = color;

                spr_vtx[1].pos = vtx[1];
                spr_vtx[1].uv[0].x = tex_param[0].texcoord.uv[1].u;
                spr_vtx[1].uv[0].y = tex_param[0].texcoord.uv[1].v;
                spr_vtx[1].color = color;

                spr_vtx[2].pos = vtx[2];
                spr_vtx[2].uv[0].x = tex_param[0].texcoord.uv[2].u;
                spr_vtx[2].uv[0].y = tex_param[0].texcoord.uv[2].v;
                spr_vtx[2].color = color;

                spr_vtx[3].pos = vtx[3];
                spr_vtx[3].uv[0].x = tex_param[0].texcoord.uv[3].u;
                spr_vtx[3].uv[0].y = tex_param[0].texcoord.uv[3].v;
                spr_vtx[3].color = color;

                draw_param.mode = GL_TRIANGLES;
                draw_param.start = (GLuint)vertex_buffer.size();
                draw_param.end = draw_param.start + 3;
                draw_param.count = 6;
                draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                uint32_t start_vertex_index = (uint32_t)vertex_buffer.size();

                vertex_buffer.reserve(4);
                vertex_buffer.push_back(spr_vtx[0]); // LB
                vertex_buffer.push_back(spr_vtx[3]); // RB
                vertex_buffer.push_back(spr_vtx[2]); // RT
                vertex_buffer.push_back(spr_vtx[1]); // LT

                index_buffer.reserve(6);
                index_buffer.push_back(start_vertex_index + 0); // LB
                index_buffer.push_back(start_vertex_index + 3); // LT
                index_buffer.push_back(start_vertex_index + 1); // RB
                index_buffer.push_back(start_vertex_index + 1); // RB
                index_buffer.push_back(start_vertex_index + 3); // LT
                index_buffer.push_back(start_vertex_index + 2); // RT
            }
            break;
        case 2: {
            if (vtx[0] == 0.0f && vtx[1] == 0.0f && vtx[2] == 0.0f && vtx[3] == 0.0f) {
                draw_param_buffer.pop_back();
                return;
            }

            draw_param.texture[0] = tex_param[0].texture ? tex_param[0].texture->glid : 0;
            draw_param.texture[1] = tex_param[1].texture ? tex_param[1].texture->glid : 0;
            draw_param.sampler = rctx->sprite_samplers[2];

            sprite_draw_vertex spr_vtx[4] = {};
            spr_vtx[0].pos = vtx[0];
            spr_vtx[0].uv[0].x = tex_param[0].texcoord.uv[0].u;
            spr_vtx[0].uv[0].y = tex_param[0].texcoord.uv[0].v;
            spr_vtx[0].uv[1].x = tex_param[1].texcoord.uv[0].u;
            spr_vtx[0].uv[1].y = tex_param[1].texcoord.uv[0].v;
            spr_vtx[0].color = color;

            spr_vtx[1].pos = vtx[1];
            spr_vtx[1].uv[0].x = tex_param[0].texcoord.uv[1].u;
            spr_vtx[1].uv[0].y = tex_param[0].texcoord.uv[1].v;
            spr_vtx[1].uv[1].x = tex_param[1].texcoord.uv[1].u;
            spr_vtx[1].uv[1].y = tex_param[1].texcoord.uv[1].v;
            spr_vtx[1].color = color;

            spr_vtx[2].pos = vtx[2];
            spr_vtx[2].uv[0].x = tex_param[0].texcoord.uv[2].u;
            spr_vtx[2].uv[0].y = tex_param[0].texcoord.uv[2].v;
            spr_vtx[2].uv[1].x = tex_param[1].texcoord.uv[2].u;
            spr_vtx[2].uv[1].y = tex_param[1].texcoord.uv[2].v;
            spr_vtx[2].color = color;

            spr_vtx[3].pos = vtx[3];
            spr_vtx[3].uv[0].x = tex_param[0].texcoord.uv[3].u;
            spr_vtx[3].uv[0].y = tex_param[0].texcoord.uv[3].v;
            spr_vtx[3].uv[1].x = tex_param[1].texcoord.uv[3].u;
            spr_vtx[3].uv[1].y = tex_param[1].texcoord.uv[3].v;
            spr_vtx[3].color = color;

            draw_param.mode = GL_TRIANGLES;
            draw_param.start = (GLuint)vertex_buffer.size();
            draw_param.end = draw_param.start + 3;
            draw_param.count = 6;
            draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

            uint32_t start_vertex_index = (uint32_t)vertex_buffer.size();

            vertex_buffer.reserve(4);
            vertex_buffer.push_back(spr_vtx[0]); // LB
            vertex_buffer.push_back(spr_vtx[3]); // RB
            vertex_buffer.push_back(spr_vtx[2]); // RT
            vertex_buffer.push_back(spr_vtx[1]); // LT

            index_buffer.reserve(6);
            index_buffer.push_back(start_vertex_index + 0); // LB
            index_buffer.push_back(start_vertex_index + 3); // LT
            index_buffer.push_back(start_vertex_index + 1); // RB
            index_buffer.push_back(start_vertex_index + 1); // RB
            index_buffer.push_back(start_vertex_index + 3); // LT
            index_buffer.push_back(start_vertex_index + 2); // RT
        } break;
        }

        if (draw_param_buffer.size() >= 2) {
            sprite_draw_param& draw_param_2 = draw_param_buffer.data()[draw_param_buffer.size() - 2];
            sprite_draw_param& draw_param_1 = draw_param_buffer.data()[draw_param_buffer.size() - 1];

            if (draw_param_1.mode == GL_TRIANGLES
                && draw_param_2.mode == draw_param_1.mode
                && draw_param_2.blend == draw_param_1.blend
                && draw_param_2.blend_src_rgb == draw_param_1.blend_src_rgb
                && draw_param_2.blend_src_alpha == draw_param_1.blend_src_alpha
                && draw_param_2.blend_dst_rgb == draw_param_1.blend_dst_rgb
                && draw_param_2.blend_dst_alpha == draw_param_1.blend_dst_alpha
                && !draw_param_2.copy_texture && !draw_param_1.copy_texture
                && draw_param_2.shader == draw_param_1.shader
                && draw_param_2.tex_0_type == draw_param_1.tex_0_type
                && draw_param_2.tex_1_type == draw_param_1.tex_1_type
                && draw_param_2.combiner == draw_param_1.combiner
                && draw_param_2.texture[0] == draw_param_1.texture[0]
                && draw_param_2.texture[1] == draw_param_1.texture[1]
                && draw_param_2.sampler == draw_param_1.sampler
                && draw_param_2.end + 1 == draw_param_1.start) {
                draw_param_2.end = draw_param_1.end;
                draw_param_2.count += draw_param_1.count;
                draw_param_buffer.pop_back();
            }
        }
    }

    static void draw_sprite_begin(render_context* rctx) {
        gl_state_disable_blend();
        gl_state_active_bind_texture_2d(0, rctx->empty_texture_2d);
        gl_state_bind_sampler(0, 0);
        gl_state_active_bind_texture_2d(1, rctx->empty_texture_2d);
        gl_state_bind_sampler(1, 0);
        gl_state_bind_sampler(7, 0);
        gl_state_set_blend_func_separate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    }

    static void draw_sprite_end() {
        gl_state_disable_blend();
        gl_state_active_bind_texture_2d(0, 0);
        gl_state_bind_sampler(0, 0);
        gl_state_active_bind_texture_2d(1, 0);
        gl_state_bind_sampler(1, 0);
        gl_state_active_bind_texture_2d(7, 0);
        gl_state_bind_sampler(7, 0);
        gl_state_set_blend_func_separate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        shader::unbind();
    }

    static void draw_sprite_scale(spr::SprArgs* args) {
        int32_t index = args->index;
        resolution_mode mode = res_window_get()->resolution_mode;
        while (args) {
            resolution_mode_scale_data data(args->resolution_mode_screen, mode);
            if (args->resolution_mode_screen != RESOLUTION_MODE_MAX
                && args->resolution_mode_screen != mode && (index <= 0 || index >= 3)) {
                float_t scale_x = data.scale.x;
                float_t scale_y = data.scale.y;
                float_t src_res_x = data.src_res.x;
                float_t src_res_y = data.src_res.y;
                float_t dst_res_x = data.dst_res.x;
                float_t dst_res_y = data.dst_res.y;
                args->trans.x = (args->trans.x - src_res_x) * scale_x + src_res_x;
                args->trans.y = (args->trans.y - src_res_y) * scale_y + src_res_y;
                args->trans.z = args->trans.z * scale_y;

#if BREAK_SPRITE_VERTEX_LIMIT
                SpriteVertex* vtx = sprite_vertex_array + args->vertex_array;
#else
                SpriteVertex* vtx = args->vertex_array;
#endif
                for (size_t i = args->num_vertex; i; i--, vtx++) {
                    vtx->pos.x = (vtx->pos.x - src_res_x) * scale_x + dst_res_x;
                    vtx->pos.y = (vtx->pos.y - src_res_y) * scale_y + dst_res_y;
                }
            }

            if (args->resolution_mode_sprite != RESOLUTION_MODE_MAX) {
                vec2 scale = resolution_mode_get_scale(mode, args->resolution_mode_sprite);
                args->scale.x = scale.x * args->scale.x;
                args->scale.y = scale.y * args->scale.y;
            }
            args = args->next;
        }
    }
}

SprSet::SprSet(uint32_t index) : flag(), spr_set(),
textures(), load_count(), ready() {
    this->index = index;
    hash = hash_murmurhash_empty;
}

SprSet::~SprSet() {

}

const char* SprSet::GetName(spr_info info) {
    //if (info.set_index & 0x1000)
    if (info.set_index & 0x4000)
        return spr_set->texname[info.index];
    else
        return spr_set->sprname[info.index];
}

rectangle SprSet::GetRectangle(spr_info info) {
    //if (info.set_index & 0x1000) {
    if (info.set_index & 0x4000) {
        texture* tex = textures[info.index];
        return { 0.0f, 0.0f, (float_t)tex->width, (float_t)tex->height };
    }

    spr::SprInfo* sprinfo = &spr_set->sprinfo[info.index];
    return { (float_t)(int32_t)sprinfo->px, (float_t)(int32_t)sprinfo->py,
        (float_t)(int32_t)sprinfo->width, (float_t)(int32_t)sprinfo->height };
}

resolution_mode SprSet::GetResolutionMode(spr_info info) {
    //if (info.set_index & 0x1000)
    if (info.set_index & 0x4000)
        return RESOLUTION_MODE_HD;
    else
        return spr_set->sprdata[info.index].resolution_mode;
}

texture* SprSet::GetTexture(spr_info info) {
    //if (info.set_index & 0x1000)
    if (info.set_index & 0x4000)
        return textures[info.index];
    else
        return textures[spr_set->sprinfo[info.index].texid];
}

bool SprSet::LoadData() {
    const void* data = file_handler.get_data();
    size_t size = file_handler.get_size();
    if (!data || !size)
        return false;

    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    ::spr_set* set = alloc->allocate<::spr_set>();
    this->spr_set = set;
    set->unpack_file(alloc, data, size, false);
    return true;
}

bool SprSet::LoadDataModern(const void* data, size_t size) {
    if (!data || !size)
        return false;

    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    ::spr_set* set = alloc->allocate<::spr_set>();
    this->spr_set = set;
    set->unpack_file(alloc, data, size, true);
    file_handler.reset();
    return true;
}

bool SprSet::LoadFile() {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;
    else if (!LoadData() || !LoadTexture())
        return false;

    file_handler.reset();
    ready = true;
    return true;
}

bool SprSet::LoadFileModern(sprite_database* spr_db) {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;

    const void* data = file_handler.get_data();
    size_t size = file_handler.get_size();
    if (!data || !size)
        return false;

    farc f;
    f.read(data, size, true);

    std::string& file = file_handler.ptr->file;

    size_t file_len = file.size();
    if (file_len >= 0x100 - 4)
        return false;

    const char* t = strrchr(file.c_str(), '.');
    if (t)
        file_len = t - file.c_str();

    char buf[0x100];
    memcpy(buf, file.c_str(), file_len);
    char* ext = buf + file_len;
    size_t ext_len = sizeof(buf) - file_len;

    memcpy_s(ext, ext_len, ".spr", 5);
    farc_file* spr = f.read_file(buf);
    if (!spr)
        return false;

    memcpy_s(ext, ext_len, ".spi", 5);
    farc_file* spi = f.read_file(buf);
    if (!spi)
        return false;

    sprite_database_file spr_db_file;
    spr_db_file.read(spi->data, spi->size, true);

    spr_db_spr_set_file* spr_set_file = 0;
    if (spr_db_file.ready)
        for (spr_db_spr_set_file& m : spr_db_file.sprite_set)
            if (m.id == hash) {
                spr_set_file = &m;
                break;
            }

    if (!spr_set_file)
        return false;

    if (spr_db_file.ready)
        spr_db->parse(spr_set_file, name, sprite_ids);

    if (!LoadDataModern(spr->data, spr->size) || !LoadTextureModern(spr->data, spr->size))
        return false;

    file_handler.reset();
    ready = true;
    return true;
}

bool SprSet::LoadTexture() {
    size_t data = (size_t)file_handler.get_data();
    if (!data)
        return false;

    std::vector<uint32_t> ids(spr_set->num_of_texture);
    //uint32_t set_index = (uint32_t)(0x1000 | index & 0x0FFF);
    uint32_t set_index = (uint32_t)(0x4000 | index & 0x3FFF);
    uint32_t index = 0;
    for (uint32_t& i : ids)
        i = (set_index << 16) | index++;
    texture_txp_set_load(spr_set->txp, &textures, ids.data());
    return !!textures;
}

bool SprSet::LoadTextureModern(const void* data, size_t size) {
    if (!data || !size)
        return false;

    std::vector<uint32_t> ids(spr_set->num_of_texture);
    //uint32_t set_index = (uint32_t)(0x8000 | 0x1000 | index & 0x0FFF);
    uint32_t set_index = (uint32_t)(0x8000 | 0x4000 | index & 0x3FFF);
    uint32_t index = 0;
    for (uint32_t& i : ids)
        i = (set_index << 16) | index++;
    texture_txp_set_load(spr_set->txp, &textures, ids.data());
    return !!textures;
}

void SprSet::ReadFile(const char* file, std::string& mdata_dir, void* data) {
    if (load_count > 1) {
        load_count++;
        return;
    }

    load_count = 1;
    this->file.assign(file);

    std::string farc_file(file);
    size_t off = farc_file.rfind(".bin");
    if (off != -1 && off == farc_file.size() - 4)
        farc_file.replace(off, 4, ".farc");

    const char* dir = "./rom/2d/";
    if (mdata_dir.size() && ((data_struct*)data)->check_directory_exists(mdata_dir.c_str()))
        dir = mdata_dir.c_str();

    if (((data_struct*)data)->check_file_exists(dir, farc_file.c_str()))
        file_handler.read_file(data, dir, farc_file.c_str(), file, false);
    ready = false;
}

void SprSet::ReadFileModern(uint32_t set_hash, void* data) {
    if (load_count > 1) {
        load_count++;
        return;
    }

    std::string file;
    if (((data_struct*)data)->get_file("root+/2d/", set_hash, ".farc", file))
        file_handler.read_file(data, "root+/2d/", file.c_str());
    this->hash = set_hash;
    ready = false;
}

void SprSet::Unload() {
    if (load_count > 1) {
        load_count--;
        return;
    }

    file_handler.reset();
    UnloadTexture();
    alloc_handler.reset();

    spr_set = 0;
    file.clear();
    ready = false;
    load_count = 0;
}

void SprSet::UnloadModern(sprite_database* spr_db) {
    if (load_count > 1) {
        load_count--;
        return;
    }

    spr_db->remove_spr_set(hash, index, name.c_str(), sprite_ids);

    file_handler.reset();
    UnloadTexture();
    alloc_handler.reset();

    spr_set = 0;
    file.clear();
    ready = false;
    load_count = 0;

    hash = hash_murmurhash_empty;
    name.clear();
    sprite_ids.clear();
}

bool SprSet::UnloadTexture() {
    if (spr_set && spr_set->txp) {
        delete spr_set->txp;
        spr_set->txp = 0;
    }

    if (textures) {
        texture_array_free(textures);
        textures = 0;
        return true;
    }
    return false;
}
