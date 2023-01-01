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
        if (!size)
            return 0;

        T* arr = (T*)allocate(sizeof(T) * size);
        for (size_t i = 0; i < size; i++)
            new(&arr[i]) T();
        return arr;
    }

    template <typename T>
    inline T* allocate(const T* src) {
        if (!src)
            return 0;

        return new((T*)allocate(sizeof(T))) T(*src);
    }

    template <typename T>
    inline T* allocate(const T* src, size_t size) {
        if (!src || !size)
            return 0;

        T* dst = (T*)allocate(sizeof(T) * size);
        for (size_t i = 0; i < size; i++)
            new(&dst[i]) T(src[i]);
        return dst;
    }
};
