/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "GL/buffer.hpp"
#include "config.hpp"
#include "gl.hpp"

// Added
struct IndexBuffer {
    GLuint ib;

    IndexBuffer();

    bool create(uint32_t size, const void* buf);
#if SHARED_OBJECT_BUFFER
    void create(GLuint in_ib);
#endif
    void destroy();
#if SHARED_OBJECT_BUFFER
    void destroy_shared();
#endif
    GLuint get_glib() const;
};

struct VertexBuffer {
    static const uint32_t NUM_FLIP_MAX = 3;

    uint32_t num_flip_chain;
    GLuint vb[NUM_FLIP_MAX];
    uint32_t flip_index;
    GLenum target;
    uint32_t vb_size; // Added
#if SHARED_OBJECT_BUFFER
    uint32_t vb_offset; // Added
#endif

    VertexBuffer();
    ~VertexBuffer();

    bool create(uint32_t size, const void* buf, uint32_t num_flip, GL::BufferUsage usage = GL::BUFFER_USAGE_STATIC);
#if SHARED_OBJECT_BUFFER
    void create(GLuint in_vb, uint32_t size, uint32_t offset); // Added
#endif
    void destroy();
#if SHARED_OBJECT_BUFFER
    void destroy_shared(); // Added
#endif
    void flip();
    GLuint get_glvb() const;
    GLuint get_glvb_at(uint32_t index) const;
    uint32_t get_glvb_offset() const; // Added
    uint32_t get_num_flip_chain(uint32_t index) const;
    uint32_t get_size() const;
    GLenum get_target() const;
};

extern GLuint create_index_buffer(uint32_t size, const void* buf);
extern void free_index_buffer(GLuint ib);

extern GLuint create_vertex_buffer(uint32_t size, const void* buf, GL::BufferUsage usage = GL::BUFFER_USAGE_STATIC);
extern void free_vertex_buffer(GLuint vb);

extern void get_vertex_buffer_memory_info(int32_t* vb_all_size, int32_t* vb_peak_size);

extern uint32_t get_vertex_size(uint32_t vertex_format, uint32_t compression = 0);
