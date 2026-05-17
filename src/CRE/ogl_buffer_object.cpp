/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "ogl_buffer_object.hpp"
#include "../KKdLib/obj.hpp"
#include "gl_state.hpp"
#include "render_context.hpp"

struct BufObjMgr {
    int32_t vb_peak_size;
    int32_t vb_all_size;
    int32_t ib_peak_size;
    int32_t ib_all_size;
};

BufObjMgr bufobj_mgr;

// Added
IndexBuffer::IndexBuffer() : ib() {

}

// Added
bool IndexBuffer::create(uint32_t size, const void* buf) {
    if (!size)
        return false;

    ib = create_index_buffer(size, buf);
    return true;
}

#if SHARED_OBJECT_BUFFER
// Added
void IndexBuffer::create(GLuint in_ib) {
    ib = in_ib;
}
#endif

// Added
void IndexBuffer::destroy() {
    free_index_buffer(ib);
    ib = 0;
}

#if SHARED_OBJECT_BUFFER
// Added
void IndexBuffer::destroy_shared() {
    ib = 0;
}
#endif

// Added
GLuint IndexBuffer::get_glib() const {
    return ib;
}

// 0x1404615C0
#if SHARED_OBJECT_BUFFER
VertexBuffer::VertexBuffer() : num_flip_chain(), vb(),
flip_index(), target(GL_ARRAY_BUFFER), vb_size(), vb_offset() {
#else
VertexBuffer::VertexBuffer() : num_flip_chain(), vb(),
flip_index(), target(GL_ARRAY_BUFFER), vb_size() {
#endif

}

// 0x1404615E0
VertexBuffer::~VertexBuffer() {
    destroy();
}

// 0x140461650
bool VertexBuffer::create(uint32_t size, const void* buf, uint32_t num_flip, GL::BufferUsage usage) {
    if (!size || num_flip > VertexBuffer::NUM_FLIP_MAX)
        return false;

    num_flip_chain = num_flip;
    flip_index = 0;
    vb_size = size;
#if SHARED_OBJECT_BUFFER
    vb_offset = 0;
#endif

    for (uint32_t i = 0; i < num_flip; i++) {
        vb[i] = create_vertex_buffer(size, buf, usage);
        if (!vb[i]) {
            destroy();
            return false;
        }
    }
    return true;
}

#if SHARED_OBJECT_BUFFER
// Added
void VertexBuffer::create(GLuint in_vb, uint32_t size, uint32_t offset) {
    destroy_shared();

    if (size) {
        num_flip_chain = 1;
        flip_index = 0;
        vb[0] = in_vb;
        vb_size = size;
        vb_offset = offset;

        bufobj_mgr.vb_all_size += size;
        bufobj_mgr.vb_peak_size = max_def(bufobj_mgr.vb_peak_size, bufobj_mgr.vb_all_size);
    }
}
#endif

// 0x140461870
void VertexBuffer::destroy() {
    for (uint32_t i = 0; i < num_flip_chain; i++) {
        free_vertex_buffer(vb[i]);
        vb[i] = 0;
    }

    num_flip_chain = 0;
    flip_index = 0;
    vb_size = 0;
#if SHARED_OBJECT_BUFFER
    vb_offset = 0;
#endif
}

#if SHARED_OBJECT_BUFFER
// Added
void VertexBuffer::destroy_shared() {
    for (uint32_t i = 0; i < num_flip_chain; i++)
        vb[i] = 0;

    bufobj_mgr.vb_all_size -= vb_size;

    num_flip_chain = 0;
    flip_index = 0;
    vb_size = 0;
    vb_offset = 0;
}
#endif

// 0x1404618E0
void VertexBuffer::flip() {
    if (++flip_index >= num_flip_chain)
        flip_index = 0;
}

// 0x140461A20
GLuint VertexBuffer::get_glvb() const {
    if (flip_index < num_flip_chain)
        return vb[flip_index];
    return 0;
}

// Missing
GLuint VertexBuffer::get_glvb_at(uint32_t index) const {
    if (index < num_flip_chain)
        return vb[index];
    return 0;
}

// Added
uint32_t VertexBuffer::get_glvb_offset() const {
#if SHARED_OBJECT_BUFFER
    if (vb[0])
        return vb_offset;
#endif
    return 0;
}

// 0x140461A40
uint32_t VertexBuffer::get_num_flip_chain(uint32_t index) const {
    return num_flip_chain;
}

// 0x140461A50
uint32_t VertexBuffer::get_size() const {
    if (vb[0])
        return vb_size;
    return 0;
}

// 0x140461AE0
GLenum VertexBuffer::get_target() const {
    return GL_ARRAY_BUFFER;
}

// 0x1404616F0
GLuint create_index_buffer(uint32_t size, const void* data) {
    GLuint ib = 0;
    glGenBuffers(1, &ib);
    gl_state.bind_element_array_buffer(ib);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, size, data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    gl_state.bind_element_array_buffer(0);

    if (glGetError()) {
        glDeleteBuffers(1, &ib);
        return 0;
    }

    bufobj_mgr.ib_all_size += size;
    bufobj_mgr.ib_peak_size = max_def(bufobj_mgr.ib_peak_size, bufobj_mgr.ib_all_size);
    return ib;
}

// 0x140461900
void free_index_buffer(GLuint ib) {
    if (!ib)
        return;

    extern render_context* rctx_ptr;
    rctx_ptr->disp_manager->remove_index_buffer(ib);

    GLint size = 0;
    if (GLAD_GL_VERSION_4_5)
        glGetNamedBufferParameteriv(ib, GL_BUFFER_SIZE, &size);
    else {
        gl_state.bind_element_array_buffer(ib);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        gl_state.bind_element_array_buffer(0);
    }

    bufobj_mgr.ib_all_size -= size;

    glDeleteBuffers(1, &ib);
    glGetError();
}

// 0x1404617B0
GLuint create_vertex_buffer(uint32_t size, const void* data, GL::BufferUsage usage) {
    GLuint vb = 0;
    glGenBuffers(1, &vb);
    gl_state.bind_array_buffer(vb);
    if (GLAD_GL_VERSION_4_4 && usage != GL::BUFFER_USAGE_STREAM) {
        GLbitfield flags = usage == GL::BUFFER_USAGE_DYNAMIC
            ? GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT : 0;
        glBufferStorage(GL_ARRAY_BUFFER, size, data, flags);
    }
    else
        glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToGLenum(usage));
    gl_state.bind_array_buffer(0);

    if (glGetError()) {
        glDeleteBuffers(1, &vb);
        return 0;
    }

    bufobj_mgr.vb_all_size += size;
    bufobj_mgr.vb_peak_size = max_def(bufobj_mgr.vb_peak_size, bufobj_mgr.vb_all_size);
    return vb;
}

// 0x140461990
void free_vertex_buffer(GLuint vb) {
    if (!vb)
        return;

    extern render_context* rctx_ptr;
    rctx_ptr->disp_manager->remove_vertex_buffer(vb);

    GLint size = 0;
    if (GLAD_GL_VERSION_4_5)
        glGetNamedBufferParameteriv(vb, GL_BUFFER_SIZE, &size);
    else {
        gl_state.bind_array_buffer(vb);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        gl_state.bind_array_buffer(0);
    }

    bufobj_mgr.vb_all_size -= size;

    glDeleteBuffers(1, &vb);
    glGetError();
}

// 0x140461AF0
void get_vertex_buffer_memory_info(int32_t* vb_all_size, int32_t* vb_peak_size) {
    if (vb_all_size)
        *vb_all_size = bufobj_mgr.vb_all_size;
    if (vb_peak_size)
        *vb_peak_size = bufobj_mgr.vb_peak_size;
}

// 0x140469640
uint32_t get_vertex_size(uint32_t format, uint32_t compression) {
    uint32_t size = 0;
    switch (compression) {
    case 0:
    default:
        if (format & OBJ_VERTEX_POSITION)
            size += 12;
        if (format & OBJ_VERTEX_NORMAL)
            size += 12;
        if (format & OBJ_VERTEX_TANGENT)
            size += 16;
        if (format & OBJ_VERTEX_BINORMAL)
            size += 12;
        if (format & OBJ_VERTEX_TEXCOORD0)
            size += 8;
        if (format & OBJ_VERTEX_TEXCOORD1)
            size += 8;
        if (format & OBJ_VERTEX_TEXCOORD2)
            size += 8;
        if (format & OBJ_VERTEX_TEXCOORD3)
            size += 8;
        if (format & OBJ_VERTEX_COLOR0)
            size += 16;
        if (format & OBJ_VERTEX_COLOR1)
            size += 16;
        if (format & OBJ_VERTEX_BONE_DATA)
            size += 24;
        if (format & OBJ_VERTEX_UNKNOWN)
            size += 16;
        break;
    case 1: // Added
        if (format & OBJ_VERTEX_POSITION)
            size += 12;
        if (format & OBJ_VERTEX_NORMAL)
            size += 8;
        if (format & OBJ_VERTEX_TANGENT)
            size += 8;
        if (format & OBJ_VERTEX_TEXCOORD0)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD1)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD2)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD3)
            size += 4;
        if (format & OBJ_VERTEX_COLOR0)
            size += 8;
        if (format & OBJ_VERTEX_BONE_DATA)
            size += 16;
        break;
    case 2: // Added
        if (format & OBJ_VERTEX_POSITION)
            size += 12;
        if (format & OBJ_VERTEX_NORMAL)
            size += 4;
        if (format & OBJ_VERTEX_TANGENT)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD0)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD1)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD2)
            size += 4;
        if (format & OBJ_VERTEX_TEXCOORD3)
            size += 4;
        if (format & OBJ_VERTEX_COLOR0)
            size += 8;
        if (format & OBJ_VERTEX_BONE_DATA)
            size += 8;
        break;
    }
    return size;
}
