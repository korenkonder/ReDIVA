/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"

struct alloc_node {
    alloc_node* next;
    size_t size;
    size_t capacity;
#pragma warning(suppress: 4200)
    uint8_t data[];
};

struct alloc_data {
    size_t size;
    alloc_node* next;

    alloc_data();
    ~alloc_data();

    void* allocate(size_t size);
    void deallocate();

    template <typename T>
    inline T* allocate() {
        return new((T*)allocate(sizeof(T))) T;
    }

    template <typename T>
    inline T* allocate(size_t size) {
        return new((T*)allocate(sizeof(T) * size)) T;
    }
};
