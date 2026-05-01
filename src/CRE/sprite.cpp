/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sprite.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/prj/stack_allocator.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/txp.hpp"
#include "GL/array_buffer.hpp"
#include "GL/element_array_buffer.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "gl.hpp"
#include "gl_state.hpp"
#include "render_context.hpp"
#include "shader_ft.hpp"
#include "texture.hpp"

class SPRSET {
public:
    int32_t flag;
    uint32_t id;
    ::spr_set* spr_set;
    texture** textures;
    int32_t reference;
    p_file_handler file_handler;
    bool ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    std::string name;

    std::string set_name;
    std::vector<uint32_t> sprite_ids;
    uint32_t hash;

    SPRSET(uint32_t in_id);
    virtual ~SPRSET();
    
    void request(const char* fname, const std::string mdata_dir, void* data);
    void requestModern(uint32_t sethash, void* data); // Added
    bool wait();
    bool waitModern(SprDb* spr_db); // Added
    void free();
    void freeModern(SprDb* spr_db); // Added
    int32_t getTexNum() const;
    int32_t getSprNum() const;
    bool getReady() const;
    int32_t getReference() const;
    const char* getFileName() const;
    const texture* getTexture(SprId in_id) const;
    const spr::SprArgs::Rect getRect(SprId in_id) const;
    uint32_t getAttr(SprId in_id) const;
    SCREEN_MODE getScreen(SprId in_id) const;
    const char* getName(SprId in_id) const;
    bool readTable();
    bool readTableModern(const void* data, size_t size); // Added
    bool readTexHeader();
    bool readTexHeaderModern(const void* data, size_t size); // Added
    bool freeTex();
};

struct sprite_vertex_data_uv0 {
    vec3 pos;
    color4u8 color;
};

struct sprite_vertex_data_uv1 {
    vec3 pos;
    color4u8 color;
    vec2 uv;
};

struct sprite_vertex_data_uv2 {
    vec3 pos;
    color4u8 color;
    vec2 uv[2];
};

struct sprite_draw_param_attrib_member {
    uint32_t primitive : 4;
    uint32_t enable_blend : 1;
    uint32_t blend : 3;
    uint32_t sampler : 2;
    uint32_t vao : 2;
};

union sprite_draw_param_attrib {
    sprite_draw_param_attrib_member m;
    uint32_t w;
};

struct sprite_draw_param {
    union {
        GLint first;
        struct {
            GLuint start;
            GLuint end;
        };
    };
    GLsizei count;
    GLintptr offset;
    const texture* texs[2];
    int32_t shader_name;
    sprite_draw_param_attrib attrib;
    vec3 vtx[4];
};

namespace spr {
    // Own stuff
    struct RenderData {
        std::vector<sprite_draw_param> draw_param_buffer;
        std::vector<uint8_t> vertex_buffer;
        std::vector<uint32_t> index_buffer;
        GLuint vao[3];
        GL::ArrayBuffer vbo;
        size_t vbo_vertex_count;
        GL::ElementArrayBuffer ebo;
        size_t ebo_index_count;

        RenderData();
        ~RenderData();

        template <typename T>
        size_t AddData(T*& data, size_t nb_quad);
        void Clear();
        void Update();
    };
    
    struct SprArgsDraw {
        int32_t num_texture;
        int32_t blend;
        const texture* texs[4];
        int32_t shader_name;
        spr::SprKind kind;
    };

    struct SprMgr {
        struct Projection {
            float_t aspect;

            Projection();
        };

        struct ViewPort {
            SCREEN_MODE screen;
            rectangle rect;

            ViewPort();
        };

        std::map<uint32_t, SPRSET> sets;
        std::list<SprArgs> obj_list[SPR_TARGET_MAX][SPR_LAYER_MAX][SPR_PRIO_MAX];
        Projection projection_list[SPR_LAYER_MAX];
        ViewPort view_list[SPR_LAYER_MAX];
        SPR_TARGET target;
        mat4 view_projection;
        mat4 mat;
        SCREEN_MODE screen_mode_back;

        uint32_t set_counter;

        static SprMgr* p_sprmgr;

        SprMgr();
        ~SprMgr();

    public:
        const SPRSET* find(uint32_t id);
        void init();
        void init(const SprDb* spr_db); // Added
        void dest();
        void dest(const SprDb* spr_db); // Added
        bool ready();
        void setProjection(SPR_LAYER layer, float_t aspect);
        void getViewport(SCREEN_MODE& screen, rectangle& rect, SPR_LAYER layer) const;
        void setViewport(SPR_LAYER layer, SCREEN_MODE mode, const rectangle rect);
        void setTarget(SPR_TARGET in_target);
        SPR_TARGET getTarget();
        void flush(render_data_context& rend_data_ctx,
            SPR_TARGET in_target, bool in_scale, texture* overlay_tex, const mat4& vp);
        void refresh();
        void request(uint32_t id, const char* file, const std::string mdata_dir, void* data);
        void requestModern(uint32_t id, uint32_t sethash, void* data, SprDb* spr_db); // Added
        bool wait(uint32_t id);
        bool waitModern(uint32_t id, SprDb* spr_db); // Added
        void free(uint32_t id);
        void freeModern(uint32_t id, SprDb* spr_db); // Added
        int32_t getTexNum(uint32_t id);
        int32_t getSprNum(uint32_t id);
        bool getReady(uint32_t id);
        const char* getName(SprId id);
        size_t getObjListCount(SPR_TARGET in_target);
        SprArgs::Rect getRect(SprId id);
        uint32_t getAttr(SprId id);
        SCREEN_MODE getScreen(SprId id);
        const texture* getTexture(SprId id);
        SprArgs* put(const SprArgs& args, const SprDb* spr_db);

        uint32_t addSet(); // Added
        void resetTarget();

        void postFlush(); // Added
        void preFlush(); // Added

        inline static SprMgr& sprmgr() {
            return *p_sprmgr;
        }

    private:
        static void begin_render(render_data_context& rend_data_ctx);
        static void end_render(render_data_context& rend_data_ctx);
    };

    RenderData* render_data;

    SprMgr* SprMgr::p_sprmgr;

    const GLenum SprArgs::blend_table[spr::SPR_BLEND_MAX][4] = {
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE  },
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE  },
        { GL_SRC_ALPHA, GL_ONE,                 GL_ZERO, GL_ONE  },
        { GL_DST_COLOR, GL_ZERO,                GL_ZERO, GL_ONE  },
        { GL_ONE,       GL_ZERO,                GL_ONE,  GL_ZERO },
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE  },
    };

    static bool use_quad_line = true;
}

#if BREAK_SPRITE_VERTEX_LIMIT
static size_t string_quad_max_count = 0x2000;

static spr::SprArgs::Quad* string_quad;
#else
const size_t string_quad_max_count = 0x2000;

static spr::SprArgs::Quad string_quad[string_quad_max_count];
#endif
static size_t string_quad_idx;

extern render_context* rctx_ptr;

namespace spr {
    SprArgs::SprArgs() : kind(), attr(), blend(), target(), layer(), prio(),
        screen_trans(), screen_scale(), tex(), shader_name(),
        sprite_draw_param_index(), quad(), nb_quad(), flags(), next() {
        init();
    }

    // 0x14063A110
    void SprArgs::draw(render_data_context& rend_data_ctx, const mat4& mat,
        int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max, texture* overlay_tex) {
        drawObject(rend_data_ctx, mat, x_min, y_min, x_max, y_max, overlay_tex);
    }

    // Inlined
    inline SprArgs::Quad* SprArgs::getQuadArgs() {
#if BREAK_SPRITE_VERTEX_LIMIT
        return string_quad + quad;
#else
        return quad;
#endif
    }

    // 0x14063CA10
    void SprArgs::init() {
        kind = SPR_KIND_NORMAL;
        id = {};
        color = 0xFFFFFFFF;
        attr = (SPR_ATTR)0;
        blend = SPR_BLEND_DEFAULT;
        target = SPR_TARGET_DEFAULT;
        layer = SPR_LAYER_DEFAULT;
        prio = SPR_PRIO_DEFAULT;
        screen_trans = SCREEN_MODE_HD;
        screen_scale = SCREEN_MODE_HD;
        anchor = 0.0f;
        trans = 0.0f;
        scale = 1.0f;
        rot = 0.0f;
        slant = 0.0f;
        shear = 0.0f;
        matrix = mat4_identity;
        tex = 0;
        shader_name = SHADER_FT_FFP;
        sprite_draw_param_index = -1;
#if BREAK_SPRITE_VERTEX_LIMIT
        quad = -1;
#else
        quad = 0;
#endif
        nb_quad = 0;
        flags = (SprArgs::Flags)0;
        size = 0.0f;
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.width = 1.0f;
        rect.height = 1.0f;
        next = 0;
    }

    // 0x14063EFE0
    void SprArgs::setChild(SprArgs* args) {
        SprArgs* _args = this;
        while (_args->next)
            _args = _args->next;
        _args->next = args;
        args->kind = SPR_KIND_MULTI;
    }

    // 0x14063F040
    void SprArgs::setQuadArgs(const SprArgs::Quad* quad_arg, size_t len) {
#if BREAK_SPRITE_VERTEX_LIMIT
        if (string_quad_idx + len >= string_quad_max_count) {
            while (string_quad_idx + len >= string_quad_max_count)
                string_quad_max_count *= 2;

            spr::SprArgs::Quad* _string_quad = new spr::SprArgs::Quad[string_quad_max_count];
            memmove(_string_quad, string_quad, sizeof(SprArgs::Quad) * string_quad_idx);
            delete[] string_quad;
            string_quad = _string_quad;
        }
#else
        if (string_quad_idx + len >= string_quad_max_count)
            return;
#endif

        this->nb_quad = len;
#if BREAK_SPRITE_VERTEX_LIMIT
        this->quad = string_quad_idx;
        memmove(string_quad + this->quad, quad_arg, sizeof(SprArgs::Quad) * len);
#else
        this->quad = &string_quad[string_quad_idx];
        memmove(this->quad, quad_arg, sizeof(SprArgs::Quad) * len);
#endif
        string_quad_idx += len;
    }

    // 0x14063F0E0
    void SprArgs::setSize(vec2 in_size) {
        if (flags & SPRITE_SIZE)
            return;

        enum_or(flags, SPRITE_SIZE);
        size.x = in_size.x;
        size.y = in_size.y;
        size.z = 0.0f;
    }

    // 0x14063F0A0
    void SprArgs::setRect(float_t x, float_t y, float_t width, float_t height) {
        if (flags & TEXTURE_POS_SIZE)
            return;

        enum_or(flags, TEXTURE_POS_SIZE);
        rect.x = x;
        rect.y = y;
        rect.width = width;
        rect.height = height;
    }

    // Added
    void SprArgs::addDraw(SprArgs* args_array[0x100],
        const int32_t args_count, bool in_scale, SprArgsDraw& args_draw) {
        SprArgs& args = *args_array[0];
        if (args.kind == SPR_KIND_MULTI)
            return;

        const color4u8 color = args.color;

        if (in_scale)
            args.correctResolution();

        vec3 spr_vtx[4] = {};
        SprArgs::TexParam tex_param[4] = {};
        args.calcTextureVertex(spr_vtx, tex_param, in_scale);

        std::vector<sprite_draw_param>& draw_param_buffer = render_data->draw_param_buffer;
        std::vector<uint32_t>& index_buffer = render_data->index_buffer;

        draw_param_buffer.push_back({});
        sprite_draw_param& draw_param = draw_param_buffer.back();

        if (args.attr & M_SPR_ATTR_IGNORE_ALPHA) {
            draw_param.attrib.m.enable_blend = 0;
            draw_param.attrib.m.blend = 0;
        }
        else {
            draw_param.attrib.m.enable_blend = 1;
            draw_param.attrib.m.blend = args_draw.blend;
        }

        draw_param.shader_name = args_draw.shader_name;
        if (draw_param.shader_name == SHADER_FT_FFP)
            draw_param.shader_name = SHADER_FT_SPRITE;

        switch (args_draw.num_texture) {
        case 0:
            draw_param.attrib.m.vao = 0;
            switch (args_draw.kind) {
            case SPR_KIND_LINE: {
                sprite_vertex_data_uv0* vtx_data = 0;
                size_t count = 2ULL * args_count;
                size_t first = render_data->AddData(vtx_data, count);

                draw_param.attrib.m.primitive = GL_LINES;
                draw_param.first = (GLint)first;
                draw_param.count = (GLint)count;

                vtx_data[0] = { spr_vtx[0], color };
                vtx_data[1] = { spr_vtx[2], color };
                vtx_data += 2;

                for (int32_t i = 1; i < args_count; i++) {
                    SprArgs& args = *args_array[i];

                    const color4u8 color = args.color;

                    if (in_scale)
                        args.correctResolution();

                    args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                    vtx_data[0] = { spr_vtx[0], color };
                    vtx_data[1] = { spr_vtx[2], color };
                    vtx_data += 2;
                }
            } break;
            case SPR_KIND_RECT: {
                sprite_vertex_data_uv0* vtx_data = 0;
                size_t vtx_count = 4ULL * args_count;
                size_t idx_count = 6ULL * args_count;
                size_t start = render_data->AddData(vtx_data, vtx_count);
                index_buffer.reserve(idx_count);

                draw_param.attrib.m.primitive = GL_TRIANGLES;
                draw_param.start = (GLuint)start;
                draw_param.end = (GLuint)(start + vtx_count - 1);
                draw_param.count = (GLsizei)idx_count;
                draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                vtx_data[0] = { spr_vtx[0], color }; // LB
                vtx_data[1] = { spr_vtx[1], color }; // LT
                vtx_data[2] = { spr_vtx[2], color }; // RT
                vtx_data[3] = { spr_vtx[3], color }; // RB
                vtx_data += 4;

                index_buffer.push_back((uint32_t)(start + 0)); // LB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 2)); // RT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                start += 4;

                for (int32_t i = 1; i < args_count; i++) {
                    SprArgs& args = *args_array[i];

                    const color4u8 color = args.color;

                    if (in_scale)
                        args.correctResolution();

                    args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                    vtx_data[0] = { spr_vtx[0], color }; // LB
                    vtx_data[1] = { spr_vtx[1], color }; // LT
                    vtx_data[2] = { spr_vtx[2], color }; // RT
                    vtx_data[3] = { spr_vtx[3], color }; // RB
                    vtx_data += 4;

                    index_buffer.push_back((uint32_t)(start + 0)); // LB
                    index_buffer.push_back((uint32_t)(start + 1)); // LT
                    index_buffer.push_back((uint32_t)(start + 3)); // RB
                    index_buffer.push_back((uint32_t)(start + 1)); // LT
                    index_buffer.push_back((uint32_t)(start + 2)); // RT
                    index_buffer.push_back((uint32_t)(start + 3)); // RB
                    start += 4;
                }
            } break;
            case SPR_KIND_LINE_BOX: {
                sprite_vertex_data_uv0* vtx_data = 0;
                size_t count = 5;
                size_t first = render_data->AddData(vtx_data, count);

                draw_param.attrib.m.primitive = GL_LINE_STRIP;
                draw_param.first = (GLint)first;
                draw_param.count = (GLsizei)count;

                vtx_data[0] = { spr_vtx[0], color };
                vtx_data[1] = { spr_vtx[1], color };
                vtx_data[2] = { spr_vtx[2], color };
                vtx_data[3] = { spr_vtx[3], color };
                vtx_data[4] = { spr_vtx[0], color };

            } break;
            case SPR_KIND_LINES: {
                if (args.nb_quad) {
                    sprite_vertex_data_uv0* vtx_data = 0;
                    size_t count = args.nb_quad & ~0x01;
                    size_t first = render_data->AddData(vtx_data, count);

                    draw_param.attrib.m.primitive = GL_LINES;
                    draw_param.first = (GLint)first;
                    draw_param.count = (GLsizei)count;

                    SprArgs::Quad* vtx = args.getQuadArgs();
                    for (size_t i = args.nb_quad / 2; i; i--, vtx += 2, vtx_data += 2) {
                        vtx_data[0] = { vtx[0].pos, color };
                        vtx_data[1] = { vtx[1].pos, color };
                    }
                }
                else {
                    sprite_vertex_data_uv0* vtx_data = 0;
                    size_t count = 2;
                    size_t first = render_data->AddData(vtx_data, count);

                    draw_param.attrib.m.primitive = GL_LINES;
                    draw_param.first = (GLint)first;
                    draw_param.count = (GLsizei)count;

                    vtx_data[0] = { spr_vtx[0], color };
                    vtx_data[1] = { spr_vtx[2], color };
                }

                for (int32_t i = 1; i < args_count; i++) {
                    SprArgs& args = *args_array[i];

                    const color4u8 color = args.color;

                    if (in_scale)
                        args.correctResolution();

                    args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                    if (args.nb_quad) {
                        sprite_vertex_data_uv0* vtx_data = 0;
                        size_t count = args.nb_quad & ~0x01;
                        render_data->AddData(vtx_data, count);

                        draw_param.count += (GLsizei)count;

                        SprArgs::Quad* vtx = args.getQuadArgs();
                        for (size_t i = args.nb_quad / 2; i; i--, vtx += 2, vtx_data += 2) {
                            vtx_data[0] = { vtx[0].pos, color };
                            vtx_data[1] = { vtx[1].pos, color };
                        }
                    }
                    else {
                        sprite_vertex_data_uv0* vtx_data = 0;
                        size_t count = 2;
                        render_data->AddData(vtx_data, count);

                        draw_param.count += (GLsizei)count;

                        vtx_data[0] = { spr_vtx[0], color };
                        vtx_data[1] = { spr_vtx[2], color };

                    }
                }
            } break;
            case SPR_KIND_POLY_LINE: {
                sprite_vertex_data_uv0* vtx_data = 0;
                size_t count = args.nb_quad;
                size_t first = render_data->AddData(vtx_data, count);

                draw_param.attrib.m.primitive = GL_LINE_STRIP;
                draw_param.first = (GLint)first;
                draw_param.count = (GLsizei)count;

                SprArgs::Quad* vtx = args.getQuadArgs();
                for (size_t i = args.nb_quad; i; i--, vtx++, vtx_data++)
                    *vtx_data = { vtx->pos, color };
            } break;
            case SPR_KIND_STRIP: {
                sprite_vertex_data_uv0* vtx_data = 0;
                size_t count = args.nb_quad;
                size_t first = render_data->AddData(vtx_data, count);

                draw_param.attrib.m.primitive = GL_TRIANGLE_STRIP;
                draw_param.first = (GLint)first;
                draw_param.count = (GLsizei)count;

                SprArgs::Quad* vtx = args.getQuadArgs();
                for (size_t i = args.nb_quad; i; i--, vtx++, vtx_data++)
                    *vtx_data = { vtx->pos, vtx->color };

            } break;
            }
            break;
        case 1:
            draw_param.texs[0] = tex_param[0].texture;
            draw_param.attrib.m.sampler = 0;
            draw_param.attrib.m.vao = 1;

            if (args.nb_quad) {
                if (args_draw.kind == SPR_KIND_STRIP) {
                    draw_param.attrib.m.sampler = 1;

                    sprite_vertex_data_uv1* vtx_data = 0;
                    size_t count = args.nb_quad;
                    size_t first = render_data->AddData(vtx_data, count);

                    draw_param.attrib.m.primitive = GL_TRIANGLE_STRIP;
                    draw_param.first = (GLint)first;
                    draw_param.count = (GLsizei)count;

                    SprArgs::Quad* vtx = args.getQuadArgs();
                    for (size_t i = args.nb_quad; i; i--, vtx++, vtx_data++)
                        *vtx_data = { vtx->pos, vtx->color, vtx->uv };
                }
                else {
                    sprite_vertex_data_uv1* vtx_data = 0;
                    size_t vtx_count = args.nb_quad;
                    size_t idx_count = args.nb_quad / 4 * 6;
                    size_t start = render_data->AddData(vtx_data, vtx_count);
                    index_buffer.reserve(idx_count);

                    draw_param.attrib.m.primitive = GL_TRIANGLES;
                    draw_param.start = (GLuint)start;
                    draw_param.end = (GLuint)(start + vtx_count - 1);
                    draw_param.count = (GLsizei)idx_count;
                    draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                    SprArgs::Quad* vtx = args.getQuadArgs();
                    for (size_t i = args.nb_quad / 4, j = 0; i; i--, j += 4, vtx += 4) {
                        vtx_data[0] = { vtx[0].pos, vtx[0].color, vtx[0].uv }; // LB
                        vtx_data[1] = { vtx[1].pos, vtx[1].color, vtx[1].uv }; // LT
                        vtx_data[2] = { vtx[2].pos, vtx[2].color, vtx[2].uv }; // RT
                        vtx_data[3] = { vtx[3].pos, vtx[3].color, vtx[3].uv }; // RB
                        vtx_data += 4;

                        index_buffer.push_back((uint32_t)(start + j + 0)); // LB
                        index_buffer.push_back((uint32_t)(start + j + 1)); // LT
                        index_buffer.push_back((uint32_t)(start + j + 3)); // RB
                        index_buffer.push_back((uint32_t)(start + j + 1)); // LT
                        index_buffer.push_back((uint32_t)(start + j + 2)); // RT
                        index_buffer.push_back((uint32_t)(start + j + 3)); // RB
                    }
                    start += args.nb_quad;

                    for (int32_t i = 1; i < args_count; i++) {
                        SprArgs& args = *args_array[i];

                        size_t vtx_count = args.nb_quad;
                        size_t idx_count = args.nb_quad / 4 * 6;

                        draw_param.end += (GLuint)vtx_count;
                        draw_param.count += (GLsizei)idx_count;

                        render_data->AddData(vtx_data, vtx_count);
                        index_buffer.reserve(idx_count);

                        if (in_scale)
                            args.correctResolution();

                        args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                        SprArgs::Quad* vtx = args.getQuadArgs();
                        for (size_t i = args.nb_quad / 4, j = 0; i; i--, j += 4, vtx += 4) {

                            vtx_data[0] = { vtx[0].pos, vtx[0].color, vtx[0].uv }; // LB
                            vtx_data[1] = { vtx[1].pos, vtx[1].color, vtx[1].uv }; // LT
                            vtx_data[2] = { vtx[2].pos, vtx[2].color, vtx[2].uv }; // RT
                            vtx_data[3] = { vtx[3].pos, vtx[3].color, vtx[3].uv }; // RB
                            vtx_data += 4;

                            index_buffer.push_back((uint32_t)(start + j + 0)); // LB
                            index_buffer.push_back((uint32_t)(start + j + 1)); // LT
                            index_buffer.push_back((uint32_t)(start + j + 3)); // RB
                            index_buffer.push_back((uint32_t)(start + j + 1)); // LT
                            index_buffer.push_back((uint32_t)(start + j + 2)); // RT
                            index_buffer.push_back((uint32_t)(start + j + 3)); // RB
                        }
                        start += args.nb_quad;
                    }
                }
            }
            else {
                sprite_vertex_data_uv1* vtx_data = 0;
                size_t vtx_count = 4ULL * args_count;
                size_t idx_count = 6ULL * args_count;
                size_t start = render_data->AddData(vtx_data, vtx_count);
                index_buffer.reserve(idx_count);

                draw_param.attrib.m.primitive = GL_TRIANGLES;
                draw_param.start = (GLuint)start;
                draw_param.end = (GLuint)(start + vtx_count - 1);
                draw_param.count = (GLsizei)idx_count;
                draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

                vtx_data[0] = { spr_vtx[0], color, tex_param[0].uv[0] }; // LB
                vtx_data[1] = { spr_vtx[1], color, tex_param[0].uv[1] }; // LT
                vtx_data[2] = { spr_vtx[2], color, tex_param[0].uv[2] }; // RT
                vtx_data[3] = { spr_vtx[3], color, tex_param[0].uv[3] }; // RB
                vtx_data += 4;

                index_buffer.push_back((uint32_t)(start + 0)); // LB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 2)); // RT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                start += 4;

                for (int32_t i = 1; i < args_count; i++) {
                    SprArgs& args = *args_array[i];

                    const color4u8 color = args.color;

                    if (in_scale)
                        args.correctResolution();

                    args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                    vtx_data[0] = { spr_vtx[0], color, tex_param[0].uv[0] }; // LB
                    vtx_data[1] = { spr_vtx[1], color, tex_param[0].uv[1] }; // LT
                    vtx_data[2] = { spr_vtx[2], color, tex_param[0].uv[2] }; // RT
                    vtx_data[3] = { spr_vtx[3], color, tex_param[0].uv[3] }; // RB
                    vtx_data += 4;

                    index_buffer.push_back((uint32_t)(start + 0)); // LB
                    index_buffer.push_back((uint32_t)(start + 1)); // LT
                    index_buffer.push_back((uint32_t)(start + 3)); // RB
                    index_buffer.push_back((uint32_t)(start + 1)); // LT
                    index_buffer.push_back((uint32_t)(start + 2)); // RT
                    index_buffer.push_back((uint32_t)(start + 3)); // RB
                    start += 4;
                }
            }
            break;
        case 2: {
            draw_param.texs[0] = tex_param[0].texture;
            draw_param.texs[1] = tex_param[1].texture;
            draw_param.attrib.m.sampler = 2;
            draw_param.attrib.m.vao = 2;

            sprite_vertex_data_uv2* vtx_data = 0;
            size_t vtx_count = 4ULL * args_count;
            size_t idx_count = 6ULL * args_count;
            size_t start = render_data->AddData(vtx_data, vtx_count);
            index_buffer.reserve(idx_count);

            draw_param.attrib.m.primitive = GL_TRIANGLES;
            draw_param.start = (GLuint)start;
            draw_param.end = (GLuint)(start + vtx_count - 1);
            draw_param.count = (GLsizei)idx_count;
            draw_param.offset = (GLintptr)(index_buffer.size() * sizeof(uint32_t));

            vtx_data[0] = { spr_vtx[0], color,
                tex_param[0].uv[0], tex_param[1].uv[0] }; // LB
            vtx_data[1] = { spr_vtx[1], color,
                tex_param[0].uv[1], tex_param[1].uv[1] }; // LT
            vtx_data[2] = { spr_vtx[2], color,
                tex_param[0].uv[2], tex_param[1].uv[2] }; // RT
            vtx_data[3] = { spr_vtx[3], color,
                tex_param[0].uv[3], tex_param[1].uv[3] }; // RB
            vtx_data += 4;

            index_buffer.push_back((uint32_t)(start + 0)); // LB
            index_buffer.push_back((uint32_t)(start + 1)); // LT
            index_buffer.push_back((uint32_t)(start + 3)); // RB
            index_buffer.push_back((uint32_t)(start + 1)); // LT
            index_buffer.push_back((uint32_t)(start + 2)); // RT
            index_buffer.push_back((uint32_t)(start + 3)); // RB
            start += 4;

            for (int32_t i = 1; i < args_count; i++) {
                SprArgs& args = *args_array[i];

                const color4u8 color = args.color;

                if (in_scale)
                    args.correctResolution();

                args.calcTextureVertex(spr_vtx, tex_param, in_scale);

                vtx_data[0] = { spr_vtx[0], color,
                    tex_param[0].uv[0], tex_param[1].uv[0] }; // LB
                vtx_data[1] = { spr_vtx[1], color,
                    tex_param[0].uv[1], tex_param[1].uv[1] }; // LT
                vtx_data[2] = { spr_vtx[2], color,
                    tex_param[0].uv[2], tex_param[1].uv[2] }; // RT
                vtx_data[3] = { spr_vtx[3], color,
                    tex_param[0].uv[3], tex_param[1].uv[3] }; // RB
                vtx_data += 4;

                index_buffer.push_back((uint32_t)(start + 0)); // LB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                index_buffer.push_back((uint32_t)(start + 1)); // LT
                index_buffer.push_back((uint32_t)(start + 2)); // RT
                index_buffer.push_back((uint32_t)(start + 3)); // RB
                start += 4;
            }
        } break;
        }

        if (!draw_param.count) {
            draw_param_buffer.pop_back();

            args.sprite_draw_param_index = -1;
        }
        else {
            if (args_draw.blend == SPR_BLEND_OVERLAY) {
                draw_param.vtx[0] = spr_vtx[0];
                draw_param.vtx[1] = spr_vtx[1];
                draw_param.vtx[2] = spr_vtx[2];
                draw_param.vtx[3] = spr_vtx[3];
            }

            args.sprite_draw_param_index = (int32_t)(draw_param_buffer.size() - 1);
        }

        for (int32_t i = 1; i < args_count; i++)
            args_array[i]->sprite_draw_param_index = -1;
    }

    // 0x140638CD0
    int32_t SprArgs::calcTexture(SprArgs::TexParam* param, vec3 vertex[4], bool in_scale) {
        SprArgs* args = this;
        int32_t tex_param_count = 0;
        const texture* tex = args->tex;
        while (args) {
            param->texture = args->tex;

            float_t width = (float_t)args->tex->width;
            float_t height = (float_t)args->tex->height;

            float_t u_scale = 1.0f / width;
            float_t v_scale = 1.0f / height;

            if (!args->nb_quad) {
                vec2 uv00;
                vec2 uv01;
                vec2 uv10;
                vec2 uv11;
                if (args->kind == SPR_KIND_MULTI) {
                    vec3 v42[4];
                    mat4 mat;
                    args->calcVertex(v42, &mat, in_scale);
                    mat4_invert(&mat, &mat);
                    mat4_transform_point(&mat, &vertex[0], &v42[0]);
                    mat4_transform_point(&mat, &vertex[1], &v42[1]);
                    mat4_transform_point(&mat, &vertex[2], &v42[2]);
                    mat4_transform_point(&mat, &vertex[3], &v42[3]);
                    uv00 = *(vec2*)&v42[0].x;
                    uv01 = *(vec2*)&v42[1].x;
                    uv10 = *(vec2*)&v42[2].x;
                    uv11 = *(vec2*)&v42[3].x;
                }
                else {
                    uv00.x = 0.0f;
                    uv00.y = 0.0f;
                    uv01.x = 0.0f;
                    uv01.y = args->rect.height;
                    uv10.x = args->rect.width;
                    uv10.y = args->rect.height;
                    uv11.x = args->rect.width;
                    uv11.y = 0.0f;
                }

                const float_t rect_x = args->rect.x;
                const float_t rect_y = args->rect.y;
                param->uv[0].x = (uv00.x + rect_x) * u_scale;
                param->uv[0].y = (height - (uv00.y + rect_y)) * v_scale;
                param->uv[1].x = (uv01.x + rect_x) * u_scale;
                param->uv[1].y = (height - (uv01.y + rect_y)) * v_scale;
                param->uv[2].x = (uv10.x + rect_x) * u_scale;
                param->uv[2].y = (height - (uv10.y + rect_y)) * v_scale;
                param->uv[3].x = (uv11.x + rect_x) * u_scale;
                param->uv[3].y = (height - (uv11.y + rect_y)) * v_scale;
            }
            else if (in_scale) {
                SprArgs::Quad* quad = args->getQuadArgs();
                for (size_t i = args->nb_quad; i; i--, quad++) {
                    quad->uv.x = quad->uv.x * u_scale;
                    quad->uv.y = (height - quad->uv.y) * v_scale;
                }
            }

            args = args->next;
            param++;
            tex_param_count++;
        }
        return tex_param_count;
    }

    // Added
    int32_t SprArgs::calcTextureVertex(vec3 vtx[4], SprArgs::TexParam* param, bool in_scale) {
        vec3 _vtx[4] = {};
        if (flags & SPRITE_SIZE)
            calcVertex(_vtx, 0, in_scale);

        int32_t tex_param_count = 0;
        if ((flags & TEXTURE_POS_SIZE) && tex)
            tex_param_count = calcTexture(param, _vtx, in_scale);
        vtx[0] = _vtx[0];
        vtx[1] = _vtx[1];
        vtx[2] = _vtx[2];
        vtx[3] = _vtx[3];
        return tex_param_count;
    }

    // 0x140639150
    void SprArgs::calcVertex(vec3 vertex[4], mat4* trans_matrix, bool in_scale) {
        vertex[0].x = 0.0f;
        vertex[0].y = 0.0f;
        vertex[0].z = 0.0f;
        vertex[1].x = 0.0f;
        vertex[1].y = size.y;
        vertex[1].z = 0.0f;
        vertex[2].x = size.x;
        vertex[2].y = size.y;
        vertex[2].z = 0.0f;
        vertex[3].x = size.x;
        vertex[3].y = 0.0f;
        vertex[3].z = 0.0f;

        mat4 m;
        SPR_ATTR attr = this->attr;
        if (!(attr & M_SPR_ATTR_NOTRANSFORM)) {
            if (attr & M_SPR_ATTR_CTR_ALL) {
                if (attr & M_SPR_ATTR_CTR_LT)
                    anchor = 0.0f;
                else if (attr & M_SPR_ATTR_CTR_LC) {
                    anchor.x = 0.0f;
                    anchor.y = size.y * 0.5f;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_LB) {
                    anchor.x = 0.0f;
                    anchor.y = size.y;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_CT) {
                    anchor.x = size.x * 0.5f;
                    anchor.y = 0.0f;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_CC) {
                    anchor.x = size.x * 0.5f;
                    anchor.y = size.y * 0.5f;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_CB) {
                    anchor.x = size.x * 0.5f;
                    anchor.y = size.y;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_RT) {
                    anchor.x = size.x;
                    anchor.y = 0.0f;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_RC) {
                    anchor.x = size.x;
                    anchor.y = size.y * 0.5f;
                    anchor.z = 0.0f;
                }
                else if (attr & M_SPR_ATTR_CTR_RB) {
                    anchor.x = size.x;
                    anchor.y = size.y;
                    anchor.z = 0.0f;
                }
            }

            if (in_scale) {
                if (attr & M_SPR_ATTR_FLIP_H) {
                    scale.x = -scale.x;
                    rot.z = -rot.z;
                }
                if (attr & M_SPR_ATTR_FLIP_V) {
                    scale.y = -scale.y;
                    rot.z = -rot.z;
                }
            }

            if (fabsf(slant) > 0.000001f) {
                float_t skew_width = tanf(slant) * size.y * 0.5f;
                vertex[0].x = vertex[0].x + skew_width;
                vertex[1].x = vertex[1].x - skew_width;
                vertex[2].x = vertex[2].x - skew_width;
                vertex[3].x = vertex[3].x + skew_width;
            }

            if (fabsf(shear) > 0.000001f) {
                float_t skew_height = tanf(shear) * size.x * 0.5f;
                vertex[0].y = vertex[0].y - skew_height;
                vertex[1].y = vertex[1].y - skew_height;
                vertex[2].y = vertex[2].y + skew_height;
                vertex[3].y = vertex[3].y + skew_height;
            }

            mat4_translate(&trans, &m);
            if (fabsf(rot.x) > 0.000001f)
                mat4_mul_rotate_x(&m, rot.x, &m);
            if (fabsf(rot.y) > 0.000001f)
                mat4_mul_rotate_y(&m, rot.y, &m);
            if (fabsf(rot.z) > 0.000001f)
                mat4_mul_rotate_z(&m, rot.z, &m);
            mat4_scale_rot(&m, &scale, &m);
            const vec3 anchor = -anchor;
            mat4_mul_translate(&m, &anchor, &m);
        }
        else {
            mat4_translate(&trans, &m);
            mat4_scale_rot(&m, &scale, &m);
        }

        mat4_mul(&matrix, &m, &m);

        mat4_transform_point(&m, &vertex[0], &vertex[0]);
        mat4_transform_point(&m, &vertex[1], &vertex[1]);
        mat4_transform_point(&m, &vertex[2], &vertex[2]);
        mat4_transform_point(&m, &vertex[3], &vertex[3]);

        if (trans_matrix)
            *trans_matrix = m;
    }

    // 0x140639CC0
    void SprArgs::correctResolution() {
        SprArgs* args = this;
        SPR_TARGET target = args->target;
        SCREEN_MODE screen_mode = get_screen_param().mode;
        while (args) {
            screen_mode_scale_data data(args->screen_trans, screen_mode);
            if (args->screen_trans != SCREEN_MODE_MAX
                && args->screen_trans != screen_mode && (target < SPR_TARGET_FRONT_3D_SURF || target > SPR_TARGET_BACK)) {
                float_t scale_x = data.scale.x;
                float_t scale_y = data.scale.y;
                float_t src_res_x = data.src_res.x;
                float_t src_res_y = data.src_res.y;
                float_t dst_res_x = data.dst_res.x;
                float_t dst_res_y = data.dst_res.y;
                args->trans.x = (args->trans.x - src_res_x) * scale_x + src_res_x;
                args->trans.y = (args->trans.y - src_res_y) * scale_y + src_res_y;
                args->trans.z = args->trans.z * scale_y;

                SprArgs::Quad* vtx = args->getQuadArgs();
                for (size_t i = args->nb_quad; i; i--, vtx++) {
                    vtx->pos.x = (vtx->pos.x - src_res_x) * scale_x + dst_res_x;
                    vtx->pos.y = (vtx->pos.y - src_res_y) * scale_y + dst_res_y;
                }
            }

            if (args->screen_scale != SCREEN_MODE_MAX) {
                vec2 scale = get_screen_conv_scale(screen_mode, args->screen_scale);
                args->scale.x = scale.x * args->scale.x;
                args->scale.y = scale.y * args->scale.y;
            }
            args = args->next;
        }
    }

    // Inlined
    inline void SprArgs::drawObject(render_data_context& rend_data_ctx, const mat4& mat,
        int32_t x_min, int32_t y_min, int32_t x_max, int32_t y_max, texture* overlay_tex) {
        sprite_draw_param& draw_param = render_data->draw_param_buffer.data()[sprite_draw_param_index];

        int32_t combiner = 0;
        if (draw_param.attrib.m.enable_blend) {
            if (draw_param.attrib.m.blend == SPR_BLEND_OVERLAY) {
                rend_data_ctx.state.active_bind_texture_2d(7, overlay_tex->glid);
                drawObjectCopyOverlayTex(rend_data_ctx,
                    mat, draw_param.vtx, x_min, y_min, x_max, y_max);
                combiner = 2;
            }
            else
                combiner = draw_param.attrib.m.blend == SPR_BLEND_MODULATE ? 1 : 0;

            const GLenum* blend = blend_table[draw_param.attrib.m.blend];

            rend_data_ctx.state.enable_blend();
            rend_data_ctx.state.set_blend_func_separate(blend[0], blend[1], blend[2], blend[3]);
        }
        else
            rend_data_ctx.state.disable_blend();

        if (draw_param.shader_name == SHADER_FT_SPRITE) {
            int32_t tex_0_type = 0;
            int32_t tex_1_type = 0;
            if (draw_param.texs[0]) {
                if (draw_param.texs[0]->internal_format == GL_COMPRESSED_RED_RGTC1)
                    tex_0_type = 3;
                else if (draw_param.texs[0]->internal_format == GL_COMPRESSED_RG_RGTC2)
                    tex_0_type = 2;
                else
                    tex_0_type = 1;

                if (draw_param.texs[1]) {
                    if (draw_param.texs[1]->internal_format == GL_COMPRESSED_RED_RGTC1)
                        tex_1_type = 3;
                    else if (draw_param.texs[1]->internal_format == GL_COMPRESSED_RG_RGTC2)
                        tex_1_type = 2;
                    else
                        tex_1_type = 1;
                }
            }

            rend_data_ctx.shader_flags.arr[U_TEX_0_TYPE] = tex_0_type;
            rend_data_ctx.shader_flags.arr[U_TEX_1_TYPE] = tex_1_type;
            rend_data_ctx.shader_flags.arr[U_COMBINER] = combiner;
        }

        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, draw_param.shader_name);
        if (draw_param.texs[0]) {
            rend_data_ctx.state.active_bind_texture_2d(0, draw_param.texs[0]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx_ptr->sprite_samplers[draw_param.attrib.m.sampler]);

            if (draw_param.texs[1]) {
                rend_data_ctx.state.active_bind_texture_2d(1, draw_param.texs[1]->glid);
                rend_data_ctx.state.bind_sampler(1, rctx_ptr->sprite_samplers[draw_param.attrib.m.sampler]);
            }
            else
                rend_data_ctx.state.active_bind_texture_2d(1, rctx_ptr->empty_texture_2d->glid);
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(0, rctx_ptr->empty_texture_2d->glid);
            rend_data_ctx.state.active_bind_texture_2d(1, rctx_ptr->empty_texture_2d->glid);
        }

        rend_data_ctx.state.bind_vertex_array(render_data->vao[draw_param.attrib.m.vao]);
        if (draw_param.attrib.m.primitive != GL_TRIANGLES)
            rend_data_ctx.state.draw_arrays(
                draw_param.attrib.m.primitive, draw_param.first, draw_param.count);
        else
            rend_data_ctx.state.draw_range_elements(
                draw_param.attrib.m.primitive, draw_param.start, draw_param.end,
                draw_param.count, GL_UNSIGNED_INT, (void*)draw_param.offset);
    }

    // Added
    void SprArgs::drawObjectCopyOverlayTex(render_data_context& rend_data_ctx, const mat4& mat,
        const vec3 vertex[4], int32_t over_x_min, int32_t over_y_min, int32_t over_x_max, int32_t over_y_max) {
        if (nb_quad || kind != SPR_KIND_NORMAL)
            return;

        mat4 mat_t;
        mat4_transpose(&mat, &mat_t);

        float_t x_min_coord = 0.0f;
        float_t y_min_coord = 0.0f;
        float_t x_max_coord = 0.0f;
        float_t y_max_coord = 0.0f;
        for (int32_t i = 0, j = 0; i < 4; i++) {
            vec4 v;
            *(vec3*)&v = vertex[i];
            v.w = 1.0f;

            const float_t w = vec4::dot(mat_t.row3, v);
            if (w * w <= 0.0001f)
                continue;

            const float_t x_coord = vec4::dot(mat_t.row0, v) * (1.0f / w);
            const float_t y_coord = vec4::dot(mat_t.row1, v) * (1.0f / w);
            if (j) {
                if (x_min_coord > x_coord)
                    x_min_coord = x_coord;
                else if (x_max_coord < x_coord)
                    x_max_coord = x_coord;

                if (y_min_coord > y_coord)
                    y_min_coord = y_coord;
                else if (y_max_coord < y_coord)
                    y_max_coord = y_coord;
            }
            else {
                x_min_coord = x_coord;
                x_max_coord = x_coord;
                y_min_coord = y_coord;
                y_max_coord = y_coord;
            }
            j++;
        }

        int32_t x_max = (int32_t)x_max_coord + 1;
        int32_t y_max = (int32_t)y_max_coord + 1;
        int32_t x_min = (int32_t)x_min_coord - 1;
        int32_t y_min = (int32_t)y_min_coord - 1;

        x_max = clamp_def(x_max, over_x_min, over_x_min + over_x_max - 1);
        y_max = clamp_def(y_max, over_y_min, over_y_min + over_y_max - 1);
        x_min = clamp_def(x_min, over_x_min, over_x_min + over_x_max - 1);
        y_min = clamp_def(y_min, over_y_min, over_y_min + over_y_max - 1);

        const int32_t width = x_max - x_min + 1;
        const int32_t height = y_max - y_min + 1;
        if (width * height > 0) {
            rend_data_ctx.state.active_texture(7);
            rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D,
                0, x_min, y_min, x_min, y_min, width, height);
        }
    }

    // 0x14063F860
    void dest() {
        SprMgr::sprmgr().dest();
    }

    // Added
    void dest(const SprDb* spr_db) {
        SprMgr::sprmgr().dest(spr_db);
    }

    // 0x14063F870
    void flush(render_data_context& rend_data_ctx,
        SPR_TARGET target, bool in_scale, texture* overlay_tex, const mat4& vp) {
        SprMgr::sprmgr().flush(rend_data_ctx, target, in_scale, overlay_tex, vp);
    }

    // 0x14063F890
    void free(const std::vector<uint32_t>& setids, const SprDb* spr_db) {
        for (uint32_t setid : setids)
            free(setid, spr_db);
    }

    // 0x14063F8D0
    void free(uint32_t setid, const SprDb* spr_db) {
        SprMgr::sprmgr().free(spr_db->getSetIdFromUid(setid));
    }

    // Added
    void freeModern(const std::vector<uint32_t>& sethashes, SprDb* spr_db) {
        for (uint32_t sethash : sethashes)
            freeModern(sethash, spr_db);
    }

    // Added
    void freeModern(uint32_t sethash, SprDb* spr_db) {
        SprMgr::sprmgr().freeModern(spr_db->getSetIdFromUid(sethash), spr_db);
    }

    // 0x14063F900
    const char* getName(uint32_t uid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getName(spr_db->getSprIdFromUid(uid));
    }

    // 0x14063FA90
    size_t getObjListCount(SPR_TARGET target) {
        return SprMgr::sprmgr().getObjListCount(target);
    }

    // 0x14063FAF0
    bool getReady(uint32_t setid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getReady(spr_db->getSetIdFromUid(setid));
    }

    // 0x14063F930
    const SprArgs::Rect getRect(uint32_t uid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getRect(spr_db->getSprIdFromUid(uid));
    }

    // 0x14063F9A0
    uint32_t getSprNum(uint32_t setid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getSprNum(spr_db->getSetIdFromUid(setid));
    }

    // 0x14063F9D0
    SPR_TARGET getTarget() {
        return SprMgr::sprmgr().getTarget();
    }

    // 0x14063F9E0
    uint32_t getTexNum(uint32_t setid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getTexNum(spr_db->getSetIdFromUid(setid));
    }

    // Inlined
    const texture* getTexture(uint32_t uid, const SprDb* spr_db) {
        return SprMgr::sprmgr().getTexture(spr_db->getSprIdFromUid(uid));
    }

    // 0x14063FA10
    void getViewport(SCREEN_MODE& screen, rectangle& rect, SPR_LAYER layer) {
        SprMgr::sprmgr().getViewport(screen, rect, layer);
    }

    // 0x14063FB20
    void init() {
        SprMgr::sprmgr().init();
    }

    // Added
    void init(const SprDb* spr_db) {
        SprMgr::sprmgr().init(spr_db);
    }

    // Missing
    void putGlutFont(SprArgs args, const SprDb* spr_db) {
        args.kind = SPR_KIND_GLUT;
        SprMgr::sprmgr().put(args, spr_db);
    }

    // Added
    void postFlush() {
        spr::SprMgr::sprmgr().postFlush();
    }

    // Added
    void preFlush() {
        spr::SprMgr::sprmgr().preFlush();
    }

    // 0x14063FC40
    SprArgs* putLine(vec2 p0, vec2 p1, SCREEN_MODE screen, SPR_PRIO prio, color4u8 color, SPR_LAYER layer) {
        SprArgs args;
        args.trans.x = p0.x;
        args.trans.y = p0.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_LINE;
        args.screen_trans = screen;
        args.screen_scale = screen;
        args.prio = prio;
        args.color = color;
        args.setSize({ p1.x - p0.x, p1.y - p0.y });
        enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);
        return SprMgr::sprmgr().put(args, 0);
    }

    // 0x14063FD60
    void putLineBox(rectangle rect, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer) {
        SprArgs args;
        args.trans.x = rect.pos.x;
        args.trans.y = rect.pos.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_LINE_BOX;
        args.screen_trans = screen;
        args.screen_scale = screen;
        args.prio = prio;
        args.color = color;
        args.setSize(rect.size);
        enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);
        SprMgr::sprmgr().put(args, 0);
    }

    // 0x14063FE60
    spr::SprArgs* putLines(const vec2* pos_arg, size_t len, SCREEN_MODE screen,
        spr::SPR_PRIO prio, color4u8 color, spr::SPR_LAYER layer) {
        if (use_quad_line) {
            spr::SprArgs args;
            args.kind = SPR_KIND_LINES;
            args.layer = layer;
            args.screen_trans = screen;
            args.screen_scale = screen;
            args.prio = prio;
            args.color = (color4u8_bgra)(color.b | ((color.g | ((color.r | (color.a << 8)) << 8)) << 8));
            enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);

            std::vector<spr::SprArgs::Quad> quads;
            for (size_t i = 0; i < len; i++) {
                spr::SprArgs::Quad quad;
                quad.pos.x = pos_arg[0].x;
                quad.pos.y = pos_arg[0].y;
                quads.push_back(quad);
                quad.pos.x = pos_arg[1].x;
                quad.pos.y = pos_arg[1].x;
                quads.push_back(quad);
                pos_arg += 2;
            }

            args.setQuadArgs(quads.data(), quads.size());
            SprMgr::sprmgr().put(args, 0);
        }
        else
            for (size_t i = 0; i < len; i++) {
                spr::SprArgs args;
                args.kind = SPR_KIND_LINES;
                args.layer = layer;
                args.screen_trans = screen;
                args.screen_scale = screen;
                args.prio = prio;
                args.color = color;
                enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);

                args.trans.x = pos_arg[0].x;
                args.trans.y = pos_arg[0].y;
                args.trans.z = 0.0f;
                args.setSize(pos_arg[1] - pos_arg[0]);
                SprMgr::sprmgr().put(args, 0);
                pos_arg += 2;
            }
        return 0;
    }

    // 0x140640590
    void putMask() {
        putRect({ 0.0f, 0.0f, 1280.0f, 64.0f }, SCREEN_MODE_WXGA, SPR_PRIO_DEFAULT, color_black);
        putRect({ 0.0f, 704.0f, 1280.0f, 64.0f }, SCREEN_MODE_WXGA, SPR_PRIO_DEFAULT, color_black);
    }

    // 0x140640610
    SprArgs* putObject(const SprArgs& args, const SprDb* spr_db) {
        SprArgs _args = args;
        if (args.kind == SPR_KIND_NORMAL)
            _args.id = spr_db->getSprIdFromUid(args.id.w);
        return SprMgr::sprmgr().put(_args, spr_db);
    }

    // 0x140640740
    SprArgs* put(const SprArgs& args, const SprDb* spr_db) {
        return SprMgr::sprmgr().put(args, spr_db);
    }

    // 0x140640750
    void putPolyLine(const vec2* pos_arg, size_t len, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer) {
        SprArgs args;
        args.kind = SPR_KIND_POLY_LINE;
        args.layer = layer;
        args.screen_trans = screen;
        args.screen_scale = screen;
        args.prio = prio;
        args.color = color;

        std::vector<SprArgs::Quad> quad;
        quad.reserve(len);

        for (size_t i = 0; i < len; i++, pos_arg++) {
            SprArgs::Quad vert = {};
            *(vec2*)&vert.pos = *pos_arg;
            quad.push_back(vert);
        }

        args.setQuadArgs(quad.data(), quad.size());
        SprMgr::sprmgr().put(args, 0);
    }

    // 0x140640B50
    void putRect(rectangle rect, SCREEN_MODE screen,
        SPR_PRIO prio, color4u8 color, SPR_LAYER layer) {
        SprArgs args;
        args.trans.x = rect.pos.x;
        args.trans.y = rect.pos.y;
        args.trans.z = 0.0f;
        args.layer = layer;
        args.kind = SPR_KIND_RECT;
        args.screen_trans = screen;
        args.screen_scale = screen;
        args.prio = prio;
        args.color = color;
        args.setSize(rect.size);
        enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);
        SprMgr::sprmgr().put(args, 0);
    }

    // 0x140640C50
    void putStrip(const SprArgs::Quad* quad_arg, size_t len, SCREEN_MODE screen,
        SPR_PRIO prio, uint32_t spr_id, SPR_LAYER layer, const SprDb* spr_db) {
        SprArgs args;
        args.layer = layer;
        args.kind = SPR_KIND_STRIP;
        args.screen_trans = screen;
        args.screen_scale = screen;
        args.prio = prio;
        args.color = quad_arg[0].color;
        if (spr_id != -1)
            args.id = spr_db->getSprIdFromUid(spr_id);
        enum_or(args.attr, M_SPR_ATTR_NOTRANSFORM);
        args.setQuadArgs(quad_arg, len);
        SprMgr::sprmgr().put(args, spr_db);
    }

    // 0x140640D40
    void refresh() {
        SprMgr::sprmgr().refresh();
    }

    // 0x140640D50
    void request(const std::vector<uint32_t>& setids,
        const std::string mdata_dir, void* data, const SprDb* spr_db) {
        for (uint32_t setid : setids)
            request(setid, mdata_dir, data, spr_db);
    }

    // 0x140640E10
    void request(uint32_t setid, const std::string mdata_dir, void* data, const SprDb* spr_db) {
        SprMgr::sprmgr().request(spr_db->getSetIdFromUid(setid),
            spr_db->getSetFileFromUid(setid), mdata_dir, data);
    }
    
    // Added
    void requestModern(const std::vector<uint32_t>& sethashes, void* data, SprDb* spr_db) {
        for (uint32_t sethash : sethashes)
            requestModern(sethash, data, spr_db);
    }

    // Added
    void requestModern(uint32_t sethash, void* data, SprDb* spr_db) {
        SprMgr::sprmgr().requestModern(spr_db->getSetIdFromUid(sethash), sethash, data, spr_db);
    }

    void setProjection(SPR_LAYER layer, float_t aspect) {
        SprMgr::sprmgr().setProjection(layer, aspect);
    }

    // 0x140640F10
    void setTarget(SPR_TARGET in_target) {
        SprMgr::sprmgr().setTarget(in_target);
    }

    // 0x140640F20
    void setViewport(SPR_LAYER layer, SCREEN_MODE screen, const rectangle rect) {
        SprMgr::sprmgr().setViewport(layer, screen, rect);
    }

    // 0x140640F50
    bool wait(const std::vector<uint32_t>& setids, const SprDb* spr_db) {
        bool ret = false;
        for (uint32_t setid : setids)
            if (wait(setid, spr_db))
                ret = true;
        return ret;
    }

    // 0x140640FB0
    bool wait(uint32_t setid, const SprDb* spr_db) {
        return SprMgr::sprmgr().wait(spr_db->getSetIdFromUid(setid));
    }

    // Added
    bool waitModern(const std::vector<uint32_t>& sethashes, SprDb* spr_db) {
        bool ret = false;
        for (uint32_t sethash : sethashes)
            if (waitModern(sethash, spr_db))
                ret = true;
        return ret;
    }

    // Added
    bool waitModern(uint32_t sethash, SprDb* spr_db) {
        return SprMgr::sprmgr().waitModern(spr_db->getSetIdFromUid(sethash), spr_db);
    }
}

void sprite_manager_init() {
    if (!spr::SprMgr::p_sprmgr)
        spr::SprMgr::p_sprmgr = new spr::SprMgr;

    if (!spr::render_data)
        spr::render_data = new spr::RenderData;

#if BREAK_SPRITE_VERTEX_LIMIT
    if (!string_quad)
        string_quad = new spr::SprArgs::Quad[string_quad_max_count];
#endif
}

void sprite_manager_add_spr_sets(const SprDb* spr_db) {
    spr::SprMgr::sprmgr().init(spr_db);
}

::spr_set* sprite_manager_get_set(uint32_t spr_id, const SprDb* spr_db) {
    const SPRSET* set = spr::SprMgr::sprmgr().find(spr_db->getSetIdFromUid(spr_id));
    if (set)
        return set->spr_set;
    return 0;
}

void sprite_manager_set_res(double_t aspect, int32_t width, int32_t height) {
    if (!spr::SprMgr::p_sprmgr)
        return;

    for (int32_t i = 0; i < spr::SPR_LAYER_MAX; i++) {
        spr::SprMgr::sprmgr().setProjection((spr::SPR_LAYER)i, (float_t)aspect);
        spr::SprMgr::sprmgr().setViewport((spr::SPR_LAYER)i, spr::SprMgr::sprmgr().view_list[i].screen,
            { 0.0f, 0.0f, (float_t)width, (float_t)height });
    }
}

void sprite_manager_free() {
#if BREAK_SPRITE_VERTEX_LIMIT
    if (string_quad) {
        delete[] string_quad;
        string_quad = 0;
    }
#endif

    if (spr::render_data) {
        delete spr::render_data;
        spr::render_data = 0;
    }

    if (spr::SprMgr::p_sprmgr) {
        delete spr::SprMgr::p_sprmgr;
        spr::SprMgr::p_sprmgr = 0;
    }
}

namespace spr {
    RenderData::RenderData() : vao() {
        vbo_vertex_count = 4096;
        ebo_index_count = 4096;

        static const GLsizei buffer_size_uv0 = sizeof(sprite_vertex_data_uv0);
        static const GLsizei buffer_size_uv1 = sizeof(sprite_vertex_data_uv1);
        static const GLsizei buffer_size_uv2 = sizeof(sprite_vertex_data_uv2);

        vbo.Create(gl_state, buffer_size_uv2 * vbo_vertex_count);
        ebo.Create(gl_state, sizeof(uint32_t) * ebo_index_count);

        glGenVertexArrays(3, vao);
        gl_state.bind_vertex_array(vao[0]);
        gl_state.bind_array_buffer(vbo, true);
        gl_state.bind_element_array_buffer(ebo, true);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv0,
            (void*)offsetof(sprite_vertex_data_uv0, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv0,
            (void*)offsetof(sprite_vertex_data_uv0, color));

        gl_state.bind_vertex_array(vao[1]);
        gl_state.bind_array_buffer(vbo, true);
        gl_state.bind_element_array_buffer(ebo, true);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv1,
            (void*)offsetof(sprite_vertex_data_uv1, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv1,
            (void*)offsetof(sprite_vertex_data_uv1, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size_uv1,
            (void*)offsetof(sprite_vertex_data_uv1, uv));

        gl_state.bind_vertex_array(vao[2]);
        gl_state.bind_array_buffer(vbo, true);
        gl_state.bind_element_array_buffer(ebo, true);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv2,
            (void*)offsetof(sprite_vertex_data_uv2, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv2,
            (void*)offsetof(sprite_vertex_data_uv2, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size_uv2,
            (void*)offsetof(sprite_vertex_data_uv2, uv[0]));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, buffer_size_uv2,
            (void*)offsetof(sprite_vertex_data_uv2, uv[1]));

        gl_state.bind_array_buffer(0);
        gl_state.bind_vertex_array(0);
        gl_state.bind_element_array_buffer(0);
    }

    RenderData::~RenderData() {
        if (vao[0]) {
            glDeleteVertexArrays(3, vao);
            vao[0] = 0;
        }

        ebo.Destroy();
        vbo.Destroy();
    }

    template <typename T>
    size_t RenderData::AddData(T*& data, size_t nb_quad) {
        size_t size = vertex_buffer.size();
        size_t align_offset = (sizeof(T) - size % sizeof(T)) % sizeof(T);
        size_t vertex_array_size = sizeof(T) * nb_quad;

        vertex_buffer.resize(size + align_offset + vertex_array_size);
        T* vtx_data = (T*)(vertex_buffer.data() + size + align_offset);
        data = vtx_data;
        return (uint32_t)((size + align_offset) / sizeof(T));
    }

    void RenderData::Clear() {
        draw_param_buffer.clear();
        vertex_buffer.clear();
        index_buffer.clear();
    }

    void RenderData::Update() {
        static const GLsizei buffer_size_uv0 = sizeof(sprite_vertex_data_uv0);
        static const GLsizei buffer_size_uv1 = sizeof(sprite_vertex_data_uv1);
        static const GLsizei buffer_size_uv2 = sizeof(sprite_vertex_data_uv2);

        if (vbo_vertex_count < vertex_buffer.size() / sizeof(sprite_vertex_data_uv2)) {
            while (vbo_vertex_count < vertex_buffer.size() / sizeof(sprite_vertex_data_uv2))
                vbo_vertex_count *= 2;

            vbo.Recreate(gl_state, buffer_size_uv2 * vbo_vertex_count);

            gl_state.bind_vertex_array(vao[0]);
            gl_state.bind_array_buffer(vbo, true);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv0,
                (void*)offsetof(sprite_vertex_data_uv0, pos));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv0,
                (void*)offsetof(sprite_vertex_data_uv0, color));

            gl_state.bind_vertex_array(vao[1]);
            gl_state.bind_array_buffer(vbo, true);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv1,
                (void*)offsetof(sprite_vertex_data_uv1, pos));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv1,
                (void*)offsetof(sprite_vertex_data_uv1, color));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size_uv1,
                (void*)offsetof(sprite_vertex_data_uv1, uv));

            gl_state.bind_vertex_array(vao[2]);
            gl_state.bind_array_buffer(vbo, true);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size_uv2,
                (void*)offsetof(sprite_vertex_data_uv2, pos));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, buffer_size_uv2,
                (void*)offsetof(sprite_vertex_data_uv2, color));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size_uv2,
                (void*)offsetof(sprite_vertex_data_uv2, uv[0]));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, buffer_size_uv2,
                (void*)offsetof(sprite_vertex_data_uv2, uv[1]));

            gl_state.bind_array_buffer(0);
            gl_state.bind_vertex_array(0);
        }

        vbo.WriteMemory(gl_state, 0, vertex_buffer.size(), vertex_buffer.data());

        if (ebo_index_count < index_buffer.size()) {
            while (ebo_index_count < index_buffer.size())
                ebo_index_count *= 2;

            ebo.Recreate(gl_state, sizeof(uint32_t) * ebo_index_count);

            gl_state.bind_vertex_array(vao[0]);
            gl_state.bind_element_array_buffer(ebo, true);
            gl_state.bind_vertex_array(vao[1]);
            gl_state.bind_element_array_buffer(ebo, true);
            gl_state.bind_vertex_array(vao[2]);
            gl_state.bind_element_array_buffer(ebo, true);

            gl_state.bind_vertex_array(0);
            gl_state.bind_element_array_buffer(0);
        }

        ebo.WriteMemory(gl_state, 0, sizeof(uint32_t) * index_buffer.size(), index_buffer.data());

        gl_state.bind_array_buffer(0);
        gl_state.bind_element_array_buffer(0);
    }

    SprMgr::Projection::Projection() : aspect() {

    }

    SprMgr::ViewPort::ViewPort() : screen(SCREEN_MODE_MAX) {

    }

    SprMgr::SprMgr() : target(), set_counter() {
        resetTarget();
        screen_mode_back = SCREEN_MODE_MAX;

        ScreenParam res = ScreenParam(SCREEN_MODE_HD);

        projection_list[0].aspect = (float_t)res.aspect;
        view_list[0].screen = res.mode;
        view_list[0].rect = { 0.0f, 0.0f, (float_t)res.width, (float_t)res.height };

        projection_list[1].aspect = (float_t)res.aspect;
        view_list[1].screen = res.mode;
        view_list[1].rect = { 0.0f, 0.0f, (float_t)res.width, (float_t)res.height };

        view_projection = mat4_identity;
        mat = mat4_identity;
    }

    SprMgr::~SprMgr() {
        refresh();
        dest();
        render_data->Clear();
    }

    // 0x14063BA60
    const SPRSET* SprMgr::find(uint32_t id) {
        auto elem = sets.find(id);
        if (elem != sets.end())
            return &elem->second;
        return 0;
    }

    // 0x14063CB40
    void SprMgr::init() {
        resetTarget();
        screen_mode_back = SCREEN_MODE_MAX;

        ScreenParam& screen_param = get_screen_param();

        for (int32_t i = 0; i < spr::SPR_LAYER_MAX; i++) {
            setProjection((spr::SPR_LAYER)i, (float_t)screen_param.aspect);
            setViewport((spr::SPR_LAYER)i, screen_param.mode,
                { 0.0f, 0.0f, (float_t)screen_param.width, (float_t)screen_param.height });
        }

        view_projection = mat4_identity;
        mat = mat4_identity;
    }

    // Added
    void SprMgr::init(const SprDb* spr_db) {
        for (auto& i : spr_db->set_idx)
            sets.insert({ i.first, SPRSET(i.first) });
    }

    // 0x140639F60
    void SprMgr::dest() {
        for (auto i : sets)
            i.second.free();
        sets.clear();
    }

    // Added
    void SprMgr::dest(const SprDb* spr_db) {
        for (auto& i : spr_db->set_idx)
            sets.erase(i.first);
    }

    // Missing (Not seen anywhere)
    bool SprMgr::ready() {
        abort();
    }

    // 0x14063F030
    void SprMgr::setProjection(SPR_LAYER layer, float_t aspect) {
        projection_list[layer].aspect = aspect;
    }

    // 0x14063C9C0
    void SprMgr::getViewport(SCREEN_MODE& screen, rectangle& rect, SPR_LAYER layer) const {
        screen =  view_list[layer].screen;
        rect = view_list[layer].rect;
    }

    // 0x14063F140
    void SprMgr::setViewport(SPR_LAYER layer, SCREEN_MODE screen, const rectangle rect) {
        view_list[layer].screen = screen;
        view_list[layer].rect = rect;
    }

    // 0x14063F130
    void SprMgr::setTarget(SPR_TARGET in_target) {
        target = in_target;
    }

    // 0x14063C890
    SPR_TARGET SprMgr::getTarget() {
        return target;
    }

    // 0x14063BAB0
    void SprMgr::flush(render_data_context& rend_data_ctx,
        SPR_TARGET in_target, bool in_scale, texture* overlay_tex, const mat4& vp) {
        SprMgr::begin_render(rend_data_ctx);

        SCREEN_MODE screen_mode = get_screen_param().mode;
        if (in_target == SPR_TARGET_BACK && screen_mode_back != SCREEN_MODE_MAX)
            get_screen_param().mode = screen_mode_back;

        gl_rend_state_rect viewport_rect = rend_data_ctx.state.get_viewport();

        for (int32_t i = 0; i < SPR_LAYER_MAX; i++) {
            auto obj_list = this->obj_list[in_target][i];

            size_t count = 0;
            for (uint32_t j = SPR_PRIO_MAX, k = 0; j; j--, k++)
                count += obj_list[k].size();

            if (!count)
                continue;

            int32_t x_min;
            int32_t y_min;
            int32_t x_max;
            int32_t y_max;
            if (in_target == SPR_TARGET_FRONT || in_target == SPR_TARGET_3) {
                ScreenParam& screen_param = get_screen_param();

                float_t sprite_half_width = (float_t)screen_param.width * 0.5f;
                float_t sprite_half_height = (float_t)screen_param.height * 0.5f;

                float_t aet_depth = rctx_ptr->camera->aet_depth;
                float_t aet_depth_1 = 1.0f / aet_depth;

                float_t v15a = sprite_half_height * projection_list[i].aspect * 0.2f * aet_depth_1;
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
                get_screen_conv_pos(min, screen_mode, view_list[i].rect.pos, view_list[i].screen);
                get_screen_conv_pos(max, screen_mode,
                    view_list[i].rect.pos + view_list[i].rect.size, view_list[i].screen);
                x_min = (int32_t)min.x;
                y_min = (int32_t)min.y;
                x_max = (int32_t)(max.x - min.x);
                y_max = (int32_t)(max.y - min.y);
                rend_data_ctx.state.set_viewport(x_min, y_min, x_max, y_max);
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
                1.0f / (float_t)overlay_tex->width,
                1.0f / (float_t)overlay_tex->height, 0.0f, 0.0f
            };
            mat4_transpose(&view_projection, &shader_data.g_transform);
            rctx_ptr->sprite_scene_ubo.WriteMemory(rend_data_ctx.state, shader_data);
            rend_data_ctx.state.bind_uniform_buffer_base(0, rctx_ptr->sprite_scene_ubo);

            for (uint32_t j = SPR_PRIO_MAX; j; j--, obj_list++)
                for (SprArgs& k : *obj_list)
                    if (k.sprite_draw_param_index >= 0)
                        k.draw(rend_data_ctx, mat, x_min, y_min, x_max, y_max, overlay_tex);
            rend_data_ctx.state.bind_vertex_array(0);
        }

        if (in_target == SPR_TARGET_BACK && screen_mode_back != SCREEN_MODE_MAX)
            get_screen_param().mode = screen_mode;

        SprMgr::end_render(rend_data_ctx);
    }

    // 0x14063D940
    void SprMgr::refresh() {
        for (auto& i : obj_list)
            for (auto& j : i)
                for (auto& k : j)
                    k.clear();

        resetTarget();

        string_quad_idx = 0;
    }

    // 0x14063E990
    void SprMgr::request(uint32_t id, const char* file, const std::string mdata_dir, void* data) {
        SPRSET* set = (SPRSET*)find(id);
        if (set)
            set->request(file, mdata_dir, data);
    }

    // Added
    void SprMgr::requestModern(uint32_t id, uint32_t sethash, void* data, SprDb* spr_db) {
        if (id == -1) {
            id = addSet();
            spr_db->addSet(sethash, id);
        }

        SPRSET* set = (SPRSET*)find(id);
        if (set)
            set->requestModern(sethash, data);
    }

    // 0x14063F6F0
    bool SprMgr::wait(uint32_t id) {
        SPRSET* set = (SPRSET*)find(id);
        if (set)
            return set->wait();
        return false;
    }

    // Added
    bool SprMgr::waitModern(uint32_t id, SprDb* spr_db) {
        SPRSET* set = (SPRSET*)find(id);
        if (set)
            return set->waitModern(spr_db);
        return false;
    }

    // 0x14063C300
    void SprMgr::free(uint32_t id) {
        SPRSET* set = (SPRSET*)find(id);
        if (set)
            set->free();
    }

    // Added
    void SprMgr::freeModern(uint32_t id, SprDb* spr_db) {
        SPRSET* set = (SPRSET*)find(id);
        if (set) {
            set->freeModern(spr_db);

            if (set->reference <= 0) {
                auto elem = sets.find(id);
                if (elem != sets.end())
                    sets.erase(elem);
            }
        }
    }

    // 0x14063F9E0
    int32_t SprMgr::getTexNum(uint32_t id) {
        const SPRSET* set = find(id);
        if (set)
            return set->getTexNum();
        return 0;
    }

    // 0x14063F9A0
    int32_t SprMgr::getSprNum(uint32_t id) {
        const SPRSET* set = find(id);
        if (set)
            return set->getSprNum();
        return 0;
    }

    // 0x14063C420
    bool SprMgr::getReady(uint32_t id) {
        const SPRSET* set = find(id);
        if (set)
            return set->getReady();
        return false;
    }

    // 0x14063F900
    const char* SprMgr::getName(SprId id) {
        //const SPRSET* set = find(id.m.set_idx & 0x0FFF);
        const SPRSET* set = find(id.m.set_idx & 0x3FFF);
        if (set)
            return set->getName(id.w);
        return "(null)";
    }

    // 0x14063FA30, Inlined
    inline size_t SprMgr::getObjListCount(SPR_TARGET in_target) {
        size_t count = 0;
        for (const auto& i : obj_list[target])
            for (const auto& j : i)
                count += j.size();
        return count;
    }

    // 0x14063C4F0
    SprArgs::Rect SprMgr::getRect(SprId id) {
        //const SPRSET* set = find(id.m.set_idx & 0x0FFF);
        const SPRSET* set = find(id.m.set_idx & 0x3FFF);
        if (set)
            return set->getRect(id.w);
        return {};
    }

    // 0x14063C370
    uint32_t SprMgr::getAttr(SprId id) {
        //const SPRSET* set = find(id.m.set_idx & 0x0FFF);
        const SPRSET* set = find(id.m.set_idx & 0x3FFF);
        if (set)
            return set->getAttr(id.w);
        return 0;
    }

    // 0x14063C580
    SCREEN_MODE SprMgr::getScreen(SprId id) {
        //const SPRSET* set = find(id.m.set_idx & 0x0FFF);
        const SPRSET* set = find(id.m.set_idx & 0x3FFF);
        if (set)
            return set->getScreen(id.w);
        return SCREEN_MODE_HD;
    }

    // 0x14063C990
    const texture* SprMgr::getTexture(SprId id) {
        //const SPRSET* set = find(id.m.set_idx & 0x0FFF);
        const SPRSET* set = find(id.m.set_idx & 0x3FFF);
        if (set)
            return set->getTexture(id.w);
        return 0;
    }

    // 0x14063D6A0
    SprArgs* SprMgr::put(const SprArgs& args, const SprDb* spr_db) {
        SPR_TARGET target = args.target;
        if (target == -1)
            target = this->target;

        auto obj_list = &this->obj_list[target][args.layer][args.prio];
        SprId id = args.id;
        if (args.kind == SPR_KIND_NORMAL)
            id = spr_db->getSprIdFromUid(id.w);

        if (id.not_null()) {
            //const SPRSET* set = find(id.m.set_idx & 0x8FFF);
            const SPRSET* set = find(id.m.set_idx & 0xBFFF);
            if (!set || !set->ready)
                return 0;

            obj_list->push_back(args);
            SprArgs& _args = obj_list->back();
            _args.id = id;
            _args.tex = set->getTexture(id);
            SprArgs::Rect rect = set->getRect(id);
            _args.setSize({ rect.width, rect.height });
            _args.setRect(rect.x, rect.y, rect.width, rect.height);
            if (_args.screen_scale != SCREEN_MODE_MAX)
                _args.screen_scale = set->getScreen(_args.id);
            return &_args;
        }

        if (args.tex) {
            obj_list->push_back(args);
            SprArgs& _args = obj_list->back();
            _args.id = -1;
            _args.setSize({ (float_t)_args.tex->width, (float_t)_args.tex->height });
            _args.setRect(0.0, 0.0, (float_t)_args.tex->width, (float_t)_args.tex->height);
            return &_args;
        }
        else {
            obj_list->push_back(args);
            SprArgs& _args = obj_list->back();
            _args.id = -1;
            return &_args;
        }
    }

    // Added
    uint32_t SprMgr::addSet() {
        uint32_t index = set_counter;
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

        sets.insert({ 0x8000 | index, SPRSET(0x8000 | index) });
        set_counter = index + 1;
        //if (index + 1 > 0x0FFF)
        if (index + 1 > 0x3FFF)
            set_counter = 1;
        return 0x8000 | index;
    }

    // 0x1406362E0
    void SprMgr::resetTarget() {
        target = SPR_TARGET_FRONT;
    }

    // Added
    void SprMgr::postFlush() {
        render_data->Clear();
    }

    // Added
    void SprMgr::preFlush() {
        auto check_same = [](const SprArgsDraw& left, const SprArgsDraw& right) -> bool {
            if (left.num_texture != right.num_texture)
                return false;

            for (int32_t i = 0; i < left.num_texture; i++)
                if (left.texs[i] != right.texs[i])
                    return false;

            return left.blend == right.blend
                && left.blend != SPR_BLEND_OVERLAY && right.blend != SPR_BLEND_OVERLAY
                && left.shader_name == right.shader_name && left.kind == right.kind
                && left.kind != SPR_KIND_POLY_LINE && right.kind != SPR_KIND_POLY_LINE
                && left.kind != SPR_KIND_STRIP && right.kind != SPR_KIND_STRIP;
        };

        auto get_args_draw = [](const SprArgs& args, SprArgsDraw& args_draw) {
            int32_t num_texture = 0;
            if (args.tex) {
                const SprArgs* _args = &args;
                while (_args) {
                    args_draw.texs[num_texture++] = _args->tex;
                    _args = _args->next;
                }
            }

            args_draw.num_texture = num_texture;
            args_draw.blend = args.blend;
            args_draw.shader_name = args.shader_name;
            args_draw.kind = args.kind;
        };

        SprArgsDraw args_draw[2];
        SprArgs* args_array[256];
        for (int32_t i = 0; i < SPR_TARGET_MAX; i++)
            for (int32_t j = 0; j < SPR_LAYER_MAX; j++)
                for (int32_t k = 0; k < SPR_PRIO_MAX; k++) {
                    int32_t args_count = 0;
                    int32_t draw_idx = 0;
                    for (SprArgs& args : obj_list[i][j][k]) {
                        if (!args_count) {
                            get_args_draw(args, args_draw[draw_idx]);

                            args_array[args_count++] = &args;
                            draw_idx = 1 - draw_idx;
                        }
                        else if (args_count >= 0x100) {
                            SprArgs::addDraw(args_array, args_count, true,
                                args_draw[1 - draw_idx]);
                            get_args_draw(args, args_draw[draw_idx]);

                            args_count = 0;
                            args_array[args_count++] = &args;
                            draw_idx = 1 - draw_idx;
                        }
                        else {
                            get_args_draw(args, args_draw[draw_idx]);
                            if (!check_same(args_draw[1 - draw_idx], args_draw[draw_idx])) {
                                SprArgs::addDraw(args_array, args_count, true,
                                    args_draw[1 - draw_idx]);

                                args_count = 0;
                                draw_idx = 1 - draw_idx;
                            }
                            args_array[args_count++] = &args;
                        }
                    }

                    if (args_count)
                        SprArgs::addDraw(args_array, args_count, true,
                            args_draw[1 - draw_idx]);
                }

        render_data->Update();
    }

    // 0x140638A80
    void SprMgr::begin_render(render_data_context& rend_data_ctx) {
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.active_bind_texture_2d(0, rctx_ptr->empty_texture_2d->glid);
        rend_data_ctx.state.bind_sampler(0, 0);
        rend_data_ctx.state.active_bind_texture_2d(1, rctx_ptr->empty_texture_2d->glid);
        rend_data_ctx.state.bind_sampler(1, 0);
        rend_data_ctx.state.bind_sampler(7, 0);
        rend_data_ctx.state.set_blend_func_separate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    }

    // 0x14063AF80
    void SprMgr::end_render(render_data_context& rend_data_ctx) {
        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.active_bind_texture_2d(0, 0);
        rend_data_ctx.state.bind_sampler(0, 0);
        rend_data_ctx.state.active_bind_texture_2d(1, 0);
        rend_data_ctx.state.bind_sampler(1, 0);
        rend_data_ctx.state.active_bind_texture_2d(7, 0);
        rend_data_ctx.state.bind_sampler(7, 0);
        rend_data_ctx.state.set_blend_func_separate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        shader::unbind(rend_data_ctx.state);
    }
}

SPRSET::SPRSET(uint32_t in_id) : flag(), spr_set(),
textures(), reference(), ready() {
    id = in_id;
    hash = hash_murmurhash_empty;
}

SPRSET::~SPRSET() {

}

// 0x14063E410
void SPRSET::request(const char* fname, const std::string mdata_dir, void* data) {
    if (reference > 1) {
        reference++;
        return;
    }

    reference = 1;
    name.assign(fname);

    std::string farc_file(name);
    size_t off = farc_file.rfind(".bin");
    if (off != -1 && off == farc_file.size() - 4)
        farc_file.replace(off, 4, ".farc");

    const char* dir = "./rom/2d/";
    if (mdata_dir.size() && ((data_struct*)data)->check_directory_exists(mdata_dir.c_str()))
        dir = mdata_dir.c_str();

    if (((data_struct*)data)->check_file_exists(dir, farc_file.c_str()))
        file_handler.read_file(data, dir, farc_file.c_str(), fname, prj::MemCSystem, false);
    ready = false;
}

// Added
void SPRSET::requestModern(uint32_t sethash, void* data) {
    if (reference > 1) {
        reference++;
        return;
    }

    std::string file;
    if (((data_struct*)data)->get_file("root+/2d/", sethash, ".farc", file))
        file_handler.read_file(data, "root+/2d/", file.c_str(), prj::MemCSystem);
    this->hash = sethash;
    ready = false;
}

// 0x14063F680
bool SPRSET::wait() {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;
    else if (!readTable() || !readTexHeader())
        return false;

    file_handler.reset();
    ready = true;
    return true;
}

// Added
bool SPRSET::waitModern(SprDb* spr_db) {
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

    SprDbFile spr_db_file;
    spr_db_file.read(spi->data, spi->size, true);

    SprDbFile::SprSet* spr_set_file = 0;
    if (spr_db_file.ready)
        for (SprDbFile::SprSet& m : spr_db_file.set)
            if (m.uid == hash) {
                spr_set_file = &m;
                break;
            }

    if (!spr_set_file)
        return false;

    if (spr_db_file.ready) {
        spr_db->parse(spr_set_file, sprite_ids);
        set_name.assign(spr_set_file->name);
    }

    if (!readTableModern(spr->data, spr->size) || !readTexHeaderModern(spr->data, spr->size))
        return false;

    file_handler.reset();
    ready = true;
    return true;
}

// 0x14063C0D0
void SPRSET::free() {
    if (reference > 1) {
        reference--;
        return;
    }

    file_handler.reset();
    freeTex();
    alloc_handler.reset();

    spr_set = 0;
    name.clear();
    ready = false;
    reference = 0;
}

// Added
void SPRSET::freeModern(SprDb* spr_db) {
    if (reference > 1) {
        reference--;
        return;
    }

    spr_db->freeSet(hash, id, set_name.c_str(), sprite_ids);

    file_handler.reset();
    freeTex();
    alloc_handler.reset();

    spr_set = 0;
    name.clear();
    ready = false;
    reference = 0;

    hash = hash_murmurhash_empty;
    set_name.clear();
    sprite_ids.clear();
}

// 0x14063C920
int32_t SPRSET::getTexNum() const {
    return spr_set->num_of_texture;
}

// 0x14063C860
int32_t SPRSET::getSprNum() const {
    return spr_set->num_of_sprite;
}

// 0x14063C410
bool SPRSET::getReady() const {
    return spr_set->ready;
}

// Missing
int32_t SPRSET::getReference() const {
    return reference;
}

// Missing
const char* SPRSET::getFileName() const {
    return name.c_str();
}

// 0x14063C950
const texture* SPRSET::getTexture(SprId info) const {
    //if (info.m.set_idx & 0x1000)
    if (info.m.set_idx & 0x4000)
        return textures[info.m.idx];
    else
        return textures[spr_set->sprinfo[info.m.idx].texid];
}

// 0x14063C440
const spr::SprArgs::Rect SPRSET::getRect(SprId in_id) const {
    //if (in_id.m.set_idx & 0x1000) {
    if (in_id.m.set_idx & 0x4000) {
        texture* tex = textures[in_id.m.idx];
        return { 0.0f, 0.0f, (float_t)tex->width, (float_t)tex->height };
    }

    spr::SprInfo* sprinfo = &spr_set->sprinfo[in_id.m.idx];
    return { (float_t)(int32_t)sprinfo->px, (float_t)(int32_t)sprinfo->py,
        (float_t)(int32_t)sprinfo->width, (float_t)(int32_t)sprinfo->height };
}

// 0x14063C350
uint32_t SPRSET::getAttr(SprId in_id) const {
    //if (in_id.m.set_idx & 0x1000)
    if (in_id.m.set_idx & 0x4000)
        return 0;
    else
        return spr_set->sprdata[in_id.m.idx].attr;
}

// 0x14063C550
SCREEN_MODE SPRSET::getScreen(SprId in_id) const {
    //if (in_id.m.set_idx & 0x1000)
    if (in_id.m.set_idx & 0x4000)
        return SCREEN_MODE_HD;
    else
        return spr_set->sprdata[in_id.m.idx].screen_mode;
}

// 0x14063C3A0
const char* SPRSET::getName(SprId in_id) const {
    //if (in_id.m.set_idx & 0x1000)
    if (in_id.m.set_idx & 0x4000)
        return spr_set->texname[in_id.m.idx];
    else
        return spr_set->sprname[in_id.m.idx];
}

// 0x14063E220
bool SPRSET::readTable() {
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

// Added
bool SPRSET::readTableModern(const void* data, size_t size) {
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

// 0x14063CCF0
bool SPRSET::readTexHeader() {
    size_t data = (size_t)file_handler.get_data();
    if (!data)
        return false;

    std::vector<uint32_t> ids(spr_set->num_of_texture);
    //uint32_t set_index = (uint32_t)(0x1000 | id & 0x0FFF);
    uint32_t set_index = (uint32_t)(0x4000 | id & 0x3FFF);
    uint32_t id = 0;
    for (uint32_t& i : ids)
        i = (set_index << 16) | id++;
    texture_txp_set_load(spr_set->txp, &textures, ids.data());
    return !!textures;
}

// Added
bool SPRSET::readTexHeaderModern(const void* data, size_t size) {
    if (!data || !size)
        return false;

    std::vector<uint32_t> ids(spr_set->num_of_texture);
    //uint32_t set_index = (uint32_t)(0x8000 | 0x1000 | id & 0x0FFF);
    uint32_t set_index = (uint32_t)(0x8000 | 0x4000 | id & 0x3FFF);
    uint32_t id = 0;
    for (uint32_t& i : ids)
        i = (set_index << 16) | id++;
    texture_txp_set_load(spr_set->txp, &textures, ids.data());
    return !!textures;
}

// 0x14063C320
bool SPRSET::freeTex() {
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
